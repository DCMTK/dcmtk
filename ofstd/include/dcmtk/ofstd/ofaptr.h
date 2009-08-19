/*
 *
 *  Copyright (C) 2009, OFFIS
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
 *  Module:  ofstd
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: Template class for automatically deleting pointers when they go out
 *           of scope.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2009-08-19 10:50:02 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/include/dcmtk/ofstd/ofaptr.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef OFAPTR_H
#define OFAPTR_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftypes.h"


/** a simple class which makes sure a pointer is deleted once it goes out of
 *  scope. This class does NOT do reference counting!
 *  This is a reimplementation of std::auto_ptr.
 */
template <class T> class OFauto_ptr
{
  protected:
    /// the pointer that it's all about
    T *ptr;

  public:
    /** constructs a OFauto_ptr from the given pointer.
     *  You must not call delete on a pointer that is managed by OFauto_ptr!
     *  @param p the pointer
     */
    explicit OFauto_ptr(T* p = NULL)
    {
        this->ptr = p;
    }

    /** constructs a OFauto_ptr from another OFauto_ptr. The other OFauto_ptr
     *  will be invalidated. This means he will point to NULL from now on!.
     *  @param other the OFauto_ptr to get the pointer from.
     */
    template <class X>
    OFauto_ptr(OFauto_ptr<X>& other)
    {
        this->ptr = other.release();
    }

    /** destructor. Destroys the pointer that is managed by this instance,
     *  if there is any.
     */
    ~OFauto_ptr()
    {
        reset();
    }

    /** assign another pointer to this OFauto_ptr, possibly deleting our
     *  current pointer.
     *  @param p the new pointer
     */
    void reset(T* p = NULL)
    {
        if (this->ptr)
            delete this->ptr;
        this->ptr = p;
    }

    /** get the pointer that is managed by this class.
     *  @return the pointer
     */
    T* get() const { return this->ptr; }

    /** get the pointer that is managed by this class.
     *  @return the pointer
     */
    T* operator->() const { return get(); }

    /** get the pointer that is managed by this class.
     *  @return the pointer
     */
    T& operator*() const { return *get(); }

    /** assigns the other pointer to this OFauto_ptr
     *  @param other other Ofauto_ptr which will be invalidated
     *  @return reference to this object
     */
    template <class X>
    OFauto_ptr& operator=(OFauto_ptr<X>& other)
    {
        *this = other.release();
        return *this;
    }

    /** assigns the other pointer to this OFauto_ptr
     *  @param other our new pointer
     *  @return reference to this object
     */
    template <class X>
    OFauto_ptr& operator=(X* p)
    {
        reset(p);
        return *this;
    }

    /** release the pointer that is managed by this class. You are now
     *  responsible for destroying that pointer yourself!
     *  @return the pointer
     */
    T* release()
    {
        T* tmp = this->ptr;
        this->ptr = NULL;
        return tmp;
    }
};

#endif


/*
** CVS/RCS Log:
** $Log: ofaptr.h,v $
** Revision 1.1  2009-08-19 10:50:02  joergr
** Added new class OFauto_ptr required for upcoming module "oflog".
**
**
*/
