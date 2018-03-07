#ifndef OTRe_HELPER_H
#define OTRe_HELPER_H

#include "ns3/object-factory.h"
#include "ns3/address.h"
#include "ns3/attribute.h"
#include "ns3/net-device.h"
#include "ns3/node-container.h"
#include "ns3/application-container.h"
#include "ns3/one-time-requesting.h"
#include <stdint.h>
#include <string>
#include "ns3/enb-cunb-mac.h"

namespace ns3 {

/**
 * This class can be used to install OneTimeRequesting applications on multiple
 * eNBs at once.
 */
class OTReHelper
{
public:
	OTReHelper ();

  ~OTReHelper ();

  void SetAttribute (std::string name, const AttributeValue &value);

  ApplicationContainer Install (NodeContainer c) const;

  ApplicationContainer Install (Ptr<Node> node) const;

  void SetSendTime (Time sendTime);

  void SetMS(Ptr<Node> ms);

  Ptr<Node> GetMS(void);

  void SetPtype(uint8_t pType);

  uint8_t GetPtype(void);

  void SetMac(Ptr<EnbCunbMac> mac);


private:
  Ptr<Application> InstallPriv (Ptr<Node> node) const;

  ObjectFactory m_factory;

  Time m_sendTime; //!< Time at which the OneShotRequester will be configured to
                   //send the packet

  Ptr<Node> m_ms;

  uint8_t m_ptype;// whether an AARQ or GETN_RQ

  Ptr<EnbCunbMac> m_mac;
};

} // namespace ns3

#endif /* OTRe_HELPER_H*/
