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
 *  Purpose: DicomScaleTemplates (Header)
 *
 */


#ifndef DISCALET_H
#define DISCALET_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofcast.h"

#include "dcmtk/dcmimgle/ditranst.h"
#include "dcmtk/dcmimgle/dipxrept.h"


/*---------------------*
 *  macro definitions  *
 *---------------------*/

#define SCALE_FACTOR 4096
#define HALFSCALE_FACTOR 2048


/*--------------------*
 *  helper functions  *
 *--------------------*/

// help function to set scaling values
static inline void setScaleValues(Uint16 data[],
                                  const Uint16 min,
                                  const Uint16 max)
{
    Uint16 remainder = max % min;
    Uint16 step0 = max / min;
    Uint16 step1 = max / min;
    if (remainder > OFstatic_cast(Uint16, min / 2))
    {
        remainder = min - remainder;
        ++step0;
    } else
        ++step1;
    const double count = OFstatic_cast(double, min) / (OFstatic_cast(double, remainder) + 1);
    Uint16 i;
    double c = count;
    for (i = 0; i < min; ++i)
    {
        if ((i >= OFstatic_cast(Uint16, c)) && (remainder > 0))
        {
            --remainder;
            c += count;
            data[i] = step1;
        }
        else
            data[i] = step0;
    }
}

// cubic value interpolation using Catmull-Rom formula.
// the interpolated pixel lies between the second and the third original pixels
static inline double cubicValue(const double v1,
                                const double v2,
                                const double v3,
                                const double v4,
                                const double dD,
                                const double minVal,
                                const double maxVal)
{
    double dVal = 0.5 * ((((-v1 + 3 * v2 - 3 * v3 + v4) * dD + (2 * v1 - 5 * v2 + 4 * v3 - v4)) * dD + (-v1 + v3)) * dD + (v2 + v2));
    return (dVal < minVal) ? minVal : ((dVal > maxVal) ? maxVal : dVal);
}


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to scale images (on pixel data level).
 *  with and without interpolation
 */
