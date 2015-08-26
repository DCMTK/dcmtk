/*
 *
 *  Copyright (C) 2015, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Source file for class TID1001_ObservationContext
 *
 *  Author: Joerg Riesmeier
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/tid1001.h"
#include "dcmtk/dcmsr/codes/dcm.h"


// helper macros for checking the return value of API calls
#define CHECK_RESULT(call) if (result.good()) result = call
#define STORE_RESULT(call) result = call
#define DELETE_ERROR(pointer) if (result.bad()) delete pointer

// index positions in node list (makes source code more readable)
#define LAST_PERSON_OBSERVER 0
#define LAST_DEVICE_OBSERVER 1

// general information on TID 1001 (Observation Context)
#define TEMPLATE_NUMBER      "1001"
#define MAPPING_RESOURCE     "DCMR"
#define MAPPING_RESOURCE_UID UID_DICOMContentMappingResource
#define TEMPLATE_TYPE        OFFalse  /* non-extensible, but included templates are */


TID1001_ObservationContext::TID1001_ObservationContext()
  : DSRSubTemplate(TEMPLATE_NUMBER, MAPPING_RESOURCE, MAPPING_RESOURCE_UID)
{
    setExtensible(TEMPLATE_TYPE);
    /* need to store last person and device observer */
    reserveEntriesInNodeList(2);
}


OFCondition TID1001_ObservationContext::addPersonObserver(const OFString &personName,
                                                          const OFString &organizationName,
                                                          const CID7452_OrganizationalRoles &organizationRole,
                                                          const CID7453_PerformingRoles &procedureRole,
                                                          const OFBool check)
{
    OFCondition result = EC_MemoryExhausted;
    /* create a new subtree in order to "rollback" in case of error */
    DSRDocumentSubTree *subTree = new DSRDocumentSubTree;
    if (subTree != NULL)
    {
        /* TID 1002 (Observer Context) Row 1 */
        STORE_RESULT(subTree->addContentItem(RT_hasObsContext, VT_Code, CODE_DCM_ObserverType));
        CHECK_RESULT(subTree->getCurrentContentItem().setCodeValue(CODE_DCM_Person, check));
        CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1002 - Row 1"));
        /* TID 1003 (Person Observer Identifying Attributes) Row 1 */
        CHECK_RESULT(subTree->addContentItem(RT_hasObsContext, VT_PName, CODE_DCM_PersonObserverName));
        CHECK_RESULT(subTree->getCurrentContentItem().setStringValue(personName, check));
        CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1003 - Row 1"));
        /* TID 1003 (Person Observer Identifying Attributes) Row 2 */
        if (!organizationName.empty())
        {
            CHECK_RESULT(subTree->addContentItem(RT_hasObsContext, VT_Text, CODE_DCM_PersonObserverOrganizationName));
            CHECK_RESULT(subTree->getCurrentContentItem().setStringValue(organizationName, check));
            CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1003 - Row 2"));
        }
        /* TID 1003 (Person Observer Identifying Attributes) Row 3 */
        if (organizationRole.hasSelectedValue())
        {
            CHECK_RESULT(subTree->addContentItem(RT_hasObsContext, VT_Code, CODE_DCM_PersonObserverRoleInTheOrganization));
            CHECK_RESULT(subTree->getCurrentContentItem().setCodeValue(organizationRole.getSelectedValue(), check));
            CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1003 - Row 3"));
        }
        /* TID 1003 (Person Observer Identifying Attributes) Row 4 */
        if (procedureRole.hasSelectedValue())
        {
            CHECK_RESULT(subTree->addContentItem(RT_hasObsContext, VT_Code, CODE_DCM_PersonObserverRoleInThisProcedure));
            CHECK_RESULT(subTree->getCurrentContentItem().setCodeValue(procedureRole.getSelectedValue(), check));
            CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1003 - Row 4"));
        }
        /* if everything was OK, insert new subtree into the template */
        if (result.good())
        {
            const size_t lastNode = subTree->getNodeID();
            /* order is significant, so go to last person observer */
            gotoLastEntryFromNodeList(this, LAST_PERSON_OBSERVER);
            /* insert subtree at current position */
            result = insertSubTree(subTree, AM_afterCurrent);
            /* store ID of recently added node for later use */
            if (result.good())
                storeEntryInNodeList(LAST_PERSON_OBSERVER, lastNode);
        }
        /* in case of error, make sure that memory is freed */
        DELETE_ERROR(subTree);
    }
    return result;
}


