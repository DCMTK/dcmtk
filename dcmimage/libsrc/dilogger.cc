/*
 *
 *  Copyright (C) 2009-2010, OFFIS e.V.
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
 *  Module:  dcmimage
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Logger (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:14 $
 *  CVS/RCS Revision: $Revision: 1.2 $
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
 * Revision 1.2  2010-10-14 13:14:14  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.1  2009-11-25 16:48:46  joergr
 * Added new source and header file for the module logger.
 *
 *
 */
