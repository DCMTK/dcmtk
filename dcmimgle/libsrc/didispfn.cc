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
 *  Purpose: DicomDisplayFunction (Source)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-02-09 14:22:31 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/didispfn.cc,v $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"
#include "ofbmanip.h"

#include "didispfn.h"
#include "displine.h"

#include <fstream.h>

//BEGIN_EXTERN_C
#ifdef HAVE_CTYPE_H
 #include <ctype.h>
#endif
 #include <math.h>
//END_EXTERN_C


/*----------------------------*
 *  constant initializations  *
 *----------------------------*/

const int DiDisplayFunction::MinBits = 8;
const int DiDisplayFunction::MaxBits = 16;
const unsigned int DiDisplayFunction::GSDFCount = 1023;


/*----------------*
 *  constructors  *
 *----------------*/

DiDisplayFunction::DiDisplayFunction(const char *filename)
  : Valid(0),
    ValueCount(0),
    MaxDDLValue(0),
    MaxOutValue(0),
    JNDMin(0),
    JNDMax(0),
    DDLValue(NULL),
    LumValue(NULL),
    GSDFValue(NULL),
    GSDFSpline(NULL)
{
    OFBitmanipTemplate<DiBartenLUT *>::zeroMem(BartenLUT, MAX_NUMBER_OF_TABLES);
    if (readConfigFile(filename) && createSortedTable(DDLValue, LumValue) && interpolateValues())
    {
        if (MaxOutValue == 0)
            MaxOutValue = MaxDDLValue;
        Valid = calculateGSDF() && calculateGSDFSpline() && calculateJNDBoundaries();
    }
}


DiDisplayFunction::DiDisplayFunction(const double *lum_tab,             // UNTESTED !!
                                     const Uint16 count,
                                     const Uint16 max,
                                     const Uint16 out)
  : Valid(0),
    ValueCount(count),
    MaxDDLValue(max),
    MaxOutValue((out == 0) ? max : out),
    JNDMin(0),
    JNDMax(0),
    DDLValue(NULL),
    LumValue(NULL),
    GSDFValue(NULL),
    GSDFSpline(NULL)
{
    OFBitmanipTemplate<DiBartenLUT *>::zeroMem(BartenLUT, MAX_NUMBER_OF_TABLES);
    if ((ValueCount > 0) && (ValueCount == MaxDDLValue + 1))
    {
        DDLValue = new Uint16[ValueCount];
        LumValue = new double[ValueCount];
        if ((DDLValue != NULL) && (LumValue != NULL))
        {
            register Uint16 i;
            for (i = 0; i < ValueCount; i++)
            {
                DDLValue[i] = i;                            // set DDL values
                LumValue[i] = lum_tab[i];                   // copy table
            }
            Valid = calculateGSDF() && calculateGSDFSpline() && calculateJNDBoundaries();
        }
    }
}


DiDisplayFunction::DiDisplayFunction(const Uint16 *ddl_tab,             // UNTESTED !!
                                     const double *lum_tab,
                                     const Uint16 count,
                                     const Uint16 max,
                                     const Uint16 out)
  : Valid(0),
    ValueCount(count),
    MaxDDLValue(max),
    MaxOutValue((out == 0) ? max : out),
    JNDMin(0),
    JNDMax(0),
    DDLValue(NULL),
    LumValue(NULL),
    GSDFValue(NULL),
    GSDFSpline(NULL)
{
    OFBitmanipTemplate<DiBartenLUT *>::zeroMem(BartenLUT, MAX_NUMBER_OF_TABLES);
    if (createSortedTable(ddl_tab, lum_tab) && interpolateValues())
    {
        Valid = calculateGSDF() && calculateGSDFSpline() && calculateJNDBoundaries();
    }
}


/*--------------*
 *  destructor  *
 *--------------*/

DiDisplayFunction::~DiDisplayFunction()
{
    delete[] DDLValue;
    delete[] LumValue;
    delete[] GSDFValue;
    register unsigned int i;
    for (i = 0; i < MAX_NUMBER_OF_TABLES; i++)
        delete BartenLUT[i];
}


/********************************************************************/


const DiBartenLUT *DiDisplayFunction::getBartenLUT(const int bits,
                                                   unsigned long count)
{
    if (Valid && (bits >= MinBits) && (bits <= MaxBits))
    {
        const int idx = bits - MinBits;
        if (count == 0)
            count = DicomImageClass::maxval(bits, 0);
        if ((BartenLUT[idx] != NULL) && (count != BartenLUT[idx]->getCount()))
        {
            delete BartenLUT[idx];
            BartenLUT[idx] = NULL;
        }
        if (BartenLUT[idx] == NULL)                             // first calculation of this LUT
        {
			if (count <= MAX_TABLE_ENTRY_COUNT)
				BartenLUT[idx] = new DiBartenLUT(count, DDLValue, LumValue, ValueCount, GSDFValue, GSDFSpline, GSDFCount, JNDMin, JNDMax);
        }
        return BartenLUT[idx];
    }
    return NULL;
}


int DiDisplayFunction::deleteBartenLUT(const int bits)
{
    if (bits == 0)                          // delete all LUTs
    {
        register int i;
        for (i = 0; i < MAX_NUMBER_OF_TABLES; i++)
        {
            delete BartenLUT[i];
            BartenLUT[i] = NULL;
        }
        return 1;
    }
    else if ((bits >= MinBits) && (bits <= MaxBits))
    {
        const int idx = bits - MinBits;
        if (BartenLUT[idx] != NULL)
        {
            delete BartenLUT[idx];
            BartenLUT[idx] = NULL;
            return 1;
        }
        return 2;
    }
    return 0;
}


