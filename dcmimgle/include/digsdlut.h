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
 *  Purpose: DicomGSDFLUT (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-09-10 08:50:24 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/digsdlut.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 * 
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIGSDLUT_H
#define __DIGSDLUT_H

#include "osconfig.h"

#include "didislut.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to compute and store the GSDF lookup table
 */
class DiGSDFLUT
  : public DiDisplayLUT
{

 public:

    DiGSDFLUT(const unsigned long count,
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
              ostream *stream = NULL);

    virtual ~DiGSDFLUT();


 protected:
 
    int createLUT(const Uint16 *ddl_tab,
                  const double *lum_tab,
                  const Uint16 ddl_cnt,
                  const double *gsdf_tab,
                  const double *gsdf_spl,
                  const unsigned int gsdf_cnt,
                  const double jnd_min,
                  const double jnd_max,
                  ostream *stream = NULL);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: digsdlut.h,v $
 * Revision 1.1  1999-09-10 08:50:24  joergr
 * Added support for CIELAB display function. Restructured class hierarchy
 * for display functions.
 *
 *
 *
 */
