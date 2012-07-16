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
 *  Purpose: DicomDisplayLUT (Header)
 *
 */


#ifndef DIDISLUT_H
#define DIDISLUT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/dibaslut.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to compute and store the Display lookup table
 */
class DCMTK_DCMIMGLE_EXPORT DiDisplayLUT
  : public DiBaseLUT
{

 public:

    /** constructor
     *
     ** @param  count  number of values to be stored
     *  @param  max    maximum value to be stored
     *  @param  amb    ambient light value
     *  @param  illum  illumination value (optional)
     */
    DiDisplayLUT(const unsigned long count,
                 const Uint16 max,
                 const double amb,
                 const double illum = 0);

    /** destructor
     */
    virtual ~DiDisplayLUT();

    /** get ambient light value
     *
     ** @return ambient light value
     */
    inline double getAmbientLightValue() const
    {
        return AmbientLight;
    }

    /** get illumination value
     *
     ** @return illumination value
     */
    inline double getIlluminationValue() const
    {
        return Illumination;
    }


 private:

    /// ambient light value (measured in cd/m^2)
    const double AmbientLight;
    /// illumination value (measured in cd/m^2)
    const double Illumination;
};


#endif
