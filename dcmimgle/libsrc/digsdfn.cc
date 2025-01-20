/*
 *
 *  Copyright (C) 1999-2024, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomGSDFunction (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/digsdfn.h"
#include "dcmtk/dcmimgle/displint.h"
#include "dcmtk/dcmimgle/digsdlut.h"

#include "dcmtk/ofstd/ofstream.h"

#include <cmath>
#include <cstring>

/*----------------------------*
 *  constant initializations  *
 *----------------------------*/

const unsigned int DiGSDFunction::GSDFCount = 1023;


/*----------------*
 *  constructors  *
 *----------------*/

DiGSDFunction::DiGSDFunction(const char *filename,
                             const E_DeviceType deviceType,
                             const signed int ord)
  : DiDisplayFunction(filename, deviceType, ord),
    JNDMin(0),
    JNDMax(0),
    GSDFValue(NULL),
    GSDFSpline(NULL)
{
    if (Valid)
        Valid = calculateGSDF() && calculateGSDFSpline() && calculateJNDBoundaries();
    if (!Valid)
    {
        DCMIMGLE_ERROR("invalid DISPLAY file ... ignoring");
    }
}


DiGSDFunction::DiGSDFunction(const double *val_tab,             // UNTESTED !!
                             const unsigned long count,
                             const Uint16 max,
                             const E_DeviceType deviceType,
                             const signed int ord)
  : DiDisplayFunction(val_tab, count, max, deviceType, ord),
    JNDMin(0),
    JNDMax(0),
    GSDFValue(NULL),
    GSDFSpline(NULL)
{
    if (Valid)
        Valid = calculateGSDF() && calculateGSDFSpline() && calculateJNDBoundaries();
    if (!Valid)
    {
        DCMIMGLE_ERROR("invalid DISPLAY values ... ignoring");
    }
}


DiGSDFunction::DiGSDFunction(const Uint16 *ddl_tab,             // UNTESTED !!
                             const double *val_tab,
                             const unsigned long count,
                             const Uint16 max,
                             const E_DeviceType deviceType,
                             const signed int ord)
  : DiDisplayFunction(ddl_tab, val_tab, count, max, deviceType, ord),
    JNDMin(0),
    JNDMax(0),
    GSDFValue(NULL),
    GSDFSpline(NULL)
{
    if (Valid)
        Valid = calculateGSDF() && calculateGSDFSpline() && calculateJNDBoundaries();
    if (!Valid)
    {
        DCMIMGLE_ERROR("invalid DISPLAY values ... ignoring");
    }
}


DiGSDFunction::DiGSDFunction(const double val_min,
                             const double val_max,
                             const unsigned long count,
                             const E_DeviceType deviceType,
                             const signed int ord)
  : DiDisplayFunction(val_min, val_max, count, deviceType, ord),
    JNDMin(0),
    JNDMax(0),
    GSDFValue(NULL),
    GSDFSpline(NULL)
{
    if (Valid)
        Valid = calculateGSDF() && calculateGSDFSpline() && calculateJNDBoundaries();
    if (!Valid)
    {
        DCMIMGLE_ERROR("invalid DISPLAY values ... ignoring");
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
        if ((DeviceType == EDT_Printer) || (DeviceType == EDT_Scanner))
        {
            /* hardcopy: values are in optical density, first convert them to luminance */
            double *tmp_tab = convertODtoLumTable(LODValue, ValueCount, OFFalse /*useAmb*/);
            if (tmp_tab != NULL)
            {
                checkMinMaxDensity();
                /* create new GSDF LUT */
                lut = new DiGSDFLUT(count, MaxDDLValue, DDLValue, tmp_tab, ValueCount,
                    GSDFValue, GSDFSpline, GSDFCount, JNDMin, JNDMax, getMinLuminanceValue(),
                    getMaxLuminanceValue(), AmbientLight, Illumination, (DeviceType == EDT_Scanner));
                /* delete temporary table */
                delete[] tmp_tab;
            }
        } else {
            /* softcopy: values are already in luminance */
            lut = new DiGSDFLUT(count, MaxDDLValue, DDLValue, LODValue, ValueCount,
                GSDFValue, GSDFSpline, GSDFCount, JNDMin, JNDMax, -1 /*Lmin*/, -1 /*Lmax*/,
                AmbientLight, Illumination, (DeviceType == EDT_Camera));
        }
    }
    return lut;
}


