#include "ns3/cunb-linklayer-header.h"
#include "ns3/log.h"
#include <bitset>
#include "math.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CunbLinkLayerHeader");

// Initialization list
CunbLinkLayerHeader::CunbLinkLayerHeader () :
  m_rfu_segno(0),
  m_segno (0),
  m_rfu_segcnt(0),
  m_segcnt (0)

{
}

CunbLinkLayerHeader::~CunbLinkLayerHeader ()
{
}

TypeId
CunbLinkLayerHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("CunbLinkLayerHeader")
    .SetParent<Header> ()
    .AddConstructor<CunbLinkLayerHeader> ()
  ;
  return tid;
}

TypeId
CunbLinkLayerHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
CunbLinkLayerHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  // Sizes in bytes:
  // 16 bits are required for LL Header DL
  uint32_t size = 2;

  NS_LOG_INFO ("CunbLinkLayerHeader serialized size: " << size);

  return size;
}

void
CunbLinkLayerHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION_NOARGS ();

  // ll header
  uint16_t llheader = 0;
  llheader |= m_rfu_segno*(uint16_t)pow(2,15);
  llheader |= m_segno*(uint16_t)pow(2,14);
  llheader |= m_rfu_segcnt*(uint16_t)pow(2,7);
  llheader |= (uint8_t)m_segcnt;
  start.WriteU16 (llheader);

}

uint32_t
CunbLinkLayerHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION_NOARGS ();
  uint16_t data;
  data = start.ReadU16 ();

  m_segcnt = data & 0b1111111;

  m_rfu_segcnt = (data >> 7) & 0b1;

  m_segno = (data >> 14) & 0b1111111;

  m_rfu_segno = (data >> 15) & 0b1;

  return 2;   // the number of bytes consumed.
}

void
CunbLinkLayerHeader::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION_NOARGS ();

  os << "Seg No=" << m_segno << std::endl;
  os << "Seg Size=" << m_segcnt << std::endl;


}

}
