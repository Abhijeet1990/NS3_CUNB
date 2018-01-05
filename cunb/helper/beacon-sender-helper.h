#ifndef BEACON_SENDER_HELPER_H
#define BEACON_SENDER_HELPER_H

#include "ns3/object-factory.h"
#include "ns3/address.h"
#include "ns3/attribute.h"
#include "ns3/net-device.h"
#include "ns3/node-container.h"
#include "ns3/application-container.h"
#include "ns3/beacon-sender.h"
#include <stdint.h>
#include <string>

namespace ns3 {

/**
 * This class is used by the eNB to send beacon channels.
 */
class BeaconSenderHelper
{
public:
	BeaconSenderHelper ();

  ~BeaconSenderHelper ();

  void SetAttribute (std::string name, const AttributeValue &value);

  ApplicationContainer Install (NodeContainer c) const;

  ApplicationContainer Install (Ptr<Node> node) const;

  void SetSendTime (Time sendTime);

private:
  Ptr<Application> InstallPriv (Ptr<Node> node) const;

  ObjectFactory m_factory;

  Time m_sendTime; //!< Time at which the BeaconSender will be configured to
                   //send the beacon Packet
};

} // namespace ns3

#endif /* BEACON_SENDER_HELPER_H*/
