#include "ns3/beacon-sender.h"
#include "ns3/ms-cunb-mac.h"
#include "ns3/pointer.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/cunb-net-device.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("BeaconSender");

NS_OBJECT_ENSURE_REGISTERED (BeaconSender);

TypeId
BeaconSender::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::BeaconSender")
    .SetParent<Application> ()
    .AddConstructor<BeaconSender> ()
    .SetGroupName ("cunb");
  return tid;
}

BeaconSender::BeaconSender ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

BeaconSender::BeaconSender (Time sendTime) :
  m_sendTime (sendTime)
{
  NS_LOG_FUNCTION_NOARGS ();
}

BeaconSender::~BeaconSender ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
BeaconSender::SetSendTime (Time sendTime)
{
  NS_LOG_FUNCTION (this << sendTime);

  m_sendTime = sendTime;
}

void
BeaconSender::SendPacket (void)
{
  NS_LOG_FUNCTION (this);

  // Create and send a new packet
  Ptr<Packet> beaconPacket = Create<Packet>(reinterpret_cast<const uint8_t*> ("hello"),10);

  m_mac->SendBeacon(beaconPacket);
}

void
BeaconSender::StartApplication (void)
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
  m_sendEvent = Simulator::Schedule (m_sendTime, &BeaconSender::SendPacket,
                                     this);
}

void
BeaconSender::StopApplication (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Simulator::Cancel (m_sendEvent);
}
}
