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


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrtextn.h"
#include "dcmtk/dcmsr/dsrcodtn.h"
#include "dcmtk/dcmsr/dsrnumtn.h"
#include "dcmtk/dcmsr/dsrdtitn.h"
#include "dcmtk/dcmsr/dsrdattn.h"
#include "dcmtk/dcmsr/dsrtimtn.h"
#include "dcmtk/dcmsr/dsruidtn.h"
#include "dcmtk/dcmsr/dsrpnmtn.h"
#include "dcmtk/dcmsr/dsrscotn.h"
#include "dcmtk/dcmsr/dsrsc3tn.h"
#include "dcmtk/dcmsr/dsrtcotn.h"
#include "dcmtk/dcmsr/dsrcomtn.h"
#include "dcmtk/dcmsr/dsrimgtn.h"
#include "dcmtk/dcmsr/dsrwavtn.h"
#include "dcmtk/dcmsr/dsrcontn.h"
#include "dcmtk/dcmsr/dsrreftn.h"
#include "dcmtk/dcmsr/dsrbascc.h"
#include "dcmtk/dcmsr/dsrenhcc.h"
#include "dcmtk/dcmsr/dsrcomcc.h"
#include "dcmtk/dcmsr/dsrkeycc.h"
#include "dcmtk/dcmsr/dsrmamcc.h"
#include "dcmtk/dcmsr/dsrchecc.h"
#include "dcmtk/dcmsr/dsrcolcc.h"
#include "dcmtk/dcmsr/dsrprocc.h"
#include "dcmtk/dcmsr/dsrxrdcc.h"
#include "dcmtk/dcmsr/dsrspecc.h"
#include "dcmtk/dcmsr/dsrmaccc.h"
#include "dcmtk/dcmsr/dsrimpcc.h"
#include "dcmtk/dcmsr/dsrc3dcc.h"
#include "dcmtk/dcmsr/dsrrrdcc.h"
#include "dcmtk/dcmsr/dsracqcc.h"
#include "dcmtk/dcmsr/dsrsaecc.h"
#include "dcmtk/dcmsr/dsrprdcc.h"
#include "dcmtk/dcmsr/dsrpficc.h"
#include "dcmtk/dcmsr/dsrplicc.h"

#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcvrda.h"
#include "dcmtk/dcmdata/dcvrdt.h"
#include "dcmtk/dcmdata/dcvrpn.h"
#include "dcmtk/dcmdata/dcvrtm.h"

#include "dcmtk/ofstd/ofstd.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"


/*---------------------------------*
 *  constant definitions (part 1)  *
 *---------------------------------*/

/* read flags */
const size_t DSRTypes::RF_readDigitalSignatures             = 1 <<  0;
const size_t DSRTypes::RF_acceptUnknownRelationshipType     = 1 <<  1;
const size_t DSRTypes::RF_acceptInvalidContentItemValue     = 1 <<  2;
const size_t DSRTypes::RF_ignoreRelationshipConstraints     = 1 <<  3;
const size_t DSRTypes::RF_ignoreContentItemErrors           = 1 <<  4;
const size_t DSRTypes::RF_skipInvalidContentItems           = 1 <<  5;
const size_t DSRTypes::RF_showCurrentlyProcessedItem        = 1 <<  6;

/* renderHTML flags */
const size_t DSRTypes::HF_neverExpandChildrenInline         = 1 <<  0;
const size_t DSRTypes::HF_alwaysExpandChildrenInline        = 1 <<  1;
const size_t DSRTypes::HF_renderInlineCodes                 = 1 <<  2;
const size_t DSRTypes::HF_useCodeDetailsTooltip             = 1 <<  3;
const size_t DSRTypes::HF_renderConceptNameCodes            = 1 <<  4;
const size_t DSRTypes::HF_renderNumericUnitCodes            = 1 <<  5;
const size_t DSRTypes::HF_useCodeMeaningAsUnit              = 1 <<  6;
const size_t DSRTypes::HF_renderPatientTitle                = 1 <<  7;
const size_t DSRTypes::HF_renderNoDocumentHeader            = 1 <<  8;
const size_t DSRTypes::HF_renderDcmtkFootnote               = 1 <<  9;
const size_t DSRTypes::HF_renderFullData                    = 1 << 10;
const size_t DSRTypes::HF_renderSectionTitlesInline         = 1 << 11;
const size_t DSRTypes::HF_copyStyleSheetContent             = 1 << 12;
const size_t DSRTypes::HF_HTML32Compatibility               = 1 << 13;
const size_t DSRTypes::HF_XHTML11Compatibility              = 1 << 14;
const size_t DSRTypes::HF_addDocumentTypeReference          = 1 << 15;
const size_t DSRTypes::HF_omitGeneratorMetaElement          = 1 << 16;
/* internal */
const size_t DSRTypes::HF_renderItemsSeparately             = 1 << 17;
const size_t DSRTypes::HF_renderItemInline                  = 1 << 18;
const size_t DSRTypes::HF_currentlyInsideAnnex              = 1 << 19;
const size_t DSRTypes::HF_createFootnoteReferences          = 1 << 20;
const size_t DSRTypes::HF_convertNonASCIICharacters         = 1 << 21;
/* shortcuts */
const size_t DSRTypes::HF_renderAllCodes                    = DSRTypes::HF_renderInlineCodes |
                                                              DSRTypes::HF_renderConceptNameCodes |
                                                              DSRTypes::HF_renderNumericUnitCodes;
const size_t DSRTypes::HF_internalUseOnly                   = DSRTypes::HF_renderItemsSeparately |
                                                              DSRTypes::HF_renderItemInline |
                                                              DSRTypes::HF_currentlyInsideAnnex |
                                                              DSRTypes::HF_createFootnoteReferences |
                                                              DSRTypes::HF_convertNonASCIICharacters;

/* read/writeXML flags */
const size_t DSRTypes::XF_writeEmptyTags                    = 1 << 0;
const size_t DSRTypes::XF_writeTemplateIdentification       = 1 << 1;
const size_t DSRTypes::XF_alwaysWriteItemIdentifier         = 1 << 2;
const size_t DSRTypes::XF_codeComponentsAsAttribute         = 1 << 3;
const size_t DSRTypes::XF_relationshipTypeAsAttribute       = 1 << 4;
const size_t DSRTypes::XF_valueTypeAsAttribute              = 1 << 5;
const size_t DSRTypes::XF_templateIdentifierAsAttribute     = 1 << 6;
const size_t DSRTypes::XF_useDcmsrNamespace                 = 1 << 7;
const size_t DSRTypes::XF_addSchemaReference                = 1 << 8;
const size_t DSRTypes::XF_validateSchema                    = 1 << 9;
const size_t DSRTypes::XF_templateElementEnclosesItems      = 1 << 10;
const size_t DSRTypes::XF_addCommentsForIncludedTemplate    = 1 << 11;
const size_t DSRTypes::XF_acceptEmptyStudySeriesInstanceUID = 1 << 12;
/* shortcuts */
const size_t DSRTypes::XF_encodeEverythingAsAttribute       = DSRTypes::XF_codeComponentsAsAttribute |
                                                              DSRTypes::XF_relationshipTypeAsAttribute |
                                                              DSRTypes::XF_valueTypeAsAttribute |
                                                              DSRTypes::XF_templateIdentifierAsAttribute;

/* print flags */
const size_t DSRTypes::PF_printItemPosition                 = 1 << 0;
const size_t DSRTypes::PF_shortenLongItemValues             = 1 << 1;
const size_t DSRTypes::PF_printSOPInstanceUID               = 1 << 2;
const size_t DSRTypes::PF_printConceptNameCodes             = 1 << 3;
const size_t DSRTypes::PF_printNoDocumentHeader             = 1 << 4;
const size_t DSRTypes::PF_printTemplateIdentification       = 1 << 5;
const size_t DSRTypes::PF_useANSIEscapeCodes                = 1 << 6;
const size_t DSRTypes::PF_printLongSOPClassName             = 1 << 7;
const size_t DSRTypes::PF_printSOPClassUID                  = 1 << 8;
const size_t DSRTypes::PF_printInvalidCodes                 = 1 << 9;
const size_t DSRTypes::PF_printNodeID                       = 1 << 10;
const size_t DSRTypes::PF_indicateEnhancedEncodingMode      = 1 << 11;
const size_t DSRTypes::PF_printAnnotation                   = 1 << 12;
const size_t DSRTypes::PF_hideIncludedTemplateNodes         = 1 << 13;
const size_t DSRTypes::PF_dontCountIncludedTemplateNodes    = 1 << 14;
/* shortcuts */
const size_t DSRTypes::PF_printAllCodes                     = DSRTypes::PF_printConceptNameCodes;

