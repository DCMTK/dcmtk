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
 *  Purpose: DicomGSDFunction (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-09-10 08:50:23 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/digsdfn.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 * 
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIGSDFN_H
#define __DIGSDFN_H

#include "osconfig.h"

#include "didispfn.h"
#include "digsdlut.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to manage GSDF LUTs (for calibration)
 */
class DiGSDFunction
  : public DiDisplayFunction
{

 public:

    DiGSDFunction(const char *filename);

    DiGSDFunction(const double *lum_tab,
                  const Uint16 count,
                  const Uint16 max = 255);

    DiGSDFunction(const Uint16 *ddl_tab,
                  const double *lum_tab,
                  const Uint16 count,
                  const Uint16 max = 255);

    virtual ~DiGSDFunction();
    
    int writeCurveData(const char *filename);
    
    int setAmbientLightValue(const double value);


 protected:

    DiDisplayLUT *getLookupTable(unsigned long count = 0);

    int calculateGSDF();
    
    int calculateGSDFSpline();

    int calculateJNDBoundaries();

    double getJNDIndex(const double lum) const;


 private:

    double JNDMin;
    double JNDMax;
    
    static const unsigned int GSDFCount;
    double *GSDFValue;
    double *GSDFSpline;

 // --- declarations to avoid compiler warnings
 
    DiGSDFunction(const DiDisplayFunction &);
    DiGSDFunction &operator=(const DiDisplayFunction &);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: digsdfn.h,v $
 * Revision 1.1  1999-09-10 08:50:23  joergr
 * Added support for CIELAB display function. Restructured class hierarchy
 * for display functions.
 *
 *
 *
 */
