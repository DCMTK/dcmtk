/*
 *
 *  Copyright (C) 2008-2011, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 * Author: Michael Onken
 *
 * Module: dcmnet
 *
 * Purpose:
 *   User Identity Negotiation for A-ASSOCIATE (Supp. 99)
 *
 * Last Update:         $Author: onken $
 * Update Date:         $Date: 2011-08-08 12:27:46 $
 * CVS/RCS Revision:    $Revision: 1.10 $
 * Status:              $State: Exp $
 *
 * CVS/RCS Log at end of file
 *
 */

#ifndef DCUSERID_H
#define DCUSERID_H

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofcond.h"

/// User Identity Negotiation always identifies with 0x58
#define DUL_TYPENEGOTIATIONOFUSERIDENTITY (unsigned char)0x58

/// Mode of User Identity Negotiation
enum T_ASC_UserIdentityNegotiationMode
{
    ASC_USER_IDENTITY_NONE            = 0,
    ASC_USER_IDENTITY_USER            = 1,
    ASC_USER_IDENTITY_USER_PASSWORD   = 2,
    ASC_USER_IDENTITY_KERBEROS        = 3,
    ASC_USER_IDENTITY_SAML            = 4,
    ASC_USER_IDENTITY_UNKNOWN
};


/** Abstract base class for User Identity Negotiation user items
 */
class UserIdentityNegotiationSubItem {

public:

  /** Constructor
   */
  UserIdentityNegotiationSubItem();

  /** Copy constructor. Needed to keep some compilers quiet.
   *  @param src item that should be copied from.
   */
  UserIdentityNegotiationSubItem(const UserIdentityNegotiationSubItem& src) : 
    m_itemType(src.m_itemType), 
    m_reserved(src.m_reserved)
  {
  }

  /** Denotes whether instance is part of a request (DUL_TYPEASSOCIATERQ)
   *  or acknowledge PDU (DUL_TYPEASSOCIATEAC)
   *  @return Constant for either request of ack message.
   */
  virtual unsigned char pduType() const =0;

  /** Returns item type, which is constantly 0x58
    * @return The item type = 0x58
    */
  virtual unsigned char getItemType() const {return m_itemType;}

  /** Returns reserved field, which is 0 in this implementation (see DICOM
    * standard)
    * @return Content of reserved field = 0
    */
  virtual unsigned char getReserved() const {return m_reserved;}

  /** Computes total length of item if streamed to a buffer
    * @param length - [out] The total ength of the item in bytes
    * @return EC_Normal if successful, error code otherwise
    */
  virtual OFCondition streamedLength(unsigned long& length) const = 0;

  /** Parse item from buffer. The buffer has to start with the correct user
   *  item type.
   *  @param readBuffer - [in/out] The buffer to read from. The pointer to the buffer
   *                      gets incremented by "bytesRead" bytes.
   *  @param bytesRead  - [out] Number of bytes read by this function
   *  @param availData  - [in] Size of the buffer.
   *  @return EC_Normal if successful, error code otherwise
   */
  virtual OFCondition parseFromBuffer(unsigned char *readBuffer,
                                      unsigned long& bytesRead,
                                      unsigned long availData) =0;

  /** Stream the package into a byte stream for network transmission
   *  @param targetBuffer - [out] The buffer to stream to.
   *  @param lengthWritten - [out] Number of bytes written to buffer
   *  @return EC_Normal, if successful, error code otherwise
   */
  virtual OFCondition stream(unsigned char *targetBuffer,
                             unsigned long& lengthWritten) const =0;

  /** Clears member variables and frees memory
   */
  virtual void clear() =0;

  /** Dump content of this user identity sub item to output stream
    * @param outstream - [out] The stream to dump to
    */
  virtual void dump(STD_NAMESPACE ostream& outstream) const =0;

  /** Destructor, nothing to clean up
   */
  virtual ~UserIdentityNegotiationSubItem() {}

private:

  /** Undefined private assignment operator. Needed to keep some compilers quiet.
   */
  UserIdentityNegotiationSubItem& operator= (const UserIdentityNegotiationSubItem&);
  
  /// Item type of this user item. For User Identity Negotiation
  /// this is always 0x58
  const unsigned char m_itemType;

  /// Reserved field, should be always sent with value 0 (default)
  const unsigned char m_reserved;

};


/** Class for User Identity Negotiation request user item
 */
class UserIdentityNegotiationSubItemRQ : public UserIdentityNegotiationSubItem {

public:

  /** Constructor
   */
  UserIdentityNegotiationSubItemRQ();

