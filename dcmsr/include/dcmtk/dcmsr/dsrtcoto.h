/*
 *
 *  Copyright (C) 2000-2010, OFFIS e.V.
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRReferencedTimeOffsetList
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:33 $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRTCOTO_H
#define DSRTCOTO_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrtlist.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for referenced time offset list
 */
class DSRReferencedTimeOffsetList
  : public DSRListOfItems<Float64>
{

  public:

    /** default constructor
     */
    DSRReferencedTimeOffsetList();

    /** copy constructor
     ** @param  lst  list to be copied
     */
    DSRReferencedTimeOffsetList(const DSRReferencedTimeOffsetList &lst);

    /** destructor
     */
    virtual ~DSRReferencedTimeOffsetList();

    /** assignment operator
     ** @param  lst  list to be copied
     ** @return reference to this list after 'lst' has been copied
     */
    DSRReferencedTimeOffsetList &operator=(const DSRReferencedTimeOffsetList &lst);

    /** print list of referenced time offsets.
     *  The output of a typical list looks like this: 1,2.5 or 1,... if shortened.
     ** @param  stream     output stream to which the list should be printed
     *  @param  flags      flag used to customize the output (see DSRTypes::PF_xxx)
     *  @param  separator  character specifying the separator between the list items
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition print(STD_NAMESPACE ostream &stream,
                      const size_t flags = 0,
                      const char separator = ',') const;

    /** read list of referenced time offsets
     ** @param  dataset    DICOM dataset from which the list should be read
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition read(DcmItem &dataset);

    /** write list of referenced time offsets
     ** @param  dataset    DICOM dataset to which the list should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition write(DcmItem &dataset) const;

    /** put list of referenced time offsets as a string.
     *  This function expects the same input format as created by print(), i.e. a comma
     *  separated list of numerical values.
     ** @param  stringValue  string value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition putString(const char *stringValue);
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrtcoto.h,v $
 *  Revision 1.12  2010-10-14 13:16:33  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.11  2009-10-13 14:57:50  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.10  2007-11-15 16:33:19  joergr
 *  Fixed coding style to be more consistent.
 *
 *  Revision 1.9  2006/08/15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.8  2005/12/08 16:05:23  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.7  2003/08/07 12:52:51  joergr
 *  Added new putString() method.
 *
 *  Revision 1.6  2003/06/03 10:16:44  meichel
 *  Renamed local variables to avoid name clashes with STL
 *
 *  Revision 1.5  2001/09/26 13:04:12  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.4  2001/06/01 15:51:04  meichel
 *  Updated copyright header
 *
 *  Revision 1.3  2000/11/06 11:20:10  joergr
 *  Added parameter to print() method specifying the item separator character.
 *
 *  Revision 1.2  2000/11/01 16:20:24  joergr
 *  Updated comments/formatting.
 *
 *  Revision 1.1  2000/10/26 14:23:25  joergr
 *  Added support for TCOORD content item.
 *
 *
 *
 */
