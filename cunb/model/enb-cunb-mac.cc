#include "ns3/enb-cunb-mac.h"
#include "ns3/cunb-mac-header.h"
#include "ns3/cunb-mac-header-ul.h"
#include "ns3/cunb-net-device.h"
#include "ns3/cunb-frame-header.h"
#include "ns3/cunb-frame-header-ul.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/ms-cunb-mac.h"
#include "ns3/cunb-mac-trailer.h"
#include "ns3/new-cosem-header.h"
#include "ns3/cunb-linklayer-header.h"
#include "ns3/app-layer-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("EnbCunbMac");

NS_OBJECT_ENSURE_REGISTERED (EnbCunbMac);

TypeId
EnbCunbMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::EnbCunbMac")
    .SetParent<CunbMac> ()
    .AddTraceSource ("SentAARequest",
                     "Sent AA Request, "
                     "to the Smart Meter",
                     MakeTraceSourceAccessor
                       (&EnbCunbMac::m_reqAssociation),
                          "ns3::Packet::TracedCallback")
    .AddTraceSource ("SentGETRequest",
                     "Sent GET Request, "
                     "to the Smart Meter",
                     MakeTraceSourceAccessor
                      (&EnbCunbMac::m_reqGet),
                         "ns3::Packet::TracedCallback")
    .AddConstructor<EnbCunbMac> ()
    .SetGroupName ("cunb");
  return tid;
}

EnbCunbMac::EnbCunbMac ():
		  m_beaconInterval(Seconds(50))
{
  NS_LOG_FUNCTION (this);
}

EnbCunbMac::~EnbCunbMac ()
{
  NS_LOG_FUNCTION (this);
}

void
EnbCunbMac::SetMType (CunbMacHeader::MType mType)
{
  m_mType = mType;
}

// This function is used to send the Association and Get Data Request to the smart meter
void
EnbCunbMac::SendRequest(Ptr<Packet> packet, Ptr<Node> ms )

{
	NS_LOG_INFO(" MS device "<< ms->GetDevice(0));

	// Get the CunbNetDevice
    Ptr<CunbNetDevice> cunbNetDevice;
	for (uint32_t i = 0; i < ms->GetNDevices (); i++)
	{
	   cunbNetDevice = ms->GetDevice (i)->GetObject<CunbNetDevice> ();
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

	NS_LOG_INFO("SM address to which ENB send request "<< msAddress);

//	m_msAddresses.push_back(msAddress);
	m_msContainer.Add(ms);

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

	CunbTxParameters params;
    params.bitrate = 600;
    params.nPreamble = 4;
    params.eccEnabled = 1;
    params.authEnabled = 1;
    params.fcsEnabled = 1;

	// Make a copy of the packet to work on
	Ptr<Packet> packetCopy = packet->Copy ();
	uint8_t pType = CheckAPDUType(packetCopy);

    // Get the duration
	Time duration = m_phy->GetOnAirTime (packet, params,ENB);

	NS_LOG_DEBUG ("Duration: " << duration.GetSeconds ());
	NS_LOG_DEBUG("pType "<<pType);

	// Use a default frequency for AA request
	double frequency = 868.3;

	if(pType == 3 || pType == 2)
		// use the same frequency with which it received the AA response
		//frequency = m_freqTorespond;
    {
		frequency = GetFreqFromAddress(msAddress);
    }

    NS_LOG_INFO("*******************Sending with Frequency "<< frequency);

    if(pType == 2) m_reqAssociation(packet); // AA Request Count

	// Find the channel with the desired frequency
	double sendingPower = m_channelHelper.GetTxPowerForChannel(CreateObject<LogicalCunbChannel> (frequency));

	NS_LOG_INFO("*******************Sending with Power "<< sendingPower);
	// Add the event to the channelHelper to keep track of duty cycle
	m_channelHelper.AddEvent (duration, CreateObject<LogicalCunbChannel>
		                               (frequency));

	// Send the packet to the PHY layer to send it on the channel
	m_phy->Send (packet, params, frequency, sendingPower);

}

void
EnbCunbMac::ReceiveRequest(Ptr<Packet const> packet)
{
	NS_LOG_INFO ("No implementation");
}

void
EnbCunbMac::Send (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);

  // Make a copy of the packet to work on
  if(packet->GetSize() > 25)
  {
  Ptr<Packet> packetCopy = packet->Copy ();
  uint8_t pType = CheckAPDUType(packetCopy);
  if(pType == 2) m_reqAssociation(packet); // AA Request Count
  if(pType == 3) m_reqGet(packet); // GET Request Count
  }

  // Get DataRate to send this packet with
  CunbTag tag;
  packet->RemovePacketTag (tag);
  double frequency = tag.GetFrequency ();

  NS_LOG_DEBUG ("Freq: " << frequency << " MHz");
  packet->AddPacketTag (tag);

  CunbTxParameters params;
  params.bitrate = 600;
  params.nPreamble = 4;
  params.eccEnabled = 1;
  params.authEnabled = 1;
  params.fcsEnabled = 1;

  // Get the duration
  Time duration = m_phy->GetOnAirTime (packet, params,ENB);

  NS_LOG_DEBUG ("Duration: " << duration.GetSeconds ());


  // Find the channel with the desired frequency
  double sendingPower = m_channelHelper.GetTxPowerForChannel
      (CreateObject<LogicalCunbChannel> (frequency));

  // Add the event to the channelHelper to keep track of duty cycle
  m_channelHelper.AddEvent (duration, CreateObject<LogicalCunbChannel>
                              (frequency));

  NS_LOG_INFO("*******************Sending with Power "<< sendingPower);
  // Send the packet to the PHY layer to send it on the channel
  m_phy->Send (packet, params, frequency, sendingPower);
}

