/*
 *
 *  Copyright (C) 2000-2004, OFFIS
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
 *  Module: dcmsign
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose: Create and Verify DICOM Digital Signatures
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2004-02-10 17:01:02 $
 *  CVS/RCS Revision: $Revision: 1.17 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CCTYPE
#include "ofstdinc.h"

#ifdef HAVE_GUSI_H
#include <GUSI.h>
#endif

#include "cmdlnarg.h"
#include "ofconapp.h"
#include "dcuid.h"         /* for dcmtk version name */

#ifdef WITH_ZLIB
#include <zlib.h>         /* for zlibVersion() */
#endif

#define OFFIS_CONSOLE_APPLICATION "dcmsign"

static char rcsid[] = "$dcmtk: " OFFIS_CONSOLE_APPLICATION " v"
  OFFIS_DCMTK_VERSION " " OFFIS_DCMTK_RELEASEDATE " $";

#define APPLICATION_ABSTRACT "Sign and Verify DICOM Files"


#ifdef WITH_OPENSSL

#include "dcsignat.h"
#include "sinullpr.h"
#include "sibrsapr.h"
#include "siautopr.h"
#include "sicreapr.h"
#include "simac.h"
#include "simd5.h"
#include "sisha1.h"
#include "siripemd.h"
#include "siprivat.h"
#include "sicert.h"
#include "dctk.h"
#include "dcdebug.h"


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
  while(isspace(c[lpos])) ++lpos; // ignore leading space

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
    idx = (Uint32)newindex;
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
  CERR << "error: attribute name '" << aString.c_str() << "' unknown." << endl;
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
      CERR << "warning: text file too large, ignoring everything beyond 64K." << endl;
      numBytes = 65536;
    }
    result = new char[numBytes];
    if (result)
    {
      if ((size_t)numBytes != fread(result, 1, (size_t)numBytes, file))
      {
        CERR << "warning: read error in file " << filename << endl;
        delete[] result;
        result = NULL;
      }
    }
    fclose(file);
  } else {
    CERR << "error: file not found: " << filename << endl;
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
      CERR << "parse error in text file '" << filename << "'" << endl;
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
      CERR << "parse error in item location string '" << location << "'" << endl;
      return NULL;
    }
    if (token == -1)
    {
      if (! finished)
      {
      	CERR << "error: item location string '" << location << "' incomplete." << endl;
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
      	CERR << "error: attribute " << key << " not found in dataset (item location string is '" << location << "')" << endl;
        return NULL;
      }
      if (stack.top()->ident() == EVR_SQ)
      {
        sq = (DcmSequenceOfItems *)(stack.top());
      } else {
        CERR << "error: attribute " << key << " is not a sequence (item location string is '" << location << "')" << endl;
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
        CERR << "error: sequence not found in item location string '" << location << "'" <<endl;
        return NULL;
      }
      if (idx >= sq->card())
      {
        CERR << "error: sequence " << sq->getTag() << " only has " << sq->card() << " items, cannot locate item " << idx
             << " (item location string is '" << location << "')" <<endl;
        return NULL;
      }
      result = sq->getItem(idx);
      if (result == NULL)
      {
        CERR << "error: item not found in item location string '" << location << "'" <<endl;
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
  E_TransferSyntax opt_signatureXfer)
{
  OFCondition sicond = EC_Normal;
  DcmSignature signer;
  signer.attach(dataset);
  sicond = signer.createSignature(key, cert, *opt_mac, *opt_profile, opt_signatureXfer, opt_tagList);
  if (sicond != EC_Normal)
  {
    CERR << "Error: " << sicond.text() << ": while creating signature in main dataset" << endl;
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
          sq = (DcmSequenceOfItems *)elem;
          DcmTag currentTag(elem->getTag());
          tagname = currentTag.getTagName();
          if (tagname) str.append(tagname); else
          {
            sprintf(buf, "(%04x,%04x)", elem->getTag().getGroup(), elem->getTag().getElement());
            str.append(buf);
          }
          if (elem->ident() == EVR_SQ) sq = (DcmSequenceOfItems *)elem; else sq = NULL;
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
  E_TransferSyntax opt_signatureXfer)
{
  OFCondition sicond = EC_Normal;
  DcmSignature signer;
  DcmItem *sigItem = locateItemforSignatureCreation(*dataset, opt_location);
  if (sigItem == NULL) return 1;

  signer.detach();
  signer.attach(sigItem);
  sicond = signer.createSignature(key, cert, *opt_mac, *opt_profile, opt_signatureXfer, opt_tagList);
  if (sicond != EC_Normal)
  {
    CERR << "Error: " << sicond.text() << ": while creating signature in item '" << opt_location << "'" << endl;
    return 1;
  }
  signer.detach();
  return 0;
}

/* verify all signatures in the given dataset and print results to COUT.
 * @param dataset dataset to verify
 * @param opt_verbose verbose mode flag
 * @return 0 if successful, a program exit code otherwise
 */
static int do_verify(
  DcmItem *dataset,
  OFBool opt_verbose)
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
        COUT <<   "Signature #" << counter << " UID=";
        if (EC_Normal == signer.getCurrentSignatureUID(aString)) COUT << aString.c_str() << endl; else COUT << "(unknown)" << endl;
        printSignatureItemPosition(stack, aString);
        COUT <<   "  Location                    : " << aString.c_str() << endl;
        if (opt_verbose)
        {
          COUT << "  MAC ID                      : ";
          if (EC_Normal == signer.getCurrentMacID(macID)) COUT << macID << endl; else COUT << "(unknown)" << endl;
          COUT << "  MAC algorithm               : ";
          if (EC_Normal == signer.getCurrentMacName(aString)) COUT << aString.c_str() << endl; else COUT << "(unknown)" << endl;
          COUT << "  MAC calculation xfer syntax : ";
          if (EC_Normal == signer.getCurrentMacXferSyntaxName(aString)) COUT << aString.c_str() << endl; else COUT << "(unknown)" << endl;
          // data elements signed
          COUT << "  Data elements signed        : ";
          if (EC_Normal == signer.getCurrentDataElementsSigned(at))
          {
            COUT << endl;
            unsigned long atVM = at.getVM();
            for (unsigned long n=0; n<atVM; n++)
            {
              if (EC_Normal == at.getTagVal(tagkey, n))
              {
              	COUT << "      " << tagkey << " ";
              	tag = tagkey;
              	tagName = tag.getTagName();
              	if (tagName) COUT << tagName << endl; else COUT << endl;
              }
            }
          } else COUT << "all elements" << endl;

          COUT << "  Signature date/time         : ";
          if (EC_Normal == signer.getCurrentSignatureDateTime(aString)) COUT << aString.c_str() << endl; else COUT << "(unknown)" << endl;
          COUT << "  Certificate of signer       : ";
          SiCertificate *cert = signer.getCurrentCertificate();
          if ((cert == NULL)||(cert->getKeyType()==EKT_none)) COUT << "none" << endl; else
          {
            COUT << "X.509v" << cert->getX509Version() << endl;
            cert->getCertSubjectName(aString);
            COUT << "      Subject                 : " << aString.c_str() << endl;

            cert->getCertIssuerName(aString);
            COUT << "      Issued by               : " << aString.c_str() << endl
                 << "      Serial no.              : " << cert->getCertSerialNo() << endl
                 << "      Validity                : not before ";
            cert->getCertValidityNotBefore(aString);
            COUT << aString.c_str() << ", not after ";
            cert->getCertValidityNotAfter(aString);
            COUT << aString.c_str() << endl
                 << "      Public key              : ";
            switch (cert->getKeyType())
            {
              case EKT_RSA:
                COUT << "RSA, " << cert->getCertKeyBits() << " bits" << endl;
                break;
              case EKT_DSA:
                COUT << "DSA, " << cert->getCertKeyBits() << " bits" << endl;
                break;
              case EKT_DH:
                COUT << "DH, " << cert->getCertKeyBits() << " bits" << endl;
                break;
              case EKT_none: // should never happen
                COUT << "none" << endl;
                break;
            }
          }
        }
        COUT << "  Verification : ";
        sicond = signer.verifyCurrent();
        if (sicond.good())
        {
          COUT << "OK" << endl << endl;
        } else {
          corrupt_counter++;
          COUT << sicond.text() << endl << endl;
	}
      }
    }
    signer.detach();
    sigItem = DcmSignature::findNextSignatureItem(*dataset, stack);
  }
  if (counter == 0) COUT << "no signatures found in dataset." << endl;
  else if (opt_verbose) COUT << counter << " signatures verified in dataset, " << corrupt_counter << " corrupted." << endl;
  return 0;
}

