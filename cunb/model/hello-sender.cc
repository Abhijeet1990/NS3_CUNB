#include "ns3/hello-sender.h"
#include "ns3/ms-cunb-mac.h"
#include "ns3/pointer.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/cunb-net-device.h"
#include "ns3/app-layer-header.h"
#include "ns3/new-cosem-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("HelloSender");

NS_OBJECT_ENSURE_REGISTERED (HelloSender);

TypeId
HelloSender::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::HelloSender")
    .SetParent<Application> ()
    .AddConstructor<HelloSender> ()
    .SetGroupName ("cunb");
  return tid;
}

HelloSender::HelloSender ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

HelloSender::HelloSender (Time sendTime) :
  m_sendTime (sendTime)
{
  NS_LOG_FUNCTION_NOARGS ();
}

HelloSender::~HelloSender ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
HelloSender::SetSendTime (Time sendTime)
{
  NS_LOG_FUNCTION (this << sendTime);

  m_sendTime = sendTime;
}

void
HelloSender::SendPacket (void)
{
  NS_LOG_FUNCTION (this);

  // Create and send a new packet
  Ptr<Packet> packet = Create<Packet>();

  AppLayerHeader appHdr;
  appHdr.SetPtype(10);
  packet->AddHeader(appHdr);

  NewCosemWrapperHeader wrapperHdr;
  uint16_t sourceWPort = 80;
  uint16_t destWPort = 90;
  wrapperHdr.SetSrcwPort (sourceWPort);
  wrapperHdr.SetDstwPort (destWPort);
  wrapperHdr.SetLength (packet->GetSize ());
  packet->AddHeader (wrapperHdr);

  m_mac->GetObject<MSCunbMac> ()->SetMType
    (CunbMacHeaderUl::HELLO);
  m_mac->Send (packet);
}

void
HelloSender::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  // Make sure we have a MAC layer
  if (m_mac == 0)
    {
      // Assumes there's only one device
      Ptr<CunbNetDevice> cunbNetDevice = m_node->GetDevice (0)->GetObject<CunbNetDevice> ();

      m_mac = cunbNetDevice->GetMac ();
      NS_ASSERT (m_mac != 0);
    }

  // Schedule the next SendPacket event
  Simulator::Cancel (m_sendEvent);
  m_sendEvent = Simulator::Schedule (m_sendTime, &HelloSender::SendPacket,
                                     this);
}

void
HelloSender::StopApplication (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Simulator::Cancel (m_sendEvent);
}
}
