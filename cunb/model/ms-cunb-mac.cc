
#include "ns3/ms-cunb-mac.h"
#include "ns3/ms-cunb-phy.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include <algorithm>
#include "ns3/app-layer-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MSCunbMac");

NS_OBJECT_ENSURE_REGISTERED (MSCunbMac);

TypeId
MSCunbMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MSCunbMac")
    .SetParent<CunbMac> ()
    .SetGroupName ("cunb")
    .AddTraceSource ("DataRate",
                     "Data Rate currently employed by this end device",
                     MakeTraceSourceAccessor
                       (&MSCunbMac::m_dataRate),
                     "ns3::TracedValueCallback::uint8_t")
    .AddTraceSource ("TxPower",
                     "Transmission power currently employed by this end device",
                     MakeTraceSourceAccessor
                       (&MSCunbMac::m_txPower),
                     "ns3::TracedValueCallback::Double")
    .AddTraceSource ("LastKnownLinkMargin",
                     "Last known demodulation margin in "
                     "communications between this end device "
                     "and a enb",
                     MakeTraceSourceAccessor
                       (&MSCunbMac::m_lastKnownLinkMargin),
                     "ns3::TracedValueCallback::Double")
    .AddTraceSource ("LastKnownEnbCount",
                     "Last known number of enbs able to "
                     "listen to this end device",
                     MakeTraceSourceAccessor
                       (&MSCunbMac::m_lastKnownEnbCount),
                     "ns3::TracedValueCallback::Int")
    .AddTraceSource ("AggregatedDutyCycle",
                     "Aggregate duty cycle, in fraction form, "
                     "this end device must respect",
                     MakeTraceSourceAccessor
                       (&MSCunbMac::m_aggregatedDutyCycle),
                     "ns3::TracedValueCallback::Double")
    .AddConstructor<MSCunbMac> ();
  return tid;
}

MSCunbMac::MSCunbMac () :
  m_dataRate (0),
  m_txPower (14),
  m_receiveDelay1 (Seconds (1)),            // CUNB default
  m_receiveDelay2 (Seconds (3)),            // CUNB default
  m_receiveDelay3 (Seconds (5)),            // CUNB default
  m_receiveWindowDuration (Seconds (0.2)),
  m_closeWindow (EventId ()),               // Initialize as the default eventId
  // m_secondReceiveWindow (EventId ()),       // Initialize as the default eventId
  // m_secondReceiveWindowDataRate (0),        // CUNB default
  m_lastKnownLinkMargin (0),
  m_lastKnownEnbCount (0),
  m_aggregatedDutyCycle (1),
  m_mType (CunbMacHeaderUl::SINGLE_ACK) // by default it would be single ACK
{
  NS_LOG_FUNCTION (this);

  // Initialize the random variable we'll use to decide which channel to
  // transmit on.
  m_uniformRV = CreateObject<UniformRandomVariable> ();

  // Void the two receiveWindow events
  m_closeWindow = EventId ();
  m_closeWindow.Cancel ();
  m_secondReceiveWindow = EventId ();
  m_secondReceiveWindow.Cancel ();
  m_thirdReceiveWindow = EventId ();
  m_thirdReceiveWindow.Cancel ();
  m_firstRetransmit = EventId ();
  m_firstRetransmit.Cancel ();
  m_secondRetransmit = EventId ();
  m_secondRetransmit.Cancel ();
}

