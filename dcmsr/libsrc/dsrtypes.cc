/*
 *
 *  Copyright (C) 2000, OFFIS
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
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRTypes
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-18 17:23:58 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dsrtypes.h"
#include "dsrtextn.h"
#include "dsrcodtn.h"
#include "dsrnumtn.h"
#include "dsrdtitn.h"
#include "dsrdattn.h"
#include "dsrtimtn.h"
#include "dsruidtn.h"
#include "dsrpnmtn.h"
#include "dsrscotn.h"
#include "dsrcomtn.h"
#include "dsrimgtn.h"
#include "dsrwavtn.h"
#include "dsrcontn.h"

BEGIN_EXTERN_C
#ifdef HAVE_TIME_H
#include <time.h>
#endif
END_EXTERN_C


/*---------------------------------*
 *  constant definitions (part 1)  *
 *---------------------------------*/

/* renderHTML flags */
const size_t DSRTypes::HF_renderItemsSeparately    =  1;
const size_t DSRTypes::HF_renderItemInline         =  2;
const size_t DSRTypes::HF_currentlyInsideAnnex     =  4;
const size_t DSRTypes::HF_createFootnoteReferences =  8;
const size_t DSRTypes::HF_neverExpandChildsInline  = 16;
const size_t DSRTypes::HF_renderConceptNameCodes   = 32;
const size_t DSRTypes::HF_renderFullData           = 64;
const size_t DSRTypes::HF_renderAllCodes           = DSRTypes::HF_renderConceptNameCodes;
const size_t DSRTypes::HF_internalUseOnly          = DSRTypes::HF_renderItemsSeparately | DSRTypes::HF_renderItemInline |
                                                     DSRTypes::HF_currentlyInsideAnnex | DSRTypes::HF_createFootnoteReferences;

/* print flags */
const size_t DSRTypes::PF_printItemPosition        = 1;
const size_t DSRTypes::PF_shortenLongItemValues    = 2;
const size_t DSRTypes::PF_printSOPInstanceUID      = 4;
const size_t DSRTypes::PF_printConceptNameCodes    = 8;
const size_t DSRTypes::PF_printAllCodes            = DSRTypes::PF_printConceptNameCodes;


/*---------------------*
 *  type declarations  *
 *---------------------*/

struct S_DocumentTypeNameMap
{
    DSRTypes::E_DocumentType Type;
    const char *SOPClassUID;
    const char *ReadableName;
};


struct S_RelationshipTypeNameMap
{
    DSRTypes::E_RelationshipType Type;
    const char *DefinedTerm;
    const char *ReadableName;
};


struct S_ValueTypeNameMap
{
    DSRTypes::E_ValueType Type;
    const char *DefinedTerm;
    const char *ReadableName;
};


struct S_GraphicTypeNameMap
{
    DSRTypes::E_GraphicType Type;
    const char *EnumeratedValue;
    const char *ReadableName;
};


struct S_ContinuityOfContentNameMap
{
    DSRTypes::E_ContinuityOfContent Type;
    const char *EnumeratedValue;
};


struct S_CompletionFlagNameMap
{
    DSRTypes::E_CompletionFlag Type;
    const char *EnumeratedValue;
};


struct S_VerificationFlagNameMap
{
    DSRTypes::E_VerificationFlag Type;
    const char *EnumeratedValue;
};


/*---------------------------------*
 *  constant definitions (part 2)  *
 *---------------------------------*/

static const S_DocumentTypeNameMap DocumentTypeNameMap[] =
{
    {DSRTypes::DT_invalid,         "",                  "invalid document type"},
    {DSRTypes::DT_BasicTextSR,     UID_BasicTextSR,     "Basic Text SR"},
    {DSRTypes::DT_EnhancedSR,      UID_EnhancedSR,      "Enhanced SR"},
    {DSRTypes::DT_ComprehensiveSR, UID_ComprehensiveSR, "Comprehensive SR"}
};


