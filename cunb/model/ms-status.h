#ifndef MS_STATUS_H
#define MS_STATUS_H

#include "ns3/object.h"
#include "ns3/cunb-net-device.h"
#include "ns3/cunb-device-address.h"
#include "ns3/cunb-mac-header.h"
#include "ns3/cunb-mac-trailer.h"
#include "ns3/cunb-frame-header.h"
#include "ns3/cunb-linklayer-header.h"
#include "ns3/ms-cunb-mac.h"

namespace ns3 {

/**
 * This class represents the CUNB Server's knowledge about an Mobile Station(MS) in
 * the Cunb network it is administering.
 *
 * The CUNB Server contains a list of instances of this class, once for
 * each MS in the network. This class holds the reply packet that the
 * CUNB server will send to this device at the first available receive
 * window. Furthermore, this class is used to keep track of all eNBs that
 * are able to receive the MS's packets. On new packet arrivals at the
 * CUNB Server, the UpdateEnbData method is called to update the
 * m_enbs map, that associates a Gateway's PointToPointNetDevice address
 * to the power it received this ED's last packet. This information is then
 * used in the GetSortedGatewayAddresses method to return a list of the
 * preferred gateways through which to reply to this device.
 */
class MSStatus
{
public:

  /**
   * Structure representing the reply that the network server will send this
   * device at the first opportunity.
   */
  struct Reply
  {
    bool hasReply = false;   // Whether this device already has a reply.
    Ptr<Packet> packet;   // The packet that will be sent as a reply.
    CunbMacHeader macHeader; // The MacHeader to attach to the reply packet.
    CunbMacTrailer macTrailer; // The MacTrailer to attach to the reply packet.
    CunbFrameHeader frameHeader; // The FrameHeader to attach to the reply packet.
    CunbLinkLayerHeader llHeader; // The LinkLayerHeader to attach to the reply packet.
  };

  MSStatus();
  virtual ~MSStatus();

  MSStatus(Ptr<MSCunbMac> MSMac);

  /**
   * Get the data rate this device is using
   *
   * \return An unsigned 8-bit integer containing the data rate.
   */
  uint8_t GetDataRate ();

  /**
   * Get the CunbDeviceAddress that the device represented by this MSStatus
   * is using.
   *
   * \return The address.
   */
  CunbDeviceAddress GetAddress ();

  /**
   * Set the CunbDeviceAddress that the device represented by this MSStatus
   * is using.
   *
   * \param address The device's address to set.
   */
  void SetAddress (CunbDeviceAddress address);

  /**
   * Update the MSStatus to take into account the power with which a
   * packet was received by the enb with this P2P address.
   *
   * \param enbAddress The enb's P2P interface address.
   * \param rcvPower The receive power, in dBm, with which the enb received
   * the device's last packet.
   */
  void UpdateEnbData (Address enbAddress, double rcvPower);

  /**
   * Return the address of the enb that received this device's last packet
   * with the highest power.
   *
   * \return The best gateway's P2P link address.
   */
  Address GetBestEnbAddress (void);

  /**
   * Return an iterator to the enb addresses that received a packet by this
   * device, in order from best to worst (i.e., from highest receive power to
   * lowest receive power).
   *
   * \return A list of addresses.
   */
  std::list<Address> GetSortedEnbAddresses (void);

  /**
   * Set the reply to send to this device.
   *
   * \param reply The reply structure to use for the next downlink transmission.
   */
  void SetReply (struct Reply reply);

  /**
   * Check whether this device already has a reply packet.
   *
   * \return True if there's already a reply for this device, false otherwise.
   */
  bool HasReply (void);

  /**
   * Return this device's next downlink packet.
   *
   * This method returns a full packet, to which headers are already added.
   *
   * \return The full packet for reply.
   */
  Ptr<Packet> GetReplyPacket (void);

  /**
   * Set the first window frequency of this device.
   */
  void SetFirstReceiveWindowFrequency (double frequency);

  /**
   * Get the first window frequency of this device.
   */
  double GetFirstReceiveWindowFrequency (void);

  /**
   * Return the second window frequency of this device.
   *
   * This value is _not_ memorized in this object, and instead it's queried
   * using the pointer to the device's MAC layer.
   */
  double GetSecondReceiveWindowFrequency (void);

  /**
   * Return the data rate this device expects on the first receive window.
   *
   * This value is memorized in this object, based on the dataRate used by the
   * uplink packet.
   */
  uint8_t GetFirstReceiveWindowDataRate (void);

  /**
   * Return the data rate this device expects on the second receive window.
   *
   * This value is _not_ memorized in this object, and instead it's queried
   * using the pointer to the device's MAC layer.
   */
  uint8_t GetSecondReceiveWindowDataRate (void);

  /**
   * Return the third window frequency of this device.
   *
   * This value is _not_ memorized in this object, and instead it's queried
   * using the pointer to the device's MAC layer.
   */
  double GetThirdReceiveWindowFrequency (void);

  /**
   * Return the data rate this device expects on the third receive window.
   *
   * This value is _not_ memorized in this object, and instead it's queried
   * using the pointer to the device's MAC layer.
   */
  uint8_t GetThirdReceiveWindowDataRate (void);

private:

  Ptr<MSCunbMac> m_mac;   //!< Pointer to the device

  CunbDeviceAddress m_address;   //!< The address of this device

  std::map<Address, double> m_enbs;   //!< The enbs that received a
                                          //!packet from the device represented
                                          //!by this MSStatus

  struct Reply m_reply; //!< Structure containing the next reply meant for this
                        //!device

  double m_firstReceiveWindowFrequency; //!< Frequency at which the device will
                                        //!open the first receive window

  // TODO Add missing information:
  // - Up/Down frame counters
};
}

#endif /* MS_STATUS_H */
