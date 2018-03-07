
#include "ns3/log.h"
#include "new-cosem-header.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("NewTypeAPDU");

/*-----------------------------------------------------------------------------
 *  APDU TYPE HEADER
 *-----------------------------------------------------------------------------
 */
NS_OBJECT_ENSURE_REGISTERED (NewTypeAPDU);

TypeId
NewTypeAPDU::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NewTypeAPDU")
    .SetParent<Header> ()
    .AddConstructor<NewTypeAPDU> ()
    ;
  return tid;
}

NewTypeAPDU::NewTypeAPDU ()
{
  m_type = AARQ;
}

NewTypeAPDU::~NewTypeAPDU()
{

}

TypeId
NewTypeAPDU::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
NewTypeAPDU::GetSerializedSize (void) const
{
  return 1;
}

void
NewTypeAPDU::Serialize (Buffer::Iterator start) const
{
  start.WriteU8 ((uint8_t) m_type);
}

uint32_t
NewTypeAPDU::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  uint8_t type = i.ReadU8 ();
  switch (type)
   {
    case AARQ:
    case AARE:
    case RLRQ:
    case RLRE:
    case GETRQ_N:
    case GETRES_N:
      {
        m_type = (ApduType) type;
        break;
      }
    default:
      NS_LOG_INFO ("ApduType not valid!");
   }
  return GetSerializedSize ();
}

void
NewTypeAPDU::Print (std::ostream &os) const
{
 switch (m_type)
   {
    case AARQ:
      {
        os << "AARQ";
        break;
      }
    case AARE:
      {
        os << "AARE";
        break;
      }
    case RLRQ:
      {
        os << "RLRQ";
        break;
      }
    case RLRE:
      {
        os << "RLRE";
        break;
      }
    case GETRQ_N:
      {
        os << "GETRQ_N";
        break;
      }
    case GETRES_N:
      {
        os << "GETRES_N";
        break;
      }
    default:
      os << "UNKNOWN_TYPE";
   }
}

void
NewTypeAPDU::SetApduType (ApduType type)
{
  m_type = type;
}

ApduType
NewTypeAPDU::GetApduType () const
{
  return m_type;
}


/*-----------------------------------------------------------------------------
 *  AARQ APDU
 *-----------------------------------------------------------------------------
 */
NS_OBJECT_ENSURE_REGISTERED (NewCosemAarqHeader);

TypeId
NewCosemAarqHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NewCosemAarqHeader")
    .SetParent<Header> ()
    .AddConstructor<NewCosemAarqHeader> ()
    ;
  return tid;
}

NewCosemAarqHeader::NewCosemAarqHeader ()
{
  m_idApdu = AARQ ;
  m_protocolVersion = 0 ;
  m_applicationContextName = 0;
  m_dedicatedKey = 0;
  m_responseAllowed = true;
  m_proposedQualityOfService = 0;
  m_proposedDlmsVersionNumber = 0;
  m_proposedConformance = 0;
  m_clientMaxReceivePduSize = 0;
}

NewCosemAarqHeader::~NewCosemAarqHeader ()
{

}

TypeId
NewCosemAarqHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
NewCosemAarqHeader::GetSerializedSize (void) const
{
  return 20;
}

void
NewCosemAarqHeader::Serialize (Buffer::Iterator start) const
{
  start.WriteU8 (m_idApdu);
  start.WriteU8 (m_protocolVersion);
  start.WriteHtonU64 (m_applicationContextName);
  start.WriteU8 (m_dedicatedKey);
  start.WriteU8 ((uint8_t) m_responseAllowed);
  start.WriteU8 (m_proposedQualityOfService);
  start.WriteU8 (m_proposedDlmsVersionNumber);
  start.WriteHtonU32 (m_proposedConformance);
  start.WriteHtonU16 (m_clientMaxReceivePduSize);
}

uint32_t
NewCosemAarqHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_idApdu = i.ReadU8 ();
  m_protocolVersion = i.ReadU8 ();
  m_applicationContextName = i.ReadNtohU64 ();
  m_dedicatedKey = i.ReadU8 ();
  m_responseAllowed = (bool)i.ReadU8 ();
  m_proposedQualityOfService = i.ReadU8 ();
  m_proposedDlmsVersionNumber = i.ReadU8 ();
  m_proposedConformance = i.ReadNtohU32 ();
  m_clientMaxReceivePduSize = i.ReadNtohU16 ();
  return GetSerializedSize ();
}