MSCunbMac::~MSCunbMac ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
MSCunbMac::Send (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);

  AppLayerHeader appHdr;
  Ptr<Packet> packetCopy = packet->Copy();
  packetCopy->RemoveHeader(appHdr);

  uint16_t pType = appHdr.GetPtype();
  NS_LOG_INFO("App Packet Type MS MAC " << pType);

  // Check that there are no scheduled receive windows.
  // We cannot send a packet if we are in the process of transmitting or waiting
  // for reception.
  if (!m_closeWindow.IsExpired () || !m_secondReceiveWindow.IsExpired () || !m_thirdReceiveWindow.IsExpired ())
    {
      NS_LOG_WARN ("Attempting to send when there are receive windows" <<
                   " Transmission canceled");
      return;
    }

  // Check that payload length is below the allowed maximum
  // Here we need to use the technique of Fragmentation and Reassembly
  // Currently it drops the packet if the packet size is higher than maxPayload allowed
  // yet to be implemented
  if (packet->GetSize () > m_maxAppPayloadForDataRate.at (m_dataRate))
    {
      NS_LOG_WARN ("Attempting to send a packet larger than the maximum allowed"
                   << " size at this DataRate (DR" << unsigned(m_dataRate) <<
                   "). Transmission canceled.");
      return;
    }

  // Check that we can transmit according to the aggregate duty cycle timer
  if (m_channelHelper.GetAggregatedWaitingTime () != Seconds (0))
    {
      NS_LOG_WARN ("Attempting to send, but the aggregate duty cycle won't allow it");
      return;
    }

  // Pick a channel on which to transmit the packet
  Ptr<LogicalCunbChannel> txChannel = GetChannelForTx ();

  if (txChannel) // Proceed with transmission
    {
      /////////////////////////////////////////////////////////
      // Add headers, prepare TX parameters and send the packet
      /////////////////////////////////////////////////////////

      // Add the Cunb Link Layer Header to the packet
      CunbLinkLayerHeader llHdr;
      packet->AddHeader (llHdr);
      NS_LOG_INFO ("Added link layer header of size " << llHdr.GetSerializedSize () <<
                   " bytes");

      // Add the Cunb Frame Header to the packet
      CunbFrameHeaderUl frameHdr;
      ApplyNecessaryOptions (frameHdr);
            packet->AddHeader (frameHdr);
            NS_LOG_INFO ("Added frame header of size " << frameHdr.GetSerializedSize () <<
                         " bytes");

      // Add the Cunb Mac header to the packet
      CunbMacHeaderUl macHdr;
      ApplyNecessaryOptions (macHdr);
      packet->AddHeader (macHdr);
      NS_LOG_INFO ("Added MAC header of size " << macHdr.GetSerializedSize () <<
                   " bytes");

      CunbMacTrailer macTlr;
      macTlr.EnableFcs(true);
      macTlr.SetFcs(packet);
      macTlr.SetAuth(packet);
      packet->AddTrailer(macTlr);
      NS_LOG_INFO ("Added MAC trailer of size " << macTlr.GetSerializedSize () <<
                         " bytes and FCS: " << macTlr.GetFcs());

      // Craft CunbTxParameters object
      CunbTxParameters params;
      params.bitrate = 250; // bit rate for uplink is 250bps
      params.nPreamble = m_nPreambleSymbols;
      params.eccEnabled = 1;
      params.authEnabled = 1;
      params.fcsEnabled = 1;

      // Wake up PHY layer and directly send the packet

      // Make sure we can transmit at the current power on this channel
      NS_ASSERT (m_txPower <= m_channelHelper.GetTxPowerForChannel (txChannel));
      m_phy->GetObject<MSCunbPhy> ()->SwitchToStandby ();
      m_phy->Send (packet, params, txChannel->GetFrequency (), m_txPower);

      //////////////////////////////////////////////
      // Register packet transmission for duty cycle
      //////////////////////////////////////////////

      // Compute packet duration
      Time duration = m_phy->GetOnAirTime (packet, params,MS);

      // Register the sent packet into the LogicalCunbChannelHelper
      m_channelHelper.AddEvent (duration, txChannel);

      //////////////////////////////
      // Prepare for the downlink //
      //////////////////////////////

      // Switch the PHY to the channel so that it will listen here for downlink
      m_phy->GetObject<MSCunbPhy> ()->SetFrequency (txChannel->GetFrequency ());

      // Instruct the PHY on the right data rate to listen for during the window
      uint8_t replyDataRate = GetFirstReceiveWindowDataRate ();
      NS_LOG_DEBUG ("m_dataRate: " << unsigned (m_dataRate) <<
                    ", replyDataRate: " << unsigned (replyDataRate));

      // Schedule the opening of the first receive window
        Simulator::Schedule (m_receiveDelay1,
                             &MSCunbMac::OpenFirstReceiveWindow, this);

      // Normal and Keep Alive Packets for both commercial and household
        if((pType == 1 || pType == 2 || pType == 4 || pType == 5))
        {
      	  // Schedule the opening of the second receive window
            m_secondReceiveWindow = Simulator::Schedule (m_receiveDelay2,
                                                     &MSCunbMac::OpenSecondReceiveWindow,
                                                     this);
            // Retransmit the packet
            m_firstRetransmit = Simulator::Schedule(m_receiveDelay2,&MSCunbMac::Send, this,packet);
        }

       // Alarm packets for both Household and commercial
        if(pType == 2 || pType == 5)

          {
            // Schedule the opening of the third receive window
            m_thirdReceiveWindow = Simulator::Schedule (m_receiveDelay3,
                                                       &MSCunbMac::OpenThirdReceiveWindow,
                                                     this);
            // Retransmit the packet
            m_secondRetransmit = Simulator::Schedule(m_receiveDelay3,&MSCunbMac::Send, this, packet);
          }


    }
  else // Transmission cannot be performed
    {
      m_cannotSendBecauseDutyCycle (packet);
    }
}

