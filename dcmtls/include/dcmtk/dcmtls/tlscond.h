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

// Declaration of generic error code instances

extern DCMTK_DCMTLS_EXPORT const DcmTLSECUnknownCiphersuiteConst DCMTLS_EC_UnknownCiphersuite;
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_FailedToCreateTLSTransportLayer;
extern DCMTK_DCMTLS_EXPORT const DcmTLSECFailedToLoadPrivateKeyConst DCMTLS_EC_FailedToLoadPrivateKey;
extern DCMTK_DCMTLS_EXPORT const DcmTLSECFailedToLoadCertificateConst DCMTLS_EC_FailedToLoadCertificate;
extern DCMTK_DCMTLS_EXPORT const DcmTLSECMismatchedPrivateKeyAndCertificateConst DCMTLS_EC_MismatchedPrivateKeyAndCertificate;
extern DCMTK_DCMTLS_EXPORT const OFConditionConst DCMTLS_EC_FailedToSetCiphersuites;
extern DCMTK_DCMTLS_EXPORT const DcmTLSECFailedToWriteRandomSeedFileConst DCMTLS_EC_FailedToWriteRandomSeedFile;

#endif // TLSCOND_H
