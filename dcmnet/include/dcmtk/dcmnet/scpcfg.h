/*
 *
 *  Copyright (C) 2012-2015, OFFIS e.V.
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
 *  Author:  Michael Onken, Jan Schlamelcher
 *
 *  Purpose: Class representing configuration of an SCP.
 *
 */

#ifndef SCPCFG_H
#define SCPCFG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmnet/dcasccff.h"  /* For reading a association config file */
#include "dcmtk/dcmnet/dcasccfg.h"  /* For holding association cfg file infos */
#include "dcmtk/dcmnet/dimse.h"
#include "dcmtk/ofstd/ofmem.h"      /* For OFshared_ptr */

/** Class that encapsulates an SCP configuration that is needed in order to
 *  configure the service negotiation behavior (presentation contexts, AE
 *  title, listening port, etc) as well as some runtime configuration like
 *  the DIMSE timeout. The configuration, however, is not updated to reflect
 *  any runtime connection information, e.g. information about the current
 *  SCU connected or the like.
 *
 */
class DCMTK_DCMNET_EXPORT DcmSCPConfig
{

public:

  /** Constructor, sets configuration's default values */
  DcmSCPConfig();

  /** Destructor, cleans up memory */
  ~DcmSCPConfig();

  /** copy constructor, performs deep copy.
   *  @param old item to be copied
   */
  DcmSCPConfig(const DcmSCPConfig &old);

  /** assignment operator, performs deep copy.
   *  @param obj the config to copy from
   */
  DcmSCPConfig &operator=(const DcmSCPConfig &obj);

  /** This method evaluates an incoming association request according
   *  presentation context settings maintained by this configuration.
   *  @param assoc T_ASC_Association structure to be evaluated
   *  @return EC_Normal if successful, an error code otherwise
   */
  OFCondition evaluateIncomingAssociation(T_ASC_Association &assoc) const;

  /* ************************************************************* */
  /*             Set methods for configuring SCP behavior          */
  /* ************************************************************* */

  /** Add abstract syntax to presentation contexts the SCP is able to negotiate with SCUs.
   *  @param abstractSyntax [in] The UID of the abstract syntax (e.g.\ SOP class) to add
   *  @param xferSyntaxes   [in] List of transfer syntaxes (UIDs) that should be supported
   *                             for the given abstract syntax name
   *  @param role           [in] The role to be negotiated
   *  @param profile        [in] The profile the abstract syntax should be added to. The
   *                             default is to add it to the DcmSCP's internal standard
   *                             profile called "DEFAULT".
   *  @return EC_Normal if adding was successful, an error code otherwise
   */
  OFCondition addPresentationContext(const OFString &abstractSyntax,
                                     const OFList<OFString> &xferSyntaxes,
                                     const T_ASC_SC_ROLE role = ASC_SC_ROLE_DEFAULT,
                                     const OFString &profile = "DEFAULT");

  /** Set SCP's TCP/IP listening port
   *  @param port [in] The port number to listen on. Note that usually on Unix-like systems
   *                   only root user is permitted to open ports below 1024.
   */
  void setPort(const Uint16 port);

  /** Set AE title of the server
   *  @param aetitle [in] The AE title of the server. By default, all SCU association requests
   *                      calling another AE title will be rejected. This behavior can be
   *                      changed by using the setRespondWithCalledAETitle() method.
   */
  void setAETitle(const OFString &aetitle);

  /** Set SCP to use the called AE title from the SCU request for the response, i.e.\ the SCP
   *  will always respond with setting its own name to the one the SCU used for calling.
   *  Overrides any AE title eventually set with setAETitle().
   *  @param useCalled [in] If OFTrue, the SCP will use the called AE title from the request
   *                        for responding. DcmSCP's default is OFFalse.
   */
  void setRespondWithCalledAETitle(const OFBool useCalled);

  /** Loads association configuration file
   *  @param assocFile [in] The filename of the association configuration to be loaded. The
   *                        association configuration file must be valid for an SCP.
   *  @return EC_Normal if loading was successful, error otherwise
   */
  OFCondition loadAssociationCfgFile(const OFString &assocFile);

