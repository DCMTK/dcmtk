/*
 *
 *  Copyright (C) 2002-2010, OFFIS e.V.
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Interface class for simplified creation of a DICOMDIR
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-11-05 13:11:11 $
 *  CVS/RCS Revision: $Revision: 1.23 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCDDIRIF_H
#define DCDDIRIF_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dcdicdir.h"


/*-----------------------------------*
 *  contant declarations and macros  *
 *-----------------------------------*/

// default fileset ID
#define DEFAULT_FILESETID "DCMTK_MEDIA_DEMO"
// default specific character set of fileset descriptor file
#define DEFAULT_DESCRIPTOR_CHARSET "ISO_IR 100"


/*----------------------*
 *  class declarations  *
 *----------------------*/

/** Abstract interface to plugable image support for the DICOMDIR class.
 *  This is an abstract base class used as an interface to access DICOM
 *  images from the DicomDirInterface.  The implementation can be found
 *  in dcmjpeg/libsrc/ddpiimpl.cc (incl. JPEG support).
 */
class DicomDirImagePlugin
{
  public:

    /** destructor (virtual)
     */
    virtual ~DicomDirImagePlugin() {}

    /** scale image (pixel data) to specified size
     *  @param srcData pointer to 8 bit source pixel data (original image)
     *  @param srcWidth width of the source image
     *  @param srcHeight height of the source image
     *  @param dstData pointer to 8 bit destination pixel data (scaled image).
     *    This array needs to be allocated prior to calling this function.
     *  @param dstWidth width of the destination image
     *  @param dstHeight height of the destination image
     *  @return OFTrue if successful, OFFalse otherwise
     */
    virtual OFBool scaleData(const Uint8 *srcData,
                             const unsigned int srcWidth,
                             const unsigned int srcHeight,
                             Uint8 *dstData,
                             const unsigned int dstWidth,
                             const unsigned int dstHeight) const = 0;

    /** scale image (from DICOM dataset) to specified size
     *  @param dataset DICOM dataset where the pixel data is stored (original image)
     *  @param pixel pointer to 8 bit destination pixel data (scaled image).
     *    This array needs to be allocated prior to calling this function.
     *  @param count number of bytes allocated for the 'pixel' array
     *  @param frame index of the frame to be scaled (0..n-1)
     *  @param width width of the destination image
     *  @param height height of the destination image
     *  @param decompressAll always decompress complete pixel data if true
     *  @return OFTrue if successful, OFFalse otherwise
     */
    virtual OFBool scaleImage(DcmItem *dataset,
                              Uint8 *pixel,
                              const unsigned long count,
                              const unsigned long frame,
                              const unsigned int width,
                              const unsigned int height,
                              const OFBool decompressAll = OFFalse) const = 0;

  protected:

    /** constructor (protected)
     */
    DicomDirImagePlugin() {}
};


/** An interface class for simplified creation of a DICOMDIR
 */
class DicomDirInterface
{

  public:

    /** list of supported media storage application profiles
     */
    enum E_ApplicationProfile
    {
        /// General Purpose Interchange on CD-R or DVD-RAM Media (STD-GEN-CD/DVD-RAM)
        AP_GeneralPurpose,
        /// default application profile: GeneralPurpose
        AP_Default = AP_GeneralPurpose,
        /// General Purpose DVD with Compression Interchange (STD-GEN-DVD)
        AP_GeneralPurposeDVD,
        /// General Purpose MIME Interchange Profile (STD-GEN-MIME)
        AP_GeneralPurposeMIME,
        /// General Purpose USB and Flash Memory with Compression Interchange (STD-GEN-USB/MMC/CF/SD-JPEG/J2K)
        AP_USBandFlash,
        /// DVD Interchange with MPEG2 MP\@ML (STD-DVD-MPEG2-MPML)
        AP_MPEG2MPatMLDVD,
        /// Basic Cardiac X-Ray Angiographic Studies on CD-R Media (STD-XABC-CD)
        AP_BasicCardiac,
        /// 1024 X-Ray Angiographic Studies on CD-R Media (STD-XA1K-CD)
        AP_XrayAngiographic,
        /// 1024 X-Ray Angiographic Studies on DVD Media (STD-XA1K-DVD)
        AP_XrayAngiographicDVD,
        /// Dental Radiograph Interchange (STD-DEN-CD)
        AP_DentalRadiograph,
        /// CT/MR Studies (STD-CTMR-xxxx)
        AP_CTandMR,
        /// Ultrasound Single Frame for Image Display (STD-US-ID-SF-xxxx)
        AP_UltrasoundIDSF,
        /// Ultrasound Single Frame with Spatial Calibration (STD-US-SC-SF-xxxx)
        AP_UltrasoundSCSF,
        /// Ultrasound Single Frame with Combined Calibration (STD-US-CC-SF-xxxx)
        AP_UltrasoundCCSF,
        /// Ultrasound Single & Multi-Frame for Image Display (STD-US-ID-MF-xxxx)
        AP_UltrasoundIDMF,
        /// Ultrasound Single & Multi-Frame with Spatial Calibration (STD-UD-SC-MF-xxxx)
        AP_UltrasoundSCMF,
        /// Ultrasound Single & Multi-Frame with Combined Calibration (STD-UD-CC-MF-xxxx)
        AP_UltrasoundCCMF,
        /// 12-lead ECG Interchange on Diskette (STD-WVFM-ECG-FD)
        AP_TwelveLeadECG,
        /// Hemodynamic Waveform Interchange on Diskette (STD-WVFM-HD-FD)
        AP_HemodynamicWaveform
    };

    /** constructor (default).
     *  No DICOMDIR object is created by default (see methods createNewDicomDir,
     *  appendToDicomDir and updateDicomDir).
     */
    DicomDirInterface();

    /** destructor (virtual).
     *  Free all memory allocated by this class.
     */
    virtual ~DicomDirInterface();

    /** reset the object to its initial state.
     *  That means e.g. free memory.
     */
    void cleanup();

    /** check whether current DICOMDIR is valid.
     *  That means e.g. that it can be written to a file.
     *  @return OFTrue if DICOMDIR is valid, OFFalse otherwise
     */
    OFBool isDicomDirValid() const;

