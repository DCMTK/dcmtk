/*
 *
 *  Copyright (C) 2000-2017, OFFIS e.V.
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
 *  Module: dcmsign
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose: Create and Verify DICOM Digital Signatures
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmdata/cmdlnarg.h"
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmdata/dcuid.h"      /* for dcmtk version name */

#ifdef WITH_ZLIB
#include <zlib.h>                     /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmsign"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

static OFLogger dcmsignLogger = OFLog::getLogger("dcmtk.apps." OFFIS_CONSOLE_APPLICATION);

#define APPLICATION_ABSTRACT "Sign and Verify DICOM Files"


#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/dcsignat.h"
#include "dcmtk/dcmsign/sinullpr.h"
#include "dcmtk/dcmsign/sibrsapr.h"
#include "dcmtk/dcmsign/siautopr.h"
#include "dcmtk/dcmsign/sicreapr.h"
#include "dcmtk/dcmsign/simac.h"
#include "dcmtk/dcmsign/simd5.h"
#include "dcmtk/dcmsign/sisha1.h"
#include "dcmtk/dcmsign/sisha256.h"
#include "dcmtk/dcmsign/sisha384.h"
#include "dcmtk/dcmsign/sisha512.h"
#include "dcmtk/dcmsign/siripemd.h"
#include "dcmtk/dcmsign/siprivat.h"
#include "dcmtk/dcmsign/sicert.h"
#include "dcmtk/dcmdata/dctk.h"


BEGIN_EXTERN_C
#include <openssl/x509.h>
END_EXTERN_C

// ********************************************

enum DcmSignOperation
{
  DSO_verify,
  DSO_sign,
  DSO_signItem,
  DSO_remove,
  DSO_removeAll
};

/* reads an attribute tag in the form "gggg,eeee" and adds it
 * to the given attribute tag list
 * @param c input string
 * @param tagList list to be added to
 * @return true if successful, false otherwise
 */
static OFBool addTag(const char *c, DcmAttributeTag& tagList)
{
  OFBool result = OFFalse;
  unsigned int group = 0xffff;
  unsigned int elem = 0xffff;
  if (sscanf(c, "%x,%x", &group, &elem ) != 2 )
  {
    /* it is a name */
    const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
    const DcmDictEntry *dicent = globalDataDict.findEntry(c);
    if (dicent)
    {
      if (EC_Normal == tagList.putTagVal(dicent->getKey(), tagList.getVM())) result = OFTrue;
    }
    dcmDataDict.unlock();
  } else {
    if (EC_Normal == tagList.putTagVal(DcmTagKey(group,elem), tagList.getVM())) result = OFTrue;
  }
  return result;
}

/* scans a token from the given string and returns it. Ignores leading whitespace.
 * @param c string to parse
 * @param pos position within string, modified after successful scan
 * @param key tag key returned in this parameter if return value is "tag key".
 * @param idx index returned in this parameter if return value is "index".
 * @return -1 for "EOF", 0 for "parse error", 1 for "tag key", 2 for "index", 3 for "period"
 */
static int readNextToken(const char *c, int& pos, DcmTagKey& key, Uint32& idx)
{
  OFString aString;
  int lpos = pos;
  int spos = 0;
  while(isspace(OFstatic_cast(unsigned char, c[lpos]))) ++lpos; // ignore leading space

  if (c[lpos]=='\0') return -1; // EOF
  if (c[lpos]=='.')
  {
    ++pos;
    return 3; // period
  }
  if (c[lpos]=='[')
  {
    spos = ++lpos;
    while ((c[lpos] >= '0')&&(c[lpos] <= '9')) ++lpos;
    if (c[lpos] != ']') return 0; // parse error
    unsigned long newindex = 0;
    if (1 != sscanf(c+spos,"%lu", &newindex)) return 0; // parse error
    idx = OFstatic_cast(Uint32, newindex);
    pos = ++lpos;
    return 2; // index
  }
  if (c[lpos]=='(')
  {
    spos = ++lpos;
    while ((c[lpos] != ')')&&(c[lpos] != '\0')) ++lpos;
    if (c[lpos] != ')') return 0; // parse error
    unsigned int group=0;
    unsigned int elem=0;
    if (2 != sscanf(c+spos,"%x,%x", &group, &elem)) return 0; // parse error
    key = DcmTagKey(group,elem);
    pos = ++lpos;
    return 1; // tag key
  }
  spos = lpos;
  while ( ((c[lpos] >= 'a')&&(c[lpos] <= 'z')) || ((c[lpos] >= 'A')&&(c[lpos] <= 'Z')) || ((c[lpos] >= '0')&&(c[lpos] <= '9'))) ++lpos;
  aString.append(c + spos, (lpos-spos));
  const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
  const DcmDictEntry *dicent = globalDataDict.findEntry(aString.c_str());
  if (dicent)
  {
    key = dicent->getKey();
    dcmDataDict.unlock();
    pos = lpos;
    return 1; // tag key;
  }
  dcmDataDict.unlock();
  OFLOG_ERROR(dcmsignLogger, "attribute name '" << aString.c_str() << "' unknown.");
  return 0; // parse error
}

/* reads a complete text file (max 64K) into a memory block
 * and returns a pointer to the memory block.
 * memory must be freed by called.
 * @param filename file to be read
 * @return pointer to memory block if successful, NULL otherwise.
 */
