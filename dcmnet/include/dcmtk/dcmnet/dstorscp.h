/*
 *
 *  Copyright (C) 2013-2014, OFFIS e.V.
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
 *  Module:  dcmnet
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DICOM Storage Service Class Provider (SCP)
 *
 */

#ifndef DSTORSCP_H
#define DSTORSCP_H

#include "dcmtk/config/osconfig.h"  /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/offname.h"    /* for OFFilenameCreator */
#include "dcmtk/dcmnet/scp.h"       /* for base class DcmSCP */


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Interface class for a Storage Service Class Provider (SCP).
 *  This class supports C-STORE and C-ECHO messages as an SCP.  The received datasets are
 *  always stored as DICOM files with the same Transfer Syntax as used for the network
 *  transmission.  Both the generation of the directory structure and the filenames can
 *  be configured by the user.
 *  @note The current implementation always requires to load a so-called association
 *    negotiation profile from a configuration file, which specifies the list of
 *    Presentation Contexts (i.e. combination of SOP Class and Transfer Syntaxes) to be
 *    supported by this Storage SCP.  If no association negotiation profile is loaded,
 *    the SCP will only support the Verification SOP Class (with Default Transfer Syntax,
 *    i.e. C-ECHO with Implicit VR Little Endian), which is required for all DICOM
 *    Service Class Providers.
 */
