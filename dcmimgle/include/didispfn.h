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
 *  Purpose: DicomDisplayFunction (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-02-05 16:43:12 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/didispfn.h,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 * 
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIDISPFN_H
#define __DIDISPFN_H

#include "osconfig.h"
#include "dctypes.h"

#include "dibarlut.h"


/*---------------------*
 *  macro definitions  *
 *---------------------*/

#define MAX_NUMBER_OF_TABLES  9
#define WIDTH_OF_PVALUES     16


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiDisplayFunction
{

 public:

    DiDisplayFunction(const char *filename);

    DiDisplayFunction(const double *lum_tab,
                      const Uint16 count,
                      const Uint16 max = 255,
                      const Uint16 out = 0);

    DiDisplayFunction(const Uint16 *ddl_tab,
                      const double *lum_tab,
                      const Uint16 count,
                      const Uint16 max = 255,
                      const Uint16 out = 0);

    virtual ~DiDisplayFunction();

    inline int isValid() const
    {
        return Valid;
    }

    inline Uint16 getValueCount() const
    {
        return ValueCount;
    }

    const DiBartenLUT *getBartenLUT(const int bits,
                                    unsigned long count = 0);
    
    int deleteBartenLUT(const int bits);


 protected:

    int readConfigFile(const char *filename);

    int createSortedTable(const Uint16 *ddl_tab,
                          const double *lum_tab);

    int interpolateValues();
    
    int calculateGSDF();
    
    int calculateGSDFSpline();

    int calculateJNDBoundaries();

    double getJNDIndex(const double lum) const;


 private:

    int Valid;

    Uint16 ValueCount;
    Uint16 MaxDDLValue;
    Uint16 MaxOutValue;
    
    double JNDMin;
    double JNDMax;

    Uint16 *DDLValue;
    double *LumValue;

    static const int MinBits;
    static const int MaxBits;

    static const unsigned int GSDFCount;
    double *GSDFValue;
    double *GSDFSpline;

    DiBartenLUT *BartenLUT[MAX_NUMBER_OF_TABLES];

 // --- declarations to avoid compiler warnings
 
    DiDisplayFunction(const DiDisplayFunction &);
    DiDisplayFunction &operator=(const DiDisplayFunction &);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: didispfn.h,v $
 * Revision 1.2  1999-02-05 16:43:12  joergr
 * Added optional parameter to method convertPValueToDDL to specify width
 * of output data (number of bits).
 *
 * Revision 1.1  1999/02/03 17:46:37  joergr
 * Added support for calibration according to Barten transformation (incl.
 * a DISPLAY file describing the monitor characteristic).
 *
 *
 *
 */
