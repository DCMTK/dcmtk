/*
 *
 *  Copyright (C) 2003-2010, OFFIS e.V.
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
 *    classes: DSRXMLDocument
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-12-15 13:59:58 $
 *  CVS/RCS Revision: $Revision: 1.19 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrxmld.h"

#define INCLUDE_CSTDARG
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef WITH_LIBXML
#include <libxml/xmlversion.h>

#ifdef LIBXML_SCHEMAS_ENABLED
#include <libxml/xmlschemas.h>
#endif /* LIBXML_SCHEMAS_ENABLED */

// This function is also used in xml2dcm, try to stay in sync!
extern "C" void errorFunction(void * ctx, const char *msg, ...)
{
    OFString &buffer = *OFstatic_cast(OFString*, ctx);
    OFLogger xmlLogger = OFLog::getLogger("dcmtk.dcmsr.libxml");

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

#ifndef HAVE_VSNPRINTF
    // Only the vsnprintf() branch above uses 'buffer' which means the compiler
    // would warn about an unused variable if HAVE_VSNPRINTF is undefined.
    buffer += "";
#endif
}

#endif /* WITH_LIBXML */


/* ------------------------ */


DSRXMLDocument::DSRXMLDocument()
  : Document(NULL),
    EncodingHandler(NULL)
{
}


DSRXMLDocument::~DSRXMLDocument()
{
    clear();
}


void DSRXMLDocument::clear()
{
#ifdef WITH_LIBXML
    /* free allocated memory */
    xmlFreeDoc(Document);
#endif
    /* remove all references to libxml structures */
    Document = NULL;
    EncodingHandler = NULL;
}


OFBool DSRXMLDocument::valid() const
{
    return (Document != NULL);
}


#ifdef WITH_LIBXML
OFCondition DSRXMLDocument::read(const OFString &filename,
                                 const size_t flags)
{
    OFCondition result = SR_EC_InvalidDocument;
    /* temporary buffer needed for errorFunction - more detailed explanation there */
    OFString tmpErrorString;
    /* first remove any possibly existing document from memory */
    clear();
    /* substitute default entities (XML mnenonics) */
    xmlSubstituteEntitiesDefault(1);
    /* add line number to debug messages */
    xmlLineNumbersDefault(1);
    /* enable libxml warnings and error messages */
    xmlGetWarningsDefaultValue = 1;
    xmlSetGenericErrorFunc(&tmpErrorString, errorFunction);

    xmlGenericError(xmlGenericErrorContext, "--- libxml parsing ------\n");
    /* build an XML tree from the given file */
    Document = xmlParseFile(filename.c_str());
    if (Document != NULL)
    {
        OFBool isValid = OFTrue;
        /* validate Schema */
        if (flags & XF_validateSchema)
        {
            xmlGenericError(xmlGenericErrorContext, "--- libxml validating ---\n");
#ifdef LIBXML_SCHEMAS_ENABLED
#if 1
            /* create context for Schema validation */
            xmlSchemaParserCtxtPtr context = xmlSchemaNewParserCtxt(DCMSR_XML_XSD_FILE);
            xmlSchemaSetParserErrors(context, errorFunction, errorFunction, &tmpErrorString);
            /* parse Schema file */
            xmlSchemaPtr schema = xmlSchemaParse(context);
            if (schema != NULL)
            {
                xmlSchemaValidCtxtPtr validCtx = xmlSchemaNewValidCtxt(schema);
                xmlSchemaSetValidErrors(validCtx, errorFunction, errorFunction, &tmpErrorString);
                /* validate the document */
                isValid = (xmlSchemaValidateDoc(validCtx, Document) == 0);
                xmlSchemaFreeValidCtxt(validCtx);
                xmlSchemaFree(schema);
            } else
                xmlGenericError(xmlGenericErrorContext, "error: failed to compile schema \"%s\"\n", DCMSR_XML_XSD_FILE);
            xmlSchemaFreeParserCtxt(context);
#else // 0
            /* ### the following code fragment is not yet working! ### */

            /* create context for Schema validation */
            xmlSchemaValidCtxtPtr context = xmlSchemaNewValidCtxt(NULL);
            xmlSchemaSetValidErrors(context, errorFunction, errorFunction, &tmpErrorString);
            /* validate the document */
            isValid = (xmlSchemaValidateDoc(context, Document) == 0);
#endif
#else /* LIBXML_SCHEMAS_ENABLED */
            xmlGenericError(xmlGenericErrorContext, "no support for XML Schema\n");
#endif
        }
        xmlGenericError(xmlGenericErrorContext, "-------------------------\n");
        /* check whether the document is of the right kind */
        xmlNodePtr current = xmlDocGetRootElement(Document);
        if ((current != NULL) && isValid)
        {
            /* check namespace declaration (if required) */
            if (!(flags & XF_useDcmsrNamespace) ||
                (xmlSearchNsByHref(Document, current, OFreinterpret_cast(const xmlChar *, DCMSR_XML_NAMESPACE_URI)) != NULL))
            {
                result = EC_Normal;
            } else
                DCMSR_ERROR("Document has wrong type, dcmsr namespace not found");
        } else {
            if (isValid)
                DCMSR_ERROR("Document is empty");
            else
                DCMSR_ERROR("Document does not validate");
        }
    } else {
        xmlGenericError(xmlGenericErrorContext, "-------------------------\n");
        DCMSR_ERROR("Could not parse document");
    }

    /* Reset to default function because we used a local string as context for
     * the error function.
     */
    xmlSetGenericErrorFunc(NULL, NULL);
    return result;
}
#else /* WITH_LIBXML */
OFCondition DSRXMLDocument::read(const OFString &,
                                 const size_t)
{
    return EC_IllegalCall;
}
#endif


