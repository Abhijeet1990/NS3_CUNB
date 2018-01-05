#include "ns3/cunb-device-address-generator.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CunbDeviceAddressGenerator");

TypeId
CunbDeviceAddressGenerator::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CunbDeviceAddressGenerator")
    .SetParent<Object> ()
    .SetGroupName ("cunb")
    .AddConstructor<CunbDeviceAddressGenerator> ();
  return tid;
}

CunbDeviceAddressGenerator::CunbDeviceAddressGenerator (const uint8_t nwkId,
                                                        const uint32_t nwkAddr)
{
  NS_LOG_FUNCTION (this << unsigned(nwkId) << nwkAddr);

  m_currentNwkId.Set (nwkId);
  m_currentNwkAddr.Set (nwkAddr);
}

CunbDeviceAddress
CunbDeviceAddressGenerator::NextNetwork (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_currentNwkId.Set (m_currentNwkId.Get () + 1);
  m_currentNwkAddr.Set (0);

  return CunbDeviceAddress (m_currentNwkId, m_currentNwkAddr);
}

CunbDeviceAddress
CunbDeviceAddressGenerator::NextAddress (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  NwkAddr oldNwkAddr = m_currentNwkAddr;
  m_currentNwkAddr.Set (m_currentNwkAddr.Get () + 1);

  return CunbDeviceAddress (m_currentNwkId, oldNwkAddr);
}

CunbDeviceAddress
CunbDeviceAddressGenerator::GetNextAddress (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  return CunbDeviceAddress (m_currentNwkId.Get (), m_currentNwkAddr.Get () + 1);
}
}
