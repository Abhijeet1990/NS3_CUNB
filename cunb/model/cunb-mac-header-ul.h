#ifndef CUNB_MAC_HEADER_UL_H
#define CUNB_MAC_HEADER_UL_H

#include "ns3/header.h"

namespace ns3 {

/**
  * This class represents the Mac header of a CUNB packet.
  */
class CunbMacHeaderUl : public Header
{
public:

  /**
   * The message type.
   *
   * The enum value corresponds to the value that will be written in the header
   * by the Serialize method.
   */
	enum MType {
		SINGLE_ACK = 0,
	    MULTIPLE_ACK = 1,
	    HELLO = 2
	  };

  static TypeId GetTypeId (void);

  CunbMacHeaderUl ();
  ~CunbMacHeaderUl ();

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


  /**
   * Check whether this header is for an uplink message
   *
   * \return True if the message is meant to be sent from an ED to a GW, false
   * otherwise.
   */
  bool IsUplink (void) const;

  /**
      * Set the message type.
      *
      * \param mtype The message type of this header.
      */
  void SetMType (enum MType mtype);

     /**
      * Get the message type from the header.
      *
      * \return The uint8_t corresponding to this header's message type.
      */
  uint8_t GetMType (void) const;

  void SetPreamble(uint8_t preamble);

  uint8_t GetPreamble (void) const;

  void SetFsize(uint8_t fsize);

  uint8_t GetFsize (void) const;

  void SetAckFlags(uint8_t ackFlags);

  uint8_t GetAckFlags (void) const;

  void SetRepCnts(uint8_t repCnts);

  uint8_t GetRepCnts(void) const;

  void SetSeqCnt(uint8_t seqCnt);

  uint8_t GetSeqCnt(void) const;

  void SetIdent(uint16_t ident);

  uint16_t GetIdent(void) const;

  /*
  void SetIdent(uint32_t ident);

  uint32_t GetIdent(void) const;
  */
  void SetData(uint64_t data);

  uint64_t GetData(void) const;



private:

  /*
   * MAC PDU in CUNB DL
   * 40 bits : Header :
   *    a. a preamble for frame detection and bit rate synchronization
   *    b. a frame type
   *    c. a frame length
   *    d. acknowledgement flags
   *    e. repitition counter
   */
  uint8_t m_preamble;
  uint8_t m_mtype;
  uint8_t m_fsize;
  uint8_t m_ack_flags;
  uint8_t m_rep_cnt;
  uint8_t m_seq_cnt;
  uint16_t m_ident;
  //uint32_t m_ident;
  uint64_t m_data;


};
}

#endif
