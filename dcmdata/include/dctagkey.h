/*
**
** Author: Andrew Hewett	Created: 4.11.95
** Kuratorium OFFIS e.V.
**
** Module: dctagkey.h
**
** Purpose:
** Basis class for dicom tags.  
** 
**
** Last Update:		$Author: hewett $
** Update Date:		$Date: 1997-08-26 13:45:54 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dctagkey.h,v $
** CVS/RCS Revision:	$Revision: 1.2 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/


#ifndef DCMTAGKEY_H
#define DCMTAGKEY_H 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <iostream.h>

#include "dctypes.h"

/*
** Defines
*/

#define DCM_UndefinedTagKey	DcmTagKey(0xffff, 0xffff);


/*
 * Unique key generation for DICOM tags
 */

class DcmTagKey {
private:
    Uint16 group;
    Uint16 element;

protected:
    int groupLT(const DcmTagKey& key) const;
    int groupGT(const DcmTagKey& key) const;
    int groupEQ(const DcmTagKey& key) const;
    int elementLT(const DcmTagKey& key) const;
    int elementGT(const DcmTagKey& key) const;
    int elementEQ(const DcmTagKey& key) const;

public:
    DcmTagKey();
    DcmTagKey(const DcmTagKey& key);
    DcmTagKey(Uint16 g, Uint16 e);
    
    void set(const DcmTagKey& key);
    void set(Uint16 g, Uint16 e);
    void setGroup(Uint16 g);
    void setElement(Uint16 e);
    Uint16 getGroup() const;
    Uint16 getElement() const;

    Uint32 hash() const; // generate simple hash code

    DcmTagKey& operator = (const DcmTagKey& key);
    int operator == (const DcmTagKey& key) const;
    int operator != (const DcmTagKey& key) const;
    int operator < (const DcmTagKey& key) const;
    int operator > (const DcmTagKey& key) const;
    int operator <= (const DcmTagKey& key) const;
    int operator >= (const DcmTagKey& key) const;

    friend ostream&   operator<<(ostream& s, const DcmTagKey& k);
};


/* 
** inline versions of functions 
*/

/* Constructors */

inline  
DcmTagKey::DcmTagKey() 
{ 
    group = 0xffff;
    element = 0xffff;
}

inline 
DcmTagKey::DcmTagKey(const DcmTagKey& key) 
{ 
    group = key.group;
    element = key.element;
}

inline 
DcmTagKey::DcmTagKey(Uint16 g, Uint16 e) 
{ 
    group = g;
    element = e;
}

/* access methods */

inline void 
DcmTagKey::set(const DcmTagKey& key) 	
{ 
    group = key.group; 
    element = key.element; 
}

inline void 
DcmTagKey::set(Uint16 g, Uint16 e)	
{ 
    group = g; 
    element = e; 
}

inline void 
DcmTagKey::setGroup(Uint16 g) 		
{ 
    group = g; 
}

inline void 
DcmTagKey::setElement(Uint16 e) 		
{ 
    element = e; 
}

inline Uint16 
DcmTagKey::getGroup() const 		
{ 
    return group; 
}

inline Uint16 
DcmTagKey::getElement() const 		
{ 
    return element; 
}

inline DcmTagKey& 
DcmTagKey::operator=(const DcmTagKey& key) 
{ 
    set(key); 
    return *this;
}

/* Simple Hash Function */

inline Uint32 
DcmTagKey::hash() const
{
    // generate simple hash code
    return (((getGroup() << 16) & 0xffff0000) | (getElement() & 0xffff));
}

/* Comparisons */

inline int 
DcmTagKey::groupLT(const DcmTagKey& key) const
{
    return (getGroup() < key.getGroup());
}

inline int 
DcmTagKey::groupGT(const DcmTagKey& key) const
{
    return (getGroup() > key.getGroup());
}

inline int 
DcmTagKey::groupEQ(const DcmTagKey& key) const
{
    return getGroup() == key.getGroup();
}

inline int 
DcmTagKey::elementLT(const DcmTagKey& key) const
{
    return (getElement() < key.getElement());
}

inline int 
DcmTagKey::elementGT(const DcmTagKey& key) const
{
    return (getElement() > key.getElement());
}

inline int 
DcmTagKey::elementEQ(const DcmTagKey& key) const
{
    return getElement() == key.getElement();
}

inline int 
DcmTagKey::operator == (const DcmTagKey& key) const
{ 
    return ( groupEQ(key) && elementEQ(key) );
}

inline int 
DcmTagKey::operator != (const DcmTagKey& key) const
{ 
    return !(*this == key); 
}

inline int 
DcmTagKey::operator < (const DcmTagKey& key) const
{ 
    return (groupLT(key) || (groupEQ(key) && elementLT(key)));
}

inline int 
DcmTagKey::operator > (const DcmTagKey& key) const
{ 
    return (groupGT(key) || (groupEQ(key) && elementGT(key)));
}

inline int 
DcmTagKey::operator <= (const DcmTagKey& key) const
{ 
    return (*this < key) || (*this == key);
}

inline int 
DcmTagKey::operator >= (const DcmTagKey& key) const
{ 
    return (*this > key) || (*this == key);
}

#endif


/*
** CVS/RCS Log:
** $Log: dctagkey.h,v $
** Revision 1.2  1997-08-26 13:45:54  hewett
** Added simple hash function method.
**
** Revision 1.1  1995/11/23 16:38:04  hewett
** Updated for loadable data dictionary + some cleanup (more to do).
**
**
*/