void
MSCunbMac::Receive (Ptr<Packet const> packet)
{
  NS_LOG_FUNCTION (this << packet);

  // Work on a copy of the packet
  Ptr<Packet> packetCopy = packet->Copy ();



  CunbMacTrailer macTlr;
  macTlr.EnableFcs(true);
  packetCopy->RemoveTrailer(macTlr);
  bool verify = macTlr.CheckFcs(packetCopy);
  bool verifyAuth = macTlr.CheckAuth(packetCopy);
  NS_LOG_INFO ("Verifying CRC Downlink " << verify << " and Auth "<< verifyAuth);


  // Remove the Mac Header to get some information
  CunbMacHeader macHdr;
  packetCopy->RemoveHeader (macHdr);


  // Only keep analyzing the packet if it's downlink
  if (!macHdr.IsUplink ())
    {
      NS_LOG_INFO ("Found a downlink packet");

      // Remove the Frame Header
      CunbFrameHeader frameHdr;
      packetCopy->RemoveHeader (frameHdr);

      // Remove the Link Layer Header
      CunbLinkLayerHeader llHdr;
      packetCopy->RemoveHeader (llHdr);

      // As of now the Link Layer header for the DL is not prepared yet

      // Determine whether this packet is for that mobile device
      bool messageForUs = (m_address == frameHdr.GetAddress ());

      if (messageForUs)
        {
          NS_LOG_INFO ("The message is for us!");

          // If it exists, cancel the second and third receive window event
          Simulator::Cancel (m_secondReceiveWindow);
          Simulator::Cancel (m_thirdReceiveWindow);

          // If it exists, cancel the first and second retransmission
          Simulator::Cancel(m_firstRetransmit);
          Simulator::Cancel(m_secondRetransmit);

          // Call the trace source
          m_receivedPacket (packet);
        }
      else
        {
          NS_LOG_DEBUG ("The message is intended for another recipient.");
        }
    }
}

void
MSCunbMac::SendBeacon(Ptr<Packet> packet)
{
   NS_LOG_INFO("Beacons are only received by mobile stations");
}
void
MSCunbMac::ReceiveBeacon(Ptr<Packet const> beaconPacket)
{
	NS_LOG_FUNCTION (this << beaconPacket);

    // Extract data from the beacon packet to set the transmission properties of reporting signals
    ExtractBeaconInformation(beaconPacket);


}

