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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRTypes
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-16 11:52:58 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRTYPES_H
#define DSRTYPES_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dctk.h"

#include "oftypes.h"

#ifdef HAVE_STRSTREA_H
#include <strstrea.h>      /* for ostrstream */
#endif
#ifdef HAVE_STRSTREAM_H
#include <strstream.h>     /* for ostrstream */
#endif


/*---------------------*
 *  macro definitions  *
 *---------------------*/

// private coding scheme designator used for internal codes
#define OFFIS_CODING_SCHEME_DESIGNATOR "99_OFFIS_DCMTK"


/*-----------------------*
 *  forward declaration  *
 *-----------------------*/

class DSRDocumentTreeNode;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** General purpose class hiding global functions, constants and types from the
 *  global namespace.  Some of them might be moved to 'ofstd' later on.
 *  All functions and constants are static and can, therefore, be accessed without
 *  creating an instance of this class.
 */
class DSRTypes
{

  public:

  // --- constant declarations ---

    /** @name renderHTML() flags.
     *  These flags can be combined and passed to the renderHMTL() methods.
     *  Please note that only the 'external' flags can be used from outside
     *  this library.  The 'shortcut' flags can be used for common cobinations.
     */
    //@{

    /// internal: render items separately (for container with SEPARATE flag)
    static const size_t HF_renderItemsSeparately;

    /// internal: expand items inline when they are short and have no child nodes
    static const size_t HF_renderItemInline;

    ///internal: content item is rendered fully inside the annex
    static const size_t HF_currentlyInsideAnnex;
    
    /// internal: create footnote references
    static const size_t HF_createFootnoteReferences;

    /// external: never expand child nodes inline
    static const size_t HF_neverExpandChildsInline;

    /// external: render concept name codes (otherwise only the code meaning)
    static const size_t HF_renderConceptNameCodes;

    /// shortcut: render the full data of all content items
    static const size_t HF_renderFullData;

    /// shortcut: render all codes
    static const size_t HF_renderAllCodes;

    /// shortcut: filter all flags that are only used internally
    static const size_t HF_internalUseOnly;
    //@}


    /** @name print() flags
     *  These flags can be combined and passed to the renderHMTL() methods.
     *  The 'shortcut' flags can be used for common cobinations.
     */
    //@{

    /// print item position ("1.2.3") instead of line indention
    static const size_t PF_printItemPosition;

    /// shorten long item value (e.g. long texts)
    static const size_t PF_shortenLongItemValues;

    /// print SOP instance UID of referenced objects
    static const size_t PF_printSOPInstanceUID;

    /// print coding scheme designator/version and code value of concept names
    static const size_t PF_printConceptNameCodes;

    /// shortcut: print all codes
    static const size_t PF_printAllCodes;
    //@}


  // --- type definitions ---

    /** SR document types
     */
    enum E_DocumentType
    {
        //// internal type used to indicate an error
        DT_invalid,
        /// internal type used to indicate an unknown document type (defined term)
        DT_unknown = DT_invalid,
        /// DICOM SOP Class: Basic Text SR
        DT_BasicTextSR,
        /// DICOM SOP Class: Enhanced SR
        DT_EnhancedSR,
        /// DICOM SOP Class: Comprehensive SR
        DT_ComprehensiveSR,
        /// internal type used to mark the last entry
        DT_last = DT_ComprehensiveSR
    };

    /** SR relationship types
     */
    enum E_RelationshipType
    {
        /// internal type used to indicate an error
        RT_invalid,
        /// internal type used to indicate an unknown relationship type (defined term)
        RT_unknown = RT_invalid,
        /// internal type used for the document root
        RT_isRoot,
        /// DICOM Relationship Type: CONTAINS
        RT_contains,
        /// DICOM Relationship Type: HAS OBS CONTEXT
        RT_hasObsContext,
        /// DICOM Relationship Type: HAS ACQ CONTEXT
        RT_hasAcqContext,
        /// DICOM Relationship Type: HAS CONCEPT MOD
        RT_hasConceptMod,
        /// DICOM Relationship Type: HAS PROPERTIES
        RT_hasProperties,
        /// DICOM Relationship Type: INFERRED FROM
        RT_inferredFrom,
        /// DICOM Relationship Type: SELECTED FROM
        RT_selectedFrom,
        /// internal type used to mark the last entry
        RT_last = RT_selectedFrom
    };

