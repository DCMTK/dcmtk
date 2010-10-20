/*
 *
 *  Copyright (C) 1999-2010, OFFIS e.V.
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
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-10-20 07:41:35 $
 *  CVS/RCS Revision: $Revision: 1.51 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
            register unsigned int i;
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
            register Uint16 i;
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
    register int i;
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
        register unsigned long j = 0;
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
        register int i;
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
#ifdef HAVE_IOS_NOCREATE
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
            register unsigned long i;
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
                    register unsigned int i;
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
                    register unsigned int i;
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
        register unsigned long i;
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
            register unsigned int i;
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


/*
 *
 * CVS/RCS Log:
 * $Log: didispfn.cc,v $
 * Revision 1.51  2010-10-20 07:41:35  uli
 * Made sure isalpha() & friends are only called with valid arguments.
 *
 * Revision 1.50  2010-10-14 13:14:17  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.49  2010-03-01 09:08:47  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.48  2009-11-25 16:11:03  joergr
 * Removed inclusion of header file "ofconsol.h".
 *
 * Revision 1.47  2009-10-28 14:26:01  joergr
 * Fixed minor issues in log output.
 *
 * Revision 1.46  2009-10-28 09:53:40  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.45  2006-08-15 16:30:11  meichel
 * Updated the code in module dcmimgle to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.44  2005/12/08 15:42:47  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.43  2005/06/24 10:02:16  joergr
 * Fixed problem in constructor which uses a linear chacteristic curve.
 *
 * Revision 1.42  2004/01/05 14:58:42  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.41  2003/12/23 16:03:18  joergr
 * Replaced post-increment/decrement operators by pre-increment/decrement
 * operators where appropriate (e.g. 'i++' by '++i').
 *
 * Revision 1.40  2003/12/08 15:10:08  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 * Revision 1.39  2003/04/14 14:27:27  meichel
 * Added explicit typecasts in calls to pow(). Needed by Visual C++ .NET 2003.
 *
 * Revision 1.38  2003/02/12 11:37:14  joergr
 * Added Dmin/max support to CIELAB calibration routines.
 *
 * Revision 1.37  2003/02/11 16:32:15  joergr
 * Added two new functions to determine the luminance/OD value of a particular
 * DDL according to the device's characteristic curve and vice versa.
 *
 * Revision 1.36  2003/02/11 10:02:31  joergr
 * Added support for Dmin/max to calibration routines (required for printers).
 *
 * Revision 1.35  2002/11/27 14:08:11  meichel
 * Adapted module dcmimgle to use of new header file ofstdinc.h
 *
 * Revision 1.34  2002/07/19 13:10:15  joergr
 * Fixed bug which occurred for very large number of DDLs only (65536).
 *
 * Revision 1.33  2002/07/18 12:33:55  joergr
 * Added support for hardcopy and softcopy input devices (camera and scanner).
 * Added polynomial curve fitting algorithm as an alternate interpolation
 * method.
 *
 * Revision 1.32  2002/07/03 13:50:59  joergr
 * Fixed inconsistencies regarding the handling of ambient light.
 *
 * Revision 1.31  2002/07/02 16:52:40  joergr
 * Minor fixes to keep MSVC6 quiet.
 *
 * Revision 1.30  2002/07/02 16:24:37  joergr
 * Added support for hardcopy devices to the calibrated output routines.
 *
 * Revision 1.29  2002/04/16 13:53:31  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.28  2001/09/28 13:12:20  joergr
 * Added check whether ios::nocreate exists.
 *
 * Revision 1.27  2001/06/01 15:49:54  meichel
 * Updated copyright header
 *
 * Revision 1.26  2000/05/03 09:47:23  joergr
 * Removed most informational and some warning messages from release built
 * (#ifndef DEBUG).
 *
 * Revision 1.25  2000/04/28 12:33:42  joergr
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
