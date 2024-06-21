/*
 *
 *  Copyright (C) 2017-2023, OFFIS e.V.
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
 *  Module: dcmtls
 *
 *  Author: Jan Schlamelcher, Marco Eichelberg
 *
 *  Purpose:
 *    classes: DcmTLSOptions
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmtls/tlsopt.h"
#include "dcmtk/ofstd/ofconapp.h"
#include "dcmtk/dcmtls/tlscond.h"
#include "dcmtk/dcmnet/assoc.h"       /* for ASC_setTransportLayer() */
#include "tlsfmacr.h"                 /* for OpenSSL feature macros */

void DcmTLSOptionsBase::printLibraryVersion()
{
#ifdef WITH_OPENSSL
    COUT << "- " << DcmTLSTransportLayer::getOpenSSLVersionName() << OFendl;
#endif // WITH_OPENSSL
}

#ifdef WITH_OPENSSL
DcmTLSOptionsBase::DcmTLSOptionsBase(T_ASC_NetworkRole networkRole)
: opt_keyFileFormat(DCF_Filetype_PEM)
, opt_doAuthenticate(OFFalse)
, opt_privateKeyFile(OFnullptr)
, opt_certificateFile(OFnullptr)
, opt_passwd(OFnullptr)
, opt_tlsProfile(TSP_Profile_BCP_195_RFC_8996) // default: BCP 195 RFC 8996 TLS Profile
, opt_readSeedFile(OFnullptr)
, opt_writeSeedFile(OFnullptr)
, opt_certVerification(DCV_requireCertificate)
, opt_dhparam(OFnullptr)
, opt_secureConnection(OFFalse) // default: no secure connection
, opt_networkRole(networkRole)
, opt_clientSNI(OFnullptr)
, opt_serverSNI(OFnullptr)
, opt_crlMode(TCR_noCRL)
, tLayer(OFnullptr)
#else
DcmTLSOptionsBase::DcmTLSOptionsBase(T_ASC_NetworkRole /* networkRole */)
#endif
{
}

DcmTLSOptionsBase::~DcmTLSOptionsBase()
{
#ifdef WITH_OPENSSL
  delete tLayer;
#endif
}

#ifdef WITH_OPENSSL
OFBool DcmTLSOptionsBase::listOfCiphersRequested(OFCommandLine& cmd)
{
  if (cmd.findOption("--list-ciphers")) return OFTrue;
  return OFFalse;
}
#else
OFBool DcmTLSOptionsBase::listOfCiphersRequested(OFCommandLine& /* cmd */)
{
  return OFFalse;
}
#endif

#ifdef WITH_OPENSSL
void DcmTLSOptionsBase::printSupportedCiphersuites(OFConsoleApplication& app, STD_NAMESPACE ostream& os)
{
  DcmTLSCiphersuiteHandler csh;
  app.printHeader(OFTrue /*print host identifier*/);
  os << OFendl << "Supported TLS ciphersuites are:" << OFendl;
  csh.printSupportedCiphersuites(os);
}
#else
void DcmTLSOptionsBase::printSupportedCiphersuites(OFConsoleApplication& /* app */, STD_NAMESPACE ostream& /* os */)
{
}
#endif

OFBool DcmTLSOptionsBase::secureConnectionRequested() const
{
#ifdef WITH_OPENSSL
  return opt_secureConnection;
#else
  return OFFalse;
#endif
}

DcmTransportLayer *DcmTLSOptionsBase::getTransportLayer()
{
#ifdef WITH_OPENSSL
  return tLayer;
#else
  return NULL;
#endif
}

OFCondition DcmTLSOptionsBase::writeRandomSeed()
{
#ifdef WITH_OPENSSL
    if (opt_writeSeedFile && tLayer)
    {
        if (tLayer->canWriteRandomSeed())
        {
            if (!tLayer->writeRandomSeed(opt_writeSeedFile))
                return DCMTLS_EC_FailedToWriteRandomSeedFile(opt_writeSeedFile);
        }
        else return DCMTLS_EC_FailedToWriteRandomSeedFile(opt_writeSeedFile);
    }
#endif
    return EC_Normal;
}

