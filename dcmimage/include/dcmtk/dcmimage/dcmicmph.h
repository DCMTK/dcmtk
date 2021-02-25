/*
 *
 *  Copyright (C) 2018-2021, OFFIS e.V.
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
 *  Module:  dcmimage
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: A helper class for comparing an original image with
 *    a processed version (e.g. after lossy compression)
 *
 */

#ifndef DCMICMPH_H
#define DCMICMPH_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmimage/dicdefin.h" /* for export macros */
#include "dcmtk/dcmimgle/diutils.h"  /* for EW_WindowType */
#include "dcmtk/ofstd/ofstd.h"       /* for OFCondition */
#include "dcmtk/ofstd/ofcmdln.h"     /* for OFCmdUnsignedInt */
#include "dcmtk/dcmdata/dcxfer.h"    /* for E_TransferSyntax */

#include <cmath>

class DicomImage;
class DcmFileFormat;
class DcmItem;

/** A helper class for comparing an original image with
 *  a processed version (e.g. after lossy compression).
 */
class DCMTK_DCMIMAGE_EXPORT DicomImageComparison
{
public:
  /// default constructor
  DicomImageComparison();

  /// destructor
  virtual ~DicomImageComparison();

  /** instruct the DicomImageComparison instance that the creation
   *  of a difference image during image comparison is requested.
   *  This function must be called prior to the call to readReferenceImage().
   *  @param amplify factor by which all pixel values in the difference image will be multiplied
   *  @return EC_Normal if successful, and error code otherwise
   */
  OFCondition initializeDiffImage(OFCmdFloat amplify = 1.0);

  /** save the difference image created during image comparison.
   *  This function must be called after to the call to computeImageComparisonMetrics().
   *  @param ofname filename to be created
   *  @return EC_Normal if successful, and error code otherwise
   */
  OFCondition saveDiffImage(const char *ofname);

  /** read reference image for image comparison from file
   *  @param ifname filename for DcmFileFormat::loadFile()
   *  @param xfer DICOM transfer syntax for DcmFileFormat::loadFile()
   *  @param readMode file read mode for DcmFileFormat::loadFile()
   *  @param compatibilityMode compatibility mode for DicomImage constructor
   *  @return EC_Normal if successful, and error code otherwise
   */
  OFCondition readReferenceImage(
    const char *ifname,
    E_TransferSyntax xfer,
    E_FileReadMode readMode,
    unsigned long compatibilityMode);

  /** read test image for image comparison from file
   *  @param ifname filename for DcmFileFormat::loadFile()
   *  @param xfer DICOM transfer syntax for DcmFileFormat::loadFile()
   *  @param readMode file read mode for DcmFileFormat::loadFile()
   *  @param compatibilityMode compatibility mode for DicomImage constructor
   *  @return EC_Normal if successful, and error code otherwise
   */
  OFCondition readTestImage(
    const char *ifname,
    E_TransferSyntax xfer,
    E_FileReadMode readMode,
    unsigned long compatibilityMode);

  /** configure the display pipelines for both referenced and test image.
   *  @param windowType VOI LUT transformation to apply
   *  @param sharedWindow compute common min/max window for both images if true, separate windows if false
   *  @param windowParameter index of VOI LUT/window in file (if windowType is EWT_window_from_file or EWT_voi_lut_from_file)
   *  @param windowCenter window center (ignored unless windowType is EWT_window_parameters)
   *  @param windowWidth window width (ignored unless windowType is EWT_window_parameters)
   *  @param voiFunction VOI LUT function (linear or sigmoid)
   *  @param presShape Presentation LUT Shape (identity, reverse, lin OD)
   *  @return EC_Normal if successful, and error code otherwise
   */
  OFCondition configureImages(
    EW_WindowType windowType,
    OFBool sharedWindow,
    OFCmdUnsignedInt windowParameter,
    OFCmdFloat windowCenter,
    OFCmdFloat windowWidth,
    EF_VoiLutFunction voiFunction,
    ES_PresentationLut presShape);

  /** compare both images and compute image comparion metrics.
   *  @param windowType VOI LUT transformation to apply
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition computeImageComparisonMetrics(EW_WindowType windowType);

  /// return maximum absolute error
  unsigned long getMaxAbsoluteError() const
  {
    return max_error;
  }

  /// return mean absolute error
  double getMeanAbsoluteError() const
  {
    return meanAbsoluteError;
  }

  /// return root mean square error
  double getRootMeanSquareError() const
  {
    return rootMeanSquareError;
  }

  /// return peak signal to noise ratio
  double getPeakSignalToNoiseRatio() const
  {
    return peakSignalToNoiseRatio;
  }

  /// return signal to noise ratio
  double getSignalToNoiseRatio() const
  {
    return signalToNoiseRatio;
  }

private:

  /// private undefined copy constructor
  DicomImageComparison(const DicomImageComparison& rhs);

  /// private undefined copy assignment operator
  DicomImageComparison& operator=(const DicomImageComparison& arg);

  /** read DICOM image and store pointer to it
   *  @param di pointer to DICOM image, not NULL upon success, NULL otherwise
   *  @param isReference indicates whether we're reading the reference image or the test image
   *  @param ifname filename for DcmFileFormat::loadFile()
   *  @param xfer DICOM transfer syntax for DcmFileFormat::loadFile()
   *  @param readMode file read mode for DcmFileFormat::loadFile()
   *  @param compatibilityMode compatibility mode for DicomImage constructor
   *  @param diffImage pointer to DICOM dataset for difference image, may be NULL
   *  @return EC_Normal if successful, and error code otherwise
   */
  static OFCondition readDICOMImage(
    DicomImage *& di,
    OFBool isReference,
    const char *ifname,
    E_TransferSyntax xfer,
    E_FileReadMode readMode,
    unsigned long compatibilityMode,
    DcmItem *diffImage);

