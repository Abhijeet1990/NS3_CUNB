#include "ns3/cunb-beacon-header.h"
#include "ns3/log.h"
#include <bitset>
#include <math.h>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CunbBeaconHeader");

CunbBeaconHeader::CunbBeaconHeader():m_preamble(32),m_ftype(7),m_netId(4),m_cellId(21),m_sysInfoCnt(2),m_grpSeqNo(1)
{

}

CunbBeaconHeader::~CunbBeaconHeader ()
{
}

void
CunbBeaconHeader::SetData (double data)
{
  m_data = data;
}

double
CunbBeaconHeader::GetData (void)
{
  return m_data;
}

TypeId
CunbBeaconHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("CunbBeaconHeader")
    .SetParent<Header> ()
    .AddConstructor<CunbBeaconHeader> ()
  ;
  return tid;
}

TypeId
CunbBeaconHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
CunbBeaconHeader::GetSerializedSize (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return 16;   // This header only consists in 16 bytes
}

void
CunbBeaconHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_FUNCTION_NOARGS ();

    start.WriteU32(m_preamble);

  // The header we need to fill
    uint64_t header = 0;

    header |= m_ftype*(uint64_t)pow(2,45);

    header |= m_netId*(uint64_t)pow(2,30);

    header |= m_cellId*(uint64_t)pow(2,15);

    header |= m_sysGroupType*(uint64_t)pow(2,9);

    header |= m_sysInfoCnt*(uint64_t)pow(2,5);

    header |= m_grpSeqNo;

    // Write the byte
    start.WriteU64(header);

    NS_LOG_DEBUG ("Serialization of MAC header: " << std::bitset<32>(m_preamble) << std::bitset<64>(header));

}

uint32_t
CunbBeaconHeader::Deserialize (Buffer::Iterator start)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Save the byte on a temporary variable

    m_preamble = start.ReadU32 ();

    uint64_t data;

    data = start.ReadU64();

    m_grpSeqNo = data & 0b11111;

    m_ftype = (data >> 45) & 0xff;

    m_netId = (data >> 30) & 0b111111111111111;

    m_cellId = (data >> 15) & 0b111111111111111;

    m_sysGroupType = (data >> 9) & 0b1111;

    m_sysInfoCnt = (data >> 5) & 0b11111;

  return 16;   // the number of bytes consumed.
}

void
CunbBeaconHeader::Print (std::ostream &os) const
{
  os << "Preamble=" << unsigned(m_preamble) << std::endl;
  os << "FType=" << unsigned(m_ftype) << std::endl;
  os << "netId=" << unsigned(m_netId) << std::endl;
  os << "cellId=" << unsigned(m_cellId) << std::endl;
  os << "Group Type=" << unsigned(m_sysGroupType) << std::endl;
  os << "SysInfoCount=" << unsigned(m_sysInfoCnt) << std::endl;
  os << "Group Seq No="<< unsigned(m_grpSeqNo) << std::endl;
}

Mac48Address
CunbBeaconHeader::GetAddress()
{
	return m_broadcastAdr;
}

void
CunbBeaconHeader::SetAddress(Mac48Address adr)
{
   m_broadcastAdr = adr;
}


uint32_t
CunbBeaconHeader::GetPreamble(){
	return m_preamble;

}

void
CunbBeaconHeader::SetPreamble(uint32_t preamble) {
	m_preamble = preamble;
}

uint16_t
CunbBeaconHeader::GetFtype(){
	return m_ftype;
}

void
CunbBeaconHeader::SetFtype(uint16_t frameType) {
	m_ftype = frameType;
}

uint16_t
CunbBeaconHeader::GetNetId(){
	return m_netId;
}

void
CunbBeaconHeader::SetNetId(uint16_t netId) {
	m_netId = netId;
}

uint16_t
CunbBeaconHeader::GetCellId(){
	return m_cellId;
}

void
CunbBeaconHeader::SetCellId(uint16_t cellId) {
	m_cellId = cellId;
}

uint8_t
CunbBeaconHeader::GetGroupType(){
	return m_sysGroupType;
}

void
CunbBeaconHeader::SetGroupType(uint8_t groupType) {
	m_sysGroupType = groupType;
}

uint8_t
CunbBeaconHeader::GetSysInfoCnt(){
	return m_sysInfoCnt;
}

void
CunbBeaconHeader::SetSysInfoCnt(uint8_t sysInfoCnt) {
	m_sysGroupType = sysInfoCnt;
}

uint8_t
CunbBeaconHeader::GetGrpSeqNo(){
	return m_grpSeqNo;
}

void
CunbBeaconHeader::SetGrpSeqNo(uint8_t grpSeqNo) {
	m_grpSeqNo = grpSeqNo;
}
}
