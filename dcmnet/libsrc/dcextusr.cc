/*
**
** Author: Michael Onken
**
** Module: dcmnet
**
** Purpose:
**   Extended Negotiation of User Identity for A-ASSOCIATE (Supp. 99)
**
** Last Update:         $Author: onken $
** Update Date:         $Date: 2007-09-07 08:49:29 $
** Source File:         $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/libsrc/Attic/dcextusr.cc,v $
** CVS/RCS Revision:    $Revision: 1.1 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/dcextusr.h"
#include "dcmtk/dcmnet/dul.h"
#include "dulstruc.h"

/* ************************************************************************* */
/*       Implementation of class ExtendedNegotiationUserIdentitySubItem      */
/* ************************************************************************* */

ExtendedNegotiationUserIdentitySubItem::ExtendedNegotiationUserIdentitySubItem() 
  : itemType (0x58), reserved1(0), itemLength(0)
{
}

ExtendedNegotiationUserIdentitySubItem& ExtendedNegotiationUserIdentitySubItem::operator= (const ExtendedNegotiationUserIdentitySubItem& rhs)
{
  reserved1 = rhs.reserved1;
  itemLength = rhs.itemLength;
  return *this;
}

ExtendedNegotiationUserIdentitySubItem::ExtendedNegotiationUserIdentitySubItem(const ExtendedNegotiationUserIdentitySubItem& rhs) : itemType(DUL_TYPEEXTENDEDNEGOTIATIONUSERIDENTITY)
{
  *this = rhs;
}


/* ************************************************************************* */
/*     Implementation of class ExtendedNegotiationUserIdentitySubItemRQ      */
/* ************************************************************************* */

ExtendedNegotiationUserIdentitySubItemRQ::ExtendedNegotiationUserIdentitySubItemRQ() :
    userIdentityType(0)
  , positiveResponseRequested(0)
  , primaryFieldLength(0)
  , primaryField("")
  , secondaryFieldLength(0)
  , secondaryField("")
  {
  }

// Returns that this subitem is part of a request
unsigned char ExtendedNegotiationUserIdentitySubItemRQ::type() const 
{
  return DUL_TYPEASSOCIATERQ;
}

// Stream the package into a byte stream for network transmission
OFCondition ExtendedNegotiationUserIdentitySubItemRQ::stream(unsigned char *targetBuffer, 
                                                             unsigned long& lengthWritten)
{
  // recompute the length fields
  constructItem(lengthWritten);

  *targetBuffer++ = itemType;
  *targetBuffer++ = reserved1;
  COPY_SHORT_BIG(itemLength, targetBuffer);
  targetBuffer += 2;

  *targetBuffer++ = userIdentityType;
  *targetBuffer++ = positiveResponseRequested;

  COPY_SHORT_BIG(primaryFieldLength, targetBuffer);
  targetBuffer += 2;

  (void) memcpy(targetBuffer, primaryField.c_str(), primaryFieldLength);
  targetBuffer += primaryFieldLength;
  
  COPY_SHORT_BIG(secondaryFieldLength, targetBuffer);
  targetBuffer += 2;

  // If length is equal to zero, nothing is copied. Should be as fast as if statement
  (void) memcpy(targetBuffer, secondaryField.c_str(), secondaryFieldLength);
  targetBuffer += secondaryFieldLength;

  return EC_Normal;
}

// Calculates length values and returns total length of the user item if streamed 
OFCondition ExtendedNegotiationUserIdentitySubItemRQ::constructItem(unsigned long& length)  
{
  // userIdentityType + positive Response Requested + primary field length + primary field (+ 2nd field length + 2nd field)
  primaryFieldLength = primaryField.length();
  secondaryFieldLength = secondaryField.length();
  itemLength = 1 + 1 + 2 + primaryFieldLength;
  if (userIdentityType == 2)
  {
    secondaryFieldLength = secondaryField.length();  
    itemLength += 2 + secondaryFieldLength;
  }

  // add complete length of item (item type + reserved + item length) to total length
  length = 4 + itemLength;

  return EC_Normal;
}

// Parse User Identity request from read buffer. Buffer must start with item type field.
OFCondition ExtendedNegotiationUserIdentitySubItemRQ::parseFromBuffer(unsigned char *readBuffer, 
                                                                      unsigned long &bytesRead)
{
  bytesRead = 0;
  readBuffer += 2; // Skip "item type" and "reserved" field
  EXTRACT_SHORT_BIG(readBuffer, itemLength);
  readBuffer += 2;

  userIdentityType = *readBuffer;
  readBuffer++;

  positiveResponseRequested = *readBuffer;
  readBuffer++;

  EXTRACT_SHORT_BIG(readBuffer, primaryFieldLength);
  readBuffer += 2;

  primaryField.append((const char*)readBuffer, primaryFieldLength);
  readBuffer += primaryFieldLength;

  bytesRead = 8 + primaryFieldLength; // 2+2+1+1+2+primary field length

  // only user identity type 2 (Username + password authentication) requires second value field
  if (userIdentityType == 2)
  {
    EXTRACT_SHORT_BIG(readBuffer, secondaryFieldLength);
    readBuffer += 2;
    secondaryField.append((const char*)readBuffer, secondaryFieldLength);
    readBuffer += secondaryFieldLength;
    bytesRead += secondaryFieldLength + 2;
  }
  return EC_Normal;
}

