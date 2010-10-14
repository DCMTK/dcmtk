/*
 *
 *  Copyright (C) 2003-2010, OFFIS e.V.
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
 *    classes: DSRXMLCursor
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:33 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DSRXMLC_H
#define DSRXMLC_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftypes.h"    /* for definition of OFBool */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDDEF
#include "dcmtk/ofstd/ofstdinc.h"

#ifdef WITH_LIBXML
#include <libxml/parser.h>
#endif


/*--------------------*
 *  type definitions  *
 *--------------------*/

#ifndef WITH_LIBXML
// define pointer type if 'libxml' absent
typedef void (*xmlNodePtr);
#endif


/*-----------------------*
 *  forward declaration  *
 *-----------------------*/

class DSRXMLDocument;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for XML document cursors.
 *  This class encapsulates the access to the 'libxml' node pointer and,
 *  therefore, allows to replace the XML library with little effort (if
 *  required).
 */
class DSRXMLCursor
{

    // allow DSRXMLDocument to access private member variable 'Node' directly
    friend class DSRXMLDocument;


  public:

    /** default constructor
     */
    DSRXMLCursor();

    /** copy constructor
     ** @param cursor cursor object to be copied
     */
    DSRXMLCursor(const DSRXMLCursor &cursor);

    /** destructor
     */
    virtual ~DSRXMLCursor();

    /** assignment operator
     ** @param  cursor  cursor object to be copied
     ** @return reference to this cursor object after 'cursor' has been copied
     */
    DSRXMLCursor &operator=(const DSRXMLCursor &cursor);

    /** check whether cursor is valid.
     *  Valid means that the cursor points to a valid XML node.
     ** @return OFTrue if valid, OFFalse otherwise
     */
    inline OFBool valid() const
    {
        return (Node != NULL);
    }

    /** set cursor to next XML node (same level).
     *  Blank (empty or whitespace only) nodes are ignored/skipped.
     ** @return reference to this cursor object (might be invalid)
     */
    DSRXMLCursor &gotoNext();

    /** set cursor to first XML child node (next lower level).
     *  Blank (empty or whitespace only) nodes are ignored/skipped.
     ** @return reference to this cursor object (might be invalid)
     */
    DSRXMLCursor &gotoChild();

    /** get cursor pointing to next XML node (same level).
     *  This cursor object is not modified.
     ** @return copy of the requested cursor object (might be invalid)
     */
    DSRXMLCursor getNext() const;

    /** get cursor pointing to first XML child node (next lower level).
     *  This cursor object is not modified.
     ** @return copy of the requested cursor object (might be invalid)
     */
    DSRXMLCursor getChild() const;


  private:

    /// pointer to the associated 'libxml' node
    xmlNodePtr Node;
};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dsrxmlc.h,v $
 *  Revision 1.7  2010-10-14 13:16:33  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.6  2005-12-08 16:05:35  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.5  2003/10/06 09:51:43  joergr
 *  Replaced wrong newline character sequence.
 *
 *  Revision 1.4  2003/09/04 10:14:30  joergr
 *  Combined two #include "ofstdinc.h" statements.
 *
 *  Revision 1.3  2003/09/03 16:00:11  meichel
 *  Added standard includes needed on Win32
 *
 *  Revision 1.2  2003/08/29 12:52:02  joergr
 *  Replaced inclusion of unistd.h by cstddef/stddef.h to compile under Windows
 *  with libxml support (required for definition of NULL).
 *
 *  Revision 1.1  2003/08/07 12:16:37  joergr
 *  Added interface classes hiding the access to libxml (document and cursor
 *  class).
 *
 *
 */
