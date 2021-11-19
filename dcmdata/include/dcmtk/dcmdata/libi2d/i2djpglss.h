/*
 *
 *  Copyright (C) 2007-2021, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class to extract pixel data and meta information from JPEG file
 *
 */

#ifndef I2DJPGLSS_H
#define I2DJPGLSS_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/libi2d/i2dimgs.h"
#include "dcmtk/dcmdata/libi2d/i2djutil.h"
#include "dcmtk/ofstd/offile.h"
#include "dcmtk/ofstd/oflist.h"


/** This is a I2DImgSource implementation that can parse JPEG-LS files and convert
 *  them into DICOM images.
 */
class DCMTK_I2D_EXPORT I2DJpegLsSource : public I2DImgSource
{

public:
    /** Constructor, initializes member variables
     *  @return none
     */
    I2DJpegLsSource();

    /** Returns format of input image. For this class "JPEG" is returned.
     *  @return Returns format of input image, i.e. "JPEG".
     */
    OFString inputFormat() const;

    /** Extracts the raw JPEG-LS pixel data stream from a JPEG file and returns some
     *  further information about this pixel data. Raw means here that all APP
     *  markers (e.g. JFIF information) are removed from the JPEG stream.
     *  The pixel data returned is a JPEG stream in JPEG interchange format.
     *  This function allocates memory for the pixel data returned to the user.
     *  The caller of this function is responsible for deleting the memory buffer.
     *  @param rows - [out] Rows of image
     *  @param cols - [out] Columns of image
     *  @param samplesPerPixel - [out] Number of components per pixel
     *  @param photoMetrInt - [out] The DICOM color model used for the compressed data
     *  @param bitsAlloc - [out] Bits Allocated for one sample
     *  @param bitsStored - [out] Bits Stored, Number of bits actually stored within
     *                            Bits Allocated
     *  @param highBit - [out] High Bit, Highest stored in bit within Bits Allocated
     *  @param pixelRepr - [out] Pixel Representation (0=unsigned, 1=signed)
     *  @param planConf - [out] Planar Configuration
     *  @param pixAspectH - [out] Horizontal value of pixel aspect ratio
     *  @param pixAspectV - [out] Vertical value of pixel aspect ratio
     *  @param pixData - [out] Pointer to the pixel data in JPEG Interchange Format
     *                         (but without APPx markers).
     *  @param length - [out] Length of pixel data
     *  @param ts - [out] The transfer syntax imposed by the imported pixel pixel
     *                    data. This is necessary for the JPEG importer that needs
     *                    to report which TS must be used for the imported JPEG data
     *                    (ie. baseline, progressive, ...).
     *  @return EC_Normal, if successful, error otherwise
     */
    OFCondition readPixelData(Uint16& rows,
                              Uint16& cols,
                              Uint16& samplesPerPixel,
                              OFString& photoMetrInt,
                              Uint16& bitsAlloc,
                              Uint16& bitsStored,
                              Uint16& highBit,
                              Uint16& pixelRepr,
                              Uint16& planConf,
                              Uint16& pixAspectH,
                              Uint16& pixAspectV,
                              char*& pixData,
                              Uint32& length,
                              E_TransferSyntax& ts);

    /** After reading of pixel data, this function can be used for getting
     *  information about lossy compression parameters.
     *  @param srcEncodingLossy - [out] Denotes, whether the encoding of the pixel
     *                            data read was lossy (OFTrue) or lossless (OFFalse)
     *  @param srcLossyComprMethod - [out] Denotes the lossy compression method used
     *                               in source if there is one (srcEncodingLossy = OFTrue).
     *                               Should use defined terms of attribute Lossy
     *                               Compression Method.
     *  @return EC_Normal if information is available, error otherwise
     */
    OFCondition getLossyComprInfo(OFBool& srcEncodingLossy, OFString& srcLossyComprMethod) const;




    /**  If enabled, APPn markers (except JFIF!) are also copied to the output file.
     *   Default: false
     *   @param enabled - [in] OFTrue: copy APPn, OFFalse: cut off APPn info
     *   @return none
     */
    void setKeepAPPn(const OFBool enabled);

    /**  If enabled, COM marker is also copied to the output file.
     *   Default: true
     *   @param enabled - [in] OFTrue: copy COM, OFFalse: cut off COM segment
     *   @return none
     */
    void setKeepCOM(const OFBool enabled);

    /** Destructor, frees some memory.
     *  @return none
     */
    ~I2DJpegLsSource();

protected:
    /// 
    I2DJpegUtil m_jpegUtil;

    /// After reading pixel data, this denotes whether the source
    /// data is already lossy compressed
    OFBool m_lossyCompressed;
};

#endif // I2DJPGLSS_H
