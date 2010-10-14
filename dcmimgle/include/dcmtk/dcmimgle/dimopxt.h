/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Purpose: DicomMonochromePixelTemplate (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:26 $
 *  CVS/RCS Revision: $Revision: 1.34 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIMOPXT_H
#define DIMOPXT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofbmanip.h"
#include "dcmtk/ofstd/ofcast.h"

#include "dcmtk/dcmimgle/dipxrept.h"
#include "dcmtk/dcmimgle/dimopx.h"
#include "dcmtk/dcmimgle/dimoopx.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to handle monochrome pixel data
 */
template<class T>
class DiMonoPixelTemplate
  : public DiMonoPixel,
    public DiPixelRepresentationTemplate<T>
{

 public:

    /** constructor
     *
     ** @param  count  number of pixels
     */
    DiMonoPixelTemplate(const unsigned long count)
      : DiMonoPixel(count),
        Data(NULL)
    {
        MinValue[0] = 0;
        MinValue[1] = 0;
        MaxValue[0] = 0;
        MaxValue[1] = 0;
        // allocate buffer of given size
        Data = new T[Count];
    }

    /** constructor
     *
     ** @param  pixel     pointer to input pixel data
     *  @param  modality  pointer to object managing modality transform
     */
    DiMonoPixelTemplate(const DiInputPixel *pixel,
                        DiMonoModality *modality)
      : DiMonoPixel(pixel, modality),
        Data(NULL)
    {
        MinValue[0] = 0;
        MinValue[1] = 0;
        MaxValue[0] = 0;
        MaxValue[1] = 0;
    }

    /** constructor
     *
     ** @param  pixel     pointer to output pixel data used for intermediate representation
     *  @param  modality  pointer to object managing modality transform
     */
    DiMonoPixelTemplate(DiMonoOutputPixel *pixel,
                        DiMonoModality *modality)
      : DiMonoPixel(pixel, modality),
        Data(OFstatic_cast(T *, pixel->getDataPtr()))
    {
        MinValue[0] = 0;
        MinValue[1] = 0;
        MaxValue[0] = 0;
        MaxValue[1] = 0;
    }

    /** destructor
     */
    virtual ~DiMonoPixelTemplate()
    {
        delete[] Data;
    }

    /** get integer representation
     *
     ** @return integer representation of the internally stored pixel data
     */
    inline EP_Representation getRepresentation() const
    {
        return DiPixelRepresentationTemplate<T>::getRepresentation();
    }

    /** get pointer to internal pixel data
     *
     ** @return pointer to pixel data
     */
    inline const void *getData() const
    {
        return OFstatic_cast(const void *, Data);
    }

    /** get pointer to internal pixel data
     *
     ** @return pointer to pixel data
     */
    inline void *getDataPtr()
    {
        return OFstatic_cast(void *, Data);
    }

    /** get reference to pointer to internal pixel data.
     *  The returned array points to the (single) image plane.  The behaviour of
     *  this method is, therefore, identical for both monochrome and color images.
     *
     ** @return reference to pointer to pixel data
     */
    inline void *getDataArrayPtr()
    {
        return OFstatic_cast(void *, &Data);
    }

    /** get minimum and maximum pixel values
     *
     ** @param  min  reference to storage area for minimum pixel value
     *  @param  max  reference to storage area for maximum pixel value
     *
     ** @return status, true if successful, false otherwise
     */
    inline int getMinMaxValues(double &min,
                               double &max) const
    {
        min = MinValue[0];
        max = MaxValue[0];
        return 1;
    }

    /** get automatically computed min-max window
     *
     ** @param  idx     ignore global min/max pixel values if > 0
     *  @param  center  reference to storage area for window center value
     *  @param  width   reference to storage area for window width value
     *
     ** @return status, true if successful, false otherwise
     */
    inline int getMinMaxWindow(const int idx,
                               double &center,
                               double &width)
    {
        int result = 0;
        if ((idx >= 0) && (idx <= 1))
        {
            if ((idx == 1) && (MinValue[1] == 0) && (MaxValue[1] == 0))
                determineMinMax(0, 0, 0x2);                                     // determine on demand
            /* suppl. 33: "A Window Center of 2^n-1 and a Window Width of 2^n
                           selects the range of input values from 0 to 2^n-1."
            */
            center = (OFstatic_cast(double, MinValue[idx]) + OFstatic_cast(double, MaxValue[idx]) + 1) / 2;  // type cast to avoid overflows !
            width = OFstatic_cast(double, MaxValue[idx]) - OFstatic_cast(double, MinValue[idx]) + 1;
            result = (width > 0);                                               // check for valid value
        }
        return result;
    }

    /** get automatically computed Region of Interest (ROI) window
     *
     ** @param  left_pos   x-coordinate of the top left-hand corner of the ROI (starting from 0)
     *  @param  top_pos    y-coordinate of the top left-hand corner of the ROI (starting from 0)
     *  @param  width      width in pixels of the rectangular ROI (minimum: 1)
     *  @param  height     height in pixels of the rectangular ROI (minimum: 1)
     *  @param  columns    number of columns (width) of the associated image
     *  @param  rows       number of rows (height) of the associated image
     *  @param  frame      index of the frame to be used for the calculation
     *  @param  voiCenter  reference to storage area for window center value
     *  @param  voiWidth   reference to storage area for window width value
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int getRoiWindow(const unsigned long left_pos,
                             const unsigned long top_pos,
                             const unsigned long width,
                             const unsigned long height,
                             const unsigned long columns,
                             const unsigned long rows,
                             const unsigned long frame,
                             double &voiCenter,
                             double &voiWidth)
    {
        int result = 0;
        if ((Data != NULL) && (left_pos < columns) && (top_pos < rows))
        {
            register T *p = Data + (columns * rows * frame) + (top_pos * columns) + left_pos;
            const unsigned long right_pos = (left_pos + width < columns) ? left_pos + width : columns;
            const unsigned long bottom = (top_pos + height < rows) ? top_pos + height : rows;
            const unsigned long skip_x = left_pos + (columns - right_pos);
            register unsigned long x;
            register unsigned long y;
            register T value = 0;
            register T min = *p;                    // get first pixel as initial value for min ...
            register T max = min;                   // ... and max
            for (y = top_pos; y < bottom; ++y)
            {
                for (x = left_pos; x < right_pos; ++x)
                {
                    value = *(p++);
                    if (value < min)
                        min = value;
                    else if (value > max)
                        max = value;
                }
                p += skip_x;                        // skip rest of current line and beginning of next
            }
            /* suppl. 33: "A Window Center of 2^n-1 and a Window Width of 2^n
                           selects the range of input values from 0 to 2^n-1."
            */
            voiCenter = (OFstatic_cast(double, min) + OFstatic_cast(double, max) + 1) / 2;  // type cast to avoid overflows !
            voiWidth = OFstatic_cast(double, max) - OFstatic_cast(double, min) + 1;
            result = (width > 0);                               // check for valid value
        }
        return result;
    }

    /** get automatically computed histogram window
     *
     ** @param  thresh  ignore certain percentage of pixels at lower and upper boundaries
     *  @param  center  reference to storage area for window center value
     *  @param  width   reference to storage area for window width value
     *
     ** @return status, true if successful, false otherwise
     */
    int getHistogramWindow(const double thresh,                 // could be optimized if necessary (see diinpxt.h)!
                           double &center,
                           double &width)
    {
        if ((Data != NULL) && (MinValue[0] < MaxValue[0]))
        {
            const Uint32 count = OFstatic_cast(Uint32, MaxValue[0] - MinValue[0] + 1);
            Uint32 *quant = new Uint32[count];
            if (quant != NULL)
            {
                register unsigned long i;
                OFBitmanipTemplate<Uint32>::zeroMem(quant, count);                  // initialize array
                for (i = 0; i < Count; ++i)
                {
                    if ((Data[i] >= MinValue[0]) && (Data[i] <= MaxValue[0]))       // only for stability !
                        ++quant[OFstatic_cast(Uint32, Data[i] - MinValue[0])];      // count values
#ifdef DEBUG
                    else
                        DCMIMGLE_WARN("invalid value (" << Data[i] << ") in DiMonoPixelTemplate<T>::getHistogramWindow()");
#endif
                }
                const Uint32 threshvalue = OFstatic_cast(Uint32, thresh * OFstatic_cast(double, Count));
                register Uint32 t = 0;
                i = 0;
                while ((i < count) && (t < threshvalue))
                    t += quant[i++];
                const T minvalue = (i < count) ? OFstatic_cast(T, MinValue[0] + i) : 0;
                t = 0;
                i = count;
                while ((i > 0) && (t < threshvalue))
                    t += quant[--i];
                const T maxvalue = (i > 0) ? OFstatic_cast(T, MinValue[0] + i) : 0;
                delete[] quant;
                if (minvalue < maxvalue)
                {
                    /* suppl. 33: "A Window Center of 2^n-1 and a Window Width of 2^n
                                   selects the range of input values from 0 to 2^n-1."
                    */
                    center = (OFstatic_cast(double, minvalue) + OFstatic_cast(double, maxvalue) + 1) / 2;
                    width = OFstatic_cast(double, maxvalue) - OFstatic_cast(double, minvalue) + 1;
                    return (width > 0);
                }
            }
        }
        return 0;
    }


 protected:

    /** constructor
     *
     ** @param  pixel     pointer to intermediate pixel data (not necessarily monochrome)
     *  @param  modality  pointer to object managing modality transform
     */
    DiMonoPixelTemplate(const DiPixel *pixel,
                        DiMonoModality *modality)
      : DiMonoPixel(pixel, modality),
        Data(NULL)
    {
        MinValue[0] = 0;
        MinValue[1] = 0;
        MaxValue[0] = 0;
        MaxValue[1] = 0;
    }

    /** constructor
     *
     ** @param  pixel  pointer to intermediate monochrome pixel data
     *  @param  count  number of pixels
     */
    DiMonoPixelTemplate(const DiMonoPixel *pixel,
                        const unsigned long count)
      : DiMonoPixel(pixel, count),
        Data(NULL)
    {
        MinValue[0] = 0;
        MinValue[1] = 0;
        MaxValue[0] = 0;
        MaxValue[1] = 0;
    }

    /** determine minimum and maximum pixel values
     *
     ** @param  minvalue  starting global minimum value (0 = invalid)
     *  @param  maxvalue  starting global maximum value (0 = invalid)
     *  @param  mode      calculate global min/max if 0x1 bit is set (default),
     *                    calculate next min/max if 0x2 bit is set
     */
    void determineMinMax(T minvalue = 0,
                         T maxvalue = 0,
                         const int mode = 0x1)
    {
        if (Data != NULL)
        {
            if (mode & 0x1)
            {
                if ((minvalue == 0) && (maxvalue == 0))
                {
                    DCMIMGLE_DEBUG("determining global minimum and maximum pixel values for monochrome image");
                    register T *p = Data;
                    register T value = *p;
                    register unsigned long i;
                    minvalue = value;
                    maxvalue = value;
                    for (i = Count; i > 1; --i)                 // could be optimized if necessary (see diinpxt.h) !
                    {
                        value = *(++p);
                        if (value < minvalue)
                            minvalue = value;
                        else if (value > maxvalue)
                            maxvalue = value;
                    }
                }
                MinValue[0] = minvalue;                         // global minimum
                MaxValue[0] = maxvalue;                         // global maximum
                MinValue[1] = 0;                                // invalidate value
                MaxValue[1] = 0;
            } else {
                minvalue = MinValue[0];
                maxvalue = MaxValue[0];
            }
            if (mode & 0x2)
            {
                DCMIMGLE_DEBUG("determining next minimum and maximum pixel values for monochrome image");
                register T *p = Data;
                register T value;
                register int firstmin = 1;
                register int firstmax = 1;
                register unsigned long i;
                for (i = Count; i != 0; --i)                    // could be optimized if necessary (see diinpxt.h) !
                {
                    value = *(p++);
                    if ((value > minvalue) && ((value < MinValue[1]) || firstmin))
                    {
                        MinValue[1] = value;
                        firstmin = 0;
                    }
                    if ((value < maxvalue) && ((value > MaxValue[1]) || firstmax))
                    {
                        MaxValue[1] = value;
                        firstmax = 0;
                    }
                }
            }
        }
    }

    /// pointer to pixel data
    T *Data;


 private:

    /// minimum pixel values (0 = global, 1 = ignoring global)
    T MinValue[2];
    /// maximum pixel values
    T MaxValue[2];

 // --- declarations to avoid compiler warnings

    DiMonoPixelTemplate(const DiMonoPixelTemplate<T> &);
    DiMonoPixelTemplate<T> &operator=(const DiMonoPixelTemplate<T> &);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dimopxt.h,v $
 * Revision 1.34  2010-10-14 13:16:26  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.33  2010-03-01 09:08:47  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.32  2009-11-25 16:09:22  joergr
 * Removed inclusion of header file "ofconsol.h". Added more logging messages.
 *
 * Revision 1.31  2009-10-28 14:38:17  joergr
 * Fixed minor issues in log output.
 *
 * Revision 1.30  2009-10-28 09:53:40  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.29  2009-09-28 13:27:30  joergr
 * Moved general purpose definition file from module dcmdata to ofstd, and
 * added new defines in order to make the usage easier.
 *
 * Revision 1.28  2006/08/15 16:30:11  meichel
 * Updated the code in module dcmimgle to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.27  2005/12/08 16:47:56  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.26  2004/10/19 12:58:24  joergr
 * Enhanced API documentation.
 *
 * Revision 1.25  2004/02/06 11:07:50  joergr
 * Distinguish more clearly between const and non-const access to pixel data.
 *
 * Revision 1.24  2004/01/05 14:52:20  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.23  2003/12/23 15:53:22  joergr
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.22  2003/12/09 10:02:04  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.21  2002/12/09 13:32:54  joergr
 * Renamed parameter/local variable to avoid name clashes with global
 * declaration left and/or right (used for as iostream manipulators).
 *
 * Revision 1.20  2002/10/21 10:13:51  joergr
 * Corrected wrong calculation of min/max pixel value in cases where the
 * stored pixel data exceeds the expected size.
 *
 * Revision 1.19  2002/06/26 16:05:43  joergr
 * Enhanced handling of corrupted pixel data and/or length.
 *
 * Revision 1.18  2001/11/19 12:56:27  joergr
 * Added parameter 'frame' to setRoiWindow().
 *
 * Revision 1.17  2001/09/28 13:09:30  joergr
 * Added method setRoiWindow() which automatically calculates a min-max VOI
 * window for a specified rectangular region of the image.
 * Made min-max window calculation consistent with latest release of the DICOM
 * standard (supplement 33).
 *
 * Revision 1.16  2001/06/01 15:49:47  meichel
 * Updated copyright header
 *
 * Revision 1.15  2000/05/03 09:46:29  joergr
 * Removed most informational and some warning messages from release built
 * (#ifndef DEBUG).
 *
 * Revision 1.14  2000/04/28 12:32:32  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.13  2000/04/27 13:08:41  joergr
 * Dcmimgle library code now consistently uses ofConsole for error output.
 *
 * Revision 1.12  2000/03/08 16:24:21  meichel
 * Updated copyright header.
 *
 * Revision 1.11  2000/03/03 14:09:14  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.10  1999/10/06 13:44:35  joergr
 * Corrected creation of PrintBitmap pixel data: VOI windows should be applied
 * before clipping to avoid that the region outside the image (border) is also
 * windowed (this requires a new method in dcmimgle to create a DicomImage
 * with the grayscale transformations already applied).
 *
 * Revision 1.9  1999/09/17 12:42:40  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 * Enhanced efficiency of the implementation to determine min/max values of
 * the input pixels.
 *
 * Revision 1.8  1999/05/31 12:35:16  joergr
 * Corrected bug concerning the conversion of color images to grayscale.
 *
 * Revision 1.7  1999/04/30 16:10:51  meichel
 * Minor code purifications to keep IBM xlC quiet
 *
 * Revision 1.6  1999/04/28 14:52:12  joergr
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.5  1999/03/24 17:20:16  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.4  1999/01/20 15:11:38  joergr
 * Replaced invocation of getCount() by member variable Count where possible.
 *
 * Revision 1.3  1999/01/11 09:36:13  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.2  1998/12/22 14:34:30  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.1  1998/11/27 15:36:43  joergr
 * Added copyright message.
 * Replaced delete by delete[] for array types.
 * Added method to give direct (non-const) access to internal data buffer.
 * Added support for new bit manipulation class.
 *
 * Revision 1.7  1998/07/01 08:39:25  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.6  1998/05/11 14:53:23  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
