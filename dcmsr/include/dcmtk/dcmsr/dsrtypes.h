/*
 *
 *  Copyright (C) 2000-2019, OFFIS e.V.
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
 *    classes: DSRTypes
 *
 */


#ifndef DSRTYPES_H
#define DSRTYPES_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsdefine.h"

#include "dcmtk/dcmdata/dcelem.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dcsequen.h"

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/oflog/oflog.h"


// global definitions for logging mechanism provided by the oflog module

extern DCMTK_DCMSR_EXPORT OFLogger DCM_dcmsrLogger;

#define DCMSR_TRACE(msg) OFLOG_TRACE(DCM_dcmsrLogger, msg)
#define DCMSR_DEBUG(msg) OFLOG_DEBUG(DCM_dcmsrLogger, msg)
#define DCMSR_INFO(msg)  OFLOG_INFO(DCM_dcmsrLogger, msg)
#define DCMSR_WARN(msg)  OFLOG_WARN(DCM_dcmsrLogger, msg)
#define DCMSR_ERROR(msg) OFLOG_ERROR(DCM_dcmsrLogger, msg)
#define DCMSR_FATAL(msg) OFLOG_FATAL(DCM_dcmsrLogger, msg)


// include this file in doxygen documentation

/** @file dsrtypes.h
 *  @brief type definitions, constants and helper functions for the dcmsr module
 */


/*---------------------*
 *  macro definitions  *
 *---------------------*/

// private coding scheme designator used for internal codes
#define OFFIS_CODING_SCHEME_DESIGNATOR "99_OFFIS_DCMTK"
// name of the private coding scheme
#define OFFIS_CODING_SCHEME_NAME "OFFIS DCMTK Coding Scheme"
// organization responsible for the private coding scheme
#define OFFIS_CODING_SCHEME_RESPONSIBLE_ORGANIZATION "OFFIS e.V., Escherweg 2, 26121 Oldenburg, Germany"

// protocol, hostname and CGI script name used for HTML hyperlinks to composite objects
#define HTML_HYPERLINK_PREFIX_FOR_CGI "http://localhost/dicom.cgi"
// URL of the DICOM toolkit DCMTK
#define DCMTK_INTERNET_URL "http://dicom.offis.de/dcmtk"

// XML namespace URI for dcmsr module
#define DCMSR_XML_NAMESPACE_URI "http://dicom.offis.de/dcmsr"
// XML Schema file for dcmsr module
#define DCMSR_XML_XSD_FILE "dsr2xml.xsd"
// XML Schema Instance URI
#define XML_SCHEMA_INSTANCE_URI "http://www.w3.org/2001/XMLSchema-instance"

// ANSI escape codes for color output of the print() method
#define DCMSR_ANSI_ESCAPE_CODE_RESET             "\033[0m"
#define DCMSR_ANSI_ESCAPE_CODE_DOCUMENT_TYPE     "\033[22m\033[35m"
#define DCMSR_ANSI_ESCAPE_CODE_HEADER_NAME       "\033[1m\033[34m"
#define DCMSR_ANSI_ESCAPE_CODE_HEADER_VALUE      "\033[1m\033[37m"
#define DCMSR_ANSI_ESCAPE_CODE_ITEM_POSITION     "\033[22m\033[36m"
#define DCMSR_ANSI_ESCAPE_CODE_VALUE_TYPE        "\033[22m\033[31m"
#define DCMSR_ANSI_ESCAPE_CODE_RELATIONSHIP_TYPE "\033[22m\033[32m"
#define DCMSR_ANSI_ESCAPE_CODE_CONCEPT_NAME      "\033[22m\033[33m"
#define DCMSR_ANSI_ESCAPE_CODE_ITEM_VALUE        "\033[1m\033[37m"
#define DCMSR_ANSI_ESCAPE_CODE_TEMPLATE_ID       "\033[22m\033[35m"
#define DCMSR_ANSI_ESCAPE_CODE_ANNOTATION        "\033[22m\033[36m"
#define DCMSR_ANSI_ESCAPE_CODE_DELIMITER         "\033[1m\033[30m"

// helper macro for the conditional usage of ANSI escape codes
#define DCMSR_PRINT_ANSI_ESCAPE_CODE(ansi_code)  \
    if (flags & PF_useANSIEscapeCodes)           \
    {                                            \
        stream << ansi_code;                     \
    }


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DSRXMLCursor;
class DSRXMLDocument;
class DSRDocumentTreeNode;
class DSRIODConstraintChecker;


/*-----------------------*
 *  constant definitions  *
 *-----------------------*/

/** @name specific error conditions for module dcmsr.
 *  These error codes can be used in addition to the general purpose
 *  codes defined in module dcmdata.
 */
//@{

/// error: the document type (SOP class UID) is unknown or not supported
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_UnknownDocumentType;

/// error: the document status is invalid
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_InvalidDocument;

/// error: the document tree is invalid (e.g. corrupted structure)
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_InvalidDocumentTree;

/// error: a mandatory attribute is missing
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_MandatoryAttributeMissing;

/// error: a value is invalid according to the DICOM standard
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_InvalidValue;

/// error: a value is not supported by this implementation
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_UnsupportedValue;

/// error: an unknown value type is used
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_UnknownValueType;

/// error: an unknown relationship type is used
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_UnknownRelationshipType;

/// error: the by-value relationship between two content items is not allowed
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_InvalidByValueRelationship;

/// error: the by-reference relationship between two content items is not allowed
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_InvalidByReferenceRelationship;

/// error: the specified SOP instance could not be found
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_SOPInstanceNotFound;

/// error: a SOP instance has different SOP classes
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_DifferentSOPClassesForAnInstance;

/// error: the specified coding scheme designator could not be found
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_CodingSchemeNotFound;

/// error: the XML structure is corrupted (XML parser error)
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_CorruptedXMLStructure;

/// error: the requested representation is not available
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_RepresentationNotAvailable;

/// error: an icon image could not be created from the given DICOM image
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_CannotCreateIconImage;

/// error: a new content item could not be added to the current one
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_CannotAddContentItem;

/// error: an invalid concept name code has been passed or is used
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_InvalidConceptName;

/// error: a given subtree could not be inserted at the current node
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_CannotInsertSubTree;

/// error: the current relationship type cannot be replaced by the given one
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_CannotChangeRelationshipType;

/// error: the document tree does not comply with the given SR IOD constraints
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_IncompatibleDocumentTree;

/// error: the specified content item (node) could not be found
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_ContentItemNotFound;

/// error: the specified subtree could not be removed from the document tree
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_CannotRemoveSubTree;

/// error: the document tree is empty (and this is not allowed in this context)
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_EmptyDocumentTree;

/// error: the content item is invalid, e.g. the concept name or the stored value
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_InvalidContentItem;

/// error: template identification cannot be used, i.e. requirements are not fulfilled
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_CannotUseTemplateIdentification;

/// error: cannot add content item or insert subtree to non-extensible template
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_NonExtensibleTemplate;

/// error: cannot add coded entry to non-extensible context group
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_NonExtensibleContextGroup;

/// error: a given coded entry in not contained in the context group
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_CodedEntryNotInContextGroup;

/// normal: a given coded entry is contained in the context group (i.e. a standard code)
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_CodedEntryInStandardContextGroup;

/// normal: a given coded entry is known as an extension of the context group
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_CodedEntryIsExtensionOfContextGroup;

/// error: a value violates the value set constraint of a particular template row
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_ValueSetConstraintViolated;

/// error: the internally managed structure of a template class is invalid
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_InvalidTemplateStructure;

/// error: cannot process document tree with included templates
extern DCMTK_DCMSR_EXPORT const OFConditionConst SR_EC_CannotProcessIncludedTemplates;

