/*
 *
 *  Copyright (C) 1998-2001, OFFIS
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
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:50:51 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef SITSTAMP_H
#define SITSTAMP_H

#include "osconfig.h"

#ifdef WITH_OPENSSL

#include "sitypes.h"
#include "dcerror.h"

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
  virtual SI_E_Condition stamp(
    const unsigned char *inputData, 
    unsigned long inputDataSize) = 0;

  /** writes the current timestamp into an item of the Digital Signatures Sequence
   *  and creates the timestamp type information in the item.
   *  @param item item of the DigitalSignatureSQ to which the timestamp is written
   *  @return dcmdata E_Condition status code
   */
  virtual E_Condition write(DcmItem& item) = 0;

};

#endif
#endif

/*
 *  $Log: sitstamp.h,v $
 *  Revision 1.2  2001-06-01 15:50:51  meichel
 *  Updated copyright header
 *
 *  Revision 1.1  2000/11/07 16:49:00  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

