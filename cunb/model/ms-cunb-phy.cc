#include <algorithm>
#include "ns3/ms-cunb-phy.h"
#include "ns3/simulator.h"
#include "ns3/cunb-tag.h"
#include "ns3/cunb-frame-header.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MSCunbPhy");

NS_OBJECT_ENSURE_REGISTERED (MSCunbPhy);

TypeId
MSCunbPhy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MSCunbPhy")
    .SetParent<CunbPhy> ()
    .SetGroupName ("cunb")
    .AddConstructor<MSCunbPhy> ()
    .AddTraceSource ("LostPacketBecauseWrongFrequency",
                     "Trace source indicating a packet "
                     "could not be correctly decoded because"
                     "the ED was listening on a different frequency",
                     MakeTraceSourceAccessor (&MSCunbPhy::m_wrongFrequency),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("EndDeviceState",
                     "The current state of the device",
                     MakeTraceSourceAccessor
                       (&MSCunbPhy::m_state),
                     "ns3::TracedValueCallback::MSCunbPhy::State");
  return tid;
}

// Initialize the device with some common settings.
// These will then be changed by helpers.
MSCunbPhy::MSCunbPhy () :
  m_state (SLEEP),
  m_frequency (868.1),
  m_beacon_frequency(868.5)
{
}

MSCunbPhy::~MSCunbPhy()
{
}

// Set a fixed sensitivity of MS

const double MSCunbPhy::sensitivity = -104; //https://sites.google.com/site/lteencyclopedia/lte-radio-link-budgeting-and-rf-planning


void
MSCunbPhy::Send (Ptr<Packet> packet, CunbTxParameters txParams,
                        double frequencyMHz, double txPowerDbm)
{
  NS_LOG_FUNCTION (this << packet << txParams << frequencyMHz << txPowerDbm);

  NS_LOG_INFO ("Current state: " << m_state);

  // We must be either in STANDBY or SLEEP mode to send a packet
  if (m_state != STANDBY && m_state != SLEEP)
    {
      NS_LOG_INFO ("Cannot send because device is currently not in STANDBY or SLEEP mode");
      return;
    }

  // We can send the packet: switch to the TX state
  SwitchToTx ();

  // Compute the duration of the transmission
  Time duration = GetOnAirTime (packet, txParams,MS);

  // Tag the packet with information
  CunbTag tag;
  packet->RemovePacketTag (tag);
  packet->AddPacketTag (tag);

  // Send the packet over the channel
  NS_LOG_INFO ("Sending the packet in the channel");
  m_channel->Send (this, packet, txPowerDbm, txParams, duration, frequencyMHz);

  // Schedule the switch back to STANDBY mode.
  // For reference see SX1272 datasheet, section 4.1.6
  Simulator::Schedule (duration, &MSCunbPhy::SwitchToStandby, this);

  // Schedule the txFinished callback, if it was set
  // The call is scheduled just after the switch to standby in case the upper
  // layer wishes to change the state. This ensures that it will find a PHY in
  // STANDBY mode.
  if (!m_txFinishedCallback.IsNull ())
    {
      Simulator::Schedule (duration + NanoSeconds (10),
                           &MSCunbPhy::m_txFinishedCallback, this,
                           packet);
    }


  // Call the trace source
  if (m_device)
    {
      m_startSending (packet, m_device->GetNode ()->GetId ());
    }
  else
    {
      m_startSending (packet, 0);
    }
}

