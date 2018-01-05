

#ifndef SUB_BAND_CUNB_H
#define SUB_BAND_CUNB_H

#include "ns3/object.h"
#include "ns3/logical-cunb-channel.h"
#include "ns3/nstime.h"

namespace ns3 {

class LogicalCunbChannel;

/**
 * Class representing a SubBand, i.e., a frequency band subject to some
 * regulations on duty cycle and transmission power.
 */
class SubBandCunb : public Object
{
public:

  static TypeId GetTypeId (void);

  SubBandCunb ();

  /**
   * Create a new SubBand by specifying all of its properties.
   *
   * \param firstFrequency The SubBand's lowest frequency.
   * \param lastFrequency The SubBand's highest frequency.
   * \param maxTxPowerDbm The maximum transmission power [dBm] allowed on this SubBand.
   */
  SubBandCunb (double firstFrequency, double lastFrequency,  double maxTxPowerDbm);

  virtual ~SubBandCunb ();

  /**
   * Get the lowest frequency of the SubBand.
   *
   * \return The lowest frequency of the SubBand.
   */
  double GetFirstFrequency (void);

  /**
   * Get the last frequency of the subband.
   *
   * \return The lowest frequency of the SubBand.
   */
  double GetLastFrequency (void);


  /**
   * Update the next transmission time.
   *
   * This function is used by LogicalLoraChannelHelper, which computes the time
   * based on the SubBand's duty cycle and on the transmission duration.
   *
   * \param nextTime The future time from which transmission should be allowed
   * again.
   */
  void SetNextTransmissionTime (Time nextTime);

  /**
   * Returns the next time from which transmission on this subband will be
   * possible.
   *
   * \return The next time at which transmission in this SubBand will be
   * allowed.
   */
  Time GetNextTransmissionTime (void);

  /**
   * Return whether or not a frequency belongs to this SubBand.
   *
   * \param frequency the frequency we want to test against the current subband
   * \return True if the frequency is between firstFrequency and lastFrequency,
   * false otherwise.
   */
  bool BelongsToSubBand (double frequency);

  /**
   * Return whether or not a channel belongs to this SubBand.
   *
   * \param channel the channel we want to test against the current subband
   * \return True if the channel's center frequency is between firstFrequency
   * and lastFrequency, false otherwise.
   */
  bool BelongsToSubBand (Ptr<LogicalCunbChannel> channel);

  /**
   * Set the maximum transmission power that is allowed on this SubBand.
   *
   * \param maxTxPowerDbm The maximum transmission power [dBm] to set.
   */
  void SetMaxTxPowerDbm (double maxTxPowerDbm);

  /**
   * Return the maximum transmission power that is allowed on this SubBand
   *
   * \return The maximum transmission power, in dBm.
   */
  double GetMaxTxPowerDbm (void);

private:

  double m_firstFrequency;   //!< Starting frequency of the subband, in MHz
  double m_lastFrequency;   //!< Ending frequency of the subband, in MHz
  Time m_nextTransmissionTime;   //!< The next time a transmission will be allowed in this subband
  double m_maxTxPowerDbm;   //!< The maximum transmission power that is admitted on this subband based on the base station
                            //beacon messages from the Base stat
};
} /* namespace ns3 */
#endif /* SUB_BAND_CUNB_H */
