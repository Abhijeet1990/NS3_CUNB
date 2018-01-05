#ifndef CUNB_MAC_HELPER_H
#define CUNB_MAC_HELPER_H

#include "ns3/net-device.h"
#include "ns3/cunb-channel.h"
#include "ns3/cunb-phy.h"
#include "ns3/cunb-mac.h"
#include "ns3/cunb-device-address-generator.h"
#include "ns3/ms-cunb-mac.h"
#include "ns3/enb-cunb-mac.h"
#include "ns3/node-container.h"

namespace ns3 {

class CunbMacHelper
{
public:

  /**
   * Define the kind of device. Can be either eNB (Base Station) or MS (Mobile Station).
   */
  enum DeviceType
  {
    ENB,
    MS
  };

  /**
   * Define the operational region.
   */
  enum Regions
  {
    EU,
    US,
    China,
    EU433MHz,
    Australia,
    CN,
    AS923MHz,
    SouthKorea
  };

  /**
   * Create a mac helper without any parameter set. The user must set
   * them all to be able to call Install later.
   */
  CunbMacHelper ();

  /**
   * Set an attribute of the underlying MAC object.
   *
   * \param name the name of the attribute to set.
   * \param v the value of the attribute.
   */
  void Set (std::string name, const AttributeValue &v);

  /**
   * Set the address generator to use for creation of these nodes.
   */
  void SetAddressGenerator (Ptr<CunbDeviceAddressGenerator> addrGen);

  /**
   * Set the kind of MAC this helper will create.
   *
   * \param dt the device type (either gateway or end device).
   */
  void SetDeviceType (enum DeviceType dt);

  /**
   * Set the region in which the device is to operate.
   */
  void SetRegion (enum Regions region);

  /**
   * Create the CunbMac instance and connect it to a device
   *
   * \param node the node on which we wish to create a wifi MAC.
   * \param device the device within which this MAC will be created.
   * \returns a newly-created LoraMac object.
   */
  Ptr<CunbMac> Create (Ptr<Node> node, Ptr<NetDevice> device) const;

  /**
   * Set up the mobile stations's data rates
   * This function assumes we are using the following convention:
   * SF7 -> DR5
   * SF8 -> DR4
   * SF9 -> DR3
   * SF10 -> DR2
   * SF11 -> DR1
   * SF12 -> DR0
   */
  static void SetSpreadingFactorsUp (NodeContainer mss,
                                     NodeContainer enbs,
                                     Ptr<CunbChannel> channel);

private:

  /**
   * Perform region-specific configurations for the 868 MHz EU band.
   */
  void ConfigureForEuRegion (Ptr<MSCunbMac> msMac) const;

  /**
   * Perform region-specific configurations for the 868 MHz EU band.
   */
  void ConfigureForEuRegion (Ptr<EnbCunbMac> enbMac) const;

  /**
   * Apply configurations that are common both for the EnbCunbMac and the
   * MSCunbMac classes.
   */
  void ApplyCommonEuConfigurations (Ptr<CunbMac> loraMac) const;

  ObjectFactory m_mac;
  Ptr<CunbDeviceAddressGenerator> m_addrGen; //!< Pointer to the address generator to use
  enum DeviceType m_deviceType; //!< The kind of device to install
  enum Regions m_region; //!< The region in which the device will operate
};

} //namespace ns3

#endif /* CUNB_MAC_HELPER_H */
