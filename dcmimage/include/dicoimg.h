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
 *  Purpose: DicomColorImage (Header)
 *
 *  Last Update:         $Author: meichel $
 *  Update Date:         $Date: 2000-03-08 16:21:50 $
 *  Source File:         $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicoimg.h,v $
 *  CVS/RCS Revision:    $Revision: 1.9 $
 *  Status:              $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DICOIMG_H
#define __DICOIMG_H

#include "osconfig.h"
#include "dctypes.h"

#include "diimage.h"
#include "diutils.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiMonoImage;
class DiColorPixel;
class DiColorOutputPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Base class for color images
 */
class DiColorImage
  : public DiImage
{

 public:

    DiColorImage(const DiDocument *docu,
                 const EI_Status status,
                 const int spp);

    DiColorImage(const DiMonoImage *image);

    virtual ~DiColorImage();

    void *getOutputData(const unsigned long frame,
                        const int bits,
                        const int planar = 0);

    int getOutputData(void *buffer,
                      const unsigned long size,
                      const unsigned long frame,
                      const int bits,
                      const int planar = 0);

    void *getOutputPlane(const int plane) const;

    void deleteOutputData();
    
    DiImage *createImage(const unsigned long fstart,
                         const unsigned long fcount) const;

    DiImage *createScale(const signed long left,
                         const signed long top,
                         const unsigned long src_cols,
                         const unsigned long src_rows,                 
                         const unsigned long dest_cols,
                         const unsigned long dest_rows,
                         const int interpolate,
                         const int aspect,
                         const Uint16 /*pvalue*/) const;

    DiImage *createClip(const signed long left,
                        const signed long top,
                        const unsigned long width,
                        const unsigned long height) const;

    int flip(const int horz,
             const int vert);
             
    DiImage *createFlip(const int horz,
                        const int vert) const;
             
    int rotate(const int degree);

    DiImage *createRotate(const int degree) const;

    DiImage *createMono(const double red,
                        const double green,
                        const double blue) const;

    const DiColorPixel *getInterData() const
    {
        return InterData;
    }

    void *createDIB(const unsigned long frame);

    void *createAWTBitmap(const unsigned long frame,
                          const int bits);

    int writePPM(ostream &stream,
                 const unsigned long frame,
                 const int bits);

    int writePPM(FILE *stream,
                 const unsigned long frame,
                 const int bits);

    int writeRawPPM(FILE *stream,
                    const unsigned long frame,
                    const int bits);


 protected:

    DiColorImage(const DiColorImage *image,
                 const unsigned long fstart,
                 const unsigned long fcount);

    DiColorImage(const DiColorImage *image,
                 const signed long left,
                 const signed long top,
                 const Uint16 src_cols,
                 const Uint16 src_rows,                 
                 const Uint16 dest_cols,
                 const Uint16 dest_rows,
                 const int interpolate = 0,
                 const int aspect = 0);
                 
    DiColorImage(const DiColorImage *image,
                 const signed long left,
                 const signed long top,
                 const Uint16 columns,
                 const Uint16 rows);

    DiColorImage(const DiColorImage *image,
                 const int horz,
                 const int vert);

    DiColorImage(const DiColorImage *image,
                 const int degree);

    int checkInterData(const int mode = 1);

    void *getData(void *buffer,
                  const unsigned long size,
                  const unsigned long frame,
                  const int bits,
                  const int planar);

    DiColorPixel *InterData;


 private:

    DiColorOutputPixel *OutputData;

 // --- declarations to avoid compiler warnings
 
    DiColorImage(const DiColorImage &);
    DiColorImage &operator=(const DiColorImage &);
};


#endif



/*
 *
 * CVS/RCS Log:
 * $Log: dicoimg.h,v $
 * Revision 1.9  2000-03-08 16:21:50  meichel
 * Updated copyright header.
 *
 * Revision 1.8  1999/08/25 16:58:06  joergr
 * Added new feature: Allow clipping region to be outside the image
 * (overlapping).
 *
 * Revision 1.7  1999/04/28 12:51:57  joergr
 * Corrected some typos, comments and formatting.
 *
 * Revision 1.6  1999/01/20 14:39:52  joergr
 * Added new output method to fill external memory buffer with rendered pixel
 * data.
 *
 * Revision 1.5  1998/11/27 13:43:29  joergr
 * Added methods and constructors for flipping and rotating, changed for
 * scaling and clipping.
 *
 * Revision 1.4  1998/07/01 08:39:18  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.3  1998/05/11 14:53:11  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