void
NewCosemAarqHeader::Print (std::ostream &os) const
{
  os << "id AARQ " << m_idApdu
     << "Protocol Version " << m_protocolVersion
     << "Application Context Name " << m_applicationContextName
     << "Dedicated Key " << m_dedicatedKey
     << "Reponse Allowed " << m_responseAllowed
     << "Proposed Quality Of Service " << m_proposedQualityOfService
     << "Version number " << m_proposedDlmsVersionNumber
     << "Proposed Conformance " << m_proposedConformance
     << "Client Max Receive PDU Size " << m_clientMaxReceivePduSize;
}

void
NewCosemAarqHeader::SetIdApdu (uint8_t idApdu)
{
  m_idApdu = idApdu;
}

uint8_t
NewCosemAarqHeader::GetIdApdu (void) const
{
  return m_idApdu;
}

void
NewCosemAarqHeader::SetProtocolVersion (uint8_t protocolVersion)
{
  m_protocolVersion = protocolVersion;
}

uint8_t
NewCosemAarqHeader::GetProtocolVersion (void) const
{
  return m_protocolVersion;
}

void
NewCosemAarqHeader::SetApplicationContextName (uint64_t applicationContextName)
{
  m_applicationContextName = applicationContextName;
}

uint64_t
NewCosemAarqHeader::GetApplicationContextName (void) const
{
  return m_applicationContextName;
}

void
NewCosemAarqHeader::SetDedicatedKey (uint8_t dedicatedKey)
{
  m_dedicatedKey = dedicatedKey;
}

uint8_t
NewCosemAarqHeader::GetDedicatedKey (void) const
{
  return m_dedicatedKey;
}

void
NewCosemAarqHeader::SetResponseAllowed (bool responseAllowed)
{
  m_responseAllowed = responseAllowed;
}

bool
NewCosemAarqHeader::GetResponseAllowed (void) const
{
  return m_responseAllowed;
}

void
NewCosemAarqHeader::SetProposedQualityOfService (uint8_t proposedQualityOfService)
{
  m_proposedQualityOfService = proposedQualityOfService;
}

uint8_t
NewCosemAarqHeader::GetProposedQualityOfService (void) const
{
  return m_proposedQualityOfService;
}

void
NewCosemAarqHeader::SetProposedDlmsVersionNumber (uint8_t proposedDlmsVersionNumber)
{
  m_proposedDlmsVersionNumber = proposedDlmsVersionNumber;
}

uint8_t
NewCosemAarqHeader::GetProposedDlmsVersionNumber (void) const
{
  return m_proposedDlmsVersionNumber;
}

void
NewCosemAarqHeader::SetProposedConformance (uint32_t proposedConformance)
{
  m_proposedConformance = proposedConformance;
}

uint32_t
NewCosemAarqHeader::GetProposedConformance (void) const
{
  return m_proposedConformance;
}

void
NewCosemAarqHeader::SetClientMaxReceivePduSize (uint16_t clientMaxReceivePduSize)
{
  m_clientMaxReceivePduSize = clientMaxReceivePduSize;
}

uint16_t
NewCosemAarqHeader::GetClientMaxReceivePduSize (void) const
{
  return m_clientMaxReceivePduSize;
}

/*-----------------------------------------------------------------------------
 *  AARE APDU
 *-----------------------------------------------------------------------------
 */
NS_OBJECT_ENSURE_REGISTERED (NewCosemAareHeader);

TypeId
NewCosemAareHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NewCosemAareHeader")
    .SetParent<Header> ()
    .AddConstructor<NewCosemAareHeader> ()
    ;
  return tid;
}

