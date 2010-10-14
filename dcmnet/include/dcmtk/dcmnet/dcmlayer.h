/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *  Module: dcmnet
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DcmTransportLayer
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:17:22 $
 *  CVS/RCS Revision: $Revision: 1.8 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef DCMLAYER_H
#define DCMLAYER_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftypes.h"
#include "dcmtk/ofstd/ofstring.h"

#define INCLUDE_UNISTD
#include "dcmtk/ofstd/ofstdinc.h"

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

  /** operation failed due to an error within the TLS protocol layer
   */
  TCS_tlsError,

  /** operation failed because an illegal parameter was passed
   */
  TCS_illegalCall,

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
   *  #param networkRole network role to be used by the application, influences
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
 *  Revision 1.8  2010-10-14 13:17:22  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.7  2005/12/08 16:02:16  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.6  2004/08/03 11:42:43  meichel
 *  Headers libc.h and unistd.h are now included via ofstdinc.h
 *
 *  Revision 1.5  2003/12/05 10:39:45  joergr
 *  Removed leading underscore characters from preprocessor symbols (reserved
 *  symbols).
 *
 *  Revision 1.4  2003/06/12 15:16:10  joergr
 *  Fixed inconsistent API documentation reported by Doxygen.
 *
 *  Revision 1.3  2001/06/01 15:50:04  meichel
 *  Updated copyright header
 *
 *  Revision 1.2  2000/10/10 12:06:53  meichel
 *  Updated transport layer error codes and routines for printing
 *    connection parameters.
 *
 *  Revision 1.1  2000/08/10 14:50:52  meichel
 *  Added initial OpenSSL support.
 *
 *
 */