/* remove all signatures from the given dataset, print action details.
 * @param dataset dataset to modify
 * @param opt_verbose verbose mode flag
 * @return 0 if successful, a program exit code otherwise
 */
static int do_remove_all(
  DcmItem *dataset,
  OFBool opt_verbose)
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
        COUT <<   "Signature #" << counter << " UID=";
        if (EC_Normal == signer.getCurrentSignatureUID(aString)) COUT << aString.c_str() << endl; else COUT << "(unknown)" << endl;
        printSignatureItemPosition(stack, aString);
        COUT <<   "  Location                    : " << aString.c_str() << endl;
      }
      sicond = signer.removeSignature(0);
      if (sicond != EC_Normal)
      {
        CERR << "Error: " << sicond.text() << ": while removing signature" << endl;
        return 1;
      }
    }
    signer.detach();
    stack.pop(); // remove pointer to the Digital Signatures Sequence that we've just deleted.
    sigItem = DcmSignature::findNextSignatureItem(*dataset, stack);
  }

  if (opt_verbose) COUT << counter << " signatures found and removed from dataset." << endl;
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
            COUT <<   "Signature UID=" << aString.c_str() << endl;
            printSignatureItemPosition(stack, aString);
            COUT <<   "  Location                    : " << aString.c_str() << endl;

            sicond = signer.removeSignature(i);
            if (sicond != EC_Normal)
            {
              CERR << "Error: " << sicond.text() << ": while removing signature" << endl;
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
  CERR << "Error: signature with UID '" << opt_location << "' not found." << endl;
  return 1;
}


