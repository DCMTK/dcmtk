/*
 *
 *  Copyright (C) 2021-2023, OFFIS e.V.
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
 *  Authors: Marco Eichelberg
 *
 *  Purpose: A helper class for comparing an original image with
 *    a processed version (e.g. after lossy compression)
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmimage/dcmicmph.h"  /* header file for this implementation file */

#include "dcmtk/ofstd/ofconapp.h"        /* for OFConsoleApplication */
#include "dcmtk/ofstd/ofcmdln.h"         /* for OFCommandLine */
#include "dcmtk/ofstd/ofstd.h"           /* for OFStandard */
#include "dcmtk/dcmdata/dctk.h"          /* for various dcmdata headers */
#include "dcmtk/dcmdata/cmdlnarg.h"      /* for prepareCmdLineArgs */
#include "dcmtk/dcmdata/dcuid.h"         /* for dcmtk version name */
#include "dcmtk/dcmimgle/dcmimage.h"     /* for DicomImage */
#include "dcmtk/dcmimage/dilogger.h"     /* for the logger macros */
#include "dcmtk/dcmimage/diregist.h"     /* include to support color images */

#ifdef WITH_ZLIB
#include <zlib.h>                        /* for zlibVersion() */
#endif

// ********************************************

DicomImageComparison::DicomImageComparison()
: di_reference(NULL)
, di_test(NULL)
, diff_image(NULL)
, max_error(0)
, reference_bits(0)
, test_bits(0)
, meanAbsoluteError(0.0)
, rootMeanSquareError(0.0)
, peakSignalToNoiseRatio(0.0)
, signalToNoiseRatio(0.0)
, amplification(1.0)
{
}


DicomImageComparison::~DicomImageComparison()
{
  delete di_reference;
  delete di_test;
  delete diff_image;
}


OFCondition DicomImageComparison::initializeDiffImage(OFCmdFloat amplify)
{
    // This function can only be called prior to the call to readReferenceImage().
    if (di_reference) return EC_IllegalCall;

    amplification = amplify;
    delete diff_image;
    diff_image = new DcmFileFormat();
    return EC_Normal;
}


OFCondition DicomImageComparison::saveDiffImage(const char *ofname)
{
  if (ofname == NULL) return EC_IllegalCall;
  if (diff_image == NULL) return EC_IllegalCall;
  return diff_image->saveFile(ofname, EXS_LittleEndianExplicit);
}


OFCondition DicomImageComparison::readDICOMImage(
    DicomImage *& di,
    OFBool isReference,
    const char *ifname,
    E_TransferSyntax xfer,
    E_FileReadMode readMode,
    unsigned long compatibilityMode,
    DcmItem *diffImage)
{

    // make sure that we do not create a memory leak
    compatibilityMode |= (CIF_MayDetachPixelData | CIF_TakeOverExternalDataset) ;

    // since we process all frames anyway, decompress the complete pixel data (if required)
    compatibilityMode |= CIF_DecompressCompletePixelData;

    if (ifname == NULL) return EC_IllegalCall;

    DCMIMAGE_INFO("reading DICOM file: " << ifname);

    // load DICOM file as DcmFileFormat
    DcmFileFormat *dfile = new DcmFileFormat();
    OFCondition cond = dfile->loadFile(ifname, xfer, EGL_withoutGL, DCM_MaxReadLength, readMode);
    if (cond.bad())
    {
        DCMIMAGE_ERROR(cond.text() << ": reading file: " << ifname);
        delete dfile;
    } else {
        // determine transfer syntax
        E_TransferSyntax original_xfer = dfile->getDataset()->getOriginalXfer();

        // The DicomImage instance takes ownership of the DcmFileFormat instance. Do not delete manually.
        di = new DicomImage(dfile, original_xfer, compatibilityMode);

        // check status of the DICOM image object
        if (di->getStatus() == EIS_Normal)
        {
            logImageInfo(di, ifname);
            // initialize diff image from reference and test dataset
            if (diffImage)
            {
                if (isReference)
                    cond = copyReferenceImageAttributes(dfile->getDataset(), diffImage);
                    else cond = copyTestImageAttributes(dfile->getDataset(), diffImage);
            }
        } else {
            // Creation of DicomImage object failed. Bail out.
            const char *msg = DicomImage::getString(di->getStatus());
            DCMIMAGE_ERROR(msg);
            cond = makeOFCondition(OFM_dcmimage, 129, OF_error, msg);
            delete di;
            di = NULL;
        }
    }
    return cond;
}


OFCondition DicomImageComparison::readReferenceImage(
  const char *ifname,
  E_TransferSyntax xfer,
  E_FileReadMode readMode,
  unsigned long compatibilityMode)
{
    delete di_reference;
    di_reference = NULL;
    DcmItem *di = NULL;
    if (diff_image) di = diff_image->getDataset();
    return readDICOMImage(di_reference, OFTrue, ifname, xfer, readMode, compatibilityMode, di);
}


OFCondition DicomImageComparison::readTestImage(
  const char *ifname,
  E_TransferSyntax xfer,
  E_FileReadMode readMode,
  unsigned long compatibilityMode)
{
    delete di_test;
    di_test = NULL;
    DcmItem *di = NULL;
    if (diff_image) di = diff_image->getDataset();
    return readDICOMImage(di_test, OFFalse, ifname, xfer, readMode, compatibilityMode, di);
}


OFCondition DicomImageComparison::configureImage(
    DicomImage *di,
    EW_WindowType windowType,
    OFCmdUnsignedInt windowParameter,
    OFCmdFloat& windowCenter,
    OFCmdFloat& windowWidth,
    EF_VoiLutFunction voiFunction,
    ES_PresentationLut presShape,
    int& depth)
{
    /* check parameters */
    if (di == NULL) return EC_IllegalCall;

    /* hide overlays*/
    di->hideAllOverlays();

    /* determine bit depth to request when processing the pixel data */
    depth = di->getDepth();
    if (depth > 16) depth = 16;
    if (depth < 8) depth = 8;

    /* process VOI parameters */
    switch (windowType)
    {
        case EWT_none: /* no VOI windowing */
            if (di->isMonochrome())
            {
                DCMIMAGE_DEBUG("disabling VOI window computation");
                if (!di->setNoVoiTransformation())
                    DCMIMAGE_WARN("cannot ignore VOI window");
            }
            break;
        case EWT_window_minmax: /* compute VOI window using min-max algorithm */
            if (!di->setMinMaxWindow(0))
                DCMIMAGE_WARN("cannot compute min/max VOI window");
            if (!di->getWindow(windowCenter, windowWidth))
                DCMIMAGE_WARN("cannot retrieve VOI window");
            DCMIMAGE_DEBUG("activating min/max VOI window center=" << windowCenter << ", width=" << windowWidth);
            break;
        case EWT_window_minmax_n: /* compute VOI window using min-max algorithm ignoring extremes */
            if (!di->setMinMaxWindow(0))
                DCMIMAGE_WARN("cannot compute min/max VOI window");
            if (!di->getWindow(windowCenter, windowWidth))
                DCMIMAGE_WARN("cannot retrieve VOI window");
            DCMIMAGE_DEBUG("activating min/max-n VOI window center=" << windowCenter << ", width=" << windowWidth);
            break;
        case EWT_window_from_file: /* use the n-th VOI window from the image file */
            if ((windowParameter < 1) || (windowParameter > di->getWindowCount()))
            {
                DCMIMAGE_FATAL("cannot select VOI window " << windowParameter << ", only "
                    << di->getWindowCount() << " window(s) in file");
                return makeOFCondition(OFM_dcmimage, 130, OF_error, "cannot select VOI window");
            }
            DCMIMAGE_DEBUG("activating VOI window " << windowParameter);
            if (!di->setWindow(windowParameter - 1))
                DCMIMAGE_WARN("cannot select VOI window " << windowParameter);
            break;
        case EWT_voi_lut_from_file: /* use the n-th VOI look up table from the image file */
            if ((windowParameter < 1) || (windowParameter > di->getVoiLutCount()))
            {
                DCMIMAGE_FATAL("cannot select VOI LUT " << windowParameter << ", only "
                    << di->getVoiLutCount() << " LUT(s) in file");
                return makeOFCondition(OFM_dcmimage, 131, OF_error, "cannot select VOI LUT");
            }
            DCMIMAGE_DEBUG("activating VOI LUT " << windowParameter);
            if (!di->setVoiLut(windowParameter - 1, ELM_UseValue))
                DCMIMAGE_WARN("cannot select VOI LUT " << windowParameter);
            break;
        case EWT_window_center_width: /* Compute VOI window using center and width */
            DCMIMAGE_DEBUG("activating VOI window center=" << windowCenter << ", width=" << windowWidth);
            if (!di->setWindow(windowCenter, windowWidth))
                DCMIMAGE_WARN("cannot set VOI window to specified values");
            break;
        default: /* unsupported type */
            DCMIMAGE_FATAL("unsupported type of VOI LUT transformation requested.");
            return makeOFCondition(OFM_dcmimage, 132, OF_error, "unsupported type of VOI LUT transformation requested");
            break;
    }

    /* VOI LUT function */
    if (voiFunction != EFV_Default)
    {
        if (voiFunction == EFV_Linear)
            DCMIMAGE_DEBUG("setting VOI LUT function to LINEAR");
        else if (voiFunction == EFV_Sigmoid)
            DCMIMAGE_DEBUG("setting VOI LUT function to SIGMOID");
        if (!di->setVoiLutFunction(voiFunction))
            DCMIMAGE_WARN("cannot set VOI LUT function");
    }

    /* process presentation LUT parameters */
    if (presShape != ESP_Default)
    {
        if (presShape == ESP_Identity)
            DCMIMAGE_DEBUG("setting presentation LUT shape to IDENTITY");
        else if (presShape == ESP_Inverse)
            DCMIMAGE_DEBUG("setting presentation LUT shape to INVERSE");
        else if (presShape == ESP_LinOD)
            DCMIMAGE_DEBUG("setting presentation LUT shape to LIN OD");
        if (!di->setPresentationLutShape(presShape))
            DCMIMAGE_WARN("cannot set presentation LUT shape");
    }

    return EC_Normal;
}


