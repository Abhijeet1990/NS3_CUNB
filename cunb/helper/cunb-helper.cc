/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */

#include "ns3/cunb-helper.h"
#include "ns3/log.h"
#include "ns3/trace-helper.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CunbHelper");

CunbHelper::CunbHelper ()
{
}

CunbHelper::~CunbHelper ()
{
}

NetDeviceContainer
CunbHelper::Install ( const CunbPhyHelper &phyHelper,
                      const CunbMacHelper &macHelper,
                      NodeContainer c) const
{
  NS_LOG_FUNCTION_NOARGS ();

  NetDeviceContainer devices;

  // Go over the various nodes in which to install the NetDevice
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;

      // Create the CunbNetDevice
      Ptr<CunbNetDevice> device = CreateObject<CunbNetDevice> ();

      // Create the PHY
      Ptr<CunbPhy> phy = phyHelper.Create (node, device);
      NS_ASSERT (phy != 0);
      device->SetPhy (phy);
      NS_LOG_DEBUG ("Done creating the PHY");

      // Create the MAC
      Ptr<CunbMac> mac = macHelper.Create (node, device);
      NS_ASSERT (mac != 0);
      mac->SetPhy (phy);
      NS_LOG_DEBUG ("Done creating the MAC");
      device->SetMac (mac);

      node->AddDevice (device);
      devices.Add (device);
      NS_LOG_DEBUG ("node=" << node << ", mob=" << node->GetObject<MobilityModel> ());
    }
  return devices;
}

NetDeviceContainer
CunbHelper::Install ( const CunbPhyHelper &phy,
                      const CunbMacHelper &mac,
                      Ptr<Node> node) const
{
  return Install (phy, mac, NodeContainer (node));
}

void
CunbHelper::EnablePcapInternal (std::string prefix, Ptr<NetDevice> nd, bool promiscuous, bool explicitFilename)
{
  //
  // All of the Pcap enable functions vector through here including the ones
  // that are wandering through all of devices on perhaps all of the nodes in
  // the system.  We can only deal with devices of type PointToPointNetDevice.
  //
  Ptr<CunbNetDevice> device = nd->GetObject<CunbNetDevice> ();
  if (device == 0)
    {
      NS_LOG_INFO ("CunbHelper::EnablePcapInternal(): Device " << device << " not of type ns3::PointToPointNetDevice");
      return;
    }

  PcapHelper pcapHelper;

  std::string filename;
  if (explicitFilename)
    {
      filename = prefix;
    }
  else
    {
      filename = pcapHelper.GetFilenameFromDevice (prefix, device);
    }

  Ptr<PcapFileWrapper> file = pcapHelper.CreateFile (filename, std::ios::out,
                                                     PcapHelper::DLT_PPP);
  pcapHelper.HookDefaultSink<CunbNetDevice> (device, "PromiscSniffer", file);
}

}

