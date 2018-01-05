#include "ns3/simple-cunb-server.h"
#include "ns3/simulator.h"
#include "ns3/ms-cunb-mac.h"
#include "ns3/cunb-mac-header.h"
#include "ns3/cunb-mac-trailer-ul.h"
#include "ns3/cunb-frame-header.h"
#include "ns3/cunb-tag.h"
#include "ns3/log.h"
#include "ns3/app-layer-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("SimpleCunbServer");

NS_OBJECT_ENSURE_REGISTERED (SimpleCunbServer);

TypeId
SimpleCunbServer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::SimpleCunbServer")
    .SetParent<Application> ()
    .AddConstructor<SimpleCunbServer> ()
    .SetGroupName ("cunb");
  return tid;
}

SimpleCunbServer::SimpleCunbServer()
{
  NS_LOG_FUNCTION_NOARGS ();
}

SimpleCunbServer::~SimpleCunbServer()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
SimpleCunbServer::StartApplication (void)
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
SimpleCunbServer::StopApplication (void)
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
SimpleCunbServer::AddEnb (Ptr<Node> enb, Ptr<NetDevice> netDevice)
{
  NS_LOG_FUNCTION (this << enb);

  // Get the PointToPointNetDevice
  Ptr<PointToPointNetDevice> p2pNetDevice;
  for (uint32_t i = 0; i < enb->GetNDevices (); i++)
    {
      p2pNetDevice = enb->GetDevice (i)->GetObject<PointToPointNetDevice> ();
      if (p2pNetDevice != 0)
        {
          // We found a p2pNetDevice on eNB
          break;
        }
    }

  // Get the eNB's Cunb MAC layer (assumes eNB's MAC is configured as first device)
  Ptr<EnbCunbMac> enbMac = enb->GetDevice (0)->GetObject<CunbNetDevice> ()->
    GetMac ()->GetObject<EnbCunbMac> ();
  NS_ASSERT (enbMac != 0);

  // Get the Address
  Address enbAddress = p2pNetDevice->GetAddress ();

  // Check whether this device already exists
  if (m_enbStatuses.find (enbAddress) == m_enbStatuses.end ())
    {
      // The device doesn't exist

      // Create new enbStatus
      EnbStatus enbStatus = EnbStatus (enbAddress, netDevice, enbMac);
      // Add it to the map
      m_enbStatuses.insert (std::pair<Address, EnbStatus>
                                  (enbAddress, enbStatus));
      NS_LOG_DEBUG ("Added an enb to the list");
    }
}

void
SimpleCunbServer::AddNodes (NodeContainer nodes)
{
  NS_LOG_FUNCTION_NOARGS ();

  // For each node in the container, call the function to add that single node
  NodeContainer::Iterator it;
  for (it = nodes.Begin (); it != nodes.End (); it++)
    {
      AddNode (*it);
    }
}

void
SimpleCunbServer::AddNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this << node);

  // Get the CunbNetDevice
  Ptr<CunbNetDevice> cunbNetDevice;
  for (uint32_t i = 0; i < node->GetNDevices (); i++)
    {
      cunbNetDevice = node->GetDevice (i)->GetObject<CunbNetDevice> ();
      if (cunbNetDevice != 0)
        {
          // We found a CunbNetDevice on the node
          break;
        }
    }
  // Get the MAC
  Ptr<MSCunbMac> msCunbMac = cunbNetDevice->GetMac ()->GetObject<MSCunbMac> ();

  // Get the Address
  CunbDeviceAddress msAddress = msCunbMac->GetDeviceAddress ();
  // Check whether this device already exists
  if (m_msStatuses.find (msAddress) == m_msStatuses.end ())
    {
      // The device doesn't exist

      // Create new DeviceStatus
      MSStatus msStatus = MSStatus (msCunbMac);
      // Add it to the map
      m_msStatuses.insert (std::pair<CunbDeviceAddress, MSStatus>
                                 (msAddress, msStatus));
      NS_LOG_DEBUG ("Added to the list a device with address " <<
                    msAddress.Print ());
    }
}

