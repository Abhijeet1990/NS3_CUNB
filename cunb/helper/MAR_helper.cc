
#include "ns3/MAR_helper.h"
#include "ns3/random-variable-stream.h"
#include "ns3/mobile-autonomous-reporting.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/simulator.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MARHelper");

MARHelper::MARHelper ()
{
  m_factory.SetTypeId ("ns3::MobileAutonomousReporting");

  //m_factory.Set ("PacketSize", StringValue ("ns3::ParetoRandomVariable[Bound=10|Shape=2.5]"));
  m_expInterval = CreateObject<ExponentialRandomVariable> ();
  m_initialDelay = CreateObject<UniformRandomVariable> ();
  m_initialDelay->SetAttribute ("Min", DoubleValue (0));
  m_packetSize = 32; // Application Payload size can vary from 0-32 bytes
  m_intervalProb = CreateObject<UniformRandomVariable> ();
}

MARHelper::~MARHelper ()
{
}

void
MARHelper::SetAttribute (std::string name, const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
MARHelper::Install (Ptr<Node> node,enum packetType pType) const
{
  return ApplicationContainer (InstallPriv (node,pType));
}

ApplicationContainer
MARHelper::Install (NodeContainer c,enum packetType pType) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i,pType));
    }

  return apps;
}

Ptr<Application>
MARHelper::InstallPriv (Ptr<Node> node, enum packetType pType) const
{
  NS_LOG_FUNCTION (this << node);

  Ptr<MobileAutonomousReporting> app = m_factory.Create<MobileAutonomousReporting> ();

  Time interval;

  switch(pType)
  {
  case KeepAlive:
	  m_intervalProb->SetAttribute("Min", DoubleValue (0.0));
	  m_intervalProb->SetAttribute ("Max", DoubleValue (60.0));
	  interval = Seconds(m_intervalProb->GetValue());
	  break;
  case Normal:
  	  m_intervalProb->SetAttribute("Min", DoubleValue (0.0));
  	  m_intervalProb->SetAttribute ("Max", DoubleValue (300.0));
  	  interval = Seconds(m_intervalProb->GetValue());
  	  break;
  case Alarm:
	  m_expInterval->SetAttribute ("Mean", DoubleValue (1.0));
  	  interval = Hours(m_expInterval->GetValue());
  	  break;
  default:
	  interval = m_period;
	  break;
  }

  app->SetInterval (interval);
  NS_LOG_DEBUG ("Created an application with interval = " <<
                interval.GetHours () << " hours");

  app->SetInitialDelay (Seconds (m_initialDelay->GetValue (0, interval.GetSeconds ())));

  app->SetNode (node);
  node->AddApplication (app);

  return app;
}

void
MARHelper::SetPeriod (Time period)
{
  m_period = period;
}
} // namespace ns3

