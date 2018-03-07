#include "ns3/cunb-mac-header-ul.h"
#include "ns3/log.h"
#include <bitset>
#include "math.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CunbMacHeaderUl");

CunbMacHeaderUl::CunbMacHeaderUl():m_preamble(3),m_mtype(7),m_fsize(4),m_ack_flags(4),m_rep_cnt(0)
{

}

CunbMacHeaderUl::~CunbMacHeaderUl()
{
}

TypeId
CunbMacHeaderUl::GetTypeId (void)
{
  static TypeId tid = TypeId ("CunbMacHeaderUl")
    .SetParent<Header> ()
    .AddConstructor<CunbMacHeaderUl> ()
  ;
  return tid;
}

TypeId
CunbMacHeaderUl::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
CunbMacHeaderUl::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return 8;   // This header only consists in 8 bits
}

void
CunbMacHeaderUl::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION_NOARGS ();

  // The 40 bit header
  uint64_t header = 0;

      header |= m_preamble*(uint64_t)pow(2,56);
      header |= m_mtype*(uint64_t)pow(2,48);
      header |= m_fsize*(uint64_t)pow(2,40);
      header |= m_ack_flags*(uint64_t)pow(2,32);
      header |= m_rep_cnt*(uint64_t)pow(2,24);
      header |= m_seq_cnt*(uint64_t)pow(2,16);
      header |= m_ident;
/*
  header |= m_preamble*(uint64_t)pow(2,57);
  header |= m_mtype*(uint64_t)pow(2,49);
  header |= m_fsize*(uint64_t)pow(2,42);
  header |= m_ack_flags*(uint64_t)pow(2,34);
  header |= m_rep_cnt*(uint64_t)pow(2,26);
  header |= m_seq_cnt*(uint64_t)pow(2,18);
  header |= m_ident;
 */
  // Write the byte
  start.WriteU64 (header);

  NS_LOG_DEBUG ("Serialization of MAC header: " << std::bitset<64>(header));

}

uint32_t
CunbMacHeaderUl::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION_NOARGS ();


  // Save the byte on a temporary variable
  uint64_t data;
  data = start.ReadU64 ();
  // Get header components
  /*
  m_ident = data & 0xff;
  m_seq_cnt = (data >> 16) & 0xf;
  m_rep_cnt = (data >> 24) & 0xf;
  m_ack_flags = (data >> 32) & 0xf;
  m_fsize = (data >> 40) & 0xf;
  m_mtype = (data >> 48) & 0xf;
  m_preamble = (data >> 56) & 0xf;
  */
  m_ident = data & 0xffff;
   m_seq_cnt = (data >> 16) & 0xff;
   m_rep_cnt = (data >> 24) & 0xff;
   m_ack_flags = (data >> 32) & 0xff;
   m_fsize = (data >> 40) & 0xff;
   m_mtype = (data >> 48) & 0xff;
   m_preamble = (data >> 56) & 0xff;

  return 8;   // the number of bytes consumed.
}

void
CunbMacHeaderUl::Print (std::ostream &os) const
{
  os << "MType=" << unsigned(m_mtype) << std::endl;
  os << "Frame Size=" << unsigned(m_fsize) << std::endl;
  os << "Ack Flags=" << unsigned(m_ack_flags) << std::endl;
  os << "Preamble=" << unsigned(m_preamble) << std::endl;
  os << "Rep Count=" << unsigned(m_rep_cnt) << std::endl;

}

void
CunbMacHeaderUl::SetData(uint64_t data)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_data = data;
}

uint64_t
CunbMacHeaderUl::GetData(void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_data;
}

bool
CunbMacHeaderUl::IsUplink (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return true;
}

void
CunbMacHeaderUl::SetMType (enum MType mtype)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_mtype = mtype;
}

uint8_t
CunbMacHeaderUl::GetMType (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_mtype;
}

void
CunbMacHeaderUl::SetPreamble(uint8_t preamble)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_preamble = preamble;
}

uint8_t
CunbMacHeaderUl::GetPreamble (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_preamble;
}
void
CunbMacHeaderUl::SetFsize(uint8_t fsize)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_fsize = fsize;
}

uint8_t
CunbMacHeaderUl::GetFsize (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_fsize;
}
void
CunbMacHeaderUl::SetAckFlags(uint8_t ackFlags)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_ack_flags = ackFlags;
}

uint8_t
CunbMacHeaderUl::GetAckFlags (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_ack_flags;
}
void
CunbMacHeaderUl::SetRepCnts(uint8_t repCnts)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_rep_cnt = repCnts;
}

uint8_t
CunbMacHeaderUl::GetRepCnts(void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_rep_cnt;
}

void
CunbMacHeaderUl::SetSeqCnt(uint8_t seqCnt)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_seq_cnt = seqCnt;
}

uint8_t
CunbMacHeaderUl::GetSeqCnt(void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_seq_cnt;
}


void
CunbMacHeaderUl::SetIdent(uint16_t ident)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_ident = ident;
}

uint16_t
CunbMacHeaderUl::GetIdent(void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_ident;
}

/*
void
CunbMacHeaderUl::SetIdent(uint32_t ident)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_ident = ident;
}

uint32_t
CunbMacHeaderUl::GetIdent(void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_ident;
}
*/
}
