/*
 *
 *  Copyright (C) 1996-2000, OFFIS
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
 *  Purpose: DicomYBRPart422PixelTemplate (Header)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:21:55 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/diyp2pxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIYP2PXT_H
#define __DIYP2PXT_H

#include "osconfig.h"

#include "ofconsol.h"    /* for CERR, COUT */
#include "dicopxt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to handle YCbCr Partial 4:2:2 pixel data
 */
template<class T1, class T2>
class DiYBRPart422PixelTemplate
  : public DiColorPixelTemplate<T2>
{

 public:

    DiYBRPart422PixelTemplate(const DiDocument *docu,
                              const DiInputPixel *pixel,
                              EI_Status &status,
                              const int bits)
      : DiColorPixelTemplate<T2>(docu, pixel, 3, status, 2)
    {
        if ((pixel != NULL) && (Count > 0) && (status == EIS_Normal))
        {
            if (PlanarConfiguration)
            {
                status = EIS_InvalidValue;
                if (DicomImageClass::DebugLevel & DicomImageClass::DL_Errors)
                    CERR << "ERROR: invalid value for 'PlanarConfiguration' (" << PlanarConfiguration << ") ! " << endl;
            }
            else
                convert((const T1 *)pixel->getData(), bits);
        }   
    }

    virtual ~DiYBRPart422PixelTemplate()
    {
    }


 private:

    inline void convert(const T1 *pixel,
                        const int bits)
    {
        if (Init(pixel))
        {
            register T2 *r = Data[0];
            register T2 *g = Data[1];
            register T2 *b = Data[2];
            register unsigned long i;
            const T2 maxvalue = (T2)DicomImageClass::maxval(bits);
            const T1 offset = (T1)DicomImageClass::maxval(bits - 1);
            
            register const T1 *p = pixel;
            register T2 y1;
            register T2 y2;
            register T2 cb;
            register T2 cr;
            for (i = Count / 2; i != 0; i--)
            {
                y1 = removeSign(*(p++), offset); 
                y2 = removeSign(*(p++), offset);
                cb = removeSign(*(p++), offset);
                cr = removeSign(*(p++), offset);
                convertValue(*(r++), *(g++), *(b++), y1, cb, cr, maxvalue);
                convertValue(*(r++), *(g++), *(b++), y2, cb, cr, maxvalue);
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
        double dr = 1.1631 * (double)y + 1.5969 * (double)cr - 0.8713 * double(maxvalue);
        double dg = 1.1631 * (double)y - 0.3913 * (double)cb - 0.8121 * (double)cr + 0.5290 * double(maxvalue);
        double db = 1.1631 * (double)y + 2.0177 * (double)cb - 1.0820 * double(maxvalue);
        red   = (dr < 0.0) ? 0 : (dr > (double)maxvalue) ? maxvalue : (T2)dr;
        green = (dg < 0.0) ? 0 : (dg > (double)maxvalue) ? maxvalue : (T2)dg;
        blue  = (db < 0.0) ? 0 : (db > (double)maxvalue) ? maxvalue : (T2)db;
    }
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diyp2pxt.h,v $
 * Revision 1.9  2000-03-08 16:21:55  meichel
 * Updated copyright header.
 *
 * Revision 1.8  2000/03/03 14:07:53  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.7  1999/09/17 14:03:47  joergr
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.6  1999/04/28 12:45:22  joergr
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.5  1999/02/03 16:55:30  joergr
 * Moved global functions maxval() and determineRepresentation() to class
 * DicomImageClass (as static methods).
 *
 * Revision 1.4  1999/01/20 14:48:13  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 *
 * Revision 1.3  1998/11/27 14:22:55  joergr
 * Introduced global debug level for dcmimage module to control error output.
 *
 * Revision 1.2  1998/05/11 14:53:34  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
