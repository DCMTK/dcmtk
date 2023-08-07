/*
 *
 *  Copyright (C) 2003-2022, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Helper class for converting an XML document to DICOM file or data set
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcmxml/xml2dcm.h"

#ifdef WITH_LIBXML

#include "dcmtk/dcmdata/dctypes.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcpixseq.h"
#include "dcmtk/dcmdata/dcpixel.h"
#include "dcmtk/dcmdata/dcpxitem.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/dcmdata/dcswap.h"
#include <libxml/parser.h>

// This function is also used in dcmsr, try to stay in sync!
#if defined(HAVE_VSNPRINTF) && defined(HAVE_PROTOTYPE_VSNPRINTF)
extern "C" void xml2dcm_errorFunction(void * ctx, const char *msg, ...)
{
    // Classic C requires us to declare variables at the beginning of the function.
    OFString &buffer = *OFstatic_cast(OFString*, ctx);
#else
extern "C" void xml2dcm_errorFunction(void * /* ctx */, const char *msg, ...)
{
#endif

    if (!DCM_dcmdataLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
        return;

#if defined(HAVE_VSNPRINTF) && defined(HAVE_PROTOTYPE_VSNPRINTF)
    // libxml calls us multiple times for one line of log output which would
    // result in garbled output. To avoid this, we buffer the output in a local
    // string in the caller which we get through our 'ctx' parameter. Then, we
    // output this string on one go when we receive a newline.
    va_list ap;
    char buf[1024];

    va_start(ap, msg);
#ifdef HAVE_PROTOTYPE_STD__VSNPRINTF
    std::vsnprintf(buf, 1024, msg, ap);
#else
    vsnprintf(buf, 1024, msg, ap);
#endif
    va_end(ap);

    // Since we can't do anything about a too small buffer for vsnprintf(), we
    // ignore it. But we do make sure the buffer is null-terminated!
    buf[1023] = '\0';
    buffer += buf;

    // If there is a full line in the buffer...
    size_t pos = buffer.find('\n');
    while (pos != OFString_npos)
    {
        // ..output it and remove it from the buffer
        DCMDATA_DEBUG(buffer.substr(0, pos));
        buffer.erase(0, pos + 1);

        pos = buffer.find('\n');
    }
#elif defined(HAVE_VPRINTF)
    // No vsnprint, but at least vfprintf. Output the messages directly to stderr.
    va_list ap;
    va_start(ap, msg);
#ifdef HAVE_PROTOTYPE_STD__VFPRINTF
    std::vfprintf(stderr, msg, ap);
#else
    vfprintf(stderr, msg, ap);
#endif
    va_end(ap);
#else
    // We can only show the most basic part of the message, this will look bad :(
    printf("%s", msg);
#endif
}


static OFBool xmlIsBlankNodeOrComment(xmlNode *node)
{
  if (xmlIsBlankNode(node)) return OFTrue;
  return (0 == xmlStrcmp(node->name, OFreinterpret_cast(const xmlChar *, "comment")));
}


DcmXMLParseHelper::DcmXMLParseHelper()
: EncodingHandler(NULL)
{
}


DcmXMLParseHelper::~DcmXMLParseHelper()
{
}


void DcmXMLParseHelper::initLibrary()
{
    /* check for compatible libxml version */
    LIBXML_TEST_VERSION

    /* initialize the XML library (only required for MT-safety) */
    xmlInitParser();

    /* do not substitute entities (other than the standard ones) */
    xmlSubstituteEntitiesDefault(0);

    /* add line number to debug messages */
    xmlLineNumbersDefault(1);

    /* enable node indenting for tree output */
    xmlIndentTreeOutput = 1;
    xmlKeepBlanksDefault(0);

    /* enable libxml warnings and error messages */
    xmlGetWarningsDefaultValue = 1;
}


void DcmXMLParseHelper::cleanupLibrary()
{
    xmlCleanupParser();
}


OFBool DcmXMLParseHelper::convertUtf8ToCharset(
    const xmlChar *fromString,
    OFString &toString)
{
    OFBool result = OFFalse;
    if (EncodingHandler != NULL)
    {
        /* prepare input/output buffers */
        xmlBufferPtr fromBuffer = xmlBufferCreate();
        xmlBufferPtr toBuffer = xmlBufferCreate();
        xmlBufferCat(fromBuffer, fromString);
        /* convert character encoding of given string */
        result = (xmlCharEncOutFunc(EncodingHandler, toBuffer, fromBuffer) >= 0);
        if (result)
            toString = OFreinterpret_cast(const char *, xmlBufferContent(toBuffer));
        /* free allocated memory */
        xmlBufferFree(toBuffer);
        xmlBufferFree(fromBuffer);
    }
    return result;
}


OFCondition DcmXMLParseHelper::checkNode(
    xmlNodePtr current,
    const char *name)
{
    OFCondition result = EC_Normal;
    /* check whether node is valid at all */
    if (current != NULL)
    {
        /* check whether node has expected name */
        if (xmlStrcmp(current->name, OFreinterpret_cast(const xmlChar *, name)) != 0)
        {
            DCMDATA_ERROR("document of the wrong type, was '" << current->name << "', '" << name << "' expected");
            result = EC_XMLParseError;
        }
    } else {
        DCMDATA_ERROR("document of the wrong type, '" << name << "' expected");
        result = EC_XMLParseError;
    }
    return result;
}


OFCondition DcmXMLParseHelper::createNewElement(
    xmlNodePtr current,
    DcmElement *&newElem)
{
    OFCondition result = EC_IllegalCall;
    /* check whether node is valid */
    if (current != NULL)
    {
        /* get required information from XML element */
        xmlChar *elemTag = xmlGetProp(current, OFreinterpret_cast(const xmlChar *, "tag"));
        xmlChar *elemVR = xmlGetProp(current, OFreinterpret_cast(const xmlChar *, "vr"));
        /* convert tag string */
        DcmTagKey dcmTagKey;
        unsigned int group = 0xffff;
        unsigned int elem = 0xffff;
        /* make sure that "tag" attribute exists */
        if (elemTag == NULL)
        {
            DCMDATA_WARN("missing 'tag' attribute for node '" << current->name << "'");
            result = EC_XMLParseError;
        }
        /* determine group and element number from "tag" */
        else if (sscanf(OFreinterpret_cast(char *, elemTag), "%x,%x", &group, &elem ) == 2)
        {
            dcmTagKey.set(OFstatic_cast(Uint16, group), OFstatic_cast(Uint16, elem));
            DcmTag dcmTag(dcmTagKey);
            /* convert vr string */
            DcmVR dcmVR(OFreinterpret_cast(char *, elemVR));
            DcmEVR dcmEVR = dcmVR.getEVR();
            if (dcmEVR == EVR_UNKNOWN)
            {
                /* check whether "vr" attribute exists */
                if (elemVR == NULL)
                {
                    DCMDATA_WARN("missing 'vr' attribute for " << dcmTag
                        << ", using unknown VR");
                } else {
                    DCMDATA_WARN("invalid 'vr' attribute (" << elemVR
                        << ") for " << dcmTag << ", using unknown VR");
                }
            }
            /* check for correct vr */
            const DcmEVR tagEVR = dcmTag.getEVR();
            if ((tagEVR != dcmEVR) && (dcmEVR != EVR_UNKNOWN) && (tagEVR != EVR_UNKNOWN) &&
                ((dcmTagKey != DCM_LUTData) || ((dcmEVR != EVR_US) && (dcmEVR != EVR_SS) && (dcmEVR != EVR_OW))) &&
                ((tagEVR != EVR_xs) || ((dcmEVR != EVR_US) && (dcmEVR != EVR_SS))) &&
                (((tagEVR != EVR_ox) && (tagEVR != EVR_px)) || ((dcmEVR != EVR_OB) && (dcmEVR != EVR_OW))))
            {
                DCMDATA_WARN("element " << dcmTag << " has wrong VR (" << dcmVR.getVRName()
                    << "), correct is " << dcmTag.getVR().getVRName());
            }
            if (dcmEVR != EVR_UNKNOWN)
                dcmTag.setVR(dcmVR);
            /* create DICOM element */
            result = DcmItem::newDicomElementWithVR(newElem, dcmTag);
        } else {
            DCMDATA_WARN("invalid 'tag' attribute (" << elemTag << ") for node '" << current->name << "'");
            result = EC_XMLParseError;
        }
        if (result.bad())
        {
            /* delete new element if an error occurred */
            delete newElem;
            newElem = NULL;
        }
        /* free allocated memory */
        xmlFree(elemTag);
        xmlFree(elemVR);
    }
    return result;
}


OFCondition DcmXMLParseHelper::putElementContent(
    xmlNodePtr current,
    DcmElement *element)
{
    OFCondition result = EC_Normal;
    /* check whether node and element are valid */
    if ((current != NULL) && (element != NULL))
    {
        DcmEVR dcmEVR = element->getVR();
        /* get the XML node content */
        xmlChar *elemVal = xmlNodeGetContent(current);
        xmlChar *attrVal = xmlGetProp(current, OFreinterpret_cast(const xmlChar *, "binary"));
        /* check whether node content is present */
        if (xmlStrcmp(attrVal, OFreinterpret_cast(const xmlChar *, "hidden")) == 0)
        {
            DCMDATA_WARN("content of node " << element->getTag() << " is 'hidden', empty element inserted");
            /* return an error unless the element is part of the file meta information */
            if (element->getGTag() != 0x0002)
                result = EC_MissingValue;
        }
        /* check whether node content is base64 encoded */
        else if (xmlStrcmp(attrVal, OFreinterpret_cast(const xmlChar *, "base64")) == 0)
        {
            Uint8 *data = NULL;
            const size_t length = OFStandard::decodeBase64(OFreinterpret_cast(char *, elemVal), data);
            if (length > 0)
            {
                if (dcmEVR == EVR_OW)
                {
                    /* Base64 decoder produces big endian output data, convert to local byte order */
                    swapIfNecessary(gLocalByteOrder, EBO_BigEndian, data, OFstatic_cast(Uint32, length), sizeof(Uint16));
                }
                result = element->putUint8Array(data, OFstatic_cast(Uint32, length));
                /* delete buffer since data is copied into the element */
                delete[] data;
            }
        }
        /* check whether node content is stored in a file */
        else if (xmlStrcmp(attrVal, OFreinterpret_cast(const xmlChar *, "file")) == 0)
        {
            if (xmlStrlen(elemVal) > 0)
            {
                const char *filename = OFreinterpret_cast(char *, elemVal);
                /* try to open binary file */
                FILE *f = fopen(filename, "rb");
                if (f != NULL)
                {
                    /* determine filesize */
                    const size_t fileSize = OFStandard::getFileSize(filename);
                    size_t buflen = fileSize;
                    /* if odd then make even (DICOM requires even length values) */
                    if (buflen & 1)
                        buflen++;
                    Uint8 *buf = NULL;
                    /* create buffer of OB or OW data */
                    if (dcmEVR == EVR_OW)
                    {
                        Uint16 *buf16 = NULL;
                        result = element->createUint16Array(OFstatic_cast(Uint32, buflen / 2), buf16);
                        buf = OFreinterpret_cast(Uint8 *, buf16);
                    } else
                        result = element->createUint8Array(OFstatic_cast(Uint32, buflen), buf);
                    if (result.good())
                    {
                        DCMDATA_INFO("reading " << fileSize << " bytes from binary data file: " << filename);
                        DCMDATA_DEBUG("  and storing it in the element " << element->getTag());
                        /* read binary file into the buffer */
                        if (fread(buf, 1, OFstatic_cast(size_t, fileSize), f) != fileSize)
                        {
                            DCMDATA_WARN("cannot read binary data file: " << filename << ": " << OFStandard::getLastSystemErrorCode().message());
                            result = EC_CorruptedData;
                        }
                        else if (dcmEVR == EVR_OW)
                        {
                            /* swap 16 bit OW data (if necessary) */
                            swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, buf, OFstatic_cast(Uint32, buflen), sizeof(Uint16));
                        }
                    }
                    fclose(f);
                } else {
                    DCMDATA_WARN("cannot open binary data file: " << filename);
                    result = EC_InvalidTag;
                }
            } else
                DCMDATA_WARN("filename for element " << element->getTag() << " is missing, empty element inserted");
        } else {
            OFString dicomVal;
            /* convert character set from UTF-8 (for specific VRs only) */
            if (element->isAffectedBySpecificCharacterSet() &&
               (xmlStrlen(elemVal) > 0) && convertUtf8ToCharset(elemVal, dicomVal))
            {
                result = element->putOFStringArray(dicomVal);
            } else {
                /* set the value of the newly created element */
                result = element->putString(OFreinterpret_cast(char *, elemVal));
            }
            if (result.bad())
                DCMDATA_WARN("cannot put content to element " << element->getTag() << ": " << result.text());
        }
        /* free allocated memory */
        xmlFree(elemVal);
        xmlFree(attrVal);
    } else
        result = EC_IllegalCall;
    return result;
}


