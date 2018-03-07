#include "ns3/simple-cunb-server.h"
#include "ns3/simulator.h"
#include "ns3/ms-cunb-mac.h"
#include "ns3/cunb-mac-header.h"
#include "ns3/cunb-mac-trailer-ul.h"
#include "ns3/cunb-frame-header.h"
#include "ns3/cunb-tag.h"
#include "ns3/log.h"
#include "ns3/app-layer-header.h"
#include "ns3/new-cosem-header.h"
#include "ns3/OTRe_Helper.h"
#include "ns3/one-time-requesting.h"

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

int SimpleCunbServer::helloCount = 0;
int SimpleCunbServer::AAreqCount = 0;
int SimpleCunbServer::GETreqCount = 0;

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
SimpleCunbServer::SetMss(NodeContainer mss)
{
	m_mss = mss;
}

void
SimpleCunbServer::SetEnbs(NodeContainer enbs)
{
	m_enbs = enbs;
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
      m_enbNode_address_pairs.push_back(std::make_pair(enb,enbAddress));

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
  //NS_LOG_FUNCTION (this << packet << protocol << address);
  NS_LOG_FUNCTION(" Packet Size "<< packet->GetSize());

  // Create a copy of the packet
  Ptr<Packet> myPacket = packet->Copy ();

  // Extract the mac trailer
  CunbMacTrailer macTlr;
  macTlr.EnableFcs(true);
  myPacket->RemoveTrailer(macTlr);

  CunbMacHeaderUl macHdr;
  myPacket->PeekHeader(macHdr);
  NS_LOG_INFO("ident " << macHdr.GetIdent() << " seqNo " << (int)macHdr.GetSeqCnt());
  macTlr.SetMacHeader(macHdr);

  // Verify CRC for the uplink packet
  bool verify= macTlr.CheckFcs(myPacket);
  //NS_LOG_INFO ("FCS: "<< macTlr.GetFcs()<<" Verifying CRC Uplink " << verify);

  bool verifyAuth= macTlr.CheckAuth(myPacket);
  NS_LOG_INFO ("Auth: "<< macTlr.GetAuth()<<" Verifying Auth Uplink " << verifyAuth);

  if(verify == 0 || verifyAuth == 0)
	{
	  NS_LOG_INFO("CRC Error or AUth Failure. Drop Packet");
	  return false;
	}

  // Extract the mac header

  myPacket->RemoveHeader (macHdr);
  uint8_t seqNo = macHdr.GetSeqCnt();
  uint16_t ident = macHdr.GetIdent();
  //uint8_t repCnt = macHdr.GetRepCnts();

  //NS_LOG_INFO("ident " << ident << "seqNo" << seqNo);

  // store the identifier and sequence Number
  std::pair<uint16_t,uint8_t> seq_id_pair = std::make_pair(ident,seqNo);
  //std::pair<std::pair<uint16_t,uint8_t>,uint8_t> seq_id_rep_pair = std::make_pair(std::make_pair(ident,seqNo),repCnt);

  // Extract the frame header
  CunbFrameHeaderUl frameHdr;
  myPacket->RemoveHeader (frameHdr);

  // Extract the Link Layer header
  CunbLinkLayerHeader llHdr;
  myPacket->RemoveHeader(llHdr);

  // Extract Transport Layer Header
  NewCosemWrapperHeader wrapperHdr;
  myPacket->RemoveHeader(wrapperHdr);

  // Retreive the wPort of the Remote Server
  //NS_LOG_INFO("Source WPort: "<<wrapperHdr.GetSrcwPort ());

  CunbTag tag;
  myPacket->RemovePacketTag (tag);

  // Register which eNB this packet came from
  double rcvPower = tag.GetReceivePower ();
  //NS_LOG_INFO("Received Power " << rcvPower);
  m_msStatuses.at (frameHdr.GetAddress ()).UpdateEnbData (address,rcvPower);

  // Extract the App Layer header
  AppLayerHeader appHdr;
  myPacket->RemoveHeader(appHdr);

  if(macHdr.GetMType() == CunbMacHeaderUl::HELLO) // If the packet is a Hello Packet
  {
     if(!PairExist(seq_id_pair))
	 //if(!PairSeqIdentRepExist(seq_id_rep_pair))
	 {
		 helloCount++;
		 NS_LOG_INFO("hello count " << helloCount << " Address "<<frameHdr.GetAddress());

		 m_id_seq_pair.push_back(seq_id_pair);

         /*
		 Simulator::Schedule (Seconds (1), &SimpleCunbServer::TriggerOneTimeRequesting,
		 	                             this, GetNodeFromIdent(ident),frameHdr.GetAddress(),tag.GetFrequency());
		 */
		 Simulator::Schedule (Seconds (1), &SimpleCunbServer::SendRequest,
		 		 	                             this, frameHdr.GetAddress(),tag.GetFrequency(),1);
	 }

     return true;

  }

  NewTypeAPDU typeHdr2;
  myPacket->RemoveHeader (typeHdr2);

  // If the APDU type is a AA Response call the function Receive Response to trigger GET Request
  if (typeHdr2.GetApduType() == AARE)
  {
	if(!PairExist(seq_id_pair))
	{
		m_id_seq_pair.push_back(seq_id_pair);
		//m_id_seq_rep_pair.push_back(seq_id_rep_pair);
		Simulator::Schedule (Seconds (1), &SimpleCunbServer::SendRequest,
				 		 	                             this, frameHdr.GetAddress(),tag.GetFrequency(),0);
	}
  }

  // Extract the Cosem Response Header
  NewCosemGetResponseNormalHeader hdr;
  myPacket->RemoveHeader (hdr);
  m_reqData = hdr.GetData ();
  m_sizeReqData = hdr.GetSerializedSize () - 4 ; // without CosemApp header (4B)
 // NS_LOG_INFO("Data received " << m_reqData);

  // Determine whether the packet requires a reply
  if ((macHdr.GetMType () == CunbMacHeaderUl::SINGLE_ACK ||
	  macHdr.GetMType () == CunbMacHeaderUl::MULTIPLE_ACK)  &&
      !PairExist(seq_id_pair))
    {
      //NS_LOG_DEBUG ("Scheduling a reply for this device");
	  m_id_seq_pair.push_back(seq_id_pair);

      // Remove Old Pair because the Sequence Number count repeats cyclically
      RemoveOldPair(seq_id_pair);

      MSStatus::Reply reply;
      reply.hasReply = true;

      // this is the ACK packet sent to the MS. It can be a Multiple or Single Ack
      Ptr<Packet> replyPacket = Create<Packet> ();
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

      //NS_LOG_INFO("Set Ack Bit "<< seqNo);
      replyMacHdr.SetAckBits(seqNo);

      reply.macHeader = replyMacHdr;

      CunbMacTrailer replyMacTlr = CunbMacTrailer();
      reply.macTrailer = replyMacTlr;

      m_msStatuses.at (frameHdr.GetAddress ()).SetFirstReceiveWindowFrequency (tag.GetFrequency ());
      m_msStatuses.at (frameHdr.GetAddress ()).SetReply (reply);


      uint16_t pType = appHdr.GetPtype();
      //NS_LOG_DEBUG("App Packet Type" << pType);

      // Schedule a reply on the first receive window
      Simulator::Schedule (Seconds (1), &SimpleCunbServer::SendOnFirstWindow,
                           this, frameHdr.GetAddress (), pType, seqNo, ident );
    }

  return true;
}