NewCosemAareHeader::NewCosemAareHeader ()
{
  m_idApdu = AARE;
  m_protocolVersion = 0;
  m_applicationContextName = 0;
  m_result = 0; // Result of the request AA, {0, accepted}
  m_resultSourceDiagnostic = 0;  // Result of a rejection of the a request AA, {0, null}
  m_negotiatedQualityOfService = 0;
  m_negotiatedDlmsVersionNumber = 6;
  m_negotiatedConformance = 0x001010; // {0x001010}, Based on the example in Annex C IEC 62056-53
  m_serverMaxReceivePduSize = 0x1F4;  // Server_Max_Receive_PDU_Size,{0x1F4}: 500 bytes
  m_vaaName = 0x0007; // Dummy Value {0x0007}.Taken from page 98 IEC 62056-53
}

NewCosemAareHeader::~NewCosemAareHeader ()
{

}

TypeId
NewCosemAareHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
NewCosemAareHeader::GetSerializedSize (void) const
{
  return 22;
}

void
NewCosemAareHeader::Serialize (Buffer::Iterator start) const
{
  start.WriteU8 (m_idApdu);
  start.WriteU8 (m_protocolVersion);
  start.WriteHtonU64 (m_applicationContextName);
  start.WriteU8 (m_result);
  start.WriteU8 (m_resultSourceDiagnostic);
  start.WriteU8 (m_negotiatedQualityOfService);
  start.WriteU8 (m_negotiatedDlmsVersionNumber);
  start.WriteHtonU32 (m_negotiatedConformance);
  start.WriteHtonU16 (m_serverMaxReceivePduSize);
  start.WriteHtonU16 (m_vaaName);
}

uint32_t
NewCosemAareHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_idApdu = i.ReadU8 ();
  m_protocolVersion = i.ReadU8 ();
  m_applicationContextName = i.ReadNtohU64 ();
  m_result = i.ReadU8 ();
  m_resultSourceDiagnostic = i.ReadU8 ();
  m_negotiatedQualityOfService = i.ReadU8 ();
  m_negotiatedDlmsVersionNumber = i.ReadU8 ();
  m_negotiatedConformance = i.ReadNtohU32 ();
  m_serverMaxReceivePduSize = i.ReadNtohU16 ();
  m_vaaName = i.ReadNtohU16 ();

  return GetSerializedSize ();
}

void
NewCosemAareHeader::Print (std::ostream &os) const
{
  os << "id AARQ " << m_idApdu
     << "Protocol Version " << m_protocolVersion
     << "Application Context Name " << m_applicationContextName
     << "Result of the request AA " << m_result
     << "Result of a rejection of the a request AA " << m_resultSourceDiagnostic
     << "Negotiated Quality Of Service " << m_negotiatedQualityOfService
     << "Negotiated Dlms Version Number " <<  m_negotiatedDlmsVersionNumber
     << "Negotiated Conformance " << m_negotiatedConformance
     << "Server Max Receive Pdu Size " <<  m_serverMaxReceivePduSize
     << "vaa-name " <<  m_vaaName;
}

void
NewCosemAareHeader::SetIdApdu (uint8_t idApdu)
{
  m_idApdu = idApdu;
}

uint8_t
NewCosemAareHeader::GetIdApdu (void) const
{
  return m_idApdu;
}

void
NewCosemAareHeader::SetProtocolVersion (uint8_t protocolVersion)
{
  m_protocolVersion = protocolVersion;
}

uint8_t
NewCosemAareHeader::GetProtocolVersion (void) const
{
  return m_protocolVersion;
}

void
NewCosemAareHeader::SetApplicationContextName (uint64_t applicationContextName)
{
  m_applicationContextName = applicationContextName;
}

uint64_t
NewCosemAareHeader::GetApplicationContextName (void) const
{
  return m_applicationContextName;
}

void
NewCosemAareHeader::SetResult (uint8_t result)
{
  m_result = result;
}

uint8_t
NewCosemAareHeader::GetResult (void) const
{
  return m_result;
}

void
NewCosemAareHeader::SetResultSourceDiagnostic (uint8_t resultSourceDiagnostic)
{
  m_resultSourceDiagnostic = resultSourceDiagnostic;
}

uint8_t
NewCosemAareHeader::GetResultSourceDiagnostic (void) const
{
  return m_resultSourceDiagnostic;
}

