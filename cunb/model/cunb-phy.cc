/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 Texas A & M University, College Station
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Abhijeet Sahu <abhijeet_ntpc@tamu.edu>
 */
#include "ns3/cunb-phy.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include <algorithm>

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CunbPhy");

NS_OBJECT_ENSURE_REGISTERED (CunbPhy);

TypeId
CunbPhy::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CunbPhy")
    .SetParent<Object> ()
    .SetGroupName ("cunb")
    .AddTraceSource ("StartSending",
                     "Trace source indicating the PHY layer"
                     "has begun the sending process for a packet",
                     MakeTraceSourceAccessor (&CunbPhy::m_startSending),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("PhyRxBegin",
                     "Trace source indicating a packet "
                     "is now being received from the channel medium "
                     "by the device",
                     MakeTraceSourceAccessor (&CunbPhy::m_phyRxBeginTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("PhyRxEnd",
                     "Trace source indicating the PHY has finished "
                     "the reception process for a packet",
                     MakeTraceSourceAccessor (&CunbPhy::m_phyRxEndTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("ReceivedPacket",
                     "Trace source indicating a packet "
                     "was correctly received",
                     MakeTraceSourceAccessor
                       (&CunbPhy::m_successfullyReceivedPacket),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("LostPacketBecauseInterference",
                     "Trace source indicating a packet "
                     "could not be correctly decoded because of interfering"
                     "signals",
                     MakeTraceSourceAccessor (&CunbPhy::m_interferedPacket),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("LostPacketBecauseUnderSensitivity",
                     "Trace source indicating a packet "
                     "could not be correctly received because"
                     "its received power is below the sensitivity of the receiver",
                     MakeTraceSourceAccessor (&CunbPhy::m_underSensitivity),
                     "ns3::Packet::TracedCallback");
  return tid;
}

CunbPhy::CunbPhy ()
{
}

CunbPhy::~CunbPhy ()
{
}

Ptr<NetDevice>
CunbPhy::GetDevice (void) const
{
  return m_device;
}

void
CunbPhy::SetDevice (Ptr<NetDevice> device)
{
  NS_LOG_FUNCTION (this << device);

  m_device = device;
}

Ptr<CunbChannel>
CunbPhy::GetChannel (void) const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_channel;
}

Ptr<MobilityModel>
CunbPhy::GetMobility (void)
{
  NS_LOG_FUNCTION_NOARGS ();

  // If there is a mobility model associated to this PHY, take the mobility from
  // there
  if (m_mobility != 0)
    {
      return m_mobility;
    }
  else // Else, take it from the node
    {
      return m_device->GetNode ()->GetObject<MobilityModel> ();
    }
}

void
CunbPhy::SetMobility (Ptr<MobilityModel> mobility)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_mobility = mobility;
}

void
CunbPhy::SetChannel (Ptr<CunbChannel> channel)
{
  NS_LOG_FUNCTION (this << channel);

  m_channel = channel;
}

void
CunbPhy::SetReceiveOkCallback (RxOkCallback callback)
{
  m_rxOkCallback = callback;
}

void
CunbPhy::SetReceiveOkBeaconCallback (RxOkCallbackBeacon callback)
{
  m_rxOkCallbackBeacon = callback;
}

void
CunbPhy::SetTxFinishedCallback (TxFinishedCallback callback)
{
  m_txFinishedCallback = callback;
}


Time
CunbPhy::GetOnAirTime (Ptr<Packet> packet, CunbTxParameters txParams, enum DeviceType dType)
{
  double totalTime;

  NS_LOG_FUNCTION (packet << txParams);
  double tSym;
  double tPreamble;
  double tHeader;
  double tTrailer;
  double tPayload;
  double tSeq_and_id; // only for uplink
  // Payload size can range from 0-34 bytes
  uint32_t pl = packet->GetSize ();  // Size in bytes
  NS_LOG_DEBUG ("Packet of size " << pl << " bytes");

  double ecc = txParams.eccEnabled ? 1 : 0;
  double auth = txParams.authEnabled ? 1 : 0;
  double fcs = txParams.fcsEnabled ? 1 : 0;

  // DL bit rate = 600 bps
  // Uses D-BPSK Modulation hence 2 bits represents one symbol
  // tSym = 2/600 = 3.33 milliseconds

  // Compute the symbol duration
  // Bandwidth is in Hz
  tSym = 2/txParams.bitrate;

  // Compute the preamble duration
  // Considering 8 bits for a preamble represent 4 symbol.
  tPreamble = txParams.nPreamble*tSym;

  switch(dType)
  {
  case ENB:
	    // Header excluding Preamble, comprising of frame type, payload length, ack bits
	    // Total = 48 bits = 24 symbol
	    tHeader = 24 * tSym;

	    // Trailer including auth,ecc and fcs
	    tTrailer = (auth * 8 + ecc * 16 + fcs * 4)*tSym;

	    // Time to transmit the payload
	    tPayload = pl * 4 * tSym; // multiplied by 4 as we are looking for no. of symbols

	    totalTime = tPreamble + tHeader + tTrailer + tPayload;
	    break;
  case MS:
	   // Header excluding Preamble, comprising of frame type, payload length, ack bits
	   // Total = 32 bits = 16 symbol
	    tHeader = 16 * tSym;

	   // 12 bits for Sequence Counter and 40 bits for identifier
        tSeq_and_id = (txParams.nSeqCounter + txParams.nMsId)*tSym;

	   // Trailer including auth,ecc and fcs
	  	tTrailer = (auth * 8 + ecc * 8 + fcs * 4)*tSym;

	   // Time to transmit the payload
	  	tPayload = pl * 4 * tSym; // multiplied by 4 as we are looking for no. of symbols

	  	totalTime = tPreamble + tHeader + tSeq_and_id + tTrailer + tPayload;
	    break;
  }



  NS_LOG_DEBUG ("tPreamble = " << tPreamble);
  NS_LOG_DEBUG ("tHeader = " << tHeader);
  NS_LOG_DEBUG ("tTrailer = " << tTrailer);
  NS_LOG_DEBUG ("tPayload = " << tPayload);
  NS_LOG_DEBUG ("Total time = " << totalTime);

  // Compute and return the total packet on-air time
  return Seconds (totalTime);
}

std::ostream &operator << (std::ostream &os, const CunbTxParameters &params)
{
  os << "BitRate: " << unsigned(params.bitrate) <<
  ", nPreamble: " << params.nPreamble <<
  ", eccEnabled: " << params.eccEnabled <<
  ", authEnabled: " << params.authEnabled <<
  ")";

  return os;
}
}
