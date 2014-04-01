/*
 *
 *  Copyright (C) 1994-2014, OFFIS e.V.
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
 *  Author:  Gerd Ehlers, Andrew Hewett
 *
 *  Purpose: global type and constant definitions
 *
 */


#ifndef DCTYPES_H
#define DCTYPES_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/dcmdata/dcdefine.h"

#define INCLUDE_CSTDLIB
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TYPES_H
/* needed e.g. on Solaris for definition of size_t */
#include <sys/types.h>
#endif
END_EXTERN_C

/*
** Logging
*/

extern DCMTK_DCMDATA_EXPORT OFLogger DCM_dcmdataLogger;

#define DCMDATA_TRACE(msg) OFLOG_TRACE(DCM_dcmdataLogger, msg)
#define DCMDATA_DEBUG(msg) OFLOG_DEBUG(DCM_dcmdataLogger, msg)
#define DCMDATA_INFO(msg)  OFLOG_INFO(DCM_dcmdataLogger, msg)
#define DCMDATA_WARN(msg)  OFLOG_WARN(DCM_dcmdataLogger, msg)
#define DCMDATA_ERROR(msg) OFLOG_ERROR(DCM_dcmdataLogger, msg)
#define DCMDATA_FATAL(msg) OFLOG_FATAL(DCM_dcmdataLogger, msg)

/*
** Macro Definitions
*/

/// XML namespace URI for the dcmtk
#define DCMTK_XML_NAMESPACE_URI "http://dicom.offis.de/dcmtk"
/// XML namespace URI for Native DICOM Model (see DICOM part 19)
#define NATIVE_DICOM_MODEL_XML_NAMESPACE_URI "http://dicom.nema.org/PS3.19/models/NativeDICOM"

// ANSI escape codes for color output of the print() method
#define DCMDATA_ANSI_ESCAPE_CODE_RESET      "\033[0m"
#define DCMDATA_ANSI_ESCAPE_CODE_TAG        "\033[22m\033[32m"
#define DCMDATA_ANSI_ESCAPE_CODE_VR         "\033[22m\033[31m"
#define DCMDATA_ANSI_ESCAPE_CODE_VALUE      "\033[1m\033[37m"
#define DCMDATA_ANSI_ESCAPE_CODE_INFO       "\033[1m\033[30m"
#define DCMDATA_ANSI_ESCAPE_CODE_LENGTH     "\033[22m\033[36m"
#define DCMDATA_ANSI_ESCAPE_CODE_VM         "\033[22m\033[35m"
#define DCMDATA_ANSI_ESCAPE_CODE_NAME       "\033[22m\033[33m"
#define DCMDATA_ANSI_ESCAPE_CODE_NAME_1     "\033[1m\033[33m"
#define DCMDATA_ANSI_ESCAPE_CODE_SEQUENCE   "\033[22m\033[32m"
#define DCMDATA_ANSI_ESCAPE_CODE_SEQUENCE_1 "\033[1m\033[32m"
#define DCMDATA_ANSI_ESCAPE_CODE_ITEM       "\033[1m\033[30m"
#define DCMDATA_ANSI_ESCAPE_CODE_LINE       "\033[1m\033[30m"
#define DCMDATA_ANSI_ESCAPE_CODE_COMMENT    "\033[1m\033[30m"


// include this file in doxygen documentation

/** @file dctypes.h
 *  @brief type definitions for the dcmdata module
 */


/*
** Enumerated Types
*/

/// encoding type for sequences and sequence items
typedef enum {
    /// defined length
    EET_ExplicitLength = 0,
    /// undefined length
    EET_UndefinedLength = 1
} E_EncodingType;


/// handling of group length elements when reading/writing a dataset
typedef enum {
    /// no change of group length values, WARNING: DO NOT USE THIS VALUE FOR WRITE OPERATIONS
    EGL_noChange = 0,
    /// remove group length tags
    EGL_withoutGL = 1,
    /// add group length tags for every group
    EGL_withGL = 2,
    /// recalculate values for existing group length tags
    EGL_recalcGL = 3
} E_GrpLenEncoding;

