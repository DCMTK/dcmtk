/*
 *
 *  Copyright (C) 2002, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRSOPInstanceReferenceList
 *             - InstanceStruct, SeriesStruct, StudyStruct
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-05-07 12:54:28 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrsoprf.h"

#include "dcdeftag.h"


// --- DSRSOPInstanceReferenceList::InstanceStruct ---

DSRSOPInstanceReferenceList::InstanceStruct::InstanceStruct(const OFString &sopClassUID,
                                                            const OFString &instanceUID)
  : SOPClassUID(sopClassUID),
    InstanceUID(instanceUID)
{
}


// --- DSRSOPInstanceReferenceList::SeriesStruct ---


DSRSOPInstanceReferenceList::SeriesStruct::SeriesStruct(const OFString &seriesUID)
  : SeriesUID(seriesUID)
{
    /* initialize list cursor */
    Iterator = InstanceList.end();
}


DSRSOPInstanceReferenceList::SeriesStruct::~SeriesStruct()
{
    Iterator = InstanceList.begin();
    const OFListIterator(InstanceStruct *) last = InstanceList.end();
    /* delete all items and free memory */
    while (Iterator != last)
    {
        delete (*Iterator);
        Iterator = InstanceList.erase(Iterator);
    }
}


OFCondition DSRSOPInstanceReferenceList::SeriesStruct::read(DcmItem &dataset,
                                                            OFConsole *logStream)
{
    /* first, read optional attributes on series level */
    getAndCheckStringValueFromDataset(dataset, DCM_RetrieveAETitle, RetrieveAETitle, "1-n", "3", logStream);
    getAndCheckStringValueFromDataset(dataset, DCM_StorageMediaFileSetID, StorageMediaFileSetID, "1", "3", logStream);
    getAndCheckStringValueFromDataset(dataset, DCM_StorageMediaFileSetUID, StorageMediaFileSetUID, "1", "3", logStream);
    /* then, check whether sequence is present and non-empty */
    DcmSequenceOfItems sequence(DCM_ReferencedSOPSequence);
    OFCondition result = getSequenceFromDataset(dataset, sequence);
    checkElementValue(sequence, "1-n", "1", logStream, result);
    if (result.good())
    {
        /* iterate over all sequence items */
        for (unsigned long i = 0; i < sequence.card(); i++)
        {
            DcmItem *item = sequence.getItem(i);
            if (item != NULL)
            {
                /* get the SOP class and SOP instance UID */
                OFString sopClassUID, instanceUID;
                if (getAndCheckStringValueFromDataset(*item, DCM_ReferencedSOPClassUID, sopClassUID, "1", "1", logStream).good() &&
                    getAndCheckStringValueFromDataset(*item, DCM_ReferencedSOPInstanceUID, instanceUID, "1", "1", logStream).good())
                {
                    /* create new instance list item */
                    InstanceStruct *instance = new InstanceStruct(sopClassUID, instanceUID);
                    if (instance != NULL)
                    {
                        InstanceList.push_back(instance);
                        /* set cursor to new position */
                        Iterator = --InstanceList.end();
                    } else {
                        result = EC_MemoryExhausted;
                        break;
                    }
                }
            }
        }
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::SeriesStruct::write(DcmItem &dataset,
                                                             OFConsole * /*logStream*/) const
{
    OFCondition result = EC_Normal;
    /* store the series level attributes */
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, SeriesUID);
    /* write optional attributes if non-empty */
    if (RetrieveAETitle.length() > 0)
        dataset.putAndInsertOFStringArray(DCM_RetrieveAETitle, RetrieveAETitle);
    if (StorageMediaFileSetID.length() > 0)
        dataset.putAndInsertOFStringArray(DCM_StorageMediaFileSetID, StorageMediaFileSetID);
    if (StorageMediaFileSetUID.length() > 0)
        dataset.putAndInsertOFStringArray(DCM_StorageMediaFileSetUID, StorageMediaFileSetUID);
    /* iterate over all list items */
    OFListIterator(InstanceStruct *) iter = InstanceList.begin();
    const OFListIterator(InstanceStruct *) last = InstanceList.end();
    while ((iter != last) && result.good())
    {
        InstanceStruct *instance = *iter;
        /* check whether list item really exists */
        if (instance != NULL)
        {
            DcmItem *item = NULL;
            /* create a new item (and a sequence if required) */
            result = dataset.findOrCreateSequenceItem(DCM_ReferencedSOPSequence, item, -2 /*append new*/);
            if (result.good())
            {
                /* store the instance level attributes */
                item->putAndInsertOFStringArray(DCM_ReferencedSOPClassUID, instance->SOPClassUID);
                item->putAndInsertOFStringArray(DCM_ReferencedSOPInstanceUID, instance->InstanceUID);
            }
        }
        iter++;
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::SeriesStruct::writeXML(ostream &stream,
                                                                const size_t flags) const
{
    OFString string;
    /* write the series level attributes */
    stream << "<series uid=\"" << convertToMarkupString(SeriesUID, string) << "\">" << endl;
    writeStringValueToXML(stream, RetrieveAETitle, "aetitle", flags & XF_writeEmptyTags);
    if ((flags & XF_writeEmptyTags) || (StorageMediaFileSetUID.length() > 0) || (StorageMediaFileSetID.length() > 0))
    {
        stream << "<fileset";
        if (StorageMediaFileSetUID.length() > 0)
            stream << " uid=\"" << convertToMarkupString(StorageMediaFileSetUID, string) << "\"";
        stream << ">" << convertToMarkupString(StorageMediaFileSetID, string) << "</fileset>" << endl;
    }
    /* iterate over all list items */
    OFListIterator(InstanceStruct *) iter = InstanceList.begin();
    const OFListIterator(InstanceStruct *) last = InstanceList.end();
    while (iter != last)
    {
        InstanceStruct *instance = *iter;
        /* check whether list item really exists */
        if (instance != NULL)
        {
            /* write instance level */
            stream << "<reference>" << endl;
            stream << "<sopclass uid=\"" << convertToMarkupString(instance->SOPClassUID, string) << "\">";
            /* retrieve name of SOP class */
            const char *sopClass = dcmFindNameOfUID(instance->SOPClassUID.c_str());
            if (sopClass != NULL)
                stream << sopClass;
            stream << "</sopclass>" << endl;
            stream << "<instance uid=\"" << convertToMarkupString(instance->InstanceUID, string) << "\"/>" << endl;
            stream << "</reference>" << endl;
        }
        iter++;
    }
    stream << "</series>" << endl;
    return EC_Normal;
}


DSRSOPInstanceReferenceList::InstanceStruct *DSRSOPInstanceReferenceList::SeriesStruct::gotoInstance(const OFString &instanceUID)
{
    InstanceStruct *instance = NULL;
    /* first, check whether the current instance is the one we're searching for */
    if ((Iterator != InstanceList.end()) && (*Iterator != NULL) && ((*Iterator)->InstanceUID == instanceUID))
        instance = *Iterator;
    else
    {
        /* start with the first list item */
        Iterator = InstanceList.begin();
        const OFListIterator(InstanceStruct *) last = InstanceList.end();
        /* search for given SOP instance UID */
        while ((Iterator != last) && ((*Iterator == NULL) || ((*Iterator)->InstanceUID != instanceUID)))
            Iterator++;
        /* item found */
        if (Iterator != last)
            instance = *Iterator;
    }
    return instance;
}


OFCondition DSRSOPInstanceReferenceList::SeriesStruct::addItem(const OFString &sopClassUID,
                                                               const OFString &instanceUID)
{
    OFCondition result = EC_Normal;
    /* check whether series already exists */
    InstanceStruct *instance = gotoInstance(instanceUID);
    if (instance == NULL)
    {
        /* if not, create new instance item and add it to the list */
        instance = new InstanceStruct(sopClassUID, instanceUID);
        if (instance != NULL)
        {
            InstanceList.push_back(instance);
            /* set cursor to new position */
            Iterator = --InstanceList.end();
        } else
            result = EC_MemoryExhausted;
    } else {
        /* check whether given SOP class is the same as stored */
        if (instance->SOPClassUID != sopClassUID)
            result = SR_EC_DifferentSOPClassesForAnInstance;
    }
    return result;
}


// --- DSRSOPInstanceReferenceList::StudyStruct ---


DSRSOPInstanceReferenceList::StudyStruct::StudyStruct(const OFString &studyUID)
  : StudyUID(studyUID)
{
    /* initialize list cursor */
    Iterator = SeriesList.end();
}


DSRSOPInstanceReferenceList::StudyStruct::~StudyStruct()
{
    Iterator = SeriesList.begin();
    const OFListIterator(SeriesStruct *) last = SeriesList.end();
    /* delete all items and free memory */
    while (Iterator != last)
    {
        delete (*Iterator);
        Iterator = SeriesList.erase(Iterator);
    }
}

OFCondition DSRSOPInstanceReferenceList::StudyStruct::read(DcmItem &dataset,
                                                           OFConsole *logStream)
{
    /* first, check whether sequence is present and non-empty */
    DcmSequenceOfItems sequence(DCM_ReferencedSeriesSequence);
    OFCondition result = getSequenceFromDataset(dataset, sequence);
    checkElementValue(sequence, "1-n", "1", logStream, result);
    if (result.good())
    {
        /* iterate over all sequence items */
        for (unsigned long i = 0; i < sequence.card(); i++)
        {
            DcmItem *item = sequence.getItem(i);
            if (item != NULL)
            {
                /* get the series instance UID */
                OFString seriesUID;
                if (getAndCheckStringValueFromDataset(*item, DCM_SeriesInstanceUID, seriesUID, "1", "1", logStream).good())
                {
                    /* create new series list item */
                    SeriesStruct *series = new SeriesStruct(seriesUID);
                    if (series != NULL)
                    {
                        SeriesList.push_back(series);
                        /* set cursor to new position */
                        Iterator = --SeriesList.end();
                        /* read further attributes on series level and the instance level */
                        result = series->read(*item, logStream);
                    } else {
                        result = EC_MemoryExhausted;
                        break;
                    }
                }
            }
        }
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::StudyStruct::write(DcmItem &dataset,
                                                            OFConsole *logStream) const
{
    OFCondition result = EC_Normal;
    /* store the study level attributes */
    dataset.putAndInsertOFStringArray(DCM_StudyInstanceUID, StudyUID);
    /* iterate over all list items */
    OFListIterator(SeriesStruct *) iter = SeriesList.begin();
    const OFListIterator(SeriesStruct *) last = SeriesList.end();
    while ((iter != last) && result.good())
    {
        SeriesStruct *series = (SeriesStruct *)(*iter);
        /* check whether list item really exists */
        if (series != NULL)
        {
            DcmItem *item = NULL;
            /* create a new item (and a sequence if required) */
            result = dataset.findOrCreateSequenceItem(DCM_ReferencedSeriesSequence, item, -2 /*append new*/);
            /* write series and instance level */
            if (result.good())
                result = series->write(*item, logStream);
        }
        iter++;
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::StudyStruct::writeXML(ostream &stream,
                                                               const size_t flags) const
{
    OFString string;
    OFCondition result = EC_Normal;
    /* write the study level attributes */
    stream << "<study uid=\"" << convertToMarkupString(StudyUID, string) << "\">" << endl;
    /* iterate over all list items */
    OFListIterator(SeriesStruct *) iter = SeriesList.begin();
    const OFListIterator(SeriesStruct *) last = SeriesList.end();
    while ((iter != last) && result.good())
    {
        SeriesStruct *series = (SeriesStruct *)(*iter);
        /* check whether list item really exists */
        if (series != NULL)
        {
            /* write series and instance level */
            result = series->writeXML(stream, flags);
        }
        iter++;
    }
    stream << "</study>" << endl;
    return result;
}


DSRSOPInstanceReferenceList::SeriesStruct *DSRSOPInstanceReferenceList::StudyStruct::gotoSeries(const OFString &seriesUID)
{
    SeriesStruct *series = NULL;
    /* first, check whether the current series is the one we're searching for */
    if ((Iterator != SeriesList.end()) && (*Iterator != NULL) && (((SeriesStruct *)(*Iterator))->SeriesUID == seriesUID))
        series = (SeriesStruct *)(*Iterator);
    else
    {
        /* start with the first list item */
        Iterator = SeriesList.begin();
        const OFListIterator(SeriesStruct *) last = SeriesList.end();
        /* search for given series UID */
        while ((Iterator != last) && ((*Iterator == NULL) || (((SeriesStruct *)(*Iterator))->SeriesUID != seriesUID)))
            Iterator++;
        /* item found */
        if (Iterator != last)
            series = (SeriesStruct *)(*Iterator);
    }
    return series;
}


DSRSOPInstanceReferenceList::InstanceStruct *DSRSOPInstanceReferenceList::StudyStruct::gotoInstance(const OFString &instanceUID)
{
    InstanceStruct *instance = NULL;
    /* start with the first list item */
    Iterator = SeriesList.begin();
    const OFListIterator(SeriesStruct *) last = SeriesList.end();
    /* search for given series UID */
    while ((Iterator != last) && (instance == NULL))
    {
        SeriesStruct *series = (SeriesStruct *)(*Iterator);
        /* continue search on instance level */
        if (series != NULL)
            instance = series->gotoInstance(instanceUID);
        /* if found exit loop */
        if (instance == NULL)
            Iterator++;
    }
    return instance;
}


OFCondition DSRSOPInstanceReferenceList::StudyStruct::addItem(const OFString &seriesUID,
                                                              const OFString &sopClassUID,
                                                              const OFString &instanceUID)
{
    OFCondition result = EC_Normal;
    /* check whether series already exists */
    SeriesStruct *series = gotoSeries(seriesUID);
    if (series == NULL)
    {
        /* if not create new series item and add it to the list */
        series = new SeriesStruct(seriesUID);
        if (series != NULL)
        {
            SeriesList.push_back(series);
            /* set cursor to new position */
            Iterator = --SeriesList.end();
        } else
            result = EC_MemoryExhausted;
    }
    /* do the same for the instance level */
    if (series != NULL)
        result = series->addItem(sopClassUID, instanceUID);
    return result;
}


// --- DSRSOPInstanceReferenceList ---

DSRSOPInstanceReferenceList::DSRSOPInstanceReferenceList(const DcmTagKey &tagKey)
  : SequenceTag(tagKey),
    StudyList(),
    Iterator()
{
    /* initialize list cursor */
    Iterator = StudyList.end();
}


DSRSOPInstanceReferenceList::~DSRSOPInstanceReferenceList()
{
    /* clear reference list and delete allocated memory */
    clear();
}


void DSRSOPInstanceReferenceList::clear()
{
    Iterator = StudyList.begin();
    const OFListIterator(StudyStruct *) last = StudyList.end();
    /* delete all items and free memory */
    while (Iterator != last)
    {
        delete (*Iterator);
        Iterator = StudyList.erase(Iterator);
    }
    /* make sure that the list is empty */
    StudyList.clear();
    Iterator = StudyList.end();
}


OFBool DSRSOPInstanceReferenceList::empty() const
{
    return StudyList.empty();
}


OFCondition DSRSOPInstanceReferenceList::read(DcmItem &dataset,
                                              OFConsole *logStream)
{
    /* first, check whether sequence is present and non-empty */
    DcmSequenceOfItems sequence(SequenceTag);
    OFCondition result = getSequenceFromDataset(dataset, sequence);
    checkElementValue(sequence, "1-n", "1C", logStream, result);
    if (result.good())
    {
        /* iterate over all sequence items */
        for (unsigned long i = 0; i < sequence.card(); i++)
        {
            DcmItem *item = sequence.getItem(i);
            if (item != NULL)
            {
                /* get the study instance UID */
                OFString studyUID;
                if (getAndCheckStringValueFromDataset(*item, DCM_StudyInstanceUID, studyUID, "1", "1", logStream).good())
                {
                    /* create new study list item */
                    StudyStruct *study = new StudyStruct(studyUID);
                    if (study != NULL)
                    {
                        StudyList.push_back(study);
                        /* set cursor to new position */
                        Iterator = --StudyList.end();
                        /* read attributes on series and instance level */
                        result = study->read(*item, logStream);
                    } else {
                        result = EC_MemoryExhausted;
                        break;
                    }
                }
            }
        }
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::write(DcmItem &dataset,
                                               OFConsole *logStream) const
{
    OFCondition result = EC_Normal;
    /* iterate over all list items */
    OFListIterator(StudyStruct *) iter = StudyList.begin();
    const OFListIterator(StudyStruct *) last = StudyList.end();
    while ((iter != last) && result.good())
    {
        StudyStruct *study = (StudyStruct *)(*iter);
        /* check whether list item really exists */
        if (study != NULL)
        {
            DcmItem *item = NULL;
            /* create a new item (and a sequence if required) */
            result = dataset.findOrCreateSequenceItem(SequenceTag, item, -2 /*append new*/);
            /* write study, series and instance level */
            if (result.good())
                result = study->write(*item, logStream);
        }
        iter++;
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::writeXML(ostream &stream,
                                                  const size_t flags) const
{
    OFCondition result = EC_Normal;
    /* iterate over all list items */
    OFListIterator(StudyStruct *) iter = StudyList.begin();
    const OFListIterator(StudyStruct *) last = StudyList.end();
    while ((iter != last) && result.good())
    {
        StudyStruct *study = (StudyStruct *)(*iter);
        /* check whether list item really exists */
        if (study != NULL)
        {
            /* write study, series and instance level */
            result = study->writeXML(stream, flags);
        }
        iter++;
    }
    return result;
}


DSRSOPInstanceReferenceList::StudyStruct *DSRSOPInstanceReferenceList::gotoStudy(const OFString &studyUID)
{
    StudyStruct *study = NULL;
    /* first, check whether the current study is the one we're searching for */
    if ((Iterator != StudyList.end()) && (*Iterator != NULL) && (((StudyStruct *)(*Iterator))->StudyUID == studyUID))
        study = (StudyStruct *)(*Iterator);
    else
    {
        /* start with the first list item */
        Iterator = StudyList.begin();
        const OFListIterator(StudyStruct *) last = StudyList.end();
        /* search for given study UID */
        while ((Iterator != last) && ((*Iterator == NULL) || (((StudyStruct *)(*Iterator))->StudyUID != studyUID)))
            Iterator++;
        /* item found */
        if (Iterator != last)
            study = (StudyStruct *)(*Iterator);
    }
    return study;
}


OFCondition DSRSOPInstanceReferenceList::addItem(const OFString &studyUID,
                                                 const OFString &seriesUID,
                                                 const OFString &sopClassUID,
                                                 const OFString &instanceUID)
{
    OFCondition result = EC_IllegalParameter;
    /* check parameters first */
    if ((studyUID.length() > 0) && (seriesUID.length() > 0) && (sopClassUID.length() > 0) && (instanceUID.length() > 0))
    {
        result = EC_Normal;
        StudyStruct *study = gotoStudy(studyUID);
        /* check whether study already exists */
        if (study == NULL)
        {
            /* if not create new study item and add it to the list */
            study = new StudyStruct(studyUID);
            if (study != NULL)
            {
                StudyList.push_back(study);
                /* set cursor to new position */
                Iterator = --StudyList.end();
            } else
                result = EC_MemoryExhausted;
        }
        /* do the same for the series and instance level */
        if (study != NULL)
            result = study->addItem(seriesUID, sopClassUID, instanceUID);
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::gotoItem(const OFString &sopClassUID,
                                                  const OFString &instanceUID)
{
    OFCondition result = EC_IllegalParameter;
    /* check parameters first */
    if ((sopClassUID.length() > 0) && (instanceUID.length() > 0))
    {
        OFBool sopClassMatch = OFFalse;
        result = SR_EC_SOPInstanceNotFound;
        /* start with first study */
        Iterator = StudyList.begin();
        const OFListIterator(StudyStruct *) last = StudyList.end();
        /* iterate over all studies */
        while ((Iterator != last) && result.bad())
        {
            StudyStruct *study = (StudyStruct *)(*Iterator);
            /* continue search on series level */
            if (study != NULL)
            {
                InstanceStruct *instance = study->gotoInstance(instanceUID);
                /* if instance found, exit loop */
                if (instance != NULL)
                {
                    /* finally, check whether SOP class matches */
                    sopClassMatch = (instance->SOPClassUID == sopClassUID);
                    result = EC_Normal;
                } else
                    Iterator ++;
            } else
                Iterator++;
        }
        /* report an error in case of SOP class mismatch */
        if (result.good() && !sopClassMatch)
            result = SR_EC_DifferentSOPClassesForAnInstance;
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::gotoItem(const OFString &studyUID,
                                                  const OFString &seriesUID,
                                                  const OFString &instanceUID)
{
    OFCondition result = EC_IllegalParameter;
    /* check parameters first */
    if ((studyUID.length() > 0) && (seriesUID.length() > 0) && (instanceUID.length() > 0))
    {
        result = SR_EC_SOPInstanceNotFound;
        /* search for given study */
        StudyStruct *study = gotoStudy(studyUID);
        if (study != NULL)
        {
            /* do the same for the series ... */
            SeriesStruct *series = study->gotoSeries(seriesUID);
            if (series != NULL)
            {
                /* ... and instance level */
                if (series->gotoInstance(instanceUID) != NULL)
                    result = EC_Normal;
            }
        }
    }
    return result;
}


DSRSOPInstanceReferenceList::StudyStruct *DSRSOPInstanceReferenceList::getCurrentStudy() const
{
    StudyStruct *study = NULL;
    /* check whether current study is valid */
    if (Iterator != StudyList.end())
        study = (StudyStruct *)(*Iterator);
    return study;
}


DSRSOPInstanceReferenceList::SeriesStruct *DSRSOPInstanceReferenceList::getCurrentSeries() const
{
    SeriesStruct *series = NULL;
    StudyStruct *study = getCurrentStudy();
    /* check whether current series is valid */
    if ((study != NULL) && (study->Iterator != study->SeriesList.end()))
        series = (SeriesStruct *)(*(study->Iterator));
    return series;
}


DSRSOPInstanceReferenceList::InstanceStruct *DSRSOPInstanceReferenceList::getCurrentInstance() const
{
    InstanceStruct *instance = NULL;
    SeriesStruct *series = getCurrentSeries();
    /* check whether current instance is valid */
    if ((series != NULL) && (series->Iterator != series->InstanceList.end()))
        instance = *(series->Iterator);
    return instance;
}


const OFString &DSRSOPInstanceReferenceList::getStudyInstanceUID(OFString &string) const
{
    /* check whether current study is valid */
    StudyStruct *study = getCurrentStudy();
    if (study != NULL)
        string = study->StudyUID;
    else
        string.clear();
    return string;
}


const OFString &DSRSOPInstanceReferenceList::getSeriesInstanceUID(OFString &string) const
{
    /* check whether current series is valid */
    SeriesStruct *series = getCurrentSeries();
    if (series != NULL)
        string = series->SeriesUID;
    else
        string.clear();
    return string;
}


const OFString &DSRSOPInstanceReferenceList::getSOPInstanceUID(OFString &string) const
{
    /* check whether current instance is valid */
    InstanceStruct *instance = getCurrentInstance();
    if (instance != NULL)
        string = instance->InstanceUID;
    else
        string.clear();
    return string;
}


const OFString &DSRSOPInstanceReferenceList::getSOPClassUID(OFString &string) const
{
    /* check whether current instance is valid */
    InstanceStruct *instance = getCurrentInstance();
    if (instance != NULL)
        string = instance->SOPClassUID;
    else
        string.clear();
    return string;
}


const OFString &DSRSOPInstanceReferenceList::getRetrieveAETitle(OFString &string) const
{
    /* check whether current series is valid */
    SeriesStruct *series = getCurrentSeries();
    if (series != NULL)
        string = series->RetrieveAETitle;
    else
        string.clear();
    return string;
}


const OFString &DSRSOPInstanceReferenceList::getStorageMediaFileSetID(OFString &string) const
{
    /* check whether current series is valid */
    SeriesStruct *series = getCurrentSeries();
    if (series != NULL)
        string = series->StorageMediaFileSetID;
    else
        string.clear();
    return string;
}


const OFString &DSRSOPInstanceReferenceList::getStorageMediaFileSetUID(OFString &string) const
{
    /* check whether current series is valid */
    SeriesStruct *series = getCurrentSeries();
    if (series != NULL)
        string = series->StorageMediaFileSetUID;
    else
        string.clear();
    return string;
}


OFCondition DSRSOPInstanceReferenceList::setRetrieveAETitle(const OFString &value)
{
    OFCondition result = EC_IllegalCall;
    /* check whether current series is valid */
    SeriesStruct *series = getCurrentSeries();
    if (series != NULL)
    {
        /* set the value */
        series->RetrieveAETitle = value;
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::setStorageMediaFileSetID(const OFString &value)
{
    OFCondition result = EC_IllegalCall;
    /* check whether current series is valid */
    SeriesStruct *series = getCurrentSeries();
    if (series != NULL)
    {
        /* set the value */
        series->StorageMediaFileSetID = value;
        result = EC_Normal;
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::setStorageMediaFileSetUID(const OFString &value)
{
    OFCondition result = EC_IllegalCall;
    /* check whether current series is valid */
    SeriesStruct *series = getCurrentSeries();
    if (series != NULL)
    {
        /* set the value */
        series->StorageMediaFileSetUID = value;
        result = EC_Normal;
    }
    return result;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrsoprf.cc,v $
 *  Revision 1.1  2002-05-07 12:54:28  joergr
 *  Added support for the Current Requested Procedure Evidence Sequence and the
 *  Pertinent Other Evidence Sequence to the dcmsr module.
 *
 *
 *
 */