OFBool DSRXMLDocument::encodingHandlerValid() const
{
    return (EncodingHandler != NULL);
}


#ifdef WITH_LIBXML
OFCondition DSRXMLDocument::setEncodingHandler(const char *charset)
{
    OFCondition result = EC_IllegalParameter;
    if ((charset != NULL) && (strlen(charset) > 0))
    {
        /* find appropriate encoding handler */
        EncodingHandler = xmlFindCharEncodingHandler(charset);
        if (EncodingHandler != NULL)
            result = EC_Normal;
    }
    return result;
}
#else /* WITH_LIBXML */
OFCondition DSRXMLDocument::setEncodingHandler(const char *)
{
    return EC_IllegalCall;
}
#endif


DSRXMLCursor DSRXMLDocument::getRootNode() const
{
    DSRXMLCursor cursor;
#ifdef WITH_LIBXML
    /* set cursor to root node */
    cursor.Node = xmlDocGetRootElement(Document);
#endif
    return cursor;
}


#ifdef WITH_LIBXML
DSRXMLCursor DSRXMLDocument::getNamedNode(const DSRXMLCursor &cursor,
                                          const char *name,
                                          const OFBool required) const
{
    DSRXMLCursor result;
    /* check whether given name is valid */
    if ((name != NULL) && (strlen(name) > 0))
    {
        xmlNodePtr current = cursor.Node;
        /* iterate over all nodes */
        while (current != NULL)
        {
            /* ignore blank (empty or whitespace only) nodes */
            while ((current != NULL) && xmlIsBlankNode(current))
                current = current->next;
            if (current != NULL)
            {
                /* check whether node has expected name */
                if (xmlStrcmp(current->name, OFreinterpret_cast(const xmlChar *, name)) == 0)
                    break;
                /* proceed with next node */
                current = current->next;
            }
        }
        if (current == NULL)
        {
            /* report error message */
            if (required)
            {
                OFString tmpString;
                DCMSR_ERROR("Document of the wrong type, '" << name
                    << "' expected at " << getFullNodePath(cursor, tmpString, OFFalse /*omitCurrent*/));
            }
        } else {
            /* return new node position */
            result.Node = current;
        }
    }
    return result;
}
#else /* WITH_LIBXML */
DSRXMLCursor DSRXMLDocument::getNamedNode(const DSRXMLCursor &,
                                          const char *,
                                          const OFBool) const
{
    DSRXMLCursor result;
    return result;
}
#endif


