#include "ns3/cunb-linklayer-header-ul.h"
#include "ns3/log.h"
#include <bitset>
#include "math.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CunbLinkLayerHeaderUl");

// Initialization list
CunbLinkLayerHeaderUl::CunbLinkLayerHeaderUl () :
  m_segno (0),
  m_segcnt (0)

{
}

CunbLinkLayerHeaderUl::~CunbLinkLayerHeaderUl ()
{
}

TypeId
CunbLinkLayerHeaderUl::GetTypeId (void)
{
  static TypeId tid = TypeId ("CunbLinkLayerHeaderUl")
    .SetParent<Header> ()
    .AddConstructor<CunbLinkLayerHeaderUl> ()
  ;
  return tid;
}

TypeId
CunbLinkLayerHeaderUl::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
CunbLinkLayerHeaderUl::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  // Sizes in bytes:
  // 8 bits are required for LL Header
  uint32_t size = 1;

  NS_LOG_INFO ("CunbLinkLayerHeaderUlUl serialized size: " << size);

  return size;
}

void
CunbLinkLayerHeaderUl::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION_NOARGS ();

  // ll header
  uint8_t llheader = 0;
  llheader |= m_segno*(uint8_t)pow(2,4);
  llheader |= (uint8_t)m_segcnt;
  start.WriteU8 (llheader);

}

uint32_t
CunbLinkLayerHeaderUl::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION_NOARGS ();
  uint8_t data;
  data = start.ReadU8 ();

  // Get the 2 least significant bits to have the Major
  m_segcnt = data & 0b1111;

  m_segno = (data >> 4) & 0b1111;

  return 1;   // the number of bytes consumed.
}

void
CunbLinkLayerHeaderUl::Print (std::ostream &os) const
{
  NS_LOG_FUNCTION_NOARGS ();

  os << "Seg No=" << m_segno << std::endl;
  os << "Seg Size=" << m_segcnt << std::endl;


}

}
