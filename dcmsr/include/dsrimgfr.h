/*
 *
 *  Copyright (C) 2000-2001, OFFIS
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
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-09-26 13:04:08 $
 *  CVS/RCS Revision: $Revision: 1.6 $
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

    /** default constructor
     */
    DSRImageFrameList();

    /** copy constructor
     ** @param  list  list to be copied
     */
    DSRImageFrameList(const DSRImageFrameList &list);

    /** destructor
     */
    virtual ~DSRImageFrameList();

    /** assignment operator
     ** @param  list  list to be copied
     ** @return reference to this list after 'list' has been copied
     */
    DSRImageFrameList &operator=(const DSRImageFrameList &list);

    /** print list of referenced frame numbers.
     *  The output of a typical list looks like this: 1,2,3 or 1,... if shortened.
     ** @param  stream     output stream to which the list should be printed
     *  @param  flags      flag used to customize the output (see DSRTypes::PF_xxx)
     *  @param  separator  character specifying the separator between the list items
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition print(ostream &stream,
                      const size_t flags = 0,
                      const char separator = ',') const;

    /** read list of referenced frame numbers
     ** @param  dataset    DICOM dataset from which the list should be read
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition read(DcmItem &dataset,
                     OFConsole *logStream);

    /** write list of referenced frame numbers
     ** @param  dataset    DICOM dataset to which the list should be written
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition write(DcmItem &dataset,
                      OFConsole *logStream) const;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrimgfr.h,v $
 *  Revision 1.6  2001-09-26 13:04:08  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.5  2001/06/01 15:51:01  meichel
 *  Updated copyright header
 *
 *  Revision 1.4  2000/11/06 11:16:06  joergr
 *  Added parameter to print() method specifying the item separator character.
 *
 *  Revision 1.3  2000/11/01 16:19:20  joergr
 *  Updated comments/formatting.
 *
 *  Revision 1.2  2000/10/18 17:03:28  joergr
 *  Added doc++ comments.
 *
 *  Revision 1.1  2000/10/13 07:49:27  joergr
 *  Added new module 'dcmsr' providing access to DICOM structured reporting
 *  documents (supplement 23).  Doc++ documentation not yet completed.
 *
 *
 */
