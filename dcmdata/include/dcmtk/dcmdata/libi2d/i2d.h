/*
 *
 *  Copyright (C) 2001-2021, OFFIS e.V.
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
 *  Purpose: Class to control conversion of image format to DICOM
 *
 */

#ifndef I2D_H
#define I2D_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/libi2d/i2doutpl.h"
#include "dcmtk/dcmdata/libi2d/i2dimgs.h"
#include "dcmtk/dcmdata/dcpixel.h"
#include "dcmtk/dcmdata/dcofsetl.h"

class DcmPixelItem;

class DCMTK_I2D_EXPORT Image2Dcm
{

public:

  /** Constructor, initializes command line options
   */
  Image2Dcm();

  /** Destructor, frees plugin memory
   */
  ~Image2Dcm();

  /** Start the conversion. Needs a fully configured input plugin
   *  and a fully configured output plugin to operate. Returns
   *  a dataset with the resulting DICOM object.
   *  @param inputPlug - [in] The input plugin to read pixel data
   *  @param outPlug - [in] The output plugin for specific SOP class output
   *  @param numberOfFrames - [in] The number of frames to be written
   *  @param resultDset - [out] The DICOM object resulting from the conversion
   *         The dataset is allocated in this method and must be freed by the
   *         caller.
   *  @param proposedTS - [out] The proposed transfer syntax (needed e. g.
   *                            by JPEG input plugin)
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition convertFirstFrame(
    I2DImgSource *inputPlug,
    I2DOutputPlug *outPlug,
    size_t numberOfFrames,
    DcmDataset*& resultDset,
    E_TransferSyntax& proposedTS);


  /** Start the conversion. Needs a fully configured input plugin
   *  to operate. Updates the pixel data element created by convertFirstFrame().
   *  @param inputPlug - [in] The input plugin to read pixel data
   *  @param frameNumber - [in] The number of the current frame, must be larger than 1
   *    and smaller or equal to the numberOfFrames value passed to convertFirstFrame().
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition convertNextFrame(
    I2DImgSource *inputPlug,
    size_t frameNumber);

  /** Update the offset table in the case of an encapsulated image
   */
  OFCondition updateOffsetTable();

  /** Finalizes the conversion after the last call to convertNextFrame()
   *  by writing the attributes related to lossy image compression.
   *  @param inputPlug - [in] The input plugin to read pixel data
   *  @param numberOfFrames - [in] The number of frames to be written
   *  @param dset - [in/out] The DICOM object resulting from the conversion
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition updateLossyCompressionInfo(
    I2DImgSource *inputPlug,
    size_t numberOfFrames,
    DcmDataset *dset);

  /** Sets a file that should serve as a template for the resulting
    * DICOM object. Only the dataset of the given file is imported.
    * @param file - [in] The filename of the template file,
    *   which is either in DICOM or XML format.
    * @return none
    */
  void setTemplateFile(const OFString& file);

  /** Sets the format of the template file.
    * @param isXML - [in] true for XML, false for DICOM format
    * @return none
    */
  void setTemplateFileIsXML(OFBool isXML);

  /** activates or deactivates XML validation
    * @param enabled - [in] true to enable validation
    * @return none
    */
  void setXMLvalidation(OFBool enabled);

  /** activates or deactivates an XML namespace check
    * @param enabled - [in] true to enable namespace check
    * @return none
    */
  void setXMLnamespaceCheck(OFBool enabled);

  /** Set file from which patient/study/series data should be imported from.
   *  @param file - [in] The DICOM file to read from
   *  @return none
   */
  void setSeriesFrom(const OFString& file);

  /** Set file from which patient/study/series data should be imported from.
   *  @param file - [in] The DICOM file to read from
   *  @return none
   */
  void setStudyFrom(const OFString& file);

  /** Enables/Disables incrementing instance number taken over from DICOM file.
   *  This can either be a the study/series file or the template file
   *  provided.
   *  @param incInstNo - [in] If true, the instance number will be incremented
   *  @return none
   */
  void setIncrementInstanceNumber(OFBool incInstNo);

  /** Sets the conversion flags for character set conversion of
   *  the study/series file
   * @param conversionFlags - [in] conversion flags
   * @return none;
   */
  void setConversionFlags(size_t conversionFlags);

