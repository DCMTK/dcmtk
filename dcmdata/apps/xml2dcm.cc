/*
 *
 *  Copyright (C) 2003-2019, OFFIS e.V.
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
 *  Purpose: Convert XML document to DICOM file or data set
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcpxitem.h"   /* for class DcmPixelItem */
#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcostrmz.h"   /* for dcmZlibCompressionLevel */

#define INCLUDE_CSTDARG
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef WITH_ZLIB
#include <zlib.h>                     /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "xml2dcm"
#define OFFIS_CONSOLE_DESCRIPTION "Convert XML document to DICOM file or data set"

// currently not used since DTD is always retrieved from XML document
//#define DOCUMENT_TYPE_DEFINITION_FILE "dcm2xml.dtd"

static OFLogger xml2dcmLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);
static OFLogger xmlLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION ".libxml");

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

// ********************************************

#ifdef WITH_LIBXML

#ifdef __ibmxl__
// IBM xlC defines __GNUC__ but does not support the GNUC extension
// __attribute__ ((format (printf, 2, 3))).
// This avoids a compiler warning in <libxml/parser.h>.
#define LIBXML_ATTR_FORMAT(fmt,args)
#endif

#include <libxml/parser.h>


// stores pointer to character encoding handler
static xmlCharEncodingHandlerPtr EncodingHandler = NULL;

