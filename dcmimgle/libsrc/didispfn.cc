/*
 *
 *  Copyright (C) 1999-2019, OFFIS e.V.
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
 *  Purpose: DicomDisplayFunction (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofbmanip.h"
#include "dcmtk/dcmimgle/didispfn.h"
#include "dcmtk/dcmimgle/displint.h"
#include "dcmtk/dcmimgle/dicrvfit.h"
#include "dcmtk/dcmimgle/didislut.h"
#include "dcmtk/ofstd/ofstream.h"

#define INCLUDE_CCTYPE
#define INCLUDE_CMATH
#include "dcmtk/ofstd/ofstdinc.h"


/*----------------------------*
 *  constant initializations  *
 *----------------------------*/

const int DiDisplayFunction::MinBits = 2;
const int DiDisplayFunction::MaxBits = 16;


/*----------------*
 *  constructors  *
 *----------------*/

DiDisplayFunction::DiDisplayFunction(const char *filename,
                                     const E_DeviceType deviceType,
                                     const signed int ord)
  : Valid(0),
    DeviceType(deviceType),
    ValueCount(0),
    MaxDDLValue(0),
    Order(0),
    AmbientLight(0),
    Illumination(0),
    MinDensity(-1),
    MaxDensity(-1),
    DDLValue(NULL),
    LODValue(NULL),
    MinValue(0),
    MaxValue(0)
{
    OFBitmanipTemplate<DiDisplayLUT *>::zeroMem(LookupTable, MAX_NUMBER_OF_TABLES);
    if (readConfigFile(filename))
    {
        /* overwrite file setting for polynomial order */
        if (ord >= 0)
            Order = ord;
        Valid = createSortedTable(DDLValue, LODValue) && calculateMinMax() && interpolateValues();
    }
}


DiDisplayFunction::DiDisplayFunction(const double *val_tab,             // UNTESTED !!
                                     const unsigned long count,
                                     const Uint16 max,
                                     const E_DeviceType deviceType,
                                     const signed int ord)
  : Valid(0),
    DeviceType(deviceType),
    ValueCount(count),
    MaxDDLValue(max),
    Order(ord),
    AmbientLight(0),
    Illumination(0),
    MinDensity(-1),
    MaxDensity(-1),
    DDLValue(NULL),
    LODValue(NULL),
    MinValue(0),
    MaxValue(0)
{
    OFBitmanipTemplate<DiDisplayLUT *>::zeroMem(LookupTable, MAX_NUMBER_OF_TABLES);
    /* check number of entries */
    if ((ValueCount > 0) && (ValueCount == OFstatic_cast(unsigned long, MaxDDLValue) + 1))
    {
        /* copy value table */
        DDLValue = new Uint16[ValueCount];
        LODValue = new double[ValueCount];
        if ((DDLValue != NULL) && (LODValue != NULL))
        {
            unsigned int i;
            for (i = 0; i <= MaxDDLValue; ++i)
            {
                DDLValue[i] = OFstatic_cast(Uint16, i);     // set DDL values
                LODValue[i] = val_tab[i];                   // copy table
            }
            Valid = calculateMinMax();
        }
    }
}


DiDisplayFunction::DiDisplayFunction(const Uint16 *ddl_tab,             // UNTESTED !!
                                     const double *val_tab,
                                     const unsigned long count,
                                     const Uint16 max,
                                     const E_DeviceType deviceType,
                                     const signed int ord)
  : Valid(0),
    DeviceType(deviceType),
    ValueCount(count),
    MaxDDLValue(max),
    Order(ord),
    AmbientLight(0),
    Illumination(0),
    MinDensity(-1),
    MaxDensity(-1),
    DDLValue(NULL),
    LODValue(NULL),
    MinValue(0),
    MaxValue(0)
{
    OFBitmanipTemplate<DiDisplayLUT *>::zeroMem(LookupTable, MAX_NUMBER_OF_TABLES);
    /* check for maximum number of entries */
    if (ValueCount <= MAX_TABLE_ENTRY_COUNT)
        Valid = createSortedTable(ddl_tab, val_tab) && calculateMinMax() && interpolateValues();
}