bool
SimpleCunbServer::Receive (Ptr<NetDevice> device, Ptr<const Packet> packet,
                              uint16_t protocol, const Address& address)
{
  NS_LOG_FUNCTION (this << packet << protocol << address);

  // Create a copy of the packet
  Ptr<Packet> myPacket = packet->Copy ();

  // Extract the mac trailer
  CunbMacTrailer macTlr;
  macTlr.EnableFcs(true);
  myPacket->RemoveTrailer(macTlr);

  // Verify CRC for the uplink packet
  bool verify= macTlr.CheckFcs(myPacket);
  NS_LOG_INFO ("FCS: "<< macTlr.GetFcs()<<" Verifying CRC Uplink " << verify);

  bool verifyAuth= macTlr.CheckAuth(myPacket);
  NS_LOG_INFO ("Auth: "<< macTlr.GetAuth()<<" Verifying Auth Uplink " << verifyAuth);

  if(verify == 0 || verifyAuth == 0)
	{
	  NS_LOG_INFO("CRC Error or AUth Failure. Drop Packet");
	  return false;
	}

  // Extract the mac header
  CunbMacHeaderUl macHdr;
  myPacket->RemoveHeader (macHdr);

  // Extract the frame header
  CunbFrameHeaderUl frameHdr;
  myPacket->RemoveHeader (frameHdr);

  // Extract the Link Layer header
  CunbLinkLayerHeader llHdr;
  myPacket->RemoveHeader(llHdr);

  // Extract the App Layer header
  AppLayerHeader appHdr;
  myPacket->RemoveHeader(appHdr);

  CunbTag tag;
  myPacket->RemovePacketTag (tag);

  // Register which eNB this packet came from
  double rcvPower = tag.GetReceivePower ();
  m_msStatuses.at (frameHdr.GetAddress ()).UpdateEnbData (address,rcvPower);

  // Determine whether the packet requires a reply
  if ((macHdr.GetMType () == CunbMacHeaderUl::SINGLE_ACK ||
	  macHdr.GetMType () == CunbMacHeaderUl::MULTIPLE_ACK)  &&
      !m_msStatuses.at (frameHdr.GetAddress ()).HasReply ())
    {
      NS_LOG_DEBUG ("Scheduling a reply for this device");


      MSStatus::Reply reply;
      reply.hasReply = true;

      // this is the ACK packet sent to the MS. It can be a Multiple or Single Ack
      Ptr<Packet> replyPacket = Create<Packet> (10);
      reply.packet = replyPacket;

      CunbLinkLayerHeader replyllHdr;
      reply.llHeader = replyllHdr;

      CunbFrameHeader replyFrameHdr = CunbFrameHeader ();
      replyFrameHdr.SetAddress (frameHdr.GetAddress ());
      reply.frameHeader = replyFrameHdr;

      CunbMacHeader replyMacHdr = CunbMacHeader ();

      if(macHdr.GetMType () == CunbMacHeaderUl::SINGLE_ACK)
      {
    	  replyMacHdr.SetMType (CunbMacHeader::SINGLE_ACK);
      }
      else
      {
    	  replyMacHdr.SetMType (CunbMacHeader::MULTIPLE_ACK);
      }
      reply.macHeader = replyMacHdr;

      CunbMacTrailer replyMacTlr = CunbMacTrailer();
      reply.macTrailer = replyMacTlr;

      NS_LOG_DEBUG ("Address of the MS " <<
    		  frameHdr.GetAddress ().Print ());


      m_msStatuses.at (frameHdr.GetAddress ()).SetReply (reply);
      m_msStatuses.at (frameHdr.GetAddress ()).SetFirstReceiveWindowFrequency (tag.GetFrequency ());

      uint16_t pType = appHdr.GetPtype();
      NS_LOG_DEBUG("App Packet Type" << pType);

      // Schedule a reply on the first receive window
      Simulator::Schedule (Seconds (1), &SimpleCunbServer::SendOnFirstWindow,
                           this, frameHdr.GetAddress (),pType );
    }

  return true;
}

void
SimpleCunbServer::SendOnFirstWindow (CunbDeviceAddress address,uint16_t ptype)
{
  NS_LOG_FUNCTION (this << address);
  NS_LOG_DEBUG("App Packet Type" << ptype);

  // Decide on which gateway we'll transmit our reply
  double firstReceiveWindowFrequency = m_msStatuses.at
      (address).GetFirstReceiveWindowFrequency ();

  Address enbForReply = GetEnbForReply (address,
                                                firstReceiveWindowFrequency);

  if (enbForReply != Address ())
    {
      NS_LOG_FUNCTION ("Found a suitable eNB!");

      // Get the packet to use in the reply
      Ptr<Packet> replyPacket = m_msStatuses.at (address).GetReplyPacket ();
      NS_LOG_DEBUG ("Packet size: " << replyPacket->GetSize ());

      // Tag the packet so that the Gateway sends it according to the first
      // receive window parameters
      CunbTag replyPacketTag;
      uint8_t dataRate = m_msStatuses.at (address).GetFirstReceiveWindowDataRate ();
      double frequency = m_msStatuses.at (address).GetFirstReceiveWindowFrequency ();
      replyPacketTag.SetDataRate (dataRate);
      replyPacketTag.SetFrequency (frequency);

      replyPacket->AddPacketTag (replyPacketTag);

      NS_LOG_INFO ("Sending reply through the enb with address " << enbForReply);

      // Inform the eNB of the transmission
      m_enbStatuses.find (enbForReply)->second.GetNetDevice ()->
      Send (replyPacket, enbForReply, 0x0800);
    }
  else
    {
      NS_LOG_FUNCTION ("No suitable eNB found, scheduling second window reply");

      if (ptype == 1 || ptype == 2 || ptype == 4 || ptype ==5  )
      {
    	  // Schedule a reply on the second receive window
            Simulator::Schedule (Seconds (1), &SimpleCunbServer::SendOnSecondWindow, this,
                           address, ptype);
      }
    }
}

