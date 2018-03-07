#ifndef MS_CUNB_MAC_H
#define MS_CUNB_MAC_H

#include "ns3/cunb-mac.h"
#include "ns3/cunb-mac-header-ul.h"
#include "ns3/cunb-linklayer-header.h"
#include "ns3/cunb-frame-header-ul.h"
#include "ns3/cunb-mac-trailer.h"
#include "ns3/cunb-mac-trailer-ul.h"
#include "ns3/cunb-mac-header.h"
#include "ns3/cunb-frame-header.h"
#include "ns3/random-variable-stream.h"
#include "ns3/cunb-device-address.h"
#include "ns3/traced-value.h"
#include "ns3/mac48-address.h"
#include "ns3/cunb-beacon-header.h"
#include "ns3/cunb-beacon-trailer.h"
#include "ns3/node-container.h"
#include "ns3/one-time-reporting.h"
#include "ns3/mobile-autonomous-reporting.h"
#include "ns3/hello-sender.h"
namespace ns3 {

/**
  * Class representing the MAC layer of a cunb device.
  */
class MSCunbMac : public CunbMac
{
public:

  static TypeId GetTypeId (void);

  MSCunbMac();
  virtual ~MSCunbMac();

  /////////////////////////////////
  // Sending / receiving methods //
  /////////////////////////////////

  /**
   * Send a packet.
   *
   * The MAC layer of the MS will take care of using the right parameters.
   *
   * \param packet the packet to send
   */
  virtual void Send (Ptr<Packet> packet);

  void SendRetransmitted(Ptr<Packet> packet, uint8_t repCount);

  void RetransmitHello(Ptr<Packet> packet, uint8_t repCount);

  /**
   * Receive a packet.
   *
   * This method is typically registered as a callback in the underlying PHY
   * layer so that it's called when a packet is going up the stack.
   *
   * \param packet the received packet.
   */
  virtual void Receive (Ptr<Packet const> packet);

  virtual void ReceiveRequest (Ptr<Packet const> packet);

  virtual void SendRequest (Ptr<Packet> packet , Ptr<Node> ms);
  /**
   * Perform the actions that are required after a packet send.
   *
   * This function handles opening of the first receive window.
   */
  void TxFinished (Ptr<const Packet> packet);

  /**
   * Perform operations needed to open the first receive window.
   */
  void OpenFirstReceiveWindow (uint8_t pType);

  /**
   * Perform operations needed to open the second receive window.
   */
  void OpenSecondReceiveWindow (void);

  /**
     * Perform operations needed to open the third receive window.
     */
  void OpenThirdReceiveWindow (void);

  /**
   * Perform operations needed to close the first receive window.
   */
  void CloseFirstReceiveWindow (void);

  /**
   * Perform operations needed to close the second receive window.
   */
  void CloseSecondReceiveWindow (void);

  /**
    * Perform operations needed to close the third receive window.
    */
  void CloseThirdReceiveWindow (void);

  // check the APDU type
  uint8_t CheckAPDUType(Ptr<Packet> packet);

  /////////////////////////
  // Getters and Setters //
  /////////////////////////

  void SetIdent(uint16_t ident);

  uint16_t GetIdent(void);
  /**
   * Set the data rate this end device will use when transmitting. For MS
   * , this value is assumed to be fixed, and can be modified via MAC
   * commands issued by the eNB.
   *
   * \param dataRate The dataRate to use when transmitting.
   */
  void SetDataRate (uint8_t dataRate);

  /**
   * Get the data rate this end device is set to use.
   *
   * \return The data rate this device uses when transmitting.
   */
  uint8_t GetDataRate (void);

  /**
   * Set the network address of this device.
   *
   * \param address The address to set.
   */
  void SetDeviceAddress (CunbDeviceAddress address);

  /**
   * Get the network address of this device.
   *
   * \return This device's address.
   */
  CunbDeviceAddress GetDeviceAddress (void);

  /**
   * Set the Data Rate to be used in the second receive window.
   *
   * \param dataRate The Data Rate.
   */
  void SetSecondReceiveWindowDataRate (uint8_t dataRate);

  /**
   * Get the Data Rate that will be used in the first receive window.
   *
   * \return The Data Rate
   */
  uint8_t GetFirstReceiveWindowDataRate (void);

  /**
   * Get the Data Rate that will be used in the second receive window.
   *
   * \return The Data Rate
   */
  uint8_t GetSecondReceiveWindowDataRate (void);

