/*
 *
 *  Copyright (C) 2017, OFFIS e.V.
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
 *  Author: Jan Schlamelcher
 *
 *  Purpose:
 *    classes: DcmTLSOptions
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofcmdln.h"
#include "dcmtk/dcmtls/tlscond.h"
#include "dcmtk/dcmtls/tlsopt.h"

void DcmTLSOptions::printLibraryVersion()
{
#ifdef WITH_OPENSSL
    COUT << "- " << OPENSSL_VERSION_TEXT << OFendl;
#endif
}

DcmTLSOptions::DcmTLSOptions( OFConsoleApplication& app, OFCommandLine& cmd )
#ifdef WITH_OPENSSL
: m_ConsoleApp( app )
, m_CommandLine( cmd )
, opt_keyFileFormat( SSL_FILETYPE_PEM )
, opt_doAuthenticate( OFFalse )
, opt_privateKeyFile( OFnullptr )
, opt_certificateFile( OFnullptr )
, opt_passwd( OFnullptr )
#if OPENSSL_VERSION_NUMBER >= 0x0090700fL
, opt_ciphersuites( TLS1_TXT_RSA_WITH_AES_128_SHA ":" SSL3_TXT_RSA_DES_192_CBC3_SHA )
#else
, opt_ciphersuites( SSL3_TXT_RSA_DES_192_CBC3_SHA )
#endif
, opt_readSeedFile( OFnullptr )
, opt_writeSeedFile( OFnullptr )
, opt_certVerification( DCV_requireCertificate )
, opt_dhparam( OFnullptr )
, opt_secureConnection( OFFalse ) // default: no secure connection
{
    m_CommandLine.addGroup( "transport layer security (TLS) options:" );
        m_CommandLine.addSubGroup( "transport protocol stack:");
            m_CommandLine.addOption( "--disable-tls",         "-tls",    "use normal TCP/IP connection (default)" );
            m_CommandLine.addOption( "--enable-tls",          "+tls", 2, "[p]rivate key file, [c]ertificate file: string",
                                                              "use authenticated secure TLS connection" );
            m_CommandLine.addOption( "--anonymous-tls",       "+tla",    "use secure TLS connection without certificate" );
        m_CommandLine.addSubGroup( "private key password (only with --enable-tls):" );
            m_CommandLine.addOption( "--std-passwd",          "+ps",     "prompt user to type password on stdin (default)" );
            m_CommandLine.addOption( "--use-passwd",          "+pw",  1, "[p]assword: string ",
                                                              "use specified password" );
            m_CommandLine.addOption( "--null-passwd",         "-pw",     "use empty string as password" );
        m_CommandLine.addSubGroup( "key and certificate file format:" );
            m_CommandLine.addOption( "--pem-keys",            "-pem",    "read keys and certificates as PEM file (default)" );
            m_CommandLine.addOption( "--der-keys",            "-der",    "read keys and certificates as DER file" );
        m_CommandLine.addSubGroup( "certification authority:" );
            m_CommandLine.addOption( "--add-cert-file",       "+cf",  1, "[c]ertificate filename: string",
                                                              "add certificate file to list of certificates", OFCommandLine::AF_NoWarning );
            m_CommandLine.addOption( "--add-cert-dir",        "+cd",  1, "[c]ertificate directory: string",
                                                              "add certificates in d to list of certificates", OFCommandLine::AF_NoWarning );
        m_CommandLine.addSubGroup( "ciphersuite:");
            m_CommandLine.addOption( "--cipher",              "+cs",  1, "[c]iphersuite name: string",
                                                              "add ciphersuite to list of negotiated suites" );
            m_CommandLine.addOption( "--dhparam",             "+dp",  1, "[f]ilename: string",
                                                              "read DH parameters for DH/DSS ciphersuites" );
        m_CommandLine.addSubGroup( "pseudo random generator:" );
            m_CommandLine.addOption( "--seed",                "+rs",  1, "[f]ilename: string",
                                                              "seed random generator with contents of f" );
            m_CommandLine.addOption( "--write-seed",          "+ws",     "write back modified seed (only with --seed)" );
            m_CommandLine.addOption( "--write-seed-file",     "+wf",  1, "[f]ilename: string (only with --seed)",
                                                              "write modified seed to file f" );
        m_CommandLine.addSubGroup( "peer authentication:" );
            m_CommandLine.addOption( "--require-peer-cert",   "-rc",     "verify peer certificate, fail if absent (default)" );
            m_CommandLine.addOption( "--verify-peer-cert",    "-vc",     "verify peer certificate if present" );
            m_CommandLine.addOption( "--ignore-peer-cert",    "-ic",     "don't verify peer certificate" );
}
#else // WITH_OPENSSL
{
    // no OpenSSL available, don't add any command line arguments
}
#endif // WITH_OPENSSL

OFCondition DcmTLSOptions::parseArguments()
{
#ifdef WITH_OPENSSL
    m_CommandLine.beginOptionBlock();
    if( m_CommandLine.findOption( "--disable-tls" ) )
        opt_secureConnection = OFFalse;
    if( m_CommandLine.findOption( "--enable-tls" ) )
    {
        opt_secureConnection = OFTrue;
        opt_doAuthenticate = OFTrue;
        m_ConsoleApp.checkValue( m_CommandLine.getValue( opt_privateKeyFile ) );
        m_ConsoleApp.checkValue( m_CommandLine.getValue( opt_certificateFile ) );
    }
    if( m_CommandLine.findOption( "--anonymous-tls" ) )
        opt_secureConnection = OFTrue;
    m_CommandLine.endOptionBlock();

    m_CommandLine.beginOptionBlock();
    if( m_CommandLine.findOption( "--std-passwd" ) )
    {
        m_ConsoleApp.checkDependence("--std-passwd", "--enable-tls", opt_doAuthenticate);
        opt_passwd = OFnullptr;
    }
    if( m_CommandLine.findOption("--use-passwd") )
    {
        m_ConsoleApp.checkDependence( "--use-passwd", "--enable-tls", opt_doAuthenticate );
        m_ConsoleApp.checkValue( m_CommandLine.getValue( opt_passwd ) );
    }
    if( m_CommandLine.findOption( "--null-passwd" ) )
    {
        m_ConsoleApp.checkDependence( "--null-passwd", "--enable-tls", opt_doAuthenticate );
        opt_passwd = "";
    }
    m_CommandLine.endOptionBlock();

    m_CommandLine.beginOptionBlock();
    if( m_CommandLine.findOption( "--pem-keys" ) )
        opt_keyFileFormat = SSL_FILETYPE_PEM;
    if( m_CommandLine.findOption( "--der-keys" ) )
        opt_keyFileFormat = SSL_FILETYPE_ASN1;
    m_CommandLine.endOptionBlock();

    if( m_CommandLine.findOption( "--dhparam" ) )
        m_ConsoleApp.checkValue( m_CommandLine.getValue( opt_dhparam ) );
    if( m_CommandLine.findOption( "--seed" ) )
        m_ConsoleApp.checkValue( m_CommandLine.getValue( opt_readSeedFile ) );

    m_CommandLine.beginOptionBlock();
    if( m_CommandLine.findOption( "--write-seed" ) )
    {
        m_ConsoleApp.checkDependence( "--write-seed", "--seed", opt_readSeedFile != OFnullptr );
        opt_writeSeedFile = opt_readSeedFile;
    }
    if( m_CommandLine.findOption( "--write-seed-file" ) )
    {
        m_ConsoleApp.checkDependence( "--write-seed-file", "--seed", opt_readSeedFile != OFnullptr );
        m_ConsoleApp.checkValue( m_CommandLine.getValue( opt_writeSeedFile ) );
    }
    m_CommandLine.endOptionBlock();

    m_CommandLine.beginOptionBlock();
    if( m_CommandLine.findOption( "--require-peer-cert" ) )
        opt_certVerification = DCV_requireCertificate;
    if( m_CommandLine.findOption( "--verify-peer-cert" ) )
        opt_certVerification = DCV_checkCertificate;
    if( m_CommandLine.findOption( "--ignore-peer-cert" ) )
        opt_certVerification = DCV_ignoreCertificate;
    m_CommandLine.endOptionBlock();

    const char* current = OFnullptr;
    const char* currentOpenSSL;
    if( m_CommandLine.findOption( "--cipher", 0, OFCommandLine::FOM_First ) )
    {
        opt_ciphersuites.clear();
        do
        {
            m_ConsoleApp.checkValue( m_CommandLine.getValue( current ) );
            if( !( currentOpenSSL = DcmTLSTransportLayer::findOpenSSLCipherSuiteName( current ) ) )
            {
                return DCMTLS_EC_UnknownCiphersuite( current );
            }
            else
            {
                if( !opt_ciphersuites.empty() )
                    opt_ciphersuites += ":";
                opt_ciphersuites += currentOpenSSL;
            }
        }
        while( m_CommandLine.findOption( "--cipher", 0, OFCommandLine::FOM_Next ) );
    }
#endif
    return EC_Normal;
}

OFrvalue<OFvariant<OFCondition,DcmTLSTransportLayer> >
DcmTLSOptions::createTransportLayer( int networkRole )
{
    // Pre C++11 compilers won't understand that the result is implicitly
    // convertable, therefore, we need to explicitly construct the return type.
    // This typedef eases it a bit.
    typedef OFrvalue<OFvariant<OFCondition,DcmTLSTransportLayer> > return_type;

    if( opt_secureConnection )
    {
        DcmTLSTransportLayer tlsLayer( networkRole, opt_readSeedFile );
        if( !tlsLayer )
            return return_type( DCMTLS_EC_FailedToCreateTLSTransportLayer );

        if( m_CommandLine.findOption( "--add-cert-file", 0, OFCommandLine::FOM_First ) )
        {
            const char *current = OFnullptr;
            do
            {
                m_ConsoleApp.checkValue( m_CommandLine.getValue( current ) );
                if( TCS_ok != tlsLayer.addTrustedCertificateFile( current, opt_keyFileFormat ) )
                    DCMTLS_WARN( "unable to load certificate file '" << current << "', ignoring" );
            }
            while( m_CommandLine.findOption( "--add-cert-file", 0, OFCommandLine::FOM_Next ) );
        }

        if( m_CommandLine.findOption( "--add-cert-dir", 0, OFCommandLine::FOM_First ) )
        {
            const char *current = OFnullptr;
            do
            {
                m_ConsoleApp.checkValue( m_CommandLine.getValue( current ) );
                if( TCS_ok != tlsLayer.addTrustedCertificateDir( current, opt_keyFileFormat ) )
                    DCMTLS_WARN( "unable to load certificates from directory '" << current << "', ignoring" );
            }
            while( m_CommandLine.findOption( "--add-cert-dir", 0, OFCommandLine::FOM_Next ) );
        }

        if( opt_dhparam && !tlsLayer.setTempDHParameters( opt_dhparam ) )
            DCMTLS_WARN( "unable to load temporary DH parameter file '" << opt_dhparam << "', ignoring" );

        if( opt_doAuthenticate )
        {
            if( opt_passwd )
                tlsLayer.setPrivateKeyPasswd( opt_passwd );
            if( TCS_ok != tlsLayer.setPrivateKeyFile( opt_privateKeyFile, opt_keyFileFormat ) )
                return return_type( DCMTLS_EC_FailedToLoadPrivateKey( opt_privateKeyFile ) );
            if( TCS_ok != tlsLayer.setCertificateFile( opt_certificateFile, opt_keyFileFormat ) )
                return return_type( DCMTLS_EC_FailedToLoadCertificate( opt_certificateFile ) );
            if( !tlsLayer.checkPrivateKeyMatchesCertificate() )
                return return_type( DCMTLS_EC_MismatchedPrivateKeyAndCertificate( opt_privateKeyFile, opt_certificateFile ) );
        }

        if( TCS_ok != tlsLayer.setCipherSuites( opt_ciphersuites.c_str() ) )
            return return_type( DCMTLS_EC_FailedToSetCiphersuites );

        tlsLayer.setCertificateVerification( opt_certVerification );

        return return_type( OFmove( tlsLayer ) );
    }

    return return_type( EC_Normal );
}

OFCondition DcmTLSOptions::writeRandomSeedFileIfEnabled( DcmTLSTransportLayer& layer )
{
    if( opt_writeSeedFile )
    {
        if( layer.canWriteRandomSeed() )
        {
            if( !layer.writeRandomSeed( opt_writeSeedFile ) )
                return DCMTLS_EC_FailedToWriteRandomSeedFile( opt_writeSeedFile );
        }
        else return DCMTLS_EC_FailedToWriteRandomSeedFile;
    }
    return EC_Normal;
}