void
SimpleCunbServer::SendOnFirstWindow (CunbDeviceAddress address,uint16_t ptype, uint8_t seqNo, uint16_t ident)
{
  //NS_LOG_FUNCTION (this << address);
  //NS_LOG_DEBUG("App Packet Type" << ptype);

  // Decide on which gateway we'll transmit our reply
  double firstReceiveWindowFrequency = m_msStatuses.at
      (address).GetFirstReceiveWindowFrequency ();

  //NS_LOG_INFO ("firstReceiveWindowFrequency " << firstReceiveWindowFrequency);
  Address enbForReply = GetEnbForReply (address,
                                                firstReceiveWindowFrequency);

  //NS_LOG_INFO ("GetEnbzforReply address " << enbForReply);

  if (enbForReply != Address ())
    {
      //NS_LOG_FUNCTION ("Found a suitable eNB!");

      // Get the packet to use in the reply
      Ptr<Packet> replyPacket = m_msStatuses.at (address).GetReplyPacket (seqNo,ident);
      //NS_LOG_DEBUG ("Packet size: " << replyPacket->GetSize ());

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
      NS_LOG_INFO("ACK size "<<replyPacket->GetSize());
    }
  else
    {
      //NS_LOG_FUNCTION ("No suitable eNB found, scheduling second window reply");

      if (ptype == 1 || ptype == 2 || ptype == 4 || ptype ==5  )
      {
    	  // Schedule a reply on the second receive window
            Simulator::Schedule (Seconds (1), &SimpleCunbServer::SendOnSecondWindow, this,
                           address, ptype,seqNo,ident);
      }
    }
}

