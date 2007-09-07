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
** Update Date:         $Date: 2007-09-07 08:49:12 $
** Source File:         $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/include/dcmtk/dcmnet/Attic/dcextusr.h,v $
** CVS/RCS Revision:    $Revision: 1.1 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCEXTUSR_H
#define DCEXTUSR_H

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmdata/dctk.h"

#define DUL_TYPEEXTENDEDNEGOTIATIONUSERIDENTITY (unsigned char)0x58

/** Abstract base class for Extended Negotiation of User Identity user items
 */
class ExtendedNegotiationUserIdentitySubItem {
  
public:
  
  /** Constructor
   */
  ExtendedNegotiationUserIdentitySubItem();

  /** Denotes whether instance is part of a request (DUL_TYPEASSOCIATERQ)
   *  or acknowledge PDU (DUL_TYPEASSOCIATEAC)
   *  @return Constant for either request of ack message.
   */
  virtual unsigned char type() const =0;

  /** Computes length of item if streamed into buffer
    * @param length - [out] The length of the item if streamed
    * @return EC_Normal if successful, error code otherwise 
    */
  virtual OFCondition constructItem(unsigned long& length) = 0;

  /** Parse sub item from buffer. The buffer has to start with the correct user 
   *  item type.
   *  @param readBuffer - [in/out] The buffer to read from. The pointer to the buffer
   *                      gets incremented by "bytesRead" bytes.
   *  @param bytesRead  - [out] Number of bytes read by this function
   *  @return EC_Normal if successful, error code otherwise
   */
  virtual OFCondition parseFromBuffer(unsigned char *readBuffer, 
                                      unsigned long& bytesRead) =0;

  /** Stream the package into a byte stream for network transmission 
   *  @param targetBuffer - [out] The buffer to stream to.
   *  @param lengthWritten - [out] Number of bytes written to buffer
   *  @return EC_Normal, if successful, error code otherwise
   */
  virtual OFCondition stream(unsigned char *targetBuffer, 
                             unsigned long& lengthWritten) =0;

  /** Dump content of this user identity sub item to output stream 
    * @param outstream - [out] The stream to dump to
    * @return none
    */
  virtual void dump(STD_NAMESPACE ostream& outstream) const =0;

  /** Assignment operator, does a deep copy of a class instance
   * @param rhs - [in] Right hand side of assignment
   * @return none
   */
  ExtendedNegotiationUserIdentitySubItem& operator= (const ExtendedNegotiationUserIdentitySubItem& rhs);

  /** Copy constructor, does a deep copy of a class instance
   * @param rhs - [in] The class instance to copy from
   * @return none
   */
  ExtendedNegotiationUserIdentitySubItem(const ExtendedNegotiationUserIdentitySubItem& rhs);

  /** Destructor, nothing to clean up 
   */
  virtual ~ExtendedNegotiationUserIdentitySubItem() {}
  
  /// Item type of this user item. For Extended Negotiation of User Identity
  /// this is always 0x58
  const unsigned char itemType;

  /// Reserved field, should be always sent with value 0 (default)
  unsigned char reserved1;

  /// Item length of this user item 
  // (total item length - item type field - reserved1)
  unsigned short itemLength;

};

/** Class for Extended Negotiation of User Identity request user item
 */
class ExtendedNegotiationUserIdentitySubItemRQ : public ExtendedNegotiationUserIdentitySubItem {
  
public:
  
  /** Constructor
   */
  ExtendedNegotiationUserIdentitySubItemRQ();

  /** Denotes that instance is part of a request (DUL_TYPEASSOCIATERQ)
   *  @return DUL_TYPEASSOCIATERQ
   */
  unsigned char type() const;

  /** Stream the package into a byte stream for network transmission 
   *  @param targetBuffer - [out] The buffer to stream to.
   *  @param lengthWritten - [out] Number of bytes written to buffer
   *  @return EC_Normal, if successful, error code otherwise
   */
  OFCondition stream(unsigned char *targetBuffer, 
                     unsigned long& lengthWritten);

  /** Computes length of item if streamed into buffer
    * @param length - [out] The length of the item if streamed
    * @return EC_Normal if successful, error code otherwise 
    */
  OFCondition constructItem(unsigned long& length);

