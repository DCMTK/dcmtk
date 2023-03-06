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
 *  Module:  dcmtls
 *
 *  Author:  Jan Schlamelcher, Marco Eichelberg
 *
 *  Purpose: error conditions for DcmTLS
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmtls/tlscond.h"

#ifdef WITH_OPENSSL
BEGIN_EXTERN_C
#include <openssl/x509_vfy.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
END_EXTERN_C
#endif

// instantiation of pure generic error codes

makeOFConditionConst( DCMTLS_EC_FailedToCreateTLSTransportLayer,      OFM_dcmtls,  1, OF_error, "Unable to create TLS transport layer" );
makeOFConditionConst( DCMTLS_EC_FailedToSetCiphersuites,              OFM_dcmtls,  2, OF_error, "Unable to set selected cipher suites" );
// error codes 3..8 are used by the specializable error codes defined below
makeOFConditionConst( DCMTLS_EC_NoTLSTransportConnectionPresent,      OFM_dcmtls,  9, OF_error, "TLS function called but no TLS transport connection present" );
makeOFConditionConst( DCMTLS_EC_CiphersuiteNotAllowed,                OFM_dcmtls, 10, OF_error, "Ciphersuite not allowed in current DICOM security profile" );
makeOFConditionConst( DCMTLS_EC_TLSReadOperationDidNotComplete,       OFM_dcmtls, 11, OF_error, "TLS read operation did not complete (SSL_ERROR_WANT_READ)" );
makeOFConditionConst( DCMTLS_EC_TLSWriteOperationDidNotComplete,      OFM_dcmtls, 12, OF_error, "TLS write operation did not complete (SSL_ERROR_WANT_WRITE)" );
makeOFConditionConst( DCMTLS_EC_TLSX509LookupOperationDidNotComplete, OFM_dcmtls, 13, OF_error, "TLS X.509 lookup operation did not complete (SSL_ERROR_WANT_X509_LOOKUP)" );
makeOFConditionConst( DCMTLS_EC_TLSConnectionClosedByPeer,            OFM_dcmtls, 14, OF_error, "TLS connection closed by peer (SSL_ERROR_ZERO_RETURN)" );
makeOFConditionConst( DCMTLS_EC_TLSConnectOperationDidNotComplete,    OFM_dcmtls, 15, OF_error, "TLS connect operation did not complete (SSL_ERROR_WANT_CONNECT)" );
makeOFConditionConst( DCMTLS_EC_TLSAcceptOperationDidNotComplete,     OFM_dcmtls, 16, OF_error, "TLS accept operation did not complete (SSL_ERROR_WANT_ACCEPT)" );
makeOFConditionConst( DCMTLS_EC_TLSAsyncOperationDidNotComplete,      OFM_dcmtls, 17, OF_error, "TLS operation did not complete because an asynchronous engine is still processing data (SSL_ERROR_WANT_ASYNC)" );
makeOFConditionConst( DCMTLS_EC_TLSAsyncJobCouldNotBeStarted,         OFM_dcmtls, 18, OF_error, "TLS asynchronous job could not be started (SSL_ERROR_WANT_ASYNC_JOB)" );
makeOFConditionConst( DCMTLS_EC_TLSClientHelloCallbackNeeded,         OFM_dcmtls, 19, OF_error, "TLS client hello callback needed (SSL_ERROR_WANT_CLIENT_HELLO_CB)" );
makeOFConditionConst( DCMTLS_EC_OtherSSLError,                        OFM_dcmtls, 20, OF_error, "Other TLS error (unknown SSL_ERROR code)" );
makeOFConditionConst( DCMTLS_EC_FailedToSetVerificationMode,          OFM_dcmtls, 21, OF_error, "Setting of the X.509 verification mode failed" );
makeOFConditionConst( DCMTLS_EC_CertStoreCtxInitFailed,               OFM_dcmtls, 22, OF_error, "Certificate store context initialization failed" );
makeOFConditionConst( DCMTLS_EC_OpenSSLIOError,                       OFM_dcmtls, 23, OF_error, "OpenSSL I/O error (SSL_ERROR_SYSCALL)" );
makeOFConditionConst( DCMTLS_EC_TLSProfileNotSupported,               OFM_dcmtls, 24, OF_error, "DICOM TLS Secure Transport Connection Profile not supported" );

// implementation of specializable error codes

DcmTLSECUnknownCiphersuiteConst::DcmTLSECUnknownCiphersuiteConst()
{
    theModule = OFM_dcmtls;
    theCode = 3;
    theStatus = OF_error;
    theText = "Unknown or invalid ciphersuite";
}

OFCondition DcmTLSECUnknownCiphersuiteConst::operator()( const char* ciphersuiteName ) const
{
    if( !ciphersuiteName )
        return *this;
    OFOStringStream os;
    os << theText << ": " << ciphersuiteName;
    OFCondition cond;
    OFSTRINGSTREAM_GETSTR( os, c )
    cond = makeOFCondition( theModule, theCode, theStatus, c );
    OFSTRINGSTREAM_FREESTR( c )
    return cond;
}

DcmTLSECFailedToLoadPrivateKeyConst::DcmTLSECFailedToLoadPrivateKeyConst()
{
    theModule = OFM_dcmtls;
    theCode = 4;
    theStatus = OF_error;
    theText = "Unable to load private TLS key";
}

OFCondition DcmTLSECFailedToLoadPrivateKeyConst::operator()( const char* privateKeyFile ) const
{
    if( !privateKeyFile )
        return *this;
    OFOStringStream os;
    os << theText << " from '" << privateKeyFile << '\'';
    OFCondition cond;
    OFSTRINGSTREAM_GETSTR( os, c )
    cond = makeOFCondition( theModule, theCode, theStatus, c );
    OFSTRINGSTREAM_FREESTR( c )
    return cond;
}

DcmTLSECFailedToLoadCertificateConst::DcmTLSECFailedToLoadCertificateConst()
{
    theModule = OFM_dcmtls;
    theCode = 5;
    theStatus = OF_error;
    theText = "Unable to load certificate";
}

OFCondition DcmTLSECFailedToLoadCertificateConst::operator()( const char* certificateFile ) const
{
    if( !certificateFile )
        return *this;
    OFOStringStream os;
    os << theText << " from '" << certificateFile << '\'';
    OFCondition cond;
    OFSTRINGSTREAM_GETSTR( os, c )
    cond = makeOFCondition( theModule, theCode, theStatus, c );
    OFSTRINGSTREAM_FREESTR( c )
    return cond;
}

DcmTLSECMismatchedPrivateKeyAndCertificateConst::DcmTLSECMismatchedPrivateKeyAndCertificateConst()
{
    theModule = OFM_dcmtls;
    theCode = 6;
    theStatus = OF_error;
    theText = "Private key and certificate do not match";
}

OFCondition DcmTLSECMismatchedPrivateKeyAndCertificateConst::operator()( const char* privateKeyFile,
                                                                         const char* certificateFile ) const
{
    if( !privateKeyFile || !certificateFile )
        return *this;
    OFOStringStream os;
    os << "Private key '" << privateKeyFile << "' and certificate '" << certificateFile << "' do not match";
    OFCondition cond;
    OFSTRINGSTREAM_GETSTR( os, c )
    cond = makeOFCondition( theModule, theCode, theStatus, c );
    OFSTRINGSTREAM_FREESTR( c )
    return cond;
}

DcmTLSECFailedToWriteRandomSeedFileConst::DcmTLSECFailedToWriteRandomSeedFileConst()
{
    theModule = OFM_dcmtls;
    theCode = 7;
    theStatus = OF_error;
    theText = "Cannot write random seed";
}

OFCondition DcmTLSECFailedToWriteRandomSeedFileConst::operator()( const char* seedFile ) const
{
    if( !seedFile )
        return *this;
    OFOStringStream os;
    os << theText << " '" << seedFile << '\'';
    OFCondition cond;
    OFSTRINGSTREAM_GETSTR( os, c )
    cond = makeOFCondition( theModule, theCode, theStatus, c );
    OFSTRINGSTREAM_FREESTR( c )
    return cond;
}

DcmTLSECGenericOpenSSLError::DcmTLSECGenericOpenSSLError()
{
    theModule = OFM_dcmtls;
    theCode = 8;
    theStatus = OF_error;
    theText = "OpenSSL error";
}

OFCondition DcmTLSECGenericOpenSSLError::operator()(unsigned long errorCode) const
{
#ifdef WITH_OPENSSL
    const char *ls = ERR_lib_error_string(errorCode);
    if (ls == NULL) ls = "unknown library";

    const char *rs = ERR_reason_error_string(errorCode);
    if (rs == NULL) rs = "unknown error";
#else
    const char *ls = "unknown library";
    const char *rs = "unknown error";
#endif

    OFOStringStream os;
    os << theText << " "
       << STD_NAMESPACE hex << STD_NAMESPACE setfill('0') << STD_NAMESPACE setw(8) << errorCode
       << ": " << ls << ": " << rs;

    OFCondition cond;
    OFSTRINGSTREAM_GETSTR( os, c )
    cond = makeOFCondition( theModule, theCode, theStatus, c );
    OFSTRINGSTREAM_FREESTR( c )
    return cond;
}

// instantiation of specializable error codes

const DcmTLSECUnknownCiphersuiteConst DCMTLS_EC_UnknownCiphersuite;
const DcmTLSECFailedToLoadPrivateKeyConst DCMTLS_EC_FailedToLoadPrivateKey;
const DcmTLSECFailedToLoadCertificateConst DCMTLS_EC_FailedToLoadCertificate;
const DcmTLSECMismatchedPrivateKeyAndCertificateConst DCMTLS_EC_MismatchedPrivateKeyAndCertificate;
const DcmTLSECFailedToWriteRandomSeedFileConst DCMTLS_EC_FailedToWriteRandomSeedFile;
const DcmTLSECGenericOpenSSLError DCMTLS_EC_GenericOpenSSLError;

// define all constants that may be missing.

// the following defines are present in OpenSSL 1.0.1

#ifndef X509_V_ERR_UNSPECIFIED
#define X509_V_ERR_UNSPECIFIED                          1
#endif
#ifndef X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT
#define X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT            2
#endif
#ifndef X509_V_ERR_UNABLE_TO_GET_CRL
#define X509_V_ERR_UNABLE_TO_GET_CRL                    3
#endif
#ifndef X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE
#define X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE     4
#endif
#ifndef X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE
#define X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE      5
#endif
#ifndef X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY
#define X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY   6
#endif
#ifndef X509_V_ERR_CERT_SIGNATURE_FAILURE
#define X509_V_ERR_CERT_SIGNATURE_FAILURE               7
#endif
#ifndef X509_V_ERR_CRL_SIGNATURE_FAILURE
#define X509_V_ERR_CRL_SIGNATURE_FAILURE                8
#endif
#ifndef X509_V_ERR_CERT_NOT_YET_VALID
#define X509_V_ERR_CERT_NOT_YET_VALID                   9
#endif
#ifndef X509_V_ERR_CERT_HAS_EXPIRED
#define X509_V_ERR_CERT_HAS_EXPIRED                     10
#endif
#ifndef X509_V_ERR_CRL_NOT_YET_VALID
#define X509_V_ERR_CRL_NOT_YET_VALID                    11
#endif
#ifndef X509_V_ERR_CRL_HAS_EXPIRED
#define X509_V_ERR_CRL_HAS_EXPIRED                      12
#endif
#ifndef X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD
#define X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD       13
#endif
#ifndef X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD
#define X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD        14
#endif
#ifndef X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD
#define X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD       15
#endif
#ifndef X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD
#define X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD       16
#endif
#ifndef X509_V_ERR_OUT_OF_MEM
#define X509_V_ERR_OUT_OF_MEM                           17
#endif
#ifndef X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT
#define X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT          18
#endif
#ifndef X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN
#define X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN            19
#endif
#ifndef X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY
#define X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY    20
#endif
#ifndef X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE
#define X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE      21
#endif
#ifndef X509_V_ERR_CERT_CHAIN_TOO_LONG
#define X509_V_ERR_CERT_CHAIN_TOO_LONG                  22
#endif
#ifndef X509_V_ERR_CERT_REVOKED
#define X509_V_ERR_CERT_REVOKED                         23
#endif
#ifndef X509_V_ERR_INVALID_CA
#define X509_V_ERR_INVALID_CA                           24
#endif
#ifndef X509_V_ERR_PATH_LENGTH_EXCEEDED
#define X509_V_ERR_PATH_LENGTH_EXCEEDED                 25
#endif
#ifndef X509_V_ERR_INVALID_PURPOSE
#define X509_V_ERR_INVALID_PURPOSE                      26
#endif
#ifndef X509_V_ERR_CERT_UNTRUSTED
#define X509_V_ERR_CERT_UNTRUSTED                       27
#endif
#ifndef X509_V_ERR_CERT_REJECTED
#define X509_V_ERR_CERT_REJECTED                        28
#endif
#ifndef X509_V_ERR_SUBJECT_ISSUER_MISMATCH
#define X509_V_ERR_SUBJECT_ISSUER_MISMATCH              29
#endif
#ifndef X509_V_ERR_AKID_SKID_MISMATCH
#define X509_V_ERR_AKID_SKID_MISMATCH                   30
#endif
#ifndef X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH
#define X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH          31
#endif
#ifndef X509_V_ERR_KEYUSAGE_NO_CERTSIGN
#define X509_V_ERR_KEYUSAGE_NO_CERTSIGN                 32
#endif
#ifndef X509_V_ERR_UNABLE_TO_GET_CRL_ISSUER
#define X509_V_ERR_UNABLE_TO_GET_CRL_ISSUER             33
#endif
#ifndef X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION
#define X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION         34
#endif
#ifndef X509_V_ERR_KEYUSAGE_NO_CRL_SIGN
#define X509_V_ERR_KEYUSAGE_NO_CRL_SIGN                 35
#endif
#ifndef X509_V_ERR_UNHANDLED_CRITICAL_CRL_EXTENSION
#define X509_V_ERR_UNHANDLED_CRITICAL_CRL_EXTENSION     36
#endif
#ifndef X509_V_ERR_INVALID_NON_CA
#define X509_V_ERR_INVALID_NON_CA                       37
#endif
#ifndef X509_V_ERR_PROXY_PATH_LENGTH_EXCEEDED
#define X509_V_ERR_PROXY_PATH_LENGTH_EXCEEDED           38
#endif
#ifndef X509_V_ERR_KEYUSAGE_NO_DIGITAL_SIGNATURE
#define X509_V_ERR_KEYUSAGE_NO_DIGITAL_SIGNATURE        39
#endif
#ifndef X509_V_ERR_PROXY_CERTIFICATES_NOT_ALLOWED
#define X509_V_ERR_PROXY_CERTIFICATES_NOT_ALLOWED       40
#endif
#ifndef X509_V_ERR_INVALID_EXTENSION
#define X509_V_ERR_INVALID_EXTENSION                    41
#endif
#ifndef X509_V_ERR_INVALID_POLICY_EXTENSION
#define X509_V_ERR_INVALID_POLICY_EXTENSION             42
#endif
#ifndef X509_V_ERR_NO_EXPLICIT_POLICY
#define X509_V_ERR_NO_EXPLICIT_POLICY                   43
#endif
#ifndef X509_V_ERR_DIFFERENT_CRL_SCOPE
#define X509_V_ERR_DIFFERENT_CRL_SCOPE                  44
#endif
#ifndef X509_V_ERR_UNSUPPORTED_EXTENSION_FEATURE
#define X509_V_ERR_UNSUPPORTED_EXTENSION_FEATURE        45
#endif
#ifndef X509_V_ERR_UNNESTED_RESOURCE
#define X509_V_ERR_UNNESTED_RESOURCE                    46
#endif
#ifndef X509_V_ERR_PERMITTED_VIOLATION
#define X509_V_ERR_PERMITTED_VIOLATION                  47
#endif
#ifndef X509_V_ERR_EXCLUDED_VIOLATION
#define X509_V_ERR_EXCLUDED_VIOLATION                   48
#endif
#ifndef X509_V_ERR_SUBTREE_MINMAX
#define X509_V_ERR_SUBTREE_MINMAX                       49
#endif
#ifndef X509_V_ERR_APPLICATION_VERIFICATION
#define X509_V_ERR_APPLICATION_VERIFICATION             50
#endif
#ifndef X509_V_ERR_UNSUPPORTED_CONSTRAINT_TYPE
#define X509_V_ERR_UNSUPPORTED_CONSTRAINT_TYPE          51
#endif
#ifndef X509_V_ERR_UNSUPPORTED_CONSTRAINT_SYNTAX
#define X509_V_ERR_UNSUPPORTED_CONSTRAINT_SYNTAX        52
#endif
#ifndef X509_V_ERR_UNSUPPORTED_NAME_SYNTAX
#define X509_V_ERR_UNSUPPORTED_NAME_SYNTAX              53
#endif
#ifndef X509_V_ERR_CRL_PATH_VALIDATION_ERROR
#define X509_V_ERR_CRL_PATH_VALIDATION_ERROR            54
#endif
#ifndef X509_V_ERR_SUITE_B_INVALID_VERSION
#define X509_V_ERR_SUITE_B_INVALID_VERSION              56
#endif
#ifndef X509_V_ERR_SUITE_B_INVALID_ALGORITHM
#define X509_V_ERR_SUITE_B_INVALID_ALGORITHM            57
#endif
#ifndef X509_V_ERR_SUITE_B_INVALID_CURVE
#define X509_V_ERR_SUITE_B_INVALID_CURVE                58
#endif
#ifndef X509_V_ERR_SUITE_B_INVALID_SIGNATURE_ALGORITHM
#define X509_V_ERR_SUITE_B_INVALID_SIGNATURE_ALGORITHM  59
#endif
#ifndef X509_V_ERR_SUITE_B_LOS_NOT_ALLOWED
#define X509_V_ERR_SUITE_B_LOS_NOT_ALLOWED              60
#endif
#ifndef X509_V_ERR_SUITE_B_CANNOT_SIGN_P_384_WITH_P_256
#define X509_V_ERR_SUITE_B_CANNOT_SIGN_P_384_WITH_P_256 61
#endif
#ifndef X509_V_ERR_HOSTNAME_MISMATCH
#define X509_V_ERR_HOSTNAME_MISMATCH                    62
#endif
#ifndef X509_V_ERR_EMAIL_MISMATCH
#define X509_V_ERR_EMAIL_MISMATCH                       63
#endif
#ifndef X509_V_ERR_IP_ADDRESS_MISMATCH
#define X509_V_ERR_IP_ADDRESS_MISMATCH                  64
#endif
#ifndef X509_V_ERR_INVALID_CALL
#define X509_V_ERR_INVALID_CALL                         65
#endif
#ifndef X509_V_ERR_STORE_LOOKUP
#define X509_V_ERR_STORE_LOOKUP                         66
#endif
#ifndef X509_V_ERR_PROXY_SUBJECT_NAME_VIOLATION
#define X509_V_ERR_PROXY_SUBJECT_NAME_VIOLATION         67
#endif

// the following defines were added in OpenSSL 1.0.2

#ifndef X509_V_ERR_PATH_LOOP
#define X509_V_ERR_PATH_LOOP                            55
#endif
#ifndef X509_V_ERR_SUITE_B_INVALID_VERSION
#define X509_V_ERR_SUITE_B_INVALID_VERSION              56
#endif
#ifndef X509_V_ERR_SUITE_B_INVALID_ALGORITHM
#define X509_V_ERR_SUITE_B_INVALID_ALGORITHM            57
#endif
#ifndef X509_V_ERR_SUITE_B_INVALID_CURVE
#define X509_V_ERR_SUITE_B_INVALID_CURVE                58
#endif
#ifndef X509_V_ERR_SUITE_B_INVALID_SIGNATURE_ALGORITHM
#define X509_V_ERR_SUITE_B_INVALID_SIGNATURE_ALGORITHM  59
#endif
#ifndef X509_V_ERR_SUITE_B_LOS_NOT_ALLOWED
#define X509_V_ERR_SUITE_B_LOS_NOT_ALLOWED              60
#endif
#ifndef X509_V_ERR_SUITE_B_CANNOT_SIGN_P_384_WITH_P_256
#define X509_V_ERR_SUITE_B_CANNOT_SIGN_P_384_WITH_P_256 61
#endif
#ifndef X509_V_ERR_HOSTNAME_MISMATCH
#define X509_V_ERR_HOSTNAME_MISMATCH                    62
#endif
#ifndef X509_V_ERR_EMAIL_MISMATCH
#define X509_V_ERR_EMAIL_MISMATCH                       63
#endif
#ifndef X509_V_ERR_IP_ADDRESS_MISMATCH
#define X509_V_ERR_IP_ADDRESS_MISMATCH                  64
#endif
#ifndef X509_V_ERR_DANE_NO_MATCH
#define X509_V_ERR_DANE_NO_MATCH                        65
#endif
#ifndef X509_V_ERR_EE_KEY_TOO_SMALL
#define X509_V_ERR_EE_KEY_TOO_SMALL                     66
#endif
#ifndef X509_V_ERR_CA_KEY_TOO_SMALL
#define X509_V_ERR_CA_KEY_TOO_SMALL                     67
#endif

// the following defines were added in OpenSSL 1.1.0

#ifndef X509_V_ERR_PATH_LOOP
#define X509_V_ERR_PATH_LOOP                            55
#endif
#ifndef X509_V_ERR_DANE_NO_MATCH
#define X509_V_ERR_DANE_NO_MATCH                        65
#endif
#ifndef X509_V_ERR_EE_KEY_TOO_SMALL
#define X509_V_ERR_EE_KEY_TOO_SMALL                     66
#endif
#ifndef X509_V_ERR_CA_KEY_TOO_SMALL
#define X509_V_ERR_CA_KEY_TOO_SMALL                     67
#endif

// the following defines were added in OpenSSL 1.1.1