//@}


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** General purpose class hiding global functions, constants and types from the
 *  global namespace.  Some of them might be moved to 'ofstd' later on.
 *  All functions and constants are static and can, therefore, be accessed without
 *  creating an instance of this class.
 */
class DCMTK_DCMSR_EXPORT DSRTypes
{

  public:

  // --- constant declarations ---

    /** @name read() flags
     *  These flags can be combined and passed to the read() methods.
     *  The 'shortcut' flags can be used for common combinations.
     */
    //@{

    /// read digital signatures from dataset
    static const size_t RF_readDigitalSignatures;

    /// accept unknown/missing relationship type
    static const size_t RF_acceptUnknownRelationshipType;

    /// accept invalid content item value (e.g. violation of VR or VM definition)
    static const size_t RF_acceptInvalidContentItemValue;

    /// ignore relationship constraints for this document class
    static const size_t RF_ignoreRelationshipConstraints;

    /// do not abort on content item errors (e.g. missing value type specific attributes)
    static const size_t RF_ignoreContentItemErrors;

    /// do not abort when detecting an invalid content item, skip invalid sub-tree instead
    static const size_t RF_skipInvalidContentItems;

    /// show the currently processed content item (e.g. "1.2.3")
    static const size_t RF_showCurrentlyProcessedItem;
    //@}


    /** @name renderHTML() flags
     *  These flags can be combined and passed to the renderHMTL() methods.
     *  Please note that only the 'external' flags can be used from outside
     *  this library.  The 'shortcut' flags can be used for common combinations.
     */
    //@{

    /// external: never expand child nodes inline
    static const size_t HF_neverExpandChildrenInline;

    /// external: always expand child nodes inline
    static const size_t HF_alwaysExpandChildrenInline;

    /// external: render codes even if they appear inline
    static const size_t HF_renderInlineCodes;

    /// external: render code details as a tooltip (not with HTML 3.2)
    static const size_t HF_useCodeDetailsTooltip;

    /// external: render concept name codes (default: code meaning only)
    static const size_t HF_renderConceptNameCodes;

    /// external: render the code of the numeric measurement unit
    static const size_t HF_renderNumericUnitCodes;

    /// external: use code meaning for the numeric measurement unit (default: code value)
    static const size_t HF_useCodeMeaningAsUnit;

    /// external: use patient information as document title (default: document type)
    static const size_t HF_renderPatientTitle;

    /// external: render no general document information (header)
    static const size_t HF_renderNoDocumentHeader;

    /// external: render dcmtk/dcmsr comment at the end of the document
    static const size_t HF_renderDcmtkFootnote;

    /// external: render the full data of all content items
    static const size_t HF_renderFullData;

    /// external: render section titles inline (default: separate paragraph)
    static const size_t HF_renderSectionTitlesInline;

    /// external: copy Cascading Style Sheet (CSS) content to HTML file
    static const size_t HF_copyStyleSheetContent;

    /// external: output compatible to HTML version 3.2 (default: 4.01)
    static const size_t HF_HTML32Compatibility;

    /// external: output compatible to XHTML version 1.1 (default: HTML 4.01)
    static const size_t HF_XHTML11Compatibility;

    /// external: add explicit reference to HTML document type (DTD)
    static const size_t HF_addDocumentTypeReference;

    /// external: omit generator meta element referring to the DCMTK
    static const size_t HF_omitGeneratorMetaElement;

    /// internal: render items separately (for container with SEPARATE flag)
    static const size_t HF_renderItemsSeparately;

    /// internal: expand items inline when they are short and have no child nodes
    static const size_t HF_renderItemInline;

    /// internal: content item is rendered fully inside the annex
    static const size_t HF_currentlyInsideAnnex;

    /// internal: create footnote references
    static const size_t HF_createFootnoteReferences;

    /// internal: convert non-ASCII characters (> #127) to &\#nnn;
    static const size_t HF_convertNonASCIICharacters;

    /// shortcut: render all codes
    static const size_t HF_renderAllCodes;

    /// shortcut: filter all flags that are only used internally
    static const size_t HF_internalUseOnly;
    //@}


    /** @name read/writeXML() flags
     *  These flags can be combined and passed to the read/writeXML() methods.
     *  The 'shortcut' flags can be used for common combinations.
     */
    //@{

    /// write: write all tags even if their value is empty
    static const size_t XF_writeEmptyTags;

    /// write: write template identification information (TID and mapping resource)
    static const size_t XF_writeTemplateIdentification;

    /// write: always write item identifier "id", not only when item is referenced
    static const size_t XF_alwaysWriteItemIdentifier;

    /// write: encode code value, coding scheme designator and coding scheme version as attribute instead of element text
    static const size_t XF_codeComponentsAsAttribute;

    /// write: encode relationship type as attribute instead of element text
    static const size_t XF_relationshipTypeAsAttribute;

    /// write: encode value type as attribute instead of element text
    static const size_t XF_valueTypeAsAttribute;

    /// write: encode template identifier as attribute instead of element text
    static const size_t XF_templateIdentifierAsAttribute;

    /// write: add DCMSR namespace declaration to the XML output
    static const size_t XF_useDcmsrNamespace;

    /// write: add Schema reference to XML document
    static const size_t XF_addSchemaReference;

    /// read: validate content of XML document against Schema
    static const size_t XF_validateSchema;

    /// read/write: template identification element encloses content items
    static const size_t XF_templateElementEnclosesItems;

    /// write: add comments with details at beginning/end of included template (might be useful for debugging purposes)
    static const size_t XF_addCommentsForIncludedTemplate;

    /// read: accept empty Study/Series/SOP Instance UID attribute values (must be filled later)
    static const size_t XF_acceptEmptyStudySeriesInstanceUID;

    /// shortcut: combines all XF_xxxAsAttribute write flags (see above)
    static const size_t XF_encodeEverythingAsAttribute;
    //@}


    /** @name print() flags
     *  These flags can be combined and passed to the print() methods.
     *  The 'shortcut' flags can be used for common combinations.
     */
    //@{

    /// print item position ("1.2.3") instead of line indentation
    static const size_t PF_printItemPosition;

    /// shorten long item value (e.g. long texts)
    static const size_t PF_shortenLongItemValues;

    /// print SOP instance UID of referenced objects
    static const size_t PF_printSOPInstanceUID;

    /// print coding scheme designator/version and code value of concept names
    static const size_t PF_printConceptNameCodes;

    /// print no general document information (header)
    static const size_t PF_printNoDocumentHeader;

    /// print template identification (TID and mapping resource)
    static const size_t PF_printTemplateIdentification;

    /// use ANSI escape codes for output
    static const size_t PF_useANSIEscapeCodes;

    /// print long SOP class name of referenced objects (default: short name for images)
    static const size_t PF_printLongSOPClassName;

    /// print SOP class UID of referenced objects (instead of the name)
    static const size_t PF_printSOPClassUID;

    /// print code triple for invalid codes (instead of the text "invalid code")
    static const size_t PF_printInvalidCodes;

    /// print node ID at the beginning of each line (might be useful for debugging purposes)
    static const size_t PF_printNodeID;

    /// indicate with a "*" that the "enhanced encoding mode" is used for codes
    static const size_t PF_indicateEnhancedEncodingMode;

    /// print annotation of a content item (optional, e.g. user-defined information)
    static const size_t PF_printAnnotation;

    /// do not print internal "included template" nodes (position counter is still increased)
    static const size_t PF_hideIncludedTemplateNodes;

    /// do not count internal "included template" nodes (only with PF_hideIncludedTemplateNodes)
    static const size_t PF_dontCountIncludedTemplateNodes;

    /// shortcut: print all codes (combines all PF_printXxxCodes flags, see above)
    static const size_t PF_printAllCodes;
    //@}