// This function is also used in dcmsr, try to stay in sync!
extern "C" void errorFunction(void * ctx, const char *msg, ...)
{
#if defined(HAVE_VSNPRINTF) && defined(HAVE_PROTOTYPE_VSNPRINTF)
    // Classic C requires us to declare variables at the beginning of the function.
    OFString &buffer = *OFstatic_cast(OFString*, ctx);
#endif

    if (!xmlLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
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
        OFLOG_DEBUG(xmlLogger, buffer.substr(0, pos));
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


static OFBool convertUtf8ToCharset(const xmlChar *fromString,
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


static OFCondition checkNode(xmlNodePtr current,
                             const char *name)
{
    OFCondition result = EC_Normal;
    /* check whether node is valid at all */
    if (current != NULL)
    {
        /* check whether node has expected name */
        if (xmlStrcmp(current->name, OFreinterpret_cast(const xmlChar *, name)) != 0)
        {
            OFLOG_ERROR(xml2dcmLogger, "document of the wrong type, was '" << current->name << "', '" << name << "' expected");
            result = EC_IllegalCall;
        }
    } else {
        OFLOG_ERROR(xml2dcmLogger, "document of the wrong type, '" << name << "' expected");
        result = EC_IllegalCall;
    }
    return result;
}


static OFCondition createNewElement(xmlNodePtr current,
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
        if (sscanf(OFreinterpret_cast(char *, elemTag), "%x,%x", &group, &elem ) == 2)
        {
            dcmTagKey.set(OFstatic_cast(Uint16, group), OFstatic_cast(Uint16, elem));
            DcmTag dcmTag(dcmTagKey);
            /* convert vr string */
            DcmVR dcmVR(OFreinterpret_cast(char *, elemVR));
            DcmEVR dcmEVR = dcmVR.getEVR();
            if (dcmEVR == EVR_UNKNOWN)
            {
                OFLOG_WARN(xml2dcmLogger, "invalid 'vr' attribute (" << elemVR
                    << ") for " << dcmTag << ", using unknown VR");
            }
            /* check for correct vr */
            const DcmEVR tagEVR = dcmTag.getEVR();
            if ((tagEVR != dcmEVR) && (dcmEVR != EVR_UNKNOWN) && (tagEVR != EVR_UNKNOWN) &&
                ((dcmTagKey != DCM_LUTData) || ((dcmEVR != EVR_US) && (dcmEVR != EVR_SS) && (dcmEVR != EVR_OW))) &&
                ((tagEVR != EVR_xs) || ((dcmEVR != EVR_US) && (dcmEVR != EVR_SS))) &&
                ((tagEVR != EVR_ox) || ((dcmEVR != EVR_OB) && (dcmEVR != EVR_OW))))
            {
                OFLOG_WARN(xml2dcmLogger, "tag " << dcmTag << " has wrong VR (" << dcmVR.getVRName()
                    << "), correct is " << dcmTag.getVR().getVRName());
            }
            if (dcmEVR != EVR_UNKNOWN)
                dcmTag.setVR(dcmVR);
            /* create DICOM element */
            result = DcmItem::newDicomElementWithVR(newElem, dcmTag);
        } else {
            OFLOG_WARN(xml2dcmLogger, "invalid 'tag' attribute (" << elemTag << "), ignoring node");
            result = EC_InvalidTag;
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


static OFCondition putElementContent(xmlNodePtr current,
                                     DcmElement *element)
{
    OFCondition result = EC_IllegalCall;
    /* check whether node and element are valid */
    if ((current != NULL) && (element != NULL))
    {
        DcmEVR dcmEVR = element->getVR();
        /* get the XML node content */
        xmlChar *elemVal = xmlNodeGetContent(current);
        xmlChar *attrVal = xmlGetProp(current, OFreinterpret_cast(const xmlChar *, "binary"));
        /* check whether node content is present */
        if (xmlStrcmp(attrVal, OFreinterpret_cast(const xmlChar *, "hidden")) == 0)
            OFLOG_WARN(xml2dcmLogger, "content of node " << element->getTag() << " is 'hidden', empty element inserted");
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
                        /* read binary file into the buffer */
                        if (fread(buf, 1, OFstatic_cast(size_t, fileSize), f) != fileSize)
                        {
                            OFLOG_ERROR(xml2dcmLogger, "error reading binary data file: " << filename << ": " << OFStandard::getLastSystemErrorCode().message());
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
                    OFLOG_ERROR(xml2dcmLogger, "cannot open binary data file: " << filename);
                    result = EC_InvalidTag;
                }
            } else
                OFLOG_ERROR(xml2dcmLogger, "filename for element " << element->getTag() << " is missing, empty element inserted");
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
        }
        /* free allocated memory */
        xmlFree(elemVal);
        xmlFree(attrVal);
    }
    return result;
}


static OFCondition parseElement(DcmItem *dataset,
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
            else if (xmlStrcmp(elemVal, OFreinterpret_cast(const xmlChar *, "ISO_IR 148")) == 0)
                encString = "ISO-8859-9";
            else if (xmlStrcmp(elemVal, OFreinterpret_cast(const xmlChar *, "ISO_IR 144")) == 0)
                encString = "ISO-8859-5";
            else if (xmlStrcmp(elemVal, OFreinterpret_cast(const xmlChar *, "ISO_IR 127")) == 0)
                encString = "ISO-8859-6";
            else if (xmlStrcmp(elemVal, OFreinterpret_cast(const xmlChar *, "ISO_IR 126")) == 0)
                encString = "ISO-8859-7";
            else if (xmlStrcmp(elemVal, OFreinterpret_cast(const xmlChar *, "ISO_IR 138")) == 0)
                encString = "ISO-8859-8";
            else if (xmlStrlen(elemVal) > 0)
                OFLOG_ERROR(xml2dcmLogger, "character set '" << elemVal << "' not supported");
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


// forward declaration
static OFCondition parseDataSet(DcmItem *dataset,
                                xmlNodePtr current,
                                E_TransferSyntax xfer);


static OFCondition parseSequence(DcmSequenceOfItems *sequence,
                                 xmlNodePtr current,
                                 E_TransferSyntax xfer)
{
    OFCondition result = EC_IllegalCall;
    if (sequence != NULL)
    {
        /* ignore blank (empty or whitespace only) nodes */
        while ((current != NULL) && xmlIsBlankNode(current))
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
                    parseDataSet(newItem, current->xmlChildrenNode, xfer);
                }
            } else if (!xmlIsBlankNode(current))
                OFLOG_WARN(xml2dcmLogger, "unexpected node '" << current->name << "', 'item' expected, skipping");
            /* proceed with next node */
            current = current->next;
        }
        result = EC_Normal;
    }
    return result;
}


static OFCondition parsePixelSequence(DcmPixelSequence *sequence,
                                      xmlNodePtr current)
{
    OFCondition result = EC_IllegalCall;
    if (sequence != NULL)
    {
        /* ignore blank (empty or whitespace only) nodes */
        while ((current != NULL) && xmlIsBlankNode(current))
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
                    putElementContent(current, newItem);
                }
            } else if (!xmlIsBlankNode(current))
                OFLOG_WARN(xml2dcmLogger, "unexpected node '" << current->name << "', 'pixel-item' expected, skipping");
            /* proceed with next node */
            current = current->next;
        }
        result = EC_Normal;
    }
    return result;
}


