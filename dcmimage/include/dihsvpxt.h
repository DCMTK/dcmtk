/*
 *
 *  Copyright (C) 1996-2002, OFFIS
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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomHSVPixelTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-06-26 16:18:10 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dihsvpxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.16 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIHSVPXT_H
#define __DIHSVPXT_H

#include "osconfig.h"

#include "ofconsol.h"    /* for ofConsole */
#include "dicopxt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to handle HSV pixel data
 */
template<class T1, class T2>
class DiHSVPixelTemplate
  : public DiColorPixelTemplate<T2>
{

 public:

    /** constructor
     *
     ** @param  docu       pointer to DICOM document
     *  @param  pixel      pointer to input pixel representation
     *  @param  status     reference to status variable
     *  @param  planeSize  number of pixels in a plane
     *  @param  bits       number of bits per sample
     */
    DiHSVPixelTemplate(const DiDocument *docu,
                       const DiInputPixel *pixel,
                       EI_Status &status,
                       const unsigned long planeSize,
                       const int bits)
      : DiColorPixelTemplate<T2>(docu, pixel, 3, status)
    {
        if ((pixel != NULL) && (Count > 0) && (status == EIS_Normal))
            convert((const T1 *)pixel->getData() + pixel->getPixelStart(), planeSize, bits);
    }

    /** destructor
     */
    virtual ~DiHSVPixelTemplate()
    {
    }


 private:

    /** convert input pixel data to intermediate representation
     *
     ** @param  pixel      pointer to input pixel data
     *  @param  planeSize  number of pixels in a plane
     *  @param  bits       number of bits per sample
     */
    void convert(const T1 *pixel,
                 const unsigned long planeSize,
                 const int bits)
    {
        if (Init(pixel))
        {
            register T2 *r = Data[0];
            register T2 *g = Data[1];
            register T2 *b = Data[2];
            const T2 maxvalue = (T2)DicomImageClass::maxval(bits);
            const T1 offset = (T1)DicomImageClass::maxval(bits - 1);
            // use the number of input pixels derived from the length of the 'PixelData'
            // attribute), but not more than the size of the intermediate buffer
            const unsigned long count = (InputCount < Count) ? InputCount : Count;
            if (PlanarConfiguration)
            {
/*
                register const T1 *h = pixel;
                register const T1 *s = h + InputCount;
                register const T1 *v = s + InputCount;
                for (i = count; i != 0; i--)
                    convertValue(*(r++), *(g++), *(b++), removeSign(*(h++), offset), removeSign(*(s++), offset),
                        removeSign(*(v++), offset), maxvalue);
*/
                register unsigned long l;
                register unsigned long i = count;
                register const T1 *h = pixel;
                register const T1 *s = h + planeSize;
                register const T1 *v = s + planeSize;
                while (i != 0)
                {
                    /* convert a single frame */
                    for (l = planeSize; (l != 0) && (i != 0); l--, i--)
                    {
                        convertValue(*(r++), *(g++), *(b++), removeSign(*(h++), offset), removeSign(*(s++), offset),
                            removeSign(*(v++), offset), maxvalue);
                    }
                    /* jump to next frame start (skip 2 planes) */
                    h += 2 * planeSize;
                    s += 2 * planeSize;
                    v += 2 * planeSize;
                }
            } 
            else
            {
                register const T1 *p = pixel;
                register T2 h;
                register T2 s;
                register T2 v;
                register unsigned long i;
                for (i = count; i != 0; i--)
                {
                    h = removeSign(*(p++), offset); 
                    s = removeSign(*(p++), offset);
                    v = removeSign(*(p++), offset);
                    convertValue(*(r++), *(g++), *(b++), h, s, v, maxvalue);
                }
            }
        }
    }

    /** convert a single HSV value to RGB
     */
    void convertValue(T2 &red,
                      T2 &green,
                      T2 &blue,
                      const T2 hue,
                      const T2 saturation,
                      const T2 value,
                      const T2 maxvalue)
    {
        /*
         *   conversion algorithm taken from Foley et al.: 'Computer Graphics: Principles and Practice' (1990)
         */

        if (saturation == 0)
        {
            red = value;
            green = value;
            blue = value;
        }
        else
        {
            const double h = ((double)hue * 6) / ((double)maxvalue + 1);            // '... + 1' to assert h < 6
            const double s = (double)saturation / (double)maxvalue;
            const double v = (double)value / (double)maxvalue;
            const T2 hi = (T2)h;
            const double hf = h - hi;
            const T2 p = (T2)(maxvalue * v * (1 - s));
            const T2 q = (T2)(maxvalue * v * (1 - s * hf));
            const T2 t = (T2)(maxvalue * v * (1 - s * (1 - hf)));
            switch (hi)
            {
                case 0:
                    red = value;
                    green = t;
                    blue = p;
                    break;
                case 1:
                    red = q;
                    green = value;
                    blue = p;
                    break;
                case 2:
                    red = p;
                    green = value;
                    blue = t;
                    break;
                case 3:
                    red = p;
                    green = q;
                    blue = value;
                    break;
                case 4:
                    red = t;
                    green = p;
                    blue = value;
                    break;
                case 5:
                    red = value;
                    green = p;
                    blue = q;
                    break;
                default:
                    if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                    {
                        ofConsole.lockCerr() << "WARNING: invalid value for 'hi' while converting HSV to RGB !" << endl;
                        ofConsole.unlockCerr();
                    }
            }
        }
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dihsvpxt.h,v $
 * Revision 1.16  2002-06-26 16:18:10  joergr
 * Enhanced handling of corrupted pixel data and/or length.
 * Corrected decoding of multi-frame, planar images.
 *
 * Revision 1.15  2001/11/09 16:47:01  joergr
 * Removed 'inline' specifier from certain methods.
 *
 * Revision 1.14  2001/06/01 15:49:30  meichel
 * Updated copyright header
 *
 * Revision 1.13  2000/04/28 12:39:32  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.12  2000/04/27 13:15:13  joergr
 * Dcmimage library code now consistently uses ofConsole for error output.
 *
 * Revision 1.11  2000/03/08 16:21:52  meichel
 * Updated copyright header.
 *
 * Revision 1.10  2000/03/03 14:07:52  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.9  1999/09/17 14:03:45  joergr
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.8  1999/04/28 12:47:04  joergr
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.7  1999/02/03 16:54:27  joergr
 * Moved global functions maxval() and determineRepresentation() to class
 * DicomImageClass (as static methods).
 *
 * Revision 1.6  1999/01/20 14:46:15  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 *
 * Revision 1.5  1998/11/27 13:51:50  joergr
 * Added copyright message.
 *
 * Revision 1.4  1998/05/11 14:53:16  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
