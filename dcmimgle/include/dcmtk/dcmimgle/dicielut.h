/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomCIELABLUT (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:25 $
 *  CVS/RCS Revision: $Revision: 1.12 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DICIELUT_H
#define DICIELUT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/didislut.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to compute and store the CIELAB lookup table
 */
class DiCIELABLUT
  : public DiDisplayLUT
{

 public:

    /** constructor
     *
     ** @param  count      number of values to be stored in the LUT
     *  @param  max        maximum value to be stored in the LUT
     *  @param  ddl_tab    array of DDL values
     *  @param  val_tab    array of values
     *  @param  ddl_cnt    number of DDL values
     *  @param  val_min    minimum value in the array
     *  @param  val_max    maximum value in the array
     *  @param  lum_min    minimum luminance value to be used (lower border)
     *  @param  lum_max    maximum luminance value to be used (upper border)
     *  @param  amb        ambient light value
     *  @param  inverse    apply inverse transformation if OFTrue
     *  @param  stream     output stream (used to write curve data to a file)
     *  @param  printMode  write CC and PSC to stream if OFTrue
     */
    DiCIELABLUT(const unsigned long count,
                const Uint16 max,
                const Uint16 *ddl_tab,
                const double *val_tab,
                const unsigned long ddl_cnt,
                const double val_min,
                const double val_max,
                const double lum_min,
                const double lum_max,
                const double amb,
                const OFBool inverse = OFFalse,
                STD_NAMESPACE ostream *stream = NULL,
                const OFBool printMode = OFTrue);

    /** destructor
     */
    virtual ~DiCIELABLUT();


 protected:

    /** create lookup table
     *
     ** @param  ddl_tab    array of DDL values
     *  @param  val_tab    array of values
     *  @param  ddl_cnt    number of DDL values
     *  @param  val_min    minimum value in the array
     *  @param  val_max    maximum value in the array
     *  @param  lum_min    minimum luminance value to be used (lower border)
     *  @param  lum_max    maximum luminance value to be used (upper border)
     *  @param  inverse    apply inverse transformation if OFTrue
     *  @param  stream     output stream (used to write curve data to a file)
     *  @param  printMode  write CC and PSC to stream if OFTrue
     *
     ** @return status, true if successful, false otherwise
     */
    int createLUT(const Uint16 *ddl_tab,
                  const double *val_tab,
                  const unsigned long ddl_cnt,
                  const double val_min,
                  const double val_max,
                  const double lum_min,
                  const double lum_max,
                  const OFBool inverse = OFFalse,
                  STD_NAMESPACE ostream *stream = NULL,
                  const OFBool printMode = OFTrue);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dicielut.h,v $
 * Revision 1.12  2010-10-14 13:16:25  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.11  2006/08/15 16:30:11  meichel
 * Updated the code in module dcmimgle to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.10  2005/12/08 16:47:34  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.9  2003/12/08 18:16:55  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated CVS header.
 *
 * Revision 1.8  2003/02/12 11:35:16  joergr
 * Added Dmin/max support to CIELAB calibration routines.
 *
 * Revision 1.7  2002/07/18 12:27:01  joergr
 * Added polygonal curve fitting algorithm as an alternate interpolation
 * method.
 *
 * Revision 1.6  2002/07/02 16:23:41  joergr
 * Added support for hardcopy devices to the calibrated output routines.
 *
 * Revision 1.5  2001/06/01 15:49:39  meichel
 * Updated copyright header
 *
 * Revision 1.4  2000/03/08 16:24:14  meichel
 * Updated copyright header.
 *
 * Revision 1.3  1999/10/18 15:05:50  joergr
 * Enhanced command line tool dcmdspfn (added new options).
 *
 * Revision 1.2  1999/09/17 12:08:23  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.1  1999/09/10 08:50:23  joergr
 * Added support for CIELAB display function. Restructured class hierarchy
 * for display functions.
 *
 */
