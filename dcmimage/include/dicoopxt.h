/*
 *
 *  Copyright (C) 1996-99, OFFIS
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
 *  Purpose: DicomColorOutputPixelTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-12-22 13:23:57 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicoopxt.h,v $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DICOOPXT_H
#define __DICOOPXT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dicoopx.h"
#include "dipxrept.h"
#include "diutils.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

template<class T1, class T2>
class DiColorOutputPixelTemplate
  : public DiColorOutputPixel,
    public DiPixelRepresentationTemplate<T2>
{

 public:

    DiColorOutputPixelTemplate(const DiColorPixel *pixel,
                               const unsigned long frame,
                               const unsigned long frames,
                               Sint16 shift,
                               const int planar)
      : DiColorOutputPixel(pixel, frames),
        Data(NULL),
        isPlanar(planar)
    {
        if ((pixel != NULL) && (getCount() > 0))
            convert((const T1 **)(pixel->getData()), frame * getCount(), shift, planar);
    }

    virtual ~DiColorOutputPixelTemplate()
    {
        delete[] Data;
    }

    inline EP_Representation getRepresentation() const
    {
        return DiPixelRepresentationTemplate<T2>::getRepresentation();
    }

    inline size_t getItemSize() const
    {
        return sizeof(T2) * 3;
    }
    
    inline void *getData() const
    {
        return (void *)Data;
    }
    
    inline void *getPlane(const int plane) const
    {
        void *result = NULL;
        if (Data != NULL)
        {
            if (plane <= 0)
                result = (void *)Data;
            else
            {
                if (isPlanar)
                    result = (void *)(Data + ((plane == 1) ? 1 : 2) * getCount());
                else
                    result = (void *)(Data + ((plane == 1) ? 1 : 2));
            }
        }
        return result; 
    }
    
    inline int writePPM(ostream &stream) const
    {
        if (Data != NULL)
        {
            register T2 *p = Data;
            register unsigned long i;
            register int j;
            for (i = 0; i < getCount(); i++)
                for (j = 0; j < 3; j++)
                    stream << (unsigned long)*(p++) << " ";     // typecast to resolve problems with 'char'
            return 1;
        }
        return 0;
    }

    inline int writePPM(FILE *stream) const
    {
        if (Data != NULL)
        {
            register T2 *p = Data;
            register unsigned long i;
            register int j;
            for (i = 0; i < getCount(); i++)
                for (j = 0; j < 3; j++)
                    fprintf(stream, "%lu ", (unsigned long)*(p++));
            return 1;
        }
        return 0;
    }


 private:

    inline void convert(const T1 *pixel[3],
                        const unsigned long start,
                        Sint16 shift,
                        const int planar)
    {
        if ((pixel[0] != NULL) && (pixel[1] != NULL) && (pixel[2] != NULL))
        {
            Data = new T2[getCount() * 3];
            if (Data != NULL)
            {
                register T2 *q = Data;
                register unsigned long i;
                if (planar)
                {
                    register const T1 *p;
                    if (shift == 0)
                    {
                        for (int j = 0; j < 3; j++)
                        {
                            p = pixel[j] + start;
                            for (i = 0; i < getCount(); i++)
                                *(q++) = (T2)*(p++);                            // copy
                        }
                    }
                    else if (shift < 0)
                    {
                        shift = -shift;
                        for (int j = 0; j < 3; j++)
                        {
                            p = pixel[j] + start;
                            for (i = 0; i < getCount(); i++)
                                *(q++) = (T2)(*(p++) << shift);                 // expand depth: simple left shift is not correct !!
                        }                                                       // ... to be enhanced !
                    }
                    else /* shift > 0 */
                    {
                        for (int j = 0; j < 3; j++)
                        {
                            p = pixel[j] + start;
                            for (i = 0; i < getCount(); i++)
                                *(q++) = (T2)(*(p++) >> shift);                 // reduce depth: correct ?
                        }
                    }
                }
                else
                {
                    register int j;
                    if (shift == 0)
                    {
                        for (i = start; i < start + getCount(); i++)
                            for (j = 0; j < 3; j++)
                                *(q++) = (T2)pixel[j][i];                       // copy
                    }
                    else if (shift < 0)
                    {
                        shift = -shift;
                        for (i = start; i < start + getCount(); i++)
                            for (j = 0; j < 3; j++)
                                *(q++) = (T2)(pixel[j][i] << shift);            // expand depth: simple left shift is not correct !!
                    }                                                           // ... to be enhanced !
                    else /* shift > 0 */
                    {
                        for (i = start; i < start + getCount(); i++)
                            for (j = 0; j < 3; j++)
                                *(q++) = (T2)(pixel[j][i] >> shift);            // reduce depth: correct ?
                    }
                }
            }
        }
    }
 
    T2 *Data;
    int isPlanar;

 // --- declarations to avoid compiler warnings
 
    DiColorOutputPixelTemplate(const DiColorOutputPixelTemplate &);
    DiColorOutputPixelTemplate &operator=(const DiColorOutputPixelTemplate &);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dicoopxt.h,v $
 * Revision 1.7  1998-12-22 13:23:57  joergr
 * Added comments that the routines for expanding pixel's depth have to be
 * enhanced in the future (replicate bit pattern instead of shifting). Same
 * question for reducing depth.
 *
 * Revision 1.6  1998/11/27 13:46:00  joergr
 * Added copyright message. Replaced delete by delete[] for array types.
 *
 * Revision 1.5  1998/07/01 08:39:19  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.4  1998/05/11 14:53:12  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
