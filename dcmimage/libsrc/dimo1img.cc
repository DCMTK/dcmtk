/*********************************************************************
** 
**  DicomMonochrome1Image (Source)
**
**  author   : Joerg Riesmeier
**  created  : 30.12.96
**  modified : 05.03.98 
**
*********************************************************************/


#include "osconfig.h"
#include "dctypes.h"

#include "dimo1img.h"
#include "diutils.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiMono1Image::DiMono1Image(const DiDocument *docu, const EI_Status status)
  : DiMonoImage(docu, status)
{
} 


DiMono1Image::DiMono1Image(const DiMonoImage *image, const Uint16 columns, const Uint16 rows, const int interpolate)
  : DiMonoImage(image, columns, rows, interpolate)
{
}


DiMono1Image::DiMono1Image(const DiMonoImage *image, const Uint16 left, const Uint16 top, const Uint16 columns,
	const Uint16 rows)
  : DiMonoImage(image, left, top, columns, rows)
{
}


/*--------------*
 *  destructor  *
 *--------------*/

DiMono1Image::~DiMono1Image()
{
}


/*********************************************************************/


void *DiMono1Image::getOutputData(const unsigned long frame, const int bits, const int)
{
	return DiMonoImage::getData(frame, bits, maxval(bits), 0); 
}


DiImage *DiMono1Image::scale(const unsigned long width, const unsigned long height, const int interpolate) const
{
	DiImage *image = new DiMono1Image(this, (Uint16)width, (Uint16)height, interpolate);
	return image;
}


DiImage *DiMono1Image::clip(const unsigned long left, const unsigned long top, const unsigned long width,
	const unsigned long height) const
{
	DiImage *image = new DiMono1Image(this, (Uint16)left, (Uint16)top, (Uint16)width, (Uint16)height);
	return image;
}


DiImage *DiMono1Image::makeMonochrome(const double, const double, const double) const
{
	DiImage *image = new DiMono1Image(this, (Uint16)Columns, (Uint16)Rows);
	return image;
}