OFCondition DcmXMLParseHelper::parseElement(
    DcmItem *dataset,
    xmlNodePtr current)
{
    DcmElement *newElem = NULL;
    /* create new DICOM element from XML element */
    OFCondition result = createNewElement(current, newElem);
    if (result.good())
    {
        /* retrieve specific character set (only on main dataset level) */
        if ((EncodingHandler == NULL) && (dataset->ident() == EVR_dataset) &&
            (newElem->getTag() == DCM_SpecificCharacterSet))
        {
            const char *encString = NULL;
            xmlChar *elemVal = xmlNodeGetContent(current);
            /* check for known character set */
            if (xmlStrcmp(elemVal, OFreinterpret_cast(const xmlChar *, "ISO_IR 6")) == 0)
                encString = "UTF-8";
            else if (xmlStrcmp(elemVal, OFreinterpret_cast(const xmlChar *, "ISO_IR 192")) == 0)
                encString = "UTF-8";
            else if (xmlStrcmp(elemVal, OFreinterpret_cast(const xmlChar *, "ISO_IR 100")) == 0)
                encString = "ISO-8859-1";
            else if (xmlStrcmp(elemVal, OFreinterpret_cast(const xmlChar *, "ISO_IR 101")) == 0)
                encString = "ISO-8859-2";
            else if (xmlStrcmp(elemVal, OFreinterpret_cast(const xmlChar *, "ISO_IR 109")) == 0)
                encString = "ISO-8859-3";
            else if (xmlStrcmp(elemVal, OFreinterpret_cast(const xmlChar *, "ISO_IR 110")) == 0)
                encString = "ISO-8859-4";
            else if (xmlStrcmp(elemVal, OFreinterpret_cast(const xmlChar *, "ISO_IR 144")) == 0)
                encString = "ISO-8859-5";
            else if (xmlStrcmp(elemVal, OFreinterpret_cast(const xmlChar *, "ISO_IR 127")) == 0)
                encString = "ISO-8859-6";
            else if (xmlStrcmp(elemVal, OFreinterpret_cast(const xmlChar *, "ISO_IR 126")) == 0)
                encString = "ISO-8859-7";
            else if (xmlStrcmp(elemVal, OFreinterpret_cast(const xmlChar *, "ISO_IR 138")) == 0)
                encString = "ISO-8859-8";
            else if (xmlStrcmp(elemVal, OFreinterpret_cast(const xmlChar *, "ISO_IR 148")) == 0)
                encString = "ISO-8859-9";
            else if (xmlStrcmp(elemVal, OFreinterpret_cast(const xmlChar *, "ISO_IR 203")) == 0)
                encString = "ISO-8859-15";
            else if (xmlStrlen(elemVal) > 0)
                DCMDATA_WARN("character set '" << elemVal << "' not supported");
            if (encString != NULL)
            {
                /* find appropriate encoding handler */
                EncodingHandler = xmlFindCharEncodingHandler(encString);
            }
            xmlFree(elemVal);
        }
        /* set the element value */
        result = putElementContent(current, newElem);
        /* insert the new element into the dataset */
        if (result.good())
            result = dataset->insert(newElem, OFTrue /*replaceOld*/);
        if (result.bad())
        {
            /* delete element if insertion or putting the value failed */
            delete newElem;
        }
    }
    return result;
}

