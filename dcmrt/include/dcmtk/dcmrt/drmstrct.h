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
 *  Purpose: Mid-Level API for RT Structure Set objects
 *
 */

#ifndef DRMSTRCT_H
#define DRMSTRCT_H

#include "dcmtk/config/osconfig.h"     /* make sure OS specific configuration is included first */
#include "dcmtk/dcmrt/drtstrct.h"


/**
 * helper class for working with radio therapy structure set objects. This class
 * provides several helper functions on top of DRTStructureSet.
 *
 * @note These functions hopefully simplify working with DRTStructureSetIOD.
 * This is a work in progress, please propose any improvements which would make
 * working with this class easier for you and that your consider useful.
 */
class DCMTK_DCMRT_EXPORT DRTStructureSet : public DRTStructureSetIOD
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

    /** find a frame of reference by its fraction UID.
     *  @param uid the UID to find.
     *  @return the item representing the frame of reference or the
     *          EmptyDefaultItem of the DRTReferencedFrameOfReferenceSequence.
     */
    DRTReferencedFrameOfReferenceSequence::Item& getFrameOfReference(const OFString& uid);

    /** find a region of interest by its roi number.
     *  @param roiNumber the ROI to find.
     *  @return the item representing the region of interest or the
     *          EmptyDefaultItem of the DRTStructureSetROISequence.
     */
    DRTStructureSetROISequence::Item& getROI(Sint32 roiNumber);

    /** find an observation by its observation number.
     *  @param observationNumber the observation number to find.
     *  @return the item representing the observation or the
     *          EmptyDefaultItem of the DRTRTROIObservationsSequence.
     */
    DRTRTROIObservationsSequence::Item& getObservation(Sint32 observationNumber);

    /** find an observation belonging to a given ROI number.
     *  @param roiNumber the ROI number to find.
     *  @return the item representing the observation or the
     *          EmptyDefaultItem of the DRTRTROIObservationsSequence.
     */
    DRTRTROIObservationsSequence::Item& getObservationByROINumber(Sint32 roiNumber);

    /** get all contours for a region of interest.
     *  @param result vector for storing the contours
     *  @param roiNumber ROI number to look for
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getContoursForROINumber(OFVector<DRTROIContourSequence::Item*> result, Sint32 roiNumber);

    /** get all contours for a region of interest.
     *  @param result list for storing the contours
     *  @param roiNumber ROI number to look for
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getContoursForROINumber(OFList<DRTROIContourSequence::Item*> result, Sint32 roiNumber);
};

#endif