template<class T>
class DiScaleTemplate
  : public DiTransTemplate<T>
{

 public:

    /** constructor, scale clipping area.
     *
     ** @param  planes     number of planes (1 or 3)
     *  @param  columns    width of source image
     *  @param  rows       height of source image
     *  @param  left_pos   left coordinate of clipping area
     *  @param  top_pos    top coordinate of clipping area
     *  @param  src_cols   width of clipping area
     *  @param  src_rows   height of clipping area
     *  @param  dest_cols  width of destination image (scaled image)
     *  @param  dest_rows  height of destination image
     *  @param  frames     number of frames
     *  @param  bits       number of bits per plane/pixel
     */
    DiScaleTemplate(const int planes,
                    const Uint16 columns,           /* resolution of source image */
                    const Uint16 rows,
                    const signed long left_pos,     /* origin of clipping area */
                    const signed long top_pos,
                    const Uint16 src_cols,          /* extension of clipping area */
                    const Uint16 src_rows,
                    const Uint16 dest_cols,         /* extension of destination image */
                    const Uint16 dest_rows,
                    const Uint32 frames,            /* number of frames */
                    const int bits = 0)
      : DiTransTemplate<T>(planes, src_cols, src_rows, dest_cols, dest_rows, frames, bits),
        Left(left_pos),
        Top(top_pos),
        Columns(columns),
        Rows(rows)
    {
    }

    /** constructor, scale whole image.
     *
     ** @param  planes     number of planes (1 or 3)
     *  @param  src_cols   width of source image
     *  @param  src_rows   height of source image
     *  @param  dest_cols  width of destination image (scaled image)
     *  @param  dest_rows  height of destination image
     *  @param  frames     number of frames
     *  @param  bits       number of bits per plane/pixel
     */
    DiScaleTemplate(const int planes,
                    const Uint16 src_cols,          /* resolution of source image */
                    const Uint16 src_rows,
                    const Uint16 dest_cols,         /* resolution of destination image */
                    const Uint16 dest_rows,
                    const Uint32 frames,            /* number of frames */
                    const int bits = 0)
      : DiTransTemplate<T>(planes, src_cols, src_rows, dest_cols, dest_rows, frames, bits),
        Left(0),
        Top(0),
        Columns(src_cols),
        Rows(src_rows)
    {
    }

    /** destructor
     */
    virtual ~DiScaleTemplate()
    {
    }

    /** check whether template type T is signed or not
     *
     ** @return true if signed, false otherwise
     */
    inline int isSigned() const
    {
        const DiPixelRepresentationTemplate<T> rep;
        return rep.isSigned();
    }

    /** choose scaling/clipping algorithm depending on specified parameters.
     *
     ** @param  src          array of pointers to source image pixels
     *  @param  dest         array of pointers to destination image pixels
     *  @param  interpolate  preferred interpolation algorithm (0 = no interpolation, 1 = pbmplus algorithm,
     *                         2 = c't algorithm, 3 = bilinear magnification, 4 = bicubic magnification)
     *  @param  value        value to be set outside the image boundaries (used for clipping, default: 0)
     */
    void scaleData(const T *src[],
                   T *dest[],
                   const int interpolate,
                   const T value = 0)
    {
        if ((src != NULL) && (dest != NULL))
        {
            DCMIMGLE_TRACE("Col/Rows: " << Columns << " " << Rows << OFendl
                        << "Left/Top: " << Left << " " << Top << OFendl
                        << "Src  X/Y: " << this->Src_X << " " << this->Src_Y << OFendl
                        << "Dest X/Y: " << this->Dest_X << " " << this->Dest_Y);
            if ((Left + OFstatic_cast(signed long, this->Src_X) <= 0) || (Top + OFstatic_cast(signed long, this->Src_Y) <= 0) ||
                (Left >= OFstatic_cast(signed long, Columns)) || (Top >= OFstatic_cast(signed long, Rows)))
            {                                                                         // no image to be displayed
                DCMIMGLE_DEBUG("clipping area is fully outside the image boundaries");
                this->fillPixel(dest, value);                                         // ... fill bitmap
            }
            else if ((this->Src_X == this->Dest_X) && (this->Src_Y == this->Dest_Y))  // no scaling
            {
                if ((Left == 0) && (Top == 0) && (Columns == this->Src_X) && (Rows == this->Src_Y))
                    this->copyPixel(src, dest);                                       // copying
                else if ((Left >= 0) && (OFstatic_cast(Uint16, Left + this->Src_X) <= Columns) &&
                         (Top >= 0) && (OFstatic_cast(Uint16, Top + this->Src_Y) <= Rows))
                    clipPixel(src, dest);                                             // clipping
                else
                    clipBorderPixel(src, dest, value);                                // clipping (with border)
            }
            else if ((interpolate == 1) && (this->Bits <= MAX_INTERPOLATION_BITS))
                interpolatePixel(src, dest);                                          // interpolation (pbmplus)
            else if ((interpolate == 4) && (this->Dest_X >= this->Src_X) && (this->Dest_Y >= this->Src_Y) &&
                     (this->Src_X >= 3) && (this->Src_Y >= 3))
                bicubicPixel(src, dest);                                              // bicubic magnification
            else if ((interpolate >= 3) && (this->Dest_X >= this->Src_X) && (this->Dest_Y >= this->Src_Y) &&
                     (this->Src_X >= 2) && (this->Src_Y >= 2))
                bilinearPixel(src, dest);                                             // bilinear magnification
            else if ((interpolate >= 1) && (this->Dest_X >= this->Src_X) && (this->Dest_Y >= this->Src_Y))
                expandPixel(src, dest);                                               // interpolated expansion (c't)
            else if ((interpolate >= 1) && (this->Src_X >= this->Dest_X) && (this->Src_Y >= this->Dest_Y))
                reducePixel(src, dest);                                               // interpolated reduction (c't)
            else if ((interpolate >= 1) && (this->Bits <= MAX_INTERPOLATION_BITS))
                interpolatePixel(src, dest);                                          // interpolation (pbmplus), fallback
            else if ((this->Dest_X % this->Src_X == 0) && (this->Dest_Y % this->Src_Y == 0))
                replicatePixel(src, dest);                                            // replication
            else if ((this->Src_X % this->Dest_X == 0) && (this->Src_Y % this->Dest_Y == 0))
                suppressPixel(src, dest);                                             // suppression
            else
                scalePixel(src, dest);                                                // general scaling
        }
    }

 protected:

    /// left coordinate of clipping area
    const signed long Left;
    /// top coordinate of clipping area
    const signed long Top;
    /// width of source image
    const Uint16 Columns;
    /// height of source image
    const Uint16 Rows;


 private:

    /** clip image to specified area (only inside image boundaries).
     *  This is an optimization of the more general method clipBorderPixel().
     *
     ** @param  src   array of pointers to source image pixels
     *  @param  dest  array of pointers to destination image pixels
     */
    void clipPixel(const T *src[],
                   T *dest[])
    {
        DCMIMGLE_DEBUG("using clip image to specified area algorithm");
        const unsigned long x_feed = Columns - this->Src_X;
        const unsigned long y_feed = OFstatic_cast(unsigned long, Rows - this->Src_Y) * OFstatic_cast(unsigned long, Columns);
        Uint16 x;
        Uint16 y;
        const T *p;
        T *q;
        for (int j = 0; j < this->Planes; ++j)
        {
            p = src[j] + OFstatic_cast(unsigned long, Top) * OFstatic_cast(unsigned long, Columns) + Left;
            q = dest[j];
            for (unsigned long f = this->Frames; f != 0; --f)
            {
                for (y = this->Dest_Y; y != 0; --y)
                {
                    for (x = this->Dest_X; x != 0; --x)
                        *(q++) = *(p++);
                    p += x_feed;
                }
                p += y_feed;
            }
        }
    }

    /** clip image to specified area and add a border if necessary
     *
     ** @param  src    array of pointers to source image pixels
     *  @param  dest   array of pointers to destination image pixels
     *  @param  value  value to be set outside the image boundaries
     */
    void clipBorderPixel(const T *src[],
                         T *dest[],
                         const T value)
    {
        DCMIMGLE_DEBUG("using clip image to specified area and add border algorithm");
        const Uint16 s_left = (Left > 0) ? OFstatic_cast(Uint16, Left) : 0;
        const Uint16 s_top = (Top > 0) ? OFstatic_cast(Uint16, Top) : 0;
        const Uint16 d_left = (Left < 0 ? OFstatic_cast(Uint16, -Left) : 0);
        const Uint16 d_top = (Top < 0) ? OFstatic_cast(Uint16, -Top) : 0;
        const Uint16 d_right = (OFstatic_cast(unsigned long, this->Src_X) + OFstatic_cast(unsigned long, s_left) <
                                OFstatic_cast(unsigned long, Columns) + OFstatic_cast(unsigned long, d_left)) ?
                               (this->Src_X - 1) : (Columns + d_left - s_left - 1);
        const Uint16 d_bottom = (OFstatic_cast(unsigned long, this->Src_Y) + OFstatic_cast(unsigned long, s_top) <
                                 OFstatic_cast(unsigned long, Rows) + OFstatic_cast(unsigned long, d_top)) ?
                                (this->Src_Y - 1) : (Rows + d_top - s_top - 1);
        const Uint16 x_count = d_right - d_left + 1;
        const Uint16 y_count = d_bottom - d_top + 1;
        const unsigned long s_start = OFstatic_cast(unsigned long, s_top) * OFstatic_cast(unsigned long, Columns) + s_left;
        const unsigned long x_feed = Columns - x_count;
        const unsigned long y_feed = OFstatic_cast(unsigned long, Rows - y_count) * Columns;
        const unsigned long t_feed = OFstatic_cast(unsigned long, d_top) * OFstatic_cast(unsigned long, this->Src_X);
        const unsigned long b_feed = OFstatic_cast(unsigned long, this->Src_Y - d_bottom - 1) * OFstatic_cast(unsigned long, this->Src_X);

        /*
         *  The approach is to divide the destination image in up to four areas outside the source image
         *  plus one area for the source image. The for and while loops are scanning linearly over the
         *  destination image and setting the appropriate value depending on the current area. This is
         *  different from most of the other algorithms in this toolkit where the source image is scanned
         *  linearly.
         */
        Uint16 x;
        Uint16 y;
        unsigned long i;
        const T *p;
        T *q;
        for (int j = 0; j < this->Planes; ++j)
        {
            p = src[j] + s_start;
            q = dest[j];
            for (unsigned long f = this->Frames; f != 0; --f)
            {
                for (i = t_feed; i != 0; --i)               // top
                    *(q++) = value;
                for (y = y_count; y != 0; --y)              // middle part:
                {
                    x = 0;
                    while (x < d_left)                      // - left
                    {
                        *(q++) = value;
                        ++x;
                    }
                    while (x <= d_right)                    // - middle
                    {
                        *(q++) = *(p++);
                        ++x;
                    }
                    while (x < this->Src_X)                 // - right
                    {
                        *(q++) = value;
                        ++x;
                    }
                    p += x_feed;
                }
                for (i = b_feed; i != 0; --i)               // bottom
                    *(q++) = value;
                p += y_feed;
            }
        }
    }

    /** enlarge image by an integer factor.
     *  Pixels are replicated independently in both directions.
     *
     ** @param  src   array of pointers to source image pixels
     *  @param  dest  array of pointers to destination image pixels
     */
    void replicatePixel(const T *src[],
                        T *dest[])
    {
        DCMIMGLE_DEBUG("using replicate pixel scaling algorithm without interpolation");
        const Uint16 x_factor = this->Dest_X / this->Src_X;
        const Uint16 y_factor = this->Dest_Y / this->Src_Y;
        const unsigned long x_feed = Columns;
        const unsigned long y_feed = OFstatic_cast(unsigned long, Rows - this->Src_Y) * OFstatic_cast(unsigned long, Columns);
        const T *sp;
        Uint16 x;
        Uint16 y;
        Uint16 dx;
        Uint16 dy;
        const T *p;
        T *q;
        T value;
        for (int j = 0; j < this->Planes; ++j)
        {
            sp = src[j] + OFstatic_cast(unsigned long, Top) * OFstatic_cast(unsigned long, Columns) + Left;
            q = dest[j];
            for (unsigned long f = this->Frames; f != 0; --f)
            {
                for (y = this->Src_Y; y != 0; --y)
                {
                    for (dy = y_factor; dy != 0; --dy)
                    {
                        for (x = this->Src_X, p = sp; x != 0; --x)
                        {
                            value = *(p++);
                            for (dx = x_factor; dx != 0; --dx)
                                *(q++) = value;
                        }
                    }
                    sp += x_feed;
                }
                sp += y_feed;
            }
        }
    }

    /** shrink image by an integer divisor.
     *  Pixels are suppressed independently in both directions.
     *
     ** @param  src   array of pointers to source image pixels
     *  @param  dest  array of pointers to destination image pixels
     */
    void suppressPixel(const T *src[],
                       T *dest[])
    {
        DCMIMGLE_DEBUG("using suppress pixel scaling algorithm without interpolation");
        const unsigned int x_divisor = this->Src_X / this->Dest_X;
        const unsigned long x_feed = OFstatic_cast(unsigned long, this->Src_Y / this->Dest_Y) * OFstatic_cast(unsigned long, Columns) - this->Src_X;
        const unsigned long y_feed = OFstatic_cast(unsigned long, Rows - this->Src_Y) * OFstatic_cast(unsigned long, Columns);
        Uint16 x;
        Uint16 y;
        const T *p;
        T *q;
        for (int j = 0; j < this->Planes; ++j)
        {
            p = src[j] + OFstatic_cast(unsigned long, Top) * OFstatic_cast(unsigned long, Columns) + Left;
            q = dest[j];
            for (unsigned long f = this->Frames; f != 0; --f)
            {
                for (y = this->Dest_Y; y != 0; --y)
                {
                    for (x = this->Dest_X; x != 0; --x)
                    {
                        *(q++) = *p;
                        p += x_divisor;
                    }
                    p += x_feed;
                }
                p += y_feed;
            }
        }
    }

    /** free scaling method without interpolation.
     *  This algorithm is necessary for overlays (1 bpp).
     *
     ** @param  src   array of pointers to source image pixels
     *  @param  dest  array of pointers to destination image pixels
     */
    void scalePixel(const T *src[],
                    T *dest[])
    {
        DCMIMGLE_DEBUG("using free scaling algorithm without interpolation");
        const Uint16 xmin = (this->Dest_X < this->Src_X) ? this->Dest_X : this->Src_X;  // minimum width
        const Uint16 ymin = (this->Dest_Y < this->Src_Y) ? this->Dest_Y : this->Src_Y;  // minimum height
        Uint16 *x_step = new Uint16[xmin];
        Uint16 *y_step = new Uint16[ymin];
        Uint16 *x_fact = new Uint16[xmin];
        Uint16 *y_fact = new Uint16[ymin];

       /*
        *  Approach: If one pixel line has to be added or removed it is taken from the middle of the image (1/2).
        *  For two lines it is at 1/3 and 2/3 of the image and so on. It sounds easy but it was a hard job ;-)
        */

        if ((x_step != NULL) && (y_step != NULL) && (x_fact != NULL) && (y_fact != NULL))
        {
            Uint16 x;
            Uint16 y;
            if (this->Dest_X < this->Src_X)
                setScaleValues(x_step, this->Dest_X, this->Src_X);
            else if (this->Dest_X > this->Src_X)
                setScaleValues(x_fact, this->Src_X, this->Dest_X);
            if (this->Dest_X <= this->Src_X)
                OFBitmanipTemplate<Uint16>::setMem(x_fact, 1, xmin);  // initialize with default values
            if (this->Dest_X >= this->Src_X)
                OFBitmanipTemplate<Uint16>::setMem(x_step, 1, xmin);  // initialize with default values
            x_step[xmin - 1] += Columns - this->Src_X;                // skip to next line
            if (this->Dest_Y < this->Src_Y)
                setScaleValues(y_step, this->Dest_Y, this->Src_Y);
            else if (this->Dest_Y > this->Src_Y)
                setScaleValues(y_fact, this->Src_Y, this->Dest_Y);
            if (this->Dest_Y <= this->Src_Y)
                OFBitmanipTemplate<Uint16>::setMem(y_fact, 1, ymin);  // initialize with default values
            if (this->Dest_Y >= this->Src_Y)
                OFBitmanipTemplate<Uint16>::setMem(y_step, 1, ymin);  // initialize with default values
            y_step[ymin - 1] += Rows - this->Src_Y;                   // skip to next frame
            const T *sp;
            Uint16 dx;
            Uint16 dy;
            const T *p;
            T *q;
            T value;
            for (int j = 0; j < this->Planes; ++j)
            {
                sp = src[j] + OFstatic_cast(unsigned long, Top) * OFstatic_cast(unsigned long, Columns) + Left;
                q = dest[j];
                for (unsigned long f = 0; f < this->Frames; ++f)
                {
                    for (y = 0; y < ymin; ++y)
                    {
                        for (dy = 0; dy < y_fact[y]; ++dy)
                        {
                            for (x = 0, p = sp; x < xmin; ++x)
                            {
                                value = *p;
                                for (dx = 0; dx < x_fact[x]; ++dx)
                                    *(q++) = value;
                                p += x_step[x];
                            }
                        }
                        sp += OFstatic_cast(unsigned long, y_step[y]) * OFstatic_cast(unsigned long, Columns);
                    }
                }
            }
        }
        delete[] x_step;
        delete[] y_step;
        delete[] x_fact;
        delete[] y_fact;
    }


    /** free scaling method with interpolation
     *
     ** @param  src   array of pointers to source image pixels
     *  @param  dest  array of pointers to destination image pixels
     */
    void interpolatePixel(const T *src[],
                          T *dest[])
    {
        DCMIMGLE_DEBUG("using scaling algorithm with interpolation from pbmplus toolkit");
        if ((this->Src_X != Columns) || (this->Src_Y != Rows))
        {
            DCMIMGLE_ERROR("interpolated scaling and clipping at the same time not implemented ... ignoring clipping region");
            this->Src_X = Columns;            // temporarily removed 'const' for 'Src_X' in class 'DiTransTemplate'
            this->Src_Y = Rows;               //                             ... 'Src_Y' ...
        }

        /*
         *   based on scaling algorithm from "Extended Portable Bitmap Toolkit" (pbmplus10dec91)
         *   (adapted to be used with signed pixel representation, inverse images - mono1,
         *    various bit depths, multi-frame and multi-plane/color images)
         */

        Uint16 x;
        Uint16 y;
        const T *p;
        T *q;
        const T *sp = NULL;                         // initialization avoids compiler warning
        const T *fp;
        T *sq;

        const unsigned long sxscale = OFstatic_cast(unsigned long, (OFstatic_cast(double, this->Dest_X) / OFstatic_cast(double, this->Src_X)) * SCALE_FACTOR);
        const unsigned long syscale = OFstatic_cast(unsigned long, (OFstatic_cast(double, this->Dest_Y) / OFstatic_cast(double, this->Src_Y)) * SCALE_FACTOR);
        const signed long maxvalue = DicomImageClass::maxval(this->Bits - isSigned());

        T *xtemp = new T[this->Src_X];
        signed long *xvalue = new signed long[this->Src_X];

        if ((xtemp == NULL) || (xvalue == NULL))
        {
            DCMIMGLE_ERROR("can't allocate temporary buffers for interpolation scaling");
            this->clearPixel(dest);
        } else {
            for (int j = 0; j < this->Planes; ++j)
            {
                fp = src[j];
                sq = dest[j];
                for (unsigned long f = this->Frames; f != 0; --f)
                {
                    for (x = 0; x < this->Src_X; ++x)
                        xvalue[x] = HALFSCALE_FACTOR;
                    unsigned long yfill = SCALE_FACTOR;
                    unsigned long yleft = syscale;
                    int yneed = 1;
                    int ysrc = 0;
                    for (y = 0; y < this->Dest_Y; ++y)
                    {
                        if (this->Src_Y == this->Dest_Y)
                        {
                            sp = fp;
                            for (x = this->Src_X, p = sp, q = xtemp; x != 0; --x)
                                *(q++) = *(p++);
                            fp += this->Src_X;
                        }
                        else
                        {
                            while (yleft < yfill)
                            {
                                if (yneed && (ysrc < OFstatic_cast(int, this->Src_Y)))
                                {
                                    sp = fp;
                                    fp += this->Src_X;
                                    ++ysrc;
                                }
                                for (x = 0, p = sp; x < this->Src_X; ++x)
                                    xvalue[x] += yleft * OFstatic_cast(signed long, *(p++));
                                yfill -= yleft;
                                yleft = syscale;
                                yneed = 1;
                            }
                            if (yneed && (ysrc < OFstatic_cast(int, this->Src_Y)))
                            {
                                sp = fp;
                                fp += this->Src_X;
                                ++ysrc;
                                yneed = 0;
                            }
                            signed long v;
                            for (x = 0, p = sp, q = xtemp; x < this->Src_X; ++x)
                            {
                                v = xvalue[x] + yfill * OFstatic_cast(signed long, *(p++));
                                v /= SCALE_FACTOR;
                                *(q++) = OFstatic_cast(T, (v > maxvalue) ? maxvalue : v);
                                xvalue[x] = HALFSCALE_FACTOR;
                            }
                            yleft -= yfill;
                            if (yleft == 0)
                            {
                                yleft = syscale;
                                yneed = 1;
                            }
                            yfill = SCALE_FACTOR;
                        }
                        if (this->Src_X == this->Dest_X)
                        {
                            for (x = this->Dest_X, p = xtemp, q = sq; x != 0; --x)
                                *(q++) = *(p++);
                            sq += this->Dest_X;
                        }
                        else
                        {
                            signed long v = HALFSCALE_FACTOR;
                            unsigned long xfill = SCALE_FACTOR;
                            unsigned long xleft;
                            int xneed = 0;
                            q = sq;
                            for (x = 0, p = xtemp; x < this->Src_X; ++x, ++p)
                            {
                                xleft = sxscale;
                                while (xleft >= xfill)
                                {
                                    if (xneed)
                                    {
                                        ++q;
                                        v = HALFSCALE_FACTOR;
                                    }
                                    v += xfill * OFstatic_cast(signed long, *p);
                                    v /= SCALE_FACTOR;
                                    *q = OFstatic_cast(T, (v > maxvalue) ? maxvalue : v);
                                    xleft -= xfill;
                                    xfill = SCALE_FACTOR;
                                    xneed = 1;
                                }
                                if (xleft > 0)
                                {
                                    if (xneed)
                                    {
                                        ++q;
                                        v = HALFSCALE_FACTOR;
                                        xneed = 0;
                                    }
                                    v += xleft * OFstatic_cast(signed long, *p);
                                    xfill -= xleft;
                                }
                            }
                            if (xfill > 0)
                                v += xfill * OFstatic_cast(signed long, *(--p));
                            if (!xneed)
                            {
                                v /= SCALE_FACTOR;
                                *q = OFstatic_cast(T, (v > maxvalue) ? maxvalue : v);
                            }
                            sq += this->Dest_X;
                        }
                    }
                }
            }
        }
        delete[] xtemp;
        delete[] xvalue;
    }


    /** free scaling method with interpolation (only for magnification)
     *
     ** @param  src   array of pointers to source image pixels
     *  @param  dest  array of pointers to destination image pixels
     */
    void expandPixel(const T *src[],
                     T *dest[])
    {
        DCMIMGLE_DEBUG("using expand pixel scaling algorithm with interpolation from c't magazine");
        const double x_factor = OFstatic_cast(double, this->Src_X) / OFstatic_cast(double, this->Dest_X);
        const double y_factor = OFstatic_cast(double, this->Src_Y) / OFstatic_cast(double, this->Dest_Y);
        const unsigned long f_size = OFstatic_cast(unsigned long, Rows) * OFstatic_cast(unsigned long, Columns);
        const T *sp;
        double bx, ex;
        double by, ey;
        int bxi, exi;
        int byi, eyi;
        unsigned long offset;
        double value, sum;
        double x_part, y_part;
        double l_factor, r_factor;
        double t_factor, b_factor;
        int xi;
        int yi;
        Uint16 x;
        Uint16 y;
        const T *p;
        T *q;

        /*
         *   based on scaling algorithm from "c't - Magazin fuer Computertechnik" (c't 11/94)
         *   (adapted to be used with signed pixel representation, inverse images - mono1,
         *    various bit depths, multi-frame and multi-plane/color images, combined clipping/scaling)
         */

        for (int j = 0; j < this->Planes; ++j)
        {
            sp = src[j] + OFstatic_cast(unsigned long, Top) * OFstatic_cast(unsigned long, Columns) + Left;
            q = dest[j];
            for (unsigned long f = 0; f < this->Frames; ++f)
            {
                for (y = 0; y < this->Dest_Y; ++y)
                {
                    by = y_factor * OFstatic_cast(double, y);
                    ey = y_factor * (OFstatic_cast(double, y) + 1.0);
                    if (ey > this->Src_Y)
                    {
#ifdef DEBUG            // this output is only useful for debugging purposes
                        DCMIMGLE_TRACE("  limiting value of 'ey' to 'Src_Y': " << ey << " -> " << this->Src_Y);
#endif
                        // see reducePixel()
                        ey = this->Src_Y;
                    }
                    byi = OFstatic_cast(int, by);
                    eyi = OFstatic_cast(int, ey);
                    if (OFstatic_cast(double, eyi) == ey)
                    {
#ifdef DEBUG            // this output is only useful for debugging purposes
                        DCMIMGLE_TRACE("  decreasing value of 'eyi' by 1: " << eyi << " -> " << (eyi - 1));
#endif
                        --eyi;
                    }
                    y_part = OFstatic_cast(double, eyi) / y_factor;
                    b_factor = y_part - OFstatic_cast(double, y);
                    t_factor = (OFstatic_cast(double, y) + 1.0) - y_part;
                    for (x = 0; x < this->Dest_X; ++x)
                    {
                        value = 0;
                        bx = x_factor * OFstatic_cast(double, x);
                        ex = x_factor * (OFstatic_cast(double, x) + 1.0);
                        if (ex > this->Src_X)
                        {
#ifdef DEBUG                // this output is only useful for debugging purposes
                            DCMIMGLE_TRACE("  limiting value of 'ex' to 'Src_X': " << ex << " -> " << this->Src_X);
#endif
                            // see reducePixel()
                            ex = this->Src_X;
                        }
                        bxi = OFstatic_cast(int, bx);
                        exi = OFstatic_cast(int, ex);
                        if (OFstatic_cast(double, exi) == ex)
                        {
#ifdef DEBUG                // this output is only useful for debugging purposes
                            DCMIMGLE_TRACE("  decreasing value of 'exi' by 1: " << exi << " -> " << (exi - 1));
#endif
                            --exi;
                        }
                        x_part = OFstatic_cast(double, exi) / x_factor;
                        l_factor = x_part - OFstatic_cast(double, x);
                        r_factor = (OFstatic_cast(double, x) + 1.0) - x_part;
                        offset = OFstatic_cast(unsigned long, byi) * OFstatic_cast(unsigned long, Columns);
                        for (yi = byi; yi <= eyi; ++yi)
                        {
                            p = sp + offset + bxi;
                            for (xi = bxi; xi <= exi; ++xi)
                            {
                                sum = OFstatic_cast(double, *(p++));
                                if (bxi != exi)
                                {
                                    if (xi == bxi)
                                        sum *= l_factor;
                                    else
                                        sum *= r_factor;
                                }
                                if (byi != eyi)
                                {
                                    if (yi == byi)
                                        sum *= b_factor;
                                    else
                                        sum *= t_factor;
                                }
                                value += sum;
                            }
                            offset += Columns;
                        }
                        *(q++) = OFstatic_cast(T, value + 0.5);
                    }
                }
                sp += f_size;
            }
        }
    }


    /** free scaling method with interpolation (only for reduction)
     *
     ** @param  src   array of pointers to source image pixels
     *  @param  dest  array of pointers to destination image pixels
     */
    void reducePixel(const T *src[],
                     T *dest[])
    {
        DCMIMGLE_DEBUG("using reduce pixel scaling algorithm with interpolation from c't magazine");
        const double x_factor = OFstatic_cast(double, this->Src_X) / OFstatic_cast(double, this->Dest_X);
        const double y_factor = OFstatic_cast(double, this->Src_Y) / OFstatic_cast(double, this->Dest_Y);
        const double xy_factor = x_factor * y_factor;
        const unsigned long f_size = OFstatic_cast(unsigned long, Rows) * OFstatic_cast(unsigned long, Columns);
        const T *sp;
        double bx, ex;
        double by, ey;
        int bxi, exi;
        int byi, eyi;
        unsigned long offset;
        double value, sum;
        double l_factor, r_factor;
        double t_factor, b_factor;
        int xi;
        int yi;
        Uint16 x;
        Uint16 y;
        const T *p;
        T *q;

        /*
         *   based on scaling algorithm from "c't - Magazin fuer Computertechnik" (c't 11/94)
         *   (adapted to be used with signed pixel representation, inverse images - mono1,
         *    various bit depths, multi-frame and multi-plane/color images, combined clipping/scaling)
         */

        for (int j = 0; j < this->Planes; ++j)
        {
            sp = src[j] + OFstatic_cast(unsigned long, Top) * OFstatic_cast(unsigned long, Columns) + Left;
            q = dest[j];
            for (unsigned long f = 0; f < this->Frames; ++f)
            {
                for (y = 0; y < this->Dest_Y; ++y)
                {
                    by = y_factor * OFstatic_cast(double, y);
                    ey = y_factor * (OFstatic_cast(double, y) + 1.0);
                    if (ey > this->Src_Y)
                    {
#ifdef DEBUG            // this output is only useful for debugging purposes
                        DCMIMGLE_TRACE("  limiting value of 'ey' to 'Src_Y': " << ey << " -> " << this->Src_Y);
#endif
                        // yes, this can happen due to rounding, e.g. double(943) / double(471) * double(471)
                        // is something like 943.00000000000011368683772161602974 and then, the eyi == ey check
                        // fails to bring eyi back into range!
                        ey = this->Src_Y;
                    }
                    byi = OFstatic_cast(int, by);
                    eyi = OFstatic_cast(int, ey);
                    if (OFstatic_cast(double, eyi) == ey)
                    {
#ifdef DEBUG            // this output is only useful for debugging purposes
                        DCMIMGLE_TRACE("  decreasing value of 'eyi' by 1: " << eyi << " -> " << (eyi - 1));
#endif
                        --eyi;
                    }
                    b_factor = 1 + OFstatic_cast(double, byi) - by;
                    t_factor = ey - OFstatic_cast(double, eyi);
                    for (x = 0; x < this->Dest_X; ++x)
                    {
                        value = 0;
                        bx = x_factor * OFstatic_cast(double, x);
                        ex = x_factor * (OFstatic_cast(double, x) + 1.0);
                        if (ex > this->Src_X)
                        {
#ifdef DEBUG                // this output is only useful for debugging purposes
                            DCMIMGLE_TRACE("  limiting value of 'ex' to 'Src_X': " << ex << " -> " << this->Src_X);
#endif
                            // see above comment
                            ex = this->Src_X;
                        }
                        bxi = OFstatic_cast(int, bx);
                        exi = OFstatic_cast(int, ex);
                        if (OFstatic_cast(double, exi) == ex)
                        {
#ifdef DEBUG                // this output is only useful for debugging purposes
                            DCMIMGLE_TRACE("  decreasing value of 'exi' by 1: " << exi << " -> " << (exi - 1));
#endif
                            --exi;
                        }
                        l_factor = 1 + OFstatic_cast(double, bxi) - bx;
                        r_factor = ex - OFstatic_cast(double, exi);
                        offset = OFstatic_cast(unsigned long, byi) * OFstatic_cast(unsigned long, Columns);
                        for (yi = byi; yi <= eyi; ++yi)
                        {
                            p = sp + offset + bxi;
                            for (xi = bxi; xi <= exi; ++xi)
                            {
                                sum = OFstatic_cast(double, *(p++)) / xy_factor;
                                if (xi == bxi)
                                    sum *= l_factor;
                                else if (xi == exi)
                                    sum *= r_factor;
                                if (yi == byi)
                                    sum *= b_factor;
                                else if (yi == eyi)
                                    sum *= t_factor;
                                value += sum;
                            }
                            offset += Columns;
                        }
                        *(q++) = OFstatic_cast(T, value + 0.5);
                    }
                }
                sp += f_size;
            }
        }
    }

   /** bilinear interpolation method (only for magnification)
    *
    ** @param  src   array of pointers to source image pixels
    *  @param  dest  array of pointers to destination image pixels
    */
    void bilinearPixel(const T *src[],
                       T *dest[])
    {
        DCMIMGLE_DEBUG("using magnification algorithm with bilinear interpolation contributed by Eduard Stanescu");
        const double x_factor = OFstatic_cast(double, this->Src_X) / OFstatic_cast(double, this->Dest_X);
        const double y_factor = OFstatic_cast(double, this->Src_Y) / OFstatic_cast(double, this->Dest_Y);
        const unsigned long f_size = OFstatic_cast(unsigned long, Rows) * OFstatic_cast(unsigned long, Columns);
        const unsigned long l_offset = OFstatic_cast(unsigned long, this->Src_Y - 1) * OFstatic_cast(unsigned long, this->Dest_X);
        Uint16 x;
        Uint16 y;
        T *pD;
        T *pCurrTemp;
        const T *pCurrSrc;
        Uint16 nSrcIndex;
        double dOff;
        T *pT;
        const T *pS;
        const T *pF;

        // buffer used for storing temporarily the interpolated lines
        T *pTemp = new T[OFstatic_cast(unsigned long, this->Src_Y) * OFstatic_cast(unsigned long, this->Dest_X)];
        if (pTemp == NULL)
        {
            DCMIMGLE_ERROR("can't allocate temporary buffer for interpolation scaling");
            this->clearPixel(dest);
        } else {

            /*
             *   based on scaling algorithm contributed by Eduard Stanescu
             *   (adapted to be used with signed pixel representation, inverse images - mono1,
             *    various bit depths, multi-frame multi-plane/color images, combined clipping/scaling)
             */

            for (int j = 0; j < this->Planes; ++j)
            {
                pF = src[j] + OFstatic_cast(unsigned long, Top) * OFstatic_cast(unsigned long, Columns) + Left;
                pD = dest[j];
                for (unsigned long f = this->Frames; f != 0; --f)
                {
                    pT = pCurrTemp = pTemp;
                    pS = pCurrSrc = pF;
                    // first, interpolate the columns:
                    // column 0, just copy the source data column 0
                    for (y = this->Src_Y; y != 0; --y)
                    {
                        *(pCurrTemp) = *(pCurrSrc);
                        pCurrSrc += Columns;
                        pCurrTemp += this->Dest_X;
                    }
                    pCurrSrc = pS;
                    nSrcIndex = 0;
                    // column 1 to column Dest_X - 1
                    for (x = 1; x < this->Dest_X - 1; ++x)
                    {
                        pCurrTemp = ++pT;
                        dOff = x * x_factor - nSrcIndex;
                        dOff = (1.0 < dOff) ? 1.0 : dOff;
                        for (y = 0; y < this->Src_Y; ++y)
                        {
                            // use floating points in order to avoid possible integer overflow
                            const double v1 = OFstatic_cast(double, *(pCurrSrc));
                            const double v2 = OFstatic_cast(double, *(pCurrSrc + 1));
                            *(pCurrTemp) = OFstatic_cast(T, v1 + (v2 - v1) * dOff);
                            pCurrSrc += Columns;
                            pCurrTemp += this->Dest_X;
                        }
                        // don't go beyond the source data
                        if ((nSrcIndex < this->Src_X - 2) &&  (x * x_factor >= nSrcIndex + 1))
                        {
                            pS++;
                            nSrcIndex++;
                        }
                        pCurrSrc = pS;
                    }
                    pCurrTemp = ++pT;
                    // last column, just copy the source data column Src_X
                    for (y = this->Src_Y; y != 0; --y)
                    {
                        *(pCurrTemp) = *(pCurrSrc);
                        pCurrSrc += Columns;
                        pCurrTemp += this->Dest_X;
                    }
                    // now the columns are interpolated in temp buffer, so interpolate the lines
                    pT = pCurrTemp = pTemp;
                    // line 0, just copy the temp buffer line 0
                    for (x = this->Dest_X; x != 0; --x)
                       *(pD++) = *(pCurrTemp++);
                    nSrcIndex = 0;
                    pCurrTemp = pTemp;
                    for (y = 1; y < this->Dest_Y - 1; ++y)
                    {
                        dOff = y * y_factor - nSrcIndex;
                        dOff = (1.0 < dOff) ? 1.0 : dOff;
                        for (x = this->Dest_X; x != 0; --x)
                        {
                            // use floating points in order to avoid possible integer overflow
                            const double v1 = OFstatic_cast(double, *(pCurrTemp));
                            const double v2 = OFstatic_cast(double, *(pCurrTemp + this->Dest_X));
                            *(pD++) = OFstatic_cast(T, v1 + (v2 - v1) * dOff);
                            pCurrTemp++;
                        }
                        // don't go beyond the source data
                        if ((nSrcIndex < this->Src_Y - 2) && (y * y_factor >= nSrcIndex + 1))
                        {
                            pT += this->Dest_X;
                            nSrcIndex++;
                        }
                        pCurrTemp = pT;
                    }
                    // the last line, just copy the temp buffer line Src_X
                    pCurrTemp = pTemp + l_offset;
                    for (x = this->Dest_X; x != 0; --x)
                        *(pD++) = *(pCurrTemp++);
                    // skip to next frame
                    pF += f_size;
                }
            }
        }
        delete[] pTemp;
    }

   /** bicubic interpolation method (only for magnification)
    *
    ** @param  src   array of pointers to source image pixels
    *  @param  dest  array of pointers to destination image pixels
    */
    void bicubicPixel(const T *src[],
                      T *dest[])
    {
        DCMIMGLE_DEBUG("using magnification algorithm with bicubic interpolation contributed by Eduard Stanescu");
        const double minVal = (isSigned()) ? -OFstatic_cast(double, DicomImageClass::maxval(this->Bits - 1, 0)) : 0.0;
        const double maxVal = OFstatic_cast(double, DicomImageClass::maxval(this->Bits - isSigned()));
        const double x_factor = OFstatic_cast(double, this->Src_X) / OFstatic_cast(double, this->Dest_X);
        const double y_factor = OFstatic_cast(double, this->Src_Y) / OFstatic_cast(double, this->Dest_Y);
        const Uint16 xDelta = OFstatic_cast(Uint16, 1 / x_factor);
        const Uint16 yDelta = OFstatic_cast(Uint16, 1 / y_factor);
        const unsigned long f_size = OFstatic_cast(unsigned long, Rows) * OFstatic_cast(unsigned long, Columns);
        const unsigned long l_offset = OFstatic_cast(unsigned long, this->Src_Y - 1) * OFstatic_cast(unsigned long, this->Dest_X);
        Uint16 x;
        Uint16 y;
        T *pD;
        T *pCurrTemp;
        const T *pCurrSrc;
        Uint16 nSrcIndex;
        double dOff;
        T *pT;
        const T *pS;
        const T *pF;

        // buffer used for storing temporarily the interpolated lines
        T *pTemp = pT = pCurrTemp = new T[OFstatic_cast(unsigned long, this->Src_Y) * OFstatic_cast(unsigned long, this->Dest_X)];
        if (pTemp == NULL)
        {
            DCMIMGLE_ERROR("can't allocate temporary buffer for interpolation scaling");
            this->clearPixel(dest);
        } else {

            /*
             *   based on scaling algorithm contributed by Eduard Stanescu
             *   (adapted to be used with signed pixel representation, inverse images - mono1,
             *    various bit depths, multi-frame multi-plane/color images, combined clipping/scaling)
             */

            for (int j = 0; j < this->Planes; ++j)
            {
                pF = src[j] + OFstatic_cast(unsigned long, Top) * OFstatic_cast(unsigned long, Columns) + Left;
                pD = dest[j];
                for (unsigned long f = this->Frames; f != 0; --f)
                {
                    pT = pCurrTemp = pTemp;
                    pS = pCurrSrc = pF;
                    // first, interpolate the columns:
                    // column 0, just copy the source data column 0
                    for (y = this->Src_Y; y != 0; --y)
                    {
                        *(pCurrTemp) = *(pCurrSrc);
                        pCurrSrc += Columns;
                        pCurrTemp += this->Dest_X;
                    }
                    pCurrSrc = pS;
                    // for the next few columns, linear interpolation
                    for (x = 1; x < xDelta + 1; ++x)
                    {
                        pCurrSrc = pS;
                        pCurrTemp = ++pT;
                        dOff = x * x_factor;
                        dOff = (1.0 < dOff) ? 1.0 : dOff;
                        for (y = this->Src_Y; y != 0; --y)
                        {
                            *(pCurrTemp) = OFstatic_cast(T, *(pCurrSrc) + (*(pCurrSrc + 1) - *(pCurrSrc)) * dOff);
                            pCurrSrc += Columns;
                            pCurrTemp += this->Dest_X;
                        }
                    }
                    nSrcIndex = 1;
                    pCurrSrc = ++pS;
                    // the majority of the columns
                    for (x = xDelta + 1; x < this->Dest_X - 2 * xDelta; ++x)
                    {
                        pCurrTemp = ++pT;
                        dOff = x * x_factor - nSrcIndex;
                        dOff = (1.0 < dOff) ? 1.0 : dOff;
                        for (y = this->Src_Y; y != 0; --y)
                        {
                            *(pCurrTemp) = OFstatic_cast(T, cubicValue(*(pCurrSrc - 1), *(pCurrSrc), *(pCurrSrc + 1), *(pCurrSrc + 2), dOff, minVal, maxVal));
                            pCurrSrc += Columns;
                            pCurrTemp += this->Dest_X;
                        }
                        // don't go beyond the source data
                        if ((nSrcIndex < this->Src_X - 3) && (x * x_factor >= nSrcIndex + 1))
                        {
                            pS++;
                            nSrcIndex++;
                        }
                        pCurrSrc = pS;
                    }
                    // last few columns except the very last one, linear interpolation
                    for (x = this->Dest_X - 2 * xDelta; x < this->Dest_X - 1; ++x)
                    {
                        pCurrTemp = ++pT;
                        dOff = x * x_factor - nSrcIndex;
                        dOff = (1.0 < dOff) ? 1.0 : dOff;
                        for (y = this->Src_Y; y != 0; --y)
                        {
                            *(pCurrTemp) = OFstatic_cast(T, *(pCurrSrc) + (*(pCurrSrc + 1) - *(pCurrSrc)) * dOff);
                            pCurrSrc += Columns;
                            pCurrTemp += this->Dest_X;
                        }
                        // don't go beyond the source data
                        if ((nSrcIndex < this->Src_X - 2) && (x * x_factor >= nSrcIndex + 1))
                        {
                            pS++;
                            nSrcIndex++;
                        }
                        pCurrSrc = pS;
                    }
                    // last column, just copy the source data column Src_X
                    pCurrTemp = pTemp + this->Dest_X - 1;
                    pCurrSrc = pF + this->Src_X - 1;
                    for (y = this->Src_Y; y != 0; --y)
                    {
                        *(pCurrTemp) = *(pCurrSrc);
                        pCurrSrc += Columns;
                        pCurrTemp += this->Dest_X;
                    }
                    // now the columns are interpolated in temp buffer, so interpolate the lines
                    pT = pCurrTemp = pTemp;
                    // line 0, just copy the temp buffer line 0
                    for (x = this->Dest_X; x != 0; --x)
                        *(pD++) = *(pCurrTemp++);
                    // for the next few lines, linear interpolation between line 0 and 1 of the temp buffer
                    for (y = 1; y < yDelta + 1; ++y)
                    {
                        pCurrTemp = pTemp;
                        dOff = y * y_factor;
                        dOff = (1.0 < dOff) ? 1.0 : dOff;
                        for (x = this->Dest_X; x != 0; --x)
                        {
                            *(pD++) = OFstatic_cast(T, *(pCurrTemp) + (*(pCurrTemp + this->Dest_X) - *(pCurrTemp)) * dOff);
                            pCurrTemp++;
                        }
                    }
                    nSrcIndex = 1;
                    pCurrTemp = pT = pTemp + this->Dest_X;
                    for (y = yDelta + 1; y < this->Dest_Y - yDelta - 1; ++y)
                    {
                        dOff = y * y_factor - nSrcIndex;
                        dOff = (1.0 < dOff) ? 1.0 : dOff;
                        for (x = this->Dest_X; x != 0; --x)
                        {
                            *(pD++) = OFstatic_cast(T, cubicValue(*(pCurrTemp - this->Dest_X),*(pCurrTemp), *(pCurrTemp + this->Dest_X),
                                                                  *(pCurrTemp + this->Dest_X + this->Dest_X), dOff, minVal, maxVal));
                            pCurrTemp++;
                        }
                        // don't go beyond the source data
                        if ((nSrcIndex < this->Src_Y - 3) && (y * y_factor >= nSrcIndex + 1))
                        {
                            pT += this->Dest_X;
                            nSrcIndex++;
                        }
                        pCurrTemp = pT;
                    }
                    // the last few lines except the very last one, linear interpolation in between the second last and the last lines
                    pCurrTemp = pT = pTemp + OFstatic_cast(unsigned long, this->Src_Y - 2) * OFstatic_cast(unsigned long, this->Dest_X);
                    for (y = this->Dest_Y - yDelta - 1; y < this->Dest_Y - 1; ++y)
                    {
                        dOff = y * y_factor - nSrcIndex;
                        dOff = (1.0 < dOff) ? 1.0 : dOff;
                        for (x = this->Dest_X; x != 0; --x)
                        {
                            *(pD++) = OFstatic_cast(T, *(pCurrTemp) + (*(pCurrTemp + this->Dest_X) - *(pCurrTemp)) * dOff);
                            pCurrTemp++;
                        }
                        pCurrTemp = pT;
                    }
                    // the the last line, just copy the temp buffer line Src_X
                    pCurrTemp = pTemp + l_offset;
                    for (x = this->Dest_X; x != 0; --x)
                        *(pD++) = *(pCurrTemp++);
                    // skip to next frame
                    pF += f_size;
                }
            }
        }
        delete[] pTemp;
    }
};

#endif
