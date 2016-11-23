/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class TID1600_ImageLibrary
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/tid1600.h"
#include "dcmtk/dcmsr/cmr/cid29e.h"
#include "dcmtk/dcmsr/cmr/cid244e.h"
#include "dcmtk/dcmsr/cmr/cid4031e.h"
#include "dcmtk/dcmsr/cmr/cid10013e.h"
#include "dcmtk/dcmsr/cmr/cid10033e.h"
#include "dcmtk/dcmsr/cmr/logger.h"
#include "dcmtk/dcmsr/codes/dcm.h"
#include "dcmtk/dcmsr/codes/srt.h"
#include "dcmtk/dcmsr/codes/ucum.h"
#include "dcmtk/dcmsr/dsrdattn.h"

#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcuid.h"


// helper macros for checking the return value of API calls
#define CHECK_RESULT(call) if (result.good()) result = call
#define STORE_RESULT(call) result = call
#define GOOD_RESULT(call) if (result.good()) call
#define BAD_RESULT(call) if (result.bad()) call

// index positions in node list (makes source code more readable)
#define IMAGE_LIBRARY            0
#define LAST_IMAGE_LIBRARY_GROUP 1
#define LAST_IMAGE_LIBRARY_ENTRY 2

// general information on TID 1600 (Image Library)
#define TEMPLATE_NUMBER      "1600"
#define MAPPING_RESOURCE     "DCMR"
#define MAPPING_RESOURCE_UID UID_DICOMContentMappingResource
#define TEMPLATE_TYPE        OFTrue  /* extensible */


// conditions constants
makeOFConditionConst(CMR_EC_NoImageLibrary,                                OFM_dcmsr, 1600, OF_error, "No Image Library");
makeOFConditionConst(CMR_EC_NoImageLibraryGroup,                           OFM_dcmsr, 1601, OF_error, "No Image Library Group");
makeOFConditionConst(CMR_EC_NoImageLibraryEntry,                           OFM_dcmsr, 1602, OF_error, "No Image Library Entry");
makeOFConditionConst(CMR_EC_CannotAddMultipleImageLibraryEntryDescriptors, OFM_dcmsr, 1603, OF_error, "Cannot add multiple Image Library Entry Descriptors");
makeOFConditionConst(CMR_EC_MissingImageLibraryEntryDescriptorModality,    OFM_dcmsr, 1604, OF_error, "Missing Image Library Entry Descriptor 'Modality'");
makeOFConditionConst(CMR_EC_WrongImageLibraryEntryDescriptorModality,      OFM_dcmsr, 1605, OF_error, "Wrong Image Library Entry Descriptor 'Modality'");
makeOFConditionConst(CMR_EC_NoImageLibraryEntryDescriptorsToBeAdded,       OFM_dcmsr, 1606, OF_ok,    "No Image Library Entry Descriptors to be added");


TID1600_ImageLibrary::TID1600_ImageLibrary(const OFBool createLibrary)
  : DSRSubTemplate(TEMPLATE_NUMBER, MAPPING_RESOURCE, MAPPING_RESOURCE_UID)
{
    setExtensible(TEMPLATE_TYPE);
    /* need to store image library, last image library group and entry */
    reserveEntriesInNodeList(3);
    /* TID 1600 (Image Library) Row 1 */
    if (createLibrary)
        createImageLibrary();
}


OFBool TID1600_ImageLibrary::isValid() const
{
    /* check whether base class is valid and image library exists */
    return DSRSubTemplate::isValid() && hasImageLibrary();
}


OFBool TID1600_ImageLibrary::hasImageLibrary() const
{
    /* check for image library (TID 1600 - Row 1) */
    return (getEntryFromNodeList(IMAGE_LIBRARY) > 0);
}


OFBool TID1600_ImageLibrary::hasImageLibraryGroup() const
{
    /* check for image library group (TID 1600 - Row 2) */
    return (getEntryFromNodeList(LAST_IMAGE_LIBRARY_GROUP) > 0);
}


OFCondition TID1600_ImageLibrary::createNewImageLibrary()
{
    clear();
    /* TID 1600 (Image Library) Row 1 */
    return createImageLibrary();
}


OFCondition TID1600_ImageLibrary::addImageGroup()
{
    OFCondition result = CMR_EC_NoImageLibrary;
    /* check whether image library exists */
    if (hasImageLibrary())
    {
        /* go to last image library group (if any) */
        if (gotoEntryFromNodeList(this, LAST_IMAGE_LIBRARY_GROUP) > 0)
        {
            /* TID 1600 (Image Library) Row 2, append to last group */
            STORE_RESULT(addContentItem(RT_contains, VT_Container, CODE_DCM_ImageLibraryGroup));
        }
        /* go to image library (root node) */
        else if (gotoEntryFromNodeList(this, IMAGE_LIBRARY) > 0)
        {
            /* TID 1600 (Image Library) Row 2, create first group (child) */
            STORE_RESULT(addChildContentItem(RT_contains, VT_Container, CODE_DCM_ImageLibraryGroup));
        } else {
            /* should never happen but ... */
            result = SR_EC_InvalidTemplateStructure;
        }
        CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1600 - Row 2"));
        /* store ID of recently added node for later use */
        if (result.good())
        {
            storeEntryInNodeList(LAST_IMAGE_LIBRARY_GROUP, getNodeID());
            storeEntryInNodeList(LAST_IMAGE_LIBRARY_ENTRY, 0 /* forget last entry */);
        }
    }
    return result;
}


