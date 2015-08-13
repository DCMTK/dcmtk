/*
*
*  Copyright (C) 2013, Open Connections GmbH
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
*  Purpose: Commandline-Application to get inforamtion about Enhanced MF files
*
*/

#include "dcmtk/config/osconfig.h"   // make sure OS specific configuration is included first
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmseg/segdoc.h"
#include "dcmtk/dcmseg/segment.h"
#include "dcmtk/dcmfg/fgderimg.h"
#include "dcmtk/dcmfg/fgfracon.h"
#include "dcmtk/dcmfg/fgplanor.h"
#include "dcmtk/dcmfg/fgplanpo.h"
#include "dcmtk/dcmfg/stackinterface.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

//#define SAVEFILE         "/tmp/makeseg.dcm"
//#define IMPORTHEADERFILE "/home/michael/data/projects/qiicr/testdata/patientstudyseries.dcm"

#ifdef WITH_ZLIB
#include <zlib.h>                     /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "makeseg"

static OFLogger makesegLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);
static const Uint16 ROWCOLSIZE=512;

char* importHeaderFile = NULL;
char* saveFile = NULL;

static void fillEquipmentInfo(IODGeneralEquipmentModule::EquipmentInfo& eq)
{
  eq.m_Manufacturer = "Open Connections";
  eq.m_DeviceSerialNumber = "1234";
  eq.m_ManufacturerModelName = "TEST DEVICE";
  eq.m_SoftwareVersions = "My own alpha version";
}


static void fillIdentificationInfo(ContentIdentificationMacro& ident)
{
  ident.setContentCreatorName("ONKEN^MICHAEL");
  ident.setContentDescription("TEST SEGMENTATION CONTENT");
  ident.setContentLabel("TEST LABEL");
  ident.setInstanceNumber("1234");
}


static FGDerivationImage* createDerivationImageFG(char* fileName = NULL)
{
  FGDerivationImage* fgder = new FGDerivationImage();
  fgder->clearData();
  if(!fileName){
    abort();
    OFVector<ImageSOPInstanceReferenceMacro> derivationImages;
    ImageSOPInstanceReferenceMacro ref;
    ref.addReferencedFrameNumber(0);
    ref.setReferencedSOPClassUID(UID_CTImageStorage);
    ref.setReferencedSOPInstanceUID("1.2.3.4");
    derivationImages.push_back(ref);
    ref.setReferencedSOPInstanceUID("2.3.4.5");
    derivationImages.push_back(ref);
    FGDerivationImage* fg = DcmSegmentation::createDerivationImageFG(derivationImages, "Test Derivation Description");
  } else {
   fgder->clearData();

   DerivationImageItem *derimgItem;
   OFCondition result = fgder->addDerivationImageItem(CodeSequenceMacro("113076","DCM","Segmentation"),"",derimgItem);
   if (result.bad())
   {
     CERR << "Cannot add Derivation Item" << OFendl;
     exit(1);
   }
   OFVector<OFString> siVector;
   siVector.push_back(fileName);
   OFVector<SourceImageItem*> srcimgItems;
   result = derimgItem->addSourceImageItems(siVector,
                                            CodeSequenceMacro("121322","DCM","Source image for image processing operation"),
                                            srcimgItems);
   if (result.bad())
   {
     CERR << "Cannot add Source Image" << OFendl;
     exit(1);
   }
  }
  return fgder;
}


static FGFrameContent* createFrameContentFG()
{
  FGFrameContent *fg = new FGFrameContent();
  if (fg)
  {
    fg->setFrameLabel("My frame label");
    fg->setFrameComments("Artificial frame comment");
  }
  else
  {
    exit(1);
  }
  return fg;
}


static void varyPerFrameGroup(OFVector<FGBase*>& groups)
{
  static Uint32 count = 1;
  FGPlanePosPatient* fgPos = OFstatic_cast(FGPlanePosPatient*, groups[0]);
  OFStringStream oss;
  oss << count;
  OFSTRINGSTREAM_GETOFSTRING(oss, ipp)
  fgPos->setImagePositionPatient(ipp, "1", "1");
  FGFrameContent* fracon = OFstatic_cast(FGFrameContent*, groups[1]);
  if (fracon)
    fracon->setDimensionIndexValues(count, 0);
  else
  {
    CERR << "Cannot get frame content group (wrong position)" << OFendl;
  }
  FGDerivationImage* der = OFstatic_cast(FGDerivationImage*, groups[2]);
  if (der)
  {
    der->clearData();
    DerivationImageItem *derimgItem;
    der->addDerivationImageItem(CodeSequenceMacro("113076","DCM","Segmentation"),"",derimgItem);

    OFVector<OFString> siVector;
    siVector.push_back(OFString(importHeaderFile));
    OFVector<SourceImageItem*> srcimgItems;
    derimgItem->addSourceImageItems(siVector,
              CodeSequenceMacro("121322","DCM","Source image for image processing operation"),
              srcimgItems);
  }
  else
  {
    CERR << "Cannot get derivation image FG (wrong position)" << OFendl;
  }
  count++;
}


static void createPerFrameGroups(OFVector<FGBase*>& perFrameGroups, char* importHeaderFile)
{
  // Create frame content macro
  FGPlanePosPatient* fgPos = FGPlanePosPatient::createMinimal("1", "1", "1");
  FGFrameContent* frameContent = createFrameContentFG();
  FGDerivationImage* fgDerivation = createDerivationImageFG(importHeaderFile);
  perFrameGroups.push_back(fgPos);
  perFrameGroups.push_back(frameContent);
  perFrameGroups.push_back(fgDerivation);
}


