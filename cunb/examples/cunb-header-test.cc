#include "ns3/packet.h"
#include "ns3/cunb-frame-header.h"
#include "ns3/cunb-mac-header.h"
#include "ns3/cunb-mac-header.h"
#include "ns3/cunb-mac-trailer.h"
#include "ns3/cunb-mac-header-ul.h"
#include "ns3/cunb-mac-trailer-ul.h"
#include "ns3/log.h"
#include "ns3/command-line.h"
#include <bitset>
#include <cstdlib>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("CunbPacketTest");

int main (int argc, char *argv[])
{

  CommandLine cmd;
  cmd.Parse (argc, argv);

  LogComponentEnable ("CunbPacketTest", LOG_LEVEL_ALL);
  LogComponentEnable ("CunbFrameHeader", LOG_LEVEL_ALL);
  LogComponentEnable ("CunbMacHeader", LOG_LEVEL_ALL);
  LogComponentEnable ("CunbMacHeaderUl", LOG_LEVEL_ALL);
  //LogComponentEnable ("CunbMacTrailer", LOG_LEVEL_ALL);
  //LogComponentEnable ("CunbMacTrailerUl", LOG_LEVEL_ALL);
  LogComponentEnable ("Packet", LOG_LEVEL_ALL);

  // Test the CunbMacHeader class
  /////////////////////////////////
  NS_LOG_INFO ("Testing MacHeader");

  CunbMacHeader macHdr;
  macHdr.SetMType(CunbMacHeader::SINGLE_ACK);

  // Serialization
  Buffer macBuf;
  macBuf.AddAtStart (100);
  Buffer::Iterator macSerialized = macBuf.Begin ();
  macHdr.Serialize (macSerialized);

  // Deserialization
  macHdr.Deserialize (macSerialized);

  NS_ASSERT (macHdr.GetMType () == CunbMacHeader::SINGLE_ACK);


  NS_LOG_INFO ("Testing FrameHeader");
  CunbFrameHeader frameHdr;
  frameHdr.SetFPort(34);

  // Serialization
  Buffer buf;
  buf.AddAtStart (100);
  Buffer::Iterator serialized = buf.Begin ();
  frameHdr.Serialize (serialized);

  // Deserialization
  frameHdr.Deserialize (serialized);

  NS_LOG_INFO ("Testing MacTrailer");
  CunbMacTrailer macTrailer;

  //Ptr<LinkCheckAns> command = (*(frameHdr.GetCommands ().begin ()))->GetObject<LinkCheckAns> ();

  /////////////////////////////////////////////////
  // Test a combination of the two above classes //
  /////////////////////////////////////////////////
  Ptr<Packet> pkt = Create<Packet> (5);
  pkt->AddHeader (frameHdr);
  pkt->AddHeader (macHdr);
  macTrailer.EnableFcs(true);

  pkt->Print(std::cout);
  macTrailer.SetFcs(pkt);
  macTrailer.SetAuth(pkt);
  pkt->AddTrailer(macTrailer);



  // Length = Payload + FrameHeader + MacHeader
  //        = 10 + (8+3) + 1 = 22
  CunbMacTrailer macTrailer1;
  macTrailer1.EnableFcs(true);
  pkt->RemoveTrailer(macTrailer1);
  pkt->Print(std::cout);
  bool verify = macTrailer1.CheckFcs(pkt);
  bool verifyAuth = macTrailer1.CheckAuth(pkt);




  CunbMacHeader macHdr1;
  pkt->RemoveHeader (macHdr1);

  //pkt->Print(std::cout);

  CunbFrameHeader frameHdr1;
  pkt->RemoveHeader (frameHdr1);

  //pkt->Print(std::cout);

  // Verify contents of removed MAC header and Trailer
  std::cout << unsigned( macHdr1.GetMType ()) << " " << unsigned( macHdr.GetMType ()) << std::endl;
  std::cout << unsigned( macTrailer1.GetFcs ()) << " " << unsigned( macTrailer.GetFcs ()) << std::endl <<"verify:"<< verify;
  std::cout << unsigned( macTrailer1.GetAuth ()) << " " << unsigned( macTrailer.GetAuth ()) << std::endl <<"verify Auth:"<< verifyAuth;
  // Verify contents of removed frame header
  std::cout << (frameHdr1.GetFPort () == frameHdr.GetFPort ()) << std::endl;


  return 0;
}
