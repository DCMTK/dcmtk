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
 *  Author: Marco Eichelberg, Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DVConfiguration
 *
 */


#ifndef DVPSCF_H
#define DVPSCF_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstring.h"   /* for class OFString */
#include "dcmtk/oflog/oflog.h"         /* for OFLogger::LogLevel */
#include "dcmtk/dcmdata/dctypes.h"    /* for Uint32 */
#include "dcmtk/dcmpstat/dvpstyp.h"    /* for DVPS enums */
#include "dcmtk/dcmpstat/dpdefine.h"

class OFConfigFile;

/** Configuration class for the Softcopy Presentation State viewer.
 *  This class manages the configuration file content used by the
 *  presentation state viewer.
 */
class DCMTK_DCMPSTAT_EXPORT DVConfiguration
{
 public:

  /** constructor.
   *  @param config_file filename (path) of the config file to be used.
   *     The caller should make sure that the config file
   *     really exists because the constructor cannot return an error status.
   *     If a non-existing filename (or NULL) is passed, an empty configuration file
   *     is assumed.
   */
  DVConfiguration(const char *config_file=NULL);

  /** destructor.
   */
  virtual ~DVConfiguration();

  /* access to communication partner data */

  /** returns the number of communication partners (send targets)
   *  in the configuration file.
   *  @param peerType defines a filter for the peer service type. Default: handle only storage peers.
   *  @return number of communication partners
   */
  Uint32 getNumberOfTargets(DVPSPeerType peerType=DVPSE_storage);

  /** returns the target identifier of the communication partner
   *  with the given index. The target identifier is unique within the configuration file
   *  @param idx index, must be < getNumberOfTargets()
   *  @param peerType defines a filter for the peer service type. Default: handle only storage peers.
   *  @return target identifier if found, NULL otherwise.
   */
  const char *getTargetID(Uint32 idx, DVPSPeerType peerType=DVPSE_storage);

  /** returns the DESCRIPTION entry for the communication partner with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID().
   *  @return entry if present in the config file, NULL otherwise.
   */
  const char *getTargetDescription(const char *targetID);

  /** returns the HOSTNAME entry for the communication partner with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID().
   *  @return entry if present in the config file, NULL otherwise.
   */
  const char *getTargetHostname(const char *targetID);

  /** returns the PORT entry for the communication partner with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID().
   *  @return entry if present and parsable in the config file, 0 otherwise.
   */
  unsigned short getTargetPort(const char *targetID);

  /** returns the TYPE entry for the communication partner with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID().
   *  @return entry if present and parsable in the config file, DVPSE_storage otherwise.
   */
  DVPSPeerType getTargetType(const char *targetID);

  /** returns the AETITLE entry for the communication partner with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID().
   *  @return entry if present in the config file, NULL otherwise.
   */
  const char *getTargetAETitle(const char *targetID);

  /** returns the MAXPDU entry for the communication partner with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID().
   *  @return entry if present and parsable in the config file, 0 otherwise.
   */
  unsigned long getTargetMaxPDU(const char *targetID);

  /** returns the TIMEOUT entry for the communication partner with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID().
   *  @return entry if present and parsable in the config file, -1 otherwise.
   */
  Sint32 getTargetTimeout(const char *targetID);

  /** returns the IMPLICITONLY entry for the communication partner with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID().
   *  @return entry if present in the config file, OFFalse otherwise.
   */
  OFBool getTargetImplicitOnly(const char *targetID);

  /** returns the DISABLENEWVRS entry for the communication partner with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID().
   *  @return entry if present in the config file, OFFalse otherwise.
   */
  OFBool getTargetDisableNewVRs(const char *targetID);

  /** returns the BITPRESERVINGMODE entry for the storage peer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID().
   *  @return entry if present in the config file, OFFalse otherwise.
   */
  OFBool getTargetBitPreservingMode(const char *targetID);

  /** returns the CORRECTUIDPADDING entry for the storage peer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID().
   *  @return entry if present in the config file, OFFalse otherwise.
   */
  OFBool getTargetCorrectUIDPadding(const char *targetID);

  /** returns the USETLS entry for the storage peer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID().
   *  @return entry if present in the config file, OFFalse otherwise.
   */
  OFBool getTargetUseTLS(const char *targetID);

  /** returns the CERTIFICATE entry for the storage peer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID().
   *  @return entry if present in the config file, NULL otherwise.
   */
  const char *getTargetCertificate(const char *targetID);

  /** returns the PRIVATEKEY entry for the storage peer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID().
   *  @return entry if present in the config file, NULL otherwise.
   */
  const char *getTargetPrivateKey(const char *targetID);

