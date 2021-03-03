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
 *  Module:  dcmtls
 *
 *  Author:  Jan Schlamelcher
 *
 *  Purpose: error conditions for DcmTLS
 *
 */

#ifndef TLSCOND_H
#define TLSCOND_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmtls/tlsdefin.h"

// offset added to error codes returned by OpenSSL's X509_STORE_CTX_get_error()
// function to convert them into OFCondition codes.
#define DCMTLS_EC_X509Verify_Offset 1024

// highest OpenSSL X.509 verification error code for which a mapping to
// DCMTK OFCondition constants has been defined.
#define DCMTLS_EC_X509Verify_Max 94

// offset added to ERR_LIB_SSL error codes returned by OpenSSL's ERR_GET_REASON()
// macro to convert them into OFCondition codes.
#define DCMTLS_EC_SSL_Offset 2048

// These classes allow creating error conditions that may either give a general
// description of the error or may also contain some specific information,
// e.g. a file name, while both a specialized object and a generic one would
// use the same error code so it stays comparable.

/** A struct for creating an unknown ciphersuite error condition.
 */
struct DCMTK_DCMTLS_EXPORT DcmTLSECUnknownCiphersuiteConst : OFConditionConst
{
    /** Constructor, initializes the OFConditionConst base.
     *  Don't use this if you don't have a very good reason.
     */
    DcmTLSECUnknownCiphersuiteConst();

    /** Create a specialized unknown ciphersuite error condition that
     *  refers to the name of the unknown ciphersuite that was used.
     *  @param ciphersuiteName the name of the unknown ciphersuite.
     *  @return an OFCondition that compares equal to
     *    DCMTLS_EC_UnknownCiphersuite but also refers to the name of the
     *    ciphersuite in its descriptive text.
     */
    OFCondition operator()( const char* ciphersuiteName ) const;
};

/** A struct for creating a failed to load private key error condition.
 */
struct DCMTK_DCMTLS_EXPORT DcmTLSECFailedToLoadPrivateKeyConst : OFConditionConst
{
    /** Constructor, initializes the OFConditionConst base.
     *  Don't use this if you don't have a very good reason.
     */
    DcmTLSECFailedToLoadPrivateKeyConst();

    /** Create a specialized failed to load private key error condition that
     *  refers to the file name of the private key that failed to load.
     *  @param privateKeyFile the file name of the private key file.
     *  @return an OFCondition that compares equal to
     *    DCMTLS_EC_FailedToLoadPrivateKey but also refers to the file name of
     *    the private key in its descriptive text.
     */
    OFCondition operator()( const char* privateKeyFile ) const;
};

/** A struct for creating a failed to load certificate error condition.
 */
struct DCMTK_DCMTLS_EXPORT DcmTLSECFailedToLoadCertificateConst : OFConditionConst
{
    /** Constructor, initializes the OFConditionConst base.
     *  Don't use this if you don't have a very good reason.
     */
    DcmTLSECFailedToLoadCertificateConst();

    /** Create a specialized failed to load certificate error condition that
     *  refers to the file name of the certificate that failed to load.
     *  @param certificateFile the file name of the certificate file.
     *  @return an OFCondition that compares equal to
     *    DCMTLS_EC_FailedToLoadCertificate but also refers to the file name of
     *    the certificate in its descriptive text.
     */
    OFCondition operator()( const char* certificateFile ) const;
};

/** A struct for creating a mismatched private key and certificate error condition.
 */
struct DCMTK_DCMTLS_EXPORT DcmTLSECMismatchedPrivateKeyAndCertificateConst : OFConditionConst
{
    /** Constructor, initializes the OFConditionConst base.
     *  Don't use this if you don't have a very good reason.
     */
    DcmTLSECMismatchedPrivateKeyAndCertificateConst();

    /** Create a specialized mismatched private key and certificate error condition
     *  that refers to the file names of the private key and certificate file that
     *  did not match.
     *  @param privateKeyFile the file name of the private key file.
     *  @param certificateFile the file name of the certificate file.
     *  @return an OFCondition that compares equal to
     *    DCMTLS_EC_MismatchedPrivateKeyAndCertificate but also refers to the file
     *    names of the private key and certificate in its descriptive text.
     */
    OFCondition operator()( const char* privateKeyFile, const char* certificateFile ) const;
};

/** A struct for creating a failed to write random seed file error condition.
 */
struct DCMTK_DCMTLS_EXPORT DcmTLSECFailedToWriteRandomSeedFileConst : OFConditionConst
{
    /** Constructor, initializes the OFConditionConst base.
     *  Don't use this if you don't have a very good reason.
     */
    DcmTLSECFailedToWriteRandomSeedFileConst();

    /** Create a specialized failed to write random seed file error condition
     *  that refers to the file name of the seed file that could not be written.
     *  @param seedFile the file name of the random seed file.
     *  @return an OFCondition that compares equal to
     *    DCMTLS_EC_FailedToWriteRandomSeedFile but also refers to the file name
     *    of the random seed file in its descriptive text.
     */
    OFCondition operator()( const char* seedFile ) const;
};

/** A struct for creating an error condition related to a generic OpenSSL error
 */
struct DCMTK_DCMTLS_EXPORT DcmTLSECGenericOpenSSLError : OFConditionConst
{
    /** Constructor, initializes the OFConditionConst base.
     *  Don't use this if you don't have a very good reason.
     */
    DcmTLSECGenericOpenSSLError();

    /** Create a specialized OpenSSL error condition
     *  that refers to the OpenSSL error code.
     *  @param errorCode OpenSSL error code
     *  @return an OFCondition that compares equal to
     *    DCMTLS_EC_GenericOpenSSLError but also refers to
     *    the OpenSSL error code, library and reason in its descriptive text.
     */
    OFCondition operator()(unsigned long errorCode) const;

};

// Declaration of generic error code instances

