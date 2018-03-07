#include "ns3/cunb-mac-helper.h"
#include "ns3/enb-cunb-phy.h"
#include "ns3/cunb-net-device.h"
#include "ns3/log.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("CunbMacHelper");

CunbMacHelper::CunbMacHelper () :
  m_region (CunbMacHelper::EU)
{
}

void
CunbMacHelper::Set (std::string name, const AttributeValue &v)
{
  m_mac.Set (name, v);
}

void
CunbMacHelper::SetDeviceType (enum DeviceType dt)
{
  NS_LOG_FUNCTION (this << dt);
  switch (dt)
    {
    case ENB:
      m_mac.SetTypeId ("ns3::EnbCunbMac");
      break;
    case MS:
      m_mac.SetTypeId ("ns3::MSCunbMac");
      break;
    }
  m_deviceType = dt;
}

void
CunbMacHelper::SetAddressGenerator (Ptr<CunbDeviceAddressGenerator> addrGen)
{
  NS_LOG_FUNCTION (this);

  m_addrGen = addrGen;
}

void
CunbMacHelper::SetRegion (enum CunbMacHelper::Regions region)
{
  m_region = region;
}

Ptr<CunbMac>
CunbMacHelper::Create (Ptr<Node> node, Ptr<NetDevice> device) const
{
  Ptr<CunbMac> mac = m_mac.Create<CunbMac> ();
  mac->SetDevice (device);

  // If we are operating on an end device, add an address to it
  if (m_deviceType == MS && m_addrGen != 0)
    {
      mac->GetObject<MSCunbMac> ()->SetDeviceAddress
        (m_addrGen->NextAddress ());
    }

  // Add a basic list of channels based on the region where the device is
  // operating
  if (m_deviceType == MS)
    {
      Ptr<MSCunbMac> msMac = mac->GetObject<MSCunbMac> ();
      switch (m_region)
        {
        case CunbMacHelper::EU:
          {
            ConfigureForEuRegion (msMac);
            break;
          }
        default:
          {
            NS_LOG_ERROR ("This region isn't supported yet!");
            break;
          }
        }
    }
  else
    {
      Ptr<EnbCunbMac> enbMac = mac->GetObject<EnbCunbMac> ();
      switch (m_region)
        {
        case CunbMacHelper::EU:
          {
            ConfigureForEuRegion (enbMac);
            break;
          }
        default:
          {
            NS_LOG_ERROR ("This region isn't supported yet!");
            break;
          }
        }
    }
  return mac;
}

void
CunbMacHelper::ConfigureForEuRegion (Ptr<MSCunbMac> msMac) const
{
  NS_LOG_FUNCTION_NOARGS ();

  ApplyCommonEuConfigurations (msMac);

  /////////////////////////////////////////////////////
  // TxPower -> Transmission power in dBm conversion //
  /////////////////////////////////////////////////////
  msMac->SetTxDbmForTxPower (std::vector<double> {16, 14, 12, 10, 8, 6, 4, 2});

  /////////////////////
  // Preamble length //
  /////////////////////
  msMac->SetNPreambleSymbols (8);

  //////////////////////////////////////
  // Second receive window parameters //
  //////////////////////////////////////
  msMac->SetSecondReceiveWindowDataRate (0);
  msMac->SetSecondReceiveWindowFrequency (868.2);
  msMac->SetThirdReceiveWindowFrequency (868.25);
}

void
CunbMacHelper::ConfigureForEuRegion (Ptr<EnbCunbMac> enbMac) const
{
  NS_LOG_FUNCTION_NOARGS ();

  ///////////////////////////////
  // ReceivePath configuration //
  ///////////////////////////////
  Ptr<EnbCunbPhy> enbPhy = enbMac->GetDevice ()->
    GetObject<CunbNetDevice> ()->GetPhy ()->GetObject<EnbCunbPhy> ();

  ApplyCommonEuConfigurations (enbMac);

  if (enbPhy) // If cast is successful, there's a EnbCunbPhy
    {
      NS_LOG_DEBUG ("Resetting reception paths");
      enbPhy->ResetReceptionPaths ();

      std::vector<double> frequencies;

      double end_freq = 868.3;
      double start_freq = 868.1;
      uint8_t microchannels = 150;

      // create micro channels
      double step_size = (end_freq - start_freq)/microchannels;
      for (uint8_t i = 0; i < microchannels;i++)
      {
    	end_freq = start_freq + step_size;
    	double center_freq = (start_freq + end_freq) / 2;
        frequencies.push_back(center_freq);
        start_freq = start_freq + step_size;
       }

      std::vector<double>::iterator it = frequencies.begin ();

      int receptionPaths = 0;
      int maxReceptionPaths = microchannels;
      while (receptionPaths < maxReceptionPaths)
        {
          if (it == frequencies.end ())
            it = frequencies.begin ();
          enbPhy->GetObject<EnbCunbPhy> ()->AddReceptionPath (*it);
          ++it;
          receptionPaths++;
        }
    }
}