class DCMTK_DCMNET_EXPORT DcmStorageSCP
  : public DcmSCP
{

  public:

    // --- public types ---

    /** modes for generating subdirectories
     */
    enum E_DirectoryGenerationMode
    {
        /// do not generate any subdirectories
        DGM_NoSubdirectory,
        /// generated subdirectories based on the Series Date (0008,0021)
        DGM_SeriesDate,
        /// default value
        DGM_Default = DGM_NoSubdirectory
    };

    /** modes for generating filenames
     */
    enum E_FilenameGenerationMode
    {
        /// generate filename from SOP Instance UID (0008,0018)
        FGM_SOPInstanceUID,
        /// generate unique filename based on new UID
        FGM_UniqueFromNewUID,
        /// generate short pseudo-random unique filename
        FGM_ShortUniquePseudoRandom,
        /// generate filename from current system time
        FGM_CurrentSystemTime,
        /// default value
        FGM_Default = FGM_SOPInstanceUID
    };

    /** modes specifying whether and how to store the received datasets
     */
    enum E_DatasetStorageMode
    {
        /// receive dataset in memory, perform some conversions and store it to file
        DGM_StoreToFile,
        /// receive dataset directly to file, i.e. write data exactly as received
        DGM_StoreBitPreserving,
        /// receive dataset in memory, but do not store it to file
        DSM_Ignore,
        /// default value
        DSM_Default = DGM_StoreToFile
    };

    // --- public methods ---

    /** default constructor
     */
    DcmStorageSCP();

    /** destructor
     */
    virtual ~DcmStorageSCP();

    /** clear the internal member variables, i.e.\ set them to their default values
     */
    virtual void clear();

    // get methods

    /** get the output directory to be used for the storage of the received DICOM
     *  datasets.  Depending on the current mode for generating subdirectories (see
     *  getDirectoryGenerationMode()), further substructures are created automatically.
     *  @return name of the output directory that is used for storing the received
     *    DICOM datasets
     */
    const OFString &getOutputDirectory() const;

    /** get the mode for generating subdirectories used to store the received datasets
     *  @return current mode for generating subdirectories
     *    (see DcmStorageSCP::E_DirectoryGenerationMode)
     */
    E_DirectoryGenerationMode getDirectoryGenerationMode() const;

    /** get the mode for generating filenames for the received datasets
     *  @return current mode for generating filenames
     *    (see DcmStorageSCP::E_FilenameGenerationMode)
     */
    E_FilenameGenerationMode getFilenameGenerationMode() const;

    /** get the filename extension that is appended to the generated filenames
     *  @return current filename extension that is appended to the generated filenames
     */
    const OFString &getFilenameExtension() const;

    /** get the mode specifying whether and how to store the received datasets
     *  @return current mode specifying whether and how to store the received datasets
     *    (see DcmStorageSCP::E_DatasetStorageMode)
     */
    E_DatasetStorageMode getDatasetStorageMode() const;

    // set methods

    /** specify the output directory to be used for the storage of the received DICOM
     *  datasets.  Depending on the current mode for generating subdirectories (see
     *  getDirectoryGenerationMode()), further substructures are created automatically.
     *  Before setting the new directory name, it is checked whether the specified
     *  directory exists and is writable.  By default, the current directory is used.
     *  @param  directory  name of the output directory to be used
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setOutputDirectory(const OFString &directory);

    /** set the mode for generating subdirectories used to store the received datasets.
     *  These subdirectories are created below the specified output directory.
     *  The default value is specified by DcmStorageSCP::DGM_Default.
     *  @param  mode  mode to be set for generating subdirectories
     *               (see DcmStorageSCP::E_DirectoryGenerationMode)
     */
    void setDirectoryGenerationMode(const E_DirectoryGenerationMode mode);

    /** set the mode for generating filenames for the received datasets.
     *  The default value is specified by DcmStorageSCP::FGM_Default.
     *  @param  mode  mode to be set for generating filenames
     *               (see DcmStorageSCP::E_FilenameGenerationMode)
     */
    void setFilenameGenerationMode(const E_FilenameGenerationMode mode);

    /** specify the filename extension to be appended to the generated filenames.
     *  The default value is specified by DcmStorageSCP::DEF_FilenameExtension
     *  (empty string).  A typical non-empty value would be ".dcm" (i.e. that "."
     *  has to be specified explicitly).
     *  @param  extension  filename extension appended to the generated filenames
     */
    void setFilenameExtension(const OFString &extension);

    /** set the mode specifying how to store the received datasets.  This mode also
     *  allows for specifying whether to store the received datasets at all.
     *  The default value is specified by DcmStorageSCP::DSM_Default.
     *  @param  mode  mode to be set specifying whether and how to store the received
     *                datasets (see DcmStorageSCP::E_DatasetStorageMode)
     */
    void setDatasetStorageMode(const E_DatasetStorageMode mode);

    // other methods

    /** load an association negotiation profile from a configuration file.  This profile
     *  specifies which Presentation Contexts (i.e. combination of SOP Class and Transfer
     *  Syntaxes) are supported by this Storage SCP.  If no association negotiation
     *  profile is loaded, the SCP will only support the Verification SOP Class (with
     *  Default Transfer Syntax, i.e. C-ECHO with Implicit VR Litte Endian), which is
     *  required for all DICOM Service Class Providers.
     *  The format and semantics of this configuration file are described in
     *  \ref file_asconfig.
     *  @param  filename  filename of the configuration file to be loaded
     *  @param  profile   name of the profile specified in the configuration file to be
     *                    used
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition loadAssociationConfiguration(const OFString &filename,
                                             const OFString &profile);


  protected:

    /** handler that is called for each incoming command message.  This handler supports
     *  C-ECHO and C-STORE requests.  All other messages will be reported as an error.
     *  After a valid C-STORE request has been received, the request and associated
     *  dataset will be checked and further processed by checkAndProcessSTORERequest().
     *  @param  incomingMsg  pointer to data structure containing the DIMSE message
     *  @param  presInfo     additional information on the Presentation Context used
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition handleIncomingCommand(T_DIMSE_Message *incomingMsg,
                                              const DcmPresentationContextInfo &presInfo);

    /** check the given C-STORE request and dataset for validity.  This method is called
     *  by handleIncomingCommand() before sending the response in order to determine the
     *  DIMSE status code to be used for the response message.  If this check has been
     *  passed successfully, the received dataset is stored as a DICOM file.
     *  @param  reqMessage  C-STORE request message data structure to be checked and
     *                      processed
     *  @param  fileformat  DICOM fileformat structure containing the C-STORE request
     *                      dataset to be checked and processed
     *  @return DIMSE status code to be used for the C-STORE response
     */
    virtual Uint16 checkAndProcessSTORERequest(const T_DIMSE_C_StoreRQ &reqMessage,
                                               DcmFileFormat &fileformat);

    /** generate a directory and file name for a DICOM dataset that will be received.
     *  The naming scheme can be specified by the methods setDirectoryGenerationMode(),
     *  setFilenameGenerationMode() and setFilenameExtension().
     *  Please note that this method also creates the directory structure (if needed).
     *  @param  reqMessage  C-STORE request message data structure used to generate the
     *                      filename (depending on the specified options)
     *  @param  filename    reference to variable that will store the resulting filename
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition generateSTORERequestFilename(const T_DIMSE_C_StoreRQ &reqMessage,
                                                     OFString &filename);

    /** notification handler that is called for each DICOM object that has been received
     *  with a C-STORE request and stored as a DICOM file
     *  @param  filename        filename (with full path) of the object stored
     *  @param  sopClassUID     SOP Class UID of the object stored
     *  @param  sopInstanceUID  SOP Instance UID of the object stored
     *  @param  dataset         pointer to dataset of the object stored (or NULL if the
     *                          dataset has been stored directly to file).
     *                          Please note that this dataset will be deleted by the calling
     *                          method, so do not store any references to it!
     */
    virtual void notifyInstanceStored(const OFString &filename,
                                      const OFString &sopClassUID,
                                      const OFString &sopInstanceUID,
                                      DcmDataset *dataset = NULL) const;

    /** generate a directory and file name for a DICOM dataset that has been received.
     *  The naming scheme can be specified by the methods setDirectoryGenerationMode(),
     *  setFilenameGenerationMode() and setFilenameExtension().
     *  Please note that this method only generates the names but neither creates the
     *  directory structure nor the DICOM file.
     *  @param  filename        reference to variable that will store the resulting
     *                          filename
     *  @param  directoryName   reference to variable that will store the resulting
     *                          directory name (including the main output directory)
     *  @param  sopClassUID     SOP Class UID of the DICOM object.  This is both an
     *                          input and output parameter.  If an empty value is passed
     *                          to this method, the value of the data element SOP Class
     *                          UID (0008,0016) is determined from the DICOM dataset.
     *  @param  sopInstanceUID  SOP Instance UID of the DICOM object.  This is both an
     *                          input and output parameter.  If an empty value is passed
     *                          to this method, the value of the data element SOP
     *                          Instance UID (0008,0018) is determined from the dataset.
     *  @param  dataset         pointer to dataset for which the directory and file name
     *                          is to be generated (optional)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition generateDirAndFilename(OFString &filename,
                                               OFString &directoryName,
                                               OFString &sopClassUID,
                                               OFString &sopInstanceUID,
                                               DcmDataset *dataset = NULL);

    // --- public constants ---

    /// default value for the name of the subdirectory that might be used for the
    /// "normal" case
    static const char *DEF_StandardSubdirectory;
    /// default value for the name of the subdirectory that might be used for the
    /// "exceptional" case
    static const char *DEF_UndefinedSubdirectory;
    /// default value for the filename extension appended to the generated filenames
    static const char *DEF_FilenameExtension;


private:

    /// name of the output directory that is used to store the received datasets
    OFString OutputDirectory;
    /// name of the subdirectory that might be used for the "normal" case, i.e.\ if the
    /// name of the subdirectory could be generated according to the current mode
    OFString StandardSubdirectory;
    /// name of the subdirectory that might be used for the "exceptional" case, i.e.\ if
    /// the name of the subdirectory could not be generated according to the current mode
    OFString UndefinedSubdirectory;
    /// filename extension appended to the generated filenames
    OFString FilenameExtension;
    /// mode that is used to generate subdirectories to store the received datasets
    E_DirectoryGenerationMode DirectoryGeneration;
    /// mode that is used to generate filenames for the received datasets
    E_FilenameGenerationMode FilenameGeneration;
    /// unique pseudo-random filename creator, which also checks for existing files
    OFFilenameCreator FilenameCreator;
    /// mode specifying how to store the received datasets (also allows for skipping the storage)
    E_DatasetStorageMode DatasetStorage;

    // private undefined copy constructor
    DcmStorageSCP(const DcmStorageSCP &);

    // private undefined assignment operator
    DcmStorageSCP &operator=(const DcmStorageSCP &);
};

#endif // DSTORSCP_H