OFCondition TID1600_ImageLibrary::addImageEntry(DcmItem &dataset,
                                                const AddImageMode mode,
                                                const OFBool check)
{
    OFCondition result = CMR_EC_NoImageLibraryGroup;
    /* check whether image library group exists */
    if (hasImageLibraryGroup())
    {
        /* create a new subtree in order to "rollback" in case of error */
        DSRDocumentSubTree *tid1601 = new DSRDocumentSubTree;
        if (tid1601 != NULL)
        {
            DSRImageReferenceValue imageRef;
            /* TID 1601 (Image Library Entry) Row 1 */
            STORE_RESULT(tid1601->addContentItem(RT_contains, VT_Image, DSRCodedEntryValue(), check));
            CHECK_RESULT(imageRef.setReference(dataset, check));
            CHECK_RESULT(tid1601->getCurrentContentItem().setImageReference(imageRef, check));
            CHECK_RESULT(tid1601->getCurrentContentItem().setAnnotationText("TID 1601 - Row 1"));
            const size_t lastNode = tid1601->getNodeID();
            /* TID 1601 (Image Library Entry) Row 2 */
            if (mode == withAllDescriptors)
            {
                /* create a new subtree for TID 1602 (Image Library Entry Descriptors) */
                DSRDocumentSubTree *tid1602 = new DSRDocumentSubTree;
                if (tid1602 != NULL)
                {
                    /* call the function doing the real work */
                    STORE_RESULT(addImageEntryDescriptors(*tid1602, dataset, check));
                    /* if everything was OK, insert new subtree into the template */
                    if (result.good() && !tid1602->isEmpty())
                    {
                        /* insert subtree below current node */
                        STORE_RESULT(tid1601->insertSubTree(tid1602));
                        /* in case of error, make sure that memory is freed */
                        BAD_RESULT(delete tid1602);
                    } else {
                        CHECK_RESULT(CMR_EC_NoImageLibraryEntryDescriptorsToBeAdded);
                        /* delete the new subtree since it has not been inserted */
                        delete tid1602;
                    }
                } else
                    result = EC_MemoryExhausted;
            }
            /* if everything was OK, insert new subtree into the template */
            if (result.good())
            {
                E_AddMode addMode = AM_afterCurrent;
                /* go to last image library entry (if any) */
                if (gotoEntryFromNodeList(this, LAST_IMAGE_LIBRARY_ENTRY) > 0)
                    addMode = AM_afterCurrent;
                /* go to last image library group */
                else if (gotoEntryFromNodeList(this, LAST_IMAGE_LIBRARY_GROUP) > 0)
                    addMode = AM_belowCurrent;
                else {
                    /* should never happen but ... */
                    result = CMR_EC_NoImageLibraryGroup;
                }
                /* there is at least an image library group */
                if (result.good())
                {
                    /* insert subtree at current position */
                    STORE_RESULT(insertSubTree(tid1601, addMode));
                    /* store ID of recently added node for later use */
                    GOOD_RESULT(storeEntryInNodeList(LAST_IMAGE_LIBRARY_ENTRY, lastNode));
                }
            }
            /* in case of error, make sure that memory is freed */
            BAD_RESULT(delete tid1601);
        } else
            result = EC_MemoryExhausted;
    }
    return result;
}


OFCondition TID1600_ImageLibrary::addImageEntryDescriptors(DcmItem &dataset,
                                                           const OFBool check)
{
    OFCondition result = CMR_EC_NoImageLibraryGroup;
    /* check whether image library group exists */
    if (hasImageLibraryGroup())
    {
        /* create a new subtree in order to "rollback" in case of error */
        DSRDocumentSubTree *subTree = new DSRDocumentSubTree;
        if (subTree != NULL)
        {
            /* call the function doing the real work */
            STORE_RESULT(addImageEntryDescriptors(*subTree, dataset, check));
            /* if everything was OK, insert new subtree into the template */
            if (result.good() && !subTree->isEmpty())
            {
                /* go to last image library group (if any) */
                if (gotoEntryFromNodeList(this, LAST_IMAGE_LIBRARY_GROUP) > 0)
                {
                    /* check whether TID 1600 (Image Library) Row 3 is already there */
                    const DSRDocumentTreeNode *childNode = getChildNode();
                    if ((childNode != NULL) && (childNode->getRelationshipType() == RT_hasAcqContext))
                    {
                        /* only a single invocation of the included template allowed */
                        result = CMR_EC_CannotAddMultipleImageLibraryEntryDescriptors;
                    } else {
                        /* insert subtree at current position */
                        STORE_RESULT(insertSubTree(subTree, AM_belowCurrentBeforeFirstChild));
                    }
                } else {
                    /* should never happen but ... */
                    result = CMR_EC_NoImageLibraryGroup;
                }
                /* in case of error, make sure that memory is freed */
                BAD_RESULT(delete subTree);
            } else {
                CHECK_RESULT(CMR_EC_NoImageLibraryEntryDescriptorsToBeAdded);
                /* delete the new subtree since it has not been inserted */
                delete subTree;
            }
        } else
            result = EC_MemoryExhausted;
    }
    return result;
}


OFCondition TID1600_ImageLibrary::getImageEntryModality(DSRCodedEntryValue &modalityCode)
{
    OFCondition result = CMR_EC_NoImageLibraryEntry;
    /* go to last image library entry (if any) */
    if (gotoEntryFromNodeList(this, LAST_IMAGE_LIBRARY_ENTRY) > 0)
    {
        /* get value of TID 1602 (Image Library Entry Descriptors) Row 1 */
        if ((gotoNamedChildNode(CODE_DCM_Modality, OFFalse /*searchIntoSub*/) > 0))
        {
            modalityCode = getCurrentContentItem().getCodeValue();
            result = EC_Normal;
        } else
            result = CMR_EC_MissingImageLibraryEntryDescriptorModality;
    }
    /* in case of error, clear the result variable */
    BAD_RESULT(modalityCode.clear());
    return result;
}


// set methods

OFCondition TID1600_ImageLibrary::setPETImageRadionuclide(const CID4020_PETRadionuclide &radionuclide,
                                                          const OFBool check)
{
    /* remember current node */
    const DSRDocumentTreeNodeCursor cursor(getCursor());
    /* go to image library entry and check for expected modality */
    OFCondition result = goAndCheckImageLibraryEntry(CODE_DCM_PositronEmissionTomography);
    /* set content item value (and add a new content item if needed) */
    CHECK_RESULT(setCodeContentItemFromValue(CODE_SRT_Radionuclide, radionuclide, "TID 1607 - Row 1", check));
    /* in case of error, reset cursor to stored node */
    BAD_RESULT(setCursor(cursor));
    return result;
}


OFCondition TID1600_ImageLibrary::setPETImageRadiopharmaceuticalAgent(const CID4021_PETRadiopharmaceutical &agent,
                                                                      const OFBool check)
{
    /* remember current node */
    const DSRDocumentTreeNodeCursor cursor(getCursor());
    /* go to image library entry and check for expected modality */
    OFCondition result = goAndCheckImageLibraryEntry(CODE_DCM_PositronEmissionTomography);
    /* set content item value (and add a new content item if needed) */
    CHECK_RESULT(setCodeContentItemFromValue(CODE_SRT_RadiopharmaceuticalAgent, agent, "TID 1607 - Row 2", check));
    /* in case of error, reset cursor to stored node */
    BAD_RESULT(setCursor(cursor));
    return result;
}


