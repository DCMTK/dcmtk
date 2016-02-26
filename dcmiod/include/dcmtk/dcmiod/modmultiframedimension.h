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
 *  Module: dcmiod
 *
 *  Author: Michael Onken
 *
 *  Purpose: Class for managing the Multi-Frame Dimension Module
 *
 */

#ifndef MODMULTIFRAMEDIMENSION_H
#define MODMULTIFRAMEDIMENSION_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/iodrules.h"
#include "dcmtk/dcmiod/modbase.h"

/** Class representing the Multi-Frame Dimension Module:
 *
 * Dimension Organization Sequence: (SQ, VM 1-n, Type 1)
 * > Dimension Organization UID: (UI, 1, 1)
 * Dimension Organization Type: (CS, 1, 1)
 * Dimension Index Sequence: (SQ, 1-n, 1)
 * > Dimension Index Pointer (AT, 1, 1)
 * > Dimension Index Private Creator (LO, 1, 1C)
 * > Functional Group Pointer (AT, 1, 1C)
 * > Functional Group Private Creator (LO, 1, 1C)
 * > Dimension Organization UID (UI, 1, 1C)
 * > Dimension Description Label (LO, 1, 3)
 *
 * The Dimension Organization UIDs within the Dimension Organization Sequence
 * cannot be set explicitly. Instead the sequence is populated from the data
 * found within the Dimension Index Sequence.
 */
class DCMTK_DCMIOD_EXPORT IODMultiframeDimensionModule : public IODModule
{

public:

  /** Helper class representing an item within the Dimension Index Sequence,
   *  i.e.\ a single dimension description
   */
  class DimensionOrganizationItem : public IODComponent
  {

  public:

    /** Constructor
     *  @param  data The item to be used for data storage. If NULL, the
     *          class creates an empty data container.
     *  @param  rules The rule set for this class. If NULL, the class creates
     *          one from scratch and adds its values.
     *  @param  parent The parent of the IOD component (NULL if none or unknown)
     */
    DimensionOrganizationItem(OFshared_ptr<DcmItem> data,
                              OFshared_ptr<IODRules> rules,
                              IODComponent* parent);

    /** Constructor
     *  @param  parent The parent of the IOD component (NULL if none or unknown)
     */
    DimensionOrganizationItem(IODComponent* parent = NULL);

    /** Destructor
     */
    virtual ~DimensionOrganizationItem();

    /** Get name of module ("DimensionIndexPointerSequence")
     *  @return Name of the module ("DimensionIndexPointerSequence")
     */
    virtual OFString getName() const;

    /** Resets rules of this module to their original values
     */
    virtual void resetRules();

    /** Get Dimension Organization UID
     *  @param  value  Reference to variable in which the value should be
     *          stored
     *  @param  pos  Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDimensionOrganizationUID(OFString &value,
                                                    const signed long pos = 0) const;

    /** Set Dimension Organization UID
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (PN) and VM (1) if enabled
     *  @return status EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDimensionOrganizationUID(const OFString& value,
                                                    const OFBool checkValue = OFTrue);
  };

  /** Class representing a Dimension Index Pointer Sequence Item
   *  managed by the Multi-frame Dimension Module
   */
  class DimensionIndexItem : public IODComponent
  {

  public:

    /** Constructor
     *  @param  data The item to be used for data storage. If NULL, the
     *          class creates an empty data container.
     *  @param  rules The rule set for this class. If NULL, the class creates
     *          one from scratch and adds its values.
     *  @param  parent The parent of the IOD component (NULL if none or unknown)
     */
    DimensionIndexItem(OFshared_ptr<DcmItem> data,
                       OFshared_ptr<IODRules> rules,
                       IODComponent* parent);

    /** Constructor
     *  @param  parent The parent of the IOD component (NULL if none or unknown)
     */
    DimensionIndexItem(IODComponent* parent = NULL);

    /** Destructor
     */
    virtual ~DimensionIndexItem();

    /** Get name of module ("DimensionIndexSequence")
     *  @return Name of the module ("[parent.]DimensionIndexSequence")
     */
    virtual OFString getName() const;