OFCondition TID1001_ObservationContext::addDeviceObserver(const OFString &deviceUID,
                                                          const OFString &deviceName,
                                                          const OFString &manufacturer,
                                                          const OFString &modelName,
                                                          const OFString &serialNumber,
                                                          const OFString &physicalLocation,
                                                          const DeviceParticipatingRolesList &procedureRoles,
                                                          const OFBool check)
{
    OFCondition result = EC_MemoryExhausted;
    /* create a new subtree in order to "rollback" in case of error */
    DSRDocumentSubTree *subTree = new DSRDocumentSubTree;
    if (subTree != NULL)
    {
        /* TID 1002 (Observer Context) Row 1 */
        STORE_RESULT(subTree->addContentItem(RT_hasObsContext, VT_Code, CODE_DCM_ObserverType));
        CHECK_RESULT(subTree->getCurrentContentItem().setCodeValue(CODE_DCM_Device, check));
        CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1002 - Row 1"));
        /* TID 1004 (Device Observer Identifying Attributes) Row 1 */
        CHECK_RESULT(subTree->addContentItem(RT_hasObsContext, VT_UIDRef, CODE_DCM_DeviceObserverUID));
        CHECK_RESULT(subTree->getCurrentContentItem().setStringValue(deviceUID, check));
        CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1004 - Row 1"));
        /* TID 1004 (Device Observer Identifying Attributes) Row 2 */
        if (!deviceName.empty())
        {
            CHECK_RESULT(subTree->addContentItem(RT_hasObsContext, VT_Text, CODE_DCM_DeviceObserverName));
            CHECK_RESULT(subTree->getCurrentContentItem().setStringValue(deviceName, check));
            CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1004 - Row 2"));
        }
        /* TID 1004 (Device Observer Identifying Attributes) Row 3 */
        if (!manufacturer.empty())
        {
            CHECK_RESULT(subTree->addContentItem(RT_hasObsContext, VT_Text, CODE_DCM_DeviceObserverManufacturer));
            CHECK_RESULT(subTree->getCurrentContentItem().setStringValue(manufacturer, check));
            CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1004 - Row 3"));
        }
        /* TID 1004 (Device Observer Identifying Attributes) Row 4 */
        if (!modelName.empty())
        {
            CHECK_RESULT(subTree->addContentItem(RT_hasObsContext, VT_Text, CODE_DCM_DeviceObserverModelName));
            CHECK_RESULT(subTree->getCurrentContentItem().setStringValue(modelName, check));
            CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1004 - Row 4"));
        }
        /* TID 1004 (Device Observer Identifying Attributes) Row 5 */
        if (!serialNumber.empty())
        {
            CHECK_RESULT(subTree->addContentItem(RT_hasObsContext, VT_Text, CODE_DCM_DeviceObserverSerialNumber));
            CHECK_RESULT(subTree->getCurrentContentItem().setStringValue(serialNumber, check));
            CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1004 - Row 5"));
        }
        /* TID 1004 (Device Observer Identifying Attributes) Row 6 */
        if (!physicalLocation.empty())
        {
            CHECK_RESULT(subTree->addContentItem(RT_hasObsContext, VT_Text, CODE_DCM_DeviceObserverPhysicalLocationDuringObservation));
            CHECK_RESULT(subTree->getCurrentContentItem().setStringValue(physicalLocation, check));
            CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1004 - Row 6"));
        }
        /* TID 1004 (Device Observer Identifying Attributes) Row 7 */
        if (!procedureRoles.empty())
        {
            OFListConstIterator(CID7445_DeviceParticipatingRoles) iter = procedureRoles.begin();
            const OFListConstIterator(CID7445_DeviceParticipatingRoles) last = procedureRoles.end();
            /* iterate over all list entries */
            while ((iter != last) && result.good())
            {
                STORE_RESULT(subTree->addContentItem(RT_hasObsContext, VT_Code, CODE_DCM_DeviceRoleInProcedure));
                CHECK_RESULT(subTree->getCurrentContentItem().setCodeValue(iter->getSelectedValue(), check));
                CHECK_RESULT(subTree->getCurrentContentItem().setAnnotationText("TID 1004 - Row 7"));
                ++iter;
            }
        }
        /* if everything was OK, insert new subtree into the template */
        if (result.good())
        {
            const size_t lastNode = subTree->getNodeID();
            /* order is significant, so go to last device (or person) observer */
            gotoLastEntryFromNodeList(this, LAST_DEVICE_OBSERVER);
            /* insert subtree at current position */
            result = insertSubTree(subTree, AM_afterCurrent);
            /* store ID of recently added node for later use */
            if (result.good())
                storeEntryInNodeList(LAST_DEVICE_OBSERVER, lastNode);
        }
        /* in case of error, make sure that memory is freed */
        DELETE_ERROR(subTree);
}
    return result;
}


OFCondition TID1001_ObservationContext::addProcedureContext()
{
    /* tbd: not yet implemented */
    return EC_NotYetImplemented;
}


OFCondition TID1001_ObservationContext::addSubjectContext()
{
    /* tbd: not yet implemented */
    return EC_NotYetImplemented;
}
