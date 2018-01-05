#ifndef ENB_STATUS_H
#define ENB_STATUS_H

#include "ns3/object.h"
#include "ns3/address.h"
#include "ns3/net-device.h"
#include "ns3/enb-cunb-mac.h"

namespace ns3 {

class EnbStatus
{
public:

  EnbStatus ();
  virtual ~EnbStatus ();
  EnbStatus (Address address, Ptr<NetDevice> netDevice, Ptr<EnbCunbMac> gwMac);

  /**
   * Get this eNB's P2P link address.
   */
  Address GetAddress ();

  /**
   * Set this eNB's P2P link address.
   */
  void SetAddress (Address address);

  /**
   * Get the NetDevice through which it's possible to contact this eNB.
   */
  Ptr<NetDevice> GetNetDevice ();

  /**
   * Set the NetDevice through which it's possible to contact this eNB.
   */
  void SetNetDevice (Ptr<NetDevice> netDevice);

  /**
   * Get a pointer to this eNB's MAC instance.
   */
  Ptr<EnbCunbMac> GetEnbMac (void);

  /**
   * Set a pointer to this eNB's MAC instance.
   */
  void SetEnbMac (Ptr<EnbCunbMac> enbMac);

  /**
   * Query whether or not this eNB is available for immediate transmission
   * on this frequency.
   *
   * \param frequency The frequency at which the eNB's availability should
   * be queried.
   * \return True if the eNB's available, false otherwise.
   */
  bool IsAvailableForTransmission (double frequency);

  void SetNextTransmissionTime (Time nextTransmissionTime);
  Time GetNextTransmissionTime (void);

private:

  Address m_address; //!< The Address of the P2PNetDevice of this eNB

  Ptr<NetDevice> m_netDevice;   //!< The NetDevice through which to reach this eNB

  Ptr<EnbCunbMac> m_enbMac;   //!< The Mac layer of the eNB

  bool m_isTransmitting; //!< Whether this eNB is already booked for
                         //!transmission or not

  Time m_nextTransmissionTime; //!< This eNB's next transmission time
};
}

#endif /* ENB_CUNB_STATUS_H */
