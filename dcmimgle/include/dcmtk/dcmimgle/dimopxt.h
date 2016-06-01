/*
 *
 *  Copyright (C) 1996-2016, OFFIS e.V.
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
#ifdef HAVE_STD__NOTHROW
        /* use a non-throwing new here (if available) because the allocated buffer can be huge */
        Data = new (std::nothrow) T[Count];
#else
        /* make sure that the pointer is set to NULL in case of error */
        try
        {
            Data = new T[Count];
        }
        catch (STD_NAMESPACE bad_alloc const &)
        {
            Data = NULL;
        }
#endif
        if (Data == NULL)
            DCMIMGLE_DEBUG("cannot allocate memory buffer for 'Data' in DiMonoPixelTemplate constructor");
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
#if defined(HAVE_STD__NOTHROW) && defined(HAVE_NOTHROW_DELETE)
        /* use a non-throwing delete (if available) */
        operator delete[] (Data, std::nothrow);
#else
        delete[] Data;
#endif
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
            T *p = Data + (columns * rows * frame) + (top_pos * columns) + left_pos;
            const unsigned long right_pos = (left_pos + width < columns) ? left_pos + width : columns;
            const unsigned long bottom = (top_pos + height < rows) ? top_pos + height : rows;
            const unsigned long skip_x = left_pos + (columns - right_pos);
            unsigned long x;
            unsigned long y;
            T value = 0;
            T min = *p;                             // get first pixel as initial value for min ...
            T max = min;                            // ... and max
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
                unsigned long i;
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
                Uint32 t = 0;
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
                    T *p = Data;
                    T value = *p;
                    unsigned long i;
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
                T *p = Data;
                T value;
                int firstmin = 1;
                int firstmax = 1;
                unsigned long i;
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