#ifdef WITH_OPENSSL
OFCondition DcmTLSOptionsBase::verifyClientCertificate(const char *fileName)
{
  if (tLayer) return tLayer->verifyClientCertificate(fileName, opt_keyFileFormat);
  return EC_IllegalCall;
}
#else
OFCondition DcmTLSOptionsBase::verifyClientCertificate(const char * /* fileName */)
{
  return EC_IllegalCall;
}
#endif

#ifdef WITH_OPENSSL
OFCondition DcmTLSOptionsBase::isRootCertificate(const char *fileName)
{
  return DcmTLSTransportLayer::isRootCertificate(fileName, opt_keyFileFormat);
}
#else
OFCondition DcmTLSOptionsBase::isRootCertificate(const char * /* fileName */)
{
  return EC_IllegalCall;
}
#endif

// ----------------------------------------------------------------------------

DcmTLSOptions::DcmTLSOptions(T_ASC_NetworkRole networkRole)
: DcmTLSOptionsBase(networkRole)
{
}

DcmTLSOptions::~DcmTLSOptions()
{
}

#ifdef WITH_OPENSSL
void DcmTLSOptions::addTLSCommandlineOptions(OFCommandLine& cmd)
{
  DcmTLSCiphersuiteHandler csh;

  cmd.addGroup("transport layer security (TLS) options:");
    cmd.addSubGroup("transport protocol stack:");
      cmd.addOption("--disable-tls",        "-tls",    "use normal TCP/IP connection (default)");
      cmd.addOption("--enable-tls",         "+tls", 2, "[p]rivate key file, [c]ertificate file: string",
                                                       "use authenticated secure TLS connection");
      if (opt_networkRole == NET_REQUESTOR)
      {
        // this command line option only makes sense for association requesters (TLS clients)
        cmd.addOption("--anonymous-tls",    "+tla",    "use secure TLS connection without certificate");
      }
    cmd.addSubGroup("private key password (only with --enable-tls):");
      cmd.addOption("--std-passwd",         "+ps",     "prompt user to type password on stdin (default)");
      cmd.addOption("--use-passwd",         "+pw",  1, "[p]assword: string ",
                                                       "use specified password");
      cmd.addOption("--null-passwd",        "-pw",     "use empty string as password");
    cmd.addSubGroup("key and certificate file format:");
      cmd.addOption("--pem-keys",           "-pem",    "read keys and certs as PEM file (default)");
      cmd.addOption("--der-keys",           "-der",    "read keys and certificates as DER file");
    cmd.addSubGroup("certification authority:");
      cmd.addOption("--add-cert-file",      "+cf",  1, "[f]ilename: string",
                                                       "add certificate file to list of certificates");
      cmd.addOption("--add-cert-dir",       "+cd",  1, "[d]irectory: string",
                                                       "add certificates in d to list of certificates");
      cmd.addOption("--add-crl-file",       "+crl", 1, "[f]ilename: string",
                                                       "add certificate revocation list file\n(implies --enable-crl-vfy)");
      cmd.addOption("--enable-crl-vfy",     "+crv",    "enable leaf CRL verification");
      cmd.addOption("--enable-crl-all",     "+cra",    "enable full chain CRL verification");
    cmd.addSubGroup("security profile:");
      cmd.addOption("--profile-8996",       "+pg",     "BCP 195 RFC 8996 TLS Profile (default)");
#ifdef DCMTK_Modified_BCP195_RFC8996_TLS_Profile_Supported
      cmd.addOption("--profile-8996-mod",   "+pm",     "Modified BCP 195 RFC 8996 TLS Profile");
#endif
      cmd.addOption("--profile-bcp195-nd",  "+py",     "Non-downgrading BCP 195 TLS Profile (retired)");
      cmd.addOption("--profile-bcp195",     "+px",     "BCP 195 TLS Profile (retired)");
      cmd.addOption("--profile-bcp195-ex",  "+pz",     "Extended BCP 195 TLS Profile (retired)");
      if (csh.cipher3DESsupported())
      {
        cmd.addOption("--profile-basic",    "+pb",     "Basic TLS Secure Transport Connection Profile\n(retired)");
      }
      cmd.addOption("--profile-aes" ,       "+pa",     "AES TLS Secure Transport Connection Profile\n(retired)");
      if (csh.cipherNULLsupported())
      {
        cmd.addOption("--profile-null",     "+pn",     "Authenticated unencrypted communication\n(retired, was used in IHE ATNA)");
      }

    cmd.addSubGroup("ciphersuite:");
      cmd.addOption("--list-ciphers",       "+cc",     "list supported TLS ciphersuites and exit", OFCommandLine::AF_Exclusive);
      cmd.addOption("--cipher",             "+cs",  1, "[c]iphersuite name: string",
                                                       "add ciphersuite to list of negotiated suites\n(not with --profile-bcp195-ex)");
      if (opt_networkRole != NET_REQUESTOR)
      {
        // this command line option only makes sense for association acceptors (TLS servers)
        // or systems that accept and request associations
        cmd.addOption("--dhparam",          "+dp",  1, "[f]ilename: string",
                                                       "read DH parameters for DH/DSS ciphersuites");
      }
    cmd.addSubGroup("server name indication:");
      cmd.addOption("--no-sni",                        "do not use SNI (default)");
      if (opt_networkRole != NET_ACCEPTOR)
      {
        cmd.addOption("--request-sni",              1, "[s]erver name: string",
                                                       "request server name s");
      }
      if (opt_networkRole != NET_REQUESTOR)
      {
        cmd.addOption("--expect-sni",               1, "[s]erver name: string",
                                                       "expect requests for server name s");
      }

    cmd.addSubGroup("pseudo random generator:");
      cmd.addOption("--seed",               "+rs",  1, "[f]ilename: string",
                                                       "seed random generator with contents of f");
      cmd.addOption("--write-seed",         "+ws",     "write back modified seed (only with --seed)");
      cmd.addOption("--write-seed-file",    "+wf",  1, "[f]ilename: string (only with --seed)",
                                                       "write modified seed to file f");
    cmd.addSubGroup("peer authentication:");
      cmd.addOption("--require-peer-cert",  "-rc",     "verify peer cert, fail if absent (default)");
      if (opt_networkRole != NET_REQUESTOR)
      {
        // this command line option only makes sense for association acceptors (TLS servers)
        // or systems that accept and request associations
        cmd.addOption("--verify-peer-cert", "-vc",     "verify peer certificate if present");
      }
      cmd.addOption("--ignore-peer-cert",   "-ic",     "don't verify peer certificate");
}
#else
void DcmTLSOptions::addTLSCommandlineOptions(OFCommandLine& /* cmd */)
{
}
#endif // WITH_OPENSSL

