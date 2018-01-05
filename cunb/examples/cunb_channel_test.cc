#include "ns3/log.h"
#include "ns3/command-line.h"
#include "ns3/logical-cunb-channel-helper.h"
#include <cstdlib>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ChannelTest");

int main (int argc, char *argv[])
{

  CommandLine cmd;
  cmd.Parse (argc, argv);

  LogComponentEnable ("ChannelTest", LOG_LEVEL_ALL);
  LogComponentEnable ("LogicalCunbChannel", LOG_LEVEL_ALL);
  LogComponentEnable ("LogicalCunbChannelHelper", LOG_LEVEL_ALL);
  LogComponentEnable ("SubBand", LOG_LEVEL_ALL);

  /////////////////////////////
  // Test LogicalCunbChannel //
  /////////////////////////////

  // Set the central frequency of channel
  Ptr<LogicalCunbChannel> channel1 = CreateObject<LogicalCunbChannel> (868);
  Ptr<LogicalCunbChannel> channel2 = CreateObject<LogicalCunbChannel> (868);
  Ptr<LogicalCunbChannel> channel3 = CreateObject<LogicalCunbChannel> (868.1);
  Ptr<LogicalCunbChannel> channel4 = CreateObject<LogicalCunbChannel> (868.001);

  // Equality between channels
  // Test the == and != operators
  NS_ASSERT (channel1 == channel2);
  NS_ASSERT (channel1 != channel3);
  NS_ASSERT (channel1 != channel4);

  //////////////////
  // Test SubBand //
  //////////////////

  // Setup: the max transmit power and the subband is decided by the beacon mesaages. Here we have statically configured
  SubBandCunb subBand (868, 868.7,  14);
  Ptr<LogicalCunbChannel> channel5 = CreateObject<LogicalCunbChannel> (870);

  // Test BelongsToSubBand
  NS_ASSERT (subBand.BelongsToSubBand (channel3));
  NS_ASSERT (subBand.BelongsToSubBand (channel3->GetFrequency ()));
  NS_ASSERT (!subBand.BelongsToSubBand (channel5));

  ///////////////////////////////////
  // Test LogicalCunbChannelHelper //
  ///////////////////////////////////

  // Setup
  Ptr<LogicalCunbChannelHelper> channelHelper = CreateObject<LogicalCunbChannelHelper> ();
  SubBandCunb subBand1 (869, 869.4,  27);
  channel1 = CreateObject<LogicalCunbChannel> (868.1);
  channel2 = CreateObject<LogicalCunbChannel> (868.3);
  channel3 = CreateObject<LogicalCunbChannel> (868.5);
  channel4 = CreateObject<LogicalCunbChannel> (869.1);
  channel5 = CreateObject<LogicalCunbChannel> (869.3);

  // Channel diagram
  //
  // Channels      1      2      3                     4       5
  // SubBands  868 ----- 0.1% ----- 868.7       869 ----- 1% ----- 869.4

  // Add SubBands and LogicalCunbChannels to the helper
  channelHelper->AddSubBand (&subBand);
  channelHelper->AddSubBand (&subBand1);
  channelHelper->AddChannel (channel1);
  channelHelper->AddChannel (channel2);
  channelHelper->AddChannel (channel3);
  channelHelper->AddChannel (channel4);
  channelHelper->AddChannel (channel5);

  // Duty Cycle tests
  // (high level duty cycle behavior)
  ///////////////////////////////////

  // Waiting time is computed correctly
  channelHelper->AddEvent (Seconds (2), channel1);
  Time expectedTimeOff = Seconds (2/0.01 - 2);
  NS_ASSERT (channelHelper->GetWaitingTime (channel1) == expectedTimeOff);

  // Duty Cycle involves the whole SubBand, not just a channel
  NS_ASSERT (channelHelper->GetWaitingTime (channel2) == expectedTimeOff);
  NS_ASSERT (channelHelper->GetWaitingTime (channel3) == expectedTimeOff);

  // Other bands are not affected by this transmission
  NS_ASSERT (channelHelper->GetWaitingTime (channel4) == 0);
  NS_ASSERT (channelHelper->GetWaitingTime (channel5) == 0);

  return 0;
}
