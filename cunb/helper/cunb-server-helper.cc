#include "ns3/cunb-server-helper.h"
#include "ns3/simple-cunb-server.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CunbServerHelper");

CunbServerHelper::CunbServerHelper ()
{
  m_factory.SetTypeId ("ns3::SimpleCunbServer");
  p2pHelper.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  p2pHelper.SetChannelAttribute ("Delay", StringValue ("2ms"));
}

CunbServerHelper::~CunbServerHelper ()
{
}

void
CunbServerHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

void
CunbServerHelper::SetEnbs (NodeContainer enbs)
{
  m_enbs = enbs;
}

void
CunbServerHelper::SetMSs (NodeContainer mss)
{
  m_mss = mss;
}

ApplicationContainer
CunbServerHelper::Install (Ptr<Node> node)
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
CunbServerHelper::Install (NodeContainer c)
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
CunbServerHelper::InstallPriv (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this << node);

  Ptr<SimpleCunbServer> app = m_factory.Create<SimpleCunbServer> ();

  app->SetNode (node);
  node->AddApplication (app);

  // Cycle on each gateway
  for (NodeContainer::Iterator i = m_enbs.Begin ();
       i != m_enbs.End ();
       i++)
    {
      // Add the connections with the eNB
      // Create a PointToPoint link between eNB and CUNB server
      NetDeviceContainer container = p2pHelper.Install (node, *i);

      // Add the eNB to the CUNB server list
      app->AddEnb(*i, container.Get (0));
    }

  // Link the SimpleCunbServer to its NetDevices
  for (uint32_t i = 0; i < node->GetNDevices (); i++)
    {
      Ptr<NetDevice> currentNetDevice = node->GetDevice (i);
      currentNetDevice->SetReceiveCallback (MakeCallback
                                              (&SimpleCunbServer::Receive,
                                              app));
    }

  // Add the end devices
  app->AddNodes (m_mss);

  return app;
}
} // namespace ns3
