/*
 *
 *  Copyright (C) 1996-2011, OFFIS e.V.
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
 *  Purpose: DicomGSDFLUT (Header)
 *
 */


#ifndef DIGSDLUT_H
#define DIGSDLUT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/didislut.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to compute and store the GSDF lookup table
 */
class DCMTK_DCMIMGLE_EXPORT DiGSDFLUT
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
     *  @param  gsdf_tab   array with Grayscale Standard Display Function
     *  @param  gsdf_spl   array with helper function used for interpolation
     *  @param  gsdf_cnt   number of values in GSDF array
     *  @param  jnd_min    minimum JND index value
     *  @param  jnd_max    maximum JND index value
     *  @param  lum_min    minimum luminance value to be used (lower border)
     *  @param  lum_max    maximum luminance value to be used (upper border)
     *  @param  amb        (reflected) ambient light value
     *  @param  illum      illumination value
     *  @param  inverse    apply inverse transformation if OFTrue
     *  @param  stream     output stream (used to write curve data to a file)
     *  @param  printMode  write CC and PSC to stream if OFTrue
     */
    DiGSDFLUT(const unsigned long count,
              const Uint16 max,
              const Uint16 *ddl_tab,
              const double *val_tab,
              const unsigned long ddl_cnt,
              const double *gsdf_tab,
              const double *gsdf_spl,
              const unsigned int gsdf_cnt,
              const double jnd_min,
              const double jnd_max,
              const double lum_min,
              const double lum_max,
              const double amb,
              const double illum,
              const OFBool inverse = OFFalse,
              STD_NAMESPACE ostream *stream = NULL,
              const OFBool printMode = OFTrue);

    /** destructor
     */
    virtual ~DiGSDFLUT();


 protected:

    /** create lookup table
     *
     ** @param  ddl_tab    array of DDL values
     *  @param  val_tab    array of luminance values
     *  @param  ddl_cnt    number of DDL values
     *  @param  gsdf_tab   array with Grayscale Standard Display Function
     *  @param  gsdf_spl   array with helper function used for interpolation
     *  @param  gsdf_cnt   number of values in GSDF array
     *  @param  jnd_min    minimum JND index value
     *  @param  jnd_max    maximum JND index value
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
                  const double *gsdf_tab,
                  const double *gsdf_spl,
                  const unsigned int gsdf_cnt,
                  const double jnd_min,
                  const double jnd_max,
                  const double lum_min,
                  const double lum_max,
                  const OFBool inverse = OFFalse,
                  STD_NAMESPACE ostream *stream = NULL,
                  const OFBool printMode = OFTrue);
};


#endif