OFCondition DicomImageComparison::configureImages(
    EW_WindowType windowType,
    OFBool sharedWindow,
    OFCmdUnsignedInt windowParameter,
    OFCmdFloat windowCenter,
    OFCmdFloat windowWidth,
    EF_VoiLutFunction voiFunction,
    ES_PresentationLut presShape)
{
  OFCondition cond = configureImage(di_reference, windowType, windowParameter, windowCenter, windowWidth, voiFunction, presShape, reference_bits);

  // When the user has requested a shared min/max window, re-use the VOI window computed for the reference image also for the test image
  EW_WindowType windowTypeTestImage = windowType;
  if (sharedWindow && ((windowType == EWT_window_minmax)||(windowType == EWT_window_minmax_n)))
      windowTypeTestImage = EWT_window_center_width;

  if (cond.good()) cond = configureImage(di_test, windowTypeTestImage, windowParameter, windowCenter, windowWidth, voiFunction, presShape, test_bits);

  if (di_reference->isMonochrome() && (windowType != EWT_none))
  {
      // If a VOI window is applied, use the maximum of the bit depths
      // determined by configureImage() for both images.
      if (reference_bits > test_bits)
          test_bits = reference_bits;
          else reference_bits = test_bits;
  }

  DCMIMAGE_DEBUG("Bits/sample selected for reference image: " << reference_bits);
  DCMIMAGE_DEBUG("Bits/sample selected for test image: " << test_bits);
  return cond;
}

OFCondition DicomImageComparison::checkImageCharacteristics() const
{
  //check if images have actually been loaded
  if (di_reference == NULL) return EC_IllegalCall;
  if (di_test == NULL) return EC_IllegalCall;

  // check resolution
  if ((di_reference->getWidth() != di_test->getWidth()) || (di_reference->getHeight() != di_test->getHeight()))
  {
      DCMIMAGE_FATAL("Resolution mismatch: "
        << di_reference->getWidth() << " x " << di_reference->getHeight() << " vs. "
        << di_test->getWidth() << " x " << di_test->getHeight());
      return makeOFCondition(OFM_dcmimage, 133, OF_error, "resolution mismatch between reference and test image");
  }

  // check color model
  if ((di_reference->isMonochrome() && (! di_test->isMonochrome())) || (di_test->isMonochrome() && (! di_reference->isMonochrome())))
  {
      const char *colorModel1 =  di_reference->getString(di_reference->getPhotometricInterpretation());
      if (colorModel1 == NULL) colorModel1 = "unknown";

      const char *colorModel2 =  di_test->getString(di_test->getPhotometricInterpretation());
      if (colorModel2 == NULL) colorModel2 = "unknown";

      DCMIMAGE_FATAL("Color model mismatch: "
        << colorModel1 << " vs. " << colorModel2);
      return makeOFCondition(OFM_dcmimage, 133, OF_error, "color model mismatch between reference and test image");
  }

  // check number of frames to process
  if (di_reference->getFrameCount() != di_test->getFrameCount())
  {
      DCMIMAGE_FATAL("Frame count mismatch: "
        << di_reference->getFrameCount() << " vs. " << di_test->getFrameCount());
      return makeOFCondition(OFM_dcmimage, 133, OF_error, "frame count mismatch between reference and test image");
  }

  return EC_Normal;
}


OFCondition DicomImageComparison::computeImageComparisonMetrics(EW_WindowType windowType)
{
    DCMIMAGE_DEBUG("Computing image comparison metrics");

    // initialize comparison results
    max_error = 0;
    meanAbsoluteError = 0.0;
    rootMeanSquareError = 0.0;
    peakSignalToNoiseRatio = 0.0;
    signalToNoiseRatio = 0.0;

    OFCondition cond = checkImageCharacteristics();
    if (cond.good())
    {
       // we don't need to check if di_reference == NULL,
       // because checkImageCharacteristics() already did that for us.
       if (di_reference->isMonochrome() && ((reference_bits > 8) || (windowType == EWT_none)))
       {
              // monochrome image with more than 8 bits/pixel
              if (diff_image)
              {
                  // monochrome image with more than 8 bits/pixel
                  // write Multiframe Grayscale Word SC Image
                  cond = diff_image->getDataset()->putAndInsertString(DCM_SOPClassUID, UID_MultiframeGrayscaleWordSecondaryCaptureImageStorage);
                  if (cond.bad()) return cond;
                  cond = diff_image->getDataset()->putAndInsertUint16(DCM_BitsAllocated, 16);
                  if (cond.bad()) return cond;
                  cond = diff_image->getDataset()->putAndInsertUint16(DCM_BitsStored, 16);
                  if (cond.bad()) return cond;
                  cond = diff_image->getDataset()->putAndInsertUint16(DCM_HighBit, 15);
                  if (cond.bad()) return cond;
              }
              if (windowType == EWT_none)
                  cond = computeMonochromeImageComparionMetricsRaw();
                  else cond = computeMonochromeImageComparionMetricsWord();
       }
       else
       {
          if (diff_image)
          {
              // image with 8 bits/pixel (color or monochrome)
              // the difference image is always be a Multiframe Grayscale Byte SC Image
              cond = diff_image->getDataset()->putAndInsertString(DCM_SOPClassUID, UID_MultiframeGrayscaleByteSecondaryCaptureImageStorage);
              if (cond.bad()) return cond;
              cond = diff_image->getDataset()->putAndInsertUint16(DCM_BitsAllocated, 8);
              if (cond.bad()) return cond;
              cond = diff_image->getDataset()->putAndInsertUint16(DCM_BitsStored, 8);
              if (cond.bad()) return cond;
              cond = diff_image->getDataset()->putAndInsertUint16(DCM_HighBit, 7);
              if (cond.bad()) return cond;
          }
          if (di_reference->isMonochrome())
              cond = computeMonochromeImageComparionMetricsByte();
              else cond = computeColorImageComparionMetrics();
       }
    }
    return cond;
}


