/*
 *
 *  Copyright (C) 2000-2010, OFFIS e.V.
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
 *  Module: dcmpstat
 *
 *  Author: Andrew Hewett, Marco Eichelberg
 *
 *  Purpose:
 *    VR and IOD checker for Presentation States
 *
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:13:45 $
 *  CVS/RCS Revision: $Revision: 1.31 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"     /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef HAVE_WINDOWS_H
#include <windows.h>                   /* this includes either winsock.h or winsock2.h */
#endif

#ifdef HAVE_GUSI_H
#include <GUSI.h>                      /* needed for Macintosh */
#include <SIOUX.h>
#endif

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstring.h"      /* for class OFString */
#include "dcmtk/ofstd/ofconapp.h"      /* for OFConsoleApplication */
#include "dcmtk/dcmdata/dctk.h"        /* for class DcmDataset */
#include "dcmtk/dcmnet/dul.h"
#include "dcmtk/dcmpstat/dcmpstat.h"   /* for DcmPresentationState */

#ifdef WITH_ZLIB
#include <zlib.h>                      /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmpschk"

static OFLogger dcmpschkLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

/* command line options */
static const char *opt_filename         = NULL;

// ********************************************

enum ErrorMode
{
  EM_ok,
  EM_informational,
  EM_warning,
  EM_error
};

#define MSG_invalidVR      "Error: Unsupported Value Representation."
#define MSG_lengthtoolarge "Error: Value Length too large."
#define MSG_lengthtoosmall "Error: Value Length too small."
#define MSG_unexpectedVR   "Error: Unexpected Value Representation."
#define MSG_vmtoolarge     "Error: Value Multiplicity too large."
#define MSG_vmtoosmall     "Error: Value Multiplicity too small."
#define MSGe_mhxferError   "Error: Meta-header encoded using invalid transfer syntax."
#define MSGe_missingAtt    "Error: Attribute is missing."
#define MSGe_wrongAtt      "Error: Attribute is incorrect."
#define MSGe_wrongDType    "Error: Attribute value does not conform to data type definition."
#define MSGi_wrongDType    "Informational: Attribute value does not conform to data type definition."
#define MSGw_wrongDType    "Warning: Attribute value uses retired form."
#define MSGw_dubiousDate   "Warning: Dubious date (year before 1850 or after 2050)."

void printVRError(
  ErrorMode mode,
  const char *elementDescription,
  const DcmDictEntry* dictRef,
  const char *format)
{
  OFOStringStream out;
  if      (mode == EM_error)         out << MSGe_wrongDType << OFendl;
  else if (mode == EM_warning)       out << MSGw_wrongDType << OFendl;
  else if (mode == EM_informational) out << MSGi_wrongDType << OFendl;
  out << "   Affected attribute: ";
  if (dictRef)
  {
     out << dictRef->getTagName();
     out << " " << dictRef->getKey();
     out << ", Type " << dictRef->getVR().getVRName() << OFendl;
  } else out << "(unnamed)" << OFendl;
  out << "   Attribute value   : ";
  if (elementDescription) out << "[" << elementDescription << "]" << OFendl;
  else out << "(empty)" << OFendl;
  if (mode == EM_error)
  {
    out << "   Expected format for each value: ";
    if (format)
    {
      out << format << OFendl;
    } else out << "(undefined)" << OFendl;
  }
  out << OFStringStream_ends;
  OFSTRINGSTREAM_GETSTR(out, tmp)
  OFLOG_ERROR(dcmpschkLogger, tmp);
  OFSTRINGSTREAM_FREESTR(tmp)
}

void printResult(
  DcmStack& stack,
  OFBool showFullData)
{
    unsigned long n = stack.card();
    if (n == 0) {
        return;
    }

    OFString tmp;

    /* print the path leading up to the top stack elem */
    for (unsigned long i=n-1; i>=1; i--)
    {
        DcmObject *dobj = stack.elem(i);
        /* do not print if a DCM_Item as this is not
         * very helpful to distinguish instances.
         */
        if (dobj != NULL && dobj->getTag().getXTag() != DCM_Item)
        {
            char buf[128];
            sprintf(buf, "(%04x,%04x).",
                    (unsigned)dobj->getGTag(),
                    (unsigned)dobj->getETag());
            tmp += buf;
        }
    }

    /* print the tag and its value */
    DcmObject *dobj = stack.top();
    OFLOG_WARN(dcmpschkLogger, tmp << DcmObject::PrintHelper(*dobj, showFullData ? 0 : DCMTypes::PF_shortenLongTagValues));
}

OFBool isaStringVR(DcmVR& vr)
{
    OFBool isaString = OFFalse;
    switch(vr.getEVR())
    {
    case EVR_AE:
    case EVR_AS:
    case EVR_CS:
    case EVR_DA:
    case EVR_DS:
    case EVR_DT:
    case EVR_IS:
    case EVR_LO:
    case EVR_LT:
    case EVR_PN:
    case EVR_SH:
    case EVR_ST:
    case EVR_TM:
    case EVR_UI:
    case EVR_UT:
        isaString = OFTrue;
        break;
    default:
        isaString = OFFalse;
        break;
    }
    return isaString;
}

