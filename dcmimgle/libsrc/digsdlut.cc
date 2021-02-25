/*
 *
 *  Copyright (C) 1999-2021, OFFIS e.V.
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
 *  Purpose: DicomGSDFLUT (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/digsdlut.h"
#include "dcmtk/dcmimgle/displint.h"

#include <cmath>

/*----------------*
 *  constructors  *
 *----------------*/

DiGSDFLUT::DiGSDFLUT(const unsigned long count,
                     const Uint16 max,
                     const Uint16 *ddl_tab,
                     const double *val_tab,
                     const unsigned long ddl_cnt,
                     const double *gsdf_tab,
                     const double *gsdf_spl,
                     const unsigned int gsdf_cnt,
                     const double jnd_min,
                     const double jnd_max,
                     const double lum_min,
                     const double lum_max,
                     const double amb,
                     const double illum,
                     const OFBool inverse,
                     STD_NAMESPACE ostream *stream,
                     const OFBool printMode)
  : DiDisplayLUT(count, max, amb, illum)
{
    if ((Count > 0) && (Bits > 0))
    {
        DCMIMGLE_DEBUG("new GSDF LUT with " << Bits << " bits output and " << Count << " entries created");
        if (jnd_min >= jnd_max)
        {
            DCMIMGLE_ERROR("invalid JND range for GSDF LUT creation (" << jnd_min << " - " << jnd_max << ")");
        }
        /* create the lookup table */
        Valid = createLUT(ddl_tab, val_tab, ddl_cnt, gsdf_tab, gsdf_spl, gsdf_cnt,
                          jnd_min, jnd_max, lum_min, lum_max, inverse, stream, printMode);
    }
}


/*--------------*
 *  destructor  *
 *--------------*/

DiGSDFLUT::~DiGSDFLUT()
{
}


/********************************************************************/


int DiGSDFLUT::createLUT(const Uint16 *ddl_tab,
                         const double *val_tab,
                         const unsigned long ddl_cnt,
                         const double *gsdf_tab,
                         const double *gsdf_spl,
                         const unsigned int gsdf_cnt,
                         const double jnd_min,
                         const double jnd_max,
                         const double lum_min,
                         const double lum_max,
                         const OFBool inverse,
                         STD_NAMESPACE ostream *stream,
                         const OFBool printMode)
{
    /* check for valid parameters */
    if ((ddl_tab != NULL) && (val_tab != NULL) && (ddl_cnt > 0) && (gsdf_tab != NULL) && (gsdf_spl != NULL) &&
        (gsdf_cnt > 0) && (jnd_min < jnd_max))
    {
        int status = 0;
        const unsigned long gin_ctn = (inverse) ? ddl_cnt : Count;      // number of points to be interpolated
        double *jidx = new double[gin_ctn];
        if (jidx != NULL)
        {
            const double dist = (jnd_max - jnd_min) / (gin_ctn - 1);    // distance between two entries
            unsigned long i;
            double *s = jidx;
            double value = jnd_min;                                     // first value is fixed !
            for (i = gin_ctn; i > 1; --i)                               // initialize scaled JND index array
            {
                *(s++) = value;
                value += dist;                                          // add step by step ...
            }
            *s = jnd_max;                                               // last value is fixed !
            double *jnd_idx = new double[gsdf_cnt];
            if (jnd_idx != NULL)
            {
                s = jnd_idx;
                for (i = 0; i < gsdf_cnt; ++i)                          // initialize JND index array
                    *(s++) = i + 1;
                double *gsdf = new double[gin_ctn];                     // interpolated GSDF
                if (gsdf != NULL)
                {
                    if (DiCubicSpline<double, double>::Interpolation(jnd_idx, gsdf_tab, gsdf_spl, gsdf_cnt, jidx, gsdf,
                        OFstatic_cast(unsigned int, gin_ctn)))
                    {
                        DataBuffer = new Uint16[Count];
                        if (DataBuffer != NULL)
                        {
                            const double amb = getAmbientLightValue();
                            Uint16 *q = DataBuffer;
                            unsigned long j = 0;
                            /* check whether to apply the inverse transformation */
                            if (inverse)
                            {
                                double v;
                                const double factor = OFstatic_cast(double, ddl_cnt - 1) / OFstatic_cast(double, Count - 1);
                                /* convert DDL to P-Value */
                                for (i = 0; i < Count; ++i)
                                {
                                    v = val_tab[OFstatic_cast(int, i * factor)] + amb;    // need to scale index to range of value table
                                    while ((j + 1 < ddl_cnt) && (gsdf[j] < v))            // search for closest index, assuming monotony
                                        ++j;
                                    if ((j > 0) && (fabs(gsdf[j - 1] - v) < fabs(gsdf[j] - v)))
                                        --j;
                                    *(q++) = ddl_tab[j];
                                }
                            } else {
                                /* initial DDL boundaries */
                                unsigned long ddl_min = 0;
                                unsigned long ddl_max= ddl_cnt - 1;
                                /* check whether minimum luminance is specified */
                                if (lum_min >= 0)
                                {
                                    j = ddl_min;
                                    /* determine corresponding minimum DDL value */
                                    while ((j < ddl_max) && (val_tab[j] + amb < lum_min))
                                        ++j;
                                    ddl_min = j;
                                }
                                /* check whether maximum luminance is specified */
                                if (lum_max >= 0)
                                {
                                    j = ddl_max;
                                    /* determine corresponding maximum DDL value */
                                    while ((j > ddl_min) && (val_tab[j] + amb > lum_max))
                                        --j;
                                    ddl_max = j;
                                }
                                j = ddl_min;
                                const double *r = gsdf;
                                /* convert P-Value to DDL */
                                for (i = Count; i != 0; --i, ++r)
                                {
                                    while ((j < ddl_max) && (val_tab[j] + amb < *r))  // search for closest index, assuming monotony
                                        ++j;
                                    if ((j > 0) && (fabs(val_tab[j - 1] + amb - *r) < fabs(val_tab[j] + amb - *r)))
                                        --j;
                                    *(q++) = ddl_tab[j];
                                }
                            }
                            Data = DataBuffer;
                            if (stream != NULL)                         // write curve data to file
                            {
                                if (Count == ddl_cnt)                   // check whether GSDF LUT fits exactly to DISPLAY file
                                {
                                    for (i = 0; i < ddl_cnt; ++i)
                                    {
                                        (*stream) << ddl_tab[i];                               // DDL
                                        stream->setf(STD_NAMESPACE ios::fixed, STD_NAMESPACE ios::floatfield);
                                        if (printMode)
                                            (*stream) << "\t" << val_tab[i] + amb;             // CC
                                        (*stream) << "\t" << gsdf[i];                          // GSDF
                                        if (printMode)
                                        {
                                            if (inverse)
                                                (*stream) << "\t" << gsdf[Data[i]];            // PSC'
                                            else
                                                (*stream) << "\t" << val_tab[Data[i]] + amb;   // PSC
                                        }
                                        (*stream) << OFendl;
                                    }
                                } else {
                                    DCMIMGLE_WARN("can't write curve data, wrong DISPLAY file or GSDF LUT");
                                }
                            }
                            status = 1;
                        }
                    }
                }
                delete[] gsdf;
            }
            delete[] jnd_idx;
        }
        delete[] jidx;
        return status;
    }
    return 0;
}