  /** Specifies some attributes that should be inserted after conversion.
   *  They will override any identical attributes already existing in the
   *  converted result DICOM object. The override keys are applied at the very
   *  end of the conversion and do not undergoe any validity checking.
   *  @param ovkeys [in] override keys that can be tags, dictionary names
   *                     and paths (see DcmPath for syntax). Also it is
   *                     permitted to set a value if appropriate, e. g.
   *                     "PatientName=Doe^John" would be a valid override
   *                     key.
   *  @return none
   */
  void setOverrideKeys(const OFList<OFString>& ovkeys);

  /** Enable/Disable basic validity checks for output dataset
   *  @param doChecks - [in] OFTrue enables checking, OFFalse turns it off.
   *  @param insertMissingType2 - [in] If true (default), missing type 2
   *         attributes are inserted automatically
   *  @param inventMissingType1 - [in] If true , missing type 1
   *         attributes are inserted automatically with a predefined
   *         value (if possible). An existing empty type 1 attribute is
   *         assigned a value, too.
   *  @return none
   */
  void setValidityChecking(OFBool doChecks,
                           OFBool insertMissingType2 = OFTrue,
                           OFBool inventMissingType1 = OFTrue);

protected:

  /** Cleans up template for future insertion of pixel data information, ie
    * generally removes attributes from image pixel module
    * @param targetDset - [out] The dataset that should be cleand up
    * @return none
    */
  void cleanupTemplate(DcmDataset *targetDset);

  /** Parses patient/study/series level attributes from file as defined
   *  on command line and fills internal attributes accordingly.
   *  @param targetDset - [out] The dataset to write the attributes to
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition applyStudyOrSeriesFromFile(DcmDataset *targetDset);

  /** Increments instance number in given dataset
   *  @param targetDset - [in/out] Dataset to increment instance number in
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition incrementInstanceNumber(DcmDataset *targetDset);

  /** Generates and inserts missing UIDs, ie UIDs not taken over
   *  from --dataset-from --study from and --series-from options
   *  @param dset - [in] The dataset to write the UIDs to
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition generateUIDs(DcmDataset *dset);

  /** Reads pixel data and corresponding attributes like rows etc. from image
   *  file and inserts them into dataset.
   *  @param imageSource - [in] The input plugin that actually reads the pixel data
   *  @param numberOfFrames - [in] The number of frames to be written
   *  @param dset - [out] The dataset to export the pixel data attributes to
   *  @param outputTS - [out] The proposed transfex syntax of the dataset
   *  @param compressionRatio - [out] compression ratio of the pixel data, 1.0 for uncompressed
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition readAndInsertPixelDataFirstFrame(
    I2DImgSource* imageSource,
    size_t numberOfFrames,
    DcmDataset* dset,
    E_TransferSyntax& outputTS,
    double& compressionRatio);

  /** Reads pixel data and corresponding attributes like rows etc. from image
   *  file and inserts them into dataset as an additional frame.
   *  @param imageSource - [in] The input plugin that actually reads the pixel data
   *  @param frameNumber - [in] The number of the current frame, must be larger than 1
   *    and smaller or equal to the numberOfFrames value passed to convertFirstFrame().
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition readAndInsertPixelDataNextFrame(
    I2DImgSource* imageSource,
    size_t frameNumber);

  /** Do some completeness / validity checks. Should be called when
   *  dataset is completed and is about to be saved.
   *  @param dataset - [in] The dataset to check
   *  @return Error string if error occurs, empty string otherwise
   */
  OFString isValid(DcmDataset& dataset) const;

  /** Copy override keys over existing keys in given dataset.
   *  @param outputDset - [out] dataset to which the override keys are copied
   *  @return Error code if error occurs, EC_Normal otherwise
   */
   OFCondition applyOverrideKeys(DcmDataset *outputDset);

   /** Inserts "ISO_IR100" in the attribute "Specific Character Set".
    *  Overwrites any existing value.
    *  @param outputDset - [out] The dataset to write to
    *  @return EC_Normal if insertion is successful, error code otherwise
    */
   OFCondition insertLatin1(DcmDataset *outputDset);

   /** Check whether given type 1 attribute is present and has a length > 0.
    *  @param  key - [in] The attribute tag check
    *  @param  targetDset - [out] targetDset
    *  @param  defaultValue - [in] value to be inserted if attribute is missing
    *                         (needs invent option for type 1 attributes enabled)
    *  @return A string with an error message if attribute is not present
    *          or has length of 0
    */
   OFString checkAndInventType1Attrib(const DcmTagKey& key,
                                      DcmDataset* targetDset,
                                      const OFString& defaultValue = "") const;


