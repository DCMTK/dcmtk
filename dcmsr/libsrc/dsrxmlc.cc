/*
 *
 *  Copyright (C) 2003, OFFIS
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
 *  Module:  dcmsr
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DSRXMLCursor
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2005-12-08 15:48:24 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrxmlc.h"


DSRXMLCursor::DSRXMLCursor()
  : Node(NULL)
{
}


DSRXMLCursor::DSRXMLCursor(const DSRXMLCursor &cursor)
  : Node(cursor.Node)
{
}


DSRXMLCursor::~DSRXMLCursor()
{
}


DSRXMLCursor &DSRXMLCursor::operator=(const DSRXMLCursor &cursor)
{
    Node = cursor.Node;
    return *this;
}


DSRXMLCursor &DSRXMLCursor::gotoNext()
{
#ifdef WITH_LIBXML
    if (Node != NULL)
    {
        Node = Node->next;
        /* ignore blank (empty or whitespace only) nodes */
        while ((Node != NULL) && xmlIsBlankNode(Node))
            Node = Node->next;
    }
#endif
    return *this;
}


DSRXMLCursor &DSRXMLCursor::gotoChild()
{
#ifdef WITH_LIBXML
    if (Node != NULL)
    {
        Node = Node->xmlChildrenNode;
        /* ignore blank (empty or whitespace only) nodes */
        while ((Node != NULL) && xmlIsBlankNode(Node))
            Node = Node->next;
    }
#endif
    return *this;
}


DSRXMLCursor DSRXMLCursor::getNext() const
{
    DSRXMLCursor cursor;
#ifdef WITH_LIBXML
    if (Node != NULL)
    {
        cursor.Node = Node->next;
        /* ignore blank (empty or whitespace only) nodes */
        while ((cursor.Node != NULL) && xmlIsBlankNode(cursor.Node))
            cursor.Node = cursor.Node->next;
    }
#endif
    return cursor;
}


DSRXMLCursor DSRXMLCursor::getChild() const
{
    DSRXMLCursor cursor;
#ifdef WITH_LIBXML
    if (Node != NULL)
    {
        cursor.Node = Node->xmlChildrenNode;
        /* ignore blank (empty or whitespace only) nodes */
        while ((cursor.Node != NULL) && xmlIsBlankNode(cursor.Node))
            cursor.Node = cursor.Node->next;
    }
#endif
    return cursor;
}


/*
 *  CVS/RCS Log:
 *  $Log: dsrxmlc.cc,v $
 *  Revision 1.2  2005-12-08 15:48:24  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.1  2003/08/07 14:49:36  joergr
 *  Added interface classes hiding the access to libxml (document and cursor
 *  class).
 *
 *
 */
