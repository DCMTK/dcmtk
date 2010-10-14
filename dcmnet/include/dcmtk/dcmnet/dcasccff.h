/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Module:  dcmnet
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: 
 *    class DcmAssociationConfigurationFile
 * 
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:17:21 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCASCCFF_H
#define DCASCCFF_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcond.h"   /* for class OFCondition */

class DcmAssociationConfiguration;
class OFConfigFile;

/** helper class that encapsulated methods for filling 
 *  a DcmAssociationConfiguration object from a config file
 */
class DcmAssociationConfigurationFile
{
public:

  /** reads the given configuration file and feeds the information
   *  contained therein into the association configuration object.
   *  @param cfg association configuration object to initialize
   *  @param filename name of config file
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition initialize(
    DcmAssociationConfiguration& cfg,
    const char *filename);

private:

  /** parses the transfer syntax lists in the config file.
   *  @param cfg association configuration object to initialize
   *  @param filename name of config file
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition parseTransferSyntaxes(
    DcmAssociationConfiguration& cfg,
    OFConfigFile& config);

  /** parses the presentation context lists in the config file.
   *  @param cfg association configuration object to initialize
   *  @param filename name of config file
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition parsePresentationContexts(
    DcmAssociationConfiguration& cfg,
    OFConfigFile& config);

  /** parses the role selection lists in the config file.
   *  @param cfg association configuration object to initialize
   *  @param filename name of config file
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition parseRoleSelectionItems(
    DcmAssociationConfiguration& cfg,
    OFConfigFile& config);

  /** parses the extended negotiation lists in the config file.
   *  @param cfg association configuration object to initialize
   *  @param filename name of config file
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition parseExtendedNegotiationItems(
    DcmAssociationConfiguration& cfg,
    OFConfigFile& config);

  /** parses the association configuration profile lists in the config file.
   *  @param cfg association configuration object to initialize
   *  @param filename name of config file
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition parseProfiles(
    DcmAssociationConfiguration& cfg,
    OFConfigFile& config);

};

#endif

/*
 * CVS/RCS Log
 * $Log: dcasccff.h,v $
 * Revision 1.3  2010-10-14 13:17:21  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.2  2005/12/08 16:02:08  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.1  2003/06/10 14:27:33  meichel
 * Initial release of class DcmAssociationConfiguration and support
 *   classes. This class maintains a list of association negotiation
 *   profiles that can be addressed by symbolic keys. The profiles may
 *   be read from a configuration file.
 *
 *
 */
