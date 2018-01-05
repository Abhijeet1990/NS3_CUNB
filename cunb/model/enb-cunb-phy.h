#ifndef ENB_CUNB_PHY_H
#define ENB_CUNB_PHY_H

#include "ns3/object.h"
#include "ns3/net-device.h"
#include "ns3/nstime.h"
#include "ns3/mobility-model.h"
#include "ns3/node.h"
#include "ns3/cunb-phy.h"
#include "ns3/traced-value.h"
#include <list>

namespace ns3 {

class CunbChannel;

/**
 * Class modeling a eNB Physical Layer
 *
 * This class models the behaviour of the hardware in eNBs. These
 * chips are characterized by the presence of 8 receive paths, or parallel
 * receivers, which can be employed to listen to different channels
 * simultaneously. This characteristic of the chip is modeled using the
 * ReceivePath class, which describes a single parallel receiver. EnbCunbPhy
 * essentially holds and manages a collection of these objects.
 */
class EnbCunbPhy : public CunbPhy
{
public:

  static TypeId GetTypeId (void);

  EnbCunbPhy();
  virtual ~EnbCunbPhy();

  virtual void StartReceive (Ptr<Packet> packet, double rxPowerDbm,
                             Time duration, double frequencyMHz);

  virtual void EndReceive (Ptr<Packet> packet,
                           Ptr<CunbInterferenceHelper::Event> event);

  virtual void Send (Ptr<Packet> packet, CunbTxParameters txParams,
                     double frequencyMHz, double txPowerDbm);

  virtual void TxFinished (Ptr<Packet> packet);
  //virtual void RxFinished (Ptr<Packet> packet);

  bool IsTransmitting (void);

  //bool IsReceiving(void);

  virtual bool IsOnFrequency (double frequencyMHz);

  /**
   * Add a reception path, locked on a specific frequency.
   *
   * \param frequencyMHz The frequency on which to set this ReceptionPath.
   */
  void AddReceptionPath (double frequencyMHz);

  /**
   * Reset the list of reception paths.
   *
   * This method deletes all currently available ReceptionPath objects.
   */
  void ResetReceptionPaths (void);

  /**
   * Receiver Sensitivity.
   */
  static const double sensitivity;

private:

  /**
   * This class represents a configurable reception path.
   *
   * ReceptionPaths are configured to listen on a certain frequency. Differently
   * from MSCunbPhys, these do not need to be configured to listen for a
   * certain Frequency. ReceptionPaths be either locked on an event or free.
   */
  class ReceptionPath : public SimpleRefCount<EnbCunbPhy::ReceptionPath>
  {

public:

    /**
     * Constructor.
     *
     * \param frequencyMHz The frequency this path is set to listen on.
     */
    ReceptionPath (double frequencyMHz);

    ~ReceptionPath();

    /**
     * Getter for the operating frequency.
     *
     * \return The frequency this ReceivePath is configured to listen on.
     */
    double GetFrequency (void);

    /**
     * Setter for the frequency.
     *
     * \param frequencyMHz The frequency [MHz] this ReceptionPath will listen on.
     */
    void SetFrequency (double frequencyMHz);

    /**
     * Query whether this reception path is available to lock on a signal.
     *
     * \return True if its current state is free, false if it's currently locked.
     */
    bool IsAvailable (void);

    /**
     * Set this reception path as available.
     *
     * This function sets the m_available variable as true, and deletes the
     * CunbInterferenceHelper Event this ReceivePath was previously locked on.
     */
    void Free (void);

    /**
     * Set this reception path as not available and lock it on the
     * provided event.
     *
     * \param event The CunbInterferenceHelper Event to lock on.
     */
    void LockOnEvent (Ptr<CunbInterferenceHelper::Event> event);

    /**
     * Set the event this reception path is currently on.
     *
     * \param event the event to lock this ReceptionPath on.
     */
    void SetEvent (Ptr<CunbInterferenceHelper::Event> event);

    /**
     * Get the event this reception path is currently on.
     *
     * \returns 0 if no event is currently being received, a pointer to
     * the event otherwise.
     */
    Ptr<CunbInterferenceHelper::Event> GetEvent (void);

private:

    /**
     * The frequency this path is currently listening on, in MHz.
     */
    double m_frequencyMHz;

    /**
     * Whether this reception path is available to lock on a signal or not.
     */
    bool m_available;

    /**
     * The event this reception path is currently locked on.
     */
    Ptr< CunbInterferenceHelper::Event > m_event;
  };

  /**
   * A list containing the various parallel receivers that are managed by this
   * eNB.
   */
  std::list<Ptr<ReceptionPath> > m_receptionPaths;

  /**
   * The number of occupied reception paths.
   */
  TracedValue<int> m_occupiedReceptionPaths;

  /**
   * Trace source that is fired when a packet cannot be received because all
   * available ReceivePath instances are busy.
   *
   * \see class CallBackTraceSource
   */
  TracedCallback<Ptr<const Packet>, uint32_t> m_noMoreDemodulators;

  bool m_isTransmitting; //!< Flag indicating whether a transmission is going on

  bool m_isReceiving; //!< Flag indicating whether a reception is going on
};

} /* namespace ns3 */

#endif /* ENB_CUNB_PHY_H */