bool
EnbCunbMac::IsTransmitting (void)
{
  return m_phy->IsTransmitting ();
}

double
EnbCunbMac::GetFreqToRespond()
{
	return m_freqTorespond;
}

void
EnbCunbMac::SetFreqToRespond(double frequency)
{
	m_freqTorespond = frequency;
}

double
EnbCunbMac::GetFreqFromAddress(CunbDeviceAddress address)
{
	for (std::list<std::pair<CunbDeviceAddress,double>>::iterator it =  m_address_freq_pair.end (); it !=  m_address_freq_pair.begin (); --it)
    {
		if((*it).first == address)
	    {
			  return (*it).second;
		}
    }
	return m_freqTorespond;
}

void
EnbCunbMac::Receive (Ptr<Packet const> packet)
{
  NS_LOG_FUNCTION (this << packet);

  // Make a copy of the packet to work on
  Ptr<Packet> packetCopy = packet->Copy ();

  Ptr<Packet> packetCopy2 = packet->Copy ();

  CunbTag tag;
  packetCopy->RemovePacketTag(tag);
  m_freqTorespond = tag.GetFrequency();

  // Only forward the packet if it's uplink
  CunbMacHeaderUl macHdrUl;
  packetCopy->PeekHeader (macHdrUl);

  NS_LOG_INFO("* * * * *Address of SM " << GetSMAddress(packetCopy) << "Freq is "<< tag.GetFrequency());

  m_address_freq_pair.push_back(std::make_pair(GetSMAddress(packetCopy),tag.GetFrequency()));

  NS_LOG_INFO("frequency received " << m_freqTorespond<< "Packet Size is "<< packetCopy->GetSize());

  uint8_t pType;

  // If it is an ACK packet from other eNBs if received, return or if it is not an Hello Packet
  if (packetCopy->GetSize() < 25) return;

  if (macHdrUl.IsUplink ())
    {
	  // If it is an Hello Packet forward to the C-UNB Server to decide which Base Station should respond
	  bool ifHello = CheckIfHello(packetCopy);

	  if(!ifHello)
		  pType = CheckAPDUType(packetCopy);

      NS_LOG_INFO("Packet Type " << pType << " Hello: "<<ifHello);

	  // If it is the GET response
	  if(pType == 0 || ifHello || pType == 1)
	  {
         m_device->GetObject<CunbNetDevice> ()->Receive (packetCopy2);
         m_receivedPacket (packet);

         NS_LOG_INFO("Forwarding GET response to CUNB server");
	  }
    }
  else
    {
      NS_LOG_DEBUG ("Not forwarding downlink message to NetDevice");
    }
}