#ifdef WITH_OPENSSL
void DcmTLSOptions::parseArguments(OFConsoleApplication& app, OFCommandLine& cmd)
{
    DcmTLSCiphersuiteHandler csh;

    const char *tlsopts = (opt_networkRole == NET_REQUESTOR ?
        "--enable-tls or --anonymous-tls" : "--enable-tls");

    cmd.beginOptionBlock();
    if (cmd.findOption("--disable-tls"))
    {
        opt_secureConnection = OFFalse;
        opt_doAuthenticate = OFFalse;
    }
    if (cmd.findOption("--enable-tls"))
    {
        opt_secureConnection = OFTrue;
        opt_doAuthenticate = OFTrue;
        app.checkValue(cmd.getValue(opt_privateKeyFile));
        app.checkValue(cmd.getValue(opt_certificateFile));
    }
    if (opt_networkRole == NET_REQUESTOR)
    {
        if (cmd.findOption("--anonymous-tls"))
        {
            opt_secureConnection = OFTrue;
            opt_doAuthenticate = OFFalse;
        }
    }
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--std-passwd"))
    {
        app.checkDependence("--std-passwd", "--enable-tls", opt_doAuthenticate);
        opt_passwd = OFnullptr;
    }
    if (cmd.findOption("--use-passwd"))
    {
        app.checkDependence("--use-passwd", "--enable-tls", opt_doAuthenticate);
        app.checkValue(cmd.getValue(opt_passwd));
    }
    if (cmd.findOption("--null-passwd"))
    {
        app.checkDependence("--null-passwd", "--enable-tls", opt_doAuthenticate);
        opt_passwd = "";
    }
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--pem-keys"))
    {
        app.checkDependence("--pem-keys", tlsopts, opt_secureConnection);
        opt_keyFileFormat = DCF_Filetype_PEM;
    }
    if (cmd.findOption("--der-keys"))
    {
        app.checkDependence("--der-keys", tlsopts, opt_secureConnection);
        opt_keyFileFormat = DCF_Filetype_ASN1;
    }
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--enable-crl-vfy"))
    {
        app.checkDependence("--enable-crl-vfy", tlsopts, opt_secureConnection);
        opt_crlMode = TCR_checkLeafCRL;
    }
    if (cmd.findOption("--enable-crl-all"))
    {
        app.checkDependence("--enable-crl-all", tlsopts, opt_secureConnection);
        opt_crlMode = TCR_checkAllCRL;
    }
    cmd.endOptionBlock();

    if ((opt_networkRole != NET_REQUESTOR) && cmd.findOption("--dhparam"))
    {
        app.checkDependence("--dhparam", tlsopts, opt_secureConnection);
        app.checkValue(cmd.getValue(opt_dhparam));
    }

    if ((opt_networkRole != NET_ACCEPTOR) && cmd.findOption("--request-sni"))
    {
        app.checkValue(cmd.getValue(opt_clientSNI));
    }
    if ((opt_networkRole != NET_REQUESTOR) && cmd.findOption("--expect-sni"))
    {
        app.checkValue(cmd.getValue(opt_serverSNI));
    }
    if (cmd.findOption("--no-sni"))
    {
        app.checkConflict("--no-sni", "--request-sni", (opt_clientSNI != NULL));
        app.checkConflict("--no-sni", "--expect-sni", (opt_serverSNI != NULL));
        opt_clientSNI = OFnullptr;
        opt_serverSNI = OFnullptr;
    }

    if (cmd.findOption("--seed"))
    {
        app.checkDependence("--seed", tlsopts, opt_secureConnection);
        app.checkValue(cmd.getValue(opt_readSeedFile));
    }

    cmd.beginOptionBlock();
    if (cmd.findOption("--write-seed"))
    {
        app.checkDependence("--write-seed", tlsopts, opt_secureConnection);
        app.checkDependence("--write-seed", "--seed", opt_readSeedFile != OFnullptr);
        opt_writeSeedFile = opt_readSeedFile;
    }
    if (cmd.findOption("--write-seed-file"))
    {
        app.checkDependence("--write-seed-file", tlsopts, opt_secureConnection);
        app.checkDependence("--write-seed-file", "--seed", opt_readSeedFile != OFnullptr);
        app.checkValue(cmd.getValue(opt_writeSeedFile));
    }
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--require-peer-cert"))
    {
        app.checkDependence("--require-peer-cert", tlsopts, opt_secureConnection);
        opt_certVerification = DCV_requireCertificate;
    }
    if (cmd.findOption("--ignore-peer-cert"))
    {
        app.checkDependence("--ignore-peer-cert", tlsopts, opt_secureConnection);
        opt_certVerification = DCV_ignoreCertificate;
    }
    if ((opt_networkRole != NET_REQUESTOR) && cmd.findOption("--verify-peer-cert"))
    {
        app.checkDependence("--verify-peer-cert", tlsopts, opt_secureConnection);
        opt_certVerification = DCV_checkCertificate;
    }
    cmd.endOptionBlock();

    cmd.beginOptionBlock();
    if (cmd.findOption("--profile-8996"))
    {
        app.checkDependence("--profile-8996", tlsopts, opt_secureConnection);
        opt_tlsProfile = TSP_Profile_BCP_195_RFC_8996;
    }
#ifdef DCMTK_Modified_BCP195_RFC8996_TLS_Profile_Supported
    if (cmd.findOption("--profile-8996-mod"))
    {
        app.checkDependence("--profile-8996-mod", tlsopts, opt_secureConnection);
        opt_tlsProfile = TSP_Profile_BCP_195_RFC_8996_Modified;
    }
#endif
    if (cmd.findOption("--profile-bcp195"))
    {
        app.checkDependence("--profile-bcp195", tlsopts, opt_secureConnection);
        opt_tlsProfile = TSP_Profile_BCP195;
    }
    if (cmd.findOption("--profile-bcp195-nd"))
    {
        app.checkDependence("--profile-bcp195-nd", tlsopts, opt_secureConnection);
        opt_tlsProfile = TSP_Profile_BCP195_ND;
    }
    if (cmd.findOption("--profile-bcp195-ex"))
    {
        app.checkDependence("--profile-bcp195-ex", tlsopts, opt_secureConnection);
        opt_tlsProfile = TSP_Profile_BCP195_Extended;
    }
    if (csh.cipher3DESsupported())
    {
        if (cmd.findOption("--profile-basic"))
        {
            app.checkDependence("--profile-basic", tlsopts, opt_secureConnection);
            opt_tlsProfile = TSP_Profile_Basic;
        }
    }
    if (cmd.findOption("--profile-aes"))
    {
        app.checkDependence("--profile-basic", tlsopts, opt_secureConnection);
        opt_tlsProfile = TSP_Profile_AES;
    }
    if (csh.cipherNULLsupported())
    {
        if (cmd.findOption("--profile-null"))
        {
            app.checkDependence("--profile-null", tlsopts, opt_secureConnection);
            opt_tlsProfile = TSP_Profile_IHE_ATNA_Unencrypted;
        }
    }
    cmd.endOptionBlock();

    // check the other TLS specific options that will only be evaluated
    // later in DcmTLSOptions::createTransportLayer().
    if (cmd.findOption("--add-cert-file"))
        app.checkDependence("--add-cert-file", tlsopts, opt_secureConnection);
    if (cmd.findOption("--add-cert-dir"))
        app.checkDependence("--add-cert-dir", tlsopts, opt_secureConnection);
    if (cmd.findOption("--add-crl-file"))
        app.checkDependence("--add-crl-file", tlsopts, opt_secureConnection);

    if (cmd.findOption("--cipher"))
    {
        app.checkDependence("--cipher", tlsopts, opt_secureConnection);
        app.checkConflict("--cipher", "--profile-bcp195-ex", (opt_tlsProfile == TSP_Profile_BCP195_Extended));
    }
}
#else
void DcmTLSOptions::parseArguments(OFConsoleApplication& /* app */, OFCommandLine& /* cmd */)
{
}
#endif

