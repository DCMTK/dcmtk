/*
**
** Author: Andrew Hewett	Created: 4.11.95
** Kuratorium OFFIS e.V.
**
** Module: dcdicent.h
**
** Purpose:
** Interface for a dictionary entry in the loadable DICOM data dictionary
** 
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1996-03-20 16:43:49 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcdicent.h,v $
** CVS/RCS Revision:	$Revision: 1.3 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#ifndef DCDICENT_H
#define DCDICENT_H 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <iostream.h>

#include "dctagkey.h"
#include "dcvr.h"


#define DcmVariableVM	-1

#define DCM_INRANGE(x,a,b) (((x) >= (a)) && ((x) <= (b)))
#define DCM_IS_ODD(x) (((x) % 2) == 1)
#define DCM_IS_EVEN(x) (((x) % 2) == 0)

/*
 * DcmDictEntry is an entry in the DICOM Data Dictionary
 */ 

enum DcmDictRangeRestriction {
    DcmDictRange_Unspecified,
    DcmDictRange_Odd,
    DcmDictRange_Even
};

class DcmDictEntry : public DcmTagKey {
private:
    DcmTagKey	upperKey;	/* upper limit of repeating group/element */
    DcmVR	valueRepresentation;
    const char*	tagName;
    int		valueMultiplicityMin;
    int		valueMultiplicityMax;
    const char*	standardVersion;
    BOOL	stringsAreCopies;
    DcmDictRangeRestriction groupRangeRestriction;
    DcmDictRangeRestriction elementRangeRestriction;

public:
    /* constructors */
    DcmDictEntry();
    DcmDictEntry(DcmTagKey k);
    DcmDictEntry(Uint16 g, Uint16 e, DcmVR vr, 
        const char* nam=NULL, int vmMin=1, int vmMax=1,
        const char* vers="DICOM3", BOOL doCopyStrings=TRUE);	
    DcmDictEntry(Uint16 g, Uint16 e, Uint16 ug, Uint16 ue, DcmVR vr,
        const char* nam=NULL, int vmMin=1, int vmMax=1,
        const char* vers="DICOM3", BOOL doCopyStrings=TRUE);	
    
    /* destructor */
    ~DcmDictEntry();

    /* access methods */

    DcmVR getVR() const;
    DcmEVR getEVR() const;
    const char* getStandardVersion() const;
    const char* getTagName() const;
    
    int getVMMin() const;
    int getVMMax() const;
    BOOL isFixedSingleVM() const;
    BOOL isFixedRangeVM() const;
    BOOL isVariableRangeVM() const;

    void setUpper(const DcmTagKey& key);
    void setUpperGroup(Uint16 ug);
    void setUpperElement(Uint16 ue);
    Uint16 getUpperGroup() const;
    Uint16 getUpperElement() const;

    DcmTagKey getKey();
    DcmTagKey getUpperKey();
    
    int isRepeatingGroup() const;
    int isRepeatingElement() const;
    int isRepeating() const;

    DcmDictRangeRestriction getGroupRangeRestriction() const;
    void setGroupRangeRestriction(DcmDictRangeRestriction rr);
    DcmDictRangeRestriction getElementRangeRestriction() const;
    void setElementRangeRestriction(DcmDictRangeRestriction rr);

    /* containment */
    int contains(const DcmTagKey& key) const; /* this contains key */

    /* set relations */
    int subset(const DcmDictEntry& e) const; /* this is a subset of key */
    int setEQ(const DcmDictEntry& e) const; /* this is set equal to key */

    friend  ostream& operator<<(ostream& s, const DcmDictEntry& e);
};


/*
** Inline member functions
*/

inline DcmVR 
DcmDictEntry::getVR() const 
{ 
    return valueRepresentation; 
}

inline DcmEVR 
DcmDictEntry::getEVR() const 
{ 
    return valueRepresentation.getEVR(); 
}

inline const char* 
DcmDictEntry::getStandardVersion() const 
{ 
    return standardVersion; 
}

inline const char* 
DcmDictEntry::getTagName() const 
{ 
    return tagName; 
}
    
inline int 
DcmDictEntry::getVMMin() const 
{ 
    return valueMultiplicityMin; 
}

inline int 
DcmDictEntry::getVMMax() const 
{ 
    return valueMultiplicityMax; 
}

inline BOOL 
DcmDictEntry::isFixedSingleVM() const 
{ 
    return (BOOL)((valueMultiplicityMin != DcmVariableVM) &&
		  (valueMultiplicityMin == valueMultiplicityMax));
}