    /** create a new DICOMDIR object.
     *  This function replaces any previously existing DICOMDIR file with the specified
     *  'filename'.  If the backup mode (see 'enableBackupMode') is enabled a backup
     *  copy ('filename' + ".BAK") is created from the existing file and automatically
     *  deleted after the new file has been written without any errors.
     *  @param profile media storage application profile to be used for the DICOMDIR
     *  @param filename name of the DICOMDIR file to be created (default: 'DICOMDIR').
     *    The filename may include a fully qualified pathname.
     *  @param filesetID value of the attribute FileSetID (default: 'DCMTK_MEDIA_DEMO')
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition createNewDicomDir(const E_ApplicationProfile profile = AP_GeneralPurpose,
                                  const char *filename = DEFAULT_DICOMDIR_NAME,
                                  const char *filesetID = DEFAULT_FILESETID);

    /** create a DICOMDIR object based on an existing DICOMDIR file (append).
     *  This function can be used to append new entries to an existing DICOMDIR file.
     *  If the backup mode (see 'enableBackupMode') is enabled a backup copy ('filename'
     *  + ".BAK") is created from the existing file and automatically deleted after the
     *  new file has been written without any errors.
     *  @param profile media storage application profile to be used for the DICOMDIR.
     *    NB: The same profile should be used as for the creation of the DICOMDIR file.
     *  @param filename name of the DICOMDIR file to be appended.  The filename may
     *    include a fully qualified pathname.
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition appendToDicomDir(const E_ApplicationProfile profile,
                                 const char *filename);

    /** create a DICOMDIR object based on an existing DICOMDIR file (update).
     *  This function can be used to append new entries to and update existing entries
     *  in an existing DICOMDIR file.
     *  If the backup mode (see 'enableBackupMode') is enabled a backup copy ('filename'
     *  + ".BAK") is created from the existing file and automatically deleted after the
     *  new file has been written without any errors.
     *  @param profile media storage application profile to be used for the DICOMDIR
     *  @param filename name of the DICOMDIR file to be appended.  The filename may
     *    include a fully qualified pathname.
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition updateDicomDir(const E_ApplicationProfile profile,
                               const char *filename);

    /** write the current DICOMDIR object to file.
     *  NB: The filename has already been specified for the object creation (see above).
     *  @param encodingType flag, specifying the encoding with undefined or explicit length
     *  @param groupLength flag, specifying how to handle the group length tags
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition writeDicomDir(const E_EncodingType encodingType = EET_UndefinedLength,
                              const E_GrpLenEncoding groupLength = EGL_withoutGL);

    /** check whether specified filename is valid. i.e. conforms to the DICOM standard
     *  requirements (length, number of components and proper characters).  This function
     *  is called automatically for the following methods: checkDicomFile(), addDicomFile()
     *  and setFilesetDescriptor().  So usually there's no need to call it manually
     *  (especially not in addition to the above mentioned methods).
     *  @param filename filename to be checked for standard conformance
     *  @param allowEmpty empty filename (zero length) allowed if OFTrue
     *  @return OFTrue if filename is valid, OFFalse otherwise
     */
    OFBool isFilenameValid(const char *filename,
                           const OFBool allowEmpty = OFFalse);

    /** check whether given charset identifier is valid.
     *  Valid character sets are (see DICOM PS3.3 for details): ISO_IR 100, ISO_IR 101,
     *  ISO_IR 109, ISO_IR 110, ISO_IR 144, ISO_IR 127, ISO_IR 126, ISO_IR 138, ISO_IR 148,
     *  ISO_IR 166, ISO_IR 13, ISO_IR 192.
     *  @param charset character set identifier to be checked
     *  @return OFTrue if charset is valid, OFFalse otherwise
     */
    OFBool isCharsetValid(const char *charset);

    /** check whether specified DICOM file is suitable to be included into the DICOMDIR.
     *  This method loads the given file and checks whether it conforms to the current
     *  application profile.  Since this check is also performed by addDicomFile() there
     *  is usually no need to call this method directly.
     *  @param filename name of the DICOM file to be checked
     *  @param directory directory where the DICOM file is stored (optional).
     *    This parameter might be useful in cases where the DICOM file is not (yet)
     *    stored in the final directory (i.e. "relative" to the DICOMDIR location).
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition checkDicomFile(const char *filename,
                               const char *directory = NULL);

    /** add specified DICOM file to the current DICOMDIR.
     *  This method loads the given file, checks whether it conforms to the current
     *  application profile and finally adds it to the DICOMDIR (in case of conformance).
     *  @param filename name of the DICOM file to be added
     *  @param directory directory where the DICOM file is stored (optional).
     *    This parameter might be useful in cases where the DICOM file is not (yet)
     *    stored in the final directory (i.e. "relative" to the DICOMDIR location).
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition addDicomFile(const char *filename,
                             const char *directory = NULL);

    /** set the fileset descriptor file ID and character set.
     *  Prior to any internal modification both 'filename' and 'charset' are checked
     *  using the above checking routines.  Existence of 'filename' is not checked.
     *  NB: Requires a DICOMDIR to exist (see createNewDicomDir and appendToDicomDir).
     *  @param filename name of the fileset descriptor file to be set
     *  @param charset character set of the fileset descriptor file to be set.
     *    default: ISO Latin 1 ("ISO_IR 100"), use NULL or empty string to omit value.
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition setFilesetDescriptor(const char *filename,
                                     const char *charset = DEFAULT_DESCRIPTOR_CHARSET);

    /** set preferred size of the icon images.
     *  NB: some application profiles require a particicular icon size.
     *      In those cases this manual setting is implicitly ignored.
     *  @param size size of the icon images in pixels (1..256, initial: 64)
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition setIconSize(const unsigned int size);

    /** set filename prefix for alternative icon images.
     *  If non-empty the filename prefix is used to create the icon image from an
     *  externally stored PGM (portable gray map, 8 bit binary) file instead of the
     *  DICOM image file.  The PGM filename is: 'prefix' + 'dicom_filename'.  The
     *  image does not need to have the correct size as it is scaled automatically.
     *  @param prefix filename prefix (NULL to disable = default)
     *  @return always returns EC_Normal
     */
    OFCondition setIconPrefix(const char *prefix);

    /** set filename of default icon image.
     *  For cases that the icon image cannot be created (neither from PGM nor from
     *  DICOM file, respectively) a default icon (8 bit binary PGM) can be specified.
     *  If this image also fails to load a black icon (filled with zeros) is used.
     *  @param filename name of the default PGM file
     *  @return always returns EC_Normal
     */
    OFCondition setDefaultIcon(const char *filename);

    /** get current status of the "abort on first error" mode.
     *  See enableAbortMode() for more details.
     *  @return OFTrue if mode is enabled, OFFalse otherwise
     */
    OFBool abortMode() const
    {
        return AbortMode;
    }

    /** get current status of the "map filenames" mode.
     *  See enableMapFilenamesMode() for more details.
     *  @return OFTrue if mode is enabled, OFFalse otherwise
     */
    OFBool mapFilenamesMode() const
    {
        return MapFilenamesMode;
    }

    /** get current status of the "invent missing values" mode.
     *  See enableInventMode() for more details.
     *  @return OFTrue if mode is enabled, OFFalse otherwise
     */
    OFBool inventMode() const
    {
        return InventMode;
    }

    /** get current status of the "invent missing patient ID" mode.
     *  See enableInventPatientIDMode() for more details.
     *  @return OFTrue if mode is enabled, OFFalse otherwise
     */
    OFBool inventPatientIDMode() const
    {
        return InventPatientIDMode;
    }