#ifdef WITH_LIBXML
OFBool DSRXMLDocument::matchNode(const DSRXMLCursor &cursor,
                                 const char *name) const
{
    OFBool result = OFFalse;
    if (cursor.Node != NULL)
    {
        /* check whether node name matches */
        if ((name != NULL) && (strlen(name) > 0))
            result = (xmlStrcmp(cursor.Node->name, OFreinterpret_cast(const xmlChar *, name)) == 0);
    }
    return result;
}
#else /* WITH_LIBXML */
OFBool DSRXMLDocument::matchNode(const DSRXMLCursor &,
                                 const char *) const
{
    return OFFalse;
}
#endif


#ifdef WITH_LIBXML
OFCondition DSRXMLDocument::checkNode(const DSRXMLCursor &cursor,
                                      const char *name) const
{
    OFCondition result = EC_IllegalParameter;
    /* check whether parameters are valid */
    if ((name != NULL) && (strlen(name) > 0))
    {
        /* check whether node is valid at all */
        if (cursor.Node != NULL)
        {
            /* check whether node has expected name */
            if (xmlStrcmp(cursor.Node->name, OFreinterpret_cast(const xmlChar *, name)) != 0)
            {
                DCMSR_ERROR("Document of the wrong type, was '" << OFreinterpret_cast(const char *, cursor.Node->name)
                    << "', '" << name << "' expected");
                result = SR_EC_InvalidDocument;
            } else
                result = EC_Normal;
        } else {
            DCMSR_ERROR("Document of the wrong type, '" << name << "' expected");
            result = EC_IllegalParameter;
        }
    }
    return result;
}
#else /* WITH_LIBXML */
OFCondition DSRXMLDocument::checkNode(const DSRXMLCursor &,
                                      const char *) const
{
    return EC_IllegalCall;
}
#endif


#ifdef WITH_LIBXML
OFBool DSRXMLDocument::convertUtf8ToCharset(const xmlChar *fromString,
                                            OFString &toString) const
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
#else /* WITH_LIBXML */
OFBool DSRXMLDocument::convertUtf8ToCharset(const xmlChar *,
                                            OFString &) const
{
    return OFFalse;
}
#endif


#ifdef WITH_LIBXML
OFBool DSRXMLDocument::hasAttribute(const DSRXMLCursor &cursor,
                                    const char *name) const
{
    OFBool result = OFFalse;
    if (cursor.Node != NULL)
    {
        /* check whether attribute exists */
        if ((name != NULL) && (strlen(name) > 0))
            result = (xmlHasProp(cursor.Node, OFreinterpret_cast(const xmlChar *, name)) != NULL);
    }
    return result;
}
#else /* WITH_LIBXML */
OFBool DSRXMLDocument::hasAttribute(const DSRXMLCursor &,
                                    const char *) const
{
    return OFFalse;
}
#endif


