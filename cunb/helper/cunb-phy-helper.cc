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


      // We expect that MacHelper instances will overwrite this setting if the
      // device will operate in a different region
      std::vector<double> frequencies;

      // Considering 180 KHz as the system bandwidth after allocating 20 KHz to Guard band
      // For the uplink we have 250 bps with D-BPSK we need at least 500 Hz for each micro channel
      // Hence we can have 180 KHz / 500 Hz i.e. 360 micro channels

      // For the downlink data rate is 600 bps . If we use the same system bandwidth we would require
      // 180 KHz/ 1200 Hz i.e. 150 micro channels.

      // So taking the minimum for both uplink and downlink we consider 150 micro channels
      double end_freq = 868.3;
      double start_freq = 868.1;
      uint8_t microchannels = 150;

      // create micro channels
      double step_size = (end_freq - start_freq)/microchannels;
      for (uint8_t i = 0; i < microchannels;i++)
      {
    	  end_freq = start_freq + step_size;
    	  double center_freq = (start_freq + end_freq)/2;
    	  //NS_LOG_INFO("Center freq "<< center_freq);
    	  frequencies.push_back(center_freq);
    	  start_freq = start_freq + step_size;

      }

      std::vector<double>::iterator it = frequencies.begin ();

      int receptionPaths = 0;
      int maxReceptionPaths = microchannels;
      while (receptionPaths < maxReceptionPaths)
        {
          if (it == frequencies.end ())
            it = frequencies.begin ();
          NS_LOG_INFO("Center freq "<< *it);
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
