/*
 *
 *  Copyright (C) 1996-2001, OFFIS
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
 *  Purpose: DicomGSDFLUT (Header)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-06-01 15:49:41 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/digsdlut.h,v $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 * 
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIGSDLUT_H
#define __DIGSDLUT_H

#include "osconfig.h"

#include "didislut.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to compute and store the GSDF lookup table
 */
class DiGSDFLUT
  : public DiDisplayLUT
{

 public:

    /** constructor
     *
     ** @param  count     number of values to be stored
     *  @param  max       maximum value to be stored
     *  @param  ddl_tab   array of DDL values
     *  @param  lum_tab   array of luminance values
     *  @param  ddl_cnt   number of DDL values
     *  @param  gsdf_tab  array with GSDF
     *  @param  gsdf_spl  array with helper function used for interpolation
     *  @param  gsdf_cnt  number of values in GSDF
     *  @param  jnd_min   minimum JND index value
     *  @param  jnd_max   maximum JND index value
     *  @param  amb       ambient light value
     *  @param  stream    output stream (used to write curve data to a file)
     *  @param  mode      write CC and PSC to stream if OFTrue
     */
    DiGSDFLUT(const unsigned long count,
              const Uint16 max,
              const Uint16 *ddl_tab,
              const double *lum_tab,
              const unsigned long ddl_cnt,
              const double *gsdf_tab,
              const double *gsdf_spl,
              const unsigned int gsdf_cnt,
              const double jnd_min,
              const double jnd_max,
              const double amb,
              ostream *stream = NULL,
              const OFBool mode = OFTrue);

    /** destructor
     */
    virtual ~DiGSDFLUT();


 protected:
 
    /** create lookup table
     *
     ** @param  ddl_tab   array of DDL values
     *  @param  lum_tab   array of luminance values
     *  @param  ddl_cnt   number of DDL values
     *  @param  gsdf_tab  array with GSDF
     *  @param  gsdf_spl  array with helper function used for interpolation
     *  @param  gsdf_cnt  number of values in GSDF
     *  @param  jnd_min   minimum JND index value
     *  @param  jnd_max   maximum JND index value
     *  @param  stream    output stream (used to write curve data to a file)
     *  @param  mode      write CC and PSC to stream if OFTrue
     *
     ** @return status, true if successful, false otherwise
     */
    int createLUT(const Uint16 *ddl_tab,
                  const double *lum_tab,
                  const unsigned long ddl_cnt,
                  const double *gsdf_tab,
                  const double *gsdf_spl,
                  const unsigned int gsdf_cnt,
                  const double jnd_min,
                  const double jnd_max,
                  ostream *stream = NULL,
                  const OFBool mode = OFTrue);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: digsdlut.h,v $
 * Revision 1.5  2001-06-01 15:49:41  meichel
 * Updated copyright header
 *
 * Revision 1.4  2000/03/08 16:24:16  meichel
 * Updated copyright header.
 *
 * Revision 1.3  1999/10/18 15:05:52  joergr
 * Enhanced command line tool dcmdspfn (added new options).
 *
 * Revision 1.2  1999/09/17 12:11:32  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.1  1999/09/10 08:50:24  joergr
 * Added support for CIELAB display function. Restructured class hierarchy
 * for display functions.
 *
 */