const char* streamvm(const DcmDictEntry *e)
{
    static char buf[256];
    if (e->isFixedSingleVM()) {
        sprintf(buf, "%d", e->getVMMax());
    } else if (e->isVariableRangeVM()) {
        sprintf(buf, "%d-n", e->getVMMin());
    } else if (e->isFixedRangeVM()){
        sprintf(buf, "%d-%d", e->getVMMin(), e->getVMMax());
    } else {
        sprintf(buf, "?(%d-%d)?", e->getVMMin(), e->getVMMax());
    }
    return buf;
}

const char* streamLengthOfValue(DcmVR& vr)
{
    static char buf[256];
    Uint32 min = vr.getMinValueLength();
    Uint32 max = vr.getMaxValueLength();
    Uint32 undefLen = DCM_UndefinedLength;

    if (min==max) {
        sprintf(buf, "%d bytes fixed length", (int)min);
    } else if (min==0) {
        if (max==undefLen) {
            sprintf(buf, "unrestricted length");
        } else {
            sprintf(buf, "%d bytes maximum", (int)max);
        }
    } else {
        sprintf(buf, "range %d-%d bytes length", (int)min, (int)max);
    }
    return buf;
}

int splitFields(
  char* line,
  char* fields[],
  Uint32 maxFields,
  char splitChar)
{
    char* p;
    Uint32 foundFields = 0;
    int len;

    do {
        p = strchr(line, splitChar);
        if (p == NULL) {
            len = strlen(line);
        } else {
            len = p - line;
        }
        fields[foundFields] = new char[len+1];
        strncpy(fields[foundFields], line, len);
        fields[foundFields][len] = '\0';

        foundFields++;
        line = p + 1;
    } while ((foundFields < maxFields) && (p != NULL));

    return foundFields;
}

OFBool isaKnownPointer(DcmTag& t)
{
    /*
    ** The DICOMDIR code automatically converts any pointers
    ** to have VR=up even if when the data has an explicit VR encoding.
    ** If this attribute is a known pointer then it is ok for it to
    ** have the internal VR=up.
    */

    OFBool result = OFFalse;
    const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
    const DcmDictEntry *dictRef = globalDataDict.findEntry(t, NULL);

    if (dictRef && (t.getEVR() == EVR_up) && (t.getEVR() == dictRef->getEVR())) result = OFTrue;

    dcmDataDict.unlock();
    return result;
}

