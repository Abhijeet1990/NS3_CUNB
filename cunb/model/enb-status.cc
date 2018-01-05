#include "ns3/enb-status.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("EnbStatus");

EnbStatus::EnbStatus ()
{
  NS_LOG_FUNCTION (this);
}

EnbStatus::~EnbStatus ()
{
  NS_LOG_FUNCTION (this);
}

EnbStatus::EnbStatus (Address address, Ptr<NetDevice> netDevice,
                              Ptr<EnbCunbMac> enbMac) :
  m_address (address),
  m_netDevice (netDevice),
  m_enbMac (enbMac),
  m_nextTransmissionTime (Seconds (0))
{
  NS_LOG_FUNCTION (this);
}

Address
EnbStatus::GetAddress ()
{
  NS_LOG_FUNCTION (this);

  return m_address;
}

void
EnbStatus::SetAddress (Address address)
{
  NS_LOG_FUNCTION (this);

  m_address = address;
}

Ptr<NetDevice>
EnbStatus::GetNetDevice ()
{
  return m_netDevice;
}

void
EnbStatus::SetNetDevice (Ptr<NetDevice> netDevice)
{
  m_netDevice = netDevice;
}

Ptr<EnbCunbMac>
EnbStatus::GetEnbMac (void)
{
  return m_enbMac;
}

bool
EnbStatus::IsAvailableForTransmission (double frequency)
{
  // We can't send multiple packets at once

  // Check that the eNB was not already "booked"
  if (m_nextTransmissionTime > Simulator::Now () - MilliSeconds (1))
    {
      NS_LOG_INFO ("This eNB is already booked for a transmission");
      return false;
    }

  // Check that the eNB is not already in TX mode
  if (m_enbMac->IsTransmitting ())
    {
      NS_LOG_INFO ("This eNB is currently transmitting");
      return false;
    }

  // Check that the eNB is not constrained by the duty cycle
  Time waitingTime = m_enbMac->GetWaitingTime (frequency);
  if (waitingTime > Seconds (0))
    {
      NS_LOG_INFO ("eNB cannot be used because of duty cycle");
      NS_LOG_INFO ("Waiting time at current eNB: " << waitingTime.GetSeconds ()
                                                  << " seconds");

      return false;
    }

  return true;
}

void
EnbStatus::SetNextTransmissionTime (Time nextTransmissionTime)
{
  m_nextTransmissionTime = nextTransmissionTime;
}
}