static const S_RelationshipTypeNameMap RelationshipTypeNameMap[] =
{
    {DSRTypes::RT_invalid,       "",                "invalid/unknown relationship type"},
    {DSRTypes::RT_isRoot,        "",                ""},
    {DSRTypes::RT_contains,      "CONTAINS",        "contains"},
    {DSRTypes::RT_hasObsContext, "HAS OBS CONTEXT", "has obs context"},
    {DSRTypes::RT_hasAcqContext, "HAS ACQ CONTEXT", "has acq context"},
    {DSRTypes::RT_hasConceptMod, "HAS CONCEPT MOD", "has concept mod"},
    {DSRTypes::RT_hasProperties, "HAS PROPERTIES",  "has properties"},
    {DSRTypes::RT_inferredFrom,  "INFERRED FROM",   "inferred from"},
    {DSRTypes::RT_selectedFrom,  "SELECTED FROM",   "selected from"}
};


static const S_ValueTypeNameMap ValueTypeNameMap[] =
{
    {DSRTypes::VT_invalid,   "",          "invalid/unknown value type"},
    {DSRTypes::VT_Text,      "TEXT",      "Text"},
    {DSRTypes::VT_Code,      "CODE",      "Code"},
    {DSRTypes::VT_Num,       "NUM",       "Number"},
    {DSRTypes::VT_DateTime,  "DATETIME",  "Date/Time"},
    {DSRTypes::VT_Date,      "DATE",      "Date"},
    {DSRTypes::VT_Time,      "TIME",      "Time"},
    {DSRTypes::VT_UIDRef,    "UIDREF",    "UID Reference"},
    {DSRTypes::VT_PName,     "PNAME",     "Person Name"},
    {DSRTypes::VT_SCoord,    "SCOORD",    "Spatial Coordinates"},
    {DSRTypes::VT_TCoord,    "TCOORD",    "Temporal Coordinates"},
    {DSRTypes::VT_Composite, "COMPOSITE", "Composite Object"},
    {DSRTypes::VT_Image,     "IMAGE",     "Image"},
    {DSRTypes::VT_Waveform,  "WAVEFORM",  "Waveform"},
    {DSRTypes::VT_Container, "CONTAINER", "Container"}
};


static const S_GraphicTypeNameMap GraphicTypeNameMap[] =
{
    {DSRTypes::GT_invalid,    "",           "invalid/unknown graphic type"},
    {DSRTypes::GT_Point,      "POINT",      "Point"},
    {DSRTypes::GT_Multipoint, "MULTIPOINT", "Multiple Points"},
    {DSRTypes::GT_Polyline,   "POLYLINE",   "Polyline"},
    {DSRTypes::GT_Circle,     "CIRCLE",     "Circle"},
    {DSRTypes::GT_Ellipse,    "ELLIPSE",    "Ellipse"}
};


static const S_ContinuityOfContentNameMap ContinuityOfContentNameMap[] =
{
    {DSRTypes::COC_invalid,    ""},
    {DSRTypes::COC_Separate,   "SEPARATE"},
    {DSRTypes::COC_Continuous, "CONTINUOUS"}
};


static const S_CompletionFlagNameMap CompletionFlagNameMap[] =
{
    {DSRTypes::CF_invalid,  ""},
    {DSRTypes::CF_Partial,  "PARTIAL"},
    {DSRTypes::CF_Complete, "COMPLETE"}
};


static const S_VerificationFlagNameMap VerificationFlagNameMap[] =
{
    {DSRTypes::VF_invalid,    ""},
    {DSRTypes::VF_Unverified, "UNVERIFIED"},
    {DSRTypes::VF_Verified,   "VERIFIED"}
};


/*------------------*
 *  implementation  *
 *------------------*/

const char *DSRTypes::documentTypeToSOPClassUID(const E_DocumentType documentType)
{
    const S_DocumentTypeNameMap *iterator = DocumentTypeNameMap;
    while ((iterator->Type != DT_last) && (iterator->Type != documentType))
        iterator++;
    return iterator->SOPClassUID;
}


const char *DSRTypes::documentTypeToReadableName(const E_DocumentType documentType)
{
    const S_DocumentTypeNameMap *iterator = DocumentTypeNameMap;
    while ((iterator->Type != DT_last) && (iterator->Type != documentType))
        iterator++;
    return iterator->ReadableName;
}


const char *DSRTypes::relationshipTypeToDefinedTerm(const E_RelationshipType relationshipType)
{
    const S_RelationshipTypeNameMap *iterator = RelationshipTypeNameMap;
    while ((iterator->Type != RT_last) && (iterator->Type != relationshipType))
        iterator++;
    return iterator->DefinedTerm;
}