    /** SR value types
     */
    enum E_ValueType
    {
        /// internal type used to indicate an error
        VT_invalid,
        /// internal type used to indicate an unknown value type (defined term)
        VT_unknown = VT_invalid,
        /// DICOM Value Type: TEXT
        VT_Text,
        /// DICOM Value Type: CODE
        VT_Code,
        /// DICOM Value Type: NUM
        VT_Num,
        /// DICOM Value Type: DATETIME
        VT_DateTime,
        /// DICOM Value Type: DATE
        VT_Date,
        /// DICOM Value Type: TIME
        VT_Time,
        /// DICOM Value Type: UIDREF
        VT_UIDRef,
        /// DICOM Value Type: PNAME
        VT_PName,
        /// DICOM Value Type: SCOORD
        VT_SCoord,
        /// DICOM Value Type: TCOORD
        VT_TCoord,
        /// DICOM Value Type: COMPOSITE
        VT_Composite,
        /// DICOM Value Type: IMAGE
        VT_Image,
        /// DICOM Value Type: WAVEFORM
        VT_Waveform,
        /// DICOM Value Type: CONTAINER
        VT_Container,
        /// internal type used to mark the last entry
        VT_last = VT_Container
    };

    /** SR graphic types.  Used for content item SCOORD.
     */
    enum E_GraphicType
    {
        /// internal type used to indicate an error
        GT_invalid,
        /// internal type used to indicate an unknown value type (defined term)
        GT_unknown = GT_invalid,
        /// DICOM Graphic Type: POINT
        GT_Point,
        /// DICOM Graphic Type: MULTIPOINT
        GT_Multipoint,
        /// DICOM Graphic Type: POLYLINE
        GT_Polyline,
        /// DICOM Graphic Type: CIRCLE
        GT_Circle,
        /// DICOM Graphic Type: ELLIPSE
        GT_Ellipse,
        /// internal type used to mark the last entry
        GT_last = GT_Ellipse
    };

    /** SR continuity of content flag. Used for content item CONTAINER.
     */
    enum E_ContinuityOfContent
    {
        /// internal type used to indicate an error */
        COC_invalid,
        /// DICOM defined term: SEPARATE
        COC_Separate,
        /// DICOM defined term: CONTINUOUS
        COC_Continuous,
        /// internal type used to mark the last entry
        COC_last = COC_Continuous
    };

    /** SR document completion flag
     */
    enum E_CompletionFlag
    {
        /// internal type used to indicate an error */
        CF_invalid,
        /// DICOM defined term: PARTIAL
        CF_Partial,
        /// DICOM defined term: COMPLETE
        CF_Complete,
        /// internal type used to mark the last entry
        CF_last = CF_Complete
    };

    /** SR document verification flag
     */
    enum E_VerificationFlag
    {
        /// internal type used to indicate an error */
        VF_invalid,
        /// DICOM defined term: UNVERIFIED
        VF_Unverified,
        /// DICOM defined term: VERIFIED
        VF_Verified,
        /// internal type used to mark the last entry
        VF_last = VF_Verified
    };

    /** Add node mode
     */
    enum E_AddMode
    {
        /// add new node after current one (sibling)
        AM_afterCurrent,
        /// add new node before current one (sibling)
        AM_beforeCurrent,
        /// add new node below current one (child)
        AM_belowCurrent
    };


  // --- conversion functions ---

