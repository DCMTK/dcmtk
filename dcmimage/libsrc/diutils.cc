/*********************************************************************
** 
**  Utilities (Source)
**
**  author   : Joerg Riesmeier
**  created  : 10.12.96
**  modified : 09.03.98
*********************************************************************/


#include "osconfig.h"
#include "dctypes.h"

#include "diutils.h"

#include <iostream.h>


/*------------------------*
 *  function definitions  *
 *------------------------*/

EP_Representation determineRepresentation(double minvalue, double maxvalue)
{
    if (minvalue > maxvalue)                        /* assertion: min < max ! */
    {
        const double temp = minvalue;
        minvalue = maxvalue;
        maxvalue = temp;
    }
    if (minvalue < 0)                               /* signed */
    {
        if ((-minvalue <= maxval(7, 0)) && (maxvalue <= maxval(7)))
            return EPR_Sint8;
        if ((-minvalue <= maxval(15, 0)) && (maxvalue <= maxval(15)))
            return EPR_Sint16;
        if (-minvalue > maxval(MAX_BITS - 1, 0))
        {
            cerr << "WARNING: minimum pixel value (" << minvalue << ") exceeds signed " << MAX_BITS << " bit ";
            cerr << "representation after modality transformation !" << endl;
        }
        if (maxvalue > maxval(MAX_BITS - 1))
        {
            cerr << "WARNING: maximum pixel value (" << maxvalue << ") exceeds signed " << MAX_BITS << " bit ";
            cerr << "representation after modality transformation !" << endl;
        }
        return EPR_Sint32;
    }
    if (maxvalue <= maxval(8))
        return EPR_Uint8;
    if (maxvalue <= maxval(16))
        return EPR_Uint16;
    if (maxvalue > maxval(MAX_BITS))
    {
        cerr << "WARNING: maximum pixel value (" << maxvalue << ") exceeds unsigned " << MAX_BITS << " bit ";
        cerr << "representation after modality transformation !" << endl;
    }
    return EPR_Uint32;
}
