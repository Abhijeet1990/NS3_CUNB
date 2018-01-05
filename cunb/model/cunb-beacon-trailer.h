#ifndef CUNB_BEACON_TRAILER_H
#define CUNB_BEACON_TRAILER_H

#include "ns3/trailer.h"

namespace ns3 {

/**
  * This class represents the Beacon Trailer of a CUNB packet. This is broadcasted by eNB to all the MS
  */
class CunbBeaconTrailer : public Trailer
{
public:


  static TypeId GetTypeId (void);

  CunbBeaconTrailer ();
  ~CunbBeaconTrailer ();

  // Pure virtual methods from Trailer that need to be implemented by this class
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;

  /**
   * Serialize the Trailer.
   *
   * \param start A pointer to the buffer that will be filled with the
   * serialization.
   */
  virtual void Serialize (Buffer::Iterator start) const;

  /**
   * Deserialize the Trailer.
   *
   * \param start A pointer to the buffer we need to deserialize.
   * \return The number of consumed bytes.
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);

  /**
   * Print the Trailer in a human readable format.
   *
   * \param os The std::ostream on which to print the Trailer.
   */
  virtual void Print (std::ostream &os) const;


private:

  /*
   * Beacon Trailer
   * a) 1 byte: FCS
   * b) 4 bytes : ECC
   */

  uint8_t m_fcs;
  uint32_t m_ecc;

};
}
#endif