    /** convert SR document type to SOP class UID
     ** @param  documentType  SR document type to be converted
     ** @return SOP class UID if successful, empty string otherwise (never NULL)
     */
    static const char *documentTypeToSOPClassUID(const E_DocumentType documentType);

    /** convert SR document type to readable name.
     *  Such a readable name is better suited for printing/rendering.
     ** @param  documentType  SR document type to be converted
     ** @return readable name if successful, empty string otherwise (never NULL)
     */
    static const char *documentTypeToReadableName(const E_DocumentType documentType);

    /** convert relationship type to DICOM defined term
     ** @param  relationshipType  relationship type to be converted
     ** @return defined term if successful, empty string otherwise (never NULL)
     */
    static const char *relationshipTypeToDefinedTerm(const E_RelationshipType relationshipType);

    /** convert relationship type to readable name.
     *  Such a readable name is better suited for printing/rendering.
     ** @param  relationshipType  relationship type to be converted
     ** @return readable name if successful, empty string otherwise (never NULL)
     */
    static const char *relationshipTypeToReadableName(const E_RelationshipType relationshipType);

    /** convert value type to DICOM defined term
     ** @param  valueType  value type to be converted
     ** @return defined term if successful, empty string otherwise (never NULL)
     */
    static const char *valueTypeToDefinedTerm(const E_ValueType valueType);

    /** convert value type to readable name.
     *  Such a readable name is better suited for printing/rendering.
     ** @param  valueType  value type to be converted
     ** @return readable name if successful, empty string otherwise (never NULL)
     */
    static const char *valueTypeToReadableName(const E_ValueType valueType);

    /** convert graphic type to DICOM enumerated value
     ** @param  graphicType  graphic type to be converted
     ** @return enumerated value if successful, empty string otherwise (never NULL)
     */
    static const char *graphicTypeToEnumeratedValue(const E_GraphicType graphicType);

    /** convert graphic type to readable name.
     *  Such a readable name is better suited for printing/rendering.
     ** @param  graphicType  graphic type to be converted
     ** @return readable name if successful, empty string otherwise (never NULL)
     */
    static const char *graphicTypeToReadableName(const E_GraphicType graphicType);

    /** convert continuity of content flag to DICOM enumerated value
     ** @param  continuityOfContent  continuity of content flag to be converted
     ** @return enumerated value if successful, empty string otherwise (never NULL)
     */
    static const char *continuityOfContentToEnumeratedValue(const E_ContinuityOfContent continuityOfContent);

    /** convert completion flag to DICOM enumerated value
     ** @param  completionFlag  completion flag to be converted
     ** @return enumerated value if successful, empty string otherwise (never NULL)
     */
    static const char *completionFlagToEnumeratedValue(const E_CompletionFlag completionFlag);

    /** convert verification flag to DICOM enumerated value
     ** @param  verificationFlag  verification flag to be converted
     ** @return enumerated value if successful, empty string otherwise (never NULL)
     */
    static const char *verificationFlagToEnumeratedValue(const E_VerificationFlag verificationFlag);

    /** convert SOP class UID to SR document type
     ** @param  sopClassUID  SOP class UID to be converted
     ** @return SR document type if successful, DT_invalid otherwise
     */
    static E_DocumentType sopClassUIDToDocumentType(const OFString &sopClassUID);

    /** convert DICOM defined term to relationship type
     ** @param  definedTerm  defined term to be converted
     ** @return relationship type if successful, RT_invalid otherwise
     */
    static E_RelationshipType definedTermToRelationshipType(const OFString &definedTerm);

    /** convert DICOM defined term to value type
     ** @param  definedTerm  defined term to be converted
     ** @return value type if successful, VT_invalid otherwise
     */
    static E_ValueType definedTermToValueType(const OFString &definedTerm);

    /** convert DICOM enumerated value to graphic type
     ** @param  enumeratedValue  enumerated value to be converted
     ** @return graphic type if successful, GT_invalid otherwise
     */
    static E_GraphicType enumeratedValueToGraphicType(const OFString &enumeratedValue);

