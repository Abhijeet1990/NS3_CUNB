#include "ns3/logical-cunb-channel.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("LogicalCunbChannel");

NS_OBJECT_ENSURE_REGISTERED (LogicalCunbChannel);

TypeId
LogicalCunbChannel::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::LogicalCunbChannel")
    .SetParent<Object> ()
    .SetGroupName ("cunb");
  return tid;
}

LogicalCunbChannel::LogicalCunbChannel () :
  m_frequency (0),
  m_minDataRate (0),
  m_maxDataRate (5),
  m_enabledForUplink (true)
{
  NS_LOG_FUNCTION (this);
}

LogicalCunbChannel::~LogicalCunbChannel () {
  NS_LOG_FUNCTION (this);
}

LogicalCunbChannel::LogicalCunbChannel (double frequency) :
  m_frequency (frequency),
  m_enabledForUplink (true)
{
  NS_LOG_FUNCTION (this);
}

LogicalCunbChannel::LogicalCunbChannel (double frequency, uint8_t minDataRate,
                                        uint8_t maxDataRate) :
  m_frequency (frequency),
  m_minDataRate (minDataRate),
  m_maxDataRate (maxDataRate),
  m_enabledForUplink (true)
{
  NS_LOG_FUNCTION (this);
}

double
LogicalCunbChannel::GetFrequency (void) const
{
  return m_frequency;
}

double
LogicalCunbChannel::GetBeaconFrequency (void) const
{
  return m_beacon_frequency;
}

void
LogicalCunbChannel::SetMinimumDataRate (uint8_t minDataRate)
{
  m_minDataRate = minDataRate;
}

void
LogicalCunbChannel::SetMaximumDataRate (uint8_t maxDataRate)
{
  m_maxDataRate = maxDataRate;
}

uint8_t
LogicalCunbChannel::GetMinimumDataRate (void)
{
  return m_minDataRate;
}

uint8_t
LogicalCunbChannel::GetMaximumDataRate (void)
{
  return m_maxDataRate;
}

void
LogicalCunbChannel::SetEnabledForUplink (void)
{
  m_enabledForUplink = true;
}

void
LogicalCunbChannel::DisableForUplink (void)
{
  m_enabledForUplink = false;
}

bool
LogicalCunbChannel::IsEnabledForUplink (void)
{
  return m_enabledForUplink;
}

bool
operator== (const Ptr<LogicalCunbChannel>& first,
            const Ptr<LogicalCunbChannel>& second)
{
  double thisFreq = first->GetFrequency ();
  double otherFreq = second->GetFrequency ();

  NS_LOG_DEBUG ("Checking equality between logical cunb channels: " <<
                thisFreq << " " << otherFreq);

  NS_LOG_DEBUG ("Result:" << (thisFreq == otherFreq));
  return (thisFreq == otherFreq);
}

bool
operator!= (const Ptr<LogicalCunbChannel>& first,
            const Ptr<LogicalCunbChannel>& second)
{
  return !(first == second);
}
}
