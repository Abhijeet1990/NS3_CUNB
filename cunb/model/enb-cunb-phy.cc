#include "ns3/enb-cunb-phy.h"
#include "ns3/cunb-tag.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("EnbCunbPhy");

NS_OBJECT_ENSURE_REGISTERED (EnbCunbPhy);

/**************************************
 *    ReceptionPath implementation    *
 **************************************/
EnbCunbPhy::ReceptionPath::ReceptionPath(double frequencyMHz) :
  m_frequencyMHz (frequencyMHz),
  m_available (1),
  m_event (0)
{
  //NS_LOG_FUNCTION_NOARGS ();
}

EnbCunbPhy::ReceptionPath::~ReceptionPath(void)
{
  //NS_LOG_FUNCTION_NOARGS ();
}

double
EnbCunbPhy::ReceptionPath::GetFrequency (void)
{
  return m_frequencyMHz;
}

bool
EnbCunbPhy::ReceptionPath::IsAvailable (void)
{
  return m_available;
}

void
EnbCunbPhy::ReceptionPath::Free (void)
{
  m_available = true;
  m_event = 0;
}

void
EnbCunbPhy::ReceptionPath::LockOnEvent (Ptr<CunbInterferenceHelper::Event>
                                            event)
{
  m_available = false;
  m_event = event;
}

void
EnbCunbPhy::ReceptionPath::SetEvent (Ptr<CunbInterferenceHelper::Event>
                                         event)
{
  m_event = event;
}

Ptr<CunbInterferenceHelper::Event>
EnbCunbPhy::ReceptionPath::GetEvent (void)
{
  return m_event;
}

void
EnbCunbPhy::ReceptionPath::SetFrequency (double frequencyMHz)
{
  m_frequencyMHz = frequencyMHz;
}

/***********************************************************************
 *                 Implementation of Enb methods                   *
 ***********************************************************************/

TypeId
EnbCunbPhy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::EnbCunbPhy")
    .SetParent<CunbPhy> ()
    .SetGroupName ("cunb")
    .AddConstructor<EnbCunbPhy> ()
    .AddTraceSource ("LostPacketBecauseNoMoreReceivers",
                     "Trace source indicating a packet "
                     "could not be correctly received because"
                     "there are no more demodulators available",
                     MakeTraceSourceAccessor
                       (&EnbCunbPhy::m_noMoreDemodulators),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("OccupiedReceptionPaths",
                     "Number of currently occupied reception paths",
                     MakeTraceSourceAccessor
                       (&EnbCunbPhy::m_occupiedReceptionPaths),
                     "ns3::TracedValueCallback::Int");
  return tid;
}

EnbCunbPhy::EnbCunbPhy () :
  m_isTransmitting (false)
{
  //NS_LOG_FUNCTION_NOARGS ();
}

EnbCunbPhy::~EnbCunbPhy ()
{
  //NS_LOG_FUNCTION_NOARGS ();
}

// Set a fixed Receiver Sensitivity in dBm
//const double EnbCunbPhy::sensitivity = -150.0;

//const double EnbCunbPhy::sensitivity = -130.0;
const double EnbCunbPhy::sensitivity = -120.0;

void
EnbCunbPhy::AddReceptionPath (double frequencyMHz)
{
  //NS_LOG_FUNCTION (this << frequencyMHz);

  m_receptionPaths.push_back (Create<EnbCunbPhy::ReceptionPath>
                                (frequencyMHz));
}

void
EnbCunbPhy::ResetReceptionPaths (void)
{
  //NS_LOG_FUNCTION (this);

  m_receptionPaths.clear ();
}

