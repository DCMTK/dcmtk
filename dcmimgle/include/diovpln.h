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
 *  Purpose: DicomOverlayPlane (Header) - Multiframe Overlays UNTESTED !
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 15:45:09 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/diovpln.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 * 
 */


#ifndef __DIOVPLN_H
#define __DIOVPLN_H

#include "osconfig.h"
#include "dctypes.h"

#include "diutils.h"

#include <stddef.h>
#include <iostream.h>


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiDocument;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiOverlayPlane
{
 public:
    DiOverlayPlane(const DiDocument *,
                   const unsigned int,
                   const Uint16);
                   
    DiOverlayPlane(DiOverlayPlane *,
                   const unsigned int,
                   Uint16 *,
                   Uint16 *,
                   const Uint16,
                   const Uint16,
                   const Uint16,
                   const Uint16,
                   const double = 1,
                   const double = 1);
                   
    ~DiOverlayPlane();
    
    inline Sint16 getLeft(const Uint16 left = 0) const
        { return (Sint16)((Sint32)Left - (Sint32)left); }
    inline Sint16 getTop(const Uint16 top = 0) const
        { return (Sint16)((Sint32)Top - (Sint32)top); }
    inline Uint16 getWidth() const
        { return Width; }
    inline Uint16 getHeight() const
        { return Height; }
    inline Uint16 getRight(const Uint16 left = 0) const
        { return ((Sint32)Left + (Sint32)Width - (Sint32)left > 0) ? (Uint16)((Sint32)Left + (Sint32)Width -
            (Sint32)left) : 0; }
    inline Uint16 getBottom(const Uint16 top = 0) const
        { return ((Sint32)Top + (Sint32)Height - (Sint32)top > 0) ? (Uint16)((Sint32)Top + (Sint32)Height -
            (Sint32)top) : 0; }

    inline int isValid() const
        { return Valid; }
    inline int isVisible() const
        { return Visible; }
    inline void show()
        { Visible = 1; }
    inline void hide()
        { Visible = 0; }
    inline void place(const signed int left, const signed int top)
        { Left = left; Top = top; }
    
    void flipOrigin(const int horz,
                    const int vert);
                    
//    void rotate(const int degree);

    void show(const double, const double, const EM_Overlay);

    inline unsigned long getNumberOfFrames() const
        { return NumberOfFrames; }
    inline double getForeground() const
        { return Foreground; }
    inline double getThreshold() const
        { return Threshold; }
    inline EM_Overlay getMode() const
        { return Mode; }
     inline int isEmbedded() const
        { return EmbeddedData; }
    
    inline int reset(const unsigned long);
    inline int getNextBit();
    inline void setStart(const Uint16, const Uint16);

 protected:
    Uint32 NumberOfFrames;          // number of frames
    Uint16 ImageFrameOrigin;        // number of starting frame

    Sint16 Top;                     // y-coordinate of overlay plane's origin
    Sint16 Left;                    // x-coordinate of overlay plane's origin
    Uint16 Height;                  // visible height
    Uint16 Width;                   // visible width
    Uint16 Rows;                    // number of (stored) rows
    Uint16 Columns;                 // number of (stored) columns
    Uint16 BitsAllocated;           // number of allocated bits per pixel
    Uint16 BitPosition;             // position of overlay plane bit
    
    double Foreground;              // color of overlay plane ('0.0' = dark, '1.0' = bright)
    double Threshold;               // threshold value
    EM_Overlay Mode;                // overlay mode
    EM_Overlay DefaultMode;         // default overlay mode

    int Valid;                      // validity status
    int Visible;                    // visibility status
    
 private:
    unsigned long BitPos;           // current bit position
    unsigned long StartBitPos;      // starting bit position of current frame
    
    int EmbeddedData;               // true, if overlay data in embedded in pixel data
 
    const Uint16 *Ptr;              // points to current element of 'Data'
    const Uint16 *StartPtr;         // points to starting element of current frame
    const Uint16 *Data;             // point to overlay data (standalone) or pixel data (embedded)

 // --- declarations to avoid compiler warnings
 
    DiOverlayPlane(const DiOverlayPlane &);
    DiOverlayPlane &operator=(const DiOverlayPlane &);
};


/********************************************************************/


inline int DiOverlayPlane::reset(const unsigned long frame)
{
    int result = 0;
    if (Valid && (Data != NULL) && (frame >= ImageFrameOrigin) && (frame < ImageFrameOrigin + NumberOfFrames))
    {
        const unsigned long bits = frame * (unsigned long)Rows * (unsigned long)Columns * (unsigned long)BitsAllocated;
        StartBitPos = BitPos = (unsigned long)BitPosition + bits;
        StartPtr = Ptr = Data + (bits >> 4);
        result = (getRight() > 0) && (getBottom() > 0);
    } 
    return result;
}


inline int DiOverlayPlane::getNextBit()
{
    int result; 
    if (BitsAllocated == 16)                                    // optimization
        result = (int)(*(Ptr++) & (1 << BitPosition));
    else
    {
       Ptr = StartPtr + (BitPos >> 4);                          // div 16
       result = (int)(*Ptr & (1 << (BitPos & 0xf)));            // mod 16
       BitPos += BitsAllocated;                                 // next bit
    }    
    return result;
}


inline void DiOverlayPlane::setStart(const Uint16 x, const Uint16 y)
{
    if (BitsAllocated == 16)
        Ptr = StartPtr + (unsigned long)(y - Top) * (unsigned long)Columns + (unsigned long)(x - Left);
    else
        BitPos = StartBitPos + ((unsigned long)(y - Top) * (unsigned long)Columns + (unsigned long)(x - Left)) *
            (unsigned long)BitsAllocated;
}


#endif


/*
**
** CVS/RCS Log:
** $Log: diovpln.h,v $
** Revision 1.1  1998-11-27 15:45:09  joergr
** Added copyright message.
** Added method to detach pixel data if it is no longer needed.
** Added methods and constructors for flipping and rotating, changed for
** scaling and clipping.
**
** Revision 1.7  1998/07/01 08:39:26  joergr
** Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
** options), e.g. add copy constructors.
**
** Revision 1.6  1998/05/11 14:53:26  joergr
** Added CVS/RCS header to each file.
**
**
*/

