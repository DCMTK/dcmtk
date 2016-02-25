/*
 *
 *  Copyright (C) 2015-2016, Open Connections GmbH
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
 *  Module:  dcmfg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class for managing Derivation Image Functional Group
 *
 */


#ifndef FGDERIMG_H
#define FGDERIMG_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmiod/iodmacro.h"
#include "dcmtk/dcmfg/fgbase.h"

/** Class representing a single item of the Source Image Sequence
 */
class DCMTK_DCMFG_EXPORT SourceImageItem
{

public:

  /** Constructor, constructs empty Source Image Sequence item
   */
  SourceImageItem();

  /** Virtual destructor, cleans up memory
   */
  virtual ~SourceImageItem();

  /** Clears all data handled by this component
   */
  virtual void clearData();

  /** Check whether this item contains valid data
   *  @return EC_Normal if data is valid, error otherwise
   */
  virtual OFCondition check() const;

  /** Return handle to to purpose of reference code
   *  @return EC_Normal if data is valid, error otherwise
   */
  virtual CodeSequenceMacro& getPurposeOfReferenceCode();

  /** Returns handle to to image sop instance reference
   *  @return EC_Normal if data is valid, error otherwise
   */
  virtual ImageSOPInstanceReferenceMacro& getImageSOPInstanceReference();

  /** Reads source image item from given item
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& itemOfSourceImageSequence,
                           const OFBool clearOldData = OFTrue);

  /** Writes source image item to given item
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& itemOfSourceImageSequence);


  /** Comparison operator that compares the normalized value of this object
   *  with a given object of the same type, i.e.\ the elements within both
   *  functional groups (this and rhs parameter) are compared by value!
   *  Both objects (this and rhs) need to have the same type (i.e.\ both
   *  FGUnknown) to be comparable. This function is used in order
   *  to decide whether a functional group already exists, or is new. This
   *  is used in particular to find out whether a given functional group
   *  can be shared (i.e.\ the same information already exists as shared
   *  functional group) or is different from the same shared group. In that
   *  case the shared functional group must be distributed into per-frame
   *  functional groups, instead. The exact implementation for implementing
   *  the comparison is not relevant. However, it must be a comparison
   *  by value.
   *  @param  rhs the right hand side of the comparison
   *  @return 0 if the object values are equal.
   *          -1 if either the value of the first component that does not match
   *          is lower in the this object, or all compared components match
   *          but this component is shorter. Also returned if this type and
   *          rhs type (DcmFGTypes::E_FGType) do not match.
   *          1 if either the value of the first component that does not match
   *          is greater in this object, or all compared components match
   *          but this component is longer.
   */
  virtual int compare(const SourceImageItem& rhs) const;

  /** Assignment operator, deletes old data
   *  @param  rhs The item that should be assigned to "this" class
   *  @return Reference to "this" class
   */
  SourceImageItem& operator=(const SourceImageItem& rhs);

private:

  /// Describes purpose of reference (single item of Purpose of Reference Code
  /// Sequence)
  CodeSequenceMacro m_PurposeOfReferenceCode;

  /// Contains the referenced images (as represented by one of the items of
  /// "this" Source Image Sequence)
  ImageSOPInstanceReferenceMacro m_ImageSOPInstanceReference;
};

/// Iterator for traversing over items of the Source Image Sequence
typedef OFVector<SourceImageItem*>::iterator SourceImageIterator;


/** Class representing a single item in Derivation Image Sequence
 */
class DCMTK_DCMFG_EXPORT DerivationImageItem
{
public:

  /** Constructor, initializes empty derivation image item
   */
  DerivationImageItem();

  /** Virtual destructor
   */
  virtual ~DerivationImageItem();

  /** Assignment operator, deletes old data
   *  @param  rhs The item that should be assigned to "this" class
   *  @return Reference to "this" class
   */
  DerivationImageItem& operator=(const DerivationImageItem& rhs);

  /** Copy constructor, deletes old data
   *  @param  rhs The item that should be used for initialization
   */
  DerivationImageItem(const DerivationImageItem& rhs);

