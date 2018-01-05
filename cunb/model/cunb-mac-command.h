#ifndef CUNB_MAC_COMMAND_H
#define CUNB_MAC_COMMAND_H

#include "ns3/object.h"
#include "ns3/nstime.h"
#include "ns3/buffer.h"

namespace ns3 {

/**
  * Enum for every possible command type
  */
enum MacCommandType {
  INVALID,
  LINK_CHECK_REQ,
  LINK_CHECK_ANS,
  LINK_ADR_REQ,
  LINK_ADR_ANS,
  DUTY_CYCLE_REQ,
  DUTY_CYCLE_ANS,
  RX_PARAM_SETUP_REQ,
  RX_PARAM_SETUP_ANS,
  DEV_STATUS_REQ,
  DEV_STATUS_ANS,
  NEW_CHANNEL_REQ,
  NEW_CHANNEL_ANS,
  RX_TIMING_SETUP_REQ,
  RX_TIMING_SETUP_ANS,
  TX_PARAM_SETUP_REQ,
  TX_PARAM_SETUP_ANS,
  DL_CHANNEL_REQ,
  DL_CHANNEL_ANS
};

/**
 * This base class is used to represent a general MAC command.
 *
 * Pure virtual methods that handle serialization, deserialization and other
 * common features are supposed to be defined in detail by child classes, based
 * on that MAC command's attributes and structure.
 */
class CunbMacCommand : public Object
{
public:

  static TypeId GetTypeId (void);

  CunbMacCommand();
  virtual ~CunbMacCommand();

  /**
   * Serialize the contents of this MAC command into a buffer, according to the
   * CUNB standard.
   *
   * \param start A pointer to the buffer into which to serialize the command.
   */
  virtual void Serialize (Buffer::Iterator &start) const = 0;

  /**
   * Deserialize the buffer into a MAC command.
   *
   * \param start A pointer to the buffer that contains the serialized command.
   * \return the number of bytes that were consumed.
   */
  virtual uint8_t Deserialize (Buffer::Iterator &start) = 0;

  /**
   * Print the contents of this MAC command in human-readable format.
   *
   * \param os The std::ostream instance on which to print the MAC command.
   */
  virtual void Print (std::ostream &os) const = 0;

  /**
   * Get serialized length of this MAC command.
   *
   * \return The number of bytes the MAC command takes up.
   */
  virtual uint8_t GetSerializedSize (void) const;

  /**
   * Get the commandType of this MAC command.
   *
   * \return The type of MAC command this object represents.
   */
  virtual enum MacCommandType GetCommandType (void) const;

  /**
   * Get the CID that corresponds to this MAC command.
   *
   * \return The CID as a uint8_t type.
   */
  static uint8_t GetCIDFromMacCommand (enum MacCommandType commandType);

protected:

  /**
   * The type of this command.
   */
  enum MacCommandType m_commandType;

  /**
   * This MAC command's serialized size.
   */
  uint8_t m_serializedSize;

};

/**
  * Implementation of the LinkCheckReq Cunb MAC command.
  *
  * This command holds no variables, and just consists in the CID.
  */
class LinkCheckReq : public CunbMacCommand
{
public:

  LinkCheckReq ();
  ~LinkCheckReq ();
  virtual void Serialize (Buffer::Iterator &start) const;
  virtual uint8_t Deserialize (Buffer::Iterator &start);
  virtual void Print (std::ostream &os) const;
};

/**
  * Implementation of the LinkCheckAns Cunb MAC command.
  *
  * This command contains the demodulation margin and the number of receiving
  * eNBs of the packet containing the LinkCheckReq command.
  */
class LinkCheckAns : public CunbMacCommand
{
public:

  LinkCheckAns ();
  LinkCheckAns (uint8_t margin, uint8_t gwCnt);

  virtual void Serialize (Buffer::Iterator &start) const;
  virtual uint8_t Deserialize (Buffer::Iterator &start);
  virtual void Print (std::ostream &os) const;

