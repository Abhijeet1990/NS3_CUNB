#include "ns3/cunb-frame-header-ul.h"
#include "ns3/log.h"
#include <bitset>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CunbFrameHeaderUl");

// Initialization list
CunbFrameHeaderUl::CunbFrameHeaderUl () :
  m_fPort     (0),
  m_address   (CunbDeviceAddress (0,0))

{
}

CunbFrameHeaderUl::~CunbFrameHeaderUl ()
{
}

TypeId
CunbFrameHeaderUl::GetTypeId (void)
{
  static TypeId tid = TypeId ("CunbFrameHeaderUl")
    .SetParent<Header> ()
    .AddConstructor<CunbFrameHeaderUl> ()
  ;
  return tid;
}

TypeId
CunbFrameHeaderUl::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
CunbFrameHeaderUl::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION_NOARGS ();


  return 5;
}

void
CunbFrameHeaderUl::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION_NOARGS ();

  // Device Address field
  start.WriteU32 (m_address.Get ());
  // FPort
  start.WriteU8 (m_fPort);
}

uint32_t
CunbFrameHeaderUl::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Read from buffer and save into local variables
  m_address.Set (start.ReadU32 ());
  m_fPort = uint8_t (start.ReadU8 ());

  return 5;   // the number of bytes consumed.
}

void
CunbFrameHeaderUl::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION_NOARGS ();

  os << "Address=" << m_address.Print () << std::endl;
  os << "FPort=" << unsigned(m_fPort) << std::endl;
}

void
CunbFrameHeaderUl::SetFPort (uint8_t fPort)
{
  m_fPort = fPort;
}

uint8_t
CunbFrameHeaderUl::GetFPort (void) const
{
  return m_fPort;
}

void
CunbFrameHeaderUl::SetAddress (CunbDeviceAddress address)
{
  m_address = address;
}

CunbDeviceAddress
CunbFrameHeaderUl::GetAddress (void) const
{
  return m_address;
}

}