  /**
   * Set the frequency that will be used for the second receive window.
   *
   * \param frequencyMHz the Frequency.
   */
  void SetSecondReceiveWindowFrequency (double frequencyMHz);

  /**
   * Get the frequency that is used for the second receive window.
   *
   * @return The frequency, in MHz
   */
  double GetSecondReceiveWindowFrequency (void);

  /**
    * Set the Data Rate to be used in the thirdreceive window.
    *
    * \param dataRate The Data Rate.
    */
   void SetThirdReceiveWindowDataRate (uint8_t dataRate);

  /**
     * Get the Data Rate that will be used in the third receive window.
     *
     * \return The Data Rate
     */
    uint8_t GetThirdReceiveWindowDataRate (void);

    /**
     * Set the frequency that will be used for the third receive window.
     *
     * \param frequencyMHz the Frequency.
     */
    void SetThirdReceiveWindowFrequency (double frequencyMHz);

    /**
     * Get the frequency that is used for the third receive window.
     *
     * @return The frequency, in MHz
     */
    double GetThirdReceiveWindowFrequency (void);


  /**
   * Get the aggregated duty cycle.
   *
   * \return A time instance containing the aggregated duty cycle in fractional
   * form.
   */
  double GetAggregatedDutyCycle (void);

  /////////////////////////
  // MAC command methods //
  /////////////////////////

  /**
   * Add the necessary options and MAC commands to the CunbFrameHeader.
   *
   * \param frameHeader The frame header on which to apply the options.
   */
  void ApplyNecessaryOptions (CunbFrameHeaderUl &frameHeader);

  /**
   * Add the necessary options and MAC commands to the CunbMacHeader.
   *
   * \param macHeader The mac header on which to apply the options.
   */
  void ApplyNecessaryOptions (CunbMacHeaderUl &macHeader);

  /**
   * Set the message type to send when the Send method is called.
   */
  void SetMType (CunbMacHeaderUl::MType mType);

  void SetOneTimeReporting(Ptr<OneTimeReporting> otr);

  Ptr<OneTimeReporting> GetOneTimeReporting (void);

  void SetMobileAutonomousReporting(Ptr<MobileAutonomousReporting> otr);

  Ptr<MobileAutonomousReporting> GetMobileAutonomousReporting (void);

  void SetHelloSender(Ptr<HelloSender> hs);

  Ptr<HelloSender> GetHelloSender (void);


  ////////////////////////////////////
  // Logical channel administration //
  ////////////////////////////////////

  /**
   * Add a logical channel to the helper.
   *
   * \param frequency The channel's center frequency.
   */
  void AddLogicalChannel (double frequency);

  /**
   * Set a new logical channel in the helper.
   *
   * \param chIndex The channel's new index.
   * \param frequency The channel's center frequency.
   * \param minDataRate The minimum data rate allowed on the channel.
   * \param maxDataRate The maximum data rate allowed on the channel.
   */
  void SetLogicalChannel (uint8_t chIndex, double frequency,
                          uint8_t minDataRate, uint8_t maxDataRate);

  /**
   * Add a logical channel to the helper.
   *
   * \param frequency The channel's center frequency.
   */
  void AddLogicalChannel (Ptr<LogicalCunbChannel> logicalChannel);

  /**
   * Add a subband to the logical channel helper.
   *
   * \param startFrequency The SubBand's lowest frequency.
   * \param endFrequency The SubBand's highest frequency.
   * \param dutyCycle The SubBand's duty cycle, in fraction form.
   * \param maxTxPowerDbm The maximum transmission power allowed on the SubBand.
   */
  void AddSubBand (double startFrequency, double endFrequency,
                   double maxTxPowerDbm);

  void SetFrequencyToSend(double frequency);

  double GetFrequencyToSend(void);

  virtual void ReceiveBeacon(Ptr<Packet const> beaconPacket);
  virtual void SendBeacon(Ptr<Packet> beaconPacket);

  void ExtractBeaconInformation(Ptr<Packet const> beaconPacket);

private:

  /**
   * Randomly shuffle a Ptr<LogicalCunbChannel> vector.
   *
   * Used to pick a random channel on which to send the packet.
   */
  std::vector<Ptr<LogicalCunbChannel> > Shuffle
    (std::vector<Ptr<LogicalCunbChannel> > vector);

