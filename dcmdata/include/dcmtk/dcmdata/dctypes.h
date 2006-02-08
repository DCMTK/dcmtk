/*
 *
 *  Copyright (C) 1994-2006, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers, Andrew Hewett
 *
 *  Purpose: global type and constant definitions
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2006-02-08 15:21:15 $
 *  CVS/RCS Revision: $Revision: 1.22 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCTYPES_H
#define DCTYPES_H 1

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftypes.h"

#define INCLUDE_CSTDLIB
#include "dcmtk/ofstd/ofstdinc.h"


/*
** Macro Definitions
*/

// XML namespace URI for the dcmtk
#define DCMTK_XML_NAMESPACE_URI "http://dicom.offis.de/dcmtk"


/*
** Enumerated Types
*/

typedef enum {
    EET_ExplicitLength = 0,
    EET_UndefinedLength = 1
} E_EncodingType;


typedef enum {
    EGL_noChange = 0,       // no change of GL values, WARNING: DO NOT USE FOR WRITE
    EGL_withoutGL = 1,      // remove group length tags
    EGL_withGL = 2,         // add group length tags for every group
    EGL_recalcGL = 3        // recalculate values for existing group length tags
} E_GrpLenEncoding;

typedef enum {
    EPD_noChange = 0,       // no change of padding tags
    EPD_withoutPadding = 1, // remove all padding tags
    EPD_withPadding = 2     // add padding tags
} E_PaddingEncoding;


typedef enum {
    ESM_fromHere = 0,
    ESM_fromStackTop = 1,
    ESM_afterStackTop = 2
} E_SearchMode;


typedef enum {
    ERW_init = 0,
    ERW_ready = 1,
    ERW_inWork = 2,
    ERW_notInitialized = 3
} E_TransferState;


typedef enum {
    ERM_autoDetect = 0,     // auto detect: fileformat or dataset
    ERM_dataset = 1,        // dataset (ignore meta header)
    ERM_fileOnly = 2        // fileformat only
} E_FileReadMode;


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

    /// internal: current entry is the last one on the level
    static const size_t PF_lastEntry;

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
    //@}
};


// Undefined Length Identifier
const Uint32 DCM_UndefinedLength = 0xffffffff;


#endif /* !DCTYPES_H */


/*
 * CVS/RCS Log:
 * $Log: dctypes.h,v $
 * Revision 1.22  2006-02-08 15:21:15  joergr
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