OFCondition DicomImageComparison::computeMonochromeImageComparionMetricsWord()
{
    DCMIMAGE_DEBUG("Type of computation: monochrome, 16-bits/sample");

    // this check should have already been performed, but just to be sure, and since it's cheap...
    OFCondition cond = checkImageCharacteristics();
    if (cond.good() && di_reference->isMonochrome() && (reference_bits > 8))
    {
        unsigned long numBytes = di_reference->getOutputDataSize(reference_bits);
        unsigned int fcount = di_reference->getFrameCount();
        if (numBytes != di_test->getOutputDataSize(test_bits))
        {
            DCMIMAGE_FATAL("Frame size mismatch: "
                << numBytes << " vs. " << di_test->getOutputDataSize(test_bits) << " bytes");
            return makeOFCondition(OFM_dcmimage, 134, OF_error, "frame size mismatch");
        }
        unsigned long numValues = (numBytes+1) / sizeof(Uint16);

        // prepare creation of difference image pixel data
        Uint16 *dv = NULL;
        double dvp;
        if (diff_image)
        {
            // create empty pixel data element and insert into main dataset
            DcmPixelData *pxd = new DcmPixelData(DCM_PixelData);
            cond = diff_image->getDataset()->insert(pxd);
            if (cond.bad()) return cond;

            // create pixel data buffer for all frames of the difference image
            Uint32 pixelDataLen = numValues * fcount;
            cond = pxd->createUint16Array(pixelDataLen, dv);
            if (cond.bad()) return cond;
        }

        const Uint16 *f1;
        const Uint16 *f2;

        double square_error_sum = 0; // sum of squared differences between reference and test
        double simple_error_sum = 0; // sum of (unsigned) differences between reference and test
        double square_reference_sum = 0; // sum of squared pixel values in reference image
        unsigned long square_signal_strength = 0; // maximum pixel value in reference image
        max_error = 0; // maximum (unsigned) difference between reference and test pixel value

        // iterate over all frames
        for (unsigned int frame = 0; frame < fcount; frame++)
        {
            DCMIMAGE_DEBUG("Processing frame " << frame);
            const void *frame1 = di_reference->getOutputData(16, frame);
            const void *frame2 = di_test->getOutputData(16, frame);
            if ((frame1 == NULL) || (frame2 == NULL))
            {
              DCMIMAGE_FATAL("Memory exhausted while accessing frames");
              return EC_MemoryExhausted;
            }
            f1 = OFreinterpret_cast(const Uint16 *, frame1);
            f2 = OFreinterpret_cast(const Uint16 *, frame2);
            long i1, i2;
            unsigned long i3, t;

            // iterate over all samples (pixel values) of the current frame
            for (unsigned long i = 0; i < numValues; ++i)
            {
              i1 = *f1; // pixel value in the reference image
              i2 = *f2; // pixel value in the test image
              i3 = OFstatic_cast(unsigned long, labs(i1 - i2)); // absolute difference, without sign
              if (dv)
              {
                  dvp = amplification * i3;  // compute diff image pixel value
                  if (dvp > 65535)
                      *dv++ = OFstatic_cast(Uint16, 65535);
                      else *dv++ = OFstatic_cast(Uint16, dvp);

              }
              max_error = (i3 > max_error ? i3 : max_error);
              square_error_sum += (double)(i3*i3);
              simple_error_sum += i3;
              t = i1 * i1;
              square_reference_sum += t;
              square_signal_strength = (t > square_signal_strength) ? t : square_signal_strength;
              ++f1;
              ++f2;
            }
        }

        // The following four metrics, MAE, RMSE, PSNR and SNR are computed as defined in
        // R.C. Gonzalez and R.E. Woods, "Digital Image Processing," Prentice Hall 2008

        DCMIMAGE_DEBUG("square_signal_strength: " << square_signal_strength);
        DCMIMAGE_DEBUG("square_error_sum: " << square_error_sum);

        // as a helper variable, divide the sum of squared errors by the total number of samples
        double meanSquareError = square_error_sum / (numValues * fcount);

        // the mean absolute error is the sum of (unsigned) error values divided by the total number of samples
        meanAbsoluteError = simple_error_sum / (numValues * fcount);

        // RMSE is the square root of the mean square error
        rootMeanSquareError = sqrt(meanSquareError);

        // PSNR is -10 * the logarithm of the mean square error divided by the squared signal strength
        //(maximum pixel value in reference image)
        peakSignalToNoiseRatio = log10(meanSquareError / square_signal_strength) * -10.0;

        // SNR is the 10 * the logarithm of the signal (squared sum of all reference pixel values)
        // divided by the noise (squared sum of all difference values)
        signalToNoiseRatio =  log10(square_reference_sum / square_error_sum) * 10.0;

        if (diff_image)
        {
            char buf[30];
            OFStandard::snprintf(buf, 30, "%lu", OFstatic_cast(unsigned long, di_reference->getFrameCount()));
            cond = diff_image->getDataset()->putAndInsertUint16(DCM_Rows, OFstatic_cast(Uint16, di_reference->getHeight()));
            if (cond.good()) cond = diff_image->getDataset()->putAndInsertUint16(DCM_Columns, OFstatic_cast(Uint16, di_reference->getWidth()));
            if (cond.good()) cond = diff_image->getDataset()->putAndInsertString(DCM_NumberOfFrames, buf);
            if (di_reference->getFrameCount() > 1)
            {
                DcmTagKey frameIncrementPointer(0x0018, 0x2002);
                if (cond.good()) cond = diff_image->getDataset()->putAndInsertTagKey(DCM_FrameIncrementPointer, frameIncrementPointer);
                OFString frameLabelVector;
                createFrameLabelVector(frameLabelVector, di_reference->getFrameCount(), OFFalse);
                if (cond.good()) cond = diff_image->getDataset()->putAndInsertOFStringArray(DCM_FrameLabelVector, frameLabelVector);
            }
        }
    }
    return cond;
}

