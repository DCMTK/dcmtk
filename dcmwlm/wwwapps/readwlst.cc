/*
 *
 *  Copyright (C) 1996-2011, OFFIS e.V.
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
 *  Module:  dcmwlm
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose:
 *   Program to read a DICOM worklist file and pass (nearly) all tags
 *   to a WWW CGI perl script (as hexadecimal encoded ASCII via stdout).
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmdata/dcuid.h"    /* for dcmtk version name */

static char rcsid[] = "$dcmtk: readwlst v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

static void printHexString(char *c)
{
  if (c) while (*c) printf("%02X",(unsigned char)(*c++));
}

static void printStringElement(DcmObject& obj, DcmTagKey searchtag)
{
  DcmStack stack;
  char *c = NULL;
  if (obj.search(searchtag, stack, ESM_fromHere, OFFalse ) == EC_Normal )
  {
    DcmObject *searchedObj = stack.top();
//    searchedObj->verify(OFTrue);
/*
** Use the simpler (virtual) method of DcmElement to get
** the string pointer.
** Modified while adding support for the VR's UT and VS.
*/
#ifdef ELABORATE_BUT_PROBABLY_UNNECESSARY_CODE
    if (searchedObj->getLength() > 0) switch(searchedObj->ident())
    {
      case EVR_AE:
      case EVR_AS:
      case EVR_CS:
      case EVR_DA:
      case EVR_DS:
      case EVR_DT:
      case EVR_IS:
      case EVR_TM:
      case EVR_UI:
        ((DcmByteString*)searchedObj)->getString(c);
        break;
      case EVR_LO:
      case EVR_LT:
      case EVR_PN:
      case EVR_SH:
      case EVR_ST:
        ((DcmCharString*)searchedObj)->getString(c);
        break;
      default:
        break;
    }
#else
    if ((searchedObj->getLength() > 0) && searchedObj->isaString()) {
	DcmElement* elem = (DcmElement*)searchedObj;
	elem->getString(c);
    }
#endif
  }
  printHexString(c);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
      fprintf(stderr, "%s\n\nusage: readwlst <dicomfile>\n", rcsid);
      return 0;
    }

    /* make sure data dictionary is loaded */
    if (!dcmDataDict.isDictionaryLoaded())
    {
      fprintf(stderr, "Warning: no data dictionary loaded, check environment variable: %s\n",
        DCM_DICT_ENVIRONMENT_VARIABLE);
      return 0;
    }

    DcmFileFormat dfile;
    OFCondition cond = dfile.loadFile(argv[1]);
    if (cond.bad())
    {
	fprintf(stderr, "readwlst: error: %s: reading file: %s\n", cond.text(), argv[1]);
        return 0;
    }

    DcmDataset *dataset = dfile.getDataset();
    if (dataset)
    {
      printf("\\"); printStringElement(*dataset, DCM_RequestedProcedureID);
      printf("\\"); printStringElement(*dataset, DCM_RequestedProcedureDescription);
      printf("\\"); printStringElement(*dataset, DCM_StudyInstanceUID);
      printf("\\"); printStringElement(*dataset, DCM_RequestedProcedurePriority);
      printf("\\"); printStringElement(*dataset, DCM_AccessionNumber);
      printf("\\"); printStringElement(*dataset, DCM_RequestingPhysician);
      printf("\\"); printStringElement(*dataset, DCM_PatientName);
      printf("\\"); printStringElement(*dataset, DCM_PatientID);
      printf("\\"); printStringElement(*dataset, DCM_PatientBirthDate);
      printf("\\"); printStringElement(*dataset, DCM_PatientSex);
      printf("\\"); printStringElement(*dataset, DCM_MedicalAlerts);
      printf("\\"); printStringElement(*dataset, DCM_Allergies);
      DcmStack stack;
      if (dataset->search(DCM_ScheduledProcedureStepSequence, stack, ESM_fromHere, OFFalse ) == EC_Normal )
      {
        DcmSequenceOfItems *seq = (DcmSequenceOfItems *)stack.top();
        if (seq->card() > 0)
        {
          DcmItem *item=seq->getItem(0);
          printf("\\"); if (item) printStringElement(*item, DCM_ScheduledStationAETitle);
          printf("\\"); if (item) printStringElement(*item, DCM_ScheduledProcedureStepStartDate);
          printf("\\"); if (item) printStringElement(*item, DCM_ScheduledProcedureStepStartTime);
          printf("\\"); if (item) printStringElement(*item, DCM_Modality);
          printf("\\"); if (item) printStringElement(*item, DCM_ScheduledPerformingPhysicianName);
          printf("\\"); if (item) printStringElement(*item, DCM_ScheduledProcedureStepDescription);
          printf("\\"); if (item) printStringElement(*item, DCM_ScheduledStationName);
          printf("\\"); if (item) printStringElement(*item, DCM_ScheduledProcedureStepLocation);
          printf("\\"); if (item) printStringElement(*item, DCM_PreMedication);
          printf("\\"); if (item) printStringElement(*item, DCM_ScheduledProcedureStepID);
          printf("\\"); if (item) printStringElement(*item, DCM_RequestedContrastAgent);
          printf("\\"); if (item) printStringElement(*item, DCM_CommentsOnTheScheduledProcedureStep);
        } else {
          printf("\\\\\\\\\\\\\\\\\\\\\\\\");
        }
      } else {
        printf("\\\\\\\\\\\\\\\\\\\\\\\\");
      }
      printf("\\");
    } else {
      printf("\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\");
    }
    return 0;
}