/// condition constant: TLS ciphersuite not supported
extern DCMTK_DCMTLS_EXPORT const DcmTLSECUnknownCiphersuiteConst DCMTLS_EC_UnknownCiphersuite;

/// condition constant: creation of the TLS transport layer failed
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_FailedToCreateTLSTransportLayer;

/// condition constant: loading of the private key failed
extern DCMTK_DCMTLS_EXPORT const DcmTLSECFailedToLoadPrivateKeyConst DCMTLS_EC_FailedToLoadPrivateKey;

/// condition constant: loading of the certificate failed
extern DCMTK_DCMTLS_EXPORT const DcmTLSECFailedToLoadCertificateConst DCMTLS_EC_FailedToLoadCertificate;

/// condition constant: mismatch between private key and certificate
extern DCMTK_DCMTLS_EXPORT const DcmTLSECMismatchedPrivateKeyAndCertificateConst DCMTLS_EC_MismatchedPrivateKeyAndCertificate;

/// condition constant: activation of ciphersuites failed
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_FailedToSetCiphersuites;

/// condition constant: failed to write file with PRNG seed
extern DCMTK_DCMTLS_EXPORT const DcmTLSECFailedToWriteRandomSeedFileConst DCMTLS_EC_FailedToWriteRandomSeedFile;

/// condition constant: TLS function called but no TLS transport connection present
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_NoTLSTransportConnectionPresent;

/// condition constant: ciphersuite not allowed in current DICOM security profile
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_CiphersuiteNotAllowed;

/// condition constant: TLS read operation did not complete (SSL_ERROR_WANT_READ)
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSReadOperationDidNotComplete;

/// condition constant: TLS write operation did not complete (SSL_ERROR_WANT_WRITE)
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSWriteOperationDidNotComplete;

/// condition constant: TLS X.509 lookup operation did not complete (SSL_ERROR_WANT_X509_LOOKUP)
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSX509LookupOperationDidNotComplete;

/// condition constant: TLS connection closed by peer (SSL_ERROR_ZERO_RETURN)
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSConnectionClosedByPeer;

/// condition constant: TLS connect operation did not complete (SSL_ERROR_WANT_CONNECT)
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSConnectOperationDidNotComplete;

/// condition constant: TLS accept operation did not complete (SSL_ERROR_WANT_ACCEPT)
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSAcceptOperationDidNotComplete;

/// condition constant: TLS operation did not complete because an asynchronous engine is still processing data (SSL_ERROR_WANT_ASYNC)
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSAsyncOperationDidNotComplete;

/// condition constant: TLS asynchronous job could not be started (SSL_ERROR_WANT_ASYNC_JOB)
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSAsyncJobCouldNotBeStarted;

/// condition constant: TLS client hello callback needed (SSL_ERROR_WANT_CLIENT_HELLO_CB)
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSClientHelloCallbackNeeded;

/// condition constant: other TLS error (unknown SSL_ERROR code)
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_OtherSSLError;

/// condition constant: setting of the X.509 verification mode failed
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_FailedToSetVerificationMode;

/// condition constant: certificate store context initialization failed
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_CertStoreCtxInitFailed;

/// condition constant: OpenSSL error
extern DCMTK_DCMTLS_EXPORT const DcmTLSECGenericOpenSSLError DCMTLS_EC_GenericOpenSSLError;

/// condition constant: OpenSSL I/O error, not further specified (SSL_ERROR_SYSCALL)
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_OpenSSLIOError;

// X.509 Verification Errors. These use condition codes > DCMTLS_EC_X509Verify_Offset.

/// condition constant: X.509 certificate verification error - unspecified
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyUnspecified;

/// condition constant: X.509 certificate verification error - unable to get issuer certificate
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyUnableToGetIssuerCert;

/// condition constant: X.509 certificate verification error - unable to get certificate CRL
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyUnableToGetCRL;

/// condition constant: X.509 certificate verification error - unable to decrypt certificate's signature
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyUnableToDecryptCertSignature;

/// condition constant: X.509 certificate verification error - unable to decrypt CRL's signature
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyUnableToDecryptCRLSignature;

/// condition constant: X.509 certificate verification error - unable to decode issuer public key
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyUnableToDecodeIssuerPublicKey;

/// condition constant: X.509 certificate verification error - certificate signature failure
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyCertSignatureFailure;

/// condition constant: X.509 certificate verification error - CRL signature failure
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyCRLSignatureFailure;

/// condition constant: X.509 certificate verification error - certificate is not yet valid
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyCertNotYetValid;

/// condition constant: X.509 certificate verification error - certificate has expired
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyCertHasExpired;

/// condition constant: X.509 certificate verification error - CRL is not yet valid
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyCRLNotYetValid;

/// condition constant: X.509 certificate verification error - CRL has expired
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyCRLHasExpired;

/// condition constant: X.509 certificate verification error - format error in certificate's notBefore field
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyErrorInCertNotBeforeField;

/// condition constant: X.509 certificate verification error - format error in certificate's notAfter field
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyErrorInCertNotAfterField;

/// condition constant: X.509 certificate verification error - format error in CRL's lastUpdate field
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyErrorInCRLLastUpdateField;

/// condition constant: X.509 certificate verification error - format error in CRL's nextUpdate field
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyErrorInCRLNextUpdateField;

/// condition constant: X.509 certificate verification error - out of memory
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyOutOfMem;

/// condition constant: X.509 certificate verification error - self signed certificate
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyDepthZeroSelfSignedCert;

/// condition constant: X.509 certificate verification error - self signed certificate in certificate chain
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifySelfSignedCertInChain;

/// condition constant: X.509 certificate verification error - unable to get local issuer certificate
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyUnableToGetIssuerCertLocally;

/// condition constant: X.509 certificate verification error - unable to verify the first certificate
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyUnableToVerifyLeafSignature;

/// condition constant: X.509 certificate verification error - certificate chain too long
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyCertChainTooLong;