#ifndef X509_V_ERR_CA_MD_TOO_WEAK
#define X509_V_ERR_CA_MD_TOO_WEAK                       68
#endif
#ifndef X509_V_ERR_INVALID_CALL
#define X509_V_ERR_INVALID_CALL                         69
#endif
#ifndef X509_V_ERR_STORE_LOOKUP
#define X509_V_ERR_STORE_LOOKUP                         70
#endif
#ifndef X509_V_ERR_NO_VALID_SCTS
#define X509_V_ERR_NO_VALID_SCTS                        71
#endif
#ifndef X509_V_ERR_PROXY_SUBJECT_NAME_VIOLATION
#define X509_V_ERR_PROXY_SUBJECT_NAME_VIOLATION         72
#endif
#ifndef X509_V_ERR_OCSP_VERIFY_NEEDED
#define X509_V_ERR_OCSP_VERIFY_NEEDED                   73
#endif
#ifndef X509_V_ERR_OCSP_VERIFY_FAILED
#define X509_V_ERR_OCSP_VERIFY_FAILED                   74
#endif
#ifndef X509_V_ERR_OCSP_CERT_UNKNOWN
#define X509_V_ERR_OCSP_CERT_UNKNOWN                    75
#endif
#ifndef X509_V_ERR_INVALID_CA
#define X509_V_ERR_INVALID_CA                           79
#endif

// the following defines were added in OpenSSL 3.0.0

#ifndef X509_V_ERR_UNSUPPORTED_SIGNATURE_ALGORITHM
#define X509_V_ERR_UNSUPPORTED_SIGNATURE_ALGORITHM      76
#endif
#ifndef X509_V_ERR_SIGNATURE_ALGORITHM_MISMATCH
#define X509_V_ERR_SIGNATURE_ALGORITHM_MISMATCH         77
#endif
#ifndef X509_V_ERR_SIGNATURE_ALGORITHM_INCONSISTENCY
#define X509_V_ERR_SIGNATURE_ALGORITHM_INCONSISTENCY    78
#endif
#ifndef X509_V_ERR_PATHLEN_INVALID_FOR_NON_CA
#define X509_V_ERR_PATHLEN_INVALID_FOR_NON_CA           80
#endif
#ifndef X509_V_ERR_PATHLEN_WITHOUT_KU_KEY_CERT_SIGN
#define X509_V_ERR_PATHLEN_WITHOUT_KU_KEY_CERT_SIGN     81
#endif
#ifndef X509_V_ERR_KU_KEY_CERT_SIGN_INVALID_FOR_NON_CA
#define X509_V_ERR_KU_KEY_CERT_SIGN_INVALID_FOR_NON_CA  82
#endif
#ifndef X509_V_ERR_ISSUER_NAME_EMPTY
#define X509_V_ERR_ISSUER_NAME_EMPTY                    83
#endif
#ifndef X509_V_ERR_SUBJECT_NAME_EMPTY
#define X509_V_ERR_SUBJECT_NAME_EMPTY                   84
#endif
#ifndef X509_V_ERR_MISSING_AUTHORITY_KEY_IDENTIFIER
#define X509_V_ERR_MISSING_AUTHORITY_KEY_IDENTIFIER     85
#endif
#ifndef X509_V_ERR_MISSING_SUBJECT_KEY_IDENTIFIER
#define X509_V_ERR_MISSING_SUBJECT_KEY_IDENTIFIER       86
#endif
#ifndef X509_V_ERR_EMPTY_SUBJECT_ALT_NAME
#define X509_V_ERR_EMPTY_SUBJECT_ALT_NAME               87
#endif
#ifndef X509_V_ERR_EMPTY_SUBJECT_SAN_NOT_CRITICAL
#define X509_V_ERR_EMPTY_SUBJECT_SAN_NOT_CRITICAL       88
#endif
#ifndef X509_V_ERR_CA_BCONS_NOT_CRITICAL
#define X509_V_ERR_CA_BCONS_NOT_CRITICAL                89
#endif
#ifndef X509_V_ERR_AUTHORITY_KEY_IDENTIFIER_CRITICAL
#define X509_V_ERR_AUTHORITY_KEY_IDENTIFIER_CRITICAL    90
#endif
#ifndef X509_V_ERR_SUBJECT_KEY_IDENTIFIER_CRITICAL
#define X509_V_ERR_SUBJECT_KEY_IDENTIFIER_CRITICAL      91
#endif
#ifndef X509_V_ERR_CA_CERT_MISSING_KEY_USAGE
#define X509_V_ERR_CA_CERT_MISSING_KEY_USAGE            92
#endif
#ifndef X509_V_ERR_EXTENSIONS_REQUIRE_VERSION_3
#define X509_V_ERR_EXTENSIONS_REQUIRE_VERSION_3         93
#endif
#ifndef X509_V_ERR_EC_KEY_EXPLICIT_PARAMS
#define X509_V_ERR_EC_KEY_EXPLICIT_PARAMS               94
#endif

// OpenSSL 1.x defines X509_V_ERR_INVALID_CA as 24 and does not define X509_V_ERR_NO_ISSUER_PUBLIC_KEY.
// OpenSSL 3.x defines X509_V_ERR_INVALID_CA as 79 and X509_V_ERR_NO_ISSUER_PUBLIC_KEY as 24.
// Since we're using the values defined in OpenSSL's header files when possible, this should work
// in both cases.
#ifndef X509_V_ERR_NO_ISSUER_PUBLIC_KEY
#define X509_V_ERR_NO_ISSUER_PUBLIC_KEY                 24
#endif

// X.509 certificate verification errors use condition codes > DCMTLS_EC_X509Verify_Offset.

makeOFConditionConst( DCMTLS_EC_X509VerifyUnspecified,                     OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_UNSPECIFIED,                          OF_error, "X.509 certificate verification error - unspecified" );
makeOFConditionConst( DCMTLS_EC_X509VerifyUnableToGetIssuerCert,           OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT,            OF_error, "X.509 certificate verification error - unable to get issuer certificate" );
makeOFConditionConst( DCMTLS_EC_X509VerifyUnableToGetCRL,                  OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_UNABLE_TO_GET_CRL,                    OF_error, "X.509 certificate verification error - unable to get certificate CRL" );
makeOFConditionConst( DCMTLS_EC_X509VerifyUnableToDecryptCertSignature,    OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE,     OF_error, "X.509 certificate verification error - unable to decrypt certificate's signature" );
makeOFConditionConst( DCMTLS_EC_X509VerifyUnableToDecryptCRLSignature,     OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE,      OF_error, "X.509 certificate verification error - unable to decrypt CRL's signature" );
makeOFConditionConst( DCMTLS_EC_X509VerifyUnableToDecodeIssuerPublicKey,   OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY,   OF_error, "X.509 certificate verification error - unable to decode issuer public key" );
makeOFConditionConst( DCMTLS_EC_X509VerifyCertSignatureFailure,            OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_CERT_SIGNATURE_FAILURE,               OF_error, "X.509 certificate verification error - certificate signature failure" );
makeOFConditionConst( DCMTLS_EC_X509VerifyCRLSignatureFailure,             OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_CRL_SIGNATURE_FAILURE,                OF_error, "X.509 certificate verification error - CRL signature failure" );
makeOFConditionConst( DCMTLS_EC_X509VerifyCertNotYetValid,                 OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_CERT_NOT_YET_VALID,                   OF_error, "X.509 certificate verification error - certificate is not yet valid" );
makeOFConditionConst( DCMTLS_EC_X509VerifyCertHasExpired,                  OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_CERT_HAS_EXPIRED,                     OF_error, "X.509 certificate verification error - certificate has expired" );
makeOFConditionConst( DCMTLS_EC_X509VerifyCRLNotYetValid,                  OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_CRL_NOT_YET_VALID,                    OF_error, "X.509 certificate verification error - CRL is not yet valid" );
makeOFConditionConst( DCMTLS_EC_X509VerifyCRLHasExpired,                   OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_CRL_HAS_EXPIRED,                      OF_error, "X.509 certificate verification error - CRL has expired" );
makeOFConditionConst( DCMTLS_EC_X509VerifyErrorInCertNotBeforeField,       OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD,       OF_error, "X.509 certificate verification error - format error in certificate's notBefore field" );
makeOFConditionConst( DCMTLS_EC_X509VerifyErrorInCertNotAfterField,        OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD,        OF_error, "X.509 certificate verification error - format error in certificate's notAfter field" );
makeOFConditionConst( DCMTLS_EC_X509VerifyErrorInCRLLastUpdateField,       OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD,       OF_error, "X.509 certificate verification error - format error in CRL's lastUpdate field" );
makeOFConditionConst( DCMTLS_EC_X509VerifyErrorInCRLNextUpdateField,       OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD,       OF_error, "X.509 certificate verification error - format error in CRL's nextUpdate field" );
makeOFConditionConst( DCMTLS_EC_X509VerifyOutOfMem,                        OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_OUT_OF_MEM,                           OF_error, "X.509 certificate verification error - out of memory" );
makeOFConditionConst( DCMTLS_EC_X509VerifyDepthZeroSelfSignedCert,         OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT,          OF_error, "X.509 certificate verification error - self signed certificate" );
makeOFConditionConst( DCMTLS_EC_X509VerifySelfSignedCertInChain,           OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN,            OF_error, "X.509 certificate verification error - self signed certificate in certificate chain" );
makeOFConditionConst( DCMTLS_EC_X509VerifyUnableToGetIssuerCertLocally,    OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY,    OF_error, "X.509 certificate verification error - unable to get local issuer certificate" );
makeOFConditionConst( DCMTLS_EC_X509VerifyUnableToVerifyLeafSignature,     OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE,      OF_error, "X.509 certificate verification error - unable to verify the first certificate" );
makeOFConditionConst( DCMTLS_EC_X509VerifyCertChainTooLong,                OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_CERT_CHAIN_TOO_LONG,                  OF_error, "X.509 certificate verification error - certificate chain too long" );
makeOFConditionConst( DCMTLS_EC_X509VerifyCertRevoked,                     OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_CERT_REVOKED,                         OF_error, "X.509 certificate verification error - certificate revoked" );
makeOFConditionConst( DCMTLS_EC_X509VerifyNoIssuerPublicKey,               OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_NO_ISSUER_PUBLIC_KEY,                 OF_error, "X.509 certificate verification error - issuer certificate doesn't have a public key" );
makeOFConditionConst( DCMTLS_EC_X509VerifyPathLengthExceeded,              OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_PATH_LENGTH_EXCEEDED,                 OF_error, "X.509 certificate verification error - path length constraint exceeded" );
makeOFConditionConst( DCMTLS_EC_X509VerifyInvalidPurpose,                  OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_INVALID_PURPOSE,                      OF_error, "X.509 certificate verification error - unsupported certificate purpose" );
makeOFConditionConst( DCMTLS_EC_X509VerifyCertUntrusted,                   OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_CERT_UNTRUSTED,                       OF_error, "X.509 certificate verification error - certificate not trusted" );
makeOFConditionConst( DCMTLS_EC_X509VerifyCertRejected,                    OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_CERT_REJECTED,                        OF_error, "X.509 certificate verification error - certificate rejected" );
makeOFConditionConst( DCMTLS_EC_X509VerifySubjectIssuerMismatch,           OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_SUBJECT_ISSUER_MISMATCH,              OF_error, "X.509 certificate verification error - subject issuer mismatch" );
makeOFConditionConst( DCMTLS_EC_X509VerifyAkidSkidMismatch,                OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_AKID_SKID_MISMATCH,                   OF_error, "X.509 certificate verification error - authority and subject key identifier mismatch" );
makeOFConditionConst( DCMTLS_EC_X509VerifyAkidIssuerSerialMismatch,        OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH,          OF_error, "X.509 certificate verification error - authority and issuer serial number mismatch" );
makeOFConditionConst( DCMTLS_EC_X509VerifyKeyusageNoCertsign,              OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_KEYUSAGE_NO_CERTSIGN,                 OF_error, "X.509 certificate verification error - key usage does not include certificate signing" );
makeOFConditionConst( DCMTLS_EC_X509VerifyUnableToGetCRLIssuer,            OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_UNABLE_TO_GET_CRL_ISSUER,             OF_error, "X.509 certificate verification error - unable to get CRL issuer certificate" );
makeOFConditionConst( DCMTLS_EC_X509VerifyUnhandledCriticalExtension,      OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION,         OF_error, "X.509 certificate verification error - unhandled critical extension" );
makeOFConditionConst( DCMTLS_EC_X509VerifyKeyusageNoCRLSign,               OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_KEYUSAGE_NO_CRL_SIGN,                 OF_error, "X.509 certificate verification error - key usage does not include CRL signing" );
makeOFConditionConst( DCMTLS_EC_X509VerifyUnhandledCriticalCRLExtension,   OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_UNHANDLED_CRITICAL_CRL_EXTENSION,     OF_error, "X.509 certificate verification error - unhandled critical CRL extension" );
makeOFConditionConst( DCMTLS_EC_X509VerifyInvalidNonCA,                    OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_INVALID_NON_CA,                       OF_error, "X.509 certificate verification error - invalid non-CA certificate (has CA markings)" );
makeOFConditionConst( DCMTLS_EC_X509VerifyProxyPathLengthExceeded,         OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_PROXY_PATH_LENGTH_EXCEEDED,           OF_error, "X.509 certificate verification error - proxy path length constraint exceeded" );
makeOFConditionConst( DCMTLS_EC_X509VerifyKeyusageNoDigitalSignature,      OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_KEYUSAGE_NO_DIGITAL_SIGNATURE,        OF_error, "X.509 certificate verification error - key usage does not include digital signature" );
makeOFConditionConst( DCMTLS_EC_X509VerifyProxyCertificatesNotAllowed,     OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_PROXY_CERTIFICATES_NOT_ALLOWED,       OF_error, "X.509 certificate verification error - proxy certificates not allowed, please set the appropriate flag" );
makeOFConditionConst( DCMTLS_EC_X509VerifyInvalidExtension,                OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_INVALID_EXTENSION,                    OF_error, "X.509 certificate verification error - invalid or inconsistent certificate extension" );
makeOFConditionConst( DCMTLS_EC_X509VerifyInvalidPolicyExtension,          OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_INVALID_POLICY_EXTENSION,             OF_error, "X.509 certificate verification error - invalid or inconsistent certificate policy extension" );
makeOFConditionConst( DCMTLS_EC_X509VerifyNoExplicitPolicy,                OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_NO_EXPLICIT_POLICY,                   OF_error, "X.509 certificate verification error - no explicit policy" );
makeOFConditionConst( DCMTLS_EC_X509VerifyDifferentCRLScope,               OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_DIFFERENT_CRL_SCOPE,                  OF_error, "X.509 certificate verification error - Different CRL scope" );
makeOFConditionConst( DCMTLS_EC_X509VerifyUnsupportedExtensionFeature,     OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_UNSUPPORTED_EXTENSION_FEATURE,        OF_error, "X.509 certificate verification error - Unsupported extension feature" );
makeOFConditionConst( DCMTLS_EC_X509VerifyUnnestedResource,                OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_UNNESTED_RESOURCE,                    OF_error, "X.509 certificate verification error - RFC 3779 resource not subset of parent's resources" );
makeOFConditionConst( DCMTLS_EC_X509VerifyPermittedViolation,              OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_PERMITTED_VIOLATION,                  OF_error, "X.509 certificate verification error - permitted subtree violation" );
makeOFConditionConst( DCMTLS_EC_X509VerifyExcludedViolation,               OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_EXCLUDED_VIOLATION,                   OF_error, "X.509 certificate verification error - excluded subtree violation" );
makeOFConditionConst( DCMTLS_EC_X509VerifySubtreeMinmax,                   OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_SUBTREE_MINMAX,                       OF_error, "X.509 certificate verification error - name constraints minimum and maximum not supported" );
makeOFConditionConst( DCMTLS_EC_X509VerifyApplicationVerification,         OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_APPLICATION_VERIFICATION,             OF_error, "X.509 certificate verification error - application verification failure" );
makeOFConditionConst( DCMTLS_EC_X509VerifyUnsupportedConstraintType,       OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_UNSUPPORTED_CONSTRAINT_TYPE,          OF_error, "X.509 certificate verification error - unsupported name constraint type" );
makeOFConditionConst( DCMTLS_EC_X509VerifyUnsupportedConstraintSyntax,     OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_UNSUPPORTED_CONSTRAINT_SYNTAX,        OF_error, "X.509 certificate verification error - unsupported or invalid name constraint syntax" );
makeOFConditionConst( DCMTLS_EC_X509VerifyUnsupportedNameSyntax,           OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_UNSUPPORTED_NAME_SYNTAX,              OF_error, "X.509 certificate verification error - unsupported or invalid name syntax" );
makeOFConditionConst( DCMTLS_EC_X509VerifyCRLPathValidationError,          OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_CRL_PATH_VALIDATION_ERROR,            OF_error, "X.509 certificate verification error - CRL path validation error" );
makeOFConditionConst( DCMTLS_EC_X509VerifyPathLoop,                        OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_PATH_LOOP,                            OF_error, "X.509 certificate verification error - Path Loop" );
makeOFConditionConst( DCMTLS_EC_X509VerifySuiteBInvalidVersion,            OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_SUITE_B_INVALID_VERSION,              OF_error, "X.509 certificate verification error - Suite B: certificate version invalid" );
makeOFConditionConst( DCMTLS_EC_X509VerifySuiteBInvalidAlgorithm,          OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_SUITE_B_INVALID_ALGORITHM,            OF_error, "X.509 certificate verification error - Suite B: invalid public key algorithm" );
makeOFConditionConst( DCMTLS_EC_X509VerifySuiteBInvalidCurve,              OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_SUITE_B_INVALID_CURVE,                OF_error, "X.509 certificate verification error - Suite B: invalid ECC curve" );
makeOFConditionConst( DCMTLS_EC_X509VerifySuiteBInvalidSignatureAlgorithm, OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_SUITE_B_INVALID_SIGNATURE_ALGORITHM,  OF_error, "X.509 certificate verification error - Suite B: invalid signature algorithm" );
makeOFConditionConst( DCMTLS_EC_X509VerifySuiteBLosNotAllowed,             OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_SUITE_B_LOS_NOT_ALLOWED,              OF_error, "X.509 certificate verification error - Suite B: curve not allowed for this LOS" );
makeOFConditionConst( DCMTLS_EC_X509VerifySuiteBCannotSignP384WithP256,    OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_SUITE_B_CANNOT_SIGN_P_384_WITH_P_256, OF_error, "X.509 certificate verification error - Suite B: cannot sign P-384 with P-256" );
makeOFConditionConst( DCMTLS_EC_X509VerifyHostnameMismatch,                OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_HOSTNAME_MISMATCH,                    OF_error, "X.509 certificate verification error - Hostname mismatch" );
makeOFConditionConst( DCMTLS_EC_X509VerifyEmailMismatch,                   OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_EMAIL_MISMATCH,                       OF_error, "X.509 certificate verification error - Email address mismatch" );
makeOFConditionConst( DCMTLS_EC_X509VerifyIPAddressMismatch,               OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_IP_ADDRESS_MISMATCH,                  OF_error, "X.509 certificate verification error - IP address mismatch" );
makeOFConditionConst( DCMTLS_EC_X509VerifyDANENoMatch,                     OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_DANE_NO_MATCH,                        OF_error, "X.509 certificate verification error - No matching DANE TLSA records" );
makeOFConditionConst( DCMTLS_EC_X509VerifyEEKeyTooSmall,                   OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_EE_KEY_TOO_SMALL,                     OF_error, "X.509 certificate verification error - EE certificate key too weak" );
makeOFConditionConst( DCMTLS_EC_X509VerifyCAKeyTooSmall,                   OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_CA_KEY_TOO_SMALL,                     OF_error, "X.509 certificate verification error - CA certificate key too weak" );
makeOFConditionConst( DCMTLS_EC_X509VerifyCAMDTooWeak,                     OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_CA_MD_TOO_WEAK,                       OF_error, "X.509 certificate verification error - CA signature digest algorithm too weak" );
makeOFConditionConst( DCMTLS_EC_X509VerifyInvalidCall,                     OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_INVALID_CALL,                         OF_error, "X.509 certificate verification error - Invalid certificate verification context" );
makeOFConditionConst( DCMTLS_EC_X509VerifyStoreLookup,                     OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_STORE_LOOKUP,                         OF_error, "X.509 certificate verification error - Issuer certificate lookup error" );
makeOFConditionConst( DCMTLS_EC_X509VerifyNoValidSCTs,                     OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_NO_VALID_SCTS,                        OF_error, "X.509 certificate verification error - Certificate Transparency required, but no valid SCTs found" );
makeOFConditionConst( DCMTLS_EC_X509VerifyProxySubjectNameViolation,       OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_PROXY_SUBJECT_NAME_VIOLATION,         OF_error, "X.509 certificate verification error - proxy subject name violation" );
makeOFConditionConst( DCMTLS_EC_X509VerifyOCSPVerifyNeeded,                OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_OCSP_VERIFY_NEEDED,                   OF_error, "X.509 certificate verification error - OCSP verification needed" );
makeOFConditionConst( DCMTLS_EC_X509VerifyOCSPVerifyFailed,                OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_OCSP_VERIFY_FAILED,                   OF_error, "X.509 certificate verification error - OCSP verification failed" );
makeOFConditionConst( DCMTLS_EC_X509VerifyOCSPCertUnknown,                 OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_OCSP_CERT_UNKNOWN,                    OF_error, "X.509 certificate verification error - OCSP unknown cert" );
makeOFConditionConst( DCMTLS_EC_X509VerifyUnsupportedSignatureAlgorithm,   OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_UNSUPPORTED_SIGNATURE_ALGORITHM,      OF_error, "X.509 certificate verification error - Cannot find certificate signature algorithm" );
makeOFConditionConst( DCMTLS_EC_X509VerifySignatureAlgorithmMismatch,      OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_SIGNATURE_ALGORITHM_MISMATCH,         OF_error, "X.509 certificate verification error - subject signature algorithm and issuer public key algorithm mismatch" );
makeOFConditionConst( DCMTLS_EC_X509VerifySignatureAlgorithmInconsistency, OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_SIGNATURE_ALGORITHM_INCONSISTENCY,    OF_error, "X.509 certificate verification error - cert info siganature and signature algorithm mismatch" );
makeOFConditionConst( DCMTLS_EC_X509VerifyInvalidCA,                       OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_INVALID_CA,                           OF_error, "X.509 certificate verification error - invalid CA certificate" );
makeOFConditionConst( DCMTLS_EC_X509VerifyPathlenInvalidForNonCA,          OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_PATHLEN_INVALID_FOR_NON_CA,           OF_error, "X.509 certificate verification error - Path length invalid for non-CA cert" );
makeOFConditionConst( DCMTLS_EC_X509VerifyPathlenWithoutKUKeyCertSign,     OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_PATHLEN_WITHOUT_KU_KEY_CERT_SIGN,     OF_error, "X.509 certificate verification error - Path length given without key usage keyCertSign" );
makeOFConditionConst( DCMTLS_EC_X509VerifyKUKeyCertSignInvalidForNonCA,    OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_KU_KEY_CERT_SIGN_INVALID_FOR_NON_CA,  OF_error, "X.509 certificate verification error - Key usage keyCertSign invalid for non-CA cert" );
makeOFConditionConst( DCMTLS_EC_X509VerifyIssuerNameEmpty,                 OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_ISSUER_NAME_EMPTY,                    OF_error, "X.509 certificate verification error - Issuer name empty" );
makeOFConditionConst( DCMTLS_EC_X509VerifySubjectNameEmpty,                OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_SUBJECT_NAME_EMPTY,                   OF_error, "X.509 certificate verification error - Subject name empty" );
makeOFConditionConst( DCMTLS_EC_X509VerifyMissingAuthorityKeyIdentifier,   OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_MISSING_AUTHORITY_KEY_IDENTIFIER,     OF_error, "X.509 certificate verification error - Missing Authority Key Identifier" );
makeOFConditionConst( DCMTLS_EC_X509VerifyMissingSubjectKeyIdentifier,     OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_MISSING_SUBJECT_KEY_IDENTIFIER,       OF_error, "X.509 certificate verification error - Missing Subject Key Identifier" );
makeOFConditionConst( DCMTLS_EC_X509VerifyEmptySubjectAltName,             OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_EMPTY_SUBJECT_ALT_NAME,               OF_error, "X.509 certificate verification error - Empty Subject Alternative Name extension" );
makeOFConditionConst( DCMTLS_EC_X509VerifyEmptySubjectSanNotCritical,      OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_EMPTY_SUBJECT_SAN_NOT_CRITICAL,       OF_error, "X.509 certificate verification error - Basic Constraints of CA cert not marked critical" );
makeOFConditionConst( DCMTLS_EC_X509VerifyCABconsNotCritical,              OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_CA_BCONS_NOT_CRITICAL,                OF_error, "X.509 certificate verification error - Subject empty and Subject Alt Name extension not critical" );
makeOFConditionConst( DCMTLS_EC_X509VerifyAuthorityKeyIdentifierCritical,  OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_AUTHORITY_KEY_IDENTIFIER_CRITICAL,    OF_error, "X.509 certificate verification error - Authority Key Identifier marked critical" );
makeOFConditionConst( DCMTLS_EC_X509VerifySubjectKeyIdentifierCritical,    OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_SUBJECT_KEY_IDENTIFIER_CRITICAL,      OF_error, "X.509 certificate verification error - Subject Key Identifier marked critical" );
makeOFConditionConst( DCMTLS_EC_X509VerifyCACertMissingKeyUsage,           OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_CA_CERT_MISSING_KEY_USAGE,            OF_error, "X.509 certificate verification error - CA cert does not include key usage extension" );
makeOFConditionConst( DCMTLS_EC_X509VerifyExtensionsRequireVersion3,       OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_EXTENSIONS_REQUIRE_VERSION_3,         OF_error, "X.509 certificate verification error - Using cert extension requires at least X509v3" );
makeOFConditionConst( DCMTLS_EC_X509VerifyECKeyExplicitParams,             OFM_dcmtls, DCMTLS_EC_X509Verify_Offset + X509_V_ERR_EC_KEY_EXPLICIT_PARAMS,               OF_error, "X.509 certificate verification error - Certificate public key has explicit ECC parameters" );

