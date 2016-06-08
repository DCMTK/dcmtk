/*
 *
 *  Copyright (C) 2016, Open Connections GmbH
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
 *  Module: dcmiod
 *
 *  Author: Michael Onken
 *
 *  Purpose: Class for managing SOP references as used in different modules
 *
 */

#ifndef IODREFERENCES_H
#define IODREFERENCES_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/dcmiod/ioddef.h"

// Forward declaration
class DcmItem;

/** Class that represents a reference to another DICOM objects. DICOM uses
 *  different types of references for different purposes: Sometimes only SOP
 *  Class UID and SOP Instance UID are required, sometimes also Series and
 *  Study Instance UIDs. Also, in different IOD modules and macros DICOM uses
 *  different attributes to store those references in objects. Sometimes the
 *  same references are even required in different structures within the same
 *  object.
 *  This class can carry identifiers for Patient, Study, Series and Instance
 *  level and can be passed around in order to write them as required into
 *  the attribute structures needed by a specific DIOCM IOD, module or macro.
 */
class DCMTK_DCMIOD_EXPORT IODReference
{
  public:
    /// The maximum DICOM entity level that this reference refers to. This is
    /// mostly used in order to check whether a reference is complete or not.
    enum MAX_LEVEL
    {
      /// Instance Level
      LEVEL_INSTANCE,
      /// Series Level
      LEVEL_SERIES,
      /// Study level
      LEVEL_STUDY,
      /// Patient level
      LEVEL_PATIENT
    };

    /// Type of reference
    enum TYPE
    {
      /// Generic (or unknown)
      GENERIC,
      /// Reference to an image object, see also class IODImageReference
      IMAGE,
      /// Reference to a segmentation object, see also class IDOSegmentationReference
      SEGMENT
    };

    /** Constructor, creates empty reference, default level is "STUDY"
     */
    IODReference();

    /** Constructor, creates empty reference with specified level
     *  @param  level The reference level
     */
    IODReference(const MAX_LEVEL level);

    /** Destructor
     */
    virtual ~IODReference();

    /** Create (deep) clone copy of this reference
     *  @return Returns eep copy of this reference or NULL in case of error
     */
    virtual IODReference* clone() const;

    /** Get type of reference. Base class always returns "GENERIC"
     *  @return Returns "GENERIC" type
     */
    virtual TYPE getType() const {return GENERIC; }

    /** Check whether this reference is valid, i.e. complete related to its
     *  level. Also in case of UIDs it is checked whether they conform to the
     *  UI Value Representation definition.
     *  @return EC_Normal if reference is valid, error otherwise
     */
    virtual OFCondition check() const;

    /** Returns reference as a one line string. Mostly useful for debugging.
     *  @return Reference as a string.
     */
    virtual OFString toString() const;

    /** Initializes this reference from given file, i.e. extracts all identifiers
     *  up to the level defined for this reference.
     *  @param  filename The file to read from
     *  @return Returns OFTrue if reference could be read, otherwise OFFalse
     */
    virtual OFBool readFromFile(const OFString& filename);

    /** Initializes this reference from given item, i.e. extracts all identifiers
     *  up to the level defined for this reference.
     *  @param  item The item to read from
     *  @return Returns OFTrue if reference could be read, otherwise OFFalse
     */
    virtual OFBool readFromItem(DcmItem& item);

    /** Clear all reference data
     */
    virtual void clear();

    /// Patient ID
    OFString m_PatientID;

    /// Study Instance UID
    OFString m_StudyInstanceUID;

    /// Series Instance UID
    OFString m_SeriesInstanceUID;

    /// SOP Class UID
    OFString m_SOPClassUID;

    /// SOP Instance UID
    OFString m_SOPInstanceUID;

    /// Maximum level this reference refers to. E.g. for level SERIES, only
    /// Series, SOP Instance and SOP Class UID must be provided. However,
    //  setting the Patient ID too does not harm.
    MAX_LEVEL m_Level;
};


/** Class representing a reference to an image. Compared to IODReference, this
 *  class also allows to provide references to specific frames
 */
class DCMTK_DCMIOD_EXPORT IODImageReference : public IODReference
{
  public:

    /** Default constructor, uses level STUDY
     */
    IODImageReference();

    /** Constructor allowing to set reference level.
     *  @param  level The maximum level this reference should deal with
     */
    IODImageReference(const MAX_LEVEL level);

    /** Convenience constructor setting reference data including frame numbers
     *  @param  patientID Patient ID
     *  @param  studyUID Study Instance UID
     *  @param  seriesUID Series Instance UID
     *  @param  sopInstanceUID SOP Instance UID
     *  @param  sopClassUID SOP Class UID
     *  @param  refFrameNumbers Referenced Frame Numbers
     */
    IODImageReference(const OFString& patientID,
                      const OFString& studyUID,
                      const OFString& seriesUID,
                      const OFString& sopInstanceUID,
                      const OFString& sopClassUID,
                      const OFVector<Uint32>& refFrameNumbers);

    /** Convenience constructor setting reference data without frame numbers
     *  @param  patientID Patient ID
     *  @param  studyUID Study Instance UID
     *  @param  seriesUID Series Instance UID
     *  @param  sopInstanceUID SOP Instance UID
     *  @param  sopClassUID SOP Class UID
     */
    IODImageReference(const OFString& patientID,
                      const OFString& studyUID,
                      const OFString& seriesUID,
                      const OFString& sopInstanceUID,
                      const OFString& sopClassUID);

