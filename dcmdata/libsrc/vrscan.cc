/*
 *
 *  Copyright (C) 2010-2021, OFFIS e.V.
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
 *  Author:  Uli Schlachter
 *
 *  Purpose: Interface to the VR scanner.
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/vrscan.h"
#include "dcmtk/ofstd/ofbmanip.h"
#include "dcmtk/ofstd/ofstd.h"        /* For OFString::strerror() */
#include "dcmtk/dcmdata/dctypes.h"    /* For DCMDATA_WARN() */
#include "dcmtk/ofstd/ofdiag.h"

BEGIN_EXTERN_C
#include "vrscani.h"
#include "vrscanl.h"
END_EXTERN_C

#include DCMTK_DIAGNOSTIC_PUSH
#include DCMTK_DIAGNOSTIC_IGNORE_VISUAL_STUDIO_OBJECT_DESTRUCTION_WARNING

int vrscan::scan(const OFString& vr, const char* const value, const size_t size)
{
    yyscan_t scanner;
    if (yylex_init(&scanner))
    {
        DCMDATA_WARN("Error while setting up lexer: "
                << OFStandard::getLastSystemErrorCode().message());
        return 16 /* UNKNOWN */;
    }

    struct cleanup_t
    {
        cleanup_t(yyscan_t& y) : t(y) {}
        ~cleanup_t() { yylex_destroy(t); }
        yyscan_t& t;
    }
    cleanup(scanner);

    OFString buffer;
    buffer.reserve(vr.size() + size + 2);
    buffer.append(vr);
    buffer.append(value, size);
    buffer.append("\0\0", 2); // yy_scan_buffer() requires this

    struct vrscan_error error;
    error.error_msg = "(Unknown error)";
    yyset_extra(&error, scanner);

    if (setjmp(error.setjmp_buffer)) // poor man's catch()
    {
        DCMDATA_WARN("Fatal error in lexer: " << error.error_msg);
        return 16 /* UNKNOWN */;
    }

    yy_scan_buffer(OFconst_cast(char*, buffer.data()), buffer.size(), scanner);
    const int result = yylex(scanner);
    if (yylex(scanner))
        return 16 /* UNKNOWN */;

    return result;
}

#include DCMTK_DIAGNOSTIC_POP

int vrscan::scan(const OFString& vr, const OFString& value)
{
    return scan(vr, value.data(), value.size());
}