void
MSCunbMac::ExtractBeaconInformation(Ptr<Packet const> beaconPacket)
{
	// Work on a copy of the packet
		  Ptr<Packet> packetCopy = beaconPacket->Copy ();

		  // Remove the Frame Header
		  CunbFrameHeader frameHdr;
		  packetCopy->RemoveHeader(frameHdr);

		  // Remove the Beacon Trailer
		  CunbBeaconTrailer beaconTlr;
		  packetCopy->RemoveTrailer(beaconTlr);

		  // Remove the Beacon Header
		  CunbBeaconHeader beaconHdr;
		  packetCopy->RemoveHeader (beaconHdr);

		  packetCopy->Print(std::cout);

		  NS_LOG_INFO("Data "<< beaconHdr.GetData());
		  NS_LOG_INFO("GroupType "<< beaconHdr.GetGroupType());
}

void
MSCunbMac::ApplyNecessaryOptions (CunbFrameHeaderUl & frameHeader)
{
  NS_LOG_FUNCTION_NOARGS ();

  frameHeader.SetFPort (1); // TODO Use an appropriate frame port based on the application
  frameHeader.SetAddress (m_address);

}

void
MSCunbMac::ApplyNecessaryOptions (CunbMacHeaderUl & macHeader)
{
  NS_LOG_FUNCTION_NOARGS ();

  macHeader.SetMType (m_mType);
}

void
MSCunbMac::SetMType (CunbMacHeaderUl::MType mType)
{
  m_mType = mType;
}

void
MSCunbMac::TxFinished (Ptr<const Packet> packet)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Switch the PHY to sleep
  m_phy->GetObject<MSCunbPhy> ()->SwitchToSleep ();
}

void
MSCunbMac::OpenFirstReceiveWindow (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Set Phy in Standby mode
  m_phy->GetObject<MSCunbPhy> ()->SwitchToStandby ();

  // Schedule return to sleep after "at least the time required by the end
  // device's radio transceiver to effectively detect a downlink preamble"
  m_closeWindow = Simulator::Schedule (m_receiveWindowDuration,
                                       &MSCunbMac::CloseFirstReceiveWindow, this);
}

void
MSCunbMac::CloseFirstReceiveWindow (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  Ptr<MSCunbPhy> phy = m_phy->GetObject<MSCunbPhy> ();

  // Check the Phy layer's state:
  // - RX -> We have received a preamble.
  // - STANDBY -> Nothing was detected.
  // We should never be in TX or SLEEP mode at this point
  switch (phy->GetState ())
    {
    case MSCunbPhy::TX:
    case MSCunbPhy::SLEEP:
      NS_ABORT_MSG ("PHY was in TX or SLEEP mode when attempting to " <<
                    "close a receive window");
      break;
    case MSCunbPhy::RX:
      // PHY is receiving: let it finish. The Receive method will switch it back to SLEEP.
      break;
    case MSCunbPhy::STANDBY:
      // Turn PHY layer to SLEEP
      phy->SwitchToSleep ();
      break;
    }
}

void
MSCunbMac::OpenSecondReceiveWindow (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Check for receiver status: if it's locked on a packet, don't open this
  // window at all.
  if (m_phy->GetObject<MSCunbPhy> ()->GetState () == MSCunbPhy::RX)
    {
      NS_LOG_INFO ("Won't open second receive window since we are in RX mode");

      return;
    }

  // Set Phy in Standby mode
  m_phy->GetObject<MSCunbPhy> ()->SwitchToStandby ();

  // Switch to appropriate channel and data rate
  NS_LOG_INFO ("Using parameters: " << m_secondReceiveWindowFrequency << "Hz, DR"
                                    << unsigned(m_secondReceiveWindowDataRate));

  m_phy->GetObject<MSCunbPhy> ()->SetFrequency
    (m_secondReceiveWindowFrequency);

  // Schedule return to sleep after "at least the time required by the end
  // device's radio transceiver to effectively detect a downlink preamble"
  m_closeWindow = Simulator::Schedule (m_receiveWindowDuration,
                                       &MSCunbMac::CloseSecondReceiveWindow, this);
}