OFCondition DicomImageComparison::computeMonochromeImageComparionMetricsRaw()
{
    DCMIMAGE_DEBUG("Type of computation: monochrome, without VOI transformation");

    // this check should have already been performed, but just to be sure, and since it's cheap...
    OFCondition cond = checkImageCharacteristics();
    if (cond.good() && di_reference->isMonochrome())
    {
        // get pointer to internal image representation
        // (which means Modality LUT has been applied unless disabled, but nothing else)
        const DiPixel *dinter_reference = di_reference->getInterData();
        if (dinter_reference == NULL) return EC_IllegalCall;
        const DiPixel *dinter_test = di_test->getInterData();
        if (dinter_test == NULL) return EC_IllegalCall;

        // get pointer to internal raw representations of image data
        const void *draw_reference = dinter_reference->getData();
        if (draw_reference == NULL) return EC_IllegalCall;
        const void *draw_test = dinter_test->getData();
        if (draw_test == NULL) return EC_IllegalCall;

        // get internal representations used for both images
        EP_Representation drep_reference = dinter_reference->getRepresentation();
        EP_Representation drep_test = dinter_test->getRepresentation();

        // determine number of pixels
        unsigned int fcount = di_reference->getFrameCount();
        unsigned long numValues = di_reference->getWidth() * di_reference->getHeight() * fcount;
        if (dinter_reference->getCount() < numValues)
        {
            DCMIMAGE_FATAL("Insufficient pixel data in reference image: " << numValues << " needed, " << dinter_reference->getCount() << " found.");
            return makeOFCondition(OFM_dcmimage, 138, OF_error, "insufficient pixel data in reference image");
        }
        if (dinter_test->getCount() < numValues)
        {
            DCMIMAGE_FATAL("Insufficient pixel data in test image: " << numValues << " needed, " << dinter_reference->getCount() << " found.");
            return makeOFCondition(OFM_dcmimage, 139, OF_error, "insufficient pixel data in test image");
        }

        // prepare creation of difference image pixel data
        Uint16 *dv = NULL;
        double dvp;
        if (diff_image)
        {
            // create empty pixel data element and insert into main dataset
            DcmPixelData *pxd = new DcmPixelData(DCM_PixelData);
            cond = diff_image->getDataset()->insert(pxd);
            if (cond.bad()) return cond;

            // create pixel data buffer for all frames of the difference image
            Uint32 pixelDataLen = numValues;
            cond = pxd->createUint16Array(pixelDataLen, dv);
            if (cond.bad()) return cond;
        }

        // create pointers to reference image pixel data in all possible formats
        const Uint8  *ref_as_uint8  = OFreinterpret_cast(const Uint8  *, draw_reference);
        const Sint8  *ref_as_sint8  = OFreinterpret_cast(const Sint8  *, draw_reference);
        const Uint16 *ref_as_uint16 = OFreinterpret_cast(const Uint16 *, draw_reference);
        const Sint16 *ref_as_sint16 = OFreinterpret_cast(const Sint16 *, draw_reference);

        // create pointers to test image pixel data in all possible formats
        const Uint8  *test_as_uint8  = OFreinterpret_cast(const Uint8  *, draw_test);
        const Sint8  *test_as_sint8  = OFreinterpret_cast(const Sint8  *, draw_test);
        const Uint16 *test_as_uint16 = OFreinterpret_cast(const Uint16 *, draw_test);
        const Sint16 *test_as_sint16 = OFreinterpret_cast(const Sint16 *, draw_test);

        double square_error_sum = 0; // sum of squared differences between reference and test
        double simple_error_sum = 0; // sum of (unsigned) differences between reference and test
        double square_reference_sum = 0; // sum of squared pixel values in reference image
        unsigned long square_signal_strength = 0; // maximum pixel value in reference image
        max_error = 0; // maximum (unsigned) difference between reference and test pixel value

        long i1, i2;
        unsigned long i3, t;

        // iterate over all samples (pixel values) of all frames
        for (unsigned int i = 0; i < numValues; i++)
        {
            switch(drep_reference)
            {
                case EPR_Uint8: // image representation is 8 bit unsigned
                    i1 = *ref_as_uint8++;
                    break;
                case EPR_Sint8: // image representation is 8 bit signed
                    i1 = *ref_as_sint8++;
                    break;
                case EPR_Uint16: // image representation is 16 bit unsigned
                    i1 = *ref_as_uint16++;
                    break;
                case EPR_Sint16: // image representation is 16 bit signed
                    i1 = *ref_as_sint16++;
                    break;
                default:
                    return makeOFCondition(OFM_dcmimage, 140, OF_error, "unsupported internal pixel representation");
                    break;
            }
            switch(drep_test)
            {
                case EPR_Uint8: // image representation is 8 bit unsigned
                    i2 = *test_as_uint8++;
                    break;
                case EPR_Sint8: // image representation is 8 bit signed
                    i2 = *test_as_sint8++;
                    break;
                case EPR_Uint16: // image representation is 16 bit unsigned
                    i2 = *test_as_uint16++;
                    break;
                case EPR_Sint16: // image representation is 16 bit signed
                    i2 = *test_as_sint16++;
                    break;
                default:
                    return makeOFCondition(OFM_dcmimage, 141, OF_error, "unsupported internal pixel representation");
                    break;
            }

            i3 = OFstatic_cast(unsigned long, labs(i1 - i2)); // absolute difference, without sign
            if (dv)
            {
                dvp = amplification * i3;  // compute diff image pixel value
                if (dvp > 65535)
                    *dv++ = OFstatic_cast(Uint16, 65535);
                    else *dv++ = OFstatic_cast(Uint16, dvp);

            }
            max_error = (i3 > max_error ? i3 : max_error);
            square_error_sum += (double)(i3*i3);
            simple_error_sum += i3;
            t = i1 * i1;
            square_reference_sum += t;
            square_signal_strength = (t > square_signal_strength) ? t : square_signal_strength;
        }

        // The following four metrics, MAE, RMSE, PSNR and SNR are computed as defined in
        // R.C. Gonzalez and R.E. Woods, "Digital Image Processing," Prentice Hall 2008

        DCMIMAGE_DEBUG("square_signal_strength: " << square_signal_strength);
        DCMIMAGE_DEBUG("square_error_sum: " << square_error_sum);

        // as a helper variable, divide the sum of squared errors by the total number of samples
        double meanSquareError = square_error_sum / numValues;

        // the mean absolute error is the sum of (unsigned) error values divided by the total number of samples
        meanAbsoluteError = simple_error_sum / numValues;

        // RMSE is the square root of the mean square error
        rootMeanSquareError = sqrt(meanSquareError);

        // PSNR is -10 * the logarithm of the mean square error divided by the squared signal strength
        //(maximum pixel value in reference image)
        peakSignalToNoiseRatio = log10(meanSquareError / square_signal_strength) * -10.0;

        // SNR is the 10 * the logarithm of the signal (squared sum of all reference pixel values)
        // divided by the noise (squared sum of all difference values)
        signalToNoiseRatio =  log10(square_reference_sum / square_error_sum) * 10.0;

        if (diff_image)
        {
            char buf[30];
            OFStandard::snprintf(buf, 30, "%lu", OFstatic_cast(unsigned long, di_reference->getFrameCount()));
            cond = diff_image->getDataset()->putAndInsertUint16(DCM_Rows, OFstatic_cast(Uint16, di_reference->getHeight()));
            if (cond.good()) cond = diff_image->getDataset()->putAndInsertUint16(DCM_Columns, OFstatic_cast(Uint16, di_reference->getWidth()));
            if (cond.good()) cond = diff_image->getDataset()->putAndInsertString(DCM_NumberOfFrames, buf);
            if (di_reference->getFrameCount() > 1)
            {
                DcmTagKey frameIncrementPointer(0x0018, 0x2002);
                if (cond.good()) cond = diff_image->getDataset()->putAndInsertTagKey(DCM_FrameIncrementPointer, frameIncrementPointer);
                OFString frameLabelVector;
                createFrameLabelVector(frameLabelVector, di_reference->getFrameCount(), OFFalse);
                if (cond.good()) cond = diff_image->getDataset()->putAndInsertOFStringArray(DCM_FrameLabelVector, frameLabelVector);
            }
        }
    }
    return cond;
}


OFCondition DicomImageComparison::computeMonochromeImageComparionMetricsByte()
{
    DCMIMAGE_DEBUG("Type of computation: monochrome, 8-bits/sample");

    // this check should have already been performed, but just to be sure, and since it's cheap...
    OFCondition cond = checkImageCharacteristics();
    if (cond.good() && di_reference->isMonochrome())
    {
        unsigned long numBytes = di_reference->getOutputDataSize(8);
        unsigned int fcount = di_reference->getFrameCount();
        if (numBytes != di_test->getOutputDataSize(8))
        {
          DCMIMAGE_FATAL("Frame size mismatch: "
            << numBytes << " vs. " << di_test->getOutputDataSize(8) << " bytes");
          return makeOFCondition(OFM_dcmimage, 135, OF_error, "frame size mismatch");
        }
        unsigned long numValues = numBytes;

        // prepare creation of difference image pixel data
        Uint8 *dv = NULL;
        double dvp;
        if (diff_image)
        {
            // create empty pixel data element and insert into main dataset
            DcmPixelData *pxd = new DcmPixelData(DCM_PixelData);
            cond = diff_image->getDataset()->insert(pxd);
            if (cond.bad()) return cond;

            // create pixel data buffer for all frames of the difference image
            Uint32 pixelDataLen = numValues * fcount;
            cond = pxd->createUint8Array(pixelDataLen, dv);
            if (cond.bad()) return cond;
        }

        const Uint8 *f1;
        const Uint8 *f2;

        double square_error_sum = 0; // sum of squared differences between reference and test
        double simple_error_sum = 0; // sum of (unsigned) differences between reference and test
        double square_reference_sum = 0; // sum of squared pixel values in reference image
        unsigned long square_signal_strength = 0; // maximum pixel value in reference image
        max_error = 0; // maximum (unsigned) difference between reference and test pixel value

        // iterate over all frames
        for (unsigned int frame = 0; frame < fcount; frame++)
        {
            DCMIMAGE_DEBUG("Processing frame " << frame);
            const void *frame1 = di_reference->getOutputData(8, frame);
            const void *frame2 = di_test->getOutputData(8, frame);
            if ((frame1 == NULL) || (frame2 == NULL))
            {
              DCMIMAGE_FATAL("Memory exhausted while accessing frames");
              return EC_MemoryExhausted;
            }
            f1 = OFreinterpret_cast(const Uint8 *, frame1);
            f2 = OFreinterpret_cast(const Uint8 *, frame2);
            long i1, i2;
            unsigned long i3, t;

            // iterate over all samples (pixel values) of the current frame
            for (unsigned long i = 0; i < numValues; ++i)
            {
              i1 = *f1; // pixel value in the reference image
              i2 = *f2; // pixel value in the test image
              i3 = OFstatic_cast(unsigned long, labs(i1 - i2)); // absolute difference, without sign
              if (dv)
              {
                  dvp = amplification * i3;  // compute diff image pixel value
                  if (dvp > 255)
                      *dv++ = OFstatic_cast(Uint8, 255);
                      else *dv++ = OFstatic_cast(Uint8, dvp);

              }
              max_error = (i3 > max_error ? i3 : max_error);
              square_error_sum += (double)(i3*i3);
              simple_error_sum += i3;
              t = i1 * i1;
              square_reference_sum += t;
              square_signal_strength = (t > square_signal_strength) ? t : square_signal_strength;
              ++f1;
              ++f2;
            }
        }

        // The following four metrics, MAE, RMSE, PSNR and SNR are computed as defined in
        // R.C. Gonzalez and R.E. Woods, "Digital Image Processing," Prentice Hall 2008

        // as a helper variable, divide the sum of squared errors by the total number of samples
        double meanSquareError = square_error_sum / (numValues * fcount);

        // the mean absolute error is the sum of (unsigned) error values divided by the total number of samples
        meanAbsoluteError = simple_error_sum / (numValues * fcount);

        // Compute RMSE by taking the square root of the mean square error
        rootMeanSquareError = sqrt(meanSquareError);

        // PSNR is -10 * the logarithm of the mean square error divided by the squared signal strength
        //(maximum pixel value in reference image)
        peakSignalToNoiseRatio = log10(meanSquareError / square_signal_strength) * -10.0;

        // SNR is the 10 * the logarithm of the signal (squared sum of all reference pixel values)
        // divided by the noise (squared sum of all difference values)
        signalToNoiseRatio =  log10(square_reference_sum / square_error_sum) * 10.0;

        if (diff_image)
        {
            char buf[30];
            OFStandard::snprintf(buf, 30, "%lu", OFstatic_cast(unsigned long, di_reference->getFrameCount()));
            cond = diff_image->getDataset()->putAndInsertUint16(DCM_Rows, OFstatic_cast(Uint16, di_reference->getHeight()));
            if (cond.good()) cond = diff_image->getDataset()->putAndInsertUint16(DCM_Columns, OFstatic_cast(Uint16, di_reference->getWidth()));
            if (cond.good()) cond = diff_image->getDataset()->putAndInsertString(DCM_NumberOfFrames, buf);
            if (di_reference->getFrameCount() > 1)
            {
                DcmTagKey frameIncrementPointer(0x0018, 0x2002);
                if (cond.good()) cond = diff_image->getDataset()->putAndInsertTagKey(DCM_FrameIncrementPointer, frameIncrementPointer);
                OFString frameLabelVector;
                createFrameLabelVector(frameLabelVector, di_reference->getFrameCount(), OFFalse);
                if (cond.good()) cond = diff_image->getDataset()->putAndInsertOFStringArray(DCM_FrameLabelVector, frameLabelVector);
            }
        }
    }
    return cond;
}