int checkelem(
  DcmElement *elem,
  DcmXfer& oxfer,
  DcmStack& stack,
  OFBool showFullData,
  int& dderrors)
{
    DcmVR vr(elem->getVR());
    Uint32 len = elem->getLength();
    DcmTag tag(elem->getTag());
    Uint32 vm = elem->getVM();
    const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
    const DcmDictEntry *dictRef = globalDataDict.findEntry(tag, NULL);
    int i = 0;

    /*
    ** if the data was encoded in explicit VR then check that the given VR matches
    ** the value in the data dictionary.
    */
    if (oxfer.isExplicitVR()) {
        if (!vr.isStandard() && !isaKnownPointer(tag)) {
            OFLOG_WARN(dcmpschkLogger, MSG_invalidVR << OFendl
                << "   Affected VR       : [" << vr.getVRName() << "]"
                << (dictRef ? OFString(", should be [")
                    + dictRef->getVR().getVRName() + "] according to data dictionary." : "")
                << OFendl << "   Affected attribute: ");
            printResult(stack, showFullData);
            dderrors++;
        } else if (dictRef && !vr.isEquivalent(dictRef->getVR())) {
            OFLOG_WARN(dcmpschkLogger, MSG_unexpectedVR << OFendl
                << "   Affected VR       : [" << vr.getVRName() << "], should be ["
                << dictRef->getVR().getVRName() << "] according to data dictionary." << OFendl
                << "   Affected attribute: ");
            printResult(stack, showFullData);
            dderrors++;
        }
    }

    if (len) /* type 2 attributes can be empty. */
    {

       /*
       ** Check value multiplicity
       */
       if ((dictRef)&&(vm < (Uint32)dictRef->getVMMin()))
       {
           OFLOG_WARN(dcmpschkLogger, MSG_vmtoosmall << OFendl
               << "   Affected VM       : " << vm << ", should be "
               << streamvm(dictRef) << " according to data dictionary."
               << OFendl << "   Affected attribute: ");
           printResult(stack, showFullData);
           dderrors++;
       }

       if ((dictRef)&&(vm > (Uint32)dictRef->getVMMax()))
       {
           OFLOG_WARN(dcmpschkLogger, MSG_vmtoolarge << OFendl
               << "   Affected VM       : " << vm << ", should be "
               << streamvm(dictRef) << " according to data dictionary."
               << OFendl << "   Affected attribute: ");
           printResult(stack, showFullData);
           dderrors++;
       }

       /*
       ** Check length of attribute
       */
       /* Need to split the value into its components if VM>1 */
       if (isaStringVR(vr)) {
           /* only strings have variable length components */
           char* value = NULL;
        ((DcmByteString*)elem)->getString(value);

           char **fields = new char*[vm+1];
           if (fields == NULL) {
               printResult(stack, showFullData);
               OFLOG_FATAL(dcmpschkLogger, "Internal error: out of memory (value multiplicity too large)");
           } else {
               int nfields = splitFields(value, fields, vm, '\\');
               if ((Uint32)nfields != vm) {
                   printResult(stack, showFullData);
                   OFLOG_FATAL(dcmpschkLogger, "Internal error: splitFields inconsistency ("
                       << nfields << "!=" << vm << ")");
                   exit(1);
               }
               for (i=0; (Uint32)i<vm; i++) {
                   char* s = fields[i];
                   int slen = strlen(s);
                   if ((Uint32)slen > vr.getMaxValueLength()) {
                       OFLOG_WARN(dcmpschkLogger, MSG_lengthtoolarge << OFendl
                           << "   Affected length   : " << slen << " bytes, should be "
                           << streamLengthOfValue(vr) << " for " << vr.getVRName() << ".");
                       if (vm > 1)
                         OFLOG_WARN(dcmpschkLogger, "   Affected value [" << i << "]: \"" << s << "\"");
                       OFLOG_WARN(dcmpschkLogger, "   Affected attribute: ");
                       printResult(stack, showFullData);
                    dderrors++;
                   }
                   if ((Uint32)slen < vr.getMinValueLength()) {
                       OFLOG_WARN(dcmpschkLogger, MSG_lengthtoosmall << OFendl
                           << "   Affected length   : " << slen << " bytes, should be "
                           << streamLengthOfValue(vr) << " for " << vr.getVRName() << ".");
                       if (vm > 1)
                         OFLOG_WARN(dcmpschkLogger, "   Affected value [" << i << "]: \"" << s << "\"");
                       OFLOG_WARN(dcmpschkLogger, "   Affected attribute: ");
                       printResult(stack, showFullData);
                    dderrors++;
                   }

                   delete[] fields[i];
                   fields[i] = NULL;
               }
               delete[] fields;
           }
       } else {
           Uint32 componentSize = len; /* vm is 0 if value field is too short, e.g. < 8 bytes for FD */
           if (vm>0) componentSize = len/vm;
           if (componentSize > vr.getMaxValueLength()) {
               OFLOG_WARN(dcmpschkLogger, MSG_lengthtoolarge << OFendl
                   << "   Affected length   : " << componentSize << " bytes, should be "
                   << streamLengthOfValue(vr) << " for " << vr.getVRName() << "." << OFendl
                   << "   Affected attribute: ");
               printResult(stack, showFullData);
            dderrors++;
        }
           if (componentSize < vr.getMinValueLength()) {
               OFLOG_WARN(dcmpschkLogger, MSG_lengthtoosmall << OFendl
                   << "   Affected length   : " << componentSize << " bytes, should be "
                   << streamLengthOfValue(vr) << " for " << vr.getVRName() << "." << OFendl
                   << "   Affected attribute: ");
               printResult(stack, showFullData);
            dderrors++;
           }
       }

      /*
      **  check, wether the value of the element is suitable to the data type.
      */

      /*** differenciate all value representations */
       if (isaStringVR(vr))
       {
         char* value = NULL;
         if (EC_Normal == ((DcmByteString*)elem)->getString(value) && value) switch (vr.getEVR())
         {
           case EVR_AE:
              {
                const int realVR = DcmElement::scanValue(value, "ae");
                if (realVR != 13)
                {
                   printVRError(EM_error, value, dictRef, "all but control characters");
                   dderrors++;
                }
              }
             break;
           case EVR_AS:
              {
                const int realVR = DcmElement::scanValue(value, "as");
                if (realVR != 1)
                {
                   printVRError(EM_error, value, dictRef, "[0-9]{3}[DWMY]");
                   dderrors++;
                }
              }
              break;
           case EVR_CS:
              {
                const int realVR = DcmElement::scanValue(value, "cs");
                if (realVR != 10)
                {
                   printVRError(EM_error, value, dictRef, "[0-9A-Z _]+");
                   dderrors++;
                }
             }
             break;
           case EVR_DA:
              {
                const int realVR = DcmElement::scanValue(value, "da");
                if (realVR != 2)
                {
                  switch (realVR)
                  {
                    case 3:
                      printVRError(EM_warning, value, dictRef, NULL);
                      dderrors++;
                      break;
                    case 17:
                      OFLOG_WARN(dcmpschkLogger, MSGw_dubiousDate);
                      printVRError(EM_ok, value, dictRef, NULL);
                      dderrors++;
                      break;
                    default:
                      printVRError(EM_error, value, dictRef, "[0-9]{8} with valid values for year, month and day");
                      dderrors++;
                      break;
                  }
                }
              }
              break;
           case EVR_DS:
              {
                const int realVR = DcmElement::scanValue(value, "ds");
                if (realVR != 6)
                {
                   printVRError(EM_error, value, dictRef, "([\\-\\+]?[0-9]*[\\.]?[0-9]+)|([\\-\\+]?[0-9][\\.]?[0-9]+[Ee][\\+\\-][0-9]+)");
                   dderrors++;
                }
              }
              break;
           case EVR_DT:
              {
                const int realVR = DcmElement::scanValue(value, "dt");
                if (realVR != 7)
                {
                  if (realVR == 18)
                  {
                     OFLOG_WARN(dcmpschkLogger, MSGw_dubiousDate);
                     printVRError(EM_ok, value, dictRef, NULL);
                     dderrors++;
                  } else {
                     printVRError(EM_error, value, dictRef, "[0-9]{8}[0-9]{2}([0-9]{2}([0-9]{2}(\\.[0-9]{1,6})?)?)?([\\+\\-][0-9]{4})?");
                     dderrors++;
                  }
                }
              }
              break;
           case EVR_IS:
              {
                const int realVR = DcmElement::scanValue(value, "is");
                if (realVR != 8)
                {
                   printVRError(EM_error, value, dictRef, "[\\+\\-]?[0-9]+ in the range -2^31 .. 2^31-1");
                   dderrors++;
                }
              }
              break;
           case EVR_SH:
           case EVR_LO:
              {
                const int realVR = DcmElement::scanValue(value, "lo");
                if (realVR != 12)
                {
                   printVRError(EM_error, value, dictRef, "all but '\\' and control characters");
                   dderrors++;
                }
              }
              break;
           case EVR_ST:
           case EVR_LT:
           case EVR_UT:
              {
                const int realVR = DcmElement::scanValue(value, "lt");
                if (realVR != 14)
                {
                   printVRError(EM_error, value, dictRef, "all");
                   dderrors++;
                }
              }
              break;
           case EVR_PN:
              {
                const int realVR = DcmElement::scanValue(value, "pn");
                if (realVR != 11)
                {
                   if (realVR == 15) /* OLD_PN */
                   {
                     printVRError(EM_warning, value, dictRef, NULL);
                     dderrors++;
                   } else {
                     printVRError(EM_error, value, dictRef, "{all}*([\\^]{all}*([\\^]{all}*([\\^]{all}*(\\^{all}*)?)?)?)?");
                     dderrors++;
                   }
                }
              }
              break;
           case EVR_TM:
              {
                const int realVR = DcmElement::scanValue(value, "tm");
                if (realVR != 4)
                {
                   if (realVR == 5)
                   {
                     printVRError(EM_warning, value, dictRef, NULL);
                     dderrors++;
                   } else {
                     printVRError(EM_error, value, dictRef, "[0-9]{2}([0-9]{2}([0-9]{2}(\\.[0-9]{1,6})?)?)? with valid values for hour, minute and second");
                     dderrors++;
                   }
                }
              }
              break;
           case EVR_UI:
              {
                const int realVR = DcmElement::scanValue(value, "ui");
                if (realVR != 9)
                {
                   printVRError(EM_error, value, dictRef, "([0-9]+\\.)*[0-9]+ without any leading zeroes");
                   dderrors++;
                }
              }
              break;
           default:
              break;
         }

      } //end of if (isaStringVR(vr))

    }
    dcmDataDict.unlock();
    return 0;
}