    /** convert DICOM enumerated value to continuity of content flag
     ** @param  enumeratedValue  enumerated value to be converted
     ** @return continuity of content flag type if successful, COC_invalid otherwise
     */
    static E_ContinuityOfContent enumeratedValueToContinuityOfContent(const OFString &enumeratedValue);

    /** convert DICOM enumerated value to completion flag
     ** @param  enumeratedValue  enumerated value to be converted
     ** @return completion flag type if successful, CF_invalid otherwise
     */
    static E_CompletionFlag enumeratedValueToCompletionFlag(const OFString &enumeratedValue);

    /** convert DICOM enumerated value to verification flag
     ** @param  enumeratedValue  enumerated value to be converted
     ** @return verification flag type if successful, VF_invalid otherwise
     */
    static E_VerificationFlag enumeratedValueToVerificationFlag(const OFString &enumeratedValue);


  // --- misc helper functions ---

    /** check whether specified SR document type is supported by this library.
     *  Currently only BasicTextSR and EnhancedSR are supported.
     ** @param  documentType  SR document type to be checked
     ** @return status, OFTrue if SR document type is supported, OFFalse otherwise
     */
    static OFBool isDocumentTypeSupported(const E_DocumentType documentType);

    /** get current date in DICOM 'DA' format
     ** @param  dateString  string used to store the current date.
     *                      ('19000101' current date could not be retrieved)
     ** @return resulting character string (see 'dateString')
     */
    static const OFString &currentDate(OFString &dateString);

    /** get current time in DICOM 'TM' format
     ** @param  timeString  string used to store the current time
     *                      ('00000000' current time could not be retrieved)
     ** @return resulting character string (see 'timeString')
     */
    static const OFString &currentTime(OFString &timeString);

    /** get current date and time in DICOM 'DT' format
     ** @param  dateTimeString  string used to store the current date and time
     *                      ('1900010100000000' current date/time could not be retrieved)
     ** @return resulting character string (see 'dateTimeString')
     */
    static const OFString &currentDateTime(OFString &dateTimeString);

    /** convert unsigned integer number to character string
     ** @param  number  unsigned integer number to be converted
     *  @param  string  character string used to store the result
     ** @return pointer to the first character of the resulting string (may be NULL if 'string' was NULL)
     */
    static const char *numberToString(const size_t number,
                                      char *string);

    /** convert string to unsigned integer number
     ** @param  string  character string to be converted
     ** @return resulting unsigned integer number if successful, 0 otherwise
     */
    static size_t stringToNumber(const char *string);


    /** create specified document tree node
     *  This is a shortcut and the only location where document tree nodes are created.
     *  It facilitates the introduction of new relationship/value types and the maintenance.
     ** @param  relationshipType  relationship type of the node to be created
     *  @param  valueType         value type of the node to be created
     ** @return pointer to the new document tree nodeif successful, NULL otherwise
     */
    static DSRDocumentTreeNode *createDocumentTreeNode(const E_RelationshipType relationshipType,
                                                       const E_ValueType valueType);


  // --- DICOM data structure access functions ---

    /** add given element to the dataset.
     *  The element is only added if 'result' is EC_Normal and the 'delem' pointer is not NULL.
     ** @param  result   reference to status variable (checked before adding and updated afterwards!)
     *  @param  dataset  reference to DICOM dataset to which the element should be added
     *  @param  delem    pointer to DICOM element which should be added
     ** @return current value of 'result', EC_Normal if successful, an error code otherwise
     */
    static E_Condition addElementToDataset(E_Condition &result,
                                           DcmItem &dataset,
                                           DcmElement *delem);

