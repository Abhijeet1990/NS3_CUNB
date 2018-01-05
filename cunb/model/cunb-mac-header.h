#ifndef CUNB_MAC_HEADER_H
#define CUNB_MAC_HEADER_H

#include "ns3/header.h"

namespace ns3 {

/**
  * This class represents the Mac header of a CUNB packet.
  */
class CunbMacHeader : public Header
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
	  MULTIPLE_ACK = 1
  };

  static TypeId GetTypeId (void);

  CunbMacHeader ();
  ~CunbMacHeader ();

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

   void SetPreamble (uint8_t preamble);

   uint8_t GetPreamble (void) const;

   uint8_t GetPayloadSize (void) const;

   void SetPayloadSize (uint8_t payloadSize);

   uint32_t GetAckBits (void) const;

   void SetAckBits (uint32_t ackBits);

   void SetData(uint64_t data);

   uint64_t GetData(void) const;


private:

  /*
   * MAC PDU in CUNB DL
   * 56 bits : Header :
   *    a. a preamble for frame detection and bit rate synchronization
   *    b. a frame type
   *    c. a payload length
   *    d. acknowledgement bits
   */

  uint8_t m_preamble;
  uint8_t m_mtype;
  uint8_t m_payload_size;
  uint32_t m_ack_bits;

  uint64_t m_data;


};
}

#endif
