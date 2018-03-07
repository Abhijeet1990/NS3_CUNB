#include "ns3/cunb-interference-helper.h"
#include "ns3/log.h"
#include <limits>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CunbInterferenceHelper");

/***************************************
 *    CunbInterferenceHelper::Event    *
 ***************************************/

// Event Constructor
CunbInterferenceHelper::Event::Event (Time duration, double rxPowerdBm,
                                      Ptr<Packet> packet, double frequencyMHz) :
  m_startTime (Simulator::Now ()),
  m_endTime (m_startTime + duration),
  m_rxPowerdBm (rxPowerdBm),
  m_packet (packet),
  m_frequencyMHz (frequencyMHz)
{
  // NS_LOG_FUNCTION_NOARGS ();
}

// Event Destructor
CunbInterferenceHelper::Event::~Event ()
{
  // NS_LOG_FUNCTION_NOARGS ();
}

// Getters
Time
CunbInterferenceHelper::Event::GetStartTime (void) const
{
  return m_startTime;
}

Time
CunbInterferenceHelper::Event::GetEndTime (void) const
{
  return m_endTime;
}

Time
CunbInterferenceHelper::Event::GetDuration (void) const
{
  return m_endTime - m_startTime;
}

double
CunbInterferenceHelper::Event::GetRxPowerdBm (void) const
{
  return m_rxPowerdBm;
}

Ptr<Packet>
CunbInterferenceHelper::Event::GetPacket (void) const
{
  return m_packet;
}

double
CunbInterferenceHelper::Event::GetFrequency (void) const
{
  return m_frequencyMHz;
}

void
CunbInterferenceHelper::Event::Print (std::ostream &stream) const
{
  stream << "(" << m_startTime.GetSeconds () << " s - " <<
  m_endTime.GetSeconds () << " s)," <<
  m_rxPowerdBm << " dBm, "<< m_frequencyMHz << " MHz";
}

std::ostream &operator << (std::ostream &os, const CunbInterferenceHelper::Event &event)
{
  event.Print (os);

  return os;
}

/****************************
 *  CunbInterferenceHelper  *
 ****************************/

NS_OBJECT_ENSURE_REGISTERED (CunbInterferenceHelper);

TypeId
CunbInterferenceHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CunbInterferenceHelper")
    .SetParent<Object> ()
    .SetGroupName ("cunb");

  return tid;
}

CunbInterferenceHelper::CunbInterferenceHelper ()
{
  //NS_LOG_FUNCTION (this);
}

CunbInterferenceHelper::~CunbInterferenceHelper ()
{
  //NS_LOG_FUNCTION (this);
}

const double CunbInterferenceHelper::collisionSnir = 6;

Time CunbInterferenceHelper::oldEventThreshold = Seconds (5);

Ptr<CunbInterferenceHelper::Event>
CunbInterferenceHelper::Add (Time duration, double rxPower,
                             Ptr<Packet> packet,
                             double frequencyMHz)
{

  //NS_LOG_FUNCTION (this << duration.GetSeconds () << rxPower << packet << frequencyMHz);

  // Create an event based on the parameters
  Ptr<CunbInterferenceHelper::Event> event =
    Create<CunbInterferenceHelper::Event> (duration, rxPower,
                                           packet, frequencyMHz);

  // Add the event to the list
  m_events.push_back (event);

  // Clean the event list
  if (m_events.size () > 100)
    {
      CleanOldEvents ();
    }

  return event;
}

void
CunbInterferenceHelper::CleanOldEvents (void)
{
  //NS_LOG_FUNCTION (this);

  // Cycle the events, and clean up if an event is old.
  for (auto it = m_events.begin (); it != m_events.end ();)
    {
      if ((*it)->GetEndTime () + oldEventThreshold < Simulator::Now ())
        {
          it = m_events.erase (it);
        }
      it++;
    }
}

std::list<Ptr<CunbInterferenceHelper::Event> >
CunbInterferenceHelper::GetInterferers ()
{
  return m_events;
}

void
CunbInterferenceHelper::PrintEvents (std::ostream &stream)
{
  //NS_LOG_FUNCTION_NOARGS ();

  stream << "Currently registered events:" << std::endl;

  for (auto it = m_events.begin (); it != m_events.end (); it++)
    {
      (*it)->Print (stream);
      stream << std::endl;
    }
}

