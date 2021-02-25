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
 *  Purpose: DicomCIELABLUT (Source)
 *
 */


#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/dicielut.h"

#include <cmath>

/*----------------*
 *  constructors  *
 *----------------*/

DiCIELABLUT::DiCIELABLUT(const unsigned long count,
                         const Uint16 max,
                         const Uint16 *ddl_tab,
                         const double *val_tab,
                         const unsigned long ddl_cnt,
                         const double val_min,
                         const double val_max,
                         const double lum_min,
                         const double lum_max,
                         const double amb,
                         const OFBool inverse,
                         STD_NAMESPACE ostream *stream,
                         const OFBool printMode)
  : DiDisplayLUT(count, max, amb /*, 'illum' not used*/)
{
    if ((Count > 0) && (Bits > 0))
    {
        DCMIMGLE_DEBUG("new CIELAB LUT with " << Bits << " bits output and " << Count << " entries created");
        if (val_min >= val_max)
        {
            DCMIMGLE_ERROR("invalid value range for CIELAB LUT creation (" << val_min << " - " << val_max << ")");
        }
        /* create the lookup table */
        Valid = createLUT(ddl_tab, val_tab, ddl_cnt, val_min, val_max, lum_min, lum_max,
                          inverse, stream, printMode);
    }
}


/*--------------*
 *  destructor  *
 *--------------*/

DiCIELABLUT::~DiCIELABLUT()
{
}


/********************************************************************/


int DiCIELABLUT::createLUT(const Uint16 *ddl_tab,
                           const double *val_tab,
                           const unsigned long ddl_cnt,
                           const double val_min,
                           const double val_max,
                           const double lum_min,
                           const double lum_max,
                           const OFBool inverse,
                           STD_NAMESPACE ostream *stream,
                           const OFBool printMode)
{
    int status = 0;
    if ((ddl_tab != NULL) && (val_tab != NULL) && (ddl_cnt > 0) && (val_max > 0) && (val_min < val_max))
    {
        const unsigned long cin_ctn = (inverse) ? ddl_cnt : Count;      // number of points to be interpolated
        double *cielab = new double[cin_ctn];
        if (cielab != NULL)
        {
            unsigned long i;
            double llin = 0;
            double cub = 0;
            const double amb = getAmbientLightValue();
            /* check whether Lmin or Lmax is set */
            const double min = (lum_min < 0) ? val_min + amb : lum_min /*includes 'amb'*/;
            const double max = (lum_max < 0) ? val_max + amb : lum_max /*includes 'amb'*/;
            const double lmin = min / max;
            const double hmin = (lmin > 0.008856) ? 116.0 * pow(lmin, 1.0 / 3.0) - 16 : 903.3 * lmin;
            const double lfac = (100.0 - hmin) / (OFstatic_cast(double, cin_ctn - 1) * 903.3);
            const double loff = hmin / 903.3;
            const double cfac = (100.0 - hmin) / (OFstatic_cast(double, cin_ctn - 1) * 116.0);
            const double coff = (16.0  + hmin) / 116.0;
            for (i = 0; i < cin_ctn; ++i)                   // compute CIELAB function
            {
                llin = OFstatic_cast(double, i) * lfac + loff;
                cub = OFstatic_cast(double, i) * cfac + coff;
                cielab[i] = ((llin > 0.008856) ? cub * cub * cub : llin) * max;
            }
            DataBuffer = new Uint16[Count];
            if (DataBuffer != NULL)                         // create look-up table
            {
                Uint16 *q = DataBuffer;
                unsigned long j = 0;
                /* check whether to apply the inverse transformation */
                if (inverse)
                {
                    double v;
                    const double factor = OFstatic_cast(double, ddl_cnt - 1) / OFstatic_cast(double, Count - 1);
                    /* convert from DDL */
                    for (i = 0; i < Count; ++i)
                    {
                        v = val_tab[OFstatic_cast(int, i * factor)] + amb;    // need to scale index to range of value table
                        while ((j + 1 < ddl_cnt) && (cielab[j] < v))          // search for closest index, assuming monotony
                            ++j;
                        if ((j > 0) && (fabs(cielab[j - 1] - v) < fabs(cielab[j] - v)))
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
                    const double *r = cielab;
                    /* convert to DDL */
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
                    if (Count == ddl_cnt)                   // check whether CIELAB LUT fits exactly to DISPLAY file
                    {
                        for (i = 0; i < ddl_cnt; ++i)
                        {
                            (*stream) << ddl_tab[i];                               // DDL
                            stream->setf(STD_NAMESPACE ios::fixed, STD_NAMESPACE ios::floatfield);
                            if (printMode)
                                (*stream) << "\t" << val_tab[i] + amb;             // CC
                            (*stream) << "\t" << cielab[i];                        // CIELAB
                            if (printMode)
                            {
                                if (inverse)
                                    (*stream) << "\t" << cielab[Data[i]];          // PSC'
                                else
                                    (*stream) << "\t" << val_tab[Data[i]] + amb;   // PSC
                            }
                            (*stream) << OFendl;
                        }
                    } else {
                        DCMIMGLE_WARN("can't write curve data, wrong DISPLAY file or CIELAB LUT");
                    }
                }
                status = 1;
            }
        }
        delete[] cielab;
    }
    return status;
}
