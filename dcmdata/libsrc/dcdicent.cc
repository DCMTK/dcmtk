/*
**
** Author: Andrew Hewett	Created: 4.11.95
** Kuratorium OFFIS e.V.
**
** Module: dcdicent.cc
**
** Purpose:
** A dictionary entry in the loadable DICOM data dictionary
** 
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1997-08-26 13:48:27 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/libsrc/dcdicent.cc,v $
** CVS/RCS Revision:	$Revision: 1.5 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include <string.h>

#include "dcdicent.h"



/*
** DcmDictEntry member functions
*/

DcmDictEntry::DcmDictEntry() 
{ 
    tagName = (char*)NULL;
    valueMultiplicityMin = valueMultiplicityMax = 0;
    standardVersion = (char*)NULL;
    groupRangeRestriction = elementRangeRestriction = DcmDictRange_Unspecified;
    stringsAreCopies = OFFalse;
}

DcmDictEntry::DcmDictEntry(const DcmTagKey& k) : DcmTagKey(k)
{
    upperKey.set(k);
    tagName = (char*)NULL;
    valueMultiplicityMin = valueMultiplicityMax = 0;
    standardVersion = (char*)NULL;
    groupRangeRestriction = elementRangeRestriction = DcmDictRange_Unspecified;
    stringsAreCopies = OFFalse;
}

static
char* strdup_new(const char* str)
{
    char* s = NULL;
    if (str != NULL) {
	s = new char[strlen(str)+1];
	strcpy(s, str);
    }
    return s;
}

DcmDictEntry::DcmDictEntry(Uint16 g, Uint16 e, DcmVR vr, 
        const char* nam, int vmMin, int vmMax,
        const char* vers, OFBool doCopyStrings) : DcmTagKey(g,e) 
{
    upperKey.set(g,e);	/* default: make upper key same as normal key */
    valueRepresentation.setVR(vr);
    valueMultiplicityMin = vmMin;
    valueMultiplicityMax = vmMax;
    if (doCopyStrings) {
	tagName = strdup_new(nam);
	standardVersion = strdup_new(vers);
    } else {
	tagName = nam;
	standardVersion = vers;
    }
    stringsAreCopies = doCopyStrings;
    groupRangeRestriction = elementRangeRestriction = DcmDictRange_Unspecified;
}

DcmDictEntry::DcmDictEntry(Uint16 g, Uint16 e, Uint16 ug, Uint16 ue, DcmVR vr, 
        const char* nam, int vmMin, int vmMax,
        const char* vers, OFBool doCopyStrings) : DcmTagKey(g,e)
{
    upperKey.set(ug, ue);
    valueRepresentation.setVR(vr);
    valueMultiplicityMin = vmMin;
    valueMultiplicityMax = vmMax;
    if (doCopyStrings) {
	tagName = strdup_new(nam);
	standardVersion = strdup_new(vers);
    } else {
	tagName = nam;
	standardVersion = vers;
    }
    stringsAreCopies = doCopyStrings;
    groupRangeRestriction = elementRangeRestriction = DcmDictRange_Unspecified;
}

DcmDictEntry::DcmDictEntry(const DcmDictEntry& e)
    : DcmTagKey(e)
    , upperKey(e.upperKey)
    , valueRepresentation(e.valueRepresentation)
    , groupRangeRestriction(e.groupRangeRestriction)
    , elementRangeRestriction(e.elementRangeRestriction)
{
    valueMultiplicityMin = e.valueMultiplicityMin;
    valueMultiplicityMax = e.valueMultiplicityMax;
    stringsAreCopies = e.stringsAreCopies;
    if (stringsAreCopies)
    {
        tagName = strdup_new(e.tagName);
	standardVersion = strdup_new(e.standardVersion);
    } else {
        tagName = e.tagName;
        standardVersion = e.standardVersion;
    }
}


DcmDictEntry::~DcmDictEntry()
{
    if (stringsAreCopies) {
	/* we have allocated them so it is ok to deallocate them */
	if (tagName) delete (char*)tagName;
	if (standardVersion) delete (char*)standardVersion;
    }
}

/*
** DcmDictEntry friend functions
*/

ostream& operator<<(ostream& s, const DcmDictEntry& e) {

    const DcmDictEntry* ep = &e;
    if (ep == NULL) {
	s << "(nil)";
	return s;
    }

    char tagBuf[16];

    sprintf(tagBuf, "(%04x", e.getGroup());
    s << tagBuf;
    if (e.isRepeatingGroup()) {
	sprintf(tagBuf, "-%04x", e.getUpperGroup());
	s << tagBuf;
    }
    sprintf(tagBuf, ",%04x", e.getElement());
    s << tagBuf;
    if (e.isRepeatingElement()) {
	sprintf(tagBuf, "-%04x", e.getUpperElement());
	s << tagBuf;
    }
    s << ")";


    s << " " << e.getVR().getVRName() << " \"" << e.getTagName() << "\" ";
    if (e.isFixedSingleVM()) {
	s << "vm=" << e.getVMMax() << " ";
    } else if (e.isVariableRangeVM()) {
	s << "vm=" << e.getVMMin() << "-n ";
    } else if (e.isFixedRangeVM()){
	s << "vm=" << e.getVMMin() << "-" << e.getVMMax() << " ";
    } else {
	s << "vm=?(" <<  e.getVMMin() << "-" << e.getVMMax() << ")? ";
    }
    if (e.getStandardVersion() != NULL) {
	s << " Version=\"" << e.getStandardVersion() << "\" ";
    }
    return s;
}

/*
** CVS/RCS Log:
** $Log: dcdicent.cc,v $
** Revision 1.5  1997-08-26 13:48:27  hewett
** Modified constructors to take const reference parameters.
**
** Revision 1.4  1997/07/31 14:40:36  meichel
** Created copy constructor for class DcmDictEntry, required by dcmcheck.
**
** Revision 1.3  1997/07/21 08:25:24  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.2  1996/03/20 16:44:04  hewett
** Updated for revised data dictionary.  Repeating tags are now handled better.
** A linear list of repeating tags has been introduced with a subset ordering
** mechanism to ensure that dictionary searches locate the most precise
** dictionary entry.
**
**
*/
