/*
 *
 *  Copyright (C) 1997-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 * Author: Michael Onken
 *
 * Module: dcmnet
 *
 * Purpose:
 *   User Identity Negotiation for A-ASSOCIATE (Supp. 99)
 *
 * Last Update:         $Author: uli $
 * Update Date:         $Date: 2010-11-01 10:42:44 $
 * CVS/RCS Revision:    $Revision: 1.9 $
 * Status:              $State: Exp $
 *
 * CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/dcuserid.h"
#include "dcmtk/dcmnet/dul.h"
#include "dulstruc.h"

/* ************************************************************************* */
/*       Implementation of class UserIdentityNegotiationSubItem              */
/* ************************************************************************* */

// Constructor, sets constants
UserIdentityNegotiationSubItem::UserIdentityNegotiationSubItem() :
  m_itemType (DUL_TYPENEGOTIATIONOFUSERIDENTITY),
  m_reserved(0)
{
}

/* ************************************************************************* */
/*     Implementation of class UserIdentityNegotiationSubItemRQ              */
/* ************************************************************************* */

// Constructor, constructs empty Identity Negotiation structure
UserIdentityNegotiationSubItemRQ::UserIdentityNegotiationSubItemRQ() :
    m_userIdentityType(ASC_USER_IDENTITY_NONE)
  , m_posRspRequested(0)
  , m_primField(NULL)
  , m_primFieldLength(0)
  , m_secField(NULL)
  , m_secFieldLength(0)
{
}


// Returns that this subitem is part of a request
unsigned char UserIdentityNegotiationSubItemRQ::pduType() const
{
  return DUL_TYPEASSOCIATERQ;
}


// Clears all member variables and frees memory
void UserIdentityNegotiationSubItemRQ::clear()
{
  m_userIdentityType = ASC_USER_IDENTITY_NONE;
  m_posRspRequested = 0;
  if (m_primField != NULL)
  {
    delete[] m_primField; m_primField = NULL;
  }
  m_primFieldLength = 0;
  if (m_secField != NULL)
  {
    delete[] m_secField; m_secField = NULL;
  }
  m_secFieldLength = 0;
}


// Sets identification mode
void
UserIdentityNegotiationSubItemRQ::setIdentityType(const T_ASC_UserIdentityNegotiationMode& mode)
{
  m_userIdentityType = mode;
}


// Returns identity type
T_ASC_UserIdentityNegotiationMode
UserIdentityNegotiationSubItemRQ::getIdentityType()
{
  return m_userIdentityType;
}


// Sets primary field (copied from parameter)
void
UserIdentityNegotiationSubItemRQ::setPrimField(const char *buffer,
                                               const Uint16& length)
{
  if (m_primField != NULL)
  {
    delete[] m_primField;
    m_primField = NULL;
  }
  m_primFieldLength = length;
  if ((length == 0) || (buffer == NULL))
    return;

  m_primField = new char[length];
  memcpy(m_primField, buffer, length);
}


// Sets secondary field (copied form parameter)
void
UserIdentityNegotiationSubItemRQ::setSecField(const char *buffer,
                                              const Uint16& length)
{
  if (m_secField != NULL)
  {
    delete[] m_secField;
    m_secField = NULL;
  }
  m_secFieldLength = length;
  if ((length == 0) || (buffer == NULL))
    return;

  m_secField = new char[length];
  memcpy(m_secField, buffer, length);
}


// Returns primary field. Memory must be freed by caller.
Uint16
UserIdentityNegotiationSubItemRQ::getPrimField(char*& resultBuf,
                                               Uint16& resultLen) const
{
  if ((m_primFieldLength == 0) || (m_primField == NULL))
  {
    resultBuf = NULL;
    resultLen = 0;
    return 0;
  }
  resultBuf = new char[m_primFieldLength];
  memcpy(resultBuf, m_primField, m_primFieldLength);
  resultLen = m_primFieldLength;
  return resultLen;
}


// Returns secondary field. Memory must be freed by caller.
unsigned short
UserIdentityNegotiationSubItemRQ::getSecField(char*& resultBuf,
                                              Uint16& resultLen) const
{
  if ((m_secFieldLength == 0) || (m_secField == NULL))
  {
    resultBuf = NULL;
    resultLen = 0;
    return 0;
  }
  resultBuf = new char[m_secFieldLength];
  memcpy(resultBuf, m_secField, m_secFieldLength);
  resultLen = m_secFieldLength;
  return resultLen;
}


// Enables/disables requesting of a positive identity response
void UserIdentityNegotiationSubItemRQ::setReqPosResponse(const OFBool& reqPosRsp)
{
  m_posRspRequested = reqPosRsp ? 1 : 0;
}


