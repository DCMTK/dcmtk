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

#ifndef TLSOPT_H
#define TLSOPT_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofvriant.h"
#include "dcmtk/dcmtls/tlslayer.h"

/** A class that handles the command line arguments used by applications
 *  that support TLS.
 *  DcmTLSOptions handles storing the relevant options, printing the associated
 *  help text an information (e.g. OpenSSL library version), parsing and
 *  evaluating the given command line arguments and creating a
 *  DcmTLSTransportLayer object based on the collected information.
 */
class DCMTK_DCMTLS_EXPORT DcmTLSOptions
{
public:

    /** Print OpenSSL library version string.
     *  Does nothing if OpenSSL is not available.
     */
    static void printLibraryVersion();

    /** Constructor.
     *  @param app a reference to an OFConsoleApplication object used in the
     *    calling application.
     *  @param cmd a reference to an OFCommandLine object used to parse
     *    the command line argument give to the calling application.
     */
    DcmTLSOptions( class OFConsoleApplication& app, class OFCommandLine& cmd );

    /** Parse and evaluate the given command line arguments.
     *  @return EC_Normal on success, an error condition indicating what went
     *    wrong if any invalid argument was encountered, e.g. a selected
     *    ciphersuite is not supported.
     */
    OFCondition parseArguments();

    /** Create a DcmTLSTransportLayer object based on the collected command
     *  line arguments.
     *  @param networkRole the network role to create a transport layer for,
     *    DICOM_APPLICATION_ACCEPTOR or DICOM_APPLICATION_REQUESTOR.
     *  @return Either a newly created DcmTLSTransportLayer object or an
     *    OFCondition indicating why no DcmTLSTransportLayer object was created.
     *    There are two possible reasons why this might happen: either the
     *    given command line arguments did not request to use a secure transport
     *    layer - in this case EC_Normal will be returned indicating one should
     *    simply ignore the result - or an error occurred when trying to create
     *    the DcmTLSTransport layer. In the latter case the returned OFCondition
     *    will contain an error condition describing what went wrong.
     */
    OFrvalue<OFvariant<OFCondition,DcmTLSTransportLayer> >
    createTransportLayer( int networkRole );

    /** Update the random seed file if this was requested by the given command
     *  line arguments.
     *  @param layer a reference to a DcmTLSTransportLayer object that contains
     *    the information for updating the random seed file, e.g. created via
     *    createTransportLayer().
     *  @return EC_Normal if the random seed file was successfully updated or
     *    if the user did not request the random seed file to be update. An
     *    error condition indicating what went wrong in case the random seed
     *    file could not be updated.
     */
    OFCondition writeRandomSeedFileIfEnabled( DcmTLSTransportLayer& layer );

private:
#ifdef WITH_OPENSSL
    class OFConsoleApplication& m_ConsoleApp;
    class OFCommandLine& m_CommandLine;
    int         opt_keyFileFormat;
    OFBool      opt_doAuthenticate;
    const char* opt_privateKeyFile;
    const char* opt_certificateFile;
    const char* opt_passwd;
    OFString    opt_ciphersuites;
    const char* opt_readSeedFile;
    const char* opt_writeSeedFile;
    DcmCertificateVerification opt_certVerification;
    const char* opt_dhparam;
    OFBool opt_secureConnection;
#endif // WITH_OPENSSL
};

#endif // TLSOPT_H
