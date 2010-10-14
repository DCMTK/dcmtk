/*
 *
 *  Copyright (C) 2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:11 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
        char buf[256];
        DCMDATA_WARN("Error while setting up lexer: "
                << OFStandard::strerror(errno, buf, sizeof(buf)));
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


/*
** CVS/RCS Log:
** $Log: vrscan.cc,v $
** Revision 1.8  2010-10-14 13:14:11  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.7  2010-09-02 12:02:06  uli
** Use longjmp() for error handling in the VR scanner.
**
** Revision 1.6  2010-09-02 10:16:02  uli
** The VR scanner now only copies the input data once, not twice.
**
** Revision 1.5  2010-09-02 09:49:38  uli
** Add the VR prefix into the scanner instead of adding it in the caller.
**
** Revision 1.4  2010-09-02 09:23:15  uli
** Made the VR scanner reentrant again.
**
** Revision 1.3  2010-08-26 12:29:48  uli
** Ported vrscan from ancient flex++ to current flex version.
**
**
*/
