/*
 *
 *  Copyright (C) 2017-2021, OFFIS e.V.
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

#ifndef TLSOPT_H
#define TLSOPT_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstream.h"     /* for ostream */
#include "dcmtk/dcmtls/tlslayer.h"    /* for DcmTLSTransportLayer */

class OFConsoleApplication;
class OFCommandLine;
class DcmTLSTransportLayer;
struct T_ASC_Network;
struct T_ASC_Parameters;

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

    /** Constructor.
     *  @param networkRole the network role to create a transport layer for
     */
    DcmTLSOptions(T_ASC_NetworkRole networkRole);

    /// Destructor
    virtual ~DcmTLSOptions();

    /** Add TLS specific command line options to the OFCommandLine object
     *  passed to the constructor.
     *  @param cmd a reference to an OFCommandLine object used to parse
     *    the command line argument give to the calling application.
     */
    void addTLSCommandlineOptions(OFCommandLine& cmd);

    /** Parse and evaluate the given command line arguments.
     *  @param app a reference to an OFConsoleApplication object used in the
     *    calling application.
     *  @param cmd a reference to an OFCommandLine object used to parse
     *    the command line argument give to the calling application.
     */
    void parseArguments(OFConsoleApplication& app, OFCommandLine& cmd);

    /** Create a DcmTLSTransportLayer object based on the collected command
     *  line arguments.
     *  @param net pointer to network object in which the transport layer
     *    should be registered. May be NULL, in which case the caller
     *    must activate the transport layer manually using ASC_setTransportLayer().
     *  @param params pointer to the association negotiation parameters object.
     *    For an association acceptor, this parameter is passed as NULL.
     *    If NULL is passed and the caller in an association requestor,
     *    then it is the responsibility of the caller to call ASC_setTransportLayerType()
     *    and set the right transport layer type for the association parameters.
     *  @param app a reference to an OFConsoleApplication object used in the
     *    calling application.
     *  @param cmd a reference to an OFCommandLine object used to parse
     *    the command line argument give to the calling application.
     *  @return EC_Normal if successful, an error code otherwise
     */
    OFCondition createTransportLayer(
      T_ASC_Network *net,
      T_ASC_Parameters *params,
      OFConsoleApplication& app,
      OFCommandLine& cmd);

    /** Update the random seed file if this was requested by the given command
     *  line arguments.
     *  @return EC_Normal if the random seed file was successfully updated or
     *    if the user did not request the random seed file to be update. An
     *    error condition indicating what went wrong in case the random seed
     *    file could not be updated.
     */
    OFCondition writeRandomSeed();

    /** Returns true if a secure connection was requested, false otherwise.
     *  Caller must ensure that parseArguments() has been run before this method.
     *  @return true if secure connection requested, false otherwise
     */
    OFBool secureConnectionRequested() const;

    /** Returns a pointer to the transport layer object, or NULL if the object
     *  has not yet been created by a call to createTransportLayer().
     *  @return pointer to transport layer object, may be NULL.
     */
    DcmTransportLayer *getTransportLayer();

    /** loads a certificate or certificate chain from a file and checks whether
     *  it can be verified against the current settings of the trust store.
     *  @param fileName path to the certificate file
     *  @return EC_Normal if verification succeeded, an error code otherwise
     */
    OFCondition verifyClientCertificate(const char *fileName);

    /** loads a certificate file and checks whether it is a
     *  valid (e.g. non-expired), self-signed root certificate that
     *  can be verified against itself
     *  @param fileName path to the certificate file
     *  @return EC_Normal if certificate is a root certificate, an error code otherwise
     */
    OFCondition isRootCertificate(const char *fileName);

    /** checks if the command line option --list-ciphers was given.
     *  In this case the list of supported TLS ciphersuites should be
     *  printed to stdout and the application should terminate.
     *  @param cmd the command line that was used
     *  @return true if --list-ciphers option was found, false otherwise.
     */
    static OFBool listOfCiphersRequested(OFCommandLine& cmd);

    /** print a list of supported ciphersuites to the given output stream
     *  @param app a reference to an OFConsoleApplication object used in the
     *    calling application.
     *  @param os output stream
     */
    static void printSupportedCiphersuites(OFConsoleApplication& app, STD_NAMESPACE ostream& os);

    /** Print OpenSSL library version string.
     *  Does nothing if OpenSSL is not available.
     */
    static void printLibraryVersion();

private:
#ifdef WITH_OPENSSL
    /// flag indicating the file format of certificates and private keys: PEM or ASN.1
    /// @remark this member is only available if DCMTK is compiled with
    /// OpenSSL support enabled.
    DcmKeyFileFormat opt_keyFileFormat;

    /// flag indicating whether we will authenticate ourselves using a certificate and private key
    /// @remark this member is only available if DCMTK is compiled with
    /// OpenSSL support enabled.
    OFBool opt_doAuthenticate;

    /// filename of private key file we use to authenticate ourselves
    /// @remark this member is only available if DCMTK is compiled with
    /// OpenSSL support enabled.
    const char* opt_privateKeyFile;

    /// filename of certificate file we use to authenticate ourselves
    /// @remark this member is only available if DCMTK is compiled with
    /// OpenSSL support enabled.
    const char* opt_certificateFile;

    /** password for reading the private key file, may be NULL.
     *  In this case the password is read from STDIN.
     *  @remark this member is only available if DCMTK is compiled with
     *  OpenSSL support enabled.
     */
    const char* opt_passwd;

    /// DICOM TLS Security Profile selected
    /// @remark this member is only available if DCMTK is compiled with
    /// OpenSSL support enabled.
    DcmTLSSecurityProfile opt_tlsProfile;

    /// filename of file containing at least 1K of entropy used to seed the PRNG
    /// @remark this member is only available if DCMTK is compiled with
    /// OpenSSL support enabled.
    const char* opt_readSeedFile;

    /// filename to which the modified PRNG state is written back
    /// @remark this member is only available if DCMTK is compiled with
    /// OpenSSL support enabled.
    const char* opt_writeSeedFile;

    /// indicates whether we should verify the remote peer's certificate
    /// @remark this member is only available if DCMTK is compiled with
    /// OpenSSL support enabled.
    DcmCertificateVerification opt_certVerification;

    /// filename of Diffie-Hellman parameters file, may be NULL
    /// @remark this member is only available if DCMTK is compiled with
    /// OpenSSL support enabled.
    const char* opt_dhparam;

    /// a flag indicating whether or not a secure connection was requested
    /// @remark this member is only available if DCMTK is compiled with
    /// OpenSSL support enabled.
    OFBool opt_secureConnection;

    /// indicates whether we act as client, server or both
    T_ASC_NetworkRole opt_networkRole;

    /// pointer to the secure transport layer managed by this object
    /// @remark this member is only available if DCMTK is compiled with
    /// OpenSSL support enabled.
    DcmTLSTransportLayer *tLayer;
#endif // WITH_OPENSSL
};

#endif // TLSOPT_H
