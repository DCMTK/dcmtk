/*
 *
 *  Copyright (C) 1996-2015, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomOverlayPlane (Header)
 *
 */


#ifndef DIOVPLN_H
#define DIOVPLN_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofcast.h"

#include "dcmtk/dcmimgle/diutils.h"

#define INCLUDE_CSTDDEF
#include "dcmtk/ofstd/ofstdinc.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DcmOverlayData;
class DcmLongString;

class DiDocument;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to handle a single overlay plane
 */
class DCMTK_DCMIMGLE_EXPORT DiOverlayPlane
{

 public:

    /** constructor
     *
     ** @param  docu    pointer to dataset (encapsulated)
     *  @param  group   group number of the overlay plane
     *  @param  alloc   value for bits allocated of the surrounding image
     *  @param  stored  value for bits stored of the surrounding image
     *  @param  high    value for high bit of the surrounding image
     */
    DiOverlayPlane(const DiDocument *docu,
                   const unsigned int group,
                   Uint16 alloc,
                   const Uint16 stored,
                   const Uint16 high);

    /** constructor, additional
     *
     ** @param  group        group number of the overlay plane
     *  @param  left_pos     x-coordinate of the plane origin
     *  @param  top_pos      y-coordinate of the plane origin
     *  @param  columns      width of the overlay plane
     *  @param  rows         height of the overlay plane
     *  @param  data         element containing the plane data
     *  @param  label        element containing the plane label
     *  @param  description  element containing the plane description
     *  @param  mode         overlay plane mode
     */
    DiOverlayPlane(const unsigned int group,
                   const Sint16 left_pos,
                   const Sint16 top_pos,
                   const Uint16 columns,
                   const Uint16 rows,
                   const DcmOverlayData &data,
                   const DcmLongString &label,
                   const DcmLongString &description,
                   const EM_Overlay mode);

    /** constructor, copy
     *
     ** @param  plane    reference overlay plane to be copied
     *  @param  bit      position of the plane bit
     *  @param  data     pointer to buffer for plane data
     *  @param  temp     temporary buffer used to extract plane information
     *  @param  width    width of the array where the plane is stored
     *  @param  height   height of the array where the plane is stored
     *  @param  columns  width of the overlay plane
     *  @param  rows     height of the overlay plane
     */
    DiOverlayPlane(DiOverlayPlane *plane,
                   const unsigned int bit,
                   Uint16 *data,
                   Uint16 *temp,
                   const Uint16 width,
                   const Uint16 height,
                   const Uint16 columns,
                   const Uint16 rows);

    /** destructor
     */
    ~DiOverlayPlane();

    /** get x-coordinate of overlay plane origin
     *
     ** @param  left_pos  offset to be subtracted from the actual value (optional)
     *
     ** @return x-coordinate of overlay plane origin
     */
    inline Sint16 getLeft(const Sint32 left_pos = 0) const
    {
        return OFstatic_cast(Sint16, OFstatic_cast(Sint32, Left) - left_pos);
    }

    /** get y-coordinate of overlay plane origin
     *
     ** @param  top_pos  offset to be subtracted from the actual value (optional)
     *
     ** @return y-coordinate of overlay plane origin
     */
    inline Sint16 getTop(const Sint32 top_pos = 0) const
    {
        return OFstatic_cast(Sint16, OFstatic_cast(Sint32, Top) - top_pos);
    }

    /** get width of overlay plane
     *
     ** @return width of overlay plane
     */
    inline Uint16 getWidth() const
    {
        return Width;
    }

    /** get height of overlay plane
     *
     ** @return height of overlay plane
     */
    inline Uint16 getHeight() const
    {
        return Height;
    }

    /** get right border of overlay plane origin
     *
     ** @param  left_pos  offset to be subtracted from the actual value (optional).
     *
     ** @return right border of overlay plane origin. Negative values are set to 0.
     */
    inline Uint16 getRight(const Sint32 left_pos = 0) const
    {
        return (OFstatic_cast(Sint32, Left) + OFstatic_cast(Sint32, Width) - left_pos > 0) ?
            OFstatic_cast(Uint16, OFstatic_cast(Sint32, Left) + OFstatic_cast(Sint32, Width) - left_pos) : 0;
    }

