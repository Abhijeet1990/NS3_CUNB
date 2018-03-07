#ifndef MOBILE_AUTONOMOUS_REPORTING_H
#define MOBILE_AUTONOMOUS_REPORTING_H

#include "ns3/application.h"
#include "ns3/nstime.h"
#include "ns3/cunb-mac.h"
#include "ns3/attribute.h"

namespace ns3 {

class MobileAutonomousReporting : public Application {
public:

  MobileAutonomousReporting();
  ~MobileAutonomousReporting ();

  static TypeId GetTypeId (void);

  /**
   * Set the sending interval
   * \param interval the interval between two packet sendings
   */
  void SetInterval (Time interval);

  /**
   * Get the sending inteval
   * \returns the interval between two packet sends
   */
  Time GetInterval (void) const;

  /**
   * Set the initial delay of this application
   */
  void SetInitialDelay (Time delay);

  /**
   * Send a packet using the CunbNetDevice's Send method
   */
  void SendPacket (double frequency);

  /**
   * Start the application by scheduling the first SendPacket event
   */
  void StartApplication (void);

  /**
   * Stop the application
   */
  void StopApplication (void);

  void StartMAR(Ptr<Packet> packet, double frequency);

private:
  /**
   * The interval between to consecutive send events
   */
  Time m_interval;

  /**
   * The initial delay of this application
   */
  Time m_initialDelay;

  /**
   * The sending event scheduled as next
   */
  EventId m_sendEvent;

  EventId m_startMAREvent;

  /**
   * The MAC layer of this node
   */
  Ptr<CunbMac> m_mac;

  /**
   * The size of the packets this application sends
   */
  //Ptr<RandomVariableStream> m_pktSize;
  uint32_t m_pktSize;

  /**
   * Whether or not this application uses a random packet size.
   */
  bool m_randomPktSize;


};

} //namespace ns3

#endif /* SENDER_APPLICATION */
