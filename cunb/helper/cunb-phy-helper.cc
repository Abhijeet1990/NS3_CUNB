#include "ns3/cunb-phy-helper.h"
#include "ns3/log.h"
#include "ns3/sub-band.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CunbPhyHelper");

CunbPhyHelper::CunbPhyHelper ()
{
}

void
CunbPhyHelper::SetChannel (Ptr<CunbChannel> channel)
{
  m_channel = channel;
}

void
CunbPhyHelper::SetDeviceType (enum DeviceType dt)
{

  NS_LOG_FUNCTION (this << dt);
  switch (dt)
    {
    case ENB:
      m_phy.SetTypeId ("ns3::EnbCunbPhy");
      break;
    case MS:
      m_phy.SetTypeId ("ns3::MSCunbPhy");
      break;
    }
}

void
CunbPhyHelper::Set (std::string name, const AttributeValue &v)
{
  m_phy.Set (name, v);
}

Ptr<CunbPhy>
CunbPhyHelper::Create (Ptr<Node> node, Ptr<NetDevice> device) const
{
  NS_LOG_FUNCTION (this << node << device);

  // Create the PHY and set its channel
  Ptr<CunbPhy> phy = m_phy.Create<CunbPhy> ();
  phy->SetChannel (m_channel);

  // Configuration is different based on the kind of device we have to create
  std::string typeId = m_phy.GetTypeId ().GetName ();
  if (typeId == "ns3::EnbCunbPhy")
    {
      // Inform the channel of the presence of this PHY
      m_channel->Add (phy);

      // For now, assume that the PHY will listen to the default EU channels
      // with this ReceivePath configuration:
      // 3 ReceivePaths on 868.1
      // 3 ReceivePaths on 868.3
      // 2 ReceivePaths on 868.5

      // We expect that MacHelper instances will overwrite this setting if the
      // device will operate in a different region
      std::vector<double> frequencies;
      frequencies.push_back (868.1);
      frequencies.push_back (868.3);
      frequencies.push_back (868.5);

      std::vector<double>::iterator it = frequencies.begin ();

      int receptionPaths = 0;
      int maxReceptionPaths = 8;
      while (receptionPaths < maxReceptionPaths)
        {
          if (it == frequencies.end ())
            it = frequencies.begin ();
          phy->GetObject<EnbCunbPhy> ()->AddReceptionPath (*it);
          ++it;
          receptionPaths++;
        }

    }
  else if (typeId == "ns3::MSCunbPhy")
    {
      // The line below can be commented to speed up uplink-only simulations.
      // This implies that the CunbChannel instance will only know about
      // eNBs, and it will not lose time delivering packets and interference
      // information to devices which will never listen.

      m_channel->Add (phy);
    }

  // Link the PHY to its net device
  phy->SetDevice (device);

  return phy;
}
}