void
EnbCunbMac::SetReceiveResponseOkCallback (RxReEnbOkCallback callback)
{
	m_rxReEnbOkCallback = callback;
}

void
EnbCunbMac::TxFinished (Ptr<const Packet> packet)
{
  NS_LOG_FUNCTION_NOARGS ();
}

Time
EnbCunbMac::GetWaitingTime (double frequency)
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_channelHelper.GetWaitingTime (CreateObject<LogicalCunbChannel>
                                           (frequency));
}


// Check APDU Type
uint8_t
EnbCunbMac::CheckAPDUType(Ptr<Packet> packet)
{
	Ptr<Packet> pCopy = packet->Copy();

	// implement the header check
	CunbMacTrailer macTlr;
	pCopy->RemoveTrailer(macTlr);

	// Extract the mac header
	CunbMacHeaderUl macHdr;
	pCopy->RemoveHeader (macHdr);

	// Extract the frame header
	CunbFrameHeaderUl frameHdr;
	pCopy->RemoveHeader (frameHdr);

	// Extract the Link Layer header
	CunbLinkLayerHeader llHdr;
	pCopy->RemoveHeader(llHdr);

	// Extract Transport Layer Header
	NewCosemWrapperHeader wrapperHdr;
	pCopy->RemoveHeader(wrapperHdr);

	// Extract the App Layer header
	AppLayerHeader appHdr;
	pCopy->RemoveHeader(appHdr);

	NewTypeAPDU typeHdr2;
	pCopy->RemoveHeader (typeHdr2);

	ApduType apduType = typeHdr2.GetApduType();

	NS_LOG_INFO("APDU type received or sent Enb: "<< apduType);

	if(apduType == AARE) return 1;
	else if(apduType == GETRES_N) return 0;
	else if(apduType == AARQ) return 2;
	else if(apduType == GETRQ_N) return 3;

	return 0;

}
// Get the SM to whom the Enb must respond
CunbDeviceAddress
EnbCunbMac::GetSMAddress(Ptr<Packet> packet)
{
	Ptr<Packet> pCopy = packet->Copy();

	// implement the header check
	CunbMacTrailer macTlr;
	pCopy->RemoveTrailer(macTlr);

	// Extract the mac header
	CunbMacHeaderUl macHdr;
	pCopy->RemoveHeader (macHdr);

	// Extract the frame header
	CunbFrameHeaderUl frameHdr;
	pCopy->RemoveHeader (frameHdr);

	CunbDeviceAddress msAddress = frameHdr.GetAddress();

	return msAddress;

}
// Get the SM to whom the Enb must respond
Ptr<Node>
EnbCunbMac::GetSMToRespond(Ptr<Packet> packet)
{
	Ptr<Packet> pCopy = packet->Copy();

	//Ptr<Node> dummyNode = Create<Node>();

	CunbDeviceAddress msAddress = GetSMAddress(pCopy);

	NS_LOG_INFO("SM to respond" << msAddress);

	for(uint8_t i = 0; i < m_msContainer.GetN() ; i++)
	{
		Ptr<Node> ms = m_msContainer.Get(i);
		NS_LOG_INFO("address of meters "<< ms->GetDevice (0)->GetObject<CunbNetDevice>()->GetMac ()->GetObject<MSCunbMac>()->GetDeviceAddress ());
		if(msAddress == ms->GetDevice (0)->GetObject<CunbNetDevice>()->GetMac ()->GetObject<MSCunbMac>()->GetDeviceAddress ())
		{
			NS_LOG_INFO("SM address to respond "<< msAddress);
			return ms;
		}
	}
	return Ptr<Node>();
}