  /** Denotes that instance is part of a request (DUL_TYPEASSOCIATERQ)
   *  @return DUL_TYPEASSOCIATERQ
   */
  unsigned char pduType() const;

  /** Clears member variables and frees memory
   */
  virtual void clear();

  /** Sets identity type to be used.
   *  At this time, user, user/password, kerberos and SAML are known.
   *  @param mode - [in] the identification mode
   */
  void setIdentityType(const T_ASC_UserIdentityNegotiationMode& mode);

  /** Returns identity type that will be used.
   *  At this time, user, user/password, kerberos and SAML are known.
   *  @return the identification mode
   */
   T_ASC_UserIdentityNegotiationMode getIdentityType();

  /** Sets content of primary field.
   *  @param buffer - [in] Content of primary field.
   *  @param length - [in] Length of buffer
   */
  void setPrimField(const char *buffer,
                    const Uint16& length);

  /** Sets content of secondary field.
   *  @param buffer - [in] Content of secondary field.
   *  @param length - [in ] Length of buffer
   */
  void setSecField(const char *buffer,
                   const Uint16& length);

  /** Returns content of primary field. Memory is allocated by this function
   *  and must be freed by the caller.
   *  @param resultBuf - [out] Content of primary field. NULL if not set. Memory
   *                     of buffer must be freed by the caller.
   *  @param resultLen - [out] Length of returned buffer
   *  @return Content of the primary field
   */
  Uint16 getPrimField(char*& resultBuf,
                      Uint16& resultLen) const;

  /** Returns content of secondary field. Memory is allocated by this function
   *  and must be freed by the caller.
   *  @param resultBuf - [out] Content of secondary field. NULL if not set. Memory
   *                     of buffer must be freed by the caller.
   *  @param resultLen - [out] Length of returned buffer
   *  @return Content of the secondary field.
   */
  Uint16 getSecField(char*& resultBuf,
                     Uint16& resultLen) const;

  /** Enables/disables requesting a positive response from the server.
   *  @param reqPosRsp - [in] If true, a positive response is requested
   */
  void setReqPosResponse(const OFBool& reqPosRsp);


  /** Informs (the server) whether a positive response was requested.
   *  @return OFTrue if  a response was requested
   */
  OFBool isPosResponseRequested()
  {
     return (m_posRspRequested != 0) ? OFTrue: OFFalse;
  }

  /** Stream the package into a byte stream for network transmission
   *  @param targetBuffer  - [out] The buffer to stream to. Must be
   *                         big enough (not allocated in function).
   *  @param lengthWritten - [out] Number of bytes written to buffer
   *  @return EC_Normal, if successful, error code otherwise
   */
  OFCondition stream(unsigned char *targetBuffer,
                     unsigned long& lengthWritten) const;

  /** Computes total length of item if streamed into buffer
    * @param length - [out] The total length of the item in bytes
    * @return EC_Normal if successful, error code otherwise
    */
  OFCondition streamedLength(unsigned long& length) const;

  /** Parse sub item from buffer. The buffer has to start with the correct user
   *  item type.
   *  @param readBuffer - [in] The buffer to read from. The pointer to the buffer
   *                      gets incremented by "bytesRead" bytes.
   *  @param bytesRead  - [out] Number of bytes read by this function
   *  @param availData  - [in] Size of the buffer.
   *  @return EC_Normal if successful, error code otherwise
   */
  OFCondition parseFromBuffer(unsigned char *readBuffer,
                              unsigned long &bytesRead,
                              unsigned long availData);

  /** Dump content of this user identity sub item to output stream
    * @param outstream - [out] The stream to dump to
    */
  void dump(STD_NAMESPACE ostream& outstream) const;

  /** Assignment operator, does a deep copy of a class instance
   * @param rhs - [in] Right hand side of assignment
   */
  UserIdentityNegotiationSubItemRQ& operator= (const UserIdentityNegotiationSubItemRQ& rhs);

  /** Copy constructor, does a deep copy of a class instance
   * @param rhs - [in] The class instance to copy from
   */
  UserIdentityNegotiationSubItemRQ(const UserIdentityNegotiationSubItemRQ& rhs);

  /** Destructor, nothing to clean up
   */
  ~UserIdentityNegotiationSubItemRQ();

private:

  /// User Identity Type: 1 (username), 2 (username/password), 3 (kerberos), 4 (SAML)
  T_ASC_UserIdentityNegotiationMode m_userIdentityType;
  /// If 1, positive response is requested from server. Set to 0 otherwise
  unsigned char m_posRspRequested;
  /// Buffer for primary value field
  char* m_primField;
  /// Length of primary value field in bytes
  Uint16 m_primFieldLength;
  /// Buffer for secondary value field
  char* m_secField;
  /// Length of primary value field in bytes
  Uint16 m_secFieldLength;
};


