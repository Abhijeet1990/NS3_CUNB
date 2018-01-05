#include "ns3/beacon-sender-helper.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("BeaconSenderHelper");

BeaconSenderHelper::BeaconSenderHelper ()
{
  m_factory.SetTypeId ("ns3::BeaconSender");
}

BeaconSenderHelper::~BeaconSenderHelper ()
{
}

void
BeaconSenderHelper::SetSendTime (Time sendTime)
{
  m_sendTime = sendTime;
}

void
BeaconSenderHelper::SetAttribute (std::string name,
                                   const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
BeaconSenderHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
BeaconSenderHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
BeaconSenderHelper::InstallPriv (Ptr<Node> node) const
{
  NS_LOG_FUNCTION (this << node);

  Ptr<BeaconSender> app = m_factory.Create<BeaconSender> ();

  app->SetSendTime (m_sendTime);

  app->SetNode (node);
  node->AddApplication (app);

  return app;
}
} // namespace ns3
