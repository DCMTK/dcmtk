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
 *  Update Date:      $Date: 1999-02-08 12:39:50 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/diovlay.h,v $
 *  CVS/RCS Revision: $Revision: 1.9 $
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


/*-------------------------------*
 *  declaration of helper class  *
 *-------------------------------*/

class DiOverlayData
  : public DiObjectCounter
{
    friend class DiOverlay;

 public:

    DiOverlayData(unsigned int entries,
                  unsigned int count = 0);
    
    virtual ~DiOverlayData();

    
 private:

    unsigned int Count;                 // number of (valid) planes
    unsigned int ArrayEntries;          // number of array entries (allocated memory)
    
    DiOverlayPlane **Planes;            // points to an array of planes
    Uint16 *DataBuffer;                 // points to overlay data (if scaled, flipped or rotated)

 // --- declarations to avoid compiler warnings
 
    DiOverlayData(const DiOverlayData &);
    DiOverlayData &operator=(const DiOverlayData &);
};


/********************************************************************/


class DiOverlay
  : public DiObjectCounter
{

 public:

    DiOverlay(const DiDocument *docu = NULL,
              const Uint16 alloc = 0);
              
    DiOverlay(const DiOverlay *overlay,
              const Uint16 left,
              const Uint16 top,
              const double xfactor,
              const double yfactor);
    
    DiOverlay(const DiOverlay *overlay,
              const int horz,
              const int vert,
              const Uint16 columns,
              const Uint16 rows);

    DiOverlay(const DiOverlay *overlay,
              const int degree,
              const Uint16 columns,
              const Uint16 rows);

    virtual ~DiOverlay();
    
    int isPlaneVisible(unsigned int plane);

    int showPlane(unsigned int plane);

    int showPlane(unsigned int plane,
                  const double,
                  const double,
                  const EM_Overlay);

    int showPlane(unsigned int plane,
                  const Uint16 pvalue);

    int showAllPlanes();

    int showAllPlanes(const double,
                      const double,
                      const EM_Overlay);

    int hidePlane(unsigned int plane);

    int hideAllPlanes();

    int placePlane(unsigned int plane,
                   const signed int,
                   const signed int);
    
    unsigned int getPlaneGroupNumber(unsigned int plane) const;

    const char *getPlaneLabel(unsigned int plane) const;

    const char *getPlaneDescription(unsigned int plane) const;

    EM_Overlay getPlaneMode(unsigned int plane) const;

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
        
    int hasEmbeddedData() const;
        
    int addPlane(const unsigned int group,
                 const signed int left,
                 const signed int top,
                 const unsigned int columns,
                 const unsigned int rows,
                 const DcmOverlayData &data,
                 const DcmLongString &label,
                 const DcmLongString &description,
                 const EM_Overlay mode);
                 
    int removePlane(const unsigned int group);
/*
    int removeAllPlanes();
*/
    inline DiOverlayPlane *getPlane(const unsigned int plane) const
    {
        if ((Data != NULL) && (Data->Planes != NULL) && (plane < Data->Count))
            return Data->Planes[plane];
        return (DiOverlayPlane *)NULL;
    }
    
    inline int hasPlane(unsigned int plane,
                        const int visible = 0) const
    {
        return (convertToPlaneNumber(plane, AdditionalPlanes) > 1) && (!visible || Data->Planes[plane]->isVisible());
    }
    
    Uint8 *getPlaneData(const unsigned long frame,
                        unsigned int plane,
                        unsigned int &left,
                        unsigned int &top,
                        unsigned int &width,
                        unsigned int &height,
                        EM_Overlay &mode,                        
                        const Uint16 columns,
                        const Uint16 rows,
                        const Uint8 value = 0xff);

    static const unsigned int MaxOverlayCount;
    static const unsigned int FirstOverlayGroup;


 protected:

    Uint16 *Init(const DiOverlay *overlay);

    int convertToPlaneNumber(unsigned int &plane,
                             const int mode) const;
    
    const unsigned int convertToGroupNumber(const unsigned int plane) const
    {
        return FirstOverlayGroup + 2 * plane;
    }
    
    int isValidGroupNumber(const unsigned int group) const;

    int checkPlane(const unsigned int plane,
                   const int mode = 1);


 private:

    Uint16 Left;                        // left offset to all planes
    Uint16 Top;                         // top offset to all planes
    Uint16 Width;                       // maximum width of all planes 
    Uint16 Height;                      // maximum height of all planes
    unsigned long Frames;               // maximum number of frames
    
    int AdditionalPlanes;               // 

    DiOverlayData *Data;                // points to overlay data (provides shared data)

 // --- declarations to avoid compiler warnings
 
    DiOverlay(const DiOverlay &);
    DiOverlay &operator=(const DiOverlay &);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diovlay.h,v $
 * Revision 1.9  1999-02-08 12:39:50  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.8  1999/02/03 17:33:51  joergr
 * Added support for calibration according to Barten transformation (incl.
 * a DISPLAY file describing the monitor characteristic).
 *
 * Revision 1.7  1998/12/23 13:21:29  joergr
 * Changed parameter type (long to int) to avoid warning reported by MSVC5.
 *
 * Revision 1.6  1998/12/23 11:36:28  joergr
 * Changed order of parameters for addOverlay() and getOverlayData().
 * Corrected bug concerning additional overlay planes.
 *
 * Revision 1.5  1998/12/22 14:35:30  joergr
 * Added method to check whether plane is visible, to get plane mode and to
 * remove all planes. Set 'value' used for getOverlay/PlaneData().
 * Changed meaning of return values (differentiate between different value
 * for 'true').
 *
 * Revision 1.4  1998/12/16 16:37:50  joergr
 * Added method to export overlay planes (create 8-bit bitmap).
 * Implemented flipping and rotation of overlay planes.
 *
 * Revision 1.3  1998/12/14 17:27:35  joergr
 * Added methods to add and remove additional overlay planes (still untested).
 * Added methods to support overlay labels and descriptions.
 *
 * Revision 1.2  1998/11/30 12:24:26  joergr
 * Removed wrong 'inline' from method declaration (reported by MSVC5).
 *
 * Revision 1.1  1998/11/27 15:42:39  joergr
 * Added copyright message.
 * Added method to detach pixel data if it is no longer needed.
 * Replaced delete by delete[] for array types.
 * Added methods and constructors for flipping and rotating, changed for
 * scaling and clipping.
 *
 * Revision 1.5  1998/07/01 08:39:26  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.4  1998/05/11 14:53:25  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