    /** get current status of the "retired SOP class support" mode.
     *  See enableRetiredSOPClassSupport() for more details.
     *  @return OFTrue if support is enabled, OFFalse otherwise
     */
    OFBool retiredSOPClassSupport() const
    {
        return RetiredSOPClassSupport;
    }

    /** get current status of the "create icon images" mode.
     *  See enableIconImageMode() for more details.
     *  @return OFTrue if mode is enabled, OFFalse otherwise
     */
    OFBool iconImageMode() const
    {
        return IconImageMode;
    }

    /** get current status of the "create backup" mode.
     *  See enableBackupMode() for more details.
     *  @return OFTrue if mode is enabled, OFFalse otherwise
     */
    OFBool backupMode() const
    {
        return BackupMode;
    }

    /** get current status of the "pixel encoding check" mode.
     *  See disableEncodingCheck() for more details.
     *  @return OFTrue if check is enabled, OFFalse otherwise
     */
    OFBool encodingCheck() const
    {
        return EncodingCheck;
    }

    /** get current status of the "spatial resolution check" mode.
     *  See disableResolutionCheck() for more details.
     *  @return OFTrue if check is enabled, OFFalse otherwise
     */
    OFBool resolutionCheck() const
    {
        return ResolutionCheck;
    }

    /** get current status of the "transfer syntax check" mode.
     *  See disableTransferSyntaxCheck() for more details.
     *  @return OFTrue if check is enabled, OFFalse otherwise
     */
    OFBool transferSyntaxCheck() const
    {
        return TransferSyntaxCheck;
    }

    /** get current status of the "consistency check" mode.
     *  See enableConsistencyCheck() for more details.
     *  @return OFTrue if check is enabled, OFFalse otherwise
     */
    OFBool consistencyCheck() const
    {
        return ConsistencyCheck;
    }

    /** enable/disable the "abort on first error" mode.
     *  If the mode is enabled addDicomFile() reports an error message and
     *  returns with an error status code if something went wrong.
     *  Default: off, do not abort
     *  @param newMode enable mode if OFTrue, disable if OFFalse
     *  @return previously stored value
     */
    OFBool enableAbortMode(const OFBool newMode = OFTrue);

    /** enable/disable the "map filenames" mode.
     *  If the mode is enabled filenames are automatically mapped to DICOM format
     *  (convert lower case to upper case characters and remove trailing period).
     *  Default: off, do not map filenames
     *  @param newMode enable mode if OFTrue, disable if OFFalse
     *  @return previously stored value
     */
    OFBool enableMapFilenamesMode(const OFBool newMode = OFTrue);

    /** enable/disable the "invent missing values" mode.
     *  If the mode is enabled required DICOMDIR attributes (type 1) are
     *  invented when missing in the DICOM file.
     *  Default: off, do not invent attribute values
     *  @param newMode enable mode if OFTrue, disable if OFFalse
     *  @return previously stored value
     */
    OFBool enableInventMode(const OFBool newMode = OFTrue);

    /** enable/disable the "invent new patient ID" mode.
     *  If the mode is enabled a new PatientID is invented in case of
     *  inconsistent PatientName attributes, i.e. when different patients
     *  share the same ID.
     *  Default: off, do not invent new patient ID
     *  @param newMode enable mode if OFTrue, disable if OFFalse
     *  @return previously stored value
     */
    OFBool enableInventPatientIDMode(const OFBool newMode = OFTrue);

    /** enable/disable the "retired SOP class support" mode.
     *  If the mode is enabled retired SOP classes defined in previous editions
     *  of the DICOM standard are also accepted.
     *  Default: off, do not accept retired SOP classes
     *  @param newMode enable mode if OFTrue, disable if OFFalse
     *  @return previously stored value
     */
    OFBool enableRetiredSOPClassSupport(const OFBool newMode = OFTrue);

    /** enable/disable the "create icon images" mode.
     *  If the mode is enabled icon images are created for each IMAGE record.
     *  Please note that particular application profiles (e.g. Basic Cardiac)
     *  require an icon images to be present.  Therefore, this mode does not
     *  affect the icon images creation of such profiles.
     *  Default: off, do not create (optional) icon images
     *  @param newMode enable mode if OFTrue, disable if OFFalse
     *  @return previously stored value
     */
    OFBool enableIconImageMode(const OFBool newMode = OFTrue);

    /** disable/enable the "create backup file" mode.
     *  If this mode is disabled no backup file of an existing DICOMDIR is created.
     *  However, when appending new files to an existing DICOMDIR a _temporary_
     *  backup file "<dicomdir>.$$$" is always created.
     *  Default: on, create a backup file "<dicomdir>.BAK"
     *  @param newMode disable mode if OFFalse, enable if OFTrue
     *  @return previously stored value
     */
    OFBool disableBackupMode(const OFBool newMode = OFFalse);

    /** disable/enable the "pixel encoding check".
     *  If this mode is disabled the pixel encoding is not check for compliance
     *  with the selected application profile.  Please use this switch with care
     *  since the resulting DICOMDIR will probably violate the rules for the
     *  selected application profile.
     *  Default: on, check pixel encoding (bits allocated/stored, high bit)
     *  @param newMode disable check if OFFalse, enable if OFTrue
     *  @return previously stored value
     */
    OFBool disableEncodingCheck(const OFBool newMode = OFFalse);

    /** disable/enable the "spatial resolution check".
     *  If this mode is disabled the spatial resolution is not check for compliance
     *  with the selected application profile.  Please use this switch with care
     *  since the resulting DICOMDIR will probably violate the rules for the
     *  selected application profile.
     *  Default: on, check spatial resolution
     *  @param newMode disable check if OFFalse, enable if OFTrue
     *  @return previously stored value
     */
    OFBool disableResolutionCheck(const OFBool newMode = OFFalse);

    /** disable/enable the "transfer syntax check".
     *  If this mode is disabled the transfer syntax is not check for compliance
     *  with the selected application profile.  Please use this switch with care
     *  since the resulting DICOMDIR will probably violate the rules for the
     *  selected application profile.
     *  Default: on, check transfer syntax
     *  @param newMode disable check if OFFalse, enable if OFTrue
     *  @return previously stored value
     */
    OFBool disableTransferSyntaxCheck(const OFBool newMode = OFFalse);

    /** disable/enable the "consistency check".
     *  If this mode is disabled the consistency of newly added records with
     *  already existing ones is not checked (see 'warnAboutInconsistentAttributes'
     *  for details).
     *  Default: on, perform consistency check
     *  @param newMode disable check if OFFalse, enable if OFTrue
     *  @return previously stored value
     */
    OFBool disableConsistencyCheck(const OFBool newMode = OFFalse);