  /** returns the PRIVATEKEYPASSWORD entry for the storage peer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID().
   *  @return entry if present in the config file, NULL otherwise.
   */
  const char *getTargetPrivateKeyPassword(const char *targetID);

  /** returns the number of distinct values (separated by backslash characters)
   *  in the CIPHERSUITES entry for the storage peer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID().
   *  @return number of values if entry present in the config file, 0 otherwise.
   */
  Uint32 getTargetNumberOfCipherSuites(const char *targetID);

  /** returns one value from the CIPHERSUITES entry for the storage peer
   *  with the given target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID().
   *  @param idx index of the value, must be < getTargetNumberOfCipherSuites(targetID)
   *  @param value the result is both stored in this object and returned as return value.
   *  @return value if present, NULL otherwise.
   */
  const char *getTargetCipherSuite(const char *targetID, Uint32 idx, OFString& value);

  /** returns the PEERAUTHENTICATION entry for the communication partner with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID().
   *  @return entry if present and parsable in the config file, DVPSQ_require otherwise.
   */
  DVPSCertificateVerificationType getTargetPeerAuthentication(const char *targetID);

  /** returns the DIFFIEHELLMANPARAMETERS entry for the storage peer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID().
   *  @return entry if present in the config file, NULL otherwise.
   */
  const char *getTargetDiffieHellmanParameters(const char *targetID);

  /** returns the RANDOMSEED entry for the storage peer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID().
   *  @return entry if present in the config file, NULL otherwise.
   */
  const char *getTargetRandomSeed(const char *targetID);

  /** returns the SUPPORTSPRESENTATIONLUT entry for the printer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return entry if present in the config file, OFFalse otherwise.
   */
  OFBool getTargetPrinterSupportsPresentationLUT(const char *targetID);

  /** returns the PRESENTATIONLUTMATCHREQUIRED entry for the printer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return entry if present in the config file, OFTrue otherwise.
   */
  OFBool getTargetPrinterPresentationLUTMatchRequired(const char *targetID);

  /** returns the PRESENTATIONLUTPREFERSCPRENDERING entry for the printer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return entry if present in the config file, OFFalse otherwise.
   */
  OFBool getTargetPrinterPresentationLUTPreferSCPRendering(const char *targetID);

  /** returns the PRESENTATIONLUTINFILMSESSION entry for the printer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return entry if present in the config file, OFFalse otherwise.
   */
  OFBool getTargetPrinterPresentationLUTinFilmSession(const char *targetID);

  /** returns the SUPPORTS12BIT entry for the printer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return entry if present in the config file, OFTrue otherwise
   *   (default is 12 bit supported).
   */
  OFBool getTargetPrinterSupports12BitTransmission(const char *targetID);

  /** returns the SUPPORTSIMAGESIZE entry for the printer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return entry if present in the config file, OFFalse otherwise.
   */
  OFBool getTargetPrinterSupportsRequestedImageSize(const char *targetID);

  /** returns the SUPPORTSDECIMATECROP entry for the printer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return entry if present in the config file, OFFalse otherwise.
   */
  OFBool getTargetPrinterSupportsDecimateCrop(const char *targetID);

  /** returns the SUPPORTSTRIM entry for the printer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return entry if present in the config file, OFFalse otherwise.
   */
  OFBool getTargetPrinterSupportsTrim(const char *targetID);

  /** returns the number of distinct values (separated by backslash characters)
   *  in the BORDERDENSITY entry for the printer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return number of values if entry present in the config file, 0 otherwise.
   */
  Uint32 getTargetPrinterNumberOfBorderDensities(const char *targetID);

  /** returns one value from the BORDERDENSITY entry for the printer
   *  with the given target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @param idx index of the value, must be < getTargetPrinterNumberOfBorderDensities(targetID)
   *  @param value the result is both stored in this object and returned as return value.
   *  @return value if present, NULL otherwise.
   */
  const char *getTargetPrinterBorderDensity(const char *targetID, Uint32 idx, OFString& value);

  /** returns the number of distinct values (separated by backslash characters)
   *  in the MAXDENSITY entry for the printer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return number of values if entry present in the config file, 0 otherwise.
   */
  Uint32 getTargetPrinterNumberOfMaxDensities(const char *targetID);

  /** returns one value from the MAXDENSITY entry for the printer
   *  with the given target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @param idx index of the value, must be < getTargetPrinterNumberOfMaxDensities(targetID)
   *  @param value the result is both stored in this object and returned as return value.
   *  @return value if present, NULL otherwise.
   */
  const char *getTargetPrinterMaxDensity(const char *targetID, Uint32 idx, OFString& value);

