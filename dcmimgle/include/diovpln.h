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
 *  Update Date:      $Date: 1999-03-22 08:52:18 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/diovpln.h,v $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 * 
 */


#ifndef __DIOVPLN_H
#define __DIOVPLN_H

#include "osconfig.h"
#include "dctypes.h"
#include "ofstring.h"

#include "diutils.h"

BEGIN_EXTERN_C
 #include <stddef.h>
END_EXTERN_C

#include <iostream.h>


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DcmOverlayData;
class DcmLongString;

class DiDocument;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiOverlayPlane
{

 public:

    DiOverlayPlane(const DiDocument *docu,
                   const unsigned int group,
                   const Uint16 alloc);

    DiOverlayPlane(const unsigned int group,
                   const Sint16 left,
                   const Sint16 top,
                   const Uint16 columns,
                   const Uint16 rows,
                   const DcmOverlayData &data,
                   const DcmLongString &label,
                   const DcmLongString &description,
                   const EM_Overlay mode);

    DiOverlayPlane(DiOverlayPlane *plane,
                   const unsigned int bit,
                   Uint16 *data,
                   Uint16 *temp,
                   const Uint16 width,
                   const Uint16 height,
                   const Uint16 columns,
                   const Uint16 rows);

    ~DiOverlayPlane();

    inline Sint16 getLeft(const Uint16 left = 0) const
    {
        return (Sint16)((Sint32)Left - (Sint32)left);
    }

    inline Sint16 getTop(const Uint16 top = 0) const
    {
        return (Sint16)((Sint32)Top - (Sint32)top);
    }

    inline Uint16 getWidth() const
    {
        return Width;
    }

    inline Uint16 getHeight() const
    {
        return Height;
    }

    inline Uint16 getRight(const Uint16 left = 0) const
    {
        return ((Sint32)Left + (Sint32)Width - (Sint32)left > 0) ? (Uint16)((Sint32)Left + (Sint32)Width - (Sint32)left) : 0;
    }

    inline Uint16 getBottom(const Uint16 top = 0) const
    {
        return ((Sint32)Top + (Sint32)Height - (Sint32)top > 0) ? (Uint16)((Sint32)Top + (Sint32)Height - (Sint32)top) : 0;
    }

    inline int isValid() const
    {
        return Valid;
    }

    inline int isVisible() const
    {
        return Visible;
    }

    inline void show()
    {
        Visible = 1;
    }

    inline void hide()
    {
        Visible = 0;
    }
    
    inline void place(const signed int left,
                      const signed int top)
    {
        Left = left; Top = top;
    }
    
    void setScaling(const double xfactor,
                    const double yfactor);

    void setFlipping(const int horz,
                     const int vert,
                     const Uint16 columns,
                     const Uint16 rows);
                    
    void setRotation(const int degree,
                     const Uint16 columns,
                     const Uint16 rows);

    void show(const double fore,
              const double thresh,
              const EM_Overlay mode);

    int show(const Uint16 pvalue);

    inline unsigned long getNumberOfFrames() const
    {
        return NumberOfFrames;
    }

    inline double getForeground() const
    {
        return Foreground;
    }

    inline double getThreshold() const
    {
        return Threshold;
    }

    inline Uint16 getPValue() const
    {
        return PValue;
    }

    inline EM_Overlay getMode() const
    {
        return Mode;
    }

    inline int isEmbedded() const
    {
        return EmbeddedData;
    }
    
    const char *getLabel() const
    {
        if (!Label.empty())
            return Label.c_str();
        return NULL;
    }

    const char *getDescription() const
    {
        if (!Description.empty())
            return Description.c_str();
        return NULL;
    }

    const Uint16 getGroupNumber() const
    {
        return GroupNumber;
    }

    Uint8 *getData(const unsigned long frame,
                   const Uint16 xmin,
                   const Uint16 ymin,
                   const Uint16 xmax,
                   const Uint16 ymax,
                   const Uint8 fore,
                   const Uint8 back);

    inline int reset(const unsigned long);

    inline int getNextBit();

    inline void setStart(const Uint16 x,
                         const Uint16 y);


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
    Uint16 PValue;                  // P-value

    EM_Overlay Mode;                // overlay mode
    EM_Overlay DefaultMode;         // default overlay mode

    OFString Label;                 // label of overlay plane
    OFString Description;           // textual description of overlay plane
 
    Uint16 GroupNumber;             // group number of overlay plane

    int Valid;                      // validity status
    int Visible;                    // visibility status


 private:

    unsigned long BitPos;           // current bit position
    unsigned long StartBitPos;      // starting bit position of current frame

    unsigned int StartLeft;         // x-coordinate of first pixel in surrounding memory buffer
    unsigned int StartTop;          // y-coordinate of first pixel in surrounding memory buffer
    
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
        const unsigned long bits = ((unsigned long)StartLeft + (unsigned long)StartTop * (unsigned long)Columns +
            frame * (unsigned long)Rows * (unsigned long)Columns) * (unsigned long)BitsAllocated;
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


inline void DiOverlayPlane::setStart(const Uint16 x,
                                     const Uint16 y)
{
    if (BitsAllocated == 16)
        Ptr = StartPtr + (unsigned long)(y - Top) * (unsigned long)Columns + (unsigned long)(x - Left);
    else
        BitPos = StartBitPos + ((unsigned long)(y - Top) * (unsigned long)Columns + (unsigned long)(x - Left)) * (unsigned long)BitsAllocated;
}


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diovpln.h,v $
 * Revision 1.8  1999-03-22 08:52:18  joergr
 * Added parameter to specify (transparent) background color for method
 * getOverlayData().
 *
 * Revision 1.7  1999/02/03 17:34:36  joergr
 * Added BEGIN_EXTERN_C and END_EXTERN_C to some C includes.
 * Added support for calibration according to Barten transformation (incl.
 * a DISPLAY file describing the monitor characteristic).
 *
 * Revision 1.6  1998/12/23 13:21:29  joergr
 * Changed parameter type (long to int) to avoid warning reported by MSVC5.
 *
 * Revision 1.5  1998/12/23 11:37:42  joergr
 * Changed order of parameters for addOverlay() and getOverlayData().
 * Changed behaviour of getLabel/Description/Explanation() methods: return
 * NULL if string empty, no empty string "".
 *
 * Revision 1.4  1998/12/22 14:36:30  joergr
 * Added method to check whether plane is visible, to get plane mode and to
 * remove all planes. Set 'value' used for getOverlay/PlaneData().
 *
 * Revision 1.3  1998/12/16 16:37:51  joergr
 * Added method to export overlay planes (create 8-bit bitmap).
 * Implemented flipping and rotation of overlay planes.
 *
 * Revision 1.2  1998/12/14 17:28:18  joergr
 * Added methods to add and remove additional overlay planes (still untested).
 * Added methods to support overlay labels and descriptions.
 *
 * Revision 1.1  1998/11/27 15:45:09  joergr
 * Added copyright message.
 * Added method to detach pixel data if it is no longer needed.
 * Added methods and constructors for flipping and rotating, changed for
 * scaling and clipping.
 *
 * Revision 1.7  1998/07/01 08:39:26  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.6  1998/05/11 14:53:26  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
