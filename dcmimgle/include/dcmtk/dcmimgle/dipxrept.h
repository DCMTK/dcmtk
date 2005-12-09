/*
 *
 *  Copyright (C) 1996-2005, OFFIS
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomPixelRepresentationTemplate (Header)
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2005-12-09 14:48:35 $
 *  CVS/RCS Revision: $Revision: 1.15 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIPXREPT_H
#define DIPXREPT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmimgle/diutils.h"

#ifdef HAVE_EXPLICIT_TEMPLATE_SPECIALIZATION
#define EXPLICIT_SPECIALIZATION template<>
#else
#define EXPLICIT_SPECIALIZATION
#endif


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to determine pixel representation.
 *  Size of basic structure, sign-extension
 */
template<class T>
class DiPixelRepresentationTemplate
{

 public:

    /// destructor
    virtual ~DiPixelRepresentationTemplate() {}

    /** check whether template type T is signed or not
     *
     ** @return true if signed, false otherwise
     */
    inline int isSigned() const;

 protected:

    /** determine integer representation for template type T
     *
     ** @return integer representation
     */
    virtual inline EP_Representation getRepresentation() const;
};


/********************************************************************/


EXPLICIT_SPECIALIZATION
inline EP_Representation DiPixelRepresentationTemplate<Uint8>::getRepresentation() const
{
    return EPR_Uint8;
}


EXPLICIT_SPECIALIZATION
inline EP_Representation DiPixelRepresentationTemplate<Sint8>::getRepresentation() const
{
    return EPR_Sint8;
}


EXPLICIT_SPECIALIZATION
inline EP_Representation DiPixelRepresentationTemplate<Uint16>::getRepresentation() const
{
    return EPR_Uint16;
}


EXPLICIT_SPECIALIZATION
inline EP_Representation DiPixelRepresentationTemplate<Sint16>::getRepresentation() const
{
    return EPR_Sint16;
}


EXPLICIT_SPECIALIZATION
inline EP_Representation DiPixelRepresentationTemplate<Uint32>::getRepresentation() const
{
    return EPR_Uint32;
}


EXPLICIT_SPECIALIZATION
inline EP_Representation DiPixelRepresentationTemplate<Sint32>::getRepresentation() const
{
    return EPR_Sint32;
}


EXPLICIT_SPECIALIZATION
inline int DiPixelRepresentationTemplate<Uint8>::isSigned() const
{
    return 0;
}


EXPLICIT_SPECIALIZATION
inline int DiPixelRepresentationTemplate<Uint16>::isSigned() const
{
    return 0;
}


EXPLICIT_SPECIALIZATION
inline int DiPixelRepresentationTemplate<Uint32>::isSigned() const
{
    return 0;
}


EXPLICIT_SPECIALIZATION
inline int DiPixelRepresentationTemplate<Sint8>::isSigned() const
{
    return 1;
}


EXPLICIT_SPECIALIZATION
inline int DiPixelRepresentationTemplate<Sint16>::isSigned() const
{
    return 1;
}


EXPLICIT_SPECIALIZATION
inline int DiPixelRepresentationTemplate<Sint32>::isSigned() const
{
    return 1;
}


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: dipxrept.h,v $
 * Revision 1.15  2005-12-09 14:48:35  meichel
 * Added missing virtual destructors
 *
 * Revision 1.14  2005/12/08 16:48:06  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.13  2003/12/08 19:26:45  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated copyright header.
 *
 * Revision 1.12  2001/06/01 15:49:50  meichel
 * Updated copyright header
 *
 * Revision 1.11  2000/03/08 16:24:23  meichel
 * Updated copyright header.
 *
 * Revision 1.10  2000/02/02 11:02:39  joergr
 * Removed space characters before preprocessor directives.
 *
 * Revision 1.9  1999/09/17 12:47:52  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.8  1999/04/28 17:03:17  joergr
 * Added test whether the compiler supports the new explicit template
 * specialization syntax (see below).
 *
 * Revision 1.7  1999/04/28 14:54:37  joergr
 * Added "template<>" to specialized template functions/methods to avoid
 * compiler warnings reported by gcc version egcs-2.91.66 (conforms with new
 * C++ standard).
 *
 * Revision 1.6  1999/03/24 17:20:23  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.5  1999/02/11 16:41:39  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.4  1998/12/22 14:37:22  joergr
 * Removed methods getMinimum/Maximum().
 *
 * Revision 1.3  1998/12/16 16:38:23  joergr
 * Added methods to determine absolute minimum and maximum value for given
 * value representation.
 *
 * Revision 1.2  1998/12/14 17:28:53  joergr
 * Removed old source header.
 *
 * Revision 1.1  1998/11/27 15:45:50  joergr
 * Added copyright message.
 *
 * Revision 1.2  1998/05/11 14:53:28  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