    /** Resets rules to their original values
     */
    virtual void resetRules();

    /** Get Dimension Index Pointer
     *  @param  value  Reference to variable in which the value should be stored
     *  @param  pos    Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDimensionIndexPointer(DcmTagKey &value,
                                                 const signed long pos = 0) const;

    /** Get Dimension Index Private Creator
     *  @param  value  Reference to variable in which the value should be stored
     *  @param  pos    Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDimensionIndexPrivateCreator(OFString& value,
                                                        const signed long pos = 0) const;

    /** Get Functional Group Pointer
     *  @param  value  Reference to variable in which the value should be stored
     *  @param  pos    Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getFunctionalGroupPointer(DcmTagKey &value,
                                                  const signed long pos = 0) const;

    /** Get Functional Group Private Creator
     *  @param  value  Reference to variable in which the value should be stored
     *  @param  pos    Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getFunctionalGroupPrivateCreator(OFString &value,
                                                         const signed long pos = 0) const;

    /** Get Dimension Organization UID
     *  @param  value  Reference to variable in which the value should be stored
     *  @param  pos    Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDimensionOrganizationUID(OFString &value,
                                                    const signed long pos = 0) const;

    /** Get Dimension Description Label
     *  @param  value  Reference to variable in which the value should be stored
     *  @param  pos    Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getDimensionDescriptionLabel(OFString &value,
                                                     const signed long pos = 0) const;

    /** Set Dimension Index Pointer
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value'. Not evaluated (here for consistency
     *          with other setter functions).
     *  @return status EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDimensionIndexPointer(const DcmTagKey&value,
                                                 const OFBool checkValue = OFTrue);

    /** Set Dimension Index Private Creator
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR (PN) and VM (1) if enabled
     *  @return status EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDimensionIndexPrivateCreator(const OFString& value,
                                                        const OFBool checkValue = OFTrue);

    /** Set Dimension Index Functional Group Pointer
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value'. Not evaluated (here for consistency
     *          with other setter functions).
     *  @return status EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setFunctionalGroupPointer(const DcmTagKey& value,
                                                  const OFBool checkValue = OFTrue);

    /** Set Dimension Index Functional Group Private Creator
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value. Not evaluated (here for consistency
     *          with other setter functions).
     *
     *  @return status EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setFunctionalGroupPrivateCreator(const OFString& value,
                                                         const OFBool checkValue = OFTrue);

    /** Set Dimension Organization UID
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR and VM if enabled
     *  @return status EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDimensionOrganizationUID(const OFString& value,
                                                    const OFBool checkValue = OFTrue);

    /** Set Dimension Description Label
     *  @param  value Value to be set (single value only) or "" for no value
     *  @param  checkValue Check 'value' for conformance with VR and VM if enabled
     *  @return status EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition setDimensionDescriptionLabel(const OFString& value,
                                                     const OFBool checkValue = OFTrue);
  };

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          one from scratch and adds its values.
   */
  IODMultiframeDimensionModule(OFshared_ptr<DcmItem> item,
                               OFshared_ptr<IODRules> rules);

  /** Constructor
   */
  IODMultiframeDimensionModule();

  /** Destructor
   */
  virtual ~IODMultiframeDimensionModule();

  /** Convenience method to add Dimension Index.
   *  @param  dimensionIndexPointer The Dimension Index Pointer attribute
   *  @param  dimensionOrganizationUID The Dimension Organization UID of the dimension organization
   *          the pointer refers to
   *  @param  functionalGroupPointer The functional group where the pointer attribute can be found
   *  @param  dimensionDescriptionLabel The description label of this dimension (optional)
   *  @param  dimensionIndexPrivateCreator The private creator of the Dimension Index Pointer
   *          attribute. Required if dimensionIndexPointer points to a private attribute tag.
   *  @param  functionalGroupPrivateCreator The private creator of the functional group. Required
   *          if functionalGroupPointer points to a private attribute tag.
   *  @return EC_Normal if index could be added, error otherwise (basic parameter checking)
   *
   */
  virtual OFCondition addDimensionIndex(const DcmTagKey& dimensionIndexPointer,
                                        const OFString&  dimensionOrganizationUID,
                                        const DcmTagKey& functionalGroupPointer,
                                        const OFString&  dimensionDescriptionLabel = "",
                                        const OFString&  dimensionIndexPrivateCreator = "",
                                        const OFString&  functionalGroupPrivateCreator = "");

