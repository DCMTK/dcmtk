/*
 *
 *  Copyright (C) 1994-2003, OFFIS
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
 *  Purpose: Basis class for dicom tags.
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2003-11-13 14:06:36 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/include/Attic/dctagkey.h,v $
 *  CVS/RCS Revision: $Revision: 1.14 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCMTAGKEY_H
#define DCMTAGKEY_H 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofstream.h"
#include "dctypes.h"
#include "ofstring.h"

/*
** Defines
*/

#define DCM_UndefinedTagKey     DcmTagKey(0xffff, 0xffff)


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

    friend ostream& operator<<(ostream& s, const DcmTagKey& k);

    OFString toString() const;
    
    /** returns true if a data element with the given tag key can
     *  be digitally signed, false otherwise
     *  @return true if signable, false otherwise
     */
    OFBool isSignableTag() const;
};

/** stream output operator for tag keys
 *  @param s output stream
 *  @param k tag key
 *  @return reference to output stream
 */
ostream& operator<<(ostream& s, const DcmTagKey& k);

/* 
** inline versions of functions 
*/

/* Constructors */

inline  
DcmTagKey::DcmTagKey()
  : group(0xffff),
    element(0xffff)
{ 
}

inline 
DcmTagKey::DcmTagKey(const DcmTagKey& key) 
  : group(key.group),
    element(key.element)
{ 
}

inline 
DcmTagKey::DcmTagKey(Uint16 g, Uint16 e) 
  : group(g),
    element(e)
{ 
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
** Revision 1.14  2003-11-13 14:06:36  meichel
** Fixed definition of DCM_UndefinedTagKey
**
** Revision 1.13  2003/11/05 15:56:31  meichel
** Added declaration of operator<< for DcmTagKeys.
**   Fixes compilation issue on Visual C++ 6.0 SP 0.
**
** Revision 1.12  2002/04/16 13:41:44  joergr
** Added configurable support for C++ ANSI standard includes (e.g. streams).
** Thanks to Andreas Barth <Andreas.Barth@bruker-biospin.de> for his
** contribution.
**
** Revision 1.11  2001/11/19 15:23:11  meichel
** Cleaned up signature code to avoid some gcc warnings.
**
** Revision 1.10  2001/11/16 15:54:40  meichel
** Adapted digital signature code to final text of supplement 41.
**
** Revision 1.9  2001/06/01 15:48:45  meichel
** Updated copyright header
**
** Revision 1.8  2000/11/07 16:56:10  meichel
** Initial release of dcmsign module for DICOM Digital Signatures
**
** Revision 1.7  2000/03/08 16:26:19  meichel
** Updated copyright header.
**
** Revision 1.6  2000/02/07 14:45:16  meichel
** Removed const qualifier from DcmTagKey::toString(), avoids warning on Irix.
**
** Revision 1.5  1999/03/31 09:24:49  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.4  1999/03/17 11:08:54  meichel
** added method DcmTagKey::toString()
**
** Revision 1.3  1998/07/15 15:48:54  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.2  1997/08/26 13:45:54  hewett
** Added simple hash function method.
**
** Revision 1.1  1995/11/23 16:38:04  hewett
** Updated for loadable data dictionary + some cleanup (more to do).
**
*/