int DiGSDFunction::writeCurveData(const char *filename,
                                  const OFBool mode)
{
    if ((filename != NULL) && (strlen(filename) > 0))
    {
        STD_NAMESPACE ofstream file(filename);
        if (file)
        {
            const OFBool inverseLUT = (DeviceType == EDT_Scanner) || (DeviceType == EDT_Camera);
            /* comment header */
            file << "# Display function       : GSDF (DICOM Part 14)" << OFendl;
            if (DeviceType == EDT_Printer)
                file << "# Type of output device  : Printer (hardcopy)" << OFendl;
            else if (DeviceType == EDT_Scanner)
                file << "# Type of output device  : Scanner (hardcopy)" << OFendl;
            else if (DeviceType == EDT_Camera)
                file << "# Type of output device  : Camera (softcopy)" << OFendl;
            else
                file << "# Type of output device  : Monitor (softcopy)" << OFendl;
            file << "# Digital driving levels : " << ValueCount << OFendl;
            if ((DeviceType == EDT_Printer) || (DeviceType == EDT_Scanner))
                file << "# Illumination  [cd/m^2] : " << Illumination << OFendl;
            file << "# Ambient light [cd/m^2] : " << AmbientLight << OFendl;
            if ((DeviceType == EDT_Printer) || (DeviceType == EDT_Scanner))
            {
                const double min_lum = getMinLuminanceValue();
                const double max_lum = getMaxLuminanceValue();
                file << "# Luminance w/o [cd/m^2] : " << convertODtoLum(MaxValue, OFFalse /*useAmb*/) << " - "
                                                      << convertODtoLum(MinValue, OFFalse /*useAmb*/);
                if ((min_lum >= 0) || (max_lum >= 0))
                {
                    file << " (Lmin = ";
                    if (min_lum >= 0)
                        file << min_lum;
                    else
                        file << "n/s";
                    file << ", Lmax = ";
                    if (max_lum >= 0)
                        file << max_lum;
                    else
                        file << "n/s";
                    file << ")";
                }
                file << OFendl;
                file << "# Optical density   [OD] : " << MinValue << " - " << MaxValue;
                if ((MinDensity >= 0) || (MaxDensity >= 0))
                {
                    file << " (Dmin = ";
                    if (MinDensity >= 0)
                        file << MinDensity;
                    else
                        file << "n/s";
                    file << ", Dmax = ";
                    if (MaxDensity >= 0)
                        file << MaxDensity;
                    else
                        file << "n/s";
                    file << ")";
                }
                file << OFendl;
            } else
                file << "# Luminance w/o [cd/m^2] : " << MinValue << " - " << MaxValue << OFendl;
            file << "# Barten JND index range : " << JNDMin << " - " << JNDMax << " (" << (JNDMax - JNDMin) << ")" << OFendl;
            file << "# Interpolation method   : ";
            if (getPolynomialOrder() > 0)
                file << "Curve fitting algorithm with order " << getPolynomialOrder() << OFendl << OFendl;
            else
                file << "Cubic spline interpolation" << OFendl << OFendl;
            /* print headings of the table */
            if (mode)
            {
                file << "# NB: values for CC, GSDF and PSC";
                if (inverseLUT)
                    file << "'";            // add ' to PSC
                file << " are specified in cd/m^2" << OFendl << OFendl;
                file << "DDL\tCC\tGSDF\tPSC";
                if (inverseLUT)
                    file << "'";            // add ' to PSC
                file << OFendl;
            } else {
                file << "# NB: values for CC and GSDF are specified in cd/m^2" << OFendl << OFendl;
                file << "DDL\tGSDF" << OFendl;
            }
            /* create GSDF LUT and write curve data to file */
            DiGSDFLUT *lut = NULL;
            if ((DeviceType == EDT_Printer) || (DeviceType == EDT_Scanner))
            {
                /* hardcopy: values are in optical density, first convert them to luminance */
                double *tmp_tab = convertODtoLumTable(LODValue, ValueCount, OFFalse /*useAmb*/);
                if (tmp_tab != NULL)
                {
                    checkMinMaxDensity();
                    lut = new DiGSDFLUT(ValueCount, MaxDDLValue, DDLValue, tmp_tab, ValueCount,
                        GSDFValue, GSDFSpline, GSDFCount, JNDMin, JNDMax, getMinLuminanceValue(),
                        getMaxLuminanceValue(), AmbientLight, Illumination, inverseLUT, &file, mode);
                    /* delete temporary table */
                    delete[] tmp_tab;
                }
            } else {
                /* softcopy: values are already in luminance */
                lut = new DiGSDFLUT(ValueCount, MaxDDLValue, DDLValue, LODValue, ValueCount,
                    GSDFValue, GSDFSpline, GSDFCount, JNDMin, JNDMax, -1 /*Lmin*/, -1 /*Lmax*/,
                    AmbientLight, Illumination, inverseLUT, &file, mode);
            }
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
        Valid = calculateJNDBoundaries();       // check validity
    return status;
}


int DiGSDFunction::setIlluminationValue(const double value)
{
    int status = DiDisplayFunction::setIlluminationValue(value);
    if (status && ((DeviceType == EDT_Printer) || (DeviceType == EDT_Scanner)))
        Valid = calculateJNDBoundaries();       // check validity
    return status;
}


int DiGSDFunction::setMinDensityValue(const double value)
{
    int status = DiDisplayFunction::setMinDensityValue(value);
    if (status && (DeviceType == EDT_Printer))
        Valid = calculateJNDBoundaries();       // check validity
    return status;
}


int DiGSDFunction::setMaxDensityValue(const double value)
{
    int status = DiDisplayFunction::setMaxDensityValue(value);
    if (status && (DeviceType == EDT_Printer))
        Valid = calculateJNDBoundaries();       // check validity
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
        unsigned int i;
        double ln;
        double ln2;
        double ln3;
        double ln4;
        for (i = 0; i < GSDFCount; ++i)
        {
            ln = log(OFstatic_cast(double, i + 1));
            ln2 = ln * ln;
            ln3 = ln2 * ln;
            ln4 = ln3 * ln;
            GSDFValue[i] = pow(OFstatic_cast(double, 10), (a + c*ln + e*ln2 + g*ln3 + m*ln4) / (1 + b*ln + d*ln2 + f*ln3 + h*ln4 + k*(ln4*ln)));
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
            unsigned int i;
            unsigned int *p = jidx;
            for (i = 1; i <= GSDFCount; ++i)
                *(p++) = i;
            status = DiCubicSpline<unsigned int, double>::Function(jidx, GSDFValue, GSDFCount, GSDFSpline);
        }
        delete[] jidx;
    }
    return status;
}


int DiGSDFunction::calculateJNDBoundaries()
{
    if ((LODValue != NULL) && (ValueCount > 0))
    {
        if ((DeviceType == EDT_Printer) || (DeviceType == EDT_Scanner))
        {
            /* hardcopy device (printer/scanner), values are in OD */
            if (MaxDensity < 0)
                JNDMin = getJNDIndex(convertODtoLum(MaxValue));
            else // max density specified
                JNDMin = getJNDIndex(convertODtoLum(MaxDensity));
            if (MinDensity < 0)
                JNDMax = getJNDIndex(convertODtoLum(MinValue));
            else // min density specified
                JNDMax = getJNDIndex(convertODtoLum(MinDensity));
        } else {
            /* softcopy device (monitor/camera), values are in cd/m^2 */
            JNDMin = getJNDIndex(MinValue + AmbientLight);
            JNDMax = getJNDIndex(MaxValue + AmbientLight);
        }
        return (JNDMin >= 0) && (JNDMax >= 0);
    }
    return 0;
}


double DiGSDFunction::getJNDIndex(const double lum)
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
        unsigned int j;
        for (j = 0; j < 7; ++j)                         // reduce number of multiplications
            lg10[j + 1] = lg10[j] * lg10[0];
        return a + b*lg10[0] + c*lg10[1] + d*lg10[2] + e*lg10[3] + f*lg10[4] + g*lg10[5] + h*lg10[6] + i*lg10[7];
    }
    return -1;
}