void
NewCosemAareHeader::SetNegotiatedQualityOfService (uint8_t negotiatedQualityOfService)
{
  m_negotiatedQualityOfService = negotiatedQualityOfService;
}

uint8_t
NewCosemAareHeader::GetNegotiatedQualityOfService (void) const
{
  return m_negotiatedQualityOfService;
}

void
NewCosemAareHeader::SetNegotiatedDlmsVersionNumber (uint8_t negotiatedDlmsVersionNumber)
{
  m_negotiatedDlmsVersionNumber = negotiatedDlmsVersionNumber;
}

uint8_t
NewCosemAareHeader::GetNegotiatedDlmsVersionNumber (void) const
{
  return m_negotiatedDlmsVersionNumber;
}

void
NewCosemAareHeader::SetNegotiatedConformance (uint32_t negotiatedConformance)
{
  m_negotiatedConformance = negotiatedConformance;
}

uint32_t
NewCosemAareHeader::GetNegotiatedConformance (void) const
{
  return m_negotiatedConformance;
}

void
NewCosemAareHeader::SetServerMaxReceivePduSize (uint16_t serverMaxReceivePduSize)
{
  m_serverMaxReceivePduSize = serverMaxReceivePduSize;
}

uint16_t
NewCosemAareHeader::GetServerMaxReceivePduSize (void) const
{
  return m_serverMaxReceivePduSize;
}

void
NewCosemAareHeader::SetVaaName (uint16_t vaaName)
{
  m_vaaName = vaaName;
}

uint16_t
NewCosemAareHeader::GetVaaName (void) const
{
  return m_vaaName;
}

/*-----------------------------------------------------------------------------
 *  GET-Request (Normal) APDU
 *-----------------------------------------------------------------------------
 */

//NS_LOG_COMPONENT_DEFINE ("NewCosemGetRequestNormalHeader");
NS_OBJECT_ENSURE_REGISTERED (NewCosemGetRequestNormalHeader);

TypeId
NewCosemGetRequestNormalHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NewCosemGetRequestNormalHeader")
    .SetParent<Header> ()
    .AddConstructor<NewCosemGetRequestNormalHeader> ()
    ;
  return tid;
}

NewCosemGetRequestNormalHeader::NewCosemGetRequestNormalHeader ()
{
  m_idApdu = GETRQ_N;
  m_typeGetRequest = 1;   // Normal
  m_invokeIdAndPriority = 0x02; // 0000 0010 (invoke_id {0b0000}),service_class= 1 (confirmed) priority level ({normal}))
  m_classId = 0X03;  // Class Register
  m_instanceId = 0x010100070000;  // OBIS CODE: 1.1.0.7.0.0
  m_attributeId = 0x02; // Second Attribut = Value
}

NewCosemGetRequestNormalHeader::~NewCosemGetRequestNormalHeader ()
{

}

TypeId
NewCosemGetRequestNormalHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
NewCosemGetRequestNormalHeader::GetSerializedSize (void) const
{
  return 14;
}

void
NewCosemGetRequestNormalHeader::Serialize (Buffer::Iterator start) const
{
  start.WriteU8 (m_idApdu);
  start.WriteU8 (m_typeGetRequest);
  start.WriteU8 (m_invokeIdAndPriority);
  start.WriteHtonU16 (m_classId);
  start.WriteHtonU64 (m_instanceId);
  start.WriteU8 (m_attributeId);
}

uint32_t
NewCosemGetRequestNormalHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_idApdu = i.ReadU8 ();
  m_typeGetRequest = i.ReadU8 ();
  m_invokeIdAndPriority = i.ReadU8 ();
  m_classId = i.ReadNtohU16 ();
  m_instanceId = i.ReadNtohU64 ();
  m_attributeId = i.ReadU8 ();

  return GetSerializedSize ();
}

void
NewCosemGetRequestNormalHeader::Print (std::ostream &os) const
{
  os << "id GET-Request (Normal) APDU " << m_idApdu
     << "Type Get-Request " << m_typeGetRequest
     << "Invoke-Id And Priority " << m_invokeIdAndPriority
     << "Cosem Attribute Descriptor: Class-Id " <<  m_classId
     << "Cosem Attribute Descriptor: Instance-Id " << m_instanceId
     << "Cosem Attribute Descriptor: Attribute-Id " << m_attributeId;
}

