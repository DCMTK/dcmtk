/*
 *
 *  Copyright (C) 2010, OFFIS
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Defines a template vector class based on the STL vector class
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-04-26 11:57:35 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef OFVECTOR_H
#define OFVECTOR_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#ifndef HAVE_CLASS_TEMPLATE
#error Your C++ compiler cannot handle class templates:
#endif

#if defined(HAVE_STL) || defined(HAVE_STL_VECTOR)

// Use the standard template library (STL) vector class.
#include <vector>

#ifdef HAVE_STD_NAMESPACE
#define OFVector std::vector
#else
#define OFVector vector
#endif

#else

// Currently, there is no alternative implementation to the STL one,
// so the vector class cannot be used but will result in compiler errors.

#endif

#endif


/*
 * CVS/RCS Log:
 * $Log: ofvector.h,v $
 * Revision 1.1  2010-04-26 11:57:35  joergr
 * Added initial definitions for using the STL vector class. Please note that
 * there is currently no alternative implementation to this standard class.
 *
 *
 *
 */
