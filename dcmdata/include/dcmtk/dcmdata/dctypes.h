/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-29 10:57:17 $
 *  CVS/RCS Revision: $Revision: 1.35 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCTYPES_H
#define DCTYPES_H 1

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/oflog/oflog.h"

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

OFLogger DCM_dcmdataGetLogger();

#define DCMDATA_TRACE(msg) OFLOG_TRACE(DCM_dcmdataGetLogger(), msg)
#define DCMDATA_DEBUG(msg) OFLOG_DEBUG(DCM_dcmdataGetLogger(), msg)
#define DCMDATA_INFO(msg)  OFLOG_INFO(DCM_dcmdataGetLogger(), msg)
#define DCMDATA_WARN(msg)  OFLOG_WARN(DCM_dcmdataGetLogger(), msg)
#define DCMDATA_ERROR(msg) OFLOG_ERROR(DCM_dcmdataGetLogger(), msg)
#define DCMDATA_FATAL(msg) OFLOG_FATAL(DCM_dcmdataGetLogger(), msg)

/*
** Macro Definitions
*/

/// XML namespace URI for the dcmtk
#define DCMTK_XML_NAMESPACE_URI "http://dicom.offis.de/dcmtk"

// ANSI escape codes for color output of the print() method
#define ANSI_ESCAPE_CODE_RESET      "\033[0m"
#define ANSI_ESCAPE_CODE_TAG        "\033[22m\033[32m"
#define ANSI_ESCAPE_CODE_VR         "\033[22m\033[31m"
#define ANSI_ESCAPE_CODE_VALUE      "\033[1m\033[37m"
#define ANSI_ESCAPE_CODE_INFO       "\033[1m\033[30m"
#define ANSI_ESCAPE_CODE_LENGTH     "\033[22m\033[36m"
#define ANSI_ESCAPE_CODE_VM         "\033[22m\033[35m"
#define ANSI_ESCAPE_CODE_NAME       "\033[22m\033[33m"
#define ANSI_ESCAPE_CODE_NAME_1     "\033[1m\033[33m"
#define ANSI_ESCAPE_CODE_SEQUENCE   "\033[22m\033[32m"
#define ANSI_ESCAPE_CODE_SEQUENCE_1 "\033[1m\033[32m"
#define ANSI_ESCAPE_CODE_ITEM       "\033[1m\033[30m"
#define ANSI_ESCAPE_CODE_LINE       "\033[1m\033[30m"
#define ANSI_ESCAPE_CODE_COMMENT    "\033[1m\033[30m"


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
struct DCMTypes
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

    /// use ANSI escape codes for output
    static const size_t PF_useANSIEscapeCodes;
    //@}

    /** @name writeXML() flags.
     *  These flags can be combined and passed to the writeXML() methods.
     */
    //@{

    /// add document type definition (DTD)
    static const size_t XF_addDocumentType;

    /// write binary data to XML output file
    static const size_t XF_writeBinaryData;

    /// encode binary data as Base64 (MIME)
    static const size_t XF_encodeBase64;

    /// XML namespace URI for dcmsr module
    static const size_t XF_useDcmtkNamespace;

    /// embed content of document type definition
    static const size_t XF_embedDocumentType;

    /// don't write name of the DICOM data elements
    static const size_t XF_omitDataElementName;
    //@}
};


// Undefined Length Identifier
const Uint32 DCM_UndefinedLength = 0xffffffff;


#endif /* !DCTYPES_H */


/*
 * CVS/RCS Log:
 * $Log: dctypes.h,v $
 * Revision 1.35  2010-10-29 10:57:17  joergr
 * Added support for colored output to the print() method.
 *
 * Revision 1.34  2010-10-14 13:15:42  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.33  2010-06-07 13:57:57  joergr
 * Added new file read mode that allows for loading the meta-header only.
 *
 * Revision 1.32  2010-03-01 09:08:44  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.31  2009-11-04 09:58:07  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.30  2009-08-21 10:45:06  joergr
 * Added new 'writeMode' which does not update the the meta header. This could
 * be useful for tools like dump2dcm and xml2dcm.
 *
 * Revision 1.29  2009-08-21 09:17:16  joergr
 * Added parameter 'writeMode' to save/write methods which allows for specifying
 * whether to write a dataset or fileformat as well as whether to update the
 * file meta information or to create a new file meta information header.
 *
 * Revision 1.28  2009-08-19 11:55:44  meichel
 * Added additional includes needed for Sun Studio 11 on Solaris.
 *
 * Revision 1.27  2009-01-06 16:28:11  joergr
 * Reworked print() output format for option PF_showTreeStructure.
 *
 * Revision 1.26  2008-04-28 09:22:42  meichel
 * New flag DCMTypes::PF_convertToMarkup causes DcmObject::print to
 *   quote non-ASCII string content as XML markup
 *
 * Revision 1.25  2008-02-26 16:56:15  joergr
 * Added new print flag that disables the mapping of well-known UID numbers to
 * their associated names (e.g. transfer syntax or SOP class).
 *
 * Revision 1.24  2007/11/29 14:30:35  meichel
 * Updated doxygen API documentation
 *
 * Revision 1.23  2006/05/11 08:55:22  joergr
 * Added new option that allows to omit the element name in the XML output.
 *
 * Revision 1.22  2006/02/08 15:21:15  joergr
 * Removed unused type definitions.
 *
 * Revision 1.21  2005/12/08 16:28:47  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.20  2005/12/02 08:50:30  joergr
 * Added new file read mode that makes it possible to distinguish between DICOM
 * files, datasets and other non-DICOM files.
 *
 * Revision 1.19  2003/04/22 08:19:09  joergr
 * Added new command line option which allows to embed the content of the DTD
 * instead of referencing the DTD file.
 *
 * Revision 1.18  2003/04/01 14:57:08  joergr
 * Added support for XML namespaces.
 *
 * Revision 1.17  2002/12/06 12:21:00  joergr
 * Enhanced "print()" function by re-working the implementation and replacing
 * the boolean "showFullData" parameter by a more general integer flag.
 *
 * Revision 1.16  2002/11/27 12:07:23  meichel
 * Adapted module dcmdata to use of new header file ofstdinc.h
 *
 * Revision 1.15  2002/07/10 11:45:40  meichel
 * Moved definitions for Uint8, Sint8 ... Float64 from dcmdata to ofstd
 *   since these types are not DICOM specific
 *
 * Revision 1.14  2002/06/06 14:51:13  meichel
 * Corrected code for inclusion of stdlib.h
 *
 * Revision 1.13  2002/05/14 08:20:29  joergr
 * Added support for Base64 (MIME) encoded binary data.
 *
 * Revision 1.12  2002/04/25 10:07:13  joergr
 * Added support for XML output of DICOM objects.
 *
 * Revision 1.11  2001/06/01 15:48:45  meichel
 * Updated copyright header
 *
 * Revision 1.10  2000/03/08 16:26:19  meichel
 * Updated copyright header.
 *
 * Revision 1.9  2000/03/03 14:05:26  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.8  2000/02/07 14:44:45  meichel
 * The typedef for Sint8 now defaults to char instead of signed char.
 *   This avoids warnings on certain c-front related compilers.
 *   The old behaviour can be restored by compiling with the symbol
 *   CHAR_IS_UNSIGNED defined.
 *
 * Revision 1.7  1999/03/31 09:24:51  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
