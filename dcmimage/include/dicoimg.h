/*
**
** Author:  Joerg Riesmeier
** Created: 20.12.96
**
** Module:  dicoimg.h
**
** Purpose: DicomColorImage (Header)
**
** Last Update:         $Author: joergr $
** Update Date:         $Date: 1998-07-01 08:39:18 $
** Source File:         $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dicoimg.h,v $
** CVS/RCS Revision:    $Revision: 1.4 $
** Status:              $State: Exp $
**
** CVS/RCS Log at end of file
**
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

class DiColorImage : public DiImage {

 public:
    DiColorImage(const DiDocument *, const EI_Status);
    DiColorImage(const DiMonoImage *);
    virtual ~DiColorImage();

    void *getOutputData(const unsigned long, const int, const int = 0);
    void *getOutputPlane(const int) const;
    void deleteOutputData();
    
    DiImage *scale(const unsigned long, const unsigned long, const int) const;
    DiImage *clip(const unsigned long, const unsigned long, const unsigned long, const unsigned long) const;

    DiImage *makeMonochrome(const double, const double, const double) const;
    const DiColorPixel *getInterData() const
        { return InterData; }

    void *createDIB(const unsigned long);

    int writePPM(ostream &, const unsigned long, const int); 
    int writePPM(FILE *, const unsigned long, const int); 
    int writeRawPPM(FILE *, const unsigned long, const int); 

 protected:
    DiColorImage(const DiColorImage *, const Uint16, const Uint16, const int = 0);
    DiColorImage(const DiColorImage *, const Uint16, const Uint16, const Uint16, const Uint16);

    void checkInterData();

    DiColorPixel *InterData;

 private:
    DiColorOutputPixel *OutputData;

 // --- declarations to avoid compiler warnings
 
    DiColorImage(const DiColorImage &);
    DiColorImage &operator=(const DiColorImage &);
};


#endif




/*
**
** CVS/RCS Log:
** $Log: dicoimg.h,v $
** Revision 1.4  1998-07-01 08:39:18  joergr
** Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
** options), e.g. add copy constructors.
**
** Revision 1.3  1998/05/11 14:53:11  joergr
** Added CVS/RCS header to each file.
**
**
*/

