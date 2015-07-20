/*
 *
 *  Copyright (C) 2015, OFFIS e.V.
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose:
 *    classes: ContentMappingResource
 *
 */


#ifndef CMR_INIT_H
#define CMR_INIT_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/cmr/define.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** General purpose class hiding global functions used for the "Content Mapping Resource"
 *  (CMR) from the global namespace.
 *  All functions are static and can, therefore, be accessed without creating an instance
 *  of this class.
 */
class DCMTK_CMR_EXPORT ContentMappingResource
{

  public:

    /** initialize \e all supported context groups of the "Content Mapping Resource"
     *  explicitly.  Currently, the following context group classes are supported:
     *  - CID42_NumericValueQualifier
     *  - CID5000_Languages
     *  - CID5001_Countries
     *  - CID7445_DeviceParticipatingRoles
     *  - CID7452_OrganizationalRoles
     *  - CID7453_PerformingRoles
     *
     ** @note In case of multi-threaded applications, this function could be called
     *        before any context group is actually used.  This might be easier than
     *        calling the initialize() function of the respective context group classes
     *        separately.  However, since the list of supported context groups is
     *        increasing over time, the additional resource requirements should be
     *        considered.  That means, if only a few context groups are used, it is
     *        recommended to call the individual initialize() functions instead.
     */
    static void initializeAllContextGroups();
};

#endif
