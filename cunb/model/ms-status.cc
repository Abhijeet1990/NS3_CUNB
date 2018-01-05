#include "ns3/ms-status.h"
#include "ns3/log.h"
#include <algorithm>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MSStatus");

MSStatus::MSStatus ()
{
  NS_LOG_FUNCTION (this);
}

MSStatus::~MSStatus ()
{
  NS_LOG_FUNCTION (this);
}

MSStatus::MSStatus (Ptr<MSCunbMac> msMac) :
  m_mac (msMac)
{
  NS_LOG_FUNCTION (this);

  m_reply = MSStatus::Reply ();
}

uint8_t
MSStatus::GetDataRate ()
{
  NS_LOG_FUNCTION (this);

  return m_mac->GetDataRate ();
}

CunbDeviceAddress
MSStatus::GetAddress ()
{
  NS_LOG_FUNCTION (this);

  return m_address;
}

void
MSStatus::SetAddress (CunbDeviceAddress address)
{
  NS_LOG_FUNCTION (this);

  m_address = address;
}

void
MSStatus::UpdateEnbData (Address enbAddress, double rcvPower)
{
  NS_LOG_FUNCTION (this << enbAddress << rcvPower);

  std::map<Address, double>::iterator it = m_enbs.find (enbAddress);
  if (it != m_enbs.end ())
    {
      // Erase the existing entry
      m_enbs.erase (it);
    }
  // Create a new entry
  m_enbs.insert (std::pair<Address, double> (enbAddress, rcvPower));
}

Address
MSStatus::GetBestEnbAddress (void)
{
  NS_LOG_FUNCTION (this);

  return (*(std::max_element (m_enbs.begin (), m_enbs.end (),
                              [] (const std::pair<Address, double>&p1,
                                  const std::pair<Address, double>&p2)
                              { return p1.second > p2.second; }
                              ))).first;
}

std::list<Address>
MSStatus::GetSortedEnbAddresses (void)
{
  NS_LOG_FUNCTION (this);

  // Copy the map pairs into a vector
  std::vector<std::pair<Address, double> > pairs;
  for (auto it = m_enbs.begin (); it != m_enbs.end (); ++it)
    {
      pairs.push_back (*it);
    }

  // Sort the vector
  std::sort (pairs.begin (), pairs.end (),
             [] (const std::pair<Address, double>&p1,
                 const std::pair<Address, double>&p2)
             { return p1.second > p2.second; }
             );

  // Create a new array with only the addresses
  std::list<Address> addresses;
  for (auto it = pairs.begin (); it != pairs.end (); ++it)
    {
      addresses.push_back ((*it).first);
    }

  // Return the list
  return addresses;
}

bool
MSStatus::HasReply (void)
{
  NS_LOG_FUNCTION (this);

  return m_reply.hasReply;
}

void
MSStatus::SetReply (struct Reply reply)
{
  NS_LOG_FUNCTION (this);

  m_reply = reply;
}

Ptr<Packet>
MSStatus::GetReplyPacket (void)
{
  NS_LOG_FUNCTION (this);

  // Add headers to the packet
  Ptr<Packet> replyPacket = m_reply.packet->Copy ();

  replyPacket->AddHeader(m_reply.llHeader);
  replyPacket->AddHeader (m_reply.frameHeader);
  replyPacket->AddHeader (m_reply.macHeader);
  m_reply.macTrailer.EnableFcs(true);
  m_reply.macTrailer.SetFcs(replyPacket);
  m_reply.macTrailer.SetAuth(replyPacket);
  replyPacket->AddTrailer(m_reply.macTrailer);


  NS_LOG_INFO("Reply Packet dest address" << m_reply.frameHeader.GetAddress() );
  return replyPacket;
}

void
MSStatus::SetFirstReceiveWindowFrequency (double frequency)
{
  NS_LOG_FUNCTION (this << frequency);

  m_firstReceiveWindowFrequency = frequency;
}

double
MSStatus::GetFirstReceiveWindowFrequency (void)
{
  NS_LOG_FUNCTION (this);

  return m_firstReceiveWindowFrequency;
}

double
MSStatus::GetSecondReceiveWindowFrequency (void)
{
  NS_LOG_FUNCTION (this);

  return m_mac->GetSecondReceiveWindowFrequency ();
}

double
MSStatus::GetThirdReceiveWindowFrequency (void)
{
  NS_LOG_FUNCTION (this);

  return m_mac->GetThirdReceiveWindowFrequency ();
}

uint8_t
MSStatus::GetFirstReceiveWindowDataRate (void)
{
  NS_LOG_FUNCTION (this);

  return m_mac->GetFirstReceiveWindowDataRate ();
}

uint8_t
MSStatus::GetSecondReceiveWindowDataRate (void)
{
  NS_LOG_FUNCTION (this);

  return m_mac->GetSecondReceiveWindowDataRate ();
}

uint8_t
MSStatus::GetThirdReceiveWindowDataRate (void)
{
  NS_LOG_FUNCTION (this);

  return m_mac->GetThirdReceiveWindowDataRate ();
}

}
