#ifndef CUNB_PHY_HELPER_H
#define CUNB_PHY_HELPER_H

#include "ns3/object-factory.h"
#include "ns3/net-device.h"
#include "ns3/cunb-channel.h"
#include "ns3/cunb-phy.h"
#include "ns3/ms-cunb-phy.h"
#include "ns3/enb-cunb-phy.h"
#include "ns3/cunb-mac.h"

namespace ns3 {

/**
  * Helper to install CunbPhy instances on multiple Nodes.
  */
class CunbPhyHelper
{
public:

  /**
   * Enum for the type of device: Mobile Station (MS) or Base Station(eNB)
   */
  enum DeviceType
  {
    ENB,
    MS
  };

  /**
   * Create a phy helper without any parameter set. The user must set
   * them all to be able to call Install later.
   */
  CunbPhyHelper ();

  /**
   * Set the CunbChannel to connect the PHYs to.
   *
   * Every PHY created by a call to Install is associated to this channel.
   *
   * \param channel the channel to associate to this helper.
   */
  void SetChannel (Ptr<CunbChannel> channel);

  /**
   * Set the kind of PHY this helper will create.
   *
   * \param dt the device type.
   */
  void SetDeviceType (enum DeviceType dt);

  /**
   * Set an attribute of the underlying PHY object.
   *
   * \param name the name of the attribute to set.
   * \param v the value of the attribute.
   */
  void Set (std::string name, const AttributeValue &v);

  /**
   * Crate a CunbPhy and connect it to a device on a node.
   *
   * \param node the node on which we wish to create a wifi PHY.
   * \param device the device within which this PHY will be created.
   * \return a newly-created PHY object.
   */
  Ptr<CunbPhy> Create (Ptr<Node> node, Ptr<NetDevice> device) const;

private:

  /**
   * The PHY layer factory object.
   */
  ObjectFactory m_phy;

  /**
   * The channel instance the PHYs will be connected to.
   */
  Ptr<CunbChannel> m_channel;
};

} //namespace ns3

#endif /* CUNB_PHY_HELPER_H */
