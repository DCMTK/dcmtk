/*
 *
 *  Copyright (C) 1996-2024, OFFIS e.V.
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomPixelRepresentationTemplate (Header)
 *
 */


#ifndef DIPXREPT_H
#define DIPXREPT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmimgle/diutils.h"

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

    /// default constructor
    DiPixelRepresentationTemplate() {}

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


template<>
inline EP_Representation DiPixelRepresentationTemplate<Uint8>::getRepresentation() const
{
    return EPR_Uint8;
}


template<>
inline EP_Representation DiPixelRepresentationTemplate<Sint8>::getRepresentation() const
{
    return EPR_Sint8;
}


template<>
inline EP_Representation DiPixelRepresentationTemplate<Uint16>::getRepresentation() const
{
    return EPR_Uint16;
}


template<>
inline EP_Representation DiPixelRepresentationTemplate<Sint16>::getRepresentation() const
{
    return EPR_Sint16;
}


template<>
inline EP_Representation DiPixelRepresentationTemplate<Uint32>::getRepresentation() const
{
    return EPR_Uint32;
}


template<>
inline EP_Representation DiPixelRepresentationTemplate<Sint32>::getRepresentation() const
{
    return EPR_Sint32;
}


template<>
inline int DiPixelRepresentationTemplate<Uint8>::isSigned() const
{
    return 0;
}


template<>
inline int DiPixelRepresentationTemplate<Uint16>::isSigned() const
{
    return 0;
}


template<>
inline int DiPixelRepresentationTemplate<Uint32>::isSigned() const
{
    return 0;
}


template<>
inline int DiPixelRepresentationTemplate<Sint8>::isSigned() const
{
    return 1;
}


template<>
inline int DiPixelRepresentationTemplate<Sint16>::isSigned() const
{
    return 1;
}


template<>
inline int DiPixelRepresentationTemplate<Sint32>::isSigned() const
{
    return 1;
}


#endif
