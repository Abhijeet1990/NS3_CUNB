#ifndef CUNB_NET_DEVICE_H
#define CUNB_NET_DEVICE_H

#include "ns3/net-device.h"
#include "ns3/cunb-channel.h"
#include "ns3/cunb-phy.h"
#include "ns3/cunb-mac.h"
#include "ns3/cunb-beacon-header.h"
#include "ns3/cunb-beacon-trailer.h"
#include "ns3/cunb-frame-header.h"

namespace ns3 {

class CunbChannel;
class CunbPhy;
class CunbMac;


class CunbNetDevice : public NetDevice
{
public:
  static TypeId GetTypeId (void);

  // Constructor and destructor
  CunbNetDevice ();
  virtual ~CunbNetDevice ();

  /**
   * Set which CunbMac instance is linked to this device.
   *
   * \param mac the mac layer to use.
   */
  void SetMac (Ptr<CunbMac> mac);

  /**
   * Set which CunbPhy instance is linked to this device.
   *
   * \param phy the phy layer to use.
   */
  void SetPhy (Ptr<CunbPhy> phy);

  /**
   * Get the CunbMac instance that is linked to this NetDevice.
   *
   * \return the mac we are currently using.
   */
  Ptr<CunbMac> GetMac (void) const;

  /**
   * Get the CunbPhy instance that is linked to this NetDevice.
   *
   * \return the phy we are currently using.
   */
  Ptr<CunbPhy> GetPhy (void) const;

  /**
   * Send a packet through the CUNB stack.
   *
   * \param packet The packet to send.
   */
  void Send (Ptr<Packet> packet);

  /**
   * Callback the Mac layer calls whenever a packet arrives and needs to be
   * forwarded up the stack.
   *
   * \param packet The packet that was received.
   */
  void Receive (Ptr<Packet> packet);

  // From class NetDevice. Some of these have little meaning for a CUNB
  // network device (since, for instance, IP is not used in the standard)
  virtual void SetReceiveCallback (NetDevice::ReceiveCallback cb);
  virtual Ptr<Channel> GetChannel (void) const;
  virtual void SetNode (Ptr<Node> node);
  virtual Ptr<Node> GetNode (void) const;

  virtual void SetIfIndex (const uint32_t index);
  virtual uint32_t GetIfIndex (void) const;
  virtual void SetAddress (Address address);
  virtual Address GetAddress (void) const;
  virtual bool SetMtu (const uint16_t mtu);
  virtual uint16_t GetMtu (void) const;
  virtual bool IsLinkUp (void) const;
  virtual void AddLinkChangeCallback (Callback<void> callback);
  virtual bool IsBroadcast (void) const;
  bool IsBeacon(Ptr<Packet> packet);
  virtual Address GetBroadcast (void) const;
  virtual bool IsMulticast (void) const;
  virtual Address GetMulticast (Ipv4Address multicastGroup) const;
  virtual Address GetMulticast (Ipv6Address addr) const;
  virtual bool IsBridge (void) const;
  virtual bool IsPointToPoint (void) const;
  virtual bool Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);
  virtual bool SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber);
  virtual bool NeedsArp (void) const;
  virtual void SetPromiscReceiveCallback (PromiscReceiveCallback cb);
  virtual bool SupportsSendFrom (void) const;

protected:

  /**
   * Receive a packet from the lower layer and pass the
   * packet up the stack.
   *
   * \param packet The packet we need to forward.
   * \param from The from address.
   * \param to The to address.
   */
  void ForwardUp (Ptr<Packet> packet, Mac48Address from, Mac48Address to);

private:

  /**
   * Return the CunbChannel this device is connected to.
   */
  Ptr<CunbChannel> DoGetChannel (void) const;

  /**
   * Complete the configuration of this Cunb device by connecting all lower
   * components (PHY, MAC, Channel) together.
   */
  void CompleteConfig (void);

  // Member variables
  Ptr<Node> m_node; //!< The Node this NetDevice is connected to.
  Ptr<CunbPhy> m_phy; //!< The CunbPhy this NetDevice is connected to.
  Ptr<CunbMac> m_mac; //!< The CunbMac this NetDevice is connected to.
  bool m_configComplete; //!< Whether the configuration was already completed.
  /**
     * The MAC address which has been assigned to this device.
     */
  Mac48Address m_address;

  /**
   * Upper layer callback used for notification of new data packet arrivals.
   */
  NetDevice::ReceiveCallback m_receiveCallback;
};


}
# endif