int checkitem(
  DcmItem *item,
  DcmXfer& oxfer,
  DcmStack& stack,
  OFBool showFullData,
  int& dderrors)
{

    if (item == NULL) {
        return 0;
    }
    /*
    ** Step through each attribute and check it.
    */

    unsigned long count = item->card();
    for (unsigned long i=0; i<count; i++) {
        DcmElement *elem = item->getElement(i);

        stack.push(elem);
        checkelem(elem, oxfer, stack, showFullData, dderrors);
        stack.pop();

        if (elem->ident() == EVR_SQ) {
            DcmSequenceOfItems *seq = (DcmSequenceOfItems*)elem;
            unsigned long nitems = seq->card();
            for (unsigned long j=0; j<nitems; j++) {
                /* check each item.  an item is just another dataset */
                stack.push(seq);
                checkitem(seq->getItem(j), oxfer, stack, showFullData, dderrors);
                stack.pop();
            }
        }
    }
    return 0;
}

int dcmchk(
  const char* ifname,
  E_FileReadMode readMode,
  E_TransferSyntax xfer,
  OFBool showFullData,
  OFBool loadAllDataInMemory,
  int& dderrors)
{
    DcmFileFormat *ds = new DcmFileFormat();

    OFCondition cond = ds->loadFile(ifname, xfer, EGL_noChange, DCM_MaxReadLength, readMode);
    if (! cond.good())
    {
      OFLOG_ERROR(dcmpschkLogger, cond.text() << " reading file: " << ifname);
    }

    if (loadAllDataInMemory) {
        ds->loadAllDataIntoMemory();
        if (ds->error() != EC_Normal)
        {
            OFLOG_ERROR(dcmpschkLogger, ds->error().text()
                << " reading file: " << ifname);
            return 1;
        }
    }

    DcmStack stack;
    DcmXfer oxfer(META_HEADER_DEFAULT_TRANSFERSYNTAX);

    DcmMetaInfo *mi = ds->getMetaInfo();
    if (mi->card() > 0)
    {
      // we only check the meta-header if there is something to check
      checkitem(mi, oxfer, stack, showFullData, dderrors);
    }

    oxfer = ds->getDataset()->getOriginalXfer();
    checkitem(ds->getDataset(), oxfer, stack, showFullData, dderrors);

    delete ds;

    return 0;
}