void
EnbCunbPhy::Send (Ptr<Packet> packet, CunbTxParameters txParams,
                      double frequencyMHz, double txPowerDbm)
{
  //NS_LOG_FUNCTION (this << packet << frequencyMHz << txPowerDbm);

  // Get the time a packet with these parameters will take to be transmitted
  Time duration = GetOnAirTime (packet, txParams,ENB);

  /*
   *  Differently from what is done in MS, where packets cannot be
   *  transmitted while in RX state, eNB sending is assumed to have priority
   *  over reception.
   *
   *  This different behaviour is motivated by the asymmetry in a typical
   *  cunbWAN network, where Downlink messages are more critical to network
   *  performance than Uplink ones. Even if the eNB is receiving a packet
   *  on the channel when it is asked to transmit by the upper layer, in order
   *  not to miss the receive window of the MS, the eNB will still
   *  need to send the packet. In order to model this fact, the send event is
   *  registered in the eNB's InterferenceHelper as a received event.
   *  While this may not destroy packets incoming on the same frequency, this
   *  is almost always guaranteed to do so due to the fact that this event can
   *  have a power up to 27 dBm.
   */
  m_interference.Add (duration, txPowerDbm, packet, frequencyMHz);

  NS_LOG_INFO("Sending Packet of duration "<< duration <<" using Frequency "<<frequencyMHz);

  // Send the packet in the channel
  m_channel->Send (this, packet, txPowerDbm, txParams, duration, frequencyMHz);

  Simulator::Schedule (duration, &EnbCunbPhy::TxFinished, this, packet);

  m_isTransmitting = true;

  // Fire the trace source
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
EnbCunbPhy::TxFinished (Ptr<Packet> packet)
{
  m_isTransmitting = false;
}

bool
EnbCunbPhy::IsTransmitting (void)
{
  return m_isTransmitting;
}

void
EnbCunbPhy::StartReceive (Ptr<Packet> packet, double rxPowerDbm,
                               Time duration, double frequencyMHz)
{
  NS_LOG_FUNCTION (this << packet << rxPowerDbm << duration << frequencyMHz);

  // Fire the trace source
  m_phyRxBeginTrace (packet);

  // Add the event to the cunbInterferenceHelper
  Ptr<CunbInterferenceHelper::Event> event;
  event = m_interference.Add (duration, rxPowerDbm, packet, frequencyMHz);

  // Cycle over the receive paths to check availability to receive the packet
  std::list<Ptr<EnbCunbPhy::ReceptionPath> >::iterator it;

  for (it = m_receptionPaths.begin (); it != m_receptionPaths.end (); ++it)
    {
      Ptr<EnbCunbPhy::ReceptionPath> currentPath = *it;

      //NS_LOG_DEBUG ("Current ReceptionPath is centered on frequency = " <<currentPath->GetFrequency ());

      // If the receive path is available and listening on the channel of
      // interest, we have a candidate
      if (currentPath->GetFrequency () == frequencyMHz &&
          currentPath->IsAvailable ())
        {
          // See whether the reception power is above or below the sensitivity
          double sensitivity = EnbCunbPhy::sensitivity;

          if (rxPowerDbm < sensitivity)   // Packet arrived below sensitivity
            {
              //NS_LOG_INFO ("Dropping packet reception of packet because under the sensitivity of "<< sensitivity << " dBm");

              if (m_device)
                {
                  m_underSensitivity (packet, m_device->GetNode ()->GetId ());
                }
              else
                {
                  m_underSensitivity (packet, 0);
                }

              // Since the packet is below sensitivity, it makes no sense to
              // search for another ReceivePath
              return;
            }
          else    // We have sufficient sensitivity to start receiving
            {
              //NS_LOG_INFO ("Scheduling reception of a packet, occupying one demodulator");

              // Block this resource
              currentPath->LockOnEvent (event);
              m_occupiedReceptionPaths++;

              // Schedule the end of the reception of the packet
              Simulator::Schedule (duration, &CunbPhy::EndReceive, this,
                                   packet, event);

              // Make sure we don't go on searching for other ReceivePaths
              return;
            }
        }
    }
  // If we get to this point, there are no demodulators we can use
  //NS_LOG_INFO ("Dropping packet reception of packet because no suitable demodulator was found for "<< frequencyMHz);

  // Fire the trace source
  if (m_device)
    {
      m_noMoreDemodulators (packet, m_device->GetNode ()->GetId ());
    }
  else
    {
      m_noMoreDemodulators (packet, 0);
    }
}

void
EnbCunbPhy::EndReceive (Ptr<Packet> packet,
                            Ptr<CunbInterferenceHelper::Event> event)
{
  //NS_LOG_FUNCTION (this << packet << *event);

  // Call the trace source
  m_phyRxEndTrace (packet);

  // Call the cunbInterferenceHelper to determine whether there was
  // destructive interference. If the packet is correctly received, this
  // method returns a 0.
  uint8_t packetDestroyed = 0;
  packetDestroyed = m_interference.IsDestroyedByInterference (event);

  // Check whether the packet was destroyed
  if (packetDestroyed != uint8_t (0))
    {
      NS_LOG_INFO ("packetDestroyed by " << unsigned(packetDestroyed));

      // Update the packet's cunbTag
      CunbTag tag;
      packet->RemovePacketTag (tag);
      tag.SetDestroyedBy (packetDestroyed);
      packet->AddPacketTag (tag);

      // Fire the trace source
      if (m_device)
        {
          m_interferedPacket (packet, m_device->GetNode ()->GetId ());
        }
      else
        {
          m_interferedPacket (packet, 0);
        }
    }
  else   // Reception was correct
    {
      // Fire the trace source
      if (m_device)
        {
    	  //NS_LOG_INFO("Packet succesfully received at Enb");
          m_successfullyReceivedPacket (packet, m_device->GetNode ()->GetId ());
        }
      else
        {
          m_successfullyReceivedPacket (packet, 0);
        }

      // Forward the packet to the upper layer
      if (!m_rxOkCallback.IsNull ())
        {
          // Make a copy of the packet
          Ptr<Packet> packetCopy = packet->Copy ();

          // Set the receive power and frequency of this packet in the cunbTag: this
          // information can be useful for upper layers trying to control link
          // quality.
          CunbTag tag;
          packetCopy->RemovePacketTag (tag);
          tag.SetReceivePower (event->GetRxPowerdBm ());
          tag.SetFrequency (event->GetFrequency ());
          packetCopy->AddPacketTag (tag);

          m_rxOkCallback (packetCopy);
        }

    }

  // Search for the demodulator that was locked on this event to free it.

  std::list< Ptr< EnbCunbPhy::ReceptionPath > >::iterator it;

  for (it = m_receptionPaths.begin (); it != m_receptionPaths.end (); ++it)
    {
      Ptr<EnbCunbPhy::ReceptionPath> currentPath = *it;

      if (currentPath->GetEvent () == event)
        {
          currentPath->Free ();
          m_occupiedReceptionPaths--;
          return;
        }
    }
}

bool
EnbCunbPhy::IsOnFrequency (double frequencyMHz)
{
  //NS_LOG_FUNCTION (this << frequencyMHz);

  // Search every demodulator to see whether there's one listening on this
  // frequency.
  std::list< Ptr< EnbCunbPhy::ReceptionPath > >::iterator it;

  for (it = m_receptionPaths.begin (); it != m_receptionPaths.end (); ++it)
    {
      Ptr<EnbCunbPhy::ReceptionPath> currentPath = *it;

      //NS_LOG_DEBUG ("Current reception path is on frequency " <<currentPath->GetFrequency ());

      if (currentPath->GetFrequency () == frequencyMHz)
        {
          return true;
        }
    }
  return false;
}
}