    /** add pluggable image support.
     *  NB: This plugin is required to create icon images from DICOM files!
     *  @param plugin pointer to an instance of the plugin implementation.
     *    See class DicomDirImageImplementation (dcmjpeg/include/ddpiimpl.h).
     *  @return OFTrue if successful, OFFalse otherwise
     */
    OFBool addImageSupport(DicomDirImagePlugin *plugin);

    /* -- static function -- */

    /** get name/identifier associated with the given application profile
     *  @param profile media storage application profile
     *  @return name of the application profile (e.g. "STD-GEN-CD/DVD-RAM")
     */
    static const char *getProfileName(const E_ApplicationProfile profile);


  protected:

    /** select given application profile
     *  @param profile storage media application profile to be selected
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition selectApplicationProfile(const E_ApplicationProfile profile);

    /** load and check DICOM file regarding the current application profile
     *  @param filename name of the DICOM file to be checked
     *  @param directory directory where the DICOM file is stored (optional)
     *  @param fileformat object in which the loaded data is stored
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition loadAndCheckDicomFile(const char *filename,
                                      const char *directory,
                                      DcmFileFormat &fileformat);

    /** check SOP class and transfer syntax for compliance with current profile
     *  @param metainfo object where the DICOM file meta information is stored
     *  @param dataset object where the DICOM dataset is stored
     *  @param filename name of the DICOM file to be checked
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition checkSOPClassAndXfer(DcmMetaInfo *metainfo,
                                     DcmItem *dataset,
                                     const char *filename);

    /** check attributes for compliance with Basic Cardiac application profile
     *  @param dataset object where the DICOM dataset is stored
     *  @param filename name of the DICOM file to be checked
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition checkBasicCardiacAttributes(DcmItem *dataset,
                                            const char *filename);

    /** check attributes for compliance with X-ray Angiography application profile
     *  @param dataset object where the DICOM dataset is stored
     *  @param sopClass SOP class of the DICOM data to be checked
     *  @param filename name of the DICOM file to be checked
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition checkXrayAngiographicAttributes(DcmItem *dataset,
                                                const OFString &sopClass,
                                                const char *filename);

    /** check attributes for compliance with dental radiograph application profile
     *  @param dataset object where the DICOM dataset is stored
     *  @param filename name of the DICOM file to be checked
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition checkDentalRadiographAttributes(DcmItem *dataset,
                                                const char *filename);

    /** check attributes for compliance with CT and MR application profile
     *  @param dataset object where the DICOM dataset is stored
     *  @param sopClass SOP class of the DICOM data to be checked
     *  @param filename name of the DICOM file to be checked
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition checkCTandMRAttributes(DcmItem *dataset,
                                       const OFString &sopClass,
                                       const char *filename);

    /** check attributes for compliance with Ultrasound application profiles
     *  @param dataset object where the DICOM dataset is stored
     *  @param transferSyntax transfer syntax of the DICOM data to be checked
     *  @param filename name of the DICOM file to be checked
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition checkUltrasoundAttributes(DcmItem *dataset,
                                          const OFString &transferSyntax,
                                          const char *filename);

    /** check attributes for comliance with current application profile
     *  @param metainfo object where the DICOM file meta information is stored
     *  @param dataset object where the DICOM dataset is stored
     *  @param filename name of the DICOM file to be checked
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition checkMandatoryAttributes(DcmMetaInfo *metainfo,
                                         DcmItem *dataset,
                                         const char *filename);

    /** check whether given directory record matches dataset.
     *  The check depends on the record type and is performed mainly based on
     *  the unique key defined for the particular record type (e.g. SOPInstanceUID
     *  for IMAGE records).  For PATIENT records the PatientName may also be used
     *  if the PatientID is absent.
     *  @param record directory record to be checked
     *  @param dataset DICOM dataset of the current file
     *  @return OFTrue if record matches, OFFalse otherwise
     */
    OFBool recordMatchesDataset(DcmDirectoryRecord *record,
                                DcmItem *dataset);

    /** search for a given directory record
     *  @param parent higher-level structure where the records are stored
     *  @param recordType type of directory record to be searched for
     *  @param dataset DICOM dataset of the current file
     *  @return pointer to record if found, NULL otherwise
     */
    DcmDirectoryRecord *findExistingRecord(DcmDirectoryRecord *parent,
                                           const E_DirRecType recordType,
                                           DcmItem* dataset);