// the following defines are present in OpenSSL 1.0.1

#ifndef SSL_R_APP_DATA_IN_HANDSHAKE
#define SSL_R_APP_DATA_IN_HANDSHAKE                      100
#endif
#ifndef SSL_R_ATTEMPT_TO_REUSE_SESSION_IN_DIFFERENT_CONTEXT
#define SSL_R_ATTEMPT_TO_REUSE_SESSION_IN_DIFFERENT_CONTEXT 272
#endif
#ifndef SSL_R_BAD_ALERT_RECORD
#define SSL_R_BAD_ALERT_RECORD                           101
#endif
#ifndef SSL_R_BAD_AUTHENTICATION_TYPE
#define SSL_R_BAD_AUTHENTICATION_TYPE                    102
#endif
#ifndef SSL_R_BAD_CHANGE_CIPHER_SPEC
#define SSL_R_BAD_CHANGE_CIPHER_SPEC                     103
#endif
#ifndef SSL_R_BAD_CHECKSUM
#define SSL_R_BAD_CHECKSUM                               104
#endif
#ifndef SSL_R_BAD_DATA_RETURNED_BY_CALLBACK
#define SSL_R_BAD_DATA_RETURNED_BY_CALLBACK              106
#endif
#ifndef SSL_R_BAD_DECOMPRESSION
#define SSL_R_BAD_DECOMPRESSION                          107
#endif
#ifndef SSL_R_BAD_DH_G_LENGTH
#define SSL_R_BAD_DH_G_LENGTH                            108
#endif
#ifndef SSL_R_BAD_DH_G_VALUE
#define SSL_R_BAD_DH_G_VALUE                             375
#endif
#ifndef SSL_R_BAD_DH_PUB_KEY_LENGTH
#define SSL_R_BAD_DH_PUB_KEY_LENGTH                      109
#endif
#ifndef SSL_R_BAD_DH_PUB_KEY_VALUE
#define SSL_R_BAD_DH_PUB_KEY_VALUE                       393
#endif
#ifndef SSL_R_BAD_DH_P_LENGTH
#define SSL_R_BAD_DH_P_LENGTH                            110
#endif
#ifndef SSL_R_BAD_DH_P_VALUE
#define SSL_R_BAD_DH_P_VALUE                             395
#endif
#ifndef SSL_R_BAD_DIGEST_LENGTH
#define SSL_R_BAD_DIGEST_LENGTH                          111
#endif
#ifndef SSL_R_BAD_DSA_SIGNATURE
#define SSL_R_BAD_DSA_SIGNATURE                          112
#endif
#ifndef SSL_R_BAD_ECC_CERT
#define SSL_R_BAD_ECC_CERT                               304
#endif
#ifndef SSL_R_BAD_ECDSA_SIGNATURE
#define SSL_R_BAD_ECDSA_SIGNATURE                        305
#endif
#ifndef SSL_R_BAD_ECPOINT
#define SSL_R_BAD_ECPOINT                                306
#endif
#ifndef SSL_R_BAD_HANDSHAKE_LENGTH
#define SSL_R_BAD_HANDSHAKE_LENGTH                       332
#endif
#ifndef SSL_R_BAD_HELLO_REQUEST
#define SSL_R_BAD_HELLO_REQUEST                          105
#endif
#ifndef SSL_R_BAD_LENGTH
#define SSL_R_BAD_LENGTH                                 271
#endif
#ifndef SSL_R_BAD_MAC_DECODE
#define SSL_R_BAD_MAC_DECODE                             113
#endif
#ifndef SSL_R_BAD_MAC_LENGTH
#define SSL_R_BAD_MAC_LENGTH                             333
#endif
#ifndef SSL_R_BAD_MESSAGE_TYPE
#define SSL_R_BAD_MESSAGE_TYPE                           114
#endif
#ifndef SSL_R_BAD_PACKET_LENGTH
#define SSL_R_BAD_PACKET_LENGTH                          115
#endif
#ifndef SSL_R_BAD_PROTOCOL_VERSION_NUMBER
#define SSL_R_BAD_PROTOCOL_VERSION_NUMBER                116
#endif
#ifndef SSL_R_BAD_PSK_IDENTITY_HINT_LENGTH
#define SSL_R_BAD_PSK_IDENTITY_HINT_LENGTH               316
#endif
#ifndef SSL_R_BAD_RESPONSE_ARGUMENT
#define SSL_R_BAD_RESPONSE_ARGUMENT                      117
#endif
#ifndef SSL_R_BAD_RSA_DECRYPT
#define SSL_R_BAD_RSA_DECRYPT                            118
#endif
#ifndef SSL_R_BAD_RSA_ENCRYPT
#define SSL_R_BAD_RSA_ENCRYPT                            119
#endif
#ifndef SSL_R_BAD_RSA_E_LENGTH
#define SSL_R_BAD_RSA_E_LENGTH                           120
#endif
#ifndef SSL_R_BAD_RSA_MODULUS_LENGTH
#define SSL_R_BAD_RSA_MODULUS_LENGTH                     121
#endif
#ifndef SSL_R_BAD_RSA_SIGNATURE
#define SSL_R_BAD_RSA_SIGNATURE                          122
#endif
#ifndef SSL_R_BAD_SIGNATURE
#define SSL_R_BAD_SIGNATURE                              123
#endif
#ifndef SSL_R_BAD_SRP_A_LENGTH
#define SSL_R_BAD_SRP_A_LENGTH                           347
#endif
#ifndef SSL_R_BAD_SRP_B_LENGTH
#define SSL_R_BAD_SRP_B_LENGTH                           348
#endif
#ifndef SSL_R_BAD_SRP_G_LENGTH
#define SSL_R_BAD_SRP_G_LENGTH                           349
#endif
#ifndef SSL_R_BAD_SRP_N_LENGTH
#define SSL_R_BAD_SRP_N_LENGTH                           350
#endif
#ifndef SSL_R_BAD_SRP_PARAMETERS
#define SSL_R_BAD_SRP_PARAMETERS                         371
#endif
#ifndef SSL_R_BAD_SRP_S_LENGTH
#define SSL_R_BAD_SRP_S_LENGTH                           351
#endif
#ifndef SSL_R_BAD_SRTP_MKI_VALUE
#define SSL_R_BAD_SRTP_MKI_VALUE                         352
#endif
#ifndef SSL_R_BAD_SRTP_PROTECTION_PROFILE_LIST
#define SSL_R_BAD_SRTP_PROTECTION_PROFILE_LIST           353
#endif
#ifndef SSL_R_BAD_SSL_FILETYPE
#define SSL_R_BAD_SSL_FILETYPE                           124
#endif
#ifndef SSL_R_BAD_SSL_SESSION_ID_LENGTH
#define SSL_R_BAD_SSL_SESSION_ID_LENGTH                  125
#endif
#ifndef SSL_R_BAD_STATE
#define SSL_R_BAD_STATE                                  126
#endif
#ifndef SSL_R_BAD_WRITE_RETRY
#define SSL_R_BAD_WRITE_RETRY                            127
#endif
#ifndef SSL_R_BIO_NOT_SET
#define SSL_R_BIO_NOT_SET                                128
#endif
#ifndef SSL_R_BLOCK_CIPHER_PAD_IS_WRONG
#define SSL_R_BLOCK_CIPHER_PAD_IS_WRONG                  129
#endif
#ifndef SSL_R_BN_LIB
#define SSL_R_BN_LIB                                     130
#endif
#ifndef SSL_R_CA_DN_LENGTH_MISMATCH
#define SSL_R_CA_DN_LENGTH_MISMATCH                      131
#endif
#ifndef SSL_R_CA_DN_TOO_LONG
#define SSL_R_CA_DN_TOO_LONG                             132
#endif
#ifndef SSL_R_CCS_RECEIVED_EARLY
#define SSL_R_CCS_RECEIVED_EARLY                         133
#endif
#ifndef SSL_R_CERTIFICATE_VERIFY_FAILED
#define SSL_R_CERTIFICATE_VERIFY_FAILED                  134
#endif
#ifndef SSL_R_CERT_LENGTH_MISMATCH
#define SSL_R_CERT_LENGTH_MISMATCH                       135
#endif
#ifndef SSL_R_CHALLENGE_IS_DIFFERENT
#define SSL_R_CHALLENGE_IS_DIFFERENT                     136
#endif
#ifndef SSL_R_CIPHER_CODE_WRONG_LENGTH
#define SSL_R_CIPHER_CODE_WRONG_LENGTH                   137
#endif
#ifndef SSL_R_CIPHER_OR_HASH_UNAVAILABLE
#define SSL_R_CIPHER_OR_HASH_UNAVAILABLE                 138
#endif
#ifndef SSL_R_CIPHER_TABLE_SRC_ERROR
#define SSL_R_CIPHER_TABLE_SRC_ERROR                     139
#endif
#ifndef SSL_R_CLIENTHELLO_TLSEXT
#define SSL_R_CLIENTHELLO_TLSEXT                         226
#endif
#ifndef SSL_R_COMPRESSED_LENGTH_TOO_LONG
#define SSL_R_COMPRESSED_LENGTH_TOO_LONG                 140
#endif
#ifndef SSL_R_COMPRESSION_DISABLED
#define SSL_R_COMPRESSION_DISABLED                       343
#endif
#ifndef SSL_R_COMPRESSION_FAILURE
#define SSL_R_COMPRESSION_FAILURE                        141
#endif
#ifndef SSL_R_COMPRESSION_ID_NOT_WITHIN_PRIVATE_RANGE
#define SSL_R_COMPRESSION_ID_NOT_WITHIN_PRIVATE_RANGE    307
#endif
#ifndef SSL_R_COMPRESSION_LIBRARY_ERROR
#define SSL_R_COMPRESSION_LIBRARY_ERROR                  142
#endif
#ifndef SSL_R_CONNECTION_ID_IS_DIFFERENT
#define SSL_R_CONNECTION_ID_IS_DIFFERENT                 143
#endif
#ifndef SSL_R_CONNECTION_TYPE_NOT_SET
#define SSL_R_CONNECTION_TYPE_NOT_SET                    144
#endif
#ifndef SSL_R_COOKIE_MISMATCH
#define SSL_R_COOKIE_MISMATCH                            308
#endif
#ifndef SSL_R_DATA_BETWEEN_CCS_AND_FINISHED
#define SSL_R_DATA_BETWEEN_CCS_AND_FINISHED              145
#endif
#ifndef SSL_R_DATA_LENGTH_TOO_LONG
#define SSL_R_DATA_LENGTH_TOO_LONG                       146
#endif
#ifndef SSL_R_DECRYPTION_FAILED
#define SSL_R_DECRYPTION_FAILED                          147
#endif
#ifndef SSL_R_DECRYPTION_FAILED_OR_BAD_RECORD_MAC
#define SSL_R_DECRYPTION_FAILED_OR_BAD_RECORD_MAC        281
#endif
#ifndef SSL_R_DH_KEY_TOO_SMALL
#define SSL_R_DH_KEY_TOO_SMALL                           372
#endif
#ifndef SSL_R_DH_PUBLIC_VALUE_LENGTH_IS_WRONG
#define SSL_R_DH_PUBLIC_VALUE_LENGTH_IS_WRONG            148
#endif
#ifndef SSL_R_DIGEST_CHECK_FAILED
#define SSL_R_DIGEST_CHECK_FAILED                        149
#endif
#ifndef SSL_R_DTLS_MESSAGE_TOO_BIG
#define SSL_R_DTLS_MESSAGE_TOO_BIG                       334
#endif
#ifndef SSL_R_DUPLICATE_COMPRESSION_ID
#define SSL_R_DUPLICATE_COMPRESSION_ID                   309
#endif
#ifndef SSL_R_ECC_CERT_NOT_FOR_KEY_AGREEMENT
#define SSL_R_ECC_CERT_NOT_FOR_KEY_AGREEMENT             317
#endif
#ifndef SSL_R_ECC_CERT_NOT_FOR_SIGNING
#define SSL_R_ECC_CERT_NOT_FOR_SIGNING                   318
#endif
#ifndef SSL_R_ECC_CERT_SHOULD_HAVE_RSA_SIGNATURE
#define SSL_R_ECC_CERT_SHOULD_HAVE_RSA_SIGNATURE         322
#endif
#ifndef SSL_R_ECC_CERT_SHOULD_HAVE_SHA1_SIGNATURE
#define SSL_R_ECC_CERT_SHOULD_HAVE_SHA1_SIGNATURE        323
#endif
#ifndef SSL_R_ECGROUP_TOO_LARGE_FOR_CIPHER
#define SSL_R_ECGROUP_TOO_LARGE_FOR_CIPHER               310
#endif
#ifndef SSL_R_EMPTY_SRTP_PROTECTION_PROFILE_LIST
#define SSL_R_EMPTY_SRTP_PROTECTION_PROFILE_LIST         354
#endif
#ifndef SSL_R_ENCRYPTED_LENGTH_TOO_LONG
#define SSL_R_ENCRYPTED_LENGTH_TOO_LONG                  150
#endif
#ifndef SSL_R_ERROR_GENERATING_TMP_RSA_KEY
#define SSL_R_ERROR_GENERATING_TMP_RSA_KEY               282
#endif
#ifndef SSL_R_ERROR_IN_RECEIVED_CIPHER_LIST
#define SSL_R_ERROR_IN_RECEIVED_CIPHER_LIST              151
#endif
#ifndef SSL_R_EXCESSIVE_MESSAGE_SIZE
#define SSL_R_EXCESSIVE_MESSAGE_SIZE                     152
#endif
#ifndef SSL_R_EXTRA_DATA_IN_MESSAGE
#define SSL_R_EXTRA_DATA_IN_MESSAGE                      153
#endif
#ifndef SSL_R_GOT_A_FIN_BEFORE_A_CCS
#define SSL_R_GOT_A_FIN_BEFORE_A_CCS                     154
#endif
#ifndef SSL_R_GOT_NEXT_PROTO_BEFORE_A_CCS
#define SSL_R_GOT_NEXT_PROTO_BEFORE_A_CCS                355
#endif
#ifndef SSL_R_GOT_NEXT_PROTO_WITHOUT_EXTENSION
#define SSL_R_GOT_NEXT_PROTO_WITHOUT_EXTENSION           356
#endif
#ifndef SSL_R_HTTPS_PROXY_REQUEST
#define SSL_R_HTTPS_PROXY_REQUEST                        155
#endif
#ifndef SSL_R_HTTP_REQUEST
#define SSL_R_HTTP_REQUEST                               156
#endif
#ifndef SSL_R_ILLEGAL_PADDING
#define SSL_R_ILLEGAL_PADDING                            283
#endif
#ifndef SSL_R_INAPPROPRIATE_FALLBACK
#define SSL_R_INAPPROPRIATE_FALLBACK                     373
#endif
#ifndef SSL_R_INCONSISTENT_COMPRESSION
#define SSL_R_INCONSISTENT_COMPRESSION                   340
#endif
#ifndef SSL_R_INVALID_CHALLENGE_LENGTH
#define SSL_R_INVALID_CHALLENGE_LENGTH                   158
#endif
#ifndef SSL_R_INVALID_COMMAND
#define SSL_R_INVALID_COMMAND                            280
#endif
#ifndef SSL_R_INVALID_COMPRESSION_ALGORITHM
#define SSL_R_INVALID_COMPRESSION_ALGORITHM              341
#endif
#ifndef SSL_R_INVALID_PURPOSE
#define SSL_R_INVALID_PURPOSE                            278
#endif
#ifndef SSL_R_INVALID_SRP_USERNAME
#define SSL_R_INVALID_SRP_USERNAME                       357
#endif
#ifndef SSL_R_INVALID_STATUS_RESPONSE
#define SSL_R_INVALID_STATUS_RESPONSE                    328
#endif
#ifndef SSL_R_INVALID_TICKET_KEYS_LENGTH
#define SSL_R_INVALID_TICKET_KEYS_LENGTH                 325
#endif
#ifndef SSL_R_INVALID_TRUST
#define SSL_R_INVALID_TRUST                              279
#endif
#ifndef SSL_R_KEY_ARG_TOO_LONG
#define SSL_R_KEY_ARG_TOO_LONG                           284
#endif
#ifndef SSL_R_KRB5
#define SSL_R_KRB5                                       285
#endif
#ifndef SSL_R_KRB5_C_CC_PRINC
#define SSL_R_KRB5_C_CC_PRINC                            286
#endif
#ifndef SSL_R_KRB5_C_GET_CRED
#define SSL_R_KRB5_C_GET_CRED                            287
#endif
#ifndef SSL_R_KRB5_C_INIT
#define SSL_R_KRB5_C_INIT                                288
#endif
#ifndef SSL_R_KRB5_C_MK_REQ
#define SSL_R_KRB5_C_MK_REQ                              289
#endif
#ifndef SSL_R_KRB5_S_BAD_TICKET
#define SSL_R_KRB5_S_BAD_TICKET                          290
#endif
#ifndef SSL_R_KRB5_S_INIT
#define SSL_R_KRB5_S_INIT                                291
#endif
#ifndef SSL_R_KRB5_S_RD_REQ
#define SSL_R_KRB5_S_RD_REQ                              292
#endif
#ifndef SSL_R_KRB5_S_TKT_EXPIRED
#define SSL_R_KRB5_S_TKT_EXPIRED                         293
#endif
#ifndef SSL_R_KRB5_S_TKT_NYV
#define SSL_R_KRB5_S_TKT_NYV                             294
#endif
#ifndef SSL_R_KRB5_S_TKT_SKEW
#define SSL_R_KRB5_S_TKT_SKEW                            295
#endif
#ifndef SSL_R_LENGTH_MISMATCH
#define SSL_R_LENGTH_MISMATCH                            159
#endif
#ifndef SSL_R_LENGTH_TOO_SHORT
#define SSL_R_LENGTH_TOO_SHORT                           160
#endif
#ifndef SSL_R_LIBRARY_BUG
#define SSL_R_LIBRARY_BUG                                274
#endif
#ifndef SSL_R_LIBRARY_HAS_NO_CIPHERS
#define SSL_R_LIBRARY_HAS_NO_CIPHERS                     161
#endif
#ifndef SSL_R_MESSAGE_TOO_LONG
#define SSL_R_MESSAGE_TOO_LONG                           296
#endif
#ifndef SSL_R_MISSING_DH_DSA_CERT
#define SSL_R_MISSING_DH_DSA_CERT                        162
#endif
#ifndef SSL_R_MISSING_DH_KEY
#define SSL_R_MISSING_DH_KEY                             163
#endif
#ifndef SSL_R_MISSING_DH_RSA_CERT
#define SSL_R_MISSING_DH_RSA_CERT                        164
#endif
#ifndef SSL_R_MISSING_DSA_SIGNING_CERT
#define SSL_R_MISSING_DSA_SIGNING_CERT                   165
#endif
#ifndef SSL_R_MISSING_EXPORT_TMP_DH_KEY
#define SSL_R_MISSING_EXPORT_TMP_DH_KEY                  166
#endif
#ifndef SSL_R_MISSING_EXPORT_TMP_RSA_KEY
#define SSL_R_MISSING_EXPORT_TMP_RSA_KEY                 167
#endif
#ifndef SSL_R_MISSING_RSA_CERTIFICATE
#define SSL_R_MISSING_RSA_CERTIFICATE                    168
#endif
#ifndef SSL_R_MISSING_RSA_ENCRYPTING_CERT
#define SSL_R_MISSING_RSA_ENCRYPTING_CERT                169
#endif
#ifndef SSL_R_MISSING_RSA_SIGNING_CERT
#define SSL_R_MISSING_RSA_SIGNING_CERT                   170
#endif
#ifndef SSL_R_MISSING_SRP_PARAM
#define SSL_R_MISSING_SRP_PARAM                          358
#endif
#ifndef SSL_R_MISSING_TMP_DH_KEY
#define SSL_R_MISSING_TMP_DH_KEY                         171
#endif
#ifndef SSL_R_MISSING_TMP_ECDH_KEY
#define SSL_R_MISSING_TMP_ECDH_KEY                       311
#endif
#ifndef SSL_R_MISSING_TMP_RSA_KEY
#define SSL_R_MISSING_TMP_RSA_KEY                        172
#endif
#ifndef SSL_R_MISSING_TMP_RSA_PKEY
#define SSL_R_MISSING_TMP_RSA_PKEY                       173
#endif
#ifndef SSL_R_MISSING_VERIFY_MESSAGE
#define SSL_R_MISSING_VERIFY_MESSAGE                     174
#endif
#ifndef SSL_R_MULTIPLE_SGC_RESTARTS
#define SSL_R_MULTIPLE_SGC_RESTARTS                      346
#endif
#ifndef SSL_R_NON_SSLV2_INITIAL_PACKET
#define SSL_R_NON_SSLV2_INITIAL_PACKET                   175
#endif
#ifndef SSL_R_NO_CERTIFICATES_RETURNED
#define SSL_R_NO_CERTIFICATES_RETURNED                   176
#endif
#ifndef SSL_R_NO_CERTIFICATE_ASSIGNED
#define SSL_R_NO_CERTIFICATE_ASSIGNED                    177
#endif
#ifndef SSL_R_NO_CERTIFICATE_RETURNED
#define SSL_R_NO_CERTIFICATE_RETURNED                    178
#endif
#ifndef SSL_R_NO_CERTIFICATE_SET
#define SSL_R_NO_CERTIFICATE_SET                         179
#endif
#ifndef SSL_R_NO_CERTIFICATE_SPECIFIED
#define SSL_R_NO_CERTIFICATE_SPECIFIED                   180
#endif
#ifndef SSL_R_NO_CIPHERS_AVAILABLE
#define SSL_R_NO_CIPHERS_AVAILABLE                       181
#endif
#ifndef SSL_R_NO_CIPHERS_PASSED
#define SSL_R_NO_CIPHERS_PASSED                          182
#endif
#ifndef SSL_R_NO_CIPHERS_SPECIFIED
#define SSL_R_NO_CIPHERS_SPECIFIED                       183
#endif
#ifndef SSL_R_NO_CIPHER_LIST
#define SSL_R_NO_CIPHER_LIST                             184
#endif
#ifndef SSL_R_NO_CIPHER_MATCH
#define SSL_R_NO_CIPHER_MATCH                            185
#endif
#ifndef SSL_R_NO_CLIENT_CERT_METHOD
#define SSL_R_NO_CLIENT_CERT_METHOD                      331
#endif
#ifndef SSL_R_NO_CLIENT_CERT_RECEIVED
#define SSL_R_NO_CLIENT_CERT_RECEIVED                    186
#endif
#ifndef SSL_R_NO_COMPRESSION_SPECIFIED
#define SSL_R_NO_COMPRESSION_SPECIFIED                   187
#endif
#ifndef SSL_R_NO_GOST_CERTIFICATE_SENT_BY_PEER
#define SSL_R_NO_GOST_CERTIFICATE_SENT_BY_PEER           330
#endif
#ifndef SSL_R_NO_METHOD_SPECIFIED
#define SSL_R_NO_METHOD_SPECIFIED                        188
#endif
#ifndef SSL_R_NO_PRIVATEKEY
#define SSL_R_NO_PRIVATEKEY                              189
#endif
#ifndef SSL_R_NO_PRIVATE_KEY_ASSIGNED
#define SSL_R_NO_PRIVATE_KEY_ASSIGNED                    190
#endif
#ifndef SSL_R_NO_PROTOCOLS_AVAILABLE
#define SSL_R_NO_PROTOCOLS_AVAILABLE                     191
#endif
#ifndef SSL_R_NO_PUBLICKEY
#define SSL_R_NO_PUBLICKEY                               192
#endif
#ifndef SSL_R_NO_RENEGOTIATION
#define SSL_R_NO_RENEGOTIATION                           339
#endif
#ifndef SSL_R_NO_REQUIRED_DIGEST
#define SSL_R_NO_REQUIRED_DIGEST                         324
#endif
#ifndef SSL_R_NO_SHARED_CIPHER
#define SSL_R_NO_SHARED_CIPHER                           193
#endif
#ifndef SSL_R_NO_SRTP_PROFILES
#define SSL_R_NO_SRTP_PROFILES                           359
#endif
#ifndef SSL_R_NO_VERIFY_CALLBACK
#define SSL_R_NO_VERIFY_CALLBACK                         194
#endif
#ifndef SSL_R_NULL_SSL_CTX
#define SSL_R_NULL_SSL_CTX                               195
#endif
#ifndef SSL_R_NULL_SSL_METHOD_PASSED
#define SSL_R_NULL_SSL_METHOD_PASSED                     196
#endif
#ifndef SSL_R_OLD_SESSION_CIPHER_NOT_RETURNED
#define SSL_R_OLD_SESSION_CIPHER_NOT_RETURNED            197
#endif
#ifndef SSL_R_OLD_SESSION_COMPRESSION_ALGORITHM_NOT_RETURNED
#define SSL_R_OLD_SESSION_COMPRESSION_ALGORITHM_NOT_RETURNED 344
#endif
#ifndef SSL_R_ONLY_TLS_ALLOWED_IN_FIPS_MODE
#define SSL_R_ONLY_TLS_ALLOWED_IN_FIPS_MODE              297
#endif
#ifndef SSL_R_OPAQUE_PRF_INPUT_TOO_LONG
#define SSL_R_OPAQUE_PRF_INPUT_TOO_LONG                  327
#endif
#ifndef SSL_R_PACKET_LENGTH_TOO_LONG
#define SSL_R_PACKET_LENGTH_TOO_LONG                     198
#endif
#ifndef SSL_R_PARSE_TLSEXT
#define SSL_R_PARSE_TLSEXT                               227
#endif
#ifndef SSL_R_PATH_TOO_LONG
#define SSL_R_PATH_TOO_LONG                              270
#endif
#ifndef SSL_R_PEER_DID_NOT_RETURN_A_CERTIFICATE
#define SSL_R_PEER_DID_NOT_RETURN_A_CERTIFICATE          199
#endif
#ifndef SSL_R_PEER_ERROR
#define SSL_R_PEER_ERROR                                 200
#endif
#ifndef SSL_R_PEER_ERROR_CERTIFICATE
#define SSL_R_PEER_ERROR_CERTIFICATE                     201
#endif
#ifndef SSL_R_PEER_ERROR_NO_CERTIFICATE
#define SSL_R_PEER_ERROR_NO_CERTIFICATE                  202
#endif
#ifndef SSL_R_PEER_ERROR_NO_CIPHER
#define SSL_R_PEER_ERROR_NO_CIPHER                       203
#endif
#ifndef SSL_R_PEER_ERROR_UNSUPPORTED_CERTIFICATE_TYPE
#define SSL_R_PEER_ERROR_UNSUPPORTED_CERTIFICATE_TYPE    204
#endif
#ifndef SSL_R_PRE_MAC_LENGTH_TOO_LONG
#define SSL_R_PRE_MAC_LENGTH_TOO_LONG                    205
#endif
#ifndef SSL_R_PROBLEMS_MAPPING_CIPHER_FUNCTIONS
#define SSL_R_PROBLEMS_MAPPING_CIPHER_FUNCTIONS          206
#endif
#ifndef SSL_R_PROTOCOL_IS_SHUTDOWN
#define SSL_R_PROTOCOL_IS_SHUTDOWN                       207
#endif
#ifndef SSL_R_PSK_IDENTITY_NOT_FOUND
#define SSL_R_PSK_IDENTITY_NOT_FOUND                     223
#endif
#ifndef SSL_R_PSK_NO_CLIENT_CB
#define SSL_R_PSK_NO_CLIENT_CB                           224
#endif
#ifndef SSL_R_PSK_NO_SERVER_CB
#define SSL_R_PSK_NO_SERVER_CB                           225
#endif
#ifndef SSL_R_PUBLIC_KEY_ENCRYPT_ERROR
#define SSL_R_PUBLIC_KEY_ENCRYPT_ERROR                   208
#endif
#ifndef SSL_R_PUBLIC_KEY_IS_NOT_RSA
#define SSL_R_PUBLIC_KEY_IS_NOT_RSA                      209
#endif
#ifndef SSL_R_PUBLIC_KEY_NOT_RSA
#define SSL_R_PUBLIC_KEY_NOT_RSA                         210
#endif
#ifndef SSL_R_READ_BIO_NOT_SET
#define SSL_R_READ_BIO_NOT_SET                           211
#endif
#ifndef SSL_R_READ_TIMEOUT_EXPIRED
#define SSL_R_READ_TIMEOUT_EXPIRED                       312
#endif
#ifndef SSL_R_READ_WRONG_PACKET_TYPE
#define SSL_R_READ_WRONG_PACKET_TYPE                     212
#endif
#ifndef SSL_R_RECORD_LENGTH_MISMATCH
#define SSL_R_RECORD_LENGTH_MISMATCH                     213
#endif
#ifndef SSL_R_RECORD_TOO_LARGE
#define SSL_R_RECORD_TOO_LARGE                           214
#endif
#ifndef SSL_R_RECORD_TOO_SMALL
#define SSL_R_RECORD_TOO_SMALL                           298
#endif
#ifndef SSL_R_RENEGOTIATE_EXT_TOO_LONG
#define SSL_R_RENEGOTIATE_EXT_TOO_LONG                   335
#endif
#ifndef SSL_R_RENEGOTIATION_ENCODING_ERR
#define SSL_R_RENEGOTIATION_ENCODING_ERR                 336
#endif
#ifndef SSL_R_RENEGOTIATION_MISMATCH
#define SSL_R_RENEGOTIATION_MISMATCH                     337
#endif
#ifndef SSL_R_REQUIRED_CIPHER_MISSING
#define SSL_R_REQUIRED_CIPHER_MISSING                    215
#endif
#ifndef SSL_R_REQUIRED_COMPRESSSION_ALGORITHM_MISSING
#define SSL_R_REQUIRED_COMPRESSSION_ALGORITHM_MISSING    342
#endif
#ifndef SSL_R_REUSE_CERT_LENGTH_NOT_ZERO
#define SSL_R_REUSE_CERT_LENGTH_NOT_ZERO                 216
#endif
#ifndef SSL_R_REUSE_CERT_TYPE_NOT_ZERO
#define SSL_R_REUSE_CERT_TYPE_NOT_ZERO                   217
#endif
#ifndef SSL_R_REUSE_CIPHER_LIST_NOT_ZERO
#define SSL_R_REUSE_CIPHER_LIST_NOT_ZERO                 218
#endif
#ifndef SSL_R_SCSV_RECEIVED_WHEN_RENEGOTIATING
#define SSL_R_SCSV_RECEIVED_WHEN_RENEGOTIATING           345
#endif
#ifndef SSL_R_SERVERHELLO_TLSEXT
#define SSL_R_SERVERHELLO_TLSEXT                         275
#endif
#ifndef SSL_R_SESSION_ID_CONTEXT_UNINITIALIZED
#define SSL_R_SESSION_ID_CONTEXT_UNINITIALIZED           277
#endif
#ifndef SSL_R_SHORT_READ
#define SSL_R_SHORT_READ                                 219
#endif
#ifndef SSL_R_SIGNATURE_ALGORITHMS_ERROR
#define SSL_R_SIGNATURE_ALGORITHMS_ERROR                 360
#endif
#ifndef SSL_R_SIGNATURE_FOR_NON_SIGNING_CERTIFICATE
#define SSL_R_SIGNATURE_FOR_NON_SIGNING_CERTIFICATE      220
#endif
#ifndef SSL_R_SRP_A_CALC
#define SSL_R_SRP_A_CALC                                 361
#endif
#ifndef SSL_R_SRTP_COULD_NOT_ALLOCATE_PROFILES
#define SSL_R_SRTP_COULD_NOT_ALLOCATE_PROFILES           362
#endif
#ifndef SSL_R_SRTP_PROTECTION_PROFILE_LIST_TOO_LONG
#define SSL_R_SRTP_PROTECTION_PROFILE_LIST_TOO_LONG      363
#endif
#ifndef SSL_R_SRTP_UNKNOWN_PROTECTION_PROFILE
#define SSL_R_SRTP_UNKNOWN_PROTECTION_PROFILE            364
#endif
#ifndef SSL_R_SSL23_DOING_SESSION_ID_REUSE
#define SSL_R_SSL23_DOING_SESSION_ID_REUSE               221
#endif
#ifndef SSL_R_SSL2_CONNECTION_ID_TOO_LONG
#define SSL_R_SSL2_CONNECTION_ID_TOO_LONG                299
#endif
#ifndef SSL_R_SSL3_EXT_INVALID_ECPOINTFORMAT
#define SSL_R_SSL3_EXT_INVALID_ECPOINTFORMAT             321
#endif
#ifndef SSL_R_SSL3_EXT_INVALID_SERVERNAME
#define SSL_R_SSL3_EXT_INVALID_SERVERNAME                319
#endif
#ifndef SSL_R_SSL3_EXT_INVALID_SERVERNAME_TYPE
#define SSL_R_SSL3_EXT_INVALID_SERVERNAME_TYPE           320
#endif
#ifndef SSL_R_SSL3_SESSION_ID_TOO_LONG
#define SSL_R_SSL3_SESSION_ID_TOO_LONG                   300
#endif
#ifndef SSL_R_SSL3_SESSION_ID_TOO_SHORT
#define SSL_R_SSL3_SESSION_ID_TOO_SHORT                  222
#endif
#ifndef SSL_R_SSLV3_ALERT_BAD_CERTIFICATE
#define SSL_R_SSLV3_ALERT_BAD_CERTIFICATE                1042
#endif
#ifndef SSL_R_SSLV3_ALERT_BAD_RECORD_MAC
#define SSL_R_SSLV3_ALERT_BAD_RECORD_MAC                 1020
#endif
#ifndef SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED
#define SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED            1045
#endif
#ifndef SSL_R_SSLV3_ALERT_CERTIFICATE_REVOKED
#define SSL_R_SSLV3_ALERT_CERTIFICATE_REVOKED            1044
#endif
#ifndef SSL_R_SSLV3_ALERT_CERTIFICATE_UNKNOWN
#define SSL_R_SSLV3_ALERT_CERTIFICATE_UNKNOWN            1046
#endif
#ifndef SSL_R_SSLV3_ALERT_DECOMPRESSION_FAILURE
#define SSL_R_SSLV3_ALERT_DECOMPRESSION_FAILURE          1030
#endif
#ifndef SSL_R_SSLV3_ALERT_HANDSHAKE_FAILURE
#define SSL_R_SSLV3_ALERT_HANDSHAKE_FAILURE              1040
#endif
#ifndef SSL_R_SSLV3_ALERT_ILLEGAL_PARAMETER
#define SSL_R_SSLV3_ALERT_ILLEGAL_PARAMETER              1047
#endif
#ifndef SSL_R_SSLV3_ALERT_NO_CERTIFICATE
#define SSL_R_SSLV3_ALERT_NO_CERTIFICATE                 1041
#endif
#ifndef SSL_R_SSLV3_ALERT_UNEXPECTED_MESSAGE
#define SSL_R_SSLV3_ALERT_UNEXPECTED_MESSAGE             1010
#endif
#ifndef SSL_R_SSLV3_ALERT_UNSUPPORTED_CERTIFICATE
#define SSL_R_SSLV3_ALERT_UNSUPPORTED_CERTIFICATE        1043
#endif
#ifndef SSL_R_SSL_CTX_HAS_NO_DEFAULT_SSL_VERSION
#define SSL_R_SSL_CTX_HAS_NO_DEFAULT_SSL_VERSION         228
#endif
#ifndef SSL_R_SSL_HANDSHAKE_FAILURE
#define SSL_R_SSL_HANDSHAKE_FAILURE                      229
#endif
#ifndef SSL_R_SSL_LIBRARY_HAS_NO_CIPHERS
#define SSL_R_SSL_LIBRARY_HAS_NO_CIPHERS                 230
#endif
#ifndef SSL_R_SSL_SESSION_ID_CALLBACK_FAILED
#define SSL_R_SSL_SESSION_ID_CALLBACK_FAILED             301
#endif
#ifndef SSL_R_SSL_SESSION_ID_CONFLICT
#define SSL_R_SSL_SESSION_ID_CONFLICT                    302
#endif
#ifndef SSL_R_SSL_SESSION_ID_CONTEXT_TOO_LONG
#define SSL_R_SSL_SESSION_ID_CONTEXT_TOO_LONG            273
#endif
#ifndef SSL_R_SSL_SESSION_ID_HAS_BAD_LENGTH
#define SSL_R_SSL_SESSION_ID_HAS_BAD_LENGTH              303
#endif
#ifndef SSL_R_SSL_SESSION_ID_IS_DIFFERENT
#define SSL_R_SSL_SESSION_ID_IS_DIFFERENT                231
#endif
#ifndef SSL_R_TLSV1_ALERT_ACCESS_DENIED
#define SSL_R_TLSV1_ALERT_ACCESS_DENIED                  1049
#endif
#ifndef SSL_R_TLSV1_ALERT_DECODE_ERROR
#define SSL_R_TLSV1_ALERT_DECODE_ERROR                   1050
#endif
#ifndef SSL_R_TLSV1_ALERT_DECRYPTION_FAILED
#define SSL_R_TLSV1_ALERT_DECRYPTION_FAILED              1021
#endif
#ifndef SSL_R_TLSV1_ALERT_DECRYPT_ERROR
#define SSL_R_TLSV1_ALERT_DECRYPT_ERROR                  1051
#endif
#ifndef SSL_R_TLSV1_ALERT_EXPORT_RESTRICTION
#define SSL_R_TLSV1_ALERT_EXPORT_RESTRICTION             1060
#endif
#ifndef SSL_R_TLSV1_ALERT_INAPPROPRIATE_FALLBACK
#define SSL_R_TLSV1_ALERT_INAPPROPRIATE_FALLBACK         1086
#endif
#ifndef SSL_R_TLSV1_ALERT_INSUFFICIENT_SECURITY
#define SSL_R_TLSV1_ALERT_INSUFFICIENT_SECURITY          1071
#endif
#ifndef SSL_R_TLSV1_ALERT_INTERNAL_ERROR
#define SSL_R_TLSV1_ALERT_INTERNAL_ERROR                 1080
#endif
#ifndef SSL_R_TLSV1_ALERT_NO_RENEGOTIATION
#define SSL_R_TLSV1_ALERT_NO_RENEGOTIATION               1100
#endif
#ifndef SSL_R_TLSV1_ALERT_PROTOCOL_VERSION
#define SSL_R_TLSV1_ALERT_PROTOCOL_VERSION               1070
#endif
#ifndef SSL_R_TLSV1_ALERT_RECORD_OVERFLOW
#define SSL_R_TLSV1_ALERT_RECORD_OVERFLOW                1022
#endif
#ifndef SSL_R_TLSV1_ALERT_UNKNOWN_CA
#define SSL_R_TLSV1_ALERT_UNKNOWN_CA                     1048
#endif
#ifndef SSL_R_TLSV1_ALERT_USER_CANCELLED
#define SSL_R_TLSV1_ALERT_USER_CANCELLED                 1090
#endif
#ifndef SSL_R_TLSV1_BAD_CERTIFICATE_HASH_VALUE
#define SSL_R_TLSV1_BAD_CERTIFICATE_HASH_VALUE           1114
#endif
#ifndef SSL_R_TLSV1_BAD_CERTIFICATE_STATUS_RESPONSE
#define SSL_R_TLSV1_BAD_CERTIFICATE_STATUS_RESPONSE      1113
#endif
#ifndef SSL_R_TLSV1_CERTIFICATE_UNOBTAINABLE
#define SSL_R_TLSV1_CERTIFICATE_UNOBTAINABLE             1111
#endif
#ifndef SSL_R_TLSV1_UNRECOGNIZED_NAME
#define SSL_R_TLSV1_UNRECOGNIZED_NAME                    1112
#endif
#ifndef SSL_R_TLSV1_UNSUPPORTED_EXTENSION
#define SSL_R_TLSV1_UNSUPPORTED_EXTENSION                1110
#endif
#ifndef SSL_R_TLS_CLIENT_CERT_REQ_WITH_ANON_CIPHER
#define SSL_R_TLS_CLIENT_CERT_REQ_WITH_ANON_CIPHER       232
#endif
#ifndef SSL_R_TLS_HEARTBEAT_PEER_DOESNT_ACCEPT
#define SSL_R_TLS_HEARTBEAT_PEER_DOESNT_ACCEPT           365
#endif
#ifndef SSL_R_TLS_HEARTBEAT_PENDING
#define SSL_R_TLS_HEARTBEAT_PENDING                      366
#endif
#ifndef SSL_R_TLS_ILLEGAL_EXPORTER_LABEL
#define SSL_R_TLS_ILLEGAL_EXPORTER_LABEL                 367
#endif
#ifndef SSL_R_TLS_INVALID_ECPOINTFORMAT_LIST
#define SSL_R_TLS_INVALID_ECPOINTFORMAT_LIST             157
#endif
#ifndef SSL_R_TLS_PEER_DID_NOT_RESPOND_WITH_CERTIFICATE_LIST
#define SSL_R_TLS_PEER_DID_NOT_RESPOND_WITH_CERTIFICATE_LIST 233
#endif
#ifndef SSL_R_TLS_RSA_ENCRYPTED_VALUE_LENGTH_IS_WRONG
#define SSL_R_TLS_RSA_ENCRYPTED_VALUE_LENGTH_IS_WRONG    234
#endif
#ifndef SSL_R_TRIED_TO_USE_UNSUPPORTED_CIPHER
#define SSL_R_TRIED_TO_USE_UNSUPPORTED_CIPHER            235
#endif
#ifndef SSL_R_UNABLE_TO_DECODE_DH_CERTS
#define SSL_R_UNABLE_TO_DECODE_DH_CERTS                  236
#endif
#ifndef SSL_R_UNABLE_TO_DECODE_ECDH_CERTS
#define SSL_R_UNABLE_TO_DECODE_ECDH_CERTS                313
#endif
#ifndef SSL_R_UNABLE_TO_EXTRACT_PUBLIC_KEY
#define SSL_R_UNABLE_TO_EXTRACT_PUBLIC_KEY               237
#endif
#ifndef SSL_R_UNABLE_TO_FIND_DH_PARAMETERS
#define SSL_R_UNABLE_TO_FIND_DH_PARAMETERS               238
#endif
#ifndef SSL_R_UNABLE_TO_FIND_ECDH_PARAMETERS
#define SSL_R_UNABLE_TO_FIND_ECDH_PARAMETERS             314
#endif
#ifndef SSL_R_UNABLE_TO_FIND_PUBLIC_KEY_PARAMETERS
#define SSL_R_UNABLE_TO_FIND_PUBLIC_KEY_PARAMETERS       239
#endif
#ifndef SSL_R_UNABLE_TO_FIND_SSL_METHOD
#define SSL_R_UNABLE_TO_FIND_SSL_METHOD                  240
#endif
#ifndef SSL_R_UNABLE_TO_LOAD_SSL2_MD5_ROUTINES
#define SSL_R_UNABLE_TO_LOAD_SSL2_MD5_ROUTINES           241
#endif
#ifndef SSL_R_UNABLE_TO_LOAD_SSL3_MD5_ROUTINES
#define SSL_R_UNABLE_TO_LOAD_SSL3_MD5_ROUTINES           242
#endif
#ifndef SSL_R_UNABLE_TO_LOAD_SSL3_SHA1_ROUTINES
#define SSL_R_UNABLE_TO_LOAD_SSL3_SHA1_ROUTINES          243
#endif
#ifndef SSL_R_UNEXPECTED_MESSAGE
#define SSL_R_UNEXPECTED_MESSAGE                         244
#endif
#ifndef SSL_R_UNEXPECTED_RECORD
#define SSL_R_UNEXPECTED_RECORD                          245
#endif
#ifndef SSL_R_UNINITIALIZED
#define SSL_R_UNINITIALIZED                              276
#endif
#ifndef SSL_R_UNKNOWN_ALERT_TYPE
#define SSL_R_UNKNOWN_ALERT_TYPE                         246
#endif
#ifndef SSL_R_UNKNOWN_CERTIFICATE_TYPE
#define SSL_R_UNKNOWN_CERTIFICATE_TYPE                   247
#endif
#ifndef SSL_R_UNKNOWN_CIPHER_RETURNED
#define SSL_R_UNKNOWN_CIPHER_RETURNED                    248
#endif
#ifndef SSL_R_UNKNOWN_CIPHER_TYPE
#define SSL_R_UNKNOWN_CIPHER_TYPE                        249
#endif
#ifndef SSL_R_UNKNOWN_DIGEST
#define SSL_R_UNKNOWN_DIGEST                             368
#endif
#ifndef SSL_R_UNKNOWN_KEY_EXCHANGE_TYPE
#define SSL_R_UNKNOWN_KEY_EXCHANGE_TYPE                  250
#endif
#ifndef SSL_R_UNKNOWN_PKEY_TYPE
#define SSL_R_UNKNOWN_PKEY_TYPE                          251
#endif
#ifndef SSL_R_UNKNOWN_PROTOCOL
#define SSL_R_UNKNOWN_PROTOCOL                           252
#endif
#ifndef SSL_R_UNKNOWN_REMOTE_ERROR_TYPE
#define SSL_R_UNKNOWN_REMOTE_ERROR_TYPE                  253
#endif
#ifndef SSL_R_UNKNOWN_SSL_VERSION
#define SSL_R_UNKNOWN_SSL_VERSION                        254
#endif
#ifndef SSL_R_UNKNOWN_STATE
#define SSL_R_UNKNOWN_STATE                              255
#endif
#ifndef SSL_R_UNSAFE_LEGACY_RENEGOTIATION_DISABLED
#define SSL_R_UNSAFE_LEGACY_RENEGOTIATION_DISABLED       338
#endif
#ifndef SSL_R_UNSUPPORTED_CIPHER
#define SSL_R_UNSUPPORTED_CIPHER                         256
#endif
#ifndef SSL_R_UNSUPPORTED_COMPRESSION_ALGORITHM
#define SSL_R_UNSUPPORTED_COMPRESSION_ALGORITHM          257
#endif
#ifndef SSL_R_UNSUPPORTED_DIGEST_TYPE
#define SSL_R_UNSUPPORTED_DIGEST_TYPE                    326
#endif
#ifndef SSL_R_UNSUPPORTED_ELLIPTIC_CURVE
#define SSL_R_UNSUPPORTED_ELLIPTIC_CURVE                 315
#endif
#ifndef SSL_R_UNSUPPORTED_PROTOCOL
#define SSL_R_UNSUPPORTED_PROTOCOL                       258
#endif
#ifndef SSL_R_UNSUPPORTED_SSL_VERSION
#define SSL_R_UNSUPPORTED_SSL_VERSION                    259
#endif
#ifndef SSL_R_UNSUPPORTED_STATUS_TYPE
#define SSL_R_UNSUPPORTED_STATUS_TYPE                    329
#endif
#ifndef SSL_R_USE_SRTP_NOT_NEGOTIATED
#define SSL_R_USE_SRTP_NOT_NEGOTIATED                    369
#endif
#ifndef SSL_R_WRITE_BIO_NOT_SET
#define SSL_R_WRITE_BIO_NOT_SET                          260
#endif
#ifndef SSL_R_WRONG_CIPHER_RETURNED
#define SSL_R_WRONG_CIPHER_RETURNED                      261
#endif
#ifndef SSL_R_WRONG_MESSAGE_TYPE
#define SSL_R_WRONG_MESSAGE_TYPE                         262
#endif
#ifndef SSL_R_WRONG_NUMBER_OF_KEY_BITS
#define SSL_R_WRONG_NUMBER_OF_KEY_BITS                   263
#endif
#ifndef SSL_R_WRONG_SIGNATURE_LENGTH
#define SSL_R_WRONG_SIGNATURE_LENGTH                     264
#endif
#ifndef SSL_R_WRONG_SIGNATURE_SIZE
#define SSL_R_WRONG_SIGNATURE_SIZE                       265
#endif
#ifndef SSL_R_WRONG_SIGNATURE_TYPE
#define SSL_R_WRONG_SIGNATURE_TYPE                       370
#endif
#ifndef SSL_R_WRONG_SSL_VERSION
#define SSL_R_WRONG_SSL_VERSION                          266
#endif
#ifndef SSL_R_WRONG_VERSION_NUMBER
#define SSL_R_WRONG_VERSION_NUMBER                       267
#endif
#ifndef SSL_R_X509_LIB
#define SSL_R_X509_LIB                                   268
#endif
#ifndef SSL_R_X509_VERIFICATION_SETUP_PROBLEMS
#define SSL_R_X509_VERIFICATION_SETUP_PROBLEMS           269
#endif

