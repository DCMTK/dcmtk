/*
 *
 *  Copyright (C) 2009, OFFIS
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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Logger (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2009-11-25 16:48:46 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimage/dilogger.h"


OFLogger DCM_dcmimageGetLogger()
{
    static OFLogger DCM_dcmimageLogger = OFLog::getLogger("dcmtk.dcmimage");
    return DCM_dcmimageLogger;
}


/*
 *
 * CVS/RCS Log:
 * $Log: dilogger.cc,v $
 * Revision 1.1  2009-11-25 16:48:46  joergr
 * Added new source and header file for the module logger.
 *
 *
 */