  /** returns the number of distinct values (separated by backslash characters)
   *  in the MINDENSITY entry for the printer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return number of values if entry present in the config file, 0 otherwise.
   */
  Uint32 getTargetPrinterNumberOfMinDensities(const char *targetID);

  /** returns one value from the MINDENSITY entry for the printer
   *  with the given target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @param idx index of the value, must be < getTargetPrinterNumberOfMinDensities(targetID)
   *  @param value the result is both stored in this object and returned as return value.
   *  @return value if present, NULL otherwise.
   */
  const char *getTargetPrinterMinDensity(const char *targetID, Uint32 idx, OFString& value);

  /** returns the number of distinct values (separated by backslash characters)
   *  in the DISPLAYFORMAT entry for the printer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return number of values if entry present in the config file, 0 otherwise.
   */
  Uint32 getTargetPrinterNumberOfPortraitDisplayFormats(const char *targetID);

  /** returns one row value from the DISPLAYFORMAT entry for the printer
   *  with the given target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @param idx index of the value, must be < getTargetPrinterNumberOfPortraitDisplayFormats(targetID)
   *  @return number of rows for this display format if present, 0 otherwise
   */
  Uint32 getTargetPrinterPortraitDisplayFormatRows(const char *targetID, Uint32 idx);

  /** returns one columns value from the DISPLAYFORMAT entry for the printer
   *  with the given target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @param idx index of the value, must be < getTargetPrinterNumberOfPortraitDisplayFormats(targetID)
   *  @return number of columns for this display format if present, 0 otherwise
   */
  Uint32 getTargetPrinterPortraitDisplayFormatColumns(const char *targetID, Uint32 idx);

  /** returns OFTrue if an ANNOTATION entry for the printer with the given
   *  target ID from the configuration file exists or if the
   *  SESSIONLABELANNOTATION flag is true for the printer.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return OFTrue if printer supports annotation, OFFalse otherwise.
   */
  OFBool getTargetPrinterSupportsAnnotation(const char *targetID);

  /** returns OFTrue if an ANNOTATION entry for the printer with the given
   *  target ID from the configuration file exists.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return OFTrue if printer supports annotation, OFFalse otherwise.
   */
  OFBool getTargetPrinterSupportsAnnotationBoxSOPClass(const char *targetID);

  /** returns OFTrue if an SESSIONLABELANNOTATION entry for the printer 
   *  with the given target ID from the configuration file exists and is true.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return OFTrue if printer supports annotations in film session label, OFFalse otherwise.
   */
  OFBool getTargetPrinterSessionLabelAnnotation(const char *targetID);

  /** returns the second value from the ANNOTATION entry for the printer
   *  with the given target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @param value the result is both stored in this object and returned as return value.
   *  @return value if present, NULL otherwise.
   */
  const char *getTargetPrinterAnnotationDisplayFormatID(const char *targetID, OFString& value);
  
  /** returns the first value from the ANNOTATION entry for the printer
   *  with the given target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return value if present, 0 otherwise.
   */
  Uint16 getTargetPrinterAnnotationPosition(const char *targetID);
  
  /** returns the number of distinct values (separated by backslash characters)
   *  in the FILMSIZEID entry for the printer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return number of values if entry present in the config file, 0 otherwise.
   */
  Uint32 getTargetPrinterNumberOfFilmSizeIDs(const char *targetID);

  /** returns one value from the FILMSIZEID entry for the printer
   *  with the given target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @param idx index of the value, must be < getTargetPrinterNumberOfFilmSizeIDs(targetID)
   *  @param value the result is both stored in this object and returned as return value.
   *  @return value if present, NULL otherwise.
   */
  const char *getTargetPrinterFilmSizeID(const char *targetID, Uint32 idx, OFString& value);

  /** returns the number of distinct values (separated by backslash characters)
   *  in the MEDIUMTYPE entry for the printer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return number of values if entry present in the config file, 0 otherwise.
   */
  Uint32 getTargetPrinterNumberOfMediumTypes(const char *targetID);

  /** returns one value from the MEDIUMTYPE entry for the printer
   *  with the given target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @param idx index of the value, must be < getTargetPrinterNumberOfMediumTypes(targetID)
   *  @param value the result is both stored in this object and returned as return value.
   *  @return value if present, NULL otherwise.
   */
  const char *getTargetPrinterMediumType(const char *targetID, Uint32 idx, OFString& value);

  /** returns the number of distinct values (separated by backslash characters)
   *  in the FILMDESTINATION entry for the printer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return number of values if entry present in the config file, 0 otherwise.
   */
  Uint32 getTargetPrinterNumberOfFilmDestinations(const char *targetID);