    /** get bottom border of overlay plane origin
     *
     ** @param  top_pos  offset to be subtracted from the actual value (optional).
     *
     ** @return bottom border of overlay plane origin. Negative values are set to 0.
     */
    inline Uint16 getBottom(const Sint32 top_pos = 0) const
    {
        return (OFstatic_cast(Sint32, Top) + OFstatic_cast(Sint32, Height) - top_pos > 0) ?
            OFstatic_cast(Uint16, OFstatic_cast(Sint32, Top) + OFstatic_cast(Sint32, Height) - top_pos) : 0;
    }

    /** check whether overlay plane is valid
     *
     ** @return true if plane is valid, false otherwise
     */
    inline int isValid() const
    {
        return Valid;
    }

    /** check whether overlay plane is visible
     *
     ** @return true if plane is visible, false otherwise
     */
    inline int isVisible() const
    {
        return Visible;
    }

    /** make overlay plane visible
     */
    inline void show()
    {
        Visible = 1;
    }

    /** make overlay plane invisible
     */
    inline void hide()
    {
        Visible = 0;
    }

    /** move overlay plane to a new place
     *
     ** @param  left_pos  x-coordinate of the new plane origin (maybe negative)
     *  @param  top_pos   y-coordinate of the new plane origin (maybe negative)
     */
    void place(const signed int left_pos,
               const signed int top_pos);

    /** set scaling factor in x- and y-direction
     *
     ** @param  xfactor  scaling factor in x-direction
     *  @param  yfactor  scaling factor in y-direction
     */
    void setScaling(const double xfactor,
                    const double yfactor);

    /** set flipping
     *
     ** @param  horz     flip horizontally if true
     *  @param  vert     flip vertically if true
     *  @param  columns  width of surrounding image (incl. origin offset)
     *  @param  rows     height of surrounding image (incl. origin offset)
     */
    void setFlipping(const int horz,
                     const int vert,
                     const signed long columns,
                     const signed long rows);

    /** set rotation
     *
     ** @param  degree    angle by which the plane should be rotated
     *  @param  left_pos  x-coordinate of the origin for all overlay planes
     *  @param  top_pos   y-coordinate of the origin for all overlay planes
     *  @param  columns   width of surrounding image (already rotated)
     *  @param  rows      height of surrounding image (already rotated)
     */
    void setRotation(const int degree,
                     const signed long left_pos,
                     const signed long top_pos,
                     const Uint16 columns,
                     const Uint16 rows);

    /** make overlay plane visible and set parameters
     *
     ** @param  fore    foreground color of the plane (in percent: 0.0-1.0)
     *  @param  thresh  threshold value for 'threshold replace' (0.0-1.0)
     *  @param  mode    new overlay plane mode (EMO_Default for stored mode)
     */
    void show(const double fore,
              const double thresh,
              const EM_Overlay mode);

    /** make overlay plane visible and set p-value.
     *  Only applicable for bitmap shutters.
     *
     ** @param  pvalue  p-value to be used for the overlay plane (0..65535)
     *
     ** @return status, true if successful, false otherwise
     */
    int show(const Uint16 pvalue);

    /** get number of frames
     *
     ** @return number of frames
     */
    inline unsigned long getNumberOfFrames() const
    {
        return NumberOfFrames;
    }

    /** get foreground color of the plane
     *
     ** @return foreground color (in percent: 0.0-1.0)
     */
    inline double getForeground() const
    {
        return Foreground;
    }

    /** get threshold value of the plane
     *
     ** @return threshold value (in percent: 0.0-1.0)
     */
    inline double getThreshold() const
    {
        return Threshold;
    }

    /** get p-value of the plane.
     *  Only valid for bitmap shutters.
     *
     ** @return p-value (0..65535)
     */
    inline Uint16 getPValue() const
    {
        return PValue;
    }

    /** get overlay plane mode
     *
     ** @return overlay plane mode
     */
    inline EM_Overlay getMode() const
    {
        return Mode;
    }