    /** get element from dataset
     ** @param  dataset  reference to DICOM dataset from which the element should be retrieved.
     *                   (Would be 'const' if the methods from 'dcmdata' would also be 'const'.)
     *  @param  delem    reference to DICOM element which should be retrieved.  The return value
     *                   is also stored in this parameter.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    static E_Condition getElementFromDataset(DcmItem &dataset,
                                             DcmElement &delem);

    /** get sequence from dataset.
     *  This method has been introduced in addition to the above one since in this case all
     *  items contained in the sequence are also stored in the resulting parameter.
     ** @param  dataset  reference to DICOM dataset from which the element should be retrieved.
     *                   (Would be 'const' if the methods from 'dcmdata' would also be 'const'.)
     *  @param  dseq     reference to DICOM sequence which should be retrieved.  The return value
     *                   is also stored in this parameter.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    static E_Condition getSequenceFromDataset(DcmItem &dataset,
                                              DcmSequenceOfItems &dseq);

    /** get string value from element
     ** @param  delem  DICOM element from which the string value should be retrieved
     ** @return pointer to character string if successful, NULL otherwise
     */
    static const char *getStringValueFromElement(const DcmElement &delem);

    /** get string value from element
     ** @param  delem        reference to DICOM element from which the string value should be retrieved
     *  @param  stringValue  reference to character string where the result should be stored
     ** @return reference character string if successful, empty string otherwise
     */
    static const OFString &getStringValueFromElement(const DcmElement &delem,
                                                     OFString &stringValue);

    /** get string value from dataset
     ** @param  dataset      reference to DICOM dataset from which the string should be retrieved.
     *                       (Would be 'const' if the methods from 'dcmdata' would also be 'const'.)
     *  @param  tagKey       DICOM tag specifying the attribute from which the string should be retrieved
     *  @param  stringValue  reference to character string in which the result should be stored.
     *                       (This parameter is automatically cleared if the tag could not be found.)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    static E_Condition getStringValueFromDataset(DcmItem &dataset,
                                                 const DcmTagKey &tagKey,
                                                 OFString &stringValue);

    /** put string value to dataset
     ** @param  dataset      reference to DICOM dataset to which the string should be put.
     *  @param  tagKey       DICOM tag specifying the attribute to which the string should be put
     *  @param  stringValue  character string specifying the value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    static E_Condition putStringValueToDataset(DcmItem &dataset,
                                               const DcmTagKey &tagKey,
                                               const OFString &stringValue);

    /** check element value for correct value multipicity and type.
     *  If the 'stream' parameter is valid a warning message is printed automatically if something
     *  is wrong.
     ** @param  delem       DICOM element to be checked
     *  @param  vm          value multiplicity (valid value: "1", "1-n", "2", "2-2n")
     *  @param  type        value multiplicity (valid value: "1", "2", something else)
     *  @param  stream      optional output stream used for warning messages
     *  @param  searchCond  optional flag indicating the status of a previous 'search' function call
     ** @return OFTrue if element value is correct, OFFalse otherwise
     */
    static OFBool checkElementValue(DcmElement &delem,
                                    const OFString &vm,
                                    const OFString &type,
                                    OFConsole *stream = NULL,
                                    const E_Condition searchCond = EC_Normal);

    /** check element value for correct value multipicity and type.
     *  If the 'stream' parameter is valid a warning message is printed automatically if something
     *  is wrong.  This functions calls the above one to check the element value.
     ** @param  dataset     reference to DICOM dataset from which the element should be retrieved.
     *                      (Would be 'const' if the methods from 'dcmdata' would also be 'const'.)
     *  @param  delem       DICOM element used to store the value
     *  @param  vm          value multiplicity (valid value: "1", "1-n", "2", "2-2n")
     *  @param  type        value type (valid value: "1", "2", something else which is not checked)
     *  @param  stream      optional output stream used for warning messages
     ** @return status, EC_Normal if element could be retrieved and value is correct, an error code otherwise
     */
    static E_Condition getAndCheckElementFromDataset(DcmItem &dataset,
                                                     DcmElement &delem,
                                                     const OFString &vm,
                                                     const OFString &type,
                                                     OFConsole *stream = NULL);


