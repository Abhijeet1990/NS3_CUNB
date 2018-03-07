#ifndef NEW_COSEM_HEADER_H
#define NEW_COSEM_HEADER_H

#include "ns3/header.h"

namespace ns3 {

enum ApduType
{
  AARQ = 1,
  AARE = 2,
  RLRQ = 3,
  RLRE = 4,
  GETRQ_N = 5,
  GETRES_N = 6
};

class NewTypeAPDU : public Header
{
public:
  NewTypeAPDU ();
  virtual ~NewTypeAPDU ();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;

  // Allow protocol-specific access to the header
  void SetApduType (ApduType type);
  ApduType GetApduType () const;

private:

  ApduType m_type;
};

/*
 * COSEM, ACSE APDU: AARQ-APDU
 */

class NewCosemAarqHeader : public Header
{
public:
  NewCosemAarqHeader ();
  virtual ~NewCosemAarqHeader ();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;

  // Allow protocol-specific access to the header
  void SetIdApdu (uint8_t idApdu);
  uint8_t GetIdApdu (void) const;
  void SetProtocolVersion (uint8_t protocolVersion);
  uint8_t GetProtocolVersion (void) const;
  /*
   * AAs may be established between a client AP and server APs using various application contexts,
authentication mechanisms and xDLMS contexts as well as other parameters. For example, a client
AP may establish an AA with a server AP with an application context using short name (SN)
referencing and with another server AP with an application context using logical name (LN)
referencing. Although the messages exchanged depend on the application context of the AA
established, both server APs are interoperable with the client AP if it is able to establish the AA
using the right context with both server APs.
   */
  void SetApplicationContextName (uint64_t applicationContextName);
  uint64_t GetApplicationContextName (void) const;

  void SetDedicatedKey (uint8_t dedicatedKey);
  uint8_t GetDedicatedKey (void) const;
  void SetResponseAllowed (bool responseAllowed);
  bool GetResponseAllowed (void) const;
  void SetProposedQualityOfService (uint8_t proposedQualityOfService);
  uint8_t GetProposedQualityOfService (void) const;
  void SetProposedDlmsVersionNumber (uint8_t proposedDlmsVersionNumber);
  uint8_t GetProposedDlmsVersionNumber (void) const;
  void SetProposedConformance (uint32_t proposedConformance);
  uint32_t GetProposedConformance (void) const;
  void SetClientMaxReceivePduSize (uint16_t clientMaxReceivePduSize);
  uint16_t GetClientMaxReceivePduSize (void) const;

private:

  uint8_t m_idApdu;  // APDU identifier (1B)
  uint8_t m_protocolVersion;  // Protocol Version (1B)
  uint64_t m_applicationContextName;  // Application Context Name (rules that govern th exchange-parameters), Pag. 72 of IEC 62056-53 (8B)

  /*
   * user-information (xDLMS-Initiate.request PDU)
   */
  uint8_t m_dedicatedKey;  // Dedicated Key (1B)
  bool m_responseAllowed; // Reponse Allowed (AA is confirmed) (1B)
  uint8_t m_proposedQualityOfService;  // N/A (1B)
  uint8_t m_proposedDlmsVersionNumber;	// Version number (1B)
  uint32_t m_proposedConformance;  // Based on the example in Annex C IEC 62056-53 (4B)
  uint16_t m_clientMaxReceivePduSize;  // Client_Max_Receive_PDU_Size (2B)
};

/*
 * COSEM, ACSE APDU: AARE-APDU
 */

class NewCosemAareHeader : public Header
{
public:
  NewCosemAareHeader ();
  virtual ~NewCosemAareHeader ();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;

  // Allow protocol-specific access to the header
  void SetIdApdu (uint8_t idApdu);
  uint8_t GetIdApdu (void) const;
  void SetProtocolVersion (uint8_t protocolVersion);
  uint8_t GetProtocolVersion (void) const;
  void SetApplicationContextName (uint64_t applicationContextName);
  uint64_t GetApplicationContextName (void) const;
  void SetResult (uint8_t result);
  uint8_t GetResult (void) const;
  void SetResultSourceDiagnostic (uint8_t resultSourceDiagnostic);
  uint8_t GetResultSourceDiagnostic (void) const;

  void SetNegotiatedQualityOfService (uint8_t negotiatedQualityOfService);
  uint8_t GetNegotiatedQualityOfService (void) const;
  void SetNegotiatedDlmsVersionNumber (uint8_t negotiatedDlmsVersionNumber);
  uint8_t GetNegotiatedDlmsVersionNumber (void) const;
  void SetNegotiatedConformance (uint32_t negotiatedConformance);
  uint32_t GetNegotiatedConformance (void) const;
  void SetServerMaxReceivePduSize (uint16_t serverMaxReceivePduSize);
  uint16_t GetServerMaxReceivePduSize (void) const;
  void SetVaaName (uint16_t vaaName);
  uint16_t GetVaaName (void) const;

private:

