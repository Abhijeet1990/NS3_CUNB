#ifndef ONE_TIME_REQUESTING_H
#define ONE_TIME_REQUESTING_H

#include "ns3/application.h"
#include "ns3/nstime.h"
#include "ns3/cunb-mac.h"
#include "ns3/simple-cunb-server.h"
#include "ns3/attribute.h"
#include "ns3/enb-cunb-mac.h"

namespace ns3 {

class OneTimeRequesting : public Application {
public:

  OneTimeRequesting ();
  OneTimeRequesting (Time sendTime);
  ~OneTimeRequesting ();

  static TypeId GetTypeId (void);

  static int AAreCount;

  /**
   * Send a packet using the CunbNetDevice's Send method.
   */
  void SendPacket (uint8_t pType);

  /**
   * Set the time at which this app will send a packet.
   */
  void SetSendTime (Time sendTime);

  /**
   * Start the application by scheduling the first SendPacket event.
   */
  void StartApplication (void);

  /**
   * Stop the application.
   */
  void StopApplication (void);

  void SetMS(Ptr<Node> ms);

  Ptr<Node> GetMS(void);
/*
  void SetPtype(uint8_t pType);

  uint8_t GetPtype(void);
  */

  void SetMac(Ptr<EnbCunbMac> mac);

  Ptr<EnbCunbMac> GetMac(void);

  void ReceivePacket(Ptr<Packet const> packet,Ptr<Node> ms);

private:

  /**
   * The time at which to send the packet.
   */
  Time m_sendTime;

  /**
   * The sending event.
   */
  EventId m_sendEvent;

  /**
   * The MAC layer of this node.
   */
  //Ptr<CunbMac> m_mac;
  Ptr<EnbCunbMac> m_mac;

  Ptr<Node> m_ms;

  //uint8_t m_ptype;
};

} //namespace ns3

#endif /* ONE_REQUEST_APPLICATION */
