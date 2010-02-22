/*
 *
 *  Copyright (C) 1998-2005, OFFIS
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
 *  Module: dcmsign
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiTimeStamp
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-02-22 11:39:54 $
 *  CVS/RCS Revision: $Revision: 1.6 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef SITSTAMP_H
#define SITSTAMP_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmsign/sitypes.h"

#ifdef WITH_OPENSSL

class DcmItem;
 
/** pure virtual base class for a timestamp client.
 *  Instances of derived classes are able to request timestamps from a timestamp service.
 *  Timestamps are not supported (yet).
 */
class SiTimeStamp
{    
public:

  /// default constructor
  SiTimeStamp() { }
  
  /// destructor
  virtual ~SiTimeStamp() { }

  /** takes a block of raw data and requests a time stamp for this raw data.
   *  @param inputData pointer to raw data
   *  @param inputDataSize length of raw data block in bytes
   *  @return status code
   */
  virtual OFCondition stamp(
    const unsigned char *inputData, 
    unsigned long inputDataSize) = 0;

  /** writes the current timestamp into an item of the Digital Signatures Sequence
   *  and creates the timestamp type information in the item.
   *  @param item item of the DigitalSignatureSQ to which the timestamp is written
   *  @return dcmdata OFCondition status code
   */
  virtual OFCondition write(DcmItem& item) = 0;

};

#endif
#endif

/*
 *  $Log: sitstamp.h,v $
 *  Revision 1.6  2010-02-22 11:39:54  uli
 *  Remove some unneeded includes.
 *
 *  Revision 1.5  2005-12-08 16:04:46  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.4  2003/06/04 14:21:03  meichel
 *  Simplified include structure to avoid preprocessor limitation
 *    (max 32 #if levels) on MSVC5 with STL.
 *
 *  Revision 1.3  2001/09/26 14:30:22  meichel
 *  Adapted dcmsign to class OFCondition
 *
 *  Revision 1.2  2001/06/01 15:50:51  meichel
 *  Updated copyright header
 *
 *  Revision 1.1  2000/11/07 16:49:00  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