#ifdef WITH_LIBXML
OFString &DSRXMLDocument::getStringFromAttribute(const DSRXMLCursor &cursor,
                                                 OFString &stringValue,
                                                 const char *name,
                                                 const OFBool encoding,
                                                 const OFBool required) const
{
    /* always clear result string */
    stringValue.clear();
    /* check whether parameters are valid */
    if ((cursor.Node != NULL) && (name != NULL) && (strlen(name) > 0))
    {
        /* get the XML attribute value */
        xmlChar *attrVal = xmlGetProp(cursor.Node, OFreinterpret_cast(const xmlChar *, name));
        if ((attrVal != NULL) && (xmlStrlen(attrVal) > 0))
        {
            /* put value to the result variable */
            if (!encoding || !convertUtf8ToCharset(attrVal, stringValue))
                stringValue = OFreinterpret_cast(const char *, attrVal);
        } else if (required)
            printMissingAttributeError(cursor, name);
        /* free allocated memory */
        xmlFree(attrVal);
    }
    return stringValue;
}
#else /* WITH_LIBXML */
OFString &DSRXMLDocument::getStringFromAttribute(const DSRXMLCursor &,
                                                 OFString &stringValue,
                                                 const char *,
                                                 const OFBool,
                                                 const OFBool) const
{
    stringValue.clear();
    return stringValue;
}
#endif


#ifdef WITH_LIBXML
OFCondition DSRXMLDocument::getElementFromAttribute(const DSRXMLCursor &cursor,
                                                    DcmElement &delem,
                                                    const char *name,
                                                    const OFBool encoding,
                                                    const OFBool required) const
{
    OFCondition result = SR_EC_InvalidDocument;
    /* check whether parameters are valid */
    if ((cursor.Node != NULL) && (name != NULL) && (strlen(name) > 0))
    {
        /* get the XML attribute value */
        xmlChar *attrVal = xmlGetProp(cursor.Node, OFreinterpret_cast(const xmlChar *, name));
        if ((attrVal != NULL) && (xmlStrlen(attrVal) > 0))
        {
            OFString attrStr;
            /* put value to DICOM element */
            if (encoding && convertUtf8ToCharset(attrVal, attrStr))
                result = delem.putString(attrStr.c_str());
            else
                result = delem.putString(OFreinterpret_cast(const char *, attrVal));
        } else if (required)
            printMissingAttributeError(cursor, name);
        /* free allocated memory */
        xmlFree(attrVal);
    }
    return result;
}
#else /* WITH_LIBXML */
OFCondition DSRXMLDocument::getElementFromAttribute(const DSRXMLCursor &,
                                                    DcmElement &,
                                                    const char *,
                                                    const OFBool,
                                                    const OFBool) const
{
    return EC_IllegalCall;
}
#endif


#ifdef WITH_LIBXML
OFString &DSRXMLDocument::getStringFromNodeContent(const DSRXMLCursor &cursor,
                                                   OFString &stringValue,
                                                   const char *name,
                                                   const OFBool encoding,
                                                   const OFBool clearString) const
{
    if (clearString)
        stringValue.clear();
    if (cursor.Node != NULL)
    {
        /* compare element name if required */
        if ((name == NULL) || (xmlStrcmp(cursor.Node->name, OFreinterpret_cast(const xmlChar *, name)) == 0))
        {

            /* get the XML node content */
            xmlChar *elemVal = xmlNodeGetContent(cursor.Node);
            /* put value to the result variable */
            if (!encoding || !convertUtf8ToCharset(elemVal, stringValue))
                stringValue = OFreinterpret_cast(const char *, elemVal);
            /* free allocated memory */
            xmlFree(elemVal);
        }
    }
    return stringValue;
}
#else /* WITH_LIBXML */
OFString &DSRXMLDocument::getStringFromNodeContent(const DSRXMLCursor &,
                                                   OFString &stringValue,
                                                   const char *,
                                                   const OFBool,
                                                   const OFBool) const
{
    stringValue.clear();
    return stringValue;
}
#endif


