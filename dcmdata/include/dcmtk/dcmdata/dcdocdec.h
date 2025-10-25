/*
 *
 *  Copyright (C) 2007-2025, OFFIS e.V.
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
 *  Author:  Marco Eichelberg, Tingyan Xu
 *
 *  Purpose: Helper class for extracting encapsulated file from DICOM encapsulated storage object
 *
 */

#ifndef DCDOCDEC_H
#define DCDOCDEC_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcdefine.h"   /* for DCMTK_DCMDATA_EXPORT */
#include "dcmtk/dcmdata/dcfilefo.h"   /* for DcmFileFormat */

/** Helper class for extracting the encapsulated document from a
 *  DICOM encapsulated storage object to file
 */
class DCMTK_DCMDATA_EXPORT DcmDocumentDecapsulator
{
public:
  /// Constructor
  DcmDocumentDecapsulator();

  /// Destructor
  virtual ~DcmDocumentDecapsulator();

  /** set the read mode for reading a DICOM file (default: ERM_autoDetect)
   *  @param mode new read mode
   */
  void setReadMode(E_FileReadMode mode)
  {
      readMode_ = mode;
  }

  /** set the input transfer syntax for reading a DICOM file (default: EXS_Unknown)
   *  @param xfer new input transfer syntax
   */
  void setInputXferSyntax(E_TransferSyntax xfer)
  {
      inputXfer_ = xfer;
  }

  /** set a command line string to be executed after the encapsulated document has been written to file
   *  @param execString command line string
   */
  void setExecString(const char *execString)
  {
      execString_ = execString;
  }

  /** set the filename (path) for the DICOM file to be read
   *  @param fname input file name
   */
  void setInputFile(const char *fname)
  {
      inputFname_ = fname;
  }

  /** set the filename (path) for the output file to be written
   *  @param fname output file name
   */
  void setOutputFile(const char *fname)
  {
      outputFname_ = fname;
  }

  /** load the encapsulated DICOM file into memory.
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition loadDICOMFile();

  /** extract the encapsulated document, remove a pad byte if necessary,
   *  and write the document to the output file
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition writeEncapsulatedContentToFile();

  /** execute the pre-defined command line, replacing the placeholder
   *  "#f" with the actual output filename.
   *  @return EC_Normal upon success, an error code otherwise
   */
  OFCondition executeCommand();

private:

  /// input file read mode
  E_FileReadMode readMode_;

  /// input transfer syntax
  E_TransferSyntax inputXfer_;

  /// command line string to be executed
  const char *execString_;

  /// input filename
  const char *inputFname_;

  /// output filename
  const char *outputFname_;

  /// DICOM file
  DcmFileFormat dicomFile_;

};

#endif // DCDOCDEC_H