void
NewCosemGetRequestNormalHeader::SetIdApdu (uint8_t idApdu)
{
  m_idApdu = idApdu;
}

uint8_t
NewCosemGetRequestNormalHeader::GetIdApdu (void) const
{
  return m_idApdu;
}

void
NewCosemGetRequestNormalHeader::SetTypeGetRequest (uint8_t typeGetRequest)
{
  m_typeGetRequest = typeGetRequest;
}

uint8_t
NewCosemGetRequestNormalHeader::GetTypeGetRequest(void) const
{
  return m_typeGetRequest;
}

void
NewCosemGetRequestNormalHeader::SetInvokeIdAndPriority (uint8_t invokeIdAndPriority)
{
  m_invokeIdAndPriority = invokeIdAndPriority;
}

uint8_t
NewCosemGetRequestNormalHeader::GetInvokeIdAndPriority (void) const
{
  return m_invokeIdAndPriority;
}

void
NewCosemGetRequestNormalHeader::SetClassId (uint16_t classId)
{
  m_classId = classId;
}

uint16_t
NewCosemGetRequestNormalHeader::GetClassId (void) const
{
  return m_classId;
}

void
NewCosemGetRequestNormalHeader::SetInstanceId (uint64_t instanceId)
{
  m_instanceId = instanceId;
}

uint64_t
NewCosemGetRequestNormalHeader::GetInstanceId (void) const
{
  return m_instanceId;
}

void
NewCosemGetRequestNormalHeader::SetAttributeId (uint8_t attributeId)
{
  m_attributeId = attributeId;
}

uint8_t
NewCosemGetRequestNormalHeader::GetAttributeId (void) const
{
  return m_attributeId;
}



/*-----------------------------------------------------------------------------
 *  GET-Response (Normal) APDU
 *-----------------------------------------------------------------------------
 */
//NS_LOG_COMPONENT_DEFINE ("NewCosemGetResponseNormalHeader");
NS_OBJECT_ENSURE_REGISTERED (NewCosemGetResponseNormalHeader);

TypeId
NewCosemGetResponseNormalHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NewCosemGetResponseNormalHeader")
    .SetParent<Header> ()
    .AddConstructor<NewCosemGetResponseNormalHeader> ()
    ;
  return tid;
}

NewCosemGetResponseNormalHeader::NewCosemGetResponseNormalHeader ()
{
  m_idApdu = GETRES_N;
  m_typeGetResponse = 1;   // Normal
  m_invokeIdAndPriority = 0x02; // 0000 0010 (invoke_id {0b0000}),service_class= 1 (confirmed) priority level ({normal}))
  m_data = 0;
  m_dataAccessResult = 0;
}

NewCosemGetResponseNormalHeader::~NewCosemGetResponseNormalHeader ()
{

}

TypeId
NewCosemGetResponseNormalHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
NewCosemGetResponseNormalHeader::GetSerializedSize (void) const
{
  return 8;
}

void
NewCosemGetResponseNormalHeader::Serialize (Buffer::Iterator start) const
{
  start.WriteU8 (m_idApdu);
  start.WriteU8 (m_typeGetResponse);
  start.WriteU8 (m_invokeIdAndPriority);
  start.WriteHtonU32 (m_data);
  start.WriteU8 (m_dataAccessResult);
}

uint32_t
NewCosemGetResponseNormalHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_idApdu = i.ReadU8 ();
  m_typeGetResponse = i.ReadU8 ();
  m_invokeIdAndPriority = i.ReadU8 ();
  m_data = i.ReadNtohU32 ();
  m_dataAccessResult = i.ReadU8 ();

  return GetSerializedSize ();
}

void
NewCosemGetResponseNormalHeader::Print (std::ostream &os) const
{
  os << "id GET-Response (Normal) APDU " << m_idApdu
     << "Type Get-Request " << m_typeGetResponse
     << "Invoke-Id And Priority " << m_invokeIdAndPriority
     << "Result: Data " <<  m_data
     << "Result: Data-Access-Result " << m_dataAccessResult;
}