/// condition constant: X.509 certificate verification error - certificate revoked
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyCertRevoked;

/// condition constant: X.509 certificate verification error - issuer certificate doesn't have a public key
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyNoIssuerPublicKey;

/// condition constant: X.509 certificate verification error - path length constraint exceeded
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyPathLengthExceeded;

/// condition constant: X.509 certificate verification error - unsupported certificate purpose
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyInvalidPurpose;

/// condition constant: X.509 certificate verification error - certificate not trusted
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyCertUntrusted;

/// condition constant: X.509 certificate verification error - certificate rejected
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyCertRejected;

/// condition constant: X.509 certificate verification error - subject issuer mismatch
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifySubjectIssuerMismatch;

/// condition constant: X.509 certificate verification error - authority and subject key identifier mismatch
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyAkidSkidMismatch;

/// condition constant: X.509 certificate verification error - authority and issuer serial number mismatch
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyAkidIssuerSerialMismatch;

/// condition constant: X.509 certificate verification error - key usage does not include certificate signing
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyKeyusageNoCertsign;

/// condition constant: X.509 certificate verification error - unable to get CRL issuer certificate
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyUnableToGetCRLIssuer;

/// condition constant: X.509 certificate verification error - unhandled critical extension
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyUnhandledCriticalExtension;

/// condition constant: X.509 certificate verification error - key usage does not include CRL signing
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyKeyusageNoCRLSign;

/// condition constant: X.509 certificate verification error - unhandled critical CRL extension
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyUnhandledCriticalCRLExtension;

/// condition constant: X.509 certificate verification error - invalid non-CA certificate (has CA markings)
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyInvalidNonCA;

/// condition constant: X.509 certificate verification error - proxy path length constraint exceeded
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyProxyPathLengthExceeded;

/// condition constant: X.509 certificate verification error - key usage does not include digital signature
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyKeyusageNoDigitalSignature;

/// condition constant: X.509 certificate verification error - proxy certificates not allowed, please set the appropriate flag
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyProxyCertificatesNotAllowed;

/// condition constant: X.509 certificate verification error - invalid or inconsistent certificate extension
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyInvalidExtension;

/// condition constant: X.509 certificate verification error - invalid or inconsistent certificate policy extension
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyInvalidPolicyExtension;

/// condition constant: X.509 certificate verification error - no explicit policy
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyNoExplicitPolicy;

/// condition constant: X.509 certificate verification error - Different CRL scope
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyDifferentCRLScope;

/// condition constant: X.509 certificate verification error - Unsupported extension feature
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyUnsupportedExtensionFeature;

/// condition constant: X.509 certificate verification error - RFC 3779 resource not subset of parent's resources
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyUnnestedResource;

/// condition constant: X.509 certificate verification error - permitted subtree violation
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyPermittedViolation;

/// condition constant: X.509 certificate verification error - excluded subtree violation
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyExcludedViolation;

/// condition constant: X.509 certificate verification error - name constraints minimum and maximum not supported
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifySubtreeMinmax;

/// condition constant: X.509 certificate verification error - application verification failure
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyApplicationVerification;

/// condition constant: X.509 certificate verification error - unsupported name constraint type
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyUnsupportedConstraintType;

/// condition constant: X.509 certificate verification error - unsupported or invalid name constraint syntax
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyUnsupportedConstraintSyntax;

/// condition constant: X.509 certificate verification error - unsupported or invalid name syntax
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyUnsupportedNameSyntax;

/// condition constant: X.509 certificate verification error - CRL path validation error
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyCRLPathValidationError;

/// condition constant: X.509 certificate verification error - Path Loop
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyPathLoop;

/// condition constant: X.509 certificate verification error - Suite B: certificate version invalid
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifySuiteBInvalidVersion;

/// condition constant: X.509 certificate verification error - Suite B: invalid public key algorithm
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifySuiteBInvalidAlgorithm;

/// condition constant: X.509 certificate verification error - Suite B: invalid ECC curve
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifySuiteBInvalidCurve;

/// condition constant: X.509 certificate verification error - Suite B: invalid signature algorithm
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifySuiteBInvalidSignatureAlgorithm;

/// condition constant: X.509 certificate verification error - Suite B: curve not allowed for this LOS
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifySuiteBLosNotAllowed;

/// condition constant: X.509 certificate verification error - Suite B: cannot sign P-384 with P-256
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifySuiteBCannotSignP384WithP256;

/// condition constant: X.509 certificate verification error - Hostname mismatch
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyHostnameMismatch;

/// condition constant: X.509 certificate verification error - Email address mismatch
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyEmailMismatch;

/// condition constant: X.509 certificate verification error - IP address mismatch
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyIPAddressMismatch;

/// condition constant: X.509 certificate verification error - No matching DANE TLSA records
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyDANENoMatch;

/// condition constant: X.509 certificate verification error - EE certificate key too weak
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyEEKeyTooSmall;

/// condition constant: X.509 certificate verification error - CA certificate key too weak
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyCAKeyTooSmall;

/// condition constant: X.509 certificate verification error - CA signature digest algorithm too weak
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyCAMDTooWeak;

/// condition constant: X.509 certificate verification error - Invalid certificate verification context
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyInvalidCall;

/// condition constant: X.509 certificate verification error - Issuer certificate lookup error
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyStoreLookup;

/// condition constant: X.509 certificate verification error - Certificate Transparency required, but no valid SCTs found
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyNoValidSCTs;

/// condition constant: X.509 certificate verification error - proxy subject name violation
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyProxySubjectNameViolation;

/// condition constant: X.509 certificate verification error - OCSP verification needed
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyOCSPVerifyNeeded;

/// condition constant: X.509 certificate verification error - OCSP verification failed
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyOCSPVerifyFailed;

/// condition constant: X.509 certificate verification error - OCSP unknown cert
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyOCSPCertUnknown;