  /** If an association profile should be selected, either by loading an association
   *  configuration file or using the addPresentationContext() function, one of those can
   *  be selected and checked for validity using this method.
   *  @param profileName [in] The name of the association profile which must be configured
   *                          before being selected here
   *  @return EC_Normal if selecting/checking was successful, an error code otherwise
   */
  OFCondition setAndCheckAssociationProfile(const OFString &profileName);

  /** Force every association request to be refused by SCP, no matter what the SCU is
   *  offering
   *  @param doRefuse [in] If OFTrue, every association is being refused. DcmSCP's default
   *                       is not to refuse every association.
   */
  void forceAssociationRefuse(const OFBool doRefuse);

  /** Set maximum PDU size the SCP is able to receive. This size is sent in association
   *  response message to SCU.
   *  @param maxRecPDU [in] The maximum PDU size to use in bytes
   */
  void setMaxReceivePDULength(const Uint32 maxRecPDU);

  /** Set whether waiting for a TCP/IP connection should be blocking or non-blocking.
   *  In non-blocking mode, the networking routines will wait for specified connection
   *  timeout, see setConnectionTimeout() function. In blocking mode, no timeout is set
   *  but the operating system's network routines will be used to read from the socket
   *  for incoming data. In the worst case, this may be a long time until that call
   *  returns. The default of DcmSCP is blocking mode.
   *  @param blockingMode [in] Either DUL_BLOCK for blocking mode or DUL_NOBLOCK
   *                           for non-blocking mode
   */
  void setConnectionBlockingMode(const DUL_BLOCKOPTIONS blockingMode);

  /** Set whether DIMSE messaging should be blocking or non-blocking. In non-blocking mode,
   *  the networking routines will wait for DIMSE messages for the specified DIMSE timeout
   *  time, see setDIMSETimeout() function. In blocking mode, no timeout is set but the
   *  operating system's network routines will be used to read from the socket for new data.
   *  In the worst case, this may be a long time until that call returns. The default of
   *  DcmSCP is blocking mode.
   *  @param blockingMode [in] Either DIMSE_BLOCKING for blocking mode or DIMSE_NONBLOCKING
   *                           for non-blocking mode
   */
  void setDIMSEBlockingMode(const T_DIMSE_BlockingMode blockingMode);

  /** Set the timeout to be waited for incoming DIMSE message packets. This is only relevant
   *  for DIMSE blocking mode messaging (see also setDIMSEBlockingMode().
   *  @param dimseTimeout [in] DIMSE receive timeout in seconds
   */
  void setDIMSETimeout(const Uint32 dimseTimeout);

  /** Set the timeout used during ACSE messaging protocol.
   *  @param acseTimeout [in] ACSE timeout in seconds.
   */
  void setACSETimeout(const Uint32 acseTimeout);

  /** Set the timeout that should be waited for connection requests.
   *  Only relevant in non-blocking mode (default).
   *  @param timeout [in] TCP/IP connection timeout in seconds.
   */
  void setConnectionTimeout(const Uint32 timeout);

  /** Set whether to show presentation contexts in verbose or debug mode
   *  @param mode [in] Show presentation contexts in verbose mode if OFTrue. By default, the
   *                   presentation contexts are shown in debug mode.
   */
  void setVerbosePCMode(const OFBool mode);

  /** Enables or disables looking up the host name from a connecting system.
   *  Note that this sets a GLOBAL flag in DCMTK, i.e. the behavior changes
   *  for all servers. This should be changed in the future.
   *  @param mode [in] OFTrue, if hostname lookup should be enabled,
   *              OFFalse for disabling it.
   */
  void setHostLookupEnabled(const OFBool mode);

  /** Set the mode that specifies whether the progress of sending and receiving DIMSE messages
   *  is notified by calling DcmSCP::notifySENDProgress() and DcmSCP::notifyRECEIVEProgress(),
   *  respectively. The progress notification is enabled by default.
   *  @param mode [in] Disable progress notification if OFFalse
   */
  void setProgressNotificationMode(const OFBool mode);

  /* Get methods for SCP settings */

  /** Returns TCP/IP port number SCP listens for new connection requests
   *  @return The port number
   */
  Uint16 getPort() const;

