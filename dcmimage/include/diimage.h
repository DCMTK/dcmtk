/*********************************************************************
** 
**  DicomImage (Header)
**
**  author   : Joerg Riesmeier
**  created  : 10.12.96
**  modified : 13.02.97
**
*********************************************************************/


#ifndef __DIIMAGE_H
#define __DIIMAGE_H

#include "osconfig.h"
#include "dctypes.h"

#ifdef SUNCC
 #include "didocu.h"
#endif

#include "diutils.h"

#include <stdio.h>
#include <iostream.h>
#include <fstream.h>


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DcmPixelData;

#ifndef SUNCC
 class DiDocument;
#endif

class DiInputPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiImage
{
 public:
    DiImage(const DiDocument *, const EI_Status);
    virtual ~DiImage();

	inline EI_Status getStatus() const
		{ return Status; }
	
	inline Uint32 getNumberOfFrames() const
		{ return NumberOfFrames; }

	inline Uint16 getRows() const
		{ return Rows; }
	inline Uint16 getColumns() const
		{ return Columns; }

	inline double getPixelWidth() const
		{ return (PixelWidth > 0) ? PixelWidth : 1; }
	inline double getPixelHeight() const
		{ return (PixelHeight > 0) ? PixelHeight : 1; }

	inline double getRowColumnRatio() const
		{ return getPixelHeight() / getPixelWidth(); }
	inline double getColumnRowRatio() const
		{ return getPixelWidth() / getPixelHeight(); }

	virtual int getMinMaxValues(double &, double &) const
		{ return 0; }
	inline int getBits(const int bits = 0) const
		{ return ((bits < 1) || (bits > MAX_BITS)) ? BitsPerSample : bits; }
	
	virtual void *getOutputData(const unsigned long, const int, const int) = 0;
	virtual void *getOutputPlane(const int) const = 0;
	virtual void deleteOutputData() = 0;
	
	virtual int setMinMaxWindow(const int)
		{ return 0; }
	virtual int setHistogramWindow(const double)
		{ return 0; }
		
	virtual int setWindow(const unsigned long = 0)
		{ return 0; }
	virtual int setWindow(const double, const double)
		{ return 0; }
	virtual int getWindow(double &, double &)
		{ return 0; }
	virtual unsigned long getWindowCount() const
		{ return 0; }
	
	virtual int setVoiLut(const unsigned long)
		{ return 0; }
	virtual unsigned long getVoiLutCount() const
		{ return 0; }
	
	virtual int showOverlay(const unsigned int)
		{ return 0; }
	virtual int showOverlay(const unsigned int, const double, const double, const EM_Overlay)
		{ return 0; }
		
	virtual int showAllOverlays()
		{ return 0; }
	virtual int showAllOverlays(const double, const double, const EM_Overlay)
		{ return 0; }

	virtual int hideOverlay(const unsigned int)
		{ return 0; }
	virtual int hideAllOverlays()
		{ return 0; }

	virtual int placeOverlay(const unsigned int, const signed int, const signed int)
		{ return 0; }
	virtual unsigned int getOverlayCount() const
		{ return 0; }

	virtual DiImage *scale(const unsigned long, const unsigned long, const int) const = 0;
	virtual DiImage *clip(const unsigned long, const unsigned long, const unsigned long, const unsigned long)
		const = 0;

	virtual DiImage *makeMonochrome(const double, const double, const double) const = 0;

	virtual void *createDIB(const unsigned long) = 0;

	virtual int writePPM(ostream &, const unsigned long, const int) = 0;
	virtual int writePPM(FILE *, const unsigned long, const int) = 0; 
	virtual int writeRawPPM(FILE *, const unsigned long, const int) = 0; 
    
 protected:
    DiImage(const DiDocument *, const EI_Status, const int);
    DiImage(const DiImage *);
    DiImage(const DiImage *, const Uint16, const Uint16);

	void deleteInputData();
	void checkPixelExtension();
    void convertPixelData(/*const*/ DcmPixelData *);
 		
	EI_Status Status;								// copy of status variable declared in class 'DicomImage'
    const DiDocument *Document;						// points to special object, which capsulates the DCMTK

	Uint32 NumberOfFrames;							// number of frames in case of multi-frame images (otherwise '1')
    Uint16 Rows;									// number of rows (in pixel)
    Uint16 Columns;									// number of columns (in pixel)
	double PixelWidth;								// width of each pixel according to 'PixelSpacing/AspectRatio'
	double PixelHeight;								// height of each pixel according to 'PixelSpacing/AspectRatio'
    Uint16 BitsAllocated;							// number of bits allocated for each pixel
    Uint16 BitsStored;								// number of bits stored for each pixel (see 'BitsPerSample')
    Uint16 HighBit;									// position of highest stored bit 
	
	int BitsPerSample;								// actual number of bits per sample (depth)
    
    int hasSignedRepresentation;					// is 'true' if pixel data is signed
	int hasPixelSpacing;							// is 'true' if attribute 'PixelSpacing' is present
	int hasPixelAspectRatio;						// is 'true' if attribute 'PixelAspectRatio' is present
	int isOriginal;									// is 'false' if derived from original image data (e.g. scaled)

    DiInputPixel *InputData;						// points to intermediate pixel representation (template object)
};


#endif
