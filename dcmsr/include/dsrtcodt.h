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
 *    classes: DSRReferencedDatetimeList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-10-26 14:23:24 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRTCODT_H
#define DSRTCODT_H

#include "osconfig.h"   /* make sure OS specific configuration is included first */

#include "dsrtypes.h"
#include "dsrtlist.h"

#include "ofstring.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for referenced datetime list
 */
class DSRReferencedDatetimeList
  : public DSRListOfItems<OFString>
{

  public:

    /** default constructor
     */
    DSRReferencedDatetimeList();

    /** copy constructor
     ** @param  list  list to be copied
     */
    DSRReferencedDatetimeList(const DSRReferencedDatetimeList &list);

    /** destructor
     */
    virtual ~DSRReferencedDatetimeList();

    /** assignment operator
     ** @param  list  list to be copied
     ** @return reference to this list after 'list' has been copied
     */
    DSRReferencedDatetimeList &operator=(const DSRReferencedDatetimeList &list);

    /** print list of referenced datetime.
     *  The output of a typical list looks like this: 20001010120000, ...
     ** @param  stream  output stream to which the list should be printed
     *  @param  flags   flag used to customize the output (see DSRTypes::PF_xxx)
     */
    E_Condition print(ostream &stream,
                      const size_t flags = 0) const;

    /** read list of referenced datetime
     ** @param  dataset    DICOM dataset from which the list should be read
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition read(DcmItem &dataset,
                     OFConsole *logStream);

    /** write list of referenced datetime
     ** @param  dataset    DICOM dataset to which the list should be written
     *  @param  logStream  pointer to error/warning output stream (output disabled if NULL)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    E_Condition write(DcmItem &dataset,
                      OFConsole *logStream) const;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrtcodt.h,v $
 *  Revision 1.1  2000-10-26 14:23:24  joergr
 *  Added support for TCOORD content item.
 *
 *
 *
 */