void
MSCunbPhy::StartReceive (Ptr<Packet> packet, double rxPowerDbm,
                                Time duration, double frequencyMHz)
{

  NS_LOG_FUNCTION (this << packet << rxPowerDbm  << duration <<
                   frequencyMHz);

  // Notify the CunbInterferenceHelper of the impinging signal, and remember
  // the event it creates. This will be used then to correctly handle the end
  // of reception event.
  //
  // We need to do this regardless of our state or frequency, since these could
  // change (and making the interference relevant) while the interference is
  // still incoming.

  Ptr<CunbInterferenceHelper::Event> event;
  event = m_interference.Add (duration, rxPowerDbm, packet, frequencyMHz);

  // temporarily set the state to be STANDBY for beacon testing
  m_state = STANDBY;

  // Switch on the current PHY state
  switch (m_state)
    {
    // In the SLEEP, TX and RX cases we cannot receive the packet: we only add
    // it to the list of interferers and do not schedule an EndReceive event for
    // it.
    case SLEEP:
      {
        NS_LOG_INFO ("Dropping packet because device is in SLEEP state");
        break;
      }
    case TX:
      {
        NS_LOG_INFO ("Dropping packet because device is in TX state");
        break;
      }
    case RX:
      {
        NS_LOG_INFO ("Dropping packet because device is already in RX state");
        break;
      }
    // If we are in STANDBY mode, we can potentially lock on the currently
    // incoming transmission
    case STANDBY:
      {
        // There are a series of properties the packet needs to respect in order
        // for us to be able to lock on it:
        // - It's on frequency we are listening on
        // - Its receive power is above the device sensitivity

        // Flag to signal whether we can receive the packet or not
        bool canLockOnPacket = true;

        // Save needed sensitivity
        double sensitivity = MSCunbPhy::sensitivity;

        Ptr<Packet> pcopy = packet->Copy();
        CunbFrameHeader frameHdr;
        pcopy->RemoveHeader(frameHdr);
        uint32_t addr = frameHdr.GetAddress().Get();

        NS_LOG_INFO("Broadcast Address"<<addr);

        // Check frequency
        //////////////////
        if (!IsOnFrequency (frequencyMHz) && addr != 4294967295)
          {
            NS_LOG_INFO ("Packet lost because it's on frequency " <<
                         frequencyMHz << " MHz and we are listening at " <<
                         m_frequency << " MHz");

            // Fire the trace source for this event.
            if (m_device)
              {
                m_wrongFrequency (packet, m_device->GetNode ()->GetId ());
              }
            else
              {
                m_wrongFrequency (packet, 0);
              }

            canLockOnPacket = false;
          }

        if(!IsOnBeaconFrequency (frequencyMHz) && addr == 4294967295)
        {
          NS_LOG_INFO ("Packet lost because it's on frequency " <<
                       frequencyMHz << " MHz and we are listening at " <<
                       m_beacon_frequency << " MHz");

          // Fire the trace source for this event.
          if (m_device)
            {
              m_wrongFrequency (packet, m_device->GetNode ()->GetId ());
            }
          else
            {
              m_wrongFrequency (packet, 0);
            }

          canLockOnPacket = false;
        }

        // Check Sensitivity
        ////////////////////
        if (rxPowerDbm < sensitivity)
          {
            NS_LOG_INFO ("Dropping packet reception of packet because under the sensitivity of " <<
                         sensitivity << " dBm");

            // Fire the trace source for this event.
            if (m_device)
              {
                m_underSensitivity (packet, m_device->GetNode ()->GetId ());
              }
            else
              {
                m_underSensitivity (packet, 0);
              }

            canLockOnPacket = false;
          }

        // Check if one of the above failed
        ///////////////////////////////////
        if (canLockOnPacket)
          {
            // Switch to RX state
            // EndReceive will handle the switch back to STANDBY state
            SwitchToRx ();

            // Schedule the end of the reception of the packet
            NS_LOG_INFO ("Scheduling reception of a packet. End in " <<
                         duration.GetSeconds () << " seconds");

            Simulator::Schedule (duration, &CunbPhy::EndReceive, this, packet,
                                 event);

            // Fire the beginning of reception trace source
            m_phyRxBeginTrace (packet);
          }
      }
    }
}

void
MSCunbPhy::EndReceive (Ptr<Packet> packet,
                              Ptr<CunbInterferenceHelper::Event> event)
{
  NS_LOG_FUNCTION (this << packet << event);

  // Fire the trace source
  m_phyRxEndTrace (packet);

  // Call the CunbInterferenceHelper to determine whether there was destructive
  // interference on this event.
  bool packetDestroyed = m_interference.IsDestroyedByInterference (event);

  // Fire the trace source if packet was destroyed
  if (packetDestroyed)
    {
      NS_LOG_INFO ("Packet destroyed by interference");

      if (m_device)
        {
          m_interferedPacket (packet, m_device->GetNode ()->GetId ());
        }
      else
        {
          m_interferedPacket (packet, 0);
        }

    }
  else
    {
      NS_LOG_INFO ("Packet received correctly");

      if (m_device)
        {
          m_successfullyReceivedPacket (packet, m_device->GetNode ()->GetId ());
        }
      else
        {
          m_successfullyReceivedPacket (packet, 0);
        }

      Ptr<Packet> pcopy = packet->Copy();
      CunbFrameHeader frameHdr;
      pcopy->RemoveHeader(frameHdr);
      uint32_t addr = frameHdr.GetAddress().Get();

      NS_LOG_INFO("Broadcast Address"<<addr);
      // Only keep analyzing the packet if it's downlink
      if (addr == 4294967295 && !m_rxOkCallbackBeacon.IsNull ())
         {
    	  m_rxOkCallbackBeacon (packet);
    	  SwitchToStandby ();
    	  return;
         }
      // If there is one, perform the callback to inform the upper layer
      else if (!m_rxOkCallback.IsNull ())
        {
          m_rxOkCallback (packet);
          SwitchToStandby ();
          return;

        }

    }
  // Automatically switch to Standby in either case
  SwitchToStandby ();
}

bool
MSCunbPhy::IsTransmitting (void)
{
  return m_state == TX;
}

bool
MSCunbPhy::IsReceiving (void)
{
  return m_state == RX;
}

bool
MSCunbPhy::IsOnFrequency (double frequencyMHz)
{
  return m_frequency == frequencyMHz;
}

bool
MSCunbPhy::IsOnBeaconFrequency (double frequencyMHz)
{
  return m_beacon_frequency == frequencyMHz;
}

void
MSCunbPhy::SetFrequency (double frequencyMHz)
{
  m_frequency = frequencyMHz;
}

void
MSCunbPhy::SetBeaconFrequency (double frequencyMHz)
{
  m_beacon_frequency = frequencyMHz;
}

void
MSCunbPhy::SwitchToStandby (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_state = STANDBY;
}

void
MSCunbPhy::SwitchToRx (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  NS_ASSERT (m_state == STANDBY);

  m_state = RX;
}

void
MSCunbPhy::SwitchToTx (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  NS_ASSERT (m_state != RX);

  m_state = TX;
}

void
MSCunbPhy::SwitchToSleep (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  NS_ASSERT (m_state == STANDBY);

  m_state = SLEEP;
}

MSCunbPhy::State
MSCunbPhy::GetState (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_state;
}
}
