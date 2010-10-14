/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *  Module: dcmnet
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DcmTransportLayer
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:28 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmnet/dcmlayer.h"
#include "dcmtk/dcmnet/dcmtrans.h"

DcmTransportLayer::~DcmTransportLayer()
{
}

DcmTransportConnection * DcmTransportLayer::createConnection(int openSocket, OFBool useSecureLayer)
{
  if (useSecureLayer) return NULL;  /* secure layer connections not supported */
  else return new DcmTCPConnection(openSocket);
}


/*
 *  $Log: dcmlayer.cc,v $
 *  Revision 1.4  2010-10-14 13:14:28  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.3  2005/12/08 15:44:34  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.2  2001/06/01 15:50:05  meichel
 *  Updated copyright header
 *
 *  Revision 1.1  2000/08/10 14:50:56  meichel
 *  Added initial OpenSSL support.
 *
 *
 */