  /** Parse sub item from buffer. The buffer has to start with the correct user 
   *  item type.
   *  @param readBuffer - [in/out] The buffer to read from. The pointer to the buffer
   *                      gets incremented by "bytesRead" bytes.
   *  @param bytesRead  - [out] Number of bytes read by this function
   *  @return EC_Normal if successful, error code otherwise
   */
  OFCondition parseFromBuffer(unsigned char *readBuffer, 
                              unsigned long &bytesRead);

  /** Dump content of this user identity sub item to output stream 
    * @param outstream - [out] The stream to dump to
    * @return none
    */
  void dump(STD_NAMESPACE ostream& outstream) const;

  /** Assignment operator, does a deep copy of a class instance
   * @param rhs - [in] Right hand side of assignment
   * @return none
   */
  ExtendedNegotiationUserIdentitySubItemRQ& operator= (const ExtendedNegotiationUserIdentitySubItemRQ& rhs);
  
  /** Copy constructor, does a deep copy of a class instance
   * @param rhs - [in] The class instance to copy from
   * @return none
   */
  ExtendedNegotiationUserIdentitySubItemRQ(const ExtendedNegotiationUserIdentitySubItemRQ& rhs);

  /** Destructor, nothing to clean up 
   */
  ~ExtendedNegotiationUserIdentitySubItemRQ() {}

  /// User Identity Type: 1 (username), 2 (username/password), 3 (kerberos), 4 (SAML)
  unsigned char userIdentityType;            
  
  /// If 1, positive response is requested from server. Set to 0 otherwise
  unsigned char positiveResponseRequested;   
  
  /// Primary field length. Calculated automatically from primary field value
  unsigned short primaryFieldLength;         
  
  /// Primary value field
  OFString primaryField;                     
  
  /// Secondary field length. Calculated automatically from secondary field value
  unsigned short secondaryFieldLength;      
  
  /// Secondary value field
  OFString secondaryField; 
};


class ExtendedNegotiationUserIdentitySubItemAC : public ExtendedNegotiationUserIdentitySubItem {
  
public:
  
  /** Constructor
   */
  ExtendedNegotiationUserIdentitySubItemAC();
  
  /** Denotes that instance is part of a request (DUL_TYPEASSOCIATEAC)
   *  @return DUL_TYPEASSOCIATEAC
   */
  unsigned char type() const;

  /** Computes length of item if streamed into buffer
    * @param length - [out] The length of the item if streamed
    * @return EC_Normal if successful, error code otherwise 
    */
  OFCondition constructItem(unsigned long& length);

  /** Stream the package into a byte stream for network transmission 
   *  @param targetBuffer - [out] The buffer to stream to.
   *  @param lengthWritten - [out] Number of bytes written to buffer
   *  @return EC_Normal, if successful, error code otherwise
   */
  OFCondition stream(unsigned char *targetBuffer, 
                     unsigned long& lengthWritten);

  /** Parse sub item from buffer. The buffer has to start with the correct user 
   *  item type.
   *  @param readBuffer - [in/out] The buffer to read from. The pointer to the buffer
   *                      gets incremented by "bytesRead" bytes.
   *  @param bytesRead  - [out] Number of bytes read by this function
   *  @return EC_Normal if successful, error code otherwise
   */
  OFCondition parseFromBuffer(unsigned char *readBuffer, 
                              unsigned long &bytesRead);

  /** Dump content of this user identity sub item to output stream 
    * @param outstream - [out] The stream to dump to
    * @return none
    */
  void dump(STD_NAMESPACE ostream& outstream) const;

  /** Assignment operator, does a deep copy of a class instance
   * @param rhs - [in] Right hand side of assignment
   * @return none
   */
  ExtendedNegotiationUserIdentitySubItemAC& operator= (const ExtendedNegotiationUserIdentitySubItemAC& rhs);

  /** Copy constructor, does a deep copy of a class instance
   * @param rhs - [in] The class instance to copy from
   * @return none
   */
  ExtendedNegotiationUserIdentitySubItemAC(const ExtendedNegotiationUserIdentitySubItemAC& rhs);

  /** Destructor, nothing to clean up 
   */
  ~ExtendedNegotiationUserIdentitySubItemAC() {}

  /// Length of server response field
  unsigned short serverRspLength;    

  /// Server response to user identity request
  OFString serverRsp; 

};
#endif // DCEXTUSR_H

/*
** CVS/RCS Log:
** $Log: dcextusr.h,v $
** Revision 1.1  2007-09-07 08:49:12  onken
** Added basic support for Extended Negotiation of User Identity.
**
**
**
*/
