/*
 *
 *  Copyright (C) 2014, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Michael Onken, Andriy Fedorov
 *
 *  Purpose: Helper functions for copying common modules between datasets
 *
 */

#ifndef MODHELP_H
#define MODHELP_H

class DcmTagKey;
class DcmItem;

/** This class contains helper functions to permit copying common modules from
 *  DICOM standard part 3 between DICOM datasets. The attribute lists per
 *  module are taken over from the DICOM 2011 version of the standard if not
 *  noted otherwise.
 */
class DcmModuleHelpers {

  protected:

    /// List of tags within the Patient Module (as in DICOM 2011 + CP 1123)
    static const DcmTagKey patientModuleTags[];

    /// List of tags within the Clinical Trial Subject Module
    static const DcmTagKey clinicalTrialSubjectModuleTags[];

    /// List of tags within the General Study Module
    static const DcmTagKey generalStudyModuleTags[];

    /// List of tags within the Patient Study Module
    static const DcmTagKey patientStudyModuleTags[];

    /// List of tags within the General Series Module
    static const DcmTagKey generalSeriesModuleTags[];

    /// List of tags within the General Equipment Module
    static const DcmTagKey generalEquipmentModuleTags[];

    /// List of tags within the Frame of Reference Module
    static const DcmTagKey frameOfReferenceModuleTags[];

    /// List of tags within the SOP Common Module, excluding Digital Signatures
    /// Macro, i.e.  MAC Parameters Sequence and Digital Signatures Sequence
    static const DcmTagKey sopCommonModuleTags[];

    /// List of tags within the General Image Module (as in DICOM 2011)
    static const DcmTagKey generalImageModuleTags[];

  public:

    /** Copy element defined by tag from source item to destination item.
     *  No in-depth search is performed but only the main level is searched
     *  for the given tag. If the tag is not found, the destination item
     *  is not touched at all.
     *  @param DcmTagKey  Tag key of the element to be copied
     *  @param src  Item that serves as a a source for copying the element.
     *              In typical image objects, this will be the main dataset.
     *  @param dest  Item that serves as a destination for inserting the
     *               copied element, if found.
     */
    static void copyElement(const DcmTagKey, DcmItem& src, DcmItem& dest);

    /** Copy Patient Module attributes. Includes attributes from CP 1123.
     *  @param src  Item to copy module from. Only the main level is searched,
     *              i.e. no in-depth search is performed on the various tags.
     *  @param dest  Item to copy module to. An element not found within
     *               the source item will neither be created in the destination
     *               item, nor will any existing value be touched in the
     *               destination item.
     */
    static void copyPatientModule(DcmItem& src, DcmItem& dest);

    /** Copy Clinical Trial Subject Module attributes.
     *  @param src  Item to copy module from. Only the main level is searched,
     *              i.e. no in-depth search is performed on the various tags.
     *  @param dest  Item to copy module to. An element not found within
     *               the source item will neither be created in the destination
     *               item, nor will any existing value be touched in the
     *               destination item.
     */
    static void copyClinicalTrialSubjectModule(DcmItem& src, DcmItem& dest);

    /** Copy General Study Module attributes.
     *  @param src  Item to copy module from. Only the main level is searched,
     *              i.e. no in-depth search is performed on the various tags.
     *  @param dest  Item to copy module to. An element not found within
     *               the source item will neither be created in the destination
     *               item, nor will any existing value be touched in the
     *               destination item.
     */
    static void copyGeneralStudyModule(DcmItem& src, DcmItem& dest);

    /** Copy Patient Study Module attributes.
     *  @param src  Item to copy module from. Only the main level is searched,
     *              i.e. no in-depth search is performed on the various tags.
     *  @param dest  Item to copy module to. An element not found within
     *               the source item will neither be created in the destination
     *               item, nor will any existing value be touched in the
     *               destination item.
     */
    static void copyPatientStudyModule(DcmItem& src, DcmItem& dest);

    /** Copy General Series Module attributes.
     *  @param src  Item to copy module from. Only the main level is searched,
     *              i.e. no in-depth search is performed on the various tags.
     *  @param dest  Item to copy module to. An element not found within
     *               the source item will neither be created in the destination
     *               item, nor will any existing value be touched in the
     *               destination item.
     */
    static void copyGeneralSeriesModule(DcmItem& src, DcmItem& dest);

    /** Copy General Equipment Module attributes.
     *  @param src  Item to copy module from. Only the main level is searched,
     *              i.e. no in-depth search is performed on the various tags.
     *  @param dest  Item to copy module to. An element not found within
     *               the source item will neither be created in the destination
     *               item, nor will any existing value be touched in the
     *               destination item.
     */
    static void copyGeneralEquipmentModule(DcmItem& src, DcmItem& dest);

    /** Copy Frame of Reference Module attributes.
     *  @param src  Item to copy module from. Only the main level is searched,
     *              i.e. no in-depth search is performed on the various tags.
     *  @param dest  Item to copy module to. An element not found within
     *               the source item will neither be created in the destination
     *               item, nor will any existing value be touched in the
     *               destination item.
     */
    static void copyFrameOfReferenceModule(DcmItem& src, DcmItem& dest);

    /** Copy SOP Common Module attributes.
     *  Excludes: Digital Signatures Macro, i.e. MAC Parameters Sequence and
     *  Digital Signatures Sequence.
     *  @param src  Item to copy module from. Only the main level is searched,
     *              i.e. no in-depth search is performed on the various tags.
     *  @param dest  Item to copy module to. An element not found within
     *               the source item will neither be created in the destination
     *               item, nor will any existing value be touched in the
     *               destination item.
     */
    static void copySOPCommonModule(DcmItem& src, DcmItem& dest);

    /*  Copy General Image Module attributes.
     *  @param src  Item to copy module from. Only the main level is searched,
     *              i.e. no in-depth search is performed on the various tags.
     *  @param dest  Item to copy module to. An element not found within
     *               the source item will neither be created in the destination
     *               item, nor will any existing value be touched in the
     *               destination item.
     */
    static void copyGeneralImageModule(DcmItem& src, DcmItem& dest);

};

#endif // MODHELP_H
