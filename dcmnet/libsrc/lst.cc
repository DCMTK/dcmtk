/*
          Copyright (C) 1993, RSNA and Washington University

          The software and supporting documentation for the Radiological
          Society of North America (RSNA) 1993 Digital Imaging and
          Communications in Medicine (DICOM) Demonstration were developed
          at the
                  Electronic Radiology Laboratory
                  Mallinckrodt Institute of Radiology
                  Washington University School of Medicine
                  510 S. Kingshighway Blvd.
                  St. Louis, MO 63110
          as part of the 1993 DICOM Central Test Node project for, and
          under contract with, the Radiological Society of North America.

          THIS SOFTWARE IS MADE AVAILABLE, AS IS, AND NEITHER RSNA NOR
          WASHINGTON UNIVERSITY MAKE ANY WARRANTY ABOUT THE SOFTWARE, ITS
          PERFORMANCE, ITS MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR
          USE, FREEDOM FROM ANY COMPUTER DISEASES OR ITS CONFORMITY TO ANY
          SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND PERFORMANCE OF
          THE SOFTWARE IS WITH THE USER.

          Copyright of the software and supporting documentation is
          jointly owned by RSNA and Washington University, and free access
          is hereby granted as a license to use this software, copy this
          software and prepare derivative works based upon this software.
          However, any distribution of this software source code or
          supporting documentation or derivative works (source code and
          supporting documentation) must include the three paragraphs of
          the copyright notice.
*/
/*          lst.c
**
**  LST package for implementing atomic functions on linked lists.
**  Author:
**  Thomas R. Leith (trl@wuerl.wustl.edu)
**  Electronic Radiology Laboratory
**  Mallinckrodt Institute of Radiology
**  Washington University School of Medicine
**
**  Modifications:
**  Stephen Moore (smm@wuerl.wustl.edu) 26-May-1992
**  Changed function names to match current conventions and
**  modified functions to directly return CONDITION codes.
**
**  TR Leith (trl@wuerl.wustl.edu) 8-July-1992
**  Changed LST_Insert semantics to make the newly-inserted node
**  current.
**
*/


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#ifdef HAVE_STDLIB_H
#ifndef  _BCB4
/* workaround for bug in Borland C++ Builder 4 */
BEGIN_EXTERN_C
#endif
#include <stdlib.h>
#ifndef  _BCB4
END_EXTERN_C
#endif
#endif

#include <string.h>
#ifdef HAVE_STDDEF_H
#include <stddef.h>
#endif

#include "dicom.h"
#include "lstpriv.h"         /* Private definitions */
#include "lst.h"                /* Public definitions */

#define CURRENT  (*list)->current
#define OLD_NEXT (*list)->current->next
#define OLD_PREV (*list)->current->previous



LST_HEAD       *LST_Create(void)
/*
**  This module creates a new list head and returns your handle to it.
**
*/
{
    LST_HEAD
    * ptr;

    ptr = (LST_HEAD*)malloc(sizeof(LST_HEAD));
    if (ptr == NULL)
        return NULL;

    ptr->head = NULL;
    ptr->tail = NULL;
    ptr->current = NULL;
    ptr->count = 0;
    return ptr;
}



CONDITION       LST_Destroy(LST_HEAD ** list)
/*
 *  This routine will destroy a list.  The list must be empty.
 *  The list handle is set to NULL as a side-effect.
 *
 */
{

    if ((*list)->count != 0)
        return LST_LISTNOTEMPTY;

    free(*list);
    *list = NULL;
    return LST_NORMAL;
}



CONDITION       LST_Enqueue(LST_HEAD ** list, LST_NODE * node)
/*
 *  Adds a new node to the tail of the list and returns
 *  status.
 *
 */
{
    node->next = NULL;                         /* no next node              */
    node->previous = (*list)->tail;            /* previous is old tail      */
    if ((*list)->head == NULL)                 /* if list was empty...      */
        (*list)->head = node;                  /* it has a head now!        */
    else
        (*list)->tail->next = node;            /* old tail now has a next   */

    (*list)->tail = node;                      /* list now has a new tail    */
    (*list)->count++;                          /* bump the counter           */
    return LST_NORMAL;
}

CONDITION       LST_Push(LST_HEAD ** list, LST_NODE * node)
/*
 *  Adds a new node to the head of the list and returns
 *  status.
 *
 */

{
    node->next = (*list)->head;                /* set the forward link      */
    node->previous = NULL;                     /* set rearward link         */
    if ((*list)->tail == NULL)                 /* if the list was empty     */
        (*list)->tail = node;                  /* set the tail pointer      */
    else                                       /* otherwise,                */
        (*list)->head->previous = node;        /* old head now has a
                                                  previous                  */

    (*list)->head = node;                      /* set new first node        */
    (*list)->count++;                          /* bump the counter          */
    return LST_NORMAL;

}

