/*
 *
 *  Copyright (C) 1996-2000, OFFIS
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
 *  Purpose: DicomDisplayLUT (Header)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:24:14 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/didislut.h,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 * 
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIDISLUT_H
#define __DIDISLUT_H

#include "osconfig.h"

#include "dibaslut.h"


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
     */
    DiDisplayLUT(const unsigned long count,
                 const Uint16 max,
                 const double amb);

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


 private:
 
    /// ambient light value (measured in cd/m^2)
    double AmbientLight;
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: didislut.h,v $
 * Revision 1.4  2000-03-08 16:24:14  meichel
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