OFCondition TID1600_ImageLibrary::setPETImageRadiopharmaceuticalStartDateTime(const OFString &dateTime,
                                                                              const OFBool check)
{
    /* remember current node */
    const DSRDocumentTreeNodeCursor cursor(getCursor());
    /* go to image library entry and check for expected modality */
    OFCondition result = goAndCheckImageLibraryEntry(CODE_DCM_PositronEmissionTomography);
    /* set content item value (and add a new content item if needed) */
    CHECK_RESULT(setStringContentItemFromValue(VT_DateTime, CODE_DCM_RadiopharmaceuticalStartDateTime, dateTime, "TID 1607 - Row 4", check));
    /* in case of error, reset cursor to stored node */
    BAD_RESULT(setCursor(cursor));
    return result;
}


OFCondition TID1600_ImageLibrary::setPETImageRadiopharmaceuticalStopDateTime(const OFString &dateTime,
                                                                             const OFBool check)
{
    /* remember current node */
    const DSRDocumentTreeNodeCursor cursor(getCursor());
    /* go to image library entry and check for expected modality */
    OFCondition result = goAndCheckImageLibraryEntry(CODE_DCM_PositronEmissionTomography);
    /* set content item value (and add a new content item if needed) */
    CHECK_RESULT(setStringContentItemFromValue(VT_DateTime, CODE_DCM_RadiopharmaceuticalStopDateTime, dateTime, "TID 1607 - Row 5", check));
    /* in case of error, reset cursor to stored node */
    BAD_RESULT(setCursor(cursor));
    return result;
}


OFCondition TID1600_ImageLibrary::setPETImageRadiopharmaceuticalVolume(const CMR_SRNumericMeasurementValue &volume,
                                                                       const OFBool check)
{
    /* remember current node */
    const DSRDocumentTreeNodeCursor cursor(getCursor());
    /* go to image library entry and check for expected modality */
    OFCondition result = goAndCheckImageLibraryEntry(CODE_DCM_PositronEmissionTomography);
    /* set content item value (and add a new content item if needed) */
    CHECK_RESULT(setNumericContentItemFromValue(CODE_DCM_RadiopharmaceuticalVolume, volume, CODE_UCUM_cm3 /*might change with a CP*/, "TID 1607 - Row 6", check));
    /* in case of error, reset cursor to stored node */
    BAD_RESULT(setCursor(cursor));
    return result;
}


OFCondition TID1600_ImageLibrary::setPETImageRadionuclideTotalDose(const CMR_SRNumericMeasurementValue &totalDose,
                                                                   const OFBool check)
{
    /* remember current node */
    const DSRDocumentTreeNodeCursor cursor(getCursor());
    /* go to image library entry and check for expected modality */
    OFCondition result = goAndCheckImageLibraryEntry(CODE_DCM_PositronEmissionTomography);
    /* set content item value (and add a new content item if needed) */
    CHECK_RESULT(setNumericContentItemFromValue(CODE_DCM_RadionuclideTotalDose, totalDose, CODE_UCUM_Bq, "TID 1607 - Row 7", check));
    /* in case of error, reset cursor to stored node */
    BAD_RESULT(setCursor(cursor));
    return result;
}


// protected methods

OFCondition TID1600_ImageLibrary::createImageLibrary()
{
    OFCondition result = SR_EC_InvalidTemplateStructure;
    if (isEmpty())
    {
        /* TID 1600 (Image Library) Row 1 */
        STORE_RESULT(addContentItem(RT_unknown, VT_Container, CODE_DCM_ImageLibrary));
        CHECK_RESULT(getCurrentContentItem().setAnnotationText("TID 1600 - Row 1"));
        /* store ID of root node for later use */
        GOOD_RESULT(storeEntryInNodeList(IMAGE_LIBRARY, getNodeID()));
    }
    return result;
}


