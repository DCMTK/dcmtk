/*
 *
 *  Copyright (C) 1996-2000, OFFIS
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomLookupTable (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-07-07 13:42:30 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimgle/include/Attic/diluptab.h,v $
 *  CVS/RCS Revision: $Revision: 1.18 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DILUPTAB_H
#define __DILUPTAB_H

#include "osconfig.h"
#include "dctagkey.h"

#include "dibaslut.h"
#include "diobjcou.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DcmObject;
class DcmUnsignedShort;
class DcmLongString;
class DiDocument;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to handle color palette look-up tables
 */
class DiLookupTable
  : public DiBaseLUT,
    public DiObjectCounter
{

 public:

    /** constructor
     *
     ** @param  docu         pointer to dataset (encapsulated)
     *  @param  descriptor   tag key containing the LUT descriptor
     *  @param  data         tag key containing the LUT data
     *  @param  explanation  tag key containing the LUT explanation
     *  @param  status       pointer to image status variable (optional)
     */
    DiLookupTable(const DiDocument *docu,
                  const DcmTagKey &descriptor,
                  const DcmTagKey &data,
                  const DcmTagKey &explanation,
                  EI_Status *status = NULL);

    /** constructor
     *
     ** @param  docu         pointer to dataset (encapsulated)
     *  @param  sequence     tag key containing the LUT sequence
     *  @param  descriptor   tag key containing the LUT descriptor
     *  @param  data         tag key containing the LUT data
     *  @param  explanation  tag key containing the LUT explanation
     *  @param  pos          position in the LUT sequence which should be used (optional)
     *  @param  card         pointer to storage area where the number of sequence entries
     *                       should be stored (optional)
     */
    DiLookupTable(const DiDocument *docu,
                  const DcmTagKey &sequence,
                  const DcmTagKey &decriptor,
                  const DcmTagKey &data,
                  const DcmTagKey &explanation,
                  const unsigned long pos = 0,
                  unsigned long *card = NULL);

    /** constructor
     *
     ** @param  data         element containing the LUT data
     *  @param  descriptor   element containing the LUT descriptor
     *  @param  explanation  element containing the LUT explanation (optional)
     *  @param  first        expected value for "first input value mapped" (optional)
     *  @param  status       pointer to image status variable (optional)
     */
    DiLookupTable(const DcmUnsignedShort &data,
                  const DcmUnsignedShort &descriptor,
                  const DcmLongString *explanation = NULL,
                  const signed long first = -1,
                  EI_Status *status = NULL);

    /** constructor
     *
     ** @param  buffer  pointer to array with LUT entries
     *  @param  count   number of LUT entries
     *  @param  bits    number of bits per entry
     */
    DiLookupTable(Uint16 *buffer,
                  const Uint32 count,
                  const Uint16 bits);

    /** destructor
     */
    virtual ~DiLookupTable();

    /** invert all LUT values.
     *  (new_value = max_value - old_value, e.g. used for presentation LUTs)
     *
     ** @param  flag  if bit 0 is set (0x1, default) the internal copy of the LUT data is modified
     *                   or a copy of the original data is created (if not already existing),
     *                if bit 1 is set (0x2) the original LUT data is modified,
     *                a combination of both modes is also supported.
     *
     ** @return status true if successful (0x1 if internal data modified, 0x2 if original data, 0x3 if both),
     *                 false otherwise
     */
    int invertTable(const int flag = 0x1);

    /** mirror the order of all LUT entries.
    *   i.e. the last one becomes the first etc.
     *  (value[i] = value[last - i], e.g. used for presentation LUTs)
     *
     ** @param  flag  if bit 0 is set (0x1, default) the internal copy of the LUT data is modified
     *                   or a copy of the original data is created (if not already existing),
     *                if bit 1 is set (0x2) the original LUT data is modified,
     *                a combination of both modes is also supported.
     *
     ** @return status true if successful (0x1 if internal data modified, 0x2 if original data, 0x3 if both),
     *                 false otherwise
     */
    int mirrorTable(const int flag = 0x1);

    /** create an inverse copy of the current LUT.
     *  (input values become output values and vice versa)
     *  This function is used for DICOM print (mainly 8<->12 bit).
     *
     ** @return pointer to inverse LUT
     */
    DiLookupTable *createInverseLUT() const;

    /** compares current LUT with specified LUT
     *
     ** @param  data        element containing the LUT data
     *  @param  descriptor  element containing the LUT descriptor
     *
     ** @return true if LUTs are not equal (1 = invalid LUT / memory error,
     *                                      2 = descriptor differs,
     *                                      3 = data differs)
     *          false (0) otherwise
     */
    int compareLUT(const DcmUnsignedShort &data,
                   const DcmUnsignedShort &descriptor);


    /** compares current LUT with specified LUT
     *
     ** @param  lut  LUT to be compared with the current one
     *
     ** @return OFTrue if LUTs are equal, OFFalse otherwise
     */
    virtual OFBool operator==(const DiBaseLUT &lut);

    /** compares current LUT with specified LUT
     *
     ** @param  lut  LUT to be compared with the current one
     *
     ** @return OFTrue if LUTs are equal, OFFalse otherwise
     */
    virtual OFBool operator==(const DiLookupTable &lut);


 protected:

    /** initialize lookup table
     *
     ** @param  docu         pointer to dataset (encapsulated)
     *  @param  obj          pointer to item in dataset where to start (maybe NULL)
     *  @param  descriptor   tag key containing the LUT descriptor
     *  @param  data         tag key containing the LUT data
     *  @param  explanation  tag key containing the LUT explanation
     *  @param  status       pointer to image status variable (optional)
     */
    void Init(const DiDocument *docu,
              DcmObject *obj,
              const DcmTagKey &descriptor,
              const DcmTagKey &data,
              const DcmTagKey &explanation,
              EI_Status *status = NULL);

    /** check (and possibly correct) lookup table for consistency
     *
     ** @param  count   number of LUT entries
     *  @param  bits    bits per LUT entry
     *  @param  status  pointer to image status variable (optional)
     */
    void checkTable(unsigned long count,
                    Uint16 bits,
                    EI_Status *status = NULL);

    /** check (and possibly correct) value for bits per LUT entry.
     *  Be very lenient with the value stored in the LUT descriptor and
     *  therefore perform some addition (heuristical tests) to assume the
     *  most probable value.
     *
     ** @param  bits   actual value for bits per entry
     *  @param  right  right value (8 or 16)
     *  @param  wrong  wrong value (8 or 16)
     */
    void checkBits(const Uint16 bits,
                   const Uint16 right,
                   const Uint16 wrong = 0);

 private:

    /// number of bits originally allocated for a LUT entry
    int OriginalBitsAllocated;
    /// pointer to original data buffer (where the LUT data is created from)
    void *OriginalData;

 // --- declarations to avoid compiler warnings

    DiLookupTable(const DiLookupTable &);
    DiLookupTable &operator=(const DiLookupTable &);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diluptab.h,v $
 * Revision 1.18  2000-07-07 13:42:30  joergr
 * Added support for LIN OD presentation LUT shape.
 *
 * Revision 1.17  2000/03/08 16:24:17  meichel
 * Updated copyright header.
 *
 * Revision 1.16  2000/03/06 18:19:36  joergr
 * Moved get-method to base class, renamed method and made method virtual to
 * avoid hiding of methods (reported by Sun CC 4.2).
 *
 * Revision 1.15  1999/11/24 11:13:46  joergr
 * Added method to mirror order of entries in look-up tables.
 * Enhanced comments for methods "inverting" the LUT values/entries.
 *
 * Revision 1.14  1999/11/03 12:52:08  joergr
 * Added copy constructor and assignment operator to avoid compiler warnings.
 *
 * Revision 1.13  1999/10/20 10:34:44  joergr
 * Enhanced method invertTable to distinguish between copy of LUT data and
 * original (referenced) LUT data.
 *
 * Revision 1.12  1999/09/30 11:37:09  joergr
 * Added methods to compare two lookup tables.
 *
 * Revision 1.11  1999/09/17 12:22:53  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 *
 * Revision 1.10  1999/09/08 15:19:24  joergr
 * Completed implementation of setting inverse presentation LUT as needed
 * e.g. for DICOM print (invert 8->12 bits PLUT).
 *
 * Revision 1.9  1999/07/23 14:01:49  joergr
 * Added dummy method (no implementation yet) to create inverse LUTs.
 *
 * Revision 1.8  1999/05/03 11:09:28  joergr
 * Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.7  1999/03/24 17:20:04  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.6  1999/02/03 17:27:23  joergr
 * Added base class for look-up tables (moved main functionality of class
 * DiLookupTable to DiBaseLUT).
 *
 * Revision 1.5  1998/12/23 11:34:34  joergr
 * Changed behaviour of getLabel/Description/Explanation() methods: return
 * NULL if string empty, no empty string "".
 *
 * Revision 1.4  1998/12/22 14:24:36  joergr
 * Changed parameter type.
 * Added method getAbsMaxRange.
 *
 * Revision 1.3  1998/12/16 16:31:30  joergr
 * Added explanation string to LUT class (retrieved from dataset).
 *
 * Revision 1.2  1998/12/14 17:19:19  joergr
 * Added support for signed values as second entry in look-up tables
 * (= first value mapped).
 *
 * Revision 1.1  1998/11/27 15:10:21  joergr
 * Added copyright message.
 * Added support of object counter class.
 * Added constructors to use external modality transformations.
 * Added methods to support presentation LUTs and shapes.
 *
 * Revision 1.3  1998/07/01 08:39:22  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.2  1998/05/11 14:53:18  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