bool
EnbCunbMac::CheckIfHello(Ptr<Packet> packet)
{
	Ptr<Packet> pCopy = packet->Copy();
	// implement the header check
	CunbMacTrailer macTlr;
	pCopy->RemoveTrailer(macTlr);

	// Extract the mac header
	CunbMacHeaderUl macHdr;
	pCopy->RemoveHeader (macHdr);

	if(macHdr.GetMType() == CunbMacHeaderUl::HELLO)
	{
		return true;
	}

	return false;
}

// Beacon Generation
void
EnbCunbMac::SendBeacon(Ptr<Packet> beaconPacketDummy)
{
	 Ptr<Packet> beaconPacket = Create<Packet>(reinterpret_cast<const uint8_t*> ("19"),10);
	 CunbDeviceAddress netAddr;
	 CunbBeaconHeader hdr;
	 //hdr.SetAddress(Mac48Address("ff:ff:ff:ff:ff:ff"));
	 CunbBeaconTrailer tlr;
	 uint64_t data = 24;
	 NS_LOG_FUNCTION (this);
	 NS_LOG_DEBUG (netAddr.GetNwkAddr() << " is sending beacon");

	 //Ptr<Packet> beaconPacket = Create<Packet> (2);  // 2 bytes of data
	 hdr.SetData(data);
	 hdr.SetGroupType(1);
	 hdr.SetGrpSeqNo(1);
	 beaconPacket->AddHeader(hdr);
	 beaconPacket->AddTrailer(tlr);

	 CunbDeviceAddress addr;
	 addr.Set(4294967295);
	 CunbFrameHeader frameHdr;
	 frameHdr.SetAddress(addr);
	 beaconPacket->AddHeader(frameHdr);

	 CunbTxParameters params;
	 params.bitrate = 600;
	 params.nPreamble = 4;
	 params.eccEnabled = 1;
	 params.authEnabled = 1;
	 params.fcsEnabled = 1;

	 NS_LOG_DEBUG("Data "<< hdr.GetData());
	 NS_LOG_DEBUG("GroupType "<< hdr.GetGroupType());
	 NS_LOG_DEBUG("GroupSeqNo "<< hdr.GetGrpSeqNo());

	 // Get the duration
	 Time duration = m_phy->GetOnAirTime (beaconPacket, params,ENB);

	 NS_LOG_DEBUG ("Duration: " << duration.GetSeconds ());

	 double frequency = 868.5;// frequency to use to send the beacon signal

	 // Find the channel with the desired frequency
	 double sendingPower = m_channelHelper.GetTxPowerForChannel
	       (CreateObject<LogicalCunbChannel> (frequency));

	 // Add the event to the channelHelper to keep track of duty cycle
	 m_channelHelper.AddEvent (duration, CreateObject<LogicalCunbChannel>
	                               (frequency));

	 beaconPacket->Print (std::cout);

	 // Send the packet to the PHY layer to send it on the channel
	 m_phy->Send (beaconPacket, params, frequency, sendingPower);

	 m_beaconSendEvent = Simulator::Schedule (GetBeaconInterval (), &EnbCunbMac::SendBeacon, this, beaconPacket);
}

Time
EnbCunbMac::GetBeaconInterval () const
{
   return m_beaconInterval;
 }

void
EnbCunbMac::ReceiveBeacon(Ptr<Packet const> beaconPacket)
{
   NS_LOG_INFO("Beacons are only sent by Base Stations");
}
/*
void
EnbCunbMac::ScheduleNextBeacon()
{
	m_tbtt += GetBeaconInterval ();
	m_beaconSendEvent = Simulator::Schedule (GetBeaconInterval (), &EnbCunbMac::SendBeacon, this);
}

void
EnbCunbMac::ShiftTbtt (Time shift)
{
   // User of ShiftTbtt () must take care don't shift it to the past
   NS_ASSERT (GetTbtt () + shift > Simulator::Now ());

   m_tbtt += shift;
   // Shift scheduled event
   Simulator::Cancel (m_beaconSendEvent);
   m_beaconSendEvent = Simulator::Schedule (GetTbtt () - Simulator::Now (), &EnbCunbMac::SendBeacon,this);
 }

Time
EnbCunbMac::GetTbtt () const
 {
   return m_tbtt;
 }
 */

}