OFCondition TID1600_ImageLibrary::addImageEntryDescriptors(DSRDocumentSubTree &tree,
                                                           DcmItem &dataset,
                                                           const OFBool check)
{
    OFCondition result = EC_Normal;
    /* TID 1602 (Image Library Entry Descriptors) Row 1 */
    OFString modality;
    if (getStringValueFromDataset(dataset, DCM_Modality, modality).good() && !modality.empty())
    {
        /* determine modality code from CID 29 */
        const CID29e_AcquisitionModality contextGroup;
        DSRCodedEntryValue modalityCode(contextGroup.mapModality(modality));
        if (modalityCode.isComplete())
        {
            CHECK_RESULT(tree.addContentItem(RT_hasAcqContext, VT_Code, CODE_DCM_Modality, check));
            CHECK_RESULT(tree.getCurrentContentItem().setCodeValue(modalityCode, check));
            CHECK_RESULT(tree.getCurrentContentItem().setAnnotationText("TID 1602 - Row 1"));
        } else {
            /* do not treat this as an error, just report a warning */
            DCMSR_CMR_WARN("Cannot map Modality '" << modality << "' to a coded entry (not in CID 29)");
        }
    }
    /* TID 1602 (Image Library Entry Descriptors) Row 2 */
    DSRCodedEntryValue regionCode;
    /* try to get coded entry from code sequence */
    if (regionCode.readSequence(dataset, DCM_AnatomicRegionSequence, "3" /*type*/).bad())
    {
        OFString bodyPartExamined;
        if (getStringValueFromDataset(dataset, DCM_BodyPartExamined, bodyPartExamined).good() && !bodyPartExamined.empty())
        {
            /* alternatively, determine target region code from CID 4031 (using PS 3.16 Annex L) */
            regionCode = CID4031e_CommonAnatomicRegions::mapBodyPartExamined(bodyPartExamined);
            if (!regionCode.isComplete())
            {
                /* report this as a debug message (avoid too many warnings) */
                DCMSR_CMR_DEBUG("Cannot map Body Part Examined '" << bodyPartExamined << "' to a coded entry (no mapping to CID 4031 defined)");
            }
        }
    }
    if (regionCode.isComplete())
    {
        CHECK_RESULT(tree.addContentItem(RT_hasAcqContext, VT_Code, CODE_DCM_TargetRegion, check));
        CHECK_RESULT(tree.getCurrentContentItem().setCodeValue(regionCode, check));
        CHECK_RESULT(tree.getCurrentContentItem().setAnnotationText("TID 1602 - Row 2"));
    }
    /* TID 1602 (Image Library Entry Descriptors) Row 3 */
    OFString imageLaterality;
    if (getStringValueFromDataset(dataset, DCM_ImageLaterality, imageLaterality).good() && !imageLaterality.empty())
    {
        /* determine image laterality code from CID 244 */
        DSRCodedEntryValue lateralityCode(CID244e_Laterality::mapImageLaterality(imageLaterality));
        if (lateralityCode.isComplete())
        {
            CHECK_RESULT(tree.addContentItem(RT_hasAcqContext, VT_Code, CODE_DCM_ImageLaterality, check));
            CHECK_RESULT(tree.getCurrentContentItem().setCodeValue(lateralityCode, check));
            CHECK_RESULT(tree.getCurrentContentItem().setAnnotationText("TID 1602 - Row 3"));
        } else {
            /* do not treat this as an error, just report a warning */
            DCMSR_CMR_WARN("Cannot map Image Laterality '" << imageLaterality << "' to a coded entry (not in CID 244)");
        }
    }
    /* TID 1602 (Image Library Entry Descriptors) Row 4 */
    CHECK_RESULT(addStringContentItemFromDataset(tree, dataset, DCM_StudyDate, 0 /*pos*/, VT_Date, CODE_DCM_StudyDate, "TID 1602 - Row 4", check));
    /* TID 1602 (Image Library Entry Descriptors) Row 5 */
    CHECK_RESULT(addStringContentItemFromDataset(tree, dataset, DCM_StudyTime, 0 /*pos*/, VT_Time, CODE_DCM_StudyTime, "TID 1602 - Row 5", check));
    /* TID 1602 (Image Library Entry Descriptors) Row 6 */
    CHECK_RESULT(addStringContentItemFromDataset(tree, dataset, DCM_ContentDate, 0 /*pos*/, VT_Date, CODE_DCM_ContentDate, "TID 1602 - Row 6", check));
    /* TID 1602 (Image Library Entry Descriptors) Row 7 */
    CHECK_RESULT(addStringContentItemFromDataset(tree, dataset, DCM_ContentTime, 0 /*pos*/, VT_Time, CODE_DCM_ContentTime, "TID 1602 - Row 7", check));
    /* TID 1602 (Image Library Entry Descriptors) Row 8 */
    CHECK_RESULT(addStringContentItemFromDataset(tree, dataset, DCM_AcquisitionDate, 0 /*pos*/, VT_Date, CODE_DCM_AcquisitionDate, "TID 1602 - Row 8", check));
    /* TID 1602 (Image Library Entry Descriptors) Row 9 */
    CHECK_RESULT(addStringContentItemFromDataset(tree, dataset, DCM_AcquisitionTime, 0 /*pos*/, VT_Time, CODE_DCM_AcquisitionTime, "TID 1602 - Row 9", check));
    /* TID 1602 (Image Library Entry Descriptors) Row 10 */
    CHECK_RESULT(addStringContentItemFromDataset(tree, dataset, DCM_FrameOfReferenceUID, 0 /*pos*/, VT_UIDRef, CODE_DCM_FrameOfReferenceUID, "TID 1602 - Row 10", check));
    /* TID 1602 (Image Library Entry Descriptors) Row 11 */
    /* - tbc: what about DCM_TotalPixelMatrixRows (e.g. used for WSI images)? */
    CHECK_RESULT(addNumericContentItemFromDataset(tree, dataset, DCM_Rows, 0 /*pos*/, CODE_DCM_PixelDataRows, CODE_UCUM_Pixels, "TID 1602 - Row 11", check));
    /* TID 1602 (Image Library Entry Descriptors) Row 12 */
    /* - tbc: what about DCM_TotalPixelMatrixColumns (e.g. used for WSI images)? */
    CHECK_RESULT(addNumericContentItemFromDataset(tree, dataset, DCM_Columns, 0 /*pos*/, CODE_DCM_PixelDataColumns, CODE_UCUM_Pixels, "TID 1602 - Row 12", check));
    /* continue with modality-specific descriptors (TID 1603 to 1607) */
    CHECK_RESULT(addModalitySpecificDescriptors(tree, dataset, modality, check));
    return result;
}


OFCondition TID1600_ImageLibrary::addModalitySpecificDescriptors(DSRDocumentSubTree &tree,
                                                                 DcmItem &dataset,
                                                                 const OFString &modality,
                                                                 const OFBool check)
{
    OFCondition result = EC_Normal;
    /* TID 1603 (Image Library Entry Descriptors for Projection Radiography) */
    if ((modality == "CR") || (modality == "RG") || (modality == "DX") || (modality == "MG") || (modality == "XA") || (modality == "RF") || (modality == "PX") || (modality == "IO"))
        CHECK_RESULT(addProjectionRadiographyDescriptors(tree, dataset, check));
    /* TID 1604 (Image Library Entry Descriptors for Cross-Sectional Modalities) */
    if ((modality == "CT") || (modality == "MR") || (modality == "US") /* correct? */ || (modality == "PT") || (modality == "OCT") || (modality == "OPT") || (modality == "IVOCT"))
        CHECK_RESULT(addCrossSectionalModalitiesDescriptors(tree, dataset, check));
    /* TID 1605 (Image Library Entry Descriptors for CT) */
    if (modality == "CT")
        CHECK_RESULT(addComputedTomographyDescriptors(tree, dataset, check));
    /* TID 1606 (Image Library Entry Descriptors for MR) */
    if (modality == "MR")
        CHECK_RESULT(addMagneticResonanceDescriptors(tree, dataset, check));
    /* TID 1607 (Image Library Entry Descriptors for PET) */
    if (modality == "PT")
        CHECK_RESULT(addPositronEmissionTomographyDescriptors(tree, dataset, check));
    return result;
}