/********************************************************************/


int DiDisplayFunction::readConfigFile(const char *filename)
{
    if ((filename != NULL) && (strlen(filename) > 0))
    {
        ifstream file(filename);
        if (file)
        {
            char c;
            while (file.get(c))
            {
                if (c == '#')
                {
                    while (file.get(c) && (c != '\n') && (c != '\r'));      // skip comments
                } 
                else if (!isspace(c))                                       // skip whitespaces
                {
                    file.putback(c);
                    if (MaxDDLValue == 0)                                   // read maxvalue
                    {
                        char str[4];
                        file >> str;
                        if (strcmp(str, "max") == 0)
                        {
                            file >> MaxDDLValue;
                            if (MaxDDLValue > 0)
                            {
                                DDLValue = new Uint16[MaxDDLValue + 1];
                                LumValue = new double[MaxDDLValue + 1];
                                if ((DDLValue == NULL) || (LumValue == NULL))
                                    return 0;
                            } else {
                                if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Errors)
                                    cerr << "ERROR: invalid or missing value for maximum DDL value in DISPLAY file !" << endl;
                                return 0;                                   // abort
                            }
                        } else {
                            if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Errors)
                                cerr << "ERROR: missing keyword 'max' for maximum DDL value in DISPLAY file !" << endl;
                            return 0;                                       // abort
                        }
                    }
/*
                    else if ((MaxOutValue == 0) && (c == 'o'))              // read outvalue: optional
                    {
                        char str[4];
                        file >> str;
                        if (strcmp(str, "out") == 0)
                            file >> MaxOutValue;
                        else
                        {
                            if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Errors)
                                cerr << "ERROR: error while parsing 'out' entry in DISPLAY file !" << endl;
                            return 0;                                       // abort
                        }
                    }
*/
                    else {
                        if (ValueCount <= MaxDDLValue)
                        {
                            file >> DDLValue[ValueCount];                   // read DDL value
                            file >> LumValue[ValueCount];                   // read luminance value
                            if (file.fail())
                            {
                                if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
                                    cerr << "WARNING: missing luminance value in DISPLAY file ... ignoring last entry !" << endl;
                            }
                            else if (DDLValue[ValueCount] > MaxDDLValue)
                            {
                                if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
                                {
                                    cerr << "WARNING: DDL value (" << DDLValue[ValueCount] << ") exceeds maximum value (";
                                    cerr << MaxDDLValue << ") in DISPLAY file ..." << endl << "         ... ignoring value !" << endl;
                                }
                            } else
                                ValueCount++;
                        } else {
                            if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
                                cerr << "WARNING: too many values in DISPLAY file ... ignoring last line(s) !" << endl;
                            return 2;
                        }
                    }
                }
            }
            return ((MaxDDLValue > 0) && (ValueCount > 0) && (DDLValue != NULL) && (LumValue != NULL));
        } else {
            if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
                cerr << "WARNING: can't open DISPLAY file ... ignoring !" << endl;
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
            register Uint16 i;
            for (i = 0; i < ValueCount; i++)
            {
                if (ddl_tab[i] <= MaxDDLValue)                                      // calculate sort table
                    sort_tab[ddl_tab[i]] = i;
            }
            ValueCount = 0;
            for (i = 0; i < MaxDDLValue + 1; i++)                                   // sort ascending
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
                if (DicomImageClass::DebugLevel >= DicomImageClass::DL_Warnings)
                    cerr << "WARNING: luminance values (ordered by DDLs) don't ascend monotonous !" << endl;
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
    if (ValueCount <= MaxDDLValue)                                      // interpolation not necessary
    {
        int status = 0;
        double *spline = new double[ValueCount];
        if ((spline != NULL) && (CubicSplineFunction(DDLValue, LumValue, ValueCount, spline, 1.0e30, 1.0e30)))
        {
            const Uint16 count = ValueCount;
            Uint16 *old_ddl = DDLValue;
            double *old_lum = LumValue;
            ValueCount = MaxDDLValue + 1;
            DDLValue = new Uint16[ValueCount];
            LumValue = new double[ValueCount];
            if ((DDLValue != NULL) && (LumValue != NULL))
            {
                register Uint16 i;
                for (i = 0; i < ValueCount; i++)                        // set all DDL values, from 0 to max
                    DDLValue[i] = i;
                status = CubicSplineInterpolation(old_ddl, old_lum, spline, count, DDLValue, LumValue, ValueCount);
            }
            delete[] old_ddl;
            delete[] old_lum;
        }
        delete[] spline;
        return status;
    }
    return 2;
}


int DiDisplayFunction::calculateGSDF()
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


int DiDisplayFunction::calculateGSDFSpline()
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
            status = CubicSplineFunction(jidx, GSDFValue, GSDFCount, GSDFSpline, 1.0e30, 1.0e30);
        }
        delete[] jidx;
    }
    return status;
}


int DiDisplayFunction::calculateJNDBoundaries()
{
    if ((LumValue != NULL) && (ValueCount > 0))
    {
        JNDMin = getJNDIndex(LumValue[0]);
        JNDMax = getJNDIndex(LumValue[ValueCount - 1]);
        return (JNDMin >= 0) && (JNDMax >= 0);
    }
    return 0;
}


double DiDisplayFunction::getJNDIndex(const double lum) const
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
 * $Log: didispfn.cc,v $
 * Revision 1.3  1999-02-09 14:22:31  meichel
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
