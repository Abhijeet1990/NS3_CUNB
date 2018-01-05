#ifndef CUNB_FORWARDER_H
#define CUNB_FORWARDER_H

#include "ns3/application.h"
#include "ns3/cunb-net-device.h"
#include "ns3/point-to-point-net-device.h"
#include "ns3/nstime.h"
#include "ns3/attribute.h"

namespace ns3 {

/**
 * This application forwards packets between NetDevices:
 * CunbNetDevice -> PointToPointNetDevice and vice versa.
 * To connect with the CUNB Server in the Core Network
 */
class CunbForwarder : public Application
{
public:

  CunbForwarder ();
  ~CunbForwarder ();

  static TypeId GetTypeId (void);

  /**
   * Sets the device to use to communicate with the Core Network.
   *
   * \param NetDevice The CunbNetDevice on this node.
   */
  void SetCunbNetDevice (Ptr<CunbNetDevice> cunbNetDevice);

  /**
   * Sets the P2P device to use to communicate with the CUNB Server.
   *
   * \param pointToPointNetDevice The P2PNetDevice on this node.
   */
  void SetPointToPointNetDevice (Ptr<PointToPointNetDevice> pointToPointNetDevice);

  /**
   * Receive a packet from the CunbNetDevice.
   *
   * \param cunbNetDevice The CunbNetDevice we received the packet from.
   * \param packet The packet we received.
   * \param protocol The protocol number associated to this packet.
   * \param sender The address of the sender.
   * \returns True if we can handle the packet, false otherwise.
   */
  bool ReceiveFromCunb (Ptr<NetDevice> cunbNetDevice, Ptr<const Packet> packet,
                        uint16_t protocol, const Address& sender);

  /**
   * Receive a packet from the PointToPointNetDevice
   */
  bool ReceiveFromPointToPoint (Ptr<NetDevice> pointToPointNetDevice,
                                Ptr<const Packet> packet, uint16_t protocol,
                                const Address& sender);

  /**
   * Start the application
   */
  void StartApplication (void);

  /**
   * Stop the application
   */
  void StopApplication (void);

private:
  Ptr<CunbNetDevice> m_cunbNetDevice; //!< Pointer to the node's CunbNetDevice

  Ptr<PointToPointNetDevice> m_pointToPointNetDevice; //!< Pointer to the
                                                      //!P2PNetDevice we use to
                                                      //!communicate with the CUNB Server
};

} //namespace ns3

#endif /* CUNB_FORWARDER */
