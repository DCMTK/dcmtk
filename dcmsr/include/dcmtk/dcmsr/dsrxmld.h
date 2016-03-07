/*
 *
 *  Copyright (C) 2003-2016, OFFIS e.V.
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRXMLDocument
 *
 */


#ifndef DSRXMLD_H
#define DSRXMLD_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrxmlc.h"


/*--------------------*
 *  type definitions  *
 *--------------------*/

#ifndef WITH_LIBXML
// define types if 'libxml' absent
typedef void (*xmlDocPtr);
typedef void (*xmlCharEncodingHandlerPtr);
typedef char xmlChar;
#endif


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for XML documents.
 *  This class encapsulates the access to the 'libxml' routines.  Since the
 *  public interface of this class does not use any 'libxml' specific data
 *  structures it should be possible to replace the XML library with little
 *  effort (if required).
 */
class DCMTK_DCMSR_EXPORT DSRXMLDocument
  : protected DSRTypes
{

  public:

  // --- constructors and destructor ---

    /** default constructor
     */
    DSRXMLDocument();

    /** destructor
     */
    virtual ~DSRXMLDocument();


  // --- misc routines ---

    /** clear all internal member variables
     */
    void clear();

    /** check whether the current internal state is valid
     ** @return OFTrue if valid, OFFalse otherwise
     */
    OFBool valid() const;

  // --- input and output ---

    /** read XML document from file.
     *  In order to enable the optional Schema validation the flag XF_validateSchema has
     *  to be set.
     ** @param  filename  name of the file from which the XML document is read
     *                    ("-" for stdin)
     *  @param  flags     optional flag used to customize the reading process
     *                    (see DSRTypes::XF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition read(const OFString &filename,
                     const size_t flags = 0);


  // --- character encoding ---

    /** check whether the currently set character encoding handler is valid.
     *  If no encoding handler is set this is equivalent to an invalid handler.
     ** @return OFTrue if handler is valid, OFFalse otherwise
     */
    OFBool encodingHandlerValid() const;

    /** set the specified character encoding handler.
     *  NB: 'libxml' relies on GNU 'libiconv' for most character sets.
     ** @param  charset  XML name of the character set (e.g. "ISO-8859-1" for ISO Latin-1)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setEncodingHandler(const char *charset);


  // --- navigation and checking ---

    /** get root node of the document
     ** @return cursor pointing to the root node if successful, invalid cursor otherwise
     */
    DSRXMLCursor getRootNode() const;

    /** get a particular named node of the document.
     *  Please note that the search process is limited to the current node level, i.e. no
     *  deep search is performed.
     ** @param  cursor    cursor pointing to the node where to start from
     *  @param  name      name of the node (XML element) to be searched for
     *  @param  required  flag specifying whether the node is required or not.  If the node
     *                    is required to be present an error message is reported.
     ** @return cursor pointing to the named node if successful, invalid cursor otherwise
     */
    DSRXMLCursor getNamedNode(const DSRXMLCursor &cursor,
                              const char *name,
                              const OFBool required = OFTrue) const;

    /** check whether particular node matches a given name
     ** @param  cursor  cursor pointing to the particular node
     *  @param  name    name of the node (XML element) to be checked
     ** @return OFTrue if name matches, OFFalse otherwise
     */
    OFBool matchNode(const DSRXMLCursor &cursor,
                     const char *name) const;

    /** check whether particular node matches a given name and report an error if not
     ** @param  cursor  cursor pointing to the particular node
     *  @param  name    name of the node (XML element) to be checked
     ** @return OFTrue if name matches, OFFalse otherwise
     */
    OFCondition checkNode(const DSRXMLCursor &cursor,
                          const char *name) const;


  // --- get attributes and node content ---

    /** check whether particular node has a specific attribute
     ** @param  cursor  cursor pointing to the particular node
     *  @param  name    name of the XML attribute to be checked
     ** @return OFTrue if attribute is present, OFFalse otherwise
     */
    OFBool hasAttribute(const DSRXMLCursor &cursor,
                        const char *name) const;

    /** get string value from particular XML attribute.
     *  The result variable 'stringValue' is automatically cleared at the beginning.
     ** @param  cursor       cursor pointing to the particular node
     *  @param  stringValue  reference to string object in which the value should be stored
     *  @param  name         name of the XML attribute to be retrieved
     *  @param  encoding     use encoding handler if OFTrue, ignore character set otherwise
     *  @param  required     flag specifying whether the attribute is required or not.  If the
     *                       attribute is required to be present an error message is reported
     *                       in case it is not found.
     ** @return reference to string object (might be empty)
     */
    OFString &getStringFromAttribute(const DSRXMLCursor &cursor,
                                     OFString &stringValue,
                                     const char *name,
                                     const OFBool encoding = OFFalse,
                                     const OFBool required = OFTrue) const;

    /** get element value from particular XML attribute
     ** @param  cursor    cursor pointing to the particular node
     *  @param  delem     DICOM element in which the attribute value is stored
     *  @param  name      name of the XML attribute to be retrieved
     *  @param  encoding  use encoding handler if OFTrue, ignore character set otherwise
     *  @param  required  flag specifying whether the attribute is required or not.  If the
     *                    attribute is required to be present an error message is reported
     *                    in case it is not found.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getElementFromAttribute(const DSRXMLCursor &cursor,
                                        DcmElement &delem,
                                        const char *name,
                                        const OFBool encoding = OFFalse,
                                        const OFBool required = OFTrue) const;

    /** get string value from particular XML element
     ** @param  cursor       cursor pointing to the particular node
     *  @param  stringValue  reference to string object in which the value should be stored
     *  @param  name         name of the XML element to be retrieved
     *  @param  encoding     use encoding handler if OFTrue, ignore character set otherwise
     *  @param  clearString  flag specifying whether to clear the 'stringValue' at first or not
     ** @return reference to string object (might be empty)
     */
    OFString &getStringFromNodeContent(const DSRXMLCursor &cursor,
                                       OFString &stringValue,
                                       const char *name = NULL,
                                       const OFBool encoding = OFFalse,
                                       const OFBool clearString = OFTrue) const;

    /** get element value from particular XML element
     ** @param  cursor    cursor pointing to the particular node
     *  @param  delem     DICOM element in which the element value is stored
     *  @param  name      name of the XML element to be retrieved
     *  @param  encoding  use encoding handler if OFTrue, ignore character set otherwise
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getElementFromNodeContent(const DSRXMLCursor &cursor,
                                          DcmElement &delem,
                                          const char *name = NULL,
                                          const OFBool encoding = OFFalse) const;

    /** get value type from particular node.
     *  The value type is either stored as the element name or in the attribute "valType".
     *  Additionally, by-reference relationships are also supported (either by attribute
     *  "ref" being present or element named "reference").
     ** @param  cursor  cursor pointing to the particular node
     ** @return value type (incl. by-reference) if successful, VT_invalid/unknown otherwise
     */
    E_ValueType getValueTypeFromNode(const DSRXMLCursor &cursor) const;

    /** get relationship type from particular node.
     *  The relationship type is either stored in the element "relationship" or in the
     *  attribute "relType".
     ** @param  cursor  cursor pointing to the particular node
     ** @return relationship type if successful, RT_invalid/unknown otherwise
     */
    E_RelationshipType getRelationshipTypeFromNode(const DSRXMLCursor &cursor) const;


  // --- error/warning messages ---

    /** print warning message for unexpected node
     ** @param  cursor  cursor pointing to the unexpected node
     */
    void printUnexpectedNodeWarning(const DSRXMLCursor &cursor) const;

    /** print general node error message
     ** @param  cursor  cursor pointing to the unexpected node
     *  @param  result  status used to print details on the error (no message if EC_Normal)
     */
    void printGeneralNodeError(const DSRXMLCursor &cursor,
                               const OFCondition &result) const;

  protected:

    /** convert given string from 'libxml' format (UTF8) to current character set
     ** @param  fromString  character string to be converted
     *  @param  toString    reference to string object in which the result should be stored
     ** @return OFTrue if successful, OFFalse otherwise (e.g. no handler selected)
     */
    OFBool convertUtf8ToCharset(const xmlChar *fromString,
                                OFString &toString) const;

    /** print error message for missing attribute
     ** @param  cursor  cursor pointing to the relevant node
     *  @param  name    name of the XML attribute
     */
    void printMissingAttributeError(const DSRXMLCursor &cursor,
                                    const char *name) const;

  // --- static function ---

    /** get the full path (incl. all predecessors) to the current node.
     *  Returns "<invalid>" in case of an invalid 'cursor'.
     ** @param  cursor       cursor pointing to the relevant node
     *  @param  stringValue  reference to string object in which the result should be stored
     *  @param  omitCurrent  flag indicating whether to omit the current node or not
     */
    static OFString &getFullNodePath(const DSRXMLCursor &cursor,
                                     OFString &stringValue,
                                     const OFBool omitCurrent = OFFalse);


  private:

    /// pointer to the internal representation of the XML document (libxml)
    xmlDocPtr Document;
    /// pointer to the currently selected character encoding handler (libxml)
    xmlCharEncodingHandlerPtr EncodingHandler;

// --- declaration copy constructor and assignment operator

    DSRXMLDocument(const DSRXMLDocument &);
    DSRXMLDocument &operator=(const DSRXMLDocument &);
};


#endif
