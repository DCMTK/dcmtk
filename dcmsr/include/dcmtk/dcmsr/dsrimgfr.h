/*
 *
 *  Copyright (C) 2000-2015, OFFIS e.V.
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
 *    classes: DSRImageFrameList
 *
 */


#ifndef DSRIMGFR_H
#define DSRIMGFR_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrtypes.h"
#include "dcmtk/dcmsr/dsrtlist.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for image frame number list
 */
class DCMTK_DCMSR_EXPORT DSRImageFrameList
  : public DSRListOfItems<Sint32>
{

  public:

    /** default constructor
     */
    DSRImageFrameList();

    /** copy constructor
     ** @param  lst  list to be copied
     */
    DSRImageFrameList(const DSRImageFrameList &lst);

    /** destructor
     */
    virtual ~DSRImageFrameList();

    /** assignment operator
     ** @param  lst  list to be copied
     ** @return reference to this list after 'lst' has been copied
     */
    DSRImageFrameList &operator=(const DSRImageFrameList &lst);

    /** print list of referenced frame numbers.
     *  The output of a typical list looks like this: 1,2,3 or 1,... if shortened.
     ** @param  stream     output stream to which the list should be printed
     *  @param  flags      flag used to customize the output (see DSRTypes::PF_xxx)
     *  @param  separator  character specifying the separator between the list items
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition print(STD_NAMESPACE ostream &stream,
                      const size_t flags = 0,
                      const char separator = ',') const;

    /** read list of referenced frame numbers
     ** @param  dataset  DICOM dataset from which the list should be read
     *  @param  flags    flag used to customize the reading process (see DSRTypes::RF_xxx)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition read(DcmItem &dataset,
                     const size_t flags);

    /** write list of referenced frame numbers
     ** @param  dataset  DICOM dataset to which the list should be written
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition write(DcmItem &dataset) const;

    /** put list of referenced frame numbers as a string.
     *  This function expects the same input format as created by print(), i.e. a comma
     *  separated list of numerical values.
     ** @param  stringValue  string value to be set
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition putString(const char *stringValue);
};


#endif
