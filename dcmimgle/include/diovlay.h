/*
 *
 *  Copyright (C) 1997-99, OFFIS
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomOverlay (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 15:42:39 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/diovlay.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIOVLAY_H
#define __DIOVLAY_H

#include "osconfig.h"
#include "dctypes.h"

#include "diobjcou.h"
#include "diovpln.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiDocument;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiOverlayData : public DiObjectCounter
{
    friend class DiOverlay;

 public:
    DiOverlayData(unsigned int count = 0,
                  DiOverlayPlane **planes = NULL,
                  Uint16 *data = NULL)
      : Count(count),
        Planes(planes),
        Data(data)
    {
    }
    
    virtual ~DiOverlayData()
    {
        if (Planes != NULL)
        {
            register unsigned int i;
            for (i = 0; i < Count; i++)
                delete Planes[i];
        }
        delete Planes;
        delete[] Data;
    }
    
 private:
    unsigned int Count;                 // number of planes
    
    DiOverlayPlane **Planes;            // points to an array of planes
    Uint16 *Data;                       // points to overlay data (if scaled, flipped or rotated)

 // --- declarations to avoid compiler warnings
 
    DiOverlayData(const DiOverlayData &);
    DiOverlayData &operator=(const DiOverlayData &);
};


/********************************************************************/


class DiOverlay : public DiObjectCounter
{
 public:
    DiOverlay(const DiDocument *docu,
              const Uint16 alloc = 0);
              
    DiOverlay(const DiOverlay *overlay,
              const Uint16 left,
              const Uint16 top,
              const double xfactor,
              const double yfactor);
    
    DiOverlay(const DiOverlay *overlay,
              const int horz,
              const int vert);

    DiOverlay(const DiOverlay *overlay,
              const int degree);

    virtual ~DiOverlay();
    
    int showPlane(const unsigned int);

    int showPlane(const unsigned int,
                  const double,
                  const double,
                  const EM_Overlay);

    int showAllPlanes();

    int showAllPlanes(const double,
                      const double,
                      const EM_Overlay);

    int hidePlane(const unsigned int);

    int hideAllPlanes();

    int placePlane(const unsigned int,
                   const signed int,
                   const signed int);
    
    inline unsigned int getCount() const
    {
        if (Data != NULL)
            return Data->Count;
        return 0;
    }
        
    inline Uint16 getLeft() const
    {
        return Left;
    }

    inline Uint16 getTop() const
    {
        return Top;
    }
        
    inline int hasEmbeddedData() const;
        
    inline DiOverlayPlane *getPlane(const unsigned long plane) const
    {
        if ((Data != NULL) && (Data->Planes != NULL) && (plane < Data->Count))
            return Data->Planes[plane];
        return (DiOverlayPlane *)NULL;
    }
    
    
 protected:
    Uint16 *Init(const DiOverlay *overlay,
                 const double xfactor = 1,
                 const double yfactor = 1);

 private:

    Uint16 Left;                        // left offset to all planes
    Uint16 Top;                         // top offset to all planes
    Uint16 Width;                       // maximum width of all planes 
    Uint16 Height;                      // maximum height of all planes
    unsigned long Frames;               // maximum number of frames

    DiOverlayData *Data;                // points to overlay data (provides shared data)

 // --- declarations to avoid compiler warnings
 
    DiOverlay(const DiOverlay &);
    DiOverlay &operator=(const DiOverlay &);
};


#endif


/*
**
** CVS/RCS Log:
** $Log: diovlay.h,v $
** Revision 1.1  1998-11-27 15:42:39  joergr
** Added copyright message.
** Added method to detach pixel data if it is no longer needed.
** Replaced delete by delete[] for array types.
** Added methods and constructors for flipping and rotating, changed for
** scaling and clipping.
**
** Revision 1.5  1998/07/01 08:39:26  joergr
** Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
** options), e.g. add copy constructors.
**
** Revision 1.4  1998/05/11 14:53:25  joergr
** Added CVS/RCS header to each file.
**
**
*/
