#ifndef MAR_HELPER_H
#define MAR_HELPER_H

#include "ns3/object-factory.h"
#include "ns3/address.h"
#include "ns3/attribute.h"
#include "ns3/net-device.h"
#include "ns3/node-container.h"
#include "ns3/application-container.h"
#include "ns3/mobile-autonomous-reporting.h"
#include <stdint.h>
#include <string>

namespace ns3 {


/**
 * This class can be used to install PeriodicSender applications on a wide
 * range of nodes.
 */

class MARHelper
{
public:


enum packetType{
		KeepAlive,
		Normal,
		Alarm
	};

  MARHelper ();

  ~MARHelper ();

  void SetAttribute (std::string name, const AttributeValue &value);

  ApplicationContainer Install (NodeContainer c, enum packetType pType) const;

  ApplicationContainer Install (Ptr<Node> node, enum packetType pType) const;

  /**
   * Set the period to be used by the applications created by this helper.
   *
   * A value of Seconds (0) results in randomly generated periods according to
   * the model contained in the TR 45.820 document.
   *
   * \param period The period to set
   */
  void SetPeriod (Time period);

private:
  Ptr<Application> InstallPriv (Ptr<Node> node, enum packetType pType) const;

  ObjectFactory m_factory;

  double m_packetSize;

  Ptr<UniformRandomVariable> m_initialDelay;

  Ptr<UniformRandomVariable> m_intervalProb;

  Time m_period; //!< The period with which the application will be set to send
                 // messages
  Ptr<ExponentialRandomVariable> m_expInterval;


};

} // namespace ns3

#endif /* MAR_HELPER_H */
