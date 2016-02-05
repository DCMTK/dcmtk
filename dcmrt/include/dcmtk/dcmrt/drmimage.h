/*
 *
 *  Copyright (C) 2012-2016, OFFIS e.V.
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
 *  Purpose: Mid-Level API for RT Image objects
 *
 */

#ifndef DRMIMAGE_H
#define DRMIMAGE_H

#include "dcmtk/config/osconfig.h"     /* make sure OS specific configuration is included first */
#include "dcmtk/dcmrt/drtimage.h"
#include "dcmtk/dcmimgle/dcmimage.h"


/**
 * Helper class for working with radio therapy image objects. This class provides
 * a binding between DicomImage and DRTImageIOD. The information from the
 * dataset are managed by the DRTImageIOD base class, but the image itself is
 * represented through DicomImage.
 *
 * @note These functions hopefully simplify working with DRTImageIOD. This is a
 * work in progress, please propose any improvements which would make working
 * with this class easier for you and that your consider useful.
 */
class DCMTK_DCMRT_EXPORT DRTImage : public DRTImageIOD
{
public:
    /** Default constructor */
    DRTImage();

    /** Destructor */
    ~DRTImage();

    /** Load an object from a file
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

    /** Load an RT Image from a given dataset. Please note that this DRTImage
     *  instance assumes ownership of the dataset pointer and will delete it
     *  when done with it. You must not delete it yourself. This applies even
     *  when the function returns an error.
     *  @param  dataset DICOM dataset from which the RT Image should be read
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmItem *dataset);

    /** Get the DicomImage instance for this RT Image.
     *  @return the DicomImage instance for this RT Image, may be NULL!
     */
    DicomImage *getImage() { return image_; }
    /** Get the DicomImage instance for this RT Image.
     *  @return the DicomImage instance for this RT Image, may be NULL!
     */
    const DicomImage *getImage() const { return image_; }

    /** Get the number of pixels in one row
     *  @return number of pixels in one row
     */
    unsigned long getWidth() const
    {
        return (image_ != NULL) ? image_->getWidth() : 0;
    }

    /** Get the number of pixels in one column
     *  @return number of pixels in one column
     */
    unsigned long getHeight() const
    {
        return (image_ != NULL) ? image_->getHeight() : 0;
    }

    /** Get the number of bits per sample of the internal representation
     *  @return number of bits per sample of the internal representation
     */
    int getDepth() const
    {
        return (image_ != NULL) ? image_->getDepth() : 0;
    }

    /** Get the number of frames
     *  @return number of frames
     */
    unsigned long getFrameCount() const
    {
        return (image_ != NULL) ? image_->getFrameCount() : 0;
    }

    /** Get the current status code
     *  @return current status code
     */
    EI_Status getStatus() const
    {
        return (image_ != NULL) ? image_->getStatus() : EIS_InvalidImage;
    }

    /** render pixel data and return result as a memory buffer.
     *  apply VOI/PLUT transformation and (visible) overlay planes.
     *  The rendered pixel data is alway unsigned.
     *  @param  result  OFVector to fill with resulting frame's data
     *  @param  frame   number of frame to be rendered (0..n-1)
     *  @return EC_Normal if successful, else an error code
     */
    OFCondition getOutputData(OFVector<Uint8>& result, const unsigned long frame = 0);

    /** unset all VOI transformations (windows and LUTs).
     *
     *  @return EC_Normal if successful, else an error code
     */
    OFCondition setNoVoiTransformation()
    {
        if (image_ == NULL)
            return EC_Normal;
        return statusToCondition(image_->setNoVoiTransformation());
    }

    /** set automatically calculated minimum/maximum window.
     *  possibly active VOI LUT is implicitly disabled.
     *  Please note that the min/max values refer to the full pixel data (i.e. including
     *  all possible present frames).
     *
     ** @param  idx  ignore global min/max values if true (1)
     *
     *  @return EC_Normal if successful, else an error code
     */
    OFCondition setMinMaxWindow(const int idx = 0)
    {
        if (image_ == NULL)
            return EC_Normal;
        return statusToCondition(image_->setMinMaxWindow(idx));
    }