/// condition constant: X.509 certificate verification error - Cannot find certificate signature algorithm
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyUnsupportedSignatureAlgorithm;

/// condition constant: X.509 certificate verification error - subject signature algorithm and issuer public key algorithm mismatch
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifySignatureAlgorithmMismatch;

/// condition constant: X.509 certificate verification error - cert info siganature and signature algorithm mismatch
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifySignatureAlgorithmInconsistency;

/// condition constant: X.509 certificate verification error - invalid CA certificate
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyInvalidCA;

/// condition constant: X.509 certificate verification error - Path length invalid for non-CA cert
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyPathlenInvalidForNonCA;

/// condition constant: X.509 certificate verification error - Path length given without key usage keyCertSign
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyPathlenWithoutKUKeyCertSign;

/// condition constant: X.509 certificate verification error - Key usage keyCertSign invalid for non-CA cert
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyKUKeyCertSignInvalidForNonCA;

/// condition constant: X.509 certificate verification error - Issuer name empty
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyIssuerNameEmpty;

/// condition constant: X.509 certificate verification error - Subject name empty
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifySubjectNameEmpty;

/// condition constant: X.509 certificate verification error - Missing Authority Key Identifier
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyMissingAuthorityKeyIdentifier;

/// condition constant: X.509 certificate verification error - Missing Subject Key Identifier
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyMissingSubjectKeyIdentifier;

/// condition constant: X.509 certificate verification error - Empty Subject Alternative Name extension
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyEmptySubjectAltName;

/// condition constant: X.509 certificate verification error - Basic Constraints of CA cert not marked critical
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyEmptySubjectSanNotCritical;

/// condition constant: X.509 certificate verification error - Subject empty and Subject Alt Name extension not critical
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyCABconsNotCritical;

/// condition constant: X.509 certificate verification error - Authority Key Identifier marked critical
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyAuthorityKeyIdentifierCritical;

/// condition constant: X.509 certificate verification error - Subject Key Identifier marked critical
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifySubjectKeyIdentifierCritical;

/// condition constant: X.509 certificate verification error - CA cert does not include key usage extension
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyCACertMissingKeyUsage;

/// condition constant: X.509 certificate verification error - Using cert extension requires at least X509v3
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyExtensionsRequireVersion3;

/// condition constant: X.509 certificate verification error - Certificate public key has explicit ECC parameters
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_X509VerifyECKeyExplicitParams;

// TLS errors and alerts. These use condition codes > DCMTLS_EC_SSL_Offset.

/// condition constant: TLS error: app data in handshake
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSAppDataInHandshake;

/// condition constant: TLS error: no suitable key share
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoSuitableKeyShare;

/// condition constant: TLS error: bad dh value
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadDHValue;

/// condition constant: TLS error: bad change cipher spec
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadChangeCipherSpec;

/// condition constant: TLS error: inconsistent extms
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInconsistentExtms;

/// condition constant: TLS error: bad hello request
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadHelloRequest;

/// condition constant: TLS error: bad data returned by callback
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadDataReturnedByCallback;

/// condition constant: TLS error: bad decompression
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadDecompression;

/// condition constant: TLS error: bad key share
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadKeyShare;

/// condition constant: TLS error: cannot change cipher
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSCannotChangeCipher;

/// condition constant: TLS error: bad extension
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadExtension;

/// condition constant: TLS error: bad digest length
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadDigestLength;

/// condition constant: TLS error: missing sigalgs extension
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSMissingSigalgsExtension;

/// condition constant: TLS error: invalid configuration name
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInvalidConfigurationName;

/// condition constant: TLS error: bad psk identity
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadPSKIdentity;

/// condition constant: TLS error: bad packet length
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadPacketLength;

/// condition constant: TLS error: bad protocol version number
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadProtocolVersionNumber;

/// condition constant: TLS error: ssl command section empty
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLCommandSectionEmpty;

/// condition constant: TLS error: no suitable signature algorithm
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoSuitableSignatureAlgorithm;

/// condition constant: TLS error: bad rsa encrypt
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadRSAEncrypt;

/// condition constant: TLS error: invalid key update type
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInvalidKeyUpdateType;

/// condition constant: TLS error: still in init
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSStillInInit;

/// condition constant: TLS error: bad key update
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadKeyUpdate;

/// condition constant: TLS error: bad signature
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadSignature;

/// condition constant: TLS error: bad ssl filetype
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadSSLFiletype;

/// condition constant: TLS error: ssl command section not found
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLCommandSectionNotFound;

/// condition constant: TLS error: ssl section empty
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLSectionEmpty;

/// condition constant: TLS error: bad write retry
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadWriteRetry;

/// condition constant: TLS error: bio not set
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBioNotSet;

/// condition constant: TLS error: block cipher pad is wrong
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBlockCipherPadIsWrong;

/// condition constant: TLS error: bn lib
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBNLib;

/// condition constant: TLS error: ca dn length mismatch
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSCADNLengthMismatch;

/// condition constant: TLS error: too many key updates
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTooManyKeyUpdates;

/// condition constant: TLS error: ccs received early
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSCCSReceivedEarly;

/// condition constant: TLS error: certificate verify failed
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSCertificateVerifyFailed;

/// condition constant: TLS error: cert length mismatch
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSCertLengthMismatch;

/// condition constant: TLS error: ssl section not found
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLSectionNotFound;

/// condition constant: TLS error: cipher code wrong length
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSCipherCodeWrongLength;

/// condition constant: TLS error: unknown command
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnknownCommand;

/// condition constant: TLS error: compressed length too long
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSCompressedLengthTooLong;

/// condition constant: TLS error: compression failure
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSCompressionFailure;

/// condition constant: TLS error: compression library error
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSCompressionLibraryError;

/// condition constant: TLS error: at least TLS 1.0 needed in FIPS mode
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSAtLeastTLS10NeededInFIPSMode;