    /** @name checkByReferenceRelationships() modes
     *  These modes can be combined and passed to the checkByReferenceRelationships() method.
     */
    //@{

    /// update the position string using the node ID
    static const size_t CM_updatePositionString;

    /// update the node ID using the position string
    static const size_t CM_updateNodeID;

    /// reset the reference target flag for all nodes
    static const size_t CM_resetReferenceTargetFlag;
    //@}


    /** @name checkByReferenceRelationships() bit masks
     *  These bit masks are used to "filter" valid flags passed to checkByReferenceRelationships().
     */
    //@{

    /// bit mask (filter) for valid print flags (see PF_xxx)
    static const size_t CB_maskPrintFlags;

    /// bit mask (filter) for valid read flags (see RF_xxx)
    static const size_t CB_maskReadFlags;

    //@}


  // --- type definitions ---

    /** SR document types
     */
    enum E_DocumentType
    {
        /// internal type used to indicate an error
        DT_invalid,
        /// internal type used to indicate an unknown/unsupported document type
        DT_unknown = DT_invalid,
        /// DICOM IOD: Basic Text SR
        DT_BasicTextSR,
        /// DICOM IOD: Enhanced SR
        DT_EnhancedSR,
        /// DICOM IOD: Comprehensive SR
        DT_ComprehensiveSR,
        /// DICOM IOD: Key Object Selection Document
        DT_KeyObjectSelectionDocument,
        /// DICOM IOD: Mammography CAD SR
        DT_MammographyCadSR,
        /// DICOM IOD: Chest CAD SR
        DT_ChestCadSR,
        /// DICOM IOD: Colon CAD SR
        DT_ColonCadSR,
        /// DICOM IOD: Procedure Log
        DT_ProcedureLog,
        /// DICOM IOD: X-Ray Radiation Dose SR
        DT_XRayRadiationDoseSR,
        /// DICOM IOD: Spectacle Prescription Report
        DT_SpectaclePrescriptionReport,
        /// DICOM IOD: Macular Grid Thickness and Volume Report
        DT_MacularGridThicknessAndVolumeReport,
        /// DICOM IOD: Implantation Plan SR Document
        DT_ImplantationPlanSRDocument,
        /// DICOM IOD: Comprehensive 3D SR
        DT_Comprehensive3DSR,
        /// DICOM IOD: Radiopharmaceutical Radiation Dose SR
        DT_RadiopharmaceuticalRadiationDoseSR,
        /// DICOM IOD: Extensible SR (not yet implemented)
        DT_ExtensibleSR,
        /// DICOM IOD: Acquisition Context SR
        DT_AcquisitionContextSR,
        /// DICOM IOD: Simplified Adult Echo SR
        DT_SimplifiedAdultEchoSR,
        /// DICOM IOD: Patient Radiation Dose SR
        DT_PatientRadiationDoseSR,
        /// DICOM IOD: Performed Imaging Agent Administration SR
        DT_PerformedImagingAgentAdministrationSR,
        /// DICOM IOD: Planned Imaging Agent Administration SR
        DT_PlannedImagingAgentAdministrationSR,
        /// internal type used to mark the last entry
        DT_last = DT_PlannedImagingAgentAdministrationSR
    };

    /** SR relationship types
     */
    enum E_RelationshipType
    {
        /// internal type used to indicate an error
        RT_invalid,
        /// internal type used to indicate an unknown relationship type (defined term)
        RT_unknown,
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
        /// DICOM Value Type: SCOORD3D
        VT_SCoord3D,
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
        /// internal type used to indicate by-reference relationships
        VT_byReference,
        /// internal type used to indicate (enclose) included templates
        VT_includedTemplate,
        /// internal type used to mark the last entry
        VT_last = VT_includedTemplate
    };

    /** Softcopy presentation state types.  Used for content item IMAGE.
     */
    enum E_PresentationStateType
    {
        /// internal type used to indicate an error
        PT_invalid,
        /// internal type used to indicate an unknown presentation state type (SOP class)
        PT_unknown = PT_invalid,
        /// Grayscale Softcopy Presentation State (GSPS)
        PT_Grayscale,
        /// Color Softcopy Presentation State (CSPS)
        PT_Color,
        /// Pseudo-Color Softcopy Presentation State (PCSPS)
        PT_PseudoColor,
        /// Blending Softcopy Presentation State (BSPS)
        PT_Blending,
        /// XA/XRF Grayscale Softcopy Presentation State (XGSPS)
        PT_XAXRFGrayscale,
        /// Grayscale Planar MPR Volumetric Presentation State (GP-VPS)
        PT_GrayscalePlanarMPR,
        /// Compositing Planar MPR Volumetric Presentation State (CP-VPS)
        PT_CompositingPlanarMPR,
        /// Advanced Blending Presentation State (ABPS)
        PT_AdvancedBlending,
        /// Volume Rendering Volumetric Presentation State (VR-VPS)
        PT_VolumeRendering,
        /// Segmented Volume Rendering Volumetric Presentation State (SVR-VPS)
        PT_SegmentedVolumeRendering,
        /// Multiple Volume Rendering Volumetric Presentation State (MVR-VPS)
        PT_MultipleVolumeRendering,
        /// internal type used to mark the last entry
        PT_last = PT_MultipleVolumeRendering
    };

    /** SR graphic types.  Used for content item SCOORD.
     */
    enum E_GraphicType
    {
        /// internal type used to indicate an error
        GT_invalid,
        /// internal type used to indicate an unknown graphic type (defined term)
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

    /** SR graphic types (3D).  Used for content item SCOORD3D.
     */
    enum E_GraphicType3D
    {
        /// internal type used to indicate an error
        GT3_invalid,
        /// internal type used to indicate an unknown graphic type (defined term)
        GT3_unknown = GT3_invalid,
        /// DICOM Graphic Type: POINT
        GT3_Point,
        /// DICOM Graphic Type: MULTIPOINT
        GT3_Multipoint,
        /// DICOM Graphic Type: POLYLINE
        GT3_Polyline,
        /// DICOM Graphic Type: POLYGON
        GT3_Polygon,
        /// DICOM Graphic Type: ELLIPSE
        GT3_Ellipse,
        /// DICOM Graphic Type: ELLIPSOID
        GT3_Ellipsoid,
        /// internal type used to mark the last entry
        GT3_last = GT3_Ellipsoid
    };

    /** SR temporal range types.  Used for content item TCOORD.
     */
    enum E_TemporalRangeType
    {
        /// internal type used to indicate an error
        TRT_invalid,
        /// internal type used to indicate an unknown range type (defined term)
        TRT_unknown = TRT_invalid,
        /// DICOM Temporal Range Type: POINT
        TRT_Point,
        /// DICOM Temporal Range Type: MULTIPOINT
        TRT_Multipoint,
        /// DICOM Temporal Range Type: SEGMENT
        TRT_Segment,
        /// DICOM Temporal Range Type: MULTISEGMENT
        TRT_Multisegment,
        /// DICOM Temporal Range Type: BEGIN
        TRT_Begin,
        /// DICOM Temporal Range Type: END
        TRT_End,
        /// internal type used to mark the last entry
        TRT_last = TRT_End
    };

    /** SR continuity of content flag. Used for content item CONTAINER.
     */
    enum E_ContinuityOfContent
    {
        /// internal type used to indicate an error
        COC_invalid,
        /// DICOM enumerated value: SEPARATE
        COC_Separate,
        /// DICOM enumerated value: CONTINUOUS
        COC_Continuous,
        /// internal type used to mark the last entry
        COC_last = COC_Continuous
    };

