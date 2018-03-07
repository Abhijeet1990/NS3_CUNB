#include "ns3/one-time-reporting.h"
#include "ns3/ms-cunb-mac.h"
#include "ns3/pointer.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/cunb-net-device.h"
#include "ns3/app-layer-header.h"
#include "ns3/new-cosem-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("OneTimeReporting");

NS_OBJECT_ENSURE_REGISTERED (OneTimeReporting);

TypeId
OneTimeReporting::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::OneTimeReporting")
    .SetParent<Application> ()
    .AddConstructor<OneTimeReporting> ()
    .SetGroupName ("cunb");
  return tid;
}

OneTimeReporting::OneTimeReporting ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

OneTimeReporting::OneTimeReporting (Time sendTime) :
  m_sendTime (sendTime)
{
  NS_LOG_FUNCTION_NOARGS ();
}

OneTimeReporting::~OneTimeReporting ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
OneTimeReporting::SetSendTime (Time sendTime)
{
  NS_LOG_FUNCTION (this << sendTime);

  m_sendTime = sendTime;
}

void
OneTimeReporting::ReceiveRequest(Ptr<Packet> packet)
{

	// Extract Transport Layer Header
	NewCosemWrapperHeader wrapperHdr;
	packet->RemoveHeader(wrapperHdr);
	NS_LOG_INFO("Source Port "<< wrapperHdr.GetSrcwPort());

	// Extract the App Layer header
	AppLayerHeader appHdr;
	packet->RemoveHeader(appHdr);

	NewTypeAPDU typeHdr2;
	packet->RemoveHeader (typeHdr2);

	ApduType typeAPDU = typeHdr2.GetApduType ();

	NS_LOG_INFO("Type APDU "<< typeAPDU);

	if (typeAPDU == GETRQ_N)
	{
		/*
		NewCosemGetRequestNormalHeader hdr;
		packet->RemoveHeader (hdr);
		NS_LOG_INFO("Type APDU "<< typeAPDU);
		m_req [0] = hdr.GetClassId ();
	    m_req [1] = hdr.GetInstanceId ();
		m_req [2] = hdr.GetAttributeId ();
		NS_LOG_INFO("Class Id: "<< m_req[0]<<", Instance Id: "<< m_req[1] <<", Attribute Id:"<< m_req[2]);
        */
		m_sendEvent = Simulator::Schedule (Seconds(0.0), &OneTimeReporting::SendPacket,
		                                     this,0);

	}
	else if(typeAPDU == AARQ)
	{
		NS_LOG_INFO("Sending AA response");
		//Simulator::Cancel (m_sendReqEvent);
		m_sendReqEvent = Simulator::Schedule (Seconds(0.0), &OneTimeReporting::SendPacket,this,1);
		//SendPacket(1);
	}

}

void
OneTimeReporting::SendPacket (uint8_t pType)
{
  NS_LOG_FUNCTION (this);

  // Create and send a new packet
  Ptr<Packet> packet = Create<Packet>();

  if(pType == 0)
  {
	  uint8_t invokeIdAndPriority = 2;
      uint32_t data = 789;
      NewCosemGetResponseNormalHeader cosemHdr;
      cosemHdr.SetInvokeIdAndPriority (invokeIdAndPriority);
      cosemHdr.SetData (data);
      cosemHdr.SetDataAccessResult (0); // Success {0}
      packet->AddHeader (cosemHdr); // Copy the header into the packet

      NS_LOG_INFO("Data sending " << cosemHdr.GetData());

      NewTypeAPDU typeHdr;
      typeHdr.SetApduType ((ApduType)cosemHdr.GetIdApdu()); // Define the type of APDU
      packet->AddHeader (typeHdr); // Copy the header into the packet
  }
  else if(pType == 1)
  {
      // Build an xDLMS-Initiate.res PDU and an AARE APDU
      NewCosemAareHeader aahdr;
      aahdr.SetApplicationContextName (0);  // {N/A}Application Context Name (rules that govern th exchange-parameters)
      aahdr.SetResult (0);  // Result of the request AA, {0, accepted}
      aahdr.SetResultSourceDiagnostic (0);  // Result of a rejection of the a request AA, {0, null}

      aahdr.SetNegotiatedQualityOfService (0);  // Not used, {0, N/A}
      aahdr.SetNegotiatedDlmsVersionNumber (6);  // Version number, {6}
      aahdr.SetNegotiatedConformance (0x001010);  // {0x001010}, Based on the example in Annex C IEC 62056-53
      aahdr.SetServerMaxReceivePduSize (0x1F4);   // Server_Max_Receive_PDU_Size,{0x1F4}: 500 bytes
      aahdr.SetVaaName (0x0007);  // Dummy Value {0x0007}.Taken from page 98 IEC 62056-53
      packet->AddHeader (aahdr); // Copy the header into the packet

      NewTypeAPDU typeaaHdr;
      typeaaHdr.SetApduType ((ApduType)aahdr.GetIdApdu()); // Define the type of APDU
      packet->AddHeader (typeaaHdr); // Copy the header into the packet

      NS_LOG_INFO("Sending AA response");
  }

  AppLayerHeader appHdr;
  appHdr.SetPtype(1);
  packet->AddHeader(appHdr);


  // Add the UDP Wrapper Header
  // Add Wrapper header
  NewCosemWrapperHeader wrapperHdr;
  uint16_t sourceWPort = 80;
  uint16_t destWPort = 90;
  wrapperHdr.SetSrcwPort (sourceWPort);
  wrapperHdr.SetDstwPort (destWPort);
  wrapperHdr.SetLength (packet->GetSize ());
  packet->AddHeader (wrapperHdr);

  m_mac->GetObject<MSCunbMac> ()->SetMType
    (CunbMacHeaderUl::SINGLE_ACK);
  m_mac->Send (packet);
}

void
OneTimeReporting::StartApplication (void)
{
  NS_LOG_FUNCTION (this);

  // Make sure we have a MAC layer
  if (m_mac == 0)
    {
      // Assumes there's only one device
      Ptr<CunbNetDevice> cunbNetDevice = m_node->GetDevice (0)->GetObject<CunbNetDevice> ();

      m_mac = cunbNetDevice->GetMac ();
      NS_ASSERT (m_mac != 0);
    }

  // Schedule the next SendPacket event
  Simulator::Cancel (m_sendEvent);
  m_sendEvent = Simulator::Schedule (m_sendTime, &OneTimeReporting::SendPacket,
                                     this,0);
}

void
OneTimeReporting::StopApplication (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Simulator::Cancel (m_sendEvent);
}
}
