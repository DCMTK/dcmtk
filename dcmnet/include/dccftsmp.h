/*
 *
 *  Copyright (C) 1994-2003, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmnet
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: 
 *    class DcmTransferSyntaxMap
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2003-06-10 14:27:33 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/include/Attic/dccftsmp.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCCFTSMP_H
#define DCCFTSMP_H

#include "osconfig.h"
#include "oflist.h"   /* for class OFList<> */
#include "ofcond.h"   /* for class OFCondition */
#include "dcmsmap.h"  /* for class DcmSimpleMap<> */
#include "dccfuidh.h" /* for class DcmUIDHandler */


/** this helper class is a simple list of UID handler objects.
 *  Not intended for use by the end user.
 */
typedef OFList<DcmUIDHandler> DcmTransferSyntaxList;

/** this helper class maintains a map of transfer syntax lists.
 *  Not intended for use by the end user.
 */
class DcmTransferSyntaxMap
{
public:
  /// constructor
  DcmTransferSyntaxMap();

  /// destructor
  ~DcmTransferSyntaxMap();

  /** add new entry to list within map.
   *  If key is new, new list is created. Otherwise transfer syntax
   *  is appended to existing list.
   *  @param key map key
   *  @param transferSyntaxUID transfer syntax UID
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition add(
    const char *key,
    const char *transferSyntaxUID);

  /** checks if the key is known
   *  @param key key name, must not be NULL
   *  @return true if key is known, false otherwise
   */
  OFBool isKnownKey(const char *key) const;

  /** returns the list of transfer syntaxes identified by the given key
   *  @param key transfer syntax list to search
   *  @return pointer to transfer syntax list if found, NULL otherwise
   */
  const DcmTransferSyntaxList *getTransferSyntaxList(const char *key) const;

private:
  /// private undefined copy constructor
  DcmTransferSyntaxMap(const DcmTransferSyntaxMap& arg);

  /// private undefined copy assignment operator
  DcmTransferSyntaxMap& operator=(const DcmTransferSyntaxMap& arg);

  /// map of transfer syntax lists
  DcmSimpleMap<DcmTransferSyntaxList *> map_;

};

#endif

/*
 * CVS/RCS Log
 * $Log: dccftsmp.h,v $
 * Revision 1.1  2003-06-10 14:27:33  meichel
 * Initial release of class DcmAssociationConfiguration and support
 *   classes. This class maintains a list of association negotiation
 *   profiles that can be addressed by symbolic keys. The profiles may
 *   be read from a configuration file.
 *
 *
 */
