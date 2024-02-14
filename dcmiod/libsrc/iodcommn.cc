/*
 *
 *  Copyright (C) 2015-2024, Open Connections GmbH
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
 *  Module: dcmiod
 *
 *  Author: Michael Onken
 *
 *  Purpose: Class representing IODs by exposing common DICOM module attributes
 *
 */

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/iodcommn.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dctypes.h" // logger

DcmIODCommon::DcmIODCommon()
    : m_Item(new DcmItem())
    , m_Rules(new IODRules)
    , m_Patient(m_Item, m_Rules)
    , m_PatientStudy(m_Item, m_Rules)
    , m_Study(m_Item, m_Rules)
    , m_Equipment(m_Item, m_Rules)
    , m_Series(m_Item, m_Rules)
    , m_FrameOfReference(m_Item, m_Rules)
    , m_SOPCommon(m_Item, m_Rules)
    , m_CommonInstanceReferenceModule(m_Item, m_Rules)
    , m_Modules()
    , m_CheckValueOnWrite(OFTrue)
{
    // Set initial values for a new SOP instance
    ensureInstanceUIDs(OFFalse);
    // push this first so Specific Character Set will be written in the beginning
    m_Modules.push_back(&m_SOPCommon);
    m_Modules.push_back(&m_Patient);
    m_Modules.push_back(&m_PatientStudy);
    m_Modules.push_back(&m_Study);
    m_Modules.push_back(&m_Equipment);
    m_Modules.push_back(&m_Series);
    m_Modules.push_back(&m_FrameOfReference);
    m_Modules.push_back(&m_CommonInstanceReferenceModule);
}

DcmIODCommon::DcmIODCommon(const DcmIODCommon& rhs)
    : m_Item(rhs.m_Item)
    , m_Rules(rhs.m_Rules)
    , m_Patient(m_Item, m_Rules)
    , m_PatientStudy(m_Item, m_Rules)
    , m_Study(m_Item, m_Rules)
    , m_Equipment(m_Item, m_Rules)
    , m_Series(m_Item, m_Rules)
    , m_FrameOfReference(m_Item, m_Rules)
    , m_SOPCommon(m_Item, m_Rules)
    , m_CommonInstanceReferenceModule(m_Item, m_Rules)
    , m_Modules()
{
    // Set initial values for a new SOP instance
    ensureInstanceUIDs(OFFalse);
    // push this first so Specific Character Set will be written in the beginning
    m_Modules.push_back(&m_SOPCommon);
    m_Modules.push_back(&m_Patient);
    m_Modules.push_back(&m_PatientStudy);
    m_Modules.push_back(&m_Study);
    m_Modules.push_back(&m_Equipment);
    m_Modules.push_back(&m_Series);
    m_Modules.push_back(&m_FrameOfReference);
    m_Modules.push_back(&m_CommonInstanceReferenceModule);
}

DcmIODCommon::~DcmIODCommon()
{
}

void DcmIODCommon::clearData()
{
    // Clear data in all modules
    OFVector<IODModule*>::iterator it = m_Modules.begin();
    while (it != m_Modules.end())
    {
        (*it)->clearData();
        it++;
    }
}

IODPatientModule& DcmIODCommon::getPatient()
{
    return m_Patient;
}

IODPatientStudyModule& DcmIODCommon::getPatientStudy()
{
    return m_PatientStudy;
}

IODGeneralStudyModule& DcmIODCommon::getStudy()
{
    return m_Study;
}

IODGeneralEquipmentModule& DcmIODCommon::getEquipment()
{
    return m_Equipment;
}

IODGeneralSeriesModule& DcmIODCommon::getSeries()
{
    return m_Series;
}

IODFoRModule& DcmIODCommon::getFrameOfReference()
{
    return m_FrameOfReference;
}

IODSOPCommonModule& DcmIODCommon::getSOPCommon()
{
    return m_SOPCommon;
}

IODCommonInstanceReferenceModule& DcmIODCommon::getCommonInstanceReference()
{
    return m_CommonInstanceReferenceModule;
}

OFshared_ptr<IODRules> DcmIODCommon::getRules()
{
    return m_Rules;
}

OFshared_ptr<DcmItem> DcmIODCommon::getData()
{
    return m_Item;
}

OFCondition DcmIODCommon::read(DcmItem& dataset)
{
    /* re-initialize object */
    DcmIODCommon::clearData();

    OFVector<IODModule*>::iterator it = m_Modules.begin();
    while (it != m_Modules.end())
    {
        (*it)->read(dataset, OFTrue /* clear old data */);
        it++;
    }

    // we do not report errors here (only logger output)
    return EC_Normal;
}

