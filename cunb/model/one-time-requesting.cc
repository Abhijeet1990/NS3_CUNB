#include "ns3/one-time-requesting.h"
#include "ns3/enb-cunb-mac.h"
#include "ns3/pointer.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/cunb-net-device.h"
#include "ns3/app-layer-header.h"
#include "ns3/new-cosem-header.h"
#include "ns3/simple-cunb-server.h"
//#include "ns3/enb-cunb-mac.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("OneTimeRequesting");

NS_OBJECT_ENSURE_REGISTERED (OneTimeRequesting);

int
OneTimeRequesting::AAreCount = 0;

TypeId
OneTimeRequesting::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::OneTimeRequesting")
    .SetParent<Application> ()
    .AddConstructor<OneTimeRequesting> ()
    .SetGroupName ("cunb");
  return tid;
}

OneTimeRequesting::OneTimeRequesting ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

OneTimeRequesting::OneTimeRequesting (Time sendTime) :
  m_sendTime (sendTime)
{
  NS_LOG_FUNCTION_NOARGS ();
}

OneTimeRequesting::~OneTimeRequesting ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
OneTimeRequesting::SetSendTime (Time sendTime)
{
  NS_LOG_FUNCTION (this << sendTime);

  m_sendTime = sendTime;
}

void
OneTimeRequesting::SetMS(Ptr<Node> ms)
{
	m_ms = ms;
}

Ptr<Node>
OneTimeRequesting::GetMS(void)
{
	return m_ms;
}

void
OneTimeRequesting::SetMac(Ptr<EnbCunbMac> mac){
	m_mac = mac;
	m_mac->SetReceiveResponseOkCallback(MakeCallback (&OneTimeRequesting::ReceivePacket, this));
}


Ptr<EnbCunbMac>
OneTimeRequesting::GetMac(void){
	return m_mac;
}
/*
void
OTReHelper::SetPtype(uint8_t pType)
{
	m_ptype = pType;
}

uint8_t
OTReHelper::GetPtype()
{
	return m_ptype;
}
*/

void
OneTimeRequesting::ReceivePacket(Ptr<Packet const> packet, Ptr<Node> ms)
{
	// Send the GETN-RQ Packet
	m_ms = ms;
	m_sendEvent = Simulator::Schedule (Seconds(0), &OneTimeRequesting::SendPacket,this,0);
}

void
OneTimeRequesting::SendPacket (uint8_t pType)
{
  NS_LOG_FUNCTION (this);

  Ptr<Packet> packet = Create<Packet> (); // Create the GET-Request (Normal) APDU packet

  if(pType == 0) // for GET Request
  {
	  // Build an GET-Request (Normal) APDU
	  NewCosemGetRequestNormalHeader hdr;
      hdr.SetInvokeIdAndPriority (0x02);  // 0000 0010 (invoke_id {0b0000}),service_class= 1 (confirmed) priority level ({normal}))
      hdr.SetClassId (0X03);  // Class Register
      hdr.SetInstanceId (0x010100070000);  // OBIS CODE: 1.1.0.7.0.0
      hdr.SetAttributeId (0x02);  // Second Attribut = Value
      packet->AddHeader (hdr); // Copy the header into the packet

      NewTypeAPDU typeHdr;
      typeHdr.SetApduType ((ApduType)hdr.GetIdApdu()); // Define the type of APDU
      packet->AddHeader (typeHdr); // Copy the header into the packet
  }

  else if(pType == 1) // for AA Request
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
	  NS_LOG_INFO("AA req count "<< AAreCount++);

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
  //m_mac->GetObject<EnbCunbMac> ()->SetMType(CunbMacHeader::SINGLE_ACK);
  m_mac->SetMType(CunbMacHeader::SINGLE_ACK);
  m_mac->SendRequest (packet,m_ms);
}

void
OneTimeRequesting::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  // Make sure we have a MAC layer
   if (m_mac == 0)
     {
       // Assumes there's only one device
       Ptr<CunbNetDevice> cunbNetDevice = m_node->GetDevice (0)->GetObject<CunbNetDevice> ();

       m_mac = cunbNetDevice->GetMac ()->GetObject<EnbCunbMac> ();
       NS_ASSERT (m_mac != 0);
     }

  // Schedule the next SendPacket event
  Simulator::Cancel (m_sendEvent);

  // The last parameter is the request Packet Type. Whether it is an AARQ(1) or a GETN-RQ(0)
  m_sendEvent = Simulator::Schedule (m_sendTime, &OneTimeRequesting::SendPacket,
                                     this,1);
}

void
OneTimeRequesting::StopApplication (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Simulator::Cancel (m_sendEvent);
}
}