// the following defines were added in OpenSSL 1.0.2

#ifndef SSL_R_ECDH_REQUIRED_FOR_SUITEB_MODE
#define SSL_R_ECDH_REQUIRED_FOR_SUITEB_MODE  374
#endif
#ifndef SSL_R_CERT_CB_ERROR
#define SSL_R_CERT_CB_ERROR  377
#endif
#ifndef SSL_R_WRONG_CURVE
#define SSL_R_WRONG_CURVE  378
#endif
#ifndef SSL_R_ILLEGAL_SUITEB_DIGEST
#define SSL_R_ILLEGAL_SUITEB_DIGEST  380
#endif
#ifndef SSL_R_MISSING_ECDSA_SIGNING_CERT
#define SSL_R_MISSING_ECDSA_SIGNING_CERT  381
#endif
#ifndef SSL_R_WRONG_CERTIFICATE_TYPE
#define SSL_R_WRONG_CERTIFICATE_TYPE  383
#endif
#ifndef SSL_R_BAD_VALUE
#define SSL_R_BAD_VALUE  384
#endif
#ifndef SSL_R_INVALID_NULL_CMD_NAME
#define SSL_R_INVALID_NULL_CMD_NAME  385
#endif
#ifndef SSL_R_UNKNOWN_CMD_NAME
#define SSL_R_UNKNOWN_CMD_NAME  386
#endif
#ifndef SSL_R_INVALID_SERVERINFO_DATA
#define SSL_R_INVALID_SERVERINFO_DATA  388
#endif
#ifndef SSL_R_NO_PEM_EXTENSIONS
#define SSL_R_NO_PEM_EXTENSIONS  389
#endif
#ifndef SSL_R_BAD_DATA
#define SSL_R_BAD_DATA  390
#endif
#ifndef SSL_R_PEM_NAME_BAD_PREFIX
#define SSL_R_PEM_NAME_BAD_PREFIX  391
#endif
#ifndef SSL_R_PEM_NAME_TOO_SHORT
#define SSL_R_PEM_NAME_TOO_SHORT  392
#endif
#ifndef SSL_R_LENGTH_TOO_LONG
#define SSL_R_LENGTH_TOO_LONG  404
#endif
#ifndef SSL_R_SHUTDOWN_WHILE_IN_INIT
#define SSL_R_SHUTDOWN_WHILE_IN_INIT  407
#endif
#ifndef SSL_R_TOO_MANY_WARN_ALERTS
#define SSL_R_TOO_MANY_WARN_ALERTS  409
#endif