// Stream the package into a byte stream for network transmission
OFCondition UserIdentityNegotiationSubItemRQ::stream(unsigned char *targetBuffer,
                                                     unsigned long& lengthWritten) const
{
  // recompute the length fields
  streamedLength(lengthWritten);
  *targetBuffer++ = getItemType();
  *targetBuffer++ = getReserved();
  /* item length = total length - 1 -1 -2 */
  COPY_SHORT_BIG(OFstatic_cast(unsigned short, lengthWritten - 4), targetBuffer);
  targetBuffer += 2;

  *targetBuffer++ = OFstatic_cast(unsigned char, m_userIdentityType);
  *targetBuffer++ = m_posRspRequested;

  COPY_SHORT_BIG(m_primFieldLength, targetBuffer);
  targetBuffer += 2;

  (void) memcpy(targetBuffer, m_primField, m_primFieldLength);
  targetBuffer += m_primFieldLength;

  COPY_SHORT_BIG(m_secFieldLength, targetBuffer);
  targetBuffer += 2;

  // If length is equal to zero, nothing is copied.
  (void) memcpy(targetBuffer, m_secField, m_secFieldLength);
  targetBuffer += m_secFieldLength;

  return EC_Normal;
}


// Calculates length values and returns total length of the user item if streamed
OFCondition UserIdentityNegotiationSubItemRQ::streamedLength(unsigned long& length) const
{
  // content length = userIdentityType + positive Response Requested + primary field length field + primary field length + 2nd field length (2) (+ 2nd field)
  length = 1 + 1 + 2 + m_primFieldLength;
  length += 2;
  if (m_userIdentityType == ASC_USER_IDENTITY_USER_PASSWORD)
  {
    length += m_secFieldLength;
  }

  // sum up length of item (item type + reserved + item length field + content length) to get total length
  length += 1 + 1 + 2;

  return EC_Normal;
}


// Parse User Identity request from read buffer. Buffer must start with item type field.
OFCondition UserIdentityNegotiationSubItemRQ::parseFromBuffer(unsigned char *readBuffer,
                                                              unsigned long &bytesRead,
                                                              unsigned long availData)
{
  /* BYTE POSITIONS
   * 0       Item Type
   * 1       Reserved (=0)
   * 2-3     Item Length
   * 4       User Identity Type
   * 5       Positive Response Requested
   * 6-7     Primary-field length
   * 8-n     Primary-field
   * n+1-n+2 Secondary-field length
   * n+3-m   Secondary-field
   */

  // If n and m are both 0, this takes 10 bytes (absolute minimum)
  if (availData < 10)
  {
      char buffer[256];
      sprintf(buffer, "DUL user identity rq length %ld. Need at least 10 bytes", availData);
      return makeDcmnetCondition(DULC_ILLEGALPDULENGTH, OF_error, buffer);
  }

  bytesRead = 0;
  // Skip "item type" and "reserved" field
  readBuffer += 2;
  // Parse total item content length
  unsigned short itemLength = 0;
  EXTRACT_SHORT_BIG(readBuffer, itemLength);
  readBuffer += 2;

  // Is itemLength larger than the available data from readBuffer?
  if (availData - 4 < itemLength)
  {
      char buffer[256];
      sprintf(buffer, "DUL illegal user identify rq length %ld. Info claims to be %hd bytes.",
              availData, itemLength);
      return makeDcmnetCondition(DULC_ILLEGALPDULENGTH, OF_error, buffer);
  }

  // Do some length checking
  if (itemLength < 6 ) // at least 6 bytes are mandatory: identity type(1) pos. response(1) primLength(2), secfieldlength(2)
    return EC_CorruptedData;

  // Parse user identity type
  const unsigned char identType = *readBuffer;
  if ((identType < 1) || (identType > 4))
    m_userIdentityType = ASC_USER_IDENTITY_UNKNOWN;
  else
    m_userIdentityType = OFstatic_cast(T_ASC_UserIdentityNegotiationMode, identType);
  readBuffer++;

  // Parse Positive Response Requested field
  m_posRspRequested = *readBuffer;
  readBuffer++;

  // Parse length of primary field
  EXTRACT_SHORT_BIG(readBuffer, m_primFieldLength);
  readBuffer += 2;

  // 4 bytes read so far and 2 bytes will be read after this sub-field
  if (itemLength - 4 - 2 < m_primFieldLength)
  {
      char buffer[256];
      sprintf(buffer, "DUL illegal user identify rq length %ld. Info claims to be %hd bytes. "
              "Primary field has %hd bytes.", availData, itemLength, m_primFieldLength);
      return makeDcmnetCondition(DULC_ILLEGALPDULENGTH, OF_error, buffer);
  }
  if (m_primFieldLength > 0)
  {
    m_primField = new char[m_primFieldLength];
    memcpy(m_primField, readBuffer, m_primFieldLength);
  }

  readBuffer += m_primFieldLength;
  bytesRead = 8 + m_primFieldLength; // 2+2+1+1+2+primary field length

  EXTRACT_SHORT_BIG(readBuffer, m_secFieldLength);
  readBuffer += 2;
  bytesRead += 2;

  // 6 + m_primField bytes read so far, trying to read m_secFieldLength more bytes
  if (itemLength - 6 - m_primFieldLength < m_secFieldLength)
  {
      char buffer[256];
      sprintf(buffer, "DUL illegal user identify rq length %ld. Info claims to be %hd bytes. "
              "Primary field has %hd bytes. Secondary field has %hd bytes.",
              availData, itemLength, m_primFieldLength, m_secFieldLength);
      return makeDcmnetCondition(DULC_ILLEGALPDULENGTH, OF_error, buffer);
  }

  // only user identity type 2 (Username + password authentication) requires second value field
  if ( (m_userIdentityType == ASC_USER_IDENTITY_USER_PASSWORD) && (m_secFieldLength > 0) )
  {
    m_secField = new char[m_secFieldLength];
    memcpy(m_secField, readBuffer, m_secFieldLength);
    //m_secField.append((const char*)readBuffer, m_secFieldLength);
    readBuffer += m_secFieldLength;
    bytesRead += m_secFieldLength;
  }

  return EC_Normal;
}