/// condition constant: TLS error: connection type not set
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSConnectionTypeNotSet;

/// condition constant: TLS error: data between ccs and finished
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSDataBetweenCCSAndFinished;

/// condition constant: TLS error: data length too long
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSDataLengthTooLong;

/// condition constant: TLS error: decryption failed
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSDecryptionFailed;

/// condition constant: TLS error: dh public value length is wrong
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSDHPublicValueLengthIsWrong;

/// condition constant: TLS error: digest check failed
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSDigestCheckFailed;

/// condition constant: TLS error: encrypted length too long
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSEncryptedLengthTooLong;

/// condition constant: TLS error: error in received cipher list
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSErrorInReceivedCipherList;

/// condition constant: TLS error: excessive message size
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSExcessiveMessageSize;

/// condition constant: TLS error: extra data in message
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSExtraDataInMessage;

/// condition constant: TLS error: got a fin before a ccs
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSGotAFinBeforeACCS;

/// condition constant: TLS error: https proxy request
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSHTTPSProxyRequest;

/// condition constant: TLS error: http request
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSHTTPRequest;

/// condition constant: TLS error: tls invalid ecpointformat list
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSInvalidECPointformatList;

/// condition constant: TLS error: at least (D)TLS 1.2 needed in Suite B mode
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSAtLeastTLS12NeededInSuiteBMode;

/// condition constant: TLS error: length mismatch
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSLengthMismatch;

/// condition constant: TLS error: length too short
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSLengthTooShort;

/// condition constant: TLS error: library has no ciphers
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSLibraryHasNoCiphers;

/// condition constant: TLS error: illegal point compression
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSIllegalPointCompression;

/// condition constant: TLS error: ext length mismatch
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSExtLengthMismatch;

/// condition constant: TLS error: too much early data
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTooMuchEarlyData;

/// condition constant: TLS error: missing dsa signing cert
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSMissingDSASigningCert;

/// condition constant: TLS error: version too high
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSVersionTooHigh;

/// condition constant: TLS error: context not dane enabled
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSContextNotDANEEnabled;

/// condition constant: TLS error: missing rsa certificate
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSMissingRSACertificate;

/// condition constant: TLS error: missing rsa encrypting cert
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSMissingRSAEncryptingCert;

/// condition constant: TLS error: missing rsa signing cert
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSMissingRSASigningCert;

/// condition constant: TLS error: missing tmp dh key
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSMissingTmpDHKey;

/// condition constant: TLS error: dane already enabled
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSDANEAlreadyEnabled;

/// condition constant: TLS error: dane cannot override mtype full
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSDANECannotOverrideMtypeFull;

/// condition constant: TLS error: invalid max early data
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInvalidMaxEarlyData;

/// condition constant: TLS error: dane not enabled
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSDANENotEnabled;

/// condition constant: TLS error: no certificates returned
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoCertificatesReturned;

/// condition constant: TLS error: no certificate assigned
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoCertificateAssigned;

/// condition constant: TLS error: unexpected end of early data
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnexpectedEndOfEarlyData;

/// condition constant: TLS error: no certificate set
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoCertificateSet;

/// condition constant: TLS error: dane tlsa bad certificate
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSDANETLSABadCertificate;

/// condition constant: TLS error: no ciphers available
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoCiphersAvailable;

/// condition constant: TLS error: not on record boundary
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNotOnRecordBoundary;

/// condition constant: TLS error: no ciphers specified
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoCiphersSpecified;

/// condition constant: TLS error: dane tlsa bad certificate usage
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSDANETLSABadCertificateUsage;

/// condition constant: TLS error: no cipher match
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoCipherMatch;

/// condition constant: TLS error: bad cipher
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadCipher;

/// condition constant: TLS error: no compression specified
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoCompressionSpecified;

/// condition constant: TLS error: no method specified
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoMethodSpecified;

/// condition constant: TLS error: dane tlsa bad data length
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSDANETLSABadDataLength;

/// condition constant: TLS error: no private key assigned
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoPrivateKeyAssigned;

/// condition constant: TLS error: no protocols available
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoProtocolsAvailable;

/// condition constant: TLS error: dane tlsa bad digest length
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSDANETLSABadDigestLength;

/// condition constant: TLS error: no shared cipher
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoSharedCipher;

/// condition constant: TLS error: exceeds max fragment size
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSExceedsMaxFragmentSize;

/// condition constant: TLS error: null ssl ctx
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNullSSLCtx;

/// condition constant: TLS error: null ssl method passed
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNullSSLMethodPassed;

/// condition constant: TLS error: old session cipher not returned
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSOldSessionCipherNotReturned;

/// condition constant: TLS error: packet length too long
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSPacketLengthTooLong;

/// condition constant: TLS error: peer did not return a certificate
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSPeerDidNotReturnACertificate;

/// condition constant: TLS error: dane tlsa bad matching type
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSDANETLSABadMatchingType;

/// condition constant: TLS error: dane tlsa bad public key
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSDANETLSABadPublicKey;

/// condition constant: TLS error: dane tlsa bad selector
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSDANETLSABadSelector;

/// condition constant: TLS error: dane tlsa null data
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSDANETLSANullData;

/// condition constant: TLS error: error setting tlsa base domain
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSErrorSettingTLSABaseDomain;

/// condition constant: TLS error: invalid alert
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInvalidAlert;

/// condition constant: TLS error: custom ext handler already installed
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSCustomExtHandlerAlreadyInstalled;

/// condition constant: TLS error: protocol is shutdown
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSProtocolIsShutdown;

/// condition constant: TLS error: sct verification failed
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSCTVerificationFailed;

/// condition constant: TLS error: missing supported groups extension
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSMissingSupportedGroupsExtension;

/// condition constant: TLS error: ssl session version mismatch
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLSessionVersionMismatch;

