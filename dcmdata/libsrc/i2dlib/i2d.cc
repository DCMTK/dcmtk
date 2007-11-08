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
 *  Purpose: Implements utility for converting standard image formats to DICOM
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


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/i2dlib/i2d.h"
#include "dcmtk/dcmdata/dcpxitem.h"


Image2Dcm::Image2Dcm() : m_overrideKeys(NULL), m_templateFile(""),
  m_readStudyLevel(OFFalse), m_readSeriesLevel(OFFalse), m_studySeriesFile(),
  m_incInstNoFromFile(OFFalse), m_disableAttribChecks(OFFalse),
  m_debug(OFFalse), m_verbose(OFFalse)
{

}


OFCondition Image2Dcm::convert(I2DImgSource *inputPlug,
                               I2DOutputPlug *outPlug,
                               DcmDataset*& resultDset,
                               E_TransferSyntax& proposedTS)

{
  if (!inputPlug || !outPlug)
    return EC_IllegalParameter;

  OFCondition cond;
  if (m_verbose || m_debug)
    COUT << "Image2Dcm: Starting conversion of file: " << inputPlug->getImageFile()  << OFendl;

  // If specified, copy DICOM template file to export file
  if (m_templateFile.length() != 0)
  {
    DcmFileFormat dcmff;
    cond = dcmff.loadFile(m_templateFile.c_str());
    if (cond.bad())
      return cond;
    // remove problematic attributes from dataset
    dcmff.getDataset()->findAndDeleteElement(DCM_PixelData);
    dcmff.getDataset()->findAndDeleteElement(DCM_SOPClassUID);
    // copy from input file
    resultDset = new DcmDataset(*(dcmff.getDataset()));
  }
  else // otherwise, start with an empty DICOM file
    resultDset = new DcmDataset();
  if (!resultDset)
    return EC_MemoryExhausted;

  // Read patient and study or series information if desired and write to export file
  if (m_readStudyLevel || m_readSeriesLevel)
  {
    cond = applyStudyOrSeriesFromFile(resultDset);
    if (cond.bad())
    {
      delete resultDset; resultDset = NULL;
      return cond;
    }
  }

  // Increment instance number
  if (m_incInstNoFromFile)
  {
    cond = incrementInstanceNumber(resultDset);
    if (cond.bad())
    {
      delete resultDset; resultDset = NULL;
      return cond;
    }
  }

  // Generate and insert UIDs as necessary
  generateUIDs(resultDset);

  // Read and insert pixel data
  cond = readAndInsertPixelData(inputPlug, resultDset, proposedTS);
  if (cond.bad())
  {
    delete resultDset; resultDset = NULL;
    return cond;
  }

  // Insert SOP Class specific attributes (and values)
  cond = outPlug->convert(*resultDset);
  if (cond.bad())
  {
    delete resultDset; resultDset = NULL;
    return cond;
  }

  // At last, apply override keys on dataset
  applyOverrideKeys(resultDset);

  // Do some very basic attribute checking (e. g. existence (type 2) and values (type 1))
  if (!m_disableAttribChecks)
  {
    cond = isValid(*resultDset);
    if (cond.bad())
    {
      delete resultDset; resultDset = NULL;
      return cond;
    }
    cond = outPlug->isValid(*resultDset);
    if (cond.bad())
    {
      delete resultDset; resultDset = NULL;
      return cond;
    }
  }

  return cond;
}

