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
 *  Purpose: Logger (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2009-11-25 14:32:47 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DILOGGER_H
#define DILOGGER_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/oflog/oflog.h"

OFLogger DCM_dcmimageGetLogger();

#define DCMIMAGE_TRACE(msg) OFLOG_TRACE(DCM_dcmimageGetLogger(), msg)
#define DCMIMAGE_DEBUG(msg) OFLOG_DEBUG(DCM_dcmimageGetLogger(), msg)
#define DCMIMAGE_INFO(msg)  OFLOG_INFO(DCM_dcmimageGetLogger(), msg)
#define DCMIMAGE_WARN(msg)  OFLOG_WARN(DCM_dcmimageGetLogger(), msg)
#define DCMIMAGE_ERROR(msg) OFLOG_ERROR(DCM_dcmimageGetLogger(), msg)
#define DCMIMAGE_FATAL(msg) OFLOG_FATAL(DCM_dcmimageGetLogger(), msg)

#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dilogger.h,v $
 * Revision 1.1  2009-11-25 14:32:47  joergr
 * Added new source and header file for the module logger.
 *
 *
 *
 */