// Dump User Identity sub item to specified output stream
void UserIdentityNegotiationSubItemRQ::dump(STD_NAMESPACE ostream& outstream) const
{
  switch (m_userIdentityType)
  {
    case ASC_USER_IDENTITY_NONE:
       outstream << "none";
    break;
    case ASC_USER_IDENTITY_USER:
       outstream << "  Authentication mode 1: Username" << OFendl;
       if (m_primFieldLength > 0)
       {
         char* buf = new char[m_primFieldLength+1];
         strncpy(buf, OFstatic_cast(const char*, m_primField), m_primFieldLength);
         buf[m_primFieldLength] = '\0';
         outstream << "  Username: [" << buf << "]" << OFendl;
         delete[] buf;
       }
       else
         outstream << "  Username: []" << OFendl;
    break;
    case ASC_USER_IDENTITY_USER_PASSWORD:
       outstream << "  Authentication mode 2: Username/Password" << OFendl;
       if (m_primFieldLength > 0)
       {
         char* buf = new char[m_primFieldLength+1];
         strncpy(buf, OFstatic_cast(const char*, m_primField), m_primFieldLength);
         buf[m_primFieldLength] = '\0';
         outstream << "  Username: [" << buf << "]" << OFendl;
         delete[] buf;
       }
       else
         outstream << "  Username: []" << OFendl;
       if (m_secFieldLength > 0)
       {
         char* buf = new char[m_secFieldLength+1];
         strncpy(buf, OFstatic_cast(const char*, m_secField), m_secFieldLength);
         buf[m_secFieldLength] = '\0';
         outstream << "  Password: [" << buf << "]" << OFendl;
         delete[] buf;
       }
       else
         outstream << "  Password: []" << OFendl;
      break;
    case ASC_USER_IDENTITY_KERBEROS:
      outstream << "  Authentication mode 3: Kerberos" << OFendl <<
        "  Kerberos Service Ticket (not dumped) length: "<< m_primFieldLength << OFendl;
      break;
    case ASC_USER_IDENTITY_SAML:
      outstream << "  Authentication mode 4: SAML" << OFendl <<
        "  SAML Assertion (not dumped) length: " << m_primFieldLength << OFendl;
      break;
    default:
      outstream << "  Authentication mode: Unknown " << OFendl <<
        "  First value (not dumped), length: " << m_primFieldLength << OFendl <<
        "  Second Value (not dumped), length: " << m_secFieldLength << OFendl;
  }
  outstream << "  Positive Response requested: " << ((m_posRspRequested != 0) ? "Yes" : "No") << OFendl << OFStringStream_ends;
}


