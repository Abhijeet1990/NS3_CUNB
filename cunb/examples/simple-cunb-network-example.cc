/*
 * This script simulates a complex scenario with multiple enbs and mss
 * . The metric of interest for this script is the throughput of the
 * network.
 */

#include "ns3/ms-cunb-phy.h"
#include "ns3/enb-cunb-phy.h"
#include "ns3/ms-cunb-mac.h"
#include "ns3/enb-cunb-mac.h"
#include "ns3/cunb-net-device.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "ns3/pointer.h"
#include "ns3/constant-position-mobility-model.h"
#include "ns3/cunb-helper.h"
#include "ns3/node-container.h"
#include "ns3/mobility-helper.h"
#include "ns3/position-allocator.h"
#include "ns3/double.h"
#include "ns3/random-variable-stream.h"
#include "ns3/MAR_helper.h"
#include "ns3/command-line.h"
#include <algorithm>
#include <ctime>
#include "ns3/tap-bridge-module.h"
#include <iostream>
#include <fstream>
#include "ns3/string.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ComplexCunbNetworkExample");

// Network settings
int nMSs = 2;
int nEnbs = 1;
double radius = 7500;
double eNBRadius = 7500;
double simulationTime = 1000;
int appPeriodSeconds = 60;
std::vector<int> sfQuantity (6);

int noMoreReceivers = 0;
int interfered = 0;
int received = 0;
int underSensitivity = 0;

// Output control
bool printMSs = true;
bool buildingsEnabled = false;

/**********************
 *  Global Callbacks  *
 **********************/

enum PacketOutcome {
  RECEIVED,
  INTERFERED,
  NO_MORE_RECEIVERS,
  UNDER_SENSITIVITY,
  UNSET
};

struct PacketStatus {
  Ptr<Packet const> packet;
  uint32_t senderId;
  int outcomeNumber;
  std::vector<enum PacketOutcome> outcomes;
};

std::map<Ptr<Packet const>, PacketStatus> packetTracker;

void
CheckReceptionByAllGWsComplete (std::map<Ptr<Packet const>, PacketStatus>::iterator it)
{
  // Check whether this packet is received by all gateways
  if ((*it).second.outcomeNumber == nEnbs)
    {
      // Update the statistics
      PacketStatus status = (*it).second;
      for (int j = 0; j < nEnbs; j++)
        {
          switch (status.outcomes.at (j))
            {
            case RECEIVED:
              {
                received += 1;
                break;
              }
            case UNDER_SENSITIVITY:
              {
                underSensitivity += 1;
                break;
              }
            case NO_MORE_RECEIVERS:
              {
                noMoreReceivers += 1;
                break;
              }
            case INTERFERED:
              {
                interfered += 1;
                break;
              }
            case UNSET:
              {
                break;
              }
            }
        }
      // Remove the packet from the tracker
      packetTracker.erase (it);
    }
}

void
TransmissionCallback (Ptr<Packet const> packet, uint32_t systemId)
{
  // NS_LOG_INFO ("Transmitted a packet from device " << systemId);
  // Create a packetStatus
  PacketStatus status;
  status.packet = packet;
  status.senderId = systemId;
  status.outcomeNumber = 0;
  status.outcomes = std::vector<enum PacketOutcome> (nEnbs, UNSET);

  packetTracker.insert (std::pair<Ptr<Packet const>, PacketStatus> (packet, status));
}

void
PacketReceptionCallback (Ptr<Packet const> packet, uint32_t systemId)
{
  // Remove the successfully received packet from the list of sent ones
  // NS_LOG_INFO ("A packet was successfully received at gateway " << systemId);

  std::map<Ptr<Packet const>, PacketStatus>::iterator it = packetTracker.find (packet);
  (*it).second.outcomes.at (systemId - nMSs) = RECEIVED;
  (*it).second.outcomeNumber += 1;

  CheckReceptionByAllGWsComplete (it);
}

void
InterferenceCallback (Ptr<Packet const> packet, uint32_t systemId)
{
  // NS_LOG_INFO ("A packet was lost because of interference at gateway " << systemId);

  std::map<Ptr<Packet const>, PacketStatus>::iterator it = packetTracker.find (packet);
  (*it).second.outcomes.at (systemId - nMSs) = INTERFERED;
  (*it).second.outcomeNumber += 1;

  CheckReceptionByAllGWsComplete (it);
}

void
NoMoreReceiversCallback (Ptr<Packet const> packet, uint32_t systemId)
{
  // NS_LOG_INFO ("A packet was lost because there were no more receivers at gateway " << systemId);

  std::map<Ptr<Packet const>, PacketStatus>::iterator it = packetTracker.find (packet);
  (*it).second.outcomes.at (systemId - nMSs) = NO_MORE_RECEIVERS;
  (*it).second.outcomeNumber += 1;

  CheckReceptionByAllGWsComplete (it);
}