void
SimpleCunbServer::SendOnSecondWindow (CunbDeviceAddress address, uint16_t ptype)
{
  NS_LOG_FUNCTION (this << address);
  NS_LOG_DEBUG("App Packet Type" << ptype);

  double secondReceiveWindowFrequency = m_msStatuses.at
      (address).GetSecondReceiveWindowFrequency ();

  // Decide on which eNB we'll transmit our reply
  Address enbForReply = GetEnbForReply (address, secondReceiveWindowFrequency);

  if (enbForReply != Address ())
    {
      // Get the packet to use in the reply
      Ptr<Packet> replyPacket = m_msStatuses.at (address).GetReplyPacket ();
      NS_LOG_DEBUG ("Packet size: " << replyPacket->GetSize ());

      // Tag the packet so that the Enb sends it according to the first
      // receive window parameters
      CunbTag replyPacketTag;
      uint8_t dataRate = m_msStatuses.at (address).GetSecondReceiveWindowDataRate ();
      double frequency = m_msStatuses.at (address).GetSecondReceiveWindowFrequency ();
      replyPacketTag.SetDataRate (dataRate);
      replyPacketTag.SetFrequency (frequency);

      replyPacket->AddPacketTag (replyPacketTag);

      NS_LOG_INFO ("Sending reply through the enb with address " <<
                   enbForReply);

      // Inform the eNB of the transmission
      m_enbStatuses.find (enbForReply)->second.GetNetDevice ()->
      Send (replyPacket, enbForReply, 0x0800);
    }
  else
    {
      // Schedule a reply on the second receive window
      NS_LOG_INFO ("no eNB available for second window,scheduling third window reply");

      if(ptype == 2 || ptype == 5 )
      {// Schedule a reply on the second receive window
            Simulator::Schedule (Seconds (2), &SimpleCunbServer::SendOnThirdWindow, this,
                                 address);
      }
    }
}

void
SimpleCunbServer::SendOnThirdWindow (CunbDeviceAddress address)
{
  NS_LOG_FUNCTION (this << address);

  double thirdReceiveWindowFrequency = m_msStatuses.at
      (address).GetThirdReceiveWindowFrequency ();

  // Decide on which eNB we'll transmit our reply
  Address enbForReply = GetEnbForReply (address, thirdReceiveWindowFrequency);

  if (enbForReply != Address ())
    {
      // Get the packet to use in the reply
      Ptr<Packet> replyPacket = m_msStatuses.at (address).GetReplyPacket ();
      NS_LOG_DEBUG ("Packet size: " << replyPacket->GetSize ());

      // Tag the packet so that the Enb sends it according to the first
      // receive window parameters
      CunbTag replyPacketTag;
      uint8_t dataRate = m_msStatuses.at (address).GetThirdReceiveWindowDataRate ();
      double frequency = m_msStatuses.at (address).GetThirdReceiveWindowFrequency ();
      replyPacketTag.SetDataRate (dataRate);
      replyPacketTag.SetFrequency (frequency);

      replyPacket->AddPacketTag (replyPacketTag);

      NS_LOG_INFO ("Sending reply through the enb with address " <<
                   enbForReply);

      // Inform the eNB of the transmission
      m_enbStatuses.find (enbForReply)->second.GetNetDevice ()->
      Send (replyPacket, enbForReply, 0x0800);
    }
  else
    {
      // Schedule a reply on the second receive window
      NS_LOG_INFO ("Giving up on this reply, no eNB available for third window");
    }
}
Address
SimpleCunbServer::GetEnbForReply (CunbDeviceAddress deviceAddress,
                                         double frequency)
{
  NS_LOG_FUNCTION (this);

  // Check which eNBs can send this reply
  // Go in the order suggested by the msStatus
  std::list<Address> addresses = m_msStatuses.at
      (deviceAddress).GetSortedEnbAddresses ();

  for (auto it = addresses.begin (); it != addresses.end (); ++it)
    {
      if (m_enbStatuses.at (*it).IsAvailableForTransmission (frequency))
        {
          m_enbStatuses.at (*it).SetNextTransmissionTime (Simulator::Now ());
          return *it;
        }
    }

  return Address ();
}
}