void
CunbMacHelper::ApplyCommonEuConfigurations (Ptr<CunbMac> cunbMac) const
{
  NS_LOG_FUNCTION_NOARGS ();

  //////////////
  // SubBands //
  //////////////

  double end_freq = 868.3;
  double start_freq = 868.1;
  uint8_t microchannels = 150;

  // create micro channels
  double step_size = (end_freq - start_freq)/microchannels;

  LogicalCunbChannelHelper channelHelper;

  //////////////////////
    // Default channels //
    //////////////////////

  Ptr<LogicalCunbChannel> lc[microchannels];

  for(uint8_t i = 0 ; i < microchannels ; i++)
  {
	  end_freq = start_freq + step_size;
	  channelHelper.AddSubBand (start_freq, end_freq,  0);
	  double center_freq = (start_freq + end_freq) / 2;
      NS_LOG_INFO("center freq" << center_freq);
	  lc[i] = CreateObject<LogicalCunbChannel>(center_freq, 0, 5);
	  start_freq = start_freq + step_size;
	  channelHelper.AddChannel (lc[i]);
  }

  cunbMac->SetLogicalCunbChannelHelper (channelHelper);

  ///////////////////////////////////////////////
  // DataRate -> SF, DataRate -> Bandwidth     //
  // and DataRate -> MaxAppPayload conversions //
  ///////////////////////////////////////////////

  cunbMac->SetBandwidthForDataRate (std::vector<double>
                                    {125000,125000,125000,125000,125000,125000,250000});
  cunbMac->SetMaxAppPayloadForDataRate (std::vector<uint32_t>
                                        {59,59,59,123,230,230,230,230});

}

void
CunbMacHelper::SetSpreadingFactorsUp (NodeContainer mss, NodeContainer enbs, Ptr<CunbChannel> channel)
{
  NS_LOG_FUNCTION_NOARGS ();

  for (NodeContainer::Iterator j = mss.Begin (); j != mss.End (); ++j)
    {
      Ptr<Node> object = *j;
      Ptr<MobilityModel> position = object->GetObject<MobilityModel> ();
      NS_ASSERT (position != 0);
      Ptr<NetDevice> netDevice = object->GetDevice (0);
      Ptr<CunbNetDevice> cunbNetDevice = netDevice->GetObject<CunbNetDevice> ();
      NS_ASSERT (cunbNetDevice != 0);
      Ptr<MSCunbMac> mac = cunbNetDevice->GetMac ()->GetObject<MSCunbMac> ();
      NS_ASSERT (mac != 0);

      // Try computing the distance from each eNB and find the best one
      Ptr<Node> bestEnb = enbs.Get (0);
      Ptr<MobilityModel> bestEnbPosition = bestEnb->GetObject<MobilityModel> ();

      // Assume devices transmit at 14 dBm
      double highestRxPower = channel->GetRxPower (14, position, bestEnbPosition);

      for (NodeContainer::Iterator currentEnb = enbs.Begin () + 1;
           currentEnb != enbs.End (); ++currentEnb)
        {
          // Compute the power received from the current eNB
          Ptr<Node> curr = *currentEnb;
          Ptr<MobilityModel> currPosition = curr->GetObject<MobilityModel> ();
          double currentRxPower = channel->GetRxPower (14, position, currPosition);    // dBm

          if (currentRxPower > highestRxPower)
            {
              bestEnb = curr;
              bestEnbPosition = curr->GetObject<MobilityModel> ();
              highestRxPower = currentRxPower;
            }
        }

      // NS_LOG_DEBUG ("Rx Power: " << highestRxPower);
      double rxPower = highestRxPower;

      // Get the eNB sensitivity
      Ptr<NetDevice> enbNetDevice = bestEnb->GetDevice (0);
      Ptr<CunbNetDevice> enbCunbNetDevice = enbNetDevice->GetObject<CunbNetDevice> ();
      Ptr<EnbCunbPhy> enbPhy = enbCunbNetDevice->GetPhy ()->GetObject<EnbCunbPhy> ();
      const double enbSensitivity = enbPhy->sensitivity;

      if(rxPower > enbSensitivity)
        {
          mac->SetDataRate (5);
        }
      else // Device is out of range. Assign SF12.
        {
          mac->SetDataRate (0);
        }
    }
}
}