OFCondition TID1600_ImageLibrary::addProjectionRadiographyDescriptors(DSRDocumentSubTree &tree,
                                                                      DcmItem &dataset,
                                                                      const OFBool check)
{
    OFCondition result = EC_Normal;
    /* TID 1603 (Image Library Entry Descriptors for Projection Radiography) Row 1 */
    CHECK_RESULT(addCodeContentItemFromDataset(tree, dataset, DCM_ViewCodeSequence, CODE_DCM_ImageView, "TID 1603 - Row 1", check));
    /* TID 1603 (Image Library Entry Descriptors for Projection Radiography) Row 2 */
    if (result.good() && (tree.getCurrentContentItem().getConceptName() == CODE_DCM_ImageView))
    {
        DcmItem *item = NULL;
        /* get view modifiers (if any) */
        if (dataset.findAndGetSequenceItem(DCM_ViewCodeSequence, item, 0 /*itemNum*/).good())
        {
            DcmSequenceOfItems *sequence = NULL;
            if (item->findAndGetSequence(DCM_ViewModifierCodeSequence, sequence).good())
            {
                /* iterate over all sequence items */
                DcmObject *object = NULL;
                while (((object = sequence->nextInContainer(object)) != NULL) && result.good())
                {
                    DSRCodedEntryValue modifierCode;
                    if (modifierCode.readSequenceItem(*OFstatic_cast(DcmItem *, object), DCM_ViewModifierCodeSequence).good())
                    {
                        CHECK_RESULT(tree.addChildContentItem(RT_hasAcqContext, VT_Code, CODE_DCM_ImageViewModifier, check));
                        CHECK_RESULT(tree.getCurrentContentItem().setCodeValue(modifierCode, check));
                        CHECK_RESULT(tree.getCurrentContentItem().setAnnotationText("TID 1603 - Row 2"));
                        tree.goUp();
                    }
                }
            }
        }
    }
    /* TID 1603 (Image Library Entry Descriptors for Projection Radiography) Row 3 */
    CHECK_RESULT(addStringContentItemFromDataset(tree, dataset, DCM_PatientOrientation, 0 /*pos*/, VT_Text, CODE_DCM_PatientOrientationRow, "TID 1603 - Row 3", check));
    /* TID 1603 (Image Library Entry Descriptors for Projection Radiography) Row 4 */
    CHECK_RESULT(addStringContentItemFromDataset(tree, dataset, DCM_PatientOrientation, 1 /*pos*/, VT_Text, CODE_DCM_PatientOrientationColumn, "TID 1603 - Row 4", check));
    /* TID 1603 (Image Library Entry Descriptors for Projection Radiography) Row 5 */
    CHECK_RESULT(addNumericContentItemFromDataset(tree, dataset, DCM_ImagerPixelSpacing, 1 /*pos*/, CODE_DCM_HorizontalPixelSpacing, CODE_UCUM_Millimeter, "TID 1603 - Row 5", check));
    /* TID 1603 (Image Library Entry Descriptors for Projection Radiography) Row 6 */
    CHECK_RESULT(addNumericContentItemFromDataset(tree, dataset, DCM_ImagerPixelSpacing, 0 /*pos*/, CODE_DCM_VerticalPixelSpacing, CODE_UCUM_Millimeter, "TID 1603 - Row 6", check));
    /* TID 1603 (Image Library Entry Descriptors for Projection Radiography) Row 7 */
    CHECK_RESULT(addNumericContentItemFromDataset(tree, dataset, DCM_PositionerPrimaryAngle, 0 /*pos*/, CODE_DCM_PositionerPrimaryAngle, CODE_UCUM_Degrees, "TID 1603 - Row 7", check));
    /* TID 1603 (Image Library Entry Descriptors for Projection Radiography) Row 8 */
    CHECK_RESULT(addNumericContentItemFromDataset(tree, dataset, DCM_PositionerSecondaryAngle, 0 /*pos*/, CODE_DCM_PositionerSecondaryAngle, CODE_UCUM_Degrees, "TID 1603 - Row 8", check));
    return result;
}


OFCondition TID1600_ImageLibrary::addCrossSectionalModalitiesDescriptors(DSRDocumentSubTree &tree,
                                                                         DcmItem &dataset,
                                                                         const OFBool check)
{
    OFCondition result = EC_Normal;
    /* TID 1604 (Image Library Entry Descriptors for Cross-Sectional Modalities) Row 1 */
    CHECK_RESULT(addNumericContentItemFromDataset(tree, dataset, DCM_PixelSpacing, 1 /*pos*/, CODE_DCM_HorizontalPixelSpacing, CODE_UCUM_Millimeter, "TID 1604 - Row 1", check));
    /* TID 1604 (Image Library Entry Descriptors for Cross-Sectional Modalities) Row 2 */
    CHECK_RESULT(addNumericContentItemFromDataset(tree, dataset, DCM_PixelSpacing, 0 /*pos*/, CODE_DCM_VerticalPixelSpacing, CODE_UCUM_Millimeter, "TID 1604 - Row 2", check));
    /* TID 1604 (Image Library Entry Descriptors for Cross-Sectional Modalities) Row 3 */
    CHECK_RESULT(addNumericContentItemFromDataset(tree, dataset, DCM_SpacingBetweenSlices, 0 /*pos*/, CODE_DCM_SpacingBetweenSlices, CODE_UCUM_Millimeter, "TID 1604 - Row 3", check));
    /* TID 1604 (Image Library Entry Descriptors for Cross-Sectional Modalities) Row 4 */
    CHECK_RESULT(addNumericContentItemFromDataset(tree, dataset, DCM_SliceThickness, 0 /*pos*/, CODE_DCM_SliceThickness, CODE_UCUM_Millimeter, "TID 1604 - Row 4", check));
    /* TID 1604 (Image Library Entry Descriptors for Cross-Sectional Modalities) Row 5 */
    CHECK_RESULT(addNumericContentItemFromDataset(tree, dataset, DCM_ImagePositionPatient, 0 /*pos*/, CODE_DCM_ImagePosition_Patient_X, CODE_UCUM_Millimeter, "TID 1604 - Row 5", check));
    /* TID 1604 (Image Library Entry Descriptors for Cross-Sectional Modalities) Row 6 */
    CHECK_RESULT(addNumericContentItemFromDataset(tree, dataset, DCM_ImagePositionPatient, 1 /*pos*/, CODE_DCM_ImagePosition_Patient_Y, CODE_UCUM_Millimeter, "TID 1604 - Row 6", check));
    /* TID 1604 (Image Library Entry Descriptors for Cross-Sectional Modalities) Row 7 */
    CHECK_RESULT(addNumericContentItemFromDataset(tree, dataset, DCM_ImagePositionPatient, 2 /*pos*/, CODE_DCM_ImagePosition_Patient_Z, CODE_UCUM_Millimeter, "TID 1604 - Row 7", check));
    /* TID 1604 (Image Library Entry Descriptors for Cross-Sectional Modalities) Row 8 */
    CHECK_RESULT(addNumericContentItemFromDataset(tree, dataset, DCM_ImageOrientationPatient, 0 /*pos*/, CODE_DCM_ImageOrientation_Patient_RowX, CODE_UCUM_Minus1To1, "TID 1604 - Row 8", check));
    /* TID 1604 (Image Library Entry Descriptors for Cross-Sectional Modalities) Row 9 */
    CHECK_RESULT(addNumericContentItemFromDataset(tree, dataset, DCM_ImageOrientationPatient, 1 /*pos*/, CODE_DCM_ImageOrientation_Patient_RowY, CODE_UCUM_Minus1To1, "TID 1604 - Row 9", check));
    /* TID 1604 (Image Library Entry Descriptors for Cross-Sectional Modalities) Row 10 */
    CHECK_RESULT(addNumericContentItemFromDataset(tree, dataset, DCM_ImageOrientationPatient, 2 /*pos*/, CODE_DCM_ImageOrientation_Patient_RowZ, CODE_UCUM_Minus1To1, "TID 1604 - Row 10", check));
    /* TID 1604 (Image Library Entry Descriptors for Cross-Sectional Modalities) Row 11 */
    CHECK_RESULT(addNumericContentItemFromDataset(tree, dataset, DCM_ImageOrientationPatient, 3 /*pos*/, CODE_DCM_ImageOrientation_Patient_ColumnX, CODE_UCUM_Minus1To1, "TID 1604 - Row 11", check));
    /* TID 1604 (Image Library Entry Descriptors for Cross-Sectional Modalities) Row 12 */
    CHECK_RESULT(addNumericContentItemFromDataset(tree, dataset, DCM_ImageOrientationPatient, 4 /*pos*/, CODE_DCM_ImageOrientation_Patient_ColumnY, CODE_UCUM_Minus1To1, "TID 1604 - Row 12", check));
    /* TID 1604 (Image Library Entry Descriptors for Cross-Sectional Modalities) Row 13 */
    CHECK_RESULT(addNumericContentItemFromDataset(tree, dataset, DCM_ImageOrientationPatient, 5 /*pos*/, CODE_DCM_ImageOrientation_Patient_ColumnZ, CODE_UCUM_Minus1To1, "TID 1604 - Row 13", check));
    return result;
}