//*********************************************************

static OFString printAttribute(
  OFString &ret,
  DcmItem* dset,
  const DcmTagKey& key)
{
    DcmElement *elem = NULL;
    DcmStack stack;
    OFCondition ec = EC_Normal;
    OFOStringStream str;

    ec = dset->search(key, stack, ESM_fromHere, OFFalse);
    elem = (DcmElement*) stack.top();
    if (elem)
        elem->print(str, DCMTypes::PF_shortenLongTagValues);
    else
        str << "<Attribute not found>";
    str << OFStringStream_ends;

    OFSTRINGSTREAM_GETSTR(str, tmp)
    ret = tmp;
    OFSTRINGSTREAM_FREESTR(tmp)
    return ret;
}

static OFBool
chkType1AttributeExistance(
  DcmItem* dset,
  const DcmTagKey& key)
{
    OFBool found = OFTrue;
    if (!dset->tagExistsWithValue(key)) {
        DcmTag t(key);
        OFLOG_WARN(dcmpschkLogger, MSGe_missingAtt << OFendl
            << "   Affected attribute: " << t.getXTag()
            << " " << t.getTagName() << OFendl);
        found = OFFalse;
    }
    return found;
}

int dcmchkMetaHeader(
  DcmMetaInfo* meta,
  DcmDataset* dset)
{

    if (meta == NULL || meta->card() == 0) {
        /* no meta-header so no errors */
        return 0;
    }

    int nErrs = 0;
    OFString tmp_str;

    /*
    ** The meta-header should use the LittleEndianExplicit transfer syntax
    */
    if (meta->getOriginalXfer() != EXS_LittleEndianExplicit) {
        DcmXfer used(meta->getOriginalXfer());
        DcmXfer expected(EXS_LittleEndianExplicit);
        OFLOG_WARN(dcmpschkLogger, MSGe_mhxferError << OFendl
            << "    Expected: " << expected.getXferName() << OFendl
            << "    Used:     " << used.getXferName() << OFendl);
        nErrs++;
    }

    /*
    ** Check the meta-header contents
    */

    // examine the FileMetaInformationVersion
    DcmTagKey fmiv(DCM_FileMetaInformationVersion);
    if (chkType1AttributeExistance(meta, fmiv)) {
        Uint8 b0 = 0xff;
        Uint8 b1 = 0xff;
        // get bytes
        meta->findAndGetUint8(fmiv, b0, 0);
        meta->findAndGetUint8(fmiv, b1, 1);
        // we expect 0x00/0x01 for the version
        if ((b0 != 0x00) || (b1 != 0x01)) {
            OFLOG_WARN(dcmpschkLogger, MSGe_wrongAtt << OFendl
                << "   Invalid FileMetaInformationVersion (expected: 00\\01)" << OFendl
                << "   Affected attribute: " << OFendl
                << "      " << printAttribute(tmp_str, meta, fmiv));
            nErrs++;
        }
    } else {
        nErrs++;
    }

    // examine the MediaStorageSOPClassUID
    DcmTagKey msscuid(DCM_MediaStorageSOPClassUID);
    if (chkType1AttributeExistance(meta, msscuid)) {
        OFString metaHeaderClassUID;
        meta->findAndGetOFStringArray(msscuid, metaHeaderClassUID);
        // should be the same as SOPClassUID in the dataset
        if (dset && dset->tagExistsWithValue(DCM_SOPClassUID)) {
            OFString datasetClassUID;
            dset->findAndGetOFStringArray(DCM_SOPClassUID, datasetClassUID);
            if (metaHeaderClassUID != datasetClassUID) {
                OFLOG_WARN(dcmpschkLogger, MSGe_wrongAtt << OFendl
                    << "   Inconsistent SOP class information" << OFendl
                    << "   Affected attributes: " << OFendl
                    << "      " << printAttribute(tmp_str, meta, msscuid) << OFendl
                    << "      " << printAttribute(tmp_str, dset, DCM_SOPClassUID));
                nErrs++;
            }
        }
        if (!dcmFindNameOfUID(metaHeaderClassUID.c_str())) {
            OFLOG_WARN(dcmpschkLogger, MSGe_wrongAtt << OFendl
                << "   Unknown SOP Class" << OFendl
                << "   Affected attribute: " << OFendl
                << "      " << printAttribute(tmp_str, meta, msscuid));
            nErrs++;
        }
    } else {
        nErrs++;
    }

    // Examine MediaStorageSOPInstanceUID
    DcmTagKey mssiuid(DCM_MediaStorageSOPInstanceUID);
    if (chkType1AttributeExistance(meta, mssiuid)) {
        OFString metaHeaderInstanceUID;
        meta->findAndGetOFStringArray(mssiuid, metaHeaderInstanceUID);
        // should be the same as SOPInstanceUID in the dataset
        if (dset && dset->tagExistsWithValue(DCM_SOPInstanceUID)) {
            OFString datasetInstanceUID;
            dset->findAndGetOFStringArray(DCM_SOPInstanceUID, datasetInstanceUID);
            if (metaHeaderInstanceUID != datasetInstanceUID) {
                OFLOG_WARN(dcmpschkLogger, MSGe_wrongAtt << OFendl
                    << "   Inconsistent SOP instance information" << OFendl
                    << "   Affected attributes: " << OFendl
                    << "      " << printAttribute(tmp_str, meta, mssiuid) << OFendl
                    << "      " << printAttribute(tmp_str, dset, DCM_SOPInstanceUID));
                nErrs++;
            }
        }
    } else {
        nErrs++;
    }

    // examine the TransferSyntaxUID
    DcmTagKey tsuid(DCM_TransferSyntaxUID);
    if (chkType1AttributeExistance(meta, tsuid)) {
        OFString transferSyntaxUID;
        meta->findAndGetOFStringArray(tsuid, transferSyntaxUID);
        // is this transfer syntax known ?
        DcmXfer expected(transferSyntaxUID.c_str());
        if (expected.getXfer() == EXS_Unknown) {
            OFLOG_WARN(dcmpschkLogger, MSGe_wrongAtt << OFendl
                << "   Unknown Transfer Syntax" << OFendl
                << "   Affected attribute: " << OFendl
                << "      " << printAttribute(tmp_str, meta, tsuid));
            nErrs++;
        }

        // should be the same as transfer syntax used to read the dataset
        if (dset && (dset->getOriginalXfer() != EXS_LittleEndianExplicit)) {
            DcmXfer used(dset->getOriginalXfer());
            OFString usedTransferSyntaxUID(used.getXferID());
            if (transferSyntaxUID != usedTransferSyntaxUID) {
                OFLOG_WARN(dcmpschkLogger, MSGe_wrongAtt << OFendl
                    << "   Dataset not encoded using specified transfer syntax" << OFendl
                    << "   Affected attribute: " << OFendl
                    << "      " << printAttribute(tmp_str, meta, tsuid));
                OFLOG_WARN(dcmpschkLogger, "   Dataset encoded using: " << used.getXferName());
                nErrs++;
            }

        }
    } else {
        nErrs++;
    }

    // Check the group length information
    DcmTagKey gltag(DCM_FileMetaInformationGroupLength);
    if (chkType1AttributeExistance(meta, gltag)) {
        Uint32 len = 0;
        meta->findAndGetUint32(gltag, len, 0);
        // Compute how large the Meta-Header should be
        Uint32 expectedLength = meta->getLength(EXS_LittleEndianExplicit,
                                                EET_ExplicitLength);
        expectedLength -= 12; // less length of group length element itself

        if (len != expectedLength) {
            OFLOG_WARN(dcmpschkLogger, MSGe_wrongAtt << OFendl
                << "   Invalid meta-header group length (expected: "
                <<  expectedLength  << ")" << OFendl
                << "   Affected attribute: " << OFendl
                << "      " << printAttribute(tmp_str, meta, gltag));
            nErrs++;
        }
    } else {
        nErrs++;
    }

    return nErrs;
}

