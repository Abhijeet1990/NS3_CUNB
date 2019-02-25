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
#include "ns3/Hello_helper.h"
#include <ns3/buildings-helper.h>
#include <ns3/hybrid-buildings-propagation-loss-model.h>
#include <ns3/buildings-propagation-loss-model.h>
#include <ns3/constant-position-mobility-model.h>
#include <iostream>

#include <algorithm>
#include <ctime>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("HelloIntegrationTrace");

// Variables that stores the status of captured packets
int noMoreReceivers = 0;
int interfered = 0;
int received = 0;
int underSensitivity = 0;
int underSensitivity_at_bs = 0;
int underSensitivity_at_sm = 0;
int transmitted_by_sm = 0;
int transmitted_by_bs = 0;
int succesfully_acked = 0; // GET Response ACKed
int transmitted_data = 0; // GET Response sent
int transmitted_assoc = 0; // AA Response sent by SM
int assoc_req = 0; // AA Request sent by Data Concentrator
int get_req = 0; // GET Request sent by Data Concentrator
int sent_hello = 0;
int resent_hello = 0;

int simulation_endtime = 24000;
int reporting_interval = 240;

int received_by_sm = 0;
int received_by_bs = 0;

int wrong_freq = 0;

int phy_rx_begin = 0;

double height_of_sm = 5.0;
double height_of_enb = 30.0;

uint32_t sm_count = 180;
uint8_t enb_count = 4;

