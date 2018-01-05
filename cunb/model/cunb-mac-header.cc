#include "ns3/cunb-mac-header.h"
#include "ns3/log.h"
#include <bitset>
#include <math.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CunbMacHeader");

CunbMacHeader::CunbMacHeader():m_preamble(3),m_mtype(7),m_payload_size(4),m_ack_bits(34)
{

}

CunbMacHeader::~CunbMacHeader ()
{
}

TypeId
CunbMacHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("CunbMacHeader")
    .SetParent<Header> ()
    .AddConstructor<CunbMacHeader> ()
  ;
  return tid;
}

TypeId
CunbMacHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
CunbMacHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return 8;   // This header only consists in 8 bits
}

void
CunbMacHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION_NOARGS ();

  // The header we need to fill
    uint64_t header = 0;

    // The MType
    header |= m_preamble*(uint64_t)pow(2,48);

    header |= m_mtype*(uint64_t)pow(2,40);

    header |= m_payload_size*(uint64_t)pow(2,32);

    header |= m_ack_bits;

    // Write the byte
    start.WriteU64 (header);

    NS_LOG_DEBUG ("Serialization of MAC header: " << std::bitset<64>(header));

}

uint32_t
CunbMacHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Save the byte on a temporary variable
    uint64_t data;
    data = start.ReadU64 ();

    // Get the 2 least significant bits to have the Major
    m_ack_bits = data & 0xffff;

    m_preamble = (data >> 48) & 0xf;

    m_mtype = (data >> 40) & 0xf;

    m_payload_size = (data >> 32) & 0xf;

  return 8;   // the number of bytes consumed.
}

void
CunbMacHeader::Print (std::ostream &os) const
{
  os << "Ackbits=" << unsigned(m_ack_bits) << std::endl;
  os << "Preamble=" << unsigned(m_preamble) << std::endl;
  os << "MType=" << unsigned(m_mtype) << std::endl;
  os << "PayloadSize=" << unsigned(m_payload_size) << std::endl;
}


bool
CunbMacHeader::IsUplink (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return false;
}

void
CunbMacHeader::SetData(uint64_t data)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_data = data;
}

uint64_t
CunbMacHeader::GetData(void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_data;
}

void
CunbMacHeader::SetMType (enum MType mtype)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_mtype = mtype;
}

uint8_t
CunbMacHeader::GetMType (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_mtype;
}

void
CunbMacHeader::SetPreamble (uint8_t preamble)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_preamble = preamble;
}

uint8_t
CunbMacHeader::GetPreamble (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_preamble;
}

void
CunbMacHeader::SetPayloadSize (uint8_t payloadSize)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_payload_size = payloadSize;
}

uint8_t
CunbMacHeader::GetPayloadSize (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_payload_size;
}

void
CunbMacHeader::SetAckBits (uint32_t ackBits)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_mtype = ackBits;
}

uint32_t
CunbMacHeader::GetAckBits (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_ack_bits;
}


}