static OFCondition parseMetaHeader(DcmMetaInfo *metainfo,
                                   xmlNodePtr current,
                                   const OFBool parse)
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
                parseElement(metainfo, current);
            else if (!xmlIsBlankNode(current))
                OFLOG_WARN(xml2dcmLogger, "unexpected node '" << current->name << "', 'element' expected, skipping");
            /* proceed with next node */
            current = current->next;
        }
    }
    return result;
}


static OFCondition parseDataSet(DcmItem *dataset,
                                xmlNodePtr current,
                                E_TransferSyntax xfer)
{
    OFCondition result = EC_Normal;
    /* ignore blank (empty or whitespace only) nodes */
    while ((current != NULL) && xmlIsBlankNode(current))
        current = current->next;
    while (current != NULL)
    {
        /* ignore non-element/sequence nodes */
        if (xmlStrcmp(current->name, OFreinterpret_cast(const xmlChar *, "element")) == 0)
            parseElement(dataset, current);
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
                            /* ... insert it into the dataset and proceed with the pixel items */
                            OFstatic_cast(DcmPixelData *, newElem)->putOriginalRepresentation(xfer, NULL, sequence);
                            parsePixelSequence(sequence, current->xmlChildrenNode);
                        }
                    } else {
                        /* proceed parsing the items of the sequence */
                        parseSequence(OFstatic_cast(DcmSequenceOfItems *, newElem), current->xmlChildrenNode, xfer);
                    }
                } else {
                    /* delete element if insertion failed */
                    delete newElem;
                }
            }
        } else if (!xmlIsBlankNode(current))
            OFLOG_WARN(xml2dcmLogger, "unexpected node '" << current->name << "', skipping");
        /* proceed with next node */
        current = current->next;
    }
    return result;
}


static OFCondition validateXmlDocument(xmlDocPtr doc)
{
    OFCondition result = EC_Normal;
    OFLOG_INFO(xml2dcmLogger, "validating XML document ...");
    xmlGenericError(xmlGenericErrorContext, "--- libxml validating ---\n");
    /* temporary buffer needed for errorFunction - more detailed explanation there */
    OFString tmpErrorString;
    /* create context for document validation */
    xmlValidCtxt cvp;
    cvp.userData = &tmpErrorString;
    cvp.error = errorFunction;
    cvp.warning = errorFunction;
    /* validate the document */
    const int valid = xmlValidateDocument(&cvp, doc);
    xmlGenericError(xmlGenericErrorContext, "-------------------------\n");
    if (!valid)
    {
        OFLOG_ERROR(xml2dcmLogger, "document does not validate");
        result = EC_IllegalCall;
    }
    return result;
}


