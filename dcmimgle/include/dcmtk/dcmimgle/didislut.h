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
 *  Purpose: DicomDisplayLUT (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:25 $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
class DiDisplayLUT
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


/*
 *
 * CVS/RCS Log:
 * $Log: didislut.h,v $
 * Revision 1.9  2010-10-14 13:16:25  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.8  2005/12/08 16:47:36  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.7  2003/12/08 18:17:41  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.6  2002/07/02 16:23:41  joergr
 * Added support for hardcopy devices to the calibrated output routines.
 *
 * Revision 1.5  2001/06/01 15:49:40  meichel
 * Updated copyright header
 *
 * Revision 1.4  2000/03/08 16:24:14  meichel
 * Updated copyright header.
 *
 * Revision 1.3  2000/03/06 18:19:35  joergr
 * Moved get-method to base class, renamed method and made method virtual to
 * avoid hiding of methods (reported by Sun CC 4.2).
 *
 * Revision 1.2  1999/09/17 12:08:24  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.1  1999/09/10 08:55:19  joergr
 * Added support for CIELAB display function. Restructured class hierarchy
 * for display functions.
 *
 */