DiDisplayFunction::DiDisplayFunction(const double val_min,
                                     const double val_max,
                                     const unsigned long count,
                                     const E_DeviceType deviceType,
                                     const signed int ord)
  : Valid(0),
    DeviceType(deviceType),
    ValueCount(count),
    MaxDDLValue(0),
    Order(ord),
    AmbientLight(0),
    Illumination(0),
    MinDensity(-1),
    MaxDensity(-1),
    DDLValue(NULL),
    LODValue(NULL),
    MinValue(val_min),
    MaxValue(val_max)
{
    OFBitmanipTemplate<DiDisplayLUT *>::zeroMem(LookupTable, MAX_NUMBER_OF_TABLES);
    /* check parameters */
    if ((ValueCount > 1) && (ValueCount <= MAX_TABLE_ENTRY_COUNT) && (MinValue < MaxValue))
    {
        /* create value tables */
        MaxDDLValue = OFstatic_cast(Uint16, count - 1);
        DDLValue = new Uint16[ValueCount];
        LODValue = new double[ValueCount];
        if ((DDLValue != NULL) && (LODValue != NULL))
        {
            Uint16 i;
            const double min = ((DeviceType == EDT_Printer) || (DeviceType == EDT_Scanner)) ? val_max : val_min;
            const double max = ((DeviceType == EDT_Printer) || (DeviceType == EDT_Scanner)) ? val_min : val_max;
            const double val = (max - min) / OFstatic_cast(double, MaxDDLValue);
            DDLValue[0] = 0;
            LODValue[0] = min;
            for (i = 1; i < MaxDDLValue; ++i)
            {
                DDLValue[i] = i;                            // set DDL values
                LODValue[i] = LODValue[i - 1] + val;        // compute luminance/OD value
            }
            DDLValue[MaxDDLValue] = MaxDDLValue;
            LODValue[MaxDDLValue] = max;
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
    delete[] LODValue;
    int i;
    for (i = 0; i < MAX_NUMBER_OF_TABLES; ++i)
        delete LookupTable[i];
}


/********************************************************************/


double DiDisplayFunction::getValueforDDL(const Uint16 ddl) const
{
    if ((LODValue != NULL) && (ddl < ValueCount))
        return LODValue[ddl];
    return -1;
}


Uint16 DiDisplayFunction::getDDLforValue(const double value) const
{
    if ((LODValue != NULL) && (ValueCount > 0))
    {
        unsigned long j = 0;
        /* search for closest index, assuming monotony */
        if ((DeviceType == EDT_Printer) || (DeviceType == EDT_Scanner))
        {
            /* hardcopy device: descending values */
            while ((j + 1 < ValueCount) && (LODValue[j] > value))
                ++j;
        } else {
            /* softcopy device: ascending values */
            while ((j + 1 < ValueCount) && (LODValue[j] < value))
                ++j;
        }
        /* check which value is closer, the upper or the lower */
        if ((j > 0) && (fabs(LODValue[j - 1] - value) < fabs(LODValue[j] - value)))
            --j;
        return OFstatic_cast(Uint16, j);
    }
    return 0;
}


const DiDisplayLUT *DiDisplayFunction::getLookupTable(const int bits,
                                                      unsigned long count)
{
    if (Valid && (bits >= MinBits) && (bits <= MaxBits))
    {
        const int idx = bits - MinBits;
        /* automatically compute number of entries */
        if (count == 0)
            count = DicomImageClass::maxval(bits, 0);
        /* check whether existing LUT is still valid */
        if ((LookupTable[idx] != NULL) && ((count != LookupTable[idx]->getCount()) ||
            (AmbientLight != LookupTable[idx]->getAmbientLightValue()) ||
            (Illumination != LookupTable[idx]->getIlluminationValue())))
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
    if (bits == 0)
    {
        /* delete all LUTs */
        int i;
        for (i = 0; i < MAX_NUMBER_OF_TABLES; ++i)
        {
            delete LookupTable[i];
            LookupTable[i] = NULL;
        }
        return 1;
    }
    else if ((bits >= MinBits) && (bits <= MaxBits))
    {
        /* delete the specified LUT */
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


int DiDisplayFunction::setIlluminationValue(const double value)
{
    if (value >= 0)
    {
        Illumination = value;
        return 1;
    }
    return 0;
}


int DiDisplayFunction::setMinDensityValue(const double value)
{
    MinDensity = value;
    return (value < 0) ? 2 : 1;
}


int DiDisplayFunction::setMaxDensityValue(const double value)
{
    MaxDensity = value;
    return (value < 0) ? 2 : 1;
}


/********************************************************************/


int DiDisplayFunction::readConfigFile(const char *filename)
{
    if ((filename != NULL) && (strlen(filename) > 0))
    {
#if defined(HAVE_IOS_NOCREATE) && !defined(HAVE_CXX11)
        STD_NAMESPACE ifstream file(filename, STD_NAMESPACE ios::in | STD_NAMESPACE ios::nocreate);
#else
        STD_NAMESPACE ifstream file(filename, STD_NAMESPACE ios::in);
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
                else if (!isspace(OFstatic_cast(unsigned char, c)))         // skip whitespaces
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
                                DDLValue = new Uint16[OFstatic_cast(unsigned long, MaxDDLValue) + 1];
                                LODValue = new double[OFstatic_cast(unsigned long, MaxDDLValue) + 1];
                                if ((DDLValue == NULL) || (LODValue == NULL))
                                    return 0;
                            } else {
                                DCMIMGLE_ERROR("invalid or missing value for maximum DDL value in DISPLAY file");
                                return 0;                                   // abort
                            }
                        } else {
                            DCMIMGLE_ERROR("missing keyword 'max' for maximum DDL value in DISPLAY file");
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
                                DCMIMGLE_WARN("invalid value for ambient light in DISPLAY file ... ignoring");
                                AmbientLight = 0;
                            }
                        } else {
                            DCMIMGLE_ERROR("invalid DISPLAY file ... ignoring");
                            return 0;                                       // abort
                        }
                    }
                    else if ((Illumination == 0.0) && (c == 'l'))           // read ambient light value (optional)
                    {
                        char str[4];
                        file.get(str, sizeof(str));
                        if (strcmp(str, "lum") == 0)                        // check for key word: lum
                        {
                            file >> Illumination;
                            if (Illumination < 0)
                            {
                                DCMIMGLE_WARN("invalid value for illumination in DISPLAY file ... ignoring");
                                Illumination = 0;
                            }
                        } else {
                            DCMIMGLE_ERROR("invalid DISPLAY file ... ignoring");
                            return 0;                                       // abort
                        }
                    }
                    else if ((Order == 0) && (c == 'o'))                    // read polynomial order (optional)
                    {
                        char str[4];
                        file.get(str, sizeof(str));
                        if (strcmp(str, "ord") == 0)                        // check for key word: ord
                        {
                            file >> Order;
                            if (Order < 0)
                            {
                                DCMIMGLE_WARN("invalid value for polynomial order in DISPLAY file ... ignoring");
                                Order = 0;
                            }
                        } else {
                            DCMIMGLE_ERROR("invalid DISPLAY file ... ignoring");
                            return 0;                                       // abort
                        }
                    } else {
                        if (ValueCount <= OFstatic_cast(unsigned long, MaxDDLValue))
                        {
                            file >> DDLValue[ValueCount];                   // read DDL value
                            file >> LODValue[ValueCount];                   // read luminance/OD value
                            if (file.fail())
                            {
                                DCMIMGLE_WARN("missing luminance/OD value in DISPLAY file ... ignoring last entry");
                            }
                            else if (DDLValue[ValueCount] > MaxDDLValue)
                            {
                                DCMIMGLE_WARN("DDL value (" << DDLValue[ValueCount] << ") exceeds maximum value ("
                                    << MaxDDLValue << ") in DISPLAY file ... ignoring value");
                            } else
                                ++ValueCount;
                        } else {
                            DCMIMGLE_WARN("too many values in DISPLAY file ... ignoring last line(s)");
                            return 2;
                        }
                    }
                }
            }
            if ((MaxDDLValue > 0) && (ValueCount > 0))
                return ((DDLValue != NULL) && (LODValue != NULL));
            else {
                DCMIMGLE_WARN("invalid DISPLAY file ... ignoring");
            }
        } else {
            DCMIMGLE_WARN("can't open DISPLAY file ... ignoring");
        }
    }
    return 0;
}