// Dump User Identity sub item to specified output stream
void ExtendedNegotiationUserIdentitySubItemRQ::dump(STD_NAMESPACE ostream& outstream) const
{
  if (userIdentityType == 0)
    outstream << "none";
  else
  {
    if (userIdentityType == 1)
    {  outstream << "  Authentication mode 1: Username" << OFendl << 
                    "  Username: " << primaryField << OFendl;
    }
    else if (userIdentityType == 2)
    {
      outstream << "  Authentication mode 2: Username/Password" << OFendl << 
                   "  Username: " << primaryField << OFendl << 
                   "  Password: " << secondaryField << OFendl;
    }
    else if (userIdentityType == 3)
    {
      outstream << "  Authentication mode 3: Kerberos" << OFendl << 
                   "  Kerberos Service Ticket: " << primaryField << OFendl;
    }
    else if (userIdentityType == 4)
    {
      outstream << "  Authentication mode 2: SAML" << OFendl << 
                   "  SAML Assertion: " << primaryField << OFendl;
    }
    else 
    {
      outstream << "  Authentication mode: Unknown " << OFendl << 
                   "  First value:  " << primaryField << OFendl << 
                   "  Second Value: " << secondaryField << OFendl;
    }
    outstream << "  Positive Response requested: " << ((positiveResponseRequested != 0) ? "Yes" : "No") << OFendl;
  }
  
}

// Assignment operator
ExtendedNegotiationUserIdentitySubItemRQ& ExtendedNegotiationUserIdentitySubItemRQ::operator= (const ExtendedNegotiationUserIdentitySubItemRQ& rhs)
{
  userIdentityType = rhs.userIdentityType;
  positiveResponseRequested = rhs.positiveResponseRequested;
  primaryFieldLength = rhs.primaryFieldLength;
  primaryField = rhs.primaryField;
  secondaryFieldLength = rhs.secondaryFieldLength;
  secondaryField = rhs.secondaryField;
  return *this;
}

// Copy Constructor
ExtendedNegotiationUserIdentitySubItemRQ::ExtendedNegotiationUserIdentitySubItemRQ(const ExtendedNegotiationUserIdentitySubItemRQ& rhs)
{
  *this = rhs;
}


/* ************************************************************************* */
/*     Implementation of class ExtendedNegotiationUserIdentitySubItemAC      */
/* ************************************************************************* */

// Constructor
ExtendedNegotiationUserIdentitySubItemAC::ExtendedNegotiationUserIdentitySubItemAC()
: serverRspLength(0)
, serverRsp(0)
{
}

// Returns that this subitem is part of an acknowledge message
unsigned char ExtendedNegotiationUserIdentitySubItemAC::type() const 
{
  return DUL_TYPEASSOCIATEAC;
}

// Computes length fields and returns total length of the user item if streamed
OFCondition ExtendedNegotiationUserIdentitySubItemAC::constructItem(unsigned long& length) 
{
  serverRspLength = serverRsp.length();
  itemLength = 2 + serverRspLength;
  
   // calculate total length of item (item type + reserved + item length) 
  length = 4 + itemLength;

  return EC_Normal;
}

/// Stream the package into a byte stream for network transmission
OFCondition ExtendedNegotiationUserIdentitySubItemAC::stream(unsigned char *targetBuffer, 
                                                             unsigned long& lengthWritten)
{
  // recompute the length fields
  constructItem(lengthWritten);

  *targetBuffer++ = itemType;
  *targetBuffer++ = reserved1;
  COPY_SHORT_BIG(itemLength, targetBuffer);
  targetBuffer += 2;

  COPY_SHORT_BIG(serverRspLength, targetBuffer);
  targetBuffer += 2;

  (void) memcpy(targetBuffer, serverRsp.c_str(), serverRspLength);
  targetBuffer += serverRspLength;

  return EC_Normal;
}

// Parase User Identity sub item from buffer. Buffer has to start with item type field.
OFCondition ExtendedNegotiationUserIdentitySubItemAC::parseFromBuffer(unsigned char *readBuffer, 
                                                                      unsigned long &bytesRead)
{
  bytesRead = 0;
  readBuffer += 2; // Skip "item type" and "reserved" field
  EXTRACT_SHORT_BIG(readBuffer, itemLength);
  readBuffer += 2;

  EXTRACT_SHORT_BIG(readBuffer, serverRspLength);
  readBuffer += 2;

  serverRsp.append((const char*)readBuffer, serverRspLength);
  readBuffer += serverRspLength;

  bytesRead = 6 + serverRspLength; // 2+2+2+2+serverRspLength
  return EC_Normal;
}

// Dumps contents of this User Identity sub item to output stream
void ExtendedNegotiationUserIdentitySubItemAC::dump(STD_NAMESPACE ostream& outstream) const
{
  outstream << "  Server Response: " << serverRsp << OFendl;
}

// Assignment operator
ExtendedNegotiationUserIdentitySubItemAC& ExtendedNegotiationUserIdentitySubItemAC::operator= (const ExtendedNegotiationUserIdentitySubItemAC& rhs)
{
  serverRspLength = rhs.serverRspLength;
  serverRsp = rhs.serverRsp;
  return *this;
}

// Copy constructor
ExtendedNegotiationUserIdentitySubItemAC::ExtendedNegotiationUserIdentitySubItemAC(const ExtendedNegotiationUserIdentitySubItemAC& rhs)
{
  *this = rhs;
}

/*
** CVS/RCS Log:
** $Log: dcextusr.cc,v $
** Revision 1.1  2007-09-07 08:49:29  onken
** Added basic support for Extended Negotiation of User Identity.
**
*/
