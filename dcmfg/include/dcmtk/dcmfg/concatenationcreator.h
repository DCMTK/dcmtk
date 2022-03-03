/*
 *
 *  Copyright (C) 2019, Open Connections GmbH
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmfg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class for creating Concatenations
 *
 */

#ifndef CONCATENATIONCREATOR_H
#define CONCATENATIONCREATOR_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmfg/fgdefine.h"

/** Class for creating Concatenations from existing SOP Instances.
 *  As input, a user can either provide a full SOP Instance with Pixel Data
 *  attribute, or a SOP Instance plus separate pixel data as a vector of frame data.
 *  The input file is uncompressed if necessary and possible, and the resulting
 *  concatenation instances will also contain uncompressed pixel data.
 *  During conversion, the original source pixel data is held into memory and
 *  additionally one of the concatenation instances the moment it is being produced
 *  when calling writeNextInstance() (see below).
 *  The following workflow must be used to create a Concatenation:
 *  <ul>
 *  <li>Call to one of the setCfgInput() methods in order to set the concatenation
 *  source data, i.e. the SOP Instance that should be split into a number of
 *  concatenation instances.</li>
 *  <li>Call to other setCfg...() calls in order to set conversion options. For example
 *  the number of frames per concatenation instance produced can be configured by calling
 *  setCfgFramesPerInstance(). Per default, 25 frames per instance are being used.</li>
 *  <li>Call to one of the writeNextInstance() methods in order to write the next
 *  concatenation instance.</li>
 *  <li>Call to the load() method in order to load of the the Concatenations found
 *  during scan(). Selection is done via the Concatenations Concatenation UID.
 *  The result is made available as a single, merged dataset without Pixel Data attribute
 *  and a vector containing all frames of the merged instance.</li>
 * </ul>
 */
class DCMTK_DCMFG_EXPORT ConcatenationCreator
{

public:
    /** Constructor
     */
    ConcatenationCreator();

    /** Virtual destructor
     */
    virtual ~ConcatenationCreator();

    /** Set input dataset that should be split into a number of
     *  concatenation instances
     *  @param  srcDataset The dataset to read from (must not be NULL)
     *  @param  transferOwnership If OFTrue, the ConcatenationCreator class will
     *          free memory of the srcDataset after processing.
     *  @return EC_Normal if input is considered valid (up to now), error otherwise
     */
    virtual OFCondition setCfgInput(DcmItem* srcDataset, OFBool transferOwnership);

    /** Set input dataset with separate pixel data that should be split
     *  into a number of concatenation instances.
     *  @param  srcDataset The dataset to read from (must not be NULL and not contain
     *          the Pixel Data attribute (on main level)
     *  @param  pixelData Raw buffer of source pixel data
     *  @param  pixelDataLength Length of pixelData buffer in bytes
     *  @param  transferOwnership If OFTrue, the ConcatenationCreator class will
     *          free memory of the srcDataset and pixelData after processing.
     *  @return EC_Normal if input is considered valid (up to now), error otherwise
     */
    virtual OFCondition
    setCfgInput(DcmItem* srcDataset, Uint8* pixelData, size_t pixelDataLength, OFBool transferOwnership);

    /** Set number of frames that should go into a single concatenation instance produced.
     *  The last concatenation instance might have less frames. This setting also
     *  directly determines the number of instances produced for a specific input.
     *  @param numFramesPerInstance The number of frames to be used for each instance
     *         (last instance may differ)
     *  @return EC_Normal if setting is ok, error otherwise
     */
    virtual OFCondition setCfgFramesPerInstance(Uint32 numFramesPerInstance);

    /** Set the Instance Number that should be used in the concatenation instances produced.
     *  @param  instanceNumber The Instance Number to be used
     *  @return OFTrue if Instance Number is acceptable, OFFalse otherwise
     */
    virtual OFBool setCfgInstanceNumber(const OFString& instanceNumber);

    /** Write first/next concatenation instance
     *  @param  dstDataset The dataset to write to
     *  @return EC_Normal if instance could be written,
     *  FG_EC_ConcatenationComplete error code if the last instance has already been written,
     *  other error otherwise.
     */
    virtual OFCondition writeNextInstance(DcmItem& dstDataset);

    /** Write first/next concatenation instance to given filename
     *  @param  fn The filename to write to
     *  @return EC_Normal if instance could be written,
     *  FG_EC_ConcatenationComplete error code if the last instance has already been written,
     *  other error otherwise.
     */
    virtual OFCondition writeNextInstance(const OFFilename& fn);

    /** Get number of concatenation instances that will be produced.
     *  This can only be used after setCfgInput() was successful; otherwise
     *  it will always return 0.
     *  @return The number of instances that will be produced
     */
    virtual size_t getNumInstances();

protected:
    /** Check whether SOP Class of given dataset does allow Concatenation.
     *  Right now the Standard forbids Concatenations for:
     *  <ul>
     *  <li>Ophthalmic Tomography Image Storage</li>
     *  <li>Ophthalmic Optical Coherence Tomography B-scan Volume Analysis Storage</li>
     *  </ul>
     *  @param  srcDataset The dataset to check SOP Class for
     *  @return OFTrue if Concatenation is permitted, OFFalse otherwise
     */
    virtual OFBool checkSOPClass(DcmItem& srcDataset);

