#ifndef APP_LAYER_HEADER_H
#define APP_LAYER_H

#include "ns3/header.h"


namespace ns3 {

/**
 * This class represents the Application Header used in a CUNB network.
 * This header helps us to distinguish the type of packets. Whether it is an household or commercial packet.
 * It also defines the type of packet based on : KeepAlive, Normal or Alarm packets
 *
 */
class AppLayerHeader : public Header
{
public:

  AppLayerHeader ();
  ~AppLayerHeader ();

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
   * Deserialize the contents of the buffer into a AppLayerHeader object.
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

  void SetPtype (uint16_t ptype);

  uint16_t GetPtype (void) const;


private:

  uint16_t m_pType;
};

}

#endif
