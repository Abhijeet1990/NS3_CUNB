#ifndef SIMPLE_CUNB_SERVER_H
#define SIMPLE_CUNB_SERVER_H

#include "ns3/application.h"
#include "ns3/net-device.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/packet.h"
#include "ns3/cunb-device-address.h"
#include "ns3/ms-status.h"
#include "ns3/enb-status.h"
#include "ns3/node-container.h"

namespace ns3 {

/**
 * A SimpleCunbServer is an application standing on top of a node equipped with
 * links that connect it with the enbs.
 *
 * This version of the CunbServer isn't smart enough to handle MAC commands,
 * but can reply correctly with ACKs to confirm uplink messages.
 */
class SimpleCunbServer : public Application
{
public:

  static TypeId GetTypeId (void);

  SimpleCunbServer();
  virtual ~SimpleCunbServer();

  /**
   * Parse and take action on the commands contained on the frameHeader
   */
  void ParseCommands (CunbFrameHeader frameHeader);

  /**
   * Start the Cunb application
   */
  void StartApplication (void);

  /**
   * Stop the Cunb application
   */
  void StopApplication (void);

  /**
   * Inform the SimpleCunbServer that these nodes are connected to the network
   * This method will create a MSStatus object for each new node, and add it to the list
   */
  void AddNodes (NodeContainer nodes);

  /**
   * Inform the SimpleCunbServer that this node is connected to the network
   * This method will create a MSStatus object for the new node (if it doesn't already exist)
   */
  void AddNode (Ptr<Node> node);

  /**
   * Add this eNB to the list of eNBs connected to this CUNB Server
   */
  void AddEnb (Ptr<Node> enb, Ptr<NetDevice> netDevice);

  /**
   * Receive a packet from a eNB
   * \param packet the received packet
   */
  bool Receive (Ptr<NetDevice> device, Ptr<const Packet> packet,
                uint16_t protocol, const Address& address);

  /**
   * Send a packet through a eNB to a MS, using the first receive window
   */
  void SendOnFirstWindow (CunbDeviceAddress address, uint16_t ptype);

  /**
   * Send a packet through a eNB to a MS, using the second receive window
   */
  void SendOnSecondWindow (CunbDeviceAddress address, uint16_t ptype);

  /**
     * Send a packet through a eNB to a MS, using the third receive window
     */
    void SendOnThirdWindow (CunbDeviceAddress address);

  /**
   * Check whether a reply to the MS with a certain address already exists
   */
  bool HasReply (CunbDeviceAddress address);

  /**
   * Get the data rate that should be used when replying in the first receive window
   */
  uint8_t GetDataRateForReply (uint8_t receivedDataRate);

  /**
   * Get the best enb that is available to reply to this device.
   *
   * This method assumes the enb needs to be available at the time that
   * it is called.
   */
  Address GetEnbForReply (CunbDeviceAddress deviceAddress, double frequency);

protected:
  std::map<CunbDeviceAddress,MSStatus> m_msStatuses;

  std::map<Address,EnbStatus> m_enbStatuses;
};

} /* namespace ns3 */

#endif /* SIMPLE_CUNB_SERVER_H */
