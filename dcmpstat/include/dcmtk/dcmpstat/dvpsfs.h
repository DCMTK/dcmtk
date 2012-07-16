/*
 *
 *  Copyright (C) 1998-2012, OFFIS e.V.
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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSFilmSession
 *
 */

#ifndef DVPSFS_H
#define DVPSFS_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dvpstyp.h"
#include "dcmtk/dcmpstat/dpdefine.h"
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmnet/dimse.h"

class DVConfiguration;
class DVPSPresentationLUT_PList;
class DVPSStoredPrint_PList;
class DVPSStoredPrint;

/** a basic film session object managed by a Print SCP.
 */
class DCMTK_DCMPSTAT_EXPORT DVPSFilmSession
{
public:

  /** constructor
   *  @param illumin default Illumination setting
   *  @param reflection default Reflected Ambient Light setting
   */
  DVPSFilmSession(Uint16 illumin, Uint16 reflection);
  
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
   *  @param presentationLUTnegotiated 
   *    OFTrue if support for the Presentation LUT SOP class
   *    has been negotiated at association negotiation and is supported on
   *    Basic Film Session level
   *  @param globalPresentationLUTList
   *    list of presentation LUTs managed by the Print SCP
   *  @return OFTrue if N-CREATE was successful, OFFalse otherwise.
   */
  OFBool printSCPCreate(
    DVConfiguration& cfg, 
    const char *cfgname, 
    DcmDataset *rqDataset, 
    T_DIMSE_Message& rsp, 
    DcmDataset *& rspDataset, 
    const char *peerae,
    OFBool presentationLUTnegotiated,
    DVPSPresentationLUT_PList& globalPresentationLUTList);

  /** performs a Print SCP Basic Film Session N-SET operation on an instance 
   *  of this class. The results of the N-SET operation are stored in the 
   *  objects passed as rsp and rspDataset.
   *  @param cfg config file facility
   *  @param cfgname symbolic printer name in config file
   *  @param rqDataset N-SET request dataset
   *  @param rsp N-SET response message
   *  @param rspDataset N-SET response dataset passed back in this parameter
   *  @param presentationLUTnegotiated 
   *    OFTrue if support for the Presentation LUT SOP class
   *    has been negotiated at association negotiation and is supported on
   *    Basic Film Session level
   *  @param globalPresentationLUTList
   *    list of presentation LUTs managed by the Print SCP
   *  @param basicFilmBoxList list of basic film boxes. Presentation LUT
   *    settings are copied to all film boxes.
   *  @return OFTrue if N-SET was successful, OFFalse otherwise.
   */    
  OFBool printSCPSet(
    DVConfiguration& cfg, 
    const char *cfgname, 
    DcmDataset *rqDataset, 
    T_DIMSE_Message& rsp, 
    DcmDataset *& rspDataset,
    OFBool presentationLUTnegotiated,
    DVPSPresentationLUT_PList& globalPresentationLUTList,
    DVPSStoredPrint_PList& basicFilmBoxList);

  /** compares the SOP instance UID with the given UID string.
   *  @return OFTrue if UIDs are equal, OFFalse otherwise.
   */
  OFBool isInstance(const char *uid);
  
  /** returns the SOP instance UID of the basic film session.
   *  @return SOP instance UID
   */
  const char *getUID() { return sopInstanceUID.c_str(); }

  /** copies the film box Presentation LUT settings into the
   *  stored print object passed by reference.
   *  @param sp stored print object
   */
  void copyPresentationLUTSettings(DVPSStoredPrint& sp);
  
private:

  /// private undefined assignment operator
  DVPSFilmSession& operator=(const DVPSFilmSession&);

  /** writes a Referenced Presentation LUT SQ, Illumination and
   *  reflected ambient light to the given dataset.
   *  Helper function used when creating Basic Film Session or
   *  Basic Film Box.
   *  @param dset the dataset to which the data is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  OFCondition addPresentationLUTReference(DcmItem& dset);

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

  /// VR=US, VM=1, Type 2c required if presentation SOP class present
  DcmUnsignedShort         illumination;
  /// VR=US, VM=1, Type 2c required if presentation SOP class present
  DcmUnsignedShort         reflectedAmbientLight;
  /// the ReferencedPresentationLUTSequence is only created/read on the fly
  DcmUniqueIdentifier      referencedPresentationLUTInstanceUID;

  /** The Print SCP can be configured to enforce a rule requiring that the 
   *  number of entries in a Presentation LUT matches the bit depth of the 
   *  image pixel data. This member variable describes the type of the 
   *  current presentation LUT (if any).
   */
  DVPSPrintPresentationLUTAlignment referencedPresentationLUTAlignment;

};

#endif
