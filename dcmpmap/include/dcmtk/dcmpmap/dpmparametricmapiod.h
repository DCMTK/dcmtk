/*
 *
 *  Copyright (C) 2018-2019, Open Connections GmbH
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module: dcmpmap
 *
 *  Author: Michael Onken
 *
 *  Purpose: Class representing the Parametric Map IOD
 *
 */

#ifndef DPMPARAMETRICMAPIOD_H
#define DPMPARAMETRICMAPIOD_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/modimagepixelbase.h"
#include "dcmtk/dcmpmap/dpmparametricmapbase.h"
#include "dcmtk/dcmpmap/dpmtypes.h"


/** Class for managing the Parametric Map IOD attributes.
 */
class DCMTK_DCMPMAP_EXPORT DPMParametricMapIOD : public DPMParametricMapBase
{
public:

  /** Inner class that offers typed interface to bulk data.
   */
  template<typename PixelType>
  class DCMTK_DCMPMAP_EXPORT Frames
  {
    public:

      /// Binary pixel type
      typedef PixelType pixel_type;

      /** Method that adds a frame to the Parametric Map using the underlying
       *  binary data type.
       *  @param  data The frame data
       *  @param  numPixels The number of pixels (rows * columns) of the frame
       *  @param  perFrameInformation The per-frame functional groups applying for
       *  @return EC_Normal if adding was successful, error otherwise
       */
      OFCondition addFrame(PixelType* data,
                           const size_t numPixels,
                           const OFVector<FGBase*>& perFrameInformation);

      /** Type-specific getFrame() method that returns Float32, Uint16, or whatever
       *  is used in this Parametric Map.
       *  @param  frameNumber The frame number to get, starting from 0
       *  @return Pointer to typed pixel data
       */
      PixelType* getFrame(const size_t frameNumber);

    private:

      /// Make sure the Parametric Map object (and no one else) can use the constructor below.
      friend class DPMParametricMapIOD;

      /** Create Frames object in DPMParametricMapIOD (see friend declaration above)
       *  @param  map The map object that will contain the frames
       */
      Frames(DPMParametricMapIOD& map);

      /// Reference to Parametric Map object in order to access its pixel data
      DPMParametricMapIOD& m_Map;
  };

  /** Frames can contain Uint16, Sint16, Float32 or Float64 data, or nothing
   *  at all (in that case OFCondition denotes a corresponding error)
   */
  typedef OFvariant
  <
    OFCondition,
    Frames<Uint16>,
    Frames<Sint16>,
    Frames<Float32>,
    Frames<Float64>
  > FramesType;

  /** Virtual Destructor, frees memory.
   */
  virtual ~DPMParametricMapIOD();

  /** Create new Parametric Map object. Takes the minimum of required parameters.
   *  Afterwards, additional setters() can be used to set optional data. Frames
   *  and their related per-frame functional groups should be added using addFrame().
   *  Shared functional groups should be added using addForAllFrames().
   *  @param  modality The modality of this Parametric Map (series level attribute).
   *          A list of Defined Terms can be found in the standard. Very often
   *          "MR" is the right choice. Only if a mix of different modalities has been
   *          use for creation "OT" should be used.
   *  @param  seriesNumber A number identifying this series. If importHierarchy()
   *          is called later in order to put this Parametric Map into an existing Series,
   *          the Series  Number will be taken over from the existing series instead.
   *  @param  instanceNumber A number identifying this image
   *  @param  rows The number of rows of all frames of the Parametric Map
   *  @param  columns The number of columns of all frames of the Parametric Map
   *  @param  equipmentInfo Information about the equipment creating thing Parametric Map
   *  @param  contentIdentification General information about this content
   *  @param  imageFlavor 3rd value of attribute Image Type. See standard for list of
   *          applicable Defined Terms.
   *  @param  derivedPixelContrast 4th value of attribute Image Type. See standard for list of
   *          applicable Defined Terms.
   *  @param  contentQualification Either one of CQ_PRODUCT, CQ_RESEARCH, CQ_SERVICE
   *  @return Variant, either OFCondition with error code in case of error, Parametric
   *          Map object in case of success
   */
  template<typename ImagePixel>
  static OFvariant<OFCondition,DPMParametricMapIOD> create(const OFString& modality,
                                                           const OFString& seriesNumber,
                                                           const OFString& instanceNumber,
                                                           const Uint16 rows,
                                                           const Uint16 columns,
                                                           const IODEnhGeneralEquipmentModule::EquipmentInfo& equipmentInfo,
                                                           const ContentIdentificationMacro& contentIdentification,
                                                           const OFString& imageFlavor,
                                                           const OFString& derivedPixelContrast,
                                                           const DPMTypes::ContentQualification& contentQualification);

  /** Indicates whether or not the image contains sufficiently recognizable visual
   *  features to allow the image or a reconstruction from a set of images to
   *  identify the patient.
   *  @param  yesOrNo  Must be related "YES" or "NO" types
   *  @return EC_Normal if setting was successful, error otherwise
   */

  virtual OFCondition setRecognizableVisualFeatures(const DPMTypes::RecognizableVisibleFeatures yesOrNo);

  /** Add a functional group for all frames
   *  @param  group The group to be added as shared functional group
   *  @return EC_Normal if adding was successful, error otherwise
   */
  virtual OFCondition addForAllFrames(const FGBase& group);

  // -------------------- loading and saving ---------------------

  /** Load Parametric Map object from file
   *  @param  filename The file to read from
   *  @return Variant, either the resulting Parametric Map object in case of success,
   *          an OFCondition error code otherwise
   */
  static OFvariant<OFCondition,DPMParametricMapIOD*> loadFile(const OFString& filename);

