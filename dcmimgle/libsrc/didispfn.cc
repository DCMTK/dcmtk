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
 *  Purpose: DicomDisplayFunction (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-04-28 12:33:42 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/didispfn.cc,v $
 *  CVS/RCS Revision: $Revision: 1.25 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "ofconsol.h"
#include "ofbmanip.h"

#include "didispfn.h"
#include "displint.h"

#include <fstream.h>

BEGIN_EXTERN_C
#ifdef HAVE_CTYPE_H
#include <ctype.h>
#endif
END_EXTERN_C


/*----------------------------*
 *  constant initializations  *
 *----------------------------*/

const int DiDisplayFunction::MinBits = 2;
const int DiDisplayFunction::MaxBits = 16;


/*----------------*
 *  constructors  *
 *----------------*/

DiDisplayFunction::DiDisplayFunction(const char *filename)
  : Valid(0),
    ValueCount(0),
    MaxDDLValue(0),
    AmbientLight(0),
    DDLValue(NULL),
    LumValue(NULL),
    MinLumValue(0),
    MaxLumValue(0)
{
    OFBitmanipTemplate<DiDisplayLUT *>::zeroMem(LookupTable, MAX_NUMBER_OF_TABLES);
    if (readConfigFile(filename))
        Valid = createSortedTable(DDLValue, LumValue) && calculateMinMax() && interpolateValues();
}


DiDisplayFunction::DiDisplayFunction(const double *lum_tab,             // UNTESTED !!
                                     const unsigned long count,
                                     const Uint16 max)
  : Valid(0),
    ValueCount(count),
    MaxDDLValue(max),
    AmbientLight(0),
    DDLValue(NULL),
    LumValue(NULL),
    MinLumValue(0),
    MaxLumValue(0)
{
    OFBitmanipTemplate<DiDisplayLUT *>::zeroMem(LookupTable, MAX_NUMBER_OF_TABLES);
    if ((ValueCount > 0) && (ValueCount == (unsigned long)MaxDDLValue + 1))
    {
        DDLValue = new Uint16[ValueCount];
        LumValue = new double[ValueCount];
        if ((DDLValue != NULL) && (LumValue != NULL))
        {
            register Uint16 i;
            for (i = 0; i <= MaxDDLValue; i++)
            {
                DDLValue[i] = i;                            // set DDL values
                LumValue[i] = lum_tab[i];                   // copy table
            }
            Valid = calculateMinMax();
        }
    }
}


DiDisplayFunction::DiDisplayFunction(const Uint16 *ddl_tab,             // UNTESTED !!
                                     const double *lum_tab,
                                     const unsigned long count,
                                     const Uint16 max)
  : Valid(0),
    ValueCount(count),
    MaxDDLValue(max),
    AmbientLight(0),
    DDLValue(NULL),
    LumValue(NULL),
    MinLumValue(0),
    MaxLumValue(0)
{
    OFBitmanipTemplate<DiDisplayLUT *>::zeroMem(LookupTable, MAX_NUMBER_OF_TABLES);
    Valid = createSortedTable(ddl_tab, lum_tab) && calculateMinMax() && interpolateValues();
}


DiDisplayFunction::DiDisplayFunction(const double lum_min,
                                     const double lum_max,
                                     const unsigned long count)
  : Valid(0),
    ValueCount(count),
    MaxDDLValue(0),
    AmbientLight(0),
    DDLValue(NULL),
    LumValue(NULL),
    MinLumValue(lum_min),
    MaxLumValue(lum_max)
{
    OFBitmanipTemplate<DiDisplayLUT *>::zeroMem(LookupTable, MAX_NUMBER_OF_TABLES);
    if ((ValueCount > 1) && (MinLumValue < MaxLumValue))
    {
        MaxDDLValue = (Uint16)(count - 1);
        DDLValue = new Uint16[ValueCount];
        LumValue = new double[ValueCount];
        if ((DDLValue != NULL) && (LumValue != NULL))
        {
            register Uint16 i;
            const double lum = (lum_max - lum_min) / (double)MaxDDLValue;
            DDLValue[0] = 0;
            LumValue[0] = lum_min;
            for (i = 1; i < MaxDDLValue; i++)
            {
                DDLValue[i] = i;                            // set DDL values
                LumValue[i] = LumValue[i - 1] + lum;        // compute luminance value
            }
            DDLValue[MaxDDLValue] = MaxDDLValue;
            LumValue[MaxDDLValue] = lum_max;
            Valid = 1;
        }
    }
}


/*--------------*
 *  destructor  *
 *--------------*/

DiDisplayFunction::~DiDisplayFunction()
{
    delete[] DDLValue;
    delete[] LumValue;
    register unsigned int i;
    for (i = 0; i < MAX_NUMBER_OF_TABLES; i++)
        delete LookupTable[i];
}


/********************************************************************/


const DiDisplayLUT *DiDisplayFunction::getLookupTable(const int bits,
                                                      unsigned long count)
{
    if (Valid && (bits >= MinBits) && (bits <= MaxBits))
    {
        const int idx = bits - MinBits;
        if (count == 0)
            count = DicomImageClass::maxval(bits, 0);
        if ((LookupTable[idx] != NULL) && ((count != LookupTable[idx]->getCount()) ||
            (AmbientLight != LookupTable[idx]->getAmbientLightValue())))
        {
            delete LookupTable[idx];
            LookupTable[idx] = NULL;
        }
        if (LookupTable[idx] == NULL)                             // first calculation of this LUT
            LookupTable[idx] = getDisplayLUT(count);
        return LookupTable[idx];
    }
    return NULL;
}


int DiDisplayFunction::deleteLookupTable(const int bits)
{
    if (bits == 0)                          // delete all LUTs
    {
        register int i;
        for (i = 0; i < MAX_NUMBER_OF_TABLES; i++)
        {
            delete LookupTable[i];
            LookupTable[i] = NULL;
        }
        return 1;
    }
    else if ((bits >= MinBits) && (bits <= MaxBits))
    {
        const int idx = bits - MinBits;
        if (LookupTable[idx] != NULL)
        {
            delete LookupTable[idx];
            LookupTable[idx] = NULL;
            return 1;
        }
        return 2;
    }
    return 0;
}


int DiDisplayFunction::setAmbientLightValue(const double value)
{
    if (value >= 0)
    {
        AmbientLight = value;
        return 1;
    }
    return 0;
}


/********************************************************************/


int DiDisplayFunction::readConfigFile(const char *filename)
{
    if ((filename != NULL) && (strlen(filename) > 0))
    {
#ifdef NO_IOS_NOCREATE
        ifstream file(filename, ios::in);
#else
        ifstream file(filename, ios::in|ios::nocreate);
#endif
        if (file)
        {
            char c;
            while (file.get(c))
            {
                if (c == '#')                                               // comment character
                {
                    while (file.get(c) && (c != '\n') && (c != '\r'));      // skip comments
                }
                else if (!isspace(c))                                       // skip whitespaces
                {
                    file.putback(c);
                    if (MaxDDLValue == 0)                                   // read maxvalue
                    {
                        char str[4];
                        file.get(str, sizeof(str));
                        if (strcmp(str, "max") == 0)                        // check for key word: max
                        {
                            file >> MaxDDLValue;
                            if (MaxDDLValue > 0)
                            {
                                DDLValue = new Uint16[MaxDDLValue + 1];
                                LumValue = new double[MaxDDLValue + 1];
                                if ((DDLValue == NULL) || (LumValue == NULL))
                                    return 0;
                            } else {
                                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
                                {
                                    ofConsole.lockCerr() << "ERROR: invalid or missing value for maximum DDL value in DISPLAY file !" << endl;
                                    ofConsole.unlockCerr();
                                }
                                return 0;                                   // abort
                            }
                        } else {
                            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
                            {
                                ofConsole.lockCerr() << "ERROR: missing keyword 'max' for maximum DDL value in DISPLAY file !" << endl;
                                ofConsole.unlockCerr();
                            }
                            return 0;                                       // abort
                        }
                    }
                    else if ((AmbientLight == 0.0) && (c == 'a'))           // read ambient light value (optional)
                    {
                        char str[4];
                        file.get(str, sizeof(str));
                        if (strcmp(str, "amb") == 0)                        // check for key word: amb
                        {
                            file >> AmbientLight;
                            if (AmbientLight < 0)
                            {
                                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                                {
                                    ofConsole.lockCerr() << "WARNING: invalid value for ambient light in DISPLAY file ...ignoring !" << endl;
                                    ofConsole.unlockCerr();
                                }
                                AmbientLight = 0;
                            }
                        } else {
                            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
                            {
                                ofConsole.lockCerr() << "ERROR: invalid DISPLAY file ... ignoring !" << endl;
                                ofConsole.unlockCerr();
                            }
                            return 0;                                       // abort
                        }
                    } else {
                        if (ValueCount <= (unsigned long)MaxDDLValue)
                        {
                            file >> DDLValue[ValueCount];                   // read DDL value
                            file >> LumValue[ValueCount];                   // read luminance value
                            if (file.fail())
                            {
                                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                                {
                                    ofConsole.lockCerr() << "WARNING: missing luminance value in DISPLAY file ... ignoring last entry !" << endl;
                                    ofConsole.unlockCerr();
                                }
                            }
                            else if (DDLValue[ValueCount] > MaxDDLValue)
                            {
                                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                                {
                                    ofConsole.lockCerr() << "WARNING: DDL value (" << DDLValue[ValueCount]
                                                         << ") exceeds maximum value ("
                                                         << MaxDDLValue << ") in DISPLAY file ..." << endl
                                                         << "         ... ignoring value !" << endl;
                                    ofConsole.unlockCerr();
                                }
                            } else
                                ValueCount++;
                        } else {
                            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                            {
                                ofConsole.lockCerr() << "WARNING: too many values in DISPLAY file ... ignoring last line(s) !" << endl;
                                ofConsole.unlockCerr();
                            }
                            return 2;
                        }
                    }
                }
            }
            if ((MaxDDLValue > 0) && (ValueCount > 0))
                return ((DDLValue != NULL) && (LumValue != NULL));
            else {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                {
                    ofConsole.lockCerr() << "WARNING: invalid DISPLAY file ... ignoring !" << endl;
                    ofConsole.unlockCerr();
                }
            }
        } else {
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
            {
                ofConsole.lockCerr() << "WARNING: can't open DISPLAY file ... ignoring !" << endl;
                ofConsole.unlockCerr();
            }
        }
    }
    return 0;
}


int DiDisplayFunction::createSortedTable(const Uint16 *ddl_tab,
                                         const double *lum_tab)
{
    int status = 0;
    Uint16 *old_ddl = DDLValue;
    double *old_lum = LumValue;
    if ((ValueCount > 0) && (ddl_tab != NULL) && (lum_tab != NULL))
    {
        DDLValue = new Uint16[ValueCount];
        LumValue = new double[ValueCount];
        Sint32 *sort_tab = new Sint32[MaxDDLValue + 1];                             // auxilliary array (temporary)
        if ((DDLValue != NULL) && (LumValue != NULL) && (sort_tab != NULL))
        {
            OFBitmanipTemplate<Sint32>::setMem(sort_tab, -1, MaxDDLValue + 1);      // initialize array
            register unsigned long i;
            for (i = 0; i < ValueCount; i++)
            {
                if (ddl_tab[i] <= MaxDDLValue)                                      // calculate sort table
                    sort_tab[ddl_tab[i]] = i;
            }
            ValueCount = 0;
            for (i = 0; i <= MaxDDLValue; i++)                                      // sort ascending
            {
                if (sort_tab[i] >= 0)
                {
                    DDLValue[ValueCount] = ddl_tab[sort_tab[i]];
                    LumValue[ValueCount] = (lum_tab[sort_tab[i]] > 0) ? lum_tab[sort_tab[i]] : 0;
                    ValueCount++;                                                   // re-count to ignore values exceeding max
                }
            }
            i = 1;
            while ((i < ValueCount) && (LumValue[i - 1] <= LumValue[i]))            // check monotony
                i++;
            if (i < ValueCount)                                                     // invalid luminance value(s)
            {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                {
                    ofConsole.lockCerr() << "WARNING: luminance values (ordered by DDLs) don't ascend monotonously !" << endl;
                    ofConsole.unlockCerr();
                }
            }
            status = (ValueCount > 0);
        }
        delete[] sort_tab;
    }
    delete[] old_ddl;
    delete[] old_lum;
    return status;
}


int DiDisplayFunction::interpolateValues()
{
    if (ValueCount <= (unsigned long)MaxDDLValue)                         // interpolation necessary ?
    {
        int status = 0;
        double *spline = new double[ValueCount];
        if ((spline != NULL) &&
            (DiCubicSpline<Uint16, double>::Function(DDLValue, LumValue, (unsigned int)ValueCount, spline)))
        {
            const unsigned long count = ValueCount;
            Uint16 *old_ddl = DDLValue;
            double *old_lum = LumValue;
            ValueCount = (unsigned long)MaxDDLValue + 1;
            DDLValue = new Uint16[ValueCount];
            LumValue = new double[ValueCount];
            if ((DDLValue != NULL) && (LumValue != NULL))
            {
                register Uint16 i;
                for (i = 0; i <= MaxDDLValue; i++)                          // set all DDL values, from 0 to max
                    DDLValue[i] = i;
                status = DiCubicSpline<Uint16, double>::Interpolation(old_ddl, old_lum, spline, (unsigned int)count,
                                                                      DDLValue, LumValue, (unsigned int)ValueCount);
            }
            delete[] old_ddl;
            delete[] old_lum;
        }
        delete[] spline;
        return status;
    }
    return 2;
}


int DiDisplayFunction::calculateMinMax()
{
    if ((LumValue != NULL) && (ValueCount > 0))
    {
        MinLumValue = LumValue[0];
        MaxLumValue = LumValue[0];
        register unsigned long i;
        for (i = 1; i < ValueCount; i++)
        {
            if (LumValue[i] < MinLumValue)
                MinLumValue = LumValue[i];
            if (LumValue[i] > MaxLumValue)
                MaxLumValue = LumValue[i];
        }
        return 1;
    }
    return 0;
}


/*
 *
 * CVS/RCS Log:
 * $Log: didispfn.cc,v $
 * Revision 1.25  2000-04-28 12:33:42  joergr
 * DebugLevel - global for the module - now derived from OFGlobal (MF-safe).
 *
 * Revision 1.24  2000/04/27 13:10:25  joergr
 * Dcmimgle library code now consistently uses ofConsole for error output.
 *
 * Revision 1.23  2000/03/08 16:24:27  meichel
 * Updated copyright header.
 *
 * Revision 1.22  2000/03/07 16:15:46  joergr
 * Added explicit type casts to make Sun CC 2.0.1 happy.
 *
 * Revision 1.21  2000/03/06 18:20:35  joergr
 * Moved get-method to base class, renamed method and made method virtual to
 * avoid hiding of methods (reported by Sun CC 4.2).
 *
 * Revision 1.20  2000/03/03 14:09:17  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.19  2000/02/02 11:04:52  joergr
 * Removed space characters before preprocessor directives.
 *
 * Revision 1.18  1999/10/18 17:24:00  joergr
 * Added explicit type cast to avoid compiler warnings reported by MSVC.
 *
 * Revision 1.17  1999/10/18 15:06:24  joergr
 * Enhanced command line tool dcmdspfn (added new options).
 *
 * Revision 1.16  1999/10/18 10:14:27  joergr
 * Moved min/max value determination to display function base class. Now the
 * actual min/max values are also used for GSDFunction (instead of first and
 * last luminance value).
 *
 * Revision 1.15  1999/09/10 08:54:49  joergr
 * Added support for CIELAB display function. Restructured class hierarchy
 * for display functions.
 *
 * Revision 1.14  1999/07/23 13:34:08  joergr
 * Modified error reporting while reading calibration file.
 *
 * Revision 1.13  1999/05/03 11:05:29  joergr
 * Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.12  1999/04/29 13:49:37  joergr
 * Renamed class CubicSpline to DiCubicSpline.
 *
 * Revision 1.11  1999/04/28 15:01:44  joergr
 * Introduced new scheme for the debug level variable: now each level can be
 * set separately (there is no "include" relationship).
 *
 * Revision 1.10  1999/03/24 17:22:38  joergr
 * Added support for Barten transformation from 2 to 7 bits input (now: 2-16).
 *
 * Revision 1.9  1999/03/22 08:54:10  joergr
 * Added/Changed comments.
 *
 * Revision 1.7  1999/03/04 09:43:28  joergr
 * Barten LUT is now be re-created when ambient light value has changed.
 *
 * Revision 1.6  1999/03/03 12:06:24  joergr
 * Added support to specify ambient light value (re: Barten transformation).
 *
 * Revision 1.5  1999/02/23 16:56:06  joergr
 * Added tool to export display curves to a text file.
 *
 * Revision 1.4  1999/02/11 16:50:34  joergr
 * Removed unused parameter / member variable.
 * Renamed file to indicate the use of templates. Moved global functions for
 * cubic spline interpolation to static methods of a separate template class.
 * Added mode ios::nocreate when opening file streams for reading to avoid
 * implicit creation of non-existing files.
 *
 * Revision 1.3  1999/02/09 14:22:31  meichel
 * Removed explicit template parameters from template function calls,
 *   required for Sun CC 4.2
 *
 * Revision 1.2  1999/02/08 13:09:06  joergr
 * Added (debug) warning message when using invalid DISPLAY file names.
 *
 * Revision 1.1  1999/02/03 17:48:37  joergr
 * Added support for calibration according to Barten transformation (incl.
 * a DISPLAY file describing the monitor characteristic).
 *
 *
 */
