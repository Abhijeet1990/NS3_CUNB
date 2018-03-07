#include "ns3/cunb-device-address.h"
#include "ns3/log.h"
#include <bitset>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CunbDeviceAddress");

// NwkID
////////

NwkID::NwkID (uint8_t nwkId) : m_nwkId (nwkId)
{
}

void
NwkID::Set (uint8_t nwkId)
{
  // Check whether the MSB is set
  if (nwkId >> 7)
    {
      NS_LOG_WARN ("Attempting to set too big a network ID. Will only consider the 7 least significant bits.");
    }
  m_nwkId = nwkId & 0x7F; // 0x7f = ob01111111
}

uint8_t
NwkID::Get (void) const
{
  return m_nwkId;
}

// NwkAddr
//////////

NwkAddr::NwkAddr (uint32_t nwkAddr) : m_nwkAddr (nwkAddr)
{
}

void
NwkAddr::Set (uint32_t nwkAddr)
{
  // Check whether the most significant bits are set
  if (nwkAddr >> 25)
    {
      NS_LOG_WARN ("Attempting to set too big a network address. Will only consider the 25 least significant bits.");
    }
  m_nwkAddr = nwkAddr & 0x1FFFFFF;
}

uint32_t
NwkAddr::Get (void) const
{
  return m_nwkAddr;
}


CunbDeviceAddress::CunbDeviceAddress ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

CunbDeviceAddress::CunbDeviceAddress (uint32_t address)
{
  NS_LOG_FUNCTION (this << address);

  Set (address);
}

CunbDeviceAddress::CunbDeviceAddress (uint8_t nwkId, uint32_t nwkAddr)
{
  NS_LOG_FUNCTION (this << unsigned(nwkId) << nwkAddr);

  m_nwkId.Set (nwkId);
  m_nwkAddr.Set (nwkAddr);
}

CunbDeviceAddress::CunbDeviceAddress (NwkID nwkId, NwkAddr nwkAddr)
{
  NS_LOG_FUNCTION (this << unsigned(nwkId.Get ()) << nwkAddr.Get ());

  m_nwkId = nwkId;
  m_nwkAddr = nwkAddr;
}

void
CunbDeviceAddress::Serialize (uint8_t buf[4]) const
{
  NS_LOG_FUNCTION (this << &buf);

  uint32_t address = Get ();

  buf[0] = (address >> 24) & 0xff;
  buf[1] = (address >> 16) & 0xff;
  buf[2] = (address >> 8) & 0xff;
  buf[3] = (address >> 0) & 0xff;
}

CunbDeviceAddress
CunbDeviceAddress::Deserialize (const uint8_t buf[4])
{
  NS_LOG_FUNCTION (&buf);

  // Craft the address from the buffer
  uint32_t address = 0;
  address |= buf[0];
  address <<= 8;
  address |= buf[1];
  address <<= 8;
  address |= buf[2];
  address <<= 8;
  address |= buf[3];

  return CunbDeviceAddress (address);
}

Address
CunbDeviceAddress::ConvertTo (void) const
{
  NS_LOG_FUNCTION (this);

  uint8_t addressBuffer[4];
  Serialize (addressBuffer);
  return Address (GetType (), addressBuffer, 4);
}

CunbDeviceAddress
CunbDeviceAddress::ConvertFrom (const Address &address)
{
  // Create the new, empty address
  CunbDeviceAddress ad;
  uint8_t addressBuffer[4];

  // Check that the address we want to convert is compatible with a
  // LoraDeviceAddress
  NS_ASSERT (address.CheckCompatible (GetType (), 4));
  address.CopyTo (addressBuffer);
  ad = Deserialize (addressBuffer);
  return ad;
}

uint8_t
CunbDeviceAddress::GetType (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  static uint8_t type = Address::Register ();
  return type;
}

uint32_t
CunbDeviceAddress::Get (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  uint32_t address = 0;
  uint32_t nwkId = uint32_t (m_nwkId.Get () << 25);
  address |= (m_nwkAddr.Get () | nwkId);
  NS_LOG_DEBUG ("m_nwkId + m_nwkAddr = " << std::bitset<32>(address));

  return address;
}

void
CunbDeviceAddress::Set (uint32_t address)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_nwkId.Set (address >> 25); // Only leave the 7 most significant bits
  m_nwkAddr.Set (address & 0x1FFFFFF); // Only consider the 25 least significant bits
}

uint8_t
CunbDeviceAddress::GetNwkID (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_nwkId.Get ();
}

uint32_t
CunbDeviceAddress::GetNwkAddr (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_nwkAddr.Get ();
}

void
CunbDeviceAddress::SetNwkID (uint8_t nwkId)
{
  NS_LOG_FUNCTION (this << unsigned(nwkId));

  m_nwkId.Set (nwkId);
}

void
CunbDeviceAddress::SetNwkAddr (uint32_t nwkAddr)
{
  NS_LOG_FUNCTION (this << nwkAddr);

  m_nwkAddr.Set (nwkAddr);
}

std::string
CunbDeviceAddress::Print (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  std::string result;
  result += std::bitset<7>(m_nwkId.Get ()).to_string ();
  result += "|";
  result += std::bitset<25>(m_nwkAddr.Get ()).to_string ();
  return result;
}

bool CunbDeviceAddress::operator==
  (const CunbDeviceAddress &other) const
{
  return this->Get () == other.Get ();
}

bool CunbDeviceAddress::operator!=
  (const CunbDeviceAddress &other) const
{
  return this->Get () != other.Get ();
}

bool CunbDeviceAddress::operator<
  (const CunbDeviceAddress &other) const
{
  return this->Get () < other.Get ();
}

bool CunbDeviceAddress::operator>
  (const CunbDeviceAddress &other) const
{
  return !(this->Get () < other.Get ());
}

std::ostream& operator<< (std::ostream& os, const CunbDeviceAddress &address)
{
  os << address.Print ();
  return os;
}
}
