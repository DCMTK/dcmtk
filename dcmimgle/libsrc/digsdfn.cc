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
 *  Purpose: DicomGSDFunction (Source)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:24:27 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/digsdfn.cc,v $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#include "ofconsol.h"
#include "digsdfn.h"
#include "displint.h"

#include <fstream.h>

//BEGIN_EXTERN_C
#include <math.h>
//END_EXTERN_C


/*----------------------------*
 *  constant initializations  *
 *----------------------------*/

const unsigned int DiGSDFunction::GSDFCount = 1023;


/*----------------*
 *  constructors  *
 *----------------*/

DiGSDFunction::DiGSDFunction(const char *filename)
  : DiDisplayFunction(filename),
    JNDMin(0),
    JNDMax(0),
    GSDFValue(NULL),
    GSDFSpline(NULL)
{
    if (Valid)
        Valid = calculateGSDF() && calculateGSDFSpline() && calculateJNDBoundaries();
    if (!Valid)
    {
        if (DicomImageClass::DebugLevel & DicomImageClass::DL_Errors)
            CERR << "ERROR: invalid DISPLAY file ... ignoring !" << endl;
    }
}


DiGSDFunction::DiGSDFunction(const double *lum_tab,             // UNTESTED !!
                             const unsigned long count,
                             const Uint16 max)
  : DiDisplayFunction(lum_tab, count, max),
    JNDMin(0),
    JNDMax(0),
    GSDFValue(NULL),
    GSDFSpline(NULL)
{
    if (Valid)
        Valid = calculateGSDF() && calculateGSDFSpline() && calculateJNDBoundaries();
    if (!Valid)
    {
        if (DicomImageClass::DebugLevel & DicomImageClass::DL_Errors)
            CERR << "ERROR: invalid DISPLAY values ... ignoring !" << endl;
    }
}


DiGSDFunction::DiGSDFunction(const Uint16 *ddl_tab,             // UNTESTED !!
                             const double *lum_tab,
                             const unsigned long count,
                             const Uint16 max)
  : DiDisplayFunction(ddl_tab, lum_tab, count, max),
    JNDMin(0),
    JNDMax(0),
    GSDFValue(NULL),
    GSDFSpline(NULL)
{
    if (Valid)
        Valid = calculateGSDF() && calculateGSDFSpline() && calculateJNDBoundaries();
    if (!Valid)
    {
        if (DicomImageClass::DebugLevel & DicomImageClass::DL_Errors)
            CERR << "ERROR: invalid DISPLAY values ... ignoring !" << endl;
    }
}


DiGSDFunction::DiGSDFunction(const double lum_min,
                             const double lum_max,
                             const unsigned long count)
  : DiDisplayFunction(lum_min, lum_max, count),
    JNDMin(0),
    JNDMax(0),
    GSDFValue(NULL),
    GSDFSpline(NULL)
{
    if (Valid)
        Valid = calculateGSDF() && calculateGSDFSpline() && calculateJNDBoundaries();
    if (!Valid)
    {
        if (DicomImageClass::DebugLevel & DicomImageClass::DL_Errors)
            CERR << "ERROR: invalid DISPLAY values ... ignoring !" << endl;
    }
}


/*--------------*
 *  destructor  *
 *--------------*/

DiGSDFunction::~DiGSDFunction()
{
    delete[] GSDFValue;
    delete[] GSDFSpline;
}


/********************************************************************/


DiDisplayLUT *DiGSDFunction::getDisplayLUT(unsigned long count)
{
    DiDisplayLUT *lut = NULL;
    if (count <= MAX_TABLE_ENTRY_COUNT)
    {
        lut = new DiGSDFLUT(count, MaxDDLValue, DDLValue, LumValue, ValueCount,
            GSDFValue, GSDFSpline, GSDFCount, JNDMin, JNDMax, AmbientLight);
    }
    return lut;
}


int DiGSDFunction::writeCurveData(const char *filename,
                                  const OFBool mode)
{
    if ((filename != NULL) && (strlen(filename) > 0))
    {
        ofstream file(filename);
        if (file)
        {
            file << "# Display function: GSDF (DICOM Part 14)" << endl;
            file << "# Number of DDLs  : " << ValueCount << endl;
            file << "# Luminance range : " << MinLumValue << " - " << MaxLumValue << endl;
            file << "# Ambient light   : " << AmbientLight << endl;
            file << "# JND index range : " << JNDMin << " - " << JNDMax << endl << endl;
            if (mode)
                file << "DDL\tCC\tGSDF\tPSC" << endl;
            else
                file << "DDL\tGSDF" << endl;
            DiGSDFLUT *lut = new DiGSDFLUT(ValueCount, MaxDDLValue, DDLValue, LumValue, ValueCount,
                GSDFValue, GSDFSpline, GSDFCount, JNDMin, JNDMax, AmbientLight, &file, mode);       // write curve data to file
            int status = (lut != NULL) && (lut->isValid());
            delete lut;
            return status;
        }
    }
    return 0;
}


