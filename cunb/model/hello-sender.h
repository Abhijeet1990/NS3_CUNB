#ifndef HELLO_SENDER_H
#define HELLO_SENDER_H

#include "ns3/application.h"
#include "ns3/nstime.h"
#include "ns3/cunb-mac.h"
#include "ns3/attribute.h"

namespace ns3 {

class HelloSender : public Application {
public:

  HelloSender ();
  HelloSender (Time sendTime);
  ~HelloSender ();

  static TypeId GetTypeId (void);

  /**
   * Send a packet using the CunbNetDevice's Send method.
   */
  void SendPacket (void);

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

private:

  /**
   * The time at which to send the packet.
   */
  Time m_sendTime;

  /**
   * The sending event.
   */
  EventId m_sendEvent;

  EventId m_sendReqEvent;

  /**
   * The MAC layer of this node.
   */
  Ptr<CunbMac> m_mac;

};

} //namespace ns3

#endif /* HELLO_SENDER */
