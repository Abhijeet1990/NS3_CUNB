#ifndef CUNB_FORWARDER_HELPER_H
#define CUNB_FORWARDER_HELPER_H

#include "ns3/object-factory.h"
#include "ns3/address.h"
#include "ns3/attribute.h"
#include "ns3/net-device.h"
#include "ns3/node-container.h"
#include "ns3/application-container.h"
#include "ns3/cunb-forwarder.h"
#include <stdint.h>
#include <string>

namespace ns3 {

/**
 * This class can be used to install Forwarder applications on a set of
 * eNBs.
 */
class CunbForwarderHelper
{
public:
  CunbForwarderHelper ();

  ~CunbForwarderHelper ();

  void SetAttribute (std::string name, const AttributeValue &value);

  ApplicationContainer Install (NodeContainer c) const;

  ApplicationContainer Install (Ptr<Node> node) const;

private:
  Ptr<Application> InstallPriv (Ptr<Node> node) const;

  ObjectFactory m_factory;
};

} // namespace ns3

#endif /* CUNB_FORWARDER_HELPER_H */