#ifdef WITH_LIBXML
OFCondition DSRXMLDocument::getElementFromNodeContent(const DSRXMLCursor &cursor,
                                                      DcmElement &delem,
                                                      const char *name,
                                                      const OFBool encoding) const
{
    OFCondition result = SR_EC_InvalidDocument;
    if (cursor.Node != NULL)
    {
        /* compare element name if required */
        if ((name == NULL) || (xmlStrcmp(cursor.Node->name, OFreinterpret_cast(const xmlChar *, name)) == 0))
        {
            OFString elemStr;
            /* get the XML node content */
            xmlChar *elemVal = xmlNodeGetContent(cursor.Node);
            /* put value to DICOM element */
            if (encoding && convertUtf8ToCharset(elemVal, elemStr))
                result = delem.putString(elemStr.c_str());
            else
                result = delem.putString(OFreinterpret_cast(const char *, elemVal));
            /* free allocated memory */
            xmlFree(elemVal);
        }
    }
    return result;
}
#else /* WITH_LIBXML */
OFCondition DSRXMLDocument::getElementFromNodeContent(const DSRXMLCursor &,
                                                      DcmElement &,
                                                      const char *,
                                                      const OFBool) const
{
    return EC_IllegalCall;
}
#endif


#ifdef WITH_LIBXML
OFString &DSRXMLDocument::getFullNodePath(const DSRXMLCursor &cursor,
                                          OFString &stringValue,
                                          const OFBool omitCurrent)
{
    stringValue.clear();
    if (cursor.Node != NULL)
    {
        OFString tmpString;
        xmlNodePtr current = cursor.Node;
        if (!omitCurrent)
            stringValue = OFSTRING_GUARD(OFreinterpret_cast(const char *, current->name));
        /* follow path to parent nodes */
        while (current->parent != NULL)
        {
            current = current->parent;
            tmpString = stringValue;
            stringValue = OFSTRING_GUARD(OFreinterpret_cast(const char *, current->name));
            if (!tmpString.empty())
            {
                stringValue += '/';
                stringValue += tmpString;
            }
        }
        /* avoid empty return value */
        if (stringValue.empty() && omitCurrent)
            stringValue = '.';
    } else
        stringValue = "<invalid>";
    return stringValue;
}
#else /* WITH_LIBXML */
OFString &DSRXMLDocument::getFullNodePath(const DSRXMLCursor &,
                                          OFString &stringValue,
                                          const OFBool)
{
    stringValue.clear();
    return stringValue;
}
#endif


#ifdef WITH_LIBXML
DSRTypes::E_ValueType DSRXMLDocument::getValueTypeFromNode(const DSRXMLCursor &cursor) const
{
    E_ValueType valueType = VT_invalid;
    if (cursor.valid())
    {
        if (xmlStrcmp(cursor.Node->name, OFreinterpret_cast(const xmlChar *, "item")) == 0)
        {
            /* check for "ref" attribute */
            if (xmlHasProp(cursor.Node, OFreinterpret_cast(const xmlChar *, "ref")))
                valueType = DSRTypes::VT_byReference;
            else {
                /* get the XML attribute value */
                xmlChar *attrVal = xmlGetProp(cursor.Node, OFreinterpret_cast(const xmlChar *, "valType"));
                /* try to convert attribute value to SR value type */
                valueType = definedTermToValueType(OFreinterpret_cast(const char *, attrVal));
                /* free allocated memory */
                xmlFree(attrVal);
            }
        } else {
            /* try to convert tag name to SR value type */
            valueType = xmlTagNameToValueType(OFreinterpret_cast(const char *, cursor.Node->name));
        }
    }
    return valueType;
}
#else /* WITH_LIBXML */
DSRTypes::E_ValueType DSRXMLDocument::getValueTypeFromNode(const DSRXMLCursor &) const
{
    return VT_invalid;
}
#endif


DSRTypes::E_RelationshipType DSRXMLDocument::getRelationshipTypeFromNode(const DSRXMLCursor &cursor) const
{
    E_RelationshipType relationshipType = RT_invalid;
    if (cursor.valid())
    {
        OFString tmpString;
        /* get the XML attribute value (if present) */
        if (hasAttribute(cursor, "relType"))
        {
            /* try to convert attribute value to SR relationship type */
            relationshipType = definedTermToRelationshipType(getStringFromAttribute(cursor, tmpString, "relType"));
        } else {
            const DSRXMLCursor childCursor = getNamedNode(cursor.getChild(), "relationship");
            /* try to convert content of "relationship" tag to SR relationship type */
            if (childCursor.valid())
                relationshipType = definedTermToRelationshipType(getStringFromNodeContent(childCursor, tmpString));
        }
    }
    return relationshipType;
}