int checkfile(const char *filename)
{
    DcmFileFormat *dfile = new DcmFileFormat();
    if (dfile == NULL)
    {
      OFLOG_ERROR(dcmpschkLogger, "out of memory.");
      return -1;
    }

    OFCondition cond = dfile->loadFile(filename);
    if (! cond.good())
    {
      OFLOG_ERROR(dcmpschkLogger, cond.text() << " reading file: " << filename);
      delete dfile;
      return -1;
    }

    int numberOfErrors = 0;
    OFBool test_passed = OFTrue;

    DcmDataset *DataSet = dfile->getDataset();
    DcmMetaInfo *MetaInfo = dfile->getMetaInfo();

    OFLOG_INFO(dcmpschkLogger, "=========================================================");
    OFLOG_WARN(dcmpschkLogger, "Testing: " << filename);
    OFLOG_INFO(dcmpschkLogger, "=========================================================");

    if (MetaInfo)
    {
      OFLOG_INFO(dcmpschkLogger, "---------------------------------------------------------" << OFendl
            << "Pass 1 - Inconsistencies between Meta-header and Data Set" << OFendl
            << "---------------------------------------------------------");
      numberOfErrors += dcmchkMetaHeader(MetaInfo, DataSet);
    }

    OFLOG_INFO(dcmpschkLogger, "-------------------------------------------------------------" << OFendl
          << "Pass 2 - Inconsistencies between Data Dictionary and Data Set" << OFendl
          << "-------------------------------------------------------------");

    dcmchk(opt_filename, ERM_autoDetect, EXS_Unknown,
          OFFalse /* showFullData */, OFTrue /* loadAllDataInMemory */,
          numberOfErrors);

    OFLOG_INFO(dcmpschkLogger, "-------------------------------------------------------------" << OFendl
          << "Pass 3 - Semantic Check of Presentation State Object         " << OFendl
          << "-------------------------------------------------------------");

    DcmUniqueIdentifier sopclassuid(DCM_SOPClassUID);
    DcmStack stack;
    if (EC_Normal == DataSet->search(DCM_SOPClassUID, stack, ESM_fromHere, OFFalse))
    {
      sopclassuid = *((DcmUniqueIdentifier *)(stack.top()));
    }
    OFString aString;
    sopclassuid.getOFString(aString,0);
    if (aString == UID_GrayscaleSoftcopyPresentationStateStorage)
    {
      DcmPresentationState pState;
      if (pState.read(*DataSet).bad())
      {
        test_passed = OFFalse;
      }
    } else {
      OFLOG_INFO(dcmpschkLogger, "Not a Grayscale Softcopy Presentation State, skipping pass 3.");
    }
    if (numberOfErrors > 0) test_passed = OFFalse;

    if (test_passed) OFLOG_WARN(dcmpschkLogger, "Test passed.");
    else OFLOG_WARN(dcmpschkLogger, "Test failed - one or more errors.");

    if (dfile) delete dfile;
    return numberOfErrors;
}