const char *DSRTypes::relationshipTypeToReadableName(const E_RelationshipType relationshipType)
{
    const S_RelationshipTypeNameMap *iterator = RelationshipTypeNameMap;
    while ((iterator->Type != RT_last) && (iterator->Type != relationshipType))
        iterator++;
    return iterator->ReadableName;
}


const char *DSRTypes::valueTypeToDefinedTerm(const E_ValueType valueType)
{
    const S_ValueTypeNameMap *iterator = ValueTypeNameMap;
    while ((iterator->Type != VT_last) && (iterator->Type != valueType))
        iterator++;
    return iterator->DefinedTerm;
}


const char *DSRTypes::valueTypeToReadableName(const E_ValueType valueType)
{
    const S_ValueTypeNameMap *iterator = ValueTypeNameMap;
    while ((iterator->Type != VT_last) && (iterator->Type != valueType))
        iterator++;
    return iterator->ReadableName;
}


const char *DSRTypes::graphicTypeToEnumeratedValue(const E_GraphicType graphicType)
{
    const S_GraphicTypeNameMap *iterator = GraphicTypeNameMap;
    while ((iterator->Type != GT_last) && (iterator->Type != graphicType))
        iterator++;
    return iterator->EnumeratedValue;
}


const char *DSRTypes::graphicTypeToReadableName(const E_GraphicType graphicType)
{
    const S_GraphicTypeNameMap *iterator = GraphicTypeNameMap;
    while ((iterator->Type != GT_last) && (iterator->Type != graphicType))
        iterator++;
    return iterator->ReadableName;
}


const char *DSRTypes::continuityOfContentToEnumeratedValue(const E_ContinuityOfContent continuityOfContent)
{
    const S_ContinuityOfContentNameMap *iterator = ContinuityOfContentNameMap;
    while ((iterator->Type != COC_last) && (iterator->Type != continuityOfContent))
        iterator++;
    return iterator->EnumeratedValue;
}


const char *DSRTypes::completionFlagToEnumeratedValue(const E_CompletionFlag completionFlag)
{
    const S_CompletionFlagNameMap *iterator = CompletionFlagNameMap;
    while ((iterator->Type != CF_last) && (iterator->Type != completionFlag))
        iterator++;
    return iterator->EnumeratedValue;
}


const char *DSRTypes::verificationFlagToEnumeratedValue(const E_VerificationFlag verificationFlag)
{
    const S_VerificationFlagNameMap *iterator = VerificationFlagNameMap;
    while ((iterator->Type != VF_last) && (iterator->Type != verificationFlag))
        iterator++;
    return iterator->EnumeratedValue;
}


DSRTypes::E_DocumentType DSRTypes::sopClassUIDToDocumentType(const OFString &sopClassUID)
{
    E_DocumentType type = DT_invalid;
    const S_DocumentTypeNameMap *iterator = DocumentTypeNameMap;
    while ((iterator->Type != DT_last) && (sopClassUID != iterator->SOPClassUID))
        iterator++;
    if (sopClassUID == iterator->SOPClassUID)
        type = iterator->Type;
    return type;
}


DSRTypes::E_RelationshipType DSRTypes::definedTermToRelationshipType(const OFString &definedTerm)
{
    E_RelationshipType type = RT_invalid;
    const S_RelationshipTypeNameMap *iterator = RelationshipTypeNameMap;
    while ((iterator->Type != RT_last) && (definedTerm != iterator->DefinedTerm))
        iterator++;
    if (definedTerm == iterator->DefinedTerm)
        type = iterator->Type;
    return type;
}


DSRTypes::E_ValueType DSRTypes::definedTermToValueType(const OFString &definedTerm)
{
    E_ValueType type = VT_invalid;
    const S_ValueTypeNameMap *iterator = ValueTypeNameMap;
    while ((iterator->Type != VT_last) && (definedTerm != iterator->DefinedTerm))
        iterator++;
    if (definedTerm == iterator->DefinedTerm)
        type = iterator->Type;
    return type;
}


DSRTypes::E_GraphicType DSRTypes::enumeratedValueToGraphicType(const OFString &enumeratedValue)
{
    E_GraphicType type = GT_invalid;
    const S_GraphicTypeNameMap *iterator = GraphicTypeNameMap;
    while ((iterator->Type != GT_last) && (enumeratedValue != iterator->EnumeratedValue))
        iterator++;
    if (enumeratedValue == iterator->EnumeratedValue)
        type = iterator->Type;
    return type;
}