// the following defines were added in OpenSSL 1.1.0

#ifndef SSL_R_BAD_DH_VALUE
#define SSL_R_BAD_DH_VALUE  102
#endif
#ifndef SSL_R_INCONSISTENT_EXTMS
#define SSL_R_INCONSISTENT_EXTMS  104
#endif
#ifndef SSL_R_INVALID_CONFIGURATION_NAME
#define SSL_R_INVALID_CONFIGURATION_NAME  113
#endif
#ifndef SSL_R_SSL_COMMAND_SECTION_EMPTY
#define SSL_R_SSL_COMMAND_SECTION_EMPTY  117
#endif
#ifndef SSL_R_SSL_COMMAND_SECTION_NOT_FOUND
#define SSL_R_SSL_COMMAND_SECTION_NOT_FOUND  125
#endif
#ifndef SSL_R_SSL_SECTION_EMPTY
#define SSL_R_SSL_SECTION_EMPTY  126
#endif
#ifndef SSL_R_SSL_SECTION_NOT_FOUND
#define SSL_R_SSL_SECTION_NOT_FOUND  136
#endif
#ifndef SSL_R_UNKNOWN_COMMAND
#define SSL_R_UNKNOWN_COMMAND  139
#endif
#ifndef SSL_R_AT_LEAST_TLS_1_0_NEEDED_IN_FIPS_MODE
#define SSL_R_AT_LEAST_TLS_1_0_NEEDED_IN_FIPS_MODE  143
#endif
#ifndef SSL_R_AT_LEAST_TLS_1_2_NEEDED_IN_SUITEB_MODE
#define SSL_R_AT_LEAST_TLS_1_2_NEEDED_IN_SUITEB_MODE  158
#endif
#ifndef SSL_R_VERSION_TOO_HIGH
#define SSL_R_VERSION_TOO_HIGH  166
#endif
#ifndef SSL_R_CONTEXT_NOT_DANE_ENABLED
#define SSL_R_CONTEXT_NOT_DANE_ENABLED  167
#endif
#ifndef SSL_R_DANE_ALREADY_ENABLED
#define SSL_R_DANE_ALREADY_ENABLED  172
#endif
#ifndef SSL_R_DANE_CANNOT_OVERRIDE_MTYPE_FULL
#define SSL_R_DANE_CANNOT_OVERRIDE_MTYPE_FULL  173
#endif
#ifndef SSL_R_DANE_NOT_ENABLED
#define SSL_R_DANE_NOT_ENABLED  175
#endif
#ifndef SSL_R_DANE_TLSA_BAD_CERTIFICATE
#define SSL_R_DANE_TLSA_BAD_CERTIFICATE  180
#endif
#ifndef SSL_R_DANE_TLSA_BAD_CERTIFICATE_USAGE
#define SSL_R_DANE_TLSA_BAD_CERTIFICATE_USAGE  184
#endif
#ifndef SSL_R_DANE_TLSA_BAD_DATA_LENGTH
#define SSL_R_DANE_TLSA_BAD_DATA_LENGTH  189
#endif
#ifndef SSL_R_DANE_TLSA_BAD_DIGEST_LENGTH
#define SSL_R_DANE_TLSA_BAD_DIGEST_LENGTH  192
#endif
#ifndef SSL_R_EXCEEDS_MAX_FRAGMENT_SIZE
#define SSL_R_EXCEEDS_MAX_FRAGMENT_SIZE  194
#endif
#ifndef SSL_R_DANE_TLSA_BAD_MATCHING_TYPE
#define SSL_R_DANE_TLSA_BAD_MATCHING_TYPE  200
#endif
#ifndef SSL_R_DANE_TLSA_BAD_PUBLIC_KEY
#define SSL_R_DANE_TLSA_BAD_PUBLIC_KEY  201
#endif
#ifndef SSL_R_DANE_TLSA_BAD_SELECTOR
#define SSL_R_DANE_TLSA_BAD_SELECTOR  202
#endif
#ifndef SSL_R_DANE_TLSA_NULL_DATA
#define SSL_R_DANE_TLSA_NULL_DATA  203
#endif
#ifndef SSL_R_ERROR_SETTING_TLSA_BASE_DOMAIN
#define SSL_R_ERROR_SETTING_TLSA_BASE_DOMAIN  204
#endif
#ifndef SSL_R_CUSTOM_EXT_HANDLER_ALREADY_INSTALLED
#define SSL_R_CUSTOM_EXT_HANDLER_ALREADY_INSTALLED  206
#endif
#ifndef SSL_R_SCT_VERIFICATION_FAILED
#define SSL_R_SCT_VERIFICATION_FAILED  208
#endif
#ifndef SSL_R_SSL_SESSION_VERSION_MISMATCH
#define SSL_R_SSL_SESSION_VERSION_MISMATCH  210
#endif
#ifndef SSL_R_INVALID_CT_VALIDATION_TYPE
#define SSL_R_INVALID_CT_VALIDATION_TYPE  212
#endif
#ifndef SSL_R_NO_VALID_SCTS
#define SSL_R_NO_VALID_SCTS  216
#endif
#ifndef SSL_R_REQUIRED_COMPRESSION_ALGORITHM_MISSING
#define SSL_R_REQUIRED_COMPRESSION_ALGORITHM_MISSING  342
#endif
#ifndef SSL_R_SSL_NEGATIVE_LENGTH
#define SSL_R_SSL_NEGATIVE_LENGTH  372
#endif
#ifndef SSL_R_NO_SHARED_SIGNATURE_ALGORITHMS
#define SSL_R_NO_SHARED_SIGNATURE_ALGORITHMS  376
#endif
#ifndef SSL_R_VERSION_TOO_LOW
#define SSL_R_VERSION_TOO_LOW  396
#endif
#ifndef SSL_R_CA_KEY_TOO_SMALL
#define SSL_R_CA_KEY_TOO_SMALL  397
#endif
#ifndef SSL_R_CA_MD_TOO_WEAK
#define SSL_R_CA_MD_TOO_WEAK  398
#endif
#ifndef SSL_R_EE_KEY_TOO_SMALL
#define SSL_R_EE_KEY_TOO_SMALL  399
#endif
#ifndef SSL_R_COOKIE_GEN_CALLBACK_FAILURE
#define SSL_R_COOKIE_GEN_CALLBACK_FAILURE  400
#endif
#ifndef SSL_R_FRAGMENTED_CLIENT_HELLO
#define SSL_R_FRAGMENTED_CLIENT_HELLO  401
#endif
#ifndef SSL_R_INVALID_SEQUENCE_NUMBER
#define SSL_R_INVALID_SEQUENCE_NUMBER  402
#endif
#ifndef SSL_R_NO_VERIFY_COOKIE_CALLBACK
#define SSL_R_NO_VERIFY_COOKIE_CALLBACK  403
#endif
#ifndef SSL_R_FAILED_TO_INIT_ASYNC
#define SSL_R_FAILED_TO_INIT_ASYNC  405
#endif
#ifndef SSL_R_PIPELINE_FAILURE
#define SSL_R_PIPELINE_FAILURE  406
#endif
#ifndef SSL_R_SSL_SESSION_ID_TOO_LONG
#define SSL_R_SSL_SESSION_ID_TOO_LONG  408
#endif

// the following defines were added in OpenSSL 1.1.1

#ifndef SSL_R_NO_SUITABLE_KEY_SHARE
#define SSL_R_NO_SUITABLE_KEY_SHARE                         101
#endif
#ifndef SSL_R_BAD_KEY_SHARE
#define SSL_R_BAD_KEY_SHARE                                 108
#endif
#ifndef SSL_R_CANNOT_CHANGE_CIPHER
#define SSL_R_CANNOT_CHANGE_CIPHER                          109
#endif
#ifndef SSL_R_BAD_EXTENSION
#define SSL_R_BAD_EXTENSION                                 110
#endif
#ifndef SSL_R_MISSING_SIGALGS_EXTENSION
#define SSL_R_MISSING_SIGALGS_EXTENSION                     112
#endif
#ifndef SSL_R_BAD_PSK_IDENTITY
#define SSL_R_BAD_PSK_IDENTITY                              114
#endif
#ifndef SSL_R_NO_SUITABLE_SIGNATURE_ALGORITHM
#define SSL_R_NO_SUITABLE_SIGNATURE_ALGORITHM               118
#endif
#ifndef SSL_R_INVALID_KEY_UPDATE_TYPE
#define SSL_R_INVALID_KEY_UPDATE_TYPE                       120
#endif
#ifndef SSL_R_STILL_IN_INIT
#define SSL_R_STILL_IN_INIT                                 121
#endif
#ifndef SSL_R_BAD_KEY_UPDATE
#define SSL_R_BAD_KEY_UPDATE                                122
#endif
#ifndef SSL_R_TOO_MANY_KEY_UPDATES
#define SSL_R_TOO_MANY_KEY_UPDATES                          132
#endif
#ifndef SSL_R_ILLEGAL_POINT_COMPRESSION
#define SSL_R_ILLEGAL_POINT_COMPRESSION                     162
#endif
#ifndef SSL_R_EXT_LENGTH_MISMATCH
#define SSL_R_EXT_LENGTH_MISMATCH                           163
#endif
#ifndef SSL_R_TOO_MUCH_EARLY_DATA
#define SSL_R_TOO_MUCH_EARLY_DATA                           164
#endif
#ifndef SSL_R_INVALID_MAX_EARLY_DATA
#define SSL_R_INVALID_MAX_EARLY_DATA                        174
#endif
#ifndef SSL_R_UNEXPECTED_END_OF_EARLY_DATA
#define SSL_R_UNEXPECTED_END_OF_EARLY_DATA                  178
#endif
#ifndef SSL_R_NOT_ON_RECORD_BOUNDARY
#define SSL_R_NOT_ON_RECORD_BOUNDARY                        182
#endif
#ifndef SSL_R_BAD_CIPHER
#define SSL_R_BAD_CIPHER                                    186
#endif
#ifndef SSL_R_INVALID_ALERT
#define SSL_R_INVALID_ALERT                                 205
#endif
#ifndef SSL_R_MISSING_SUPPORTED_GROUPS_EXTENSION
#define SSL_R_MISSING_SUPPORTED_GROUPS_EXTENSION            209
#endif
#ifndef SSL_R_NO_CHANGE_FOLLOWING_HRR
#define SSL_R_NO_CHANGE_FOLLOWING_HRR                       214
#endif
#ifndef SSL_R_UNSOLICITED_EXTENSION
#define SSL_R_UNSOLICITED_EXTENSION                         217
#endif
#ifndef SSL_R_CIPHERSUITE_DIGEST_HAS_CHANGED
#define SSL_R_CIPHERSUITE_DIGEST_HAS_CHANGED                218
#endif
#ifndef SSL_R_BAD_PSK
#define SSL_R_BAD_PSK                                       219
#endif
#ifndef SSL_R_MISSING_SIGNING_CERT
#define SSL_R_MISSING_SIGNING_CERT                          221
#endif
#ifndef SSL_R_INCONSISTENT_EARLY_DATA_ALPN
#define SSL_R_INCONSISTENT_EARLY_DATA_ALPN                  222
#endif
#ifndef SSL_R_INCONSISTENT_EARLY_DATA_SNI
#define SSL_R_INCONSISTENT_EARLY_DATA_SNI                   231
#endif
#ifndef SSL_R_SSL3_EXT_INVALID_MAX_FRAGMENT_LENGTH
#define SSL_R_SSL3_EXT_INVALID_MAX_FRAGMENT_LENGTH          232
#endif
#ifndef SSL_R_BAD_EARLY_DATA
#define SSL_R_BAD_EARLY_DATA                                233
#endif
#ifndef SSL_R_CALLBACK_FAILED
#define SSL_R_CALLBACK_FAILED                               234
#endif
#ifndef SSL_R_NO_APPLICATION_PROTOCOL
#define SSL_R_NO_APPLICATION_PROTOCOL                       235
#endif
#ifndef SSL_R_BAD_HANDSHAKE_STATE
#define SSL_R_BAD_HANDSHAKE_STATE                           236
#endif
#ifndef SSL_R_OVERFLOW_ERROR
#define SSL_R_OVERFLOW_ERROR                                237
#endif
#ifndef SSL_R_INVALID_CERTIFICATE_OR_ALG
#define SSL_R_INVALID_CERTIFICATE_OR_ALG                    238
#endif
#ifndef SSL_R_BAD_PACKET
#define SSL_R_BAD_PACKET                                    240
#endif
#ifndef SSL_R_INSUFFICIENT_SECURITY
#define SSL_R_INSUFFICIENT_SECURITY                         241
#endif
#ifndef SSL_R_BINDER_DOES_NOT_VERIFY
#define SSL_R_BINDER_DOES_NOT_VERIFY                        253
#endif
#ifndef SSL_R_MISSING_FATAL
#define SSL_R_MISSING_FATAL                                 256
#endif
#ifndef SSL_R_INVALID_CCS_MESSAGE
#define SSL_R_INVALID_CCS_MESSAGE                           260
#endif
#ifndef SSL_R_UNEXPECTED_CCS_MESSAGE
#define SSL_R_UNEXPECTED_CCS_MESSAGE                        262
#endif
#ifndef SSL_R_BAD_HRR_VERSION
#define SSL_R_BAD_HRR_VERSION                               263
#endif
#ifndef SSL_R_POST_HANDSHAKE_AUTH_ENCODING_ERR
#define SSL_R_POST_HANDSHAKE_AUTH_ENCODING_ERR              278
#endif
#ifndef SSL_R_EXTENSION_NOT_RECEIVED
#define SSL_R_EXTENSION_NOT_RECEIVED                        279
#endif
#ifndef SSL_R_INVALID_CONTEXT
#define SSL_R_INVALID_CONTEXT                               282
#endif
#ifndef SSL_R_INVALID_CONFIG
#define SSL_R_INVALID_CONFIG                                283
#endif
#ifndef SSL_R_NOT_SERVER
#define SSL_R_NOT_SERVER                                    284
#endif
#ifndef SSL_R_REQUEST_PENDING
#define SSL_R_REQUEST_PENDING                               285
#endif
#ifndef SSL_R_REQUEST_SENT
#define SSL_R_REQUEST_SENT                                  286
#endif
#ifndef SSL_R_NO_COOKIE_CALLBACK_SET
#define SSL_R_NO_COOKIE_CALLBACK_SET                        287
#endif
#ifndef SSL_R_PRIVATE_KEY_MISMATCH
#define SSL_R_PRIVATE_KEY_MISMATCH                          288
#endif
#ifndef SSL_R_NOT_REPLACING_CERTIFICATE
#define SSL_R_NOT_REPLACING_CERTIFICATE                     289
#endif
#ifndef SSL_R_MISSING_PARAMETERS
#define SSL_R_MISSING_PARAMETERS                            290
#endif
#ifndef SSL_R_APPLICATION_DATA_AFTER_CLOSE_NOTIFY
#define SSL_R_APPLICATION_DATA_AFTER_CLOSE_NOTIFY           291
#endif
#ifndef SSL_R_BAD_LEGACY_VERSION
#define SSL_R_BAD_LEGACY_VERSION                            292
#endif
#ifndef SSL_R_NO_SHARED_GROUPS
#define SSL_R_NO_SHARED_GROUPS                              410
#endif
#ifndef SSL_R_BAD_RECORD_TYPE
#define SSL_R_BAD_RECORD_TYPE                               443
#endif
#ifndef SSL_R_INVALID_SESSION_ID
#define SSL_R_INVALID_SESSION_ID                            999
#endif
#ifndef SSL_R_TLSV13_ALERT_MISSING_EXTENSION
#define SSL_R_TLSV13_ALERT_MISSING_EXTENSION               1109
#endif
#ifndef SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED
#define SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED            1116
#endif