  /** returns one value from the FILMDESTINATION entry for the printer
   *  with the given target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @param idx index of the value, must be < getTargetPrinterNumberOfFilmDestinations(targetID)
   *  @param value the result is both stored in this object and returned as return value.
   *  @return value if present, NULL otherwise.
   */
  const char *getTargetPrinterFilmDestination(const char *targetID, Uint32 idx, OFString& value);

  /** returns the number of distinct values (separated by backslash characters)
   *  in the RESOLUTIONID entry for the printer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return number of values if entry present in the config file, 0 otherwise.
   */
  Uint32 getTargetPrinterNumberOfPrinterResolutionIDs(const char *targetID);

  /** returns one value from the RESOLUTIONID entry for the printer
   *  with the given target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @param idx index of the value, must be < getTargetPrinterNumberOfPrinterResolutionIDs(targetID)
   *  @param value the result is both stored in this object and returned as return value.
   *  @return value if present, NULL otherwise.
   */
  const char *getTargetPrinterResolutionID(const char *targetID, Uint32 idx, OFString& value);

  /** returns the number of distinct values (separated by backslash characters)
   *  in the MAGNIFICATIONTYPE entry for the printer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return number of values if entry present in the config file, 0 otherwise.
   */
  Uint32 getTargetPrinterNumberOfMagnificationTypes(const char *targetID);

  /** returns one value from the MAGNIFICATIONTYPE entry for the printer
   *  with the given target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @param idx index of the value, must be < getTargetPrinterNumberOfMagnificationTypes(targetID)
   *  @param value the result is both stored in this object and returned as return value.
   *  @return value if present, NULL otherwise.
   */
  const char *getTargetPrinterMagnificationType(const char *targetID, Uint32 idx, OFString& value);

  /** returns the number of distinct values (separated by backslash characters)
   *  in the SMOOTHINGTYPE entry for the printer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return number of values if entry present in the config file, 0 otherwise.
   */
  Uint32 getTargetPrinterNumberOfSmoothingTypes(const char *targetID);

  /** returns one value from the SMOOTHINGTYPE entry for the printer
   *  with the given target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @param idx index of the value, must be < getTargetPrinterNumberOfSmoothingTypes(targetID)
   *  @param value the result is both stored in this object and returned as return value.
   *  @return value if present, NULL otherwise.
   */
  const char *getTargetPrinterSmoothingType(const char *targetID, Uint32 idx, OFString& value);

  /** returns the number of distinct values in the CONFIGURATION_x entries
   *  for the printer with the given target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return number of values if entry present in the config file, 0 otherwise.
   */
  Uint32 getTargetPrinterNumberOfConfigurationSettings(const char *targetID);

  /** returns the value from one of the CONFIGURATION_x entries for the printer
   *  with the given target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @param idx index of the value, must be < getTargetPrinterNumberOfConfigurationSettings(targetID)
   *  @return value if present, NULL otherwise.
   */
  const char *getTargetPrinterConfigurationSetting(const char *targetID, Uint32 idx);

  /** returns the number of distinct values (separated by backslash characters)
   *  in the EMPTYIMAGEDENSITY entry for the printer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return number of values if entry present in the config file, 0 otherwise.
   */
  Uint32 getTargetPrinterNumberOfEmptyImageDensities(const char *targetID);

  /** returns one value from the EMPTYIMAGEDENSITY entry for the printer
   *  with the given target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @param idx index of the value, must be < getTargetPrinterNumberOfEmptyImageDensities(targetID)
   *  @param value the result is both stored in this object and returned as return value.
   *  @return value if present, NULL otherwise.
   */
  const char *getTargetPrinterEmptyImageDensity(const char *targetID, Uint32 idx, OFString& value);

  /** returns the OMITSOPCLASSUIDFROMCREATERESPONSE entry for the printer with the given
   *  target ID from the configuration file.
   *  @param targetID communication target ID, must be one of the target
   *    identifiers returned by getTargetID() for peer type DVPSE_printerAny.
   *  @return entry if present in the config file, OFFalse otherwise.
   */
  OFBool getTargetPrintSCPOmitSOPClassUIDFromCreateResponse(const char *targetID);

  /* general settings */

  /** returns the directory used to store log files.
   *  Value is taken from the section GENERAL/APPLICATION/LOGDIRECTORY
   *  in the config file.
   *  @return log directory path, NULL if absent.
   */
  const char *getLogFolder();

  /** returns the name of the log file to be used for general application messages.
   *  Value is taken from the section GENERAL/APPLICATION/LOGFILE
   *  in the config file.
   *  @return name of the log file, NULL if absent.
   */
  const char *getLogFile();

