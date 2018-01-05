#ifndef CUNB_TAG_H
#define CUNB_TAG_H

#include "ns3/tag.h"

namespace ns3 {

/**
 * Tag used to save various data about a packet, like
 * data about interference.
 */
class CunbTag : public Tag
{
public:
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;

  /**
   * Create a CunbTag with a given spreading factor and collision.
   * \param destroyedBy The SF this tag's packet was destroyed by.
   */
  CunbTag ( uint8_t destroyedBy = 0);

  virtual ~CunbTag ();

  virtual void Serialize (TagBuffer i) const;
  virtual void Deserialize (TagBuffer i);
  virtual uint32_t GetSerializedSize () const;
  virtual void Print (std::ostream &os) const;


  /**
   * Read which frequency caused this packet was destroyed by.
   *
   * \return The SF this packet was destroyed by.
   */
  uint8_t GetDestroyedBy () const;

  /**
   * Read the power this packet arrived with.
   *
   * \return This tag's packet received power.
   */
  double GetReceivePower () const;

  /**
   * Set which Spreading Factor this packet was destroyed by.
   *
   * \param sf The Spreading Factor.
   */
  void SetDestroyedBy (uint8_t sf);

  /**
   * Set the power this packet was received with.
   *
   * \param receivePower The power, in dBm.
   */
  void SetReceivePower (double receivePower);

  /**
   * Set the frequency of the packet.
   *
   * This value works in two ways:
   * - It is used by the eNB to signal to the CUNB server the frequency of the uplink
       packet
   * - It is used by the CUNB server to signal to the eNB the freqeuncy of a downlink
       packet
   */
  void SetFrequency (double frequency);

  /**
   * Get the frequency of the packet.
   */
  double GetFrequency (void);

  /**
   * Get the data rate for this packet.
   *
   * \return The data rate that needs to be employed for this packet.
   */
  uint8_t GetDataRate (void);

  /**
   * Set the data rate for this packet.
   *
   * \param dataRate The data rate.
   */
  void SetDataRate (uint8_t dataRate);

private:
  uint8_t m_destroyedBy; //!< The frequency that destroyed the packet.
  double m_receivePower; //!< The reception power of this packet.
  uint8_t m_dataRate; //!< The Data Rate that needs to be used to send this
                      //!packet.
  double m_frequency; //!< The frequency of this packet
};
} // namespace ns3
#endif