#define SHORTCOL 3
#define LONGCOL 12

int main(int argc, char *argv[])
{

#ifdef HAVE_GUSI_H
    GUSISetup(GUSIwithSIOUXSockets);
    GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef WITH_TCPWRAPPER
    // this code makes sure that the linker cannot optimize away
    // the DUL part of the network module where the external flags
    // for libwrap are defined. Needed on OpenBSD.
    dcmTCPWrapperDaemonName.set(NULL);
#endif

#ifdef HAVE_WINSOCK_H
    WSAData winSockData;
    /* we need at least version 1.1 */
    WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
    WSAStartup(winSockVersionNeeded, &winSockData);
#endif

    OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , "Checking tool for presentation states", rcsid);
    OFCommandLine cmd;
    cmd.setOptionColumns(LONGCOL, SHORTCOL);
    cmd.setParamColumn(LONGCOL + SHORTCOL + 2);

    cmd.addParam("dcmfile-in", "presentation state file(s) to be checked", OFCmdParam::PM_MultiMandatory);

    cmd.addGroup("general options:");
     cmd.addOption("--help",      "-h",    "print this help text and exit", OFCommandLine::AF_Exclusive);
     cmd.addOption("--version",            "print version information and exit", OFCommandLine::AF_Exclusive);
     OFLog::addOptions(cmd);

    /* evaluate command line */
    prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
    if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::PF_ExpandWildcards))
    {
      /* check exclusive options first */
      if (cmd.hasExclusiveOption())
      {
        if (cmd.findOption("--version"))
        {
            app.printHeader(OFTrue /*print host identifier*/);
            COUT << OFendl << "External libraries used:";
#ifdef WITH_ZLIB
            COUT << OFendl << "- ZLIB, Version " << zlibVersion() << OFendl;
#else
            COUT << " none" << OFendl;
#endif
            return 0;
         }
      }

      /* options */
      OFLog::configureFromCommandLine(cmd, app);
    }

    /* print resource identifier */
    OFLOG_DEBUG(dcmpschkLogger, rcsid << OFendl);

    int paramCount = cmd.getParamCount();
    for (int param=1; param <= paramCount; param++)
    {
      cmd.getParam(param, opt_filename);
      checkfile(opt_filename);
    }

#ifdef DEBUG
    dcmDataDict.clear();  /* useful for debugging with dmalloc */
#endif
    return 0;
}