int DiDisplayFunction::createSortedTable(const Uint16 *ddl_tab,
                                         const double *val_tab)
{
    int status = 0;
    Uint16 *old_ddl = DDLValue;
    double *old_val = LODValue;
    if ((ValueCount > 0) && (ddl_tab != NULL) && (val_tab != NULL))
    {
        const unsigned long count = OFstatic_cast(unsigned long, MaxDDLValue) + 1;
        DDLValue = new Uint16[ValueCount];
        LODValue = new double[ValueCount];
        Sint32 *sort_tab = new Sint32[count];                                       // auxilliary array (temporary)
        if ((DDLValue != NULL) && (LODValue != NULL) && (sort_tab != NULL))
        {
            OFBitmanipTemplate<Sint32>::setMem(sort_tab, -1, count);                // initialize array
            unsigned long i;
            for (i = 0; i < ValueCount; ++i)
            {
                if (ddl_tab[i] <= MaxDDLValue)                                      // calculate sort table
                    sort_tab[ddl_tab[i]] = i;
            }
            ValueCount = 0;
            for (i = 0; i <= MaxDDLValue; ++i)                                      // sort ascending
            {
                if (sort_tab[i] >= 0)
                {
                    DDLValue[ValueCount] = ddl_tab[sort_tab[i]];
                    LODValue[ValueCount] = (val_tab[sort_tab[i]] > 0) ? val_tab[sort_tab[i]] : 0;
                    ++ValueCount;                                                   // re-count to ignore values exceeding max
                }
            }
            i = 1;
            if ((DeviceType == EDT_Printer) || (DeviceType == EDT_Scanner))
            {
                /* hardcopy device: check for monotonous descending OD values */
                while ((i < ValueCount) && (LODValue[i - 1] >= LODValue[i]))
                    ++i;
                if (i < ValueCount)
                {
                    DCMIMGLE_WARN("OD values (ordered by DDLs) don't descend monotonously");
                }
            } else {
                /* softcopy device: check for monotonous ascending luminance values */
                while ((i < ValueCount) && (LODValue[i - 1] <= LODValue[i]))
                    ++i;
                if (i < ValueCount)
                {
                    DCMIMGLE_WARN("luminance values (ordered by DDLs) don't ascend monotonously");
                }
            }
            status = (ValueCount > 0);
        }
        delete[] sort_tab;
    }
    delete[] old_ddl;
    delete[] old_val;
    return status;
}