static OFCondition readXmlFile(const char *ifname,
                               DcmFileFormat &fileformat,
                               E_TransferSyntax &xfer,
                               const OFBool metaInfo,
                               const OFBool checkNamespace,
                               const OFBool validateDocument)
{
    OFCondition result = EC_Normal;
    xfer = EXS_Unknown;
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
                        OFLOG_INFO(xml2dcmLogger, "parsing file-format ...");
                        if (metaInfo)
                            OFLOG_INFO(xml2dcmLogger, "parsing meta-header ...");
                        else
                            OFLOG_INFO(xml2dcmLogger, "skipping meta-header ...");

                        current = current->xmlChildrenNode;
                        /* ignore blank (empty or whitespace only) nodes */
                        while ((current != NULL) && xmlIsBlankNode(current))
                            current = current->next;
                        /* parse/skip "meta-header" */
                        result = parseMetaHeader(fileformat.getMetaInfo(), current, metaInfo /*parse*/);
                        if (result.good())
                        {
                            current = current->next;
                            /* ignore blank (empty or whitespace only) nodes */
                            while ((current != NULL) && xmlIsBlankNode(current))
                                current = current->next;
                        }
                    }
                    /* there should always be a "data-set" node */
                    if (result.good())
                    {
                        OFLOG_INFO(xml2dcmLogger, "parsing data-set ...");
                        /* parse "data-set" */
                        result = checkNode(current, "data-set");
                        if (result.good())
                        {
                            DcmDataset *dataset = fileformat.getDataset();
                            /* determine stored transfer syntax */
                            xmlChar *xferUID = xmlGetProp(current, OFreinterpret_cast(const xmlChar *, "xfer"));
                            if (xferUID != NULL)
                                xfer = DcmXfer(OFreinterpret_cast(char *, xferUID)).getXfer();
                            result = parseDataSet(dataset, current->xmlChildrenNode, xfer);
                            /* free allocated memory */
                            xmlFree(xferUID);
                        }
                    }
                    if (result.bad() && xmlLogger.isEnabledFor(OFLogger::DEBUG_LOG_LEVEL))
                    {
                        /* dump XML document for debugging purposes */
                        xmlChar *str;
                        int size;
                        xmlDocDumpFormatMemory(doc, &str, &size, 1);
                        OFLOG_DEBUG(xmlLogger, str);
                        xmlFree(str);
                    }
                } else {
                    OFLOG_ERROR(xml2dcmLogger, "document has wrong type, dcmtk namespace not found");
                    result = EC_IllegalCall;
                }
            } else {
                OFLOG_ERROR(xml2dcmLogger, "document is empty: " << ifname);
                result = EC_IllegalCall;
            }
        }
    } else {
        OFLOG_ERROR(xml2dcmLogger, "could not parse document: " << ifname);
        result = EC_IllegalCall;
    }
    /* free allocated memory */
    xmlFreeDoc(doc);
    return result;
}


#define SHORTCOL 3
#define LONGCOL 21


int main(int argc, char *argv[])
{
    OFBool opt_metaInfo = OFTrue;
    OFBool opt_namespace = OFFalse;
    OFBool opt_validate = OFFalse;
    OFBool opt_generateUIDs = OFFalse;
    OFBool opt_overwriteUIDs = OFFalse;
    E_TransferSyntax opt_xfer = EXS_Unknown;
    E_EncodingType opt_enctype = EET_ExplicitLength;
    E_GrpLenEncoding opt_glenc = EGL_recalcGL;
    E_PaddingEncoding opt_padenc = EPD_withoutPadding;
    E_FileWriteMode opt_writeMode = EWM_fileformat;
    OFCmdUnsignedInt opt_filepad = 0;
    OFCmdUnsignedInt opt_itempad = 0;

    /* set-up command line parameters and options */
    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION, OFFIS_CONSOLE_DESCRIPTION, rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

    cmd.addParam("xmlfile-in",  "XML input filename to be converted (stdin: \"-\")", OFCmdParam::PM_Mandatory);
    cmd.addParam("dcmfile-out", "DICOM output filename", OFCmdParam::PM_Mandatory);

    cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                  "-h",     "print this help text and exit", OFCommandLine::AF_Exclusive);
      cmd.addOption("--version",                         "print version information and exit", OFCommandLine::AF_Exclusive);
      OFLog::addOptions(cmd);

    cmd.addGroup("input options:");
      cmd.addSubGroup("input file format:");
        cmd.addOption("--read-meta-info",      "+f",     "read meta information if present (default)");
        cmd.addOption("--ignore-meta-info",    "-f",     "ignore file meta information");

    cmd.addGroup("processing options:");
      cmd.addSubGroup("validation:");
        cmd.addOption("--validate-document",   "+Vd",    "validate XML document against DTD");
        cmd.addOption("--check-namespace",     "+Vn",    "check XML namespace in document root");
      cmd.addSubGroup("unique identifiers:");
        cmd.addOption("--generate-new-uids",   "+Ug",    "generate new Study/Series/SOP Instance UID");
        cmd.addOption("--dont-overwrite-uids", "-Uo",    "do not overwrite existing UIDs (default)");
        cmd.addOption("--overwrite-uids",      "+Uo",    "overwrite existing UIDs");

    cmd.addGroup("output options:");
      cmd.addSubGroup("output file format:");
        cmd.addOption("--write-file",          "+F",     "write file format (default)");
        cmd.addOption("--write-dataset",       "-F",     "write data set without file meta information");
        cmd.addOption("--update-meta-info",    "+Fu",    "update particular file meta information");
      cmd.addSubGroup("output transfer syntax:");
        cmd.addOption("--write-xfer-same",     "+t=",    "write with same TS as input (default)");
        cmd.addOption("--write-xfer-little",   "+te",    "write with explicit VR little endian TS");
        cmd.addOption("--write-xfer-big",      "+tb",    "write with explicit VR big endian TS");
        cmd.addOption("--write-xfer-implicit", "+ti",    "write with implicit VR little endian TS");