  /** Load Parametric Map object from dataset object.
   *  @param  dataset The dataset to read from
   *  @return Variant, either the resulting Parametric Map object in case of success,
   *          an OFCondition error code otherwise
   */
  static OFvariant<OFCondition,DPMParametricMapIOD*> loadDataset(DcmDataset& dataset);

  /** Read Parametric Map attributes (all those handled by this class)
   *  from given item. Reads attributes from base class DcmIODCommon before.
   *  The current content is deleted even if the reading process fails.
   *  If the log stream is set and valid the reason for any error might be
   *  obtained from the error/warning output.
   *  @param  dataset  Reference to DICOM dataset from which the document
   *          should be read
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition read(DcmItem &dataset);

  /** Write current Parametric Map's attributes to DICOM dataset.
   *  @param  dataset Reference to DICOM dataset to which the current document
   *          should be written. The dataset is not cleared before writing
   *          to it!
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition write(DcmItem &dataset);

  // -------------------- access ---------------------

  /** Get Recognizable Visual Features
   *  @return Returns related YES or NO data type
   */
  virtual DPMTypes::RecognizableVisibleFeatures getRecognizableVisualFeatures();

  /** Get Content Qualification
   *  @return Returns either one of CQ_PRODUCT, CQ_RESEARCH, CQ_SERVICE or CQ_UNKNOWN
   */
  virtual DPMTypes::ContentQualification getContentQualification();

  /** Get number of rows
   *  @param  rows The number of rows (output)
   *  @return EC_Normal if getting was successful, error otherwise
   */
  virtual OFCondition getRows(Uint16& rows);

  /** Get number of cols
   *  @param  cols The number of columns (output)
   *  @return EC_Normal if getting was successful, error otherwise
   */
  virtual OFCondition getColumns(Uint16& cols);

  /** Perform some basic checking. This method is also invoked when
   *  writing the object to a DICOM dataset or file.
   *  @return OFTrue, if no errors were found, OFFalse otherwise.
   */
  virtual OFBool check();

  /** Get number of frames, based on the number of items in the shared
   *  functional functional groups sequence (i.e.\ the attribute Number of
   *  Frames) is not trusted after reading
   *  @return The number of frames handled by this object
   */
  size_t getNumberOfFrames() const;

  /** Get access to frame data
   *  @return Class managing the frame data
   */
  FramesType getFrames();

protected:

  /** Write current generic Parametric Map's attributes to DICOM dataset, i.e.
   *  those that are not directly pixel-related
   *  @param  dataset Reference to DICOM dataset to which the current document
   *          should be written. The dataset is not cleared before writing
   *          to it.
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition writeGeneric(DcmItem &dataset);

  /** Read Parametric Map's generic (not directly pixel-data related) attributes from
   *  DICOM dataset.
   *  @param  dataset Reference to DICOM dataset to read from.
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition readGeneric(DcmItem &dataset);

  /** Decompress the given dataset if required. Only RLE compression is accepted
   *  and decompressed by this tool. A warning is printed if the dataset has been compressed
   *  before with another compression scheme and is handed into this method in uncompressed
   *  format. Handing in any other compression but RLE will lead to an error.
   *  @param  dset The dataset to be decompressed
   *  @return EC_Normal if decompression worked (or dataset is already in uncompressed format)
   *          IOD_EC_CannotDecompress otherwise
   */
  static OFCondition decompress(DcmDataset& dset);

  /** Constructor
   */
  DPMParametricMapIOD();

  /** Constructor, anonymous parameter is the image pixel module to be used
   */
  template<typename ImagePixel>
  DPMParametricMapIOD(OFin_place_type_t(ImagePixel));

private:

  // Forward declarations
  template<typename PixelType> class GetPixelDataVisitor;
  struct SetImagePixelModuleVisitor;
  struct WriteVisitor;
  struct GetFramesVisitor;
  struct ReadVisitor;
  struct ReadFramesVisitor;

  /** Initialize new Parametric Map object except directly pixel-related data
   *  @param  modality The modality of this Parametric Map (series level attribute).
   *          A list of Defined Terms can be found in the standard. Very often
   *          "MR" is the right choice. Only if a mix of different modalities has been
   *          use for creation "OT" should be used.
   *  @param  seriesNumber A number identifying this series. If importHierarchy()
   *          is called later in order to put this Parametric Map into an existing Series,
   *          the Series  Number will be taken over from the existing series instead.
   *  @param  instanceNumber A number identifying this image
   *  @param  equipmentInfo Information about the equipment creating thing Parametric Map
   *  @param  contentIdentification General information about this content
   *  @param  imageFlavor 3rd value of attribute Image Type. See standard for list of
   *          applicable Defined Terms.
   *  @param  derivedPixelContrast 4th value of attribute Image Type. See standard for list of
   *          applicable Defined Terms.
   *  @param  contentQualification Either one of CQ_PRODUCT, CQ_RESEARCH, CQ_SERVICE
   *  @return EC_Normal in case of success, error otherwise
   */
  OFCondition init(const OFString& modality,
                   const OFString& seriesNumber,
                   const OFString& instanceNumber,
                   const IODEnhGeneralEquipmentModule::EquipmentInfo& equipmentInfo,
                   const ContentIdentificationMacro& contentIdentification,
                   const OFString& imageFlavor,
                   const OFString& derivedPixelContrast,
                   const DPMTypes::ContentQualification& contentQualification);


  /// Parametric Map Image Module's Content Identification
  ContentIdentificationMacro m_ContentIdentification;

  /// Binary frame data
  OFVector<DcmIODTypes::Frame*> m_Frames;

};

#endif // DPMPARAMETRICMAPIOD_H
