#include "ns3/mobile-autonomous-reporting.h"
#include "ns3/pointer.h"
#include "ns3/log.h"
#include "ns3/double.h"
#include "ns3/string.h"
#include "ns3/cunb-net-device.h"
#include "ns3/app-layer-header.h"
#include "ns3/new-cosem-header.h"
#include "ns3/ms-cunb-mac.h"
#include "ns3/cunb-mac-header-ul.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("MobileAutonomousReporting");

NS_OBJECT_ENSURE_REGISTERED (MobileAutonomousReporting);

TypeId
MobileAutonomousReporting::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::MobileAutonomousReporting")
    .SetParent<Application> ()
    .AddConstructor<MobileAutonomousReporting> ()
    .SetGroupName ("cunb")
    .AddAttribute ("Interval", "The interval between packet sends of this app",
                   TimeValue (Seconds (0)),
                   MakeTimeAccessor (&MobileAutonomousReporting::GetInterval,
                                     &MobileAutonomousReporting::SetInterval),
                   MakeTimeChecker ());
  /*
    .AddAttribute ("PacketSize", "The size of the packets this application sends, in bytes",
                   StringValue ("ns3::ParetoRandomVariable[Bound=100,Shape=2.5]"),
                   MakePointerAccessor (&MobileAutonomousReporting::m_pktSize),
                   MakePointerChecker <RandomVariableStream>())*/
  return tid;
}

MobileAutonomousReporting::MobileAutonomousReporting () :
  m_interval (Seconds (60)),
  m_initialDelay (Seconds (1)),
  m_pktSize(49),
  m_randomPktSize (0)

{
  //NS_LOG_FUNCTION_NOARGS ();
}

MobileAutonomousReporting::~MobileAutonomousReporting ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void
MobileAutonomousReporting::SetInterval (Time interval)
{
  //NS_LOG_FUNCTION (this << interval);
  m_interval = interval;
}

Time
MobileAutonomousReporting::GetInterval (void) const
{
  //NS_LOG_FUNCTION (this);
  return m_interval;
}

void
MobileAutonomousReporting::SetInitialDelay (Time delay)
{
  //NS_LOG_FUNCTION (this << delay);
  m_initialDelay = delay;
}

void
MobileAutonomousReporting::StartMAR(Ptr<Packet> packet, double frequency)
{
	m_startMAREvent = Simulator::Schedule (Seconds(0.0), &MobileAutonomousReporting::SendPacket,
			                                     this, frequency);
}


void
MobileAutonomousReporting::SendPacket (double frequency)
{
  //NS_LOG_FUNCTION (this);

  // Create and send a new packet
  //int size = m_pktSize->GetInteger ();
  int size =m_pktSize;
  Ptr<Packet> packet;
  if (m_randomPktSize == true)
    {
      packet = Create<Packet>(size);
    }
  else
    {
      packet = Create<Packet>();
    }

  uint8_t invokeIdAndPriority = 2;
  uint32_t data = 789;
  NewCosemGetResponseNormalHeader cosemHdr;
  cosemHdr.SetInvokeIdAndPriority (invokeIdAndPriority);
  cosemHdr.SetData (data);
  cosemHdr.SetDataAccessResult (0); // Success {0}
  packet->AddHeader (cosemHdr); // Copy the header into the packet

  NewTypeAPDU typeHdr;
  typeHdr.SetApduType ((ApduType)cosemHdr.GetIdApdu()); // Define the type of APDU
  packet->AddHeader (typeHdr); // Copy the header into the packet

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
  m_mac->GetObject<MSCunbMac> ()->SetFrequencyToSend(frequency);
  m_mac->Send (packet);


  // Schedule the next SendPacket event
  m_sendEvent = Simulator::Schedule (m_interval, &MobileAutonomousReporting::SendPacket,
                                     this,frequency);

  //NS_LOG_DEBUG ("Sent a packet of size " << packet->GetSize ());
}

void
MobileAutonomousReporting::StartApplication (void)
{
  //NS_LOG_FUNCTION (this);

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
  //NS_LOG_DEBUG ("Starting up application with a first event with a " <<
    //            m_initialDelay.GetSeconds () << " seconds delay");
  m_sendEvent = Simulator::Schedule (m_initialDelay,
                                     &MobileAutonomousReporting::SendPacket, this,0.0);
  //NS_LOG_DEBUG ("Event Id: " << m_sendEvent.GetUid ());
}

void
MobileAutonomousReporting::StopApplication (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  Simulator::Cancel (m_sendEvent);
}

}