/// handling of dataset trailing padding
typedef enum {
    /// no change of padding tags
    EPD_noChange = 0,
    /// remove all padding tags
    EPD_withoutPadding = 1,
    /// add padding tags
    EPD_withPadding = 2
} E_PaddingEncoding;

/// search mode for hierarchical search operations
typedef enum {
    /// start search from current object
    ESM_fromHere = 0,
    /// start search from object pointed to by stack top
    ESM_fromStackTop = 1,
    /// start search from object following the object pointed to by stack top
    ESM_afterStackTop = 2
} E_SearchMode;

/// object state during transfer (read/write) operations
typedef enum {
    /// object prepared for transfer, no data transferred yet
    ERW_init = 0,
    /// object transfer completed
    ERW_ready = 1,
    /// object transfer in progress
    ERW_inWork = 2,
    /// object not prepared for transfer operation
    ERW_notInitialized = 3
} E_TransferState;

/// mode for file reading
typedef enum {
    /// auto detect: fileformat or dataset
    ERM_autoDetect = 0,
    /// read as dataset (assume no meta header present)
    ERM_dataset = 1,
    /// read file format only, refuse if no meta-header
    ERM_fileOnly = 2,
    /// read meta-header only, do not read the dataset
    ERM_metaOnly = 3
} E_FileReadMode;

/// mode for file writing
typedef enum {
    /// write as fileformat (update only missing information, this is the old behavior)
    EWM_fileformat = 0,
    /// write as dataset (without meta header)
    EWM_dataset = 1,
    /// write as fileformat and update required information (e.g. SOP Class/Instance UID)
    EWM_updateMeta = 2,
    /// write as fileformat and create new meta header (do not retain existing information)
    EWM_createNewMeta = 3,
    /// write as fileformat but don't update the meta header (please be careful!)
    EWM_dontUpdateMeta = 4
} E_FileWriteMode;


/** General purpose class hiding constants from the global namespace.
 */
struct DCMTK_DCMDATA_EXPORT DCMTypes
{
  public:

    /** @name print() flags.
     *  These flags can be combined and passed to the print() methods.
     */
    //@{

    /// shorten long tag values (e.g. long texts, pixel data)
    static const size_t PF_shortenLongTagValues;

    /// show hierarchical tree structure of the dataset
    static const size_t PF_showTreeStructure;

    /// do not map well-known UID numbers to UID names (e.g. Transfer Syntax and SOP Class)
    static const size_t PF_doNotMapUIDsToNames;

    /// quote non-ASCII string content as XML markup
    static const size_t PF_convertToMarkup;

    /// quote non-ASCII string content as octal numbers
    static const size_t PF_convertToOctalNumbers;

    /// use ANSI escape codes for output
    static const size_t PF_useANSIEscapeCodes;
    //@}

    /** @name writeXML() flags.
     *  These flags can be combined and passed to the writeXML() methods.
     */
    //@{

    /// add document type definition (DTD). DCMTK-specific format only.
    static const size_t XF_addDocumentType;

    /// write binary data to XML output file. DCMTK-specific format only.
    static const size_t XF_writeBinaryData;

    /// encode binary data as Base64 (MIME).
    /// - For the DCMTK-specific format, the default is to use hex numbers.
    /// - For the Native DICOM Model, the default is to use a UUID reference.
    static const size_t XF_encodeBase64;

    /// use XML namespace URI. The value depends on the selected output format.
    static const size_t XF_useXMLNamespace;

    /// embed content of document type definition. DCMTK-specific format only.
    static const size_t XF_embedDocumentType;

    /// don't write name of the DICOM data elements. DCMTK-specific format only.
    static const size_t XF_omitDataElementName;

    /// convert non-ASCII characters to numeric values. DCMTK-specific format only.
    static const size_t XF_convertNonASCII;

    /// write data in "Native DICOM Model" format as defined for Application Hosting (DICOM part 19).
    /// The default is to use the DCMTK-specific format.
    static const size_t XF_useNativeModel;

    //@}
};


// Undefined Length Identifier
const Uint32 DCM_UndefinedLength = 0xffffffff;


#endif /* !DCTYPES_H */