int DiGSDFunction::setAmbientLightValue(const double value)
{
    int status = DiDisplayFunction::setAmbientLightValue(value);
    if (status)
        calculateJNDBoundaries();
    return status;
}


/********************************************************************/


int DiGSDFunction::calculateGSDF()
{
    GSDFValue = new double[GSDFCount];
    if (GSDFValue != NULL)
    {
        
        /*
         *  algorithm taken from DICOM part 14: Grayscale Standard Display Function (GSDF)
         */ 
        
        const double a = -1.3011877;
        const double b = -2.5840191e-2;
        const double c =  8.0242636e-2;
        const double d = -1.0320229e-1;
        const double e =  1.3646699e-1;
        const double f =  2.8745620e-2;
        const double g = -2.5468404e-2;
        const double h = -3.1978977e-3;
        const double k =  1.2992634e-4;
        const double m =  1.3635334e-3;
        register unsigned int i;
        register double ln;
        register double ln2;
        register double ln3;
        register double ln4;
        for (i = 0; i < GSDFCount; i++)
        {
            ln = log(i + 1);
            ln2 = ln * ln;
            ln3 = ln2 * ln;
            ln4 = ln3 * ln;
            GSDFValue[i] = pow(10, (a + c*ln + e*ln2 + g*ln3 + m*ln4) / (1 + b*ln + d*ln2 + f*ln3 + h*ln4 + k*(ln4*ln)));
        }
        return 1;
    }
    return 0;
}


int DiGSDFunction::calculateGSDFSpline()
{
    int status = 0;
    if ((GSDFValue != NULL) && (GSDFCount > 0))
    {
        GSDFSpline = new double[GSDFCount];
        unsigned int *jidx = new unsigned int[GSDFCount];
        if ((GSDFSpline != NULL) && (jidx != NULL))
        {        
            register unsigned int i;
            register unsigned int *p = jidx;
            for (i = 1; i <= GSDFCount; i++)
                *(p++) = i;
            status = DiCubicSpline<unsigned int, double>::Function(jidx, GSDFValue, GSDFCount, GSDFSpline);
        }
        delete[] jidx;
    }
    return status;
}


int DiGSDFunction::calculateJNDBoundaries()
{
    if ((LumValue != NULL) && (ValueCount > 0))
    {
        JNDMin = getJNDIndex(MinLumValue + AmbientLight);
        JNDMax = getJNDIndex(MaxLumValue + AmbientLight);
        return (JNDMin >= 0) && (JNDMax >= 0);
    }
    return 0;
}


double DiGSDFunction::getJNDIndex(const double lum) const
{
    if (lum > 0.0)
    {
        
        /*
         *  algorithm taken from DICOM part 14: Grayscale Standard Display Function (GSDF)
         */ 
        
        const double a = 71.498068;
        const double b = 94.593053;
        const double c = 41.912053;
        const double d =  9.8247004;
        const double e =  0.28175407;
        const double f = -1.1878455;
        const double g = -0.18014349;
        const double h =  0.14710899;
        const double i = -0.017046845;
        double lg10[8];
        lg10[0] = log10(lum);
        register unsigned int j;
        for (j = 0; j < 7; j++)                         // reduce number of multiplications
            lg10[j + 1] = lg10[j] * lg10[0];
        return a + b*lg10[0] + c*lg10[1] + d*lg10[2] + e*lg10[3] + f*lg10[4] + g*lg10[5] + h*lg10[6] + i*lg10[7];
    }
    return -1;
}


/*
 *
 * CVS/RCS Log:
 * $Log: digsdfn.cc,v $
 * Revision 1.9  2000-03-08 16:24:27  meichel
 * Updated copyright header.
 *
 * Revision 1.8  2000/03/06 18:20:35  joergr
 * Moved get-method to base class, renamed method and made method virtual to
 * avoid hiding of methods (reported by Sun CC 4.2).
 *
 * Revision 1.7  2000/03/03 14:09:18  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.6  2000/02/02 11:04:53  joergr
 * Removed space characters before preprocessor directives.
 *
 * Revision 1.5  1999/10/18 17:25:46  joergr
 * Added missing variables in member initialization list (reported by egcs on
 * Solaris with additional compiler options).
 *
 * Revision 1.4  1999/10/18 15:06:25  joergr
 * Enhanced command line tool dcmdspfn (added new options).
 *
 * Revision 1.3  1999/10/18 10:14:27  joergr
 * Moved min/max value determination to display function base class. Now the
 * actual min/max values are also used for GSDFunction (instead of first and
 * last luminance value).
 *
 * Revision 1.2  1999/09/17 13:13:29  joergr
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.1  1999/09/10 08:54:49  joergr
 * Added support for CIELAB display function. Restructured class hierarchy
 * for display functions.
 *
 *
 */