LST_NODE       *LST_Dequeue(LST_HEAD ** list)
/*
 *  Removes a node from the head of the list and returns
 *  a pointer to it.
 *
 */
{
    LST_NODE
    * ptr;

    if ((*list)->head == NULL) {               /* list is empty             */
        (*list)->count = 0;
        return NULL;
    }
    ptr = (*list)->head;                       /* save the head             */
    (*list)->head = ptr->next;                 /* set new head of list      */
    if ((*list)->head == NULL)                 /* if the list is now empty  */
        (*list)->tail = NULL;                  /* there is no tail anymore  */
    else
        (*list)->head->previous = NULL;        /* new head has no previous  */
    ptr->next = NULL;                          /* hide data from user       */
    (*list)->count--;                          /* list has one fewer node   */
                                               /* now                       */
    return ptr;
}



LST_NODE       *LST_Pop(LST_HEAD ** list)
/*
 *  Removes a node from the head of the list and returns
 *  a pointer to it.
 *
 */
{
    LST_NODE
    * ptr;

    if ((*list)->head == NULL) {               /* list is empty             */
        (*list)->count = 0;
        return NULL;
    }
    ptr = (*list)->head;                       /* save the head             */
    (*list)->head = ptr->next;                 /* set new head of list      */
    if ((*list)->head == NULL)                 /* if the list is now empty  */
        (*list)->tail = NULL;                  /* there is no tail anymore  */
    else
        (*list)->head->previous = NULL;        /* new head has no previous  */
    ptr->next = NULL;                          /* hide data from user       */
    (*list)->count--;                          /* list has one fewer node   */
                                               /* now                       */
    return ptr;
}



unsigned long   LST_Count(LST_HEAD ** list)
/*
 *  Returns the number of nodes in the list.
 *
 */
{
    return (*list)->count;
}



LST_NODE       *LST_Head(LST_HEAD ** list)
/*
 *  Returns a pointer to the node at the head of the list.
 *  It does NOT remove the node from the list.
 *
 */
{
    return (*list)->head;
}


LST_NODE       *LST_Current(LST_HEAD ** list)
/*
 *  Returns a pointer to the current node.
 *  It does NOT remove the node from the list.
 *
 */
{
    return (*list)->current;
}



LST_NODE       *LST_Tail(LST_HEAD ** list)
/*
 *  Returns a pointer to the node at the tail of the list.
 *  It does NOT remove the node from the list.
 *
 */
{
    return (*list)->tail;
}


CONDITION       LST_Insert(LST_HEAD ** list, LST_NODE * node, LST_END where)
/*
**  Inserts a new node in the list.  User selects whether to insert closer
**  the HEAD end, or the TAIL end.  If the list is empty, the distinction is
**  moot.  In any case, CURRENT is set to the newly-inserted node.  In the 
**  case of an error, the list is unchanged.
**/

{
    if ((where != LST_K_BEFORE) && (where != LST_K_AFTER))
        goto badend;

    if ((*list)->head == NULL) {               /* if the list was empty     */
        (*list)->tail = node;                  /* set the tail pointer      */
        (*list)->head = node;                  /* set the head pointer      */
        (*list)->count = 0;                    /* will get bumped later...  */
        (node)->next = NULL;                   /* there is no next          */
        (node)->previous = NULL;               /* and no previous           */

    } else if (CURRENT == NULL)				   /* is he mixing semantics?	*/
        goto nocurrent;

    else if ((CURRENT == (*list)->head) &&      /* if at the head           */
             (where == LST_K_BEFORE)) {         /*   and inserting BEFORE   */
        node->next = CURRENT;                   /* splice new node in       */
        CURRENT->previous = node;               /*   before the current     */
        node->previous = NULL;                  /* new one has no previous  */
        (*list)->head = node;                   /* new one is first now     */

    } else if ((CURRENT == (*list)->tail) &&    /* if at the tail           */
               (where == LST_K_AFTER)) {        /*   and inserting AFTER    */
        node->next = NULL;                      /* new node has no next     */
        node->previous = (*list)->tail;         /* previous is old tail     */
        CURRENT->next = node;                   /* splice new node in       */
        (*list)->tail = node;                   /* new node is now the tail */

    } else if (where == LST_K_AFTER) {          /* not a special case       */
        OLD_NEXT->previous = node;              /* we preceed a node        */
        node->next = OLD_NEXT;                  /* the old next follows us  */
        node->previous = CURRENT;               /* the current preceeds us  */
        CURRENT->next = node;                   /* we follow current        */

    } else {                                    /* not a special case       */
        OLD_PREV->next = node;                  /* we follow the previous   */
        node->previous = OLD_PREV;              /*    of current            */
        node->next = CURRENT;                   /* current follows us and   */
        CURRENT->previous = node;               /*   we preceed current     */
    };

    (*list)->count++;                          /* bump the counter          */
	(*list)->current = node;				   /*    and set current        */
    return LST_NORMAL;

  badend:
    return LST_BADEND;

  nocurrent:
    return LST_NOCURRENT;
}



