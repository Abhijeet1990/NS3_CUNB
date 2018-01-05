#include "ns3/app-layer-header.h"
#include "ns3/log.h"
#include <bitset>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("AppLayerHeader");

// pType = 0 (HH,KA) 1(HH,N) 2(HH,A) 3(C,KA) 4(C,N) 5(C,A)
// A: Alarm, KA: KeepAlive, N: Normal, HH: Household, C: Commercial
// Initialization list
AppLayerHeader::AppLayerHeader () :
  m_pType    (0)
{
}

AppLayerHeader::~AppLayerHeader ()
{
}

TypeId
AppLayerHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("AppLayerHeader")
    .SetParent<Header> ()
    .AddConstructor<AppLayerHeader> ()
  ;
  return tid;
}

TypeId
AppLayerHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
AppLayerHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION_NOARGS ();


  return 2;
}

void
AppLayerHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION_NOARGS ();

  // packet Type
  start.WriteU16(m_pType);

}

uint32_t
AppLayerHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Read from buffer and save into local variables
  m_pType = start.ReadU16 ();

  NS_LOG_DEBUG ("Packet Type: " << m_pType);


  return 2;   // the number of bytes consumed.
}

void
AppLayerHeader::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION_NOARGS ();

  os << "PType=" << unsigned(m_pType) << std::endl;
}

void
AppLayerHeader::SetPtype (uint16_t ptype){
	m_pType = ptype;
}

uint16_t
AppLayerHeader::GetPtype (void) const
{
	return m_pType;
}




}