OFCondition DcmIODCommon::importHierarchy(DcmItem& dataset,
                                          const OFBool readPatient,
                                          const OFBool readStudy,
                                          const OFBool readFoR,
                                          const OFBool readSeries,
                                          const OFBool takeOverCharset)
{
    if (readPatient)
    {
        m_Patient.read(dataset, OFFalse /* do not clear old data */);
    }

    if (readStudy)
    {
        m_Study.read(dataset, OFFalse /* do not clear old data */);
        m_Equipment.read(dataset, OFFalse /* do not clear old data */);
        m_PatientStudy.read(dataset, OFFalse /* do not clear old data */);
    }

    if (readSeries)
    {
        m_Series.read(dataset, OFFalse /* do not clear old data */);
        m_FrameOfReference.read(dataset, OFFalse /* do not clear old data */);
    }

    if (readFoR)
    {
        m_FrameOfReference.read(dataset, OFFalse /* do not clear old data */);
    }

    // Take over character set from the dataset imported, if desired
    if (takeOverCharset)
    {
        OFString charset;
        dataset.findAndGetOFStringArray(DCM_SpecificCharacterSet, charset);
        if (!charset.empty())
        {
            DCMIOD_DEBUG("Taking over Specific Character Set " << charset << " on import");
            OFCondition result = m_SOPCommon.setSpecificCharacterSet(charset);
            if (result.bad())
            {
                DCMIOD_ERROR("Could not set Specific Character Set " << charset << " on import: " << result.text());
            }
        }
        else
        {
            DCMIOD_DEBUG("Taking over Default Specific Character Set (ASCII) on import");
            m_SOPCommon.getData().findAndDeleteElement(DCM_SpecificCharacterSet);
        }
    }

    return EC_Normal;
}

OFCondition DcmIODCommon::importHierarchy(const OFString& filename,
                                          const OFBool readPatient,
                                          const OFBool readStudy,
                                          const OFBool readFoR,
                                          const OFBool readSeries,
                                          const OFBool takeOverCharset)
{
    DcmFileFormat dcmff;
    OFCondition result = dcmff.loadFile(filename.c_str());
    if (result.good())
    {
        DcmDataset* dset = dcmff.getDataset();
        if (dset != NULL)
        {
            result = importHierarchy(*dset, readPatient, readStudy, readFoR, readSeries, takeOverCharset);
        }
        else
        {
            DCMIOD_ERROR("Unable to get dataset from file for copying patient, study, series and/or frame of reference "
                         "information");
            result = EC_IllegalCall;
        }
    }
    return result;
}

void DcmIODCommon::ensureInstanceUIDs(const OFBool correctInvalid)
{
    m_Study.ensureInstanceUID(correctInvalid);
    m_Series.ensureInstanceUID(correctInvalid);
    m_SOPCommon.ensureInstanceUID(correctInvalid);
}

OFCondition DcmIODCommon::write(DcmItem& dataset)
{
    OFCondition result;

    OFVector<IODModule*>::iterator it = m_Modules.begin();
    while ((it != m_Modules.end() && result.good()))
    {
        result = (*it)->write(dataset);
        it++;
    }
    return result;
}

void DcmIODCommon::setValueCheckOnWrite(const OFBool check)
{
    m_CheckValueOnWrite = check;
    // set value checking policy for all modules
    OFVector<IODModule*>::iterator it = m_Modules.begin();
    while (it != m_Modules.end())
    {
        (*it)->setValueCheckOnWrite(check);
        it++;
    }
}

OFBool DcmIODCommon::getValueCheckOnWrite() const
{
    return m_CheckValueOnWrite;
}

void DcmIODCommon::createNewStudy(const OFBool clearEquipment)
{
    // clear all study-related attributes
    m_Study.clearData();
    m_PatientStudy.clearData();
    if (clearEquipment)
        m_Equipment.clearData();
    // make sure we have a valid Study Instance UID
    m_Study.ensureInstanceUID();

    // reset series- and instance related attributes
    createNewSeries();
}

void DcmIODCommon::createNewSeries(const OFBool clearFoR)
{
    // clear all series-related attributes
    m_Series.clearData();
    // create new Series Instance UID
    m_Series.ensureInstanceUID();

    // clear frame of reference-related attributes if desired
    if (clearFoR)
        m_FrameOfReference.clearData();

    /* also creates new series (since UID is empty) */
    createNewSOPInstance();
}

void DcmIODCommon::createNewSOPInstance()
{
    m_SOPCommon.clearData();
    m_SOPCommon.ensureInstanceUID();
}
