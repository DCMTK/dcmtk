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
 *  Purpose: DicomCIELABFunction (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-09-10 08:50:22 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/diciefn.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 * 
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DICIEFN_H
#define __DICIEFN_H

#include "osconfig.h"

#include "didispfn.h"
#include "dicielut.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to manage CIELAB LUTs (for calibration)
 */
class DiCIELABFunction
  : public DiDisplayFunction
{

 public:

    DiCIELABFunction(const char *filename);

    DiCIELABFunction(const double *lum_tab,
                     const Uint16 count,
                     const Uint16 max = 255);

    DiCIELABFunction(const Uint16 *ddl_tab,
                     const double *lum_tab,
                     const Uint16 count,
                     const Uint16 max = 255);

    virtual ~DiCIELABFunction();
    
    int writeCurveData(const char *filename);


 protected:

    DiDisplayLUT *getLookupTable(unsigned long count = 0);
    
    int calculateMinMax();


 private:

    double MinLumValue;
    double MaxLumValue;

 // --- declarations to avoid compiler warnings
 
    DiCIELABFunction(const DiDisplayFunction &);
    DiCIELABFunction &operator=(const DiDisplayFunction &);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diciefn.h,v $
 * Revision 1.1  1999-09-10 08:50:22  joergr
 * Added support for CIELAB display function. Restructured class hierarchy
 * for display functions.
 *
 *
 *
 */
