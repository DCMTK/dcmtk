/*
 *
 *  Copyright (C) 2001-2007, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class to control conversion of image format to DICOM
 *
 *  Last Update:      $$
 *  Update Date:      $$
 *  Source File:      $$
 *  CVS/RCS Revision: $$
 *  Status:           $$
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef I2D_H
#define I2D_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/i2dlib/i2doutpl.h"
#include "dcmtk/dcmdata/i2dlib/i2dplvlp.h"
#include "dcmtk/dcmdata/i2dlib/i2dplsc.h"
#include "dcmtk/dcmdata/i2dlib/i2djpgs.h"

class Image2Dcm
{

public:

  /** Constructor, initializes command line options
   *  @param none
   *  @return none
   */
  Image2Dcm();

  /** Start the conversion. Needs a fully configured input plugin
   *  and a fully configured output plugin to operate. Returns
   *  a dataset with the resulting DICOM object.
   *  @param inputPlug - [in] The input plugin to read pixel data
   *  @param outputPlug - [in] The output plugin for specific SOP class output
   *  @param resultDset - [out] The DICOM object resulting from the conversion
   *  @param proposedTS - [out] The proposed transfer syntax (needed e. g. 
   *                            by JPEG input plugin)
   *  @return EC_Normal, if successfull, error otherwise
   */
  OFCondition convert(I2DImgSource *inputPlug,
                      I2DOutputPlug *outPlug,
                      DcmDataset*& resultDset,
                      E_TransferSyntax& proposedTS);

  /** Sets a DICOM file that should serve as a template for the resulting 
    * DICOM object. Only the dataset of the given file is imported.
    * @param file - [in] The filename of the template DICOM file
    * @return none
    */
  void setTemplateFile(const OFString& file);

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

  /** Specifies some attributes that should be inserted after conversion.
   *  They will override any identical attributes already existing in the 
   *  converted result DICOM object. The override keys are applied at the very
   *  end of the conversion and do not undergoe any validity checking.
   *  @param dset - [in] The dataset with override attributes (is copied)
   *  @return none
   */  
  void setOverrideKeys(const DcmDataset* dset);
  
  /** Enable/Disable basic validity checks for output dataset
   *  @param doCheck - [in] OFTrue enables checking, OFFalse turns it off.
   *  @return none
   */
  void setValidityChecking(OFBool doChecks);

  /** Sets the verbose mode
   *  @param verboseMode - [in] New status for verbose mode
   *  @return none
   */
  void setVerboseMode(const OFBool& verboseMode) { m_verbose = verboseMode; };

  /** Sets the debug mode
   *  @param debugMode - [in] New status for debug mode
   *  @return none
   */
  void setDebugMode(const OFBool& debugMode) { m_debug = debugMode; };

  /** Destructor, frees plugin memory
   *  @param none
   *  @return none
   */
  ~Image2Dcm();

protected:

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
   *  @param dset - [out] The dataset to export the pixel data attributes to
   *  @param outputTS - [out] The proposed transfex syntax of the dataset
   *  @return EC_Normal, if successful, error otherwise
   */
  OFCondition readAndInsertPixelData( I2DImgSource* imageSource,
                                      DcmDataset* dset,
                                      E_TransferSyntax& outputTS);

  /** Do some completeness / validity checks. Should be called when
   *  dataset is completed and is about to be saved.
   *  @param dataset - [in] The dataset to check
   *  @return EC_Normal if successful, error otherwise
   */
  OFCondition isValid(DcmDataset& dataset) const;

  /** Copy override keys over existing keys in given dataset.
   *  @param outputDset - [out] dataset to which the override keys are copied
   */
   void applyOverrideKeys(DcmDataset *outputDset);

private:

  /* Attributes for writing DICOM dataset */

  /// These attributes are applied to the dataset after conversion
  /// (and are not checked by the isValid() function)
  DcmDataset *m_overrideKeys;

  /// If not empty, the DICOM file specified in this variable is used
  /// as a base for the DICOM image file to be created, ie. all attributes
  /// are taken over from this template file
  OFString m_templateFile;

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

  /// If true, debug output is enabled
  OFBool m_debug;

  /// If true, verbose output is enabled
  OFBool m_verbose;
};

#endif // I2D_H

/*
 * CVS/RCS Log:
 * $Log: i2d.h,v $
 * Revision 1.1  2007-11-08 15:58:55  onken
 * Initial checkin of img2dcm application and corresponding library i2dlib.
 *
 *
 */
