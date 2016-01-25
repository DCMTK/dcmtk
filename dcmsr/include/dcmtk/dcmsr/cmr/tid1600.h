/*
 *
 *  Copyright (C) 2015-2016, J. Riesmeier, Oldenburg, Germany
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  Header file for class TID1600_ImageLibrary
 *
 *  Author: Joerg Riesmeier
 *
 */


#ifndef CMR_TID1600_H
#define CMR_TID1600_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */

#include "dcmtk/dcmsr/dsrstpl.h"

#include "dcmtk/dcmsr/cmr/define.h"
#include "dcmtk/dcmsr/cmr/cid4020.h"
#include "dcmtk/dcmsr/cmr/cid4021.h"
#include "dcmtk/dcmsr/cmr/srnumvl.h"


// include this file in doxygen documentation

/** @file tid1600.h
 *  @brief Interface class and error constants for TID 1600 in module dcmsr/cmr
 */


/*------------------------*
 *  constant definitions  *
 *------------------------*/

/** @name specific error conditions for TID 1600 in module dcmsr/cmr
 */
//@{

/// error: there is no image library to add image groups to
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_NoImageLibrary;
/// error: there is no image library group to add image entries to
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_NoImageLibraryGroup;
/// error: there is no image library entry to add descriptors to
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_NoImageLibraryEntry;
/// error: cannot add multiple image library entry descriptors (see TID 1600 Row 3)
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_CannotAddMultipleImageLibraryEntryDescriptors;
/// error: the current (most recently added) image library entry has no modality descriptor
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_MissingImageLibraryEntryDescriptorModality;
/// error: the current (most recently added) image library entry has the wrong modality descriptor
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_WrongImageLibraryEntryDescriptorModality;
/// normal: there are no image library entry descriptors to be added (copied from the dataset)
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_NoImageLibraryEntryDescriptorsToBeAdded;

//@}


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of DCMR Template:
 *  TID 1600 - Image Library (and included templates 1601-1607).
 *  All added content items are annotated with a text in the format "TID 1600 - Row [n]".
 ** @note Please note that most DICOM element values are copied from the main dataset
 *        only and not yet from the respective functional group macros.
 */
