#include "ns3/OTRe_Helper.h"
#include "ns3/one-time-requesting.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("OTReHelper");

OTReHelper::OTReHelper ()
{
  m_factory.SetTypeId ("ns3::OneTimeRequesting");
}

OTReHelper::~OTReHelper ()
{
}

void
OTReHelper::SetMac(Ptr<EnbCunbMac> mac)
{
	m_mac = mac;
}


void
OTReHelper::SetMS(Ptr<Node> ms)
{
	m_ms = ms;
}

Ptr<Node>
OTReHelper::GetMS(void)
{
	return m_ms;
}
void
OTReHelper::SetSendTime (Time sendTime)
{
  m_sendTime = sendTime;
}

void
OTReHelper::SetPtype(uint8_t pType)
{
	m_ptype = pType;
}

uint8_t
OTReHelper::GetPtype()
{
	return m_ptype;
}

void
OTReHelper::SetAttribute (std::string name,
                                   const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
OTReHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
OTReHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
OTReHelper::InstallPriv (Ptr<Node> node ) const
{
  NS_LOG_FUNCTION (this << node);

  Ptr<OneTimeRequesting> app = m_factory.Create<OneTimeRequesting> ();
  app->SetMS(m_ms);
  app->SetSendTime (m_sendTime);
  app->SetMac(m_mac);
  app->SetNode (node);
  node->AddApplication (app);

  return app;
}
} // namespace ns3