void
MSCunbMac::CloseSecondReceiveWindow (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  Ptr<MSCunbPhy> phy = m_phy->GetObject<MSCunbPhy> ();

  // NS_ASSERT (phy->m_state != EndDeviceLoraPhy::TX &&
  // phy->m_state != EndDeviceLoraPhy::SLEEP);

  // Check the Phy layer's state:
  // - RX -> We have received a preamble.
  // - STANDBY -> Nothing was detected.
  switch (phy->GetState ())
    {
    case MSCunbPhy::TX:
      break;
    case MSCunbPhy::SLEEP:
      break;
    case MSCunbPhy::RX:
      // PHY is receiving: let it finish
      break;
    case MSCunbPhy::STANDBY:
      // Turn PHY layer to sleep
      phy->SwitchToSleep ();
      break;
    }
}

void
MSCunbMac::OpenThirdReceiveWindow (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Check for receiver status: if it's locked on a packet, don't open this
  // window at all.
  if (m_phy->GetObject<MSCunbPhy> ()->GetState () == MSCunbPhy::RX)
    {
      NS_LOG_INFO ("Won't open third receive window since we are in RX mode");

      return;
    }

  // Set Phy in Standby mode
  m_phy->GetObject<MSCunbPhy> ()->SwitchToStandby ();

  // Switch to appropriate channel and data rate
  NS_LOG_INFO ("Using parameters: " << m_thirdReceiveWindowFrequency << "Hz, DR"
                                    << unsigned(m_thirdReceiveWindowDataRate));

  m_phy->GetObject<MSCunbPhy> ()->SetFrequency
    (m_thirdReceiveWindowFrequency);

  // Schedule return to sleep after "at least the time required by the end
  // device's radio transceiver to effectively detect a downlink preamble"
  m_closeWindow = Simulator::Schedule (m_receiveWindowDuration,
                                       &MSCunbMac::CloseThirdReceiveWindow, this);
}

void
MSCunbMac::CloseThirdReceiveWindow (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  Ptr<MSCunbPhy> phy = m_phy->GetObject<MSCunbPhy> ();

  // NS_ASSERT (phy->m_state != EndDeviceLoraPhy::TX &&
  // phy->m_state != EndDeviceLoraPhy::SLEEP);

  // Check the Phy layer's state:
  // - RX -> We have received a preamble.
  // - STANDBY -> Nothing was detected.
  switch (phy->GetState ())
    {
    case MSCunbPhy::TX:
      break;
    case MSCunbPhy::SLEEP:
      break;
    case MSCunbPhy::RX:
      // PHY is receiving: let it finish
      break;
    case MSCunbPhy::STANDBY:
      // Turn PHY layer to sleep
      phy->SwitchToSleep ();
      break;
    }
}

Ptr<LogicalCunbChannel>
MSCunbMac::GetChannelForTx (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Pick a random channel to transmit on
  std::vector<Ptr<LogicalCunbChannel> > logicalChannels;
  logicalChannels = m_channelHelper.GetChannelList (); // Use a separate list to do the shuffle
  logicalChannels = Shuffle (logicalChannels);

  // Try every channel
  std::vector<Ptr<LogicalCunbChannel> >::iterator it;
  for (it = logicalChannels.begin (); it != logicalChannels.end (); ++it)
    {
      // Pointer to the current channel
      Ptr<LogicalCunbChannel> logicalChannel = *it;
      double frequency = logicalChannel->GetFrequency ();

      NS_LOG_DEBUG ("Frequency of the current channel: " << frequency);

      // Verify that we can send the packet
      Time waitingTime = m_channelHelper.GetWaitingTime (logicalChannel);

      NS_LOG_DEBUG ("Waiting time for current channel = " <<
                    waitingTime.GetSeconds ());

      // Send immediately if we can
      if (waitingTime == Seconds (0))
        {
          return *it;
        }
      else
        {
          NS_LOG_DEBUG ("Packet cannot be immediately transmitted on " <<
                        "the current channel because of duty cycle limitations");
        }
    }
  return 0; // In this case, no suitable channel was found
}

