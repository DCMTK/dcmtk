/*
 *
 *  Copyright (C) 1999, OFFIS
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-09-10 08:55:19 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/didislut.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
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

    DiDisplayLUT(const unsigned long count,
                 const Uint16 max,
                 const double amb);

    virtual ~DiDisplayLUT();

    inline Uint16 getValue(const Uint16 pos) const
    {
        return Data[pos];
    }

    inline double getAmbientLightValue() const
    {
        return AmbientLight;
    }


 private:
 
    double AmbientLight;
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: didislut.h,v $
 * Revision 1.1  1999-09-10 08:55:19  joergr
 * Added support for CIELAB display function. Restructured class hierarchy
 * for display functions.
 *
 *
 *
 */
