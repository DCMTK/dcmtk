/*
 *
 *  Copyright (C) 2012, OFFIS e.V.
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
 *  Module:  dcmrt
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: Mid-Level API for RT Dose objects
 *
 */

#ifndef DRMDOSE_H
#define DRMDOSE_H

#include "dcmtk/config/osconfig.h"     /* make sure OS specific configuration is included first */
#include "dcmtk/dcmrt/drtdose.h"


/**
 * helper class for working with radio therapy dose information objects. This
 * class provides several helper functions on top of DRTDoseIOD.
 *
 * @note These functions hopefully simplify working with DRTDoseIOD. This is a
 * work in progress, please propose any improvements which would make working
 * with this class easier for you and that your consider useful.
 */
class DCMTK_DCMRT_EXPORT DRTDose : public DRTDoseIOD
{
public:
    /** load an object from a file
     *  @param fileName name of the file to load (may contain wide chars if support enabled).
     *    Since there are various constructors for the OFFilename class, a "char *", "OFString"
     *    or "wchar_t *" can also be passed directly to this parameter.
     *  @param readMode read file with or without meta header, i.e. as a fileformat or a
     *    dataset.  Use ERM_fileOnly in order to force the presence of a meta header.
     *  @param readXfer transfer syntax used to read the data (auto detection if EXS_Unknown)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition loadFile(const OFFilename &fileName,
                                 const E_FileReadMode readMode = ERM_autoDetect,
                                 const E_TransferSyntax readXfer = EXS_Unknown);

    /** get the dose information for a single pixel. The result is the same as
     *  with getUnscaledDose() * getDoseGridScaling(). Please note that this
     *  function is inefficient when used for reading most of a frame. It is
     *  better to use getDoseImage() or getDoseImages() for this.
     *  @param x     x-coordinate of the pixel. The valid range is from 0 up to
     *               getDoseImageWidth()-1. Left edge of the image has
     *               coordinate 0.
     *  @param y     y-coordinate of the pixel. The valid range is from 0 up to
     *               getDoseImageHeight()-1. Top edge of the image has
     *               coordinate 0.
     *  @param frame frame to use. The first frame has number 0, the last
     *               frame is getNumberOfFrames()-1.
     *  @return dose value for the pixel or 0 if an error occurred.
     *  @see getDoseType() and getDoseUnits() for the meaning of dose values.
     */
    virtual double getDose(unsigned int x, unsigned int y, unsigned int frame = 0) const;

    /** get the dose information for a single pixel. The result is the same as
     *  with getUnscaledDose() * getDoseGridScaling(). Please note that this
     *  function is inefficient when used for reading most of a frame. It is
     *  better to use getDoseImage() or getDoseImages() for this.
     *  @param result after this function returns successfully, this value will
     *                be set to the dose information for the requested pixel.
     *  @param x      x-coordinate of the pixel. The valid range is from 0 up to
     *                getDoseImageWidth()-1. Left edge of the image has
     *                coordinate 0.
     *  @param y      y-coordinate of the pixel. The valid range is from 0 up to
     *                getDoseImageHeight()-1. Top edge of the image has
     *                coordinate 0.
     *  @param frame  frame to use. The first frame has number 0, the last
     *                frame is getNumberOfFrames()-1.
     *  @return status, EC_Normal if successful, an error code otherwise
     *  @see getDoseType() and getDoseUnits() for the meaning of dose values.
     */
    virtual OFCondition getDose(double &result, unsigned int x, unsigned int y, unsigned int frame = 0) const;

    /** get the unscaled dose information for a single pixel. Please note that
     *  this function is inefficient when used for reading most of a frame.
     *  @param x     x-coordinate of the pixel. The valid range is from 0 up to
     *               getDoseImageWidth()-1. Left edge of the image has
     *               coordinate 0.
     *  @param y     y-coordinate of the pixel. The valid range is from 0 up to
     *               getDoseImageHeight()-1. Top edge of the image has
     *               coordinate 0.
     *  @param frame frame to use. The first frame has number 0, the last
     *               frame is getNumberOfFrames()-1.
     *  @return unscaled dose value for the pixel or 0 if an error occured.
     */
    virtual double getUnscaledDose(unsigned int x, unsigned int y, unsigned int frame = 0) const;

    /** get the unscaled dose information for a single pixel. Please note that
     *  this function is inefficient when used for reading most of a frame.
     *  @param result after this function returns successfully, this value will
     *                be set to the unscaled dose information for the requested
     *                pixel.
     *  @param x      x-coordinate of the pixel. The valid range is from 0 up to
     *                getDoseImageWidth()-1. Left edge of the image has
     *                coordinate 0.
     *  @param y      y-coordinate of the pixel. The valid range is from 0 up to
     *                getDoseImageHeight()-1. Top edge of the image has
     *                coordinate 0.
     *  @param frame  frame to use. The first frame has number 0, the last
     *                frame is getNumberOfFrames()-1.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getUnscaledDose(double &result, unsigned int x, unsigned int y, unsigned int frame = 0) const;

    /** get the dose information for a single frame of the image.
     *  @param result after this function returns successfully, this will be set
     *                to an array with getDoseImageWidth() * getDoseImageHeight()
     *                entries, one for each pixel in the frame. However, when an
     *                error occurs, this vector is cleared.
     *  @param frame frame to use. The first frame has number 0, the last
     *               frame is getNumberOfFrames()-1.
     *  @return status, EC_Normal if successful, an error code otherwise
     *  @see getDoseType() and getDoseUnits() for the meaning of dose values.
     */
    virtual OFCondition getDoseImage(OFVector<double> &result, unsigned int frame) const;

    /** get the dose information for the whole image.
     *  @param result after this function returns successfully, this will be set
     *                to an array with one item per frame. The array for each
     *                frame will have getDoseImageWidth() * getDoseImageHeight()
     *                entries, one for each pixel in the frame. However, when an
     *                error occurs, this vector is cleared.
     *  @return status, EC_Normal if successful, an error code otherwise
     *  @see getDoseType() and getDoseUnits() for the meaning of dose values.
     */
    virtual OFCondition getDoseImages(OFVector<OFVector<double> > &result) const;

    /** get the width in pixel of the dose image
     *  @return the width of the dose image
     */
    virtual Uint16 getDoseImageWidth() const;

    /** get the height in pixel of the dose image
     *  @return the height of the dose image
     */
    virtual Uint16 getDoseImageHeight() const;

    /** check whether the current internal state is valid
     *  @return OFTrue if valid, OFFalse otherwise
     */
    virtual OFBool isValid();
};

#endif
