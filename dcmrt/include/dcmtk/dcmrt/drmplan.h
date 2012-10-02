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
 *  Purpose: Mid-Level API for RT Plan objects
 *
 */

#ifndef DRMPLAN_H
#define DRMPLAN_H

#include "dcmtk/config/osconfig.h"     /* make sure OS specific configuration is included first */
#include "dcmtk/dcmrt/drtplan.h"


/**
 * helper class for working with radio therapy plan objects. This class
 * provides several helper functions on top of DRTPlanIOD.
 *
 * @note These functions hopefully simplify working with DRTPlanIOD. This is a
 * work in progress, please propose any improvements which would make working
 * with this class easier for you and that your consider useful.
 */
class DCMTK_DCMRT_EXPORT DRTPlan : public DRTPlanIOD {
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

    /** find a fraction group by its fraction group number.
     *  @param fractionGroupNumber the fraction group number to find.
     *  @return the item representing the fraction group or the EmptyDefaultItem
     *          of the DRTFractionGroupSequence.
     */
    DRTFractionGroupSequence::Item& getFractionGroup(Sint32 fractionGroupNumber);

    /** find a beam by its beam number.
     *  @param beamNumber the beam number to find.
     *  @return the item representing the beam or the EmptyDefaultItem of the
     *          DRTBeamSequence.
     */
    DRTBeamSequence::Item& getBeam(Sint32 beamNumber);

    /** find a dose reference by its dose reference number.
     *  @param referenceNumber the beam number to find.
     *  @return the item representing the dose reference or the EmptyDefaultItem
     *          of the DRTDoseReferenceSequence.
     */
    DRTDoseReferenceSequence::Item& getDoseReference(Sint32 referenceNumber);

    /** find a tolerance table by its tolerance table number.
     *  @param tableNumber the tolerance table number to find.
     *  @return the item representing the tolerance table or the
     *          EmptyDefaultItem of the DRTToleranceTableSequence.
     */
    DRTToleranceTableSequence::Item& getToleranceTable(Sint32 tableNumber);

    /** find a patient setup by its patient setup number.
     *  @param setupNumber the setup number to find.
     *  @return the item representing the setup or the
     *          EmptyDefaultItem of the DRTPatientSetupSequence.
     */
    DRTPatientSetupSequence::Item& getPatientSetup(Sint32 setupNumber);

    /** find a application setup by its application setup number.
     *  @param setupNumber the setup number to find.
     *  @return the item representing the setup or the
     *          EmptyDefaultItem of the DRTApplicationSetupSequence.
     */
    DRTApplicationSetupSequence::Item& getApplicationSetup(Sint32 setupNumber);
};

#endif