/// condition constant: TLS error: read bio not set
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSReadBioNotSet;

/// condition constant: TLS error: invalid ct validation type
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInvalidCTValidationType;

/// condition constant: TLS error: record length mismatch
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSRecordLengthMismatch;

/// condition constant: TLS error: no change following hrr
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoChangeFollowingHRR;

/// condition constant: TLS error: required cipher missing
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSRequiredCipherMissing;

/// condition constant: TLS error: no valid scts
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoValidSCTs;

/// condition constant: TLS error: unsolicited extension
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnsolicitedExtension;

/// condition constant: TLS error: ciphersuite digest has changed
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSCiphersuiteDigestHasChanged;

/// condition constant: TLS error: bad psk
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadPSK;

/// condition constant: TLS error: signature for non signing certificate
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSignatureForNonSigningCertificate;

/// condition constant: TLS error: missing signing cert
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSMissingSigningCert;

/// condition constant: TLS error: inconsistent early data alpn
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInconsistentEarlyDataAlpn;

/// condition constant: TLS error: psk identity not found
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSPSKIdentityNotFound;

/// condition constant: TLS error: psk no client cb
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSPSKNoClientCb;

/// condition constant: TLS error: psk no server cb
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSPSKNoServerCb;

/// condition constant: TLS error: clienthello tlsext
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSClienthelloTlsext;

/// condition constant: TLS error: parse tlsext
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSParseTlsext;

/// condition constant: TLS error: ssl ctx has no default ssl version
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLCtxHasNoDefaultSSLVersion;

/// condition constant: TLS error: ssl handshake failure
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLHandshakeFailure;

/// condition constant: TLS error: ssl library has no ciphers
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLLibraryHasNoCiphers;

/// condition constant: TLS error: inconsistent early data sni
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInconsistentEarlyDataSNI;

/// condition constant: TLS error: ssl3 ext invalid max fragment length
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSL3ExtInvalidMaxFragmentLength;

/// condition constant: TLS error: bad early data
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadEarlyData;

/// condition constant: TLS error: callback failed
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSCallbackFailed;

/// condition constant: TLS error: no application protocol
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoApplicationProtocol;

/// condition constant: TLS error: bad handshake state
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadHandshakeState;

/// condition constant: TLS error: overflow error
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSOverflowError;

/// condition constant: TLS error: invalid certificate or alg
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInvalidCertificateOrAlg;

/// condition constant: TLS error: unable to find public key parameters
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnableToFindPublicKeyParameters;

/// condition constant: TLS error: bad packet
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadPacket;

/// condition constant: TLS error: insufficient security
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInsufficientSecurity;

/// condition constant: TLS error: unable to load ssl3 md5 routines
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnableToLoadSSL3MD5Routines;

/// condition constant: TLS error: unable to load ssl3 sha1 routines
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnableToLoadSSL3SHA1Routines;

/// condition constant: TLS error: unexpected message
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnexpectedMessage;

/// condition constant: TLS error: unexpected record
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnexpectedRecord;

/// condition constant: TLS error: unknown alert type
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnknownAlertType;

/// condition constant: TLS error: unknown certificate type
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnknownCertificateType;

/// condition constant: TLS error: unknown cipher returned
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnknownCipherReturned;

/// condition constant: TLS error: unknown cipher type
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnknownCipherType;

/// condition constant: TLS error: unknown key exchange type
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnknownKeyExchangeType;

/// condition constant: TLS error: unknown pkey type
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnknownPkeyType;

/// condition constant: TLS error: unknown protocol
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnknownProtocol;

/// condition constant: TLS error: binder does not verify
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBinderDoesNotVerify;

/// condition constant: TLS error: unknown ssl version
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnknownSSLVersion;

/// condition constant: TLS error: unknown state
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnknownState;

/// condition constant: TLS error: missing fatal
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSMissingFatal;

/// condition constant: TLS error: unsupported compression algorithm
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnsupportedCompressionAlgorithm;

/// condition constant: TLS error: unsupported protocol
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnsupportedProtocol;

/// condition constant: TLS error: unsupported ssl version
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnsupportedSSLVersion;

/// condition constant: TLS error: invalid ccs message
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInvalidCCSMessage;

/// condition constant: TLS error: wrong cipher returned
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSWrongCipherReturned;

/// condition constant: TLS error: unexpected ccs message
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnexpectedCCSMessage;

/// condition constant: TLS error: bad hrr version
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadHrrVersion;

/// condition constant: TLS error: wrong signature length
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSWrongSignatureLength;

/// condition constant: TLS error: wrong signature size
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSWrongSignatureSize;

/// condition constant: TLS error: wrong ssl version
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSWrongSSLVersion;

/// condition constant: TLS error: wrong version number
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSWrongVersionNumber;

/// condition constant: TLS error: x509 lib
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSX509Lib;

/// condition constant: TLS error: x509 verification setup problems
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSX509VerificationSetupProblems;

/// condition constant: TLS error: path too long
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSPathTooLong;

/// condition constant: TLS error: bad length
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadLength;

/// condition constant: TLS error: attempt to reuse session in different context
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSAttemptToReuseSessionInDifferentContext;

/// condition constant: TLS error: ssl session id context too long
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLSessionIDContextTooLong;

/// condition constant: TLS error: library bug
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSLibraryBug;

/// condition constant: TLS error: serverhello tlsext
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSServerhelloTlsext;

/// condition constant: TLS error: uninitialized
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUninitialized;

/// condition constant: TLS error: session id context uninitialized
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSessionIDContextUninitialized;

/// condition constant: TLS error: post handshake auth encoding err
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSPostHandshakeAuthEncodingErr;

/// condition constant: TLS error: extension not received
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSExtensionNotReceived;

/// condition constant: TLS error: invalid command
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInvalidCommand;

/// condition constant: TLS error: decryption failed or bad record mac
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSDecryptionFailedOrBadRecordMac;

