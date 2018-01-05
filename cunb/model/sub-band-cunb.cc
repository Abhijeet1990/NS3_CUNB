

#include "ns3/sub-band-cunb.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SubBandCunb");

NS_OBJECT_ENSURE_REGISTERED (SubBandCunb);

TypeId
SubBandCunb::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SubBandCunb")
    .SetParent<Object> ()
    .SetGroupName ("cunb");
  return tid;
}

SubBandCunb::SubBandCunb ()
{
  NS_LOG_FUNCTION (this);
}

SubBandCunb::SubBandCunb (double firstFrequency, double lastFrequency,
                  double maxTxPowerDbm) :
  m_firstFrequency (firstFrequency),
  m_lastFrequency (lastFrequency),
  m_maxTxPowerDbm (maxTxPowerDbm)
{
  NS_LOG_FUNCTION (this << firstFrequency << lastFrequency <<
                   maxTxPowerDbm);
}

SubBandCunb::~SubBandCunb ()
{
  NS_LOG_FUNCTION (this);
}

double
SubBandCunb::GetFirstFrequency (void)
{
  return m_firstFrequency;
}


bool
SubBandCunb::BelongsToSubBand (double frequency)
{
  return (frequency > m_firstFrequency) && (frequency < m_lastFrequency);
}

bool
SubBandCunb::BelongsToSubBand (Ptr<LogicalCunbChannel> logicalChannel)
{
  double frequency = logicalChannel->GetFrequency ();
  return BelongsToSubBand (frequency);
}

void
SubBandCunb::SetNextTransmissionTime (Time nextTime)
{
  m_nextTransmissionTime = nextTime;
}

Time
SubBandCunb::GetNextTransmissionTime (void)
{
  return m_nextTransmissionTime;
}

void
SubBandCunb::SetMaxTxPowerDbm (double maxTxPowerDbm)
{
  m_maxTxPowerDbm = maxTxPowerDbm;
}

double
SubBandCunb::GetMaxTxPowerDbm (void)
{
  return m_maxTxPowerDbm;
}
}