DSRTypes::E_ContinuityOfContent DSRTypes::enumeratedValueToContinuityOfContent(const OFString &enumeratedValue)
{
    E_ContinuityOfContent type = COC_invalid;
    const S_ContinuityOfContentNameMap *iterator = ContinuityOfContentNameMap;
    while ((iterator->Type != COC_last) && (enumeratedValue != iterator->EnumeratedValue))
        iterator++;
    if (enumeratedValue == iterator->EnumeratedValue)
        type = iterator->Type;
    return type;
}


DSRTypes::E_CompletionFlag DSRTypes::enumeratedValueToCompletionFlag(const OFString &enumeratedValue)
{
    E_CompletionFlag type = CF_invalid;
    const S_CompletionFlagNameMap *iterator = CompletionFlagNameMap;
    while ((iterator->Type != CF_last) && (enumeratedValue != iterator->EnumeratedValue))
        iterator++;
    if (enumeratedValue == iterator->EnumeratedValue)
        type = iterator->Type;
    return type;
}


DSRTypes::E_VerificationFlag DSRTypes::enumeratedValueToVerificationFlag(const OFString &enumeratedValue)
{
    E_VerificationFlag type = VF_invalid;
    const S_VerificationFlagNameMap *iterator = VerificationFlagNameMap;
    while ((iterator->Type != VF_last) && (enumeratedValue != iterator->EnumeratedValue))
        iterator++;
    if (enumeratedValue == iterator->EnumeratedValue)
        type = iterator->Type;
    return type;
}


OFBool DSRTypes::isDocumentTypeSupported(const E_DocumentType documentType)
{
    return (documentType == DT_BasicTextSR) || (documentType == DT_EnhancedSR);
}


E_Condition DSRTypes::addElementToDataset(E_Condition &result,
                                          DcmItem &dataset,
                                          DcmElement *delem)
{
    if (result == EC_Normal)
    {
        if (delem != NULL)
            dataset.insert(delem);
        else
            result = EC_MemoryExhausted;
    }
    return result;
}


E_Condition DSRTypes::getElementFromDataset(DcmItem &dataset,
                                            DcmElement &delem)
{
    DcmStack stack;
    E_Condition result = dataset.search((DcmTagKey &)delem.getTag(), stack, ESM_fromHere, OFFalse);
    if (result == EC_Normal)
        delem = *((DcmElement *)stack.top());
    return result;
}


E_Condition DSRTypes::getSequenceFromDataset(DcmItem &dataset,
                                             DcmSequenceOfItems &dseq)
{
    DcmStack stack;
    E_Condition result = dataset.search((DcmTagKey &)dseq.getTag(), stack, ESM_fromHere, OFFalse);
    if (result == EC_Normal)
        dseq = *((DcmSequenceOfItems *)stack.top());
    return result;
}


const char *DSRTypes::getStringValueFromElement(const DcmElement &delem)
{
    char *string = NULL;
    if (((DcmElement &)delem).getString(string) != EC_Normal)
        string = NULL;
    return string;
}


const OFString &DSRTypes::getStringValueFromElement(const DcmElement &delem,
                                                    OFString &stringValue)
{
    if (((DcmElement &)delem).getOFString(stringValue, 0) != EC_Normal)
        stringValue.clear();
    return stringValue;
}


E_Condition DSRTypes::getStringValueFromDataset(DcmItem &dataset,
                                                const DcmTagKey &tagKey,
                                                OFString &stringValue)
{
    DcmStack stack;
    E_Condition result = dataset.search(tagKey, stack, ESM_fromHere, OFFalse);
    if (result == EC_Normal)
    {
        DcmElement *delem = (DcmElement *)stack.top();
        if (delem != NULL)
            result = delem->getOFString(stringValue, 0);
    }
    if (result != EC_Normal)
        stringValue.clear();
    return result;
}


