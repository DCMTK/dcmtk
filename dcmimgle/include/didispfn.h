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
 *  Update Date:      $Date: 1999-09-10 08:45:18 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/didispfn.h,v $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 * 
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIDISPFN_H
#define __DIDISPFN_H

#include "osconfig.h"

#include "didislut.h"


/*---------------------*
 *  macro definitions  *
 *---------------------*/

#define MAX_DISPLAY_FUNCTIONS 2
#define MAX_NUMBER_OF_TABLES 15
#define WIDTH_OF_PVALUES     16


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to handle monitor characteristics file 
 *  and manage display LUTs (for calibration)
 */
class DiDisplayFunction
{

 public:

    DiDisplayFunction(const char *filename);

    DiDisplayFunction(const double *lum_tab,
                      const Uint16 count,
                      const Uint16 max = 255);

    DiDisplayFunction(const Uint16 *ddl_tab,
                      const double *lum_tab,
                      const Uint16 count,
                      const Uint16 max = 255);

    virtual ~DiDisplayFunction();

    inline int isValid() const
    {
        return Valid;
    }

    inline Uint16 getMaxDDLValue() const
    {
        return MaxDDLValue;
    }

    const DiDisplayLUT *getLookupTable(const int bits,
                                       unsigned long count = 0);
    
    int deleteLookupTable(const int bits);
    
    virtual int writeCurveData(const char *filename) = 0;
    
    inline double getAmbientLightValue() const
    {
        return AmbientLight;
    }
    
    virtual int setAmbientLightValue(const double value);


 protected:

    virtual DiDisplayLUT *getLookupTable(unsigned long count = 0) = 0;

    int readConfigFile(const char *filename);

    int createSortedTable(const Uint16 *ddl_tab,
                          const double *lum_tab);

    int interpolateValues();

    int Valid;

    Uint16 ValueCount;                  // Number of DDL/Lum values
    Uint16 MaxDDLValue;                 // maximum DDL value (e.g. 255)
    
    double AmbientLight;

    Uint16 *DDLValue;
    double *LumValue;

    static const int MinBits;
    static const int MaxBits;

    DiDisplayLUT *LookupTable[MAX_NUMBER_OF_TABLES];


 private:

 // --- declarations to avoid compiler warnings
 
    DiDisplayFunction(const DiDisplayFunction &);
    DiDisplayFunction &operator=(const DiDisplayFunction &);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: didispfn.h,v $
 * Revision 1.7  1999-09-10 08:45:18  joergr
 * Added support for CIELAB display function.
 *
 * Revision 1.6  1999/03/24 17:19:20  joergr
 * Added/Modified comments and formatting.
 * Added support for Barten transformation from 2 to 7 bits input (now: 2-16).
 *
 * Revision 1.5  1999/03/03 11:47:42  joergr
 * Added support to specify ambient light value (re: Barten transformation).
 *
 * Revision 1.4  1999/02/23 16:55:32  joergr
 * Added tool to export display curves to a text file.
 *
 * Revision 1.3  1999/02/11 15:44:22  joergr
 * Removed unused parameter / member variable.
 *
 * Revision 1.2  1999/02/05 16:43:12  joergr
 * Added optional parameter to method convertPValueToDDL to specify width
 * of output data (number of bits).
 *
 * Revision 1.1  1999/02/03 17:46:37  joergr
 * Added support for calibration according to Barten transformation (incl.
 * a DISPLAY file describing the monitor characteristic).
 *
 *
 */