  /**
   * Set the demodulation margin value.
   *
   * \param margin The demodulation margin to set.
   */
  void SetMargin (uint8_t margin);

  /**
   * Get the demodulation margin value.
   *
   * \return The demodulation margin value.
   */
  uint8_t GetMargin (void) const;

  /**
   * Set the eNB count value.
   *
   * \param enbCnt The count value to set.
   */
  void SetEnbCnt (uint8_t gwCnt);

  /**
   * Get the eNB count value.
   *
   * \return The eNB count value.
   */
  uint8_t GetEnbCnt (void) const;

  /**
   * Increment this MacCommand's enbCnt value.
   */
  void IncrementEnbCnt (void);

private:

  /**
   * This MAC command's demodulation margin value.
   */
  uint8_t m_margin;

  /**
   * This MAC command's gateway count value.
   */
  uint8_t m_gwCnt;
};

/**
  * Implementation of the LinkAdrReq Cunb MAC command.
  *
  * With this command, the CUNB server can request a device to change its
  * data rate, transmission power and the channel it uses for uplink
  * transmissions.
  */
class LinkAdrReq : public CunbMacCommand
{
public:

  LinkAdrReq ();

  LinkAdrReq (uint8_t dataRate, uint8_t txPower, uint16_t channelMask,
              uint8_t chMaskCntl, uint8_t nbRep);

  virtual void Serialize (Buffer::Iterator &start) const;
  virtual uint8_t Deserialize (Buffer::Iterator &start);
  virtual void Print (std::ostream &os) const;

  /**
   * Return the data rate prescribed by this MAC command.
   *
   * \return An unsigned 8-bit integer containing the data rate.
   */
  uint8_t GetDataRate (void);

  /**
   * Get the transmission power prescribed by this MAC command.
   *
   * The MAC layer is expected to translate this value to a certain power in
   * dBm when communicating it to the PHY, and the translation will vary based
   * on the region of the device.
   *
   * \return The TX power, encoded as an unsigned 8-bit integer.
   */
  uint8_t GetTxPower (void);

  /**
   * Get the list of enabled channels. This method takes the 16-bit channel mask
   * and translates it to a list of integers that can be more easily parsed.
   *
   * \return The list of enabled channels.
   */
  std::list<int> GetEnabledChannelsList (void);

  /**
   * Get the number of repetitions prescribed by this MAC command.
   *
   * \return The number of repetitions.
   */
  int GetRepetitions (void);

private:
  uint8_t m_dataRate;
  uint8_t m_txPower;
  uint16_t m_channelMask;
  uint8_t m_chMaskCntl;
  uint8_t m_nbRep;
};

/**
 * Implementation of the LinkAdrAns Cunb MAC command.
 *
 * With this command, the MS acknowledges a LinkAdrReq.
 */
class LinkAdrAns : public CunbMacCommand
{
public:
  LinkAdrAns ();

  LinkAdrAns (bool powerAck, bool dataRateAck, bool channelMaskAck);

  virtual void Serialize (Buffer::Iterator &start) const;
  virtual uint8_t Deserialize (Buffer::Iterator &start);
  virtual void Print (std::ostream &os) const;

private:
  bool m_powerAck;
  bool m_dataRateAck;
  bool m_channelMaskAck;
};

/**
  * Implementation of the DutyCycleReq Cunb MAC command.
  *
  * With this command, the CUNB server can limit the maximum aggregated
  * transmit duty cycle of an Mobile Device. The aggregate duty cycle is computed
  * as the duty cycle among all sub bands.
  */
class DutyCycleReq : public CunbMacCommand
{
public:
  DutyCycleReq ();
  /**
   * Constructor providing initialization of all parameters.
   *
   * \param dutyCycle The duty cycle as a 8-bit unsigned integer.
   */
  DutyCycleReq (uint8_t dutyCycle);

  virtual void Serialize (Buffer::Iterator &start) const;
  virtual uint8_t Deserialize (Buffer::Iterator &start);
  virtual void Print (std::ostream &os) const;

