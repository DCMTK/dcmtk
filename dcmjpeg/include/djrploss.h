/*
 *
 *  Copyright (C) 1997-2001, OFFIS
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
 *  Module:  dcmjpeg
 *
 *  Author:  Norbert Olges, Marco Eichelberg
 *
 *  Purpose: representation parameter for lossy JPEG
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-11-13 15:56:29 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmjpeg/include/Attic/djrploss.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DJRPLOSS_H
#define DJRPLOSS_H

#include "osconfig.h"
#include "dcpixel.h" /* for class DcmRepresentationParameter */

/** representation parameter for lossy JPEG
 */
class DJ_RPLossy: public DcmRepresentationParameter
{
public:

  /** constructor
   *  @param aQuality quality factor
   */
  DJ_RPLossy(int aQuality=90);

  /// copy constructor
  DJ_RPLossy(const DJ_RPLossy& arg);

  /// destructor
  virtual ~DJ_RPLossy();
  
  /** this methods creates a copy of type DcmRepresentationParameter *
   *  it must be overweritten in every subclass.
   *  @return copy of this object
   */
  virtual DcmRepresentationParameter *clone() const;

  /** returns the class name as string.
   *  can be used in operator== as poor man's RTTI replacement.
   */
  virtual const char *className() const;

  /** compares an object to another DcmRepresentationParameter.
   *  Implementation must make sure that classes are comparable.
   *  @param arg representation parameter to compare with
   *  @return true if equal, false otherwise.
   */
  virtual OFBool operator==(const DcmRepresentationParameter &arg) const;

  /** returns the compression quality factor
   *  @return compression quality factor
   */
  int getQuality() const
  {
    return quality;
  }

private:

  /// compression quality factor
  int quality;

};


#endif

/*
 * CVS/RCS Log
 * $Log: djrploss.h,v $
 * Revision 1.1  2001-11-13 15:56:29  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