    /** SR document preliminary flag
     */
    enum E_PreliminaryFlag
    {
        /// internal type used to indicate an error or the absence of this flag
        PF_invalid,
        /// DICOM enumerated value: PRELIMINARY
        PF_Preliminary,
        /// DICOM enumerated value: FINAL
        PF_Final,
        /// internal type used to mark the last entry
        PF_last = PF_Final
    };

    /** SR document completion flag
     */
    enum E_CompletionFlag
    {
        /// internal type used to indicate an error
        CF_invalid,
        /// DICOM enumerated value: PARTIAL
        CF_Partial,
        /// DICOM enumerated value: COMPLETE
        CF_Complete,
        /// internal type used to mark the last entry
        CF_last = CF_Complete
    };

    /** SR document verification flag
     */
    enum E_VerificationFlag
    {
        /// internal type used to indicate an error
        VF_invalid,
        /// DICOM enumerated value: UNVERIFIED
        VF_Unverified,
        /// DICOM enumerated value: VERIFIED
        VF_Verified,
        /// internal type used to mark the last entry
        VF_last = VF_Verified
    };

    /** Specific character set
     */
    enum E_CharacterSet
    {
        /// internal type used to indicate an error
        CS_invalid,
        /// internal type used to indicate an unknown/unsupported character set
        CS_unknown = CS_invalid,
        /// ISO 646 (ISO-IR 6): ASCII
        CS_ASCII,
        /// internal type used to indicate the default character set
        CS_default = CS_ASCII,
        /// ISO-IR 100: Latin alphabet No. 1
        CS_Latin1,
        /// ISO-IR 101: Latin alphabet No. 2
        CS_Latin2,
        /// ISO-IR 109: Latin alphabet No. 3
        CS_Latin3,
        /// ISO-IR 110: Latin alphabet No. 4
        CS_Latin4,
        /// ISO-IR 144: Cyrillic
        CS_Cyrillic,
        /// ISO-IR 127: Arabic
        CS_Arabic,
        /// ISO-IR 126: Greek
        CS_Greek,
        /// ISO-IR 138: Hebrew
        CS_Hebrew,
        /// ISO-IR 148: Latin alphabet No. 5
        CS_Latin5,
        /// ISO-IR 166: Thai
        CS_Thai,
        /// ISO-IR 13/87: Japanese (Katakana/Romaji/Kanji)
        CS_Japanese,
        /// ISO-IR 6/149: Korean (Hangul/Hanja)
        CS_Korean,
        /// ISO-IR 6/58: Chinese
        CS_ChineseISO,
        /// GB18030: Chinese
        CS_ChineseGB18030,
        /// GBK: Chinese
        CS_ChineseGBK,
        /// UTF-8: Unicode in UTF-8
        CS_UTF8,
        /// internal type used to mark the last entry
        CS_last = CS_UTF8
    };

    /** Code value type
     */
    enum E_CodeValueType
    {
        /// determine code value type automatically
        CVT_auto,
        /// short code value (VR=SH)
        CVT_Short,
        /// long code value (VR=UC)
        CVT_Long,
        /// URN code value (VR=UR)
        CVT_URN
    };

    /** Add node mode
     */
    enum E_AddMode
    {
        /// add new node after current one (sibling)
        AM_afterCurrent,
        /// add new node before current one (sibling)
        AM_beforeCurrent,
        /// add new node below current one (after last child)
        AM_belowCurrent,
        /// add new node below current one (before first child)
        AM_belowCurrentBeforeFirstChild
    };


  // --- destructor ---

    /** destructor. (only needed to avoid compiler warnings)
     */
    virtual ~DSRTypes();


  // --- conversion functions ---

    /** convert SR document type to SOP class UID
     ** @param  documentType  SR document type to be converted
     ** @return SOP class UID if type is valid, empty string otherwise (never NULL)
     */
    static const char *documentTypeToSOPClassUID(const E_DocumentType documentType);

    /** convert SR document type to modality
     ** @param  documentType  SR document type to be converted
     ** @return modality if type is valid, empty string otherwise (never NULL)
     */
    static const char *documentTypeToModality(const E_DocumentType documentType);

    /** convert SR document type to readable name.
     *  Such a readable name is better suited for printing/rendering.
     ** @param  documentType  SR document type to be converted
     ** @return readable name if type is valid, "invalid ..." string otherwise (never NULL)
     */
    static const char *documentTypeToReadableName(const E_DocumentType documentType);

    /** convert SR document type to document title.
     *  This document title is used for printing/rendering.
     ** @param  documentType   SR document type to be converted
     *  @param  documentTitle  reference to variable where the resulting string is stored
     ** @return document title if type is valid, empty string otherwise (never NULL)
     */
    static const char *documentTypeToDocumentTitle(const E_DocumentType documentType,
                                                   OFString &documentTitle);

    /** check whether SR document type requires Enhanced General Equipment Module
     ** @param  documentType  SR document type to be checked
     ** @return OFTrue if Enhanced General Equipment Module is required, OFFalse otherwise
     */
    static OFBool requiresEnhancedEquipmentModule(const E_DocumentType documentType);

    /** check whether SR document type requires Timezone Module
     ** @param  documentType  SR document type to be checked
     ** @return OFTrue if Timezone Module is required, OFFalse otherwise
     */
    static OFBool requiresTimezoneModule(const E_DocumentType documentType);

    /** check whether SR document type requires Synchronization Module
     ** @param  documentType  SR document type to be checked
     ** @return OFTrue if Synchronization Module is required, OFFalse otherwise
     */
    static OFBool requiresSynchronizationModule(const E_DocumentType documentType);

    /** convert relationship type to DICOM defined term
     ** @param  relationshipType  relationship type to be converted
     ** @return defined term if type is valid, empty string otherwise (never NULL)
     */
    static const char *relationshipTypeToDefinedTerm(const E_RelationshipType relationshipType);

    /** convert relationship type to readable name.
     *  Such a readable name is better suited for printing/rendering.
     ** @param  relationshipType  relationship type to be converted
     ** @return readable name if type is valid, "invalid ..." string otherwise (never NULL)
     */
    static const char *relationshipTypeToReadableName(const E_RelationshipType relationshipType);

    /** convert value type to DICOM defined term
     ** @param  valueType  value type to be converted
     ** @return defined term if type is valid, empty string otherwise (never NULL)
     */
    static const char *valueTypeToDefinedTerm(const E_ValueType valueType);

    /** convert value type to XML tag name
     ** @param  valueType  value type to be converted
     ** @return XML tag name if type is valid, "item" otherwise (never NULL)
     */
    static const char *valueTypeToXMLTagName(const E_ValueType valueType);

    /** convert value type to readable name.
     *  Such a readable name is better suited for printing/rendering.
     ** @param  valueType  value type to be converted
     ** @return readable name if type is valid, "invalid ..." string otherwise (never NULL)
     */
    static const char *valueTypeToReadableName(const E_ValueType valueType);

    /** convert presentation state type to short name.
     *  Such a short name (e.g. "GSPS") is better suited for printing/rendering than a UID.
     ** @param  pstateType  presentation state type to be converted
     ** @return short name if type is valid, "invalid ..." string otherwise (never NULL)
     */
    static const char *presentationStateTypeToShortName(const E_PresentationStateType pstateType);

    /** convert graphic type to DICOM enumerated value
     ** @param  graphicType  graphic type to be converted
     ** @return enumerated value if type is valid, empty string otherwise (never NULL)
     */
    static const char *graphicTypeToEnumeratedValue(const E_GraphicType graphicType);

    /** convert graphic type to readable name.
     *  Such a readable name is better suited for printing/rendering.
     ** @param  graphicType  graphic type to be converted
     ** @return readable name if type is valid, "invalid ..." string otherwise (never NULL)
     */
    static const char *graphicTypeToReadableName(const E_GraphicType graphicType);

