/*
 *
 *  Copyright (C) 2015, Open Connections GmbH
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
 *  Module:  dcmiod
 *
 *  Author:  Michael Onken, Andriy Fedorov
 *
 *  Purpose: Helper functions for copying common modules between datasets
 *
 */

#ifndef MODHELP_H
#define MODHELP_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftypes.h"

#include "dcmtk/dcmiod/ioddef.h"
// forward declarations
class DcmTagKey;
class DcmItem;


/** This class contains helper functions that permit copying common modules
 *  from DICOM standard part 3 between DICOM datasets.
 *  @note The attribute lists per module are taken from the final text version
 *        of DICOM 2013.
 */
class DCMTK_DCMIOD_EXPORT DcmModuleHelpers {

  protected:

    /// Generic copy module helper method.
    template<size_t N>
    static inline void copyModule(const DcmTagKey (&tags)[N],
                                  DcmItem& src, DcmItem& dest);

    /// List of tags within the Patient Module
    static const DcmTagKey patientModuleTags[];

    /// List of tags within the Clinical Trial Subject Module
    static const DcmTagKey clinicalTrialSubjectModuleTags[];

    /// List of tags within the General Study Module
    static const DcmTagKey generalStudyModuleTags[];

    /// List of tags within the Patient Study Module
    static const DcmTagKey patientStudyModuleTags[];

    /// List of tags within the Clinical Trial Study Module
    static const DcmTagKey clinicalTrialStudyModuleTags[];

    /// List of tags within the General Series Module
    static const DcmTagKey generalSeriesModuleTags[];

    /// List of tags within the Clinical Trial Series Module
    static const DcmTagKey clinicalTrialSeriesModuleTags[];

    /// List of tags within the General Equipment Module
    static const DcmTagKey generalEquipmentModuleTags[];

    /// List of tags within the Frame of Reference Module
    static const DcmTagKey frameOfReferenceModuleTags[];

    /// List of tags within the SOP Common Module, excluding Digital Signatures
    /// Macro, i.e.\ MAC Parameters Sequence and Digital Signatures Sequence
    static const DcmTagKey sopCommonModuleTags[];

    /// List of tags within the General Image Module
    static const DcmTagKey generalImageModuleTags[];

  public:

    /** Copy element defined by tag from source item to destination item.
     *  No in-depth search is performed but only the main level is searched
     *  for the given tag. If the tag is not found, the destination item
     *  is not touched at all.
     *  @param tag   Tag key of the element to be copied
     *  @param src   Item that serves as a source for copying the element.
     *               In typical image objects, this will be the main dataset.
     *  @param dest  Item that serves as a destination for inserting the
     *               copied element, if found.
     */
    static void copyElement(const DcmTagKey& tag, DcmItem& src, DcmItem& dest);

    /** Copy Patient Module attributes.
     *  @param src   Item to copy module from. Only the main level is searched,
     *               i.e.\ no in-depth search is performed on the various tags.
     *  @param dest  Item to copy module to. An element not found within
     *               the source item will neither be created in the destination
     *               item, nor will any existing value be touched in the
     *               destination item.
     */
    static void copyPatientModule(DcmItem& src, DcmItem& dest);

    /** Copy Clinical Trial Subject Module attributes.
     *  @param src   Item to copy module from. Only the main level is searched,
     *               i.e.\ no in-depth search is performed on the various tags.
     *  @param dest  Item to copy module to. An element not found within
     *               the source item will neither be created in the destination
     *               item, nor will any existing value be touched in the
     *               destination item.
     */
    static void copyClinicalTrialSubjectModule(DcmItem& src, DcmItem& dest);

    /** Copy General Study Module attributes.
     *  @param src   Item to copy module from. Only the main level is searched,
     *               i.e.\ no in-depth search is performed on the various tags.
     *  @param dest  Item to copy module to. An element not found within
     *               the source item will neither be created in the destination
     *               item, nor will any existing value be touched in the
     *               destination item.
     */
    static void copyGeneralStudyModule(DcmItem& src, DcmItem& dest);

    /** Copy Patient Study Module attributes.
     *  @param src   Item to copy module from. Only the main level is searched,
     *               i.e.\ no in-depth search is performed on the various tags.
     *  @param dest  Item to copy module to. An element not found within
     *               the source item will neither be created in the destination
     *               item, nor will any existing value be touched in the
     *               destination item.
     */
    static void copyPatientStudyModule(DcmItem& src, DcmItem& dest);

    /** Copy Clinical Trial Study Module attributes.
     *  @param src   Item to copy module from. Only the main level is searched,
     *               i.e.\ no in-depth search is performed on the various tags.
     *  @param dest  Item to copy module to. An element not found within
     *               the source item will neither be created in the destination
     *               item, nor will any existing value be touched in the
     *               destination item.
     */
    static void copyClinicalTrialStudyModule(DcmItem& src, DcmItem& dest);

    /** Copy General Series Module attributes.
     *  @param src   Item to copy module from. Only the main level is searched,
     *               i.e.\ no in-depth search is performed on the various tags.
     *  @param dest  Item to copy module to. An element not found within
     *               the source item will neither be created in the destination
     *               item, nor will any existing value be touched in the
     *               destination item.
     */
    static void copyGeneralSeriesModule(DcmItem& src, DcmItem& dest);

    /** Copy Clinical Trial Series Module attributes.
     *  @param src   Item to copy module from. Only the main level is searched,
     *               i.e.\ no in-depth search is performed on the various tags.
     *  @param dest  Item to copy module to. An element not found within
     *               the source item will neither be created in the destination
     *               item, nor will any existing value be touched in the
     *               destination item.
     */
    static void copyClinicalTrialSeriesModule(DcmItem& src, DcmItem& dest);

    /** Copy General Equipment Module attributes.
     *  @param src   Item to copy module from. Only the main level is searched,
     *               i.e.\ no in-depth search is performed on the various tags.
     *  @param dest  Item to copy module to. An element not found within
     *               the source item will neither be created in the destination
     *               item, nor will any existing value be touched in the
     *               destination item.
     */
    static void copyGeneralEquipmentModule(DcmItem& src, DcmItem& dest);

    /** Copy Frame of Reference Module attributes.
     *  @param src   Item to copy module from. Only the main level is searched,
     *               i.e.\ no in-depth search is performed on the various tags.
     *  @param dest  Item to copy module to. An element not found within
     *               the source item will neither be created in the destination
     *               item, nor will any existing value be touched in the
     *               destination item.
     */
    static void copyFrameOfReferenceModule(DcmItem& src, DcmItem& dest);

    /** Copy SOP Common Module attributes.
     *  Excludes: Digital Signatures Macro, i.e.\ MAC Parameters Sequence and
     *  Digital Signatures Sequence.
     *  @param src   Item to copy module from. Only the main level is searched,
     *               i.e.\ no in-depth search is performed on the various tags.
     *  @param dest  Item to copy module to. An element not found within
     *               the source item will neither be created in the destination
     *               item, nor will any existing value be touched in the
     *               destination item.
     */
    static void copySOPCommonModule(DcmItem& src, DcmItem& dest);

    /** Copy General Image Module attributes.
     *  @param src   Item to copy module from. Only the main level is searched,
     *               i.e.\ no in-depth search is performed on the various tags.
     *  @param dest  Item to copy module to. An element not found within
     *               the source item will neither be created in the destination
     *               item, nor will any existing value be touched in the

     *               destination item.
     */
    static void copyGeneralImageModule(DcmItem& src, DcmItem& dest);
};

#endif // MODHELP_H