#ifdef WITH_OPENSSL
OFCondition DcmTLSOptions::createTransportLayer(
      T_ASC_Network *net,
      T_ASC_Parameters *params,
      OFConsoleApplication& app,
      OFCommandLine& cmd)
{
    // use mode that was passed via command line options
    DcmTLSCRLVerification crlmode = opt_crlMode;

    if (opt_secureConnection)
    {
      delete tLayer;
      tLayer = new DcmTLSTransportLayer(opt_networkRole, opt_readSeedFile, OFFalse);
      if (tLayer == NULL) return DCMTLS_EC_FailedToCreateTLSTransportLayer;

      if (cmd.findOption("--add-cert-file", 0, OFCommandLine::FOM_First))
      {
        const char *current = NULL;
        do
        {
          app.checkValue(cmd.getValue(current));
          if (tLayer->addTrustedCertificateFile(current, opt_keyFileFormat).bad())
          {
              DCMTLS_WARN("unable to load certificate file '" << current << "', ignoring");
          }
        } while (cmd.findOption("--add-cert-file", 0, OFCommandLine::FOM_Next));
      }

      if (cmd.findOption("--add-cert-dir", 0, OFCommandLine::FOM_First))
      {
        const char *current = NULL;
        do
        {
          app.checkValue(cmd.getValue(current));
          if (tLayer->addTrustedCertificateDir(current, opt_keyFileFormat).bad())
          {
            DCMTLS_WARN("unable to load certificates from directory '" << current << "', ignoring");
          }
        } while (cmd.findOption("--add-cert-dir", 0, OFCommandLine::FOM_Next));
      }

      if (cmd.findOption("--add-crl-file", 0, OFCommandLine::FOM_First))
      {
        const char *current = NULL;
        do
        {
          app.checkValue(cmd.getValue(current));
          if (tLayer->addCertificateRevocationList(current, opt_keyFileFormat).good())
          {
            // enable default CRL verification mode if file was loaded successfully
            if (crlmode == TCR_noCRL)
              crlmode = TCR_checkLeafCRL;
          } else {
            DCMTLS_WARN("unable to load CRL file '" << current << "', ignoring");
          }
        } while (cmd.findOption("--add-crl-file", 0, OFCommandLine::FOM_Next));
      }

      // set CRL verification mode
      tLayer->setCRLverification(crlmode);

      OFCondition cond;
      if (opt_doAuthenticate)
      {
        if (opt_passwd)
           tLayer->setPrivateKeyPasswd(opt_passwd);
           else tLayer->setPrivateKeyPasswdFromConsole();

        cond = tLayer->setPrivateKeyFile(opt_privateKeyFile, opt_keyFileFormat);

        // replace the low-level error message with an easier to understand one
        if (cond.bad()) return DCMTLS_EC_FailedToLoadPrivateKey(opt_privateKeyFile);

        cond = tLayer->setCertificateFile(opt_certificateFile, opt_keyFileFormat, opt_tlsProfile);

        // replace the low-level error message with an easier to understand one
        if (cond.bad()) return DCMTLS_EC_FailedToLoadCertificate(opt_certificateFile);

        if (! tLayer->checkPrivateKeyMatchesCertificate())
           return DCMTLS_EC_MismatchedPrivateKeyAndCertificate(opt_privateKeyFile, opt_certificateFile);
      }

      // set TLS profile
      cond =  tLayer->setTLSProfile(opt_tlsProfile);
      if (cond.bad()) return cond;

      // add additional ciphersuites
      if (cmd.findOption("--cipher", 0, OFCommandLine::FOM_First))
      {
        const char *current = NULL;
        do
        {
          app.checkValue(cmd.getValue(current));
          cond = tLayer->addCipherSuite(current);
          if (cond.bad()) return cond;
        } while (cmd.findOption("--cipher", 0, OFCommandLine::FOM_Next));
      }

      cond = tLayer->activateCipherSuites();
      if (cond.bad()) return cond;

      // set SNI names
      tLayer->setClientSNI(opt_clientSNI);
      tLayer->setServerSNI(opt_serverSNI);

      // loading of DH parameters should happen after the call to setTLSProfile()
      // because otherwise we cannot check profile specific restrictions
      if (opt_dhparam && ! (tLayer->setTempDHParameters(opt_dhparam)))
          DCMTLS_WARN("unable to load temporary DH parameter file '" << opt_dhparam << "', ignoring");

      tLayer->setCertificateVerification(opt_certVerification);

      if (net)
      {
        cond = ASC_setTransportLayer(net, tLayer, 0);
        if (cond.bad()) return cond;
      }

      if (params)
      {
        cond = ASC_setTransportLayerType(params, opt_secureConnection);
        if (cond.bad()) return cond;
      }
    }
    return EC_Normal;
}
#else
OFCondition DcmTLSOptions::createTransportLayer(
      T_ASC_Network * /* net */,
      T_ASC_Parameters * /* params */,
      OFConsoleApplication& /* app */,
      OFCommandLine& /* cmd */)
{
    return EC_Normal;
}
#endif