bool
CunbInterferenceHelper::IsDestroyedByInterference
  (Ptr<CunbInterferenceHelper::Event> event)
{
  //NS_LOG_FUNCTION (this << event);

  //NS_LOG_INFO ("Current number of events in CunbInterferenceHelper: " << m_events.size ());

  // We want to see the interference affecting this event: cycle through events
  // that overlap with this one and see whether it survives the interference or not.

  // Gather information about the event
  double rxPowerDbm = event->GetRxPowerdBm ();
  double frequency = event->GetFrequency ();

  // Handy information about the time frame when the packet was received
  Time now = Simulator::Now ();
  Time duration = event->GetDuration ();
  //NS_LOG_INFO("Current Event Duration "<<duration);
  Time packetStartTime = now - duration;
  Time packetEndTime = now;

  // Get the list of interfering events
  std::list<Ptr<CunbInterferenceHelper::Event> >::iterator it;

  // Cycle over the events
  for (it = m_events.begin (); it != m_events.end ();)
    {
      // Pointer to the current interferer
      Ptr< CunbInterferenceHelper::Event > interferer = *it;

      // Only consider the current event if the channel is the same
      // Assumption:  there's no interchannel interference. Also skip the current
      // event if it's the same that we want to analyze.
      if (!(interferer->GetFrequency () == frequency) || interferer == event)
        {
          //NS_LOG_DEBUG ("Different channel");
          it++;
          continue;   // Continues from the first line inside the for cycle
        }

      //NS_LOG_DEBUG ("Interferer on same channel");

      // Gather information about this interferer
      double interfererPower = interferer->GetRxPowerdBm ();
      Time interfererStartTime = interferer->GetStartTime ();
      Time interfererEndTime = interferer->GetEndTime ();

      //NS_LOG_INFO ("Found an interferer:  power = " << interfererPower<< ", start time = " << interfererStartTime<< ", end time = " << interfererEndTime<<", start time current event="<<event->GetStartTime()<<", end time current event="<<event->GetEndTime());

      // Compute the fraction of time the two events are overlapping
      Time overlap = GetOverlapTime (event, interferer);

      //temporary checking
      if (overlap.GetSeconds() > 0.0) return true;

      //NS_LOG_DEBUG ("The two events overlap for " << overlap.GetSeconds () << " s.");

      // Compute the equivalent energy of the interference
      // Power [mW] = 10^(Power[dBm]/10)
      // Power [W] = Power [mW] / 1000
      double interfererPowerW = pow (10, interfererPower/10) / 1000;
      // Energy [J] = Time [s] * Power [W]
      double interferenceEnergy = overlap.GetSeconds () * interfererPowerW;

      //NS_LOG_DEBUG ("Interferer power in W: " << interfererPowerW);
      //NS_LOG_DEBUG ("Interference energy: " << interferenceEnergy);

      double signalPowerW = pow (10, rxPowerDbm/10) / 1000;
      double signalEnergy = duration.GetSeconds () * signalPowerW;
      //NS_LOG_DEBUG ("Signal power in W: " << signalPowerW);
      //NS_LOG_DEBUG ("Signal energy: " << signalEnergy);

      double snir = 10*log10 (signalEnergy/interferenceEnergy);
      //NS_LOG_DEBUG ("The current SNIR is " << snir << " dB");

      // Here currently we are using the first value of the matrix. The collision SNIR value is fixed for the receiver.

      double snirIsolation = collisionSnir;
     // NS_LOG_DEBUG ("The needed isolation to survive is "<< snirIsolation << " dB");

      if (snir >= snirIsolation)
              {
                // Move on and check the rest of the interferers
                //NS_LOG_DEBUG ("Packet survived interference");
              }
            else
              {
            	NS_LOG_INFO ("Found an interferer:  power = " << interfererPower<< ", start time = " << interfererStartTime<< ", end time = " << interfererEndTime<<", start time current event="<<event->GetStartTime()<<", end time current event="<<event->GetEndTime());
                NS_LOG_DEBUG ("Packet destroyed by interference with frequency = "<<interferer->GetFrequency());
                NS_LOG_DEBUG ("The two events overlap for " << overlap.GetSeconds () << " s.");
                NS_LOG_DEBUG ("The current SNIR is " << snir << " dB");

                return true;
              }

      it++;
    }
  // If we get to here, it means that the packet survived all interference
  //NS_LOG_DEBUG ("Packet survived all interference");

  // Since the packet was not destroyed, we return 0.
  return false;
}

void
CunbInterferenceHelper::ClearAllEvents (void)
{
  //NS_LOG_FUNCTION_NOARGS ();

  m_events.clear ();
}

Time
CunbInterferenceHelper::GetOverlapTime (Ptr<CunbInterferenceHelper::Event> event1,
                                        Ptr<CunbInterferenceHelper::Event> event2)
{
  //NS_LOG_FUNCTION_NOARGS ();

  // Create the value we will return later
  Time overlap;

  // Get handy values
  Time s1 = event1->GetStartTime (); // Start times
  Time s2 = event2->GetStartTime ();
  Time e1 = event1->GetEndTime ();   // End times
  Time e2 = event2->GetEndTime ();

  // Event1 starts before Event2
  if (s1 < s2)
    {
      // Non-overlapping events
      if (e1 < s2)
        {
          overlap = Seconds (0);
        }
      // event1 contains event2
      else if (e1 >= e2)
        {
          overlap = e2 - s2;
        }
      // Partially overlapping events
      else
        {
          overlap = e1 - s2;
        }
    }
  // Event2 starts before Event1
  else
    {
      // Non-overlapping events
      if (e2 < s1)
        {
          overlap = Seconds (0);
        }
      // event2 contains event1
      else if (e2 >= e1)
        {
          overlap = e1 - s1;
        }
      // Partially overlapping events
      else
        {
          overlap = e2 - s1;
        }
    }
  return overlap;
}
}