OFCondition DicomImageComparison::computeColorImageComparionMetrics()
{
    DCMIMAGE_DEBUG("Type of computation: color, 8-bits/sample");

    // this check should have already been performed, but just to be sure, and since it's cheap...
    OFCondition cond = checkImageCharacteristics();
    if (cond.good() && (! di_reference->isMonochrome()))
    {
        unsigned long numBytes = di_reference->getOutputDataSize(8);
        unsigned int fcount = di_reference->getFrameCount();
        if (numBytes != di_test->getOutputDataSize(8))
        {
          DCMIMAGE_FATAL("Frame size mismatch: "
            << numBytes << " vs. " << di_test->getOutputDataSize(8) << " bytes");
          return makeOFCondition(OFM_dcmimage, 136, OF_error, "frame size mismatch");
        }
        unsigned long numValues = numBytes;

        // prepare creation of difference image pixel data
        Uint8 *dv = NULL;
        double dvp;
        if (diff_image)
        {
            // create empty pixel data element and insert into main dataset
            DcmPixelData *pxd = new DcmPixelData(DCM_PixelData);
            cond = diff_image->getDataset()->insert(pxd);
            if (cond.bad()) return cond;

            // create pixel data buffer for all frames of the difference image
            Uint32 pixelDataLen = numValues * fcount;
            cond = pxd->createUint8Array(pixelDataLen, dv);
            if (cond.bad()) return cond;
        }

        const Uint8 *f1;
        const Uint8 *f2;

        double square_error_sum = 0; // sum of squared differences between reference and test
        double simple_error_sum = 0; // sum of (unsigned) differences between reference and test
        double square_reference_sum = 0; // sum of squared pixel values in reference image
        unsigned long square_signal_strength = 0; // maximum pixel value in reference image
        max_error = 0; // maximum (unsigned) difference between reference and test pixel value

        // iterate over all frames
        for (unsigned int frame = 0; frame < fcount; frame++)
        {
            DCMIMAGE_DEBUG("Processing frame " << frame);
            const void *frame1 = di_reference->getOutputData(8, frame, 1 /* color by plane */);
            const void *frame2 = di_test->getOutputData(8, frame, 1 /* color by plane */);
            if ((frame1 == NULL) || (frame2 == NULL))
            {
              DCMIMAGE_FATAL("Memory exhausted while accessing frames");
              return EC_MemoryExhausted;
            }
            f1 = OFreinterpret_cast(const Uint8 *, frame1);
            f2 = OFreinterpret_cast(const Uint8 *, frame2);
            long i1, i2;
            unsigned long i3, t;

            // iterate over all samples (R, G and B pixel values) of the current frame
            for (unsigned long i = 0; i < numValues; ++i)
            {
              i1 = *f1; // pixel value in the reference image
              i2 = *f2; // pixel value in the test image
              i3 = OFstatic_cast(unsigned long, labs(i1 - i2)); // absolute difference, without sign
              if (dv)
              {
                  dvp = amplification * i3;  // compute diff image pixel value
                  if (dvp > 255)
                      *dv++ = OFstatic_cast(Uint8, 255);
                      else *dv++ = OFstatic_cast(Uint8, dvp);

              }
              max_error = (i3 > max_error ? i3 : max_error);
              square_error_sum += (double)(i3*i3);
              simple_error_sum += i3;
              t = i1 * i1;
              square_reference_sum += t;
              square_signal_strength = (t > square_signal_strength) ? t : square_signal_strength;
              ++f1;
              ++f2;
            }
        }

        // The following four metrics, MAE, RMSE, PSNR and SNR are computed as defined in
        // R.C. Gonzalez and R.E. Woods, "Digital Image Processing," Prentice Hall 2008

        // as a helper variable, divide the sum of squared errors by the total number of samples
        double meanSquareError = square_error_sum / (numValues * fcount);

        // the mean absolute error is the sum of (unsigned) error values divided by the total number of samples
        meanAbsoluteError = simple_error_sum / (numValues * fcount);

        // Compute RMSE by taking the square root of the mean square error
        rootMeanSquareError = sqrt(meanSquareError);

        // PSNR is -10 * the logarithm of the mean square error divided by the squared signal strength
        //(maximum pixel value in reference image)
        peakSignalToNoiseRatio = log10(meanSquareError / square_signal_strength) * -10.0;

        // SNR is the 10 * the logarithm of the signal (squared sum of all reference pixel values)
        // divided by the noise (squared sum of all difference values)
        signalToNoiseRatio =  log10(square_reference_sum / square_error_sum) * 10.0;

        if (diff_image)
        {
            char buf[30];
            // For color images, the difference image is always multiframe
            // because it contains separate R, G and B difference frames
            OFStandard::snprintf(buf, 30, "%lu", OFstatic_cast(unsigned long, 3UL * di_reference->getFrameCount()));
            cond = diff_image->getDataset()->putAndInsertUint16(DCM_Rows, OFstatic_cast(Uint16, di_reference->getHeight()));
            if (cond.good()) cond = diff_image->getDataset()->putAndInsertUint16(DCM_Columns, OFstatic_cast(Uint16, di_reference->getWidth()));
            if (cond.good()) cond = diff_image->getDataset()->putAndInsertString(DCM_NumberOfFrames, buf);
            DcmTagKey frameIncrementPointer(0x0018, 0x2002);
            if (cond.good()) cond = diff_image->getDataset()->putAndInsertTagKey(DCM_FrameIncrementPointer, frameIncrementPointer);
            OFString frameLabelVector;
            createFrameLabelVector(frameLabelVector, di_reference->getFrameCount(), OFTrue);
            if (cond.good()) cond = diff_image->getDataset()->putAndInsertOFStringArray(DCM_FrameLabelVector, frameLabelVector);
        }
    }
    return cond;
}


