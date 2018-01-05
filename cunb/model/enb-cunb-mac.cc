#include "ns3/enb-cunb-mac.h"
#include "ns3/cunb-mac-header.h"
#include "ns3/cunb-mac-header-ul.h"
#include "ns3/cunb-net-device.h"
#include "ns3/cunb-frame-header.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("EnbCunbMac");

NS_OBJECT_ENSURE_REGISTERED (EnbCunbMac);

TypeId
EnbCunbMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::EnbCunbMac")
    .SetParent<CunbMac> ()
    .AddConstructor<EnbCunbMac> ()
    .SetGroupName ("cunb");
  return tid;
}

EnbCunbMac::EnbCunbMac ():
		  m_beaconInterval(Seconds(4))
{
  NS_LOG_FUNCTION (this);
}

EnbCunbMac::~EnbCunbMac ()
{
  NS_LOG_FUNCTION (this);
}

void
EnbCunbMac::Send (Ptr<Packet> packet)
{
  NS_LOG_FUNCTION (this << packet);

  // Get DataRate to send this packet with
  CunbTag tag;
  packet->RemovePacketTag (tag);
  uint8_t dataRate = tag.GetDataRate ();
  double frequency = tag.GetFrequency ();
  NS_LOG_DEBUG ("DR: " << unsigned (dataRate));
  NS_LOG_DEBUG ("BW: " << GetBandwidthFromDataRate (dataRate));
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

  // Send the packet to the PHY layer to send it on the channel
  m_phy->Send (packet, params, frequency, sendingPower);
}

bool
EnbCunbMac::IsTransmitting (void)
{
  return m_phy->IsTransmitting ();
}

void
EnbCunbMac::Receive (Ptr<Packet const> packet)
{
  NS_LOG_FUNCTION (this << packet);

  // Make a copy of the packet to work on
  Ptr<Packet> packetCopy = packet->Copy ();

  // Only forward the packet if it's uplink
  CunbMacHeaderUl macHdrUl;
  packetCopy->PeekHeader (macHdrUl);

  if (macHdrUl.IsUplink ())
    {
      m_device->GetObject<CunbNetDevice> ()->Receive (packetCopy);
      m_receivedPacket (packet);
    }
  else
    {
      NS_LOG_DEBUG ("Not forwarding downlink message to NetDevice");
    }
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

// Beacon Generation
void
EnbCunbMac::SendBeacon(Ptr<Packet> beaconPacket)
{
	 CunbDeviceAddress netAddr;
	 CunbBeaconHeader hdr;
	 //hdr.SetAddress(Mac48Address("ff:ff:ff:ff:ff:ff"));
	 CunbBeaconTrailer tlr;
	 uint64_t data = 234;
	 NS_LOG_FUNCTION (this);
	 NS_LOG_DEBUG (netAddr.GetNwkAddr() << " is sending beacon");

	 //Ptr<Packet> beaconPacket = Create<Packet> (2);  // 2 bytes of data
	 hdr.SetData(data);
	 hdr.SetGroupType(2);
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