#ifdef WITH_ZLIB
        cmd.addOption("--write-xfer-deflated", "+td",    "write with deflated expl. VR little endian TS");
#endif
      cmd.addSubGroup("post-1993 value representations:");
        cmd.addOption("--enable-new-vr",       "+u",     "enable support for new VRs (UN/UT) (default)");
        cmd.addOption("--disable-new-vr",      "-u",     "disable support for new VRs, convert to OB");
      cmd.addSubGroup("group length encoding:");
        cmd.addOption("--group-length-recalc", "+g=",    "recalculate group lengths if present (default)");
        cmd.addOption("--group-length-create", "+g",     "always write with group length elements");
        cmd.addOption("--group-length-remove", "-g",     "always write without group length elements");
      cmd.addSubGroup("length encoding in sequences and items:");
        cmd.addOption("--length-explicit",     "+e",     "write with explicit lengths (default)");
        cmd.addOption("--length-undefined",    "-e",     "write with undefined lengths");
      cmd.addSubGroup("data set trailing padding (not with --write-dataset):");
        cmd.addOption("--padding-retain",      "-p=",    "do not change padding\n(default if not --write-dataset)");
        cmd.addOption("--padding-off",         "-p",     "no padding (implicit if --write-dataset)");
        cmd.addOption("--padding-create",      "+p",  2, "[f]ile-pad [i]tem-pad: integer",
                                                         "align file on multiple of f bytes\nand items on multiple of i bytes");
#ifdef WITH_ZLIB
      cmd.addSubGroup("deflate compression level (only with --write-xfer-deflated):");
        cmd.addOption("--compression-level",   "+cl", 1, "[l]evel: integer (default: 6)",
                                                         "0=uncompressed, 1=fastest, 9=best compression");
