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
 *  Purpose: DicomBartenLUT (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1999-02-11 15:37:02 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/dibarlut.h,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 * 
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIBARLUT_H
#define __DIBARLUT_H

#include "osconfig.h"

#include "dibaslut.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiBartenLUT
  : public DiBaseLUT
{

 public:

    DiBartenLUT(const unsigned long count,
                const Uint16 max,
                const Uint16 *ddl_tab,
                const double *lum_tab,
                const Uint16 ddl_cnt,
                const double *gsdf_tab,
                const double *gsdf_spl,
                const unsigned int gsdf_cnt,
                const double jnd_min,
                const double jnd_max);

    virtual ~DiBartenLUT();

    inline Uint16 getValue(const Uint16 pos) const
    {
        return Data[pos];
    }


 protected:
 
    int createLUT(const Uint16 *ddl_tab,
                  const double *lum_tab,
                  const Uint16 ddl_cnt,
                  const double *gsdf_tab,
                  const double *gsdf_spl,
                  const unsigned int gsdf_cnt,
                  const double jnd_min,
                  const double jnd_max);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dibarlut.h,v $
 * Revision 1.2  1999-02-11 15:37:02  joergr
 * Removed unused parameter.
 *
 * Revision 1.1  1999/02/03 17:46:15  joergr
 * Added support for calibration according to Barten transformation (incl.
 * a DISPLAY file describing the monitor characteristic).
 *
 *
 *
 */
