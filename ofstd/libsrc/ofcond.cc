/*
 *
 *  Copyright (C) 1997-2001, OFFIS
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
 *  Module:  ofstd
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: class OFCondition and helper classes
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2001-09-25 17:07:27 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/libsrc/ofcond.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "ofcond.h"


/* ---------- global condition constants ---------- */

const OFConditionConst ECC_Normal(          0, 0, OF_ok,      "Normal");
const OFConditionConst ECC_IllegalParameter(0, 1, OF_error,   "Illegal parameter");
const OFConditionConst ECC_MemoryExhausted( 0, 2, OF_failure, "Virtual Memory exhausted");

const OFCondition EC_Normal(ECC_Normal);
const OFCondition EC_IllegalParameter(ECC_IllegalParameter);
const OFCondition EC_MemoryExhausted(ECC_MemoryExhausted);

/* ---------- class OFConditionConst ---------- */


const OFConditionBase *OFConditionConst::clone() const
{
  return this; // since we're not deletable, we just return a copy to ourselves
}

unsigned long OFConditionConst::codeAndModule() const
{
  return theCodeAndModule;
}

OFStatus OFConditionConst::status() const
{
  return theStatus;
}

const char *OFConditionConst::text() const
{
  return theText;
}

OFBool OFConditionConst::deletable() const
{
  return OFFalse;
}


/* ---------- class OFConditionString ---------- */


const OFConditionBase *OFConditionString::clone() const
{
  return new OFConditionString(*this);
}

unsigned long OFConditionString::codeAndModule() const
{
  return theCodeAndModule;
}

OFStatus OFConditionString::status() const
{
  return theStatus;
}

const char *OFConditionString::text() const
{
  return theText.c_str();
}

OFBool OFConditionString::deletable() const
{
  return OFTrue;
}


/*
 *
 * CVS/RCS Log:
 * $Log: ofcond.cc,v $
 * Revision 1.2  2001-09-25 17:07:27  meichel
 * Disabled implicit conversion to bool, added default constructor
 *   to class OFCondition.
 *
 * Revision 1.1  2001/08/23 16:08:41  meichel
 * Initial release of class OFCondition, a generic approach for condition codes
 *
 *
 *
 */
