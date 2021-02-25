/*
 *
 *  Copyright (C) 1998-2021, OFFIS e.V.
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
 *  Purpose: DicomCIELABFunction (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/diciefn.h"
#include "dcmtk/ofstd/ofstream.h"

/*----------------*
 *  constructors  *
 *----------------*/

DiCIELABFunction::DiCIELABFunction(const char *filename,
                                   const E_DeviceType deviceType,
                                   const signed int ord)
  : DiDisplayFunction(filename, deviceType, ord)
{
    if (!Valid)
    {
        DCMIMGLE_ERROR("invalid DISPLAY file ... ignoring");
    }
}


DiCIELABFunction::DiCIELABFunction(const double *val_tab,             // UNTESTED !!
                                   const unsigned long count,
                                   const Uint16 max,
                                   const E_DeviceType deviceType,
                                   const signed int ord)
  : DiDisplayFunction(val_tab, count, max, deviceType, ord)
{
    if (!Valid)
    {
        DCMIMGLE_ERROR("invalid DISPLAY values ... ignoring");
    }
}


DiCIELABFunction::DiCIELABFunction(const Uint16 *ddl_tab,             // UNTESTED !!
                                   const double *val_tab,
                                   const unsigned long count,
                                   const Uint16 max,
                                   const E_DeviceType deviceType,
                                   const signed int ord)
  : DiDisplayFunction(ddl_tab, val_tab, count, max, deviceType, ord)
{
    if (!Valid)
    {
        DCMIMGLE_ERROR("invalid DISPLAY values ... ignoring");
    }
}


DiCIELABFunction::DiCIELABFunction(const double val_min,
                                   const double val_max,
                                   const unsigned long count,
                                   const E_DeviceType deviceType,
                                   const signed int ord)
  : DiDisplayFunction(val_min, val_max, count, deviceType, ord)
{
    if (!Valid)
    {
        DCMIMGLE_ERROR("invalid DISPLAY values ... ignoring");
    }
}


/*--------------*
 *  destructor  *
 *--------------*/

DiCIELABFunction::~DiCIELABFunction()
{
}


/********************************************************************/


DiDisplayLUT *DiCIELABFunction::getDisplayLUT(unsigned long count)
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
                /* create new CIELAB LUT */
                lut = new DiCIELABLUT(count, MaxDDLValue, DDLValue, tmp_tab, ValueCount,
                    convertODtoLum(MaxValue, OFFalse /*useAmb*/), convertODtoLum(MinValue, OFFalse /*useAmb*/),
                    getMinLuminanceValue(), getMaxLuminanceValue(), AmbientLight, (DeviceType == EDT_Scanner));
                /* delete temporary table */
                delete[] tmp_tab;
            }
        } else {
            /* softcopy: values are already in luminance */
            lut = new DiCIELABLUT(count, MaxDDLValue, DDLValue, LODValue, ValueCount,
                MinValue, MaxValue, -1 /*Lmin*/, -1 /*Lmax*/, AmbientLight, (DeviceType == EDT_Camera));
        }
    }
    return lut;
}


int DiCIELABFunction::writeCurveData(const char *filename,
                                     const OFBool mode)
{
    if ((filename != NULL) && (strlen(filename) > 0))
    {
        STD_NAMESPACE ofstream file(filename);
        if (file)
        {
            const OFBool inverseLUT = (DeviceType == EDT_Scanner) || (DeviceType == EDT_Camera);
            /* comment header */
            file << "# Display function       : CIELAB" << OFendl;
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
            file << "# Interpolation method   : ";
            if (getPolynomialOrder() > 0)
                file << "Curve fitting algorithm with order " << getPolynomialOrder() << OFendl << OFendl;
            else
                file << "Cubic spline interpolation" << OFendl << OFendl;
            /* print headings of the table */
            if (mode)
            {
                file << "# NB: values for CC, CIELAB and PSC";
                if (inverseLUT)
                    file << "'";                // add ' to PSC
                file << " are specified in cd/m^2" << OFendl << OFendl;
                file << "DDL\tCC\tCIELAB\tPSC";
                if (inverseLUT)
                    file << "'";                // add ' to PSC
                file << OFendl;
            } else {
                file << "# NB: values for CC and CIELAB are specified in cd/m^2" << OFendl << OFendl;
                file << "DDL\tCIELAB" << OFendl;
            }
            /* create CIELAB LUT and write curve data to file */
            DiCIELABLUT *lut = NULL;
            if ((DeviceType == EDT_Printer) || (DeviceType == EDT_Scanner))
            {
                /* hardcopy: values are in optical density, first convert them to luminance */
                double *tmp_tab = convertODtoLumTable(LODValue, ValueCount, OFFalse /*useAmb*/);
                if (tmp_tab != NULL)
                {
                    checkMinMaxDensity();
                    lut = new DiCIELABLUT(ValueCount, MaxDDLValue, DDLValue, tmp_tab, ValueCount,
                        convertODtoLum(MaxValue, OFFalse /*useAmb*/), convertODtoLum(MinValue, OFFalse /*useAmb*/),
                        getMinLuminanceValue(), getMaxLuminanceValue(), AmbientLight, inverseLUT, &file, mode);
                    /* delete temporary table */
                    delete[] tmp_tab;
                }
            } else {
                /* softcopy: values are already in luminance */
                lut = new DiCIELABLUT(ValueCount, MaxDDLValue, DDLValue, LODValue, ValueCount,
                    MinValue, MaxValue, -1 /*Lmin*/, -1 /*Lmax*/, AmbientLight, inverseLUT, &file, mode);
            }
            int status = (lut != NULL) && (lut->isValid());
            delete lut;
            return status;
        }
    }
    return 0;
}