  /** returns the log level / filter to be used for general application messages.
   *  Value is taken from the section GENERAL/APPLICATION/LOGLEVEL
   *  in the config file.
   *  @return log level, default: OFLogger::FATAL_LOG_LEVEL
   */
  OFLogger::LogLevel getLogLevel();

  /** returns the port on which the GUI application accepts notification 
   *  messages from the network processes.
   *  Value is taken from the section GENERAL/APPLICATION/MESSAGEPORT
   *  in the config file.
   *  @return message port, default: 0 (no message exchange).
   */
  unsigned short getMessagePort();

  /** Indicates whether client processes are allowed to keep their notification
   *  message port open during the lifetime of the process.
   *  Value is taken from the section GENERAL/APPLICATION/KEEPMESSAGEPORTOPEN
   *  in the config file.
   *  @return message port open flag, default: false
   */
  OFBool getMessagePortKeepOpen();

  /** returns the AETitle with which this application should identify itself.
   *  The AETitle is taken from the section GENERAL/NETWORK in the
   *  config file. If absent, a default value is returned.
   *  @return AETitle for this application. Never returns NULL.
   */
  const char *getNetworkAETitle();

  /** returns the AUTOCREATECONFIGFILE entry for the query/retrieve server
   *  from the section GENERAL/QUERY_RETRIEVE in the config file.
   *  @return entry if present in the config file, OFFalse otherwise.
   */
  OFBool getQueryRetrieveAutoCreateConfigFile();

  /** returns the AETitle with which the Q/R server should identify itself.
   *  The AETitle is taken from the section GENERAL/QUERY_RETRIEVE in the
   *  config file. If absent, a default value is returned.
   *  @return AETitle for the Q/R server. Never returns NULL.
   */
  const char *getQueryRetrieveAETitle();

  /** returns the PORT entry for the Q/R server
   *  from the section GENERAL/QUERY_RETRIEVE in the config file.
   *  @return entry if present and parsable in the config file, 0 otherwise.
   */
  unsigned short getQueryRetrievePort();

  /** returns the MAXPDU entry for the Q/R server
   *  from the section GENERAL/QUERY_RETRIEVE in the config file.
   *  @return entry if present and parsable in the config file, 0 otherwise.
   */
  unsigned long getQueryRetrieveMaxPDU();

  /** returns the TIMEOUT entry for the the Q/R server
   *  from the section GENERAL/QUERY_RETRIEVE in the config file.
   *  @return entry if present and parsable in the config file, -1 otherwise.
   */
  Sint32 getQueryRetrieveTimeout();

  /** returns the MaxAssociations entry for the Q/R server
   *  from the section GENERAL/QUERY_RETRIEVE in the config file.
   *  @return entry if present and parsable in the config file, 0 otherwise.
   */
  unsigned long getQueryRetrieveMaxAssociations();

  /** returns the database folder to be used for sending/receiving/browsing.
   *  Value is taken from the section GENERAL/DATABASE/DIRECTORY
   *  in the config file. If absent, a default value is returned.
   *  @return database folder path. Never returns NULL.
   */
  const char *getDatabaseFolder();

  /** returns the spool folder to be used for print jobs.
   *  Value is taken from the section GENERAL/PRINT/DIRECTORY
   *  in the config file. If absent, a default value is returned.
   *  @return spool folder path. Never returns NULL.
   */
  const char *getSpoolFolder();

  /** returns the DETAILEDLOG entry
   *  from the section GENERAL/PRINT in the config file.
   *  @return entry if present in the config file, OFFalse otherwise.
   */
  OFBool getDetailedLog();

  /** returns the BINARYLOG entry
   *  from the section GENERAL/PRINT in the config file.
   *  @return entry if present in the config file, OFFalse otherwise.
   */
  OFBool getBinaryLog();

  /** returns the filename (path) of the DICOM Store SCU application used
   *  for sending images, as configured in section
   *  GENERAL/NETWORK/SENDER in the config file.
   *  @return send application path name or NULL if absent.
   */
  const char *getSenderName();

  /** returns the filename (path) of the DICOM Store SCP application used
   *  for receiving images, as configured in section
   *  GENERAL/NETWORK/RECEIVER in the config file.
   *  @return receive application path name or NULL if absent.
   */
  const char *getReceiverName();

  /** returns the filename (path) of the DICOM Query/Retrieve SCP application,
   *  as configured in section GENERAL/QUERY_RETRIEVE/SERVER in the config file.
   *  @return send application path name or NULL if absent.
   */
  const char *getQueryRetrieveServerName();

