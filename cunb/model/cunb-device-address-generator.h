#ifndef CUNB_DEVICE_ADDRESS_GENERATOR_H
#define CUNB_DEVICE_ADDRESS_GENERATOR_H

#include "ns3/cunb-device-address.h"
#include "ns3/object.h"

namespace ns3 {

/**
  * This class generates sequential CunbDeviceAddress instances.
  */
class CunbDeviceAddressGenerator : public Object
{
public:

  static TypeId GetTypeId (void);

  /**
   * Initialise the base NwkID and the first NwkAddr to be used by the
   * generator.
   *
   * The first call to NextAddress() or GetAddress() will return these values.
   *
   * \param nwkId The first network id.
   * \param nwkAddr The first address.
   */
  CunbDeviceAddressGenerator (const uint8_t nwkId = 0,
                              const uint32_t nwkAddr = 0);

  /**
   * Get the first address from the next network.
   *
   * This resets the address to the base address that was used for
   * initialization.
   *
   * \return the CunbDeviceAddress address of the next network
   */
  CunbDeviceAddress NextNetwork (void);

  /**
   * Allocate the next CunbDeviceAddress.
   *
   * This operation is a post-increment, meaning that the first address
   * allocated will be the one that was initially configured.
   *
   * This keeps the nwkId constant, only incrementing nwkAddr.
   *
   * \return the CunbDeviceAddress address
   */
  CunbDeviceAddress NextAddress (void);

  /**
   * Get the CunbDeviceAddress that will be allocated upon a call to
   * NextAddress.
   *
   * Does not change the internal state; is just used to peek at the next
   * address that will be allocated upon a call to NextAddress
   *
   * \return the CunbDeviceAddress
   */
  CunbDeviceAddress GetNextAddress (void);

private:

  NwkID m_currentNwkId; //!< The current Network Id value
  NwkAddr m_currentNwkAddr; //!< The current Network Address value
};
} //namespace ns3
#endif