    /** convert graphic type (3D) to DICOM enumerated value
     ** @param  graphicType  graphic type (3D) to be converted
     ** @return enumerated value if type is valid, empty string otherwise (never NULL)
     */
    static const char *graphicType3DToEnumeratedValue(const E_GraphicType3D graphicType);

    /** convert graphic type (3D) to readable name.
     *  Such a readable name is better suited for printing/rendering.
     ** @param  graphicType  graphic type (3D) to be converted
     ** @return readable name if type is valid, "invalid ..." string otherwise (never NULL)
     */
    static const char *graphicType3DToReadableName(const E_GraphicType3D graphicType);

    /** convert temporal range type to DICOM enumerated value
     ** @param  temporalRangeType  temporal range type to be converted
     ** @return enumerated value if type is valid, "invalid ..." string otherwise (never NULL)
     */
    static const char *temporalRangeTypeToEnumeratedValue(const E_TemporalRangeType temporalRangeType);

    /** convert temporal range type to readable name.
     *  Such a readable name is better suited for printing/rendering.
     ** @param  temporalRangeType  temporal range type to be converted
     ** @return readable name if type is valid, "invalid ..." string otherwise (never NULL)
     */
    static const char *temporalRangeTypeToReadableName(const E_TemporalRangeType temporalRangeType);

    /** convert continuity of content flag to DICOM enumerated value
     ** @param  continuityOfContent  continuity of content flag to be converted
     ** @return enumerated value if flag is valid, empty string otherwise (never NULL)
     */
    static const char *continuityOfContentToEnumeratedValue(const E_ContinuityOfContent continuityOfContent);

    /** convert preliminary flag to DICOM enumerated value
     ** @param  preliminaryFlag  preliminary flag to be converted
     ** @return enumerated value if flag is valid, empty string otherwise (never NULL)
     */
    static const char *preliminaryFlagToEnumeratedValue(const E_PreliminaryFlag preliminaryFlag);

    /** convert completion flag to DICOM enumerated value
     ** @param  completionFlag  completion flag to be converted
     ** @return enumerated value if successful, empty string otherwise (never NULL)
     */
    static const char *completionFlagToEnumeratedValue(const E_CompletionFlag completionFlag);

    /** convert verification flag to DICOM enumerated value
     ** @param  verificationFlag  verification flag to be converted
     ** @return enumerated value if flag is valid, empty string otherwise (never NULL)
     */
    static const char *verificationFlagToEnumeratedValue(const E_VerificationFlag verificationFlag);

    /** convert character set to DICOM defined term
     ** @param  characterSet  character set to be converted
     ** @return defined term if character set is valid, empty string otherwise (never NULL)
     */
    static const char *characterSetToDefinedTerm(const E_CharacterSet characterSet);

    /** convert character set to HTML name.
     *  This HTML (IANA) name is used to specify the character set in an HTML document.
     ** @param  characterSet  character set to be converted
     ** @return HTML name if character set is valid, empty string or "?" otherwise (never NULL)
     */
    static const char *characterSetToHTMLName(const E_CharacterSet characterSet);

    /** convert character set to XML name.
     *  This XML name is used to specify the character set in an XML document.
     ** @param  characterSet  character set to be converted
     ** @return XML name if character set is valid, empty string or "?" otherwise (never NULL)
     */
    static const char *characterSetToXMLName(const E_CharacterSet characterSet);

    /** convert SOP class UID to SR document type
     ** @param  sopClassUID  SOP class UID to be converted
     ** @return SR document type if successful, DSRTypes::DT_invalid otherwise
     */
    static E_DocumentType sopClassUIDToDocumentType(const OFString &sopClassUID);

    /** convert DICOM defined term to relationship type
     ** @param  definedTerm  defined term to be converted
     ** @return relationship type if successful, DSRTypes::RT_invalid otherwise
     */
    static E_RelationshipType definedTermToRelationshipType(const OFString &definedTerm);

    /** convert DICOM defined term to value type
     ** @param  definedTerm  defined term to be converted
     ** @return value type if successful, DSRTypes::VT_invalid otherwise
     */
    static E_ValueType definedTermToValueType(const OFString &definedTerm);

    /** convert SOP class UID to presentation state type
     ** @param  sopClassUID  SOP class UID to be converted
     ** @return presentation state type if successful, DSRTypes::PT_invalid otherwise
     */
    static E_PresentationStateType sopClassUIDToPresentationStateType(const OFString &sopClassUID);

    /** convert XML tag name to value type
     ** @param  xmlTagName  XML tag name to be converted
     ** @return value type if successful, DSRTypes::VT_invalid otherwise
     */
    static E_ValueType xmlTagNameToValueType(const OFString &xmlTagName);

    /** convert DICOM enumerated value to graphic type
     ** @param  enumeratedValue  enumerated value to be converted
     ** @return graphic type if successful, DSRTypes::GT_invalid otherwise
     */
    static E_GraphicType enumeratedValueToGraphicType(const OFString &enumeratedValue);

    /** convert DICOM enumerated value to graphic type (3D)
     ** @param  enumeratedValue  enumerated value to be converted
     ** @return graphic type if successful, DSRTypes::GT3_invalid otherwise
     */
    static E_GraphicType3D enumeratedValueToGraphicType3D(const OFString &enumeratedValue);

    /** convert DICOM enumerated value to temporal range type
     ** @param  enumeratedValue  enumerated value to be converted
     ** @return temporal range type if successful, DSRTypes::TRT_invalid otherwise
     */
    static E_TemporalRangeType enumeratedValueToTemporalRangeType(const OFString &enumeratedValue);

    /** convert DICOM enumerated value to continuity of content flag
     ** @param  enumeratedValue  enumerated value to be converted
     ** @return continuity of content flag type if successful, DSRTypes::COC_invalid otherwise
     */
    static E_ContinuityOfContent enumeratedValueToContinuityOfContent(const OFString &enumeratedValue);

    /** convert DICOM enumerated value to preliminary flag
     ** @param  enumeratedValue  enumerated value to be converted
     ** @return preliminary flag type if successful, DSRTypes::PF_invalid otherwise
     */
    static E_PreliminaryFlag enumeratedValueToPreliminaryFlag(const OFString &enumeratedValue);

    /** convert DICOM enumerated value to completion flag
     ** @param  enumeratedValue  enumerated value to be converted
     ** @return completion flag type if successful, DSRTypes::CF_invalid otherwise
     */
    static E_CompletionFlag enumeratedValueToCompletionFlag(const OFString &enumeratedValue);

    /** convert DICOM enumerated value to verification flag
     ** @param  enumeratedValue  enumerated value to be converted
     ** @return verification flag type if successful, DSRTypes::VF_invalid otherwise
     */
    static E_VerificationFlag enumeratedValueToVerificationFlag(const OFString &enumeratedValue);

    /** convert DICOM defined term to character set.
     *  An empty defined term is mapped to DSRTypes::CS_default (which is ASCII).
     ** @param  definedTerm  defined term to be converted
     ** @return character set if successful, DSRTypes::CS_invalid otherwise
     */
    static E_CharacterSet definedTermToCharacterSet(const OFString &definedTerm);


  // --- misc helper functions ---

    /** check whether specified SR document type is supported by this library
     ** @param  documentType  SR document type to be checked
     ** @return status, OFTrue if SR document type is supported, OFFalse otherwise
     */
    static OFBool isDocumentTypeSupported(const E_DocumentType documentType);

    /** get current date in DICOM 'DA' format. (YYYYMMDD)
     ** @param  dateString  string used to store the current date.
     *                      ('19000101' if current date could not be retrieved)
     ** @return resulting character string (see 'dateString')
     */
    static const OFString &currentDate(OFString &dateString);