class DCMTK_CMR_EXPORT TID1600_ImageLibrary
  : public DSRSubTemplate
{

  public:

    /** add mode for image entries
     */
    enum AddImageMode
    {
        /// add image entry without descriptors
        withoutDescriptors,
        /// add image entry with all descriptors from TID 1602 (and included templates)
        withAllDescriptors
    };

    /** (default) constructor
     ** @param  createLibrary  flag indicating whether to create an empty image library
     *                         by calling createImageLibrary() during startup (default)
     */
    TID1600_ImageLibrary(const OFBool createLibrary = OFTrue);

    /** check whether the current internal state is valid.
     *  That means, whether the base class is valid and hasImageLibrary() returns true.
     ** @return OFTrue if valid, OFFalse otherwise
     */
    virtual OFBool isValid() const;

    /** check whether the content item 'Image Library' (TID 1600 - Row 1) is present.
     *  Initially, this mandatory content item is created by the constructor of this
     *  class (if not disabled).  After clear() has been called, it can be created again
     *  by calling createNewImageLibrary().
     ** @return OFTrue if the image library is present, OFFalse otherwise
     */
    OFBool hasImageLibrary() const;

    /** check whether there is an image group in this image library (TID 1600 - Row 2)
     ** @return OFTrue if at least one image group is present, OFFalse otherwise
     */
    OFBool hasImageLibraryGroup() const;

    /** clear the internally stored tree of content items and create the mandatory
     *  content item 'Image Library' (TID 1600 - Row 1) as the root node of this template
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition createNewImageLibrary();

    /** add an image group to the image library
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addImageGroup();

    /** add an image entry to the current image group, i.e.\ add content items for
     *  TID 1601 (Image Library Entry).  The values of the content items (including
     *  the image reference) are copied from the data elements of the given 'dataset'.
     *  If no descriptors were added, CMR_EC_NoImageLibraryEntryDescriptorsToBeAdded
     *  is returned.
     ** @param  dataset  DICOM dataset from which the values should be copied
     *  @param  mode     mode specifying which optional content items are to be added
     *  @param  check    if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise.  If no
     *          image group exists, CMR_EC_NoImageLibraryGroup is returned.
     */
    OFCondition addImageEntry(DcmItem &dataset,
                              const AddImageMode mode = withoutDescriptors,
                              const OFBool check = OFTrue);

    /** add image entry descriptors to the current image group, i.e.\ add content
     *  items for TID 1602 (Image Library Entry Descriptors) and included templates.
     *  The values of the content items are copied from the data elements of the given
     *  'dataset'.  If none were added, CMR_EC_NoImageLibraryEntryDescriptorsToBeAdded
     *  is returned.
     *  Please note that this method should only be called once for each image group.
     ** @param  dataset  DICOM dataset from which the values should be copied
     *  @param  check    if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise.  If no
     *          image group exists, CMR_EC_NoImageLibraryGroup is returned.
     */
    OFCondition addImageEntryDescriptors(DcmItem &dataset,
                                         const OFBool check = OFTrue);

    /** go to the most recently added image library entry and get the value of the
     *  descriptor 'Modality' (TID 1602 - Row 1)
     ** @param  modalityCode  reference to coded entry that will store the result
     *                        (cleared if an error occurs)
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getImageEntryModality(DSRCodedEntryValue &modalityCode);

  // --- set modality-specific content manually ---

    /** set the value of the descriptor 'Radionuclide' (TID 1607 - Row 1) for the current
     *  (most recently added) PET image library entry.
     *  If the content item already exists, its value is overwritten.  Otherwise, a new
     *  content item is added to the end of the list of image library entry descriptors.
     ** @param  radionuclide  coded entry to be set (from CID 4020 - PET Radionuclide)
     *  @param  check         if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setPETImageRadionuclide(const CID4020_PETRadionuclide &radionuclide,
                                        const OFBool check = OFTrue);

    /** set the value of the descriptor 'Radiopharmaceutical Agent' (TID 1607 - Row 2)
     *  for the current (most recently added) PET image library entry.
     *  If the content item already exists, its value is overwritten.  Otherwise, a new
     *  content item is added to the end of the list of image library entry descriptors.
     ** @param  agent  coded entry to be set (from CID 4021 - PET Radiopharmaceutical)
     *  @param  check  if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setPETImageRadiopharmaceuticalAgent(const CID4021_PETRadiopharmaceutical &agent,
                                                    const OFBool check = OFTrue);

    /** set the value of the descriptor 'Radiopharmaceutical Start Date Time' (TID 1607 -
     *  Row 4) for the current (most recently added) PET image library entry.
     *  If the content item already exists, its value is overwritten.  Otherwise, a new
     *  content item is added to the end of the list of image library entry descriptors.
     ** @param  dateTime  date/time value to be set (associated DICOM VR=DT)
     *  @param  check     if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setPETImageRadiopharmaceuticalStartDateTime(const OFString &dateTime,
                                                            const OFBool check = OFTrue);

    /** set the value of the descriptor 'Radiopharmaceutical Stop Date Time' (TID 1607 -
     *  Row 5) for the current (most recently added) PET image library entry.
     *  If the content item already exists, its value is overwritten.  Otherwise, a new
     *  content item is added to the end of the list of image library entry descriptors.
     ** @param  dateTime  date/time value to be set (associated DICOM VR=DT)
     *  @param  check     if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setPETImageRadiopharmaceuticalStopDateTime(const OFString &dateTime,
                                                           const OFBool check = OFTrue);

    /** set the value of the descriptor 'Radiopharmaceutical Volume' (TID 1607 - Row 6)
     *  for the current (most recently added) PET image library entry.
     *  If the content item already exists, its value is overwritten.  Otherwise, a new
     *  content item is added to the end of the list of image library entry descriptors.
     ** @param  volume  numeric measurement value to be set (measurement unit should be
     *                  CODE_UCUM_cm3 or a coded entry that is identical to this)
     *  @param  check   if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setPETImageRadiopharmaceuticalVolume(const CMR_SRNumericMeasurementValue &volume,
                                                     const OFBool check = OFTrue);

    /** set the value of the descriptor 'Radiopharmaceutical Total Dose' (TID 1607 -
     *  Row 7) for the current (most recently added) PET image library entry.
     *  If the content item already exists, its value is overwritten.  Otherwise, a new
     *  content item is added to the end of the list of image library entry descriptors.
     ** @param  totalDose  numeric measurement value to be set (measurement unit should
     *                     be CODE_UCUM_Bq or a coded entry that is identical to this)
     *  @param  check      if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setPETImageRadionuclideTotalDose(const CMR_SRNumericMeasurementValue &totalDose,
                                                 const OFBool check = OFTrue);


  protected:

    /** create the mandatory content item 'Image Library' (TID 1600 - Row 1) as the root
     *  node of this template.  It is expected that the tree is currently empty.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition createImageLibrary();

    /** add image library entry descriptors (TID 1602) to given document subtree.
     *  This method also calls addModalitySpecificDescriptors() in order to add the
     *  included templates (TID 1603 to 1607).
     ** @param  tree     subtree to which the content items should be added
     *  @param  dataset  DICOM dataset from which the values should be copied
     *  @param  check    if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addImageEntryDescriptors(DSRDocumentSubTree &tree,
                                         DcmItem &dataset,
                                         const OFBool check);

    /** add image library entry descriptors (TID 1603 to 1607) to given document
     *  subtree depending on the given 'modality'.
     *  @warning The mapping of modality to included templates is probably not perfect
     *           yet.  Also please note that most DICOM element values are copied from
     *           the main dataset and not from the respective functional group macros.
     ** @param  tree      subtree to which the content items should be added
     *  @param  dataset   DICOM dataset from which the values should be copied
     *  @param  modality  value of the Modality (0008,0060) attribute from 'dataset'
     *  @param  check     if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addModalitySpecificDescriptors(DSRDocumentSubTree &tree,
                                               DcmItem &dataset,
                                               const OFString &modality,
                                               const OFBool check);

    /** add image library entry descriptors for projection radiography (TID 1603)
     ** @param  tree     subtree to which the content items should be added
     *  @param  dataset  DICOM dataset from which the values should be copied
     *  @param  check    if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addProjectionRadiographyDescriptors(DSRDocumentSubTree &tree,
                                                    DcmItem &dataset,
                                                    const OFBool check);

    /** add image library entry descriptors for cross-sectional modalities (TID 1604)
     ** @param  tree     subtree to which the content items should be added
     *  @param  dataset  DICOM dataset from which the values should be copied
     *  @param  check    if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addCrossSectionalModalitiesDescriptors(DSRDocumentSubTree &tree,
                                                       DcmItem &dataset,
                                                       const OFBool check);

    /** add image library entry descriptors for CT (TID 1605)
     ** @param  tree     subtree to which the content items should be added
     *  @param  dataset  DICOM dataset from which the values should be copied
     *  @param  check    if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addComputedTomographyDescriptors(DSRDocumentSubTree &tree,
                                                 DcmItem &dataset,
                                                 const OFBool check);

    /** add image library entry descriptors for MR (TID 1606)
     ** @param  tree     subtree to which the content items should be added
     *  @param  dataset  DICOM dataset from which the values should be copied
     *  @param  check    if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addMagneticResonanceDescriptors(DSRDocumentSubTree &tree,
                                                DcmItem &dataset,
                                                const OFBool check);

    /** add image library entry descriptors for PET (TID 1607).
     *  @note The template rows 10 to 15 are not yet supported by this method.
     ** @param  tree     subtree to which the content items should be added
     *  @param  dataset  DICOM dataset from which the values should be copied
     *  @param  check    if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addPositronEmissionTomographyDescriptors(DSRDocumentSubTree &tree,
                                                         DcmItem &dataset,
                                                         const OFBool check);

    /** go to the most recently added image library entry and check the value of the
     *  descriptor 'Modality' (TID 1602 - Row 1)
     ** @param  modalityCode  coded entry (from CID 29) associated with the modality.
     *                        Used to check the image library entry.
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition goAndCheckImageLibraryEntry(const DSRCodedEntryValue &modalityCode);

    /** search for a particular content item (given by the concept name and value type)
     *  and set its string value.  If not found, a new content item is added to the
     *  current position and the value is set.
     ** @param  valueType       value type of the content item to be set/added
     *  @param  conceptName     concept name of the content item to be set/added
     *  @param  stringValue     string value of the content item to be set/added
     *  @param  annotationText  text used to annotate the content item (might be empty)
     *  @param  check           if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setStringContentItemFromValue(const E_ValueType valueType,
                                              const DSRCodedEntryValue &conceptName,
                                              const OFString &stringValue,
                                              const OFString &annotationText,
                                              const OFBool check);

    /** search for a particular CODE content item (given by the concept name) and set its
     *  coded entry value.  If not found, a new content item is added to the current
     *  position and the value is set.
     ** @param  conceptName     concept name of the content item to be set/added
     *  @param  codeValue       coded entry value of the content item to be set/added
     *  @param  annotationText  text used to annotate the content item (might be empty)
     *  @param  check           if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setCodeContentItemFromValue(const DSRCodedEntryValue &conceptName,
                                            const DSRCodedEntryValue &codeValue,
                                            const OFString &annotationText,
                                            const OFBool check);

    /** search for a particular NUM content item (given by the concept name) and set its
     *  numeric measurement value.  If not found, a new content item is added to the
     *  current position and the value is set.
     ** @param  conceptName      concept name of the content item to be set/added
     *  @param  numericValue     numeric measurement value of the content item to be
     *                           set/added
     *  @param  measurementUnit  measurement unit that should be used by 'numericValue'
     *                           (optional, might be empty).  Used for checking purposes.
     *  @param  annotationText   text used to annotate the content item (might be empty)
     *  @param  check            if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition setNumericContentItemFromValue(const DSRCodedEntryValue &conceptName,
                                               const DSRNumericMeasurementValue &numericValue,
                                               const DSRCodedEntryValue &measurementUnit,
                                               const OFString &annotationText,
                                               const OFBool check);

  // --- static helper functions ---

    /** add a content item with a string value copied from the given dataset.
     *  The content item is only created and added to the subtree if the specified data
     *  element is present in the 'dataset' and its string value is not empty.
     ** @param  tree            subtree to which the content items should be added
     *  @param  dataset         DICOM dataset from which the values should be copied
     *  @param  tagKey          DICOM tag specifying the attribute from which the string
     *                          value should be retrieved
     *  @param  pos             index of the value to get (0..vm-1), -1 for all components
     *  @param  valueType       value type of the content item to be added
     *  @param  conceptName     concept name of the content item to be added
     *  @param  annotationText  optional text used to annotate the content item
     *  @param  check           if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition addStringContentItemFromDataset(DSRDocumentSubTree &tree,
                                                       DcmItem &dataset,
                                                       const DcmTagKey &tagKey,
                                                       const signed long pos,
                                                       const E_ValueType valueType,
                                                       const DSRCodedEntryValue &conceptName,
                                                       const OFString &annotationText,
                                                       const OFBool check);

    /** add a CODE content item with a coded entry value copied from the given dataset.
     *  The content item is only created and added to the subtree if the specified code
     *  sequence is present in the 'dataset' and its value is valid (not empty).
     ** @param  tree            subtree to which the content items should be added
     *  @param  dataset         DICOM dataset from which the values should be copied
     *  @param  tagKey          DICOM tag specifying the code sequence attribute from which
     *                          the coded entry should be retrieved
     *  @param  conceptName     concept name of the content item to be added
     *  @param  annotationText  optional text used to annotate the content item
     *  @param  check           if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition addCodeContentItemFromDataset(DSRDocumentSubTree &tree,
                                                     DcmItem &dataset,
                                                     const DcmTagKey &tagKey,
                                                     const DSRCodedEntryValue &conceptName,
                                                     const OFString &annotationText,
                                                     const OFBool check);

    /** add a NUM content item with a numeric value copied from the given dataset.
     *  The content item is only created and added to the subtree if the specified data
     *  element is present in the 'dataset' and its value (retrieved as a string) is not
     *  empty.
     ** @param  tree             subtree to which the content items should be added
     *  @param  dataset          DICOM dataset from which the values should be copied
     *  @param  tagKey           DICOM tag specifying the attribute from which the string
     *                           value should be retrieved
     *  @param  pos              index of the value to get (0..vm-1), -1 for all
     *                           components
     *  @param  conceptName      concept name of the content item to be added
     *  @param  measurementUnit  measurement unit to be used for the numeric content item
     *  @param  annotationText   optional text used to annotate the content item
     *  @param  check            if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    static OFCondition addNumericContentItemFromDataset(DSRDocumentSubTree &tree,
                                                        DcmItem &dataset,
                                                        const DcmTagKey &tagKey,
                                                        const signed long pos,
                                                        const DSRCodedEntryValue &conceptName,
                                                        const DSRCodedEntryValue &measurementUnit,
                                                        const OFString &annotationText,
                                                        const OFBool check);
};


/*-------------------*
 *  type definition  *
 *-------------------*/

// define short name for the SR template class
typedef TID1600_ImageLibrary CMR_TID1600;


#endif
