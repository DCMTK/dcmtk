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
 *  Purpose: DicomYBR422PixelTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-05-24 09:53:06 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/diyf2pxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIYF2PXT_H
#define __DIYF2PXT_H

#include "osconfig.h"

#include "dicopxt.h"
#include "ofconsol.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to handle YCbCr Full 4:2:2 pixel data
 */
template<class T1, class T2>
class DiYBR422PixelTemplate
  : public DiColorPixelTemplate<T2>
{

 public:

    DiYBR422PixelTemplate(const DiDocument *docu,
                          const DiInputPixel *pixel,
                          EI_Status &status,
                          const int bits,
                          const OFBool rgb)
      : DiColorPixelTemplate<T2>(docu, pixel, 3, status, 2)
    {
        if ((pixel != NULL) && (Count > 0) && (status == EIS_Normal))
        {
            if (PlanarConfiguration)
            {
                status = EIS_InvalidValue;
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
                {
                    ofConsole.lockCerr() << "ERROR: invalid value for 'PlanarConfiguration' ("
                                         << PlanarConfiguration << ") ! " << endl;
                    ofConsole.unlockCerr();
                }
            }
            else
                convert((const T1 *)pixel->getData() + pixel->getPixelStart() * 2, bits, rgb);
        }   
    }

    virtual ~DiYBR422PixelTemplate()
    {
    }


 private:

    void convert(const T1 *pixel,
                 const int bits,
                 const OFBool rgb)
    {
        if (Init(pixel))
        {
            const T1 offset = (T1)DicomImageClass::maxval(bits - 1);
            register unsigned long i;
            register const T1 *p = pixel;
            register T2 *r = Data[0];
            register T2 *g = Data[1];
            register T2 *b = Data[2];
            register T2 y1;
            register T2 y2;
            register T2 cb;
            register T2 cr;
            if (rgb)    /* convert to RGB model */
            {
                const T2 maxvalue = (T2)DicomImageClass::maxval(bits);
                for (i = Count / 2; i != 0; i--)
                {
                    y1 = removeSign(*(p++), offset); 
                    y2 = removeSign(*(p++), offset);
                    cb = removeSign(*(p++), offset);
                    cr = removeSign(*(p++), offset);
                    convertValue(*(r++), *(g++), *(b++), y1, cb, cr, maxvalue);
                    convertValue(*(r++), *(g++), *(b++), y2, cb, cr, maxvalue);
                }
            } else {    /* retain YCbCr model: YCbCr_422_full -> YCbCr_full */
                for (i = Count / 2; i != 0; i--)
                {
                    y1 = removeSign(*(p++), offset); 
                    y2 = removeSign(*(p++), offset);
                    cb = removeSign(*(p++), offset);
                    cr = removeSign(*(p++), offset);
                    *(r++) = y1;
                    *(g++) = cb;
                    *(b++) = cr;
                    *(r++) = y2;
                    *(g++) = cb;
                    *(b++) = cr;
                }
            }
        }
    }

    inline void convertValue(T2 &red,
                             T2 &green,
                             T2 &blue,
                             const T2 y,
                             const T2 cb,
                             const T2 cr,
                             const T2 maxvalue)
    {
        double dr = (double)y + 1.4020 * (double)cr - 0.7010 * double(maxvalue);
        double dg = (double)y - 0.3441 * (double)cb - 0.7141 * (double)cr + 0.5291 * double(maxvalue);
        double db = (double)y + 1.7720 * (double)cb - 0.8859 * double(maxvalue);
        red   = (dr < 0.0) ? 0 : (dr > (double)maxvalue) ? maxvalue : (T2)dr;
        green = (dg < 0.0) ? 0 : (dg > (double)maxvalue) ? maxvalue : (T2)dg;
        blue  = (db < 0.0) ? 0 : (db > (double)maxvalue) ? maxvalue : (T2)db;
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diyf2pxt.h,v $
 * Revision 1.15  2002-05-24 09:53:06  joergr
 * Added missing #include "ofconsol.h".
 *
 * Revision 1.14  2001/11/09 16:47:03  joergr
 * Removed 'inline' specifier from certain methods.
 *
 * Revision 1.13  2001/09/28 13:55:41  joergr
 * Added new flag (CIF_KeepYCbCrColorModel) which avoids conversion of YCbCr
 * color models to RGB.
 *
 * Revision 1.12  2001/06/01 15:49:33  meichel
 * Updated copyright header
 *
 * Revision 1.11  2000/04/28 12:39:33  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.10  2000/04/27 13:15:16  joergr
 * Dcmimage library code now consistently uses ofConsole for error output.
 *
 * Revision 1.9  2000/03/08 16:21:54  meichel
 * Updated copyright header.
 *
 * Revision 1.8  2000/03/03 14:07:52  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.7  1999/09/17 14:03:47  joergr
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.6  1999/04/28 12:45:21  joergr
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.5  1999/02/03 16:55:29  joergr
 * Moved global functions maxval() and determineRepresentation() to class
 * DicomImageClass (as static methods).
 *
 * Revision 1.4  1999/01/20 14:48:01  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 *
 * Revision 1.3  1998/11/27 14:21:48  joergr
 * Added copyright message.
 * Introduced global debug level for dcmimage module to control error output.
 *
 * Revision 1.2  1998/05/11 14:53:33  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