OFCondition DcmXMLParseHelper::parseSequence(
    DcmSequenceOfItems *sequence,
    xmlNodePtr current,
    E_TransferSyntax xfer,
    const OFBool stopOnError)
{
    OFCondition result = EC_Normal;
    if (sequence != NULL)
    {
        /* ignore blank (empty or whitespace only) nodes */
        while ((current != NULL) && xmlIsBlankNodeOrComment(current))
            current = current->next;
        while (current != NULL)
        {
            /* ignore non-item nodes */
            if (xmlStrcmp(current->name, OFreinterpret_cast(const xmlChar *, "item")) == 0)
            {
                /* create new sequence item */
                DcmItem *newItem = new DcmItem();
                if (newItem != NULL)
                {
                    sequence->insert(newItem);
                    /* proceed parsing the item content */
                    result = parseDataSet(newItem, current->xmlChildrenNode, xfer, stopOnError);
                    if (result.bad())
                        DCMDATA_WARN("cannot parse invalid item: " << result.text());
                }
            } else if (!xmlIsBlankNodeOrComment(current))
                DCMDATA_WARN("unexpected node '" << current->name << "', 'item' expected, skipping");
            /* check for errors */
            if (result.bad())
            {
                if  (stopOnError)
                {
                    /* exit the loop and return with an error */
                    break;
                } else {
                    DCMDATA_DEBUG("ignoring error as requested by the user");
                    /* ignore the error */
                    result = EC_Normal;
                }
            }
            /* proceed with next node */
            current = current->next;
        }
    } else
        result = EC_IllegalCall;
    return result;
}