  /** Clear (removes) all attributes handled by the modules of this component.
   *  Rules are not reset.
   */
  virtual void clearData();

  /** Read data into this module from given item
   *  @param  source The item to read from
   *  @param  clearOldData If OFTrue, old data in this module is cleared first (default: OFTrue)
   *  @return EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write this module to given item
   *  @param  destination The item to write to
   *  @return EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& destination);

  /** Resets rules to their original values.
   */
  virtual void resetRules();

  /** Get name of module ("MultiframeDimensionModule")
   *  @return Name of the module ("MultiframeDimensionModule")
   */
  virtual OFString getName() const;

   /** Get Dimension Organization Type
    *  @param  value  Reference to variable in which the value should be stored
    *  @param  pos    Index of the value to get (0..vm-1), -1 for all components
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getDimensionOrganizationType(OFString &value,
                                                   const signed long pos = 0) const;

  /** Set Dimension Organization Type (Defined Terms as of DICOM 2014a "3D" and "3D_TEMPORAL")
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR and VM if enabled
   *  @return status EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setDimensionOrganizationType(const OFString &value,
                                                   const OFBool checkValue = OFTrue);

  /** Get content of the Dimension Index Sequence
   *  @return Reference to the Dimension Index Pointer Sequence
   */
  virtual OFVector<DimensionIndexItem*>& getDimensionIndexSequence();

  /** Get content of the Dimension Organization Sequence
   *  @return Reference to the Dimension Organization Sequence
   */
  virtual OFVector<DimensionOrganizationItem*>& getDimensionOrganizationSequence();


  /** Check dimensions for consistency with functional groups. Note that one can provide
   *  an item that provides the dataset containing the functional groups to check against.
   *  This item must contain on main level the Shared and Per-frame Functional Group
   *  Sequence. If no item is given (NULL pointer) it is tried to find these sequences
   *  within "this" object which works in the case that this class was initialized with
   *  an existing item (or the fg data was inserted in another way afterwards).
   *  @param  fgItem The item containing functional group information
   *  @return EC_Normal, if consistency is fine, error otherwise
   */
  virtual OFCondition checkDimensions(DcmItem* fgItem = NULL);

  /** Get specific index pointer element from per-frame functional group
   *  sequence. Prints out error messages if index does not exist or
   *  index location information provided by parameters is incomplete or invalid
   *  @param  perFrameFG An instance of the Per-Frame Functional Group Sequence that
   *          should be searched
   *  @param  fgPointer The tag key of the functional group the value resides in
   *  @param  indexPointer The tag of the dimension value inside the functional group
   *  @param  fgPrivateCreator The private creator of the fgPointer tag key. Only required
   *          if fgPointer is a private tag key (odd element number)
   *  @param  privateCreator The private creator of the indexPointer tag key. Only required
   *          if indexPointer is a private tag key (odd element number)
   *  @return The element pointed to. If element could not be located, NULL is returned.
   */
  virtual DcmElement* getIndexElement(DcmSequenceOfItems* perFrameFG,
                                      const DcmTagKey& fgPointer,
                                      const DcmTagKey& indexPointer,
                                      const OFString& fgPrivateCreator,
                                      const OFString& privateCreator);

protected:

  /** Go over Dimension Index Pointers and extract Dimension Organization UIDs for
   *  Dimension Organization Sequence.
   */
  void createDimensionOrganizationData();

private:

  /// This module's name (MultiframeDimensionModule)
  static const OFString m_ModuleName;

  /// Vector with all items of the Dimension Index Pointer Sequence
  OFVector<DimensionIndexItem*> m_DimensionIndexSequence;

  /// Vector with all items of the Dimension Organization Sequence
  OFVector<DimensionOrganizationItem*> m_DimensionOrganizationSequence;
};

#endif // MODMULTIFRAMEDIMENSION_H