void DSRXMLDocument::printUnexpectedNodeWarning(const DSRXMLCursor &cursor) const
{
    OFString tmpString;
    DCMSR_WARN("Unexpected node '" << getFullNodePath(cursor, tmpString) << "', skipping");
}


void DSRXMLDocument::printMissingAttributeError(const DSRXMLCursor &cursor,
                                                const char *name) const
{
    /* report error message */
    if (name != NULL)
    {
        OFString tmpString;
        DCMSR_ERROR("XML attribute '" << name << "' missing/empty in " << getFullNodePath(cursor, tmpString));
    }
}


void DSRXMLDocument::printGeneralNodeError(const DSRXMLCursor &cursor,
                                           const OFCondition &result) const
{
    /* report error message */
    if (result.bad())
    {
        OFString tmpString;
        DCMSR_ERROR("Parsing node " << getFullNodePath(cursor, tmpString) << " (" << result.text() << ")");
    }
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrxmld.cc,v $
 *  Revision 1.19  2010-12-15 13:59:58  uli
 *  Fixed a problem with a missing prototype for vsnprintf on HP-UX.
 *
 *  Revision 1.18  2010-10-14 13:14:42  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.17  2010-09-28 13:21:40  joergr
 *  Made sure that no NULL pointer is passed to the OFString constructor.
 *
 *  Revision 1.16  2010-06-15 14:34:51  uli
 *  Don't fail to compile if libxml does not support schemas.
 *
 *  Revision 1.15  2009-10-28 08:42:40  uli
 *  Use a local variable for buffering libxml error messages instead of a
 *  static one.
 *
 *  Revision 1.14  2009-10-14 10:49:33  joergr
 *  Fixed minor issues in log output. Also updated copyright date (if required).
 *
 *  Revision 1.13  2009-10-13 14:57:52  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.12  2008-04-25 15:37:20  joergr
 *  Include header file for XML Schemas only if Schema support is enabled.
 *
 *  Revision 1.11  2005/12/16 15:46:43  meichel
 *  Declared libxml2 callback functions as extern "C"
 *
 *  Revision 1.10  2005/12/08 15:48:25  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.9  2004/09/03 08:50:48  joergr
 *  Replaced tabs by spaces.
 *
 *  Revision 1.8  2004/08/04 12:12:18  joergr
 *  Disabled support for XML Schema if not compiled into libxml2 library.
 *
 *  Revision 1.7  2004/04/07 12:04:48  joergr
 *  Adapted code to avoid warnings reported by gcc when compiling without libxml
 *  support.
 *
 *  Revision 1.6  2004/03/25 17:32:41  joergr
 *  Solved issue with function pointer to std::fprintf or fprintf, respectively.
 *
 *  Revision 1.5  2004/01/21 11:56:20  meichel
 *  Using std::fprintf instead of fprintf as function pointer if configure
 *    symbol HAVE_PROTOTYPE_STD__FPRINTF is defined, needed on Borland Builder.
 *
 *  Revision 1.4  2004/01/05 14:37:00  joergr
 *  Renamed XML attribute "ref_id" to "ref".
 *
 *  Revision 1.3  2003/12/01 15:47:28  joergr
 *  Changed XML encoding of by-reference relationships if flag
 *  XF_valueTypeAsAttribute is set.
 *
 *  Revision 1.2  2003/08/07 15:21:53  joergr
 *  Added brackets around "bitwise and" operator/operands to avoid warnings
 *  reported by MSVC5.
 *
 *  Revision 1.1  2003/08/07 14:49:36  joergr
 *  Added interface classes hiding the access to libxml (document and cursor
 *  class).
 *
 *
 */
