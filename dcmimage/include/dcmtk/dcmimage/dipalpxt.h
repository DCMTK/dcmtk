/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomPalettePixelTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:29 $
 *  CVS/RCS Revision: $Revision: 1.24 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIPALPXT_H
#define DIPALPXT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimage/dicopxt.h"
#include "dcmtk/dcmimgle/diluptab.h"
#include "dcmtk/dcmimgle/diinpx.h"  /* gcc 3.4 needs this */


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to handle Palette color pixel data
 */
template<class T1, class T2, class T3>
class DiPalettePixelTemplate
  : public DiColorPixelTemplate<T3>
{

 public:

    /** constructor
     *
     ** @param  docu     pointer to DICOM document
     *  @param  pixel    pointer to input pixel representation
     *  @param  palette  pointer to RGB color palette
     *  @param  status   reference to status variable
     */
    DiPalettePixelTemplate(const DiDocument *docu,
                           const DiInputPixel *pixel,
                           DiLookupTable *palette[3],
                           EI_Status &status)
      : DiColorPixelTemplate<T3>(docu, pixel, 1, status)
    {
        if ((pixel != NULL) && (this->Count > 0) && (status == EIS_Normal))
        {
            if (this->PlanarConfiguration)
            {
                status = EIS_InvalidValue;
                DCMIMAGE_ERROR("invalid value for 'PlanarConfiguration' (" << this->PlanarConfiguration << ")");
            }
            else
                convert(OFstatic_cast(const T1 *, pixel->getData()) + pixel->getPixelStart(), palette);
        }
    }

    /** destructor
     */
    virtual ~DiPalettePixelTemplate()
    {
    }


 private:

    /** convert input pixel data to intermediate representation
     *
     ** @param  pixel    pointer to input pixel data
     *  @param  palette  pointer to RGB color palette
     */
    void convert(const T1 *pixel,
                 DiLookupTable *palette[3])
    {                                                                // can be optimized if necessary !
        if (Init(pixel))
        {
            register const T1 *p = pixel;
            register T2 value = 0;
            register unsigned long i;
            register int j;
            // use the number of input pixels derived from the length of the 'PixelData'
            // attribute), but not more than the size of the intermediate buffer
            const unsigned long count = (this->InputCount < this->Count) ? this->InputCount : this->Count;
            for (i = 0; i < count; ++i)
            {
                value = OFstatic_cast(T2, *(p++));
                for (j = 0; j < 3; ++j)
                {
                    if (value <= palette[j]->getFirstEntry(value))
                        this->Data[j][i] = OFstatic_cast(T3, palette[j]->getFirstValue());
                    else if (value >= palette[j]->getLastEntry(value))
                        this->Data[j][i] = OFstatic_cast(T3, palette[j]->getLastValue());
                    else
                        this->Data[j][i] = OFstatic_cast(T3, palette[j]->getValue(value));
                }
            }
        }
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dipalpxt.h,v $
 * Revision 1.24  2010-10-14 13:16:29  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.23  2010-03-01 09:08:46  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.22  2009-11-25 14:31:21  joergr
 * Removed inclusion of header file "ofconsol.h".
 *
 * Revision 1.21  2009-10-14 10:25:14  joergr
 * Fixed minor issues in log output. Also updated copyright date (if required).
 *
 * Revision 1.20  2009-10-13 14:08:33  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.19  2006-08-15 16:35:01  meichel
 * Updated the code in module dcmimage to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.18  2005/12/08 16:01:41  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.17  2004/04/21 10:00:31  meichel
 * Minor modifications for compilation with gcc 3.4.0
 *
 * Revision 1.16  2003/12/23 11:50:30  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.15  2002/06/26 16:19:13  joergr
 * Enhanced handling of corrupted pixel data and/or length.
 * Corrected decoding of multi-frame, planar images.
 *
 * Revision 1.14  2001/11/09 16:47:02  joergr
 * Removed 'inline' specifier from certain methods.
 *
 * Revision 1.13  2001/06/01 15:49:31  meichel
 * Updated copyright header
 *
 * Revision 1.12  2000/04/27 13:15:14  joergr
 * Dcmimage library code now consistently uses ofConsole for error output.
 *
 * Revision 1.11  2000/03/08 16:21:53  meichel
 * Updated copyright header.
 *
 * Revision 1.10  1999/09/17 14:03:45  joergr
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.9  1999/05/03 11:03:06  joergr
 * Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.8  1999/04/28 12:52:01  joergr
 * Corrected some typos, comments and formatting.
 *
 * Revision 1.7  1999/01/20 14:46:30  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 *
 * Revision 1.6  1998/12/14 17:08:56  joergr
 * Added support for signed values as second entry in look-up tables
 * (= first value mapped).
 *
 * Revision 1.5  1998/11/27 14:17:31  joergr
 * Added copyright message.
 *
 * Revision 1.4  1998/07/01 08:39:27  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.3  1998/05/11 14:53:27  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