class UserIdentityNegotiationSubItemAC : public UserIdentityNegotiationSubItem {

public:

  /** Constructor, creates an empty user identity response structure
   */
  UserIdentityNegotiationSubItemAC();

  /** Denotes that instance is part of a request (DUL_TYPEASSOCIATEAC)
   *  @return DUL_TYPEASSOCIATEAC
   */
  unsigned char pduType() const;

  /** Clears member variables and frees memory
   */
  virtual void clear();

  /** Sets server response value
   *  @param rsp - [in] Content of server response value (copied by function)
   *  @param rspLen - [in ] Length of buffer
   */
  void setServerResponse(const char* rsp,
                         const Uint16& rspLen);

  /** Returns content of server response field Memory is allocated by this
   *  function and must be freed by the caller.
   *  @param targetBuffer - [out] Content of server response field. NULL if not set.
   *                        Memory of buffer must be freed by the caller.
   *  @param resultLen - [out] Length of returned buffer
   *  @return The server response field
   */
  Uint16 getServerResponse(char*& targetBuffer,
                           Uint16& resultLen) const;

  /** Computes total length of item if streamed into buffer
    * @param length - [out] The length of the item if streamed
    * @return EC_Normal if successful, error code otherwise
    */
  OFCondition streamedLength(unsigned long& length) const;

  /** Stream the package into a byte stream for network transmission
   *  @param targetBuffer  - [out] The buffer to stream to.
   *  @param lengthWritten - [out] Number of bytes written to buffer
   *  @return EC_Normal, if successful, error code otherwise
   */
  OFCondition stream(unsigned char *targetBuffer,
                     unsigned long& lengthWritten) const;

  /** Parse sub item from buffer. The buffer has to start with the correct user
   *  item type.
   *  @param readBuffer - [in] The buffer to read from. The pointer to the buffer
   *                      gets incremented by "bytesRead" bytes.
   *  @param bytesRead  - [out] Number of bytes read by this function
   *  @param availData  - [in] Size of the buffer.
   *  @return EC_Normal if successful, error code otherwise
   */
  OFCondition parseFromBuffer(unsigned char *readBuffer,
                              unsigned long &bytesRead,
                              unsigned long availData);

  /** Dump content of this user identity sub item to output stream
    * @param outstream - [out] The stream to dump to
    */
  void dump(STD_NAMESPACE ostream& outstream) const;

  /** Assignment operator, does a deep copy of a class instance
   * @param rhs - [in] Right hand side of assignment
   */
  UserIdentityNegotiationSubItemAC& operator= (const UserIdentityNegotiationSubItemAC& rhs);

  /** Copy constructor, does a deep copy of a class instance
   * @param rhs - [in] The class instance to copy from
   */
  UserIdentityNegotiationSubItemAC(const UserIdentityNegotiationSubItemAC& rhs);

  /** Destructor, frees memory
   */
  ~UserIdentityNegotiationSubItemAC();

private:

  /// Buffer for server response
  char* m_serverRsp;
  /// Length of server response in bytes
  Uint16 m_rspLength;

};

#endif // DCUSERID_H

/*
** CVS/RCS Log:
** $Log: dcuserid.h,v $
** Revision 1.10  2011-08-08 12:27:46  onken
** Added further doxygen documentation.
**
** Revision 1.9  2011-08-08 12:10:09  onken
** Made assignment operator private and added public copy constructor.
** Fixed doxygen documentation.
**
** Revision 1.8  2011-08-08 09:55:42  onken
** Added dumb assignment operator in order to keep VS compilers quiet.
**
** Revision 1.7  2010-10-14 13:17:22  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.6  2010-10-05 11:01:04  uli
** Removed a unused copy-constructor and a unused operator=.
**
** Revision 1.5  2010-09-14 11:42:14  uli
** Verify the length fields in the PDUs that we receive.
**
** Revision 1.4  2009-09-29 14:34:16  uli
** Include only the needed headers in dcuserid.h and not dctk.h.
**
** Revision 1.3  2008-10-07 09:08:13  onken
** Fixed possible memory leak in user identity classes and added code for
** accessing user identity from the server's side. Thanks to "Pim"
**
** Revision 1.2  2008-04-17 16:09:32  onken
** Added some const definitions to functions.
**
** Revision 1.1  2008-04-17 15:28:33  onken
** Reworked and extended User Identity Negotiation code.
**
*/

