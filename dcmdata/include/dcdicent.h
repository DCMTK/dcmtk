
#ifndef DCDICENT_H
#define DCDICENT_H 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <iostream.h>

#include "dctagkey.h"
#include "dcvr.h"


#define DcmVariableVM	-1

#define DCM_INRANGE(x,a,b) (((x) >= (a)) && ((x) <= (b)))


/*
 * DcmDictEntry is an entry in the DICOM Data Dictionary
 */ 

class DcmDictEntry : public DcmTagKey {
private:
    DcmTagKey	upperKey;	/* upper limit of repeating group/element */
    DcmVR	valueRepresentation;
    const char*	tagName;
    int		valueMultiplicityMin;
    int		valueMultiplicityMax;
    const char*	standardVersion;
    BOOL	stringsAreCopies;

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
    
    int isRepeatingGroup() const;
    int isRepeatingElement() const;
    int isRepeating() const;

    /* comparisons */
    int groupLT(const DcmDictEntry& key) const;
    int groupGT(const DcmDictEntry& key) const; 
    int groupEQ(const DcmDictEntry& key) const;
    int elementLT(const DcmDictEntry& key) const;
    int elementGT(const DcmDictEntry& key) const;
    int elementEQ(const DcmDictEntry& key) const;

    /* operators */
    int operator == (const DcmDictEntry& key) const;
    int operator != (const DcmDictEntry& key) const;
    int operator < (const DcmDictEntry& key) const;
    int operator > (const DcmDictEntry& key) const;
    int operator <= (const DcmDictEntry& key) const;
    int operator >= (const DcmDictEntry& key) const;

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

/* comparisons */

inline int 
DcmDictEntry::groupLT(const DcmDictEntry& key) const
{ 
    return (getUpperGroup() < key.getGroup()); 
}

inline int 
DcmDictEntry::groupGT(const DcmDictEntry& key) const
{ 
    return (getGroup() > key.getUpperGroup()); 
}

inline int 
DcmDictEntry::groupEQ(const DcmDictEntry& key) const
{
    return (
	DCM_INRANGE(key.getGroup(), getGroup(), getUpperGroup()) ||
	DCM_INRANGE(getGroup(), key.getGroup(), key.getUpperGroup()) ); 
}

inline int 
DcmDictEntry::elementLT(const DcmDictEntry& key) const
{ 
    return (getUpperElement() < key.getElement()); 
}

inline int 
DcmDictEntry::elementGT(const DcmDictEntry& key) const
{ 
    return (getElement() > key.getUpperElement()); 
}

inline int 
DcmDictEntry::elementEQ(const DcmDictEntry& key) const
{
    return (
	DCM_INRANGE(key.getElement(), getElement(), getUpperElement()) ||
	DCM_INRANGE(getElement(), key.getElement(), key.getUpperElement()) ); 
}

    /* operators */
inline int 
DcmDictEntry::operator == (const DcmDictEntry& key) const
{ 
    return ( groupEQ(key) && elementEQ(key) ); 
}

inline int 
DcmDictEntry::operator != (const DcmDictEntry& key) const
{ 
    return !(*this == key); 
}

inline int 
DcmDictEntry::operator < (const DcmDictEntry& key) const
{ 
    return (groupLT(key) || (groupEQ(key) && elementLT(key)) ); 
}

inline int 
DcmDictEntry::operator > (const DcmDictEntry& key) const
{ 
    return (groupGT(key) || (groupEQ(key) && elementGT(key)) ); 
}

inline int 
DcmDictEntry::operator <= (const DcmDictEntry& key) const
{  
    return (*this < key) || (*this == key); 
}

inline int 
DcmDictEntry::operator >= (const DcmDictEntry& key) const
{ 
    return (*this > key) || (*this == key); 
}

#endif /* !DCDICENT_H */