void
SimpleCunbServer::SendOnSecondWindow (CunbDeviceAddress address, uint16_t ptype,uint8_t seqNo, uint16_t ident)
{
  //NS_LOG_FUNCTION (this << address);
  //NS_LOG_DEBUG("App Packet Type" << ptype);

  double secondReceiveWindowFrequency = m_msStatuses.at
      (address).GetSecondReceiveWindowFrequency ();

  // Decide on which eNB we'll transmit our reply
  Address enbForReply = GetEnbForReply (address, secondReceiveWindowFrequency);

  if (enbForReply != Address ())
    {
      // Get the packet to use in the reply
      Ptr<Packet> replyPacket = m_msStatuses.at (address).GetReplyPacket (seqNo,ident);
      //NS_LOG_DEBUG ("Packet size: " << replyPacket->GetSize ());

      // Tag the packet so that the Enb sends it according to the first
      // receive window parameters
      CunbTag replyPacketTag;
      uint8_t dataRate = m_msStatuses.at (address).GetSecondReceiveWindowDataRate ();
      double frequency = m_msStatuses.at (address).GetSecondReceiveWindowFrequency ();
      replyPacketTag.SetDataRate (dataRate);
      replyPacketTag.SetFrequency (frequency);

      replyPacket->AddPacketTag (replyPacketTag);

      //NS_LOG_INFO ("Sending reply through the enb with address " <<enbForReply);

      // Inform the eNB of the transmission
      m_enbStatuses.find (enbForReply)->second.GetNetDevice ()->
      Send (replyPacket, enbForReply, 0x0800);
    }
  else
    {
      // Schedule a reply on the second receive window
      //NS_LOG_INFO ("no eNB available for second window,scheduling third window reply");

      if(ptype == 2 || ptype == 5 )
      {// Schedule a reply on the second receive window
            Simulator::Schedule (Seconds (2), &SimpleCunbServer::SendOnThirdWindow, this,
                                 address,seqNo,ident);
      }
    }
}