OFCondition DcmXMLParseHelper::parsePixelSequence(
    DcmPixelSequence *sequence,
    xmlNodePtr current,
    const OFBool stopOnError)
{
    OFCondition result = EC_Normal;
    if (sequence != NULL)
    {
        /* ignore blank (empty or whitespace only) nodes */
        while ((current != NULL) && xmlIsBlankNodeOrComment(current))
            current = current->next;
        while (current != NULL)
        {
            /* ignore non-pixel-item nodes */
            if (xmlStrcmp(current->name, OFreinterpret_cast(const xmlChar *, "pixel-item")) == 0)
            {
                /* create new pixel item */
                DcmPixelItem *newItem = new DcmPixelItem(DCM_PixelItemTag);
                if (newItem != NULL)
                {
                    sequence->insert(newItem);
                    /* put pixel data into the item */
                    result = putElementContent(current, newItem);
                    if (result.bad())
                        DCMDATA_WARN("cannot parse invalid pixel-item: " << result.text());
                }
            } else if (!xmlIsBlankNodeOrComment(current))
                DCMDATA_WARN("unexpected node '" << current->name << "', 'pixel-item' expected, skipping");
            /* check for errors */
            if (result.bad())
            {
                if  (stopOnError)
                {
                    /* exit the loop and return with an error */
                    break;
                } else {
                    DCMDATA_DEBUG("ignoring error as requested by the user");
                    /* ignore the error */
                    result = EC_Normal;
                }
            }
            /* proceed with next node */
            current = current->next;
        }
    } else
        result = EC_IllegalCall;
    return result;
}


