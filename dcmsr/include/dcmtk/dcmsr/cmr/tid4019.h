/*
 *
 *  Copyright (C) 2018, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class TID4019_AlgorithmIdentification
 *
 *  Author: Joerg Riesmeier
 *
 */


#ifndef CMR_TID4019_H
#define CMR_TID4019_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrstpl.h"

#include "dcmtk/dcmsr/cmr/define.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Template:
 *  TID 4019 - Algorithm Identification.
 *  All added content items are annotated with a text in the format "TID 4019 - Row [n]".
 */
class DCMTK_CMR_EXPORT TID4019_AlgorithmIdentification
  : public DSRSubTemplate
{

  public:

    /** default constructor
     */
    TID4019_AlgorithmIdentification();

    /** set mandatory identification of the algorithm (according to TID 4019 Row 1 and 2).
     *  Any previously added algorithm parameters are removed if the passed values are valid.
     ** @param  algorithmName     name assigned by the manufacturer to the software algorithm
     *  @param  algorithmVersion  version identifier assigned to the software algorithm
     *  @param  check             if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setIdentification(const OFString &algorithmName,
                                  const OFString &algorithmVersion,
                                  const OFBool check = OFTrue);

    /** add optional algorithm parameter (according to TID 4019 Row 3).
     *  Calling this method requires that the mandatory identification has already been set.
     ** @param  algorithmParameter  input parameter used to configure the behavior of the
     *                              software algorithm
     *  @param  check               if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addParameter(const OFString &algorithmParameter,
                             const OFBool check = OFTrue);
};


/*-------------------*
 *  type definition  *
 *-------------------*/

// define short name for the SR template class
typedef TID4019_AlgorithmIdentification CMR_TID4019;


#endif
