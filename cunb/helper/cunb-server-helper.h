#ifndef CUNB_SERVER_HELPER_H
#define CUNB_SERVER_HELPER_H

#include "ns3/object-factory.h"
#include "ns3/address.h"
#include "ns3/attribute.h"
#include "ns3/net-device.h"
#include "ns3/node-container.h"
#include "ns3/application-container.h"
#include "ns3/point-to-point-helper.h"
#include <stdint.h>
#include <string>

namespace ns3 {

/**
 * This class can install Cunb Server applications on multiple nodes at once.
 */
class CunbServerHelper
{
public:
  CunbServerHelper ();

  ~CunbServerHelper ();

  void SetAttribute (std::string name, const AttributeValue &value);

  ApplicationContainer Install (NodeContainer c);

  ApplicationContainer Install (Ptr<Node> node);

  /**
   * Set which eNBs will need to be connected to this CUNB Server.
   */
  void SetEnbs (NodeContainer enbs);

  /**
   * Set which MSs will be managed by this CUNB Server.
   */
  void SetMSs (NodeContainer endDevices);

private:
  Ptr<Application> InstallPriv (Ptr<Node> node);

  ObjectFactory m_factory;

  NodeContainer m_enbs;   //!< Set of eNBs to connect to this CUNB server

  NodeContainer m_mss;   //!< Set of MSs to connect to this CUNB server

  PointToPointHelper p2pHelper; //!< Helper to create PointToPoint links
};

} // namespace ns3

#endif /* CUNB_SERVER_HELPER_H */