static char *readTextFile(const char *filename)
{
  char *result = NULL;
  FILE *file = fopen(filename, "rb");
  if (file)
  {
    fseek(file, 0, SEEK_END);
    long numBytes = ftell(file);
    fseek(file, 0, SEEK_SET);
    if (numBytes > 65536)
    {
      OFLOG_WARN(dcmsignLogger, "text file too large, ignoring everything beyond 64K.");
      numBytes = 65536;
    }
    result = new char[numBytes];
    if (result)
    {
      if (OFstatic_cast(size_t, numBytes) != fread(result, 1, OFstatic_cast(size_t, numBytes), file))
      {
        OFLOG_WARN(dcmsignLogger, "read error in file " << filename);
        delete[] result;
        result = NULL;
      }
    }
    fclose(file);
  } else {
    OFLOG_ERROR(dcmsignLogger, "file not found: " << filename);
  }
  return result;
}

/* reads a list of attributes from a text file.
 * The attributes can be in the form (gggg,eeee) or can be dictionary names,
 * separated by arbitrary whitespace.
 * @param filename file to be read from
 * @param tagList attribute tags are added to this list
 * @return 0 if successful, a program exit code otherwise
 */
static int parseTextFile(const char *filename, DcmAttributeTag& tagList)
{
  char *c=readTextFile(filename);
  if (c==NULL) return 10; // bail out
  int position = 0;
  int token = 0;
  Uint32 idx = 0;
  DcmTagKey key;
  int result = 0;
  do
  {
    token = readNextToken(c, position, key, idx);
    if (token == 1) // we have read a tag key
    {
      if (EC_Normal != tagList.putTagVal(key, tagList.getVM()))
      {
        result = 10;
        token = -1;
      }
    }
    else if (token >= 0)
    {
      OFLOG_ERROR(dcmsignLogger, "parse error in text file '" << filename << "'");
      result = 10;
      token = -1;
    }
  } while (token >= 0);
  delete[] c;
  return result;
}


/* locates a specific item within the given dataset.
 * @param dataset dataset to be searched
 * @param location location string. Format is "sequence[item]{.sequence[item]}*"
 *   Where sequence can be (gggg,eeee) or a dictionary name and items
 *   within sequences are counted from zero.
 * @return pointer to the item searched if found, NULL otherwise
 */
static DcmItem *locateItemforSignatureCreation(DcmItem& dataset, const char *location)
{
  DcmTagKey key;
  Uint32 idx;
  int pos = 0;
  int token = 0;
  int expected = 1;
  OFBool finished = OFFalse;
  DcmItem *result = &dataset;
  DcmSequenceOfItems *sq = NULL;
  DcmStack stack;
  do
  {
    token = readNextToken(location, pos, key, idx);
    if ((token != expected)&&(token != -1))
    {
      OFLOG_ERROR(dcmsignLogger, "parse error in item location string '" << location << "'");
      return NULL;
    }
    if (token == -1)
    {
      if (! finished)
      {
        OFLOG_ERROR(dcmsignLogger, "item location string '" << location << "' incomplete.");
        return NULL;
      }
      return result;
    }
    if (token == 1)
    {
      // we have read a tag key
      stack.clear();
      if (EC_Normal != result->search(key, stack, ESM_fromHere, OFFalse))
      {
        OFLOG_ERROR(dcmsignLogger, "attribute " << key << " not found in dataset (item location string is '" << location << "')");
        return NULL;
      }
      if (stack.top()->ident() == EVR_SQ)
      {
        sq = OFstatic_cast(DcmSequenceOfItems *, (stack.top()));
      } else {
        OFLOG_ERROR(dcmsignLogger, "attribute " << key << " is not a sequence (item location string is '" << location << "')");
        return NULL;
      }
      expected = 2;
      finished = OFFalse;
    }
    else if (token == 2)
    {
      // we have read an index
      if (sq == NULL)
      {
        OFLOG_ERROR(dcmsignLogger, "sequence not found in item location string '" << location << "'");
        return NULL;
      }
      if (idx >= sq->card())
      {
        OFLOG_ERROR(dcmsignLogger, "sequence " << sq->getTag() << " only has " << sq->card()
          << " items, cannot locate item " << idx << " (item location string is '" << location << "')");
        return NULL;
      }
      result = sq->getItem(idx);
      if (result == NULL)
      {
        OFLOG_ERROR(dcmsignLogger, "item not found in item location string '" << location << "'");
        return NULL;
      }
      expected = 3;
      finished = OFTrue;
    }
    else if (token == 3)
    {
      // we have read a period
      expected = 1;
      finished = OFFalse;
    }
  } while (token > 0);
  return NULL;
}

/* performs a signature operation on a given dataset
 * @param dataset to sign
 * @param key private key for signature
 * @param cert certificate for signature
 * @param opt_mac MAC for signature
 * @param opt_profile security profile for signature
 * @param opt_tagList list of attribute tags, may be NULL
 * @return 0 if successful, a program exit code otherwise
 */
static int do_sign(
  DcmItem *dataset,
  SiPrivateKey& key,
  SiCertificate& cert,
  SiMAC *opt_mac,
  SiSecurityProfile *opt_profile,
  DcmAttributeTag *opt_tagList,
  E_TransferSyntax opt_signatureXfer,
  FILE *dumpFile)
{
  OFCondition sicond = EC_Normal;
  DcmSignature signer;
  signer.attach(dataset);
  signer.setDumpFile(dumpFile);
  sicond = signer.createSignature(key, cert, *opt_mac, *opt_profile, opt_signatureXfer, opt_tagList);
  if (sicond != EC_Normal)
  {
    OFLOG_ERROR(dcmsignLogger, sicond.text() << ": while creating signature in main dataset");
    return 1;
  }
  return 0;
}

/* print the location stack into the given stack.
 * It is assumed that the stack top is a DigitalSignatureSequence which is not printed
 * and that the stack bottom is the main dataset, which is also not printed.
 * @param stack search stack, as returned by DcmSignature::findFirstSignatureItem() etc.
 * @param str printable text returned in this string.
 */