int DiDisplayFunction::interpolateValues()
{
    if (ValueCount <= OFstatic_cast(unsigned long, MaxDDLValue))        // interpolation necessary ?
    {
        int status = 0;
        if (Order > 0)
        {
            /* use polynomial curve fitting */
            double *coeff = new double[Order + 1];
            /* compute coefficients */
            if ((coeff != NULL) && DiCurveFitting<Uint16, double>::calculateCoefficients(DDLValue, LODValue,
                    OFstatic_cast(unsigned int, ValueCount), Order, coeff))
            {
                /* delete old data arrays */
                delete[] DDLValue;
                delete[] LODValue;
                /* create new data arrays */
                ValueCount = OFstatic_cast(unsigned long, MaxDDLValue) + 1;
                DDLValue = new Uint16[ValueCount];
                LODValue = new double[ValueCount];
                if ((DDLValue != NULL) && (LODValue != NULL))
                {
                    /* set x values linearly */
                    unsigned int i;
                    for (i = 0; i <= MaxDDLValue; ++i)
                        DDLValue[i] = OFstatic_cast(Uint16, i);
                    /* compute new y values */
                    status = DiCurveFitting<Uint16, double>::calculateValues(0, MaxDDLValue, LODValue,
                        OFstatic_cast(unsigned int, ValueCount), Order, coeff);
                }
            }
            delete[] coeff;
        } else {
            /* use cubic spline interpolation */
            double *spline = new double[ValueCount];
            if ((spline != NULL) &&
                DiCubicSpline<Uint16, double>::Function(DDLValue, LODValue, OFstatic_cast(unsigned int, ValueCount), spline))
            {
                /* save old values */
                const unsigned long count = ValueCount;
                Uint16 *old_ddl = DDLValue;
                double *old_val = LODValue;
                /* create new data arrays */
                ValueCount = OFstatic_cast(unsigned long, MaxDDLValue) + 1;
                DDLValue = new Uint16[ValueCount];
                LODValue = new double[ValueCount];
                if ((DDLValue != NULL) && (LODValue != NULL))
                {
                    /* set x values linearly */
                    unsigned int i;
                    for (i = 0; i <= MaxDDLValue; ++i)
                        DDLValue[i] = OFstatic_cast(Uint16, i);
                    /* compute new y values */
                    status = DiCubicSpline<Uint16, double>::Interpolation(old_ddl, old_val, spline,
                        OFstatic_cast(unsigned int, count), DDLValue, LODValue, OFstatic_cast(unsigned int, ValueCount));
                }
                /* delete old data arrays */
                delete[] old_ddl;
                delete[] old_val;
            }
            delete[] spline;
        }
        return status;
    }
    return 2;
}


