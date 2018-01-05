#ifndef OTR_HELPER_H
#define OTR_HELPER_H

#include "ns3/object-factory.h"
#include "ns3/address.h"
#include "ns3/attribute.h"
#include "ns3/net-device.h"
#include "ns3/node-container.h"
#include "ns3/application-container.h"
#include "ns3/one-time-reporting.h"
#include <stdint.h>
#include <string>

namespace ns3 {

/**
 * This class can be used to install OneTimeReporting applications on multiple
 * nodes at once.
 */
class OTRHelper
{
public:
	OTRHelper ();

  ~OTRHelper ();

  void SetAttribute (std::string name, const AttributeValue &value);

  ApplicationContainer Install (NodeContainer c) const;

  ApplicationContainer Install (Ptr<Node> node) const;

  void SetSendTime (Time sendTime);

private:
  Ptr<Application> InstallPriv (Ptr<Node> node) const;

  ObjectFactory m_factory;

  Time m_sendTime; //!< Time at which the OneShotSender will be configured to
                   //send the packet
};

} // namespace ns3

#endif /* OTR_HELPER_H*/