OFCondition TID1600_ImageLibrary::addComputedTomographyDescriptors(DSRDocumentSubTree &tree,
                                                                   DcmItem &dataset,
                                                                   const OFBool check)
{
    OFCondition result = EC_Normal;
    /* TID 1605 (Image Library Entry Descriptors for CT) Row 1 */
    DcmSequenceOfItems *ctAcquisitionTypeSequence = NULL;
    /* - tbd: only check in functional groups sequences? might use "dcmfg" for this purpose */
    if (dataset.findAndGetSequence(DCM_CTAcquisitionTypeSequence, ctAcquisitionTypeSequence, OFTrue /*searchIntoSub*/).good())
    {
        DcmItem *item = ctAcquisitionTypeSequence->getItem(0);
        if (item != NULL)
        {
            OFString acquisitionType;
            if (getStringValueFromDataset(*item, DCM_AcquisitionType, acquisitionType).good() && !acquisitionType.empty())
            {
                /* determine CT acquisition type code from CID 10013 */
                DSRCodedEntryValue acquisitionTypeCode(CID10013e_CTAcquisitionType::mapAcquisitionType(acquisitionType));
                if (acquisitionTypeCode.isComplete())
                {
                    CHECK_RESULT(tree.addContentItem(RT_hasAcqContext, VT_Code, CODE_DCM_CTAcquisitionType, check));
                    CHECK_RESULT(tree.getCurrentContentItem().setCodeValue(acquisitionTypeCode, check));
                    CHECK_RESULT(tree.getCurrentContentItem().setAnnotationText("TID 1605 - Row 1"));
                } else {
                    /* do not treat this as an error, just report a warning */
                    DCMSR_CMR_WARN("Cannot map Acquisition Type '" << acquisitionType << "' to a coded entry (not in CID 10013)");
                }
            }
        }
    }
    /* TID 1605 (Image Library Entry Descriptors for CT) Row 2 */
    DcmSequenceOfItems *ctReconstructionSequence = NULL;
    /* - tbd: only check in functional groups sequences? might use "dcmfg" for this purpose */
    if (dataset.findAndGetSequence(DCM_CTReconstructionSequence, ctReconstructionSequence, OFTrue /*searchIntoSub*/).good())
    {
        DcmItem *item = ctReconstructionSequence->getItem(0);
        if (item != NULL)
        {
            OFString reconstructionAlgorithm;
            if (getStringValueFromDataset(*item, DCM_ReconstructionAlgorithm, reconstructionAlgorithm).good() && !reconstructionAlgorithm.empty())
            {
                /* determine CT reconstruction algorithm code from CID 10033 */
                DSRCodedEntryValue reconstructionAlgorithmCode(CID10033e_CTReconstructionAlgorithm::mapReconstructionAlgorithm(reconstructionAlgorithm));
                if (reconstructionAlgorithmCode.isComplete())
                {
                    CHECK_RESULT(tree.addContentItem(RT_hasAcqContext, VT_Code, CODE_DCM_ReconstructionAlgorithm, check));
                    CHECK_RESULT(tree.getCurrentContentItem().setCodeValue(reconstructionAlgorithmCode, check));
                    CHECK_RESULT(tree.getCurrentContentItem().setAnnotationText("TID 1605 - Row 2"));
                } else {
                    /* do not treat this as an error, just report a warning */
                    DCMSR_CMR_WARN("Cannot map Reconstruction Algorithm '" << reconstructionAlgorithm << "' to a coded entry (not in CID 10033)");
                }
            }
        }
    }
    return result;
}


OFCondition TID1600_ImageLibrary::addMagneticResonanceDescriptors(DSRDocumentSubTree &tree,
                                                                  DcmItem &dataset,
                                                                  const OFBool check)
{
    OFCondition result = EC_Normal;
    /* TID 1606 (Image Library Entry Descriptors for MR) Row 1 */
    OFString sequenceName;
    /* get one of two alternative elements values */
    if ((getStringValueFromDataset(dataset, DCM_PulseSequenceName, sequenceName).good() && !sequenceName.empty()) ||
        (getStringValueFromDataset(dataset, DCM_SequenceName, sequenceName).good() && !sequenceName.empty()))
    {
        CHECK_RESULT(tree.addContentItem(RT_hasAcqContext, VT_Text, CODE_DCM_PulseSequenceName, check));
        CHECK_RESULT(tree.getCurrentContentItem().setStringValue(sequenceName, check));
        CHECK_RESULT(tree.getCurrentContentItem().setAnnotationText("TID 1606 - Row 1"));
    }
    return result;
}