  uint8_t m_idApdu;  // APDU identifier (1B)
  uint8_t m_protocolVersion;  // Protocol Version (1B)
  uint64_t m_applicationContextName;  // Application Context Name (rules that govern th exchange-parameters), Pag. 72 of IEC 62056-53 (8B)
  uint8_t m_result;  // Result of the request AA (1B)
  uint8_t m_resultSourceDiagnostic; // Result of a rejection of the a request AA (1B)

  /*
   * user-information (xDLMS-Initiate.request PDU)
   */

  uint8_t m_negotiatedQualityOfService;  // N/A (1B)
  uint8_t m_negotiatedDlmsVersionNumber; // Version number (1B)
  uint32_t m_negotiatedConformance; // Based on the example in Annex C IEC 62056-53 (4B)
  uint16_t m_serverMaxReceivePduSize; // Server_Max_Receive_PDU_Size (2B)
  uint16_t m_vaaName;  // Dummy Value (2B)

};

/*
 * COSEM, xDLMS_ASE APDU: GET-Request (Normal) APDU
 */

class NewCosemGetRequestNormalHeader: public Header
{
public:
  NewCosemGetRequestNormalHeader ();
  virtual ~NewCosemGetRequestNormalHeader ();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;

  // Allow protocol-specific access to the header
  void SetIdApdu (uint8_t idApdu);
  uint8_t GetIdApdu (void) const;
  void SetTypeGetRequest (uint8_t typeGetRequest);
  uint8_t GetTypeGetRequest(void) const;
  void SetInvokeIdAndPriority (uint8_t invokeIdAndPriority);
  uint8_t GetInvokeIdAndPriority (void) const;
  void SetClassId (uint16_t classId);
  uint16_t GetClassId (void) const;
  void SetInstanceId (uint64_t instanceId);
  uint64_t GetInstanceId (void) const;
  void SetAttributeId (uint8_t attributeId);
  uint8_t GetAttributeId (void) const;

private:

  uint8_t m_idApdu;  // APDU identifier (1B)
  uint8_t m_typeGetRequest;  // Type of Get-Request service (1B)
  uint8_t m_invokeIdAndPriority; // Identifier of the instance of service invocation, Page 93 of IEC 62056-53 (1B)

  // Cosem Attribute Descriptor
  uint16_t m_classId;  // Class Id of the COSEM Object (2B)
  uint64_t m_instanceId;  // Identifier of the COSEM Object (8B, 6B of information and 2B not used)
  uint8_t m_attributeId; // Identifier of the atribute's COSEM Object (1B)

};


class NewCosemGetResponseNormalHeader: public Header
{
public:
  NewCosemGetResponseNormalHeader ();
  virtual ~NewCosemGetResponseNormalHeader ();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;

  // Allow protocol-specific access to the header
  void SetIdApdu (uint8_t idApdu);
  uint8_t GetIdApdu (void) const;
  void SetTypeGetResponse (uint8_t typeGetResponse);
  uint8_t GetTypeGetResponse(void) const;
  void SetInvokeIdAndPriority (uint8_t invokeIdAndPriority);
  uint8_t GetInvokeIdAndPriority (void) const;
  void SetData (uint32_t data);
  uint32_t GetData (void) const;
  void SetDataAccessResult (uint8_t dataAccessResult);
  uint8_t GetDataAccessResult (void) const;

private:

  uint8_t m_idApdu;  // APDU identifier (1B)
  uint8_t m_typeGetResponse;  // Type of Get-Response service (1B)
  uint8_t m_invokeIdAndPriority; // Identifier of the instance of service invocation,the same as to the correspond .request (1B)

  // Result: Get-Data-Result (5B)
  uint32_t m_data;  // Requested Data [long-unsigned, Pages 22,105 of IEC 62056-62] (4B)
  uint8_t m_dataAccessResult;  // Data-access-result (1B)
};


/**
 * COSEM: WRAPPER sub-layer header
 */

class NewCosemWrapperHeader: public Header
{
public:
  NewCosemWrapperHeader ();
  virtual ~NewCosemWrapperHeader ();

  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;

  // Allow protocol-specific access to the header
  void SetVersion (uint16_t version);
  uint16_t GetVersion (void) const;
  void SetSrcwPort (uint16_t srcwPort);
  uint16_t GetSrcwPort (void) const;
  void SetDstwPort (uint16_t dstwPort);
  uint16_t GetDstwPort (void) const;
  void SetLength (uint16_t length);
  uint16_t GetLength (void) const;

private:

  uint16_t m_version;  // Version of Wrapper Sub-layer (2B)
  uint16_t m_srcwPort; // Source wrapper port number: Identify the sender AP (2B)
  uint16_t m_dstwPort; // Destination wrapper port number: Identify the destination AP (2B)
  uint16_t m_length;   // Length of the APDU transported (2B)
};

} // namespace ns3

#endif /* NEW_COSEM_HEADER_H */
