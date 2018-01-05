#include "ns3/OTR_Helper.h"
#include "ns3/one-time-reporting.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("OTRHelper");

OTRHelper::OTRHelper ()
{
  m_factory.SetTypeId ("ns3::OneTimeReporting");
}

OTRHelper::~OTRHelper ()
{
}

void
OTRHelper::SetSendTime (Time sendTime)
{
  m_sendTime = sendTime;
}

void
OTRHelper::SetAttribute (std::string name,
                                   const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
OTRHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
OTRHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
OTRHelper::InstallPriv (Ptr<Node> node) const
{
  NS_LOG_FUNCTION (this << node);

  Ptr<OneTimeReporting> app = m_factory.Create<OneTimeReporting> ();

  app->SetSendTime (m_sendTime);

  app->SetNode (node);
  node->AddApplication (app);

  return app;
}
} // namespace ns3