  // --- output functions ---

    /** print a warning message.
     *  The prefix 'DCMSR - Warning: ' is automatically added to the message text.
     ** @param  stream  output stream to which the warning message is printed
     */
    static void printWarningMessage(OFConsole *stream,
                                    const char *message);

    /** print a error message.
     *  The prefix 'DCMSR - Error: ' is automatically added to the message text.
     ** @param  stream  output stream to which the error message is printed
     */
    static void printErrorMessage(OFConsole *stream,
                                  const char *message);

    /** print the warning message that the current content item is invalid/incomlete.
     *  The value type (for DEBUG mode also the node ID) is added if the 'node' if specified.
     ** @param  stream  output stream to which the warning message is printed (no message if NULL)
     *  @param  action  text describing the current action (e.g. 'Reading'), 'Processing' if NULL
     *  @param  node    pointer to document tree node forwhich the message should be printed
     */
    static void printInvalidContentItemMessage(OFConsole *stream,
                                               const char *action,
                                               const DSRDocumentTreeNode *node);

    /** print an error message for the current content item.
     *  The value type (for DEBUG mode also the node ID) is added if the 'node' if specified.
     ** @param  stream  output stream to which the warning message is printed (no message if NULL)
     *  @param  action  text describing the current action (e.g. 'Reading'), 'Processing' if NULL
     *  @param  result  status used to print more information on the error (no message if EC_Normal)
     *  @param  node    pointer to document tree node forwhich the message should be printed
     */
    static void printContentItemErrorMessage(OFConsole *stream,
                                             const char *action,
                                             const E_Condition result,
                                             const DSRDocumentTreeNode *node);

    /** create an HTML annex entry with hyperlinks.
     *  A reference text is added to the main document and a new annex entry to the document annex
     *  with HTML hyperlinks between both.  Example for a reference: '[for details see Annex 1]'
     ** @param  docStream      output stream used for the main document
     *  @param  annexStream    output stream used for the document annex
     *  @param  referenceText  optional text added to the main document (e.g. 'for details see')
     *  @param  annexNumber    reference to the variable where the current annex number is stored.
     *                         Value is increased automatically by 1 after the new entry has been added.
     ** @return current annex number after the new entry has been added
     */
    static size_t createHTMLAnnexEntry(ostream &docStream,
                                       ostream &annexStream,
                                       const OFString &referenceText,
                                       size_t &annexNumber);

    /** create an HTML footnote with hyperlinks
     ** @param  docStream       output stream used for the main document
     *  @param  footnoteStream  output stream used for the footnote text
     *  @param  footnoteNumber  reference to the variable where the current footnote number is stored.
     *                          Value is increased automatically by 1 after the new entry has been added.
     *  @param  nodeID          unique numerical identifier of the current node for which this footnote
     *                          is created.  Used to create a unique name for the hyperlink.
     ** @return current footnote number after the new entry has been added
     */
    static size_t createHTMLFootnote(ostream &docStream,
                                     ostream &footnoteStream,
                                     size_t &footnoteNumber,
                                     const size_t nodeID);

    /** append one output stream to another.
     ** @param  mainStream  stream to which the other should be added
     *  @param  tempStream  (string) stream to be added to the other
     *  @param  heading     (optional) string which is added to the 'mainStream' before the 'tempStream'.
     *                      This string is only added if 'tempStream' is not empty.
     ** @return status, EC_Normal if stream has been added successfully, an error code otherwise
     */
    static E_Condition appendStream(ostream &mainStream,
                                    ostrstream &tempStream,
                                    const char *heading = NULL);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrtypes.h,v $
 *  Revision 1.2  2000-10-16 11:52:58  joergr
 *  Added new options: number nested items instead of indenting them, print SOP
 *  instance UID of referenced composite objects.
 *
 *  Revision 1.1  2000/10/13 07:49:35  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
