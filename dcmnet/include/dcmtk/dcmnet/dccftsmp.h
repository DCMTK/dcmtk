/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Module:  dcmnet
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: 
 *    class DcmTransferSyntaxMap
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:17:22 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCCFTSMP_H
#define DCCFTSMP_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oflist.h"   /* for class OFList<> */
#include "dcmtk/ofstd/ofcond.h"   /* for class OFCondition */
#include "dcmtk/dcmnet/dcmsmap.h"  /* for class DcmSimpleMap<> */
#include "dcmtk/dcmnet/dccfuidh.h" /* for class DcmUIDHandler */


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
 * Revision 1.3  2010-10-14 13:17:22  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.2  2005/12/08 16:02:14  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2003/06/10 14:27:33  meichel
 * Initial release of class DcmAssociationConfiguration and support
 *   classes. This class maintains a list of association negotiation
 *   profiles that can be addressed by symbolic keys. The profiles may
 *   be read from a configuration file.
 *
 *
 */
