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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg, Joerg Riesmeier
 *
 *  Purpose:
 *    classes: DVPSHelper
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:36 $
 *  CVS/RCS Revision: $Revision: 1.9 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

 
#ifndef DVPSHLP_H
#define DVPSHLP_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */
#include "dcmtk/dcmdata/dcsequen.h"

class DcmFileFormat;

/** this class contains a collection of static helper methods.
 */
class DVPSHelper
{ 
 public:
    /** helper function which loads a DICOM file and returns a
     *  pointer to a DcmFileFormat object if loading succeeds.
     *  @param filename name of DICOM file to be loaded
     *  @param fileformat pointer to DcmFileFormat object passed back here
     *  @return EC_Normal upon success, an error code otherwise.
     */
    static OFCondition loadFileFormat(const char *filename, DcmFileFormat *&fileformat);

    /** helper function which saves a DICOM object to file.
     *  @param filename name of DICOM file to be created
     *  @param fileformat DICOM object to be saved
     *  @param explicitVR selects the transfer syntax to be written. 
     *    True selects Explicit VR Little Endian, False selects Implicit VR Little Endian.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    static OFCondition saveFileFormat(const char *filename,
                                      DcmFileFormat *fileformat,
                                      OFBool explicitVR);

    /** helper function that inserts a new element into a DICOM dataset.
     *  A new DICOM element of the type determined by the tag is created.
     *  The string value (if any) is assigned and the element is inserted
     *  into the dataset. Only tags corresponding to string VRs may be passed.
     *  @param item the dataset into which the new element is inserted
     *  @param tag the tag key of the new DICOM element, must have string VR.
     *  @param value the value to be inserted. If omitted, an empty element is created.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    static OFCondition putStringValue(DcmItem *item, DcmTagKey tag, const char *value=NULL);

    /** helper function that inserts a new element into a DICOM dataset.
     *  A new DICOM element of type "US" is created, the value is assigned 
     *  and the element is inserted into the dataset. 
     *  @param item the dataset into which the new element is inserted
     *  @param tag the tag key of the new DICOM element, must have "US" VR.
     *  @param value the value to be inserted.
     *  @return EC_Normal upon success, an error code otherwise.
     */
    static OFCondition putUint16Value(DcmItem *item, DcmTagKey tag, Uint16 value);

    /** helper function that cleans up pending processes under Unix.
     *  No function if used on Windows.
     */ 
    static void cleanChildren();

    /** helper function that writes the current date in DICOM format (YYYYMMDD)
     *  @param str current date is written to this string
     */
    static void currentDate(OFString &str);

    /** helper function that writes the current time in DICOM format (HHMMSS)
     *  @param str current time is written to this string
     */
    static void currentTime(OFString &str);

    /** assigns the given value to the given DICOM element if it is empty
     *  and the status is OK, returns new status.
     *  @param result status in/out
     *  @param a_name DICOM element to be set
     *  @param a_value new value, must not be NULL.
     */
    static void setDefault(OFCondition& result, DcmElement& a_name, const char *a_value);

    /** static helper method that checks whether the given sequence contains an
     *  item with a ReferencedSOPClassUID element that matches the given UID string.
     *  @param seq sequence to be searched, should be a PrintManagementCapabilitiesSequence.
     *  @param uid UID string
     *  @return OFTrue if found, OFFalse otherwise. Returns OFFalse if uid is NULL.
     */
    static OFBool haveReferencedUIDItem(DcmSequenceOfItems& seq, const char *uid);

    /** static helper method that adds an item to the given sequence. The item
     *  contains a ReferencedSOPClassUID element with the given UID string as value.
     *  @param seq sequence to be added to, should be a PrintManagementCapabilitiesSequence.
     *  @param uid UID string, must not be NULL
     *  @return EC_Normal if successful, an error code otherwise.
     */   
    static OFCondition addReferencedUIDItem(DcmSequenceOfItems& seq, const char *uid);

};


#endif


/*
 *  CVS/RCS Log:
 *  $Log: dvpshlp.h,v $
 *  Revision 1.9  2010-10-14 13:16:36  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.8  2009-11-24 14:12:57  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.7  2009-09-30 10:42:38  uli
 *  Make dcmpstat's include headers self-sufficient by including all
 *  needed headers directly and stop using dctk.h
 *
 *  Revision 1.6  2005-12-08 16:03:49  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.5  2001/09/26 15:36:12  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.4  2001/06/01 15:50:17  meichel
 *  Updated copyright header
 *
 *  Revision 1.3  2000/06/02 16:00:47  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.2  2000/03/08 16:28:53  meichel
 *  Updated copyright header.
 *
 *  Revision 1.1  1999/09/17 14:28:00  meichel
 *  Moved static helper functions to new class DVPSHelper, removed some unused code.
 *
 *
 */
