/*
 *
 *  Copyright (C) 2015, J. Riesmeier, Oldenburg, Germany
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


/*-----------------------*
 *  constant definitions  *
 *-----------------------*/

/** @name specific error conditions for TID 1600 in module dcmsr/cmr
 */
//@{

/// error: there is no image library group to add image entries to
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_NoImageLibraryGroup;
/// error: cannot add multiple image library entry descriptors (see TID 1600 Row 3)
extern DCMTK_CMR_EXPORT const OFConditionConst CMR_EC_CannotAddMultipleImageLibraryEntryDescriptors;

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

    /** default constructor
     */
    TID1600_ImageLibrary();

    /** add an image group to the image library
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addImageGroup();

    /** add an image entry to the current image group, i.e.\ add content items for
     *  TID 1601 (Image Library Entry).  The values of the content items (including
     *  the image reference) are copied from the data elements of the given 'dataset'.
     ** @param  dataset  DICOM dataset from which the values should be copied
     *  @param  mode     mode specifying which optional content items are to be added
     *  @param  check    if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise.  If no
     *          image group exists, CMR_EC_NoImageLibraryGroup is returned.
     */
    OFCondition addImageEntry(DcmItem &dataset,
                              const AddImageMode mode = withoutDescriptors,
                              const OFBool check = OFTrue);

    /** add an image entry descriptors to the current image group, i.e.\ add content
     *  items for TID 1602 (Image Library Entry Descriptors) and included templates.
     *  The values of the content items are copied from the data elements of the given
     *  'dataset'.
     *  Please note that this method should only be called once for each image group.
     ** @param  dataset  DICOM dataset from which the values should be copied
     *  @param  check    if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise.  If no
     *          image group exists, CMR_EC_NoImageLibraryGroup is returned.
     */
    OFCondition addImageEntryDescriptors(DcmItem &dataset,
                                         const OFBool check = OFTrue);


  protected:

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
     *  @note The template rows 9 to 14 are not yet supported by this method.
     ** @param  tree     subtree to which the content items should be added
     *  @param  dataset  DICOM dataset from which the values should be copied
     *  @param  check    if enabled, check values for validity before setting them
     ** @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition addPositronEmissionTomographyDescriptors(DSRDocumentSubTree &tree,
                                                         DcmItem &dataset,
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

    /** add a content item with a coded entry value copied from the given dataset.
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

    /** add a content item with a numeric value copied from the given dataset.
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


#endif
