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
 *  Module:  dcmnet
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: 
 *    class DcmAssociationConfigurationFile
 * 
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2003-06-10 14:27:33 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmnet/include/Attic/dcasccff.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCASCCFF_H
#define DCASCCFF_H

#include "osconfig.h"
#include "ofcond.h"   /* for class OFCondition */

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
 * Revision 1.1  2003-06-10 14:27:33  meichel
 * Initial release of class DcmAssociationConfiguration and support
 *   classes. This class maintains a list of association negotiation
 *   profiles that can be addressed by symbolic keys. The profiles may
 *   be read from a configuration file.
 *
 *
 */