enum PacketOutcome {
  RECEIVED,
  INTERFERED,
  NO_MORE_RECEIVERS,
  UNDER_SENSITIVITY,
  WRONG_FREQUENCY,
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
CheckReceptionByAllEnbsComplete (std::map<Ptr<Packet const>, PacketStatus>::iterator it)
{
  // Check whether this packet is received by all gateways
  if ((*it).second.outcomeNumber == enb_count)
    {
      // Update the statistics
      PacketStatus status = (*it).second;
      for (int j = 0; j < enb_count; j++)
        {
          switch (status.outcomes.at (j))
            {
            case RECEIVED:
              {
                received += 1;
                //std::cout << "Received "<<received<<std::endl;
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
            case WRONG_FREQUENCY:
              {
                wrong_freq += 1;
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
  status.outcomes = std::vector<enum PacketOutcome> (enb_count, UNSET);
  if(systemId < sm_count)
	  transmitted_by_sm++;
  else
      transmitted_by_bs++;

  packetTracker.insert (std::pair<Ptr<Packet const>, PacketStatus> (packet, status));
}

//LostPacketBecauseWrongFrequency

void
PacketReceptionCallback (Ptr<Packet const> packet, uint32_t systemId)
{
  // Remove the successfully received packet from the list of sent ones
  // NS_LOG_INFO ("A packet was successfully received at gateway " << systemId);

  std::map<Ptr<Packet const>, PacketStatus>::iterator it = packetTracker.find (packet);
  //std::cout << "System Id: " << systemId << " packet counts " << packetTracker.count(packet) << std::endl;
  if(systemId < sm_count)
	  {received_by_sm++;
	  //std::cout <<received_by_sm<< " for SM"<<std::endl;
      (*it).second.outcomes.at (systemId) = RECEIVED;}
  else{
      received_by_bs++;
      //std::cout <<received_by_bs<< " for BS"<<std::endl;
      (*it).second.outcomes.at (systemId - sm_count) = RECEIVED;}

  (*it).second.outcomeNumber += 1;

  CheckReceptionByAllEnbsComplete (it);
}

void
InterferenceCallback (Ptr<Packet const> packet, uint32_t systemId)
{
  // NS_LOG_INFO ("A packet was lost because of interference at gateway " << systemId);

  std::map<Ptr<Packet const>, PacketStatus>::iterator it = packetTracker.find (packet);
  if(systemId < sm_count)
  {
	  (*it).second.outcomes.at (systemId) = INTERFERED;
  }
  else{
	  (*it).second.outcomes.at (systemId - sm_count) = INTERFERED;
  }
  (*it).second.outcomeNumber += 1;

  CheckReceptionByAllEnbsComplete (it);
}

void
UnderSensitivityCallback (Ptr<Packet const> packet, uint32_t systemId)
{
  // NS_LOG_INFO ("A packet arrived at the gateway under sensitivity at gateway " << systemId);

  std::map<Ptr<Packet const>, PacketStatus>::iterator it = packetTracker.find (packet);
  if(systemId < sm_count)
  {
	  underSensitivity_at_sm +=1;
     (*it).second.outcomes.at (systemId) = UNDER_SENSITIVITY;
  }
  else{
	  underSensitivity_at_bs +=1;
	  (*it).second.outcomes.at (systemId - sm_count) = UNDER_SENSITIVITY;
  }
  (*it).second.outcomeNumber += 1;

  CheckReceptionByAllEnbsComplete (it);
}

void
NoMoreReceiversCallback (Ptr<Packet const> packet, uint32_t systemId)
{
  // NS_LOG_INFO ("A packet was lost because there were no more receivers at gateway " << systemId);

  std::map<Ptr<Packet const>, PacketStatus>::iterator it = packetTracker.find (packet);
  (*it).second.outcomes.at (systemId - sm_count) = NO_MORE_RECEIVERS;
  (*it).second.outcomeNumber += 1;

  CheckReceptionByAllEnbsComplete (it);
}

void
WrongFrequencyCallback (Ptr<Packet const> packet, uint32_t systemId)
{
  // NS_LOG_INFO ("A packet was lost because of wrong frequency" << systemId);

  std::map<Ptr<Packet const>, PacketStatus>::iterator it = packetTracker.find (packet);
  (*it).second.outcomes.at (systemId) = WRONG_FREQUENCY;
  (*it).second.outcomeNumber += 1;

  CheckReceptionByAllEnbsComplete (it);
}

void
SuccesfullyAckedCallback(Ptr<Packet const> packet)
{
	succesfully_acked+=1;
}

void
TransmittedDataCallback(Ptr<Packet const> packet)
{
	transmitted_data+=1;
}
void
PhyRxBeginCallback(Ptr<Packet const> packet)
{
	phy_rx_begin+=1;
}
void
TransmittedAssociationCallback(Ptr<Packet const> packet)
{
	transmitted_assoc+=1;
}

void
AssociationRequestCallback(Ptr<Packet const> packet)
{
	assoc_req+=1;
}
void
GetRequestCallback(Ptr<Packet const> packet)
{
	get_req+=1;
}

void
SentHelloCallback(Ptr<Packet const> packet)
{
	sent_hello +=1;
}

void
ReSentHelloCallback(Ptr<Packet const> packet)
{
	resent_hello +=1;
}

int main (int argc, char *argv[])
{

  bool verbose = false;

  CommandLine cmd;
  cmd.AddValue ("verbose", "Whether to print output or not", verbose);
  cmd.Parse (argc, argv);



  // Logging
  //////////

  LogComponentEnable ("HelloIntegrationTrace", LOG_LEVEL_ALL);
  //LogComponentEnable ("SimpleCunbServer", LOG_LEVEL_ALL);
  //LogComponentEnable ("EnbCunbMac", LOG_LEVEL_ALL);
  //LogComponentEnable("CunbFrameHeader", LOG_LEVEL_ALL);
  //LogComponentEnable("CunbMacHeader", LOG_LEVEL_ALL);
  //LogComponentEnable("NewTypeAPDU", LOG_LEVEL_ALL);



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
  //LogComponentEnable ("OneTimeRequesting", LOG_LEVEL_ALL);
  //LogComponentEnable("PointToPointNetDevice", LOG_LEVEL_ALL);
  //LogComponentEnable ("CunbForwarder", LOG_LEVEL_ALL);
  //LogComponentEnable ("OneShotSender", LOG_LEVEL_ALL);
  //LogComponentEnable ("MSStatus", LOG_LEVEL_ALL);
  //LogComponentEnable ("EnbStatus", LOG_LEVEL_ALL);
  //LogComponentEnable("CunbInterferenceHelper",LOG_LEVEL_ALL);
  LogComponentEnableAll (LOG_PREFIX_FUNC);
  LogComponentEnableAll (LOG_PREFIX_NODE);
  LogComponentEnableAll (LOG_PREFIX_TIME);

  // Create a simple wireless channel
  ///////////////////////////////////

  /*
  Ptr<LogDistancePropagationLossModel> loss = CreateObject<LogDistancePropagationLossModel> ();
  loss->SetPathLossExponent (3.5);
  loss->SetReference (1, 0);
  */


  Ptr<HybridBuildingsPropagationLossModel> loss = CreateObject<HybridBuildingsPropagationLossModel> ();
  // cancel shadowing effect
  loss->SetAttribute ("ShadowSigmaOutdoor", DoubleValue (0.0));
  loss->SetAttribute ("ShadowSigmaIndoor", DoubleValue (0.0));
  loss->SetAttribute ("ShadowSigmaExtWalls", DoubleValue (0.0));


  //Ptr<BuildingsPropagationLossModel> loss = CreateObject<BuildingsPropagationLossModel>();
  Ptr<PropagationDelayModel> delay = CreateObject<ConstantSpeedPropagationDelayModel> ();

  Ptr<CunbChannel> channel = CreateObject<CunbChannel> (delay,loss);


  // Helpers
  //////////

  // End Device mobility
  Ptr<UniformRandomVariable> x_coord = CreateObject<UniformRandomVariable> ();
  Ptr<UniformRandomVariable> y_coord = CreateObject<UniformRandomVariable> ();

  MobilityHelper mobilityMs, mobilityEnb;

  // Install building for each smart meter
  for(uint32_t i = 0; i < sm_count ; i++){

	   Ptr<ConstantPositionMobilityModel> smMobility = CreateObject<ConstantPositionMobilityModel> ();
	   smMobility->SetPosition (Vector (x_coord->GetValue(0.0,3750.0), y_coord->GetValue(0.0,2000.0), height_of_sm));


       Ptr<MobilityBuildingInfo> buildingInfoUe = CreateObject<MobilityBuildingInfo> ();

       // Get the mobility model for that node
       smMobility->AggregateObject (buildingInfoUe); // operation usually done by BuildingsHelper::Install
       BuildingsHelper::MakeConsistent (smMobility);
       mobilityMs.PushReferenceMobilityModel(smMobility);

  }

  // eNB mobility
  for(uint32_t i = 0; i < enb_count ; i++){

  	   Ptr<ConstantPositionMobilityModel> enbMobility = CreateObject<ConstantPositionMobilityModel> ();
  	   if (enb_count == 1) enbMobility->SetPosition (Vector (1875.0, 1000.0, height_of_enb));
  	   if(enb_count == 2){
  		   if(i == 0) enbMobility->SetPosition (Vector (1250.0, 1000.0, height_of_enb));
  		   if(i == 1) enbMobility->SetPosition (Vector (2500.0, 1000.0, height_of_enb));
  	   }
  	   if(enb_count == 3){
  		 if(i == 0) enbMobility->SetPosition (Vector (937.0, 1000.0, height_of_enb));
  		 if(i == 1) enbMobility->SetPosition (Vector (1874.0, 1000.0, height_of_enb));
  		 if(i == 3) enbMobility->SetPosition (Vector (2811.0, 1000.0, height_of_enb));
  	   }
  	   if(enb_count == 4){
  	   		 if(i == 0) enbMobility->SetPosition (Vector (750.0, 1000.0, height_of_enb));
  	   		 if(i == 1) enbMobility->SetPosition (Vector (1874.0, 1500.0, height_of_enb));
  	   		 if(i == 3) enbMobility->SetPosition (Vector (1874.0, 500.0, height_of_enb));
  	   	     if(i == 4) enbMobility->SetPosition (Vector (3000.0, 1000.0, height_of_enb));
  	   	   }

  	   Ptr<MobilityBuildingInfo> buildingInfoEnb = CreateObject<MobilityBuildingInfo> ();

       // Get the mobility model for that node
       enbMobility->AggregateObject (buildingInfoEnb); // operation usually done by BuildingsHelper::Install
       BuildingsHelper::MakeConsistent (enbMobility);
       mobilityEnb.PushReferenceMobilityModel(enbMobility);

    }

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
  endDevices.Create (sm_count);
  mobilityMs.Install (endDevices);
  BuildingsHelper::Install(endDevices);

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


  // Set the ID for each Smart meter
  for(uint16_t i = 0; i<sm_count ;i++)
    {
  	  Ptr<MSCunbMac> mac = endDevices.Get(i)->GetDevice(0)->GetObject<CunbNetDevice>()->GetMac()->GetObject<MSCunbMac>();
  	  mac->SetIdent(i+1);
    }

  // Install tracers in Smart meters
  for (NodeContainer::Iterator j = endDevices.Begin ();
         j != endDevices.End (); ++j)
      {
        Ptr<Node> node = *j;
        Ptr<CunbNetDevice> cunbNetDevice = node->GetDevice (0)->GetObject<CunbNetDevice> ();
        //Ptr<CunbPhy> msPhy = cunbNetDevice->GetPhy ();
        Ptr<MSCunbPhy> msPhy = cunbNetDevice->GetPhy ()->GetObject<MSCunbPhy> ();

        msPhy->TraceConnectWithoutContext ("StartSending",
                                         MakeCallback (&TransmissionCallback));
        msPhy->TraceConnectWithoutContext ("PhyRxBegin",MakeCallback (&PhyRxBeginCallback));
        //msPhy->TraceConnectWithoutContext ("LostPacketBecauseUnderSensitivity",MakeCallback (&UnderSensitivityCallback));
        //msPhy->TraceConnectWithoutContext ("LostPacketBecauseInterference",MakeCallback (&InterferenceCallback));
        //msPhy->TraceConnectWithoutContext ("LostPacketBecauseWrongFrequency",MakeCallback (&WrongFrequencyCallback));

        Ptr<MSCunbMac> msMac = cunbNetDevice->GetMac()->GetObject<MSCunbMac> ();
        msMac->TraceConnectWithoutContext ("ReceivedPacket",MakeCallback (&SuccesfullyAckedCallback));
        msMac->TraceConnectWithoutContext("StartSendingData",MakeCallback(&TransmittedDataCallback));
        msMac->TraceConnectWithoutContext("SentAAResponse",MakeCallback(&TransmittedAssociationCallback));
        msMac->TraceConnectWithoutContext("HelloSent",MakeCallback(&SentHelloCallback));
        msMac->TraceConnectWithoutContext("HelloResent",MakeCallback(&ReSentHelloCallback));

      }


  // Install the buildings for each smart meters


  ////////////////
    // Create eNBs //
    ////////////////

  NodeContainer enbs;
  enbs.Create (enb_count);
  mobilityEnb.Install (enbs);
  BuildingsHelper::Install(enbs);

  // Create the CunbNetDevices of the enbs
  phyHelper.SetDeviceType (CunbPhyHelper::ENB);
  macHelper.SetDeviceType (CunbMacHelper::ENB);
  helper.Install (phyHelper, macHelper, enbs);

  // Install Tracers at enbs
    for (NodeContainer::Iterator j = enbs.Begin (); j != enbs.End (); j++)
      {

        Ptr<Node> object = *j;
        // Get the device
        Ptr<NetDevice> netDevice = object->GetDevice (0);
        Ptr<CunbNetDevice> cunbNetDevice = netDevice->GetObject<CunbNetDevice> ();
        NS_ASSERT (cunbNetDevice != 0);
        Ptr<EnbCunbPhy> enbPhy = cunbNetDevice->GetPhy ()->GetObject<EnbCunbPhy> ();

        // Global callbacks (every gateway)
        enbPhy->TraceConnectWithoutContext ("StartSending",MakeCallback (&TransmissionCallback));
        // These are all for the received signal at the Physical Layer
        //enbPhy->TraceConnectWithoutContext ("ReceivedPacket",MakeCallback (&PacketReceptionCallback));
        enbPhy->TraceConnectWithoutContext ("LostPacketBecauseInterference",
                                           MakeCallback (&InterferenceCallback));
        enbPhy->TraceConnectWithoutContext ("LostPacketBecauseNoMoreReceivers",MakeCallback (&NoMoreReceiversCallback));
        enbPhy->TraceConnectWithoutContext ("LostPacketBecauseUnderSensitivity",MakeCallback (&UnderSensitivityCallback));
        enbPhy->TraceConnectWithoutContext ("PhyRxBegin",MakeCallback (&PhyRxBeginCallback));

        //enbPhy->TraceConnectWithoutContext ("PhyRxBegin",MakeCallback (&PhyRxBeginCallback));

        Ptr<EnbCunbMac> enbMac = cunbNetDevice->GetMac ()->GetObject<EnbCunbMac> ();
        enbMac->TraceConnectWithoutContext ("SentAARequest",MakeCallback (&AssociationRequestCallback));
        enbMac->TraceConnectWithoutContext ("SentGETRequest",MakeCallback (&GetRequestCallback));
      }

  // Set spreading factors up
  //macHelper.SetSpreadingFactorsUp (endDevices, enbs, channel);

  // Random Number generator for Offset
  double min = 0.0;
  double max = 180.0;
  Ptr<UniformRandomVariable> x = CreateObject<UniformRandomVariable> ();
  x->SetAttribute ("Min", DoubleValue (min));
  x->SetAttribute ("Max", DoubleValue (max));

  // Using Poisson Process for Hello Arrivals
  Ptr<ExponentialRandomVariable> poisson = CreateObject<ExponentialRandomVariable> ();
  double lambda = 180.0;
  poisson->SetAttribute("Mean", DoubleValue(lambda));


  //ApplicationContainer appContainerReporting;
  HelloHelper helloHelper[sm_count];
  ApplicationContainer helloContainer;
  for(uint32_t i = 0; i<sm_count ;i++)
    {

	  //double sendtime = x->GetValue ();
	  double sendtime = poisson->GetValue ();
  	  helloHelper[i].SetSendTime(Seconds(sendtime));

	  //helloHelper[i].SetSendTime(Seconds(10*i));
  	  Ptr<MSCunbMac> mac = endDevices.Get(i)->GetDevice(0)->GetObject<CunbNetDevice>()->GetMac()->GetObject<MSCunbMac>();
  	  helloHelper[i].SetMac(mac);
  	  helloContainer = helloHelper[i].Install(endDevices.Get(i));
    }

  helloContainer.Start (Seconds (0));
  helloContainer.Stop (Seconds(simulation_endtime));

  OTRHelper oneTimeHelper[sm_count];
  ApplicationContainer appContainer;
  for(uint32_t i = 0; i<sm_count ;i++)
  {
	  oneTimeHelper[i].SetSendTime(Seconds(1000000));
	  Ptr<MSCunbMac> mac = endDevices.Get(i)->GetDevice(0)->GetObject<CunbNetDevice>()->GetMac()->GetObject<MSCunbMac>();
	  oneTimeHelper[i].SetMac(mac);
	  appContainer = oneTimeHelper[i].Install(endDevices.Get(i));
  }

  // Install applications in MSs
  MARHelper appHelper[sm_count];
  ApplicationContainer appContainerMAR;
   for(uint32_t i = 0; i<sm_count ;i++)
     {
   	  appHelper[i].SetPeriod(Seconds(reporting_interval));
   	  appHelper[i].SetInitialDelay(Seconds(10000000));
   	  Ptr<MSCunbMac> mac = endDevices.Get(i)->GetDevice(0)->GetObject<CunbNetDevice>()->GetMac()->GetObject<MSCunbMac>();
   	  appHelper[i].SetMac(mac);
   	  appContainerMAR = appHelper[i].Install(endDevices.Get(i),MARHelper::Normal);
     }

  appContainerMAR.Start (Seconds (0));
  appContainerMAR.Stop (Seconds(simulation_endtime));


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

 // helper.EnablePcapAll("cunb",false);

  // Start simulation
  Simulator::Stop (Seconds (simulation_endtime+1000));
  Simulator::Run ();
  Simulator::Destroy ();
  std::cout << "Transmitted by MS "<< transmitted_by_sm << " and by BS "<< transmitted_by_bs<< std::endl ;
  std::cout << "Received by MS "<< received_by_sm << " and by BS "<< received_by_bs<< std::endl ;
  std::cout << "Undersensitivity loss at MS "<< underSensitivity_at_sm << " and at BS "<< underSensitivity_at_bs << std::endl;
  std::cout << "Wrong Frequency "<<wrong_freq <<std::endl;
  std::cout << "Received " << received << ", Interfered " << interfered << ", loss due to sensitivity " << underSensitivity << ", loss due to no receiver " << noMoreReceivers << std::endl;
  std::cout << "Succesfully Acked "<< succesfully_acked<< " transmitted data " <<transmitted_data<< " AA Response Count "<<transmitted_assoc<< " AA Request Count "<< assoc_req<<std::endl;
  std::cout << "Hello Sent Count " << sent_hello << "Hello Retransmitted Count"<< resent_hello<<" GET Request from Data Concentrator " << get_req<<std::endl;

  return 0;
}