    /** check whether overlay plane is embedded in the pixel data
     *
     ** @return true if plane is embedded, false otherwise
     */
    inline int isEmbedded() const
    {
        return EmbeddedData;
    }

    /** get label of overlay plane
     *
     ** @return label if successful, NULL otherwise
     */
    const char *getLabel() const
    {
        return (Label.empty()) ? OFstatic_cast(const char *, NULL) : Label.c_str();
    }

    /** get description of overlay plane
     *
     ** @return description if successful, NULL otherwise
     */
    const char *getDescription() const
    {
        return (Description.empty()) ? OFstatic_cast(const char *, NULL) : Description.c_str();
    }

    /** get group number of overlay plane
     *
     ** @return group number (0x6000-0x60ff)
     */
    Uint16 getGroupNumber() const
    {
        return GroupNumber;
    }

    /** get overlay plane data as an array of 1/8/16 bit values.
     *  Overlay plane is clipped to the area specified by the four min/max coordinates.
     *  Memory isn't handled internally and must therefore be deleted from calling program.
     *
     ** @param  frame  number of frame
     *  @param  xmin   x-coordinate of the top left hand corner
     *  @param  ymin   y-coordinate of the top left hand corner
     *  @param  xmax   x-coordinate of the bottom right hand corner
     *  @param  ymax   y-coordinate of the bottom right hand corner
     *  @param  bits   number of bits (stored) in the resulting array
     *  @param  fore   foreground color used for the plane (0x00-0xff)
     *  @param  back   transparent background color (0x00-0xff)
     *
     ** @return pointer to pixel data if successful, NULL otherwise
     */
    void *getData(const unsigned long frame,
                  const Uint16 xmin,
                  const Uint16 ymin,
                  const Uint16 xmax,
                  const Uint16 ymax,
                  const int bits,
                  const Uint16 fore,
                  const Uint16 back);

    /** create overlay plane data in (6xxx,3000) format.
     *  (1 bit allocated and stored, foreground color is 1, background color is 0,
     *   data is 16 bit padded - even length)
     *  Memory isn't handled internally and must therefore be deleted from calling program.
     *
     ** @param  buffer  stores pointer to overlay data (memory is allocated internally)
     *  @param  width   returns width of overlay plane (in pixels)
     *  @param  height  returns height of overlay plane (in pixels)
     *  @param  frames  returns number of frames (multiple overlay frames possible!)
     *
     ** @return number of bytes allocated for the 'buffer' if successful, 0 otherwise
     */
    unsigned long create6xxx3000Data(Uint8 *&buffer,
                                     unsigned int &width,
                                     unsigned int &height,
                                     unsigned long &frames);

    /** reset internal 'cursor' to the beginning of the specified frame
     *
     ** @param  frame  number of current frame
     *
     ** @return status, true if successful, false otherwise
     */
    inline int reset(const unsigned long frame);

    /** get value of the current overlay plane bit and move 'cursor' to the next position
     *
     ** @return true if plane bit is set, false otherwise
     */
    inline int getNextBit();

    /** set internal 'cursor' to a specific position
     *
     ** @param  x  new x-coordinate to start from
     *  @param  y  new y-coordinate to start from
     */
    inline void setStart(const Uint16 x,
                         const Uint16 y);


 protected:

    /// number of frames
    Uint32 NumberOfFrames;
    /// number of starting frame
    Uint16 ImageFrameOrigin;
    /// first frame to be processed (from DicomImage constructor)
    Uint32 FirstFrame;

    /// y-coordinate of overlay plane's origin
    Sint16 Top;
    /// x-coordinate of overlay plane's origin
    Sint16 Left;
    /// visible height
    Uint16 Height;
    /// visible width
    Uint16 Width;
    /// number of (stored) rows
    Uint16 Rows;
    /// number of (stored) columns
    Uint16 Columns;
    /// number of allocated bits per pixel
    Uint16 BitsAllocated;
    /// position of overlay plane bit
    Uint16 BitPosition;

    /// "color" of overlay plane (in percent: '0.0' = dark, '1.0' = bright)
    double Foreground;
    /// threshold value used for "threshold replace"
    double Threshold;
    /// P-value used for bitmap shutters
    Uint16 PValue;