  /**
   * Get the maximum duty cycle prescribed by this Mac command, in fraction form.
   *
   * \return The maximum duty cycle.
   */
  double GetMaximumAllowedDutyCycle (void) const;

private:
  uint8_t m_maxDCycle;
};

/**
 * Implementation of the DutyCycleAns Cunb MAC command.
 *
 * This command holds no variables, and just consists in the CID.
 */
class DutyCycleAns : public CunbMacCommand
{
public:
  DutyCycleAns ();

  virtual void Serialize (Buffer::Iterator &start) const;
  virtual uint8_t Deserialize (Buffer::Iterator &start);
  virtual void Print (std::ostream &os) const;
};

/**
  * Implementation of the RxParamSetupReq Cunb MAC command.
  */
class RxParamSetupReq : public CunbMacCommand
{
public:
  RxParamSetupReq ();

  /**
   * Constructor providing initialization of all fields.
   *
   * \param rx1DrOffset The Data Rate offset to use for the first receive window.
   * \param rx2DataRate The Data Rate to use for the second receive window.
   * \param frequency The frequency in Hz to use for the second receive window.
   */
  RxParamSetupReq (uint8_t rx2DataRate, double frequency);

  virtual void Serialize (Buffer::Iterator &start) const;
  virtual uint8_t Deserialize (Buffer::Iterator &start);
  virtual void Print (std::ostream &os) const;


  /**
   * Get this command's Rx2DataRate parameter.
   *
   * \return The Rx2DataRate parameter.
   */
  uint8_t GetRx2DataRate (void);

  /**
   * Get this command's frequency.
   *
   * \return The frequency parameter, in Hz.
   */
  double GetFrequency (void);

private:
  uint8_t m_rx2DataRate;
  double m_frequency; //!< The frequency _in Hz_
};

/**
 * Implementation of the RxParamSetupAns Cunb MAC command.
 */
class RxParamSetupAns : public CunbMacCommand
{
public:
  RxParamSetupAns ();
  /**
   * Constructor with initialization of all parameters.
   *
   * \param rx2DataRateAck Whether or not the second slot data rate was correctly set.
   * \param channelAck Whether or not the second slot frequency was correctly set.
   */
  RxParamSetupAns (bool rx2DataRateAck, bool channelAck);

  virtual void Serialize (Buffer::Iterator &start) const;
  virtual uint8_t Deserialize (Buffer::Iterator &start);
  virtual void Print (std::ostream &os) const;

private:
  bool m_rx2DataRateAck;
  bool m_channelAck;
};

/**
 * Implementation of the DevStatusReq Cunb MAC command.
 */
class DevStatusReq : public CunbMacCommand
{
public:
  DevStatusReq ();

  virtual void Serialize (Buffer::Iterator &start) const;
  virtual uint8_t Deserialize (Buffer::Iterator &start);
  virtual void Print (std::ostream &os) const;
};

/**
 * Implementation of the DevStatusAns Cunb MAC command.
 */
class DevStatusAns : public CunbMacCommand
{
public:
  DevStatusAns ();
  /**
   * Constructor with initialization of all parameters.
   *
   * \param battery The battery level in [0, 255].
   * \param margin The demodulation margin of the last received DevStatusReq packet.
   */
  DevStatusAns (uint8_t battery, uint8_t margin);

  virtual void Serialize (Buffer::Iterator &start) const;
  virtual uint8_t Deserialize (Buffer::Iterator &start);
  virtual void Print (std::ostream &os) const;

  /**
   * Get the battery information contained in this MAC command.
   *
   * \return The battery level.
   */
  uint8_t GetBattery (void);

  /**
   * Get the demodulation margin contained in this MAC command.
   *
   * \return The margin.
   */
  uint8_t GetMargin (void);

private:
  uint8_t m_battery;
  uint8_t m_margin;
};

/**
 * Implementation of the NewChannelReq Cunb MAC command.
 */
class NewChannelReq : public CunbMacCommand
{
public:
  NewChannelReq ();

