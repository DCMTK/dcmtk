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

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmtls/tlscond.h"

// instantiation of pure generic error codes

makeOFConditionConst( DCMTLS_EC_FailedToCreateTLSTransportLayer,    OFM_dcmtls, 1, OF_error, "Unable to create TLS transport layer" );
makeOFConditionConst( DCMTLS_EC_FailedToSetCiphersuites,            OFM_dcmtls, 2, OF_error, "Unable to set selected cipher suites" );

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

// instantiation of specializable error codes

const DcmTLSECUnknownCiphersuiteConst DCMTLS_EC_UnknownCiphersuite;
const DcmTLSECFailedToLoadPrivateKeyConst DCMTLS_EC_FailedToLoadPrivateKey;
const DcmTLSECFailedToLoadCertificateConst DCMTLS_EC_FailedToLoadCertificate;
const DcmTLSECMismatchedPrivateKeyAndCertificateConst DCMTLS_EC_MismatchedPrivateKeyAndCertificate;
const DcmTLSECFailedToWriteRandomSeedFileConst DCMTLS_EC_FailedToWriteRandomSeedFile;