void DicomImageComparison::logImageInfo(DicomImage *di, const char *filename)
{
    if (di == NULL) return;
    if (filename == NULL) return;

    double minVal = 0.0;
    double maxVal = 0.0;
    const char *colorModel;
    unsigned long i;

    int minmaxValid = di->getMinMaxValues(minVal, maxVal);
    colorModel = di->getString(di->getPhotometricInterpretation());
    if (colorModel == NULL)
        colorModel = "unknown";

    char aspectRatio[30];
    OFStandard::ftoa(aspectRatio, sizeof(aspectRatio), di->getHeightWidthRatio(), OFStandard::ftoa_format_f, 0, 2);

    /* dump some general information */
    DCMIMAGE_DEBUG("characteristics of file '" << filename << "':");
    DCMIMAGE_DEBUG(
           "  columns x rows      : " << di->getWidth() << " x " << di->getHeight() << OFendl
        << "  bits per sample     : " << di->getDepth() << OFendl
        << "  color model         : " << colorModel << OFendl
        << "  pixel aspect ratio  : " << aspectRatio << OFendl
        << "  number of frames    : " << di->getNumberOfFrames() << " (" << di->getFrameCount() << " processed)");

    /* dump VOI windows */
    unsigned long count;
    OFString explStr, funcStr;
    count = di->getWindowCount();
    switch (di->getVoiLutFunction())
    {
        case EFV_Default:
            funcStr = "<default>";
            break;
        case EFV_Linear:
            funcStr = "LINEAR";
            break;
        case EFV_Sigmoid:
            funcStr = "SIGMOID";
            break;
    }
    DCMIMAGE_DEBUG("  VOI LUT function    : " << funcStr);
    DCMIMAGE_DEBUG("  VOI windows in file : " << di->getWindowCount());
    for (i = 0; i < count; i++)
    {
        if (di->getVoiWindowExplanation(i, explStr) == NULL)
            DCMIMAGE_DEBUG("  - <no explanation>");
        else
            DCMIMAGE_DEBUG("  - " << explStr);
    }

    /* dump VOI LUTs */
    count = di->getVoiLutCount();
    DCMIMAGE_DEBUG("  VOI LUTs in file    : " << count);
    for (i = 0; i < count; i++)
    {
        if (di->getVoiLutExplanation(i, explStr) == NULL)
            DCMIMAGE_DEBUG("  - <no explanation>");
        else
            DCMIMAGE_DEBUG("  - " << explStr);
    }

    /* dump presentation LUT shape */
    OFString shapeStr;
    switch (di->getPresentationLutShape())
    {
        case ESP_Default:
            shapeStr = "<default>";
            break;
        case ESP_Identity:
            shapeStr = "IDENTITY";
            break;
        case ESP_Inverse:
            shapeStr = "INVERSE";
            break;
        case ESP_LinOD:
            shapeStr = "LIN OD";
            break;
    }
    DCMIMAGE_DEBUG("  presentation shape  : " << shapeStr);

    /* dump overlays */
    DCMIMAGE_DEBUG("  overlays in file    : " << di->getOverlayCount());

    if (minmaxValid)
    {
      char minmaxText[30];
      OFStandard::ftoa(minmaxText, sizeof(minmaxText), maxVal, OFStandard::ftoa_format_f, 0, 0);
      DCMIMAGE_DEBUG("  maximum pixel value : " << minmaxText);
      OFStandard::ftoa(minmaxText, sizeof(minmaxText), minVal, OFStandard::ftoa_format_f, 0, 0);
      DCMIMAGE_DEBUG("  minimum pixel value : " << minmaxText);
    }

    return;
}


