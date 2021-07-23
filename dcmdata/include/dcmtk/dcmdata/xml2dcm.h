/*
 *
 *  Copyright (C) 2003-2021, OFFIS e.V.
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

#ifndef XML2DCM_H
#define XML2DCM_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef WITH_LIBXML

#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/dcmdata/dcxfer.h"

#ifdef __ibmxl__
// IBM xlC defines __GNUC__ but does not support the GNUC extension
// __attribute__ ((format (printf, 2, 3))).
// This avoids a compiler warning in <libxml/parser.h>.
#define LIBXML_ATTR_FORMAT(fmt,args)
#endif

// The libxml library also uses unicode. So we have to reuse some
// workarounds for the ICU library here as well.
// The type char16_t is only supported since C++11.
#ifndef HAVE_CHAR16_T
#define UCHAR_TYPE uint16_t
#endif

//If U_NOEXCEPT is not defined, ICU falls back to NOEXCEPT.
#ifndef HAVE_CXX11
#define U_NOEXCEPT
#endif

// forward declarations to minimize the libxml2 header files needed here
typedef struct _xmlCharEncodingHandler xmlCharEncodingHandler;
typedef xmlCharEncodingHandler *xmlCharEncodingHandlerPtr;
typedef struct _xmlNode xmlNode;
typedef xmlNode *xmlNodePtr;
typedef struct _xmlDoc xmlDoc;
typedef xmlDoc *xmlDocPtr;

#include <libxml/xmlstring.h>

class DcmElement;
class DcmItem;
class DcmSequenceOfItems;
class DcmPixelSequence;
class DcmMetaInfo;
class DcmFileFormat;

/** Helper class that provides methods for parsing an XML document in the format
 *  produced by dcm2xml into a DICOM dataset.
 */
class DCMTK_DCMDATA_EXPORT DcmXMLParseHelper
{
public:
  /// constructor;
  DcmXMLParseHelper();

  /// destructor;
  virtual ~DcmXMLParseHelper();

  /** initialize XML parser library. This function should be called once
   *  before any libxml2 code is used. This function is not re-entrant.
   */
  static void initLibrary();

  /** cleanup internal memory used by the XML parser library.
   *  This function should be called once before the application
   *  terminates. It is not re-entrant.
   */
  static void cleanupLibrary();

  /** read an XML file containing a DICOM dataset in the format produced
   *  by dcm2xml, and convert it into a DICOM file
   *  @param ifname name of the XML file to be read
   *  @param fileformat DcmFileFormat instance to be populated with the parsed XML content
   *  @param xfer output parameter indicating the transfer syntax to which the XML file was parsed
   *  @param metaInfo if true, metaheader attributes are read from the XML file, otherwise they are ignored
   *  @param checkNamespace if true, check XML namespace in document root
   *  @param validateDocument if true, validate the XML document after parsing
   *  @param stopOnError if true, stop parsing and return an error code upon encountering a parse error,
   *    otherwise ignore parse errors and continue
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition readXmlFile(
    const char *ifname,
    DcmFileFormat &fileformat,
    E_TransferSyntax &xfer,
    const OFBool metaInfo,
    const OFBool checkNamespace,
    const OFBool validateDocument,
    const OFBool stopOnError);

private:

  /** convert from UTF-8 to the current DICOM character set
   *  @param fromString input string
   *  @param toString output string
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFBool convertUtf8ToCharset(
    const xmlChar *fromString,
    OFString &toString);

  /** check the type of the current XML node, report an error if there is a mismatch
   *  @param current pointer to current XML node
   *  @param name expected name of the current XML node
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition checkNode(
    xmlNodePtr current,
    const char *name);

  /** create a new, empty DICOM element corresponding to the given XML node pointer
   *  @param current pointer to current XML node
   *  @param newElem
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition createNewElement(
    xmlNodePtr current,
    DcmElement *&newElem);

  /** parse an attribute value and store it in the given DICOM element
   *  @param current pointer to current XML node
   *  @param element attribute value stored in this element
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition putElementContent(
    xmlNodePtr current,
    DcmElement *element);

  /** parse a DICOM element that is not a sequence
   *  @param dataset
   *  @param current pointer to current XML node
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition parseElement(
    DcmItem *dataset,
    xmlNodePtr current);

  /** parse a DICOM sequence
   *  @param sequence
   *  @param current pointer to current XML node
   *  @param xfer transfer syntax in which the XML file is read
   *  @param stopOnError if true, stop parsing and return an error code upon encountering a parse error,
   *    otherwise ignore parse errors and continue
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition parseSequence(
    DcmSequenceOfItems *sequence,
    xmlNodePtr current,
    E_TransferSyntax xfer,
    const OFBool stopOnError);

  /** parse a DICOM pixel sequence of a compressed image files
   *  @param sequence
   *  @param current pointer to current XML node
   *  @param stopOnError if true, stop parsing and return an error code upon encountering a parse error,
   *    otherwise ignore parse errors and continue
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition parsePixelSequence(
    DcmPixelSequence *sequence,
    xmlNodePtr current,
    const OFBool stopOnError);

  /** parse the metaheader part of an XML file containing a DICOM file.
   *  @param metainfo metaheader stored in this parameter
   *  @param current pointer to current XML node
   *  @param parse if true, parse the metaheader, otherwise just skip it
   *  @param stopOnError if true, stop parsing and return an error code upon encountering a parse error,
   *    otherwise ignore parse errors and continue
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition parseMetaHeader(
    DcmMetaInfo *metainfo,
    xmlNodePtr current,
    const OFBool parse,
    const OFBool stopOnError);

  /** parse the dataset part of an XML file containing a DICOM file or a DICOM dataset.
   *  @param dataset dataset stored in this parameter
   *  @param current pointer to current XML node
   *  @param xfer transfer syntax in which the XML file is read
   *  @param stopOnError if true, stop parsing and return an error code upon encountering a parse error,
   *    otherwise ignore parse errors and continue
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition parseDataSet(
    DcmItem *dataset,
    xmlNodePtr current,
    E_TransferSyntax xfer,
    const OFBool stopOnError);

  /** validate the given XML document and print error output if the validation fails
   *  @param doc pointer to XML document
   *  @return EC_Normal upon successful validation, an error code otherwise
   */
  OFCondition validateXmlDocument(xmlDocPtr doc);

  /// stores pointer to character encoding handler
  xmlCharEncodingHandlerPtr EncodingHandler;

};

#endif /* WITH_LIBXML */
#endif /* XML2DCM_H */
