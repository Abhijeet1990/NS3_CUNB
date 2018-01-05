#ifndef ENB_CUNB_MAC_H
#define ENB_CUNB_MAC_H

#include "ns3/cunb-mac.h"
#include "ns3/cunb-tag.h"
#include "ns3/cunb-device-address.h"
#include "ns3/cunb-beacon-header.h"
#include "ns3/cunb-beacon-trailer.h"


namespace ns3 {

class EnbCunbMac : public CunbMac
{
public:

  static TypeId GetTypeId (void);

  EnbCunbMac();
  virtual ~EnbCunbMac();

  // Implementation of the CunbMac interface
  virtual void Send (Ptr<Packet> packet);

  // Implementation of the CunbMac interface
  bool IsTransmitting (void);

  // Implementation of the CunbMac interface
  virtual void Receive (Ptr<Packet const> packet);

  // Implementation of the CunbMac interface
  virtual void TxFinished (Ptr<Packet const> packet);

  /**
   * Return the next time at which we will be able to transmit.
   *
   * \return The next transmission time.
   */
  Time GetWaitingTime (double frequency);

  virtual void SendBeacon(Ptr<Packet> packet);
  virtual void ReceiveBeacon(Ptr<Packet const> packet);

  void ScheduleNextBeacon();

  bool GetBeaconGeneration () const;
  /**
    * Enable/disable beacons
    *
    * \param enable enable / disable flag
    */
  void SetBeaconGeneration (bool enable);

  /// Set maximum initial random delay before first beacon
  void SetRandomStartDelay (Time interval);
    /// Set interval between two successive beacons
  void SetBeaconInterval (Time interval);
    /// \return interval between two beacons
  Time GetBeaconInterval () const;
    /**
     * \brief Next beacon frame time
     * \return TBTT time
     *
     * This is supposed to be used by any entity managing beacon collision avoidance (e.g. Peer management protocol in 802.11s)
     */
  Time GetTbtt () const;
    /**
     * \brief Shift TBTT.
     * \param shift
     *
     * This is supposed to be used by any entity managing beacon collision avoidance (e.g. Peer management protocol in 802.11s)
     *
     * \attention User of ShiftTbtt () must take care to not shift it to the past.
     */
  void ShiftTbtt (Time shift);
    // \}

private:

  ///\name Mesh timing intervals
  // \{
  /// whether beaconing is enabled
  bool m_beaconEnable;
  /// Beaconing interval.
  Time m_beaconInterval;
  /// Maximum delay before first beacon
  Time m_randomStart;
  /// Time for the next frame
  Time m_tbtt;
  // \}

  /// Mesh point address
  Mac48Address m_mpAddress;

  /// "Timer" for the next beacon
  EventId m_beaconSendEvent;

protected:

};

} /* namespace ns3 */

#endif /* ENB_CUNB_MAC_H */
