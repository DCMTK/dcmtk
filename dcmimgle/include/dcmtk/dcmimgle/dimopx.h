/*
 *
 *  Copyright (C) 1996-2014, OFFIS e.V.
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
 *  Purpose: DicomMonochromePixel (Header)
 *
 */


#ifndef DIMOPX_H
#define DIMOPX_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcast.h"

#include "dcmtk/dcmimgle/dipixel.h"
#include "dcmtk/dcmimgle/dimomod.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiInputPixel;
class DiMonoOutputPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Abstract base class to handle monochrome pixel data
 */
class DCMTK_DCMIMGLE_EXPORT DiMonoPixel
  : public DiPixel
{

 public:

    /** constructor
     *
     ** @param  count  number of pixels
     */
    DiMonoPixel(const unsigned long count);

    /** constructor
     *
     ** @param  pixel     pointer to input pixel data
     *  @param  modality  pointer to object managing modality transform
     */
    DiMonoPixel(const DiInputPixel *pixel,
                DiMonoModality *modality);

    /** constructor
     *
     ** @param  pixel     pointer to output pixel data used for intermediate representation
     *  @param  modality  pointer to object managing modality transform
     */
    DiMonoPixel(DiMonoOutputPixel *pixel,
                DiMonoModality *modality);

    /** destructor
     */
    virtual ~DiMonoPixel();

    /** get number of planes
     *
     ** @return number of planes (here 1, monochrome)
     */
    inline int getPlanes() const
    {
        return 1;
    }

    /** get minimum and maximum pixel values (abstract)
     *
     ** @param  min  reference to storage area for minimum pixel value
     *  @param  max  reference to storage area for maximum pixel value
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int getMinMaxValues(double &min,
                                double &max) const = 0;

    /** get automatically computed min-max window (abstract)
     *
     ** @param  idx     ignore global min/max pixel values if > 0
     *  @param  center  reference to storage area for window center value
     *  @param  width   reference to storage area for window width value
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int getMinMaxWindow(const int idx,
                                double &center,
                                double &width) = 0;

    /** get automatically computed Region of Interest (ROI) window (abstract)
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
                             double &voiWidth) = 0;

    /** get automatically computed histogram window (abstract)
     *
     ** @param  thresh  ignore certain percentage of pixels at lower and upper boundaries
     *  @param  center  reference to storage area for window center value
     *  @param  width   reference to storage area for window width value
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int getHistogramWindow(const double thresh,
                                   double &center,
                                   double &width) = 0;

    /** get number of bits per pixel
     *
     ** @return number of bits
     */
    inline unsigned int getBits() const
    {
        return (Modality != NULL) ? Modality->getBits() : 0;
    }

    /** get absolute minimum pixel value.
     *  The result depends on former transforms.
     *
     ** @return absolute minimum value
     */
    inline double getAbsMinimum() const
    {
        return (Modality != NULL) ? Modality->getAbsMinimum() : 0;
    }

    /** get absolute maximum pixel value.
     *  The result depends on former transforms.
     *
     ** @return absolute maximum value
     */
    inline double getAbsMaximum() const
    {
        return (Modality != NULL) ? Modality->getAbsMaximum() : 0;
    }

    /** get absolute pixel range
     *
     ** @return absolute pixel range
     */
    inline double getAbsMaxRange() const
    {
        return getAbsMaximum() - getAbsMinimum() + 1;
    }

    /** get modality LUT explanation string
     *
     ** @return modality LUT explanation
     */
    inline const char *getModalityLutExplanation() const
    {
        return (Modality != NULL) ? Modality->getExplanation() : OFstatic_cast(const char *, NULL);
    }

    /** add a reference to the internally managed modality transform object
     *
     ** @return pointer to modality transform object (might be NULL)
     */
    DiMonoModality *addReferenceToModality();

    /** check whether pixel data is 'potentially' signed.
     *  This check is necessary to interpret possibly following LUT descriptors correctly
     *  (see supplement 33)
     *
     ** @return true if pixel data is potentially signed, false otherwise
     */
    inline int isPotentiallySigned() const
    {
        return (getAbsMinimum() < 0);
    }


 protected:

    /** constructor
     *
     ** @param  pixel     pointer to intermediate pixel data (not necessarily monochrome)
     *  @param  modality  pointer to object managing modality transform
     */
    DiMonoPixel(const DiPixel *pixel,
                DiMonoModality *modality);

    /** constructor
     *
     ** @param  pixel  pointer to intermediate monochrome pixel data
     *  @param  count  number of pixels
     */
    DiMonoPixel(const DiMonoPixel *pixel,
                const unsigned long count);

    /// pointer to modality transform object
    DiMonoModality *Modality;


 private:

 // --- declarations to avoid compiler warnings

    DiMonoPixel(const DiMonoPixel &);
    DiMonoPixel &operator=(const DiMonoPixel &);
};


#endif