  /**
   * Constructor providing initialization of all parameters.
   *
   * \param chIndex The index of the channel this command wants to operate on.
   * \param frequency The new frequency for this channel.
   * \param minDataRate The minimum data rate allowed on this channel.
   * \param maxDataRate The minimum data rate allowed on this channel.
   */
  NewChannelReq (uint8_t chIndex, double frequency, uint8_t minDataRate, uint8_t maxDataRate);

  virtual void Serialize (Buffer::Iterator &start) const;
  virtual uint8_t Deserialize (Buffer::Iterator &start);
  virtual void Print (std::ostream &os) const;

  uint8_t GetChannelIndex (void);
  double GetFrequency (void);
  uint8_t GetMinDataRate (void);
  uint8_t GetMaxDataRate (void);

private:
  uint8_t m_chIndex;
  double m_frequency;
  uint8_t m_minDataRate;
  uint8_t m_maxDataRate;
};

/**
 * Implementation of the NewChannelAns Cunb MAC command.
 */
class NewChannelAns : public CunbMacCommand
{
public:
  NewChannelAns ();

  /**
   * Constructor providing initialization of all parameters.
   *
   * \param dataRateRangeOk Whether or not the requested data rate range was set
   * correctly.
   * \param channelFrequencyOk Whether or not the requested channel frequency
   * was set correctly.
   */
  NewChannelAns (bool dataRateRangeOk, bool channelFrequencyOk);

  virtual void Serialize (Buffer::Iterator &start) const;
  virtual uint8_t Deserialize (Buffer::Iterator &start);
  virtual void Print (std::ostream &os) const;

private:
  bool m_dataRateRangeOk;
  bool m_channelFrequencyOk;
};

/**
 * Implementation of the RxTimingSetupReq Cunb MAC command.
 */
class RxTimingSetupReq : public CunbMacCommand
{
public:
  RxTimingSetupReq ();

  /**
   * Constructor providing initialization of all parameters.
   *
   * \param delay The delay encoded in this MAC command.
   */
  RxTimingSetupReq (uint8_t delay);

  virtual void Serialize (Buffer::Iterator &start) const;
  virtual uint8_t Deserialize (Buffer::Iterator &start);
  virtual void Print (std::ostream &os) const;

  /**
   * Get the first window delay as a Time instance.
   *
   * \return The delay.
   */
  Time GetDelay (void);

private:
  uint8_t m_delay;
};

/**
 * Implementation of the RxTimingSetupAns Cunb MAC command.
 *
 * This MAC command has an empty payload.
 */
class RxTimingSetupAns : public CunbMacCommand
{
public:
  RxTimingSetupAns ();

  virtual void Serialize (Buffer::Iterator &start) const;
  virtual uint8_t Deserialize (Buffer::Iterator &start);
  virtual void Print (std::ostream &os) const;

private:
};

/**
 * Implementation of the TxParamSetupAns Cunb MAC command.
 */
class TxParamSetupAns : public CunbMacCommand
{
public:
  TxParamSetupAns ();

  virtual void Serialize (Buffer::Iterator &start) const;
  virtual uint8_t Deserialize (Buffer::Iterator &start);
  virtual void Print (std::ostream &os) const;

private:
};

/**
 * Implementation of the TxParamSetupReq Cunb MAC command.
 */
class TxParamSetupReq : public CunbMacCommand
{
public:
  TxParamSetupReq ();

  virtual void Serialize (Buffer::Iterator &start) const;
  virtual uint8_t Deserialize (Buffer::Iterator &start);
  virtual void Print (std::ostream &os) const;

private:
};

/**
 * Implementation of the DlChannelAns Cunb MAC command.
 */
class DlChannelAns : public CunbMacCommand
{
public:
  DlChannelAns ();

  virtual void Serialize (Buffer::Iterator &start) const;
  virtual uint8_t Deserialize (Buffer::Iterator &start);
  virtual void Print (std::ostream &os) const;

private:
};
}

#endif /* CUNB_MAC_COMMAND_H */