  /** Returns SCP's own AE title. Only used if the SCP is not configured to respond with the
   *  called AE title the SCU uses for association negotiation, see setRespondWithCalledAETitle().
   *  @return The configured AE title
   */
  const OFString &getAETitle() const;

  /** Returns whether SCP uses the called AE title from SCU requests to respond to connection
   *  requests instead of a configured AE title
   *  @return OFTrue, if the SCU's calling AE title is utilized, OFFalse otherwise
   */
  OFBool getRespondWithCalledAETitle() const;

  /** Returns whether SCP should refuse any association request no matter what the SCU proposes
   *  @return OFTrue, if SCP is configured to refuse every association
   */
  OFBool getRefuseAssociation() const;

  /** Returns maximum PDU length configured to be received by SCP
   *  @return Maximum PDU length in bytes
   */
  Uint32 getMaxReceivePDULength() const;

  /** Returns whether receiving of TCP/IP connection requests is done in blocking or
   *  unblocking mode
   *  @return DUL_BLOCK if in blocking mode, otherwise DUL_NOBLOCK
   */
  DUL_BLOCKOPTIONS getConnectionBlockingMode() const;

  /** Returns whether receiving of DIMSE messages is done in blocking or unblocking mode
   *  @return DIMSE_BLOCKING if in blocking mode, otherwise DIMSE_NONBLOCKING
   */
  T_DIMSE_BlockingMode getDIMSEBlockingMode() const;

  /** Returns DIMSE timeout (only applicable in blocking mode)
   *  @return DIMSE timeout in seconds
   */
  Uint32 getDIMSETimeout() const;

  /** Returns ACSE timeout
   *  @return ACSE timeout in seconds
   */
  Uint32 getACSETimeout() const;

  /** Returns connection timeout
   *  @return TCP/IP connection timeout in seconds
   */
  Uint32 getConnectionTimeout() const;

  /** Returns the verbose presentation context mode configured specifying whether details on
   *  the presentation contexts (negotiated during association setup) should be shown in
   *  verbose or debug mode. The latter is the default.
   *  @return The verbose presentation context mode configured
   */
  OFBool getVerbosePCMode() const;

  /** Returns whether a connecting system's host name is looked up.
   *  @return OFTrue, if hostname lookup is enabled, OFFalse otherwise
   */
  OFBool getHostLookupEnabled() const;

  /** Returns the mode that specifies whether the progress of sending and receiving DIMSE
   *  messages is notified by calling DcmSCP::notifySENDProgress() and
   *  DcmSCP::notifyRECEIVEProgress(), respectively. The progress notification is enabled
   *  by default.
   *  @return The current progress notification mode, enabled if OFTrue
   */
  OFBool getProgressNotificationMode() const;

  /** Dump presentation contexts to given output stream, useful for debugging.
   *  @param out [out] The output stream
   *  @param profileName [in] The profile to dump. If empty (default), the currently
   *                     selected profile is dumped.
   */
  void dumpPresentationContexts(STD_NAMESPACE ostream &out,
                                OFString profileName = "");

protected:

  /** Mangles a given string into valid profile name.
   *  Removes all spaces, and converts lower case to upper case
   *  characters.
   *  @param profile [in] The unmangled profile name
   *  @return The mangled profile name
   */
  OFString mangleProfileName(const OFString& profile) const;

  /** Convert number to string
   *  @param num [in] The integer number to convert
   *  @return The string representing the given number
   */
  OFString numToString(const size_t num) const;

  /** Add given list of transfers syntaxes under given name to SCP's
   *  association configuration.
   *  @param tsListName [in] The name of the transfer syntax list
   *                    to create
   *  @param ts [in] The list of transfer snytaxes to add
   *  @return EC_Normal if adding was successful, error otherwise
   */
  OFCondition addNewTSList(const OFString& tsListName,
                           const OFList<OFString>& ts);

  /// Association configuration. May be filled from association configuration file or by
  /// adding presentation contexts by calling addPresentationContext() (or both)
  DcmAssociationConfiguration m_assocConfig;

  /// Profile in association configuration that should be used. By default, a profile
  /// called "DEFAULT" is used.
  OFString m_assocCfgProfileName;

  /// Port on which the SCP is listening for association requests. The default port is 104.
  Uint16 m_port;

