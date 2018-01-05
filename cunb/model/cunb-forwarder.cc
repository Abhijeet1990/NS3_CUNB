#include "ns3/cunb-forwarder.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CunbForwarder");

NS_OBJECT_ENSURE_REGISTERED (CunbForwarder);

TypeId
CunbForwarder::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CunbForwarder")
    .SetParent<Application> ()
    .AddConstructor<CunbForwarder> ()
    .SetGroupName ("cunb");
  return tid;
}

CunbForwarder::CunbForwarder ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

CunbForwarder::~CunbForwarder ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
CunbForwarder::SetPointToPointNetDevice (Ptr<PointToPointNetDevice>
                                     pointToPointNetDevice)
{
  NS_LOG_FUNCTION (this << pointToPointNetDevice);

  m_pointToPointNetDevice = pointToPointNetDevice;
}

void
CunbForwarder::SetCunbNetDevice (Ptr<CunbNetDevice> cunbNetDevice)
{
  NS_LOG_FUNCTION (this << cunbNetDevice);

  m_cunbNetDevice = cunbNetDevice;
}

bool
CunbForwarder::ReceiveFromCunb (Ptr<NetDevice> cunbNetDevice, Ptr<const Packet>
                            packet, uint16_t protocol, const Address& sender)
{
  NS_LOG_FUNCTION (this << packet << protocol << sender);

  Ptr<Packet> packetCopy = packet->Copy ();

  m_pointToPointNetDevice->Send (packetCopy,
                                 m_pointToPointNetDevice->GetBroadcast (),
                                 0x800);

  return true;
}

bool
CunbForwarder::ReceiveFromPointToPoint (Ptr<NetDevice> pointToPointNetDevice,
                                    Ptr<const Packet> packet, uint16_t protocol,
                                    const Address& sender)
{
  NS_LOG_FUNCTION (this << packet << protocol << sender);

  Ptr<Packet> packetCopy = packet->Copy ();

  m_cunbNetDevice->Send (packetCopy);

  return true;
}

void
CunbForwarder::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  // TODO Make sure we are connected to both needed devices
}

void
CunbForwarder::StopApplication (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  // TODO Get rid of callbacks
}

}