E_Condition DSRTypes::putStringValueToDataset(DcmItem &dataset,
                                              const DcmTagKey &tagKey,
                                              const OFString &stringValue)
{
    E_Condition result = EC_Normal;
    DcmTag tag(tagKey);
    DcmElement *elem = NULL;
    switch(tag.getEVR())
    {
        case EVR_AE:
            elem = new DcmApplicationEntity(tag);
            break;
        case EVR_AS:
            elem = new DcmAgeString(tag);
            break;
        case EVR_CS:
            elem = new DcmCodeString(tag);
            break;
        case EVR_DA:
            elem = new DcmDate(tag);
            break;
        case EVR_DS:
            elem = new DcmDecimalString(tag);
            break;
        case EVR_DT:
            elem = new DcmDateTime(tag);
            break;
        case EVR_IS:
            elem = new DcmIntegerString(tag);
            break;
        case EVR_TM:
            elem = new DcmTime(tag);
            break;
        case EVR_UI:
            elem = new DcmUniqueIdentifier(tag);
            break;
        case EVR_LO:
            elem = new DcmLongString(tag);
            break;
        case EVR_LT:
            elem = new DcmLongText(tag);
            break;
        case EVR_UT:
            elem = new DcmUnlimitedText(tag);
            break;
        case EVR_PN:
            elem = new DcmPersonName(tag);
            break;
        case EVR_SH:
            elem = new DcmShortString(tag);
            break;
        case EVR_ST:
            elem = new DcmShortText(tag);
            break;
        default:
            result = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        result = elem->putString(stringValue.c_str());
        if (result == EC_Normal)
            dataset.insert(elem, OFTrue /* replaceOld */);
    } else if (result == EC_Normal)
        result = EC_MemoryExhausted;
    return result;
}


OFBool DSRTypes::checkElementValue(DcmElement &delem,
                                   const OFString &vm,
                                   const OFString &type,
                                   OFConsole *stream,                                   
                                   const E_Condition searchCond,
                                   const char *moduleName)
{
    OFBool result = OFTrue;
    OFBool print = OFTrue;
    DcmTag tag = delem.getTag();
    OFString message = tag.getTagName();
    OFString module = (moduleName == NULL) ? "SR document" : moduleName;
    Uint32 lenNum;
    unsigned long vmNum;
    OFString vmText;
    /* special case: sequence of items */
    if (delem.getVR() == EVR_SQ)
    {
        lenNum = vmNum = ((DcmSequenceOfItems &)delem).card();
        vmText = " #items";
    } else {
        lenNum = delem.getLength();
        vmNum = delem.getVM();
        vmText = " VM";
    }
    /* NB: type 1C and 2C cannot be checked, assuming to be optional = type 3 */
    if (((type == "1") || (type == "2")) && (searchCond != EC_Normal))
    {
        message += " absent in ";
        message += module;
        message += " (type ";
        message += type;
        message += ")";
        result = OFFalse;
    }
    else if ((type == "1") && (lenNum == 0))
    {
        message += " empty in ";
        message += module;
        message += " (type 1)";
        result = OFFalse;
    }
    else if ((vm == "1") && (vmNum > 1))
    {
        message += vmText;
        message += " != 1 in ";
        message += module;
        result = OFFalse;
    }
    else if ((type == "1") && (vm == "1-n") && (vmNum != 1))
    {
        message += vmText;
        message += " != 1-n in ";
        message += module;
        result = OFFalse;
    }
    else if ((vm == "2") && (vmNum != 2))
    {
        message += vmText;
        message += " != 2 in ";
        message += module;
        result = OFFalse;
    }
    else if ((vm == "2-2n") && ((vmNum % 2) != 0))
    {
        message += vmText;
        message += " != 2-2n in ";
        message += module;
        result = (vmNum >= 2);
    } else
        print = OFFalse;
    if (print && (stream != NULL) && (message.length() > 0))
        printWarningMessage(stream, message.c_str());
    return result;
}


E_Condition DSRTypes::getAndCheckElementFromDataset(DcmItem &dataset,
                                                    DcmElement &delem,
                                                    const OFString &vm,
                                                    const OFString &type,
                                                    OFConsole *stream,
                                                    const char *moduleName)
{
    E_Condition result = getElementFromDataset(dataset, delem);
    checkElementValue(delem, vm, type, stream, result, moduleName);
    return result;
}


