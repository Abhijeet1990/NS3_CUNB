#ifndef CUNB_BEACON_HEADER_H
#define CUNB_BEACON_HEADER_H

#include "ns3/header.h"
#include "mac48-address.h"

namespace ns3 {

/**
  * This class represents the Beacon header of a CUNB packet. This is broadcasted by eNB to all the MS
  */
class CunbBeaconHeader : public Header
{
public:


  static TypeId GetTypeId (void);

  CunbBeaconHeader ();
  ~CunbBeaconHeader ();

  // Pure virtual methods from Header that need to be implemented by this class
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;

  /**
   * Serialize the header.
   *
   * \param start A pointer to the buffer that will be filled with the
   * serialization.
   */
  virtual void Serialize (Buffer::Iterator start) const;

  /**
   * Deserialize the header.
   *
   * \param start A pointer to the buffer we need to deserialize.
   * \return The number of consumed bytes.
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);

  /**
   * Print the header in a human readable format.
   *
   * \param os The std::ostream on which to print the header.
   */
  virtual void Print (std::ostream &os) const;

  void SetData(double data);

  double GetData(void);

  Mac48Address GetAddress();

  void SetAddress(Mac48Address) ;

  uint32_t GetPreamble();

  void SetPreamble(uint32_t preamble) ;

  uint16_t GetFtype();

  void SetFtype(uint16_t frameType) ;

  uint16_t GetNetId();

  void SetNetId(uint16_t netId) ;

  uint16_t GetCellId();

  void SetCellId(uint16_t cellId) ;

  uint8_t GetGroupType();

  void SetGroupType(uint8_t groupType) ;

  uint8_t GetSysInfoCnt();

  void SetSysInfoCnt(uint8_t sysInfoCnt) ;

  uint8_t GetGrpSeqNo();

  void SetGrpSeqNo(uint8_t grpSeqNo) ;

private:

  /*
   * Beacon Header
   * a) 6 bytes: Header :
   *    a. a preamble for frame detection (4 bytes)
   *    b. a frame type (2 bytes)
   * b) 5.5 bytes : NwInformation
   *    a. CUNB Network ID : 15 bits
   *    b. CUNB Cell ID : 15 bits
   *    c. Sys Group Type : 6 bits (defines the payload content) i.e. frequency, Tx Power, Loopback delay etc
   *    d. Total no. of System Info message in beacon channel : 4 bits
   *    e. Group Sequence Number (that keeps track of system information groups) : 5 bits
   */

  uint32_t m_preamble;
  uint16_t m_ftype;
  uint16_t m_netId;
  uint16_t m_cellId;
  uint8_t m_sysGroupType;
  uint8_t m_sysInfoCnt;
  uint8_t m_grpSeqNo;

  double m_data;
  Mac48Address m_broadcastAdr;


};
}
#endif