OFCondition Image2Dcm::applyStudyOrSeriesFromFile(DcmDataset *targetDset)
{
  if (m_debug)
    COUT << "Image2Dcm: Applying study and/or series information from file" << OFendl;
  if ( (!m_readSeriesLevel && !m_readStudyLevel) || (m_studySeriesFile.length() == 0) )
    return EC_IllegalCall;
  DcmFileFormat dcmff;
  OFString errMsg;
  OFCondition cond;

  // Open DICOM file to read patient/study/series information from
  cond = dcmff.loadFile(m_studySeriesFile.c_str());
  if (cond.bad())
  {
    errMsg = "Error: Unable to open study / series file "; errMsg += m_studySeriesFile; errMsg+=": "; errMsg += cond.text();
    return makeOFCondition(OFM_dcmdata, 18, OF_error, errMsg.c_str());
  }

  DcmDataset *srcDset = NULL;
  srcDset = dcmff.getDataset();
  if (srcDset == NULL)
    return EC_IllegalCall;

  // Patient level attributes (type 2 - if value cannot be read, insert empty value
  OFString value;
  srcDset->findAndGetOFString(DCM_PatientsName, value);
  cond = targetDset->putAndInsertOFStringArray(DCM_PatientsName, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable write Patients Name to file");
  value.clear();

  srcDset->findAndGetOFString(DCM_PatientID, value);
  cond = targetDset->putAndInsertOFStringArray(DCM_PatientID, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable write Patient ID to file");
  value.clear();

  srcDset->findAndGetOFString(DCM_PatientsSex, value);
  cond = targetDset->putAndInsertOFStringArray(DCM_PatientsSex, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable write Patient's Sex to file");
  value.clear();

  srcDset->findAndGetOFString(DCM_PatientsBirthDate, value);
  cond = targetDset->putAndInsertOFStringArray(DCM_PatientsBirthDate, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable write Patient's Birth Date to file");
  value.clear();

  // Study level attributes (type 2 except Study Instance UID)

  cond = srcDset->findAndGetOFString(DCM_StudyInstanceUID, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to read Study Instance UID (type 1) from file");
  cond = targetDset->putAndInsertOFStringArray(DCM_StudyInstanceUID, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable write Study Instance UID to file");
  value.clear();

  srcDset->findAndGetOFString(DCM_StudyDate, value);
  cond = targetDset->putAndInsertOFStringArray(DCM_StudyDate, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable write Study Date to file");
  value.clear();

  srcDset->findAndGetOFString(DCM_StudyTime, value);
  cond = targetDset->putAndInsertOFStringArray(DCM_StudyTime, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable write Study Time to file");
  value.clear();

  srcDset->findAndGetOFString(DCM_ReferringPhysiciansName, value);
  cond = targetDset->putAndInsertOFStringArray(DCM_ReferringPhysiciansName, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable write Referring Physician's Name to file");
  value.clear();

  srcDset->findAndGetOFString(DCM_StudyID, value);
  cond = targetDset->putAndInsertOFStringArray(DCM_StudyID, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable write Study ID to file");
  value.clear();

  srcDset->findAndGetOFString(DCM_AccessionNumber, value);
  cond = targetDset->putAndInsertOFStringArray(DCM_AccessionNumber, value);
  if (cond.bad())
    return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable write Accession Number to file");
  value.clear();

  // Series Level attributes (type 2 except Series Instance UID which is type 1)
  if (m_readSeriesLevel)
  {
    // General Series Module attributes
    cond = srcDset->findAndGetOFString(DCM_SeriesInstanceUID, value);
    if (cond.bad())
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to read Series Instance UID (type 1) from file");
    cond = targetDset->putAndInsertOFStringArray(DCM_SeriesInstanceUID, value);
    if (cond.bad())
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable write Series Instance UID to file");
    value.clear();

    srcDset->findAndGetOFString(DCM_SeriesNumber, value);
    cond = targetDset->putAndInsertOFStringArray(DCM_SeriesNumber, value);
    if (cond.bad())
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable write Series Number to file");
    value.clear();

    // General Equipment Module attributes
    srcDset->findAndGetOFString(DCM_Manufacturer, value);
    cond = targetDset->putAndInsertOFStringArray(DCM_Manufacturer, value);
    if (cond.bad())
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable write Manufacturer to file");
    value.clear();
  }

  return EC_Normal;
}


OFCondition Image2Dcm::incrementInstanceNumber(DcmDataset *targetDset)
{
  // Read and increment Instance Number if desired
  if (m_incInstNoFromFile)
  {
    if (m_debug)
      COUT << "Image2Dcm: Trying to read and increment instance number" << OFendl;
    Sint32 instanceNumber;
    if ( targetDset->findAndGetSint32(DCM_InstanceNumber, instanceNumber).good() )
    {
      instanceNumber++;
      char buf[100];
      sprintf(buf, "%ld", OFstatic_cast(long, instanceNumber));
      OFCondition cond = targetDset->putAndInsertOFStringArray(DCM_InstanceNumber, buf);
      if (cond.bad())
        return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable write Instance Number to dataset");
    }
    else
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable to read Instance Number from dataset");
  }
  return EC_Normal;
}


OFCondition Image2Dcm::generateUIDs(DcmDataset *dset)
{
  OFString value;
  OFCondition cond;

  if (m_debug)
    COUT << "Image2Dcm: Generate and insert new UIDs if necessary" << OFendl;
  // Generate and write Series Instance UID if not already present
  if (!m_readSeriesLevel)
  {
    cond = dset->findAndGetOFString(DCM_SeriesInstanceUID, value);
    if (cond.bad() || (value.length() == 0))
    {
      char newUID[100];
      dcmGenerateUniqueIdentifier(newUID, SITE_SERIES_UID_ROOT);
      cond = dset->putAndInsertOFStringArray(DCM_SeriesInstanceUID, newUID);
      if (cond.bad())
        return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable write Series Instance UID to file");
    }
    value.clear();
  }

  // Generate and write Study Instance UID if not already present
  if (!m_readStudyLevel)
  {
    cond = dset->findAndGetOFString(DCM_StudyInstanceUID, value);
    if (cond.bad() || (value.length() == 0))
    {
      char newUID[100];
      dcmGenerateUniqueIdentifier(newUID, SITE_STUDY_UID_ROOT);
      cond = dset->putAndInsertOFStringArray(DCM_StudyInstanceUID, newUID);
      if (cond.bad())
        return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable write Study Instance UID to file");
    }
    value.clear();
  }

  // Generate SOP Instance UID if not already present
  cond = dset->findAndGetOFString(DCM_SOPInstanceUID, value);
  if (cond.bad() || (value.length() == 0))
  {
    char newUID[100];
    dcmGenerateUniqueIdentifier(newUID, SITE_INSTANCE_UID_ROOT);
    cond = dset->putAndInsertOFStringArray(DCM_SOPInstanceUID, newUID);
    if (cond.bad())
      return makeOFCondition(OFM_dcmdata, 18, OF_error, "Unable write SOP Instance UID to file");
  }

  return EC_Normal;
}


OFCondition Image2Dcm::readAndInsertPixelData(I2DImgSource* imgSource,
                                              DcmDataset* dset,
                                              E_TransferSyntax& outputTS)
{
  imgSource->setDebugMode(m_debug);
  imgSource->setVerboseMode(m_debug);

  Uint16 samplesPerPixel, rows, cols, bitsAlloc, bitsStored, highBit, pixelRepr, planConf, pixAspectH, pixAspectV;
  OFString photoMetrInt;
  outputTS = EXS_Unknown;
  char* pixData = NULL;
  unsigned long length;

  OFCondition cond = imgSource->readPixelData(rows, cols,
    samplesPerPixel, photoMetrInt, bitsAlloc, bitsStored, highBit, pixelRepr,
    planConf, pixAspectH, pixAspectV, pixData, length, outputTS);
  if (cond.bad())
    return cond;

  DcmPixelSequence *pixelSequence = NULL;

  if (m_debug)
    COUT << "Image2Dcm: Store imported pixel data to DICOM file" << OFendl;
  // create initial pixel sequence
  pixelSequence = new DcmPixelSequence(DcmTag(DCM_PixelData, EVR_OB));
  if (pixelSequence == NULL)
    return EC_MemoryExhausted;
  // insert empty offset table into sequence
  DcmPixelItem *offsetTable = new DcmPixelItem(DcmTag(DCM_Item, EVR_OB));
  if (offsetTable == NULL)
  {
    delete pixelSequence; pixelSequence = NULL;
    return EC_MemoryExhausted;
  }
  cond = pixelSequence->insert(offsetTable);
  if (cond.bad())
  {
    delete offsetTable; offsetTable = NULL;
    delete pixelSequence; pixelSequence = NULL;
    return cond;
  }

  // insert frame into pixel sequence
  DcmOffsetList dummyList;
  cond = pixelSequence->storeCompressedFrame(dummyList, (Uint8*)pixData, length, 0);
  // storeCompressedFrame(..) does a deep copy, so the pixdata memory can be freed now
  delete[] pixData;
  if (cond.bad())
  {
    delete pixelSequence;
    return cond;
  }
  cond = dset->insert(pixelSequence);
  if (cond.bad())
    delete pixelSequence;

  if (m_debug)
    COUT << "Image2Dcm: Inserting Image Pixel module information" << OFendl;

  cond = dset->putAndInsertUint16(DCM_SamplesPerPixel, samplesPerPixel);
  if (cond.bad())
    return cond;

  cond = dset->putAndInsertOFStringArray(DCM_PhotometricInterpretation, photoMetrInt);
  if (cond.bad())
    return cond;

  // Should only be written if Samples per Pixel > 1
  if (samplesPerPixel > 1)
  {
    cond = dset->putAndInsertUint16(DCM_PlanarConfiguration, planConf);
    if (cond.bad())
      return cond;
  }

  cond = dset->putAndInsertUint16(DCM_Rows, rows);
  if (cond.bad())
    return cond;

  cond = dset->putAndInsertUint16(DCM_Columns, cols);
  if (cond.bad())
    return cond;

  cond = dset->putAndInsertUint16(DCM_BitsAllocated, bitsAlloc);
  if (cond.bad())
    return cond;

  cond = dset->putAndInsertUint16(DCM_BitsStored, bitsStored);
  if (cond.bad())
    return cond;

  cond = dset->putAndInsertUint16(DCM_HighBit, highBit);
  if (cond.bad())
    return cond;

  return dset->putAndInsertUint16(DCM_PixelRepresentation, pixelRepr);
}


OFCondition Image2Dcm::isValid(DcmDataset& dset) const
{
  if (m_debug)
    COUT << "Image2Dcm: Checking validity of DICOM output dataset" << OFendl;
  OFString dummy, err; OFCondition cond;
  // Genera Patient module attributes
  if (!dset.tagExists(DCM_PatientsName))
    err += "Error: Patient ID (type 2) not present in dataset\n";

  if (!dset.tagExists(DCM_PatientsSex))
    err += "Error: Patient's Sex (type 2) not present in dataset\n";

  if (!dset.tagExists(DCM_PatientsBirthDate))
    err += "Error: Patient's Birth Date (type 2) not present in dataset\n";

  // General Study module attributes
  cond = dset.findAndGetOFString(DCM_StudyInstanceUID, dummy);
  if (cond.bad() || (dummy.length() == 0))
  {
    err += "Error: Study Instance UID (type 1) not present or having empty value in dataset\n";
    dummy.clear();
  }

  if (!dset.tagExists(DCM_StudyDate))
    err += "Error: Study Date (type 2) not present in dataset\n";

  if (!dset.tagExists(DCM_StudyTime))
    err += "Error: Study Time (type 2) not present in dataset\n";

  if (!dset.tagExists(DCM_ReferringPhysiciansName))
    err += "Error: Referring Physcian's Name (type 2) not present in dataset\n";

  if (!dset.tagExists(DCM_StudyID))
    err += "Error: Study ID (type 2) not present in dataset\n";

  if (!dset.tagExists(DCM_AccessionNumber))
    err += "Error: Accession Number (type 2) not present in dataset\n";

  // General Series module attributes
  cond = dset.findAndGetOFString(DCM_SeriesInstanceUID, dummy);
  if (cond.bad() || (dummy.length() == 0))
  {
    err += "Error: Series Instance UID (type 1) not present or having empty value in dataset\n";
    dummy.clear();
  }
  if (!dset.tagExists(DCM_SeriesNumber))

    err += "Error: Series Number (type 2) not present in dataset\n";

  // Instance level attributes
  if (!dset.tagExists(DCM_InstanceNumber))
    err += "Error: Instance Number (type 2) not present in dataset\n";

  cond = dset.findAndGetOFString(DCM_Rows, dummy);
  if (cond.bad() || (dummy.length() == 0))
  {
    err += "Error: Rows (type 1) not present in dataset or having an empty value\n";
    dummy.clear();
  }

  cond = dset.findAndGetOFString(DCM_Columns, dummy);
  if (cond.bad() || (dummy.length() == 0))
  {
    err += "Error: Columns (type 1) not present in dataset or having an empty value\n";
    dummy.clear();
  }

  cond = dset.findAndGetOFString(DCM_SamplesPerPixel, dummy);
  if (cond.bad() || (dummy.length() == 0))
  {
    err += "Error: Samples per Pixel (type 1) not present in dataset or having an empty value\n";
    dummy.clear();
  }

  cond = dset.findAndGetOFString(DCM_PhotometricInterpretation, dummy);
  if (cond.bad() || (dummy.length() == 0))
  {
    err += "Error: Photometric Interpretation (type 1) not present in dataset or having an empty value\n";
    dummy.clear();
  }

  cond = dset.findAndGetOFString(DCM_BitsAllocated, dummy);
  if (cond.bad() || (dummy.length() == 0))
  {
    err += "Error: Bits Allocated (type 1) not present in dataset or having an empty value\n";
    dummy.clear();
  }

  cond = dset.findAndGetOFString(DCM_BitsStored, dummy);
  if (cond.bad() || (dummy.length() == 0))
  {
    err += "Error: Bits Stored (type 1) not present in dataset or having an empty value\n";
    dummy.clear();
  }

  cond = dset.findAndGetOFString(DCM_HighBit, dummy);
  if (cond.bad() || (dummy.length() == 0))
  {
    err += "Error: High Bit (type 1) not present in dataset or having an empty value\n";
    dummy.clear();
  }

  cond = dset.findAndGetOFString(DCM_PixelRepresentation, dummy);
  if (cond.bad() || (dummy.length() == 0))
  {
    err += "Error: Pixel Representation (type 1) not present in dataset or having an empty value\n";
    dummy.clear();
  }

  cond = dset.findAndGetOFString(DCM_SOPInstanceUID, dummy);
  if (cond.bad() || (dummy.length() == 0))
  {
    err += "Error: SOP Instance UID (type 1) not present or having empty value in dataset\n";
    dummy.clear();
  }

  if (err.length() != 0)
    return makeOFCondition(OFM_dcmdata, 18, OF_error, err.c_str());
  else
    return EC_Normal;
}



void Image2Dcm::setSeriesFrom(const OFString& file)
{
  m_readSeriesLevel = OFTrue;
  m_studySeriesFile = file;
}


void Image2Dcm::setStudyFrom(const OFString& file)
{
  m_readStudyLevel = OFTrue;
  m_studySeriesFile = file;
}


void Image2Dcm::setValidityChecking(OFBool doChecks)
{
  m_disableAttribChecks = !doChecks;
}


void Image2Dcm::setTemplateFile(const OFString& file)
{
  m_templateFile = file;
}



void Image2Dcm::setIncrementInstanceNumber(OFBool incInstNo)
{
  m_incInstNoFromFile = incInstNo;
}


void Image2Dcm::setOverrideKeys(const DcmDataset* dset)
{
  delete m_overrideKeys;
  m_overrideKeys = new DcmDataset(*dset);
}


void Image2Dcm::applyOverrideKeys(DcmDataset *outputDset)

{
  if ((m_overrideKeys == NULL) || (outputDset == NULL)) {
    return; /* nothing to do */
  }
  if (m_debug)
    COUT << "Image2Dcm: Applying override keys" << OFendl;
  /* copy the override keys */
  DcmDataset keys(*m_overrideKeys);

  /* put the override keys into dset replacing existing tags */
  unsigned long elemCount = keys.card();
  for (unsigned long i=0; i<elemCount; i++) {
    DcmElement *elem = keys.remove((unsigned long)0);
    outputDset->insert(elem, OFTrue);
  }
}


Image2Dcm::~Image2Dcm()
{
  if (m_debug)
    COUT << "Image2Dcm: Freeing memory" << OFendl;
  delete m_overrideKeys; m_overrideKeys = NULL;
}


/*
 * CVS/RCS Log:
 * $Log: i2d.cc,v $
 * Revision 1.1  2007-11-08 15:55:17  onken
 * Initial checkin of img2dcm application and corresponding library i2dlib.
 *
 *
 */