void
UnderSensitivityCallback (Ptr<Packet const> packet, uint32_t systemId)
{
  // NS_LOG_INFO ("A packet arrived at the gateway under sensitivity at gateway " << systemId);

  std::map<Ptr<Packet const>, PacketStatus>::iterator it = packetTracker.find (packet);
  (*it).second.outcomes.at (systemId - nMSs) = UNDER_SENSITIVITY;
  (*it).second.outcomeNumber += 1;

  CheckReceptionByAllGWsComplete (it);
}

time_t oldtime = std::time (0);

// Periodically print simulation time
void PrintSimulationTime (void)
{
  // NS_LOG_INFO ("Time: " << Simulator::Now().GetHours());
  std::cout << "Simulated time: " << Simulator::Now ().GetHours () << " hours" << std::endl;
  std::cout << "Real time from last call: " << std::time (0) - oldtime << " seconds" << std::endl;
  oldtime = std::time (0);
  Simulator::Schedule (Minutes (30), &PrintSimulationTime);
}

void
PrintEndDevices (NodeContainer endDevices, NodeContainer eNBs, std::string filename)
{
  const char * c = filename.c_str ();
  std::ofstream spreadingFactorFile;
  spreadingFactorFile.open (c);
  for (NodeContainer::Iterator j = endDevices.Begin (); j != endDevices.End (); ++j)
    {
      Ptr<Node> object = *j;
      Ptr<MobilityModel> position = object->GetObject<MobilityModel> ();
      NS_ASSERT (position != 0);
      Ptr<NetDevice> netDevice = object->GetDevice (0);
      Ptr<CunbNetDevice> cunbNetDevice = netDevice->GetObject<CunbNetDevice> ();
      NS_ASSERT (cunbNetDevice != 0);
      Ptr<MSCunbMac> mac = cunbNetDevice->GetMac ()->GetObject<MSCunbMac> ();
      int sf = int(mac->GetDataRate ());
      Vector pos = position->GetPosition ();
      spreadingFactorFile << pos.x << " " << pos.y << " " << sf << std::endl;
    }
  // Also print the eNBs
  for (NodeContainer::Iterator j = eNBs.Begin (); j != eNBs.End (); ++j)
    {
      Ptr<Node> object = *j;
      Ptr<MobilityModel> position = object->GetObject<MobilityModel> ();
      Vector pos = position->GetPosition ();
      spreadingFactorFile << pos.x << " " << pos.y << " ENB" << std::endl;
    }
  spreadingFactorFile.close ();
}