#endif

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv))
    {
        /* check exclusive options first */
        if (cmd.hasExclusiveOption())
        {
            if (cmd.findOption("--version"))
            {
                app.printHeader(OFTrue /*print host identifier*/);
                COUT << OFendl << "External libraries used:" << OFendl;
#ifdef WITH_ZLIB
                COUT << "- ZLIB, Version " << zlibVersion() << OFendl;
#endif
                COUT << "- LIBXML, Version " << LIBXML_DOTTED_VERSION << OFendl;
#if defined(LIBXML_ICONV_ENABLED) && defined(LIBXML_ZLIB_ENABLED)
                COUT << "  with built-in LIBICONV and ZLIB support" << OFendl;
#elif defined(LIBXML_ICONV_ENABLED)
                COUT << "  with built-in LIBICONV support" << OFendl;
#elif defined(LIBXML_ZLIB_ENABLED)
                COUT << "  with built-in ZLIB support" << OFendl;
#endif
                return 0;
            }
        }

        /* general options */
        OFLog::configureFromCommandLine(cmd, app);

        /* input options */

        cmd.beginOptionBlock();
        if (cmd.findOption("--read-meta-info"))
            opt_metaInfo = OFTrue;
        if (cmd.findOption("--ignore-meta-info"))
            opt_metaInfo = OFFalse;
        cmd.endOptionBlock();

        /* processing options */

        if (cmd.findOption("--validate-document"))
            opt_validate = OFTrue;
        if (cmd.findOption("--check-namespace"))
            opt_namespace = OFTrue;

        if (cmd.findOption("--generate-new-uids"))
            opt_generateUIDs = OFTrue;

        cmd.beginOptionBlock();
        if (cmd.findOption("--dont-overwrite-uids"))
        {
            app.checkDependence("--dont-overwrite-uids", "--generate-new-uids", opt_generateUIDs);
            opt_overwriteUIDs = OFFalse;
        }
        if (cmd.findOption("--overwrite-uids"))
        {
            app.checkDependence("--overwrite-uids", "--generate-new-uids", opt_generateUIDs);
            opt_overwriteUIDs = OFTrue;
        }
        cmd.endOptionBlock();

        /* output options */

        cmd.beginOptionBlock();
        if (cmd.findOption("--write-file"))
            opt_writeMode = EWM_fileformat;
        if (cmd.findOption("--write-dataset"))
            opt_writeMode = EWM_dataset;
        cmd.endOptionBlock();

        if (cmd.findOption("--update-meta-info"))
        {
            app.checkConflict("--update-meta-info", "--write-dataset", opt_writeMode == EWM_dataset);
            opt_writeMode = EWM_updateMeta;
        }

        cmd.beginOptionBlock();
        if (cmd.findOption("--write-xfer-same"))
            opt_xfer = EXS_Unknown;
        if (cmd.findOption("--write-xfer-little"))
            opt_xfer = EXS_LittleEndianExplicit;
        if (cmd.findOption("--write-xfer-big"))
            opt_xfer = EXS_BigEndianExplicit;
        if (cmd.findOption("--write-xfer-implicit"))
            opt_xfer = EXS_LittleEndianImplicit;
#ifdef WITH_ZLIB
        if (cmd.findOption("--write-xfer-deflated"))
            opt_xfer = EXS_DeflatedLittleEndianExplicit;
#endif
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--enable-new-vr"))
            dcmEnableGenerationOfNewVRs();
        if (cmd.findOption("--disable-new-vr"))
            dcmDisableGenerationOfNewVRs();
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--group-length-recalc"))
            opt_glenc = EGL_recalcGL;
        if (cmd.findOption("--group-length-create"))
            opt_glenc = EGL_withGL;
        if (cmd.findOption("--group-length-remove"))
            opt_glenc = EGL_withoutGL;
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--length-explicit"))
            opt_enctype = EET_ExplicitLength;
        if (cmd.findOption("--length-undefined"))
            opt_enctype = EET_UndefinedLength;
        cmd.endOptionBlock();

        cmd.beginOptionBlock();
        if (cmd.findOption("--padding-retain"))
        {
            app.checkConflict("--padding-retain", "--write-dataset", opt_writeMode == EWM_dataset);
            opt_padenc = EPD_noChange;
        }
        if (cmd.findOption("--padding-off"))
            opt_padenc = EPD_withoutPadding;
        if (cmd.findOption("--padding-create"))
        {
            app.checkConflict("--padding-create", "--write-dataset", opt_writeMode == EWM_dataset);
            app.checkValue(cmd.getValueAndCheckMin(opt_filepad, 0));
            app.checkValue(cmd.getValueAndCheckMin(opt_itempad, 0));
            opt_padenc = EPD_withPadding;
        }
        cmd.endOptionBlock();

#ifdef WITH_ZLIB
        if (cmd.findOption("--compression-level"))
        {
            OFCmdUnsignedInt comprLevel = 0;
            app.checkDependence("--compression-level", "--write-xfer-deflated", opt_xfer == EXS_DeflatedLittleEndianExplicit);
            app.checkValue(cmd.getValueAndCheckMinMax(comprLevel, 0, 9));
            dcmZlibCompressionLevel.set(OFstatic_cast(int, comprLevel));
        }
#endif
    }

    /* print resource identifier */
    OFLOG_DEBUG(xml2dcmLogger, rcsid << OFendl);

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
        OFLOG_WARN(xml2dcmLogger, "no data dictionary loaded, check environment variable: "
            << DCM_DICT_ENVIRONMENT_VARIABLE);
    }

    /* check for compatible libxml version */
    LIBXML_TEST_VERSION
    /* temporary buffer needed for errorFunction - more detailed explanation there */
    OFString tmpErrorString;
    /* initialize the XML library (only required for MT-safety) */
    xmlInitParser();
    /* substitute default entities (XML mnenonics) */
    xmlSubstituteEntitiesDefault(1);
    /* add line number to debug messages */
    xmlLineNumbersDefault(1);
    xmlGetWarningsDefaultValue = 1;
    xmlSetGenericErrorFunc(&tmpErrorString, errorFunction);

    OFCondition result = EC_Normal;
    const char *opt_ifname = NULL;
    const char *opt_ofname = NULL;
    cmd.getParam(1, opt_ifname);
    cmd.getParam(2, opt_ofname);

    /* check filenames */
    if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
    {
        OFLOG_ERROR(xml2dcmLogger, OFFIS_CONSOLE_APPLICATION << ": invalid input filename: <empty string>");
        result = EC_IllegalParameter;
    }
    if ((opt_ofname == NULL) || (strlen(opt_ofname) == 0))
    {
        OFLOG_ERROR(xml2dcmLogger, OFFIS_CONSOLE_APPLICATION << ": invalid output filename: <empty string>");
        result = EC_IllegalParameter;
    }

    if (result.good())
    {
        DcmFileFormat fileformat;
        E_TransferSyntax xfer;
        OFLOG_INFO(xml2dcmLogger, "reading XML input file: " << opt_ifname);
        /* read XML file and feed data into DICOM fileformat */
        result = readXmlFile(opt_ifname, fileformat, xfer, opt_metaInfo, opt_namespace, opt_validate);
        if (result.good())
        {
            DcmDataset *dataset = fileformat.getDataset();
            /* generate new UIDs (if required) */
            if (opt_generateUIDs)
            {
                char uid[100];
                if (opt_overwriteUIDs || !dataset->tagExistsWithValue(DCM_StudyInstanceUID))
                {
                    OFLOG_INFO(xml2dcmLogger, "generating new Study Instance UID");
                    dataset->putAndInsertString(DCM_StudyInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_STUDY_UID_ROOT));
                }
                if (opt_overwriteUIDs || !dataset->tagExistsWithValue(DCM_SeriesInstanceUID))
                {
                    OFLOG_INFO(xml2dcmLogger, "generating new Series Instance UID");
                    dataset->putAndInsertString(DCM_SeriesInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_SERIES_UID_ROOT));
                }
                if (opt_overwriteUIDs || !dataset->tagExistsWithValue(DCM_SOPInstanceUID))
                {
                    OFLOG_INFO(xml2dcmLogger, "generating new SOP Instance UID");
                    dataset->putAndInsertString(DCM_SOPInstanceUID, dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT));
                }
            }
            OFLOG_INFO(xml2dcmLogger, "writing DICOM output file: " << opt_ofname);
            /* determine transfer syntax to write the file */
            if ((opt_xfer == EXS_Unknown) && (xfer != EXS_Unknown))
                opt_xfer = xfer;
            /* check whether this is possible */
            if (fileformat.canWriteXfer(opt_xfer))
            {
                /* check whether pixel data is compressed */
                if ((opt_writeMode == EWM_dataset) && DcmXfer(xfer).isEncapsulated())
                {
                    OFLOG_ERROR(xml2dcmLogger, "encapsulated pixel data require file format, ignoring --write-dataset");
                    opt_writeMode = EWM_fileformat;
                }
                /* write DICOM file */
                result = fileformat.saveFile(opt_ofname, opt_xfer, opt_enctype, opt_glenc, opt_padenc,
                    OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad), opt_writeMode);
                if (result.bad())
                    OFLOG_ERROR(xml2dcmLogger, result.text() << ": writing file: "  << opt_ofname);
            } else {
                OFLOG_ERROR(xml2dcmLogger, "no conversion to transfer syntax " << DcmXfer(opt_xfer).getXferName() << " possible!");
                result = EC_CannotChangeRepresentation;
            }
        }
    }

    /* clean up XML library before quitting */
    xmlCleanupParser();

    return result.status();
}

#else /* WITH_LIBXML */

int main(int, char *[])
{
  CERR << rcsid << OFendl << OFFIS_CONSOLE_DESCRIPTION << OFendl << OFendl
       << OFFIS_CONSOLE_APPLICATION " requires the libxml library." << OFendl
       << "This " OFFIS_CONSOLE_APPLICATION " has been configured and compiled without libxml." << OFendl
       << "Please reconfigure your system and recompile if appropriate." << OFendl;
  return 0;
}

#endif /* WITH_LIBXML */
