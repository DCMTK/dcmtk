/*
 *
 *  Copyright (C) 1998-2000, OFFIS
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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSFilmSession
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-05-31 12:56:35 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSFS_H__
#define __DVPSFS_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dctk.h"
#include "dvpstyp.h"
#include "dimse.h"

class DVConfiguration;

/** a basic film session object managed by a Print SCP.
 */
class DVPSFilmSession
{
public:
  /** default constructor
   */
  DVPSFilmSession();
  
  /// copy constructor
  DVPSFilmSession(const DVPSFilmSession& copy);

  /** clone method.
   *  @return a pointer to a new DVPSFilmSession object containing
   *  a copy of this object.
   */
  DVPSFilmSession *clone() { return new DVPSFilmSession(*this); }

  /// destructor
  virtual ~DVPSFilmSession();

  /** performs a Print SCP Basic Film Session N-CREATE operation on a newly 
   *  created instance of this class. The results of the operation are 
   *  stored in the objects passed as rsp and rspDataset.
   *  @param cfg config file facility
   *  @param cfgname symbolic printer name in config file
   *  @param rqDataset N-CREATE request dataset, may be NULL
   *  @param rsp N-CREATE response message
   *  @param rspDataset N-CREATE response dataset passed back in this parameter
   *  @param peerae application entity title of the print SCU we're 
   *    communicating with. Used to create default values for ownerID
   *    and filmSessionLabel.
   *  @return OFTrue if N-CREATE was successful, OFFalse otherwise.
   */
  OFBool printSCPCreate(
    DVConfiguration& cfg, 
    const char *cfgname, 
    DcmDataset *rqDataset, 
    T_DIMSE_Message& rsp, 
    DcmDataset *& rspDataset, 
    const char *peerae);

  /** performs a Print SCP Basic Film Session N-SET operation on an instance 
   *  of this class. The results of the N-SET operation are stored in the 
   *  objects passed as rsp and rspDataset.
   *  @param cfg config file facility
   *  @param cfgname symbolic printer name in config file
   *  @param rqDataset N-SET request dataset
   *  @param rsp N-SET response message
   *  @param rspDataset N-SET response dataset passed back in this parameter
   *  @return OFTrue if N-SET was successful, OFFalse otherwise.
   */    
  OFBool printSCPSet(
    DVConfiguration& cfg, 
    const char *cfgname, 
    DcmDataset *rqDataset, 
    T_DIMSE_Message& rsp, 
    DcmDataset *& rspDataset);

  /** compares the SOP instance UID with the given UID string.
   *  @return OFTrue if UIDs are equal, OFFalse otherwise.
   */
  OFBool isInstance(const char *uid);
  
  /** returns the SOP instance UID of the basic film session.
   *  @return SOP instance UID
   */
  const char *getUID() { return sopInstanceUID.c_str(); }

private:
  /// SOP instance UID of basic film session object
  OFString                 sopInstanceUID;
  /// VR=IS, VM=1
  DcmIntegerString         numberOfCopies;
  /// VR=CS, VM=1
  DcmCodeString            printPriority;
  /// VR=CS, VM=1
  DcmCodeString            mediumType;
  /// VR=CS, VM=1
  DcmCodeString            filmDestination;
  /// VR=LO, VM=1
  DcmLongString            filmSessionLabel;
  /// VR=SH, VM=1
  DcmShortString           ownerID;  
};

#endif

/*
 *  $Log: dvpsfs.h,v $
 *  Revision 1.1  2000-05-31 12:56:35  meichel
 *  Added initial Print SCP support
 *
 *
 */