  /** returns the filename (path) of the DICOM Print SCU application used
   *  for printing images, as configured in section
   *  GENERAL/PRINT/SPOOLER in the config file.
   *  @return receive application path name or NULL if absent.
   */
  const char *getSpoolerName();

  /** returns the filename (path) of the DICOM Print SCU application used
   *  for printing images, as configured in section
   *  GENERAL/PRINT/Server in the config file.
   *  @return receive application path name or NULL if absent.
   */
  const char *getPrintServerName();

  /** returns the filename (path) of the application used
   *  for dumping DICOM objects, as configured in section
   *  GENERAL/DATABASE/DUMP in the config file.
   *  @return dump application path name or NULL if absent.
   */
  const char *getDumpToolName();

  /** returns the filename (path) of the application used
   *  for dumping DICOM objects, as configured in section
   *  GENERAL/DATABASE/CHECK in the config file.
   *  @return check application path name or NULL if absent.
   */
  const char *getCheckToolName();

  /** returns the sleep delay for the print spooler in seconds,
   *  as configured in section GENERAL/PRINT/SLEEP in the config file.
   *  @return sleep delay, 0 if absent.
   */
  unsigned long getSpoolerSleep();

  /** returns the DELETEPRINTJOBS entry for the print spooler
   *  from the section GENERAL/PRINT in the config file.
   *  @return entry if present in the config file, OFFalse otherwise.
   */
  OFBool getSpoolerDeletePrintJobs();

  /** returns the ALWAYSDELETETERMINATEJOBS entry for the print spooler
   *  from the section GENERAL/PRINT in the config file.
   *  @return entry if present in the config file, OFFalse otherwise.
   */
  OFBool getSpoolerAlwaysDeleteTerminateJobs();

  /** returns the filename (path) of the monitor characteristics file
   *  used to implement that Barten transform, as configured in section
   *  GENERAL/MONITOR/CHARACTERISTICS in the config file.
   *  @return monitor characteristics path name or NULL if absent.
   */
  const char *getMonitorCharacteristicsFile();

  /** returns the width (in mm) of one pixel on the current monitor
   *  @return pixel width, 0 if unknown
   */
  double getMonitorPixelWidth();

  /** returns the height (in mm) of one pixel on the current monitor
   *  @return pixel height, 0 if unknown
   */
  double getMonitorPixelHeight();

  /** returns the maximum horizontal preview image resolution, from
   *  GENERAL/MONITOR/PREVIEW in the config file. Default: 0.
   *  @return maximum horizontal preview resolution
   */
  Uint32 getMaxPreviewResolutionX();

  /** returns the maximum vertical preview image resolution, from
   *  GENERAL/MONITOR/PREVIEW in the config file. Default: 0.
   *  @return maximum vertical preview resolution
   */
  Uint32 getMaxPreviewResolutionY();

  /** returns the value of configuration file entry key=value
   *  in the section GENERAL/GUI of the config file.
   *  Specified key must be upper case.
   *  If the entry is absent, NULL is returned.
   *  @param key the entry key
   *  @return entry value or NULL.
   */
  const char *getGUIConfigEntry(const char *key);

  /** returns the value of configuration file entry key=value
   *  in the section GENERAL/GUI of the config file.
   *  Specified key must be upper case.
   *  If the entry is absent or cannot be parsed, the default dfl
   *  is returned.
   *  @param key the entry key
   *  @param dfl the default to be used if the value is absent or incorrect
   *  @return entry value or default.
   */
  OFBool getGUIConfigEntryBool(const char *key, OFBool dfl);

  /** returns the folder to be used for Print Presentation LUTs.
   *  Value is taken from the section GENERAL/LUT/DIRECTORY
   *  in the config file. If absent, a default value is returned.
   *  @return LUT folder path. Never returns NULL.
   */
  const char *getLUTFolder();

  /** returns the folder to be used for Structured Reporting "templates".
   *  Value is taken from the section GENERAL/REPORT/DIRECTORY
   *  in the config file. If absent, a default value is returned.
   *  @return report folder path. Never returns NULL.
   */
  const char *getReportFolder();

  /* Presentation Look Up Tables (LUTs) */

  /** returns the number of Presentation LUTs in the configuration file.
   *  @return number of LUTs
   */
  Uint32 getNumberOfLUTs();

  /** returns the identifier of the Presentation LUT
   *  with the given index. The identifier is unique within the configuration file
   *  @param idx index, must be < getNumberOfLUTs()
   *  @return identifier if found, NULL otherwise.
   */
  const char *getLUTID(Uint32 idx);

  /** returns the DESCRIPTION entry for the LUT with the given
   *  ID from the configuration file.
   *  @param lutID LUT ID, must be one of the identifiers returned by getLUTID().
   *  @return entry if present in the config file, NULL otherwise.
   */
  const char *getLUTDescription(const char *lutID);