OFCondition TID1600_ImageLibrary::addPositronEmissionTomographyDescriptors(DSRDocumentSubTree &tree,
                                                                           DcmItem &dataset,
                                                                           const OFBool check)
{
    OFCondition result = EC_Normal;
    /* get main sequence from (Enhanced) PET Isotope Module */
    DcmSequenceOfItems *radiopharmaceuticalInformationSequence = NULL;
    if (dataset.findAndGetSequence(DCM_RadiopharmaceuticalInformationSequence, radiopharmaceuticalInformationSequence, OFTrue /*searchIntoSub*/).good())
    {
        /* tbd: sequence may contain multiple items! */
        DcmItem *item = radiopharmaceuticalInformationSequence->getItem(0);
        if (item != NULL)
        {
            /* TID 1607 (Image Library Entry Descriptors for PET) Row 1 */
            CHECK_RESULT(addCodeContentItemFromDataset(tree, *item, DCM_RadionuclideCodeSequence, CODE_SRT_Radionuclide, "TID 1607 - Row 1", check));
            /* TID 1607 (Image Library Entry Descriptors for PET) Row 2 */
            CHECK_RESULT(addCodeContentItemFromDataset(tree, *item, DCM_RadiopharmaceuticalCodeSequence, CODE_SRT_RadiopharmaceuticalAgent, "TID 1607 - Row 2", check));
            /* TID 1607 (Image Library Entry Descriptors for PET) Row 3 */
            CHECK_RESULT(addNumericContentItemFromDataset(tree, *item, DCM_RadionuclideHalfLife, 0 /*pos*/, CODE_SRT_HalfLifeOfRadiopharmaceutical, CODE_UCUM_s, "TID 1607 - Row 3", check));
            /* TID 1607 (Image Library Entry Descriptors for PET) Row 4 */
            CHECK_RESULT(addStringContentItemFromDataset(tree, *item, DCM_RadiopharmaceuticalStartDateTime, 0 /*pos*/, VT_DateTime, CODE_DCM_RadiopharmaceuticalStartDateTime, "TID 1607 - Row 4", check));
            /* TID 1607 (Image Library Entry Descriptors for PET) Row 5 */
            CHECK_RESULT(addStringContentItemFromDataset(tree, *item, DCM_RadiopharmaceuticalStopDateTime, 0 /*pos*/, VT_DateTime, CODE_DCM_RadiopharmaceuticalStopDateTime, "TID 1607 - Row 5", check));
            /* TID 1607 (Image Library Entry Descriptors for PET) Row 6 */
            CHECK_RESULT(addNumericContentItemFromDataset(tree, *item, DCM_RadiopharmaceuticalVolume, 0 /*pos*/, CODE_DCM_RadiopharmaceuticalVolume, CODE_UCUM_cm3 /*might change with a CP*/, "TID 1607 - Row 6", check));
            /* TID 1607 (Image Library Entry Descriptors for PET) Row 7 */
            CHECK_RESULT(addNumericContentItemFromDataset(tree, *item, DCM_RadionuclideTotalDose, 0 /*pos*/, CODE_DCM_RadionuclideTotalDose, CODE_UCUM_Bq, "TID 1607 - Row 7", check));
            /* TID 1607 (Image Library Entry Descriptors for PET) Row 8 */
            CHECK_RESULT(addNumericContentItemFromDataset(tree, *item, DCM_RadiopharmaceuticalSpecificActivity, 0 /*pos*/, CODE_DCM_RadiopharmaceuticalSpecificActivity, CODE_UCUM_BqPerMol, "TID 1607 - Row 8", check));
            /* TID 1607 (Image Library Entry Descriptors for PET) Row 9 */
            CHECK_RESULT(addCodeContentItemFromDataset(tree, *item, DCM_AdministrationRouteCodeSequence, CODE_SRT_RouteOfAdministration, "TID 1607 - Row 9", check));
        }
    }
    /* TID 1607 (Image Library Entry Descriptors for PET) Row 10 to 11
     * - contained in TID 15101 (NM/PET Protocol Context), i.e. not available in the image
     */
    /* TID 1607 (Image Library Entry Descriptors for PET) Row 12
     * - "Radionuclide Incubation Time" calculated from time of injection and time of starting the scan
     */
    /* TID 1607 (Image Library Entry Descriptors for PET) Row 13 to 15
     * - contained in TID 15101 (NM/PET Protocol Context), i.e. available from Modality Worklist, or
     * - tbd: in TID 3470 (NM/PET Acquisition Context), i.e. from the Acquisition Context Module
     */
    return result;
}


OFCondition TID1600_ImageLibrary::goAndCheckImageLibraryEntry(const DSRCodedEntryValue &modalityCode)
{
    OFCondition result = CMR_EC_NoImageLibraryEntry;
    /* go to last image library entry (if any) */
    if (gotoEntryFromNodeList(this, LAST_IMAGE_LIBRARY_ENTRY) > 0)
    {
        /* check whether TID 1602 (Image Library Entry Descriptors) Row 1 has correct value */
        if ((gotoNamedChildNode(CODE_DCM_Modality, OFFalse /*searchIntoSub*/) > 0) &&
            (getCurrentContentItem().getCodeValue() == modalityCode))
        {
            result = EC_Normal;
        } else
            result = CMR_EC_WrongImageLibraryEntryDescriptorModality;
    }
    return result;
}


OFCondition TID1600_ImageLibrary::setStringContentItemFromValue(const E_ValueType valueType,
                                                                const DSRCodedEntryValue &conceptName,
                                                                const OFString &stringValue,
                                                                const OFString &annotationText,
                                                                const OFBool check)
{
    OFCondition result = EC_Normal;
    /* check concept name and coded entry value */
    if (conceptName.isComplete())
    {
        if (!stringValue.empty())
        {
            /* check whether content item is already present.
             * (we assume that the content item we are searching for is a successor of the current one)
             */
            if (gotoNextNamedNode(conceptName, OFFalse /*searchIntoSub*/) == 0)
            {
                CHECK_RESULT(addContentItem(RT_hasAcqContext, valueType, conceptName, check));
            } else {
                /* make sure that the value type of the existing content item is correct */
                if (getCurrentContentItem().getValueType() == valueType)
                {
                    DCMSR_CMR_DEBUG("Replacing value of '" << conceptName.getCodeMeaning()
                        << "' content item (" << annotationText << ") with \"" << stringValue << "\"");
                } else {
                    DCMSR_CMR_WARN("Cannot replace value of '" << conceptName.getCodeMeaning()
                        << "' content item (" << annotationText << ") ... wrong value type");
                    result = SR_EC_InvalidContentItem;
                }
            }
            CHECK_RESULT(getCurrentContentItem().setStringValue(stringValue, check));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText(annotationText));
        } else
            result = SR_EC_InvalidValue;
    } else
        result = SR_EC_InvalidConceptName;
    return result;
}