static void printSignatureItemPosition(DcmStack& stack, OFString& str)
{
  str.clear();
  DcmObject *elem = NULL;
  DcmSequenceOfItems *sq = NULL;
  unsigned long sqCard=0;
  const char *tagname = NULL;
  unsigned long m=0;
  char buf[20];

  if (stack.card() > 2)
  {
    // signature is located within a sequence
    for (unsigned long l=stack.card()-2; l>0; --l) // loop over all elements except the stack top and bottom
    {
      elem = stack.elem(l);
      if (elem)
      {
        if ((elem->ident() == EVR_item) && sq)
        {
          sqCard = sq->card();
          for (m=0; m<sqCard; m++)
          {
            if (sq->getItem(m) == elem)
            {
              sprintf(buf, "[%lu]", m);
              str.append(buf);
            }
          }
        }
        else
        {
          if (str.size() > 0) str.append(".");
          sq = OFstatic_cast(DcmSequenceOfItems *, elem);
          DcmTag currentTag(elem->getTag());
          tagname = currentTag.getTagName();
          if (tagname) str.append(tagname); else
          {
            sprintf(buf, "(%04x,%04x)", elem->getTag().getGroup(), elem->getTag().getElement());
            str.append(buf);
          }
          if (elem->ident() == EVR_SQ) sq = OFstatic_cast(DcmSequenceOfItems *, elem); else sq = NULL;
        }
      }
    }
  } else {
    // signature is located in the main dataset
    str = "Main Dataset";
  }
}

/* performs a signature operation on a sub-item within a dataset
 * @param dataset in which to sign
 * @param key private key for signature
 * @param cert certificate for signature
 * @param opt_mac MAC for signature
 * @param opt_profile security profile for signature
 * @param opt_tagList list of attribute tags, may be NULL
 * @param location location string. Format is "sequence[item]{.sequence[item]}*"
 *   Where sequence can be (gggg,eeee) or a dictionary name and items
 *   within sequences are counted from zero.
 * @return 0 if successful, a program exit code otherwise
 */
static int do_sign_item(
  DcmItem *dataset,
  SiPrivateKey& key,
  SiCertificate& cert,
  SiMAC *opt_mac,
  SiSecurityProfile *opt_profile,
  DcmAttributeTag *opt_tagList,
  const char *opt_location,
  E_TransferSyntax opt_signatureXfer,
  FILE *dumpFile)
{
  OFCondition sicond = EC_Normal;
  DcmSignature signer;
  DcmItem *sigItem = locateItemforSignatureCreation(*dataset, opt_location);
  if (sigItem == NULL) return 1;

  signer.detach();
  signer.attach(sigItem);
  signer.setDumpFile(dumpFile);
  sicond = signer.createSignature(key, cert, *opt_mac, *opt_profile, opt_signatureXfer, opt_tagList);
  if (sicond != EC_Normal)
  {
    OFLOG_ERROR(dcmsignLogger, sicond.text() << ": while creating signature in item '" << opt_location << "'");
    return 1;
  }
  signer.detach();
  return 0;
}

/* verify all signatures in the given dataset and print results to COUT.
 * @param dataset dataset to verify
 * @return 0 if successful, a program exit code otherwise
 */