E_Condition DSRTypes::getAndCheckStringValueFromDataset(DcmItem &dataset,
                                                        const DcmTagKey &tagKey,
                                                        OFString &stringValue,
                                                        const OFString &vm,
                                                        const OFString &type,
                                                        OFConsole *stream,
                                                        const char *moduleName)
{
    DcmStack stack;
    E_Condition result = dataset.search(tagKey, stack, ESM_fromHere, OFFalse);
    if (result == EC_Normal)
    {
        DcmElement *delem = (DcmElement *)stack.top();
        if (delem != NULL)
        {
            checkElementValue(*delem, vm, type, stream, result, moduleName);
            result = delem->getOFString(stringValue, 0);
        }
    } else {
        if ((stream != NULL) && ((type == "1") || (type == "2")))
        {
            OFString message = DcmTag(tagKey).getTagName();
            message += " absent in ";
            message += (moduleName == NULL) ? "SR document" : moduleName;
            message += " (type ";
            message += type;
            message += ")";
            printWarningMessage(stream, message.c_str());
        }
    }
    if (result != EC_Normal)
        stringValue.clear();
    return result;
}


const OFString &DSRTypes::currentDate(OFString &dateString)
{
    time_t tt = time(NULL);
    struct tm *ts = localtime(&tt);
    if (ts)
    {
        char buf[32];
        sprintf(buf, "%04d%02d%02d", 1900 + ts->tm_year, ts->tm_mon + 1, ts->tm_mday);
        dateString = buf;
    } else
        dateString = "19000101";
    return dateString;
}


const OFString &DSRTypes::currentTime(OFString &timeString)
{
    time_t tt = time(NULL);
    struct tm *ts = localtime(&tt);
    if (ts)
    {
        char buf[32];
        sprintf(buf, "%02d%02d%02d", ts->tm_hour, ts->tm_min, ts->tm_sec);
        timeString = buf;
    } else
        timeString = "000000";
    return timeString;
}


const OFString &DSRTypes::currentDateTime(OFString &dateTimeString)
{
    time_t tt = time(NULL);
    struct tm *ts = localtime(&tt);
    if (ts)
    {
        char buf[32];
        sprintf(buf, "%04d%02d%02d%02d%02d%02d", 1900 + ts->tm_year, ts->tm_mon + 1, ts->tm_mday,
            ts->tm_hour, ts->tm_min, ts->tm_sec);
        dateTimeString = buf;
    } else
        dateTimeString = "19000101000000";
    return dateTimeString;
}


const char *DSRTypes::numberToString(const size_t number,
                                     char *string)
{
    if (string != NULL)
        sprintf(string, "%u", number);
    return string;
}


size_t DSRTypes::stringToNumber(const char *string)
{
    size_t result = 0;
    if (string != NULL)
    {
        if (sscanf(string, "%u", &result) != 1)
            result = 0;
    }
    return result;
}

DSRDocumentTreeNode *DSRTypes::createDocumentTreeNode(const E_RelationshipType relationshipType,
                                                      const E_ValueType valueType)
{
    DSRDocumentTreeNode *node = NULL;
    switch (valueType)
    {
        case VT_Text:
            node = new DSRTextTreeNode(relationshipType);
            break;
        case VT_Code:
            node = new DSRCodeTreeNode(relationshipType);
            break;
        case VT_Num:
            node = new DSRNumTreeNode(relationshipType);
            break;
        case VT_DateTime:
            node = new DSRDateTimeTreeNode(relationshipType);
            break;
        case VT_Date:
            node = new DSRDateTreeNode(relationshipType);
            break;
        case VT_Time:
            node = new DSRTimeTreeNode(relationshipType);
            break;
        case VT_UIDRef:
            node = new DSRUIDRefTreeNode(relationshipType);
            break;
        case VT_PName:
            node = new DSRPNameTreeNode(relationshipType);
            break;
        case VT_SCoord:
            node = new DSRSCoordTreeNode(relationshipType);
            break;
        case VT_Composite:
            node = new DSRCompositeTreeNode(relationshipType);
            break;
        case VT_Image:
            node = new DSRImageTreeNode(relationshipType);
            break;
        case VT_Waveform:
            node = new DSRWaveformTreeNode(relationshipType);
            break;
        case VT_Container:
            node = new DSRContainerTreeNode(relationshipType);
            break;
        default:
            break;
    }
    return node;
}


void DSRTypes::printWarningMessage(OFConsole *stream,
                                   const char *message)
{
    if ((stream != NULL) && (message != NULL))
    {
        stream->lockCerr() << "DCMSR - Warning: " << message << endl;
        stream->unlockCerr();
    }
}


void DSRTypes::printErrorMessage(OFConsole *stream,
                                 const char *message)
{
    if ((stream != NULL) && (message != NULL))
    {
        stream->lockCerr() << "DCMSR - Error: " << message << endl;
        stream->unlockCerr();
    }
}