OFCondition TID1600_ImageLibrary::setCodeContentItemFromValue(const DSRCodedEntryValue &conceptName,
                                                              const DSRCodedEntryValue &codeValue,
                                                              const OFString &annotationText,
                                                              const OFBool check)
{
    OFCondition result = EC_Normal;
    /* check concept name and coded entry value */
    if (conceptName.isComplete())
    {
        if (codeValue.isComplete())
        {
            /* check whether content item is already present.
             * (we assume that the content item we are searching for is a successor of the current one)
             */
            if (gotoNextNamedNode(conceptName, OFFalse /*searchIntoSub*/) == 0)
            {
                CHECK_RESULT(addContentItem(RT_hasAcqContext, VT_Code, conceptName, check));
            } else {
                /* make sure that the value type of the existing content item is correct */
                if (getCurrentContentItem().getValueType() == VT_Code)
                {
                    DCMSR_CMR_DEBUG("Replacing value of '" << conceptName.getCodeMeaning()
                        << "' content item (" << annotationText << ") with " << codeValue);
                } else {
                    DCMSR_CMR_WARN("Cannot replace value of '" << conceptName.getCodeMeaning()
                        << "' content item (" << annotationText << ") ... wrong value type");
                    result = SR_EC_InvalidContentItem;
                }
            }
            CHECK_RESULT(getCurrentContentItem().setCodeValue(codeValue, check));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText(annotationText));
        } else
            result = SR_EC_InvalidValue;
    } else
        result = SR_EC_InvalidConceptName;
    return result;
}


OFCondition TID1600_ImageLibrary::setNumericContentItemFromValue(const DSRCodedEntryValue &conceptName,
                                                                 const DSRNumericMeasurementValue &numericValue,
                                                                 const DSRCodedEntryValue &measurementUnit,
                                                                 const OFString &annotationText,
                                                                 const OFBool check)
{
    OFCondition result = EC_Normal;
    /* check concept name and coded entry value */
    if (conceptName.isComplete())
    {
        if (numericValue.isComplete())
        {
            /* check whether content item is already present.
             * (we assume that the content item we are searching for is a successor of the current one)
             */
            if (gotoNextNamedNode(conceptName, OFFalse /*searchIntoSub*/) == 0)
            {
                CHECK_RESULT(addContentItem(RT_hasAcqContext, VT_Num, conceptName, check));
            } else {
                /* make sure that the value type of the existing content item is correct */
                if (getCurrentContentItem().getValueType() == VT_Num)
                {
                    DCMSR_CMR_DEBUG("Replacing value of '" << conceptName.getCodeMeaning()
                        << "' content item (" << annotationText << ") with " << numericValue);
                } else {
                    DCMSR_CMR_WARN("Cannot replace value of '" << conceptName.getCodeMeaning()
                        << "' content item (" << annotationText << ") ... wrong value type");
                    result = SR_EC_InvalidContentItem;
                }
            }
            /* check whether measurement unit meets the template constraint */
            if (result.good() && !measurementUnit.isEmpty() && (numericValue.getMeasurementUnit() != measurementUnit))
            {
                DCMSR_CMR_WARN("Wrong measurement unit for '" << conceptName.getCodeMeaning()
                    << "' content item (" << annotationText << ")");
                result = SR_EC_ValueSetConstraintViolated;
            }
            CHECK_RESULT(getCurrentContentItem().setNumericValue(numericValue, check));
            CHECK_RESULT(getCurrentContentItem().setAnnotationText(annotationText));
        } else
            result = SR_EC_InvalidValue;
    } else
        result = SR_EC_InvalidConceptName;
    return result;
}


// static helper functions

OFCondition TID1600_ImageLibrary::addStringContentItemFromDataset(DSRDocumentSubTree &tree,
                                                                  DcmItem &dataset,
                                                                  const DcmTagKey &tagKey,
                                                                  const signed long pos,
                                                                  const E_ValueType valueType,
                                                                  const DSRCodedEntryValue &conceptName,
                                                                  const OFString &annotationText,
                                                                  const OFBool check)
{
    OFCondition result = EC_Normal;
    OFString stringValue;
    /* get element value from dataset (textual content only) */
    if (getStringValueFromDataset(dataset, tagKey, stringValue, pos).good() && !stringValue.empty())
    {
        /* create new content item, set concept name and value */
        CHECK_RESULT(tree.addContentItem(RT_hasAcqContext, valueType, conceptName, check));
        CHECK_RESULT(tree.getCurrentContentItem().setStringValue(stringValue, check));
        if (!annotationText.empty())
            CHECK_RESULT(tree.getCurrentContentItem().setAnnotationText(annotationText));
    }
    return result;
}


OFCondition TID1600_ImageLibrary::addCodeContentItemFromDataset(DSRDocumentSubTree &tree,
                                                                DcmItem &dataset,
                                                                const DcmTagKey &tagKey,
                                                                const DSRCodedEntryValue &conceptName,
                                                                const OFString &annotationText,
                                                                const OFBool check)
{
    OFCondition result = EC_Normal;
    DSRCodedEntryValue codedEntry;
    /* get coded entry from code sequence in dataset */
    if (codedEntry.readSequence(dataset, tagKey, "3" /*type*/).good() && codedEntry.isComplete())
    {
        CHECK_RESULT(tree.addContentItem(RT_hasAcqContext, VT_Code, conceptName, check));
        CHECK_RESULT(tree.getCurrentContentItem().setCodeValue(codedEntry, check));
        if (!annotationText.empty())
            CHECK_RESULT(tree.getCurrentContentItem().setAnnotationText(annotationText));
    }
    return result;
}


OFCondition TID1600_ImageLibrary::addNumericContentItemFromDataset(DSRDocumentSubTree &tree,
                                                                   DcmItem &dataset,
                                                                   const DcmTagKey &tagKey,
                                                                   const signed long pos,
                                                                   const DSRCodedEntryValue &conceptName,
                                                                   const DSRCodedEntryValue &measurementUnit,
                                                                   const OFString &annotationText,
                                                                   const OFBool check)
{
    OFCondition result = EC_Normal;
    OFString numericValue;
    /* get element value from dataset (in text format) */
    if (getStringValueFromDataset(dataset, tagKey, numericValue, pos).good() && !numericValue.empty())
    {
        /* create new content item, set concept name and value */
        CHECK_RESULT(tree.addContentItem(RT_hasAcqContext, VT_Num, conceptName, check));
        CHECK_RESULT(tree.getCurrentContentItem().setNumericValue(DSRNumericMeasurementValue(numericValue, measurementUnit), check));
        if (!annotationText.empty())
            CHECK_RESULT(tree.getCurrentContentItem().setAnnotationText(annotationText));
    }
    return result;
}