// the following defines were added in OpenSSL 3.0

#ifndef SSL_R_MIXED_HANDSHAKE_AND_NON_HANDSHAKE_DATA
#define SSL_R_MIXED_HANDSHAKE_AND_NON_HANDSHAKE_DATA        293
#endif
#ifndef SSL_R_UNEXPECTED_EOF_WHILE_READING
#define SSL_R_UNEXPECTED_EOF_WHILE_READING                  294
#endif
#ifndef SSL_R_NO_SUITABLE_GROUPS
#define SSL_R_NO_SUITABLE_GROUPS                            295
#endif
#ifndef SSL_R_COPY_PARAMETERS_FAILED
#define SSL_R_COPY_PARAMETERS_FAILED                        296
#endif
#ifndef SSL_R_NO_SUITABLE_DIGEST_ALGORITHM
#define SSL_R_NO_SUITABLE_DIGEST_ALGORITHM                  297
#endif
#ifndef SSL_R_CANNOT_GET_GROUP_NAME
#define SSL_R_CANNOT_GET_GROUP_NAME                         299
#endif

// X.509 certificate verification errors use condition codes > DCMTLS_EC_SSL_Offset.

makeOFConditionConst( DCMTLS_EC_TLSAppDataInHandshake,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_APP_DATA_IN_HANDSHAKE,                            OF_error, "TLS error: app data in handshake" );
makeOFConditionConst( DCMTLS_EC_TLSNoSuitableKeyShare,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_SUITABLE_KEY_SHARE,                            OF_error, "TLS error: no suitable key share" );
makeOFConditionConst( DCMTLS_EC_TLSBadDHValue,                             OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_DH_VALUE,                                     OF_error, "TLS error: bad dh value" );
makeOFConditionConst( DCMTLS_EC_TLSBadChangeCipherSpec,                    OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_CHANGE_CIPHER_SPEC,                           OF_error, "TLS error: bad change cipher spec" );
makeOFConditionConst( DCMTLS_EC_TLSInconsistentExtms,                      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INCONSISTENT_EXTMS,                               OF_error, "TLS error: inconsistent extms" );
makeOFConditionConst( DCMTLS_EC_TLSBadHelloRequest,                        OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_HELLO_REQUEST,                                OF_error, "TLS error: bad hello request" );
makeOFConditionConst( DCMTLS_EC_TLSBadDataReturnedByCallback,              OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_DATA_RETURNED_BY_CALLBACK,                    OF_error, "TLS error: bad data returned by callback" );
makeOFConditionConst( DCMTLS_EC_TLSBadDecompression,                       OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_DECOMPRESSION,                                OF_error, "TLS error: bad decompression" );
makeOFConditionConst( DCMTLS_EC_TLSBadKeyShare,                            OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_KEY_SHARE,                                    OF_error, "TLS error: bad key share" );
makeOFConditionConst( DCMTLS_EC_TLSCannotChangeCipher,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_CANNOT_CHANGE_CIPHER,                             OF_error, "TLS error: cannot change cipher" );
makeOFConditionConst( DCMTLS_EC_TLSBadExtension,                           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_EXTENSION,                                    OF_error, "TLS error: bad extension" );
makeOFConditionConst( DCMTLS_EC_TLSBadDigestLength,                        OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_DIGEST_LENGTH,                                OF_error, "TLS error: bad digest length" );
makeOFConditionConst( DCMTLS_EC_TLSMissingSigalgsExtension,                OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_MISSING_SIGALGS_EXTENSION,                        OF_error, "TLS error: missing sigalgs extension" );
makeOFConditionConst( DCMTLS_EC_TLSInvalidConfigurationName,               OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INVALID_CONFIGURATION_NAME,                       OF_error, "TLS error: invalid configuration name" );
makeOFConditionConst( DCMTLS_EC_TLSBadPSKIdentity,                         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_PSK_IDENTITY,                                 OF_error, "TLS error: bad psk identity" );
makeOFConditionConst( DCMTLS_EC_TLSBadPacketLength,                        OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_PACKET_LENGTH,                                OF_error, "TLS error: bad packet length" );
makeOFConditionConst( DCMTLS_EC_TLSBadProtocolVersionNumber,               OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_PROTOCOL_VERSION_NUMBER,                      OF_error, "TLS error: bad protocol version number" );
makeOFConditionConst( DCMTLS_EC_TLSSSLCommandSectionEmpty,                 OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSL_COMMAND_SECTION_EMPTY,                        OF_error, "TLS error: ssl command section empty" );
makeOFConditionConst( DCMTLS_EC_TLSNoSuitableSignatureAlgorithm,           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_SUITABLE_SIGNATURE_ALGORITHM,                  OF_error, "TLS error: no suitable signature algorithm" );
makeOFConditionConst( DCMTLS_EC_TLSBadRSAEncrypt,                          OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_RSA_ENCRYPT,                                  OF_error, "TLS error: bad rsa encrypt" );
makeOFConditionConst( DCMTLS_EC_TLSInvalidKeyUpdateType,                   OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INVALID_KEY_UPDATE_TYPE,                          OF_error, "TLS error: invalid key update type" );
makeOFConditionConst( DCMTLS_EC_TLSStillInInit,                            OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_STILL_IN_INIT,                                    OF_error, "TLS error: still in init" );
makeOFConditionConst( DCMTLS_EC_TLSBadKeyUpdate,                           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_KEY_UPDATE,                                   OF_error, "TLS error: bad key update" );
makeOFConditionConst( DCMTLS_EC_TLSBadSignature,                           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_SIGNATURE,                                    OF_error, "TLS error: bad signature" );
makeOFConditionConst( DCMTLS_EC_TLSBadSSLFiletype,                         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_SSL_FILETYPE,                                 OF_error, "TLS error: bad ssl filetype" );
makeOFConditionConst( DCMTLS_EC_TLSSSLCommandSectionNotFound,              OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSL_COMMAND_SECTION_NOT_FOUND,                    OF_error, "TLS error: ssl command section not found" );
makeOFConditionConst( DCMTLS_EC_TLSSSLSectionEmpty,                        OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSL_SECTION_EMPTY,                                OF_error, "TLS error: ssl section empty" );
makeOFConditionConst( DCMTLS_EC_TLSBadWriteRetry,                          OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_WRITE_RETRY,                                  OF_error, "TLS error: bad write retry" );
makeOFConditionConst( DCMTLS_EC_TLSBioNotSet,                              OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BIO_NOT_SET,                                      OF_error, "TLS error: bio not set" );
makeOFConditionConst( DCMTLS_EC_TLSBlockCipherPadIsWrong,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BLOCK_CIPHER_PAD_IS_WRONG,                        OF_error, "TLS error: block cipher pad is wrong" );
makeOFConditionConst( DCMTLS_EC_TLSBNLib,                                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BN_LIB,                                           OF_error, "TLS error: bn lib" );
makeOFConditionConst( DCMTLS_EC_TLSCADNLengthMismatch,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_CA_DN_LENGTH_MISMATCH,                            OF_error, "TLS error: ca dn length mismatch" );
makeOFConditionConst( DCMTLS_EC_TLSTooManyKeyUpdates,                      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TOO_MANY_KEY_UPDATES,                             OF_error, "TLS error: too many key updates" );
makeOFConditionConst( DCMTLS_EC_TLSCCSReceivedEarly,                       OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_CCS_RECEIVED_EARLY,                               OF_error, "TLS error: ccs received early" );
makeOFConditionConst( DCMTLS_EC_TLSCertificateVerifyFailed,                OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_CERTIFICATE_VERIFY_FAILED,                        OF_error, "TLS error: certificate verify failed" );
makeOFConditionConst( DCMTLS_EC_TLSCertLengthMismatch,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_CERT_LENGTH_MISMATCH,                             OF_error, "TLS error: cert length mismatch" );
makeOFConditionConst( DCMTLS_EC_TLSSSLSectionNotFound,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSL_SECTION_NOT_FOUND,                            OF_error, "TLS error: ssl section not found" );
makeOFConditionConst( DCMTLS_EC_TLSCipherCodeWrongLength,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_CIPHER_CODE_WRONG_LENGTH,                         OF_error, "TLS error: cipher code wrong length" );
makeOFConditionConst( DCMTLS_EC_TLSUnknownCommand,                         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNKNOWN_COMMAND,                                  OF_error, "TLS error: unknown command" );
makeOFConditionConst( DCMTLS_EC_TLSCompressedLengthTooLong,                OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_COMPRESSED_LENGTH_TOO_LONG,                       OF_error, "TLS error: compressed length too long" );
makeOFConditionConst( DCMTLS_EC_TLSCompressionFailure,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_COMPRESSION_FAILURE,                              OF_error, "TLS error: compression failure" );
makeOFConditionConst( DCMTLS_EC_TLSCompressionLibraryError,                OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_COMPRESSION_LIBRARY_ERROR,                        OF_error, "TLS error: compression library error" );
makeOFConditionConst( DCMTLS_EC_TLSAtLeastTLS10NeededInFIPSMode,           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_AT_LEAST_TLS_1_0_NEEDED_IN_FIPS_MODE,             OF_error, "TLS error: at least TLS 1.0 needed in FIPS mode" );
makeOFConditionConst( DCMTLS_EC_TLSConnectionTypeNotSet,                   OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_CONNECTION_TYPE_NOT_SET,                          OF_error, "TLS error: connection type not set" );
makeOFConditionConst( DCMTLS_EC_TLSDataBetweenCCSAndFinished,              OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_DATA_BETWEEN_CCS_AND_FINISHED,                    OF_error, "TLS error: data between ccs and finished" );
makeOFConditionConst( DCMTLS_EC_TLSDataLengthTooLong,                      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_DATA_LENGTH_TOO_LONG,                             OF_error, "TLS error: data length too long" );
makeOFConditionConst( DCMTLS_EC_TLSDecryptionFailed,                       OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_DECRYPTION_FAILED,                                OF_error, "TLS error: decryption failed" );
makeOFConditionConst( DCMTLS_EC_TLSDHPublicValueLengthIsWrong,             OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_DH_PUBLIC_VALUE_LENGTH_IS_WRONG,                  OF_error, "TLS error: dh public value length is wrong" );
makeOFConditionConst( DCMTLS_EC_TLSDigestCheckFailed,                      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_DIGEST_CHECK_FAILED,                              OF_error, "TLS error: digest check failed" );
makeOFConditionConst( DCMTLS_EC_TLSEncryptedLengthTooLong,                 OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_ENCRYPTED_LENGTH_TOO_LONG,                        OF_error, "TLS error: encrypted length too long" );
makeOFConditionConst( DCMTLS_EC_TLSErrorInReceivedCipherList,              OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_ERROR_IN_RECEIVED_CIPHER_LIST,                    OF_error, "TLS error: error in received cipher list" );
makeOFConditionConst( DCMTLS_EC_TLSExcessiveMessageSize,                   OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_EXCESSIVE_MESSAGE_SIZE,                           OF_error, "TLS error: excessive message size" );
makeOFConditionConst( DCMTLS_EC_TLSExtraDataInMessage,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_EXTRA_DATA_IN_MESSAGE,                            OF_error, "TLS error: extra data in message" );
makeOFConditionConst( DCMTLS_EC_TLSGotAFinBeforeACCS,                      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_GOT_A_FIN_BEFORE_A_CCS,                           OF_error, "TLS error: got a fin before a ccs" );
makeOFConditionConst( DCMTLS_EC_TLSHTTPSProxyRequest,                      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_HTTPS_PROXY_REQUEST,                              OF_error, "TLS error: https proxy request" );
makeOFConditionConst( DCMTLS_EC_TLSHTTPRequest,                            OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_HTTP_REQUEST,                                     OF_error, "TLS error: http request" );
makeOFConditionConst( DCMTLS_EC_TLSTLSInvalidECPointformatList,            OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLS_INVALID_ECPOINTFORMAT_LIST,                   OF_error, "TLS error: tls invalid ecpointformat list" );
makeOFConditionConst( DCMTLS_EC_TLSAtLeastTLS12NeededInSuiteBMode,         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_AT_LEAST_TLS_1_2_NEEDED_IN_SUITEB_MODE,           OF_error, "TLS error: at least (D)TLS 1.2 needed in Suite B mode" );
makeOFConditionConst( DCMTLS_EC_TLSLengthMismatch,                         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_LENGTH_MISMATCH,                                  OF_error, "TLS error: length mismatch" );
makeOFConditionConst( DCMTLS_EC_TLSLengthTooShort,                         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_LENGTH_TOO_SHORT,                                 OF_error, "TLS error: length too short" );
makeOFConditionConst( DCMTLS_EC_TLSLibraryHasNoCiphers,                    OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_LIBRARY_HAS_NO_CIPHERS,                           OF_error, "TLS error: library has no ciphers" );
makeOFConditionConst( DCMTLS_EC_TLSIllegalPointCompression,                OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_ILLEGAL_POINT_COMPRESSION,                        OF_error, "TLS error: illegal point compression" );
makeOFConditionConst( DCMTLS_EC_TLSExtLengthMismatch,                      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_EXT_LENGTH_MISMATCH,                              OF_error, "TLS error: ext length mismatch" );
makeOFConditionConst( DCMTLS_EC_TLSTooMuchEarlyData,                       OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TOO_MUCH_EARLY_DATA,                              OF_error, "TLS error: too much early data" );
makeOFConditionConst( DCMTLS_EC_TLSMissingDSASigningCert,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_MISSING_DSA_SIGNING_CERT,                         OF_error, "TLS error: missing dsa signing cert" );
makeOFConditionConst( DCMTLS_EC_TLSVersionTooHigh,                         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_VERSION_TOO_HIGH,                                 OF_error, "TLS error: version too high" );
makeOFConditionConst( DCMTLS_EC_TLSContextNotDANEEnabled,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_CONTEXT_NOT_DANE_ENABLED,                         OF_error, "TLS error: context not dane enabled" );
makeOFConditionConst( DCMTLS_EC_TLSMissingRSACertificate,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_MISSING_RSA_CERTIFICATE,                          OF_error, "TLS error: missing rsa certificate" );
makeOFConditionConst( DCMTLS_EC_TLSMissingRSAEncryptingCert,               OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_MISSING_RSA_ENCRYPTING_CERT,                      OF_error, "TLS error: missing rsa encrypting cert" );
makeOFConditionConst( DCMTLS_EC_TLSMissingRSASigningCert,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_MISSING_RSA_SIGNING_CERT,                         OF_error, "TLS error: missing rsa signing cert" );
makeOFConditionConst( DCMTLS_EC_TLSMissingTmpDHKey,                        OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_MISSING_TMP_DH_KEY,                               OF_error, "TLS error: missing tmp dh key" );
makeOFConditionConst( DCMTLS_EC_TLSDANEAlreadyEnabled,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_DANE_ALREADY_ENABLED,                             OF_error, "TLS error: dane already enabled" );
makeOFConditionConst( DCMTLS_EC_TLSDANECannotOverrideMtypeFull,            OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_DANE_CANNOT_OVERRIDE_MTYPE_FULL,                  OF_error, "TLS error: dane cannot override mtype full" );
makeOFConditionConst( DCMTLS_EC_TLSInvalidMaxEarlyData,                    OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INVALID_MAX_EARLY_DATA,                           OF_error, "TLS error: invalid max early data" );
makeOFConditionConst( DCMTLS_EC_TLSDANENotEnabled,                         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_DANE_NOT_ENABLED,                                 OF_error, "TLS error: dane not enabled" );
makeOFConditionConst( DCMTLS_EC_TLSNoCertificatesReturned,                 OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_CERTIFICATES_RETURNED,                         OF_error, "TLS error: no certificates returned" );
makeOFConditionConst( DCMTLS_EC_TLSNoCertificateAssigned,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_CERTIFICATE_ASSIGNED,                          OF_error, "TLS error: no certificate assigned" );
makeOFConditionConst( DCMTLS_EC_TLSUnexpectedEndOfEarlyData,               OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNEXPECTED_END_OF_EARLY_DATA,                     OF_error, "TLS error: unexpected end of early data" );
makeOFConditionConst( DCMTLS_EC_TLSNoCertificateSet,                       OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_CERTIFICATE_SET,                               OF_error, "TLS error: no certificate set" );
makeOFConditionConst( DCMTLS_EC_TLSDANETLSABadCertificate,                 OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_DANE_TLSA_BAD_CERTIFICATE,                        OF_error, "TLS error: dane tlsa bad certificate" );
makeOFConditionConst( DCMTLS_EC_TLSNoCiphersAvailable,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_CIPHERS_AVAILABLE,                             OF_error, "TLS error: no ciphers available" );
makeOFConditionConst( DCMTLS_EC_TLSNotOnRecordBoundary,                    OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NOT_ON_RECORD_BOUNDARY,                           OF_error, "TLS error: not on record boundary" );
makeOFConditionConst( DCMTLS_EC_TLSNoCiphersSpecified,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_CIPHERS_SPECIFIED,                             OF_error, "TLS error: no ciphers specified" );
makeOFConditionConst( DCMTLS_EC_TLSDANETLSABadCertificateUsage,            OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_DANE_TLSA_BAD_CERTIFICATE_USAGE,                  OF_error, "TLS error: dane tlsa bad certificate usage" );
makeOFConditionConst( DCMTLS_EC_TLSNoCipherMatch,                          OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_CIPHER_MATCH,                                  OF_error, "TLS error: no cipher match" );
makeOFConditionConst( DCMTLS_EC_TLSBadCipher,                              OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_CIPHER,                                       OF_error, "TLS error: bad cipher" );
makeOFConditionConst( DCMTLS_EC_TLSNoCompressionSpecified,                 OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_COMPRESSION_SPECIFIED,                         OF_error, "TLS error: no compression specified" );
makeOFConditionConst( DCMTLS_EC_TLSNoMethodSpecified,                      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_METHOD_SPECIFIED,                              OF_error, "TLS error: no method specified" );
makeOFConditionConst( DCMTLS_EC_TLSDANETLSABadDataLength,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_DANE_TLSA_BAD_DATA_LENGTH,                        OF_error, "TLS error: dane tlsa bad data length" );
makeOFConditionConst( DCMTLS_EC_TLSNoPrivateKeyAssigned,                   OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_PRIVATE_KEY_ASSIGNED,                          OF_error, "TLS error: no private key assigned" );
makeOFConditionConst( DCMTLS_EC_TLSNoProtocolsAvailable,                   OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_PROTOCOLS_AVAILABLE,                           OF_error, "TLS error: no protocols available" );
makeOFConditionConst( DCMTLS_EC_TLSDANETLSABadDigestLength,                OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_DANE_TLSA_BAD_DIGEST_LENGTH,                      OF_error, "TLS error: dane tlsa bad digest length" );
makeOFConditionConst( DCMTLS_EC_TLSNoSharedCipher,                         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_SHARED_CIPHER,                                 OF_error, "TLS error: no shared cipher" );
makeOFConditionConst( DCMTLS_EC_TLSExceedsMaxFragmentSize,                 OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_EXCEEDS_MAX_FRAGMENT_SIZE,                        OF_error, "TLS error: exceeds max fragment size" );
makeOFConditionConst( DCMTLS_EC_TLSNullSSLCtx,                             OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NULL_SSL_CTX,                                     OF_error, "TLS error: null ssl ctx" );
makeOFConditionConst( DCMTLS_EC_TLSNullSSLMethodPassed,                    OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NULL_SSL_METHOD_PASSED,                           OF_error, "TLS error: null ssl method passed" );
makeOFConditionConst( DCMTLS_EC_TLSOldSessionCipherNotReturned,            OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_OLD_SESSION_CIPHER_NOT_RETURNED,                  OF_error, "TLS error: old session cipher not returned" );
makeOFConditionConst( DCMTLS_EC_TLSPacketLengthTooLong,                    OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_PACKET_LENGTH_TOO_LONG,                           OF_error, "TLS error: packet length too long" );
makeOFConditionConst( DCMTLS_EC_TLSPeerDidNotReturnACertificate,           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_PEER_DID_NOT_RETURN_A_CERTIFICATE,                OF_error, "TLS error: peer did not return a certificate" );
makeOFConditionConst( DCMTLS_EC_TLSDANETLSABadMatchingType,                OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_DANE_TLSA_BAD_MATCHING_TYPE,                      OF_error, "TLS error: dane tlsa bad matching type" );
makeOFConditionConst( DCMTLS_EC_TLSDANETLSABadPublicKey,                   OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_DANE_TLSA_BAD_PUBLIC_KEY,                         OF_error, "TLS error: dane tlsa bad public key" );
makeOFConditionConst( DCMTLS_EC_TLSDANETLSABadSelector,                    OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_DANE_TLSA_BAD_SELECTOR,                           OF_error, "TLS error: dane tlsa bad selector" );
makeOFConditionConst( DCMTLS_EC_TLSDANETLSANullData,                       OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_DANE_TLSA_NULL_DATA,                              OF_error, "TLS error: dane tlsa null data" );
makeOFConditionConst( DCMTLS_EC_TLSErrorSettingTLSABaseDomain,             OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_ERROR_SETTING_TLSA_BASE_DOMAIN,                   OF_error, "TLS error: error setting tlsa base domain" );
makeOFConditionConst( DCMTLS_EC_TLSInvalidAlert,                           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INVALID_ALERT,                                    OF_error, "TLS error: invalid alert" );
makeOFConditionConst( DCMTLS_EC_TLSCustomExtHandlerAlreadyInstalled,       OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_CUSTOM_EXT_HANDLER_ALREADY_INSTALLED,             OF_error, "TLS error: custom ext handler already installed" );
makeOFConditionConst( DCMTLS_EC_TLSProtocolIsShutdown,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_PROTOCOL_IS_SHUTDOWN,                             OF_error, "TLS error: protocol is shutdown" );
makeOFConditionConst( DCMTLS_EC_TLSSCTVerificationFailed,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SCT_VERIFICATION_FAILED,                          OF_error, "TLS error: sct verification failed" );
makeOFConditionConst( DCMTLS_EC_TLSMissingSupportedGroupsExtension,        OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_MISSING_SUPPORTED_GROUPS_EXTENSION,               OF_error, "TLS error: missing supported groups extension" );
makeOFConditionConst( DCMTLS_EC_TLSSSLSessionVersionMismatch,              OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSL_SESSION_VERSION_MISMATCH,                     OF_error, "TLS error: ssl session version mismatch" );
makeOFConditionConst( DCMTLS_EC_TLSReadBioNotSet,                          OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_READ_BIO_NOT_SET,                                 OF_error, "TLS error: read bio not set" );
makeOFConditionConst( DCMTLS_EC_TLSInvalidCTValidationType,                OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INVALID_CT_VALIDATION_TYPE,                       OF_error, "TLS error: invalid ct validation type" );
makeOFConditionConst( DCMTLS_EC_TLSRecordLengthMismatch,                   OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_RECORD_LENGTH_MISMATCH,                           OF_error, "TLS error: record length mismatch" );
makeOFConditionConst( DCMTLS_EC_TLSNoChangeFollowingHRR,                   OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_CHANGE_FOLLOWING_HRR,                          OF_error, "TLS error: no change following hrr" );
makeOFConditionConst( DCMTLS_EC_TLSRequiredCipherMissing,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_REQUIRED_CIPHER_MISSING,                          OF_error, "TLS error: required cipher missing" );
makeOFConditionConst( DCMTLS_EC_TLSNoValidSCTs,                            OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_VALID_SCTS,                                    OF_error, "TLS error: no valid scts" );
makeOFConditionConst( DCMTLS_EC_TLSUnsolicitedExtension,                   OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNSOLICITED_EXTENSION,                            OF_error, "TLS error: unsolicited extension" );
makeOFConditionConst( DCMTLS_EC_TLSCiphersuiteDigestHasChanged,            OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_CIPHERSUITE_DIGEST_HAS_CHANGED,                   OF_error, "TLS error: ciphersuite digest has changed" );
makeOFConditionConst( DCMTLS_EC_TLSBadPSK,                                 OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_PSK,                                          OF_error, "TLS error: bad psk" );
makeOFConditionConst( DCMTLS_EC_TLSSignatureForNonSigningCertificate,      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SIGNATURE_FOR_NON_SIGNING_CERTIFICATE,            OF_error, "TLS error: signature for non signing certificate" );
makeOFConditionConst( DCMTLS_EC_TLSMissingSigningCert,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_MISSING_SIGNING_CERT,                             OF_error, "TLS error: missing signing cert" );
makeOFConditionConst( DCMTLS_EC_TLSInconsistentEarlyDataAlpn,              OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INCONSISTENT_EARLY_DATA_ALPN,                     OF_error, "TLS error: inconsistent early data alpn" );
makeOFConditionConst( DCMTLS_EC_TLSPSKIdentityNotFound,                    OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_PSK_IDENTITY_NOT_FOUND,                           OF_error, "TLS error: psk identity not found" );
makeOFConditionConst( DCMTLS_EC_TLSPSKNoClientCb,                          OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_PSK_NO_CLIENT_CB,                                 OF_error, "TLS error: psk no client cb" );
makeOFConditionConst( DCMTLS_EC_TLSPSKNoServerCb,                          OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_PSK_NO_SERVER_CB,                                 OF_error, "TLS error: psk no server cb" );
makeOFConditionConst( DCMTLS_EC_TLSClienthelloTlsext,                      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_CLIENTHELLO_TLSEXT,                               OF_error, "TLS error: clienthello tlsext" );
makeOFConditionConst( DCMTLS_EC_TLSParseTlsext,                            OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_PARSE_TLSEXT,                                     OF_error, "TLS error: parse tlsext" );
makeOFConditionConst( DCMTLS_EC_TLSSSLCtxHasNoDefaultSSLVersion,           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSL_CTX_HAS_NO_DEFAULT_SSL_VERSION,               OF_error, "TLS error: ssl ctx has no default ssl version" );
makeOFConditionConst( DCMTLS_EC_TLSSSLHandshakeFailure,                    OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSL_HANDSHAKE_FAILURE,                            OF_error, "TLS error: ssl handshake failure" );
makeOFConditionConst( DCMTLS_EC_TLSSSLLibraryHasNoCiphers,                 OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSL_LIBRARY_HAS_NO_CIPHERS,                       OF_error, "TLS error: ssl library has no ciphers" );
makeOFConditionConst( DCMTLS_EC_TLSInconsistentEarlyDataSNI,               OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INCONSISTENT_EARLY_DATA_SNI,                      OF_error, "TLS error: inconsistent early data sni" );
makeOFConditionConst( DCMTLS_EC_TLSSSL3ExtInvalidMaxFragmentLength,        OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSL3_EXT_INVALID_MAX_FRAGMENT_LENGTH,             OF_error, "TLS error: ssl3 ext invalid max fragment length" );
makeOFConditionConst( DCMTLS_EC_TLSBadEarlyData,                           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_EARLY_DATA,                                   OF_error, "TLS error: bad early data" );
makeOFConditionConst( DCMTLS_EC_TLSCallbackFailed,                         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_CALLBACK_FAILED,                                  OF_error, "TLS error: callback failed" );
makeOFConditionConst( DCMTLS_EC_TLSNoApplicationProtocol,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_APPLICATION_PROTOCOL,                          OF_error, "TLS error: no application protocol" );
makeOFConditionConst( DCMTLS_EC_TLSBadHandshakeState,                      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_HANDSHAKE_STATE,                              OF_error, "TLS error: bad handshake state" );
makeOFConditionConst( DCMTLS_EC_TLSOverflowError,                          OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_OVERFLOW_ERROR,                                   OF_error, "TLS error: overflow error" );
makeOFConditionConst( DCMTLS_EC_TLSInvalidCertificateOrAlg,                OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INVALID_CERTIFICATE_OR_ALG,                       OF_error, "TLS error: invalid certificate or alg" );
makeOFConditionConst( DCMTLS_EC_TLSUnableToFindPublicKeyParameters,        OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNABLE_TO_FIND_PUBLIC_KEY_PARAMETERS,             OF_error, "TLS error: unable to find public key parameters" );
makeOFConditionConst( DCMTLS_EC_TLSBadPacket,                              OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_PACKET,                                       OF_error, "TLS error: bad packet" );
makeOFConditionConst( DCMTLS_EC_TLSInsufficientSecurity,                   OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INSUFFICIENT_SECURITY,                            OF_error, "TLS error: insufficient security" );
makeOFConditionConst( DCMTLS_EC_TLSUnableToLoadSSL3MD5Routines,            OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNABLE_TO_LOAD_SSL3_MD5_ROUTINES,                 OF_error, "TLS error: unable to load ssl3 md5 routines" );
makeOFConditionConst( DCMTLS_EC_TLSUnableToLoadSSL3SHA1Routines,           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNABLE_TO_LOAD_SSL3_SHA1_ROUTINES,                OF_error, "TLS error: unable to load ssl3 sha1 routines" );
makeOFConditionConst( DCMTLS_EC_TLSUnexpectedMessage,                      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNEXPECTED_MESSAGE,                               OF_error, "TLS error: unexpected message" );
makeOFConditionConst( DCMTLS_EC_TLSUnexpectedRecord,                       OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNEXPECTED_RECORD,                                OF_error, "TLS error: unexpected record" );
makeOFConditionConst( DCMTLS_EC_TLSUnknownAlertType,                       OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNKNOWN_ALERT_TYPE,                               OF_error, "TLS error: unknown alert type" );
makeOFConditionConst( DCMTLS_EC_TLSUnknownCertificateType,                 OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNKNOWN_CERTIFICATE_TYPE,                         OF_error, "TLS error: unknown certificate type" );
makeOFConditionConst( DCMTLS_EC_TLSUnknownCipherReturned,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNKNOWN_CIPHER_RETURNED,                          OF_error, "TLS error: unknown cipher returned" );
makeOFConditionConst( DCMTLS_EC_TLSUnknownCipherType,                      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNKNOWN_CIPHER_TYPE,                              OF_error, "TLS error: unknown cipher type" );
makeOFConditionConst( DCMTLS_EC_TLSUnknownKeyExchangeType,                 OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNKNOWN_KEY_EXCHANGE_TYPE,                        OF_error, "TLS error: unknown key exchange type" );
makeOFConditionConst( DCMTLS_EC_TLSUnknownPkeyType,                        OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNKNOWN_PKEY_TYPE,                                OF_error, "TLS error: unknown pkey type" );
makeOFConditionConst( DCMTLS_EC_TLSUnknownProtocol,                        OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNKNOWN_PROTOCOL,                                 OF_error, "TLS error: unknown protocol" );
makeOFConditionConst( DCMTLS_EC_TLSBinderDoesNotVerify,                    OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BINDER_DOES_NOT_VERIFY,                           OF_error, "TLS error: binder does not verify" );
makeOFConditionConst( DCMTLS_EC_TLSUnknownSSLVersion,                      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNKNOWN_SSL_VERSION,                              OF_error, "TLS error: unknown ssl version" );
makeOFConditionConst( DCMTLS_EC_TLSUnknownState,                           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNKNOWN_STATE,                                    OF_error, "TLS error: unknown state" );
makeOFConditionConst( DCMTLS_EC_TLSMissingFatal,                           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_MISSING_FATAL,                                    OF_error, "TLS error: missing fatal" );
makeOFConditionConst( DCMTLS_EC_TLSUnsupportedCompressionAlgorithm,        OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNSUPPORTED_COMPRESSION_ALGORITHM,                OF_error, "TLS error: unsupported compression algorithm" );
makeOFConditionConst( DCMTLS_EC_TLSUnsupportedProtocol,                    OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNSUPPORTED_PROTOCOL,                             OF_error, "TLS error: unsupported protocol" );
makeOFConditionConst( DCMTLS_EC_TLSUnsupportedSSLVersion,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNSUPPORTED_SSL_VERSION,                          OF_error, "TLS error: unsupported ssl version" );
makeOFConditionConst( DCMTLS_EC_TLSInvalidCCSMessage,                      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INVALID_CCS_MESSAGE,                              OF_error, "TLS error: invalid ccs message" );
makeOFConditionConst( DCMTLS_EC_TLSWrongCipherReturned,                    OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_WRONG_CIPHER_RETURNED,                            OF_error, "TLS error: wrong cipher returned" );
makeOFConditionConst( DCMTLS_EC_TLSUnexpectedCCSMessage,                   OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNEXPECTED_CCS_MESSAGE,                           OF_error, "TLS error: unexpected ccs message" );
makeOFConditionConst( DCMTLS_EC_TLSBadHrrVersion,                          OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_HRR_VERSION,                                  OF_error, "TLS error: bad hrr version" );
makeOFConditionConst( DCMTLS_EC_TLSWrongSignatureLength,                   OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_WRONG_SIGNATURE_LENGTH,                           OF_error, "TLS error: wrong signature length" );
makeOFConditionConst( DCMTLS_EC_TLSWrongSignatureSize,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_WRONG_SIGNATURE_SIZE,                             OF_error, "TLS error: wrong signature size" );
makeOFConditionConst( DCMTLS_EC_TLSWrongSSLVersion,                        OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_WRONG_SSL_VERSION,                                OF_error, "TLS error: wrong ssl version" );
makeOFConditionConst( DCMTLS_EC_TLSWrongVersionNumber,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_WRONG_VERSION_NUMBER,                             OF_error, "TLS error: wrong version number" );
makeOFConditionConst( DCMTLS_EC_TLSX509Lib,                                OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_X509_LIB,                                         OF_error, "TLS error: x509 lib" );
makeOFConditionConst( DCMTLS_EC_TLSX509VerificationSetupProblems,          OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_X509_VERIFICATION_SETUP_PROBLEMS,                 OF_error, "TLS error: x509 verification setup problems" );
makeOFConditionConst( DCMTLS_EC_TLSPathTooLong,                            OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_PATH_TOO_LONG,                                    OF_error, "TLS error: path too long" );
makeOFConditionConst( DCMTLS_EC_TLSBadLength,                              OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_LENGTH,                                       OF_error, "TLS error: bad length" );
makeOFConditionConst( DCMTLS_EC_TLSAttemptToReuseSessionInDifferentContext, OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_ATTEMPT_TO_REUSE_SESSION_IN_DIFFERENT_CONTEXT,   OF_error, "TLS error: attempt to reuse session in different context" );
makeOFConditionConst( DCMTLS_EC_TLSSSLSessionIDContextTooLong,             OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSL_SESSION_ID_CONTEXT_TOO_LONG,                  OF_error, "TLS error: ssl session id context too long" );
makeOFConditionConst( DCMTLS_EC_TLSLibraryBug,                             OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_LIBRARY_BUG,                                      OF_error, "TLS error: library bug" );
makeOFConditionConst( DCMTLS_EC_TLSServerhelloTlsext,                      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SERVERHELLO_TLSEXT,                               OF_error, "TLS error: serverhello tlsext" );
makeOFConditionConst( DCMTLS_EC_TLSUninitialized,                          OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNINITIALIZED,                                    OF_error, "TLS error: uninitialized" );
makeOFConditionConst( DCMTLS_EC_TLSSessionIDContextUninitialized,          OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SESSION_ID_CONTEXT_UNINITIALIZED,                 OF_error, "TLS error: session id context uninitialized" );
makeOFConditionConst( DCMTLS_EC_TLSPostHandshakeAuthEncodingErr,           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_POST_HANDSHAKE_AUTH_ENCODING_ERR,                 OF_error, "TLS error: post handshake auth encoding err" );
makeOFConditionConst( DCMTLS_EC_TLSExtensionNotReceived,                   OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_EXTENSION_NOT_RECEIVED,                           OF_error, "TLS error: extension not received" );
makeOFConditionConst( DCMTLS_EC_TLSInvalidCommand,                         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INVALID_COMMAND,                                  OF_error, "TLS error: invalid command" );
makeOFConditionConst( DCMTLS_EC_TLSDecryptionFailedOrBadRecordMac,         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_DECRYPTION_FAILED_OR_BAD_RECORD_MAC,              OF_error, "TLS error: decryption failed or bad record mac" );
makeOFConditionConst( DCMTLS_EC_TLSInvalidContext,                         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INVALID_CONTEXT,                                  OF_error, "TLS error: invalid context" );
makeOFConditionConst( DCMTLS_EC_TLSInvalidConfig,                          OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INVALID_CONFIG,                                   OF_error, "TLS error: invalid config" );
makeOFConditionConst( DCMTLS_EC_TLSNotServer,                              OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NOT_SERVER,                                       OF_error, "TLS error: not server" );
makeOFConditionConst( DCMTLS_EC_TLSRequestPending,                         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_REQUEST_PENDING,                                  OF_error, "TLS error: request pending" );
makeOFConditionConst( DCMTLS_EC_TLSRequestSent,                            OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_REQUEST_SENT,                                     OF_error, "TLS error: request sent" );
makeOFConditionConst( DCMTLS_EC_TLSNoCookieCallbackSet,                    OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_COOKIE_CALLBACK_SET,                           OF_error, "TLS error: no cookie callback set" );
makeOFConditionConst( DCMTLS_EC_TLSPrivateKeyMismatch,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_PRIVATE_KEY_MISMATCH,                             OF_error, "TLS error: private key mismatch" );
makeOFConditionConst( DCMTLS_EC_TLSNotReplacingCertificate,                OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NOT_REPLACING_CERTIFICATE,                        OF_error, "TLS error: not replacing certificate" );
makeOFConditionConst( DCMTLS_EC_TLSMissingParameters,                      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_MISSING_PARAMETERS,                               OF_error, "TLS error: missing parameters" );
makeOFConditionConst( DCMTLS_EC_TLSApplicationDataAfterCloseNotify,        OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_APPLICATION_DATA_AFTER_CLOSE_NOTIFY,              OF_error, "TLS error: application data after close notify" );
makeOFConditionConst( DCMTLS_EC_TLSBadLegacyVersion,                       OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_LEGACY_VERSION,                               OF_error, "TLS error: bad legacy version" );
makeOFConditionConst( DCMTLS_EC_TLSMixedHandshakeAndNonHandshakeData,      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_MIXED_HANDSHAKE_AND_NON_HANDSHAKE_DATA,           OF_error, "TLS error: mixed handshake and non handshake data" );
makeOFConditionConst( DCMTLS_EC_TLSUnexpectedEofWhileReading,              OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNEXPECTED_EOF_WHILE_READING,                     OF_error, "TLS error: unexpected eof while reading" );
makeOFConditionConst( DCMTLS_EC_TLSNoSuitableGroups,                       OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_SUITABLE_GROUPS,                               OF_error, "TLS error: no suitable groups" );
makeOFConditionConst( DCMTLS_EC_TLSCopyParametersFailed,                   OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_COPY_PARAMETERS_FAILED,                           OF_error, "TLS error: copy parameters failed" );
makeOFConditionConst( DCMTLS_EC_TLSNoSuitableDigestAlgorithm,              OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_SUITABLE_DIGEST_ALGORITHM,                     OF_error, "TLS error: no suitable digest algorithm" );
makeOFConditionConst( DCMTLS_EC_TLSRecordTooSmall,                         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_RECORD_TOO_SMALL,                                 OF_error, "TLS error: record too small" );
makeOFConditionConst( DCMTLS_EC_TLSCannotGetGroupName,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_CANNOT_GET_GROUP_NAME,                            OF_error, "TLS error: cannot get group name" );
makeOFConditionConst( DCMTLS_EC_TLSSSL3SessionIDTooLong,                   OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSL3_SESSION_ID_TOO_LONG,                         OF_error, "TLS error: ssl3 session id too long" );
makeOFConditionConst( DCMTLS_EC_TLSSSLSessionIDCallbackFailed,             OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSL_SESSION_ID_CALLBACK_FAILED,                   OF_error, "TLS error: ssl session id callback failed" );
makeOFConditionConst( DCMTLS_EC_TLSSSLSessionIDConflict,                   OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSL_SESSION_ID_CONFLICT,                          OF_error, "TLS error: ssl session id conflict" );
makeOFConditionConst( DCMTLS_EC_TLSSSLSessionIDHasBadLength,               OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSL_SESSION_ID_HAS_BAD_LENGTH,                    OF_error, "TLS error: ssl session id has bad length" );
makeOFConditionConst( DCMTLS_EC_TLSBadECCCert,                             OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_ECC_CERT,                                     OF_error, "TLS error: bad ecc cert" );
makeOFConditionConst( DCMTLS_EC_TLSBadEcpoint,                             OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_ECPOINT,                                      OF_error, "TLS error: bad ecpoint" );
makeOFConditionConst( DCMTLS_EC_TLSCompressionIDNotWithinPrivateRange,     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_COMPRESSION_ID_NOT_WITHIN_PRIVATE_RANGE,          OF_error, "TLS error: compression id not within private range" );
makeOFConditionConst( DCMTLS_EC_TLSCookieMismatch,                         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_COOKIE_MISMATCH,                                  OF_error, "TLS error: cookie mismatch" );
makeOFConditionConst( DCMTLS_EC_TLSDuplicateCompressionId,                 OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_DUPLICATE_COMPRESSION_ID,                         OF_error, "TLS error: duplicate compression id" );
makeOFConditionConst( DCMTLS_EC_TLSMissingTmpECDHKey,                      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_MISSING_TMP_ECDH_KEY,                             OF_error, "TLS error: missing tmp ecdh key" );
makeOFConditionConst( DCMTLS_EC_TLSReadTimeoutExpired,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_READ_TIMEOUT_EXPIRED,                             OF_error, "TLS error: read timeout expired" );
makeOFConditionConst( DCMTLS_EC_TLSUnableToFindECDHParameters,             OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNABLE_TO_FIND_ECDH_PARAMETERS,                   OF_error, "TLS error: unable to find ecdh parameters" );
makeOFConditionConst( DCMTLS_EC_TLSUnsupportedEllipticCurve,               OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNSUPPORTED_ELLIPTIC_CURVE,                       OF_error, "TLS error: unsupported elliptic curve" );
makeOFConditionConst( DCMTLS_EC_TLSECCCertNotForSigning,                   OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_ECC_CERT_NOT_FOR_SIGNING,                         OF_error, "TLS error: ecc cert not for signing" );
makeOFConditionConst( DCMTLS_EC_TLSSSL3ExtInvalidServername,               OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSL3_EXT_INVALID_SERVERNAME,                      OF_error, "TLS error: ssl3 ext invalid servername" );
makeOFConditionConst( DCMTLS_EC_TLSSSL3ExtInvalidServernameType,           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSL3_EXT_INVALID_SERVERNAME_TYPE,                 OF_error, "TLS error: ssl3 ext invalid servername type" );
makeOFConditionConst( DCMTLS_EC_TLSNoRequiredDigest,                       OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_REQUIRED_DIGEST,                               OF_error, "TLS error: no required digest" );
makeOFConditionConst( DCMTLS_EC_TLSInvalidTicketKeysLength,                OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INVALID_TICKET_KEYS_LENGTH,                       OF_error, "TLS error: invalid ticket keys length" );
makeOFConditionConst( DCMTLS_EC_TLSInvalidStatusResponse,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INVALID_STATUS_RESPONSE,                          OF_error, "TLS error: invalid status response" );
makeOFConditionConst( DCMTLS_EC_TLSUnsupportedStatusType,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNSUPPORTED_STATUS_TYPE,                          OF_error, "TLS error: unsupported status type" );
makeOFConditionConst( DCMTLS_EC_TLSNoGostCertificateSentByPeer,            OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_GOST_CERTIFICATE_SENT_BY_PEER,                 OF_error, "TLS error: Peer haven't sent GOST certificate, required for selected ciphersuite" );
makeOFConditionConst( DCMTLS_EC_TLSNoClientCertMethod,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_CLIENT_CERT_METHOD,                            OF_error, "TLS error: no client cert method" );
makeOFConditionConst( DCMTLS_EC_TLSBadHandshakeLength,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_HANDSHAKE_LENGTH,                             OF_error, "TLS error: bad handshake length" );
makeOFConditionConst( DCMTLS_EC_TLSDTLSMessageTooBig,                      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_DTLS_MESSAGE_TOO_BIG,                             OF_error, "TLS error: dtls message too big" );
makeOFConditionConst( DCMTLS_EC_TLSRenegotiateExtTooLong,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_RENEGOTIATE_EXT_TOO_LONG,                         OF_error, "TLS error: renegotiate ext too long" );
makeOFConditionConst( DCMTLS_EC_TLSRenegotiationEncodingErr,               OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_RENEGOTIATION_ENCODING_ERR,                       OF_error, "TLS error: renegotiation encoding err" );
makeOFConditionConst( DCMTLS_EC_TLSRenegotiationMismatch,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_RENEGOTIATION_MISMATCH,                           OF_error, "TLS error: renegotiation mismatch" );
makeOFConditionConst( DCMTLS_EC_TLSUnsafeLegacyRenegotiationDisabled,      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNSAFE_LEGACY_RENEGOTIATION_DISABLED,             OF_error, "TLS error: unsafe legacy renegotiation disabled" );
makeOFConditionConst( DCMTLS_EC_TLSNoRenegotiation,                        OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_RENEGOTIATION,                                 OF_error, "TLS error: no renegotiation" );
makeOFConditionConst( DCMTLS_EC_TLSInconsistentCompression,                OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INCONSISTENT_COMPRESSION,                         OF_error, "TLS error: inconsistent compression" );
makeOFConditionConst( DCMTLS_EC_TLSInvalidCompressionAlgorithm,            OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INVALID_COMPRESSION_ALGORITHM,                    OF_error, "TLS error: invalid compression algorithm" );
makeOFConditionConst( DCMTLS_EC_TLSRequiredCompressionAlgorithmMissing,    OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_REQUIRED_COMPRESSION_ALGORITHM_MISSING,           OF_error, "TLS error: required compression algorithm missing" );
makeOFConditionConst( DCMTLS_EC_TLSCompressionDisabled,                    OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_COMPRESSION_DISABLED,                             OF_error, "TLS error: compression disabled" );
makeOFConditionConst( DCMTLS_EC_TLSOldSessionCompressionAlgorithmNotReturned, OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_OLD_SESSION_COMPRESSION_ALGORITHM_NOT_RETURNED,OF_error, "TLS error: old session compression algorithm not returned" );
makeOFConditionConst( DCMTLS_EC_TLSSCSVReceivedWhenRenegotiating,          OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SCSV_RECEIVED_WHEN_RENEGOTIATING,                 OF_error, "TLS error: scsv received when renegotiating" );
makeOFConditionConst( DCMTLS_EC_TLSBadSRPALength,                          OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_SRP_A_LENGTH,                                 OF_error, "TLS error: bad srp a length" );
makeOFConditionConst( DCMTLS_EC_TLSBadSrtpMkiValue,                        OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_SRTP_MKI_VALUE,                               OF_error, "TLS error: bad srtp mki value" );
makeOFConditionConst( DCMTLS_EC_TLSBadSrtpProtectionProfileList,           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_SRTP_PROTECTION_PROFILE_LIST,                 OF_error, "TLS error: bad srtp protection profile list" );
makeOFConditionConst( DCMTLS_EC_TLSEmptySrtpProtectionProfileList,         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_EMPTY_SRTP_PROTECTION_PROFILE_LIST,               OF_error, "TLS error: empty srtp protection profile list" );
makeOFConditionConst( DCMTLS_EC_TLSInvalidSrpUsername,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INVALID_SRP_USERNAME,                             OF_error, "TLS error: invalid srp username" );
makeOFConditionConst( DCMTLS_EC_TLSMissingSrpParam,                        OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_MISSING_SRP_PARAM,                                OF_error, "TLS error: can't find SRP server param" );
makeOFConditionConst( DCMTLS_EC_TLSNoSrtpProfiles,                         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_SRTP_PROFILES,                                 OF_error, "TLS error: no srtp profiles" );
makeOFConditionConst( DCMTLS_EC_TLSSignatureAlgorithmsError,               OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SIGNATURE_ALGORITHMS_ERROR,                       OF_error, "TLS error: signature algorithms error" );
makeOFConditionConst( DCMTLS_EC_TLSSrpACalc,                               OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SRP_A_CALC,                                       OF_error, "TLS error: error with the srp params" );
makeOFConditionConst( DCMTLS_EC_TLSSrtpCouldNotAllocateProfiles,           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SRTP_COULD_NOT_ALLOCATE_PROFILES,                 OF_error, "TLS error: srtp could not allocate profiles" );
makeOFConditionConst( DCMTLS_EC_TLSSrtpProtectionProfileListTooLong,       OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SRTP_PROTECTION_PROFILE_LIST_TOO_LONG,            OF_error, "TLS error: srtp protection profile list too long" );
makeOFConditionConst( DCMTLS_EC_TLSSrtpUnknownProtectionProfile,           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SRTP_UNKNOWN_PROTECTION_PROFILE,                  OF_error, "TLS error: srtp unknown protection profile" );
makeOFConditionConst( DCMTLS_EC_TLSTLSIllegalExporterLabel,                OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLS_ILLEGAL_EXPORTER_LABEL,                       OF_error, "TLS error: tls illegal exporter label" );
makeOFConditionConst( DCMTLS_EC_TLSUnknownDigest,                          OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNKNOWN_DIGEST,                                   OF_error, "TLS error: unknown digest" );
makeOFConditionConst( DCMTLS_EC_TLSUseSrtpNotNegotiated,                   OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_USE_SRTP_NOT_NEGOTIATED,                          OF_error, "TLS error: use srtp not negotiated" );
makeOFConditionConst( DCMTLS_EC_TLSWrongSignatureType,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_WRONG_SIGNATURE_TYPE,                             OF_error, "TLS error: wrong signature type" );
makeOFConditionConst( DCMTLS_EC_TLSBadSrpParameters,                       OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_SRP_PARAMETERS,                               OF_error, "TLS error: bad srp parameters" );
makeOFConditionConst( DCMTLS_EC_TLSSSLNegativeLength,                      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSL_NEGATIVE_LENGTH,                              OF_error, "TLS error: ssl negative length" );
makeOFConditionConst( DCMTLS_EC_TLSInappropriateFallback,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INAPPROPRIATE_FALLBACK,                           OF_error, "TLS error: inappropriate fallback" );
makeOFConditionConst( DCMTLS_EC_TLSECDHRequiredForSuiteBMode,              OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_ECDH_REQUIRED_FOR_SUITEB_MODE,                    OF_error, "TLS error: ecdh required for suiteb mode" );
makeOFConditionConst( DCMTLS_EC_TLSNoSharedSignatureAlgorithms,            OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_SHARED_SIGNATURE_ALGORITHMS,                   OF_error, "TLS error: no shared signature algorithms" );
makeOFConditionConst( DCMTLS_EC_TLSCertCbError,                            OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_CERT_CB_ERROR,                                    OF_error, "TLS error: cert cb error" );
makeOFConditionConst( DCMTLS_EC_TLSWrongCurve,                             OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_WRONG_CURVE,                                      OF_error, "TLS error: wrong curve" );
makeOFConditionConst( DCMTLS_EC_TLSIllegalSuiteBDigest,                    OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_ILLEGAL_SUITEB_DIGEST,                            OF_error, "TLS error: illegal Suite B digest" );
makeOFConditionConst( DCMTLS_EC_TLSMissingECDSASigningCert,                OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_MISSING_ECDSA_SIGNING_CERT,                       OF_error, "TLS error: missing ecdsa signing cert" );
makeOFConditionConst( DCMTLS_EC_TLSWrongCertificateType,                   OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_WRONG_CERTIFICATE_TYPE,                           OF_error, "TLS error: wrong certificate type" );
makeOFConditionConst( DCMTLS_EC_TLSBadValue,                               OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_VALUE,                                        OF_error, "TLS error: bad value" );
makeOFConditionConst( DCMTLS_EC_TLSInvalidNullCmdName,                     OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INVALID_NULL_CMD_NAME,                            OF_error, "TLS error: invalid null cmd name" );
makeOFConditionConst( DCMTLS_EC_TLSUnknownCmdName,                         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_UNKNOWN_CMD_NAME,                                 OF_error, "TLS error: unknown cmd name" );
makeOFConditionConst( DCMTLS_EC_TLSInvalidServerinfoData,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INVALID_SERVERINFO_DATA,                          OF_error, "TLS error: invalid serverinfo data" );
makeOFConditionConst( DCMTLS_EC_TLSNoPEMExtensions,                        OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_PEM_EXTENSIONS,                                OF_error, "TLS error: no pem extensions" );
makeOFConditionConst( DCMTLS_EC_TLSBadData,                                OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_DATA,                                         OF_error, "TLS error: bad data" );
makeOFConditionConst( DCMTLS_EC_TLSPEMNameBadPrefix,                       OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_PEM_NAME_BAD_PREFIX,                              OF_error, "TLS error: pem name bad prefix" );
makeOFConditionConst( DCMTLS_EC_TLSPEMNameTooShort,                        OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_PEM_NAME_TOO_SHORT,                               OF_error, "TLS error: pem name too short" );
makeOFConditionConst( DCMTLS_EC_TLSDHKeyTooSmall,                          OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_DH_KEY_TOO_SMALL,                                 OF_error, "TLS error: dh key too small" );
makeOFConditionConst( DCMTLS_EC_TLSVersionTooLow,                          OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_VERSION_TOO_LOW,                                  OF_error, "TLS error: version too low" );
makeOFConditionConst( DCMTLS_EC_TLSCAKeyTooSmall,                          OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_CA_KEY_TOO_SMALL,                                 OF_error, "TLS error: ca key too small" );
makeOFConditionConst( DCMTLS_EC_TLSCAMDTooWeak,                            OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_CA_MD_TOO_WEAK,                                   OF_error, "TLS error: ca md too weak" );
makeOFConditionConst( DCMTLS_EC_TLSEeKeyTooSmall,                          OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_EE_KEY_TOO_SMALL,                                 OF_error, "TLS error: ee key too small" );
makeOFConditionConst( DCMTLS_EC_TLSCookieGenCallbackFailure,               OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_COOKIE_GEN_CALLBACK_FAILURE,                      OF_error, "TLS error: cookie gen callback failure" );
makeOFConditionConst( DCMTLS_EC_TLSFragmentedClientHello,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_FRAGMENTED_CLIENT_HELLO,                          OF_error, "TLS error: fragmented client hello" );
makeOFConditionConst( DCMTLS_EC_TLSInvalidSequenceNumber,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INVALID_SEQUENCE_NUMBER,                          OF_error, "TLS error: invalid sequence number" );
makeOFConditionConst( DCMTLS_EC_TLSNoVerifyCookieCallback,                 OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_VERIFY_COOKIE_CALLBACK,                        OF_error, "TLS error: no verify cookie callback" );
makeOFConditionConst( DCMTLS_EC_TLSLengthTooLong,                          OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_LENGTH_TOO_LONG,                                  OF_error, "TLS error: length too long" );
makeOFConditionConst( DCMTLS_EC_TLSFailedToInitAsync,                      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_FAILED_TO_INIT_ASYNC,                             OF_error, "TLS error: failed to init async" );
makeOFConditionConst( DCMTLS_EC_TLSPipelineFailure,                        OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_PIPELINE_FAILURE,                                 OF_error, "TLS error: pipeline failure" );
makeOFConditionConst( DCMTLS_EC_TLSShutdownWhileInInit,                    OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SHUTDOWN_WHILE_IN_INIT,                           OF_error, "TLS error: shutdown while in init" );
makeOFConditionConst( DCMTLS_EC_TLSSSLSessionIDTooLong,                    OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSL_SESSION_ID_TOO_LONG,                          OF_error, "TLS error: ssl session id too long" );
makeOFConditionConst( DCMTLS_EC_TLSTooManyWarnAlerts,                      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TOO_MANY_WARN_ALERTS,                             OF_error, "TLS error: too many warn alerts" );
makeOFConditionConst( DCMTLS_EC_TLSNoSharedGroups,                         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_NO_SHARED_GROUPS,                                 OF_error, "TLS error: no shared groups" );
makeOFConditionConst( DCMTLS_EC_TLSBadRecordType,                          OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_BAD_RECORD_TYPE,                                  OF_error, "TLS error: bad record type" );
makeOFConditionConst( DCMTLS_EC_TLSInvalidSessionId,                       OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_INVALID_SESSION_ID,                               OF_error, "TLS error: invalid session id" );
makeOFConditionConst( DCMTLS_EC_TLSSSLv3AlertUnexpectedMessage,            OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSLV3_ALERT_UNEXPECTED_MESSAGE,                   OF_error, "TLS alert: unexpected message" );
makeOFConditionConst( DCMTLS_EC_TLSSSLv3AlertBadRecordMac,                 OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSLV3_ALERT_BAD_RECORD_MAC,                       OF_error, "TLS alert: bad record mac" );
makeOFConditionConst( DCMTLS_EC_TLSTLSv1AlertDecryptionFailed,             OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLSV1_ALERT_DECRYPTION_FAILED,                    OF_error, "TLS alert: decryption failed" );
makeOFConditionConst( DCMTLS_EC_TLSTLSv1AlertRecordOverflow,               OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLSV1_ALERT_RECORD_OVERFLOW,                      OF_error, "TLS alert: record overflow" );
makeOFConditionConst( DCMTLS_EC_TLSSSLv3AlertDecompressionFailure,         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSLV3_ALERT_DECOMPRESSION_FAILURE,                OF_error, "TLS alert: decompression failure" );
makeOFConditionConst( DCMTLS_EC_TLSSSLv3AlertHandshakeFailure,             OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSLV3_ALERT_HANDSHAKE_FAILURE,                    OF_error, "TLS alert: handshake failure" );
makeOFConditionConst( DCMTLS_EC_TLSSSLv3AlertNoCertificate,                OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSLV3_ALERT_NO_CERTIFICATE,                       OF_error, "TLS alert: no certificate" );
makeOFConditionConst( DCMTLS_EC_TLSSSLv3AlertBadCertificate,               OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSLV3_ALERT_BAD_CERTIFICATE,                      OF_error, "TLS alert: bad certificate" );
makeOFConditionConst( DCMTLS_EC_TLSSSLv3AlertUnsupportedCertificate,       OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSLV3_ALERT_UNSUPPORTED_CERTIFICATE,              OF_error, "TLS alert: unsupported certificate" );
makeOFConditionConst( DCMTLS_EC_TLSSSLv3AlertCertificateRevoked,           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSLV3_ALERT_CERTIFICATE_REVOKED,                  OF_error, "TLS alert: certificate revoked" );
makeOFConditionConst( DCMTLS_EC_TLSSSLv3AlertCertificateExpired,           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSLV3_ALERT_CERTIFICATE_EXPIRED,                  OF_error, "TLS alert: certificate expired" );
makeOFConditionConst( DCMTLS_EC_TLSSSLv3AlertCertificateUnknown,           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSLV3_ALERT_CERTIFICATE_UNKNOWN,                  OF_error, "TLS alert: certificate unknown" );
makeOFConditionConst( DCMTLS_EC_TLSSSLv3AlertIllegalParameter,             OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_SSLV3_ALERT_ILLEGAL_PARAMETER,                    OF_error, "TLS alert: illegal parameter" );
makeOFConditionConst( DCMTLS_EC_TLSTLSv1AlertUnknownCa,                    OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLSV1_ALERT_UNKNOWN_CA,                           OF_error, "TLS alert: unknown ca," );
makeOFConditionConst( DCMTLS_EC_TLSTLSv1AlertAccessDenied,                 OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLSV1_ALERT_ACCESS_DENIED,                        OF_error, "TLS alert: access denied" );
makeOFConditionConst( DCMTLS_EC_TLSTLSv1AlertDecodeError,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLSV1_ALERT_DECODE_ERROR,                         OF_error, "TLS alert: decode error," );
makeOFConditionConst( DCMTLS_EC_TLSTLSv1AlertDecryptError,                 OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLSV1_ALERT_DECRYPT_ERROR,                        OF_error, "TLS alert: decrypt error" );
makeOFConditionConst( DCMTLS_EC_TLSTLSv1AlertExportRestriction,            OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLSV1_ALERT_EXPORT_RESTRICTION,                   OF_error, "TLS alert: export restriction" );
makeOFConditionConst( DCMTLS_EC_TLSTLSv1AlertProtocolVersion,              OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLSV1_ALERT_PROTOCOL_VERSION,                     OF_error, "TLS alert: protocol version" );
makeOFConditionConst( DCMTLS_EC_TLSTLSv1AlertInsufficientSecurity,         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLSV1_ALERT_INSUFFICIENT_SECURITY,                OF_error, "TLS alert: insufficient security" );
makeOFConditionConst( DCMTLS_EC_TLSTLSv1AlertInternalError,                OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLSV1_ALERT_INTERNAL_ERROR,                       OF_error, "TLS alert: internal error" );
makeOFConditionConst( DCMTLS_EC_TLSTLSv1AlertInappropriateFallback,        OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLSV1_ALERT_INAPPROPRIATE_FALLBACK,               OF_error, "TLS alert: inappropriate fallback" );
makeOFConditionConst( DCMTLS_EC_TLSTLSv1AlertUserCancelled,                OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLSV1_ALERT_USER_CANCELLED,                       OF_error, "TLS alert: user cancelled" );
makeOFConditionConst( DCMTLS_EC_TLSTLSv1AlertNoRenegotiation,              OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLSV1_ALERT_NO_RENEGOTIATION,                     OF_error, "TLS alert: no renegotiation" );
makeOFConditionConst( DCMTLS_EC_TLSTLSv13AlertMissingExtension,            OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLSV13_ALERT_MISSING_EXTENSION,                   OF_error, "TLS alert: missing extension" );
makeOFConditionConst( DCMTLS_EC_TLSTLSv1UnsupportedExtension,              OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLSV1_UNSUPPORTED_EXTENSION,                      OF_error, "TLS error: unsupported extension" );
makeOFConditionConst( DCMTLS_EC_TLSTLSv1CertificateUnobtainable,           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLSV1_CERTIFICATE_UNOBTAINABLE,                   OF_error, "TLS error: certificate unobtainable" );
makeOFConditionConst( DCMTLS_EC_TLSTLSv1UnrecognizedName,                  OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLSV1_UNRECOGNIZED_NAME,                          OF_error, "TLS error: unrecognized name" );
makeOFConditionConst( DCMTLS_EC_TLSTLSv1BadCertificateStatusResponse,      OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLSV1_BAD_CERTIFICATE_STATUS_RESPONSE,            OF_error, "TLS error: bad certificate status response" );
makeOFConditionConst( DCMTLS_EC_TLSTLSv1BadCertificateHashValue,           OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLSV1_BAD_CERTIFICATE_HASH_VALUE,                 OF_error, "TLS error: bad certificate hash value" );
makeOFConditionConst( DCMTLS_EC_TLSTLSv13AlertCertificateRequired,         OFM_dcmtls, DCMTLS_EC_SSL_Offset + SSL_R_TLSV13_ALERT_CERTIFICATE_REQUIRED,                OF_error, "TLS alert: certificate required" );
