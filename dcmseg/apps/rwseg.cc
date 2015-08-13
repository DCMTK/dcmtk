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
#include "dcmtk/dcmdata/dcrledrg.h"  /* for DcmRLEDecoderRegistration */
#include "dcmtk/dcmseg/segdoc.h"
#include "dcmtk/dcmseg/segment.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"


#ifdef WITH_ZLIB
#include <zlib.h>                     /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "readseg"

int main(int argc, char *argv[])
{
  if (argc < 3)
  {
    CERR << "Usage: rwseg <inputfile> <outputfile>" << OFendl;
    return 1;
  }


  // Register RLE codec
  DcmRLEDecoderRegistration::registerCodecs();

  OFLog::configure(OFLogger::DEBUG_LOG_LEVEL);
  DcmSegmentation* doc  = NULL;
  OFCondition result = DcmSegmentation::loadFile(argv[1], doc);
  if (result.bad())
  {
    CERR << "Cannot load segmentation file: " << result.text() << OFendl;
    delete doc;
    return 1;
  }
  DcmFileFormat wfile;;

  result = doc->writeDataset(*wfile.getDataset());
  {
    if (result.good())
    {
      result = wfile.saveFile(argv[2], EXS_LittleEndianExplicit);
    }
    else
    {
      CERR << "Could not write segmentation file: " << result.text() << OFendl;
      delete doc;
      return 1;
    }
  }
  delete doc;

  // Deregister RLE codec
  DcmRLEDecoderRegistration::cleanup();
  return 0;
}