    /** create or update patient record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildPatientRecord(DcmDirectoryRecord *record,
                                           DcmItem *dataset,
                                           const OFString &sourceFilename);

    /** create or update study record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildStudyRecord(DcmDirectoryRecord *record,
                                         DcmItem *dataset,
                                         const OFString &sourceFilename);

    /** create or update new series record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildSeriesRecord(DcmDirectoryRecord *record,
                                          DcmItem *dataset,
                                          const OFString &sourceFilename);

    /** create or update overlay record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildOverlayRecord(DcmDirectoryRecord *record,
                                           DcmItem *dataset,
                                           const OFString &referencedFileID,
                                           const OFString &sourceFilename);

    /** create or update modality LUT record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildModalityLutRecord(DcmDirectoryRecord *record,
                                               DcmItem *dataset,
                                               const OFString &referencedFileID,
                                               const OFString &sourceFilename);

    /** create or update VOI LUT record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildVoiLutRecord(DcmDirectoryRecord *record,
                                          DcmItem *dataset,
                                          const OFString &referencedFileID,
                                          const OFString &sourceFilename);

    /** create or update curve record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildCurveRecord(DcmDirectoryRecord *record,
                                         DcmItem *dataset,
                                         const OFString &referencedFileID,
                                         const OFString &sourceFilename);

    /** create or update structure reporting record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildStructReportRecord(DcmDirectoryRecord *record,
                                                DcmItem *dataset,
                                                const OFString &referencedFileID,
                                                const OFString &sourceFilename);

    /** create or update presentation state record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildPresentationRecord(DcmDirectoryRecord *record,
                                                DcmItem *dataset,
                                                const OFString &referencedFileID,
                                                const OFString &sourceFilename);

    /** create or update waveform record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildWaveformRecord(DcmDirectoryRecord *record,
                                            DcmItem *dataset,
                                            const OFString &referencedFileID,
                                            const OFString &sourceFilename);

    /** create or update RT dose record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildRTDoseRecord(DcmDirectoryRecord *record,
                                          DcmItem *dataset,
                                          const OFString &referencedFileID,
                                          const OFString &sourceFilename);

    /** create or update RT structure set record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildRTStructureSetRecord(DcmDirectoryRecord *record,
                                                  DcmItem *dataset,
                                                  const OFString &referencedFileID,
                                                  const OFString &sourceFilename);

    /** create or update RT plan record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildRTPlanRecord(DcmDirectoryRecord *record,
                                          DcmItem *dataset,
                                          const OFString &referencedFileID,
                                          const OFString &sourceFilename);

    /** create or update RT treatment record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildRTTreatmentRecord(DcmDirectoryRecord *record,
                                               DcmItem *dataset,
                                               const OFString &referencedFileID,
                                               const OFString &sourceFilename);

    /** create or update stored print record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildStoredPrintRecord(DcmDirectoryRecord *record,
                                               DcmItem *dataset,
                                               const OFString &referencedFileID,
                                               const OFString &sourceFilename);

    /** create or update key object doc record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildKeyObjectDocRecord(DcmDirectoryRecord *record,
                                                DcmItem *dataset,
                                                const OFString &referencedFileID,
                                                const OFString &sourceFilename);

    /** create or update registration record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildRegistrationRecord(DcmDirectoryRecord *record,
                                                DcmItem *dataset,
                                                const OFString &referencedFileID,
                                                const OFString &sourceFilename);

    /** create or update fiducial record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildFiducialRecord(DcmDirectoryRecord *record,
                                            DcmItem *dataset,
                                            const OFString &referencedFileID,
                                            const OFString &sourceFilename);

    /** create or update raw data record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildRawDataRecord(DcmDirectoryRecord *record,
                                           DcmItem *dataset,
                                           const OFString &referencedFileID,
                                           const OFString &sourceFilename);

    /** create or update spectroscopy record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildSpectroscopyRecord(DcmDirectoryRecord *record,
                                                DcmItem *dataset,
                                                const OFString &referencedFileID,
                                                const OFString &sourceFilename);

    /** create or update encap doc record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildEncapDocRecord(DcmDirectoryRecord *record,
                                            DcmItem *dataset,
                                            const OFString &referencedFileID,
                                            const OFString &sourceFilename);

    /** create or update value map record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildValueMapRecord(DcmDirectoryRecord *record,
                                            DcmItem *dataset,
                                            const OFString &referencedFileID,
                                            const OFString &sourceFilename);

    /** create or update hanging protocol record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildHangingProtocolRecord(DcmDirectoryRecord *record,
                                                   DcmItem *dataset,
                                                   const OFString &referencedFileID,
                                                   const OFString &sourceFilename);

    /** create or update stereometric record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildStereometricRecord(DcmDirectoryRecord *record,
                                                DcmItem *dataset,
                                                const OFString &referencedFileID,
                                                const OFString &sourceFilename);

    /** create or update palette record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildPaletteRecord(DcmDirectoryRecord *record,
                                           DcmItem *dataset,
                                           const OFString &referencedFileID,
                                           const OFString &sourceFilename);

    /** create or update surface record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildSurfaceRecord(DcmDirectoryRecord *record,
                                           DcmItem *dataset,
                                           const OFString &referencedFileID,
                                           const OFString &sourceFilename);

    /** create or update measurement record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildMeasurementRecord(DcmDirectoryRecord *record,
                                               DcmItem *dataset,
                                               const OFString &referencedFileID,
                                               const OFString &sourceFilename);

    /** create or update implant record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildImplantRecord(DcmDirectoryRecord *record,
                                           DcmItem *dataset,
                                           const OFString &referencedFileID,
                                           const OFString &sourceFilename);

    /** create or update implant group record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildImplantGroupRecord(DcmDirectoryRecord *record,
                                                DcmItem *dataset,
                                                const OFString &referencedFileID,
                                                const OFString &sourceFilename);

    /** create or update implant assy record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildImplantAssyRecord(DcmDirectoryRecord *record,
                                               DcmItem *dataset,
                                               const OFString &referencedFileID,
                                               const OFString &sourceFilename);

    /** create or update image record and copy required values from dataset
     *  @param record record to be updated, use NULL to create a new one
     *  @param dataset DICOM dataset of the current file
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new or updated record, NULL if an error occurred
     */
    DcmDirectoryRecord *buildImageRecord(DcmDirectoryRecord *record,
                                         DcmItem *dataset,
                                         const OFString &referencedFileID,
                                         const OFString &sourceFilename);

    /** create icon image from given PGM (portable gray map) file.
     *  Please note that only grayscale images in binary format are currently
     *  supported (magic word "P5").  Memory has to be allocated by the caller.
     *  @param filename name of the PGM file to be loaded
     *  @param pixel pointer to memory buffer where the pixel data are to be stored
     *  @param count number of bytes allocated for the 'pixel' memory buffer
     *  @param width width of the scaled icon image (in pixels )
     *  @param height height of the scaled icon image (in pixels)
     *  @return OFTrue if successful, OFFalse otherwise
     */
    OFBool getIconFromFile(const OFString &filename,
                           Uint8 *pixel,
                           const unsigned long count,
                           const unsigned int width,
                           const unsigned int height);

    /** create icon image from DICOM dataset.
     *  Please note that the memory buffer has to be allocated by the caller.
     *  @param dataset DICOM dataset from which the icon image is created
     *  @param pixel pointer to memory buffer where the pixel data are to be stored
     *  @param count number of bytes allocated for the 'pixel' memory buffer
     *  @param width width of the scaled icon image (in pixels )
     *  @param height height of the scaled icon image (in pixels)
     *  @return OFTrue if successful, OFFalse otherwise
     */
    OFBool getIconFromDataset(DcmItem *dataset,
                              Uint8 *pixel,
                              const unsigned long count,
                              const unsigned int width,
                              const unsigned int height);

    /** add icon image sequence to directory record.
     *  If the icon image cannot be created from the DICOM dataset and there is no
     *  PGM file specified (neither for the particular image not a default one) a
     *  black image is used instead.
     *  @param record directory record where the icon image is stored
     *  @param dataset DICOM dataset from which the icon image is possibly created
     *  @param size resolution of the icon image to be created (width and height)
     *  @param sourceFilename name of the source DICOM file
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition addIconImage(DcmDirectoryRecord *record,
                             DcmItem *dataset,
                             const unsigned int size,
                             const OFString &sourceFilename);

    /** add child record to a given parent record.
     *  A new record is only added if it does not already exist.
     *  @param parent parent record (add new record as a child of this one)
     *  @param recordType type of directory record to be created
     *  @param dataset DICOM dataset containing data of the new record
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     *  @return pointer to new/existing record, NULL if an error occurred
     */
    DcmDirectoryRecord *addRecord(DcmDirectoryRecord *parent,
                                  const E_DirRecType recordType,
                                  DcmItem *dataset,
                                  const OFString &referencedFileID,
                                  const OFString &sourceFilename);

    /** check referenced SOP instance for consistency with a new directory record
     *  @param record directory record to be checked
     *  @param dataset DICOM dataset containing data of the new record
     *  @param referencedFileID value of the Referenced File ID attribute
     *  @param sourceFilename name of the source DICOM file
     */
    OFBool checkReferencedSOPInstance(DcmDirectoryRecord *record,
                                      DcmItem *dataset,
                                      const OFString &referencedFileID,
                                      const OFString &sourceFilename);

