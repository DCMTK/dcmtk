/*
 *
 *  Copyright (C) 1994-2002, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmdata
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: codec parameter for RLE
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-06-06 14:52:41 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcrlecp.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "dcrlecp.h"

DcmRLECodecParameter::DcmRLECodecParameter(
    OFBool pVerbose,
    OFBool pCreateSOPInstanceUID,
    Uint32 pFragmentSize,
    OFBool pCreateOffsetTable,
    OFBool pConvertToSC)
: DcmCodecParameter()
, fragmentSize(pFragmentSize)
, createOffsetTable(pCreateOffsetTable)
, convertToSC(pConvertToSC)
, createInstanceUID(pCreateSOPInstanceUID)
, verboseMode(pVerbose)
{
}


DcmRLECodecParameter::DcmRLECodecParameter(const DcmRLECodecParameter& arg)
: DcmCodecParameter(arg)
, fragmentSize(arg.fragmentSize)
, createOffsetTable(arg.createOffsetTable)
, convertToSC(arg.convertToSC)
, createInstanceUID(arg.createInstanceUID)
, verboseMode(arg.verboseMode)
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


/*
 * CVS/RCS Log
 * $Log: dcrlecp.cc,v $
 * Revision 1.1  2002-06-06 14:52:41  meichel
 * Initial release of the new RLE codec classes
 *   and the dcmcrle/dcmdrle tools in module dcmdata
 *
 *
 */
