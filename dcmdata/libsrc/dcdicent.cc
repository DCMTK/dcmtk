
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
}

DcmDictEntry::DcmDictEntry(DcmTagKey k) : DcmTagKey(k) 
{
    upperKey.set(k);
    tagName = (char*)NULL;
    valueMultiplicityMin = valueMultiplicityMax = 0;
    standardVersion = (char*)NULL;
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
        const char* vers, BOOL doCopyStrings) : DcmTagKey(g,e) 
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
}

DcmDictEntry::DcmDictEntry(Uint16 g, Uint16 e, Uint16 ug, Uint16 ue, DcmVR vr, 
        const char* nam, int vmMin, int vmMax,
        const char* vers, BOOL doCopyStrings) : DcmTagKey(g,e)
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
