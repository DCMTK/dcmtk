/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSPresentationLUT_PList
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1999-10-07 17:21:49 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSPLL_H__
#define __DVPSPLL_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "oflist.h"
#include "dctk.h"
#include "dvpstyp.h"     /* for enum types */

class DVPSPresentationLUT;
class DVPSImageBoxContent_PList;

/** the list of presentation LUTs contained in a stored print object.
 *  This class manages the data structures comprising one complete
 *  Presentation LUT Content Sequence.
 */

class DVPSPresentationLUT_PList: private OFList<DVPSPresentationLUT *>
{
public:
  /// default constructor
  DVPSPresentationLUT_PList();
  
  /// copy constructor
  DVPSPresentationLUT_PList(const DVPSPresentationLUT_PList& copy);

  /** clone method.
   *  @return a pointer to a new DVPSPresentationLUT_PList object containing
   *  a deep copy of this object.
   */
  DVPSPresentationLUT_PList *clone() { return new DVPSPresentationLUT_PList(*this); }

  /// destructor
  virtual ~DVPSPresentationLUT_PList();

  /** reads a list of Presentation LUTs (Presentation LUT Content Sequence) from a DICOM dataset.
   *  The DICOM elements of the image references item are copied from the dataset to this object.
   *  The completeness of all items (presence of all required elements,
   *  value multiplicity) is checked.
   *  If this method returns an error code, the object is in undefined state afterwards.
   *  @param dset the DICOM dataset from which the sequence is to be read
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition read(DcmItem &dset);
  
  /** writes the list of Presentation LUTs managed by this object to a DICOM dataset.
   *  Copies of the DICOM element managed by this object are inserted into
   *  the DICOM dataset.
   *  @param dset the DICOM dataset to which the ReferencedImageSequence is written
   *  @return EC_Normal if successful, an error code otherwise.
   */
  E_Condition write(DcmItem &dset);

  /** reset the object to initial state.
   *  After this call, the object is in the same state as after
   *  creation with the default constructor.
   */
  void clear();
    
  /** gets the number of Presentation LUTs in this list.
   *  @return the number of Presentation LUTs.
   */
  size_t size() const { return OFList<DVPSPresentationLUT *>::size(); }

  /** sets a new log stream
   *  @param o new log stream, must not be NULL
   */
  void setLog(ostream *o);

  /** finds a presentation LUT by its SOP instance UID.
   *  @param instanceUID SOP instance UID
   *  @return pointer to matching presentation LUT if found, NULL otherwise.
   */
  DVPSPresentationLUT *findPresentationLUT(const char *instanceUID);

  /** removes all presentation LUT entries that are not
   *  referenced from the film box or image box level.
   *  @param filmBox Presentation LUT UID reference on film box level, may be NULL.
   *  @param imageBoxes list of image boxes
   */
  void cleanup(const char *filmBox, DVPSImageBoxContent_PList& imageBoxes);

  /** adds a Presentation LUT to the list of managed LUTs.
   *  The referenced LUT is copied. If an identical LUT already exists,
   *  no duplicate is created.
   *  @param newLUT pointer to new Presentation LUT. May be NULL.
   *  @return UID of referenced Presentation LUT. May be NULL (if input was NULL).
   */
  const char *addPresentationLUT(DVPSPresentationLUT *newLUT);
  
private:

  /** private undefined assignment operator
   */
  DVPSPresentationLUT_PList& operator=(const DVPSPresentationLUT_PList&);
  
  /** output stream for error messages, never NULL
   */
  ostream *logstream;
    
};


#endif

/*
 *  $Log: dvpspll.h,v $
 *  Revision 1.1  1999-10-07 17:21:49  meichel
 *  Reworked management of Presentation LUTs in order to create tighter
 *    coupling between Softcopy and Print.
 *
 *
 */