static void createAndAddSegment(DcmSegmentation* segdoc)
{
  OFCondition result;
  DcmSegment* segment = NULL;
  result = DcmSegment::create(segment,
                              "SEGMENT LABEL 1",
                              CodeSequenceMacro("Test Category", "99DCMTK", "Test Category Type Meaning", "Test Category Version"),
                              CodeSequenceMacro("Test Property", "99DCMTK", "Test Property Type Meaning", "Test Property Version"),
                              DcmSegTypes::SAT_AUTOMATIC,
                              "My own Algorithm");

  /* Add first segment to document */
  Uint16 segmentNumber = 0;
  if ( result.good() )
  {
    result = segdoc->addSegment(segment, segmentNumber /* returns logical segment number */);
  }

  if (result.bad())
  {
    DCMSEG_ERROR("Could not add segment to segmentation document: " << result.text());
    exit(1);
  }
}


static void addStack(DcmSegmentation& segdoc)
{
  FGStackInterface stacks;
  FGStack* stack = new FGStack("Test Stack #1");
  if (stack == NULL)
  {
    CERR << "Cannot create stack, memory exhausted" << OFendl;
    exit(1);
  }
  stack->addFrame(0, 10);
  stack->addFrame(1, 9);
  stack->addFrame(2, 8);
  stack->addFrame(3, 7);
  stack->addFrame(4, 6);
  stack->addFrame(5, 5);
  stack->addFrame(6, 4);
  stack->addFrame(7, 3);
  stack->addFrame(8, 2);
  stack->addFrame(9, 1);
  if (!stacks.addStack(stack))
  {
    CERR << "Could not add stack " << OFendl;
    exit(1);
  }
  if (stacks.write(segdoc.getFunctionalGroups()).bad())
  {
    CERR << "Could not write stacks to functional groups" << OFendl;
    exit(1);
  }
}


static void addDimension(DcmSegmentation& seg)
{
  OFCondition result = seg.getDimensions().addDimensionIndex(DCM_InStackPositionNumber, "1.3.2.4.3.5.4.6" /* dimension organization uid */, DCM_FrameContentSequence, "Stack-based Dimension");
  if (result.bad())
  {
    CERR << "Cannot add dimension: " << result.text() << OFendl;
    exit(1);
  }
}


int main(int argc, char *argv[])
{
  OFLog::configure(OFLogger::DEBUG_LOG_LEVEL);

  if(argc<3){
    std::cerr << "Usage: " << argv[0] << " importHeaderFile saveFile" << std::endl;
    return -1;
  }

  importHeaderFile = argv[1];
  saveFile = argv[2];

  // Construct Equipment and Content Identification information
  IODGeneralEquipmentModule::EquipmentInfo eq;
  fillEquipmentInfo(eq);
  ContentIdentificationMacro ident;
  fillIdentificationInfo(ident);

  // Create per frame functional groups
  OFVector<FGBase*> perFrameGroups;
  createPerFrameGroups(perFrameGroups, importHeaderFile);

  // Create new segementation document
  DcmSegmentation *segdoc = NULL;
  OFCondition result = DcmSegmentation::createBinarySegmentation(
    segdoc,   // resulting segmentation
    512,      // rows
    512,      // columns
    eq,       // equipment
    ident);   // content identification

  // Make sure the pointer is freed in any case
  OFunique_ptr<DcmSegmentation> mySegdoc(segdoc);
  if ( result.bad() )
  {
    CERR << "Could not create Segmentation document: " << result.text() << OFendl;
  }

  // Import patient and study from existing file
  if (result.good())
  {
    result = segdoc->importPatientStudyFoR(importHeaderFile, OFTrue, OFTrue, OFFalse, OFTrue);
    if ( result.bad() )
    {
      CERR << "Error: Could not import patient, study, series and/or frame of reference" << OFendl;
    }
  }

  // Add segment and add shared functional groups
  if (result.good())
  {
    createAndAddSegment(segdoc);

    // Create shared functional groups
    FGPlaneOrientationPatient* fgOrient = FGPlaneOrientationPatient::createMinimal("1.0", "1.0", "1.0", "1.0", "1.0", "1.0");
    result = segdoc->addForAllFrames(*fgOrient);
    if (result.bad())
    {
      CERR << "Error: Could not insert Plane Orientation Functional Group" << OFendl;
    }
    delete fgOrient;
  }

  // Add 10 frames
  if (result.good())
  {
    for (size_t count = 0; count < 10; count ++)
    {
      varyPerFrameGroup(perFrameGroups);
      Uint8* frameData = new Uint8[ROWCOLSIZE*ROWCOLSIZE];
      memset(frameData, 0, ROWCOLSIZE*ROWCOLSIZE);
      result = segdoc->addFrame(frameData, 1, perFrameGroups);
      if ( result.bad() )
      {
        CERR << "Cannot add frame: " << result.text() << OFendl;
       delete frameData;
        exit(1);
      }
      delete frameData;
    }
  }

  // Add stack, and add Dimension based on stack
  if (result.good())
  {
    addStack(*segdoc);
    addDimension(*segdoc);
  }

  // Store to disk
  if (result.good())
  {
    COUT << "Successfully created segmentation document" << OFendl;
    result = segdoc->saveFile(saveFile);
    if (result.bad())
    {
      CERR << "Could not save segmentation document to file: " << result.text() << OFendl;
    }
  }

  // Clean up and exit
  DcmIODUtil::freeContainer(perFrameGroups);

  if (result.bad())
    return 1;
  else
    return 0;
}
