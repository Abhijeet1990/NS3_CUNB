#include "ns3/Hello_helper.h"
#include "ns3/hello-sender.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("HelloHelper");

HelloHelper::HelloHelper ()
{
  m_factory.SetTypeId ("ns3::HelloSender");
}

HelloHelper::~HelloHelper ()
{
}

void
HelloHelper::SetSendTime (Time sendTime)
{
  m_sendTime = sendTime;
}

void
HelloHelper::SetMac(Ptr<MSCunbMac> mac)
{
	m_mac = mac;
}

void
HelloHelper::SetAttribute (std::string name,
                                   const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
HelloHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
HelloHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
HelloHelper::InstallPriv (Ptr<Node> node) const
{
  NS_LOG_FUNCTION (this << node);

  Ptr<HelloSender> app = m_factory.Create<HelloSender> ();
  m_mac->SetHelloSender(app);
  app->SetSendTime (m_sendTime);
  app->SetNode (node);
  node->AddApplication (app);

  return app;
}
} // namespace ns3