OFCondition DicomImageComparison::copyReferenceImageAttributes(DcmItem *src, DcmItem *diffImage)
{
    if (src == NULL) return EC_IllegalCall;
    if (diffImage == NULL) return EC_IllegalCall;
    OFCondition cond;

    // Copy Patient Module attributes from the reference image.
    // All these attributes are type 2 and should thus be present
    OFString patientName;
    OFString patientID;
    OFString patientBirthDate;
    OFString patientSex;

    if (src->findAndGetOFStringArray(DCM_PatientName, patientName).bad())
        DCMIMAGE_WARN("Patient Name not found in reference image, ignoring.");
    if (src->findAndGetOFStringArray(DCM_PatientID, patientID).bad())
        DCMIMAGE_WARN("Patient ID not found in reference image, ignoring.");
    if (src->findAndGetOFStringArray(DCM_PatientBirthDate, patientBirthDate).bad())
        DCMIMAGE_WARN("Patient's Sex not found in reference image, ignoring.");
    if (src->findAndGetOFStringArray(DCM_PatientSex, patientSex).bad())
        DCMIMAGE_WARN("Patient's Birth Date not found in reference image, ignoring.");

    cond = diffImage->putAndInsertOFStringArray(DCM_PatientName, patientName);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_PatientID, patientID);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_PatientBirthDate, patientBirthDate);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_PatientSex, patientSex);
    if (cond.bad()) return cond;

    // Copy General Study Module attributes from the reference image.
    // All these attributes are type 1 or 2 and should thus be present
    OFString studyInstanceUID;
    OFString studyDate;
    OFString studyTime;
    OFString referringPhysicianName;
    OFString studyID;
    OFString accessionNumber;

    cond = src->findAndGetOFStringArray(DCM_StudyInstanceUID, studyInstanceUID);
    if (cond.bad())
    {
        DCMIMAGE_ERROR("Study Instance UID not found in reference image.");
        return cond;
    }
    if (src->findAndGetOFStringArray(DCM_StudyDate, studyDate).bad())
        DCMIMAGE_WARN("Study Date not found in reference image, ignoring.");
    if (src->findAndGetOFStringArray(DCM_StudyTime, studyTime).bad())
        DCMIMAGE_WARN("Study Time not found in reference image, ignoring.");
    if (src->findAndGetOFStringArray(DCM_ReferringPhysicianName, referringPhysicianName).bad())
        DCMIMAGE_WARN("Referring Physician's Name not found in reference image, ignoring.");
    if (src->findAndGetOFStringArray(DCM_StudyID, studyID).bad())
        DCMIMAGE_WARN("Study ID not found in reference image, ignoring.");
    if (src->findAndGetOFStringArray(DCM_AccessionNumber, accessionNumber).bad())
        DCMIMAGE_WARN("Accession Number not found in reference image, ignoring.");

    cond = diffImage->putAndInsertOFStringArray(DCM_StudyInstanceUID, studyInstanceUID);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_StudyDate, studyDate);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_StudyTime, studyTime);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_ReferringPhysicianName, referringPhysicianName);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_StudyID, studyID);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_AccessionNumber, accessionNumber);
    if (cond.bad()) return cond;

    // Generate General Series Module attributes
    OFString modality = "OT"; // OT = "other"
    char uidbuf[65];
    OFString seriesInstanceUID;
    OFString laterality;
    OFString seriesNumber = "1";
    seriesInstanceUID = dcmGenerateUniqueIdentifier(uidbuf);

    cond = diffImage->putAndInsertOFStringArray(DCM_Modality, modality);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_SeriesNumber, seriesNumber);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_SeriesInstanceUID, seriesInstanceUID);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_Laterality, laterality);
    if (cond.bad()) return cond;

    // Generate SC Equipment Module attributes
    OFString conversionType = "SYN"; // SYN = "Synthetic Image"
    cond = diffImage->putAndInsertOFStringArray(DCM_ConversionType, conversionType);
    if (cond.bad()) return cond;

    // Generate General Image Module attributes
    OFString instanceNumber = "1";
    OFString contentDate;
    OFString contentTime;
    OFString imageType = "DERIVED\\SECONDARY";
    OFString patientOrientation;
    OFDateTime dt;
    if ((! dt.setCurrentDateTime()) ||
        (! dt.getDate().getISOFormattedDate(contentDate, OFFalse)) ||
        (! dt.getTime().getISOFormattedTime(contentTime, OFTrue, OFFalse, OFFalse, OFFalse)))
    {
        DCMIMAGE_ERROR("Unable to retrieve current date/time");
        return makeOFCondition(OFM_dcmimage, 137, OF_error, "Unable to retrieve current date/time");
    }

    cond = diffImage->putAndInsertOFStringArray(DCM_InstanceNumber, instanceNumber);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_ContentDate, contentDate);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_ContentTime, contentTime);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_ImageType, imageType);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_PatientOrientation, patientOrientation);
    if (cond.bad()) return cond;

    // Generate General Reference Module attributes
    OFString derivationDescription = "Difference image between reference and lossy compressed test image";
    DcmItem *derivationCodeSequence = NULL;
    OFString derivationCodeValue = "113062";
    OFString derivationCodingSchemeDesignator = "DCM";
    OFString derivationCodeMeaning = "Pixel by pixel subtraction";
    DcmItem *sourceImageSequence = NULL;
    OFString referencedSOPClassUID;
    OFString referencedSOPInstanceUID;
    DcmItem *purposeOfReferenceCodeSequence = NULL;
    OFString purposeOfReferenceCodeValue = "121320";
    OFString purposeOfReferenceCodingSchemeDesignator = "DCM";
    OFString purposeOfReferenceCodeMeaning = "Uncompressed predecessor";

    cond = src->findAndGetOFStringArray(DCM_SOPClassUID, referencedSOPClassUID);
    if (cond.bad())
    {
        DCMIMAGE_ERROR("SOP Class UID not found in reference image.");
        return cond;
    }
    cond = src->findAndGetOFStringArray(DCM_SOPInstanceUID, referencedSOPInstanceUID);
    if (cond.bad())
    {
        DCMIMAGE_ERROR("SOP Instance UID not found in reference image.");
        return cond;
    }

    cond = diffImage->putAndInsertOFStringArray(DCM_DerivationDescription, derivationDescription);
    if (cond.bad()) return cond;
    cond = diffImage->findOrCreateSequenceItem(DCM_DerivationCodeSequence, derivationCodeSequence, -2 /* append new item */);
    if (cond.bad()) return cond;
    cond = derivationCodeSequence->putAndInsertOFStringArray(DCM_CodeValue, derivationCodeValue);
    if (cond.bad()) return cond;
    cond = derivationCodeSequence->putAndInsertOFStringArray(DCM_CodingSchemeDesignator, derivationCodingSchemeDesignator);
    if (cond.bad()) return cond;
    cond = derivationCodeSequence->putAndInsertOFStringArray(DCM_CodeMeaning, derivationCodeMeaning);
    if (cond.bad()) return cond;
    cond = diffImage->findOrCreateSequenceItem(DCM_SourceImageSequence, sourceImageSequence, -2 /* append new item */);
    if (cond.bad()) return cond;
    cond = sourceImageSequence->putAndInsertOFStringArray(DCM_ReferencedSOPClassUID, referencedSOPClassUID);
    if (cond.bad()) return cond;
    cond = sourceImageSequence->putAndInsertOFStringArray(DCM_ReferencedSOPInstanceUID, referencedSOPInstanceUID);
    if (cond.bad()) return cond;
    cond = sourceImageSequence->findOrCreateSequenceItem(DCM_PurposeOfReferenceCodeSequence, purposeOfReferenceCodeSequence, -2 /* append new item */);
    if (cond.bad()) return cond;
    cond = purposeOfReferenceCodeSequence->putAndInsertOFStringArray(DCM_CodeValue, purposeOfReferenceCodeValue);
    if (cond.bad()) return cond;
    cond = purposeOfReferenceCodeSequence->putAndInsertOFStringArray(DCM_CodingSchemeDesignator, purposeOfReferenceCodingSchemeDesignator);
    if (cond.bad()) return cond;
    cond = purposeOfReferenceCodeSequence->putAndInsertOFStringArray(DCM_CodeMeaning, purposeOfReferenceCodeMeaning);
    if (cond.bad()) return cond;

    // Most of the Image Pixel Module is generated elsewhere.
    // Here we only populate a few values that are copied from the reference image or are constant

    OFString photometricInterpretation = "MONOCHROME2";
    Uint16 samplesPerPixel = 1;
    Uint16 pixelRepresentation = 0;
    OFString pixelAspectRatio;

    // First look for PixelAspectRatio in the reference image
    if (src->findAndGetOFStringArray(DCM_PixelAspectRatio, pixelAspectRatio).bad())
    {
        // Now look for PixelSpacing. Note that this is a DS (double string) element
        Float64 rowSpacing = 0.0;
        Float64 colSpacing = 0.0;
        unsigned long row;
        unsigned long col;
        char buf[30];
        if (src->findAndGetFloat64(DCM_PixelSpacing, rowSpacing, 0).good() &&
            src->findAndGetFloat64(DCM_PixelSpacing, colSpacing, 1).good())
        {
            row = OFstatic_cast(unsigned long, rowSpacing*1000);
            col = OFstatic_cast(unsigned long, colSpacing*1000);
            // only create pixelAspectRatio if pixels are not square
            if (row != col)
            {
                OFStandard::snprintf(buf, 30, "%lu\\%lu", row, col);
                pixelAspectRatio = buf;
            }
        }
        else
        {
            // Finally look for ImagerPixelSpacing. Note that this is a DS (double string) element
            if (src->findAndGetFloat64(DCM_ImagerPixelSpacing, rowSpacing, 0).good() &&
                src->findAndGetFloat64(DCM_ImagerPixelSpacing, colSpacing, 1).good())
            {
                row = OFstatic_cast(unsigned long, rowSpacing*1000);
                col = OFstatic_cast(unsigned long, colSpacing*1000);
                // only create pixelAspectRatio if pixels are not square
                if (row != col)
                {
                    OFStandard::snprintf(buf, 30, "%lu\\%lu", row, col);
                    pixelAspectRatio = buf;
                }
            }
        }
    }

    cond = diffImage->putAndInsertOFStringArray(DCM_PhotometricInterpretation, photometricInterpretation);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertUint16(DCM_SamplesPerPixel, samplesPerPixel);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertUint16(DCM_PixelRepresentation, pixelRepresentation);
    if (cond.bad()) return cond;
    if (pixelAspectRatio.length() > 0)
    {
        cond = diffImage->putAndInsertOFStringArray(DCM_PixelAspectRatio, pixelAspectRatio);
        if (cond.bad()) return cond;
    }

    // Generate SC Multi-frame Image Module attributes
    OFString burnedInAnnotation = "YES";
    OFString presentationLUTShape = "IDENTITY";
    OFString rescaleIntercept = "0";
    OFString rescaleSlope = "1";
    OFString rescaleType = "US"; // US = unspecified

    cond = diffImage->putAndInsertOFStringArray(DCM_BurnedInAnnotation, burnedInAnnotation);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_PresentationLUTShape, presentationLUTShape);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_RescaleIntercept, rescaleIntercept);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_RescaleSlope, rescaleSlope);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_RescaleType, rescaleType);
    if (cond.bad()) return cond;

    // Generate SOP Common Module attributes (except SOP Class UID)
    OFString sOPInstanceUID;
    OFString specificCharacterSet;
    // UID constant used for this tool
    OFString instanceCreatorUID = "1.2.276.0.7230010.3.1.2.1787205428.20064.1535731842.951308";
    OFString InstanceNumber = "1";
    if (src->findAndGetOFStringArray(DCM_SpecificCharacterSet, specificCharacterSet).bad()) specificCharacterSet = "";
    sOPInstanceUID = dcmGenerateUniqueIdentifier(uidbuf);

    cond = diffImage->putAndInsertOFStringArray(DCM_SOPInstanceUID, sOPInstanceUID);
    if (cond.bad()) return cond;
    if (specificCharacterSet.length() > 0)
    {
        cond = diffImage->putAndInsertOFStringArray(DCM_SpecificCharacterSet, specificCharacterSet);
        if (cond.bad()) return cond;
    }
    // our images always have the same values for content date/time and instance creation date/time
    cond = diffImage->putAndInsertOFStringArray(DCM_InstanceCreationDate, contentDate);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_InstanceCreationTime, contentTime);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_InstanceCreatorUID, instanceCreatorUID);
    if (cond.bad()) return cond;
    cond = diffImage->putAndInsertOFStringArray(DCM_InstanceNumber, instanceNumber);
    if (cond.bad()) return cond;

    // Generate Common Instance Reference Module attributes (except SOP Class UID)
    DcmItem *referencedSeriesSequence = NULL;
    DcmItem *referencedInstanceSequence = NULL;
    OFString referencedSeriesInstanceUID;
    cond = src->findAndGetOFStringArray(DCM_SeriesInstanceUID, referencedSeriesInstanceUID);
    if (cond.bad())
    {
        DCMIMAGE_ERROR("Series Instance UID not found in reference image.");
        return cond;
    }

    cond = diffImage->findOrCreateSequenceItem(DCM_ReferencedSeriesSequence, referencedSeriesSequence, -2 /* append new item */);
    if (cond.bad()) return cond;
    cond = referencedSeriesSequence->putAndInsertOFStringArray(DCM_SeriesInstanceUID, referencedSeriesInstanceUID);
    if (cond.bad()) return cond;
    cond = referencedSeriesSequence->findOrCreateSequenceItem(DCM_ReferencedInstanceSequence, referencedInstanceSequence, -2 /* append new item */);
    if (cond.bad()) return cond;
    cond = referencedInstanceSequence->putAndInsertOFStringArray(DCM_ReferencedSOPClassUID, referencedSOPClassUID);
    if (cond.bad()) return cond;
    cond = referencedInstanceSequence->putAndInsertOFStringArray(DCM_ReferencedSOPInstanceUID, referencedSOPInstanceUID);
    if (cond.bad()) return cond;

    return cond;
}

