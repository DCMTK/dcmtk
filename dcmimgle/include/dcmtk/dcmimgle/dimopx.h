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
 *  Purpose: DicomMonochromePixel (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:26 $
 *  CVS/RCS Revision: $Revision: 1.20 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
class DiMonoPixel
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

    /** check whether pixel data is 'potentially' signed.
     *  This check is necessary to interpret possibly folowing LUT descriptors correctly
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


/*
 *
 * CVS/RCS Log:
 * $Log: dimopx.h,v $
 * Revision 1.20  2010-10-14 13:16:26  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.19  2010-03-01 09:08:47  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.18  2005-12-08 16:47:55  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.17  2004/02/06 11:07:50  joergr
 * Distinguish more clearly between const and non-const access to pixel data.
 *
 * Revision 1.16  2003/12/08 18:45:29  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.15  2002/12/09 13:32:54  joergr
 * Renamed parameter/local variable to avoid name clashes with global
 * declaration left and/or right (used for as iostream manipulators).
 *
 * Revision 1.14  2001/11/19 12:56:15  joergr
 * Added parameter 'frame' to setRoiWindow().
 *
 * Revision 1.13  2001/09/28 13:07:43  joergr
 * Added method setRoiWindow() which automatically calculates a min-max VOI
 * window for a specified rectangular region of the image.
 *
 * Revision 1.12  2001/06/01 15:49:47  meichel
 * Updated copyright header
 *
 * Revision 1.11  2000/03/08 16:24:20  meichel
 * Updated copyright header.
 *
 * Revision 1.10  1999/10/06 13:43:29  joergr
 * Corrected creation of PrintBitmap pixel data: VOI windows should be applied
 * before clipping to avoid that the region outside the image (border) is also
 * windowed (this requires a new method in dcmimgle to create a DicomImage
 * with the grayscale transformations already applied).
 *
 * Revision 1.9  1999/09/17 12:41:26  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.8  1999/05/31 12:35:16  joergr
 * Corrected bug concerning the conversion of color images to grayscale.
 *
 * Revision 1.7  1999/04/28 17:03:48  joergr
 * Added type casts to NULL pointers returned as 'const char *' to avoid
 * compiler warnings reported by gcc 2.7.2.1 (Linux).
 *
 * Revision 1.6  1999/03/24 17:20:15  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.5  1999/02/03 17:33:18  joergr
 * Added member variable and related methods to store number of bits used for
 * pixel data.
 *
 * Revision 1.4  1999/01/11 09:35:56  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.3  1998/12/22 14:33:45  joergr
 * Added implementation of methods to return member variables AbsMinimum/
 * Maximum.
 *
 * Revision 1.2  1998/12/16 16:35:39  joergr
 * Added explanation string to LUT class (retrieved from dataset).
 *
 * Revision 1.1  1998/11/27 15:33:16  joergr
 * Added copyright message.
 * Introduced new pixel base class.
 *
 * Revision 1.4  1998/07/01 08:39:24  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.3  1998/05/11 14:53:23  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
