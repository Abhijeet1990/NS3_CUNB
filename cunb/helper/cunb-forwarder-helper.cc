#include "ns3/cunb-forwarder-helper.h"
#include "ns3/random-variable-stream.h"
#include "ns3/cunb-forwarder.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CunbCunbForwarderHelper");

CunbForwarderHelper::CunbForwarderHelper ()
{
  m_factory.SetTypeId ("ns3::CunbForwarder");
}

CunbForwarderHelper::~CunbForwarderHelper ()
{
}

void
CunbForwarderHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
CunbForwarderHelper::Install (Ptr<Node> node) const
{
  return ApplicationContainer (InstallPriv (node));
}

ApplicationContainer
CunbForwarderHelper::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }

  return apps;
}

Ptr<Application>
CunbForwarderHelper::InstallPriv (Ptr<Node> node) const
{
  NS_LOG_FUNCTION (this << node);

  Ptr<CunbForwarder> app = m_factory.Create<CunbForwarder> ();

  app->SetNode (node);
  node->AddApplication (app);

  // Link the Forwarder to the NetDevices
  for (uint32_t i = 0; i < node->GetNDevices (); i++)
    {
      Ptr<NetDevice> currentNetDevice = node->GetDevice (i);
      if (currentNetDevice->GetObject<CunbNetDevice> () != 0)
        {
          Ptr<CunbNetDevice> cunbNetDevice =
            currentNetDevice->GetObject<CunbNetDevice> ();
          app->SetCunbNetDevice (cunbNetDevice);
          cunbNetDevice->SetReceiveCallback (MakeCallback
                                               (&CunbForwarder::ReceiveFromCunb, app));
        }
      else if (currentNetDevice->GetObject<PointToPointNetDevice> () != 0)
        {
          Ptr<PointToPointNetDevice> pointToPointNetDevice =
            currentNetDevice->GetObject<PointToPointNetDevice> ();

          app->SetPointToPointNetDevice (pointToPointNetDevice);

          pointToPointNetDevice->SetReceiveCallback (MakeCallback
                                                       (&CunbForwarder::ReceiveFromPointToPoint,
                                                       app));
        }
      else
        {
          NS_LOG_ERROR ("Potential error: NetDevice is neither Cunb nor PointToPoint");
        }
    }

  return app;
}
} // namespace ns3