static int do_verify(
  DcmItem *dataset)
{
  OFCondition sicond = EC_Normal;
  DcmStack stack;
  DcmSignature signer;
  OFString aString;
  int counter = 0;
  int corrupt_counter = 0;
  unsigned long numSignatures = 0;
  unsigned long l=0;
  Uint16 macID = 0;
  DcmAttributeTag at(DCM_DataElementsSigned);
  DcmItem *sigItem = DcmSignature::findFirstSignatureItem(*dataset, stack);
  DcmTagKey tagkey;
  DcmTag tag;
  const char *tagName = NULL;

  while (sigItem)
  {
    signer.attach(sigItem);
    numSignatures = signer.numberOfSignatures();
    for (l=0; l<numSignatures; l++)
    {
      if (EC_Normal == signer.selectSignature(l))
      {
        ++counter;
        if (EC_Normal == signer.getCurrentSignatureUID(aString))
          OFLOG_WARN(dcmsignLogger, "Signature #" << counter << " UID=" << aString);
        else
          OFLOG_WARN(dcmsignLogger, "Signature #" << counter << " UID=" << "(unknown)");
        printSignatureItemPosition(stack, aString);
        if (dcmsignLogger.isEnabledFor(OFLogger::INFO_LOG_LEVEL))
        {
          OFLOG_INFO(dcmsignLogger, "  Location                    : " << aString);
          if (EC_Normal == signer.getCurrentMacID(macID))
            OFLOG_INFO(dcmsignLogger, "  MAC ID                      : " << macID);
          else
            OFLOG_INFO(dcmsignLogger, "  MAC ID                      : (unknown)");
          if (EC_Normal == signer.getCurrentMacName(aString))
            OFLOG_INFO(dcmsignLogger, "  MAC algorithm               : " << aString);
          else
            OFLOG_INFO(dcmsignLogger, "  MAC algorithm               : (unknown)");
          if (EC_Normal == signer.getCurrentMacXferSyntaxName(aString))
            OFLOG_INFO(dcmsignLogger, "  MAC calculation xfer syntax : " << aString);
          else
            OFLOG_INFO(dcmsignLogger, "  MAC calculation xfer syntax : (unknown)");
          // data elements signed
          if (EC_Normal != signer.getCurrentDataElementsSigned(at))
            OFLOG_INFO(dcmsignLogger, "  Data elements signed        : all elements");
          else
          {
            OFLOG_INFO(dcmsignLogger, "  Data elements signed        :");
            unsigned long atVM = at.getVM();
            for (unsigned long n=0; n<atVM; n++)
            {
              if (EC_Normal == at.getTagVal(tagkey, n))
              {
                tag = tagkey;
                tagName = tag.getTagName();
                OFLOG_INFO(dcmsignLogger, "      " << tagkey << " " << (tagName != NULL ? tagName : ""));
              }
            }
          }

          if (EC_Normal == signer.getCurrentSignatureDateTime(aString))
            OFLOG_INFO(dcmsignLogger, "  Signature date/time         : " << aString);
          else
            OFLOG_INFO(dcmsignLogger, "  Signature date/time         : (unknown)");
          OFLOG_INFO(dcmsignLogger, "  Certificate of signer       : ");
          SiCertificate *cert = signer.getCurrentCertificate();
          if ((cert == NULL)||(cert->getKeyType()==EKT_none))
            OFLOG_INFO(dcmsignLogger, "      none");
          else
          {
            OFLOG_INFO(dcmsignLogger, "      X.509v" << cert->getX509Version());
            cert->getCertSubjectName(aString);
            OFLOG_INFO(dcmsignLogger, "      Subject                 : " << aString);

            cert->getCertIssuerName(aString);
            OFLOG_INFO(dcmsignLogger, "      Issued by               : " << aString);
            OFLOG_INFO(dcmsignLogger, "      Serial no.              : " << cert->getCertSerialNo());
            cert->getCertValidityNotBefore(aString);
            OFLOG_INFO(dcmsignLogger, "      Validity                : not before " << aString);
            cert->getCertValidityNotAfter(aString);
            OFLOG_INFO(dcmsignLogger, "      Validity                : not after " << aString);
            switch (cert->getKeyType())
            {
              case EKT_RSA:
                OFLOG_INFO(dcmsignLogger, "      Public key              : RSA, " << cert->getCertKeyBits() << " bits");
                break;
              case EKT_DSA:
                OFLOG_INFO(dcmsignLogger, "      Public key              : DSA, " << cert->getCertKeyBits() << " bits");
                break;
              case EKT_DH:
                OFLOG_INFO(dcmsignLogger, "      Public key              : DH, " << cert->getCertKeyBits() << " bits");
                break;
              case EKT_none: // should never happen
                OFLOG_INFO(dcmsignLogger, "      Public key              : none");
                break;
            }
          }
          aString = "  Verification                : ";
        } else {
          OFLOG_INFO(dcmsignLogger, "  Location     : " << aString);
          aString = "  Verification : ";
        }
        sicond = signer.verifyCurrent();
        if (sicond.good())
        {
          OFLOG_WARN(dcmsignLogger, aString << "OK\n");
        } else {
          corrupt_counter++;
          OFLOG_WARN(dcmsignLogger, aString << sicond.text() << "\n");
        }
      }
    }
    signer.detach();
    sigItem = DcmSignature::findNextSignatureItem(*dataset, stack);
  }
  if (counter == 0)
    OFLOG_WARN(dcmsignLogger, "no signatures found in dataset.");
  else
    OFLOG_INFO(dcmsignLogger, counter << " signatures verified in dataset, " << corrupt_counter << " corrupted.");
  return 0;
}

/* remove all signatures from the given dataset, print action details.
 * @param dataset dataset to modify
 * @return 0 if successful, a program exit code otherwise
 */
static int do_remove_all(
  DcmItem *dataset)
{
  OFCondition sicond = EC_Normal;
  DcmSignature signer;
  int counter = 0;
  OFString aString;
  DcmStack stack;
  DcmItem *sigItem = DcmSignature::findFirstSignatureItem(*dataset, stack);
  while (sigItem)
  {
    signer.attach(sigItem);
    while (signer.numberOfSignatures() > 0)
    {
      ++counter;
      if (EC_Normal == signer.selectSignature(0))
      {
        if (EC_Normal == signer.getCurrentSignatureUID(aString))
          OFLOG_WARN(dcmsignLogger, "Signature #" << counter << " UID=" << aString);
        else
          OFLOG_WARN(dcmsignLogger, "Signature #" << counter << " UID=(unknown)");
        printSignatureItemPosition(stack, aString);
        OFLOG_WARN(dcmsignLogger, "  Location                    : " << aString);
      }
      sicond = signer.removeSignature(0);
      if (sicond != EC_Normal)
      {
        OFLOG_ERROR(dcmsignLogger, sicond.text() << ": while removing signature");
        return 1;
      }
    }
    signer.detach();
    stack.pop(); // remove pointer to the Digital Signatures Sequence that we've just deleted.
    sigItem = DcmSignature::findNextSignatureItem(*dataset, stack);
  }

  OFLOG_INFO(dcmsignLogger, counter << " signatures found and removed from dataset.");
  return 0;
}

/* remove the signature with the given UID from the dataset, print action details.
 * @param dataset dataset to modify
 * @param opt_location Digital Signature UID of the signature to remove
 * @return 0 if successful, a program exit code otherwise
 */
static int do_remove(
  DcmItem *dataset,
  const char *opt_location)
{
  OFCondition sicond = EC_Normal;
  DcmSignature signer;
  OFString aString;
  DcmStack stack;
  unsigned long cardSQ;
  unsigned long i;
  DcmItem *sigItem = DcmSignature::findFirstSignatureItem(*dataset, stack);
  while (sigItem)
  {
    signer.attach(sigItem);
    cardSQ = signer.numberOfSignatures();
    for (i=0; i<cardSQ; i++)
    {
      if (EC_Normal == signer.selectSignature(i))
      {
        if (EC_Normal == signer.getCurrentSignatureUID(aString))
        {
          if (aString == opt_location)
          {
            OFLOG_WARN(dcmsignLogger, "Signature UID=" << aString);
            printSignatureItemPosition(stack, aString);
            OFLOG_WARN(dcmsignLogger, "  Location                    : " << aString);

            sicond = signer.removeSignature(i);
            if (sicond != EC_Normal)
            {
              OFLOG_ERROR(dcmsignLogger, sicond.text() << ": while removing signature");
              return 1;
            } else {
              return 0;
            }
          }
        }
      }
    }
    signer.detach();
    sigItem = DcmSignature::findNextSignatureItem(*dataset, stack);
  }
  OFLOG_ERROR(dcmsignLogger, "signature with UID '" << opt_location << "' not found.");
  return 1;
}


