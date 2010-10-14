/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRSOPInstanceReferenceList
 *             - InstanceStruct, SeriesStruct, StudyStruct
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:42 $
 *  CVS/RCS Revision: $Revision: 1.19 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrsoprf.h"
#include "dcmtk/dcmsr/dsrxmld.h"

#include "dcmtk/dcmdata/dcdeftag.h"


// --- DSRSOPInstanceReferenceList::InstanceStruct ---

DSRSOPInstanceReferenceList::InstanceStruct::InstanceStruct(const OFString &sopClassUID,
                                                            const OFString &instanceUID)
  : SOPClassUID(sopClassUID),
    InstanceUID(instanceUID)
{
}


// --- DSRSOPInstanceReferenceList::SeriesStruct ---


DSRSOPInstanceReferenceList::SeriesStruct::SeriesStruct(const OFString &seriesUID)
  : SeriesUID(seriesUID),
    RetrieveAETitle(),
    StorageMediaFileSetID(),
    StorageMediaFileSetUID(),
    InstanceList(),
    Iterator()
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


size_t DSRSOPInstanceReferenceList::SeriesStruct::getNumberOfInstances() const
{
    return InstanceList.size();
}


OFCondition DSRSOPInstanceReferenceList::SeriesStruct::read(DcmItem &dataset)
{
    /* first, read optional attributes on series level */
    getAndCheckStringValueFromDataset(dataset, DCM_RetrieveAETitle, RetrieveAETitle, "1-n", "3", "ReferencedSeriesSequence");
    getAndCheckStringValueFromDataset(dataset, DCM_StorageMediaFileSetID, StorageMediaFileSetID, "1", "3", "ReferencedSeriesSequence");
    getAndCheckStringValueFromDataset(dataset, DCM_StorageMediaFileSetUID, StorageMediaFileSetUID, "1", "3", "ReferencedSeriesSequence");
    /* then, check whether sequence is present and non-empty */
    DcmSequenceOfItems sequence(DCM_ReferencedSOPSequence);
    OFCondition result = getElementFromDataset(dataset, sequence);
    checkElementValue(sequence, "1-n", "1", result);
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
                if (getAndCheckStringValueFromDataset(*item, DCM_ReferencedSOPClassUID, sopClassUID, "1", "1", "ReferencedSOPSequence").good() &&
                    getAndCheckStringValueFromDataset(*item, DCM_ReferencedSOPInstanceUID, instanceUID, "1", "1", "ReferencedSOPSequence").good())
                {
                    /* check whether instance item already exists */
                    InstanceStruct *instance = gotoInstance(instanceUID);
                    if (instance == NULL)
                    {
                        /* if not, create new instance list item */
                        instance = new InstanceStruct(sopClassUID, instanceUID);
                        if (instance != NULL)
                        {
                            /* add add it to the list of instances */
                            InstanceList.push_back(instance);
                            /* set cursor to new position */
                            Iterator = --InstanceList.end();
                        } else {
                            result = EC_MemoryExhausted;
                            break;
                        }
                    } else {
                        /* report a warning message and ignore this entry */
                        DCMSR_WARN("SOP Instance \"" << instanceUID << "\" already exists in reference list ... ignoring");
                    }
                }
            }
        }
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::SeriesStruct::write(DcmItem &dataset) const
{
    OFCondition result = EC_Normal;
    /* store the series level attributes */
    dataset.putAndInsertOFStringArray(DCM_SeriesInstanceUID, SeriesUID);
    /* write optional attributes if non-empty */
    if (!RetrieveAETitle.empty())
        dataset.putAndInsertOFStringArray(DCM_RetrieveAETitle, RetrieveAETitle);
    if (!StorageMediaFileSetID.empty())
        dataset.putAndInsertOFStringArray(DCM_StorageMediaFileSetID, StorageMediaFileSetID);
    if (!StorageMediaFileSetUID.empty())
        dataset.putAndInsertOFStringArray(DCM_StorageMediaFileSetUID, StorageMediaFileSetUID);
    /* iterate over all list items */
    OFListConstIterator(InstanceStruct *) iter = InstanceList.begin();
    const OFListConstIterator(InstanceStruct *) last = InstanceList.end();
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


OFCondition DSRSOPInstanceReferenceList::SeriesStruct::readXML(const DSRXMLDocument &doc,
                                                               DSRXMLCursor cursor)
{
    OFCondition result = SR_EC_InvalidDocument;
    if (cursor.valid())
    {
        /* first, read optional elements on series level */
        doc.getStringFromNodeContent(doc.getNamedNode(cursor, "aetitle", OFFalse /*required*/), RetrieveAETitle);
        const DSRXMLCursor childCursor = doc.getNamedNode(cursor, "fileset", OFFalse /*required*/);
        if (childCursor.valid())
        {
            doc.getStringFromAttribute(childCursor, StorageMediaFileSetUID, "uid", OFFalse /*encoding*/, OFFalse /*required*/);
            doc.getStringFromNodeContent(childCursor, StorageMediaFileSetID);
        }
        /* then proceed with instance level elements */
        OFString sopClassUID, instanceUID;
        do {
            /* iterate over all "value" elements */
            cursor = doc.getNamedNode(cursor, "value");
            if (cursor.valid())
            {
                if (!doc.getStringFromAttribute(doc.getNamedNode(cursor.getChild(), "sopclass"), sopClassUID, "uid").empty() &&
                    !doc.getStringFromAttribute(doc.getNamedNode(cursor.getChild(), "instance"), instanceUID, "uid").empty())
                {
                    /* check whether instance item already exists,
                       because the internal structure is organized in a strictly hierarchical manner  */
                    InstanceStruct *instance = gotoInstance(instanceUID);
                    if (instance == NULL)
                    {
                        /* if not, create a new instance list item */
                        instance = new InstanceStruct(sopClassUID, instanceUID);
                        if (instance != NULL)
                        {
                            /* and add it to the list of instances */
                            InstanceList.push_back(instance);
                            /* set cursor to new position */
                            Iterator = --InstanceList.end();
                            result = EC_Normal;
                        } else {
                            result = EC_MemoryExhausted;
                            break;
                        }
                    } else {
                        /* report a warning message and ignore this entry */
                        DCMSR_WARN("SOP Instance \"" << instanceUID << "\" already exists in reference list ... ignoring");
                    }
                }
                /* proceed with next node */
                cursor.gotoNext();
            }
        } while (cursor.valid());
        /* report a warning message if no "value" element found */
        if (result.bad())
            DCMSR_WARN("Series \"" << SeriesUID << "\" empty in reference list ... ignoring");
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::SeriesStruct::writeXML(STD_NAMESPACE ostream &stream,
                                                                const size_t flags) const
{
    /* write the series level attributes */
    stream << "<series uid=\"" << SeriesUID << "\">" << OFendl;
    writeStringValueToXML(stream, RetrieveAETitle, "aetitle", (flags & XF_writeEmptyTags) > 0);
    if ((flags & XF_writeEmptyTags) || !StorageMediaFileSetUID.empty() || !StorageMediaFileSetID.empty())
    {
        stream << "<fileset";
        if (!StorageMediaFileSetUID.empty())
            stream << " uid=\"" << StorageMediaFileSetUID << "\"";
        stream << ">" << StorageMediaFileSetID << "</fileset>" << OFendl;
    }
    /* iterate over all list items */
    OFListConstIterator(InstanceStruct *) iter = InstanceList.begin();
    const OFListConstIterator(InstanceStruct *) last = InstanceList.end();
    while (iter != last)
    {
        InstanceStruct *instance = *iter;
        /* check whether list item really exists */
        if (instance != NULL)
        {
            /* write instance level */
            stream << "<value>" << OFendl;
            stream << "<sopclass uid=\"" << instance->SOPClassUID << "\">";
            /* retrieve name of SOP class */
            const char *sopClass = dcmFindNameOfUID(instance->SOPClassUID.c_str());
            if (sopClass != NULL)
                stream << sopClass;
            stream << "</sopclass>" << OFendl;
            stream << "<instance uid=\"" << instance->InstanceUID << "\"/>" << OFendl;
            stream << "</value>" << OFendl;
        }
        iter++;
    }
    stream << "</series>" << OFendl;
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


OFCondition DSRSOPInstanceReferenceList::SeriesStruct::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    /* check for empty instance list */
    if (!InstanceList.empty())
    {
        /* set cursor to first list item */
        Iterator = InstanceList.begin();
        /* check whether list item is valid */
        if (*Iterator != NULL)
            result = EC_Normal;
        else
            result = EC_CorruptedData;
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::SeriesStruct::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    /* goto next list item */
    if (++Iterator != InstanceList.end())
    {
        /* check whether list item is valid */
        if (*Iterator != NULL)
            result = EC_Normal;
        else
            result = EC_CorruptedData;
    }
    return result;
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


OFCondition DSRSOPInstanceReferenceList::SeriesStruct::removeItem()
{
    OFCondition result = EC_IllegalCall;
    /* check whether list is empty or iterator is invalid */
    if (!InstanceList.empty() && (Iterator != InstanceList.end()))
    {
        /* free memory */
        delete (*Iterator);
        /* remove item from list */
        Iterator = InstanceList.erase(Iterator);
        result = EC_Normal;
    }
    return result;
}


// --- DSRSOPInstanceReferenceList::StudyStruct ---


DSRSOPInstanceReferenceList::StudyStruct::StudyStruct(const OFString &studyUID)
  : StudyUID(studyUID),
    SeriesList(),
    Iterator()
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


size_t DSRSOPInstanceReferenceList::StudyStruct::getNumberOfInstances() const
{
    size_t result = 0;
    OFListConstIterator(SeriesStruct *) iter = SeriesList.begin();
    const OFListConstIterator(SeriesStruct *) last = SeriesList.end();
    /* for all series in the list */
    while (iter != last)
    {
        /* sum up the number of instances */
        if (*iter != NULL)
            result += OFstatic_cast(SeriesStruct *, *iter)->getNumberOfInstances();
        ++iter;
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::StudyStruct::read(DcmItem &dataset)
{
    /* first, check whether sequence is present and non-empty */
    DcmSequenceOfItems sequence(DCM_ReferencedSeriesSequence);
    OFCondition result = getElementFromDataset(dataset, sequence);
    checkElementValue(sequence, "1-n", "1", result);
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
                if (getAndCheckStringValueFromDataset(*item, DCM_SeriesInstanceUID, seriesUID, "1", "1", "ReferencedSeriesSequence").good())
                {
                    /* check whether series item already exists,
                       because the internal structure is organized in a strictly hierarchical manner  */
                    SeriesStruct *series = gotoSeries(seriesUID);
                    if (series == NULL)
                    {
                        /* if not, create a new series list item */
                        series = new SeriesStruct(seriesUID);
                        if (series != NULL)
                        {
                            /* and add it to the list of studies */
                            SeriesList.push_back(series);
                        } else {
                            result = EC_MemoryExhausted;
                            break;
                        }
                    }
                    if (series != NULL)
                    {
                        /* set cursor to new position */
                        Iterator = --SeriesList.end();
                        /* read further attributes on series level and the instance level */
                        result = series->read(*item);
                    }
                }
            }
        }
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::StudyStruct::write(DcmItem &dataset) const
{
    OFCondition result = EC_Normal;
    /* store the study level attributes */
    dataset.putAndInsertOFStringArray(DCM_StudyInstanceUID, StudyUID);
    /* iterate over all list items */
    OFListConstIterator(SeriesStruct *) iter = SeriesList.begin();
    const OFListConstIterator(SeriesStruct *) last = SeriesList.end();
    while ((iter != last) && result.good())
    {
        SeriesStruct *series = OFstatic_cast(SeriesStruct *, *iter);
        /* check whether list item really exists */
        if (series != NULL)
        {
            DcmItem *item = NULL;
            /* create a new item (and a sequence if required) */
            result = dataset.findOrCreateSequenceItem(DCM_ReferencedSeriesSequence, item, -2 /*append new*/);
            /* write series and instance level */
            if (result.good())
                result = series->write(*item);
        }
        iter++;
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::StudyStruct::readXML(const DSRXMLDocument &doc,
                                                              DSRXMLCursor cursor)
{
    OFCondition result = SR_EC_InvalidDocument;
    if (cursor.valid())
    {
        OFString seriesUID;
        while (cursor.valid())
        {
            /* check for known element tags */
            if (doc.checkNode(cursor, "series").good())
            {
                if (!doc.getStringFromAttribute(cursor, seriesUID, "uid").empty())
                {
                    /* check whether series item already exists,
                       because the internal structure is organized in a strictly hierarchical manner  */
                    SeriesStruct *series = gotoSeries(seriesUID);
                    if (series == NULL)
                    {
                        /* if not, create a new series list item */
                        series = new SeriesStruct(seriesUID);
                        if (series != NULL)
                        {
                            /* and add it to the list of series */
                            SeriesList.push_back(series);
                        } else {
                            result = EC_MemoryExhausted;
                            break;
                        }
                    }
                    if (series != NULL)
                    {
                        /* set cursor to new position */
                        Iterator = --SeriesList.end();
                        /* read further attributes on series level and the instance level */
                        result = series->readXML(doc, cursor.getChild());
                    }
                }
            }
            /* proceed with next node */
            cursor.gotoNext();
        }
        /* report a warning message if no "value" element found */
        if (result.bad())
            DCMSR_WARN("Study \"" << StudyUID << "\" empty in reference list ... ignoring");
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::StudyStruct::writeXML(STD_NAMESPACE ostream &stream,
                                                               const size_t flags) const
{
    OFCondition result = EC_Normal;
    /* write the study level attributes */
    stream << "<study uid=\"" << StudyUID << "\">" << OFendl;
    /* iterate over all list items */
    OFListConstIterator(SeriesStruct *) iter = SeriesList.begin();
    const OFListConstIterator(SeriesStruct *) last = SeriesList.end();
    while ((iter != last) && result.good())
    {
        SeriesStruct *series = OFstatic_cast(SeriesStruct *, *iter);
        /* check whether list item really exists */
        if (series != NULL)
        {
            /* write series and instance level */
            result = series->writeXML(stream, flags);
        }
        iter++;
    }
    stream << "</study>" << OFendl;
    return result;
}


DSRSOPInstanceReferenceList::SeriesStruct *DSRSOPInstanceReferenceList::StudyStruct::gotoSeries(const OFString &seriesUID)
{
    SeriesStruct *series = NULL;
    /* first, check whether the current series is the one we're searching for */
    if ((Iterator != SeriesList.end()) && (*Iterator != NULL) && (OFstatic_cast(SeriesStruct *, *Iterator)->SeriesUID == seriesUID))
        series = OFstatic_cast(SeriesStruct *, *Iterator);
    else
    {
        /* start with the first list item */
        Iterator = SeriesList.begin();
        const OFListIterator(SeriesStruct *) last = SeriesList.end();
        /* search for given series UID */
        while ((Iterator != last) && ((*Iterator == NULL) || (OFstatic_cast(SeriesStruct *, *Iterator)->SeriesUID != seriesUID)))
            Iterator++;
        /* item found */
        if (Iterator != last)
            series = OFstatic_cast(SeriesStruct *, *Iterator);
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
        SeriesStruct *series = OFstatic_cast(SeriesStruct *, *Iterator);
        /* continue search on instance level */
        if (series != NULL)
            instance = series->gotoInstance(instanceUID);
        /* if found exit loop, else goto next */
        if (instance == NULL)
            Iterator++;
    }
    return instance;
}


OFCondition DSRSOPInstanceReferenceList::StudyStruct::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    /* check for empty series list */
    if (!SeriesList.empty())
    {
        /* set cursor to first list item */
        Iterator = SeriesList.begin();
        /* check whether list item is valid */
        if (*Iterator != NULL)
        {
            /* do the same for instance level */
            result = OFstatic_cast(SeriesStruct *, *Iterator)->gotoFirstItem();
        } else
            result = EC_CorruptedData;
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::StudyStruct::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    /* goto next list item */
    if (Iterator != SeriesList.end())
    {
        /* check whether current list item is valid */
        if (*Iterator != NULL)
        {
            /* try to go to the next instance item */
            result = OFstatic_cast(SeriesStruct *, *Iterator)->gotoNextItem();
            /* if this fails ... */
            if (result.bad())
            {
                /* goto to the first instance of the next series item */
                if (++Iterator != SeriesList.end())
                {
                    if (*Iterator != NULL)
                        result = OFstatic_cast(SeriesStruct *, *Iterator)->gotoFirstItem();
                }
            }
        } else
            result = EC_CorruptedData;
    }
    return result;
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


OFCondition DSRSOPInstanceReferenceList::StudyStruct::removeItem()
{
    OFCondition result = EC_IllegalCall;
    /* check whether list is empty or iterator is invalid */
    if (!SeriesList.empty() && (Iterator != SeriesList.end()))
    {
        SeriesStruct *series = OFstatic_cast(SeriesStruct *, *Iterator);
        if (series != NULL)
        {
            result = series->removeItem();
            /* check whether lower level list has become empty */
            if (result.good() && series->InstanceList.empty())
            {
                /* free memory */
                delete series;
                /* if so, remove series from list and set iterator to the next item */
                Iterator = SeriesList.erase(Iterator);
            }
        }
    }
    return result;
}


void DSRSOPInstanceReferenceList::StudyStruct::removeIncompleteItems()
{
    Iterator = SeriesList.begin();
    const OFListIterator(SeriesStruct *) last = SeriesList.end();
    /* for all series in the list */
    while (Iterator != last)
    {
        SeriesStruct *series = OFstatic_cast(SeriesStruct *, *Iterator);
        if (series != NULL)
        {
            /* check whether list of series is empty */
            if (series->InstanceList.empty())
            {
                /* free memory */
                delete series;
                /* if so, remove series from list and set iterator to the next item */
                Iterator = SeriesList.erase(Iterator);
            } else
                ++Iterator;
        } else
          ++Iterator;
    }
}


// --- DSRSOPInstanceReferenceList ---

DSRSOPInstanceReferenceList::DSRSOPInstanceReferenceList(const DcmTagKey &sequence)
  : SequenceTag(sequence),
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


size_t DSRSOPInstanceReferenceList::getNumberOfInstances() const
{
    size_t result = 0;
    OFListConstIterator(StudyStruct *) iter = StudyList.begin();
    const OFListConstIterator(StudyStruct *) last = StudyList.end();
    /* for all studies in the list */
    while (iter != last)
    {
        /* sum up the number of instances */
        if (*iter != NULL)
            result += OFstatic_cast(StudyStruct *, *iter)->getNumberOfInstances();
        ++iter;
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::read(DcmItem &dataset)
{
    /* first, check whether sequence is present and non-empty */
    DcmSequenceOfItems sequence(SequenceTag);
    OFCondition result = getElementFromDataset(dataset, sequence);
    checkElementValue(sequence, "1-n", "1C", result);
    if (result.good())
    {
        OFString sequenceName = DcmTag(SequenceTag).getTagName();
        /* iterate over all sequence items */
        for (unsigned long i = 0; i < sequence.card(); i++)
        {
            DcmItem *item = sequence.getItem(i);
            if (item != NULL)
            {
                /* get the study instance UID */
                OFString studyUID;
                if (getAndCheckStringValueFromDataset(*item, DCM_StudyInstanceUID, studyUID, "1", "1", sequenceName.c_str()).good())
                {
                    /* check whether study item already exists,
                       because the internal structure is organized in a strictly hierarchical manner  */
                    StudyStruct *study = gotoStudy(studyUID);
                    if (study == NULL)
                    {
                        /* if not, create a new study list item */
                        study = new StudyStruct(studyUID);
                        if (study != NULL)
                        {
                            /* and add it to the list of studies */
                            StudyList.push_back(study);
                        } else {
                            result = EC_MemoryExhausted;
                            break;
                        }
                    }
                    if (study != NULL)
                    {
                        /* set cursor to new position */
                        Iterator = --StudyList.end();
                        /* read attributes on series and instance level */
                        result = study->read(*item);
                    }
                }
            }
        }
        /* remove empty/incomplete items from the list structure */
        removeIncompleteItems();
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::write(DcmItem &dataset) const
{
    OFCondition result = EC_Normal;
    /* iterate over all list items */
    OFListConstIterator(StudyStruct *) iter = StudyList.begin();
    const OFListConstIterator(StudyStruct *) last = StudyList.end();
    while ((iter != last) && result.good())
    {
        StudyStruct *study = OFstatic_cast(StudyStruct *, *iter);
        /* check whether list item really exists */
        if (study != NULL)
        {
            DcmItem *item = NULL;
            /* create a new item (and a sequence if required) */
            result = dataset.findOrCreateSequenceItem(SequenceTag, item, -2 /*append new*/);
            /* write study, series and instance level */
            if (result.good())
                result = study->write(*item);
        }
        iter++;
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::readXML(const DSRXMLDocument &doc,
                                                 DSRXMLCursor cursor,
                                                 const size_t /*flags*/)
{
    /* default: no error, e.g. for empty list of references */
    OFCondition result = EC_Normal;
    if (cursor.valid())
    {
        OFString studyUID;
        while (cursor.valid())
        {
            /* check for known element tags */
            if (doc.checkNode(cursor, "study").good())
            {
                if (!doc.getStringFromAttribute(cursor, studyUID, "uid").empty())
                {
                    /* check whether study item already exists,
                       because the internal structure is organized in a strictly hierarchical manner  */
                    StudyStruct *study = gotoStudy(studyUID);
                    if (study == NULL)
                    {
                        /* if not, create a new study list item */
                        study = new StudyStruct(studyUID);
                        if (study != NULL)
                        {
                            /* and add it to the list of studies */
                            StudyList.push_back(study);
                        } else {
                            result = EC_MemoryExhausted;
                            break;
                        }
                    }
                    if (study != NULL)
                    {
                        /* set cursor to new position */
                        Iterator = --StudyList.end();
                        /* read attributes on series and instance level */
                        result = study->readXML(doc, cursor.getChild());
                    }
                }
            }
            /* proceed with next node */
            cursor.gotoNext();
        }
        /* remove empty/incomplete items from the list structure */
        removeIncompleteItems();
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::writeXML(STD_NAMESPACE ostream &stream,
                                                  const size_t flags) const
{
    OFCondition result = EC_Normal;
    /* iterate over all list items */
    OFListConstIterator(StudyStruct *) iter = StudyList.begin();
    const OFListConstIterator(StudyStruct *) last = StudyList.end();
    while ((iter != last) && result.good())
    {
        StudyStruct *study = OFstatic_cast(StudyStruct *, *iter);
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
    if ((Iterator != StudyList.end()) && (*Iterator != NULL) && (OFstatic_cast(StudyStruct *, *Iterator)->StudyUID == studyUID))
        study = OFstatic_cast(StudyStruct *, *Iterator);
    else
    {
        /* start with the first list item */
        Iterator = StudyList.begin();
        const OFListIterator(StudyStruct *) last = StudyList.end();
        /* search for given study UID */
        while ((Iterator != last) && ((*Iterator == NULL) || (OFstatic_cast(StudyStruct *, *Iterator)->StudyUID != studyUID)))
            Iterator++;
        /* item found */
        if (Iterator != last)
            study = OFstatic_cast(StudyStruct *, *Iterator);
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
    if (!studyUID.empty() && !seriesUID.empty() && !sopClassUID.empty() && !instanceUID.empty())
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


OFCondition DSRSOPInstanceReferenceList::addItem(DcmItem &dataset)
{
    OFString studyUID, seriesUID, sopClassUID, instanceUID;
    /* retrieve element values from dataset */
    dataset.findAndGetOFString(DCM_StudyInstanceUID, studyUID);
    dataset.findAndGetOFString(DCM_SeriesInstanceUID, seriesUID);
    dataset.findAndGetOFString(DCM_SOPClassUID, sopClassUID);
    dataset.findAndGetOFString(DCM_SOPInstanceUID, instanceUID);
    /* add new item to the list of references (if valid) */
    return addItem(studyUID, seriesUID, sopClassUID, instanceUID);
}


OFCondition DSRSOPInstanceReferenceList::removeItem()
{
    OFCondition result = EC_IllegalCall;
    /* check whether list is empty or iterator is invalid */
    if (!StudyList.empty() && (Iterator != StudyList.end()))
    {
        StudyStruct *study = OFstatic_cast(StudyStruct *, *Iterator);
        if (study != NULL)
        {
            result = study->removeItem();
            /* check whether lower level list has become empty */
            if (result.good() && study->SeriesList.empty())
            {
                /* free memory */
                delete study;
                /* if so, remove study from list and set iterator to the next item */
                Iterator = StudyList.erase(Iterator);
            }
        }
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::removeItem(const OFString &sopClassUID,
                                                    const OFString &instanceUID)
{
    /* goto specified item ... */
    OFCondition result = gotoItem(sopClassUID, instanceUID);
    /* ... and remove it */
    if (result.good())
        result = removeItem();
    return result;
}


OFCondition DSRSOPInstanceReferenceList::removeItem(const OFString &studyUID,
                                                    const OFString &seriesUID,
                                                    const OFString &instanceUID)
{
    /* goto specified item ... */
    OFCondition result = gotoItem(studyUID, seriesUID, instanceUID);
    /* ... and remove it */
    if (result.good())
        result = removeItem();
    return result;
}


void DSRSOPInstanceReferenceList::removeIncompleteItems()
{
    Iterator = StudyList.begin();
    const OFListIterator(StudyStruct *) last = StudyList.end();
    /* for all studies in the list */
    while (Iterator != last)
    {
        StudyStruct *study = OFstatic_cast(StudyStruct *, *Iterator);
        if (study != NULL)
        {
            /* remove empty/incomplete items on series/instance level */
            study->removeIncompleteItems();
            /* check whether list of series is empty */
            if (study->SeriesList.empty())
            {
                /* free memory */
                delete study;
                /* if so, remove study from list and set iterator to the next item */
                Iterator = StudyList.erase(Iterator);
            } else
                ++Iterator;
        } else
          ++Iterator;
    }
}


OFCondition DSRSOPInstanceReferenceList::gotoItem(const OFString &sopClassUID,
                                                  const OFString &instanceUID)
{
    OFCondition result = EC_IllegalParameter;
    /* check parameters first */
    if (!sopClassUID.empty() && !instanceUID.empty())
    {
        OFBool sopClassMatch = OFFalse;
        result = SR_EC_SOPInstanceNotFound;
        /* start with first study */
        Iterator = StudyList.begin();
        const OFListIterator(StudyStruct *) last = StudyList.end();
        /* iterate over all studies */
        while ((Iterator != last) && result.bad())
        {
            StudyStruct *study = OFstatic_cast(StudyStruct *, *Iterator);
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
    if (!studyUID.empty() && !seriesUID.empty() && !instanceUID.empty())
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


OFCondition DSRSOPInstanceReferenceList::gotoFirstItem()
{
    OFCondition result = EC_IllegalCall;
    /* check for empty study list */
    if (!StudyList.empty())
    {
        /* set cursor to first list item */
        Iterator = StudyList.begin();
        if (*Iterator != NULL)
        {
            /* do the same for series and instance level */
            result = OFstatic_cast(StudyStruct *, *Iterator)->gotoFirstItem();
        }
    }
    return result;
}


OFCondition DSRSOPInstanceReferenceList::gotoNextItem()
{
    OFCondition result = EC_IllegalCall;
    /* goto next list item */
    if (Iterator != StudyList.end())
    {
        /* check whether current list item is valid */
        if (*Iterator != NULL)
        {
            /* try to go to the next instance item */
            result = OFstatic_cast(StudyStruct *, *Iterator)->gotoNextItem();
            /* if this fails ... */
            if (result.bad())
            {
                /* goto to the first series/instance of the next stidy item */
                if (++Iterator != StudyList.end())
                {
                    if (*Iterator != NULL)
                        result = OFstatic_cast(StudyStruct *, *Iterator)->gotoFirstItem();
                }
            }
        } else
            result = EC_CorruptedData;
    }
    return result;
}


DSRSOPInstanceReferenceList::StudyStruct *DSRSOPInstanceReferenceList::getCurrentStudy() const
{
    StudyStruct *study = NULL;
    /* check whether current study is valid */
    OFListConstIterator(StudyStruct *) it = Iterator;
    if (it != StudyList.end())
        study = OFstatic_cast(StudyStruct *, *Iterator);
    return study;
}


DSRSOPInstanceReferenceList::SeriesStruct *DSRSOPInstanceReferenceList::getCurrentSeries() const
{
    SeriesStruct *series = NULL;
    StudyStruct *study = getCurrentStudy();
    /* check whether current series is valid */
    if ((study != NULL) && (study->Iterator != study->SeriesList.end()))
        series = OFstatic_cast(SeriesStruct *, *(study->Iterator));
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


const OFString &DSRSOPInstanceReferenceList::getStudyInstanceUID(OFString &stringValue) const
{
    /* check whether current study is valid */
    StudyStruct *study = getCurrentStudy();
    /* get study instance UID or clear string if invalid */
    if (study != NULL)
        stringValue = study->StudyUID;
    else
        stringValue.clear();
    return stringValue;
}


const OFString &DSRSOPInstanceReferenceList::getSeriesInstanceUID(OFString &stringValue) const
{
    /* check whether current series is valid */
    SeriesStruct *series = getCurrentSeries();
    /* get series instance UID or clear string if invalid */
    if (series != NULL)
        stringValue = series->SeriesUID;
    else
        stringValue.clear();
    return stringValue;
}


const OFString &DSRSOPInstanceReferenceList::getSOPInstanceUID(OFString &stringValue) const
{
    /* check whether current instance is valid */
    InstanceStruct *instance = getCurrentInstance();
    /* get SOP instance UID or clear string if invalid */
    if (instance != NULL)
        stringValue = instance->InstanceUID;
    else
        stringValue.clear();
    return stringValue;
}


const OFString &DSRSOPInstanceReferenceList::getSOPClassUID(OFString &stringValue) const
{
    /* check whether current instance is valid */
    InstanceStruct *instance = getCurrentInstance();
    /* get SOP class UID or clear string if invalid */
    if (instance != NULL)
        stringValue = instance->SOPClassUID;
    else
        stringValue.clear();
    return stringValue;
}


const OFString &DSRSOPInstanceReferenceList::getRetrieveAETitle(OFString &stringValue) const
{
    /* check whether current series is valid */
    SeriesStruct *series = getCurrentSeries();
    /* get retrieve application entity title or clear string if invalid */
    if (series != NULL)
        stringValue = series->RetrieveAETitle;
    else
        stringValue.clear();
    return stringValue;
}


const OFString &DSRSOPInstanceReferenceList::getStorageMediaFileSetID(OFString &stringValue) const
{
    /* check whether current series is valid */
    SeriesStruct *series = getCurrentSeries();
    /* get storage media file set ID or clear string if invalid */
    if (series != NULL)
        stringValue = series->StorageMediaFileSetID;
    else
        stringValue.clear();
    return stringValue;
}


const OFString &DSRSOPInstanceReferenceList::getStorageMediaFileSetUID(OFString &stringValue) const
{
    /* check whether current series is valid */
    SeriesStruct *series = getCurrentSeries();
    /* get storage media file set UID or clear string if invalid */
    if (series != NULL)
        stringValue = series->StorageMediaFileSetUID;
    else
        stringValue.clear();
    return stringValue;
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
 *  Revision 1.19  2010-10-14 13:14:42  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.18  2010-09-29 08:32:26  joergr
 *  Used more specific "moduleName" for getAndCheckElementFromDataset() and
 *  checkElementValue().
 *
 *  Revision 1.17  2009-10-14 10:49:32  joergr
 *  Fixed minor issues in log output. Also updated copyright date (if required).
 *
 *  Revision 1.16  2009-10-13 14:57:51  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.15  2009-09-04 13:53:10  meichel
 *  Minor const iterator related changes needed to compile with VC6 with HAVE_STL
 *
 *  Revision 1.14  2008-07-17 12:00:09  joergr
 *  Replaced call to getSequenceFromDataset() by getElementFromDataset().
 *
 *  Revision 1.13  2007-11-15 16:43:43  joergr
 *  Fixed coding style to be more consistent.
 *
 *  Revision 1.12  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.11  2006/05/11 09:16:49  joergr
 *  Moved containsExtendedCharacters() from dcmsr to dcmdata module.
 *
 *  Revision 1.10  2005/12/15 16:53:48  joergr
 *  Added explicit type cast, required for Sun CC 2.0.1 on Solaris.
 *
 *  Revision 1.9  2005/12/08 15:48:07  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.8  2005/07/27 16:34:26  joergr
 *  Added method that allows to add a DICOM dataset to the list of references.
 *
 *  Revision 1.7  2004/11/22 16:39:12  meichel
 *  Added method that checks if the SR document contains non-ASCII characters
 *    in any of the strings affected by SpecificCharacterSet.
 *
 *  Revision 1.6  2004/04/16 13:33:52  joergr
 *  Added explicit typecast to result of dereferencing operator to keep Sun CC
 *  2.0.1 happy.
 *
 *  Revision 1.5  2003/08/07 15:21:53  joergr
 *  Added brackets around "bitwise and" operator/operands to avoid warnings
 *  reported by MSVC5.
 *
 *  Revision 1.4  2003/08/07 13:51:53  joergr
 *  Added readXML functionality.
 *  Enhanced class DSRSOPInstanceReferenceList: empty/incomplete items (e.g.
 *  series with no instances or studies with no series) are automatically
 *  removed from the list and "degenerated" (i.e. not stricly hierarchical)
 *  sequence structures are now also supported.
 *  Adapted type casts to new-style typecast operators defined in ofcast.h.
 *  Adapted for use of OFListConstIterator, needed for compiling with HAVE_STL.
 *
 *  Revision 1.3  2002/05/24 09:52:02  joergr
 *  Added missing member variables to member initialization list (warnings
 *  reported by gcc 2.95.3 with additional options).
 *
 *  Revision 1.2  2002/05/14 08:16:29  joergr
 *  Added removeItem() methods.
 *
 *  Revision 1.1  2002/05/07 12:54:28  joergr
 *  Added support for the Current Requested Procedure Evidence Sequence and the
 *  Pertinent Other Evidence Sequence to the dcmsr module.
 *
 *
 */
