#ifndef ONE_TIME_REPORTING_H
#define ONE_TIME_REPORTING_H

#include "ns3/application.h"
#include "ns3/nstime.h"
#include "ns3/cunb-mac.h"
#include "ns3/attribute.h"

namespace ns3 {

class OneTimeReporting : public Application {
public:

  OneTimeReporting ();
  OneTimeReporting (Time sendTime);
  ~OneTimeReporting ();

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

  /**
   * The MAC layer of this node.
   */
  Ptr<CunbMac> m_mac;
};

} //namespace ns3

#endif /* ONE_SHOT_APPLICATION */