    /** get current time in DICOM 'TM' format. (HHMMSS)
     ** @param  timeString  string used to store the current time
     *                      ('000000' if current time could not be retrieved)
     ** @return resulting character string (see 'timeString')
     */
    static const OFString &currentTime(OFString &timeString);

    /** get current date and time in DICOM 'DT' format. (YYYYMMDDHHMMSS)
     *  The optional UTC notation (e.g. +0100) as well as the optional fractional second
     *  part are currently not supported.
     ** @param  dateTimeString  string used to store the current date and time
     *                          ('19000101000000' if current date/time could not
     *                           be retrieved)
     ** @return resulting character string (see 'dateTimeString')
     */
    static const OFString &currentDateTime(OFString &dateTimeString);

    /** get local timezone in DICOM format. (&ZZXX)
     ** @param  timezoneString  string used to store the local timezone
     *                          ('+0000' if timezone could not be retrieved)
     ** @return resulting character string (see 'timezoneString')
     */
    static const OFString &localTimezone(OFString &timezoneString);

    /** convert DICOM date string to readable format.
     *  The ISO format "YYYY-MM-DD" is used for the readable format.
     ** @param  dicomDate     date in DICOM DA format (YYYYMMDD)
     *  @param  readableDate  reference to variable where the resulting string is stored
     ** @return reference to resulting string (might be empty)
     */
    static const OFString &dicomToReadableDate(const OFString &dicomDate,
                                               OFString &readableDate);

    /** convert DICOM time string to readable format.
     *  The ISO format "HH:MM" or "HH:MM:SS" (if seconds are available) is used for the
     *  readable format.
     ** @param  dicomTime     time in DICOM TM format (HHMM or HHMMSS...)
     *  @param  readableTime  reference to variable where the resulting string is stored
     ** @return reference to resulting string (might be empty)
     */
    static const OFString &dicomToReadableTime(const OFString &dicomTime,
                                               OFString &readableTime);

    /** convert DICOM date time string to readable format.
     *  The format "YYYY-MM-DD, HH:MM" or "YYYY-MM-DD, HH:MM:SS" is used for the readable format.
     ** @param  dicomDateTime     time in DICOM DT format (YYYYMMDDHHMMSS...). Possible suffixes
     *                            (fractional second or UTC notation) are currently ignored.
     *  @param  readableDateTime  reference to variable where the resulting string is stored
     ** @return reference to resulting string (might be empty)
     */
    static const OFString &dicomToReadableDateTime(const OFString &dicomDateTime,
                                                   OFString &readableDateTime);

    /** convert DICOM person name to readable format.
     *  The format "<prefix> <first_name> <middle_name> <last_name>, <suffix>" is used for the
     *  readable format.
     *  Please note that only the first component group (characters before the first '=') of
     *  the DICOM person name is used - see DcmPersonName::getNameComponents() for details.
     ** @param  dicomPersonName     person name in DICOM PN format (ln^fn^mn^p^s)
     *  @param  readablePersonName  reference to variable where the resulting string is stored
     ** @return reference to resulting string (might be empty)
     */
    static const OFString &dicomToReadablePersonName(const OFString &dicomPersonName,
                                                     OFString &readablePersonName);

    /** convert DICOM person name to XML format.
     *  The tags \<prefix\>, \<first\>, \<middle\>, \<last\> and \<suffix\> are used for the XML
     *  format of a person name.  The string is automatically converted to the markup notation
     *  (see convertToMarkupString()).  Two tags are separated by a newline.
     *  Please note that only the first component group (characters before the first '=') of
     *  the DICOM person name is used - see DcmPersonName::getNameComponents() for details.
     ** @param  dicomPersonName  person name in DICOM PN format (ln^fn^mn^p^s)
     *  @param  xmlPersonName    reference to variable where the resulting string is stored
     *  @param  writeEmptyValue  optional flag indicating whether an empty value should be written
     ** @return reference to resulting string (might be empty)
     */
    static const OFString &dicomToXMLPersonName(const OFString &dicomPersonName,
                                                OFString &xmlPersonName,
                                                const OFBool writeEmptyValue = OFFalse);

    /** convert unsigned integer number to character string
     ** @param  number       unsigned integer number to be converted
     *  @param  stringValue  character string used to store the result
     ** @return pointer to the first character of the resulting string
     *          (may be NULL if 'string' was NULL)
     */
    static const char *numberToString(const size_t number,
                                      char *stringValue);

    /** convert string to unsigned integer number
     ** @param  stringValue  character string to be converted
     ** @return resulting unsigned integer number if successful, 0 otherwise
     */
    static size_t stringToNumber(const char *stringValue);

    /** convert character string to print string.
     *  This method is used to convert character strings for text "print" output.  Newline characters
     *  "\n" are replaced by "\\n", return characters "\r" by "\\r", etc.
     ** @param  sourceString  source string to be converted
     *  @param  printString   reference to variable where the result should be stored
     ** @return reference to resulting 'printString' (might be empty if 'sourceString' was empty)
     */
    static const OFString &convertToPrintString(const OFString &sourceString,
                                                OFString &printString);

    /** convert character string to HTML mnenonic string.
     *  Characters with special meaning for HTML (e.g. '<' and '&') are replace by the corresponding
     *  mnenonics (e.g. "&lt;" and "&amp;").
     *  If flag DSRTypes::HF_convertNonASCIICharacters is set, all characters > #127 are also converted
     *  (useful if only HTML 3.2 is supported which does not allow to specify the character set).
     ** @param  sourceString     source string to be converted
     *  @param  markupString     reference to variable where the result should be stored
     *  @param  flags            optional flags, only checking DSRTypes::HF_convertNonASCIICharacters,
     *                           DSRTypes::HF_HTML32Compatibility and DSRTypes::HF_XHTML11Compatibility
     *  @param  newlineAllowed   optional flag indicating whether newlines are allowed or not.
     *                           If they are allowed the text "<br>" is used, "&para;" otherwise.
     *                           The following combinations are accepted: LF, CR, LF CR, CF LF.
     ** @return reference to resulting 'markupString' (might be empty if 'sourceString' was empty)
     */
    static const OFString &convertToHTMLString(const OFString &sourceString,
                                               OFString &markupString,
                                               const size_t flags = 0,
                                               const OFBool newlineAllowed = OFFalse);

    /** convert character string to XML mnenonic string.
     *  Characters with special meaning for XML (e.g. '<' and '&') are replace by the
     *  corresponding mnenonics (e.g. "&lt;" and "&amp;").
     ** @param  sourceString  source string to be converted
     *  @param  markupString  reference to variable where the result should be stored
     ** @return reference to resulting 'markupString' (might be empty if 'sourceString' was empty)
     */
    static const OFString &convertToXMLString(const OFString &sourceString,
                                              OFString &markupString);

    /** check string for valid reference (as used for by-reference relationships).
     *  The string should be non-empty and consist of integer numbers only separated by a "." where
     *  the first and the last character of the string are always figures (0..9). Example: 1 or 1.2.3.
     *  Both the integer number 0 and a leading 0 for the numbers are forbidden.
     ** @param  stringValue  character string to be checked
     ** @result OFTrue if 'string' conforms to UID format, OFFalse otherwise
     */
    static OFBool checkForValidReference(const OFString &stringValue);

    /** create specified SR IOD content relationship constraint checker object.
     *  Please note that the created object has to be deleted by the caller.
     ** @param  documentType  associated SR document type for which the checker object is created
     ** @return pointer to new IOD checker object if successful, NULL if document type is not supported
     */
    static DSRIODConstraintChecker *createIODConstraintChecker(const E_DocumentType documentType);