  /** Comparison operator that compares the normalized value of this object
   *  with a given object of the same type
   *  @param  rhs The right hand side of the comparison
   *  @return 0 If the object values are equal.
   *          -1 if either the value of the first component that does not match
   *          is lower in the rhs object, or all compared components match
   *          but the rhs component is shorter. Also returned if rhs cannot be
   *          casted to DcmAttributeTag.
   *          1 if either the value of the first component that does not match
   *          is greater in the rhs object, or all compared components match
   *          but the rhs component is longer.
   */
  virtual int compare(const DerivationImageItem& rhs) const;

  /** Clears all data handled by this component
   */
  virtual void clearData();

  /** Check whether item contains valid data
   *  @return EC_Normal if item is valid, error otherwise
   */
  virtual OFCondition check() const;

  /** Convenience function to add item representing reference to a file. No
   *  restrictions are set which frames or segments from this image has been
   *  actually used. However, such information could be added to the source
   *  image item later by modifying the resulting item being handed back to the
   *  caller.
   * @param file Files that should be referenced by their UID, must be readable.
   * @param purposeOfReference Code representing the purpose of reference
   *        (Defined CID 7202)
   * @param resultSourceImageItem The created derivation image item if
   *        successful, NULL otherwise
   * @return EC_Normal if adding works, error code otherwise
   */
  virtual OFCondition addSourceImageItem(const OFString& file,
                                         const CodeSequenceMacro& purposeOfReference,
                                         SourceImageItem*& resultSourceImageItem);

  /** Convenience function to add items representing references to some files
   *  which all have the same purpose of reference (code). No restrictions are
   *  set which frames or segments from those images have been actually used.
   *  However, such information could be added to the source image item later by
    * modifying the resulting item handed back to the caller.
    * @param files List of files that should be referenced by their UIDs.
    * @param purposeOfReference Code representing the purpose of reference
    *        (Defined CID 7202)
    * @param resultSourceImageItems The created derivation image items (one per
    *        file) if successful, NULL otherwise
    * @param skipFileErrors If OFTrue, then files that could not be added will
    *        not lead to error.
    * @return EC_Normal if adding works, error code otherwise
   */
  virtual OFCondition addSourceImageItems(const OFVector<OFString>& files,
                                          const CodeSequenceMacro& purposeOfReference,
                                          OFVector<SourceImageItem*>& resultSourceImageItems,
                                          const OFBool skipFileErrors = OFFalse);

   /** Get Derivation Description
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1), -1 for all components
    *  @return status, EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getDerivationDescription(OFString &value,
                                               const signed long pos = 0) const;

  /** Get reference to derivation code items
   *  @return Reference to derivation code items
   */
  virtual OFVector<CodeSequenceMacro*>& getDerivationCodeItems();

  /** Get reference to source image items
   *  @return Reference to source image items
   */
  virtual OFVector<SourceImageItem*>& getSourceImageItems();

  /** Set Derivation Description
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value'. Not evaluated (here for consistency
   *          with other setter functions).
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setDerivationDescription(const OFString &value,
                                               const OFBool checkValue = OFTrue);

  /** Read Derivation Image Sequence Item describing derivation from a
   *  set of images
   *  @param  itemOfDerivationImageSequence The item to read from
   *  @param  clearOldData If OFTrue, old data is deleted first
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& itemOfDerivationImageSequence,
                           const OFBool clearOldData = OFTrue);

  /** Write Derivation Image Sequence Item describing derivation from a
   *  set of images
   *  @param  itemOfDerivationImageSequence The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& itemOfDerivationImageSequence);

private:

  // DICOM attributes.
  // The comments for each attribute describe "Name: (VR, VM, Type)".
  // See DICOM standard for further reference.

  /// Derivation Description: (ST, 1, 3)
  DcmShortText m_DerivationDescription;

  /// List of derivation codes for this set of images (1-n items permitted)
  OFVector<CodeSequenceMacro*> m_DerivationCodeItems;

  // List of source images, as described by Source Image Sequence (0-n items
  // permitted)
  OFVector<SourceImageItem*> m_SourceImageItems;

};

/** Class representing the "Derivation Image Functional Group Macro"
 */
class DCMTK_DCMFG_EXPORT FGDerivationImage : public FGBase
{
public:

  /** Constructor creating an empty functional group
   */
  FGDerivationImage();

  /** Virtual destructor, frees memory
   */
  virtual ~FGDerivationImage();

  /** Returns a deep copy of this object
   *  @return  Deep copy of this object
   */
  virtual FGBase *clone() const;

  /** Convenience function to create a minimalistic FGDerivationImage
   *  @param  derivationImages The list of SOP instances to reference
   *  @param  derivationDescription Description of the derivation performed
   *  @param  derivationCode Coded representation of the derivation description
   *  @param  purposeOfReference Purpose of referencing the SOP instances
   *  @return The created derivation image item, if successful, NULL otherwise
   */
  static FGDerivationImage* createMinimal(const OFVector<ImageSOPInstanceReferenceMacro>& derivationImages,
                                          const OFString& derivationDescription,
                                          const CodeSequenceMacro& derivationCode,
                                          const CodeSequenceMacro& purposeOfReference);

  /** Comparison operator that compares the normalized value of this object
   *  with a given object of the same type, i.e.\ the elements within both
   *  functional groups (this and rhs parameter) are compared by value!
   *  Both objects (this and rhs) need to have the same type (i.e.\ both
   *  FGDerivationImage) to be comparable. This function is used in order
   *  to decide whether a functional group already exists, or is new. This
   *  is used in particular to find out whether a given functional group
   *  can be shared (i.e.\ the same information already exists as shared
   *  functional group) or is different from the same shared group. In that
   *  case the shared functional group must be distributed into per-frame
   *  functional groups, instead. The exact implementation for implementing
   *  the comparison is not relevant. However, it must be a comparison
   *  by value.
   *  @param  rhs the right hand side of the comparison
   *  @return 0 if the object values are equal.
   *          -1 if either the value of the  first component that does not match
   *          is lower in the rhs object, or all compared components match
   *          but the rhs component is shorter. Also returned if this type and
   *          rhs type (DcmFGTypes::E_FGType) do not match.
   *          1 if either the value of the first component that does not match
   *          is greater in the rhs object, or all compared components match
   *          but the rhs component is longer.
   */
  virtual int compare(const FGBase& rhs) const;

  /** Returns shared type of this functional group
   *  @return Always returns DcmFGTypes::EFGS_BOTH
   */
  virtual DcmFGTypes::E_FGSharedType getSharedType() const {return DcmFGTypes::EFGS_BOTH;}

  /** Convenience function to add an item only having a single derivation code
    * derivation description. No source images have to provided but can be added
    * later on the resulting DerivationImageItem.
    * @param derivationCode Code describing derivation
    * @param derivationDescription Free text description of derivation (can be
    *        empty)
    * @param item The created derivation image item if successful, NULL
    *        otherwise
    * @return EC_Normal if adding works, error code otherwise
    */
  virtual OFCondition addDerivationImageItem(const CodeSequenceMacro& derivationCode,
                                             const OFString& derivationDescription,
                                             DerivationImageItem*& item);

  /** Clears all data handled by this component
   */
  virtual void clearData();

  /** Checks whether this class contains valid data
   *  @return EC_Normal if data is valid, error otherwise
   */
  virtual OFCondition check() const;

  /** Get reference to derivation image items
   *  @return Reference to derivation image items
   */
  virtual OFVector<DerivationImageItem*>& getDerivationImageItems();

  /** Read Derivation Image Sequence from given item, which can contain zero or
   *  more items
   *  @param  item The item to read from
   *  @return Returns EC_Normal if at least one derivation image item could be
   *          read or no item exists at all, otherwise an error is returned
   */
  virtual OFCondition read(DcmItem& item);

  /** Write Derivation Image Sequence (containing contain zero or more items)
   *  to given item
   *  @param  item The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& item);

private:

  /// List of derivation image items making up the Derivation Image Sequence
  OFVector<DerivationImageItem*> m_DerivationImageItems;

};

#endif // FGDERIMG_H
