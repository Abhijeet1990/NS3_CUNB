#ifndef CUNB_FRAME_HEADER_UL_H
#define CUNB_FRAME_HEADER_UL_H

#include "ns3/header.h"
#include "ns3/cunb-device-address.h"
#include "ns3/cunb-mac-command.h"

namespace ns3 {

/**
 * This class represents the Frame header (FHDR) used in a CUNB network.
 *
 * Although the specification divides the FHDR from the FPort field, this
 * implementation considers them as a unique entity (i.e., FPort is treated as
 * if it were a part of FHDR).
 *
 * \remark Prior to using it, this class needs to be informed of whether the
 * header is for an uplink or downlink message. This is necessary due to the
 * fact that UL and DL messages have subtly different structure and, hence,
 * serialization and deserialization schemes.
 */
class CunbFrameHeaderUl : public Header
{
public:

  CunbFrameHeaderUl ();
  ~CunbFrameHeaderUl ();

  // Methods inherited from Header
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  /**
   * Return the size required for serialization of this header
   *
   * \return The serialized size in bytes
   */
  virtual uint32_t GetSerializedSize (void) const;

  /**
   * Serialize the header.
   *
   * See Page 15 of LoraWAN specification for a representation of fields.
   *
   * \param start A pointer to the buffer that will be filled with the
   * serialization.
   */
  virtual void Serialize (Buffer::Iterator start) const;

  /**
   * Deserialize the contents of the buffer into a LoraFrameHeader object.
   *
   * \param start A pointer to the buffer we need to deserialize.
   * \return The number of consumed bytes.
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);

  /**
   * Print the header in a human-readable format.
   *
   * \param os The std::ostream on which to print the header.
   */
  virtual void Print (std::ostream &os) const;


  /**
   * Set the FPort value.
   *
   * \param fPort The FPort to set.
   */
  void SetFPort (uint8_t fPort);

  /**
   * Get the FPort value.
   *
   * \return The FPort value.
   */
  uint8_t GetFPort (void) const;

  /**
   * Set the address.
   *
   * \param address The LoraDeviceAddress to set.
   */
  void SetAddress (CunbDeviceAddress address);

  /**
   * Get this header's device address value.
   *
   * \return The address value stored in this header.
   */
  CunbDeviceAddress GetAddress (void) const;

private:

  uint8_t m_fPort;
  CunbDeviceAddress m_address;

};

}

#endif


