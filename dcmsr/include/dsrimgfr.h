/*
 *
 *  Copyright (C) 2000, OFFIS
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRImageFrameList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-13 07:49:27 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRIMGFR_H
#define DSRIMGFR_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrtypes.h"
#include "dsrtlist.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for image frame list
 */
class DSRImageFrameList
  : public DSRListOfItems<Sint32>
{

  public:

    DSRImageFrameList();

    DSRImageFrameList(const DSRImageFrameList &list);

    DSRImageFrameList &operator=(const DSRImageFrameList &list);

    virtual ~DSRImageFrameList();

    E_Condition print(ostream &stream,
                      const size_t flags = 0) const;

    E_Condition read(DcmItem &dataset,
                     OFConsole *logStream);

    E_Condition write(DcmItem &dataset,
                      OFConsole *logStream) const;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrimgfr.h,v $
 *  Revision 1.1  2000-10-13 07:49:27  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