#define SHORTCOL 4
#define LONGCOL 21

int main(int argc, char *argv[])
{

#ifdef HAVE_GUSI_H
  GUSISetup(GUSIwithSIOUXSockets);
  GUSISetup(GUSIwithInternetSockets);
#endif

  SetDebugLevel(( 0 ));
  DcmSignature::initializeLibrary(); // initialize dcmsign

  const char *                  opt_certfile = NULL;
  int                           opt_debugMode = 0;
  OFCmdUnsignedInt              opt_filepad = 0;
  OFBool                        opt_iDataset = OFFalse;
  const char *                  opt_ifname = NULL;
  OFCmdUnsignedInt              opt_itempad = 0;
  E_TransferSyntax              opt_ixfer = EXS_Unknown;
  const char *                  opt_keyfile = NULL;  // private key file
  int                           opt_keyFileFormat = X509_FILETYPE_PEM; // file format for certificates and private keys
  const char *                  opt_location = NULL; // location (path) within dataset
  SiMAC *                       opt_mac = NULL; // MAC object
  OFBool                        opt_oDataset = OFFalse;
  E_EncodingType                opt_oenctype = EET_ExplicitLength;
  const char *                  opt_ofname = NULL;
  E_GrpLenEncoding              opt_oglenc = EGL_recalcGL;
  E_PaddingEncoding             opt_opadenc = EPD_noChange;
  DcmSignOperation              opt_operation = DSO_verify; // command to execute
  E_TransferSyntax              opt_oxfer = EXS_Unknown;
  const char *                  opt_passwd = NULL; // password for private key
  SiSecurityProfile *           opt_profile = NULL; // security profile
  const char *                  opt_tagFile = NULL; // text file with attribute tags
  DcmAttributeTag *             opt_tagList = NULL; // list of attribute tags
  OFBool                        opt_verbose = OFFalse;
  E_TransferSyntax              opt_signatureXfer = EXS_Unknown;
  int result = 0;

  OFConsoleApplication app(OFFIS_CONSOLE_APPLICATION , APPLICATION_ABSTRACT, rcsid);
  OFCommandLine cmd;
  cmd.setOptionColumns(LONGCOL, SHORTCOL);
  cmd.setParamColumn(LONGCOL + SHORTCOL + 4);

  cmd.addParam("dcmfile-in",  "DICOM input filename to be processed");
  cmd.addParam("dcmfile-out", "DICOM output filename", OFCmdParam::PM_Optional);

  cmd.addGroup("general options:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--help",                      "-h",        "print this help text and exit");
      cmd.addOption("--version",                                "print version information and exit", OFTrue /* exclusive */);
      cmd.addOption("--verbose",                   "-v",        "verbose mode, print processing details");
      cmd.addOption("--debug",                     "-d",        "debug mode, print debug information");

  cmd.addGroup("input options:");
    cmd.addSubGroup("input file format:");
      cmd.addOption("--read-file",                 "+f",        "read file format or data set (default)");
      cmd.addOption("--read-dataset",              "-f",        "read data set without file meta information");
    cmd.addSubGroup("input transfer syntax (only with --read-dataset):", LONGCOL, SHORTCOL);
      cmd.addOption("--read-xfer-auto",            "-t=",       "use TS recognition (default)");
      cmd.addOption("--read-xfer-little",          "-te",       "read with explicit VR little endian TS");
      cmd.addOption("--read-xfer-big",             "-tb",       "read with explicit VR big endian TS");
      cmd.addOption("--read-xfer-implicit",        "-ti",       "read with implicit VR little endian TS");

  cmd.addGroup("signature commands:", LONGCOL, SHORTCOL + 2);
      cmd.addOption("--verify",                                 "verify all signatures (default)");
      cmd.addOption("--sign",                      "+s",     2, "private key file, certificate file: string",
                                                                "create signature in main object");
      cmd.addOption("--sign-item",                 "+si",    3, "keyfile, certfile, item location: string",
                                                                "create signature in sequence item");
      cmd.addOption("--remove",                    "+r",     1, "signature UID: string", "remove signature");
      cmd.addOption("--remove-all",                "+ra",       "remove all signatures from data set");

  cmd.addGroup("signature creation options (only with --sign or --sign-item):");
    cmd.addSubGroup("private key password options:");
      cmd.addOption("--std-passwd",               "+ps",        "prompt user to type password on stdin (default)");
      cmd.addOption("--use-passwd",               "+pw",    1,  "[p]assword: string ",
                                                                "use specified password");
      cmd.addOption("--null-passwd",              "-pw",        "use empty string as password");
    cmd.addSubGroup("key and certificate file format options:");
      cmd.addOption("--pem-keys",                 "-pem",       "read keys/certificates as PEM file (default)");
      cmd.addOption("--der-keys",                 "-der",       "read keys/certificates as DER file");
    cmd.addSubGroup("digital signature profile options:");
      cmd.addOption("--profile-none",             "-pf",        "don't enforce any signature profile (default)");
      cmd.addOption("--profile-base",             "+pb",        "enforce base RSA signature profile");
      cmd.addOption("--profile-creator",          "+pc",        "enforce creator RSA signature profile");
      cmd.addOption("--profile-auth",             "+pa",        "enforce authorization signature profile");
    cmd.addSubGroup("MAC algorithm options:");
      cmd.addOption("--mac-ripemd160",            "+mr",        "use RIPEMD 160 (default)");
      cmd.addOption("--mac-sha1",                 "+ms",        "use SHA-1");
      cmd.addOption("--mac-md5",                  "+mm",        "use MD 5");
    cmd.addSubGroup("tag selection options:");
      cmd.addOption("--tag",                      "-t",      1, "tag: \"xxxx,xxxx\" or a data dictionary name", "sign only specified tag\nthis option can be specified multiple times");
      cmd.addOption("--tag-file",                 "-tf",     1, "filename: string", "read list of tags from text file");
  cmd.addGroup("output options:");
    cmd.addSubGroup("output transfer syntax:");
      cmd.addOption("--write-xfer-same",          "+t=",        "write with same TS as input (default)");
      cmd.addOption("--write-xfer-little",        "+te",        "write with explicit VR little endian TS");
      cmd.addOption("--write-xfer-big",           "+tb",        "write with explicit VR big endian TS");
      cmd.addOption("--write-xfer-implicit",      "+ti",        "write with implicit VR little endian TS");
    cmd.addSubGroup("length encoding in sequences and items:");
      cmd.addOption("--length-explicit",          "+e",         "write with explicit lengths (default)");
      cmd.addOption("--length-undefined",         "-e",         "write with undefined lengths");

  /* evaluate command line */
  prepareCmdLineArgs(argc, argv, OFFIS_CONSOLE_APPLICATION);
  if (app.parseCommandLine(cmd, argc, argv, OFCommandLine::ExpandWildcards))
  {
    /* check exclusive options first */

    if (cmd.getParamCount() == 0)
    {
        if (cmd.findOption("--version"))
        {
            app.printHeader(OFTrue /*print host identifier*/);          // uses ofConsole.lockCerr()
            CERR << endl << "External libraries used:";
#if !defined(WITH_ZLIB) && !defined(WITH_OPENSSL)
            CERR << " none" << endl;
#else
            CERR << endl;
#endif
#ifdef WITH_ZLIB
            CERR << "- ZLIB, Version " << zlibVersion() << endl;
#endif
#ifdef WITH_OPENSSL
            CERR << "- " << OPENSSL_VERSION_TEXT << endl;
#endif
            return 0;
        }
    }

    /* command line parameters */

    cmd.getParam(1, opt_ifname);
    if (cmd.getParamCount() > 1) cmd.getParam(2, opt_ofname);

    if (cmd.findOption("--verbose")) opt_verbose = OFTrue;
    if (cmd.findOption("--debug")) opt_debugMode = 5;

    cmd.beginOptionBlock();
    if (cmd.findOption("--read-file")) opt_iDataset = OFFalse;
    if (cmd.findOption("--read-dataset")) opt_iDataset = OFTrue;
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--read-xfer-auto"))
    {
      if (! opt_iDataset) app.printError("--read-xfer-auto only allowed with --read-dataset");
      opt_ixfer = EXS_Unknown;
    }
    if (cmd.findOption("--read-xfer-little"))
    {
      if (! opt_iDataset) app.printError("--read-xfer-little only allowed with --read-dataset");
      opt_ixfer = EXS_LittleEndianExplicit;
    }
    if (cmd.findOption("--read-xfer-big"))
    {
      if (! opt_iDataset) app.printError("--read-xfer-big only allowed with --read-dataset");
      opt_ixfer = EXS_BigEndianExplicit;
    }
    if (cmd.findOption("--read-xfer-implicit"))
    {
      if (! opt_iDataset) app.printError("--read-xfer-implicit only allowed with --read-dataset");
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
      if ((opt_operation != DSO_sign)&&(opt_operation != DSO_signItem)) app.printError("--std-passwd only with --sign or --sign-item");
      opt_passwd = NULL;
    }
    if (cmd.findOption("--use-passwd"))
    {
      if ((opt_operation != DSO_sign)&&(opt_operation != DSO_signItem)) app.printError("--use-passwd only with --sign or --sign-item");
      app.checkValue(cmd.getValue(opt_passwd));
    }
    if (cmd.findOption("--null-passwd"))
    {
      if ((opt_operation != DSO_sign)&&(opt_operation != DSO_signItem)) app.printError("--null-passwd only with --sign or --sign-item");
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
      if ((opt_operation != DSO_sign)&&(opt_operation != DSO_signItem)) app.printError("--profile-none only with --sign or --sign-item");
      opt_profile = new SiNullProfile();
    }
    if (cmd.findOption("--profile-base"))
    {
      if ((opt_operation != DSO_sign)&&(opt_operation != DSO_signItem)) app.printError("--profile-base only with --sign or --sign-item");
      opt_profile = new SiBaseRSAProfile();
    }
    if (cmd.findOption("--profile-creator"))
    {
      if ((opt_operation != DSO_sign)&&(opt_operation != DSO_signItem)) app.printError("--profile-creator only with --sign or --sign-item");
      opt_profile = new SiCreatorProfile();
    }
    if (cmd.findOption("--profile-auth"))
    {
      if ((opt_operation != DSO_sign)&&(opt_operation != DSO_signItem)) app.printError("--profile-auth only with --sign or --sign-item");
      opt_profile = new SiAuthorizationProfile();
    }
    cmd.endOptionBlock();
    if (opt_profile == NULL) opt_profile = new SiNullProfile();

    cmd.beginOptionBlock();
    if (cmd.findOption("--mac-ripemd160"))
    {
      if ((opt_operation != DSO_sign)&&(opt_operation != DSO_signItem)) app.printError("--mac-ripemd160 only with --sign or --sign-item");
      opt_mac = new SiRIPEMD160();
    }
    if (cmd.findOption("--mac-sha1"))
    {
      if ((opt_operation != DSO_sign)&&(opt_operation != DSO_signItem)) app.printError("--mac-sha1 only with --sign or --sign-item");
      opt_mac = new SiSHA1();
    }
    if (cmd.findOption("--mac-md5"))
    {
      if ((opt_operation != DSO_sign)&&(opt_operation != DSO_signItem)) app.printError("--mac-md5 only with --sign or --sign-item");
      opt_mac = new SiMD5();
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
      	CERR << "error while reading tag file '" << opt_tagFile << "', giving up." << endl;
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
          CERR << "error: unknown attribute tag '" << current << "'" << endl;
          return 10;
        }
      } while (cmd.findOption("--tag", 0, OFCommandLine::FOM_Next));
    }

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

  }

  SetDebugLevel((opt_debugMode));

  /* make sure data dictionary is loaded */
  if (!dcmDataDict.isDictionaryLoaded())
  {
    CERR << "Warning: no data dictionary loaded, "
         << "check environment variable: "
         << DCM_DICT_ENVIRONMENT_VARIABLE << endl;
  }

  // open inputfile
  if ((opt_ifname == NULL) || (strlen(opt_ifname) == 0))
  {
      CERR << "invalid filename: <empty string>" << endl;
      return 1;
  }

  if (opt_verbose) COUT << "open input file " << opt_ifname << endl;

  DcmFileFormat *fileformat = new DcmFileFormat;
  DcmDataset *dataset = dataset = fileformat->getDataset();

  OFCondition sicond = fileformat->loadFile(opt_ifname, opt_ixfer, EGL_noChange, DCM_MaxReadLength, opt_iDataset);
  if (sicond.bad())
  {
    CERR << "Error: " << sicond.text() << ": reading file: " <<  opt_ifname << endl;
    return 1;
  }

  SiCertificate cert;
  SiPrivateKey key;

  if (opt_certfile)
  {
    sicond = cert.loadCertificate(opt_certfile, opt_keyFileFormat);
    if (sicond != EC_Normal)
    {
      CERR << "Error: " << sicond.text() << ": while loading certificate file '" << opt_certfile << "'" << endl;
      return 1;
    }
  }

  if (opt_keyfile)
  {
    if (opt_passwd) key.setPrivateKeyPasswd(opt_passwd);
    sicond = key.loadPrivateKey(opt_keyfile, opt_keyFileFormat);
    if (sicond != EC_Normal)
    {
      CERR << "Error: " << sicond.text() << ": while loading private key file '" << opt_keyfile << "'" << endl;
      return 1;
    }
  }

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
      if (opt_verbose) COUT << "verifying all signatures." << endl;
      result = do_verify(dataset, opt_verbose);
      if (result != 0) return result;
      break;
    case DSO_sign:
      if (opt_verbose) COUT << "create signature in main object." << endl;
      result = do_sign(dataset, key, cert, opt_mac, opt_profile, opt_tagList, opt_signatureXfer);
      if (result != 0) return result;
      break;
    case DSO_signItem:
      if (opt_verbose) COUT << "create signature in sequence item." << endl;
      result = do_sign_item(dataset, key, cert, opt_mac, opt_profile, opt_tagList, opt_location, opt_signatureXfer);
      if (result != 0) return result;
      break;
    case DSO_remove:
      if (opt_verbose) COUT << "removing signature from sequence item." << endl;
      result = do_remove(dataset, opt_location);
      if (result != 0) return result;
      break;
    case DSO_removeAll:
      if (opt_verbose) COUT << "removing all signatures." << endl;
      result = do_remove_all(dataset, opt_verbose);
      if (result != 0) return result;
      break;
  }

  if (opt_ofname)
  {
    if (opt_verbose) COUT << "create output file " << opt_ofname << endl;

    if (opt_oxfer == EXS_Unknown) opt_oxfer = dataset->getOriginalXfer();
    DcmXfer opt_oxferSyn(opt_oxfer);
    dataset->chooseRepresentation(opt_oxfer, NULL);
    if (! dataset->canWriteXfer(opt_oxfer))
    {
      CERR << "No conversion to transfer syntax " << opt_oxferSyn.getXferName() << " possible!\n";
      return 1;
    }

    sicond = fileformat->saveFile(opt_ofname, opt_oxfer, opt_oenctype, opt_oglenc, opt_opadenc, (Uint32) opt_filepad, (Uint32) opt_itempad, opt_oDataset);
    if (sicond.bad())
    {
      CERR << "Error: " << sicond.text() << ": writing file: " <<  opt_ofname << endl;
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
  CERR << rcsid << endl << APPLICATION_ABSTRACT << endl << endl
       << OFFIS_CONSOLE_APPLICATION " requires the OpenSSL library." << endl
       << "This " OFFIS_CONSOLE_APPLICATION " has been configured and compiled without OpenSSL." << endl
       << "Please reconfigure your system and recompile if appropriate." << endl;
  return 0;
}

#endif /* WITH_OPENSSL */

/*
 *  $Log: dcmsign.cc,v $
 *  Revision 1.17  2004-02-10 17:01:02  joergr
 *  Updated copyright header.
 *
 *  Revision 1.16  2002/12/20 14:54:09  wilkens
 *  Modified name clash resulting in a compiler error on Solaris 2.5.1 using
 *  compiler SC 2.0.1.
 *
 *  Revision 1.15  2002/11/27 14:53:38  meichel
 *  Adapted module dcmsign to use of new header file ofstdinc.h
 *
 *  Revision 1.14  2002/11/26 08:45:20  meichel
 *  Replaced all includes for "zlib.h" with <zlib.h>
 *    to avoid inclusion of zlib.h in the makefile dependencies.
 *
 *  Revision 1.13  2002/09/23 19:06:10  joergr
 *  Fixed typo in pre-processor directive.
 *
 *  Revision 1.12  2002/09/23 18:18:25  joergr
 *  Added new command line option "--version" which prints the name and version
 *  number of external libraries used (incl. preparation for future support of
 *  'config.guess' host identifiers).
 *
 *  Revision 1.11  2002/08/20 12:22:24  meichel
 *  Adapted code to new loadFile and saveFile methods, thus removing direct
 *    use of the DICOM stream classes.
 *
 *  Revision 1.10  2001/12/10 16:44:22  meichel
 *  Fixed warnings from Sun CC 4.2
 *
 *  Revision 1.9  2001/11/16 15:50:45  meichel
 *  Adapted digital signature code to final text of supplement 41.
 *
 *  Revision 1.8  2001/09/28 14:00:49  joergr
 *  Replaced "cout" by "COUT".
 *
 *  Revision 1.7  2001/09/26 14:30:16  meichel
 *  Adapted dcmsign to class OFCondition
 *
 *  Revision 1.6  2001/06/01 15:50:45  meichel
 *  Updated copyright header
 *
 *  Revision 1.5  2001/05/25 10:08:56  meichel
 *  Added warning to dcmsign application that current version is incompatible
 *    to final text of Supplement 41.
 *
 *  Revision 1.4  2000/11/09 11:55:40  joergr
 *  Corrected typo.
 *
 *  Revision 1.3  2000/11/08 11:20:13  meichel
 *  Fixed dcmsign command line options
 *
 *  Revision 1.2  2000/11/07 18:06:20  joergr
 *  Minor code purifications to keep Sun CC 2.0.1 quiet.
 *  Moved #ifdef WITH_OPENSSL statement to avoid compiler errors.
 *
 *  Revision 1.1  2000/11/07 16:48:48  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */
