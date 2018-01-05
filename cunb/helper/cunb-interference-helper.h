#ifndef CUNB_INTERFERENCE_HELPER_H
#define CUNB_INTERFERENCE_HELPER_H

#include "ns3/nstime.h"
#include "ns3/simulator.h"
#include "ns3/object.h"
#include "ns3/traced-callback.h"
#include "ns3/callback.h"
#include "ns3/packet.h"
#include "ns3/logical-cunb-channel.h"
#include <list>

namespace ns3 {

/**
  * Helper for CunbPhy that manages interference calculations
  * This class keeps a list of signals that are impinging on the antenna of the
  * device, in order to compute which ones can be correctly received and which
  * ones are lost due to interference.
  */
class CunbInterferenceHelper
{
public:
  /**
   * A class representing a signal in time.
   *
   * Used in CunbInterferenceHelper to keep track of which signals overlap and
   * cause destructive interference.
   */
  class Event : public SimpleRefCount<CunbInterferenceHelper::Event>
  {

public:

    Event (Time duration, double rxPowerdBm,
           Ptr<Packet> packet, double frequencyMHz);
    ~Event ();

    /**
     * Get the duration of the event.
     */
    Time GetDuration (void) const;

    /**
     * Get the starting time of the event.
     */
    Time GetStartTime (void) const;

    /**
     * Get the ending time of the event.
     */
    Time GetEndTime (void) const;

    /**
     * Get the power of the event.
     */
    double GetRxPowerdBm (void) const;

    /**
     * Get the packet this event was generated for.
     */
    Ptr<Packet> GetPacket (void) const;

    /**
     * Get the frequency this event was on.
     */
    double GetFrequency (void) const;

    /**
     * Print the current event in a human readable form.
     */
    void Print (std::ostream &stream) const;

private:

    /**
     * The time this signal begins (at the device).
     */
    Time m_startTime;

    /**
     * The time this signal ends (at the device).
     */
    Time m_endTime;


    /**
     * The power of this event in dBm (at the device).
     */
    double m_rxPowerdBm;

    /**
     * The packet this event was generated for.
     */
    Ptr<Packet> m_packet;

    /**
     * The frequency this event was on.
     */
    double m_frequencyMHz;

  };

  static TypeId GetTypeId (void);

  CunbInterferenceHelper();
  virtual ~CunbInterferenceHelper();

  /**
   * Add an event to the InterferenceHelper
   *
   * \param duration the duration of the packet.
   * \param rxPower the received power in dBm.
   * \param packet The packet carried by this transmission.
   * \param frequencyMHz The frequency this event was sent at.
   *
   * \return the newly created event
   */
  Ptr<CunbInterferenceHelper::Event> Add (Time duration, double rxPower,
                                          Ptr<Packet> packet,
                                          double frequencyMHz);

  /**
   * Get a list of the interferers currently registered at this
   * InterferenceHelper.
   */
  std::list< Ptr< CunbInterferenceHelper::Event > > GetInterferers ();

  /**
   * Print the events that are saved in this helper in a human readable format.
   */
  void PrintEvents (std::ostream &stream);

  /**
   * Determine whether the event was destroyed by interference or not. This is
   * the method where the SNIR tables come into play and the computations
   * regarding power are performed.

   * \param event The event for which to check the outcome.
   * \return 1 if the packets caused the loss, or 0 if there was no
   * loss.
   */
  bool IsDestroyedByInterference (Ptr<CunbInterferenceHelper::Event>
                                     event);

  /**
   * Compute the time duration in which two given events are overlapping.
   *
   * \param event1 The first event
   * \param event2 The second event
   *
   * \return The overlap time
   */
  Time GetOverlapTime (Ptr< CunbInterferenceHelper:: Event> event1,
                       Ptr<CunbInterferenceHelper:: Event> event2);

  /**
   * Delete all events in the CunbInterferenceHelper.
   */
  void ClearAllEvents (void);

  /**
   * Delete old events in this CunbInterferenceHelper.
   */
  void CleanOldEvents (void);

private:

  /**
   * A list of the events this CunbInterferenceHelper is keeping track of.
   */
  std::list< Ptr< CunbInterferenceHelper::Event > > m_events;

  /**
   * The information about how packets survive interference.
   */
  static const double collisionSnir;

  /**
   * The threshold after which an event is considered old and removed from the
   * list.
   */
  static Time oldEventThreshold;

};

/**
 * Allow easy logging of CunbInterferenceHelper Events
 */
std::ostream &operator << (std::ostream &os, const
                           CunbInterferenceHelper::Event &event);
}

#endif /* CUNB_INTERFERENCE_HELPER_H */
