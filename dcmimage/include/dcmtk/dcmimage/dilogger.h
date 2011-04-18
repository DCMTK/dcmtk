/*
 *
 *  Copyright (C) 2009-2011, OFFIS e.V.
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
 *  Purpose: Logger (Header)
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2011-04-18 07:00:58 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DILOGGER_H
#define DILOGGER_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/oflog/oflog.h"

extern OFLogger DCM_dcmimageLogger;

#define DCMIMAGE_TRACE(msg) OFLOG_TRACE(DCM_dcmimageLogger, msg)
#define DCMIMAGE_DEBUG(msg) OFLOG_DEBUG(DCM_dcmimageLogger, msg)
#define DCMIMAGE_INFO(msg)  OFLOG_INFO(DCM_dcmimageLogger, msg)
#define DCMIMAGE_WARN(msg)  OFLOG_WARN(DCM_dcmimageLogger, msg)
#define DCMIMAGE_ERROR(msg) OFLOG_ERROR(DCM_dcmimageLogger, msg)
#define DCMIMAGE_FATAL(msg) OFLOG_FATAL(DCM_dcmimageLogger, msg)

#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dilogger.h,v $
 * Revision 1.3  2011-04-18 07:00:58  uli
 * Use global variables for the logger objects. This removes the thread-unsafe
 * static local variables which were used before.
 *
 * Revision 1.2  2010-10-14 13:16:29  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.1  2009-11-25 14:32:47  joergr
 * Added new source and header file for the module logger.
 *
 *
 */
