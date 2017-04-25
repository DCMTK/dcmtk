/*
 *
 *  Copyright (C) 2010-2017, OFFIS e.V.
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

BEGIN_EXTERN_C
#include "vrscani.h"
#include "vrscanl.h"
END_EXTERN_C

char* vrscan::makeBuffer(const OFString& vr, const OFString& value, size_t& size)
{
    char *buffer, *pos;

    // Allocate the needed buffer
    size = vr.length() + value.length() + 2;
    pos = buffer = new char[size];

    // Fill it with the input
    OFBitmanipTemplate<char>::copyMem(vr.data(), pos, vr.size());
    pos += vr.size();

    OFBitmanipTemplate<char>::copyMem(value.data(), pos, value.size());
    pos += value.size();

    // yy_scan_buffer() requires this
    pos[0] = pos[1] = '\0';

    return buffer;
}

int vrscan::scan(const OFString& vr, const OFString& value)
{
    struct vrscan_error error;
    yyscan_t scanner;
    int result;

    if (yylex_init(&scanner))
    {
        DCMDATA_WARN("Error while setting up lexer: "
                << OFStandard::getLastSystemErrorCode().message());
        return 16;
    }

    size_t bufSize;
    char *buf = makeBuffer(vr, value, bufSize);
    error.error_msg = "(Unknown error)";
    yyset_extra(&error, scanner);

    if (setjmp(error.setjmp_buffer))
    {
        DCMDATA_WARN("Fatal error in lexer: " << error.error_msg);
        result = 16 /* UNKNOWN */;
    } else {
        yy_scan_buffer(buf, bufSize, scanner);

        result = yylex(scanner);
        if (yylex(scanner))
            result = 16 /* UNKNOWN */;
    }

    yylex_destroy(scanner);
    delete[] buf;

    return result;
}
