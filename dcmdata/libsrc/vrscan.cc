/*
 *
 *  Copyright (C) 2010, OFFIS
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
 *  Author:  Uli Schlachter
 *
 *  Purpose: Interface to the VR scanner.
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-09-02 09:23:15 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmdata/vrscan.h"
#include "dcmtk/ofstd/ofstd.h"        /* For OFString::strerror() */
#include "dcmtk/dcmdata/dctypes.h"    /* For DCMDATA_WARN() */

BEGIN_EXTERN_C
#include "vrscanl.h"
END_EXTERN_C

int vrscan::scan(const OFString& value)
{
    yyscan_t scanner;

    if (yylex_init(&scanner))
    {
        char buf[256];
        DCMDATA_WARN("Error while setting up lexer: "
                << OFStandard::strerror(errno, buf, sizeof(buf)));
        return 16;
    }
    yy_scan_bytes(value.c_str(), value.length(), scanner);
    int result = yylex(scanner);
    if (yylex(scanner))
        result = 16 /* UNKNOWN */;

    yylex_destroy(scanner);

    return result;
}


/*
** CVS/RCS Log:
** $Log: vrscan.cc,v $
** Revision 1.4  2010-09-02 09:23:15  uli
** Made the VR scanner reentrant again.
**
** Revision 1.3  2010-08-26 12:29:48  uli
** Ported vrscan from ancient flex++ to current flex version.
**
**
*/
