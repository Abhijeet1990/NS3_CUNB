#ifndef LOGICAL_CUNB_CHANNEL_H
#define LOGICAL_CUNB_CHANNEL_H

#include "ns3/object.h"
#include "ns3/sub-band.h"

namespace ns3 {

class SubBand;

/**
 * This class represents a logical CUNB channel.
 *
 * A logical channel is characterized by a central frequency and a range of data
 * rates that can be sent on it.
 *
 * Furthermore, a LogicalCunbChannel can be marked as enabled or disabled for
 * uplink transmission.
 */
class LogicalCunbChannel : public Object
{
public:

  static TypeId GetTypeId (void);

  LogicalCunbChannel();
  virtual ~LogicalCunbChannel();

  LogicalCunbChannel (double frequency);

  /**
  * Constructor providing initialization of frequency and data rate limits.
  *
  * \param frequency This channel's frequency.
  * \param minDataRate This channel's minimum data rate.
  * \param maxDataRate This channel's maximum data rate.
  */
  LogicalCunbChannel (double frequency, uint8_t minDataRate,
                      uint8_t maxDataRate);

  /**
   * Get the frequency (MHz).
   *
   * \return The center frequency of this channel.
   */
  double GetFrequency (void) const;

  /**
   * Set the frequency (MHz).
   *
   * \param frequencyMHz The center frequency this channel should be at.
   */
  void SetFrequency (double frequencyMHz);


  /**
    * Get the frequency (MHz).
    *
    * \return The center frequency of this channel.
    */
   double GetBeaconFrequency (void) const;

   /**
    * Set the frequency (MHz).
    *
    * \param frequencyMHz The center frequency this channel should be at.
    */
   void SetBeaconFrequency (double frequencyMHz);

  /**
   * Set the minimum Data Rate that is allowed on this channel.
   */
  void SetMinimumDataRate (uint8_t minDataRate);

  /**
   * Set the maximum Data Rate that is allowed on this channel.
   */
  void SetMaximumDataRate (uint8_t maxDataRate);

  /**
   * Get the minimum Data Rate that is allowed on this channel.
   */
  uint8_t GetMinimumDataRate (void);

  /**
   * Get the maximum Data Rate that is allowed on this channel.
   */
  uint8_t GetMaximumDataRate (void);

  /**
   * Set this channel as enabled for uplink.
   */
  void SetEnabledForUplink (void);

  /**
   * Set this channel as disabled for uplink.
   */
  void DisableForUplink (void);

  /**
   * Test whether this channel is marked as enabled for uplink.
   */
  bool IsEnabledForUplink (void);

private:
  /**
   * The central frequency of this channel, in MHz.
   */
  double m_frequency;

  /*
   * The frequency used for the beacon signals
   */

  double m_beacon_frequency;

  /**
   * The minimum Data Rate that is allowed on this channel.
   */
  uint8_t m_minDataRate;

  /**
   * The maximum Data Rate that is allowed on this channel.
   */
  uint8_t m_maxDataRate;

  /**
   * Whether this channel can be used for uplink or not.
   */
  bool m_enabledForUplink;
};

/**
 * Overload of the == operator to compare different instances of the same LogicalCunbChannel
 */
bool operator== (const Ptr<LogicalCunbChannel>& first, const Ptr<LogicalCunbChannel>& second);

/**
 * Overload the != operator to compare different instances of the same LogicalCunbChannel
 */
bool operator!= (const Ptr<LogicalCunbChannel>& first, const Ptr<LogicalCunbChannel>& second);

}

#endif /* LOGICAL_CUNB_CHANNEL_H */