    /** create specified document tree node.
     *  This is a shortcut and the only location where document tree nodes are created.
     *  It facilitates the introduction of new relationship/value types and the maintenance.
     ** @param  relationshipType  relationship type of the node to be created
     *  @param  valueType         value type of the node to be created
     ** @return pointer to the new document tree node if successful, NULL otherwise
     */
    static DSRDocumentTreeNode *createDocumentTreeNode(const E_RelationshipType relationshipType,
                                                       const E_ValueType valueType);


  // --- DICOM data structure access functions ---

    /** add given element to the dataset.
     *  The element is only added if 'result' is EC_Normal and the 'delem' pointer is not NULL.
     ** @param  result      reference to status variable (checked before adding and updated afterwards!)
     *  @param  dataset     reference to DICOM dataset to which the element should be added
     *  @param  delem       pointer to DICOM element which should be added. deleted if not inserted.
     *  @param  vm          value multiplicity (according to the data dictionary) to be checked for.
     *                      (See DcmElement::checkVM() for a list of valid values.)
     *                      Interpreted as cardinality (number of items) for sequence attributes.
     *  @param  type        value type (valid value: "1", "2" or something else which is not checked)
     *  @param  moduleName  optional module name to be printed (default: "SR document" if NULL)
     ** @return current value of 'result', EC_Normal if successful, an error code otherwise
     */
    static OFCondition addElementToDataset(OFCondition &result,
                                           DcmItem &dataset,
                                           DcmElement *delem,
                                           const OFString &vm,
                                           const OFString &type,
                                           const char *moduleName = NULL);

    /** remove given attribute from the sequence.
     *  All occurrences of the attribute in all items of the sequence are removed.
     ** @param  sequence  reference to DICOM sequence from which the attribute should be removed
     *  @param  tagKey    DICOM tag specifying the attribute which should be removed
     */
    static void removeAttributeFromSequence(DcmSequenceOfItems &sequence,
                                            const DcmTagKey &tagKey);

    /** get element from dataset
     ** @param  dataset  reference to DICOM dataset from which the element should be retrieved.
     *                   (Would be 'const' if the methods from 'dcmdata' would also be 'const'.)
     *  @param  delem    reference to DICOM element which should be retrieved.  The return value
     *                   is also stored in this parameter.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition getElementFromDataset(DcmItem &dataset,
                                             DcmElement &delem);

    /** get string value from element
     ** @param  delem  DICOM element from which the string value should be retrieved
     ** @return pointer to character string if successful, NULL otherwise
     */
    static const char *getStringValueFromElement(const DcmElement &delem);

   /** get string value from element.
    *  Please note that only the first element value is retrieved (in case of multiple values).
    ** @param  delem        DICOM element from which the string value should be retrieved
    *  @param  stringValue  reference to variable where the result should be stored
    ** @return reference character string if successful, empty string otherwise
    */
    static const OFString &getStringValueFromElement(const DcmElement &delem,
                                                     OFString &stringValue);

    /** get string value from element
     ** @param  delem        DICOM element from which the string value should be retrieved
     *  @param  stringValue  reference to variable in which the result should be stored
     *                       (This parameter is automatically cleared if an error occurs.)
     *  @param  pos          index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition getStringValueFromElement(const DcmElement &delem,
                                                 OFString &stringValue,
                                                 const signed long pos);

    /** get string value from element and convert to "print" format.
     *  Please note that only the first element value is retrieved (in case of multiple values).
     ** @param  delem        DICOM element from which the string value should be retrieved
     *  @param  stringValue  reference to variable in which the result should be stored
     ** @return reference character string if successful, empty string otherwise
     */
    static const OFString &getPrintStringFromElement(const DcmElement &delem,
                                                     OFString &stringValue);

    /** get string value from element and convert to HTML/XML.
     *  Please note that only the first element value is retrieved (in case of multiple values).
     ** @param  delem            DICOM element from which the string value should be retrieved
     *  @param  stringValue      reference to variable in which the result should be stored
     *  @param  convertNonASCII  convert non-ASCII characters (> #127) to numeric value (&\#nnn;) if OFTrue
     ** @return reference character string if successful, empty string otherwise
     */
    static const OFString &getMarkupStringFromElement(const DcmElement &delem,
                                                      OFString &stringValue,
                                                      const OFBool convertNonASCII = OFFalse);

    /** get string value from dataset
     ** @param  dataset      DICOM dataset from which the string should be retrieved.
     *                       (Would be 'const' if the methods from 'dcmdata' would also be 'const'.)
     *  @param  tagKey       DICOM tag specifying the attribute from which the string should be retrieved
     *  @param  stringValue  reference to variable in which the result should be stored
     *                       (This parameter is automatically cleared if the tag could not be found.)
     *  @param  pos          index of the value to get (0..vm-1), -1 for all components
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition getStringValueFromDataset(DcmItem &dataset,
                                                 const DcmTagKey &tagKey,
                                                 OFString &stringValue,
                                                 const signed long pos = 0);

    /** put string value to dataset
     ** @param  dataset      reference to DICOM dataset to which the string should be put.
     *  @param  tag          DICOM tag specifying the attribute to which the string should be put
     *  @param  stringValue  character string specifying the value to be set
     *  @param  allowEmpty   allow empty element to be inserted if OFTrue.
     *                       Do not insert empty element otherwise.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition putStringValueToDataset(DcmItem &dataset,
                                               const DcmTag &tag,
                                               const OFString &stringValue,
                                               const OFBool allowEmpty = OFTrue);

    /** check element value for correct value multiplicity and type.
     ** @param  delem            pointer to DICOM element to be checked (might be NULL)
     *  @param  tagKey           DICOM tag of the DICOM element the parameter 'delem' points to
     *  @param  vm               value multiplicity (according to the data dictionary) to be checked
     *                           for.  (See DcmElement::checkVM() for a list of valid values.)
     *                           Interpreted as cardinality (number of items) for sequence attributes.
     *  @param  type             value type (valid value: "1", "1C", "2", something else)
     *  @param  searchCond       optional flag indicating the status of a previous search() call
     *  @param  moduleName       optional module name to be printed (default: "SR document" if NULL)
     *  @param  acceptViolation  accept certain violations regarding the VR and VM if OFTrue.
     *                           A warning message will always be reported if the value in incorrect.
     ** @return OFTrue if element value is correct, OFFalse otherwise
     */
    static OFBool checkElementValue(DcmElement *delem,
                                    const DcmTagKey &tagKey,
                                    const OFString &vm,
                                    const OFString &type,
                                    const OFCondition &searchCond = EC_Normal,
                                    const char *moduleName = NULL,
                                    const OFBool acceptViolation = OFFalse);

    /** check element value for correct value multiplicity and type.
     ** @param  delem            DICOM element to be checked
     *  @param  vm               value multiplicity (according to the data dictionary) to be checked
     *                           for.  (See DcmElement::checkVM() for a list of valid values.)
     *                           Interpreted as cardinality (number of items) for sequence attributes.
     *  @param  type             value type (valid value: "1", "1C", "2", something else)
     *  @param  searchCond       optional flag indicating the status of a previous search() call
     *  @param  moduleName       optional module name to be printed (default: "SR document" if NULL)
     *  @param  acceptViolation  accept certain violations regarding the VR and VM if OFTrue.
     *                           A warning message will always be reported if the value in incorrect.
     ** @return OFTrue if element value is correct, OFFalse otherwise
     */
    static OFBool checkElementValue(DcmElement &delem,
                                    const OFString &vm,
                                    const OFString &type,
                                    const OFCondition &searchCond = EC_Normal,
                                    const char *moduleName = NULL,
                                    const OFBool acceptViolation = OFFalse);

