#include "ns3/cunb-mac.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CunbMac");

//NS_OBJECT_ENSURE_REGISTERED (CunbMac);

TypeId
CunbMac::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CunbMac")
    .SetParent<Object> ()
    .SetGroupName ("cunb")
    .AddTraceSource ("ReceivedPacket",
                     "Trace source indicating a packet "
                     "was correctly received at the MAC layer",
                     MakeTraceSourceAccessor (&CunbMac::m_receivedPacket),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("CannotSendBecauseDutyCycle",
                     "Trace source indicating a packet "
                     "could not be sent immediately because of duty cycle limitations",
                     MakeTraceSourceAccessor (&CunbMac::m_cannotSendBecauseDutyCycle),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("StartSendingData",
                     "Trace source indicating the MAC layer"
                     "has begun the sending process for a packet",
                     MakeTraceSourceAccessor (&CunbMac::m_startSending),
                     "ns3::Packet::TracedCallback");
  return tid;
}

CunbMac::CunbMac ()
{
  NS_LOG_FUNCTION (this);
}

CunbMac::~CunbMac () {
  NS_LOG_FUNCTION (this);
}

void
CunbMac::SetDevice (Ptr<NetDevice> device)
{
  m_device = device;
}

Ptr<NetDevice>
CunbMac::GetDevice (void)
{
  return m_device;
}

Ptr<CunbPhy>
CunbMac::GetPhy (void)
{
  return m_phy;
}

void
CunbMac::SetPhy (Ptr<CunbPhy> phy)
{
  // Set the phy
  m_phy = phy;

  // Connect the receive callbacks
  m_phy->SetReceiveOkCallback (MakeCallback (&CunbMac::Receive, this));
  m_phy->SetReceiveOkBeaconCallback (MakeCallback (&CunbMac::ReceiveBeacon, this));
  m_phy->SetReceiveRequestOkCallback (MakeCallback (&CunbMac::ReceiveRequest, this));
  m_phy->SetTxFinishedCallback (MakeCallback (&CunbMac::TxFinished, this));
}

LogicalCunbChannelHelper
CunbMac::GetLogicalCunbChannelHelper (void)
{
  return m_channelHelper;
}

void
CunbMac::SetLogicalCunbChannelHelper (LogicalCunbChannelHelper helper)
{
  m_channelHelper = helper;
}

double
CunbMac::GetBandwidthFromDataRate (uint8_t dataRate)
{
  NS_LOG_FUNCTION (this << unsigned(dataRate));

  // Check we are in range
  if (dataRate > m_bandwidthForDataRate.size ())
    {
      return 0;
    }

  return m_bandwidthForDataRate.at (dataRate);
}

double
CunbMac::GetDbmForTxPower (uint8_t txPower)
{
  NS_LOG_FUNCTION (this << unsigned (txPower));

  if (txPower > m_txDbmForTxPower.size ())
    {
      return 0;
    }

  return m_txDbmForTxPower.at (txPower);
}


void
CunbMac::SetBandwidthForDataRate (std::vector<double> bandwidthForDataRate)
{
  m_bandwidthForDataRate = bandwidthForDataRate;
}

void
CunbMac::SetMaxAppPayloadForDataRate (std::vector<uint32_t> maxAppPayloadForDataRate)
{
  m_maxAppPayloadForDataRate = maxAppPayloadForDataRate;
}

void
CunbMac::SetTxDbmForTxPower (std::vector<double> txDbmForTxPower)
{
  m_txDbmForTxPower = txDbmForTxPower;
}

void
CunbMac::SetNPreambleSymbols (int nPreambleSymbols)
{
  m_nPreambleSymbols = nPreambleSymbols;
}

int
CunbMac::GetNPreambleSymbols (void)
{
  return m_nPreambleSymbols;
}

void
CunbMac::SetReplyDataRateMatrix (ReplyDataRateMatrix replyDataRateMatrix)
{
  m_replyDataRateMatrix = replyDataRateMatrix;
}
}
