#include "ns3/cunb-frame-header.h"
#include "ns3/log.h"
#include <bitset>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CunbFrameHeader");

// Initialization list
CunbFrameHeader::CunbFrameHeader () :
  m_fPort     (0),
  m_address   (CunbDeviceAddress (0,0))
{
}

CunbFrameHeader::~CunbFrameHeader ()
{
}

TypeId
CunbFrameHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("CunbFrameHeader")
    .SetParent<Header> ()
    .AddConstructor<CunbFrameHeader> ()
  ;
  return tid;
}

TypeId
CunbFrameHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
CunbFrameHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION_NOARGS ();


  return 5;
}

void
CunbFrameHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION_NOARGS ();

  // Device Address field
  start.WriteU32 (m_address.Get ());
  // FPort
  start.WriteU8 (m_fPort);
}

uint32_t
CunbFrameHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Read from buffer and save into local variables
  m_address.Set (start.ReadU32 ());

  NS_LOG_DEBUG ("Deserialized data: ");
  NS_LOG_DEBUG ("Address: " << m_address.Print ());

  m_fPort = uint8_t (start.ReadU8 ());

  return 5;   // the number of bytes consumed.
}

void
CunbFrameHeader::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION_NOARGS ();

  os << "Address=" << m_address.Print () << std::endl;
  os << "FPort=" << unsigned(m_fPort) << std::endl;
}

void
CunbFrameHeader::SetFPort (uint8_t fPort)
{
  m_fPort = fPort;
}

uint8_t
CunbFrameHeader::GetFPort (void) const
{
  return m_fPort;
}

void
CunbFrameHeader::SetAddress (CunbDeviceAddress address)
{
  m_address = address;
}

CunbDeviceAddress
CunbFrameHeader::GetAddress (void) const
{
  return m_address;
}

}