    /** set automatically calculated histogram window.
     *  possibly active VOI LUT is implicitly disabled.
     *
     ** @param  thresh  threshold value specifying percentage of histogram border which
     *                  shall be ignored (default: 5%).
     *
     *  @return EC_Normal if successful, else an error code
     */
    OFCondition setHistogramWindow(const double thresh = 0.05)
    {
        if (image_ == NULL)
            return EC_Normal;
        return statusToCondition(image_->setHistogramWindow(thresh));
    }

    /** set automatically calculated VOI window for the specified Region of Interest (ROI).
     *  The ROI is specified by means of a rectangle (left, top, width, height).  Only the part
     *  of the ROI that overlaps with the image is regarded - if the overlapping area is empty
     *  this method returns an error.
     *  Possibly active VOI LUT is implicitly disabled.
     *
     ** @param  left_pos  x-coordinate of the top left-hand corner of the ROI (starting from 0)
     *  @param  top_pos   y-coordinate of the top left-hand corner of the ROI (starting from 0)
     *  @param  width     width in pixels of the rectangular ROI (minimum: 1)
     *  @param  height    height in pixels of the rectangular ROI (minimum: 1)
     *  @param  frame     index of the frame to be used for calculation (default: 0 = first)
     *
     *  @return EC_Normal if successful, else an error code
     */
    OFCondition setRoiWindow(const unsigned long left_pos,
                             const unsigned long top_pos,
                             const unsigned long width,
                             const unsigned long height,
                             const unsigned long frame = 0)
    {
        if (image_ == NULL)
            return EC_Normal;
        return statusToCondition(image_->setRoiWindow(left_pos, top_pos, width, height, frame));
    }

    /** set specified window (given by window width and center).
     *  possibly active VOI LUT is implicitly disabled.
     *
     ** @param  center  center of specified window
     *  @param  width   width of specified window (> 0.0)
     *
     *  @return EC_Normal if successful, else an error code
     */
    OFCondition setWindow(const double center,
                          const double width)
    {
        if (image_ == NULL)
            return EC_Normal;
        return statusToCondition(image_->setWindow(center, width));
    }

    /** get current window center and width values
     *
     ** @param  center  return current window center value
     *  @param  width   return current window width value
     *
     *  @return EC_Normal if successful, else an error code
     */
    OFCondition getWindow(double &center,
                          double &width)
    {
        center = 0;
        width = 0;
        if (image_ == NULL)
            return EC_Normal;
        return statusToCondition(image_->getWindow(center, width));
    }

    /**
     * Clear all internal state.
     */
    virtual void clear();

private:
    /// Reset this class image and DICOM object members
    virtual void reset();

    /**
     * Hide DRTImageIOD::read(). DicomImage() doesn't allow the dataset that was
     * used for loading the image to be deleted. Thus, read() cannot work.
     */
    virtual OFCondition read(DcmItem & /*dataset*/)
    {
        return EC_IllegalCall;
    }

    /**
     * Turn a status code from DicomImage into an OFCondition object.
     * @param status status code
     * @return OFCondition instance for the given status
     */
    OFCondition statusToCondition(int status)
    {
        if (status)
            return EC_Normal;
        // This is our new "unknown error" OFCondition
        return RT_EC_UnsupportedValue;
    }

    /** undefined private copy constructor */
    DRTImage(const DRTImage &);

    /** undefined private assignment operator */
    DRTImage &operator=(const DRTImage &);

    /// File format that was used when last loading an image
    DcmFileFormat  format_;

    /// Dataset that image_ is using, or NULL if it uses format_
    DcmItem       *dataset_;

    /// DicomImage instance representing the image
    DicomImage    *image_;
};

#endif