#define SHORTCOL 4
#define LONGCOL 21

int main(int argc, char *argv[])
{
  DcmSignature::initializeLibrary(); // initialize dcmsign

  const char *                  opt_certfile = NULL;
  OFCmdUnsignedInt              opt_filepad = 0;
  E_FileReadMode                opt_readMode = ERM_autoDetect;
  const char *                  opt_ifname = NULL;
  OFCmdUnsignedInt              opt_itempad = 0;
  E_TransferSyntax              opt_ixfer = EXS_Unknown;
  const char *                  opt_keyfile = NULL;  // private key file
  int                           opt_keyFileFormat = X509_FILETYPE_PEM; // file format for certificates and private keys
  const char *                  opt_location = NULL; // location (path) within dataset
  SiMAC *                       opt_mac = NULL;      // MAC object
  E_EncodingType                opt_oenctype = EET_ExplicitLength;
  const char *                  opt_ofname = NULL;
  E_GrpLenEncoding              opt_oglenc = EGL_recalcGL;
  E_PaddingEncoding             opt_opadenc = EPD_noChange;
  DcmSignOperation              opt_operation = DSO_verify; // command to execute
  E_TransferSyntax              opt_oxfer = EXS_Unknown;
  const char *                  opt_passwd = NULL;  // password for private key
  SiSecurityProfile *           opt_profile = NULL; // security profile
  const char *                  opt_tagFile = NULL; // text file with attribute tags
  DcmAttributeTag *             opt_tagList = NULL; // list of attribute tags
  E_TransferSyntax              opt_signatureXfer = EXS_Unknown;
  FILE *                        opt_dumpFile = NULL;
  int result = 0;

  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , APPLICATION_ABSTRACT, rcsid);
  OFCommandLine cmd;
  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

  cmd.addParam("dcmfile-in",  "DICOM input filename to be processed");
  cmd.addParam("dcmfile-out", "DICOM output filename", OFCmdParam::PM_Optional);

  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                      "-h",        "print this help text and exit", OFCommandLine::AF_Exclusive);
      cmd.addOption("--version",                                "print version information and exit", OFCommandLine::AF_Exclusive);
      OFLog::addOptions(cmd);

  cmd.addGroup("input options:");
    cmd.addSubGroup("input file format:");
      cmd.addOption("--read-file",                 "+f",        "read file format or data set (default)");
      cmd.addOption("--read-file-only",            "+fo",       "read file format only");
      cmd.addOption("--read-dataset",              "-f",        "read data set without file meta information");
    cmd.addSubGroup("input transfer syntax:", LONGCOL, SHORTCOL);
      cmd.addOption("--read-xfer-auto",            "-t=",       "use TS recognition (default)");
      cmd.addOption("--read-xfer-detect",          "-td",       "ignore TS specified in the file meta header");
      cmd.addOption("--read-xfer-little",          "-te",       "read with explicit VR little endian TS");
      cmd.addOption("--read-xfer-big",             "-tb",       "read with explicit VR big endian TS");
      cmd.addOption("--read-xfer-implicit",        "-ti",       "read with implicit VR little endian TS");

  cmd.addGroup("signature commands:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--verify",                                 "verify all signatures (default)");
      cmd.addOption("--sign",                      "+s",     2, "[p]rivate key file, [c]ertificate file: string",
                                                                "create signature in main object");
      cmd.addOption("--sign-item",                 "+si",    3, "[k]eyfile, [c]ertfile, [i]tem location: string",
                                                                "create signature in sequence item");
      cmd.addOption("--remove",                    "+r",     1, "[s]ignature UID: string", "remove signature");
      cmd.addOption("--remove-all",                "+ra",       "remove all signatures from data set");

  cmd.addGroup("signature creation options (only with --sign or --sign-item):");
    cmd.addSubGroup("private key password:");
      cmd.addOption("--std-passwd",               "+ps",        "prompt user to type password on stdin (default)");
      cmd.addOption("--use-passwd",               "+pw",    1,  "[p]assword: string ",
                                                                "use specified password");
      cmd.addOption("--null-passwd",              "-pw",        "use empty string as password");
    cmd.addSubGroup("key and certificate file format:");
      cmd.addOption("--pem-keys",                 "-pem",       "read keys/certificates as PEM file (default)");
      cmd.addOption("--der-keys",                 "-der",       "read keys/certificates as DER file");
    cmd.addSubGroup("digital signature profile:");
      cmd.addOption("--profile-none",             "-pf",        "don't enforce any signature profile (default)");
      cmd.addOption("--profile-base",             "+pb",        "enforce base RSA signature profile");
      cmd.addOption("--profile-creator",          "+pc",        "enforce creator RSA signature profile");
      cmd.addOption("--profile-auth",             "+pa",        "enforce authorization signature profile");
    cmd.addSubGroup("MAC algorithm:");
      cmd.addOption("--mac-ripemd160",            "+mr",        "use RIPEMD 160 (default)");
      cmd.addOption("--mac-sha1",                 "+ms",        "use SHA-1");
      cmd.addOption("--mac-md5",                  "+mm",        "use MD 5");
      cmd.addOption("--mac-sha256",               "+m2",        "use SHA-256");
      cmd.addOption("--mac-sha384",               "+m3",        "use SHA-384");
      cmd.addOption("--mac-sha512",               "+m5",        "use SHA-512");

    cmd.addSubGroup("tag selection:");
      cmd.addOption("--tag",                      "-t",      1, "[t]ag: \"gggg,eeee\" or dictionary name", "sign only specified tag\n(this option can be specified multiple times)");
      cmd.addOption("--tag-file",                 "-tf",     1, "[f]ilename: string", "read list of tags from text file");
    cmd.addSubGroup("signature format:");
      cmd.addOption("--format-new",               "-fn",        "use correct DICOM signature format (default)");
      cmd.addOption("--format-old",               "-fo",        "use old (pre-3.5.4) DCMTK signature format,\n"
                                                                "non-conformant if signature includes\ncompressed pixel data");
  cmd.addGroup("output options:");
    cmd.addSubGroup("output transfer syntax:");
      cmd.addOption("--write-xfer-same",          "+t=",        "write with same TS as input (default)");
      cmd.addOption("--write-xfer-little",        "+te",        "write with explicit VR little endian TS");
      cmd.addOption("--write-xfer-big",           "+tb",        "write with explicit VR big endian TS");
      cmd.addOption("--write-xfer-implicit",      "+ti",        "write with implicit VR little endian TS");
    cmd.addSubGroup("length encoding in sequences and items:");
      cmd.addOption("--length-explicit",          "+e",         "write with explicit lengths (default)");
      cmd.addOption("--length-undefined",         "-e",         "write with undefined lengths");
    cmd.addSubGroup("other output options:");
      cmd.addOption("--dump",                      "+d",     1, "[f]ilename: string",
                                                                "dump byte stream fed into the MAC codec to file\n(only with --sign or --sign-item)");

  /* evaluate command line */
  prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
  if (app.parseCommandLine(cmd, argc, argv))
  {
    /* check exclusive options first */
    if (cmd.hasExclusiveOption())
    {
        if (cmd.findOption("--version"))
        {
            app.printHeader(OFTrue /*print host identifier*/);
            COUT << OFendl << "External libraries used:";
#if !defined(WITH_ZLIB) && !defined(WITH_OPENSSL)
            COUT << " none" << OFendl;
#else
            COUT << OFendl;
#endif
#ifdef WITH_ZLIB
            COUT << "- ZLIB, Version " << zlibVersion() << OFendl;
#endif
#ifdef WITH_OPENSSL
            COUT << "- " << OPENSSL_VERSION_TEXT << OFendl;
#endif
            return 0;
        }
    }

    /* command line parameters */

    cmd.getParam(1, opt_ifname);
    if (cmd.getParamCount() > 1) cmd.getParam(2, opt_ofname);

    OFLog::configureFromCommandLine(cmd, app);

    cmd.beginOptionBlock();
    if (cmd.findOption("--read-file")) opt_readMode = ERM_autoDetect;
    if (cmd.findOption("--read-file-only")) opt_readMode = ERM_fileOnly;
    if (cmd.findOption("--read-dataset")) opt_readMode = ERM_dataset;
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--read-xfer-auto"))
        opt_ixfer = EXS_Unknown;
    if (cmd.findOption("--read-xfer-detect"))
        dcmAutoDetectDatasetXfer.set(OFTrue);
    if (cmd.findOption("--read-xfer-little"))
    {
        app.checkDependence("--read-xfer-little", "--read-dataset", opt_readMode == ERM_dataset);
        opt_ixfer = EXS_LittleEndianExplicit;
    }
    if (cmd.findOption("--read-xfer-big"))
    {
        app.checkDependence("--read-xfer-big", "--read-dataset", opt_readMode == ERM_dataset);
        opt_ixfer = EXS_BigEndianExplicit;
    }
    if (cmd.findOption("--read-xfer-implicit"))
    {
        app.checkDependence("--read-xfer-implicit", "--read-dataset", opt_readMode == ERM_dataset);
        opt_ixfer = EXS_LittleEndianImplicit;
    }
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--verify"))
    {
      opt_operation = DSO_verify;
    }
    if (cmd.findOption("--sign"))
    {
      opt_operation = DSO_sign;
      if (opt_ofname == NULL) app.printError("parameter dcmfile-out required for --sign");
      app.checkValue(cmd.getValue(opt_keyfile));
      app.checkValue(cmd.getValue(opt_certfile));
    }
    if (cmd.findOption("--sign-item"))
    {
      opt_operation = DSO_signItem;
      if (opt_ofname == NULL) app.printError("parameter dcmfile-out required for --sign-item");
      app.checkValue(cmd.getValue(opt_keyfile));
      app.checkValue(cmd.getValue(opt_certfile));
      app.checkValue(cmd.getValue(opt_location));
    }
    if (cmd.findOption("--remove"))
    {
      opt_operation = DSO_remove;
      if (opt_ofname == NULL) app.printError("parameter dcmfile-out required for --remove");
      app.checkValue(cmd.getValue(opt_location));
    }
    if (cmd.findOption("--remove-all"))
    {
      opt_operation = DSO_removeAll;
      if (opt_ofname == NULL) app.printError("parameter dcmfile-out required for --remove-all");
    }
    cmd.endOptionBlock();

    if ((opt_operation == DSO_verify) && opt_ofname) app.printError("parameter dcmfile-out not allowed for --verify");

    cmd.beginOptionBlock();
    if (cmd.findOption("--std-passwd"))
    {
      app.checkDependence("--std-passwd", "--sign or --sign-item", (opt_operation == DSO_sign) || (opt_operation == DSO_signItem));
      opt_passwd = NULL;
    }
    if (cmd.findOption("--use-passwd"))
    {
      app.checkDependence("--use-passwd", "--sign or --sign-item", (opt_operation == DSO_sign) || (opt_operation == DSO_signItem));
      app.checkValue(cmd.getValue(opt_passwd));
    }
    if (cmd.findOption("--null-passwd"))
    {
      app.checkDependence("--null-passwd", "--sign or --sign-item", (opt_operation == DSO_sign) || (opt_operation == DSO_signItem));
      opt_passwd = "";
    }
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--pem-keys")) opt_keyFileFormat = X509_FILETYPE_PEM;
    if (cmd.findOption("--der-keys")) opt_keyFileFormat = X509_FILETYPE_ASN1;
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--profile-none"))
    {
      app.checkDependence("--profile-none", "--sign or --sign-item", (opt_operation == DSO_sign) || (opt_operation == DSO_signItem));
      opt_profile = new SiNullProfile();
    }
    if (cmd.findOption("--profile-base"))
    {
      app.checkDependence("--profile-base", "--sign or --sign-item", (opt_operation == DSO_sign) || (opt_operation == DSO_signItem));
      opt_profile = new SiBaseRSAProfile();
    }
    if (cmd.findOption("--profile-creator"))
    {
      app.checkDependence("--profile-creator", "--sign or --sign-item", (opt_operation == DSO_sign) || (opt_operation == DSO_signItem));
      opt_profile = new SiCreatorProfile();
    }
    if (cmd.findOption("--profile-auth"))
    {
      app.checkDependence("--profile-auth", "--sign or --sign-item", (opt_operation == DSO_sign) || (opt_operation == DSO_signItem));
      opt_profile = new SiAuthorizationProfile();
    }
    cmd.endOptionBlock();
    if (opt_profile == NULL) opt_profile = new SiNullProfile();

    cmd.beginOptionBlock();
    if (cmd.findOption("--mac-ripemd160"))
    {
      app.checkDependence("--mac-ripemd160", "--sign or --sign-item", (opt_operation == DSO_sign) || (opt_operation == DSO_signItem));
      opt_mac = new SiRIPEMD160();
    }
    if (cmd.findOption("--mac-sha1"))
    {
      app.checkDependence("--mac-sha1", "--sign or --sign-item", (opt_operation == DSO_sign) || (opt_operation == DSO_signItem));
      opt_mac = new SiSHA1();
    }
    if (cmd.findOption("--mac-md5"))
    {
      app.checkDependence("--mac-md5", "--sign or --sign-item", (opt_operation == DSO_sign) || (opt_operation == DSO_signItem));
      opt_mac = new SiMD5();
    }
    if (cmd.findOption("--mac-sha256"))
    {
      app.checkDependence("--mac-sha256", "--sign or --sign-item", (opt_operation == DSO_sign) || (opt_operation == DSO_signItem));
      opt_mac = new SiSHA256();
    }
    if (cmd.findOption("--mac-sha384"))
    {
      app.checkDependence("--mac-sha384", "--sign or --sign-item", (opt_operation == DSO_sign) || (opt_operation == DSO_signItem));
      opt_mac = new SiSHA384();
    }
    if (cmd.findOption("--mac-sha512"))
    {
      app.checkDependence("--mac-sha512", "--sign or --sign-item", (opt_operation == DSO_sign) || (opt_operation == DSO_signItem));
      opt_mac = new SiSHA512();
    }

    cmd.endOptionBlock();
    if (opt_mac == NULL) opt_mac = new SiRIPEMD160();

    if (cmd.findOption("--tag-file"))
    {
      app.checkValue(cmd.getValue(opt_tagFile));
      opt_tagList = new DcmAttributeTag(DCM_DataElementsSigned);
      result = parseTextFile(opt_tagFile, *opt_tagList);
      if (result > 0)
      {
        OFLOG_FATAL(dcmsignLogger, "Error while reading tag file '" << opt_tagFile << "', giving up.");
        return result;
      }
    }

    if (cmd.findOption("--tag", 0, OFCommandLine::FOM_First))
    {
      const char *current=NULL;
      if (opt_tagList == NULL) opt_tagList = new DcmAttributeTag(DCM_DataElementsSigned);
      do
      {
        app.checkValue(cmd.getValue(current));
        if (! addTag(current, *opt_tagList))
        {
          OFLOG_FATAL(dcmsignLogger, "unknown attribute tag '" << current << "'");
          return 10;
        }
      } while (cmd.findOption("--tag", 0, OFCommandLine::FOM_Next));
    }

    cmd.beginOptionBlock();
    if (cmd.findOption("--format-new")) dcmEnableOldSignatureFormat.set(OFFalse);
    if (cmd.findOption("--format-old")) dcmEnableOldSignatureFormat.set(OFTrue);
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--write-xfer-same")) opt_oxfer = EXS_Unknown;
    if (cmd.findOption("--write-xfer-little")) opt_oxfer = EXS_LittleEndianExplicit;
    if (cmd.findOption("--write-xfer-big")) opt_oxfer = EXS_BigEndianExplicit;
    if (cmd.findOption("--write-xfer-implicit")) opt_oxfer = EXS_LittleEndianImplicit;
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--length-explicit")) opt_oenctype = EET_ExplicitLength;
    if (cmd.findOption("--length-undefined")) opt_oenctype = EET_UndefinedLength;
    cmd.endOptionBlock();

    if (cmd.findOption("--dump"))
    {
      app.checkDependence("--dump", "--sign or --sign-item", (opt_operation == DSO_sign) || (opt_operation == DSO_signItem));
      const char *fileName = NULL;
      app.checkValue(cmd.getValue(fileName));
      opt_dumpFile = fopen(fileName, "wb");
      if (opt_dumpFile == NULL)
      {
        OFLOG_FATAL(dcmsignLogger, "unable to create dump file '" << fileName << "'");
        return 10;
      }
    }
  }

  /* print resource identifier */
  OFLOG_DEBUG(dcmsignLogger, rcsid << OFendl);

  /* make sure data dictionary is loaded */
  if (!dcmDataDict.isDictionaryLoaded())
  {
    OFLOG_WARN(dcmsignLogger, "no data dictionary loaded, "
      << "check environment variable: " << DCM_DICT_ENVIRONMENT_VARIABLE);
  }

  // open inputfile
  if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
  {
    OFLOG_FATAL(dcmsignLogger, "invalid filename: <empty string>");
    return 1;
  }

  OFLOG_INFO(dcmsignLogger, "open input file " << opt_ifname);

  DcmFileFormat *fileformat = new DcmFileFormat;

  OFCondition sicond = fileformat->loadFile(opt_ifname, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_readMode);
  if (sicond.bad())
  {
    OFLOG_FATAL(dcmsignLogger, sicond.text() << ": reading file: " << opt_ifname);
    return 1;
  }

  DcmDataset *dataset = fileformat->getDataset();

  SiCertificate cert;
  SiPrivateKey key;

  if (opt_certfile)
  {
    sicond = cert.loadCertificate(opt_certfile, opt_keyFileFormat);
    if (sicond != EC_Normal)
    {
      OFLOG_FATAL(dcmsignLogger, sicond.text() << ": while loading certificate file '" << opt_certfile << "'");
      return 1;
    }
  }

  if (opt_keyfile)
  {
    if (opt_passwd) key.setPrivateKeyPasswd(opt_passwd);
    sicond = key.loadPrivateKey(opt_keyfile, opt_keyFileFormat);
    if (sicond != EC_Normal)
    {
      OFLOG_FATAL(dcmsignLogger, sicond.text() << ": while loading private key file '" << opt_keyfile << "'");
      return 1;
    }
  }

  // need to load all data into memory before signing the document,
  // otherwise the pixel data would be empty for compressed images
  fileformat->loadAllDataIntoMemory();

  // select transfer syntax in which digital signatures are created
  opt_signatureXfer = dataset->getOriginalXfer();

  // use Little Endian Explicit for uncompressed files
  if ((opt_signatureXfer == EXS_LittleEndianImplicit) ||
      (opt_signatureXfer == EXS_BigEndianExplicit))
     opt_signatureXfer = EXS_LittleEndianExplicit;

  // now do the real work
  switch (opt_operation)
  {
    case DSO_verify:
      OFLOG_INFO(dcmsignLogger, "verifying all signatures.");
      result = do_verify(dataset);
      if (result != 0) return result;
      break;
    case DSO_sign:
      OFLOG_INFO(dcmsignLogger, "create signature in main object.");
      result = do_sign(dataset, key, cert, opt_mac, opt_profile, opt_tagList, opt_signatureXfer, opt_dumpFile);
      if (result != 0) return result;
      break;
    case DSO_signItem:
      OFLOG_INFO(dcmsignLogger, "create signature in sequence item.");
      result = do_sign_item(dataset, key, cert, opt_mac, opt_profile, opt_tagList, opt_location, opt_signatureXfer, opt_dumpFile);
      if (result != 0) return result;
      break;
    case DSO_remove:
      OFLOG_INFO(dcmsignLogger, "removing signature from sequence item.");
      result = do_remove(dataset, opt_location);
      if (result != 0) return result;
      break;
    case DSO_removeAll:
      OFLOG_INFO(dcmsignLogger, "removing all signatures.");
      result = do_remove_all(dataset);
      if (result != 0) return result;
      break;
  }

  if (opt_dumpFile)
  {
    if (0 != fclose(opt_dumpFile))
    {
      OFLOG_FATAL(dcmsignLogger, "Error while closing dump file, content may be incomplete.");
    }
    opt_dumpFile = NULL;
  }

  if (opt_ofname)
  {
    OFLOG_INFO(dcmsignLogger, "create output file " << opt_ofname);

    if (opt_oxfer == EXS_Unknown) opt_oxfer = dataset->getOriginalXfer();
    DcmXfer opt_oxferSyn(opt_oxfer);
    dataset->chooseRepresentation(opt_oxfer, NULL);
    if (! dataset->canWriteXfer(opt_oxfer))
    {
      OFLOG_FATAL(dcmsignLogger, "No conversion to transfer syntax " << opt_oxferSyn.getXferName() << " possible!");
      return 1;
    }

    sicond = fileformat->saveFile(opt_ofname, opt_oxfer, opt_oenctype, opt_oglenc, opt_opadenc, OFstatic_cast(Uint32, opt_filepad), OFstatic_cast(Uint32, opt_itempad));
    if (sicond.bad())
    {
      OFLOG_FATAL(dcmsignLogger, sicond.text() << ": writing file: " <<  opt_ofname);
      return 1;
    }
  }

  delete opt_mac;
  delete opt_profile;
  delete opt_tagList;
  return 0;
}

#else /* WITH_OPENSSL */

int main(int, char *[])
{
  CERR << rcsid << OFendl << APPLICATION_ABSTRACT << OFendl << OFendl
       << OFFIS_CONSOLE_APPLICATION " requires the OpenSSL library." << OFendl
       << "This " OFFIS_CONSOLE_APPLICATION " has been configured and compiled without OpenSSL." << OFendl
       << "Please reconfigure your system and recompile if appropriate." << OFendl;
  return 0;
}

#endif /* WITH_OPENSSL */
