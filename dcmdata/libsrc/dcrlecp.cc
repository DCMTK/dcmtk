/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: codec parameter for RLE
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dcrlecp.h"

DcmRLECodecParameter::DcmRLECodecParameter(
    OFBool pCreateSOPInstanceUID,
    Uint32 pFragmentSize,
    OFBool pCreateOffsetTable,
    OFBool pConvertToSC,
    OFBool pReverseDecompressionByteOrder)
: DcmCodecParameter()
, fragmentSize(pFragmentSize)
, createOffsetTable(pCreateOffsetTable)
, convertToSC(pConvertToSC)
, createInstanceUID(pCreateSOPInstanceUID)
, reverseDecompressionByteOrder(pReverseDecompressionByteOrder)
{
}


DcmRLECodecParameter::DcmRLECodecParameter(const DcmRLECodecParameter& arg)
: DcmCodecParameter(arg)
, fragmentSize(arg.fragmentSize)
, createOffsetTable(arg.createOffsetTable)
, convertToSC(arg.convertToSC)
, createInstanceUID(arg.createInstanceUID)
, reverseDecompressionByteOrder(arg.reverseDecompressionByteOrder)
{
}

DcmRLECodecParameter::~DcmRLECodecParameter()
{
}
  
DcmCodecParameter *DcmRLECodecParameter::clone() const
{
  return new DcmRLECodecParameter(*this);
} 

const char *DcmRLECodecParameter::className() const
{
  return "DcmRLECodecParameter";
}