void
SimpleCunbServer::SendOnThirdWindow (CunbDeviceAddress address,uint8_t seqNo,uint16_t ident)
{
  //NS_LOG_FUNCTION (this << address);

  double thirdReceiveWindowFrequency = m_msStatuses.at
      (address).GetThirdReceiveWindowFrequency ();

  // Decide on which eNB we'll transmit our reply
  Address enbForReply = GetEnbForReply (address, thirdReceiveWindowFrequency);

  if (enbForReply != Address ())
    {
      // Get the packet to use in the reply
      Ptr<Packet> replyPacket = m_msStatuses.at (address).GetReplyPacket (seqNo,ident);
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
  //NS_LOG_FUNCTION (this);

  // Check which eNBs can send this reply
  // Go in the order suggested by the msStatus
  std::list<Address> addresses = m_msStatuses.at
      (deviceAddress).GetSortedEnbAddresses ();

  for (auto it = addresses.begin (); it != addresses.end (); ++it)
    {
	  //NS_LOG_INFO("Addresses : "<<(*it));
      if (m_enbStatuses.at (*it).IsAvailableForTransmission (frequency))
        {
          m_enbStatuses.at (*it).SetNextTransmissionTime (Simulator::Now ());
          return *it;
        }
    }

  return Address ();
}

bool
SimpleCunbServer::PairExist(std::pair<uint16_t,uint8_t> id_seq_pair)
{
	for (std::list<std::pair<uint16_t,uint8_t>>::iterator it = m_id_seq_pair.end (); it != m_id_seq_pair.begin (); --it)
	    {
		  //std::cout<< " ident "<< (int)(*it).first << " seq no in List "<< (int)(*it).second << std::endl;
		     if((*it).first == id_seq_pair.first && (*it).second == id_seq_pair.second)
		     {
		    	 return true;
		     }
	    }
	return false;

}

bool
SimpleCunbServer::PairSeqIdentRepExist(std::pair<std::pair<uint16_t,uint8_t>,uint8_t> id_seq_rep_pair)
{
	for (std::list<std::pair<std::pair<uint16_t,uint8_t>,uint8_t>>::iterator it = m_id_seq_rep_pair.begin (); it != m_id_seq_rep_pair.end (); ++it)
	    {
		     if((*it).first.first == id_seq_rep_pair.first.first && (*it).first.second == id_seq_rep_pair.first.second && (*it).second == id_seq_rep_pair.second)
		     {
		    	 return true;
		     }
	    }
	return false;

}

void
SimpleCunbServer::RemoveOldPair(std::pair<uint16_t,uint8_t> seq_id_pair)
{
	// Remove old ID Seq count pair
	//NS_LOG_INFO("Seq no " <<(int)seq_id_pair.second << " ident of current "<<(int)seq_id_pair.first );
	      for (std::list<std::pair<uint16_t,uint8_t>>::iterator it = m_id_seq_pair.end (); it != m_id_seq_pair.begin (); --it)
	      	    {
	    	         //std::cout << " seq no of iterator "<< (int)(*it).second << " ident no "<< (int)(*it).first<< std::endl;
	      		     if((*it).first == seq_id_pair.first)
	      		     {
	      		    	if(seq_id_pair.second == 0 && (*it).second == 255)
	      		    	//if(seq_id_pair.second == 0 && (*it).second == 15) // Remove the highest number sequence number still there in the list
	      		        {
	      		    		    std::pair<uint16_t,uint8_t> remove_pair = std::make_pair(seq_id_pair.first,(*it).second);
	      		    		    m_id_seq_pair.remove(remove_pair);
	      		    	}
	      		    	if(seq_id_pair.second == 255 && (*it).second < seq_id_pair.second)
	      		    	 //if(seq_id_pair.second == 15 && (*it).second < seq_id_pair.second) // Remove all the entries except the current
	      		    	 {
	      		    		 std::pair<uint16_t,uint8_t> remove_pair = std::make_pair(seq_id_pair.first,(*it).second);
	      		    		 m_id_seq_pair.remove(remove_pair);
	      		    		 //std::cout << " removing "<< (int) (*it).second <<std::endl;
	      		    	 }

	      		     }
	      	    }

}

void
SimpleCunbServer::RemoveOldPairModified(std::pair<std::pair<uint16_t,uint8_t>,uint8_t> seq_id_rep_pair)
{
	// Remove old ID Seq count pair
	      for (std::list<std::pair<std::pair<uint16_t,uint8_t>,uint8_t>>::iterator it = m_id_seq_rep_pair.end (); it != m_id_seq_rep_pair.begin (); --it)
	      	    {
	      		     if((*it).first.first == seq_id_rep_pair.first.first)
	      		     {
	      		    	 if(seq_id_rep_pair.first.second == 15 && (*it).first.second <= seq_id_rep_pair.first.second) // Remove past 4 entries
	      		    	 {
	      		    		 std::pair<std::pair<uint16_t,uint8_t>,uint8_t> remove_pair = std::make_pair(seq_id_rep_pair.first,(*it).second);
	      		    		 m_id_seq_rep_pair.remove(remove_pair);
	      		    	 }
	      		     }
	      	    }
}

Ptr<Node>
SimpleCunbServer::GetNodeFromIdent(uint16_t ident)
{
	for(uint8_t i =0;i < m_mss.GetN();i++)
	{
		Ptr<MSCunbMac> mac = m_mss.Get(i)->GetDevice(0)->GetObject<CunbNetDevice>()->GetMac()->GetObject<MSCunbMac>();
		if(mac->GetIdent() == ident)
		{
			return m_mss.Get(i);
		}
	}

	return Ptr<Node>();
}

Ptr<Node>
SimpleCunbServer::GetEnbNodeFromAddress(Address address)
{
	for (auto it = m_enbNode_address_pairs.begin (); it != m_enbNode_address_pairs.end (); ++it)
		    {
			     if((*it).second == address)
			     {
			    	 return (*it).first;
			     }
		    }
		return Ptr<Node>();
}

void
SimpleCunbServer::TriggerOneTimeRequesting(Ptr<Node> ms, CunbDeviceAddress address,double frequency)
{
	// initalialize an one time requesting
    Ptr<OneTimeRequesting> otrApp = Create<OneTimeRequesting>();

    NS_LOG_INFO("MS Address "<< address << " frequency "<<frequency);
   // Decide on which eNB we'll transmit our reply
    Address enbForReply = GetEnbForReply (address, frequency);
    NS_LOG_INFO("Address to reply "<< enbForReply);

    if(enbForReply==Address()) return;

	Ptr<Node> enb = GetEnbNodeFromAddress(enbForReply);
    Ptr<EnbCunbMac> enbMac = enb->GetDevice(0)->GetObject<CunbNetDevice>()->GetMac()->GetObject<EnbCunbMac>();

    otrApp->SetMS(ms);
    otrApp->SetMac(enbMac);
    otrApp->SetNode (enb);

    // The problem with the current method is the Application gets destroyed before using
    enb->AddApplication (otrApp);
}

//This function is used to send the AA and GET request
void
SimpleCunbServer::SendRequest(CunbDeviceAddress msAddress,double frequency, uint8_t requestType)
{
	// Create a DLMS-COSEM AA Request Packet
	Ptr<Packet> packet = Create<Packet> ();

   if(requestType == 1) // for AA Request
   {
	// Build an xDLMS-Initiate.req PDU and an AARQ APDU
    NewCosemAarqHeader aahdr;
    aahdr.SetApplicationContextName (0);  // {N/A}Application Context Name (rules that govern th exchange-parameters)
    aahdr.SetDedicatedKey (0);  // Dedicated Key, {0,N/A}
	aahdr.SetResponseAllowed (true);  // Reponse Allowed (AA is confirmed), {TRUE}
	aahdr.SetProposedQualityOfService (0); // Not used, {O, N/A}
    aahdr.SetProposedDlmsVersionNumber (6);  // Version number, {6}
	aahdr.SetProposedConformance (0x001010);   // {0x001010}, Based on the example in Annex C IEC 62056-53
	aahdr.SetClientMaxReceivePduSize (0x4B0);  // Client_Max_Receive_PDU_Size,{0x4B0}:1200 bytes
	packet->AddHeader (aahdr); // Copy the header into the packet

    NewTypeAPDU typeaaHdr;
	typeaaHdr.SetApduType ((ApduType)aahdr.GetIdApdu()); // Define the type of APDU
	packet->AddHeader (typeaaHdr); // Copy the header into the packet
	AAreqCount++;
    NS_LOG_INFO("AA req count "<<AAreqCount << " Address "<< msAddress);

   }
   else if(requestType == 0) // for GET Request
   {
 	 // Build an GET-Request (Normal) APDU
 	 NewCosemGetRequestNormalHeader hdr;
     hdr.SetInvokeIdAndPriority (0x02);  // 0000 0010 (invoke_id {0b0000}),service_class= 1 (confirmed) priority level ({normal}))
     hdr.SetClassId (0X03);  // Class Register
     hdr.SetInstanceId (0x010100070000);  // OBIS CODE: 1.1.0.7.0.0
     hdr.SetAttributeId (0x02);  // Second Attribut = Value
     packet->AddHeader (hdr); // Copy the header into the packet

     // The Application layer header to determine the keepalive, normal and alarm packets
     NewTypeAPDU typeHdr;
     typeHdr.SetApduType ((ApduType)hdr.GetIdApdu()); // Define the type of APDU
     packet->AddHeader (typeHdr); // Copy the header into the packet
     GETreqCount++;
     NS_LOG_INFO("GET req count "<< GETreqCount<< " Address "<< msAddress);


   }

	AppLayerHeader appHdr;
	// This packet type is for KeepAlive/Normal/ALarm etc.
	appHdr.SetPtype(1);
	packet->AddHeader(appHdr);

	// Add the UDP Wrapper Header
	// Add Wrapper header
	NewCosemWrapperHeader wrapperHdr;
	uint16_t sourceWPort = 90;
	uint16_t destWPort = 80;
	wrapperHdr.SetSrcwPort (sourceWPort);
	wrapperHdr.SetDstwPort (destWPort);
	wrapperHdr.SetLength (packet->GetSize ());
	packet->AddHeader (wrapperHdr);

	CunbLinkLayerHeader llHdr;
	packet->AddHeader(llHdr);

	CunbFrameHeader frameHdr = CunbFrameHeader ();

	frameHdr.SetAddress(msAddress);
	packet->AddHeader(frameHdr);

	CunbMacHeader macHdr = CunbMacHeader ();
	macHdr.SetMType (CunbMacHeader::SINGLE_ACK);
	packet->AddHeader(macHdr);

	CunbMacTrailer macTlr = CunbMacTrailer();
	macTlr.EnableFcs(true);
	macTlr.SetFcs(packet);
	macTlr.SetAuth(packet);

	CunbTag replyPacketTag;
	replyPacketTag.SetFrequency(frequency);
	packet->AddPacketTag(replyPacketTag);


	Address enbForReply = GetEnbForReply (msAddress, frequency);
	NS_LOG_INFO("Address to reply "<< enbForReply << " and freq to use " << frequency);

	if(enbForReply==Address()) return;

	Ptr<Node> enb = GetEnbNodeFromAddress(enbForReply);
	enb->GetDevice(0)->GetObject<CunbNetDevice>()->Send(packet, enbForReply, 0x0800);


}


}
