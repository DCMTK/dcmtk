/*
 *
 *  Copyright (C) 2000-2019, OFFIS e.V.
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
#include "dcmtk/dcmsign/dcsighlp.h"
#include "dcmtk/dcmsign/sinullpr.h"
#include "dcmtk/dcmsign/sibrsapr.h"
#include "dcmtk/dcmsign/siautopr.h"
#include "dcmtk/dcmsign/sicreapr.h"
#include "dcmtk/dcmsign/sisrpr.h"
#include "dcmtk/dcmsign/sisrvpr.h"
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
  SiSignaturePurpose::E_SignaturePurposeType opt_sigPurpose = SiSignaturePurpose::ESP_none;
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
      cmd.addOption("--profile-sr",               "+pr",        "enforce SR RSA signature profile");
      cmd.addOption("--profile-srv" ,             "+pv",        "enforce SR RSA signature profile (verification)");
    cmd.addSubGroup("MAC algorithm:");
      cmd.addOption("--mac-ripemd160",            "+mr",        "use RIPEMD 160 (default)");
      cmd.addOption("--mac-sha1",                 "+ms",        "use SHA-1 (not recommended)");
      cmd.addOption("--mac-md5",                  "+mm",        "use MD5 (not recommended)");
      cmd.addOption("--mac-sha256",               "+m2",        "use SHA-256");
      cmd.addOption("--mac-sha384",               "+m3",        "use SHA-384");
      cmd.addOption("--mac-sha512",               "+m5",        "use SHA-512");
    cmd.addSubGroup("Signature purpose:");
      cmd.addOption("--list-purposes",            "+lp",        "show list of signature purpose codes and exit", OFCommandLine::AF_Exclusive);
      cmd.addOption("--no-sig-purpose",           "-sp",        "do not add signature purpose (default)");
      cmd.addOption("--sig-purpose",              "+sp",     1, "[p]urpose code (1..18)", "add digital signature purpose code p");
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
        if (cmd.findOption("--list-purposes"))
        {
            app.printHeader(OFTrue /*print host identifier*/);
            SiSignaturePurpose::printSignatureCodes(COUT);
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
    if (cmd.findOption("--profile-sr"))
    {
      app.checkDependence("--profile-sr", "--sign", (opt_operation == DSO_sign));
      opt_profile = new SiStructuredReportingProfile();
    }
    if (cmd.findOption("--profile-srv"))
    {
      app.checkDependence("--profile-srv", "--sign", (opt_operation == DSO_sign));
      opt_profile = new SiStructuredReportingVerificationProfile();
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

    cmd.beginOptionBlock();
    if (cmd.findOption("--no-sig-purpose"))
    {
      app.checkDependence("--no-sig-purpose", "--sign or --sign-item", (opt_operation == DSO_sign) || (opt_operation == DSO_signItem));
      opt_sigPurpose = SiSignaturePurpose::ESP_none;
    }
    if (cmd.findOption("--sig-purpose"))
    {
      app.checkDependence("--sig-purpose", "--sign or --sign-item", (opt_operation == DSO_sign) || (opt_operation == DSO_signItem));
      OFCmdUnsignedInt val = 0;
      app.checkValue(cmd.getValueAndCheckMinMax(val, 1, 18));
      opt_sigPurpose = SiSignaturePurpose::lookup(OFstatic_cast(size_t, val));
      if (opt_sigPurpose == SiSignaturePurpose::ESP_none)
      {
          // should never happen
          OFLOG_WARN(dcmsignLogger, "unknown digital signature purpose code, ignoring.");
      }
    }
    cmd.endOptionBlock();

    if (cmd.findOption("--tag-file"))
    {
      app.checkValue(cmd.getValue(opt_tagFile));
      opt_tagList = new DcmAttributeTag(DCM_DataElementsSigned);
      result = DcmSignatureHelper::parseTextFile(opt_tagFile, *opt_tagList);
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
        if (! DcmSignatureHelper::addTag(current, *opt_tagList))
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
      result = DcmSignatureHelper::do_verify(dataset);
      if (result != 0) return result;
      break;
    case DSO_sign:
      OFLOG_INFO(dcmsignLogger, "create signature in main object.");
      result = DcmSignatureHelper::do_sign(dataset, key, cert, opt_mac, opt_profile, opt_tagList, opt_signatureXfer, opt_dumpFile, opt_sigPurpose);
      if (result != 0) return result;
      break;
    case DSO_signItem:
      OFLOG_INFO(dcmsignLogger, "create signature in sequence item.");
      result = DcmSignatureHelper::do_sign_item(dataset, key, cert, opt_mac, opt_profile, opt_tagList, opt_location, opt_signatureXfer, opt_dumpFile, opt_sigPurpose);
      if (result != 0) return result;
      break;
    case DSO_remove:
      OFLOG_INFO(dcmsignLogger, "removing signature from sequence item.");
      result = DcmSignatureHelper::do_remove(dataset, opt_location);
      if (result != 0) return result;
      break;
    case DSO_removeAll:
      OFLOG_INFO(dcmsignLogger, "removing all signatures.");
      result = DcmSignatureHelper::do_remove_all(dataset);
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
    if (dataset->chooseRepresentation(opt_oxfer, NULL).bad() || (! dataset->canWriteXfer(opt_oxfer)))
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
