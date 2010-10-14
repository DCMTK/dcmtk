/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomLookupTable (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:26 $
 *  CVS/RCS Revision: $Revision: 1.26 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DILUPTAB_H
#define DILUPTAB_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmdata/dctagkey.h"

#include "dcmtk/dcmimgle/dibaslut.h"
#include "dcmtk/dcmimgle/diobjcou.h"


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
     *  @param  descripMode  mode specifying the use of the bits per table entry value
     *  @param  status       pointer to image status variable (optional)
     */
    DiLookupTable(const DiDocument *docu,
                  const DcmTagKey &descriptor,
                  const DcmTagKey &data,
                  const DcmTagKey &explanation,
                  const EL_BitsPerTableEntry descripMode = ELM_UseValue,
                  EI_Status *status = NULL);

    /** constructor
     *
     ** @param  docu         pointer to dataset (encapsulated)
     *  @param  sequence     tag key containing the LUT sequence
     *  @param  descriptor   tag key containing the LUT descriptor
     *  @param  data         tag key containing the LUT data
     *  @param  explanation  tag key containing the LUT explanation
     *  @param  descripMode  mode specifying the use of the bits per table entry value
     *  @param  pos          position in the LUT sequence which should be used (optional)
     *  @param  card         pointer to storage area where the number of sequence entries
     *                       should be stored (optional)
     */
    DiLookupTable(const DiDocument *docu,
                  const DcmTagKey &sequence,
                  const DcmTagKey &descriptor,
                  const DcmTagKey &data,
                  const DcmTagKey &explanation,
                  const EL_BitsPerTableEntry descripMode = ELM_UseValue,
                  const unsigned long pos = 0,
                  unsigned long *card = NULL);

    /** constructor
     *
     ** @param  data         element containing the LUT data
     *  @param  descriptor   element containing the LUT descriptor
     *  @param  explanation  element containing the LUT explanation (optional)
     *  @param  descripMode  mode specifying the use of the bits per table entry value
     *  @param  first        expected value for "first input value mapped" (optional)
     *  @param  status       pointer to image status variable (optional)
     */
    DiLookupTable(const DcmUnsignedShort &data,
                  const DcmUnsignedShort &descriptor,
                  const DcmLongString *explanation = NULL,
                  const EL_BitsPerTableEntry descripMode = ELM_UseValue,
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
     *  @param  descripMode  mode specifying the use of the bits per table entry value
     *  @param  status       pointer to image status variable (optional)
     */
    void Init(const DiDocument *docu,
              DcmObject *obj,
              const DcmTagKey &descriptor,
              const DcmTagKey &data,
              const DcmTagKey &explanation,
              const EL_BitsPerTableEntry descripMode = ELM_UseValue,
              EI_Status *status = NULL);

    /** check (and possibly correct) lookup table for consistency
     *
     ** @param  count        number of LUT entries
     *  @param  bits         bits per LUT entry
     *  @param  descripMode  mode specifying the use of the bits per table entry value
     *  @param  status       pointer to image status variable (optional)
     */
    void checkTable(unsigned long count,
                    Uint16 bits,
                    const EL_BitsPerTableEntry descripMode = ELM_UseValue,
                    EI_Status *status = NULL);

    /** check (and possibly correct) value for bits per LUT entry.
     *  Be very lenient with the value stored in the LUT descriptor and
     *  therefore perform some addition (heuristical tests) to assume the
     *  most probable value.
     *
     ** @param  bits         actual value for bits per entry
     *  @param  rightBits    right value (8 or 16)
     *  @param  wrongBits    wrong value (8 or 16)
     *  @param  descripMode  mode specifying the use of the bits per table entry value
     */
    void checkBits(const Uint16 bits,
                   const Uint16 rightBits,
                   const Uint16 wrongBits = 0,
                   const EL_BitsPerTableEntry descripMode = ELM_UseValue);

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
 * Revision 1.26  2010-10-14 13:16:26  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.25  2007/03/16 11:56:06  joergr
 * Introduced new flag that allows to select how to handle the BitsPerTableEntry
 * value in the LUT descriptor (use, ignore or check).
 *
 * Revision 1.24  2005/12/08 16:47:45  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.23  2003/12/17 16:17:29  joergr
 * Added new compatibility flag that allows to ignore the third value of LUT
 * descriptors and to determine the bits per table entry automatically.
 *
 * Revision 1.22  2003/12/08 18:24:15  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated CVS header.
 *
 * Revision 1.21  2003/06/12 15:08:34  joergr
 * Fixed inconsistent API documentation reported by Doxygen.
 *
 * Revision 1.20  2002/12/09 13:32:51  joergr
 * Renamed parameter/local variable to avoid name clashes with global
 * declaration left and/or right (used for as iostream manipulators).
 *
 * Revision 1.19  2001/06/01 15:49:43  meichel
 * Updated copyright header
 *
 * Revision 1.18  2000/07/07 13:42:30  joergr
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
