/*
 *
 *  Copyright (C) 1996-2002, OFFIS
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomCIELABLUT (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-07-18 12:27:01 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dicielut.h,v $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DICIELUT_H
#define __DICIELUT_H

#include "osconfig.h"

#include "didislut.h"


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
     ** @param  count      number of values to be stored
     *  @param  max        maximum value to be stored
     *  @param  ddl_tab    array of DDL values
     *  @param  val_tab    array of values
     *  @param  ddl_cnt    number of DDL values
     *  @param  val_min    minimum value
     *  @param  val_max    maximum value
     *  @param  amb        ambient light value
     *  @param  inverse    apply inverse transformation
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
                const double amb,
                const OFBool inverse = OFFalse,
                ostream *stream = NULL,
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
     *  @param  val_min    minimum value
     *  @param  val_max    maximum value
     *  @param  inverse    apply inverse transformation
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
                  const OFBool inverse = OFFalse,
                  ostream *stream = NULL,
                  const OFBool printMode = OFTrue);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dicielut.h,v $
 * Revision 1.7  2002-07-18 12:27:01  joergr
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
