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
 *  Module: dcmnet
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DcmTransportLayer
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-08-10 14:50:52 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DCMLAYER_H
#define __DCMLAYER_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "oftypes.h"
#include "ofstring.h"

BEGIN_EXTERN_C
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
END_EXTERN_C

/** this enum represents the result of a transport layer operation
 *  which may be a transparent TCP/IP or a secure TLS operation.
 */
enum DcmTransportLayerStatus
{
  /** successful operation
   */
  TCS_ok,

  /** operation cannot be performed because transport connection
   *  object was not allocated.
   */
  TCS_noConnection,

  /** unspecified error
   */
  TCS_unspecifiedError
};

class DcmTransportConnection;

/** factory class which creates transport layer connections.  
 *  Base class only supports transparent TCP connections, subclasses
 *  may also support secure transport layer connections.
 */  

class DcmTransportLayer
{
public:

  /** constructor.
   *  @param networkRole network role to be used by the application, influences
   *    the choice of the secure transport layer code.
   */  
  DcmTransportLayer(int /* networkRole */ ) { /* empty */ }

  /// destructor
  virtual ~DcmTransportLayer();

  /** factory method that returns a new transport connection for the
   *  given socket.  Depending on the second parameter, either a transparent
   *  or a secure connection is established.  If the object cannot be created
   *  (e. g. because no secure layer is available), returns NULL.
   *  @param openSocket TCP/IP socket to be used for the transport connection.
   *    the connection must already be establised on socket level. If a non-null
   *    pointer is returned, the new connection object takes over control of the socket.
   *  @param useSecureLayer if true, a secure layer is used. If false, a
   *    transparent layer is used.
   *  @return pointer to new connection object if successful, NULL otherwise.
   */
  virtual DcmTransportConnection *createConnection(int openSocket, OFBool useSecureLayer);

private:

  /// private undefined copy constructor
  DcmTransportLayer(const DcmTransportLayer&);

  /// private undefined assignment operator
  DcmTransportLayer& operator=(const DcmTransportLayer&);

};


#endif

/*
 *  $Log: dcmlayer.h,v $
 *  Revision 1.1  2000-08-10 14:50:52  meichel
 *  Added initial OpenSSL support.
 *
 *
 */