void
NewCosemGetResponseNormalHeader::SetIdApdu (uint8_t idApdu)
{
  m_idApdu = idApdu;
}

uint8_t
NewCosemGetResponseNormalHeader::GetIdApdu (void) const
{
  return m_idApdu;
}

void
NewCosemGetResponseNormalHeader::SetTypeGetResponse (uint8_t typeGetResponse)
{
  m_typeGetResponse = typeGetResponse;
}

uint8_t
NewCosemGetResponseNormalHeader::GetTypeGetResponse(void) const
{
  return m_typeGetResponse;
}

void
NewCosemGetResponseNormalHeader::SetInvokeIdAndPriority (uint8_t invokeIdAndPriority)
{
  m_invokeIdAndPriority = invokeIdAndPriority;
}

uint8_t
NewCosemGetResponseNormalHeader::GetInvokeIdAndPriority (void) const
{
  return m_invokeIdAndPriority;
}

void
NewCosemGetResponseNormalHeader::SetData (uint32_t data)
{
  m_data = data;
}

uint32_t
NewCosemGetResponseNormalHeader::GetData (void) const
{
  return m_data;
}

void
NewCosemGetResponseNormalHeader::SetDataAccessResult (uint8_t dataAccessResult)
{
  m_dataAccessResult = dataAccessResult;
}

uint8_t
NewCosemGetResponseNormalHeader::GetDataAccessResult (void) const
{
  return m_dataAccessResult;
}


/*-----------------------------------------------------------------------------
 *  WRAPPER PDU
 *-----------------------------------------------------------------------------
 */
//NS_LOG_COMPONENT_DEFINE ("NewCosemWrapperHeader");
NS_OBJECT_ENSURE_REGISTERED (NewCosemWrapperHeader);

TypeId
NewCosemWrapperHeader::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NewCosemWrapperHeader")
    .SetParent<Header> ()
    .AddConstructor<NewCosemWrapperHeader> ()
    ;
  return tid;
}

NewCosemWrapperHeader::NewCosemWrapperHeader ()
{
  m_version = 0x0001;
  m_srcwPort = 0;
  m_dstwPort = 0;
  m_length = 0;
}

NewCosemWrapperHeader::~NewCosemWrapperHeader ()
{

}

TypeId
NewCosemWrapperHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

uint32_t
NewCosemWrapperHeader::GetSerializedSize (void) const
{
  return 8;
}

void
NewCosemWrapperHeader::Serialize (Buffer::Iterator start) const
{
  start.WriteHtonU16 (m_version);
  start.WriteHtonU16 (m_srcwPort);
  start.WriteHtonU16 (m_dstwPort);
  start.WriteHtonU16 (m_length);
}

uint32_t
NewCosemWrapperHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  m_version = i.ReadNtohU16 ();
  m_srcwPort = i.ReadNtohU16 ();
  m_dstwPort = i.ReadNtohU16 ();
  m_length = i.ReadNtohU16 ();

  return GetSerializedSize ();
}

void
NewCosemWrapperHeader::Print (std::ostream &os) const
{
  os << "Version of Wrapper Sub-layer " << m_version
     << "Source wrapper port number " << m_srcwPort
     << "Destination wrapper port number " << m_dstwPort
     << "Length of the APDU transported " << m_length;
}

void
NewCosemWrapperHeader::SetVersion (uint16_t version)
{
  m_version = version;
}

uint16_t
NewCosemWrapperHeader::GetVersion (void) const
{
  return m_version;
}

void
NewCosemWrapperHeader::SetSrcwPort (uint16_t srcwPort)
{
  m_srcwPort = srcwPort;
}

uint16_t
NewCosemWrapperHeader::GetSrcwPort (void) const
{
  return m_srcwPort;
}

void
NewCosemWrapperHeader::SetDstwPort (uint16_t dstwPort)
{
  m_dstwPort = dstwPort;
}

uint16_t
NewCosemWrapperHeader::GetDstwPort (void) const
{
  return m_dstwPort;
}

void
NewCosemWrapperHeader::SetLength (uint16_t length)
{
  m_length = length;
}

uint16_t
NewCosemWrapperHeader::GetLength (void) const
{
  return m_length;
}

} // namespace ns3


