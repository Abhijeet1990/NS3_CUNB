#ifndef CUNB_LINKLAYER_HEADER_UL_H
#define CUNB_LINKLAYER_HEADER_UL_H

#include "ns3/header.h"
#include "ns3/cunb-device-address.h"

namespace ns3 {

/**
 * This class represents the Frame header (FHDR) used in a CUNB network.
 *
 * This layer would be used to transmit the segment no and size for the purpose of
 * segmentation and reassembly.
 */
class CunbLinkLayerHeaderUl : public Header
{
public:

  CunbLinkLayerHeaderUl ();
  ~CunbLinkLayerHeaderUl ();

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
   * See Cunb specification for a representation of fields.
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


private:
  uint8_t m_segno;
  uint8_t m_segcnt;
};

}

#endif