int DiDisplayFunction::calculateMinMax()
{
    if ((LODValue != NULL) && (ValueCount > 0))
    {
        MinValue = LODValue[0];
        MaxValue = LODValue[0];
        unsigned long i;
        for (i = 1; i < ValueCount; ++i)
        {
            if (LODValue[i] < MinValue)
                MinValue = LODValue[i];
            if (LODValue[i] > MaxValue)
                MaxValue = LODValue[i];
        }
        return 1;
    }
    return 0;
}


int DiDisplayFunction::checkMinMaxDensity() const
{
    if ((MinDensity >= 0) && (MaxDensity >= 0) && (MinDensity >= MaxDensity))
    {
        DCMIMGLE_WARN("invalid optical density range (Dmin = " << MinDensity << ", Dmax = " << MaxDensity << ")");
        return 0;
    }
    return 1;
}


double DiDisplayFunction::getMinLuminanceValue() const
{
    /* Dmax = -1 means unspecified */
    return (MaxDensity < 0) ? -1 : convertODtoLum(MaxDensity);
}


double DiDisplayFunction::getMaxLuminanceValue() const
{
    /* Dmin = -1 means unspecified */
    return (MinDensity < 0) ? -1 : convertODtoLum(MinDensity);
}


double *DiDisplayFunction::convertODtoLumTable(const double *od_tab,
                                               const unsigned long count,
                                               const OFBool useAmb)
{
    double *lum_tab = NULL;
    if ((od_tab != NULL) && (count > 0))
    {
        /* create a new table for the luminance values */
        lum_tab = new double[count];
        if (lum_tab != NULL)
        {
            /* compute luminance values from optical density */
            unsigned int i;
            if (useAmb)
            {
                for (i = 0; i < count; ++i)
                    lum_tab[i] = AmbientLight + Illumination * pow(OFstatic_cast(double, 10), -od_tab[i]);
            } else {
                /* ambient light is added later */
                for (i = 0; i < count; ++i)
                    lum_tab[i] = Illumination * pow(OFstatic_cast(double, 10), -od_tab[i]);
            }
        }
    }
    return lum_tab;
}


double DiDisplayFunction::convertODtoLum(const double value,
                                         const OFBool useAmb) const
{
    return (useAmb) ? convertODtoLum(value, AmbientLight, Illumination) :
                      convertODtoLum(value, 0, Illumination);
}


double DiDisplayFunction::convertODtoLum(const double value,
                                         const double ambient,
                                         const double illum)
{
    /* formula from DICOM PS3.14: L = La + L0 * 10^-D */
    return (value >= 0) && (ambient >= 0) && (illum >= 0) ?
        ambient + illum * pow(OFstatic_cast(double, 10), -value) : -1 /*invalid*/;
}
