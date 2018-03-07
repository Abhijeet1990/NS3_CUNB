#ifndef CUNB_MAC_TRAILER_H
#define CUNB_MAC_TRAILER_H

#include <ns3/trailer.h>
#include "ns3/cunb-mac-header-ul.h"

namespace ns3 {

class Packet;

/**
 * \ingroup lr-wpan
 *
 * Represent the Mac Trailer with the Frame Check Sequence field.
 */
class CunbMacTrailer : public Trailer
{
public:
  /**
   * The length in octets of the IEEE 802.15.4 MAC FCS field
   */
  static const uint16_t CUNB_MAC_FCS_LENGTH;
  static const uint16_t CUNB_MAC_ECC_LENGTH;
  static const uint16_t CUNB_MAC_AUTH_LENGTH;

  /**
   * Get the type ID.
   *
   * \return the object TypeId
   */
  static TypeId GetTypeId (void);

  /**
   * Default constructor for a MAC trailer with disabled FCS calculation.
   */
  CunbMacTrailer (void);

  // Inherited from the Trailer class.
  virtual TypeId GetInstanceTypeId (void) const;
  virtual void Print (std::ostream &os) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

  /**
   * Get this trailers FCS value. If FCS calculation is disabled for this
   * trailer, the returned value is always 0.
   *
   * \return the FCS value.
   */
  uint16_t GetFcs (void) const;

  /**
   * Calculate and set the FCS value based on the given packet.
   *
   * \param p the packet for which the FCS should be calculated
   */
  void SetFcs (Ptr<const Packet> p);

  /**
   * Check the FCS of a given packet against the FCS value stored in the
   * trailer. The packet itself should contain no trailer. If FCS calculation is
   * disabled for this trailer, CheckFcs() will always return true.
   *
   * \param p the packet to be checked
   * \return false, if the FCS values do not match, true otherwise
   */
  bool CheckFcs (Ptr<const Packet> p);

  /**
   * Enable or disable FCS calculation for this trailer.
   *
   * \param enable flag, indicating if FCS calculation should be enabled or not
   */
  void EnableFcs (bool enable);

  /**
   * Query if FCS calculation is enabled for this trailer.
   *
   * \return true, if FCS calculation is enabled, false otherwise.
   */
  bool IsFcsEnabled (void);


  uint16_t GetAuth (void) const;


  void SetAuth (Ptr<const Packet> p);

  bool CheckAuth (Ptr<const Packet> p);

  void SetAuthDL (Ptr<const Packet> p, uint8_t seqCnt, uint16_t ident);

  bool CheckAuthDL (Ptr<const Packet> p, uint8_t seqCnt, uint16_t ident);

  CunbMacHeaderUl GetMacHeader(void);

  void SetMacHeader(CunbMacHeaderUl macHdr);



private:
  /**
   * Calculate the 16-bit FCS value.
   * CRC16-CCITT with a generator polynomial = ^16 + ^12 + ^5 + 1, LSB first and
   * initial value = 0x0000.
   *
   * \param data the checksum will be calculated over this data
   * \param length the length of the data
   * \return the checksum
   */
  uint8_t GenerateCrc8 (uint8_t *data, int length);

  uint16_t GenerateCrc16 (uint8_t *data, int length);

  uint16_t GenerateHash (uint8_t *data, int length);

  uint16_t GenerateHashWithSeqIdent(uint8_t *data, int length, uint16_t ident, uint8_t seq);

  /**
   * The FCS value stored in this trailer.
   */
  uint16_t m_auth;
  uint16_t m_fcs;
  uint16_t m_ecc;


  /**
   * Only if m_calcFcs is true, FCS values will be calculated and used in the
   * trailer
   */
  bool m_calcFcs;

  CunbMacHeaderUl m_macHdr;

};

} // namespace ns3

#endif /* LR_WPAN_MAC_TRAILER_H */