// Assignment operator
UserIdentityNegotiationSubItemRQ& UserIdentityNegotiationSubItemRQ::operator= (const UserIdentityNegotiationSubItemRQ& rhs)
{
  this->clear();
  m_userIdentityType = rhs.m_userIdentityType;
  m_posRspRequested = rhs.m_posRspRequested;
  if (rhs.m_primFieldLength > 0)
  {
    if (rhs.getPrimField(m_primField, m_primFieldLength) != rhs.m_primFieldLength)
    {
      m_primField = NULL; m_primFieldLength = 0;
    }
  } else m_primFieldLength = 0;
  if ((rhs.m_secFieldLength > 0) && (rhs.m_userIdentityType == ASC_USER_IDENTITY_USER_PASSWORD))
  {
    if (rhs.getSecField(m_secField, m_secFieldLength) != rhs.m_secFieldLength)
    {
      m_secField = NULL; m_secFieldLength = 0;
    }
  } else m_secFieldLength = 0;
  return *this;
}


// Copy Constructor
UserIdentityNegotiationSubItemRQ::UserIdentityNegotiationSubItemRQ(const UserIdentityNegotiationSubItemRQ& rhs) :
  UserIdentityNegotiationSubItem(rhs)
{
  *this = rhs;
}


// Destructor, frees memory of primary field
UserIdentityNegotiationSubItemRQ::~UserIdentityNegotiationSubItemRQ()
{
  if ((m_primField != NULL) && (m_primFieldLength > 0))
  {
    delete[] m_primField; m_primField = NULL;
  }
  m_primFieldLength = 0;
  if ((m_secField != NULL) && (m_secFieldLength > 0))
  {
    delete[] m_secField; m_secField = NULL;
  }
  m_secFieldLength = 0;
}


/* ************************************************************************* */
/*     Implementation of class UserIdentityNegotiationSubItemAC              */
/* ************************************************************************* */

// Constructor
UserIdentityNegotiationSubItemAC::UserIdentityNegotiationSubItemAC()
: m_serverRsp(NULL), m_rspLength(0)
{
}


// Returns that this subitem is part of an acknowledge message
unsigned char UserIdentityNegotiationSubItemAC::pduType() const
{
  return DUL_TYPEASSOCIATEAC;
}

// Clear all data and free memory
void UserIdentityNegotiationSubItemAC::clear()
{
  if (m_serverRsp != NULL)
  {
    delete[] m_serverRsp; m_serverRsp = NULL;
  }
   m_rspLength = 0;
}


// Returns server response. Memory must be freed by caller.
Uint16
UserIdentityNegotiationSubItemAC::getServerResponse(char*& targetBuffer,
                                                    Uint16& resultLen) const
{
  if ((m_rspLength == 0) || (m_serverRsp == NULL))
  {
    targetBuffer = NULL;
    resultLen = 0;
    return 0;
  }
  targetBuffer = new char[m_rspLength];
  memcpy(targetBuffer, m_serverRsp, m_rspLength);
  resultLen = m_rspLength;
  return resultLen;
}

// Sets server response. Buffer from "rsp" is copied.
void
UserIdentityNegotiationSubItemAC::setServerResponse(const char* rsp,
                                                    const Uint16& rspLen)
{
  if (m_serverRsp != NULL)
  {
    delete[] m_serverRsp;
    m_serverRsp = NULL;
  }
  m_rspLength = rspLen;
  if ((rspLen == 0) || (rsp == NULL))
    return;

  m_serverRsp = new char[rspLen];
  memcpy(m_serverRsp, rsp, rspLen);
}


// Computes and returns total length of the user item if streamed
OFCondition UserIdentityNegotiationSubItemAC::streamedLength(unsigned long& length) const
{
  // content length: server response length field (2) + length of server response field
  length = 2 + m_rspLength;
   // total length: (item type + reserved + item length field + item content length)
  length = 1 + 1 + 2 + length;
  return EC_Normal;
}


/// Stream the package into a byte stream for network transmission
OFCondition UserIdentityNegotiationSubItemAC::stream(unsigned char *targetBuffer,
                                                     unsigned long& lengthWritten) const
{
  if (OFstatic_cast(unsigned long, m_rspLength) + 2 > (unsigned long)65535 /* for response length field */)
  {
    char errbuf[500];
    sprintf(errbuf, "Length of User Identity response (%lu bytes) exceeds upper limit of 65535 bytes", (unsigned long)m_rspLength + 2);
    return makeDcmnetCondition(ASCC_CODINGERROR, OF_error, errbuf);
  }
  // compute total length of item when streamed
  streamedLength(lengthWritten);

  *targetBuffer++ = getItemType();
  *targetBuffer++ = getReserved();
  // value for item length field is = length of response value + length of length field (2)
  COPY_SHORT_BIG(OFstatic_cast(unsigned short, 2 + m_rspLength), targetBuffer);
  targetBuffer += 2;

  COPY_SHORT_BIG(m_rspLength, targetBuffer);
  targetBuffer += 2;

  (void) memcpy(targetBuffer, m_serverRsp, m_rspLength);
  targetBuffer += m_rspLength;

  return EC_Normal;
}