  /** returns the FILENAME entry for the LUT with the given
   *  ID from the configuration file.
   *  @param lutID LUT ID, must be one of the identifiers returned by getLUTID().
   *  @return entry if present in the config file, NULL otherwise.
   */
  const char *getLUTFilename(const char *lutID);

  /* Structured Reporting (SR) "templates" */

  /** returns the number of SR "templates" in the configuration file.
   *  @return number of SR "templates"
   */
  Uint32 getNumberOfReports();

  /** returns the identifier of the SR "template"
   *  with the given index. The identifier is unique within the configuration file
   *  @param idx index, must be < getNumberOfReports()
   *  @return identifier if found, NULL otherwise.
   */
  const char *getReportID(Uint32 idx);

  /** returns the DESCRIPTION entry for the SR "template" with the given
   *  ID from the configuration file.
   *  @param reportID SR "template" ID, must be one of the identifiers returned by
   *    getReportID().
   *  @return entry if present in the config file, NULL otherwise.
   */
  const char *getReportDescription(const char *reportID);

  /** returns the FILENAME entry for the SR "template" with the given
   *  ID from the configuration file.
   *  @param reportID SR "template" ID, must be one of the identifiers returned by
   *    getReportID().
   *  @return entry if present in the config file, NULL otherwise.
   */
  const char *getReportFilename(const char *reportID);

  /* general print settings */

  /** returns the minimum horizontal bitmap resolution for print, from
   *  GENERAL/PRINT/MINPRINTRESOLUTION in the config file. Default: 0.
   *  @return minimum horizontal bitmap resolution
   */
  Uint32 getMinPrintResolutionX();

  /** returns the minimum vertical bitmap resolution for print, from
   *  GENERAL/PRINT/MINPRINTRESOLUTION in the config file. Default: 0.
   *  @return minimum vertical bitmap resolution
   */
  Uint32 getMinPrintResolutionY();

  /** returns the maximum horizontal bitmap resolution for print, from
   *  GENERAL/PRINT/MAXPRINTRESOLUTION in the config file. Default: 0.
   *  @return maximum horizontal bitmap resolution
   */
  Uint32 getMaxPrintResolutionX();

  /** returns the maximum vertical bitmap resolution for print, from
   *  GENERAL/PRINT/MAXPRINTRESOLUTION in the config file. Default: 0.
   *  @return maximum vertical bitmap resolution
   */
  Uint32 getMaxPrintResolutionY();

  /** returns the default illumination for print, from
   *  GENERAL/PRINT/DEFAULTILLUMINATION in the config file. Default: 2000.
   *  @return default illumination for print
   */
  Uint16 getDefaultPrintIllumination();

  /** returns the default reflection for print, from
   *  GENERAL/PRINT/DEFAULTREFLECTION in the config file. Default: 10.
   *  @return default reflection for print
   */
  Uint16 getDefaultPrintReflection();

  /* VOI settings */

  /** returns the number of VOI Presets defined for the given modality
   *  @param modality Modality, e.g. "CT", "MR", "DX" etc.
   *  @return number of VOI Presets
   */
  Uint32 getNumberOfVOIPresets(const char *modality);

  /** returns the description string for the given VOI Preset
   *  @param modality Modality, e.g. "CT", "MR", "DX" etc.
   *  @param idx index of the value, must be < getNumberOfVOIPresets(modality)
   *  @return description if present, NULL otherwise.
   */
  const char *getVOIPresetDescription(const char *modality, Uint32 idx);

  /** returns the window center for the given VOI Preset
   *  @param modality Modality, e.g. "CT", "MR", "DX" etc.
   *  @param idx index of the value, must be < getNumberOfVOIPresets(modality)
   *  @return window center if present, 0.0 otherwise.
   */
  double getVOIPresetWindowCenter(const char *modality, Uint32 idx);

  /** returns the window width for the given VOI Preset
   *  @param modality Modality, e.g. "CT", "MR", "DX" etc.
   *  @param idx index of the value, must be < getNumberOfVOIPresets(modality)
   *  @return window width if present, 1.0 otherwise.
   */
  double getVOIPresetWindowWidth(const char *modality, Uint32 idx);

  /* TLS settings */
  
  /** returns the directory in which TLS related files (certificates, keys, 
   *  random data, Diffie-Hellman parameters etc.) are located. 
   *  @return TLS directory path, NULL if absent.
   */
  const char *getTLSFolder();

  /** returns the directory in which certificates of the trusted 
   *  Certification Authorities are located. 
   *  @return TLS CA Certificate directory path, NULL if absent.
   */
  const char *getTLSCACertificateFolder();