int main (int argc, char *argv[])
{

  CommandLine cmd;
  cmd.AddValue ("nDevices", "Number of mobile stations to include in the simulation", nMSs);
  cmd.AddValue ("nEnbs", "Number of Base Stations to include", nEnbs);
  cmd.AddValue ("radius", "The radius of the area to simulate", radius);
  cmd.AddValue ("simulationTime", "The time for which to simulate", simulationTime);
  cmd.AddValue ("appPeriod", "The period in seconds to be used by periodically transmitting applications", appPeriodSeconds);
  cmd.AddValue ("printEDs", "Whether or not to print a file containing the MS's positions", printMSs);

  cmd.Parse (argc, argv);

  // Set up logging
  LogComponentEnable ("ComplexCunbNetworkExample", LOG_LEVEL_ALL);
  LogComponentEnable("CunbChannel", LOG_LEVEL_INFO);
  LogComponentEnable("CunbPhy", LOG_LEVEL_ALL);
  LogComponentEnable("MSCunbPhy", LOG_LEVEL_ALL);
  LogComponentEnable("EnbCunbPhy", LOG_LEVEL_ALL);
  LogComponentEnable("CunbInterferenceHelper", LOG_LEVEL_ALL);
  LogComponentEnable("CunbMac", LOG_LEVEL_ALL);
  LogComponentEnable("MSCunbMac", LOG_LEVEL_ALL);
  LogComponentEnable("EnbCunbMac", LOG_LEVEL_ALL);
  LogComponentEnable("LogicalCunbChannelHelper", LOG_LEVEL_ALL);
  LogComponentEnable("LogicalCunbChannel", LOG_LEVEL_ALL);
  LogComponentEnable("CunbHelper", LOG_LEVEL_ALL);
  LogComponentEnable("CunbPhyHelper", LOG_LEVEL_ALL);
  LogComponentEnable("CunbMacHelper", LOG_LEVEL_ALL);
  LogComponentEnable("MARHelper", LOG_LEVEL_ALL);
  LogComponentEnable("MobileAutonomousReporting", LOG_LEVEL_ALL);
  LogComponentEnable("CunbMacHeader", LOG_LEVEL_ALL);
  LogComponentEnable("CunbFrameHeader", LOG_LEVEL_ALL);
  LogComponentEnable("SimpleCunbServer", LOG_LEVEL_ALL);

  /***********
  *  Setup  *
  ***********/

  // Compute the number of gateways
  //nGateways = 3*gatewayRings*gatewayRings-3*gatewayRings+1;

  // Create the time value from the period
  Time appPeriod = Seconds (appPeriodSeconds);

  // Mobility
  MobilityHelper mobility;
  mobility.SetPositionAllocator ("ns3::UniformDiscPositionAllocator",
                                 "rho", DoubleValue (radius),
                                 "X", DoubleValue (0.0),
                                 "Y", DoubleValue (0.0));
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");

  /************************
  *  Create the channel  *
  ************************/

  // Create the Cunb channel object
  Ptr<LogDistancePropagationLossModel> loss = CreateObject<LogDistancePropagationLossModel> ();
  loss->SetPathLossExponent (3.76);
  loss->SetReference (1, 8.1);

  Ptr<PropagationDelayModel> delay = CreateObject<ConstantSpeedPropagationDelayModel> ();

  Ptr<CunbChannel> channel = CreateObject<CunbChannel> (loss, delay);

  /************************
  *  Create the helpers  *
  ************************/

  // Create the CunbPhyHelper
  CunbPhyHelper phyHelper = CunbPhyHelper ();
  phyHelper.SetChannel (channel);

  // Create the CunbMacHelper
  CunbMacHelper macHelper = CunbMacHelper ();

  // Create the CunbHelper
  CunbHelper helper = CunbHelper ();

  /************************
  *  Create End Devices  *
  ************************/

  // Create a set of nodes
  NodeContainer endDevices;
  endDevices.Create (nMSs);

  // Assign a mobility model to each node
  mobility.Install (endDevices);

  // Make it so that nodes are at a certain height > 0
  for (NodeContainer::Iterator j = endDevices.Begin ();
       j != endDevices.End (); ++j)
    {
      Ptr<MobilityModel> mobility = (*j)->GetObject<MobilityModel> ();
      Vector position = mobility->GetPosition ();
      position.z = 1.2;
      mobility->SetPosition (position);
    }

  // Create the CunbNetDevices of the end devices
  phyHelper.SetDeviceType (CunbPhyHelper::MS);
  macHelper.SetDeviceType (CunbMacHelper::MS);
  NetDeviceContainer msDeviceContainer = helper.Install (phyHelper, macHelper, endDevices);

  // Now end devices are connected to the channel

  // Connect trace sources
  for (NodeContainer::Iterator j = endDevices.Begin ();
       j != endDevices.End (); ++j)
    {
      Ptr<Node> node = *j;
      Ptr<CunbNetDevice> cunbNetDevice = node->GetDevice (0)->GetObject<CunbNetDevice> ();
      Ptr<CunbPhy> phy = cunbNetDevice->GetPhy ();
      phy->TraceConnectWithoutContext ("StartSending",
                                       MakeCallback (&TransmissionCallback));
    }



  /*********************
  *  Create eNBs  *
  *********************/

  // Create the gateway nodes (allocate them uniformely on the disc)
  NodeContainer eNBs;
  eNBs.Create (nEnbs);

  Ptr<ListPositionAllocator> allocator = CreateObject<ListPositionAllocator> ();
  allocator->Add (Vector (0.0, 0.0, 0.0));
  mobility.SetPositionAllocator (allocator);
  mobility.Install (eNBs);

  // Make it so that nodes are at a certain height > 0
  for (NodeContainer::Iterator j = eNBs.Begin ();
       j != eNBs.End (); ++j)
    {
      Ptr<MobilityModel> mobility = (*j)->GetObject<MobilityModel> ();
      Vector position = mobility->GetPosition ();
      position.z = 15;
      mobility->SetPosition (position);
    }

  // Create a netdevice for each gateway
  phyHelper.SetDeviceType (CunbPhyHelper::ENB);
  macHelper.SetDeviceType (CunbMacHelper::ENB);
  helper.Install (phyHelper, macHelper, eNBs);

  /************************
  *  Configure eNBs  *
  ************************/

  // Install reception paths on eNBs
  for (NodeContainer::Iterator j = eNBs.Begin (); j != eNBs.End (); j++)
    {

      Ptr<Node> object = *j;
      // Get the device
      Ptr<NetDevice> netDevice = object->GetDevice (0);
      Ptr<CunbNetDevice> cunbNetDevice = netDevice->GetObject<CunbNetDevice> ();
      NS_ASSERT (cunbNetDevice != 0);
      Ptr<EnbCunbPhy> enbPhy = cunbNetDevice->GetPhy ()->GetObject<EnbCunbPhy> ();

      // Set up height of the gateway
      Ptr<MobilityModel> enbMob = (*j)->GetObject<MobilityModel> ();
      Vector position = enbMob->GetPosition ();
      position.z = 15;
      enbMob->SetPosition (position);

      // Global callbacks (every eNB)
      enbPhy->TraceConnectWithoutContext ("ReceivedPacket",
                                         MakeCallback (&PacketReceptionCallback));
      enbPhy->TraceConnectWithoutContext ("LostPacketBecauseInterference",
                                         MakeCallback (&InterferenceCallback));
      enbPhy->TraceConnectWithoutContext ("LostPacketBecauseNoMoreReceivers",
                                         MakeCallback (&NoMoreReceiversCallback));
      enbPhy->TraceConnectWithoutContext ("LostPacketBecauseUnderSensitivity",
                                         MakeCallback (&UnderSensitivityCallback));
    }

  /**********************************************
  *  Set up the end device's spreading factor  *
  **********************************************/

  macHelper.SetSpreadingFactorsUp (endDevices, eNBs, channel);

  NS_LOG_DEBUG ("Completed configuration");

  /*********************************************
  *  Install applications on the end devices  *
  *********************************************/

  Time appStopTime = Seconds (simulationTime);
  MARHelper appHelper = MARHelper ();
  appHelper.SetPeriod (Seconds (appPeriodSeconds));
  ApplicationContainer appContainer = appHelper.Install (endDevices, MARHelper::Normal);

  appContainer.Start (Seconds (0));
  appContainer.Stop (appStopTime);

  /**********************
   * Print output files *
   *********************/
  if (printMSs)
    {
      PrintEndDevices (endDevices, eNBs,
                       "src/cunb/examples/endDevices.dat");
    }
  /*
  std::string mode = "UseBridge";
    std::string tapName = "mytap0";

  TapBridgeHelper tapBridge;
  tapBridge.SetAttribute ("Mode", StringValue(mode));
  tapBridge.SetAttribute ("DeviceName", StringValue (tapName));
  tapBridge.Install (endDevices.Get (0),msDeviceContainer.Get(0));
*/
  /****************
  *  Simulation  *
  ****************/

  Simulator::Stop (appStopTime + Hours (2));

  // PrintSimulationTime ();

  Simulator::Run ();

  Simulator::Destroy ();

  /*************
  *  Results  *
  *************/
  double receivedProb = double(received)/nMSs;
  double interferedProb = double(interfered)/nMSs;
  double noMoreReceiversProb = double(noMoreReceivers)/nMSs;
  double underSensitivityProb = double(underSensitivity)/nMSs;

  double receivedProbGivenAboveSensitivity = double(received)/(nMSs - underSensitivity);
  double interferedProbGivenAboveSensitivity = double(interfered)/(nMSs - underSensitivity);
  double noMoreReceiversProbGivenAboveSensitivity = double(noMoreReceivers)/(nMSs - underSensitivity);
  std::cout << nMSs << " " << double(nMSs)/simulationTime << " " << receivedProb << " " << interferedProb << " " << noMoreReceiversProb << " " << underSensitivityProb <<
  " " << receivedProbGivenAboveSensitivity << " " << interferedProbGivenAboveSensitivity << " " << noMoreReceiversProbGivenAboveSensitivity << std::endl;

  // Print the packetTracker contents
  // std::cout << "Packet outcomes" << std::endl;
  // std::map<Ptr<Packet const>, PacketStatus>::iterator i;
  // for (i = packetTracker.begin (); i != packetTracker.end (); i++)
  //   {
  //     PacketStatus status = (*i).second;
  //     std::cout.width (4);
  //     std::cout << status.senderId << "\t";
  //     for (int j = 0; j < nGateways; j++)
  //       {
  //         switch (status.outcomes.at (j))
  //           {
  //           case RECEIVED:
  //             {
  //               std::cout << "R ";
  //               break;
  //             }
  //           case UNDER_SENSITIVITY:
  //             {
  //               std::cout << "U ";
  //               break;
  //             }
  //           case NO_MORE_RECEIVERS:
  //             {
  //               std::cout << "N ";
  //               break;
  //             }
  //           case INTERFERED:
  //             {
  //               std::cout << "I ";
  //               break;
  //             }
  //           case UNSET:
  //             {
  //               std::cout << "E ";
  //               break;
  //             }
  //           }
  //       }
  //     std::cout << std::endl;
  //   }

  return 0;
}
