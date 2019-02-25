/*
 * This example creates a simple network in which all CUNB components are
 * simulated: MSs, some eNBs and a CUNBServer.
 */

#include "ns3/point-to-point-module.h"
#include "ns3/cunb-forwarder-helper.h"
#include "ns3/cunb-server-helper.h"
#include "ns3/cunb-interference-helper.h"
#include "ns3/cunb-channel.h"
#include "ns3/mobility-helper.h"
#include "ns3/cunb-phy-helper.h"
#include "ns3/cunb-mac-helper.h"
#include "ns3/cunb-helper.h"
#include "ns3/enb-cunb-phy.h"
#include "ns3/mobile-autonomous-reporting.h"
#include "ns3/MAR_helper.h"
#include "ns3/log.h"
#include "ns3/string.h"
#include "ns3/command-line.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/cunb-device-address-generator.h"
#include "ns3/OTR_Helper.h"
#include "ns3/cunb-beacon-header.h"
#include "ns3/cunb-beacon-trailer.h"
#include "ns3/beacon-sender-helper.h"
#include "ns3/sub-band-cunb.h"
#include "ns3/cunb-mac-trailer-ul.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("MultipleMsBs");

int main (int argc, char *argv[])
{

  bool verbose = false;

  CommandLine cmd;
  cmd.AddValue ("verbose", "Whether to print output or not", verbose);
  cmd.Parse (argc, argv);

  // Logging
  //////////

  LogComponentEnable ("MultipleMsBs", LOG_LEVEL_ALL);
  LogComponentEnable ("SimpleCunbServer", LOG_LEVEL_ALL);
  LogComponentEnable ("EnbCunbMac", LOG_LEVEL_ALL);
  //LogComponentEnable("CunbFrameHeader", LOG_LEVEL_ALL);
  //LogComponentEnable("CunbMacHeader", LOG_LEVEL_ALL);
  //LogComponentEnable("CunbFrameHeaderUl", LOG_LEVEL_ALL);
  //LogComponentEnable("CunbMacHeaderUl", LOG_LEVEL_ALL);
  //LogComponentEnable("CunbMacTrailerUl", LOG_LEVEL_ALL);
  //LogComponentEnable("MacCommand", LOG_LEVEL_ALL);
  //LogComponentEnable("EnbCunbPhy", LOG_LEVEL_ALL);
  //LogComponentEnable("CunbPhy", LOG_LEVEL_ALL);
  //LogComponentEnable("CunbChannel", LOG_LEVEL_ALL);
  //LogComponentEnable("MSCunbPhy", LOG_LEVEL_ALL);
  //LogComponentEnable("LogicalCunbChannelHelper", LOG_LEVEL_ALL);
  LogComponentEnable ("MSCunbMac", LOG_LEVEL_ALL);
  //LogComponentEnable ("OneTimeReporting", LOG_LEVEL_ALL);
  //LogComponentEnable("PointToPointNetDevice", LOG_LEVEL_ALL);
  LogComponentEnable ("CunbForwarder", LOG_LEVEL_ALL);
  LogComponentEnable ("OneShotSender", LOG_LEVEL_ALL);
  //LogComponentEnable ("MSStatus", LOG_LEVEL_ALL);
  //LogComponentEnable ("EnbStatus", LOG_LEVEL_ALL);
  //LogComponentEnable("CunbInterferenceHelper",LOG_LEVEL_ALL);
  LogComponentEnableAll (LOG_PREFIX_FUNC);
  LogComponentEnableAll (LOG_PREFIX_NODE);
  LogComponentEnableAll (LOG_PREFIX_TIME);

  // Create a simple wireless channel
  ///////////////////////////////////

  Ptr<LogDistancePropagationLossModel> loss = CreateObject<LogDistancePropagationLossModel> ();
  loss->SetPathLossExponent (3.76);
  loss->SetReference (1, 8.1);

  Ptr<PropagationDelayModel> delay = CreateObject<ConstantSpeedPropagationDelayModel> ();

  Ptr<CunbChannel> channel = CreateObject<CunbChannel> (loss, delay);


  // Helpers
  //////////

  // End Device mobility
  MobilityHelper mobilityMs, mobilityEnb;
  Ptr<ListPositionAllocator> positionAllocEd = CreateObject<ListPositionAllocator> ();
  positionAllocEd->Add (Vector (1000.0, 0.0, 0.0));
  positionAllocEd->Add (Vector (2000.0, 0.0, 0.0));
  positionAllocEd->Add (Vector (-300.0, 0.0, 0.0));
  positionAllocEd->Add (Vector (1200.0, 0.0, 0.0));
  positionAllocEd->Add (Vector (-1700.0, 0.0, 0.0));
  positionAllocEd->Add (Vector (800.0, 0.0, 0.0));
  positionAllocEd->Add (Vector (-1900.0, 0.0, 0.0));
  positionAllocEd->Add (Vector (600.0, 0.0, 0.0));
  positionAllocEd->Add (Vector (-1300.0, 0.0, 0.0));
  positionAllocEd->Add (Vector (700.0, 0.0, 0.0));
  mobilityMs.SetPositionAllocator (positionAllocEd);
  // mobilityEd.SetPositionAllocator ("ns3::UniformDiscPositionAllocator",
  //                                "rho", DoubleValue (7500),
  //                                "X", DoubleValue (0.0),
  //                                "Y", DoubleValue (0.0));
  mobilityMs.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  // eNB mobility
  Ptr<ListPositionAllocator> positionAllocEnb = CreateObject<ListPositionAllocator> ();
  //positionAllocEnb->Add (Vector (0.0, 0.0, 0.0));
  positionAllocEnb->Add (Vector (-2000.0, 0.0, 0.0));
  positionAllocEnb->Add (Vector (500.0, 0.0, 0.0));
  mobilityEnb.SetPositionAllocator (positionAllocEnb);
  mobilityEnb.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  // Create the CunbPhyHelper
  CunbPhyHelper phyHelper = CunbPhyHelper ();
  phyHelper.SetChannel (channel);

  // Create the CunbMacHelper
  CunbMacHelper macHelper = CunbMacHelper ();

  // Create the CunbHelper
  CunbHelper helper = CunbHelper ();

  // Create MSs
  /////////////

  NodeContainer endDevices;
  endDevices.Create (10);
  mobilityMs.Install (endDevices);

  // Create a CunbDeviceAddressGenerator
  uint8_t nwkId = 54;
  uint32_t nwkAddr = 1864;
  Ptr<CunbDeviceAddressGenerator> addrGen = CreateObject<CunbDeviceAddressGenerator> (nwkId,nwkAddr);

  // Create the CunbNetDevices of the end devices
  phyHelper.SetDeviceType (CunbPhyHelper::MS);
  macHelper.SetDeviceType (CunbMacHelper::MS);
  macHelper.SetAddressGenerator (addrGen);
  //macHelper.SetRegion (CunbMacHelper::EU);
  helper.Install (phyHelper, macHelper, endDevices);

  // Install applications in MSs


  OTRHelper oneTimeHelper[10];
  ApplicationContainer appContainer;
  for(int i = 0; i<10 ;i++)
  {
	  oneTimeHelper[i].SetSendTime(Seconds(10*i));
	  Ptr<MSCunbMac> mac = endDevices.Get(i)->GetDevice(0)->GetObject<CunbNetDevice>()->GetMac()->GetObject<MSCunbMac>();
	  oneTimeHelper[i].SetMac(mac);
	  appContainer = oneTimeHelper[i].Install(endDevices.Get(i));
  }

  appContainer.Start (Seconds (0));
  appContainer.Stop (Seconds(110));


  ////////////////
  // Create eNBs //
  ////////////////

  NodeContainer enbs;
  enbs.Create (2);
  mobilityEnb.Install (enbs);

  /*
  BeaconSenderHelper beaconHelper = BeaconSenderHelper();
  beaconHelper.SetSendTime(Seconds(3));
  ApplicationContainer beaconAppContainer = beaconHelper.Install (enbs.Get (0));
  beaconAppContainer.Start (Seconds (0));
  beaconAppContainer.Stop (Seconds(8));
*/

  // Create the CunbNetDevices of the enbs
  phyHelper.SetDeviceType (CunbPhyHelper::ENB);
  macHelper.SetDeviceType (CunbMacHelper::ENB);
  helper.Install (phyHelper, macHelper, enbs);

  // Set spreading factors up
  macHelper.SetSpreadingFactorsUp (endDevices, enbs, channel);

  ////////////
  // Create CUNB Server
  ////////////

  NodeContainer cunbServers;
  cunbServers.Create (1);

  // Install the SimpleCunbServer application on the cunb server
  CunbServerHelper cunbServerHelper;
  cunbServerHelper.SetEnbs (enbs);
  cunbServerHelper.SetMSs(endDevices);
  cunbServerHelper.Install (cunbServers);

  // Install the Forwarder application on the eNBs
  CunbForwarderHelper forwarderHelper;
  forwarderHelper.Install (enbs);

  // Start simulation
  Simulator::Stop (Seconds (110));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
