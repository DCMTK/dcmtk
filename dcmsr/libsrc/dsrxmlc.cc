/*
 *
 *  Copyright (C) 2003-2014, OFFIS e.V.
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