/* checkByReferenceRelationships modes */
const size_t DSRTypes::CM_updatePositionString              = 1 << 0;
const size_t DSRTypes::CM_updateNodeID                      = 1 << 1;
const size_t DSRTypes::CM_resetReferenceTargetFlag          = 1 << 2;

/* checkByReferenceRelationships bit masks (avoid conflicts!) */
const size_t DSRTypes::CB_maskPrintFlags                    = DSRTypes::PF_dontCountIncludedTemplateNodes;
const size_t DSRTypes::CB_maskReadFlags                     = DSRTypes::RF_acceptUnknownRelationshipType |
                                                              DSRTypes::RF_ignoreRelationshipConstraints |
                                                              DSRTypes::RF_showCurrentlyProcessedItem;


/*---------------------*
 *  type declarations  *
 *---------------------*/

struct S_DocumentTypeNameMap
{
    DSRTypes::E_DocumentType Type;
    const char *SOPClassUID;
    size_t ExtendedModules;
    const char *Modality;
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
    const char *XMLTagName;
    const char *ReadableName;
};


struct S_PresentationStateTypeNameMap
{
    DSRTypes::E_PresentationStateType Type;
    const char *SOPClassUID;
    const char *ShortName;
};


struct S_GraphicTypeNameMap
{
    DSRTypes::E_GraphicType Type;
    const char *EnumeratedValue;
    const char *ReadableName;
};


struct S_GraphicType3DNameMap
{
    DSRTypes::E_GraphicType3D Type;
    const char *EnumeratedValue;
    const char *ReadableName;
};


struct S_TemporalRangeTypeNameMap
{
    DSRTypes::E_TemporalRangeType Type;
    const char *EnumeratedValue;
    const char *ReadableName;
};


struct S_ContinuityOfContentNameMap
{
    DSRTypes::E_ContinuityOfContent Type;
    const char *EnumeratedValue;
};