    /// current overlay mode
    EM_Overlay Mode;
    /// default (stored) overlay mode
    EM_Overlay DefaultMode;

    /// label of overlay plane
    OFString Label;
    /// textual description of overlay plane
    OFString Description;

    /// group number of overlay plane
    Uint16 GroupNumber;

    /// validity status
    int Valid;
    /// visibility status
    int Visible;


 private:

    /// current bit position
    unsigned long BitPos;
    /// starting bit position of current frame
    unsigned long StartBitPos;

    /// x-coordinate of first pixel in surrounding memory buffer
    unsigned int StartLeft;
    /// y-coordinate of first pixel in surrounding memory buffer
    unsigned int StartTop;

    /// true, if overlay data in embedded in pixel data
    int EmbeddedData;

    /// pointer to current element of 'Data'
    const Uint16 *Ptr;
    /// pointer to starting element of current frame
    const Uint16 *StartPtr;
    /// pointer to overlay data (standalone) or pixel data (embedded)
    const Uint16 *Data;

 // --- declarations to avoid compiler warnings

    DiOverlayPlane(const DiOverlayPlane &);
    DiOverlayPlane &operator=(const DiOverlayPlane &);
};


/********************************************************************/


inline int DiOverlayPlane::reset(const unsigned long frame)
{
    int result = 0;
    if (Valid && (Data != NULL))
    {
        const Uint32 frameNumber = OFstatic_cast(Uint32, FirstFrame + frame);
        DCMIMGLE_TRACE("reset overlay plane in group 0x" << STD_NAMESPACE hex << GroupNumber << " to start position");
        DCMIMGLE_TRACE("  frameNumber: " << frameNumber << " (" << FirstFrame << "+" << frame
            << "), ImageFrameOrigin: " << ImageFrameOrigin << ", NumberOfFrames: " << NumberOfFrames);
        if ((frameNumber >= ImageFrameOrigin) && (frameNumber < ImageFrameOrigin + NumberOfFrames))
        {
            const unsigned long bits = (OFstatic_cast(unsigned long, StartLeft) + OFstatic_cast(unsigned long, StartTop) *
                OFstatic_cast(unsigned long, Columns) + (frameNumber - ImageFrameOrigin) * OFstatic_cast(unsigned long, Rows) *
                OFstatic_cast(unsigned long, Columns)) * OFstatic_cast(unsigned long, BitsAllocated);
            StartBitPos = BitPos = OFstatic_cast(unsigned long, BitPosition) + bits;
            DCMIMGLE_TRACE("  StartBitPos: " << StartBitPos << ", BitPosition: " << BitPosition << ", bits: " << bits);
            /* distinguish between embedded and separate overlay data */
            if (BitsAllocated == 16)
                StartPtr = Ptr = Data + (bits >> 4);
            else
                StartPtr = Data;
            result = (getRight() > 0) && (getBottom() > 0);
        } else
            DCMIMGLE_TRACE("  -> overlay plane does not apply to this frame");
    }
    return result;
}


inline int DiOverlayPlane::getNextBit()
{
    int result;
    if (BitsAllocated == 16)                                        // optimization
        result = OFstatic_cast(int, *(Ptr++) & (1 << BitPosition));
    else
    {
        Ptr = StartPtr + (BitPos >> 4);                             // div 16
        result = OFstatic_cast(int, *Ptr & (1 << (BitPos & 0xf)));  // mod 16
        BitPos += BitsAllocated;                                    // next bit
    }
    return result;
}


inline void DiOverlayPlane::setStart(const Uint16 x,
                                     const Uint16 y)
{
    if (BitsAllocated == 16)
        Ptr = StartPtr + OFstatic_cast(unsigned long, y - Top) * OFstatic_cast(unsigned long, Columns) +
            OFstatic_cast(unsigned long, x - Left);
    else
        BitPos = StartBitPos + (OFstatic_cast(unsigned long, y - Top) * OFstatic_cast(unsigned long, Columns) +
            OFstatic_cast(unsigned long, x - Left)) * OFstatic_cast(unsigned long, BitsAllocated);
}


#endif