inline BOOL 
DcmDictEntry::isFixedRangeVM() const 
{ 
    return (BOOL)((valueMultiplicityMin != DcmVariableVM) &&
		  (valueMultiplicityMax != DcmVariableVM));
}

inline BOOL 
DcmDictEntry::isVariableRangeVM() const 
{
    return (BOOL)((valueMultiplicityMin != DcmVariableVM) &&
		  (valueMultiplicityMax == DcmVariableVM));
}

inline void 
DcmDictEntry::setUpper(const DcmTagKey& key) 
{ 
    upperKey = key; 
}    

inline void 
DcmDictEntry::setUpperGroup(Uint16 ug) 
{ 
    upperKey.setGroup(ug); 
}

inline void 
DcmDictEntry::setUpperElement(Uint16 ue) 
{ 
    upperKey.setElement(ue); 
}

inline Uint16 
DcmDictEntry::getUpperGroup() const 
{ 
    return upperKey.getGroup(); 
}

inline Uint16 
DcmDictEntry::getUpperElement() const 
{ 
    return upperKey.getElement(); 
}

inline DcmTagKey 
DcmDictEntry::getKey()
{
    return *(DcmTagKey*)(this);
}

inline DcmTagKey 
DcmDictEntry::getUpperKey()
{
    return upperKey;
}

inline int 
DcmDictEntry::isRepeatingGroup() const 
{ 
    return (getGroup() != getUpperGroup()); 
}

inline int 
DcmDictEntry::isRepeatingElement() const 
{ 
    return (getElement() != getUpperElement()); 
}

inline int 
DcmDictEntry::isRepeating() const 
{ 
    return (isRepeatingGroup() || isRepeatingElement()); 
}

inline DcmDictRangeRestriction 
DcmDictEntry::getGroupRangeRestriction() const
{
    return groupRangeRestriction;
}

inline void 
DcmDictEntry::setGroupRangeRestriction(DcmDictRangeRestriction rr)
{
    groupRangeRestriction = rr;
}

inline DcmDictRangeRestriction 
DcmDictEntry::getElementRangeRestriction() const
{
    return elementRangeRestriction;
}

inline void 
DcmDictEntry::setElementRangeRestriction(DcmDictRangeRestriction rr)
{
    elementRangeRestriction = rr;
}

/* containment */
inline int 
DcmDictEntry::contains(const DcmTagKey& key) const
{
    if ((getGroupRangeRestriction() == DcmDictRange_Even) && 
	DCM_IS_ODD(getGroup()))
	return FALSE;
    if ((getGroupRangeRestriction() == DcmDictRange_Odd) &&
	DCM_IS_EVEN(getGroup())) 
	return FALSE;
    if ((getElementRangeRestriction() == DcmDictRange_Even) &&
	DCM_IS_ODD(getElement()))
	return FALSE;
    if ((getElementRangeRestriction() == DcmDictRange_Odd) &&
	DCM_IS_EVEN(getElement()))
	return FALSE;

    return 
	DCM_INRANGE(key.getGroup(), getGroup(), getUpperGroup()) &&
	DCM_INRANGE(key.getElement(), getElement(), getUpperElement());
}

/* set relations */
inline int 
DcmDictEntry::subset(const DcmDictEntry& e) const
{
    return ( (getGroup() >= e.getGroup()) &&
	     (getUpperGroup() <= e.getUpperGroup()) &&
	     (getElement() >= e.getElement()) &&
	     (getUpperElement() <= e.getUpperElement())
	); 
}

inline int
DcmDictEntry::setEQ(const DcmDictEntry& e) const
{
    return ( (getGroup() == e.getGroup()) &&
	     (getUpperGroup() == e.getUpperGroup()) &&
	     (getElement() == e.getElement()) &&
	     (getUpperElement() == e.getUpperElement()) &&
	     (getGroupRangeRestriction() == e.getGroupRangeRestriction()) &&
	     (getElementRangeRestriction() == e.getElementRangeRestriction())
	); 
}


#endif /* !DCDICENT_H */

/*
** CVS/RCS Log:
** $Log: dcdicent.h,v $
** Revision 1.3  1996-03-20 16:43:49  hewett
** Updated for revised data dictionary.  Repeating tags are now handled better.
** A linear list of repeating tags has been introduced with a subset ordering
** mechanism to ensure that dictionary searches locate the most precise
** dictionary entry.
**
**
*/