    /** invent missing type 1 attributes for all child records (from patient level)
     *  @param parent invent missing attributes for all children of this record (root)
     *  @param recurse invent missing attributes also for lower level records if OFTrue.
     *    Only invent attributes for the top (patient) level if OFFalse.
     */
    void inventMissingAttributes(DcmDirectoryRecord *parent,
                                 const OFBool recurse = OFTrue);

    /** invent missing type 1 attributes for all child records (from study level)
     *  @param parent invent missing attributes for all children of this record
     */
    void inventMissingStudyLevelAttributes(DcmDirectoryRecord *parent);

    /** invent missing type 1 attributes for all child records (from series level)
     *  @param parent invent missing attributes for all children of this record
     */
    void inventMissingSeriesLevelAttributes(DcmDirectoryRecord *parent);

    /** invent missing type 1 attributes for all child records (from instance level)
     *  @param parent invent missing attributes for all children of this record
     */
    void inventMissingInstanceLevelAttributes(DcmDirectoryRecord *parent);

    /** create backup of a given file
     *  @param filename name of the file to be backuped
     */
    void createDicomDirBackup(const char *filename);

    /** delete backup file if one has been created
     */
    void deleteDicomDirBackup();

    /** print a message that the value of a given tag is unexpected.
     *  The output format is: "{Error|Warning}: attribute \<key.tagName()\> \<key\>
     *  has other value than expected[ in file: \<filename\>]"
     *  @param key affected tag
     *  @param filename name of the file (optional, might be NULL)
     *  @param errorMsg print error message if OFTrue, a warning message otherwise
     */
    void printUnexpectedValueMessage(const DcmTagKey &key,
                                     const char *filename = NULL,
                                     const OFBool errorMsg = OFTrue);

    /** print an error message that a required attribute is missing/empty.
     *  The output format is: "Error: <error.text()>: required attribute \<key.tagName()\>
     *  \<key>\ {empty|missing}[ in file: \<filename\>]"
     *  @param key affected tag
     *  @param filename name of the file (optional, might be NULL)
     *  @param emptyMsg print "empty" if OFTrue, "missing" otherwise
     */
    void printRequiredAttributeMessage(const DcmTagKey &key,
                                       const char *filename = NULL,
                                       const OFBool emptyMsg = OFFalse);

    /** print an error message that something went wrong with an attribute.
     *  The output format is: "Error: \<error.text()\>: [cannot \<operation\> ]\<key.tagName()\>
     *  \<key\>"
     *  @param key affected tag
     *  @param error status to be reported (only if ".bad()")
     *  @param operation name of the operation that failed (optional, might be NULL)
     */
    void printAttributeErrorMessage(const DcmTagKey &key,
                                    const OFCondition &error,
                                    const char *operation);

    /** print an error message that something went wrong with a given record.
     *  The output format is: "Error: \<error.text()\>: [cannot \<operation\> ]\<recordType\>
     *  directory record"
     *  @param error status to be reported (only if ".bad()")
     *  @param recordType type of directory record which caused the error
     *  @param operation name of the operation that failed (optional, might be NULL)
     */
    void printRecordErrorMessage(const OFCondition &error,
                                 const E_DirRecType recordType,
                                 const char *operation);

    /** copy contents of specified file
     *  @param fromFilename name of the file to be copied
     *  @param toFilename name of the new file (copy of 'fromFilename')
     *  @return OFTrue if successful, OFFalse otherwise
     */
    OFBool copyFile(const char *fromFilename,
                    const char *toFilename);

    /** see if all the attributes in record match the values in dataset and warn if not
     *  @param record directory record to be checked
     *  @param dataset DICOM dataset to be compared with the directory record
     *  @param sourceFilename name of the source DICOM file
     *  @param abortCheck flag indicating whether to abort on the first inconsistent record
     *  @return OFTrue in case of any inconsistency, OFFalse otherwise
     */
    OFBool warnAboutInconsistentAttributes(DcmDirectoryRecord *record,
                                           DcmItem *dataset,
                                           const OFString &sourceFilename,
                                           const OFBool abortCheck = OFFalse);

    /** check whether given fileset ID is valid
     *  @param filesetID fileset ID to be checked
     *  @return OFTrue if ID is valid, OFFalse otherwise
     */
    OFBool checkFilesetID(const OFString &filesetID);

    /** check whether given tag exists in the DICOM dataset
     *  @param dataset DICOM dataset to be checked
     *  @param key tag to be searched for
     *  @param filename of the file (optional, report any error if specified)
     *  @return OFTrue if tag exists, OFFalse otherwise
     */
    OFBool checkExists(DcmItem *dataset,
                       const DcmTagKey &key,
                       const char *filename = NULL);

    /** check whether given tag exists with a value in the DICOM dataset
     *  @param dataset DICOM dataset to be checked
     *  @param key tag to be searched for
     *  @param filename of the file (optional, report any error if specified)
     *  @return OFTrue if tag exists with value, OFFalse otherwise
     */
    OFBool checkExistsWithValue(DcmItem *dataset,
                                const DcmTagKey &key,
                                const char *filename = NULL);

    /** check whether given tag exists in the DICOM dataset and has the expected string value
     *  @param dataset DICOM dataset to be checked
     *  @param key tag to be searched for
     *  @param value expected string value
     *  @param filename of the file (optional, report any error if specified)
     *  @return OFTrue if tag exists with given string value, OFFalse otherwise
     */
    OFBool checkExistsWithStringValue(DcmItem *dataset,
                                      const DcmTagKey &key,
                                      const OFString &value,
                                      const char *filename = NULL);

    /** check whether given tag exists in the DICOM dataset and has the expected integer value
     *  @param dataset DICOM dataset to be checked
     *  @param key tag to be searched for
     *  @param value expected integer value
     *  @param filename of the file (optional, report any error if specified)
     *  @param reject report an "Error" if OFTrue, a "Warning" if OFFalse
     *  @return OFTrue if tag exists with given string value, OFFalse otherwise
     */
    OFBool checkExistsWithIntegerValue(DcmItem *dataset,
                                       const DcmTagKey &key,
                                       const long value,
                                       const char *filename = NULL,
                                       const OFBool reject = OFTrue);

    /** check whether given tag exists in the DICOM dataset and has an integer value in the expected range
     *  @param dataset DICOM dataset to be checked
     *  @param key tag to be searched for
     *  @param min minimum integer value of the expected range
     *  @param max maximum integer value of the expected range
     *  @param filename of the file (optional, report any error if specified)
     *  @param reject report an "Error" if OFTrue, a "Warning" if OFFalse
     *  @return OFTrue if tag exists with given string value, OFFalse otherwise
     */
    OFBool checkExistsWithMinMaxValue(DcmItem *dataset,
                                      const DcmTagKey &key,
                                      const long min,
                                      const long max,
                                      const char *filename = NULL,
                                      const OFBool reject = OFTrue);