    /** Check whether color model (Photometric Interpretation) and planar configuration
     *  are acceptable.
     *  Right now this class supports the following color models:
     *  <ul>
     *  <li>RGB</li>
     *  <li>YBR_FULL</li>
     *  <li>MONOCHROME1</li>
     *  <li>MONOCHROME2</li>
     *  </ul>
     *  Also, only Planar Configuration=0 is supported (or empty value implicating
     *  the same plane configuration).
     *  @param  srcDataset The dataset to check
     *  @return OFTrue if color model and planar configuration is permitted, OFFalse otherwise
     */
    virtual OFBool checkColorModel(DcmItem& srcDataset);

    /** Insert Concatenation attributes into destination dataset.
     *  @param  dstDataset The destination dataset to write to
     *  @param  numFramesCurrentInstance The number of frames that will go into
     *          the dataset
     *  @return EC_Normal if setting worked, error otherwise
     */
    virtual OFCondition setConcatenationAttributes(DcmItem& dstDataset, Uint32 numFramesCurrentInstance);

    /** Advance internal counters to next frame
     *  @return EC_Normal if there is a next frame, error otherwise
     */
    virtual OFCondition goToNextFrame();

    /** Return number of frames that must be written into current instance.
     *  Will be the same for each instance except (potentially) the last one.
     *  @return Number of frames
     */
    virtual Uint32 numFramesCurrentDstInstance();

    /** Configure class based on input/configuration settings, i.e. prepare for
     *  concatenation writing.
     *  @return EC_Normal if successful, error otherwise
     */
    virtual OFCondition configureCommon();

private:

    /// Maximum number of instances that make up a Concatenation (=2^16-1=65535),
    /// derived from attributes In-concatentation Number and In-concatenation Total Number
    /// that only store 16 bit values (VR US).
    static Uint16 const m_MAX_INSTANCES_PER_CONCATENATION;

    /// Maximum number of bytes for uncompressed pixel data (=2^32-2), derived from
    /// 32 bit length field of Pixel Data attribute with even length being required.
    static Uint32 const m_MAX_PIXEL_DATA_LENGTH;

    /// Flag denoting whether class is configured (i.e. configureCommon() has been called)
    /// and ready for concatenation writing.
    OFBool m_configured;

    /// Flag denoting whether source data (dataset and/with frame data) should be
    /// deleted by this class (OFTrue) or not (OFFalse). Value is initially OFFalse
    /// but a value must be provided in any case in setCfgInput().
    OFBool m_cfgTransferOwnership;

    /// Number of instances each concatenation instance should contain (last may differ)
    /// as configured by the user using setCfgFramesPerInstance(). Default is 25.
    Uint32 m_cfgNumFramesPerInstance;

    /// Number of bits used for each frame (same in source instance and concatenation instances).
    size_t m_numBitsFrame;

    /// Pointer to source dataset; will be modified during conversion (but content is restored
    /// before the call to this class returns).
    /// Once the ConcatenationCreator creator class goes out of scope or reset() is being called,
    /// it is set to NULL. If m_cfgTransferOwnership is OFTrue, memory is freed by this class, too.
    DcmItem* m_srcDataset;

    /// SOP Instance UID of source instance provided in setCfgInput()
    OFString m_srcSOPInstanceUID;

    /// Pointer to source pixel data; will be modified during conversion (but content is restored
    /// before the call to this class returns).
    /// Once the ConcatenationCreator creator class goes out of scope or reset() is being called,
    /// it is set to NULL. If m_cfgTransferOwnership is OFTrue, memory is freed by this class, too.
    Uint8* m_srcPixelData;

    /// VR of pixel data extracted/derived from source dataset. EVR_OB and EVR_OW are supported.
    /// Initially set to EVR_Unknown.
    DcmEVR m_VRPixelData;

    /// Pointer to Per-Frame Functional Group Sequence of source dataset. Use to copy
    /// per-frame items to concatenation instances as needed.
    DcmSequenceOfItems* m_srcPerFrameFG;

    /// Number of frames in source instance.
    Uint32 m_srcNumFrames;

    /// Number of instances to be produced for Concatenation.
    /// This is limited to 16 bit since the attributes
    /// In-concatentation Number and In-concatenation Total Number only
    /// stores 16 bit values (VR US).
    Uint16 m_dstNumInstances;

    /// Number of Frames per instance (cached) to be produced for
    /// all frames (last frame may differ)
    Uint32 m_dstNumFramesPerInstance;

    /// Number of Frames per instance (cached) to be produced for
    /// last frame
    Uint32 m_dstNumFramesLastInstance;

    /// Concatenation UID to be used in the concatenation instances. Automatically
    /// created during conversion.
    OFString m_dstConcatenationUID;

    /// Instance Number to be used in concatenation instances produced. Can be
    /// configured using setCfgInstanceNumber().
    OFString m_dstInstanceNumber;

    /// Source frame that is used next in conversion
    Uint16 m_currentSrcFrame;

    /// Per frame FG item that is used next in conversion
    DcmItem* m_currentPerFrameItem;

    /// Next instance number that is about to be produced
    Uint16 m_currentInstanceNum;
};

#endif // CONCATENATIONCREATOR_H
