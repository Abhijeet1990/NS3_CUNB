#include "ns3/cunb-beacon-trailer.h"
#include "ns3/log.h"
#include <bitset>
#include <math.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CunbBeaconTrailer");

CunbBeaconTrailer::CunbBeaconTrailer():m_fcs(2),m_ecc(7)
{

}

CunbBeaconTrailer::~CunbBeaconTrailer ()
{
}

TypeId
CunbBeaconTrailer::GetTypeId (void)
{
  static TypeId tid = TypeId ("CunbBeaconTrailer")
    .SetParent<Trailer> ()
    .AddConstructor<CunbBeaconTrailer> ()
  ;
  return tid;
}

TypeId
CunbBeaconTrailer::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
CunbBeaconTrailer::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return 5;   // This header only consists in 5 bytes
}

void
CunbBeaconTrailer::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION_NOARGS ();
    start.Prev (5);
    start.WriteU8(m_fcs);
    start.WriteU32(m_ecc);


    NS_LOG_DEBUG ("Serialization of Beacon Trailer: " << std::bitset<8>(m_fcs) << std::bitset<32>(m_ecc));

}

uint32_t
CunbBeaconTrailer::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Save the byte on a temporary variable
  start.Prev (5);
    m_fcs = start.ReadU8 ();

    m_ecc = start.ReadU32();

  return 5;   // the number of bytes consumed.
}

void
CunbBeaconTrailer::Print (std::ostream &os) const
{
  os << "fcs=" << unsigned(m_fcs) << std::endl;
  os << "ecc=" << unsigned(m_ecc) << std::endl;

}

}