    /** get string value from dataset and report an error (if any)
     *  @param dataset dataset from which the string value is to be retrieved
     *  @param key tag of the attribute to be retrieved
     *  @param result string variable in which the resulting value is stored
     *  @param searchIntoSub flag indicating whether to do a deep search or not
     *  @return reference to the resulting string value (parameter 'result')
     */
    OFString &getStringFromDataset(DcmItem *dataset,
                                   const DcmTagKey &key,
                                   OFString &result,
                                   OFBool searchIntoSub = OFFalse);

    /** get string value component from dataset and report an error (if any)
     *  @param dataset dataset from which the string value is to be retrieved
     *  @param key tag of the attribute to be retrieved
     *  @param result string variable in which the resulting value is stored
     *  @param pos index of the component in case of multi-valued elements (0..vm-1)
     *  @param searchIntoSub flag indicating whether to do a deep search or not
     *  @return reference to the resulting string value (parameter 'result')
     */
    OFString &getStringComponentFromDataset(DcmItem *dataset,
                                            const DcmTagKey &key,
                                            OFString &result,
                                            const unsigned long pos,
                                            OFBool searchIntoSub = OFFalse);

    /** get string value from file and report an error (if any)
     *  @param filename name of the file from which the string value is to be retrieved
     *  @param key tag of the attribute to be retrieved
     *  @param result string variable in which the resulting value is stored
     *  @param searchIntoSub flag indicating whether to do a deep search or not
     *  @return reference to the resulting string value (parameter 'result')
     */
    OFString &getStringFromFile(const char *filename,
                                const DcmTagKey &key,
                                OFString &result,
                                OFBool searchIntoSub = OFFalse);

    /** copy element from dataset to directory record
     *  @param dataset DICOM dataset containing the original data
     *  @param key tag of the element to be copied
     *  @param record directory record to which the element is to be copied
     *  @param sourceFilename name of the source DICOM file
     *  @param optional flag indicating whether the element is optional or required
     *  @param copyEmpty flag indicating whether to copy an empty element (no value)
     */
    void copyElement(DcmItem *dataset,
                     const DcmTagKey &key,
                     DcmDirectoryRecord *record,
                     const OFString &sourceFilename,
                     const OFBool optional = OFFalse,
                     const OFBool copyEmpty = OFTrue);

    /** copy type 1 element from dataset to directory record
     *  @param dataset DICOM dataset containing the original data
     *  @param key tag of the element to be copied
     *  @param record directory record to which the element is to be copied
     *  @param sourceFilename name of the source DICOM file
     */
    void copyElementType1(DcmItem *dataset,
                          const DcmTagKey &key,
                          DcmDirectoryRecord *record,
                          const OFString &sourceFilename)
    {
        copyElement(dataset, key, record, sourceFilename, OFFalse /*optional*/, OFFalse /*copyEmpty*/);
    }

    /** copy type 1C element from dataset to directory record
     *  @param dataset DICOM dataset containing the original data
     *  @param key tag of the element to be copied
     *  @param record directory record to which the element is to be copied
     *  @param sourceFilename name of the source DICOM file
     */
    void copyElementType1C(DcmItem *dataset,
                          const DcmTagKey &key,
                          DcmDirectoryRecord *record,
                          const OFString &sourceFilename)
    {
        copyElement(dataset, key, record, sourceFilename, OFTrue /*optional*/, OFFalse /*copyEmpty*/);
    }

    /** copy type 2 element from dataset to directory record
     *  @param dataset DICOM dataset containing the original data
     *  @param key tag of the element to be copied
     *  @param record directory record to which the element is to be copied
     *  @param sourceFilename name of the source DICOM file
     */
    void copyElementType2(DcmItem *dataset,
                          const DcmTagKey &key,
                          DcmDirectoryRecord *record,
                          const OFString &sourceFilename)
    {
        copyElement(dataset, key, record, sourceFilename, OFFalse /*optional*/, OFTrue /*copyEmpty*/);
    }

    /** copy type 3 element from dataset to directory record
     *  @param dataset DICOM dataset containing the original data
     *  @param key tag of the element to be copied
     *  @param record directory record to which the element is to be copied
     *  @param sourceFilename name of the source DICOM file
     */
    void copyElementType3(DcmItem *dataset,
                          const DcmTagKey &key,
                          DcmDirectoryRecord *record,
                          const OFString &sourceFilename)
    {
        copyElement(dataset, key, record, sourceFilename, OFTrue /*optional*/, OFTrue /*copyEmpty*/);
    }

    /** copy optional string value from dataset to directory record
     *  @param dataset DICOM dataset containing the original data
     *  @param key tag of the element value to be copied
     *  @param record directory record to which the element value is to be copied
     *  @param sourceFilename name of the source DICOM file
     *  @param defaultValue default string value used in case the element is missing
     *  @param printWarning print warning message if element does not exist (with a value)
     *    and no default value is given
     */
    void copyStringWithDefault(DcmItem *dataset,
                               const DcmTagKey &key,
                               DcmDirectoryRecord *record,
                               const OFString &sourceFilename,
                               const char *defaultValue = "",
                               const OFBool printWarning = OFFalse);

    /** compare string attribute from dataset and record and report any deviation
     *  @param dataset DICOM dataset where the string value is stored
     *  @param datKey tag of the string value to be compared (dataset)
     *  @param record directory record where the string value is stored
     *  @param recKey tag of the string value to be compared (record)
     *  @param sourceFilename name of the source DICOM file
     *  @param errorMsg report error if true, warning message otherwise (default)
     *  @return OFTrue if string values are identical, OFFalse otherwise
     */
    OFBool compareStringAttributes(DcmItem *dataset,
                                   const DcmTagKey &datKey,
                                   DcmDirectoryRecord *record,
                                   const DcmTagKey &recKey,
                                   const OFString &sourceFilename,
                                   const OFBool errorMsg = OFFalse);

    /** compare sequence attribute from dataset and record and report any deviation
     *  @param dataset DICOM dataset where the sequence value is stored
     *  @param key tag of the sequence to be compared
     *  @param record directory record where the sequence value is stored
     *  @param sourceFilename name of the source DICOM file
     *  @return OFTrue if sequences are identical, OFFalse otherwise
     */
    OFBool compareSequenceAttributes(DcmItem *dataset,
                                     DcmTagKey &key,
                                     DcmDirectoryRecord *record,
                                     const OFString &sourceFilename);

    /** set default value (number or prefix and number) to a given tag
     *  @param record directory record where the elements are stored
     *  @param key tag of the element to be modifed
     *  @param number numeric value to be set as a the element value
     *  @param prefix optional prefix to be added to the numeric value
     */
    void setDefaultValue(DcmDirectoryRecord *record,
                         const DcmTagKey &key,
                         const unsigned long number,
                         const char *prefix = NULL);


