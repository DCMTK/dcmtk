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
 *
 *  Module:  dcmjpeg
 *
 *  Author:  Norbert Olges, Marco Eichelberg
 *
 *  Purpose: (STATUS: OK)
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-11-01 08:55:56 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djutils.h"
#include "dcmtk/dcmdata/dcdeftag.h"  /* for tag constants */
#include "dcmtk/dcmdata/dcitem.h"    /* for class DcmItem */

#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"

OFLogger DCM_dcmjpegGetLogger()
{
    // We don't just use a global variable, because constructors of globals are
    // executed in random order. This guarantees that the OFLogger is constructed
    // before first use.
    static OFLogger DCM_dcmjpegLogger = OFLog::getLogger("dcmtk.dcmjpeg");
    return DCM_dcmjpegLogger;
}

const OFConditionConst EJC_Suspension(                           OFM_dcmjpeg,  1, OF_error, "IJG codec suspension return"  );
const OFConditionConst EJC_IJG8_FrameBufferTooSmall(             OFM_dcmjpeg,  2, OF_error, "Buffer for decompressed image (8 bits/sample) too small"  );
const OFConditionConst EJC_IJG12_FrameBufferTooSmall(            OFM_dcmjpeg,  3, OF_error, "Buffer for decompressed image (12 bits/sample) too small"  );
const OFConditionConst EJC_IJG16_FrameBufferTooSmall(            OFM_dcmjpeg,  4, OF_error, "Buffer for decompressed image (16 bits/sample) too small"  );
const OFConditionConst EJC_UnsupportedPhotometricInterpretation( OFM_dcmjpeg,  5, OF_error, "Codec does not support this PhotometricInterpretation"  );
const OFConditionConst EJC_UnsupportedColorConversion(           OFM_dcmjpeg,  6, OF_error, "Codec does not support this kind of color conversion"  );

const OFCondition      EJ_Suspension(                           EJC_Suspension);
const OFCondition      EJ_IJG8_FrameBufferTooSmall(             EJC_IJG8_FrameBufferTooSmall);
const OFCondition      EJ_IJG12_FrameBufferTooSmall(            EJC_IJG12_FrameBufferTooSmall);
const OFCondition      EJ_IJG16_FrameBufferTooSmall(            EJC_IJG16_FrameBufferTooSmall);
const OFCondition      EJ_UnsupportedPhotometricInterpretation( EJC_UnsupportedPhotometricInterpretation);
const OFCondition      EJ_UnsupportedColorConversion(           EJC_UnsupportedColorConversion);
EP_Interpretation DcmJpegHelper::getPhotometricInterpretation(DcmItem *item)
{
  if (item)
  {
    OFString photometric;
    if ((item->findAndGetOFString(DCM_PhotometricInterpretation, photometric)).good() && (photometric.length() > 0))
    {
      const char *c = photometric.c_str(); // guaranteed to be zero-terminated
      char cp[17]; // legal CS cannot be larger than 16 characters plus 0 byte
      int i=0; // current character index
      unsigned char cur; // current character
      while (*c && (i<16))
      {
        cur = OFstatic_cast(unsigned char, *c);
        if (isalpha(cur)) cp[i++] = toupper(cur);
        else if (isdigit(cur)) cp[i++] = cur;
        c++;
      }
      cp[i] = 0; // write terminating zero
      photometric = cp; // copy back into OFString

      // now browse PhotometricInterpretationNames
      i = 0;
      while (PhotometricInterpretationNames[i].Name)
      {
        if (photometric == PhotometricInterpretationNames[i].Name) return PhotometricInterpretationNames[i].Type;
        i++;
      }
    }
  }
  return EPI_Unknown;
}


/*
 * CVS/RCS Log
 * $Log: djutils.cc,v $
 * Revision 1.8  2010-11-01 08:55:56  uli
 * Moved variable declarations in front of their loop.
 *
 * Revision 1.7  2010-10-20 07:41:35  uli
 * Made sure isalpha() & friends are only called with valid arguments.
 *
 * Revision 1.6  2010-10-14 13:14:22  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.5  2009-10-07 12:44:34  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.4  2005-12-08 15:43:51  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.3  2005/11/30 14:13:14  onken
 * Added OFCondition constant for "unsupported color space conversions"
 *
 * Revision 1.2  2002/11/27 15:40:01  meichel
 * Adapted module dcmjpeg to use of new header file ofstdinc.h
 *
 * Revision 1.1  2001/11/13 15:58:35  meichel
 * Initial release of module dcmjpeg
 *
 *
 */
