#include "ns3/logical-cunb-channel-helper.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LogicalCunbChannelHelper");

NS_OBJECT_ENSURE_REGISTERED (LogicalCunbChannelHelper);

TypeId
LogicalCunbChannelHelper::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LogicalCunbChannelHelper")
    .SetParent<Object> ()
    .SetGroupName ("cunb");
  return tid;
}

LogicalCunbChannelHelper::LogicalCunbChannelHelper () :
  m_nextAggregatedTransmissionTime (Seconds (0)),
  m_aggregatedDutyCycle (1)
{
  NS_LOG_FUNCTION (this);
}

LogicalCunbChannelHelper::~LogicalCunbChannelHelper () {
  NS_LOG_FUNCTION (this);
}

std::vector<Ptr <LogicalCunbChannel> >
LogicalCunbChannelHelper::GetChannelList (void)
{
  NS_LOG_FUNCTION (this);

  // Make a copy of the channel vector
  std::vector<Ptr<LogicalCunbChannel> > vector;
  vector.reserve (m_channelList.size ());
  std::copy (m_channelList.begin (), m_channelList.end (), std::back_inserter
               (vector));

  return vector;
}

Ptr<SubBandCunb>
LogicalCunbChannelHelper::GetSubBandFromChannel (Ptr<LogicalCunbChannel>
                                                 channel)
{
  return GetSubBandFromFrequency (channel->GetFrequency ());
}

Ptr<SubBandCunb>
LogicalCunbChannelHelper::GetSubBandFromFrequency (double frequency)
{
  // Get the SubBand this frequency belongs to
  NS_LOG_INFO("frequency" << frequency);
  std::list< Ptr< SubBandCunb > >::iterator it;
  for (it = m_subBandList.begin (); it != m_subBandList.end (); it++) {
      if ((*it)->BelongsToSubBand (frequency))
        {
          return *it;
        }
    }

  NS_LOG_ERROR ("Warning: frequency is outside any known SubBand.");

  return 0; // If no SubBand is found, return 0
}

void
LogicalCunbChannelHelper::AddChannel (double frequency)
{
  NS_LOG_FUNCTION (this << frequency);

  // Create the new channel and increment the counter
  Ptr<LogicalCunbChannel> channel = Create<LogicalCunbChannel> (frequency);

  // Add it to the list
  m_channelList.push_back (channel);

  NS_LOG_DEBUG ("Added a channel. Current number of channels in list is " <<
                m_channelList.size ());
}

void
LogicalCunbChannelHelper::AddChannel (Ptr<LogicalCunbChannel> logicalChannel)
{
  NS_LOG_FUNCTION (this << logicalChannel);

  // Add it to the list
  m_channelList.push_back (logicalChannel);
}

void
LogicalCunbChannelHelper::SetChannel (uint8_t chIndex,
                                      Ptr<LogicalCunbChannel> logicalChannel)

{
  NS_LOG_FUNCTION (this << chIndex << logicalChannel);

  m_channelList.at (chIndex) = logicalChannel;
}

void
LogicalCunbChannelHelper::AddSubBand (double firstFrequency,
                                      double lastFrequency,
                                      double maxTxPowerDbm)
{
  NS_LOG_FUNCTION (this << firstFrequency << lastFrequency);

  Ptr<SubBandCunb> subBand = Create<SubBandCunb> (firstFrequency, lastFrequency,
                                          maxTxPowerDbm);

  m_subBandList.push_back (subBand);
}

void
LogicalCunbChannelHelper::AddSubBand (Ptr<SubBandCunb> subBand)
{
  NS_LOG_FUNCTION (this << subBand);

  m_subBandList.push_back (subBand);
}

void
LogicalCunbChannelHelper::RemoveChannel (Ptr<LogicalCunbChannel> logicalChannel)
{
  // Search and remove the channel from the list
  std::vector<Ptr<LogicalCunbChannel> >::iterator it;
  for (it = m_channelList.begin (); it != m_channelList.end (); it++)
    {
      Ptr<LogicalCunbChannel> currentChannel = *it;
      if (currentChannel == logicalChannel)
        {
          m_channelList.erase (it);
          return;
        }
    }
}

Time
LogicalCunbChannelHelper::GetAggregatedWaitingTime (void)
{
  // Aggregate waiting time
  Time aggregatedWaitingTime = m_nextAggregatedTransmissionTime - Simulator::Now ();

  // Handle case in which waiting time is negative
  aggregatedWaitingTime = Seconds (std::max (aggregatedWaitingTime.GetSeconds (),
                                             double(0)));

  NS_LOG_DEBUG ("Aggregated waiting time: " << aggregatedWaitingTime.GetSeconds ());

  return aggregatedWaitingTime;
}

Time
LogicalCunbChannelHelper::GetWaitingTime (Ptr<LogicalCunbChannel> channel)
{
  NS_LOG_FUNCTION (this << channel);

  // SubBand waiting time
  Time subBandWaitingTime = GetSubBandFromChannel (channel)->
    GetNextTransmissionTime () -
    Simulator::Now ();

  // Handle case in which waiting time is negative
  subBandWaitingTime = Seconds (std::max (subBandWaitingTime.GetSeconds (),
                                          double(0)));

  NS_LOG_DEBUG ("Waiting time: " << subBandWaitingTime.GetSeconds ());

  return subBandWaitingTime;
}

void
LogicalCunbChannelHelper::AddEvent (Time duration,
                                    Ptr<LogicalCunbChannel> channel)
{
  NS_LOG_FUNCTION (this << duration << channel);

  Ptr<SubBandCunb> subBand = GetSubBandFromChannel (channel);

  //double timeOnAir = duration.GetSeconds ();

  // Computation of necessary waiting time on this sub-band
  //subBand->SetNextTransmissionTime (Simulator::Now () + Seconds(timeOnAir/dutyCycle - timeOnAir));
  subBand->SetNextTransmissionTime (Simulator::Now ());

  // Computation of necessary aggregate waiting time
  //m_nextAggregatedTransmissionTime = Simulator::Now () + Seconds(timeOnAir/m_aggregatedDutyCycle - timeOnAir);
  m_nextAggregatedTransmissionTime = Simulator::Now ();

  //NS_LOG_DEBUG ("Time on air: " << timeOnAir);
  NS_LOG_DEBUG ("m_aggregatedDutyCycle: " << m_aggregatedDutyCycle);
  NS_LOG_DEBUG ("Current time: " << Simulator::Now ().GetSeconds ());
  NS_LOG_DEBUG ("Next transmission on this sub-band allowed at time: " <<
                (subBand->GetNextTransmissionTime ()).GetSeconds ());
  NS_LOG_DEBUG ("Next aggregated transmission allowed at time " <<
                m_nextAggregatedTransmissionTime.GetSeconds ());
}

double
LogicalCunbChannelHelper::GetTxPowerForChannel (Ptr<LogicalCunbChannel>
                                                logicalChannel)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Get the maxTxPowerDbm from the SubBand this channel is in
  std::list< Ptr< SubBandCunb > >::iterator it;
  for (it = m_subBandList.begin (); it != m_subBandList.end (); it++)
    {
      // Check whether this channel is in this SubBand
      if ((*it)->BelongsToSubBand (logicalChannel->GetFrequency ()))
        {

          return (*it)->GetMaxTxPowerDbm ();
        }
    }
  NS_ABORT_MSG ("Logical channel doesn't belong to a known SubBand");

  return 0;
}

void
LogicalCunbChannelHelper::DisableChannel (int index)
{
  NS_LOG_FUNCTION (this << index);

  m_channelList.at (index)->DisableForUplink ();
}
}
