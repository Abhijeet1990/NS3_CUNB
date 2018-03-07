#ifndef MS_CUNB_PHY_H
#define MS_CUNB_PHY_H

#include "ns3/object.h"
#include "ns3/traced-value.h"
#include "ns3/net-device.h"
#include "ns3/nstime.h"
#include "ns3/mobility-model.h"
#include "ns3/node.h"
#include "ns3/cunb-phy.h"
#include "ns3/cunb-beacon-header.h"

namespace ns3 {

class CunbChannel;

/**
 * Class representing a CUNB MS transceiver.
 *
 * The behaviors featured in this class include a State member variable
 * that expresses the current state of the device (SLEEP, TX, RX or STANDBY),
 * and the frequency this device is listening to when in
 * STANDBY mode. After transmission and reception, the device returns
 * automatically to STANDBY mode. The decision of when to go into SLEEP mode
 * is delegateed to an upper layer, which can modify the state of the device
 * through the public SwitchToSleep and SwitchToStandby methods. In SLEEP
 * mode, the device cannot lock on a packet and start reception.
 */
class MSCunbPhy : public CunbPhy
{
public:
  /**
   * An enumeration of the possible states of an MSCunbPhy.
   * It makes sense to define a state for End Devices since there's only one
   * demodulator which can either send, receive, stay idle or go in a deep
   * sleep state.
   */
  enum State
  {

    /**
     * The PHY layer is in STANDBY.
     * When the PHY is in this state, it's listening to the channel, and
     * it's also ready to transmit data passed to it by the MAC layer.
     */
    STANDBY,

    /**
     * The PHY layer is sending a packet.
     * During transmission, the device cannot receive any packet or send
     * any additional packet.
     */
    TX,

    /**
     * The PHY layer is receiving a packet.
     * While the device is locked on an incoming packet, transmission is
     * not possible.
     */
    RX
  };

  static TypeId GetTypeId (void);

  // Constructor and destructor
  MSCunbPhy ();
  virtual ~MSCunbPhy ();

  // Implementation of CunbPhy's pure virtual functions
  virtual void StartReceive (Ptr<Packet> packet, double rxPowerDbm,
                             Time duration, double frequencyMHz);

  // Implementation of CunbPhy's pure virtual functions
  virtual void EndReceive (Ptr<Packet> packet,
                           Ptr<CunbInterferenceHelper::Event> event);

  // Implementation of CunbPhy's pure virtual functions
  virtual void Send (Ptr<Packet> packet, CunbTxParameters txParams,
                     double frequencyMHz, double txPowerDbm);

  // check the APDU type
  bool checkIfAck(Ptr<Packet> packet);

  // Implementation of CunbPhy's pure virtual functions
  virtual bool IsOnFrequency (double frequencyMHz);

  // Implementation of CunbPhy's pure virtual functions
  bool IsOnBeaconFrequency (double frequencyMHz);

  // Implementation of CunbPhy's pure virtual functions
  virtual bool IsTransmitting (void);

  virtual bool IsReceiving(void);

  /**
   * Set the frequency this MS will listen on.
   *
   * Should a packet be transmitted on a frequency different than that the
   * MSCunbPhy is listening on, the packet will be discarded.
   *
   * \param The frequency [MHz] to listen to.
   */
  void SetFrequency (double frequencyMHz);

  /**
     * Set the beacon frequency any MS will listen on.
     *
     * \param The frequency [MHz] to listen to.
     */
    void SetBeaconFrequency (double frequencyMHz);

  /**
   * Return the state this MS is currently in.
   *
   * \return The state this MSCunbPhy is currently in.
   */
  MSCunbPhy::State GetState (void);

  /**
   * Switch to the STANDBY state.
   */
  void SwitchToStandby (void);

  /**
   * Switch to the SLEEP state.
   */
  //void SwitchToSleep (void);

private:
  /**
   * Switch to the RX state
   */
  void SwitchToRx (void);

  /**
   * Switch to the TX state
   */
  void SwitchToTx (void);


  /**
   * Trace source for when a packet is lost because it was transmitted on a
   * frequency different from the one this MSCunbPhy was configured to
   * listen on.
   */
  TracedCallback<Ptr<const Packet>, uint32_t> m_wrongFrequency;

  TracedCallback<Ptr<const Packet>> m_phyRxBeginTrace;

  TracedValue<State> m_state; //!< The state this PHY is currently in.

  static const double sensitivity; //!< The sensitivity vector of this device

  double m_frequency; //!< The frequency this device is listening on

  double m_beacon_frequency;
};

} /* namespace ns3 */

#endif /* MS_CUNB_PHY_H */
