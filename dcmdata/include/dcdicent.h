/*
 *
 *  Copyright (C) 1994-2000, OFFIS
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
 *  Author:  Andrew Hewett
 *
 *  Purpose: Interface for a dictionary entry in the loadable DICOM data dictionary
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:26:13 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dcdicent.h,v $
 *  CVS/RCS Revision: $Revision: 1.13 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCDICENT_H
#define DCDICENT_H 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <iostream.h>
#include <string.h>

#include "dctagkey.h"
#include "dcvr.h"


#define DcmVariableVM   -1

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
    DcmTagKey   upperKey;       /* upper limit of repeating group/element */
    DcmVR       valueRepresentation;
    const char* tagName;
    int         valueMultiplicityMin;
    int         valueMultiplicityMax;
    const char* standardVersion;
    OFBool      stringsAreCopies;
    DcmDictRangeRestriction groupRangeRestriction;
    DcmDictRangeRestriction elementRangeRestriction;

 // --- declarations to avoid compiler warnings
 
    DcmDictEntry &operator=(const DcmDictEntry &);

public:
    /* constructors */
    DcmDictEntry();
    DcmDictEntry(const DcmTagKey& k);
    DcmDictEntry(Uint16 g, Uint16 e, DcmVR vr, 
        const char* nam=NULL, int vmMin=1, int vmMax=1,
        const char* vers="DICOM3", OFBool doCopyStrings=OFTrue);        
    DcmDictEntry(Uint16 g, Uint16 e, Uint16 ug, Uint16 ue, DcmVR vr,
        const char* nam=NULL, int vmMin=1, int vmMax=1,
        const char* vers="DICOM3", OFBool doCopyStrings=OFTrue);        
    DcmDictEntry(const DcmDictEntry& e);
    
    /* destructor */
    ~DcmDictEntry();

    /* access methods */

    DcmVR getVR() const;
    DcmEVR getEVR() const;
    const char* getStandardVersion() const;
    const char* getTagName() const;
    
    int getVMMin() const;
    int getVMMax() const;
    OFBool isFixedSingleVM() const;
    OFBool isFixedRangeVM() const;
    OFBool isVariableRangeVM() const;

    void setUpper(const DcmTagKey& key);
    void setUpperGroup(Uint16 ug);
    void setUpperElement(Uint16 ue);
    Uint16 getUpperGroup() const;
    Uint16 getUpperElement() const;

    DcmTagKey getKey() const;
    DcmTagKey getUpperKey() const;
    
    int isRepeatingGroup() const;
    int isRepeatingElement() const;
    int isRepeating() const;

    DcmDictRangeRestriction getGroupRangeRestriction() const;
    void setGroupRangeRestriction(DcmDictRangeRestriction rr);
    DcmDictRangeRestriction getElementRangeRestriction() const;
    void setElementRangeRestriction(DcmDictRangeRestriction rr);

    /* containment */
    int contains(const DcmTagKey& key) const; /* this contains key */
    int contains(const char *name) const; /* this contains named key */

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

inline OFBool 
DcmDictEntry::isFixedSingleVM() const 
{ 
    return (OFBool)((valueMultiplicityMin != DcmVariableVM) &&
                  (valueMultiplicityMin == valueMultiplicityMax));
}

inline OFBool 
DcmDictEntry::isFixedRangeVM() const 
{ 
    return (OFBool)((valueMultiplicityMin != DcmVariableVM) &&
                  (valueMultiplicityMax != DcmVariableVM));
}

inline OFBool 
DcmDictEntry::isVariableRangeVM() const 
{
    return (OFBool)((valueMultiplicityMin != DcmVariableVM) &&
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
DcmDictEntry::getKey() const
{
    return *(DcmTagKey*)(this);
}

inline DcmTagKey 
DcmDictEntry::getUpperKey() const
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
        DCM_IS_ODD(key.getGroup()))
        return OFFalse;
    else if ((getGroupRangeRestriction() == DcmDictRange_Odd) &&
        DCM_IS_EVEN(key.getGroup())) 
        return OFFalse;
    else if ((getElementRangeRestriction() == DcmDictRange_Even) &&
        DCM_IS_ODD(key.getElement()))
        return OFFalse;
    else if ((getElementRangeRestriction() == DcmDictRange_Odd) &&
        DCM_IS_EVEN(key.getElement()))
        return OFFalse;
    else
    {
        return 
            DCM_INRANGE(key.getGroup(), getGroup(), getUpperGroup()) &&
            DCM_INRANGE(key.getElement(), getElement(), getUpperElement());
    }
}

inline int 
DcmDictEntry::contains(const char *name) const
{
    return !strcmp( tagName, name );
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
** Revision 1.13  2000-03-08 16:26:13  meichel
** Updated copyright header.
**
** Revision 1.12  1999/03/31 09:24:35  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.11  1998/07/15 15:48:45  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.10  1997/08/26 13:44:59  hewett
** Modified constructors to take const parameters.
**
** Revision 1.9  1997/07/31 14:40:35  meichel
** Created copy constructor for class DcmDictEntry, required by dcmcheck.
**
** Revision 1.8  1997/07/21 08:25:07  andreas
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.7  1997/04/18 08:04:39  andreas
** - Minor corrections: correct some warnings of the SUN-C++ Compiler
**   concerning the assignments of wrong types and inline compiler
**   errors
**
** Revision 1.6  1997/04/15 16:25:05  hewett
** Corrected data dictionary bug whereby the even/odd range restrictions
** were not being taken into consideration when searching the dictionary.
**
** Revision 1.5  1996/09/24 16:24:58  hewett
** Added preliminary support for the Macintosh environment (GUSI library).
**
** Revision 1.4  1996/09/18 16:37:09  hewett
** Added capability to search data dictionary by tag name.  The
** source code for these changes was contributed by Larry V. Streepy,
** Jr., Chief Technical Officer,  Healthcare Communications, Inc.,
** (mailto:streepy@healthcare.com).
**
** Revision 1.3  1996/03/20 16:43:49  hewett
** Updated for revised data dictionary.  Repeating tags are now handled better.
** A linear list of repeating tags has been introduced with a subset ordering
** mechanism to ensure that dictionary searches locate the most precise
** dictionary entry.
**
*/