OFCondition DicomImageComparison::copyTestImageAttributes(DcmItem *src, DcmItem *diffImage)
{
    if (src == NULL) return EC_IllegalCall;
    if (diffImage == NULL) return EC_IllegalCall;
    OFCondition cond;

    // Generate General Reference Module attributes
    DcmItem *sourceImageSequence = NULL;
    OFString referencedSOPClassUID;
    OFString referencedSOPInstanceUID;
    DcmItem *purposeOfReferenceCodeSequence = NULL;
    OFString purposeOfReferenceCodeValue = "121330";
    OFString purposeOfReferenceCodingSchemeDesignator = "DCM";
    OFString purposeOfReferenceCodeMeaning = "Lossy compressed predecessor";

    cond = src->findAndGetOFStringArray(DCM_SOPClassUID, referencedSOPClassUID);
    if (cond.bad())
    {
        DCMIMAGE_ERROR("SOP Class UID not found in test image.");
        return cond;
    }
    cond = src->findAndGetOFStringArray(DCM_SOPInstanceUID, referencedSOPInstanceUID);
    if (cond.bad())
    {
        DCMIMAGE_ERROR("SOP Instance UID not found in test image.");
        return cond;
    }

    // Add reference to the test image to the Source Image Sequence
    cond = diffImage->findOrCreateSequenceItem(DCM_SourceImageSequence, sourceImageSequence, -2 /* append new item */);
    if (cond.bad()) return cond;
    cond = sourceImageSequence->putAndInsertOFStringArray(DCM_ReferencedSOPClassUID, referencedSOPClassUID);
    if (cond.bad()) return cond;
    cond = sourceImageSequence->putAndInsertOFStringArray(DCM_ReferencedSOPInstanceUID, referencedSOPInstanceUID);
    if (cond.bad()) return cond;
    cond = sourceImageSequence->findOrCreateSequenceItem(DCM_PurposeOfReferenceCodeSequence, purposeOfReferenceCodeSequence, -2 /* append new item */);
    if (cond.bad()) return cond;
    cond = purposeOfReferenceCodeSequence->putAndInsertOFStringArray(DCM_CodeValue, purposeOfReferenceCodeValue);
    if (cond.bad()) return cond;
    cond = purposeOfReferenceCodeSequence->putAndInsertOFStringArray(DCM_CodingSchemeDesignator, purposeOfReferenceCodingSchemeDesignator);
    if (cond.bad()) return cond;
    cond = purposeOfReferenceCodeSequence->putAndInsertOFStringArray(DCM_CodeMeaning, purposeOfReferenceCodeMeaning);
    if (cond.bad()) return cond;

    // Generate Common Instance Reference Module attributes (except SOP Class UID)
    DcmItem *referencedSeriesSequence = NULL;
    DcmItem *referencedInstanceSequence = NULL;
    DcmItem *studiesContainingOtherReferencedInstancesSequence = NULL;
    OFString referencedStudyInstanceUID;
    OFString referencedSeriesInstanceUID;
    OFString diffImageStudyInstanceUID;
    OFString diffImageSeriesInstanceUID;

    cond = src->findAndGetOFStringArray(DCM_StudyInstanceUID, referencedStudyInstanceUID);
    if (cond.bad())
    {
        DCMIMAGE_ERROR("Study Instance UID not found in test image.");
        return cond;
    }
    cond = src->findAndGetOFStringArray(DCM_SeriesInstanceUID, referencedSeriesInstanceUID);
    if (cond.bad())
    {
        DCMIMAGE_ERROR("Series Instance UID not found in test image.");
        return cond;
    }
    cond = diffImage->findAndGetOFStringArray(DCM_StudyInstanceUID, diffImageStudyInstanceUID);
    if (cond.bad())
    {
        DCMIMAGE_ERROR("Study Instance UID not found in difference image.");
        return cond;
    }

    if (diffImageStudyInstanceUID == referencedStudyInstanceUID)
    {
        // Reference image and test image share the same Study Instance UID.
        // Now check the Series Instance UID.
        cond = diffImage->findAndGetSequenceItem(DCM_ReferencedSeriesSequence, referencedSeriesSequence, 0);
        if (cond.bad()) return cond;
        cond = referencedSeriesSequence->findAndGetOFStringArray(DCM_SeriesInstanceUID, diffImageSeriesInstanceUID);
        if (cond.bad()) return cond;
        if (diffImageSeriesInstanceUID == referencedSeriesInstanceUID)
        {
            // Reference image and test image share the same Series Instance UID.
            cond = referencedSeriesSequence->findOrCreateSequenceItem(DCM_ReferencedInstanceSequence, referencedInstanceSequence, -2);
            if (cond.bad()) return cond;
            cond = referencedInstanceSequence->putAndInsertOFStringArray(DCM_ReferencedSOPClassUID, referencedSOPClassUID);
            if (cond.bad()) return cond;
            cond = referencedInstanceSequence->putAndInsertOFStringArray(DCM_ReferencedSOPInstanceUID, referencedSOPInstanceUID);
            if (cond.bad()) return cond;
        }
        else
        {
            // Reference image and test image have different Series Instance UIDs.
            // This should not happen unless some deidentification process has taken place.
            DCMIMAGE_WARN("Test image has a different Series Instance UID than the reference image.");
            cond = diffImage->findOrCreateSequenceItem(DCM_ReferencedSeriesSequence, referencedSeriesSequence, -2 /* append new item */);
            if (cond.bad()) return cond;
            cond = referencedSeriesSequence->putAndInsertOFStringArray(DCM_SeriesInstanceUID, referencedSeriesInstanceUID);
            if (cond.bad()) return cond;
            cond = referencedSeriesSequence->findOrCreateSequenceItem(DCM_ReferencedInstanceSequence, referencedInstanceSequence, -2 /* append new item */);
            if (cond.bad()) return cond;
            cond = referencedInstanceSequence->putAndInsertOFStringArray(DCM_ReferencedSOPClassUID, referencedSOPClassUID);
            if (cond.bad()) return cond;
            cond = referencedInstanceSequence->putAndInsertOFStringArray(DCM_ReferencedSOPInstanceUID, referencedSOPInstanceUID);
            if (cond.bad()) return cond;
        }
    }
    else
    {
        // Reference image and test image have different Study Instance UIDs.
        // This should not happen unless some deidentification process has taken place.
        DCMIMAGE_WARN("Test image has a different Study Instance UID than the reference image.");
        cond = diffImage->findOrCreateSequenceItem(DCM_StudiesContainingOtherReferencedInstancesSequence, studiesContainingOtherReferencedInstancesSequence, -2 /* append new item */);
        if (cond.bad()) return cond;
        cond = studiesContainingOtherReferencedInstancesSequence->putAndInsertOFStringArray(DCM_StudyInstanceUID, referencedStudyInstanceUID);
        if (cond.bad()) return cond;
        cond = studiesContainingOtherReferencedInstancesSequence->findOrCreateSequenceItem(DCM_ReferencedSeriesSequence, referencedSeriesSequence, -2 /* append new item */);
        if (cond.bad()) return cond;
        cond = referencedSeriesSequence->putAndInsertOFStringArray(DCM_SeriesInstanceUID, referencedSeriesInstanceUID);
        if (cond.bad()) return cond;
        cond = referencedSeriesSequence->findOrCreateSequenceItem(DCM_ReferencedInstanceSequence, referencedInstanceSequence, -2 /* append new item */);
        if (cond.bad()) return cond;
        cond = referencedInstanceSequence->putAndInsertOFStringArray(DCM_ReferencedSOPClassUID, referencedSOPClassUID);
        if (cond.bad()) return cond;
        cond = referencedInstanceSequence->putAndInsertOFStringArray(DCM_ReferencedSOPInstanceUID, referencedSOPInstanceUID);
        if (cond.bad()) return cond;
    }

    return cond;
}


void DicomImageComparison::createFrameLabelVector(OFString& s, unsigned long numFrames, OFBool isColor)
{
    // Create a multi-valued string with one label for each frame.
    // For monochrome images, the labels will be F001\F002\F003...
    // For color images, the labels will be F001R\F001G\F001B\F002R\F002G\F002B...
    // since we store three monochrome difference frames for the R, G and B plane
    // of each color frame in the reference image.
    s = "";
    char buf[100];
    for (unsigned long i = 0; i < numFrames; ++i)
    {
      if (i > 0) s += "\\";
      if (isColor)
          OFStandard::snprintf(buf, 100, "F%03luR\\F%03luG\\F%03luB", i+1, i+1, i+1);
          else OFStandard::snprintf(buf, 100, "F%03lu", i+1);
      s += buf;
    }

    // check if we can really represent this string as SH element in explicit VR.
    // This may become a problem if we have a color image with more than 3100 frames,
    // or a monochrome image with more than 9300 frames.
    if (s.length() > 65534)
    {
        DCMIMAGE_WARN("frame label vector too long");
        s.erase(65534);
    }
}
