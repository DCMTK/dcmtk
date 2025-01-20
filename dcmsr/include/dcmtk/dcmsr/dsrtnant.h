/*
 *
 *  Copyright (C) 2015-2024, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
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
 *    classes: DSRTreeNodeAnnotation
 *
 */


#ifndef DSRTNANT_H
#define DSRTNANT_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsdefine.h"
#include "dcmtk/dcmsr/dsrtypes.h"


// include this file in doxygen documentation

/** @file dsrtnant.h
 *  @brief definitions for class DSRTreeNodeAnnotation
 */


/** Base class for tree node annotations.
 *  Currently, this class supports textual annotations only.
 */
class DCMTK_DCMSR_EXPORT DSRTreeNodeAnnotation
{

  public:

    /** (default) constructor
     ** @param  text  optional character string that should be set as the initial value
     *                of the annotation text
     */
    DSRTreeNodeAnnotation(const OFString &text = OFString());

    /** destructor
     */
    virtual ~DSRTreeNodeAnnotation();

    /** conversion operator that returns the currently stored annotation text
     */
    operator const OFString &() const
    {
        return Text;
    }

    /** clear the currently stored annotation text
     */
    inline void clear()
    {
        Text.clear();
    }

    /** check whether annotations are "equal".
     *  Two annotations are equal, if the internally stored character strings (text)
     *  have the same value.
     ** @param  annotation  annotation that should be compared to the current one
     ** @return OFTrue if both annotations are equal, OFFalse otherwise
     */
    OFBool isEqual(const DSRTreeNodeAnnotation &annotation) const
    {
        return (Text == annotation.Text);
    }

    /** check whether annotations are "not equal".
     *  Two annotations are not equal, if the internally stored character strings
     *  (text) have different values.
     ** @param  annotation  annotation that should be compared to the current one
     ** @return OFTrue if both annotations are not equal, OFFalse otherwise
     */
    OFBool isNotEqual(const DSRTreeNodeAnnotation &annotation) const
    {
        return (Text != annotation.Text);
    }

    /** check whether the annotation is empty, i.e.\ whether the internally stored
     *  character string (text) has an empty value
     ** @return OFTrue if the annotation is empty, OFFalse otherwise
     */
    inline OFBool isEmpty() const
    {
        return Text.empty();
    }

    /** get annotation text, i.e.\ the internally stored character string
     ** @return internally stored character string (text), might be empty
     */
    inline const OFString &getText() const
    {
        return Text;
    }

    /** set annotation text, i.e.\ the internally stored character string
     ** @param  text  character string that should be set as the current value of the
     *                annotation text.  Use an empty string ("") to delete the value.
     */
    inline void setText(const OFString &text)
    {
        Text = text;
    }


  private:

    /// internally stored character strings (text)
    OFString Text;
};


/*------------------------*
 *  comparison operators  *
 *------------------------*/

/** equality operator.
 *  Internally, the DSRTreeNodeAnnotation::isEqual() method is used.
 *  @param  lhs  left-hand side
 *  @param  rhs  right-hand side
 *  @return OFTrue if 'lhs' and 'rhs' are equal, OFFalse otherwise
 */
DCMTK_DCMSR_EXPORT OFBool operator==(const DSRTreeNodeAnnotation &lhs,
                                     const DSRTreeNodeAnnotation &rhs);


/** inequality operator.
 *  Internally, the DSRTreeNodeAnnotation::isNotEqual() method is used.
 *  @param  lhs  left-hand side
 *  @param  rhs  right-hand side
 *  @return OFTrue if 'lhs' and 'rhs' are not equal, OFFalse otherwise
 */
DCMTK_DCMSR_EXPORT OFBool operator!=(const DSRTreeNodeAnnotation &lhs,
                                     const DSRTreeNodeAnnotation &rhs);


#endif