    /** get element from dataset and check it for correct value multiplicity and type.
     ** @param  dataset          DICOM dataset from which the element should be retrieved.  (Would be
     *                           'const' if the methods from 'dcmdata' would also be 'const'.)
     *  @param  delem            DICOM element used to store the value (always creates a copy!)
     *  @param  vm               value multiplicity (according to the data dictionary) to be checked
     *                           for.  (See DcmElement::checkVM() for a list of valid values.)
     *                           Interpreted as cardinality (number of items) for sequence attributes.
     *  @param  type             value type (valid value: "1", "1C", "2", something else which is not
     *                           checked)
     *  @param  moduleName       optional module name to be printed (default: "SR document" if NULL)
     *  @param  acceptViolation  accept certain violations regarding the VR and VM if OFTrue.
     *                           A warning message will always be reported if the value in incorrect.
     ** @return status, EC_Normal if element could be retrieved and value is correct, an error code
     *          otherwise
     */
    static OFCondition getAndCheckElementFromDataset(DcmItem &dataset,
                                                     DcmElement &delem,
                                                     const OFString &vm,
                                                     const OFString &type,
                                                     const char *moduleName = NULL,
                                                     const OFBool acceptViolation = OFFalse);

    /** get string value from dataset and check it for correct value multiplicity and type.
     ** @param  dataset          DICOM dataset from which the element should be retrieved.  (Would be
     *                           'const' if the methods from 'dcmdata' would also be 'const'.)
     *  @param  tagKey           DICOM tag specifying the attribute from which the string should be
     *                           retrieved
     *  @param  stringValue      reference to variable in which the result should be stored.
     *                           (This parameter is automatically cleared if the tag could not be found.
     *                           It is not cleared if the retrieved string is invalid, e.g. violates VR
     *                           or VM definition.)
     *  @param  vm               value multiplicity (according to the data dictionary) to be checked
     *                           for.  (See DcmElement::checkVM() for a list of valid values.)
     *                           Interpreted as cardinality (number of items) for sequence attributes.
     *  @param  type             value type (valid value: "1", "1C", "2", something else which is not
     *                           checked)
     *  @param  moduleName       optional module name to be printed (default: "SR document" if NULL)
     *  @param  acceptViolation  accept certain violations regarding the VR and VM if OFTrue.
     *                           A warning message will always be reported if the value in incorrect.
     ** @return status, EC_Normal if element could be retrieved and value is correct, an error code
     *          otherwise
     */
    static OFCondition getAndCheckStringValueFromDataset(DcmItem &dataset,
                                                         const DcmTagKey &tagKey,
                                                         OFString &stringValue,
                                                         const OFString &vm,
                                                         const OFString &type,
                                                         const char *moduleName = NULL,
                                                         const OFBool acceptViolation = OFFalse);

  // --- output functions ---

    /** print the warning message that the current content item is invalid/incomplete.
     *  The value type (for DEBUG mode also the node ID) is added if the 'node' if specified.
     *  @param  action    text describing the current action (e.g. 'Reading'), 'Processing' if NULL
     *  @param  node      pointer to document tree node for which the message should be printed
     *  @param  location  position of the affected content item (e.g. '1.2.3', not printed if NULL)
     */
    static void printInvalidContentItemMessage(const char *action,
                                               const DSRDocumentTreeNode *node,
                                               const char *location = NULL);

    /** print an error message for the current content item.
     *  The value type (for DEBUG mode also the node ID) is added if the 'node' if specified.
     *  @param  action    text describing the current action (e.g. 'Reading'), 'Processing' if NULL
     *  @param  result    status used to print more information on the error (no message if EC_Normal)
     *  @param  node      pointer to document tree node for which the message should be printed
     *  @param  location  position of the affected content item (e.g. '1.2.3', not printed if NULL)
     */
    static void printContentItemErrorMessage(const char *action,
                                             const OFCondition &result,
                                             const DSRDocumentTreeNode *node,
                                             const char *location = NULL);

    /** print a warning message that an unknown/unsupported value has been determined
     *  @param  valueName  name of the unknown/unsupported value
     *  @param  readValue  value that has been read (optional)
     *  @param  action     text describing the current action (default: 'Reading'), 'Processing' if NULL
     */
    static void printUnknownValueWarningMessage(const char *valueName,
                                                const char *readValue = NULL,
                                                const char *action = "Reading");

    /** write string value to XML output stream.
     *  The output looks like this: "<" tagName ">" stringValue "</" tagName ">"
     ** @param  stream           output stream to which the XML document is written
     *  @param  stringValue      string value to be written
     *  @param  tagName          name of the XML tag used to surround the string value
     *  @param  writeEmptyValue  optional flag indicating whether an empty value should be written
     ** @return OFTrue if tag/value has been written, OFFalse otherwise
     */
    static OFBool writeStringValueToXML(STD_NAMESPACE ostream &stream,
                                        const OFString &stringValue,
                                        const OFString &tagName,
                                        const OFBool writeEmptyValue = OFFalse);

    /** write string value from DICOM element to XML output stream.
     *  The output looks like this: "<" tagName ">" stringValue "</" tagName ">"
     *  For elements with DICOM VR=PN the function dicomToXMLPersonName() is used internally.
     ** @param  stream           output stream to which the XML document is written
     *  @param  delem            DICOM element from which the value is retrieved
     *  @param  tagName          name of the XML tag used to surround the string value
     *  @param  writeEmptyValue  optional flag indicating whether an empty value should be written
     ** @return OFTrue if tag/value has been written, OFFalse otherwise
     */
    static OFBool writeStringFromElementToXML(STD_NAMESPACE ostream &stream,
                                              DcmElement &delem,
                                              const OFString &tagName,
                                              const OFBool writeEmptyValue = OFFalse);

    /** create an HTML annex entry with hyperlinks.
     *  A reference text is added to the main document and a new annex entry to the document annex
     *  with HTML hyperlinks between both.  Example for a reference: '[for details see Annex 1]'
     ** @param  docStream      output stream used for the main document
     *  @param  annexStream    output stream used for the document annex
     *  @param  referenceText  optional text added to the main document (e.g. 'for details see')
     *  @param  annexNumber    reference to the variable where the current annex number is stored.
     *                         Value is increased automatically by 1 after the new entry has been added.
     *  @param  flags          optional flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return current annex number after the new entry has been added
     */
    static size_t createHTMLAnnexEntry(STD_NAMESPACE ostream &docStream,
                                       STD_NAMESPACE ostream &annexStream,
                                       const OFString &referenceText,
                                       size_t &annexNumber,
                                       const size_t flags = 0);

    /** create an HTML footnote with hyperlinks
     ** @param  docStream       output stream used for the main document
     *  @param  footnoteStream  output stream used for the footnote text
     *  @param  footnoteNumber  reference to the variable where the current footnote number is stored.
     *                          Value is increased automatically by 1 after the new entry has been added.
     *  @param  nodeID          unique numerical identifier of the current node for which this footnote
     *                          is created.  Used to create a unique name for the hyperlink.
     *  @param  flags           optional flag used to customize the output (see DSRTypes::HF_xxx)
     ** @return current footnote number after the new entry has been added
     */
    static size_t createHTMLFootnote(STD_NAMESPACE ostream &docStream,
                                     STD_NAMESPACE ostream &footnoteStream,
                                     size_t &footnoteNumber,
                                     const size_t nodeID,
                                     const size_t flags = 0);

    /** append one output stream to another.
     ** @param  mainStream  stream to which the other should be added
     *  @param  tempStream  (string) stream to be added to the other
     *  @param  heading     (optional) string which is added to the 'mainStream' before the 'tempStream'.
     *                      This string is only added if 'tempStream' is not empty.
     ** @return status, EC_Normal if stream has been added successfully, an error code otherwise
     */
    static OFCondition appendStream(STD_NAMESPACE ostream &mainStream,
                                    OFOStringStream &tempStream,
                                    const char *heading = NULL);
};


#endif