// Parase User Identity sub item from buffer. Buffer has to start with item type field.
OFCondition UserIdentityNegotiationSubItemAC::parseFromBuffer(unsigned char *readBuffer,
                                                              unsigned long &bytesRead,
                                                              unsigned long availData)
{
  // We need at least 6 bytes for the fields that we always read
  if (availData < 6)
  {
      char buffer[256];
      sprintf(buffer, "DUL illegal user identify ac length %ld. Need at least 6 bytes for"
              " user identify ac", availData);
      return makeDcmnetCondition(DULC_ILLEGALPDULENGTH, OF_error, buffer);
  }

  bytesRead = 0;
  readBuffer += 2; // Skip "item type" and "reserved" field
  unsigned short itemLength = 0;
  EXTRACT_SHORT_BIG(readBuffer, itemLength);
  readBuffer += 2; // Skip "item length

  EXTRACT_SHORT_BIG(readBuffer, m_rspLength);
  readBuffer += 2;

  // Check if itemLength and m_rspLength are valid and we don't read past the
  // end of any buffer
  if (availData - 4 < itemLength || itemLength < 2 || itemLength - 2 < m_rspLength)
  {
      char buffer[256];
      sprintf(buffer, "DUL illegal user identify ac length %ld. Info claims to be %hd bytes. "
              "Response claims to be %hd bytes.", availData, itemLength, m_rspLength);
      return makeDcmnetCondition(DULC_ILLEGALPDULENGTH, OF_error, buffer);
  }

  if (m_rspLength > 0)
  {
    m_serverRsp = new char[m_rspLength];
    memcpy(m_serverRsp, readBuffer, m_rspLength);
    readBuffer += m_rspLength;
  }

  bytesRead = 6 + m_rspLength; // 2+2+2+ response length
  return EC_Normal;
}


// Dumps contents of this User Identity sub item to output stream
void UserIdentityNegotiationSubItemAC::dump(STD_NAMESPACE ostream& outstream) const
{
  outstream << "  Server Response (not dumped) length: " << m_rspLength << OFendl;
}


// Assignment operator
UserIdentityNegotiationSubItemAC& UserIdentityNegotiationSubItemAC::operator= (const UserIdentityNegotiationSubItemAC& rhs)
{
  this->clear();
  if (rhs.m_rspLength > 0)
  {
    if (rhs.getServerResponse(m_serverRsp, m_rspLength) != rhs.m_rspLength)
    {
      m_serverRsp = NULL; m_rspLength = 0;
    }
  } else m_rspLength = 0;
  return *this;
}


// Copy constructor
UserIdentityNegotiationSubItemAC::UserIdentityNegotiationSubItemAC(const UserIdentityNegotiationSubItemAC& rhs) :
  UserIdentityNegotiationSubItem(rhs)
{
  *this = rhs;
}

// Destructor, frees some memory
UserIdentityNegotiationSubItemAC::~UserIdentityNegotiationSubItemAC()
{
  if ( (m_serverRsp != NULL) && (m_rspLength > 0) )
  {
    delete[] m_serverRsp; m_serverRsp = NULL;
  }
  m_rspLength = 0;
}

/*
** CVS/RCS Log:
** $Log: dcuserid.cc,v $
** Revision 1.9  2010-11-01 10:42:44  uli
** Fixed some compiler warnings reported by gcc with additional flags.
**
** Revision 1.8  2010-10-14 13:14:28  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.7  2010-10-05 11:01:04  uli
** Removed a unused copy-constructor and a unused operator=.
**
** Revision 1.6  2010-10-05 10:15:20  uli
** Fixed all remaining warnings from -Wall -Wextra -pedantic.
**
** Revision 1.5  2010-09-14 11:42:14  uli
** Verify the length fields in the PDUs that we receive.
**
** Revision 1.4  2009-11-18 11:53:59  uli
** Switched to logging mechanism provided by the "new" oflog module.
**
** Revision 1.3  2008-10-07 09:08:15  onken
** Fixed possible memory leak in user identity classes and added code for
** accessing user identity from the server's side. Thanks to "Pim"
**
** Revision 1.2  2008-04-17 16:09:13  onken
** Added some const definitions to functions.
**
** Revision 1.1  2008-04-17 15:27:35  onken
** Reworked and extended User Identity Negotiation code.
**
*/