LST_NODE       *LST_Remove(LST_HEAD ** list, LST_END dir)
/*
**  Removes the current node from the list and returns a pointer to it.
**  How CURRENT gets set depends on which way the DIR argument points.  If
**  DIR is LST_K_BEFORE, CURRENT will move towards the tail-end of the
**  list.  If DIR is LST_K_AFTER, CURRENT will move towards the head-end of
**  the list.  If there is no node in the direction of DIR, CURRENT becomes
**  undefined.
**
**/
{
    LST_NODE
    * ptr;

    if ((dir != LST_K_BEFORE) && (dir != LST_K_AFTER))
        goto baddir;
    if (CURRENT == NULL)
        goto nocurrent;
    if ((*list)->head == NULL)
        goto listempty;

    ptr = CURRENT;                             /* save node                 */

    if (CURRENT == (*list)->head) {            /* removing the head         */
        (*list)->head = OLD_NEXT;              /* set new head of list      */
        if ((*list)->head == NULL)             /* if the list is now empty  */
            (*list)->tail = NULL;              /* no tail anymore either    */
        else
            (*list)->head->previous = NULL;    /* new head has no previous  */
        if (dir == LST_K_BEFORE)               /* there is nothing before   */
            (*list)->current = NULL;           /* the head of the list      */
        else                                   /* otherwise, remain         */
            (*list)->current = (*list)->head;  /*    at the head...         */

    } else if (CURRENT == (*list)->tail) {     /* removing the tail         */
        (*list)->tail = OLD_PREV;              /* set new tail of list      */
        (*list)->tail->next = NULL;            /* new tail has no next      */
        if (dir == LST_K_AFTER)                /* there is nothing after    */
            (*list)->current = NULL;           /* the tail of a list        */
        else                                   /* otherwise, remain         */
            (*list)->current = (*list)->tail;  /* at the tail...            */

    } else {                                   /* not a special case        */
        OLD_PREV->next = CURRENT->next;        /* set forward pointer       */
        OLD_NEXT->previous = CURRENT->previous;/* set backward pointer      */
        if (dir == LST_K_BEFORE)               /* depending on direction,   */
            (*list)->current = CURRENT->previous;/* set current             */
        else                                   /* in the                    */
            (*list)->current = CURRENT->next;  /* list head                 */
    }

    (*list)->count--;                          /* one fewer nodes now       */
    ptr->previous = NULL;                      /* hide data from user       */
    ptr->next = NULL;                          /* hide data from user       */
    return ptr;

  baddir:
    return NULL;

  nocurrent:
    return NULL;

  listempty:
    (*list)->count = 0;
    (*list)->current = NULL;
    (*list)->head = (*list)->tail = NULL;
    return NULL;
}



LST_NODE       *LST_Next(LST_HEAD ** list)
/*
 *  Returns a pointer to the next node in the list and
 *  makes it current.
 *
 */
{
    if ((*list)->head == NULL) {                /* list is empty            */
        (*list)->count = 0;
        return NULL;
    }
    if (CURRENT == NULL) {                      /* there is no CURRENT      */
        return NULL;
    }
    if (CURRENT->next == NULL) {               /* there is no NEXT          */
        return NULL;
    }
    CURRENT = CURRENT->next;                   /* found it                  */
    return CURRENT;
}



LST_NODE       *LST_Previous(LST_HEAD ** list)
/*
 *  Returns a pointer to the previous node in the list and
 *  makes it current.
 *
 */
{
    if ((*list)->head == NULL) {               /* list is empty     */
        (*list)->count = 0;
        return NULL;
    }
    if (CURRENT == NULL) {                     /* there is no CURRENT       */
        return NULL;
    }
    if (CURRENT->previous == NULL) {           /* no PREVIOUS               */
        return NULL;
    }
    CURRENT = CURRENT->previous;               /* found it                  */
    return CURRENT;
}



LST_NODE       *LST_Position(LST_HEAD ** list, LST_NODE * node)
/*
 *  Make a node current and return the argument.
 *
 *
 *  Notes:  node = lst_position(list, lst_head(list));
 *          makes the node at the head of the list current
 *          and returns a pointer to it.
 *
 *      The routine tries to verify that "node" is in the list
 *      by doing a few consistency checks.  It assumes that if
 *      any of three "known" pointers are what they should be
 *      that all is well.  Its not damnfoolproof, but...
 */
{
    if ((*list)->head == NULL) {               /* list is empty     */
        return NULL;
    }
    if ((node->previous == NULL) && ((*list)->head == node) ||
        (node->next == NULL) && ((*list)->tail == node) ||
        (node->previous->next == node)) {      /* its probably OK       */

        CURRENT = node;
        return CURRENT;
    };

    return NULL;
}