OFCondition DcmXMLParseHelper::parseMetaHeader(
    DcmMetaInfo *metainfo,
    xmlNodePtr current,
    const OFBool parse,
    const OFBool stopOnError)
{
    /* check for valid node and correct name */
    OFCondition result = checkNode(current, "meta-header");
    if (result.good() && parse)
    {
        /* get child nodes */
        current = current->xmlChildrenNode;
        while (current != NULL)
        {
            /* ignore non-element nodes */
            if (xmlStrcmp(current->name, OFreinterpret_cast(const xmlChar *, "element")) == 0)
                result = parseElement(metainfo, current);
            else if (!xmlIsBlankNodeOrComment(current))
                DCMDATA_WARN("unexpected node '" << current->name << "', 'element' expected, skipping");
            /* check for errors */
            if (result.bad())
            {
                if  (stopOnError)
                {
                    /* exit the loop and return with an error */
                    break;
                } else {
                    DCMDATA_DEBUG("ignoring error as requested by the user");
                    /* ignore the error */
                    result = EC_Normal;
                }
            }
            /* proceed with next node */
            current = current->next;
        }
    }
    return result;
}


OFCondition DcmXMLParseHelper::parseDataSet(
    DcmItem *dataset,
    xmlNodePtr current,
    E_TransferSyntax xfer,
    const OFBool stopOnError)
{
    OFCondition result = EC_Normal;
    /* ignore blank (empty or whitespace only) nodes */
    while ((current != NULL) && xmlIsBlankNodeOrComment(current))
        current = current->next;
    while (current != NULL)
    {
        /* ignore non-element/sequence nodes */
        if (xmlStrcmp(current->name, OFreinterpret_cast(const xmlChar *, "element")) == 0)
            result = parseElement(dataset, current);
        else if (xmlStrcmp(current->name, OFreinterpret_cast(const xmlChar *, "sequence")) == 0)
        {
            DcmElement *newElem = NULL;
            /* create new sequence element */
            if (createNewElement(current, newElem).good())
            {
                /* insert new sequence element into the dataset */
                result = dataset->insert(newElem, OFTrue /*replaceOld*/);
                if (result.good())
                {
                    /* special handling for compressed pixel data */
                    if (newElem->getTag() == DCM_PixelData)
                    {
                        /* create new pixel sequence */
                        DcmPixelSequence *sequence = new DcmPixelSequence(DCM_PixelSequenceTag);
                        if (sequence != NULL)
                        {
                            if (newElem->ident() == EVR_PixelData)
                            {
                                /* ... insert it into the dataset and proceed with the pixel items */
                                OFstatic_cast(DcmPixelData *, newElem)->putOriginalRepresentation(xfer, NULL, sequence);
                                result = parsePixelSequence(sequence, current->xmlChildrenNode, stopOnError);
                            } else
                                DCMDATA_WARN("wrong VR for 'sequence' element with pixel data, ignoring child nodes");
                        }
                    } else {
                        /* proceed parsing the items of the sequence */
                        if (newElem->ident() == EVR_SQ)
                            result = parseSequence(OFstatic_cast(DcmSequenceOfItems *, newElem), current->xmlChildrenNode, xfer, stopOnError);
                        else
                            DCMDATA_WARN("wrong VR for 'sequence' element, ignoring child nodes");
                    }
                } else {
                    /* delete element if insertion failed */
                    delete newElem;
                }
            }
        } else if (!xmlIsBlankNodeOrComment(current))
            DCMDATA_WARN("unexpected node '" << current->name << "', skipping");
        /* check for errors */
        if (result.bad())
        {
            if  (stopOnError)
            {
                /* exit the loop and return with an error */
                break;
            } else {
                DCMDATA_DEBUG("ignoring error as requested by the user");
                /* ignore the error */
                result = EC_Normal;
            }
        }
        /* proceed with next node */
        current = current->next;
    }
    return result;
}