void DSRTypes::printInvalidContentItemMessage(OFConsole *stream,
                                              const char *action,
                                              const DSRDocumentTreeNode *node)
{
    if (stream != NULL)
    {
        OFString message;
        if (action != NULL)
            message += action;
        else
            message += "Processing";
        message += " invalid/incomplete content item ";
        if (node != NULL)
        {
            message += valueTypeToDefinedTerm(node->getValueType());
#ifdef DEBUG
            message += " #";
            char string[20];
            message += numberToString(node->getNodeID(), string);
#endif   
        }         
        printWarningMessage(stream, message.c_str());
    }
}


void DSRTypes::printContentItemErrorMessage(OFConsole *stream,
                                            const char *action,
                                            const E_Condition result,
                                            const DSRDocumentTreeNode *node)
{
    if ((stream != NULL) && (result != EC_Normal))
    {
        OFString message;
        if (action != NULL)
            message += action;
        else
            message += "Processing";
        message += " content item ";
        if (node != NULL)
        {
            message += valueTypeToDefinedTerm(node->getValueType());
#ifdef DEBUG
            message += " #";
            char string[20];
            message += numberToString(node->getNodeID(), string);
#endif   
            message += " (";
            message += dcmErrorConditionToString(result);
            message += ")";
        }         
        printErrorMessage(stream, message.c_str());
    }
}


size_t DSRTypes::createHTMLAnnexEntry(ostream &docStream,
                                      ostream &annexStream,
                                      const OFString &referenceText,
                                      size_t &annexNumber)
{
    /* hyperlink to corresponding annex */
    docStream << "[";
    if (referenceText.length() > 0)
        docStream << referenceText << " ";
    docStream << "<a name=\"annex_src_" << annexNumber << "\" href=\"#annex_dst_" << annexNumber << "\">Annex " << annexNumber << "</a>]" << endl;
    /* create new annex */
    annexStream << "<h2><a name=\"annex_dst_" << annexNumber << "\" href=\"#annex_src_" << annexNumber << "\">Annex " << annexNumber << "</a></h2>" << endl;
    /* increase annex number, return previous number */
    return annexNumber++;
}


size_t DSRTypes::createHTMLFootnote(ostream &docStream,
                                    ostream &footnoteStream,
                                    size_t &footnoteNumber,
                                    const size_t nodeID)
{
    /* hyperlink to corresponding footnote */
    docStream << "<sup><small><a name=\"footnote_src_" << nodeID << "_" << footnoteNumber << "\" ";
    docStream << "href=\"#footnote_dst_" << nodeID << "_" << footnoteNumber << "\">" << footnoteNumber << "</a></small></sup>" << endl;
    /* create new footnote */
    footnoteStream << "<b><a name=\"footnote_dst_" << nodeID << "_" << footnoteNumber << "\" ";
    footnoteStream << "href=\"#footnote_src_" << nodeID << "_" << footnoteNumber << "\">Footnote " << footnoteNumber << "</a></b>" << endl;
    /* increase footnote number, return previous number */
    return footnoteNumber++;
}


E_Condition DSRTypes::appendStream(ostream &mainStream,
                                   ostrstream &tempStream,
                                   const char *heading)
{
    E_Condition result = EC_IllegalCall;
    /* add final 0 byte */
    tempStream << ends;
    /* freeze/get string (now we have full control over the array) */
    char *string = tempStream.str();
    /* should never be NULL */
    if (string != NULL)
    {
        if (strlen(string) > 0)
        {
            /* append optional heading */
            if (heading != NULL)
                mainStream << heading << endl;
            /* append temporal document to main document */
            mainStream << string;
        }
        /* very important! since we have full control we are responsible for deleting the array */
        delete[] string;
        result = EC_Normal;
    }
    return result;
}

/*
 *  CVS/RCS Log:
 *  $Log: dsrtypes.cc,v $
 *  Revision 1.4  2000-10-18 17:23:58  joergr
 *  Added new method allowing to get and check string values from dataset.
 *
 *  Revision 1.3  2000/10/16 12:09:28  joergr
 *  Added new options: number nested items instead of indenting them, print SOP
 *  instance UID of referenced composite objects.
 *
 *  Revision 1.2  2000/10/13 08:53:33  joergr
 *  Removed typedef statements to keep MSVC++ quiet.
 *
 *  Revision 1.1  2000/10/13 07:52:27  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
