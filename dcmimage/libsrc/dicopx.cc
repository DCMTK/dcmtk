/*
**
** Author:  Joerg Riesmeier
** Created: 20.12.96
**
** Module:  dicopx.cc
**
** Purpose: DicomColorPixel (Source)
**
** Last Update:      $Author: joergr $
** Update Date:      $Date: 1998-05-11 14:52:27 $
** Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/dicopx.cc,v $
** CVS/RCS Revision: $Revision: 1.3 $
** Status:           $State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#include "osconfig.h"
#include "dctypes.h"
#include "dcdeftag.h"

#include "dicopx.h"
#include "dimopx.h"
#include "diinpx.h"
#include "didocu.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiColorPixel::DiColorPixel(const DiDocument *docu, const DiInputPixel *pixel, const Uint16 samples, EI_Status &status,
    const Uint16 sample_rate)
  : PlanarConfiguration(0),
    Count(0)
{
    if (docu != NULL)
    {
        Uint16 us = 0;
        if (docu->getValue(DCM_SamplesPerPixel, us))
        {
            if (us != samples)
            {
                cerr << "WARNING: invalid value for 'SamplesPerPixel' (" << us;
                cerr << ") ... assuming " << samples << " !" << endl;
            }
            if (docu->getValue(DCM_PlanarConfiguration, us))
            {
                PlanarConfiguration = (us == 1);
                if ((us != 0) && (us != 1))
                {
                    cerr << "WARNING: invalid value for 'PlanarConfiguration' (" << us;
                    cerr << ") ... assuming 'color-by-pixel' (0) !" << endl;
                }
            }
            else if (samples > 1)
            {
                status = EIS_MissingAttribute;
                cerr << "ERROR: mandatory attribute 'PlanarConfiguration' is missing !" << endl;
                return;
            }
            if (pixel != NULL)
                Count = pixel->getCount() / ((sample_rate == 0) ? samples : sample_rate);
        }
        else
        {
            status = EIS_MissingAttribute;
            cerr << "ERROR: mandatory attribute 'SamplesPerPixel' is missing !" << endl;
        }
    }
}


DiColorPixel::DiColorPixel(const DiMonoPixel *pixel)
  : PlanarConfiguration(0),
    Count(pixel->getCount())
{
}


DiColorPixel::DiColorPixel(const DiColorPixel *pixel, const unsigned long count)
  : PlanarConfiguration(pixel->PlanarConfiguration),
    Count(count)
{
}


/*--------------*
 *  destructor  *
 *--------------*/

DiColorPixel::~DiColorPixel()
{
}


/*
**
** CVS/RCS Log:
** $Log: dicopx.cc,v $
** Revision 1.3  1998-05-11 14:52:27  joergr
** Added CVS/RCS header to each file.
**
**
*/
