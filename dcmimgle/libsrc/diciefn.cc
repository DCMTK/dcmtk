/*
 *
 *  Copyright (C) 1996-2002, OFFIS
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
 *  Update Date:      $Date: 2002-07-02 16:24:36 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/diciefn.cc,v $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#include "ofconsol.h"
#include "diciefn.h"

#include "ofstream.h"

#include <math.h>


/*----------------*
 *  constructors  *
 *----------------*/

DiCIELABFunction::DiCIELABFunction(const char *filename,
                                   const E_DeviceType deviceType)
  : DiDisplayFunction(filename, deviceType)
{
    if (!Valid)
    {
        if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
        {
            ofConsole.lockCerr() << "ERROR: invalid DISPLAY file ... ignoring !" << endl;
            ofConsole.unlockCerr();
        }
    }
}


DiCIELABFunction::DiCIELABFunction(const double *val_tab,             // UNTESTED !!
                                   const unsigned long count,
                                   const Uint16 max,
                                   const E_DeviceType deviceType)
  : DiDisplayFunction(val_tab, count, max, deviceType)
{
    if (!Valid)
    {
        if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
        {
            ofConsole.lockCerr() << "ERROR: invalid DISPLAY values ... ignoring !" << endl;
            ofConsole.unlockCerr();
        }
    }
}


DiCIELABFunction::DiCIELABFunction(const Uint16 *ddl_tab,             // UNTESTED !!
                                   const double *val_tab,
                                   const unsigned long count,
                                   const Uint16 max,
                                   const E_DeviceType deviceType)
  : DiDisplayFunction(ddl_tab, val_tab, count, max, deviceType)
{
    if (!Valid)
    {
        if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
        {
            ofConsole.lockCerr() << "ERROR: invalid DISPLAY values ... ignoring !" << endl;
            ofConsole.unlockCerr();
        }
    }
}


DiCIELABFunction::DiCIELABFunction(const double val_min,
                                   const double val_max,
                                   const unsigned long count,
                                   const E_DeviceType deviceType)
  : DiDisplayFunction(val_min, val_max, count, deviceType)
{
    if (!Valid)
    {
        if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
        {
            ofConsole.lockCerr() << "ERROR: invalid DISPLAY values ... ignoring !" << endl;
            ofConsole.unlockCerr();
        }
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
            file << "# Display function       : CIELAB" << endl;
            if (DeviceType == EDT_Printer)
            {
                file << "# Type of output device  : Printer (hardcopy)" << endl;
                file << "# Device driving levels  : " << ValueCount << endl;
                file << "# Illumination  [cd/m^2] : " << Illumination << endl;
                file << "# Ambient light [cd/m^2] : " << AmbientLight << endl;
                file << "# Luminance     [cd/m^2] : " << convertODtoLum(MaxValue) << " - " << convertODtoLum(MinValue) << endl;
                file << "# Optical density   [OD] : " << MinValue << " - " << MaxValue << endl << endl;
                file << "# NB: values for CC, CIELAB and PSC are always specified in cd/m^2" << endl << endl;
            } else {
                file << "# Type of output device  : Monitor (softcopy)" << endl;
                file << "# Device driving levels  : " << ValueCount << endl;
                file << "# Ambient light [cd/m^2] : " << AmbientLight << endl << endl;
                file << "# Luminance     [cd/m^2] : " << (MinValue + AmbientLight)<< " - " << (MaxValue + AmbientLight) << endl;
            }

            if (mode)
                file << "DDL\tCC\tCIELAB\tPSC" << endl;
            else
                file << "DDL\tCIELAB" << endl;
            /* create CIELAB LUT and write curve data to file */
            DiCIELABLUT *lut = NULL;
            if (DeviceType == EDT_Printer)
            {
                /* printer: values are in optical density, first convert them to luminance */
                double *tmp_tab = convertODtoLumTable(LODValue, ValueCount);
                if (tmp_tab != NULL)
                {
                    lut = new DiCIELABLUT(ValueCount, MaxDDLValue, DDLValue, tmp_tab, ValueCount,
                        convertODtoLum(MaxValue), convertODtoLum(MinValue), AmbientLight, &file, mode);
                    /* delete temporary table */
                    delete[] tmp_tab;
                }
            } else {
                /* monitor: values are already in luminance */
                lut = new DiCIELABLUT(ValueCount, MaxDDLValue, DDLValue, LODValue, ValueCount,
                    MinValue + AmbientLight, MaxValue + AmbientLight, AmbientLight, &file, mode);     // write curve data to file
            }
            int status = (lut != NULL) && (lut->isValid());
            delete lut;
            return status;
        }
    }
    return 0;
}


/********************************************************************/


DiDisplayLUT *DiCIELABFunction::getDisplayLUT(unsigned long count)
{
    DiDisplayLUT *lut = NULL;
    if (count <= MAX_TABLE_ENTRY_COUNT)
    {
        if (DeviceType == EDT_Printer)
        {
            /* printer: values are in optical density, first convert them to luminance */
            double *tmp_tab = convertODtoLumTable(LODValue, ValueCount);
            if (tmp_tab != NULL)
            {
                /* create new CIELAB LUT */
                lut = new DiCIELABLUT(count, MaxDDLValue, DDLValue, tmp_tab, ValueCount,
                    convertODtoLum(MaxValue), convertODtoLum(MinValue), AmbientLight);
                /* delete temporary table */
                delete[] tmp_tab;
            }
        } else {
            /* monitor: values are already in luminance */
            lut = new DiCIELABLUT(count, MaxDDLValue, DDLValue, LODValue, ValueCount,
                MinValue, MaxValue, AmbientLight);
        }
    }
    return lut;
}


/********************************************************************/


/*
 *
 * CVS/RCS Log:
 * $Log: diciefn.cc,v $
 * Revision 1.13  2002-07-02 16:24:36  joergr
 * Added support for hardcopy devices to the calibrated output routines.
 *
 * Revision 1.12  2002/04/16 13:53:31  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 * Thanks to Andreas Barth <Andreas.Barth@bruker-biospin.de> for his
 * contribution.
 *
 * Revision 1.11  2001/06/01 15:49:53  meichel
 * Updated copyright header
 *
 * Revision 1.10  2000/04/28 12:33:41  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.9  2000/04/27 13:10:24  joergr
 * Dcmimgle library code now consistently uses ofConsole for error output.
 *
 * Revision 1.8  2000/03/08 16:24:26  meichel
 * Updated copyright header.
 *
 * Revision 1.7  2000/03/06 18:20:34  joergr
 * Moved get-method to base class, renamed method and made method virtual to
 * avoid hiding of methods (reported by Sun CC 4.2).
 *
 * Revision 1.6  2000/03/03 14:09:16  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.5  2000/02/02 11:04:52  joergr
 * Removed space characters before preprocessor directives.
 *
 * Revision 1.4  1999/10/18 15:06:23  joergr
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