/*
 * CVS/RCS Log:
 * $Log: dcmpschk.cc,v $
 * Revision 1.31  2010-10-14 13:13:45  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.30  2009-11-27 10:51:16  joergr
 * Fixed various issues with syntax usage (e.g. layout and formatting).
 *
 * Revision 1.29  2009-11-24 14:12:56  uli
 * Switched to logging mechanism provided by the "new" oflog module.
 *
 * Revision 1.28  2009-08-03 09:12:53  joergr
 * Added support for checking the data type UT (Unlimited Text).
 * Moved flex++ generated lexical scanner from module "dcmpstat" to "dcmdata".
 *
 * Revision 1.27  2009-07-28 14:17:55  joergr
 * Added support for dubious date checking (year before 1850 or after 2050).
 *
 * Revision 1.26  2008-09-25 16:30:24  joergr
 * Added support for printing the expanded command line arguments.
 * Always output the resource identifier of the command line tool in debug mode.
 *
 * Revision 1.25  2008-04-30 12:38:43  meichel
 * Fixed compile errors due to changes in attribute tag names
 *
 * Revision 1.24  2006/08/15 16:57:01  meichel
 * Updated the code in module dcmpstat to correctly compile when
 *   all standard C++ classes remain in namespace std.
 *
 * Revision 1.23  2006/07/27 14:38:03  joergr
 * Changed parameter "exclusive" of method addOption() from type OFBool into an
 * integer parameter "flags". Prepended prefix "PF_" to parseLine() flags.
 * Option "--help" is no longer an exclusive option by default.
 * Made naming conventions for command line parameters more consistent, e.g.
 * used "dcmfile-in", "dcmfile-out" and "bitmap-out".
 *
 * Revision 1.22  2005/12/14 17:43:42  meichel
 * Adapted code for compilation with TCP wrappers to NetBSD
 *
 * Revision 1.21  2005/12/12 15:14:34  meichel
 * Added code needed for compilation with TCP wrappers on OpenBSD
 *
 * Revision 1.20  2005/12/08 15:46:07  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.19  2005/12/02 09:46:27  joergr
 * Added new file read mode that makes it possible to distinguish between DICOM
 * files, datasets and other non-DICOM files.  For this reason, the last
 * parameter of method loadFile() changed from OFBool to E_FileReadMode.
 *
 * Revision 1.18  2005/11/28 15:29:05  meichel
 * File dcdebug.h is not included by any other header file in the toolkit
 *   anymore, to minimize the risk of name clashes of macro debug().
 *
 * Revision 1.17  2005/03/09 18:07:53  joergr
 * Fixed spelling error.
 *
 * Revision 1.16  2004/02/04 15:44:38  joergr
 * Removed acknowledgements with e-mail addresses from CVS log.
 *
 * Revision 1.15  2003/09/05 09:00:49  meichel
 * Updated presentation state checker to use class DcmPresentationState
 *   instead of DVPresentationState. Imported updated VR checking code
 *   from module dcmcheck.
 *
 * Revision 1.14  2003/09/01 12:58:58  wilkens
 * Added #include to file to be able to compile again under Win32.
 *
 * Revision 1.13  2003/03/12 17:34:20  meichel
 * Updated DcmObject::print() flags
 *
 * Revision 1.12  2002/11/27 15:47:52  meichel
 * Adapted module dcmpstat to use of new header file ofstdinc.h
 *
 * Revision 1.11  2002/11/26 08:44:27  meichel
 * Replaced all includes for "zlib.h" with <zlib.h>
 *   to avoid inclusion of zlib.h in the makefile dependencies.
 *
 * Revision 1.10  2002/09/23 18:26:07  joergr
 * Added new command line option "--version" which prints the name and version
 * number of external libraries used (incl. preparation for future support of
 * 'config.guess' host identifiers).
 *
 * Revision 1.9  2002/08/20 12:21:53  meichel
 * Adapted code to new loadFile and saveFile methods, thus removing direct
 *   use of the DICOM stream classes.
 *
 * Revision 1.8  2002/06/14 10:44:18  meichel
 * Adapted log file handling to ofConsole singleton
 *
 * Revision 1.7  2002/05/02 14:10:04  joergr
 * Added support for standard and non-standard string streams (which one is
 * supported is detected automatically via the configure mechanism).
 *
 * Revision 1.6  2002/04/16 14:01:27  joergr
 * Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 * Revision 1.5  2001/11/09 16:04:51  joergr
 * Changed type of variable to avoid compiler warnings (comparison of signed
 * and unsigned data).
 *
 * Revision 1.4  2001/10/02 11:51:59  joergr
 * Introduced new general purpose functions to get/put DICOM element values
 * from/to an item/dataset - removed some old and rarely used functions.
 *
 * Revision 1.3  2001/09/26 15:36:02  meichel
 * Adapted dcmpstat to class OFCondition
 *
 * Revision 1.2  2001/06/01 15:50:08  meichel
 * Updated copyright header
 *
 * Revision 1.1  2000/06/21 15:40:32  meichel
 * Added initial version of Presentation State Checker.
 *
 *
 */
