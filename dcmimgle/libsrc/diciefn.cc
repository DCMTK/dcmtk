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
 *  Purpose: DicomCIELABFunction (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-10-18 15:06:23 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/diciefn.cc,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#include "diciefn.h"

#include <fstream.h>

//BEGIN_EXTERN_C
 #include <math.h>
//END_EXTERN_C



/*----------------*
 *  constructors  *
 *----------------*/

DiCIELABFunction::DiCIELABFunction(const char *filename)
  : DiDisplayFunction(filename)
{
    if (!Valid)
    {
        if (DicomImageClass::DebugLevel & DicomImageClass::DL_Errors)
            cerr << "ERROR: invalid DISPLAY file ... ignoring !" << endl;
    }
}


DiCIELABFunction::DiCIELABFunction(const double *lum_tab,             // UNTESTED !!
                                   const unsigned long count,
                                   const Uint16 max)
  : DiDisplayFunction(lum_tab, count, max)
{
    if (!Valid)
    {
        if (DicomImageClass::DebugLevel & DicomImageClass::DL_Errors)
            cerr << "ERROR: invalid DISPLAY values ... ignoring !" << endl;
    }
}


DiCIELABFunction::DiCIELABFunction(const Uint16 *ddl_tab,             // UNTESTED !!
                                   const double *lum_tab,
                                   const unsigned long count,
                                   const Uint16 max)
  : DiDisplayFunction(ddl_tab, lum_tab, count, max)
{
    if (!Valid)
    {
        if (DicomImageClass::DebugLevel & DicomImageClass::DL_Errors)
            cerr << "ERROR: invalid DISPLAY values ... ignoring !" << endl;
    }
}


DiCIELABFunction::DiCIELABFunction(const double lum_min,
                                   const double lum_max,
                                   const unsigned long count)
  : DiDisplayFunction(lum_min, lum_max, count)
{
    if (!Valid)
    {
        if (DicomImageClass::DebugLevel & DicomImageClass::DL_Errors)
            cerr << "ERROR: invalid DISPLAY values ... ignoring !" << endl;
    }
}


/*--------------*
 *  destructor  *
 *--------------*/

DiCIELABFunction::~DiCIELABFunction()
{
}


/********************************************************************/


int DiCIELABFunction::writeCurveData(const char *filename,
                                     const OFBool mode)
{
    if ((filename != NULL) && (strlen(filename) > 0))
    {
        ofstream file(filename);
        if (file)
        {
            file << "# Display function: CIELAB" << endl;
            file << "# Number of DDLs  : " << ValueCount << endl;
            file << "# Luminance range : " << MinLumValue << " - " << MaxLumValue << endl;
            file << "# Ambient light   : " << AmbientLight << endl << endl;
            if (mode)
                file << "DDL\tCC\tCIELAB\tPSC" << endl;
            else
                file << "DDL\tCIELAB" << endl;
            DiCIELABLUT *lut = new DiCIELABLUT(ValueCount, MaxDDLValue, DDLValue, LumValue, ValueCount,
                MinLumValue, MaxLumValue, AmbientLight, &file, mode);                          // write curve data to file
            int status = (lut != NULL) && (lut->isValid());
            delete lut;
            return status;
        }
    }
    return 0;
}


/********************************************************************/


DiDisplayLUT *DiCIELABFunction::getLookupTable(unsigned long count)
{
    DiDisplayLUT *lut = NULL;
    if (count <= MAX_TABLE_ENTRY_COUNT)
    {
        lut = new DiCIELABLUT(count, MaxDDLValue, DDLValue, LumValue, ValueCount, MinLumValue,
            MaxLumValue, AmbientLight);
    }
    return lut;
}


/********************************************************************/


/*
 *
 * CVS/RCS Log:
 * $Log: diciefn.cc,v $
 * Revision 1.4  1999-10-18 15:06:23  joergr
 * Enhanced command line tool dcmdspfn (added new options).
 *
 * Revision 1.3  1999/10/18 10:14:26  joergr
 * Moved min/max value determination to display function base class. Now the
 * actual min/max values are also used for GSDFunction (instead of first and
 * last luminance value).
 *
 * Revision 1.2  1999/09/17 13:13:29  joergr
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.1  1999/09/10 08:54:47  joergr
 * Added support for CIELAB display function. Restructured class hierarchy
 * for display functions.
 *
 *
 */