    /** Get (deep) clone copy of this reference
     *  @return Returns deep copy of this reference, or NULL in case of error
     */
    virtual IODReference* clone() const;

    /** Get type (always returns IMAGE type)
     *  @return Returns IMAGE type
     */
    virtual TYPE getType() const {return IMAGE; }

    /** Initialize reference from given file
     *  @param  filename The file to read from
     *  @param  frameNumbers The frame numbers to reference. It is not checked
     *          (so far) whether the referenced image file actually contains
     *           such frames. First frame is denoted by 1.
     *  @return OFTrue if initialization was successful, OFFalse otherwise
     */
    virtual OFBool readFromFile(const OFString& filename,
                                const OFVector<Uint32> frameNumbers);
    /** Destructor
     */
    virtual ~IODImageReference() {}

    /** Clear reference data
     */
    virtual void clear();

    /// Referenced frame numbers
    OFVector<Uint32> m_ReferencedFrameNumber;
};


/** Class representing a reference to a Segmentation. Compared to IODReference,
 *  this class also allows to provide references to specific segments by
 *  referencing the value of their Segment Number attribute.
 */
class DCMTK_DCMIOD_EXPORT IODSegmentationReference : public IODReference
{
  public:

    /** Constructor allowing to set reference level.
     *  @param  level The maximum level this reference should deal with
     */
    IODSegmentationReference(const MAX_LEVEL level);

    /** Default constructor, uses level STUDY
     */
    IODSegmentationReference();

    /** Get (deep) clone copy of this reference
     *  @return Returns deep copy of this reference, or NULL in case of error
     */
    virtual IODReference* clone() const;

    /** Get type (always returns SEGMENT type)
     *  @return Returns SEGMENT type
     */
    virtual TYPE getType() const {return SEGMENT; }

    /** Initialize reference from given file
     *  @param  filename The file to read from
     *  @param  segmentNumbers The segment numbers to reference. It is not
     *          checked (so far) whether the referenced image file actually
     *          contains such Segment Numbers. First segment is 1.
     *  @return OFTrue if initialization was successful, OFFalse otherwise
     */
    virtual OFBool readFromFile(const OFString& filename,
                                const OFVector<Uint16> segmentNumbers);

    /** Destructor
     */
    virtual ~IODSegmentationReference() {}

    /** Clear reference data
     */
    virtual void clear();

    /// Referenced Segment Numbers
    OFVector<Uint16> m_ReferencedSegmentNumber;
};


/** Class that holds a set of IODReference instances (or its sub classes) and
 *  offers helper functionality to read and write such references
 */
class DCMTK_DCMIOD_EXPORT IODReferences
{

  public:

    /** Default constructor
     */
    IODReferences();

    /** Copy constructor, performs deep copy of provided references
     *  @param  rhs The references to assign
     */
    IODReferences(const IODReferences& rhs);

    /** Assignment operator, copies all provided references
     */
    IODReferences& operator=(const IODReferences& rhs);

    /** Destructor, frees memory
     */
    virtual ~IODReferences();

    /** Initialize references by reading the Referenced Instance Sequence as
     *  used in the Tractography Results Module
     *  @param  source The item to read from, must contain the Referenced
     *          Instance Sequence
     *  @return EC_Normal if all references could be read, IOD_EC_InvalidReference
     *          if no reference could be read, and IOD_EC_ReferencesOmitted if
     *          at least one reference could be read but at least one had also
     *          to be skipped.
     */
    virtual OFCondition readTractographyReferencedInstanceSequence(DcmItem& source);

    /** Write references to Referenced Instance Sequence as used in the
     *  Tractography Results Module
     *  @param  item The item to write to
     *  @return EC_Normal if all references could be written, error code
     *          otherwise
     */
    virtual OFCondition writeTractographyReferencedInstanceSequence(DcmItem& item);

    /** Add reference to this set of references
     *  @param  ref The reference to add (ownership is taken if adding is
     *          successful
     *  @return OFTrue if adding is successful, OFFalse otherwise
     */
    virtual OFBool add(IODReference* ref);

    /** Initialize references from set of DICOM files. If a file could not be
     *  used, it is skipped. All references found by this method are added on
     *  top of existing ones, i.e. any existing data is not cleared by thi
     *  method.
     *  @param  dcmFiles The DICOM file names to read
     *  @param  maxLevel The maximum level to extract
     *  @return Returns number of references (successful files) that could be
     *          added
     */
    virtual size_t addFromFiles(const OFVector<OFString>& dcmFiles,
                                const IODReference::MAX_LEVEL maxLevel = IODReference::LEVEL_STUDY);

    /** Access references of this set (readonly)
     *  @return Returns references managed by this class
     */
    const OFVector<IODReference*>& get() const;

    /** Returns number of references managed by this class
     *  @return Returns number of references
     */
    virtual size_t size() const;

    /** Clears all references
     */
    virtual void clearData();

  private:

    /// Set of references managed by this class
    OFVector<IODReference*> m_References;
};


#endif // IODREFERENCES_H
