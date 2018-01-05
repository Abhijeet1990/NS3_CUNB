#include "ns3/cunb-tag.h"
#include "ns3/tag.h"
#include "ns3/uinteger.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (CunbTag);

TypeId
CunbTag::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CunbTag")
    .SetParent<Tag> ()
    .SetGroupName ("cunb")
    .AddConstructor<CunbTag> ()
  ;
  return tid;
}

TypeId
CunbTag::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

CunbTag::CunbTag (uint8_t destroyedBy) :
  m_destroyedBy (destroyedBy),
  m_receivePower (0),
  m_dataRate (0),
  m_frequency (0)
{
}

CunbTag::~CunbTag ()
{
}

uint32_t
CunbTag::GetSerializedSize (void) const
{
  // Each datum about a SF is 1 byte + receivePower (the size of a double) +
  // frequency (the size of a double)
  return 3 + 2*sizeof(double);
}

void
CunbTag::Serialize (TagBuffer i) const
{

  i.WriteU8 (m_destroyedBy);
  i.WriteDouble (m_receivePower);
  i.WriteU8 (m_dataRate);
  i.WriteDouble (m_frequency);
}

void
CunbTag::Deserialize (TagBuffer i)
{

  m_destroyedBy = i.ReadU8 ();
  m_receivePower = i.ReadDouble ();
  m_dataRate = i.ReadU8 ();
  m_frequency = i.ReadDouble ();
}

void
CunbTag::Print (std::ostream &os) const
{
  os << m_destroyedBy << " " << m_receivePower << " " <<
  m_dataRate;
}

uint8_t
CunbTag::GetDestroyedBy () const
{
  return m_destroyedBy;
}

double
CunbTag::GetReceivePower () const
{
  return m_receivePower;
}

void
CunbTag::SetDestroyedBy (uint8_t sf)
{
  m_destroyedBy = sf;
}

void
CunbTag::SetReceivePower (double receivePower)
{
  m_receivePower = receivePower;
}

void
CunbTag::SetFrequency (double frequency)
{
  m_frequency = frequency;
}

double
CunbTag::GetFrequency (void)
{
  return m_frequency;
}

uint8_t
CunbTag::GetDataRate (void)
{
  return m_dataRate;
}

void
CunbTag::SetDataRate (uint8_t dataRate)
{
  m_dataRate = dataRate;
}

} // namespace ns3