struct S_PreliminaryFlagNameMap
{
    DSRTypes::E_PreliminaryFlag Type;
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


struct S_CharacterSetNameMap
{
    DSRTypes::E_CharacterSet Type;
    const char *DefinedTerm;
    const char *HTMLName;
    const char *XMLName;
};


/*---------------------------------*
 *  constant definitions (part 2)  *
 *---------------------------------*/

// conditions
makeOFConditionConst(SR_EC_UnknownDocumentType,                 OFM_dcmsr,  1, OF_error, "Unknown Document Type");
makeOFConditionConst(SR_EC_InvalidDocument,                     OFM_dcmsr,  2, OF_error, "Invalid Document");
makeOFConditionConst(SR_EC_InvalidDocumentTree,                 OFM_dcmsr,  3, OF_error, "Invalid Document Tree");
makeOFConditionConst(SR_EC_MandatoryAttributeMissing,           OFM_dcmsr,  4, OF_error, "Mandatory Attribute missing");
makeOFConditionConst(SR_EC_InvalidValue,                        OFM_dcmsr,  5, OF_error, "Invalid Value");
makeOFConditionConst(SR_EC_UnsupportedValue,                    OFM_dcmsr,  6, OF_error, "Unsupported Value");
makeOFConditionConst(SR_EC_UnknownValueType,                    OFM_dcmsr,  7, OF_error, "Unknown Value Type");
makeOFConditionConst(SR_EC_UnknownRelationshipType,             OFM_dcmsr,  8, OF_error, "Unknown Relationship Type");
makeOFConditionConst(SR_EC_InvalidByValueRelationship,          OFM_dcmsr,  9, OF_error, "Invalid by-value Relationship");
makeOFConditionConst(SR_EC_InvalidByReferenceRelationship,      OFM_dcmsr, 10, OF_error, "Invalid by-reference Relationship");
makeOFConditionConst(SR_EC_SOPInstanceNotFound,                 OFM_dcmsr, 11, OF_error, "SOP Instance not found");
makeOFConditionConst(SR_EC_DifferentSOPClassesForAnInstance,    OFM_dcmsr, 12, OF_error, "Different SOP Classes for an Instance");
makeOFConditionConst(SR_EC_CodingSchemeNotFound,                OFM_dcmsr, 13, OF_error, "Coding Scheme Designator not found");
makeOFConditionConst(SR_EC_CorruptedXMLStructure,               OFM_dcmsr, 14, OF_error, "Corrupted XML structure");
makeOFConditionConst(SR_EC_RepresentationNotAvailable,          OFM_dcmsr, 15, OF_error, "Representation not available");
makeOFConditionConst(SR_EC_CannotCreateIconImage,               OFM_dcmsr, 16, OF_error, "Cannot create Icon Image");
makeOFConditionConst(SR_EC_CannotAddContentItem,                OFM_dcmsr, 17, OF_error, "Cannot add Content Item");
makeOFConditionConst(SR_EC_InvalidConceptName,                  OFM_dcmsr, 18, OF_error, "Invalid Concept Name");
makeOFConditionConst(SR_EC_CannotInsertSubTree,                 OFM_dcmsr, 19, OF_error, "Cannot insert Subtree");
makeOFConditionConst(SR_EC_CannotChangeRelationshipType,        OFM_dcmsr, 20, OF_error, "Cannot change Relationship Type");
makeOFConditionConst(SR_EC_IncompatibleDocumentTree,            OFM_dcmsr, 21, OF_error, "Incompatible Document Tree");
makeOFConditionConst(SR_EC_ContentItemNotFound,                 OFM_dcmsr, 22, OF_error, "Content Item not found");
makeOFConditionConst(SR_EC_CannotRemoveSubTree,                 OFM_dcmsr, 23, OF_error, "Cannot remove Subtree");
makeOFConditionConst(SR_EC_EmptyDocumentTree,                   OFM_dcmsr, 24, OF_error, "Empty Document Tree");
makeOFConditionConst(SR_EC_InvalidContentItem,                  OFM_dcmsr, 25, OF_error, "Invalid Content Item");
makeOFConditionConst(SR_EC_CannotUseTemplateIdentification,     OFM_dcmsr, 26, OF_error, "Cannot use Template Identification");
makeOFConditionConst(SR_EC_NonExtensibleTemplate,               OFM_dcmsr, 27, OF_error, "Non-extensible Template");
makeOFConditionConst(SR_EC_NonExtensibleContextGroup,           OFM_dcmsr, 28, OF_error, "Non-extensible Context Group");
makeOFConditionConst(SR_EC_CodedEntryNotInContextGroup,         OFM_dcmsr, 29, OF_error, "Coded Entry not in Context Group");
makeOFConditionConst(SR_EC_CodedEntryInStandardContextGroup,    OFM_dcmsr, 30, OF_ok,    "Coded Entry in Context Group (Standard)");
makeOFConditionConst(SR_EC_CodedEntryIsExtensionOfContextGroup, OFM_dcmsr, 31, OF_ok,    "Coded Entry in Context Group (Extension)");
makeOFConditionConst(SR_EC_ValueSetConstraintViolated,          OFM_dcmsr, 32, OF_error, "Value Set Constraint violated");
makeOFConditionConst(SR_EC_InvalidTemplateStructure,            OFM_dcmsr, 33, OF_error, "Invalid Template Structure");
makeOFConditionConst(SR_EC_CannotProcessIncludedTemplates,      OFM_dcmsr, 34, OF_error, "Cannot process Document Tree with included Templates");

// NOTE:
// error codes 1000 and above are reserved for the submodule "cmr"


/* extended IOD modules (only used internally) */
const size_t EM_EnhancedEquipment = 1 << 0;
const size_t EM_Timezone          = 1 << 1;
const size_t EM_Synchronization   = 1 << 2;

static const S_DocumentTypeNameMap DocumentTypeNameMap[] =
{
    {DSRTypes::DT_invalid,                               "",                                               0,                                         "",   "invalid document type"},
    {DSRTypes::DT_BasicTextSR,                           UID_BasicTextSRStorage,                           0,                                         "SR", "Basic Text SR"},
    {DSRTypes::DT_EnhancedSR,                            UID_EnhancedSRStorage,                            0,                                         "SR", "Enhanced SR"},
    {DSRTypes::DT_ComprehensiveSR,                       UID_ComprehensiveSRStorage,                       0,                                         "SR", "Comprehensive SR"},
    {DSRTypes::DT_KeyObjectSelectionDocument,            UID_KeyObjectSelectionDocumentStorage,            0,                                         "KO", "Key Object Selection Document"},
    {DSRTypes::DT_MammographyCadSR,                      UID_MammographyCADSRStorage,                      0,                                         "SR", "Mammography CAD SR"},
    {DSRTypes::DT_ChestCadSR,                            UID_ChestCADSRStorage,                            0,                                         "SR", "Chest CAD SR"},
    {DSRTypes::DT_ColonCadSR,                            UID_ColonCADSRStorage,                            EM_EnhancedEquipment,                      "SR", "Colon CAD SR"},
    {DSRTypes::DT_ProcedureLog,                          UID_ProcedureLogStorage,                          EM_Synchronization,                        "SR", "Procedure Log"},
    {DSRTypes::DT_XRayRadiationDoseSR,                   UID_XRayRadiationDoseSRStorage,                   EM_EnhancedEquipment,                      "SR", "X-Ray Radiation Dose SR"},
    {DSRTypes::DT_SpectaclePrescriptionReport,           UID_SpectaclePrescriptionReportStorage,           EM_EnhancedEquipment,                      "SR", "Spectacle Prescription Report"},
    {DSRTypes::DT_MacularGridThicknessAndVolumeReport,   UID_MacularGridThicknessAndVolumeReportStorage,   EM_EnhancedEquipment,                      "SR", "Macular Grid Thickness and Volume Report"},
    {DSRTypes::DT_ImplantationPlanSRDocument,            UID_ImplantationPlanSRDocumentStorage,            EM_EnhancedEquipment,                      "SR", "Implantation Plan SR Document"},
    {DSRTypes::DT_Comprehensive3DSR,                     UID_Comprehensive3DSRStorage,                     0,                                         "SR", "Comprehensive 3D SR"},
    {DSRTypes::DT_RadiopharmaceuticalRadiationDoseSR,    UID_RadiopharmaceuticalRadiationDoseSRStorage,    EM_EnhancedEquipment,                      "SR", "Radiopharmaceutical Radiation Dose SR"},
    {DSRTypes::DT_ExtensibleSR,                          UID_ExtensibleSRStorage,                          EM_EnhancedEquipment,                      "SR", "Extensible SR"},
    {DSRTypes::DT_AcquisitionContextSR,                  UID_AcquisitionContextSRStorage,                  EM_EnhancedEquipment,                      "SR", "Acquisition Context SR"},
    {DSRTypes::DT_SimplifiedAdultEchoSR,                 UID_SimplifiedAdultEchoSRStorage,                 EM_EnhancedEquipment | EM_Timezone,        "SR", "Simplified Adult Echo SR"},
    {DSRTypes::DT_PatientRadiationDoseSR,                UID_PatientRadiationDoseSRStorage,                EM_EnhancedEquipment,                      "SR", "Patient Radiation Dose SR"},
    {DSRTypes::DT_PerformedImagingAgentAdministrationSR, UID_PerformedImagingAgentAdministrationSRStorage, EM_EnhancedEquipment | EM_Synchronization, "SR", "Performed Imaging Agent Administration SR"},
    {DSRTypes::DT_PlannedImagingAgentAdministrationSR,   UID_PlannedImagingAgentAdministrationSRStorage,   EM_EnhancedEquipment,                      "SR", "Planned Imaging Agent Administration SR"}
};


static const S_RelationshipTypeNameMap RelationshipTypeNameMap[] =
{
    {DSRTypes::RT_invalid,       "",                "invalid relationship type"},
    {DSRTypes::RT_unknown,       "",                "unknown relationship type"},
    {DSRTypes::RT_isRoot,        "",                "(is root)"},
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
    {DSRTypes::VT_invalid,          "",                "item",       "invalid/unknown value type"},
    {DSRTypes::VT_Text,             "TEXT",            "text",       "Text"},
    {DSRTypes::VT_Code,             "CODE",            "code",       "Code"},
    {DSRTypes::VT_Num,              "NUM",             "num",        "Number"},
    {DSRTypes::VT_DateTime,         "DATETIME",        "datetime",   "Date/Time"},
    {DSRTypes::VT_Date,             "DATE",            "date",       "Date"},
    {DSRTypes::VT_Time,             "TIME",            "time",       "Time"},
    {DSRTypes::VT_UIDRef,           "UIDREF",          "uidref",     "UID Reference"},
    {DSRTypes::VT_PName,            "PNAME",           "pname",      "Person Name"},
    {DSRTypes::VT_SCoord,           "SCOORD",          "scoord",     "Spatial Coordinates"},
    {DSRTypes::VT_SCoord3D,         "SCOORD3D",        "scoord3d",   "Spatial Coordinates (3D)"},
    {DSRTypes::VT_TCoord,           "TCOORD",          "tcoord",     "Temporal Coordinates"},
    {DSRTypes::VT_Composite,        "COMPOSITE",       "composite",  "Composite Object"},
    {DSRTypes::VT_Image,            "IMAGE",           "image",      "Image"},
    {DSRTypes::VT_Waveform,         "WAVEFORM",        "waveform",   "Waveform"},
    {DSRTypes::VT_Container,        "CONTAINER",       "container",  "Container"},
    {DSRTypes::VT_byReference,      "(by-reference)",  "reference",  "(by-reference)"},
    {DSRTypes::VT_includedTemplate, "(incl-template)", "INCL-TEMPL", "(included template)"}     // the XML name should never be used!
};


static const S_PresentationStateTypeNameMap PresentationStateTypeNameMap[] =
{
    {DSRTypes::PT_invalid,                  "",                                                             "invalid/unknown presentation state type"},
    {DSRTypes::PT_Grayscale,                UID_GrayscaleSoftcopyPresentationStateStorage,                  "GSPS"},
    {DSRTypes::PT_Color,                    UID_ColorSoftcopyPresentationStateStorage,                      "CSPS"},
    {DSRTypes::PT_PseudoColor,              UID_PseudoColorSoftcopyPresentationStateStorage,                "PCSPS"},
    {DSRTypes::PT_Blending,                 UID_BlendingSoftcopyPresentationStateStorage,                   "BSPS"},
    {DSRTypes::PT_XAXRFGrayscale,           UID_XAXRFGrayscaleSoftcopyPresentationStateStorage,             "XGSPS"},
    {DSRTypes::PT_GrayscalePlanarMPR,       UID_GrayscalePlanarMPRVolumetricPresentationStateStorage,       "GP-VPS"},
    {DSRTypes::PT_CompositingPlanarMPR,     UID_CompositingPlanarMPRVolumetricPresentationStateStorage,     "CP-VPS"},
    {DSRTypes::PT_AdvancedBlending,         UID_AdvancedBlendingPresentationStateStorage,                   "ABPS"},
    {DSRTypes::PT_VolumeRendering,          UID_VolumeRenderingVolumetricPresentationStateStorage,          "VR-VPS"},
    {DSRTypes::PT_SegmentedVolumeRendering, UID_SegmentedVolumeRenderingVolumetricPresentationStateStorage, "SVR-VPS"},
    {DSRTypes::PT_MultipleVolumeRendering,  UID_MultipleVolumeRenderingVolumetricPresentationStateStorage,  "MVR-VPS"}
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


static const S_GraphicType3DNameMap GraphicType3DNameMap[] =
{
    {DSRTypes::GT3_invalid,    "",           "invalid/unknown graphic type"},
    {DSRTypes::GT3_Point,      "POINT",      "Point"},
    {DSRTypes::GT3_Multipoint, "MULTIPOINT", "Multiple Points"},
    {DSRTypes::GT3_Polyline,   "POLYLINE",   "Polyline"},
    {DSRTypes::GT3_Polygon ,   "POLYGON",    "Polygon"},
    {DSRTypes::GT3_Ellipse,    "ELLIPSE",    "Ellipse"},
    {DSRTypes::GT3_Ellipsoid,  "ELLIPSOID",  "Ellipsoid"}
};


static const S_TemporalRangeTypeNameMap TemporalRangeTypeNameMap[] =
{
    {DSRTypes::TRT_invalid,      "",             "invalid/unknown temporal range type"},
    {DSRTypes::TRT_Point,        "POINT",        "Point"},
    {DSRTypes::TRT_Multipoint,   "MULTIPOINT",   "Multiple Points"},
    {DSRTypes::TRT_Segment,      "SEGMENT",      "Segment"},
    {DSRTypes::TRT_Multisegment, "MULTISEGMENT", "Multiple Segments"},
    {DSRTypes::TRT_Begin,        "BEGIN",        "Begin"},
    {DSRTypes::TRT_End,          "END",          "End"}
};


static const S_ContinuityOfContentNameMap ContinuityOfContentNameMap[] =
{
    {DSRTypes::COC_invalid,    ""},
    {DSRTypes::COC_Separate,   "SEPARATE"},
    {DSRTypes::COC_Continuous, "CONTINUOUS"}
};


static const S_PreliminaryFlagNameMap PreliminaryFlagNameMap[] =
{
    {DSRTypes::PF_invalid,      ""},
    {DSRTypes::PF_Preliminary,  "PRELIMINARY"},
    {DSRTypes::PF_Final,        "FINAL"}
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


static const S_CharacterSetNameMap CharacterSetNameMap[] =
{
    // columns: enum, DICOM, HTML, XML (if "?" a warning is reported).
    // This mapping is based on Table D-1 in DICOM PS 3.18 Annex D.
    {DSRTypes::CS_invalid,        "",                               "",            ""},
    {DSRTypes::CS_ASCII,          "ISO_IR 6",                       "",            "UTF-8"},   /* "ISO_IR 6" is only used for reading */
    {DSRTypes::CS_Latin1,         "ISO_IR 100",                     "ISO-8859-1",  "ISO-8859-1"},
    {DSRTypes::CS_Latin2,         "ISO_IR 101",                     "ISO-8859-2",  "ISO-8859-2"},
    {DSRTypes::CS_Latin3,         "ISO_IR 109",                     "ISO-8859-3",  "ISO-8859-3"},
    {DSRTypes::CS_Latin4,         "ISO_IR 110",                     "ISO-8859-4",  "ISO-8859-4"},
    {DSRTypes::CS_Cyrillic,       "ISO_IR 144",                     "ISO-8859-5",  "ISO-8859-5"},
    {DSRTypes::CS_Arabic,         "ISO_IR 127",                     "ISO-8859-6",  "ISO-8859-6"},
    {DSRTypes::CS_Greek,          "ISO_IR 126",                     "ISO-8859-7",  "ISO-8859-7"},
    {DSRTypes::CS_Hebrew,         "ISO_IR 138",                     "ISO-8859-8",  "ISO-8859-8"},
    {DSRTypes::CS_Latin5,         "ISO_IR 148",                     "ISO-8859-9",  "ISO-8859-9"},
    {DSRTypes::CS_Thai,           "ISO_IR 166",                     "TIS-620",     "TIS-620"},
    {DSRTypes::CS_Japanese,       "ISO 2022 IR 13\\ISO 2022 IR 87", "ISO-2022-JP", "ISO-2022-JP"},
    {DSRTypes::CS_Korean,         "ISO 2022 IR 6\\ISO 2022 IR 149", "ISO-2022-KR", "ISO-2022-KR"},
    {DSRTypes::CS_ChineseISO,     "ISO 2022 IR 6\\ISO 2022 IR 58",  "ISO-2022-CN", "ISO-2022-CN"},
    {DSRTypes::CS_ChineseGB18030, "GB18030",                        "GB18030",     "GB18030"},
    {DSRTypes::CS_ChineseGBK,     "GBK",                            "GBK",         "GBK"},
    {DSRTypes::CS_UTF8,           "ISO_IR 192",                     "UTF-8",       "UTF-8"}
};


/*--------------------*
 *  global variables  *
 *--------------------*/

OFLogger DCM_dcmsrLogger = OFLog::getLogger("dcmtk.dcmsr");


/*------------------*
 *  implementation  *
 *------------------*/

DSRTypes::~DSRTypes()
{
}


const char *DSRTypes::documentTypeToSOPClassUID(const E_DocumentType documentType)
{
    const S_DocumentTypeNameMap *iterator = DocumentTypeNameMap;
    while ((iterator->Type != DT_last) && (iterator->Type != documentType))
        iterator++;
    return iterator->SOPClassUID;
}


const char *DSRTypes::documentTypeToModality(const E_DocumentType documentType)
{
    const S_DocumentTypeNameMap *iterator = DocumentTypeNameMap;
    while ((iterator->Type != DT_last) && (iterator->Type != documentType))
        iterator++;
    return iterator->Modality;
}


const char *DSRTypes::documentTypeToReadableName(const E_DocumentType documentType)
{
    const S_DocumentTypeNameMap *iterator = DocumentTypeNameMap;
    while ((iterator->Type != DT_last) && (iterator->Type != documentType))
        iterator++;
    return iterator->ReadableName;
}


const char *DSRTypes::documentTypeToDocumentTitle(const E_DocumentType documentType,
                                                  OFString &documentTitle)
{
    if (documentType != DT_invalid)
    {
        documentTitle = documentTypeToReadableName(documentType);
        // avoid doubling of term "Document" and/or "Report"
        if (!documentTitle.empty() && (documentTitle.find("Document") == OFString_npos) &&
                                      (documentTitle.find("Report") == OFString_npos))
        {
            documentTitle += " Document";
        }
    } else {
        // return empty string in case of invalid document
        documentTitle.clear();
    }
    return documentTitle.c_str();
}


OFBool DSRTypes::requiresEnhancedEquipmentModule(const E_DocumentType documentType)
{
    const S_DocumentTypeNameMap *iterator = DocumentTypeNameMap;
    while ((iterator->Type != DT_last) && (iterator->Type != documentType))
        iterator++;
    return (iterator->ExtendedModules & EM_EnhancedEquipment) > 0;
}


OFBool DSRTypes::requiresTimezoneModule(const E_DocumentType documentType)
{
    const S_DocumentTypeNameMap *iterator = DocumentTypeNameMap;
    while ((iterator->Type != DT_last) && (iterator->Type != documentType))
        iterator++;
    return (iterator->ExtendedModules & EM_Timezone) > 0;
}


OFBool DSRTypes::requiresSynchronizationModule(const E_DocumentType documentType)
{
    const S_DocumentTypeNameMap *iterator = DocumentTypeNameMap;
    while ((iterator->Type != DT_last) && (iterator->Type != documentType))
        iterator++;
    return (iterator->ExtendedModules & EM_Synchronization) > 0;
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


const char *DSRTypes::valueTypeToXMLTagName(const E_ValueType valueType)
{
    const S_ValueTypeNameMap *iterator = ValueTypeNameMap;
    while ((iterator->Type != VT_last) && (iterator->Type != valueType))
        iterator++;
    return iterator->XMLTagName;
}


const char *DSRTypes::valueTypeToReadableName(const E_ValueType valueType)
{
    const S_ValueTypeNameMap *iterator = ValueTypeNameMap;
    while ((iterator->Type != VT_last) && (iterator->Type != valueType))
        iterator++;
    return iterator->ReadableName;
}


const char *DSRTypes::presentationStateTypeToShortName(const E_PresentationStateType pstateType)
{
    const S_PresentationStateTypeNameMap *iterator = PresentationStateTypeNameMap;
    while ((iterator->Type != PT_last) && (iterator->Type != pstateType))
        iterator++;
    return iterator->ShortName;
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


const char *DSRTypes::graphicType3DToEnumeratedValue(const E_GraphicType3D graphicType)
{
    const S_GraphicType3DNameMap *iterator = GraphicType3DNameMap;
    while ((iterator->Type != GT3_last) && (iterator->Type != graphicType))
        iterator++;
    return iterator->EnumeratedValue;
}


const char *DSRTypes::graphicType3DToReadableName(const E_GraphicType3D graphicType)
{
    const S_GraphicType3DNameMap *iterator = GraphicType3DNameMap;
    while ((iterator->Type != GT3_last) && (iterator->Type != graphicType))
        iterator++;
    return iterator->ReadableName;
}


const char *DSRTypes::temporalRangeTypeToEnumeratedValue(const E_TemporalRangeType temporalRangeType)
{
    const S_TemporalRangeTypeNameMap *iterator = TemporalRangeTypeNameMap;
    while ((iterator->Type != TRT_last) && (iterator->Type != temporalRangeType))
        iterator++;
    return iterator->EnumeratedValue;
}


const char *DSRTypes::temporalRangeTypeToReadableName(const E_TemporalRangeType temporalRangeType)
{
    const S_TemporalRangeTypeNameMap *iterator = TemporalRangeTypeNameMap;
    while ((iterator->Type != TRT_last) && (iterator->Type != temporalRangeType))
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


const char *DSRTypes::preliminaryFlagToEnumeratedValue(const E_PreliminaryFlag preliminaryFlag)
{
    const S_PreliminaryFlagNameMap *iterator = PreliminaryFlagNameMap;
    while ((iterator->Type != PF_last) && (iterator->Type != preliminaryFlag))
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


const char *DSRTypes::characterSetToDefinedTerm(const E_CharacterSet characterSet)
{
    const S_CharacterSetNameMap *iterator = CharacterSetNameMap;
    /* make sure that we never return "ISO_IR 6", but an empty string instead */
    if (characterSet != CS_ASCII)
    {
        while ((iterator->Type != CS_last) && (iterator->Type != characterSet))
            iterator++;
    }
    return iterator->DefinedTerm;
}


const char *DSRTypes::characterSetToHTMLName(const E_CharacterSet characterSet)
{
    const S_CharacterSetNameMap *iterator = CharacterSetNameMap;
    while ((iterator->Type != CS_last) && (iterator->Type != characterSet))
        iterator++;
    return iterator->HTMLName;
}


const char *DSRTypes::characterSetToXMLName(const E_CharacterSet characterSet)
{
    const S_CharacterSetNameMap *iterator = CharacterSetNameMap;
    while ((iterator->Type != CS_last) && (iterator->Type != characterSet))
        iterator++;
    return iterator->XMLName;
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


DSRTypes::E_PresentationStateType DSRTypes::sopClassUIDToPresentationStateType(const OFString &sopClassUID)
{
    E_PresentationStateType type = PT_invalid;
    const S_PresentationStateTypeNameMap *iterator = PresentationStateTypeNameMap;
    while ((iterator->Type != PT_last) && (sopClassUID != iterator->SOPClassUID))
        iterator++;
    if (sopClassUID == iterator->SOPClassUID)
        type = iterator->Type;
    return type;
}


DSRTypes::E_ValueType DSRTypes::xmlTagNameToValueType(const OFString &xmlTagName)
{
    E_ValueType type = VT_invalid;
    const S_ValueTypeNameMap *iterator = ValueTypeNameMap;
    while ((iterator->Type != VT_last) && (xmlTagName != iterator->XMLTagName))
        iterator++;
    if (xmlTagName == iterator->XMLTagName)
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


DSRTypes::E_GraphicType3D DSRTypes::enumeratedValueToGraphicType3D(const OFString &enumeratedValue)
{
    E_GraphicType3D type = GT3_invalid;
    const S_GraphicType3DNameMap *iterator = GraphicType3DNameMap;
    while ((iterator->Type != GT3_last) && (enumeratedValue != iterator->EnumeratedValue))
        iterator++;
    if (enumeratedValue == iterator->EnumeratedValue)
        type = iterator->Type;
    return type;
}


DSRTypes::E_TemporalRangeType DSRTypes::enumeratedValueToTemporalRangeType(const OFString &enumeratedValue)
{
    E_TemporalRangeType type = TRT_invalid;
    const S_TemporalRangeTypeNameMap *iterator = TemporalRangeTypeNameMap;
    while ((iterator->Type != TRT_last) && (enumeratedValue != iterator->EnumeratedValue))
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


DSRTypes::E_PreliminaryFlag DSRTypes::enumeratedValueToPreliminaryFlag(const OFString &enumeratedValue)
{
    E_PreliminaryFlag type = PF_invalid;
    const S_PreliminaryFlagNameMap *iterator = PreliminaryFlagNameMap;
    while ((iterator->Type != PF_last) && (enumeratedValue != iterator->EnumeratedValue))
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


DSRTypes::E_CharacterSet DSRTypes::definedTermToCharacterSet(const OFString &definedTerm)
{
    E_CharacterSet type = CS_default;
    /* empty defined term means default */
    if (!definedTerm.empty())
    {
        const S_CharacterSetNameMap *iterator = CharacterSetNameMap;
        while ((iterator->Type != CS_last) && (definedTerm != iterator->DefinedTerm))
            iterator++;
        if (definedTerm == iterator->DefinedTerm)
            type = iterator->Type;
        else
            type = CS_invalid;
    }
    return type;
}


OFBool DSRTypes::isDocumentTypeSupported(const E_DocumentType documentType)
{
    return (documentType != DT_invalid) && (documentType != DT_ExtensibleSR);
}


OFCondition DSRTypes::addElementToDataset(OFCondition &result,
                                          DcmItem &dataset,
                                          DcmElement *delem,
                                          const OFString &vm,
                                          const OFString &type,
                                          const char *moduleName)
{
    if (delem != NULL)
    {
        OFBool triedToInsert = OFFalse;
        if (result.good())
        {
            if ((type == "2") || !delem->isEmpty())
            {
                triedToInsert = OFTrue;
                /* insert non-empty element or empty "type 2" element */
                result = dataset.insert(delem, OFTrue /*replaceOld*/);
                if (DCM_dcmsrLogger.isEnabledFor(OFLogger::WARN_LOG_LEVEL))
                    checkElementValue(*delem, vm, type, result, moduleName);
            }
            else if (type == "1")
            {
                /* empty element value not allowed for "type 1" */
                result = SR_EC_InvalidValue;
                if (DCM_dcmsrLogger.isEnabledFor(OFLogger::WARN_LOG_LEVEL))
                    checkElementValue(*delem, vm, type, result, moduleName);
            }
        }
        /* delete element if not inserted into the dataset */
        if (result.bad() || !triedToInsert)
            delete delem;
    } else
        result = EC_MemoryExhausted;
    return result;
}


void DSRTypes::removeAttributeFromSequence(DcmSequenceOfItems &sequence,
                                           const DcmTagKey &tagKey)
{
    /* iterate over all sequence items */
    DcmObject *dobj = NULL;
    while ((dobj = sequence.nextInContainer(dobj)) != NULL)
    {
        DcmItem *ditem = OFstatic_cast(DcmItem *, dobj);
        /* remove specified data elements (on all nesting levels) */
        ditem->findAndDeleteElement(tagKey, OFTrue /*allOccurrences*/);
    }
}


OFCondition DSRTypes::getElementFromDataset(DcmItem &dataset,
                                            DcmElement &delem)
{
    DcmStack stack;
    OFCondition result = dataset.search(delem.getTag(), stack, ESM_fromHere, OFFalse /*searchIntoSub*/);
    if (result.good())
    {
        /* copy object from search stack */
        result = delem.copyFrom(*stack.top());
    }
    return result;
}


const char *DSRTypes::getStringValueFromElement(const DcmElement &delem)
{
    char *stringValue = NULL;
    if (OFconst_cast(DcmElement &, delem).getString(stringValue).bad())
        stringValue = NULL;
    return stringValue;
}


const OFString &DSRTypes::getStringValueFromElement(const DcmElement &delem,
                                                    OFString &stringValue)
{
    if (OFconst_cast(DcmElement &, delem).getOFString(stringValue, 0).bad())
        stringValue.clear();
    return stringValue;
}


OFCondition DSRTypes::getStringValueFromElement(const DcmElement &delem,
                                                OFString &stringValue,
                                                const signed long pos)
{
    OFCondition result = EC_Normal;
    if (pos < 0)
        result = OFconst_cast(DcmElement &, delem).getOFStringArray(stringValue);
    else
        result = OFconst_cast(DcmElement &, delem).getOFString(stringValue, OFstatic_cast(unsigned long, pos));
    if (result.bad())
        stringValue.clear();
    return result;
}


const OFString &DSRTypes::getPrintStringFromElement(const DcmElement &delem,
                                                    OFString &stringValue)
{
    OFString tempString;
    return convertToPrintString(getStringValueFromElement(delem, tempString), stringValue);
}


const OFString &DSRTypes::getMarkupStringFromElement(const DcmElement &delem,
                                                     OFString &stringValue,
                                                     const OFBool convertNonASCII)
{
    OFString tempString;
    return OFStandard::convertToMarkupString(getStringValueFromElement(delem, tempString), stringValue, convertNonASCII);
}


OFCondition DSRTypes::getStringValueFromDataset(DcmItem &dataset,
                                                const DcmTagKey &tagKey,
                                                OFString &stringValue,
                                                const signed long pos)
{
    OFCondition result = EC_Normal;
    if (pos < 0)
        result = dataset.findAndGetOFStringArray(tagKey, stringValue, OFFalse /*searchIntoSub*/);
    else
        result = dataset.findAndGetOFString(tagKey, stringValue, OFstatic_cast(unsigned long, pos), OFFalse /*searchIntoSub*/);
    return result;
}


OFCondition DSRTypes::putStringValueToDataset(DcmItem &dataset,
                                              const DcmTag &tag,
                                              const OFString &stringValue,
                                              const OFBool allowEmpty)
{
    OFCondition result = EC_Normal;
    if (allowEmpty || !stringValue.empty())
        result = dataset.putAndInsertOFStringArray(tag, stringValue, OFTrue /*replaceOld*/);
    return result;
}


OFBool DSRTypes::checkElementValue(DcmElement *delem,
                                   const DcmTagKey &tagKey,
                                   const OFString &vm,
                                   const OFString &type,
                                   const OFCondition &searchCond,
                                   const char *moduleName,
                                   const OFBool acceptViolation)
{
    OFBool result = OFTrue;
    const OFString tagName = DcmTag(tagKey).getTagName();
    const OFString module = (moduleName == NULL) ? "SR document" : moduleName;
    /* NB: type 1C and 2C cannot be checked, assuming to be optional */
    if (((type == "1") || (type == "2")) && searchCond.bad())
    {
        DCMSR_WARN(tagName << " " << tagKey << " absent in " << module << " (type " << type << ")");
        result = OFFalse;
    }
    else if ((delem == NULL) || delem->isEmpty(OFTrue /*normalize*/))
    {
        /* however, type 1C should never be present with empty value */
        if (((type == "1") || (type == "1C")) && searchCond.good())
        {
            DCMSR_WARN(tagName << " " << tagKey << " empty in " << module << " (type " << type << ")");
            result = OFFalse;
        }
    } else {
        const OFCondition checkResult = delem->checkValue(vm, OFTrue /*oldFormat*/);
        if (checkResult == EC_InvalidCharacter)
        {
            DCMSR_WARN(tagName << " " << tagKey << " contains invalid character(s) in " << module);
            result = acceptViolation;
        }
        else if (checkResult == EC_ValueRepresentationViolated)
        {
            DCMSR_WARN(tagName << " " << tagKey << " violates VR definition in " << module);
            result = acceptViolation;
        }
        else if (checkResult == EC_ValueMultiplicityViolated)
        {
            const OFString vmText = (delem->getVR() == EVR_SQ) ? " #items" : " VM";
            DCMSR_WARN(tagName << " " << tagKey << vmText << " != " << vm << " in " << module);
            result = acceptViolation;
        }
        else if (checkResult == EC_MaximumLengthViolated)
        {
            DCMSR_WARN(tagName << " " << tagKey << " violates maximum VR length in " << module);
            result = acceptViolation;
        }
        else if (checkResult.bad())
        {
            DCMSR_DEBUG("INTERNAL ERROR while checking value of " << tagName << " " << tagKey << " in " << module);
        }
    }
    return result;
}


OFBool DSRTypes::checkElementValue(DcmElement &delem,
                                   const OFString &vm,
                                   const OFString &type,
                                   const OFCondition &searchCond,
                                   const char *moduleName,
                                   const OFBool acceptViolation)
{
    /* call the real function */
    return checkElementValue(&delem, delem.getTag(), vm, type, searchCond, moduleName, acceptViolation);
}


OFCondition DSRTypes::getAndCheckElementFromDataset(DcmItem &dataset,
                                                    DcmElement &delem,
                                                    const OFString &vm,
                                                    const OFString &type,
                                                    const char *moduleName,
                                                    const OFBool acceptViolation)
{
    DcmStack stack;
    const DcmTagKey tagKey = delem.getTag();
    OFCondition result = dataset.search(tagKey, stack, ESM_fromHere, OFFalse /*searchIntoSub*/);
    if (result.good())
    {
        /* copy object from search stack */
        result = delem.copyFrom(*stack.top());
        /* we need a reference to the original element in order to determine the SpecificCharacterSet */
        if (!checkElementValue(OFstatic_cast(DcmElement *, stack.top()), tagKey, vm, type, result, moduleName, acceptViolation))
            result = SR_EC_InvalidValue;
    }
    /* the element could not be found in the dataset */
    else if (!checkElementValue(delem, vm, type, result, moduleName, acceptViolation))
        result = SR_EC_InvalidValue;
    return result;
}


OFCondition DSRTypes::getAndCheckStringValueFromDataset(DcmItem &dataset,
                                                        const DcmTagKey &tagKey,
                                                        OFString &stringValue,
                                                        const OFString &vm,
                                                        const OFString &type,
                                                        const char *moduleName,
                                                        const OFBool acceptViolation)
{
    DcmStack stack;
    OFCondition result = dataset.search(tagKey, stack, ESM_fromHere, OFFalse /*searchIntoSub*/);
    if (result.good())
    {
        DcmElement *delem = OFstatic_cast(DcmElement *, stack.top());
        /* we need a reference to the original element in order to determine the SpecificCharacterSet */
        if (!checkElementValue(delem, tagKey, vm, type, result, moduleName, acceptViolation))
            result = SR_EC_InvalidValue;
        delem->getOFString(stringValue, 0);
    } else {
        if ((type == "1") || (type == "2"))
        {
            const OFString tagName = DcmTag(tagKey).getTagName();
            const OFString module = (moduleName == NULL) ? "SR document" : moduleName;
            DCMSR_WARN(tagName << " " << tagKey << " absent in " << module << " (type " << type << ")");
        }
    }
    /* clear return parameter if an error occurred, but not in case of invalid value */
    if (result.bad() && (result != SR_EC_InvalidValue))
        stringValue.clear();
    return result;
}


// --- misc helper functions ---

const OFString &DSRTypes::currentDate(OFString &dateString)
{
    DcmDate::getCurrentDate(dateString);
    return dateString;
}


const OFString &DSRTypes::currentTime(OFString &timeString)
{
    DcmTime::getCurrentTime(timeString, OFTrue /*seconds*/, OFFalse /*fraction*/);
    return timeString;
}


const OFString &DSRTypes::currentDateTime(OFString &dateTimeString)
{
    DcmDateTime::getCurrentDateTime(dateTimeString, OFTrue /*seconds*/, OFFalse /*fraction*/, OFFalse /*timeZone*/);
    return dateTimeString;
}


const OFString &DSRTypes::localTimezone(OFString &timezoneString)
{
    OFString dateTimeString;
    DcmDateTime::getCurrentDateTime(dateTimeString, OFFalse /*seconds*/, OFFalse /*fraction*/, OFTrue /*timeZone*/);
    timezoneString.assign(dateTimeString.substr(8 /* YYYYMMDD */ + 4 /* HHMM */, 5 /* &ZZZZ */));
    return timezoneString;
}


const OFString &DSRTypes::dicomToReadableDate(const OFString &dicomDate,
                                              OFString &readableDate)
{
    DcmDate::getISOFormattedDateFromString(dicomDate, readableDate);
    return readableDate;
}


const OFString &DSRTypes::dicomToReadableTime(const OFString &dicomTime,
                                              OFString &readableTime)
{
    DcmTime::getISOFormattedTimeFromString(dicomTime, readableTime, OFTrue /*seconds*/, OFFalse /*fraction*/, OFFalse /*createMissingPart*/);
    return readableTime;
}


const OFString &DSRTypes::dicomToReadableDateTime(const OFString &dicomDateTime,
                                                  OFString &readableDateTime)
{
    DcmDateTime::getISOFormattedDateTimeFromString(dicomDateTime, readableDateTime, OFTrue /*seconds*/, OFFalse /*fraction*/, OFTrue /*timeZone*/, OFFalse /*createMissingPart*/);
    return readableDateTime;
}


const OFString &DSRTypes::dicomToReadablePersonName(const OFString &dicomPersonName,
                                                    OFString &readablePersonName)
{
    if (DcmPersonName::getFormattedNameFromString(dicomPersonName, readablePersonName, 0 /*componentGroup*/).bad())
        readablePersonName = dicomPersonName;
    return readablePersonName;
}


const OFString &DSRTypes::dicomToXMLPersonName(const OFString &dicomPersonName,
                                               OFString &xmlPersonName,
                                               const OFBool writeEmptyValue)
{
    OFString str1, str2, str3, str4, str5;
    if (DcmPersonName::getNameComponentsFromString(dicomPersonName, str1, str2, str3, str4, str5, 0 /*componentGroup*/).good())
    {
        OFBool newLine = OFFalse;
        OFString xmlString;
        xmlPersonName.clear();
        /* prefix */
        if (writeEmptyValue || !str4.empty())
        {
            xmlPersonName += "<prefix>";
            xmlPersonName += convertToXMLString(str4, xmlString);
            xmlPersonName += "</prefix>";
            newLine = OFTrue;
        }
        /* first name */
        if (writeEmptyValue || !str2.empty())
        {
            if (newLine)
            {
                xmlPersonName += '\n';
                newLine = OFFalse;
            }
            xmlPersonName += "<first>";
            xmlPersonName += convertToXMLString(str2, xmlString);
            xmlPersonName += "</first>";
            newLine = OFTrue;
        }
        /* middle name */
        if (writeEmptyValue || !str3.empty())
        {
            if (newLine)
            {
                xmlPersonName += '\n';
                newLine = OFFalse;
            }
            xmlPersonName += "<middle>";
            xmlPersonName += convertToXMLString(str3, xmlString);
            xmlPersonName += "</middle>";
            newLine = OFTrue;
        }
        /* last name */
        if (writeEmptyValue || !str1.empty())
        {
            if (newLine)
            {
                xmlPersonName += '\n';
                newLine = OFFalse;
            }
            xmlPersonName += "<last>";
            xmlPersonName += convertToXMLString(str1, xmlString);
            xmlPersonName += "</last>";
            newLine = OFTrue;
        }
        /* suffix */
        if (writeEmptyValue || !str5.empty())
        {
            if (newLine)
            {
                xmlPersonName += '\n';
                newLine = OFFalse;
            }
            xmlPersonName += "<suffix>";
            xmlPersonName += convertToXMLString(str5, xmlString);
            xmlPersonName += "</suffix>";
            newLine = OFTrue;
        }
    } else
        xmlPersonName = dicomPersonName;
    return xmlPersonName;
}


const char *DSRTypes::numberToString(const size_t number,
                                     char *stringValue)
{
    if (stringValue != NULL)
    {
        /* unsigned long */
        sprintf(stringValue, "%lu", OFstatic_cast(unsigned long, number));
    }
    return stringValue;
}


size_t DSRTypes::stringToNumber(const char *stringValue)
{
    size_t result = 0;
    if (stringValue != NULL)
    {
        unsigned long lu_value = 0;
        /* unsigned long */
        if (sscanf(stringValue, "%lu", &lu_value) == 1)
            result = OFstatic_cast(size_t, lu_value);
    }
    return result;
}


const OFString &DSRTypes::convertToPrintString(const OFString &sourceString,
                                               OFString &printString)
{
    /* char ptr allows fastest access to the string */
    const char *str = sourceString.c_str();
    const size_t count = strlen(str);
    /* start with empty string */
    printString.clear();
    /* avoid to resize the string too often */
    printString.reserve(count);
    for (size_t i = 0; i < count; i++)
    {
        /* newline: depends on OS */
        if (*str == '\n')
            printString += "\\n";
        /* line feed: LF */
        else if (*str == '\012')
            printString += "\\012";
        /* return: CR */
        else if (*str == '\r')
            printString += "\\r";
        /* other character: just append */
        else
            printString += *str;
        str++;
    }
    return printString;
}


const OFString &DSRTypes::convertToHTMLString(const OFString &sourceString,
                                              OFString &markupString,
                                              const size_t flags,
                                              const OFBool newlineAllowed)
{
    const OFBool convertNonASCII = (flags & HF_convertNonASCIICharacters) > 0;
    const OFStandard::E_MarkupMode markupMode = (flags & HF_XHTML11Compatibility) ? OFStandard::MM_XHTML : (flags & HF_HTML32Compatibility) ? OFStandard::MM_HTML32 : OFStandard::MM_HTML;
    /* call the real function */
    return OFStandard::convertToMarkupString(sourceString, markupString, convertNonASCII, markupMode, newlineAllowed);
}


const OFString &DSRTypes::convertToXMLString(const OFString &sourceString,
                                             OFString &markupString)
{
    /* call the real function */
    return OFStandard::convertToMarkupString(sourceString, markupString, OFFalse /*convertNonASCII*/, OFStandard::MM_XML, OFFalse /*newlineAllowed*/);
}


OFBool DSRTypes::checkForValidReference(const OFString &stringValue)
{
    OFBool result = OFFalse;
    /* empty strings are invalid */
    if (!stringValue.empty())
    {
        const unsigned char *p = OFreinterpret_cast(const unsigned char *, stringValue.c_str());
        if (p != NULL)
        {
            /* check for leading number */
            while (isdigit(*p))
            {
                /* disallow leading 0 */
                if (!result && (*p == '0'))
                    break;
                result = OFTrue;
                p++;
            }
            /* check for separator */
            while ((*p == '.') && result)
            {
                /* trailing '.' is invalid */
                result = OFFalse;
                p++;
                /* check for trailing number */
                while (isdigit(*p))
                {
                    /* disallow leading 0 */
                    if (!result && (*p == '0'))
                        break;
                    result = OFTrue;
                    p++;
                }
            }
            /* all characters checked? */
            if (*p != 0)
                result = OFFalse;
        }
    }
    return result;
}


DSRIODConstraintChecker *DSRTypes::createIODConstraintChecker(const E_DocumentType documentType)
{
    DSRIODConstraintChecker *checker = NULL;
    switch (documentType)
    {
        case DT_BasicTextSR:
            checker = new DSRBasicTextSRConstraintChecker();
            break;
        case DT_EnhancedSR:
            checker = new DSREnhancedSRConstraintChecker();
            break;
        case DT_ComprehensiveSR:
            checker = new DSRComprehensiveSRConstraintChecker();
            break;
        case DT_KeyObjectSelectionDocument:
            checker = new DSRKeyObjectSelectionDocumentConstraintChecker();
            break;
        case DT_MammographyCadSR:
            checker = new DSRMammographyCadSRConstraintChecker();
            break;
        case DT_ChestCadSR:
            checker = new DSRChestCadSRConstraintChecker();
            break;
        case DT_ColonCadSR:
            checker = new DSRColonCadSRConstraintChecker();
            break;
        case DT_ProcedureLog:
            checker = new DSRProcedureLogConstraintChecker();
            break;
        case DT_XRayRadiationDoseSR:
            checker = new DSRXRayRadiationDoseSRConstraintChecker();
            break;
        case DT_SpectaclePrescriptionReport:
            checker = new DSRSpectaclePrescriptionReportConstraintChecker();
            break;
        case DT_MacularGridThicknessAndVolumeReport:
            checker = new DSRMacularGridThicknessAndVolumeReportConstraintChecker();
            break;
        case DT_ImplantationPlanSRDocument:
            checker = new DSRImplantationPlanSRDocumentConstraintChecker();
            break;
        case DT_Comprehensive3DSR:
            checker = new DSRComprehensive3DSRConstraintChecker();
            break;
        case DT_RadiopharmaceuticalRadiationDoseSR:
            checker = new DSRRadiopharmaceuticalRadiationDoseSRConstraintChecker();
            break;
        case DT_ExtensibleSR:
            /* not yet supported */
            break;
        case DT_AcquisitionContextSR:
            checker = new DSRAcquisitionContextSRConstraintChecker();
            break;
        case DT_SimplifiedAdultEchoSR:
            checker = new DSRSimplifiedAdultEchoSRConstraintChecker();
            break;
        case DT_PatientRadiationDoseSR:
            checker = new DSRPatientRadiationDoseSRConstraintChecker();
            break;
        case DT_PerformedImagingAgentAdministrationSR:
            checker = new DSRPerformedImagingAgentAdministrationSRConstraintChecker();
            break;
        case DT_PlannedImagingAgentAdministrationSR:
            checker = new DSRPlannedImagingAgentAdministrationSRConstraintChecker();
            break;
        case DT_invalid:
            /* nothing to do */
            break;
    }
    return checker;
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
        case VT_SCoord3D:
            node = new DSRSCoord3DTreeNode(relationshipType);
            break;
        case VT_TCoord:
            node = new DSRTCoordTreeNode(relationshipType);
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
        case VT_byReference:
            node = new DSRByReferenceTreeNode(relationshipType);
            break;
        default:
            break;
    }
    return node;
}


void DSRTypes::printInvalidContentItemMessage(const char *action,
                                              const DSRDocumentTreeNode *node,
                                              const char *location)
{
    OFString message;
    if (action != NULL)
        message += action;
    else
        message += "Processing";
    message += " invalid/incomplete content item";
    if (node != NULL)
    {
        message += " ";
        message += valueTypeToDefinedTerm(node->getValueType());
    }
    if (location != NULL)
    {
        message += " \"";
        message += location;
        message += "\"";
    }
    DCMSR_WARN(message);
}


void DSRTypes::printContentItemErrorMessage(const char *action,
                                            const OFCondition &result,
                                            const DSRDocumentTreeNode *node,
                                            const char *location)
{
    if (result.bad())
    {
        OFString message;
        if (action != NULL)
            message += action;
        else
            message += "Processing";
        message += " content item";
        if (node != NULL)
        {
            message += " ";
            message += valueTypeToDefinedTerm(node->getValueType());
        }
        if (location != NULL)
        {
            message += " \"";
            message += location;
            message += "\"";
        }
        message += " (";
        message += result.text();
        message += ")";
        DCMSR_ERROR(message);
    }
}


void DSRTypes::printUnknownValueWarningMessage(const char *valueName,
                                               const char *readValue,
                                               const char *action)
{
    if (valueName != NULL)
    {
        OFString message;
        if (action != NULL)
            message += action;
        else
            message += "Processing";
        message += " unknown/unsupported ";
        message += valueName;
        if ((readValue != NULL) && (strlen(readValue) > 0))
        {
            message += " (";
            message += readValue;
            message += ")";
        }
        DCMSR_WARN(message);
    }
}


OFBool DSRTypes::writeStringValueToXML(STD_NAMESPACE ostream &stream,
                                       const OFString &stringValue,
                                       const OFString &tagName,
                                       const OFBool writeEmptyValue)
{
    OFBool result = OFFalse;
    if (writeEmptyValue || !stringValue.empty())
    {
        stream << "<" << tagName << ">";
        OFStandard::convertToMarkupStream(stream, stringValue, OFFalse /*convertNonASCII*/, OFStandard::MM_XML, OFFalse /*newlineAllowed*/);
        stream << "</" << tagName << ">" << OFendl;
        result = OFTrue;
    }
    return result;
}


OFBool DSRTypes::writeStringFromElementToXML(STD_NAMESPACE ostream &stream,
                                             DcmElement &delem,
                                             const OFString &tagName,
                                             const OFBool writeEmptyValue)
{
    OFBool result = OFFalse;
    if (writeEmptyValue || !delem.isEmpty())
    {
        OFString tempString;
        stream << "<" << tagName << ">";
        /* special formatting for person names */
        if (delem.getVR() == EVR_PN)
        {
            OFString xmlString;
            /* use first element value only */
            stream << OFendl << dicomToXMLPersonName(getStringValueFromElement(delem, tempString), xmlString, writeEmptyValue) << OFendl;
        } else {
            /* use all element values (1-n) */
            getStringValueFromElement(delem, tempString, -1 /* all components */);
            OFStandard::convertToMarkupStream(stream, tempString, OFFalse /*convertNonASCII*/);
        }
        stream << "</" << tagName << ">" << OFendl;
        result = OFTrue;
    }
    return result;
}


size_t DSRTypes::createHTMLAnnexEntry(STD_NAMESPACE ostream &docStream,
                                      STD_NAMESPACE ostream &annexStream,
                                      const OFString &referenceText,
                                      size_t &annexNumber,
                                      const size_t flags)
{
    /* hyperlink to corresponding annex */
    const char *attrName = (flags & DSRTypes::HF_XHTML11Compatibility) ? "id" : "name";
    docStream << "[";
    if (!referenceText.empty())
        docStream << referenceText << " ";
    docStream << "<a " << attrName << "=\"annex_src_" << annexNumber << "\" href=\"#annex_dst_" << annexNumber << "\">Annex " << annexNumber << "</a>]" << OFendl;
    /* create new annex */
    annexStream << "<h2><a " << attrName << "=\"annex_dst_" << annexNumber << "\" href=\"#annex_src_" << annexNumber << "\">Annex " << annexNumber << "</a></h2>" << OFendl;
    /* increase annex number, return previous number */
    return annexNumber++;
}


size_t DSRTypes::createHTMLFootnote(STD_NAMESPACE ostream &docStream,
                                    STD_NAMESPACE ostream &footnoteStream,
                                    size_t &footnoteNumber,
                                    const size_t nodeID,
                                    const size_t flags)
{
    /* hyperlink to corresponding footnote */
    const char *attrName = (flags & DSRTypes::HF_XHTML11Compatibility) ? "id" : "name";
    if (flags & HF_XHTML11Compatibility)
        docStream << "<span class=\"super\">";
    else
        docStream << "<small><sup>";
    docStream << "<a " << attrName << "=\"footnote_src_" << nodeID << "_" << footnoteNumber << "\" ";
    docStream << "href=\"#footnote_dst_" << nodeID << "_" << footnoteNumber << "\">" << footnoteNumber << "</a>";
    if (flags & HF_XHTML11Compatibility)
        docStream << "</span>" << OFendl;
    else
        docStream << "</sup></small>" << OFendl;
    /* create new footnote */
    footnoteStream << "<b><a " << attrName << "=\"footnote_dst_" << nodeID << "_" << footnoteNumber << "\" ";
    footnoteStream << "href=\"#footnote_src_" << nodeID << "_" << footnoteNumber << "\">Footnote " << footnoteNumber << "</a></b>" << OFendl;
    /* increase footnote number, return previous number */
    return footnoteNumber++;
}


OFCondition DSRTypes::appendStream(STD_NAMESPACE ostream &mainStream,
                                   OFOStringStream &tempStream,
                                   const char *heading)
{
    OFCondition result = EC_InvalidStream;
    /* add final 0 byte (if required) */
    tempStream << OFStringStream_ends;
    /* freeze/get string (now we have full control over the array) */
    OFSTRINGSTREAM_GETSTR(tempStream, tempString)
    /* should never be NULL */
    if (tempString != NULL)
    {
        if (strlen(tempString) > 0)
        {
            /* append optional heading */
            if (heading != NULL)
                mainStream << heading << OFendl;
            /* append temporal document to main document */
            mainStream << tempString;
        }
        /* very important! since we have full control we are responsible for deleting the array */
        OFSTRINGSTREAM_FREESTR(tempString)
        result = EC_Normal;
    }
    return result;
}