   /** Inserts an empty element with the given tag into a dataset
    *  @param  key - [in] The attribute tag to check
    *  @param  targetDset - [out] targetDset
    *  @return A string with an error message if attribute is not present
    *          nor gets inserted automatically (see m_inventMissingType2Attribs)
    */
   OFString checkAndInventType2Attrib(const DcmTagKey& key,
                                      DcmDataset* targetDset) const;

private:

  /** Inserts encapsulated pixel data for the first frame
   *  @param dset [in] - The dataset to which we should add this.
   *  @param pixData [in] - The data to add.
   *  @param length [in] - The length of pixData.
   *  @param outputTS [in] - The output transfer syntax to be used
   *  @return EC_Normal, if successful, error otherwise.
   */
  OFCondition insertEncapsulatedPixelDataFirstFrame(
    DcmDataset* dset,
    char *pixData,
    Uint32 length,
    E_TransferSyntax outputTS);

  /** Inserts encapsulated pixel data for another frame
   *  @param pixData [in] - The data to add.
   *  @param length [in] - The length of pixData.
   *  @return EC_Normal, if successful, error otherwise.
   */
  OFCondition insertEncapsulatedPixelDataNextFrame(
    char *pixData,
    Uint32 length);

  /* Attributes for writing DICOM dataset */

  /// These attributes are applied to the dataset after conversion
  /// (and are not checked by the isValid() function)
  OFList<OFString> m_overrideKeys;

  /// If not empty, the DICOM or XML file specified in this variable is used
  /// as a base for the DICOM image file to be created, ie. all attributes
  /// are taken over from this template file
  OFString m_templateFile;

  /// if true, the template file is in XML format, otherwise DICOM
  OFBool m_templateFileIsXML;

  /// if true, an XML validation will be performed while parsing
  OFBool m_XMLvalidation;

  /// if true, an XML namespace check will be performed while parsing
  OFBool m_XMLnamespaceCheck;

  /// If true, patient and study data is read from file
  OFBool m_readStudyLevel;

  /// If true, patient, study and series data is read from file
  OFBool m_readSeriesLevel;

  /// File to read study and series from
  OFString m_studySeriesFile;

  /// If true, Instance Number ist read from file and incremented by 1
  OFBool m_incInstNoFromFile;

  ///  If true, some simple attribute checks (missing type 2 attributes or
  ///  missing type 1 values) are omitted
  OFBool m_disableAttribChecks;

  /// If true, missing type 2 elements are inserted automatically (empty value).
  /// Enabled per default.
  OFBool m_inventMissingType2Attribs;

  /// If true, missing type 1 elements are inserted automatically with a
  /// predefined value, if possible. Disabled per default.
  OFBool m_inventMissingType1Attribs;

  /// If enabled, ISO_IR100 (ISO Latin 1) is inserted as a standard value
  /// for attribute "Specific Character Set". Insertion takes place after
  /// loading (optional) template file.
  OFBool m_insertLatin1;

  /// number of rows
  Uint16 m_rows;

  /// number of columns
  Uint16 m_cols;

  /// samples per pixel
  Uint16 m_samplesPerPixel;

  /// bits allocated
  Uint16 m_bitsAllocated;

  /// bits stored
  Uint16 m_bitsStored;

  /// high bit
  Uint16 m_highBit;

  /// pixel representation
  Uint16 m_pixelRepresentation;

  /// planar configuration
  Uint16 m_planarConfiguration;

  /// pixel aspect ratio, horizontal
  Uint16 m_pixelAspectRatioH;

  /// pixel aspect ratio, vertical
  Uint16 m_pixelAspectRatioV;

  /// frame length in bytes
  Uint32 m_frameLength;

  /// photometric interpretation
  OFString m_photometricInterpretation;

  /// compression ratio per frame accumulated in this variable
  double m_compressionRatio;

  /// flags for character set conversion
  size_t m_conversionFlags;

  /// buffer into which uncompressed pixel data is written
  char *m_output_buffer;

  /// offset list object used for the creation of encapsulated datasets
  DcmOffsetList m_offsetList;

  /// pixel sequence object used for the creation of encapsulated datasets
  DcmPixelSequence *m_pixelSequence;

  /// offset table object used for the creation of encapsulated datasets
  DcmPixelItem *m_offsetTable;
};

#endif // I2D_H
