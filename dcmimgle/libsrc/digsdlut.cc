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
 *  Purpose: DicomGSDFLUT (Source)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-09-17 13:13:30 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/libsrc/digsdlut.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"

#include "digsdlut.h"
#include "displint.h"

//BEGIN_EXTERN_C
 #include <math.h>
//END_EXTERN_C


/*----------------*
 *  constructors  *
 *----------------*/

DiGSDFLUT::DiGSDFLUT(const unsigned long count,
                     const Uint16 max,
                     const Uint16 *ddl_tab,
                     const double *lum_tab,
                     const Uint16 ddl_cnt,
                     const double *gsdf_tab,
                     const double *gsdf_spl,
                     const unsigned int gsdf_cnt,
                     const double jnd_min,
                     const double jnd_max,
                     const double amb,
                     ostream *stream)
  : DiDisplayLUT(count, max, amb)
{
    if ((Count > 0) && (Bits > 0))
    {
        if (DicomImageClass::DebugLevel & DicomImageClass::DL_Informationals)
            cerr << "INFO: new GSDF LUT with " << Bits << " bits output and " << Count << " entries created !" << endl;
        Valid = createLUT(ddl_tab, lum_tab, ddl_cnt, gsdf_tab, gsdf_spl, gsdf_cnt, jnd_min, jnd_max, stream);
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
                         const double *lum_tab,
                         const Uint16 ddl_cnt,
                         const double *gsdf_tab,
                         const double *gsdf_spl,
                         const unsigned int gsdf_cnt,
                         const double jnd_min,
                         const double jnd_max,
                         ostream *stream)
{
    if ((ddl_tab != NULL) && (lum_tab != NULL) && (ddl_cnt > 0) && (gsdf_tab != NULL) && (gsdf_spl != NULL) && (gsdf_cnt > 0))
    {
        int status = 0;
        double *jidx = new double[Count];
        if (jidx != NULL)
        {
            const double dist = (jnd_max - jnd_min) / (Count - 1);      // distance between two entries
            register unsigned int i;
            register double *r = jidx;
            register double value = jnd_min;                            // first value is static !
            for (i = Count; i > 1; i--)                                 // initialize scaled JND index array
            {
                *(r++) = value;
                value += dist;                                          // add step by step ...
            }
            *r = jnd_max;                                               // last value is static !

            double *jnd_idx = new double[gsdf_cnt];
            if (jnd_idx != NULL)
            {
                r = jnd_idx;
                for (i = 0; i < gsdf_cnt; i++)                          // initialize JND index array
                    *(r++) = i + 1;

                double *gsdf = new double[Count];                       // interpolated GSDF
                if (gsdf != NULL)
                {     
                    if (DiCubicSpline<double, double>::Interpolation(jnd_idx, gsdf_tab, gsdf_spl, gsdf_cnt, jidx, gsdf, (unsigned int)Count))
                    {
                        DataBuffer = new Uint16[Count];
                        if (DataBuffer != NULL)
                        {
                            r = gsdf;
                            register Uint16 *q = DataBuffer;
                            register Uint16 j = 0;
                            const double amb = getAmbientLightValue();
                            for (i = Count; i != 0; i--, r++)
                            {
                                while (((Uint16)(j + 1) < ddl_cnt) && (lum_tab[j]  + amb < *r))  // search for closest index, assuming monotony
                                    j++;
                                if ((j > 0) && (fabs(lum_tab[j - 1] + amb - *r) < fabs(lum_tab[j] + amb - *r)))
                                    j--;
                                *(q++) = ddl_tab[j];
                            }
                            Data = DataBuffer;
                            if (stream != NULL)                         // write curve data to file
                            {
                                if (Count == ddl_cnt)                   // check whether GSDF LUT fits exactly to DISPLAY file
                                {
                                    for (i = 0; i < ddl_cnt; i++)
                                    {
                                        (*stream) << ddl_tab[i] << "\t"; 
                                        stream->setf(ios::fixed, ios::floatfield);
                                        (*stream) << lum_tab[i] + amb << "\t";
                                        (*stream) << gsdf[i] << "\t";
                                        (*stream) << lum_tab[Data[i]] + amb << endl;
                                    }
                                } else {
                                    if (DicomImageClass::DebugLevel & DicomImageClass::DL_Warnings)
                                        cerr << "WARNING: can't write curve data, wrong DISPLAY file or GSDF LUT !" << endl;
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


/*
 *
 * CVS/RCS Log:
 * $Log: digsdlut.cc,v $
 * Revision 1.2  1999-09-17 13:13:30  joergr
 * Enhanced efficiency of some "for" loops.
 *
 * Revision 1.1  1999/09/10 08:54:50  joergr
 * Added support for CIELAB display function. Restructured class hierarchy
 * for display functions.
 *
 *
 *
 */
