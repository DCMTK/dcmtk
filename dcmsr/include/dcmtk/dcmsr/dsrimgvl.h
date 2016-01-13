/*
 *
 *  Copyright (C) 2000-2016, OFFIS e.V.
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRImageReferenceValue
 *
 */


#ifndef DSRIMGVL_H
#define DSRIMGVL_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrcomvl.h"
#include "dcmtk/dcmsr/dsrimgfr.h"
#include "dcmtk/dcmsr/dsrimgse.h"


/*-----------------------*
 *  forward declaration  *
 *-----------------------*/

class DicomImage;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for image reference values
 */
class DCMTK_DCMSR_EXPORT DSRImageReferenceValue
  : public DSRCompositeReferenceValue
{
    // allow access to getValuePtr()
    friend class DSRContentItem;

  public:

    /** default constructor
     */
    DSRImageReferenceValue();

    /** constructor.  Accepts reference to an image object.
     ** @param  sopClassUID     referenced SOP class UID of the image object.
     *                          (VR=UI, mandatory)
     *  @param  sopInstanceUID  referenced SOP instance UID of the image object.
     *                          (VR=UI, mandatory)
     *  @param  check           if enabled, check 'sopClassUID' and 'sopInstanceUID' for
     *                          validity before setting them.  See checkXXX() for details.
     *                          Empty values are never accepted.
     */
    DSRImageReferenceValue(const OFString &sopClassUID,
                           const OFString &sopInstanceUID,
                           const OFBool check = OFTrue);

    /** constructor.  Accepts reference to both an image and presentation state object.
     ** @param  imageSOPClassUID      referenced SOP class UID of the image object.
     *                                (VR=UI, mandatory)
     *  @param  imageSOPInstanceUID   referenced SOP instance UID of the image object.
     *                                (VR=UI, mandatory)
     *  @param  pstateSOPClassUID     referenced SOP class UID of the presentation state
     *                                object. (VR=UI, optional)
     *  @param  pstateSOPInstanceUID  referenced SOP instance UID of the presentation state
     *                                object. (VR=UI, optional)
     *  @param  check                 if enabled, check all four UID values for validity
     *                                before setting them.  See checkXXX() for details.
     *                                Empty values are never accepted.
     */
    DSRImageReferenceValue(const OFString &imageSOPClassUID,
                           const OFString &imageSOPInstanceUID,
                           const OFString &pstateSOPClassUID,
                           const OFString &pstateSOPInstanceUID,
                           const OFBool check = OFTrue);

    /** copy constructor
     ** @param  referenceValue  image reference value to be copied (not checked !)
     */
    DSRImageReferenceValue(const DSRImageReferenceValue &referenceValue);

    /** copy constructor
     ** @param  imageReferenceValue   reference to image object to be copied (not checked !)
     *  @param  pstateReferenceValue  reference to presentation state object to be copied
     *                                (not checked !)
     */
    DSRImageReferenceValue(const DSRCompositeReferenceValue &imageReferenceValue,
                           const DSRCompositeReferenceValue &pstateReferenceValue);

    /** destructor
     */
    virtual ~DSRImageReferenceValue();

    /** assignment operator
     ** @param  referenceValue  image reference value to be copied (not checked !)
     ** @return reference to this image reference value after 'referenceValue' has been copied
     */
    DSRImageReferenceValue &operator=(const DSRImageReferenceValue &referenceValue);

    /** clear all internal variables.
     *  Since an empty image reference is invalid the reference becomes invalid afterwards.
     */
    virtual void clear();

    /** check whether the current image reference value is valid.
     *  The reference value is valid if both SOP class UID and SOP instance UID are valid (see
     *  checkSOPClassUID() and checkSOPInstanceUID() for details), as well as the optional
     *  presentation state and real world value mapping objects (see checkPresentationState()
     *  and checkRealWorldValueMapping(), respectively).  Also the presence of the optional
     *  list of frame and segment numbers is checked using checkListData().
     ** @return OFTrue if reference value is valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether the content is short.
     *  This method is used to check whether the rendered output of this content item can be
     *  expanded inline or not (used for renderHTML()).
     ** @param  flags  flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return OFTrue if the content is short, OFFalse otherwise
     */
    virtual OFBool isShort(const size_t flags) const;

    /** check whether the current image reference points to a DICOM segmentation object
     ** @return OFTrue if a segmentation object is referenced, OFFalse otherwise
     */
    virtual OFBool isSegmentation() const;

    /** print image reference.
     *  The output of a typical image reference value looks like this: (CT image,"1.2.3") or
     *  (CT image,"1.2.3"),(GSPS,"1.2.3.4") if a presentation state is present.
     *  If the SOP class UID is unknown, the UID is printed instead of the related name.
     *  Also, the list of referenced frame/segment numbers is shown, but not the two UIDs of
     *  the real world value mapping object (if referenced).
     ** @param  stream  output stream to which the image reference value should be printed
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition print(STD_NAMESPACE ostream &stream,
                              const size_t flags) const;

    /** read image reference from XML document
     ** @param  doc     document containing the XML file content
     *  @param  cursor  cursor pointing to the starting node
     *  @param  flags   flag used to customize the reading process (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readXML(const DSRXMLDocument &doc,
                                DSRXMLCursor cursor,
                                const size_t flags);

    /** write image reference in XML format
     ** @param  stream  output stream to which the XML document is written
     *  @param  flags   flag used to customize the output (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(STD_NAMESPACE ostream &stream,
                                 const size_t flags) const;

    /** render image reference value in HTML/XHTML format.
     *  Please note that the optional icon image is never shown in the rendered output.
     ** @param  docStream    output stream to which the main HTML/XHTML document is written
     *  @param  annexStream  output stream to which the HTML/XHTML document annex is written
     *  @param  annexNumber  reference to the variable where the current annex number is
     *                       stored.  Value is increased automatically by 1 after a new entry
     *                       has been added.
     *  @param  flags        flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition renderHTML(STD_NAMESPACE ostream &docStream,
                                   STD_NAMESPACE ostream &annexStream,
                                   size_t &annexNumber,
                                   const size_t flags) const;

    /** create an icon image from the given DICOM image and associate it with this image
     *  reference.
     *  According to the DICOM standard, this icon image should be representative of the
     *  referenced image and the size of the icon image "may be no greater than 128 rows by
     *  128 columns".  For monochrome images, either the first stored or an automatically
     *  computed min-max VOI window is selected.  Color images are converted automatically to
     *  the photometric interpretation "PALETTE COLOR" (with 256 colors) when written to the
     *  DICOM dataset.
     *  Please note that this icon image is only used in readItem() and writeItem() but not
     *  in the other input/output methods.
     ** @param  filename  name of the DICOM image file to be used to create the icon image
     *  @param  frame     number of the frame to be used to create the icon image
     *                    (0 = 1st frame)
     *  @param  width     width of the icon image (in pixels).  If 0 this value will be
     *                    calculated automatically based on the given 'height'.
     *  @param  height    height of the icon image (in pixels).  If 0 this value will be
     *                    calculated automatically based on the given 'width'.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition createIconImage(const OFString &filename,
                                const unsigned long frame = 0,
                                const unsigned long width = 64,
                                const unsigned long height = 64);

    /** create an icon image from the given DICOM image and associate it with this image
     *  reference.
     *  According to the DICOM standard, this icon image should be representative of the
     *  referenced image and the size of the icon image "may be no greater than 128 rows by
     *  128 columns".  For monochrome images, either the first stored or an automatically
     *  computed min-max VOI window is selected.  Color images are converted automatically to
     *  the photometric interpretation "PALETTE COLOR" (with 256 colors) when written to the
     *  DICOM dataset.
     *  Please note that this icon image is only used in readItem() and writeItem() but not
     *  in the other input/output methods.
     ** @param  object  pointer to DICOM data structures (fileformat, dataset or item) that
     *                  contain the DICOM image to be used to create the icon image
     *  @param  xfer    transfer syntax of the 'object'.  In case of a fileformat or dataset,
     *                  the value EXS_Unknown is also allowed.
     *  @param  frame   number of the frame to be used to create the icon image (0 = 1st frame)
     *  @param  width   width of the icon image (in pixels).  If 0 this value will be
     *                  calculated automatically based on the given 'height'.
     *  @param  height  height of the icon image (in pixels).  If 0 this value will be
     *                  calculated automatically based on the given 'width'.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition createIconImage(DcmObject *object,
                                const E_TransferSyntax xfer = EXS_Unknown,
                                const unsigned long frame = 0,
                                const unsigned long width = 64,
                                const unsigned long height = 64);

    /** create an icon image from the given DICOM image and associate it with this image
     *  reference.
     *  According to the DICOM standard, this icon image should be representative of the
     *  referenced image and the size of the icon image "may be no greater than 128 rows by
     *  128 columns".  Color images are converted automatically to the photometric
     *  interpretation "PALETTE COLOR" (with 256 colors) when written to the DICOM dataset.
     *  Please note that this icon image is only used in readItem() and writeItem() but not
     *  in the other input/output methods.
     ** @param  image   pointer to DICOM image to be used to create the icon image.  Only
     *                  single frame images should be passed since only the first frame is
     *                  used.
     *  @param  width   width of the icon image (in pixels).  If 0 this value will be
     *                  calculated automatically based on the given 'height'.
     *  @param  height  height of the icon image (in pixels).  If 0 this value will be
     *                  calculated automatically based on the given 'width'.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition createIconImage(const DicomImage *image,
                                const unsigned long width = 64,
                                const unsigned long height = 64);

    /** delete the currently stored icon image, i.e.\ free the associated memory and "forget"
     *  the internal reference to it
     */
    void deleteIconImage();

    /** get reference to icon image associated with this image reference value (if any).
     *  Please note that the icon image might be invalid even if the pointer is not NULL.
     *  Therefore, the DicomImage::getStatus() method should always be called to check the
     *  status of the image.
     ** @return reference to icon image or NULL if none is present
     */
    const DicomImage *getIconImage() const
    {
        return IconImage;
    }

    /** get reference to image reference value
     ** @return reference to image reference value
     */
    inline const DSRImageReferenceValue &getValue() const
    {
        return *this;
    }

    /** get copy of image reference value
     ** @param  referenceValue  reference to variable in which the value should be stored
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getValue(DSRImageReferenceValue &referenceValue) const;

    /** set image reference value.
     *  Before setting the reference, it is usually checked.  If the value is invalid, the
     *  current value is not replaced and remains unchanged.
     ** @param  referenceValue  value to be set
     *  @param  check           if enabled, check value for validity before setting it.
     *                          See checkXXX() for details.  Empty values are never accepted.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setValue(const DSRImageReferenceValue &referenceValue,
                         const OFBool check = OFTrue);

    /** get reference to presentation state object
     ** @return reference to presentation state object (might be empty or invalid)
     */
    inline const DSRCompositeReferenceValue &getPresentationState() const
    {
        return PresentationState;
    }

    /** set reference to presentation state object.
     *  Before setting the reference, it is usually checked.  If the value is invalid, the
     *  current value is not replaced and remains unchanged.
     ** @param  pstateValue  value to be set
     *  @param  check        If enabled, check value for validity before setting it.  See
     *                       checkPresentationState() for details.  Empty UID values are
     *                       accepted for disabling the optional presentation state.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setPresentationState(const DSRCompositeReferenceValue &pstateValue,
                                     const OFBool check = OFTrue);

    /** get reference to real world value mapping object
     ** @return reference to real world value mapping object (might be empty or invalid)
     */
    inline const DSRCompositeReferenceValue &getRealWorldValueMapping() const
    {
        return RealWorldValueMapping;
    }

    /** set reference to real world value mapping object.
     *  Before setting the reference, it is usually checked.  If the value is invalid, the
     *  current value is not replaced and remains unchanged.
     ** @param  mappingValue  value to be set
     *  @param  check         If enabled, check value for validity before setting it.  See
     *                        checkRealWorldValueMapping() for details.  Empty UID values
     *                        are accepted for disabling the optional presentation state.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setRealWorldValueMapping(const DSRCompositeReferenceValue &mappingValue,
                                         const OFBool check = OFTrue);

    /** get reference to list of referenced frame numbers.
     *  According to the DICOM standard, this list is required if the referenced image has
     *  multiple frames, and the reference does not apply to all frames and the list of
     *  referenced segment numbers is empty/absent.
     ** @return reference to frame list
     */
    inline DSRImageFrameList &getFrameList()
    {
        return FrameList;
    }

    /** get read-only access to list of referenced frame numbers
     ** @return constant reference to frame list
     */
    inline const DSRImageFrameList &getFrameList() const
    {
        return FrameList;
    }

    /** get reference to list of referenced segment numbers.
     *  According to the DICOM standard, this list is required if the referenced image is
     *  a segmentation object, and the reference does not apply to all segments and the list
     *  of referenced frame numbers is empty/absent.
     ** @return reference to segment list
     */
    inline DSRImageSegmentList &getSegmentList()
    {
        return SegmentList;
    }

    /** get read-only access to list of referenced segment numbers
     ** @return constant reference to segment list
     */
    inline const DSRImageSegmentList &getSegmentList() const
    {
        return SegmentList;
    }

    /** check whether the image reference applies to a specific frame.
     *  The image reference applies to a frame (of multiframe images) if the list of
     *  referenced frame numbers is empty or the frame number is part of the list.
     ** @param  frameNumber  number of the frame to be checked
     ** @return OFTrue if reference applies to the specified frame, OFFalse otherwise
     */
    OFBool appliesToFrame(const Sint32 frameNumber) const;

    /** check whether the image reference applies to a specific segment.
     *  The image reference applies to a segment (of a segmentation image) if the list of
     *  referenced segment numbers is empty or the segment number is part of the list.
     ** @param  segmentNumber  number of the segment to be checked
     ** @return OFTrue if reference applies to the specified segment, OFFalse otherwise
     */
    OFBool appliesToSegment(const Uint16 segmentNumber) const;


  protected:

    /** get pointer to image reference value
     ** @return pointer to image reference value (never NULL)
     */
    inline DSRImageReferenceValue *getValuePtr()
    {
        return this;
    }

    /** read image reference value from dataset
     ** @param  dataset  DICOM dataset from which the value should be read
     *  @param  flags    flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition readItem(DcmItem &dataset,
                                 const size_t flags);

    /** write image reference value to dataset
     ** @param  dataset  DICOM dataset to which the value should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeItem(DcmItem &dataset) const;

    /** check whether the given SOP class UID refers to a DICOM segmentation object
     ** @param  sopClassUID  SOP class UID to be checked
     ** @return OFTrue if the UID refers to a segmentation object, OFFalse otherwise
     */
    virtual OFBool isSegmentationObject(const OFString &sopClassUID) const;

    /** check the specified SOP class UID for validity.
     *  This method further specializes the checks performed in the base class
     *  DSRCompositeReferenceValue.  All image and segmentation SOP classes that
     *  are defined in DICOM PS 3.6-2015c are allowed.
     ** @param  sopClassUID  SOP class UID to be checked
     ** @return status, EC_Normal if value is valid, an error code otherwise
     */
    virtual OFCondition checkSOPClassUID(const OFString &sopClassUID) const;

    /** check the given reference to a presentation state object for validity.
     *  The reference is "valid" if both UIDs are empty or both are not empty and
     *  SOP class UID refers to a softcopy presentation state object (see
     *  DSRTypes::E_PresentationStateType for a list of supported SOP classes).
     ** @param  referenceValue  value to be checked
     ** @return status, EC_Normal if value is valid, an error code otherwise
     */
    virtual OFCondition checkPresentationState(const DSRCompositeReferenceValue &referenceValue) const;

    /** check the given reference to a real world value mapping object for validity.
     *  The reference is "valid" if both UIDs are empty or both are not empty and
     *  SOP class UID refers to the "Real World Value Mapping Storage SOP Class".
     ** @param  referenceValue  value to be checked
     ** @return status, EC_Normal if value is valid, an error code otherwise
     */
    virtual OFCondition checkRealWorldValueMapping(const DSRCompositeReferenceValue &referenceValue) const;

    /** check the given list of frame and segment numbers for validity.
     *  Either both lists have to be empty or only one of them has to be non-empty,
     *  because otherwise the "type 1C" condition would be violated.  Also the list
     *  of segment numbers should only be non-empty for one of the DICOM segmentation
     *  objects (see isSegmentationObject()).
     ** @param  sopClassUID     SOP class UID of the image object to be checked
     *  @param  frameList       list of referenced frame numbers to be checked
     *  @param  segmentList     list of referenced segment numbers to be checked
     *  @param  reportWarnings  if enabled, report a warning message on each deviation
     *                          from an expected value to the logger
     ** @return status, EC_Normal if checked data is valid, an error code otherwise
     */
    OFCondition checkListData(const OFString &sopClassUID,
                              const DSRImageFrameList &frameList,
                              const DSRImageSegmentList &segmentList,
                              const OFBool reportWarnings = OFFalse) const;

    /** check the currently stored image reference value for validity.
     *  See above checkXXX() methods and DSRCompositeReferenceValue::checkCurrentValue()
     *  for details.
     ** @return status, EC_Normal if current value is valid, an error code otherwise
     */
    OFCondition checkCurrentValue() const;


  private:

    /// list of referenced frame numbers (associated DICOM VR=IS, VM=1-n, type 1C)
    DSRImageFrameList FrameList;
    /// list of referenced segment numbers (associated DICOM VR=US, VM=1-n, type 1C)
    DSRImageSegmentList SegmentList;
    /// composite reference value (UIDs) to presentation state object (optional)
    DSRCompositeReferenceValue PresentationState;
    /// composite reference value (UIDs) to real world value mapping object (optional)
    DSRCompositeReferenceValue RealWorldValueMapping;
    /// icon image from Icon Image Sequence (optional)
    DicomImage *IconImage;
};


#endif
