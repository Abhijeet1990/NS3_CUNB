#include "ns3/cunb-net-device.h"
#include "ns3/pointer.h"
#include "ns3/node.h"
#include "ns3/log.h"
#include "ns3/abort.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CunbNetDevice");

NS_OBJECT_ENSURE_REGISTERED (CunbNetDevice);

TypeId
CunbNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CunbNetDevice")
    .SetParent<NetDevice> ()
    .AddConstructor<CunbNetDevice> ()
    .SetGroupName ("cunb")
    .AddAttribute ("Channel", "The channel attached to this device",
                   PointerValue (),
                   MakePointerAccessor (&CunbNetDevice::DoGetChannel),
                   MakePointerChecker<CunbChannel> ())
    .AddAttribute ("Phy", "The PHY layer attached to this device.",
                   PointerValue (),
                   MakePointerAccessor (&CunbNetDevice::GetPhy,
                                        &CunbNetDevice::SetPhy),
                   MakePointerChecker<CunbPhy> ())
    .AddAttribute ("Mac", "The MAC layer attached to this device.",
                   PointerValue (),
                   MakePointerAccessor (&CunbNetDevice::GetMac,
                                        &CunbNetDevice::SetMac),
                   MakePointerChecker<CunbMac> ());
  return tid;
}

CunbNetDevice::CunbNetDevice () :
  m_node (0),
  m_phy (0),
  m_mac (0),
  m_configComplete (0)
{
  NS_LOG_FUNCTION_NOARGS ();
}

CunbNetDevice::~CunbNetDevice ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
CunbNetDevice::SetMac (Ptr<CunbMac> mac)
{
  m_mac = mac;
}

Ptr<CunbMac>
CunbNetDevice::GetMac (void) const
{
  return m_mac;
}

void
CunbNetDevice::SetPhy (Ptr<CunbPhy> phy)
{
  m_phy = phy;
}

Ptr<CunbPhy>
CunbNetDevice::GetPhy (void) const
{
  return m_phy;
}

void
CunbNetDevice::CompleteConfig (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  // Verify we have all the necessary pieces
  if (m_mac == 0 || m_phy == 0 || m_node == 0 || m_configComplete)
    {
      return;
    }

  m_mac->SetPhy (m_phy);
  m_configComplete = true;
}

void
CunbNetDevice::Send (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);

  // Send the packet to the MAC layer, if it exists
  NS_ASSERT (m_mac != 0);


  // if packet is a beacon packet then call SendBeacon() else call Send()
  if(IsBeacon(packet))
  {
	 m_mac->SendBeacon(packet);
  }
  else
  {
     m_mac->Send (packet);
  }
}

bool
CunbNetDevice::IsBeacon (Ptr<Packet> packet)
{
   Ptr<Packet> packetCopy = packet->Copy();
   // Remove the Mac Header to get some information

   CunbFrameHeader frameHdr;
   packetCopy->RemoveHeader(frameHdr);
   uint32_t addr = frameHdr.GetAddress().Get();

   NS_LOG_INFO("Broadcast Address"<<addr);

   if (addr == 4294967295)
   {
	   return true;
   }
   return false;

}

void
CunbNetDevice::Receive (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);

  // Fill protocol and address with empty stuff
  m_receiveCallback (this, packet, 0, Address ());
}

/******************************************
 *    Methods inherited from NetDevice    *
 ******************************************/

Ptr<Channel>
CunbNetDevice::GetChannel (void) const
{
  NS_LOG_FUNCTION (this);
  return m_phy->GetChannel ();
}

Ptr<CunbChannel>
CunbNetDevice::DoGetChannel (void) const
{
  NS_LOG_FUNCTION (this);
  return m_phy->GetChannel ();
}

void
CunbNetDevice::SetIfIndex (const uint32_t index)
{
  NS_LOG_FUNCTION (this << index);
}

uint32_t
CunbNetDevice::GetIfIndex (void) const
{
  NS_LOG_FUNCTION (this);

  return 0;
}

void
CunbNetDevice::SetAddress (Address address)
{
  NS_LOG_FUNCTION (this);
  m_address = Mac48Address::ConvertFrom (address);
}

Address
CunbNetDevice::GetAddress (void) const
{
  NS_LOG_FUNCTION (this);
  return m_address;
  //return Address ();
}

bool
CunbNetDevice::SetMtu (const uint16_t mtu)
{
  NS_ABORT_MSG ("Unsupported");

  return false;
}

uint16_t
CunbNetDevice::GetMtu (void) const
{
  NS_LOG_FUNCTION (this);

  return 0;
}

bool
CunbNetDevice::IsLinkUp (void) const
{
  NS_LOG_FUNCTION (this);

  return m_phy != 0;
}

void
CunbNetDevice::AddLinkChangeCallback (Callback<void> callback)
{
  NS_LOG_FUNCTION (this);
}

bool
CunbNetDevice::IsBroadcast (void) const
{
  NS_LOG_FUNCTION (this);

  return true;
}

Address
CunbNetDevice::GetBroadcast (void) const
{
  NS_LOG_FUNCTION (this);

  return Address ();
}
bool
CunbNetDevice::IsMulticast (void) const
{
  NS_LOG_FUNCTION (this);

  return true;
}

Address
CunbNetDevice::GetMulticast (Ipv4Address multicastGroup) const
{
  NS_ABORT_MSG ("Unsupported");

  return Address ();
}

Address
CunbNetDevice::GetMulticast (Ipv6Address addr) const
{
  NS_LOG_FUNCTION (this);

  return Address ();
}
bool
CunbNetDevice::IsBridge (void) const
{
  NS_LOG_FUNCTION (this);

  return false;
}

bool
CunbNetDevice::IsPointToPoint (void) const
{
  NS_LOG_FUNCTION (this);

  return false;
}

bool
CunbNetDevice::Send (Ptr<Packet> packet, const Address& dest,
                     uint16_t protocolNumber)

{
  NS_LOG_FUNCTION (this << packet << dest << protocolNumber);

  // Fallback to the vanilla Send method
  Send (packet);

  return true;
}

bool
CunbNetDevice::SendFrom (Ptr<Packet> packet, const Address& source,
                         const Address& dest, uint16_t protocolNumber)

{
  //NS_ABORT_MSG ("Unsupported");
  //NS_ABORT_MSG ("Supported");
	Send(packet);
  return true;
}

Ptr<Node>
CunbNetDevice::GetNode (void) const
{
  NS_LOG_FUNCTION (this);

  return m_node;
}

void
CunbNetDevice::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this);

  m_node = node;
  CompleteConfig ();
}

bool
CunbNetDevice::NeedsArp (void) const
{
  NS_LOG_FUNCTION (this);

  return true;
}

void
CunbNetDevice::SetReceiveCallback (ReceiveCallback cb)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_receiveCallback = cb;
}

void
CunbNetDevice::SetPromiscReceiveCallback (PromiscReceiveCallback cb)
{
  NS_LOG_FUNCTION_NOARGS ();
}

bool
CunbNetDevice::SupportsSendFrom (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  NS_ABORT_MSG ("Supported");
  return true;
}

}
