#include "ns3/one-time-reporting.h"
#include "ns3/ms-cunb-mac.h"
#include "ns3/pointer.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/cunb-net-device.h"
#include "ns3/app-layer-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("OneTimeReporting");

NS_OBJECT_ENSURE_REGISTERED (OneTimeReporting);

TypeId
OneTimeReporting::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::OneTimeReporting")
    .SetParent<Application> ()
    .AddConstructor<OneTimeReporting> ()
    .SetGroupName ("cunb");
  return tid;
}

OneTimeReporting::OneTimeReporting ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

OneTimeReporting::OneTimeReporting (Time sendTime) :
  m_sendTime (sendTime)
{
  NS_LOG_FUNCTION_NOARGS ();
}

OneTimeReporting::~OneTimeReporting ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
OneTimeReporting::SetSendTime (Time sendTime)
{
  NS_LOG_FUNCTION (this << sendTime);

  m_sendTime = sendTime;
}

void
OneTimeReporting::SendPacket (void)
{
  NS_LOG_FUNCTION (this);

  // Create and send a new packet
  Ptr<Packet> packet = Create<Packet>(10);

  AppLayerHeader appHdr;
  appHdr.SetPtype(1);
  packet->AddHeader(appHdr);

  m_mac->GetObject<MSCunbMac> ()->SetMType
    (CunbMacHeaderUl::SINGLE_ACK);
  m_mac->Send (packet);
}

void
OneTimeReporting::StartApplication (void)
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
  m_sendEvent = Simulator::Schedule (m_sendTime, &OneTimeReporting::SendPacket,
                                     this);
}

void
OneTimeReporting::StopApplication (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Simulator::Cancel (m_sendEvent);
}
}