  /** returns the file format used for certificates, keys and Diffie-Hellman
   *  parameters. OFTrue for PEM ("privacy enhanced mail") format, OFFalse for
   *  DER ("distinguished encoding rules") format.  
   *  @return OFTrue for PEM (default), OFFalse for DER.
   */
  OFBool getTLSPEMFormat();

  /* User login settings */

  /** returns the directory in which user certificates and keys
   *  are located. 
   *  @return User key/certificate directory path, NULL if absent.
   */
  const char *getUserCertificateFolder();

  /** returns the number of configured users in the USERS section.
   *  @return number of configured users
   */
  Uint32 getNumberOfUsers();

  /** returns the symbolic identifier of the user with the given index.
   *  The symbolic identifier is unique within the configuration file.
   *  @param idx index, must be < getNumberOfUsers()
   *  @return identifier if found, NULL otherwise.
   */
  const char *getUserID(Uint32 idx);

  /** returns the login for the given user. If absent in the config file,
   *  returns NULL.
   *  @param userID user ID as returned by getUserID()
   *  @return login for the given user
   */
  const char *getUserLogin(const char *userID);

  /** returns the human readable name for the given user. 
   *  If absent in the config file, returns NULL.
   *  @param userID user ID as returned by getUserID()
   *  @return name for the given user
   */
  const char *getUserName(const char *userID);

  /** returns the name for the given user in DICOM Person Name (PN) format.
   *  If absent in the config file, returns NULL.
   *  @param userID user ID as returned by getUserID()
   *  @return DICOM PN name for the given user
   */
  const char *getUserDICOMName(const char *userID);

  /** returns the organization for the given user.
   *  If absent in the config file, returns NULL.
   *  @param userID user ID as returned by getUserID()
   *  @return organization for the given user
   */
  const char *getUserOrganization(const char *userID);

  /** returns the coding scheme designator of the user code for the given user if present.
   *  @param userID user ID as returned by getUserID()
   *  @param value the result is both stored in this object and returned as return value.
   *  @return value if present, NULL otherwise.
   */
  const char *getUserCodingSchemeDesignator(const char *userID, OFString& value);

  /** returns the coding scheme version of the user code for the given user if present.
   *  @param userID user ID as returned by getUserID()
   *  @param value the result is both stored in this object and returned as return value.
   *  @return value if present, NULL otherwise.
   */
  const char *getUserCodingSchemeVersion(const char *userID, OFString& value);

  /** returns the code value of the user code for the given user if present.
   *  @param userID user ID as returned by getUserID()
   *  @param value the result is both stored in this object and returned as return value.
   *  @return value if present, NULL otherwise.
   */
  const char *getUserCodeValue(const char *userID, OFString& value);

  /** returns the code meaning of the user code for the given user if present.
   *  @param userID user ID as returned by getUserID()
   *  @param value the result is both stored in this object and returned as return value.
   *  @return value if present, NULL otherwise.
   */
  const char *getUserCodeMeaning(const char *userID, OFString& value);

  /** returns the certificate file name for the given user.
   *  If absent in the config file, returns NULL.
   *  @param userID user ID as returned by getUserID()
   *  @return certificate file for the given user
   */
  const char *getUserCertificate(const char *userID);

  /** returns the private key file name for the given user.
   *  If absent in the config file, returns NULL.
   *  @param userID user ID as returned by getUserID()
   *  @return key file for the given user
   */
  const char *getUserPrivateKey(const char *userID);

private:

  /** private undefined copy constructor
   */
  DVConfiguration(const DVConfiguration&);

  /** private undefined assignment operator
   */
  DVConfiguration& operator=(const DVConfiguration&);

  /** returns the entry with the given keys.
   *  @param l2_key level 2 key
   *  @param l1_key level 1 key
   *  @param l0_key level 0 key (entry name)
   *  @return value assigned to the key if present, NULL otherwise.
   */
  const char *getConfigEntry(const char *l2_key, const char *l1_key, const char *l0_key);

  /** returns the entry with the given keys as bool.
   *  @param l2_key level 2 key
   *  @param l1_key level 1 key
   *  @param l0_key level 0 key (entry name)
   *  @param deflt default to be returned if entry is not present or syntax error.
   *  @return value assigned to the key if present, default otherwise.
   */
  OFBool getConfigBoolEntry(const char *l2_key, const char *l1_key, const char *l0_key, OFBool deflt);

  /* member variables */

  /** pointer to the configuration file data if the configuration file was found.
   *  NULL otherwise.
   */
  OFConfigFile *pConfig;

};

#endif