/// condition constant: TLS error: invalid context
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInvalidContext;

/// condition constant: TLS error: invalid config
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInvalidConfig;

/// condition constant: TLS error: not server
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNotServer;

/// condition constant: TLS error: request pending
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSRequestPending;

/// condition constant: TLS error: request sent
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSRequestSent;

/// condition constant: TLS error: no cookie callback set
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoCookieCallbackSet;

/// condition constant: TLS error: private key mismatch
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSPrivateKeyMismatch;

/// condition constant: TLS error: not replacing certificate
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNotReplacingCertificate;

/// condition constant: TLS error: missing parameters
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSMissingParameters;

/// condition constant: TLS error: application data after close notify
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSApplicationDataAfterCloseNotify;

/// condition constant: TLS error: bad legacy version
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadLegacyVersion;

/// condition constant: TLS error: mixed handshake and non handshake data
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSMixedHandshakeAndNonHandshakeData;

/// condition constant: TLS error: unexpected eof while reading
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnexpectedEofWhileReading;

/// condition constant: TLS error: no suitable groups
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoSuitableGroups;

/// condition constant: TLS error: copy parameters failed
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSCopyParametersFailed;

/// condition constant: TLS error: no suitable digest algorithm
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoSuitableDigestAlgorithm;

/// condition constant: TLS error: record too small
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSRecordTooSmall;

/// condition constant: TLS error: cannot get group name
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSCannotGetGroupName;

/// condition constant: TLS error: ssl3 session id too long
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSL3SessionIDTooLong;

/// condition constant: TLS error: ssl session id callback failed
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLSessionIDCallbackFailed;

/// condition constant: TLS error: ssl session id conflict
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLSessionIDConflict;

/// condition constant: TLS error: ssl session id has bad length
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLSessionIDHasBadLength;

/// condition constant: TLS error: bad ecc cert
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadECCCert;

/// condition constant: TLS error: bad ecpoint
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadEcpoint;

/// condition constant: TLS error: compression id not within private range
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSCompressionIDNotWithinPrivateRange;

/// condition constant: TLS error: cookie mismatch
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSCookieMismatch;

/// condition constant: TLS error: duplicate compression id
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSDuplicateCompressionId;

/// condition constant: TLS error: missing tmp ecdh key
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSMissingTmpECDHKey;

/// condition constant: TLS error: read timeout expired
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSReadTimeoutExpired;

/// condition constant: TLS error: unable to find ecdh parameters
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnableToFindECDHParameters;

/// condition constant: TLS error: unsupported elliptic curve
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnsupportedEllipticCurve;

/// condition constant: TLS error: ecc cert not for signing
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSECCCertNotForSigning;

/// condition constant: TLS error: ssl3 ext invalid servername
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSL3ExtInvalidServername;

/// condition constant: TLS error: ssl3 ext invalid servername type
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSL3ExtInvalidServernameType;

/// condition constant: TLS error: no required digest
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoRequiredDigest;

/// condition constant: TLS error: invalid ticket keys length
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInvalidTicketKeysLength;

/// condition constant: TLS error: invalid status response
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInvalidStatusResponse;

/// condition constant: TLS error: unsupported status type
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnsupportedStatusType;

/// condition constant: TLS error: Peer haven't sent GOST certificate, required for selected ciphersuite
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoGostCertificateSentByPeer;

/// condition constant: TLS error: no client cert method
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoClientCertMethod;

/// condition constant: TLS error: bad handshake length
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadHandshakeLength;

/// condition constant: TLS error: dtls message too big
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSDTLSMessageTooBig;

/// condition constant: TLS error: renegotiate ext too long
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSRenegotiateExtTooLong;

/// condition constant: TLS error: renegotiation encoding err
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSRenegotiationEncodingErr;

/// condition constant: TLS error: renegotiation mismatch
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSRenegotiationMismatch;

/// condition constant: TLS error: unsafe legacy renegotiation disabled
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnsafeLegacyRenegotiationDisabled;

/// condition constant: TLS error: no renegotiation
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoRenegotiation;

/// condition constant: TLS error: inconsistent compression
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInconsistentCompression;

/// condition constant: TLS error: invalid compression algorithm
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInvalidCompressionAlgorithm;

/// condition constant: TLS error: required compression algorithm missing
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSRequiredCompressionAlgorithmMissing;

/// condition constant: TLS error: compression disabled
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSCompressionDisabled;

/// condition constant: TLS error: old session compression algorithm not returned
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSOldSessionCompressionAlgorithmNotReturned;

/// condition constant: TLS error: scsv received when renegotiating
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSCSVReceivedWhenRenegotiating;

/// condition constant: TLS error: bad srp a length
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadSRPALength;

/// condition constant: TLS error: bad srtp mki value
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadSrtpMkiValue;

/// condition constant: TLS error: bad srtp protection profile list
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadSrtpProtectionProfileList;

/// condition constant: TLS error: empty srtp protection profile list
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSEmptySrtpProtectionProfileList;

/// condition constant: TLS error: invalid srp username
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInvalidSrpUsername;

/// condition constant: TLS error: can't find SRP server param
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSMissingSrpParam;

/// condition constant: TLS error: no srtp profiles
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoSrtpProfiles;

/// condition constant: TLS error: signature algorithms error
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSignatureAlgorithmsError;

/// condition constant: TLS error: error with the srp params
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSrpACalc;

/// condition constant: TLS error: srtp could not allocate profiles
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSrtpCouldNotAllocateProfiles;

/// condition constant: TLS error: srtp protection profile list too long
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSrtpProtectionProfileListTooLong;

/// condition constant: TLS error: srtp unknown protection profile
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSrtpUnknownProtectionProfile;

/// condition constant: TLS error: tls illegal exporter label
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSIllegalExporterLabel;

/// condition constant: TLS error: unknown digest
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnknownDigest;