  /**
   * Find a suitable channel for transmission. The channel is chosen among the
   * ones that are available in the ED's LogicalCunbChannel, based on their duty
   * cycle limitations.
   */
  Ptr<LogicalCunbChannel> GetChannelForTx (void);

  /**
   * An uniform random variable, used by the Shuffle method to randomly reorder
   * the channel list.
   */
  Ptr<UniformRandomVariable> m_uniformRV;

  /**
   * The DataRate this device is using to transmit.
   */
  TracedValue<uint8_t> m_dataRate;

  /**
   * The transmission power this device is using to transmit.
   */
  TracedValue<double> m_txPower;

  /**
   * The interval between when a packet is done sending and when the first
   * receive window is opened.
   */
  Time m_receiveDelay1;

  /**
   * The interval between when a packet is done sending and when the second
   * receive window is opened.
   */
  Time m_receiveDelay2;

  /**
   * The interval between when a packet is done sending and when the third
   * receive window is opened.
   */
  Time m_receiveDelay3;

  /**
   * The duration of a receive window.
   */
  Time m_receiveWindowDuration;

  /**
   * The event of the closing of a receive window.
   *
   * This Event will be canceled if there's a successful reception of a packet.
   */
  EventId m_closeWindow;

  /**
   * The event of the second receive window opening.
   *
   * This Event is used to cancel the second window in case the first one is
   * successful.
   */
  EventId m_secondReceiveWindow;

  /**
     * The event of the third receive window opening.
     *
     * This Event is used to cancel the third window in case the second one is
     * successful.
     */
  EventId m_thirdReceiveWindow;

  EventId m_firstRetransmit;

  EventId m_secondRetransmit;


  /**
   * The address of this device.
   */
  CunbDeviceAddress m_address;

  /**
   * The frequency to listen on for the second receive window.
   */
  double m_secondReceiveWindowFrequency;

  /**
   * The Data Rate to listen for during the second downlink transmission.
   */
  uint8_t m_secondReceiveWindowDataRate;

  /**
   * The frequency to listen on for the third receive window.
   */
  double m_thirdReceiveWindowFrequency;

  /**
   * The Data Rate to listen for during the third downlink transmission.
   */
  uint8_t m_thirdReceiveWindowDataRate;

  /*
   * Implementation of Events for Re-sending Hello
   * if no AA-Request is received for the Smart Meter
   *
   * */

  EventId m_secondReceiveRequestWindow;
  EventId m_thirdReceiveRequestWindow;
  Time m_firstReceiveRequestWindowDuration;
  Time m_secondReceiveRequestWindowDuration;
  EventId m_firstHelloRetransmit;
  EventId m_secondHelloRetransmit;

  /**
   * The last known link margin.
   *
   * This value is obtained (and updated) when a LinkCheckAns Mac command is
   * received.
   */
  TracedValue<double> m_lastKnownLinkMargin;

  /**
   * The last known eNB count (i.e., eNB that are in communication
   * range with this end device)
   *
   * This value is obtained (and updated) when a LinkCheckAns Mac command is
   * received.
   */
  TracedValue<int> m_lastKnownEnbCount;

  /**
   * List of the MAC commands that need to be applied to the next UL packet.
   */
  std::list<Ptr<CunbMacCommand> > m_macCommandList;

  /**
   * The aggregated duty cycle this device needs to respect across all sub-bands.
   */
  TracedValue<double> m_aggregatedDutyCycle;

  TracedCallback<Ptr<const Packet>> m_sendAssociation;
  TracedCallback<Ptr<const Packet>> m_sendHello;
  TracedCallback<Ptr<const Packet>> m_reSendHello;
  TracedCallback<Ptr<const Packet>> m_reSendData;

  /**
   * The message type to apply to packets sent with the Send method.
   */
  CunbMacHeaderUl::MType m_mType;

  Ptr<OneTimeReporting> m_oneTimeReporting;

  Ptr<HelloSender> m_helloSender;

  Ptr<MobileAutonomousReporting> m_mobileAutonomousReporting;

  bool m_ifMARStarted;

  uint8_t m_seq_cnt;

  uint16_t m_ident;

  double m_freq_to_send; // This is used to reuse the same frequency with which the ACK has been received for the data packet for further transmission

};

} /* namespace ns3 */

#endif /* MS_CUNB_MAC_H */
