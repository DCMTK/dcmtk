/*********************************************************************
** 
**  DicomMonochrome2Image (Source)
**
**  author   : Joerg Riesmeier
**  created  : 30.12.96
**  modified : 05.03.98 
**
*********************************************************************/


#include "osconfig.h"
#include "dctypes.h"

#include "dimo2img.h"
#include "diutils.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiMono2Image::DiMono2Image(const DiDocument *docu, const EI_Status status)
  : DiMonoImage(docu, status)
{
} 


DiMono2Image::DiMono2Image(const DiDocument *docu, const EI_Status status, const int dummy)
  : DiMonoImage(docu, status, dummy)
{
} 


DiMono2Image::DiMono2Image(const DiColorImage *image, const double red, const double green, const double blue)
  : DiMonoImage(image, red, green, blue)
{
}


DiMono2Image::DiMono2Image(const DiMonoImage *image, const Uint16 columns, const Uint16 rows, const int interpolate)
  : DiMonoImage(image, columns, rows, interpolate)
{
}


DiMono2Image::DiMono2Image(const DiMonoImage *image, const Uint16 left, const Uint16 top, const Uint16 columns,
    const Uint16 rows)
  : DiMonoImage(image, left, top, columns, rows)
{
}


/*--------------*
 *  destructor  *
 *--------------*/

DiMono2Image::~DiMono2Image()
{
}


/*********************************************************************/


void *DiMono2Image::getOutputData(const unsigned long frame, const int bits, const int)
{
    return DiMonoImage::getData(frame, bits, 0, maxval(bits)); 
}


DiImage *DiMono2Image::scale(const unsigned long width, const unsigned long height, const int interpolate) const
{
    DiImage *image = new DiMono2Image(this, (Uint16)width, (Uint16)height, interpolate);
    return image;
}


DiImage *DiMono2Image::clip(const unsigned long left, const unsigned long top, const unsigned long width,
    const unsigned long height) const
{
    DiImage *image = new DiMono2Image(this, (Uint16)left, (Uint16)top, (Uint16)width, (Uint16)height);
    return image;
}


DiImage *DiMono2Image::makeMonochrome(const double, const double, const double) const
{
    DiImage *image = new DiMono2Image(this, (Uint16)Columns, (Uint16)Rows);
    return image;
}