  private:

    /// pointer to the current DICOMDIR object
    DcmDicomDir *DicomDir;

    /// pointer to the optional image plugin (required for icon image support)
    DicomDirImagePlugin *ImagePlugin;

    /// currently selected application profile
    E_ApplicationProfile ApplicationProfile;

    /// create DICOMDIR backup
    OFBool BackupMode;
    /// abort on first inconsistent record
    OFBool AbortMode;
    /// automatically map filenames
    OFBool MapFilenamesMode;
    /// invent missing attributes mode
    OFBool InventMode;
    /// invent missing patient ID mode
    OFBool InventPatientIDMode;
    /// support retired SOP classes
    OFBool RetiredSOPClassSupport;
    /// check pixel encoding
    OFBool EncodingCheck;
    /// check image resolution
    OFBool ResolutionCheck;
    /// check transfer syntax
    OFBool TransferSyntaxCheck;
    /// check consistency of newly added record
    OFBool ConsistencyCheck;
    /// create icon images
    OFBool IconImageMode;
    /// update existing fileset
    OFBool FilesetUpdateMode;

    /// name of the DICOMDIR backup file
    OFString BackupFilename;
    /// flag indicating whether a backup has been created
    OFBool BackupCreated;

    /// size of the optional icon image in pixels
    unsigned int IconSize;
    /// filename prefix for the external icon images
    OFString IconPrefix;
    /// filename of the default icon (if any)
    OFString DefaultIcon;

    /// flag indicating whether RLE decompression is supported
    OFBool RLESupport;
    /// flag indicating whether JPEG decompression is supported
    OFBool JPEGSupport;
    /// flag indicating whether JPEG 2000 decompression is supported
    OFBool JP2KSupport;

    /// current patient number used to invent missing attribute values
    unsigned long AutoPatientNumber;
    /// current study number used to invent missing attribute values
    unsigned long AutoStudyNumber;
    /// current series number used to invent missing attribute values
    unsigned long AutoSeriesNumber;
    /// current instance number used to invent missing attribute values
    unsigned long AutoInstanceNumber;
    /// current overlay number used to invent missing attribute values
    unsigned long AutoOverlayNumber;
    /// current LUT number used to invent missing attribute values
    unsigned long AutoLutNumber;
    /// current curve number used to invent missing attribute values
    unsigned long AutoCurveNumber;

    /// private undefined copy constructor
    DicomDirInterface(const DicomDirInterface &obj);

    /// private undefined assignment operator
    DicomDirInterface &operator=(const DicomDirInterface &obj);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dcddirif.h,v $
 * Revision 1.23  2010-11-05 13:11:11  joergr
 * Added support for new directory record types IMPLANT, IMPLANT GROUP and
 * IMPLANT ASSY from Supplement 131 (Implant Templates).
 *
 * Revision 1.22  2010-10-14 13:15:40  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.21  2010-10-04 16:14:53  joergr
 * Fixed various Doxygen API documentation issues.
 *
 * Revision 1.20  2010-10-01 08:09:31  joergr
 * Added support for new non-image Storage SOP Classes that require the new
 * directory record type MEASUREMENT. Also fixed issues with other record types.
 *
 * Revision 1.19  2010-09-30 17:18:10  joergr
 * Added support for new non-image Storage SOP Classes that require the new
 * directory record types PALETTE and SURFACE. Also updated existing records.
 *
 * Revision 1.18  2010-08-10 11:02:58  uli
 * Removed undefined function printFileErrorMessage().
 *
 * Revision 1.17  2010-08-09 13:02:56  joergr
 * Updated data dictionary to 2009 edition of the DICOM standard. From now on,
 * the official "keyword" is used for the attribute name which results in a
 * number of minor changes (e.g. "PatientsName" is now called "PatientName").
 *
 * Revision 1.16  2009-11-25 13:31:05  joergr
 * Adapted code for new approach to access individual frames of a DICOM image.
 *
 * Revision 1.15  2009-11-04 09:58:07  uli
 * Switched to logging mechanism provided by the "new" oflog module
 *
 * Revision 1.14  2009-01-15 10:16:40  joergr
 * Added check whether (possibly required) JPEG 2000 decoder is registered.
 *
 * Revision 1.13  2008-06-23 12:05:37  joergr
 * Added check on value representation of data elements copied from the
 * referenced DICOM file to the DICOMDIR (compare VR with data dictionary).
 *
 * Revision 1.12  2007/02/02 16:01:51  joergr
 * Added error message when existing SOP instance is inconsistent with new
 * directory record in update mode (e.g. different SOP class UID).
 * Fixed incomplete warning message in update mode (filename was missing).
 *
 * Revision 1.11  2007/01/10 13:02:59  joergr
 * Added new option that enables support for retired SOP classes.
 *
 * Revision 1.10  2006/12/15 14:56:57  joergr
 * Added new option that allows to update existing entries in a DICOMDIR. This
 * also adds support for mixed media stored application profiles.
 * Changed name of enum value for the MPEG2-DVD application profile in order to
 * be more consistent with other names.
 * Slightly revised handling of type 1, 1C and 2 elements in Directory Records.
 * Fixed small bug in cardiac application profiles when checking the ImageType
 * (0008,0008).
 *
 * Revision 1.9  2006/07/27 13:05:05  joergr
 * Added support for DICOMDIR record type "STEREOMETRIC" (CP 628).
 *
 * Revision 1.8  2005/12/15 15:40:48  joergr
 * Removed unsused parameter.
 *
 * Revision 1.7  2005/12/08 16:28:03  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.6  2005/10/27 13:31:21  joergr
 * Added support for Encapsulated Document, Real World Value Mapping and
 * Hanging Protocol objects to DICOMDIR tools.
 *
 * Revision 1.5  2005/06/13 14:36:41  joergr
 * Added new options to disable check on pixel encoding and transfer syntax.
 *
 * Revision 1.4  2005/03/09 17:53:34  joergr
 * Added support for new Media Storage Application Profiles according to DICOM
 * PS 3.12-2004. Removed support for non-standard conformant "No profile".
 * Added support for UTF-8 for the contents of the fileset descriptor file.
 *
 * Revision 1.3  2004/02/13 17:36:46  joergr
 * Added support for new directory records RAW DATA and SPECTROSCOPY introduced
 * with CP 343.
 *
 * Revision 1.2  2004/02/13 14:11:15  joergr
 * Added support for new directory records REGISTRATION and FIDUCIAL introduced
 * with supplement 73 (Spatial Registration Storage SOP Classes).
 *
 * Revision 1.1  2003/08/12 14:35:00  joergr
 * Added new interface class for simplified creation of a DICOMDIR.
 *
 *
 */