/// condition constant: TLS error: use srtp not negotiated
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUseSrtpNotNegotiated;

/// condition constant: TLS error: wrong signature type
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSWrongSignatureType;

/// condition constant: TLS error: bad srp parameters
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadSrpParameters;

/// condition constant: TLS error: ssl negative length
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLNegativeLength;

/// condition constant: TLS error: inappropriate fallback
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInappropriateFallback;

/// condition constant: TLS error: ecdh required for suiteb mode
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSECDHRequiredForSuiteBMode;

/// condition constant: TLS error: no shared signature algorithms
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoSharedSignatureAlgorithms;

/// condition constant: TLS error: cert cb error
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSCertCbError;

/// condition constant: TLS error: wrong curve
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSWrongCurve;

/// condition constant: TLS error: illegal Suite B digest
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSIllegalSuiteBDigest;

/// condition constant: TLS error: missing ecdsa signing cert
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSMissingECDSASigningCert;

/// condition constant: TLS error: wrong certificate type
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSWrongCertificateType;

/// condition constant: TLS error: bad value
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadValue;

/// condition constant: TLS error: invalid null cmd name
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInvalidNullCmdName;

/// condition constant: TLS error: unknown cmd name
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSUnknownCmdName;

/// condition constant: TLS error: invalid serverinfo data
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInvalidServerinfoData;

/// condition constant: TLS error: no pem extensions
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoPEMExtensions;

/// condition constant: TLS error: bad data
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadData;

/// condition constant: TLS error: pem name bad prefix
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSPEMNameBadPrefix;

/// condition constant: TLS error: pem name too short
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSPEMNameTooShort;

/// condition constant: TLS error: dh key too small
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSDHKeyTooSmall;

/// condition constant: TLS error: version too low
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSVersionTooLow;

/// condition constant: TLS error: ca key too small
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSCAKeyTooSmall;

/// condition constant: TLS error: ca md too weak
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSCAMDTooWeak;

/// condition constant: TLS error: ee key too small
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSEeKeyTooSmall;

/// condition constant: TLS error: cookie gen callback failure
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSCookieGenCallbackFailure;

/// condition constant: TLS error: fragmented client hello
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSFragmentedClientHello;

/// condition constant: TLS error: invalid sequence number
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInvalidSequenceNumber;

/// condition constant: TLS error: no verify cookie callback
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoVerifyCookieCallback;

/// condition constant: TLS error: length too long
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSLengthTooLong;

/// condition constant: TLS error: failed to init async
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSFailedToInitAsync;

/// condition constant: TLS error: pipeline failure
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSPipelineFailure;

/// condition constant: TLS error: shutdown while in init
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSShutdownWhileInInit;

/// condition constant: TLS error: ssl session id too long
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLSessionIDTooLong;

/// condition constant: TLS error: too many warn alerts
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTooManyWarnAlerts;

/// condition constant: TLS error: no shared groups
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSNoSharedGroups;

/// condition constant: TLS error: bad record type
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSBadRecordType;

/// condition constant: TLS error: invalid session id
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSInvalidSessionId;

/// condition constant: TLS alert: unexpected message
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLv3AlertUnexpectedMessage;

/// condition constant: TLS alert: bad record mac
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLv3AlertBadRecordMac;

/// condition constant: TLS alert: decryption failed
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSv1AlertDecryptionFailed;

/// condition constant: TLS alert: record overflow
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSv1AlertRecordOverflow;

/// condition constant: TLS alert: decompression failure
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLv3AlertDecompressionFailure;

/// condition constant: TLS alert: handshake failure
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLv3AlertHandshakeFailure;

/// condition constant: TLS alert: no certificate
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLv3AlertNoCertificate;

/// condition constant: TLS alert: bad certificate
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLv3AlertBadCertificate;

/// condition constant: TLS alert: unsupported certificate
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLv3AlertUnsupportedCertificate;

/// condition constant: TLS alert: certificate revoked
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLv3AlertCertificateRevoked;

/// condition constant: TLS alert: certificate expired
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLv3AlertCertificateExpired;

/// condition constant: TLS alert: certificate unknown
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLv3AlertCertificateUnknown;

/// condition constant: TLS alert: illegal parameter
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSSSLv3AlertIllegalParameter;

/// condition constant: TLS alert: unknown ca,
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSv1AlertUnknownCa;

/// condition constant: TLS alert: access denied
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSv1AlertAccessDenied;

/// condition constant: TLS alert: decode error,
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSv1AlertDecodeError;

/// condition constant: TLS alert: decrypt error
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSv1AlertDecryptError;

/// condition constant: TLS alert: export restriction
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSv1AlertExportRestriction;

/// condition constant: TLS alert: protocol version
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSv1AlertProtocolVersion;

/// condition constant: TLS alert: insufficient security
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSv1AlertInsufficientSecurity;

/// condition constant: TLS alert: internal error
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSv1AlertInternalError;

/// condition constant: TLS alert: inappropriate fallback
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSv1AlertInappropriateFallback;

/// condition constant: TLS alert: user cancelled
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSv1AlertUserCancelled;

/// condition constant: TLS alert: no renegotiation
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSv1AlertNoRenegotiation;

/// condition constant: TLS alert: missing extension
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSv13AlertMissingExtension;

/// condition constant: TLS error: unsupported extension
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSv1UnsupportedExtension;

/// condition constant: TLS error: certificate unobtainable
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSv1CertificateUnobtainable;

/// condition constant: TLS error: unrecognized name
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSv1UnrecognizedName;

/// condition constant: TLS error: bad certificate status response
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSv1BadCertificateStatusResponse;

/// condition constant: TLS error: bad certificate hash value
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSv1BadCertificateHashValue;

/// condition constant: TLS alert: certificate required
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_TLSTLSv13AlertCertificateRequired;

#endif // TLSCOND_H