  /** configure the display pipeline for a given DICOM image.
   *  @param di DICOM image, must not be NULL
   *  @param windowType VOI LUT transformation to apply
   *  @param windowParameter index of VOI LUT/window in file (if windowType is EWT_window_from_file or EWT_voi_lut_from_file)
   *  @param windowCenter window center (input for EWT_window_parameters, output for EWT_window_min_max and EWT_window_min_max_n)
   *  @param windowWidth window width  (input for EWT_window_parameters, output for EWT_window_min_max and EWT_window_min_max_n)
   *  @param voiFunction VOI LUT function (linear or sigmoid)
   *  @param presShape Presentation LUT Shape (identity, reverse, lin OD)
   *  @param depth bit depth to be used when accessing pixel data for this image returned in this parameter
   *  @return EC_Normal if successful, and error code otherwise
   */
  static OFCondition configureImage(
    DicomImage *di,
    EW_WindowType windowType,
    OFCmdUnsignedInt windowParameter,
    OFCmdFloat& windowCenter,
    OFCmdFloat& windowWidth,
    EF_VoiLutFunction voiFunction,
    ES_PresentationLut presShape,
    int& depth);

  /** initialize the difference image DICOM dataset by copying all required
   *  attributes from the reference image and by generating all other attributes
   *  except for the image pixel module, SOP Class UID, and the information to be copied
   *  from the test image.
   *  @param src reference image main dataset
   *  @param diffImage difference image main dataset
   */
  static OFCondition copyReferenceImageAttributes(DcmItem *src, DcmItem *diffImage);

  /** further populate the difference image DICOM dataset by copying all required
   *  attributes from the test image.
   *  @param src reference image main dataset
   *  @param diffImage difference image main dataset
   */
  static OFCondition copyTestImageAttributes(DcmItem *src, DcmItem *diffImage);

  /** print the characteristics of the image to to logger at DEBUG level.
   *  @param di DICOM image
   *  @param filename filename from which the image was loaded
   */
  static void logImageInfo(DicomImage *di, const char *filename);

  /** create content for frame label vector
   *  @param s string to which the frame label vector is written
   *  @param numFrames number of frames present in the reference image
   *  @param isColor true if reference image is a color image
   */
  static void createFrameLabelVector(OFString& s, unsigned long numFrames, OFBool isColor);

  /** check if reference and test image have the same resolution,
   *  number of frames and color model (color or monochrome).
   *  @return EC_Normal if matching, an error code otherwise.
   */
  OFCondition checkImageCharacteristics() const;

  /** compare two images that are both monochrome and have more than
   *  8 bits/pixel in internal storage, as determined by BitsStored and
   *  the Modality LUT that may have been applied.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition computeMonochromeImageComparionMetricsWord();

  /** compare two monochrome images without setting a VOI window.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition computeMonochromeImageComparionMetricsRaw();

  /** compare two images that are both monochrome and have at most
   *  8 bits/pixel in internal storage, as determined by BitsStored and
   *  the Modality LUT that may have been applied.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition computeMonochromeImageComparionMetricsByte();

  /** compare two images that are both color images.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition computeColorImageComparionMetrics();

  /// pointer to reference image
  DicomImage *di_reference;

  /// pointer to test image
  DicomImage *di_test;

  /// difference image in DICOM format
  DcmFileFormat *diff_image;

  /** result of image comparison: max difference between a sample value
   *  in the reference image and the corresponding sample value in the test image
   */
  unsigned long max_error;

  /// bits/sample to use when processing the reference image
  int reference_bits;

  /// bits/sample to use when processing the test image
  int test_bits;

  /// result of image comparison: mean absolute error
  double meanAbsoluteError;

  /// result of image comparison: root mean square error (RMSE)
  double rootMeanSquareError;

  /// result of image comparison: peak signal to noise ratio (PSNR)
  double peakSignalToNoiseRatio;

  /// result of image comparison: signal to noise ratio (SNR)
  double signalToNoiseRatio;

  /// factor by which all pixel values of the difference image will be multiplied
  double amplification;
};


#endif