  /// AE title to be used for responding to SCU (default: DCMTK_SCP). This value is not
  /// evaluated if the the SCP is configured to respond to any association requests with the
  /// name the SCU used as called AE title (which is the SCP's default behavior); see
  /// setRespondWithCalledAETitle().
  OFString m_aetitle;

  /// Indicates if the application shall refuse any association attempt regardless of what
  /// the SCU proposes.
  OFBool m_refuseAssociation;

  /// Maximum PDU size the SCP is able to receive. This value is sent to the SCU during
  /// association negotiation.
  Uint32 m_maxReceivePDULength;

  /// Blocking mode for TCP/IP connection requests. If non-blocking mode is enabled, the SCP is
  /// waiting for new DIMSE data a specific (m_connectionTimeout) amount of time and then returns
  /// if not data arrives. In blocking mode, the SCP is calling the underlying operating
  /// system function for receiving data from the socket directly, which may return after a
  /// very long time (or never...), depending on the system's network configuration.
  /// Default is blocking mode.
  DUL_BLOCKOPTIONS m_connectionBlockingMode;

  /// Blocking mode for DIMSE operations. If DIMSE non-blocking mode is enabled, the SCP is
  /// waiting for new DIMSE data a specific (m_dimseTimeout) amount of time and then returns
  /// if not data arrives. In blocking mode the SCP is calling the underlying operating
  /// system function for receiving data from the socket directly, which may return after a
  /// very long time, depending on the system's network configuration.
  T_DIMSE_BlockingMode m_dimseBlockingMode;

  /// Timeout for DIMSE operations in seconds. Maximum time in DIMSE non-blocking mode to
  /// wait for incoming DIMSE data.
  Uint32 m_dimseTimeout;

  /// Timeout for ACSE operations in seconds. Maximum time during association negotiation
  /// which is given for the SCU to follow the ACSE protocol.
  Uint32 m_acseTimeout;

  /// Verbose PC mode. Flags specifying whether details on the presentation contexts
  /// (negotiated during association setup) should be shown in verbose or debug mode.
  OFBool m_verbosePCMode;

  /// Timeout in seconds that should be waited for an incoming TCP/IP connection until
  /// the call returns. It is only relevant if the the SCP is set to non-blocking
  /// connection mode. Otherwise, the timeout is ignored. Default is 1000 seconds.
  Uint32 m_connectionTimeout;

  /// If set, the AE title as received in the request (called AE title) is used in response
  /// (default: OFTrue).
  OFBool m_respondWithCalledAETitle;

  /// Progress notification mode (default: OFTrue)
  OFBool m_progressNotificationMode;
};

/** Enables sharing configurations by multiple DcmSCPs.
 *  DcmSharedSCPConfig allows multiple DcmSCP objects to share a single DcmSCPConfig object
 *  as their configuration. DcmSharedSCPConfig is implemented using reference counting,
 *  so the underlying DcmSCP config object is destroyed only after the last reference to
 *  it is removed.
 */
struct DCMTK_DCMNET_EXPORT DcmSharedSCPConfig : private OFshared_ptr<DcmSCPConfig>
{
  /** Default construct a DcmSharedSCPConfig object referring to a default
   *  constructed DcmSCPConfig object.
   */
  inline DcmSharedSCPConfig()
  : OFshared_ptr<DcmSCPConfig>( new DcmSCPConfig ) {}

  /** Copy construct a DcmSharedSCPConfig object from a DcmSCPConfig object.
   *  This constructor enables sharing of an already existing DcmSCPConfig object
   *  by copy constructing a sharable configuration object from it.
   *  @param config the DcmSCPConfig object to copy construct the DcmSharedSCPConfig
   *    object from.
   */
  inline explicit DcmSharedSCPConfig(const DcmSCPConfig& config)
  : OFshared_ptr<DcmSCPConfig>( new DcmSCPConfig( config ) ) {}

  /** Access the shared DcmSCPConfig object.
   *  @return a pointer to the shared DcmSCPConfig object.
   */
  inline DcmSCPConfig* operator->() const { return get(); }

  /** Access the shared DcmSCPConfig object.
   *  @return a reference to the shared DcmSCPConfig object.
   */
  inline DcmSCPConfig& operator*() const { return *get(); }
};

#endif // SCPCFG_H
