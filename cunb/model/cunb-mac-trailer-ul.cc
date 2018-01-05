#include "cunb-mac-trailer-ul.h"
#include <ns3/packet.h>
#include "crypto++/aes.h"
#include "crypto++/modes.h"
#include "crypto++/cmac.h"
#include "crypto++/sha.h"
#include "crypto++/filters.h"
#include "crypto++/hex.h"
#include "crypto++/base64.h"
#include <iostream>
#include <string>
#include <iomanip>

namespace ns3 {



NS_OBJECT_ENSURE_REGISTERED (CunbMacTrailerUl);

//NS_LOG_COMPONENT_DEFINE ("CunbMacTrailerUl");

const uint16_t CunbMacTrailerUl::CUNB_MAC_FCS_LENGTH = 2;
const uint16_t CunbMacTrailerUl::CUNB_MAC_ECC_LENGTH = 2;
const uint16_t CunbMacTrailerUl::CUNB_MAC_AUTH_LENGTH = 2;

CunbMacTrailerUl::CunbMacTrailerUl (void)
  : m_auth(12),m_ecc(12),m_calcFcs (true)
{
}

TypeId
CunbMacTrailerUl::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CunbMacTrailerUl")
    .SetParent<Trailer> ()
    .SetGroupName ("cunb")
    .AddConstructor<CunbMacTrailerUl> ()
  ;
  return tid;
}

TypeId
CunbMacTrailerUl::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void
CunbMacTrailerUl::Print (std::ostream &os) const
{
  os << " FCS = " << m_fcs;
  os << " ECC = " << m_ecc;
  os << " AUTH = " << m_auth;
}

uint32_t
CunbMacTrailerUl::GetSerializedSize (void) const
{
  return CUNB_MAC_FCS_LENGTH + CUNB_MAC_ECC_LENGTH + CUNB_MAC_AUTH_LENGTH;
}

void
CunbMacTrailerUl::Serialize (Buffer::Iterator start) const
{
  start.Prev (CUNB_MAC_FCS_LENGTH + CUNB_MAC_ECC_LENGTH + CUNB_MAC_AUTH_LENGTH);
  start.WriteU16(m_auth);
  start.WriteU16(m_ecc);
  start.WriteU16 (m_fcs);
  std::cout << "start at serialize: "<< m_fcs<<std::endl;
}

uint32_t
CunbMacTrailerUl::Deserialize (Buffer::Iterator start)
{
  start.Prev (CUNB_MAC_FCS_LENGTH + CUNB_MAC_ECC_LENGTH + CUNB_MAC_AUTH_LENGTH);
  m_auth = start.ReadU16 ();
    m_ecc = start.ReadU16();
    m_fcs = start.ReadU16();
    std::cout<<"start at deserialize: "<< m_fcs <<std::endl;

  return CUNB_MAC_FCS_LENGTH + CUNB_MAC_ECC_LENGTH + CUNB_MAC_AUTH_LENGTH;
}

uint8_t
CunbMacTrailerUl::GetFcsUl (void) const
{
  return m_fcs;
}

void
CunbMacTrailerUl::SetFcsUl (Ptr<const Packet> p)
{
  if (m_calcFcs)
    {
      uint16_t size = p->GetSize ();
      uint8_t *serial_packet = new uint8_t[size];

      p->CopyData (serial_packet, size);

      //m_fcs = GenerateCrc8Ul (serial_packet, size);
      m_fcs = GenerateCrc16Ul (serial_packet, size);
      std::cout << "FCS set: "<< m_fcs << std::endl;
      delete[] serial_packet;
    }
}

/* Be sure to have removed the trailer and only the trailer
 * from the packet before to use CheckFcs */
bool
CunbMacTrailerUl::CheckFcsUl (Ptr<const Packet> p)
{
  if (!m_calcFcs)
    {
      return true;
    }
  else
    {
      //uint8_t checkFcs;
      uint16_t checkFcs;
      uint16_t size = p->GetSize ();
      uint8_t *serial_packet = new uint8_t[size];

      p->CopyData (serial_packet, size);

      //checkFcs = GenerateCrc8Ul (serial_packet, size);
      checkFcs = GenerateCrc16Ul (serial_packet, size);
      delete[] serial_packet;
      std::cout<<"checkFcs: "<<checkFcs << "Get Fcs: "<< GetFcsUl();
      return (checkFcs == GetFcsUl ());
    }
}

void
CunbMacTrailerUl::EnableFcsUl (bool enable)
{
  m_calcFcs = enable;
  if (!enable)
    {
      m_fcs = 0;
    }
}

uint16_t
CunbMacTrailerUl::GetAuthUl(void) const
{
	return m_auth;
}

void
CunbMacTrailerUl::SetAuthUl (Ptr<const Packet> p)
{

}

bool CheckAuthUl (Ptr<const Packet> p)
{
  return true;
}

bool
CunbMacTrailerUl::IsFcsEnabledUl (void)
{
  return m_calcFcs;
}

uint8_t
CunbMacTrailerUl::GenerateCrc8Ul (uint8_t *data, int length)
{
  uint8_t crc8table[256] = {
    0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12,
    0x15, 0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D, 0x70, 0x77, 0x7E,
    0x79, 0x6C, 0x6B, 0x62, 0x65, 0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A,
    0x5D, 0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5, 0xD8, 0xDF, 0xD6,
    0xD1, 0xC4, 0xC3, 0xCA, 0xCD, 0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82,
    0x85, 0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD, 0xC7, 0xC0, 0xC9,
    0xCE, 0xDB, 0xDC, 0xD5, 0xD2, 0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED,
    0xEA, 0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2, 0x8F, 0x88, 0x81,
    0x86, 0x93, 0x94, 0x9D, 0x9A, 0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35,
    0x32, 0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A, 0x57, 0x50, 0x59,
    0x5E, 0x4B, 0x4C, 0x45, 0x42, 0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D,
    0x7A, 0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C, 0xB1, 0xB6, 0xBF,
    0xB8, 0xAD, 0xAA, 0xA3, 0xA4, 0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB,
    0xEC, 0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4, 0x69, 0x6E, 0x67,
    0x60, 0x75, 0x72, 0x7B, 0x7C, 0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43,
    0x44, 0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C, 0x21, 0x26, 0x2F,
    0x28, 0x3D, 0x3A, 0x33, 0x34, 0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C,
    0x5B, 0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63, 0x3E, 0x39, 0x30,
    0x37, 0x22, 0x25, 0x2C, 0x2B, 0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14,
    0x13, 0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB, 0x96, 0x91, 0x98,
    0x9F, 0x8A, 0x8D, 0x84, 0x83, 0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC,
    0xCB, 0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
  };

  uint8_t crc = 0;
    for (int i = 0; i < length; i++)
      {
        crc = crc8table[data[i] ^ crc];
      }
    return crc;
}

uint16_t
CunbMacTrailerUl::GenerateCrc16Ul (uint8_t *data, int length)
{
  int i;
  uint16_t accumulator = 0;

  for (i = 0; i < length; ++i)
    {
      accumulator ^= *data;
      accumulator  = (accumulator >> 8) | (accumulator << 8);
      accumulator ^= (accumulator & 0xff00) << 4;
      accumulator ^= (accumulator >> 8) >> 4;
      accumulator ^= (accumulator & 0xff00) >> 5;
      ++data;
    }
  return accumulator;
}

} //namespace ns3
