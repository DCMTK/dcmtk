/*********************************************************************
** 
**  DicomOverlayPlane (Source)   -   Multiframe Overlays UNTESTED !
**
**  author   : Joerg Riesmeier
**  created  : 10.01.97
**  modified : 27.03.98 
**
*********************************************************************/


#include "osconfig.h"
#include "dctypes.h"
#include "dcdeftag.h"
#include "dctagkey.h"

#include "diovpln.h"
#include "didocu.h"

#include <string.h>


/*----------------*
 *  constructors  *
 *----------------*/

DiOverlayPlane::DiOverlayPlane(const DiDocument *docu, const unsigned int plane, const Uint16 alloc)
  : NumberOfFrames(0),
    ImageFrameOrigin(0),
    Top(0),
    Left(0),
    Height(0),
    Width(0),
    Rows(0),
    Columns(0),
    BitsAllocated(0),
    BitPosition(0),
    Foreground(1),
    Threshold(1),
    Mode(EMO_Replace),
    DefaultMode(EMO_Replace),
    Valid(0),
    Visible(0),
    BitPos(0),
    StartBitPos(0),
    Ptr(NULL),
    StartPtr(NULL),
    Data(NULL)
{
    if (docu != NULL)
    {
        DcmTagKey tag(DCM_OverlayRows.getGroup() + 2 * plane, DCM_OverlayRows.getElement());
        const char *str;
        tag.setElement(DCM_OverlayType.getElement());
        if (docu->getValue(tag, str) && (strcmp(str, "R") == 0))
            DefaultMode = Mode = EMO_RegionOfInterest;
        Sint32 sl = 0;
        tag.setElement(DCM_OverlayNumberOfFrames.getElement());
        docu->getValue(tag, sl);
        NumberOfFrames = (sl < 1) ? 1 : (Uint32)sl;
        tag.setElement(DCM_ImageFrameOrigin.getElement());
        docu->getValue(tag, ImageFrameOrigin);
        if (ImageFrameOrigin > 0)                                   // image frame origin is numbered from 1
            ImageFrameOrigin--;
        tag.setElement(DCM_OverlayOrigin.getElement());
#ifdef REVERSE_OVERLAY_ORIGIN_ORDER
        Valid = (docu->getValue(tag, Top, 1) > 0);
        Valid &= (docu->getValue(tag, Left, 0) > 0);
#else
        Valid = (docu->getValue(tag, Top, 0) > 0);
        Valid &= (docu->getValue(tag, Left, 1) > 0);
#endif
        Top--;                                                      // overlay origin is numbered from 1
        Left--;
        tag.setElement(DCM_OverlayRows.getElement());
        Valid &= (docu->getValue(tag, Rows) > 0);
        Height = Rows;
        tag.setElement(DCM_OverlayColumns.getElement());
        Valid &= (docu->getValue(tag, Columns) > 0);
        Width = Columns;
        tag.setElement(DCM_OverlayBitsAllocated.getElement());
        Valid &= (docu->getValue(tag, BitsAllocated) > 0);
        tag.setElement(DCM_OverlayBitPosition.getElement());
        Valid &= (docu->getValue(tag, BitPosition) > 0);
        tag.setElement(DCM_OverlayData.getElement());
        if (Valid && !docu->getValue(tag, Data))
        {
            ImageFrameOrigin = 0;                                   // see supplement 4
            if (BitsAllocated != alloc)                             // see correction proposal 87
            {
                cerr << "WARNING: invalid value for 'OverlayBitsAllocated' (" << BitsAllocated << ") ";
                cerr << "... assuming " << (BitsAllocated = alloc) << " !" << endl; 
            } 
            docu->getValue(DCM_PixelData, Data);
            Valid = (Data != NULL);
        }
    }
}


DiOverlayPlane::DiOverlayPlane(DiOverlayPlane *plane, const unsigned int bit, Uint16 *data, Uint16 *temp,
    const Uint16 width, const Uint16 height, const Uint16 columns, const Uint16 rows, const double xfactor,
    const double yfactor)
  : NumberOfFrames(plane->NumberOfFrames),
    ImageFrameOrigin(plane->ImageFrameOrigin),
    Top((Sint16)(yfactor * plane->Top)),
    Left((Sint16)(xfactor * plane->Left)),
    Height((Uint16)(yfactor * plane->Height)),
    Width((Uint16)(xfactor * plane->Width)),
    Rows(rows),
    Columns(columns),
    BitsAllocated(16),
    BitPosition(bit),
    Foreground(plane->Foreground),
    Threshold(plane->Threshold),
    Mode(plane->Mode),
    DefaultMode(plane->DefaultMode),
    Valid(0),
    Visible(plane->Visible),
    BitPos(0),
    StartBitPos(0),
    Ptr(NULL),
    StartPtr(NULL),
    Data(data)
{
    if (temp != NULL)
    {
        register Uint16 x;
        register Uint16 y;
        register Uint16 *q = temp;
        register const Uint16 mask = 1 << bit;
        const Uint16 skip_x = width - plane->Columns;
        const unsigned long skip_f = (unsigned long)(height - plane->Rows) * (unsigned long)width;
        for (unsigned long f = 0; f < NumberOfFrames; f++)
        {
            if (plane->reset(f + ImageFrameOrigin))
            {
                for (y = 0; y < plane->Rows; y++)
                {
                    for (x = 0; x < plane->Columns; x++, q++)
                    {
                        if (plane->getNextBit())
                            *q |= mask;                         // set corresponding bit
                        else
                            *q &= ~mask;                        // unset ... bit
                    }
                    q += skip_x;                                // skip to next line start
                }
                q += skip_f;                                    // skip to next frame start
            }
        }
        Valid = (Data != NULL);
    }
}


/*--------------*
 *  destructor  *
 *--------------*/

DiOverlayPlane::~DiOverlayPlane()
{
}


/********************************************************************/


void DiOverlayPlane::show(const double fore, const double thresh, const EM_Overlay mode)
{
    Foreground = (fore < 0) ? 0 : (fore > 1) ? 1 : fore;
    Threshold = (thresh < 0) ? 0 : (thresh > 1) ? 1 : thresh;
    Mode = (mode == EMO_Default) ? DefaultMode : mode;
    Visible = 1;
}