OFCondition DcmXMLParseHelper::validateXmlDocument(xmlDocPtr doc)
{
    OFCondition result = EC_Normal;
    DCMDATA_INFO("validating XML document ...");
    xmlGenericError(xmlGenericErrorContext, "--- libxml validating ---\n");
    /* temporary buffer needed for xml2dcm_errorFunction - more detailed explanation there */
    OFString tmpErrorString;
    /* create context for document validation */
    xmlValidCtxt cvp;
    cvp.userData = &tmpErrorString;
    cvp.error = xml2dcm_errorFunction;
    cvp.warning = xml2dcm_errorFunction;
    /* validate the document */
    const int valid = xmlValidateDocument(&cvp, doc);
    xmlGenericError(xmlGenericErrorContext, "-------------------------\n");
    if (!valid)
    {
        DCMDATA_ERROR("document does not validate");
        result = EC_XMLValidationFailure;
    }
    return result;
}


OFCondition DcmXMLParseHelper::readXmlFile(
    const char *ifname,
    DcmFileFormat &fileformat,
    E_TransferSyntax &xfer,
    const OFBool metaInfo,
    const OFBool checkNamespace,
    const OFBool validateDocument,
    const OFBool stopOnError)
{
    OFCondition result = EC_Normal;
    xfer = EXS_Unknown;

    /* temporary buffer needed for xml2dcm_errorFunction - more detailed explanation there */
    OFString tmpErrorString;
    xmlSetGenericErrorFunc(&tmpErrorString, xml2dcm_errorFunction);

    xmlGenericError(xmlGenericErrorContext, "--- libxml parsing ------\n");
    /* build an XML tree from the file */
#if LIBXML_VERSION >= 20703
    /*
     *  Starting with libxml version 2.7.3, the maximum length of XML element values
     *  is limited to 10 MB.  The following code disables this default limitation.
     */
    xmlDocPtr doc = xmlReadFile(ifname, NULL /*encoding*/, XML_PARSE_HUGE);
#else
    xmlDocPtr doc = xmlParseFile(ifname);
#endif
    xmlGenericError(xmlGenericErrorContext, "-------------------------\n");
    if (doc != NULL)
    {
        /* validate document */
        if (validateDocument)
            result = validateXmlDocument(doc);
        if (result.good())
        {
            /* check whether the document is of the right kind */
            xmlNodePtr current = xmlDocGetRootElement(doc);
            if (current != NULL)
            {
                /* check namespace declaration (if required) */
                if (!checkNamespace || (xmlSearchNsByHref(doc, current, OFreinterpret_cast(const xmlChar *, DCMTK_XML_NAMESPACE_URI)) != NULL))
                {
                    /* check whether to parse a "file-format" or "data-set" */
                    if (xmlStrcmp(current->name, OFreinterpret_cast(const xmlChar *, "file-format")) == 0)
                    {
                        DCMDATA_INFO("parsing file-format ...");
                        if (metaInfo)
                            DCMDATA_INFO("parsing meta-header ...");
                        else
                            DCMDATA_INFO("skipping meta-header ...");
                        current = current->xmlChildrenNode;
                        /* ignore blank (empty or whitespace only) nodes */
                        while ((current != NULL) && xmlIsBlankNodeOrComment(current))
                            current = current->next;
                        /* parse/skip "meta-header" */
                        result = parseMetaHeader(fileformat.getMetaInfo(), current, metaInfo /*parse*/, stopOnError);
                        if (result.good())
                        {
                            current = current->next;
                            /* ignore blank (empty or whitespace only) nodes */
                            while ((current != NULL) && xmlIsBlankNodeOrComment(current))
                                current = current->next;
                        } else
                            DCMDATA_ERROR("cannot parse invalid meta-header");
                    }
                    /* there should always be a "data-set" node */
                    if (result.good())
                    {
                        DCMDATA_INFO("parsing data-set ...");
                        /* parse "data-set" */
                        result = checkNode(current, "data-set");
                        if (result.good())
                        {
                            /* determine stored transfer syntax */
                            xmlChar *xferUID = xmlGetProp(current, OFreinterpret_cast(const xmlChar *, "xfer"));
                            if (xferUID != NULL)
                                xfer = DcmXfer(OFreinterpret_cast(char *, xferUID)).getXfer();
                            result = parseDataSet(fileformat.getDataset(), current->xmlChildrenNode, xfer, stopOnError);
                            /* free allocated memory */
                            xmlFree(xferUID);
                            if (result.bad())
                                DCMDATA_ERROR("cannot parse invalid data-set");
                        }
                    }
                    if (result.bad())
                    {
                        if (DCM_dcmdataLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
                        {
                            DCMDATA_DEBUG("--- libxml dump ---------");
                            /* dump XML document for debugging purposes */
                            xmlChar *str;
                            int size;
                            xmlDocDumpFormatMemory(doc, &str, &size, 1);
                            DCMDATA_DEBUG(str);
                            xmlFree(str);
                            DCMDATA_DEBUG("-------------------------");
                        }
                        DCMDATA_ERROR("cannot read invalid document: " << ifname);
                    }
                } else {
                    DCMDATA_ERROR("document has wrong type, dcmtk namespace not found");
                    result = EC_XMLParseError;
                }
            } else {
                DCMDATA_ERROR("document is empty: " << ifname);
                result = EC_XMLParseError;
            }
        }
    } else {
        DCMDATA_ERROR("could not parse document: " << ifname);
        result = EC_XMLParseError;
    }

    /* Reset to default function because we used a local string as context for
     * the error function.
     */
    xmlSetGenericErrorFunc(NULL, NULL);

    /* free allocated memory */
    xmlFreeDoc(doc);
    return result;
}

#else  /* WITH_LIBXML */

/* make sure that the object file is not completely empty if compiled
 * without libxml2 because some linkers might fail otherwise.
 */
void xml2dcm_lib_dummy_function()
{
  return;
}

#endif /* WITH_LIBXML */