std::vector<Ptr<LogicalCunbChannel> >
MSCunbMac::Shuffle (std::vector<Ptr<LogicalCunbChannel> > vector)
{
  NS_LOG_FUNCTION_NOARGS ();

  int size = vector.size ();

  for (int i = 0; i < size; ++i)
    {
      uint16_t random = std::floor (m_uniformRV->GetValue (0, size));
      Ptr<LogicalCunbChannel> temp = vector.at (random);
      vector.at (random) = vector.at (i);
      vector.at (i) = temp;
    }

  return vector;
}

/////////////////////////
// Setters and Getters //
/////////////////////////

void
MSCunbMac::SetDataRate (uint8_t dataRate)
{
  NS_LOG_FUNCTION (this << unsigned (dataRate));

  m_dataRate = dataRate;
}

uint8_t
MSCunbMac::GetDataRate (void)
{
  NS_LOG_FUNCTION (this);

  return m_dataRate;
}

void
MSCunbMac::SetDeviceAddress (CunbDeviceAddress address)
{
  NS_LOG_FUNCTION (this << address);

  m_address = address;
}

CunbDeviceAddress
MSCunbMac::GetDeviceAddress (void)
{
  NS_LOG_FUNCTION (this);

  return m_address;
}

void
MSCunbMac::AddLogicalChannel (double frequency)
{
  NS_LOG_FUNCTION (this << frequency);

  m_channelHelper.AddChannel (frequency);
}

void
MSCunbMac::AddLogicalChannel (Ptr<LogicalCunbChannel> logicalChannel)
{
  NS_LOG_FUNCTION (this << logicalChannel);

  m_channelHelper.AddChannel (logicalChannel);
}

void
MSCunbMac::SetLogicalChannel (uint8_t chIndex, double frequency,
                                     uint8_t minDataRate, uint8_t maxDataRate)
{
  NS_LOG_FUNCTION (this << unsigned (chIndex) << frequency <<
                   unsigned (minDataRate) << unsigned(maxDataRate));

  m_channelHelper.SetChannel (chIndex, CreateObject<LogicalCunbChannel>
                                (frequency, minDataRate, maxDataRate));
}

void
MSCunbMac::AddSubBand (double startFrequency, double endFrequency, double maxTxPowerDbm)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_channelHelper.AddSubBand (startFrequency, endFrequency, maxTxPowerDbm);
}

uint8_t
MSCunbMac::GetFirstReceiveWindowDataRate (void)
{
  return m_replyDataRateMatrix.at (m_dataRate);
}

void
MSCunbMac::SetSecondReceiveWindowDataRate (uint8_t dataRate)
{
  m_secondReceiveWindowDataRate = dataRate;
}

uint8_t
MSCunbMac::GetSecondReceiveWindowDataRate (void)
{
  return m_secondReceiveWindowDataRate;
}

void
MSCunbMac::SetSecondReceiveWindowFrequency (double frequencyMHz)
{
  m_secondReceiveWindowFrequency = frequencyMHz;
}

double
MSCunbMac::GetSecondReceiveWindowFrequency (void)
{
  return m_secondReceiveWindowFrequency;
}

void
MSCunbMac::SetThirdReceiveWindowDataRate (uint8_t dataRate)
{
  m_thirdReceiveWindowDataRate = dataRate;
}

uint8_t
MSCunbMac::GetThirdReceiveWindowDataRate (void)
{
  return m_thirdReceiveWindowDataRate;
}

void
MSCunbMac::SetThirdReceiveWindowFrequency (double frequencyMHz)
{
  m_thirdReceiveWindowFrequency = frequencyMHz;
}

double
MSCunbMac::GetThirdReceiveWindowFrequency (void)
{
  return m_thirdReceiveWindowFrequency;
}

double
MSCunbMac::GetAggregatedDutyCycle (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_aggregatedDutyCycle;
}
}
