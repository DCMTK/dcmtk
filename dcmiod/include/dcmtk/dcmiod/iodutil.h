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
 *  Purpose: Static helper functionality for dcmiod module
 *
 */

#ifndef IODUTIL_H
#define IODUTIL_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/oftraits.h" // for OFremove_pointer
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/dcmdata/dcelem.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcdatutl.h"
#include "dcmtk/dcmiod/ioddef.h"
#include "dcmtk/dcmiod/iodrules.h"
#include "dcmtk/dcmiod/iodtypes.h"

/** Class with helper functions used within the dcmiod module (and beyond)
 */
class DCMTK_DCMIOD_EXPORT DcmIODUtil
{

public:

  /** Get element from dataset and check it for correct value multiplicity
   *  and type.
   *  @param  dataset DICOM dataset from which the element should be retrieved.
   *          (Would be 'const' if the methods from 'dcmdata' would also
   *          (be 'const'.)
   *  @param  delem  DICOM element used to store the value (always creates
   *          a copy of the dataset element's value)
   *  @param  vm Value multiplicity (according to the data dictionary) to be
   *          checked for. (See DcmElement::checkVM() for a list of valid
   *          values.) Interpreted as cardinality (number of items) for
   *          sequence attributes.
   *  @param  type Value type (valid value: "1", "1C", "2", something else
   *          which is not checked)
   *  @param  moduleName Optional module name to be printed (NULL: no module
   *          printed at all)
   *  @return EC_Normal if element could be retrieved and value is correct,
   *          an error code otherwise
   */
  static OFCondition getAndCheckElementFromDataset(DcmItem &dataset,
                                                   DcmElement &delem,
                                                   const OFString &vm,
                                                   const OFString &type,
                                                   const char *moduleName = NULL);

  /** Get element from dataset and check it for correct value multiplicity and
   *  type.
   *  @param  dataset DICOM dataset from which the element should be retrieved.
   *          (Would be 'const' if the methods from 'dcmdata' would also
   *          be 'const')
   *  @param  tagKey Tag key of the element to get
   *  @param  delem DICOM element that is set to a copy of the dataset's
   *          orinal element
   *  @param  vm Value multiplicity (according to the data dictionary) to be
   *          checked for. (See DcmElement::checkVM() for a list of valid
   *          values.) Interpreted as cardinality (number of items) for
   *          sequence attributes.
   *  @param  type Value type (valid value: "1", "1C", "2", something else
   *          which is not checked)
   *  @param  moduleName Optional module name to be printed (NULL: no module
   *          printed at all)
   *  @return EC_Normal if element could be retrieved and value is correct, an
   *          error code otherwise
   */
  static OFCondition getAndCheckElementFromDataset(DcmItem &dataset,
                                                   const DcmTagKey& tagKey,
                                                   DcmElement*& delem,
                                                   const OFString &vm,
                                                   const OFString &type,
                                                   const char *moduleName = NULL);

  /** Get element from dataset and check it for correct value multiplicity
   *  and type.
   *  @param  dataset DICOM dataset from which the element should be retrieved.
   *          (Would be 'const' if the methods from 'dcmdata' would also
   *          be 'const'.)
   *  @param  delem DICOM element used to store the value (always creates
   *          a copy of the value from the original element)
   *  @param  rule  Rule describing parameters to be checked on element
   *  @return EC_Normal if element could be retrieved and value is correct, an
   *          error code otherwise
   */
  static OFCondition getAndCheckElementFromDataset(DcmItem &dataset,
                                                   DcmElement &delem,
                                                   const IODRule* rule);

  /** Get element from dataset and check it for correct value multiplicity
   *  and type.
   *  @param  dataset DICOM dataset from which the element should be retrieved.
   *          (Would be 'const' if the methods from 'dcmdata' would also
   *          be 'const')
   *  @param  delem DICOM element that is set to a copy of the dataset's
   *          orinal element
   *  @param  rule  Rule describing parameters to be checked on element.
   *  @return EC_Normal if element could be retrieved and value is correct, an
   *          error code otherwise
   */
  static OFCondition getAndCheckElementFromDataset(DcmItem &dataset,
                                                   DcmElement*& delem,
                                                   const IODRule* rule);

  /** Get current date in DICOM 'DA' format (YYYYMMDD)
   *  @param  dateString String used to store the current date.
   *          (empty string if current date could not be retrieved)
   *  @return Resulting character string
   */
  static const OFString& currentDate(OFString &dateString);

  /** Get current time in DICOM 'TM' format (HHMMSS)
   *  The optional UTC notation (e.g.\ +0100) is currently not supported.
   *  @param  timeString String used to store the current time
   *                     (empty string if current time could not be retrieved)
   *  @return Resulting character string, empty if time could not be retrieved
   */
  static const OFString& currentTime(OFString &timeString);

  /** Copy given element to the dataset
   *  The element is only added if 'result' is EC_Normal. A copy of the given
   *  element is created, i.e.\ the caller is responsible for deleting the
   *  original element handed to this function (if desired).
   *  @param  result Reference to status variable (checked before adding and
   *          updated afterwards!)
   *  @param  dataset Reference to DICOM dataset to which the element should
   *          be added
   *  @param  delem Reference to DICOM element which should be added; a
   *          copy is created so caller is responsible for deleting delem (if
   *          desired).
   *  @param  vm Value multiplicity (according to the data dictionary) to be
   *          checked for. (See DcmElement::checkVM() for a list of valid
   *          values.). Interpreted as cardinality (number of items) for
   *          sequence attributes.
   *  @param  type  Value type (valid value: "1", "2" or something else which
   *          is not checked)
   *  @param  moduleName Optional module name to be printed (NULL: no module
   *          printed at all)
   *  @return Current value of 'result', EC_Normal if successful,
   *          an error code otherwise
   */
  static OFCondition copyElementToDataset(OFCondition &result,
                                          DcmItem &dataset,
                                          const DcmElement &delem,
                                          const OFString &vm,
                                          const OFString &type,
                                          const char *moduleName = NULL);

  /** Copy given element to the dataset. The element is only added if 'result'
   *  is EC_Normal.
   *  @param  result reference to status variable (checked before adding and
   *          updated afterwards!)
   *  @param  dataset reference to DICOM dataset to which the element
   *          should be added
   *  @param  delem reference to DICOM element which should be added; the value
   *          is copied from the original dataset's element
   *  @param  rule  Rule describing parameters to be checked on element.
   *  @return Current value of 'result', EC_Normal if successful, an error code
   *          otherwise
   */
  static OFCondition copyElementToDataset(OFCondition &result,
                                          DcmItem &dataset,
                                          const DcmElement &delem,
                                          const IODRule* rule);

  /** Add given element to the dataset
   *  The element is only added if 'result' is EC_Normal and the 'delem'
   *  pointer is not NULL.
   *  @param  result Reference to status variable (checked before adding and
   *          updated afterwards!)
   *  @param  dataset Reference to DICOM dataset to which the element should
   *          be added
   *  @param  delem Pointer to DICOM element which should be added. The element
   *          is always consumed by this function, i.e.\ insertion was successful
   *          and therefore ownership is transferred to the dataset, or the
   *          element is deleted from memory if it could not be inserted.
   *  @param  rule  Rule describing parameters to be checked on element.
   *  @return Current value of 'result', EC_Normal if successful, an error code otherwise
   */
  static OFCondition addElementToDataset(OFCondition &result,
                                         DcmItem &dataset,
                                         DcmElement *delem,
                                         const IODRule* rule);

  /** Check element value for correct value multiplicity and type.
   *  @param  delem Pointer to DICOM element to be checked (might be NULL)
   *  @param  tagKey DICOM tag of the DICOM element the parameter 'delem' points to
   *  @param  vm Value multiplicity (according to the data dictionary) to be checked for.
   *          (See DcmElement::checkVM() for a list of valid values.)
   *          Interpreted as cardinality (number of items) for sequence attributes.
   *  @param  type Value type (valid value: "1", "1C", "2", something else)
   *  @param  searchCond Optional flag indicating the status of a previous 'search' function call
   *  @param  moduleName Optional module name to be printed (default: "IOD" if NULL)
   *  @return EC_Normal if element value is correct, error otherwise
   */
  static OFCondition checkElementValue(const DcmElement *delem,
                                       const DcmTagKey &tagKey,
                                       const OFString &vm,
                                       const OFString &type,
                                       const OFCondition &searchCond = EC_Normal,
                                       const char *moduleName = NULL);

  /** Check element value for correct value multiplicity and type.
   *  @param  delem DICOM element to be checked
   *  @param  vm Value multiplicity (according to the data dictionary) to be checked for.
   *          (See DcmElement::checkVM() for a list of valid values.)
   *          Interpreted as cardinality (number of items) for sequence attributes.
   *  @param  type Value type (valid value: "1", "1C", "2", something else)
   *  @param  searchCond Optional flag indicating the status of a previous 'search' function call
   *  @param  moduleName Optional module name to be printed (default: "IOD" if NULL)
   *  @return EC_Normal if element value is correct, error otherwise
   */
  static OFCondition checkElementValue(const DcmElement &delem,
                                       const OFString &vm,
                                       const OFString &type,
                                       const OFCondition &searchCond = EC_Normal,
                                       const char *moduleName = NULL);

  /** Get string value from element
   *  @param  delem DICOM element from which the string value should be retrieved
   *  @param  stringValue Reference to variable in which the result should be stored.
   *          (This parameter is automatically cleared if an error occurs.)
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition getStringValueFromElement(const DcmElement &delem,
                                               OFString &stringValue,
                                               const signed long pos);

  /** Get string value from item
   *  @param  key The tag key of the attribute whose value should be retrieved
   *  @param  item The item to search the attribute in
   *  @param  result Reference to variable in which the result should be stored.
   *          (This parameter is automatically cleared if an error occurs.)
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition getStringValueFromItem(const DcmTagKey& key,
                                            DcmItem& item,
                                            OFString& result,
                                            const signed long& pos);

  /** Get Float64 value from item
   *  @param  key The tag key of the attribute whose value should be retrieved
   *  @param  item The item to search the attribute in
   *  @param  result Reference to variable in which the result should be stored.
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition getFloat64ValueFromItem(const DcmTagKey& key,
                                             DcmItem& item,
                                             Float64& result,
                                             const unsigned long& pos);

  /** Get Float64 values from item
   *  @param  key The tag key of the attribute whose value should be retrieved
   *  @param  item The item to search the attribute in
   *  @param  result Reference to variable in which the result should be stored.
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition getFloat64ValuesFromItem(const DcmTagKey& key,
                                              DcmItem& item,
                                              OFVector<Float64>& result);

  /** Get Float64 value from element
   *  @param  delem The element whose value should be retrieved
   *  @param  result Reference to variable in which the result should be stored.
   *  @param  pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition getFloat64ValueFromElement(const DcmElement &delem,
                                                Float64& result,
                                                const unsigned long pos);

  /** Get Float64 values from element
   *  @param  delem The element to get the value from
   *  @param  result Reference to variable in which the result should be stored.
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition getFloat64ValuesFromElement(const DcmElement &delem,
                                                 OFVector<Float64>& result);

  /** Set a DcmElement's content from Uint16 vector
   *  @param  delem DICOM element to be filled
   *  @param  values Vector use as a source for the values
   *  @param  vm Value multiplicity (according to the data dictionary) to be
   *           checked for. (See DcmElement::checkVM() for a list of
   *           valid values.)
   *  @param  check If OFTrue, then it is checked whether number of values
   *          conforms to the value provided by the vm parameter.
   *  @return Dtatus, EC_Normal if successful, an error code otherwise
   */
  static OFCondition setUint16ValuesOnElement(DcmElement &delem,
                                              const OFVector<Uint16>& values,
                                              const OFString& vm,
                                              const OFBool check);

  /** Get Uint16 values from DcmElement
   *  @param  delem  DICOM element to read from
   *  @param  values Vector used as destination for the values
   *  @return Status EC_Normal if successful, an error code otherwise
   */
  static OFCondition getUint16ValuesFromElement(DcmElement &delem,
                                                OFVector<Uint16>& values);

  /** Returns single item from given sequence. Returns error number of items
   *  is 0 or more than 1.
   *  @param  seq Sequence to read from.
   *  @param  item The resulting item, NULL in case of error
   *  @param  checkKey If given it is checked whether the given seq parameter
   *          matches the tag key provided in the checkKey parameter. If
   *          not, an error is returned.
   *  @return Status EC_Normal if successful, an error code otherwise
   */
  static OFCondition getAndCheckSingleItem(DcmSequenceOfItems& seq,
                                           DcmItem*& item,
                                           const DcmTagKey& checkKey = DCM_UndefinedTagKey);

  /** Copies Uint8 values from given element to destination container
   *  (must support push_back() method).
   *  @param elem the element to read from. If NULL, an error is returned.
   *  @param destination the container to store the Uint8 values to
   *  @return EC_Normal if successful, an error code otherwise
   */
  template <class Container>
  static OFCondition copyFromUint8Array(DcmElement* elem,
                                        Container& destination)
  {
    if (elem == NULL)
    return EC_IllegalParameter;

    size_t vm = elem->getVM();
    OFCondition result;
    for (size_t count = 0; (count < vm) && result.good(); count ++)
    {
      Uint8 value;
      result = elem->getUint8(value, count);
      if ( result.good() )
      {
        destination.push_back(value);
      }
    }
    if ( result.bad() )
    {
      destination.clear();
    }
    return result;
  }

  /** Check whether SOP class UID matches the expected value
   *  @param  item Item to read from. NULL value causes error return value.
   *  @param  desiredSOPClass The value that is expected
   *  @param  valueFound The value actually found (empty if no value could
   *          be retrieved)
   *  @return EC_Normal if value could be found and equals expected value,
   *          EC_TagNotFound if SOP Class UID is not found in dataset,
   *          EC_InvalidValue if SOP class differs from expected value.
   */
  static OFCondition checkSOPClass(DcmItem* item,
                                   const OFString& desiredSOPClass,
                                   OFString& valueFound);

  /** Check whether given tag is a sequence tag
   *  @param  key the tag key to be checked
   *  @param  privateCreator The private creator to check for if tag is private
   *  @return OFTrue if given tag is a sequence tag, OFFalse otherwise
   */
  static OFBool isSequenceTag(const DcmTagKey& key,
                              const OFString& privateCreator = "");

  /** Reads items from DICOM sequence into container by creating a dedicated
   *  container item for each DICOM item. The container must support
   *  push_back(T) function and the container's element type T must support
   *  the read(DcmItem*) function.
   *  @param  source The source sequence to read items from
   *  @param  seqKey The sequence's tag key that is expected
   *  @param  destination The destination container to read into
   *  @param  cardinality Expected number of items.
   *          See DcmElement::checkVM() for a list of valid values.
   *  @param  type The sequence type as noted in part 3 of the DICOM standard,
   *          i.e.\ "1,1C,2,2C or 3".
   *  @param  module Name of the module/macro this sequence is contained in. Used
   *          for error messages and can also be left empty.
   *  @return EC_Normal if successful, an error code otherwise
   */
  template <class Container>
  static OFCondition readSubSequence(DcmSequenceOfItems* source,
                                    const DcmTagKey& seqKey,
                                    Container& destination,
                                    const OFString& cardinality,
                                    const OFString& type,
                                    const OFString& module)
  {
    OFCondition result;
    OFCondition exists = EC_Normal;
    if (!source)
      exists = EC_TagNotFound;

    /* Check sequence and report errors as warnings, read anyway */
    checkElementValue(source, seqKey, cardinality, type, exists, module.c_str());
    if (source)
    {
      DcmItem *item = OFstatic_cast(DcmItem*, source->nextInContainer(NULL));
      size_t count = 0;
      while (item != NULL)
      {
        if (item != NULL)
        {
          // define the element type
          typedef typename OFremove_pointer<typename Container::value_type>::type Element;
          Element *newElem = new Element();
          // read into container item (clears old data first)
          if ( newElem != NULL)
          {
            result = (newElem)->read(*item, OFFalse /* no need to delete anything */);
            if ( result.good() )
            {
              destination.push_back(newElem);
            }
            else
            {
              delete newElem;
              DCMIOD_WARN("Could not read item #" << count << " from " << DcmTag(source->getTag()).getTagName() << " (skipping item): " << result.text());
            }
          }
          else
          {
            DCMIOD_ERROR("Could not store data from item #" << count << " from " << DcmTag(source->getTag()).getTagName() << " (skipping item): Memory exhausted?");
          }
        }
        else
        {
          DCMIOD_WARN("Could not get item #" << count << " from " <<  DcmTag(source->getTag()).getTagName() << " (malformed data or internal error), skipping item");
        }
        item = OFstatic_cast(DcmItem*, source->nextInContainer(item));
        count++;
      }
    }
    else
    {
      result = EC_IllegalParameter;
    }
    return result;
  }


  /** Reads DICOM sequence into container by creating a dedicated container element
   *  for each DICOM item. The container must support push_back(T) function
   *  and the container's element type T must support the read(DcmItem*) function.
   *  @param  source The source DICOM item read the sequence from
   *  @param  seqKey The tag key of the sequence to be read
   *  @param  destination The destination container to read into
   *  @param  cardinality Expected number of items.
   *          See DcmElement::checkVM() for a list of valid values.
   *  @param  type The sequence type as noted in part 3 of the DICOM standard,
   *          i.e.\ "1,1C,2,2C or 3".
   *  @param  module Nname of the module/macro this sequence is contained in. Used
   *          for error messages and can also be left empty.
   *  @return status EC_Normal if successful, an error code otherwise
   */
  template <class Container>
  static OFCondition readSubSequence(DcmItem& source,
                                     const DcmTagKey& seqKey,
                                     Container& destination,
                                     const OFString& cardinality,
                                     const OFString& type,
                                     const OFString& module)
  {
    OFCondition result;
    DcmSequenceOfItems *seq = NULL;

    /* Get sequence and read it */
    source.findAndGetSequence(seqKey, seq);
    result = readSubSequence(seq, seqKey, destination, cardinality, type, module );
    return result;
  }


  /** Reads DICOM sequence into container by creating a dedicated container
   *   element for each DICOM item. The container must support the
   *   push_back(T) function and the container's element type T must support
   *   the read(DcmItem*) function.
    *  @param  source The source DICOM item read the sequence from
    *  @param  seqKey The tag key of the sequence to be read
    *  @param  destination The destination container to read into
    *  @param  rule The rule for reading this sequence. If NULL, an error is returned.
    *  @return EC_Normal if successful, an error code otherwise
    */
  template <class Container>
  static OFCondition readSubSequence(DcmItem& source,
                                     const DcmTagKey& seqKey,
                                     Container& destination,
                                     IODRule *rule)
  {
    if (rule == NULL)
    {
      DCMIOD_ERROR("Cannot read sequence " << seqKey << " (no rule supplied)");
      return IOD_EC_NoSuchRule;
    }
    return readSubSequence<Container>(source, seqKey, destination, rule->getVM(), rule->getType(), rule->getModule());
  }


  /** Reads single DICOM item from a specific sequence into a destination class.
   *  The container must support the read(DcmItem) function. If a single item
   *  (i.e.\ the first item of the specified sequence) cannot be read, an error
   *  is returned.
   *  @param  source The source DICOM item read the sequence from
   *  @param  seqKey The tag key of the sequence to be read
   *  @param  destination The destination container to read into
   *          See DcmElement::checkVM() for a list of valid values.
   *  @param  type The sequence type as noted in part 3 of the DICOM standard,
   *          i.e.\ "1,1C,2,2C or 3".
   *  @param  module Name of the module/macro this sequence is contained in.
   *          Used for error messages and can also be left empty.
   *  @return EC_Normal if successful, an error code otherwise
   */
  template <class Container>
  static OFCondition readSingleItem(DcmItem& source,
                                    const DcmTagKey& seqKey,
                                    Container& destination,
                                    const OFString& type,
                                    const OFString& module)
  {
    OFCondition result;
    /* Check sequence, reports cardinality and type errors as warnings */
    checkSubSequence(result, source, seqKey, "1", type, module);

    /* Try to read sequence into internal data (ignore errors as much as possible) */
    DcmItem* item = NULL;
    result = source.findAndGetSequenceItem(seqKey, item, 0);
    if (item != NULL)
    {
      // read into Container (clears old data first)
      result = destination.read(*item, OFTrue /* clear old data */);
    }
    return result;
  }

  /** Reads single DICOM item from a specific sequence into a destination class.
   *  The container must support the read(DcmItem) function. If a single item
   *  (i.e.\ the first item of the specified sequence) cannot be read, an error
   *  is returned.
   *  @param  source The source DICOM item read the sequence from
   *  @param  seqKey The tag key of the sequence to be read
   *  @param  destination The destination container to read into
   *          See DcmElement::checkVM() for a list of valid values.
   *  @param  rule The rule (for the sequence) used for reading.
   *  @return EC_Normal if successful, an error code otherwise.  If the rule
   *          equlas NULL, then nothing is read at all and an error is returned
   *          (IOD_EC_NoSuchRule), but no error is reported to the logger.
   */
  template <class Container>
  static OFCondition readSingleItem(DcmItem& source,
                                    const DcmTagKey& seqKey,
                                    Container& destination,
                                    IODRule *rule = NULL)
  {
    if (rule == NULL)
    {
      DCMIOD_DEBUG("Will not read sequence " << seqKey << ": No rule supplied");
      return IOD_EC_NoSuchRule;
    }

    return readSingleItem(source, seqKey, destination, rule->getType(), rule->getModule());
  }

  /** Write given container into a DICOM sequence that is created within the
   *  given item. The sequence is created from scratch, i.e.\ any old sequence
   *  (and its items) will be overwritten.
   *  @param  result If writing is successful, result will contain EC_Normal.
   *          Otherwise an error code is set.
   *  @param  seqKey The tag key of the sequence to be write
   *  @param  source The source container to read from. Must support the
   *          write(DcmItem*) function.
   *  @param  destination The DICOM destination item to write the sequence to
   *          See DcmElement::checkVM() for a list of valid values.
   *  @param  cardinality  Expected number of items (i.e.\ expected number of
   *          elements in source container). See DcmElement::checkVM() for a
   *          list of valid values.
   *  @param  type The sequence type as noted in part 3 of the DICOM standard,
   *          i.e.\ "1,1C,2,2C or 3".
   *  @param  module Name of the module/macro this sequence is contained in. Used
   *                 for error messages and can also be left empty.
   *  @return EC_Normal if successful, an error code otherwise
   */
  template <class Container>
  static void writeSubSequence(OFCondition& result,
                               const DcmTagKey& seqKey,
                               Container& source,
                               DcmItem& destination,
                               const OFString& cardinality,
                               const OFString& type,
                               const OFString& module)
  {
    if ( result.good() )
    {
      // Delete old data
      destination.findAndDeleteElement(seqKey);

      // If we have meaningful data, write it
      OFBool haveData = source.size() > 0;
      if (haveData)
      {
        destination.insertEmptyElement(seqKey);
        Uint32 count = 0;
        typename Container::iterator it = source.begin();
        while ( it != source.end() && result.good() )
        {
          if ( (*it) != NULL)
          {
            DcmItem *localItem = NULL;
            // If item is NULL result.bad() is always true.
            result = destination.findOrCreateSequenceItem(seqKey, localItem, -2 /* append new */);
            if ( result.good() )
            {
              result = (*it)->write(*localItem);
              if ( result.bad() )
              {
                  destination.findAndDeleteSequenceItem(seqKey, -1 /* last */);
              }
            }
            else
            {
                DCMIOD_ERROR("Could not insert item data #" << count << " of " << DcmTag(seqKey).getTagName() << " (internal error), ignoring");
            }
          }
          else
          {
            DCMIOD_ERROR("Found uninitialized container item (NULL value) for sequence " << DcmTag(seqKey).getTagName() << " (internal error, skipping)");
          }
          count++;
          it++;
        }
      }
      // If we do not have data, create empty sequence if type 2
      else if (type == "2")
      {
        destination.insertEmptyElement(seqKey);
      }

      // Check result
      checkSubSequence(result, destination, seqKey, cardinality, type, module);

      // Clean up if we did not have success */
      if (result.bad())
      {
        destination.findAndDeleteElement(seqKey);
      }
    }
  }

  /** Write given container into a DICOM sequence that is created within the given item.
   *  The container. The sequence is created from scratch, i.e.\ any old sequence
   *  (and its items) will be overwritten.
   *  @param  result If writing is successful, result will contain EC_Normal. Otherwise
   *          an error code is set.
   *  @param  seqKey The tag key of the sequence to be write
   *  @param  source The source container to read from. Must support the write(DcmItem*)
   *          function.
   *  @param  destination The DICOM destination item to write the sequence to
   *  @param  rule Rule describing the requirements for this sequence. If NULL
   *          an error is returned (IOD_EC_NoSuchRule), but no error error is reported
   *          to the logger.
   *  @return EC_Normal if successful, an error code otherwise
   */
  template <class Container>
  static void writeSubSequence(OFCondition& result,
                               const DcmTagKey& seqKey,
                               Container& source,
                               DcmItem& destination,
                               IODRule* rule)
  {
    if ( result.good() )
    {
      if (rule == NULL)
      {
        DCMIOD_DEBUG("Will not write sequence " << seqKey << ": No rule supplied");
        result = IOD_EC_NoSuchRule;
      }
      else
      {
        writeSubSequence<Container>(result, rule->getTagKey(), source, destination, rule->getVM(), rule->getType(), rule->getModule());
      }
    }
  }


  /** Writes given container into a DICOM item of a specific sequence. The
   *  sequence is created from scratch so that any old information will be lost.
   *  @param  result If writing is successful, result will contain EC_Normal.
   *          Otherwise an error code is set.
   *  @param  seqKey The tag key of the sequence to be written
   *  @param  source The source container to read from. Must support the write(DcmItem*)
   *          function.
   *  @param  destination The DICOM item that should hold the sequence
   *          (with a single item) in the end.
   *  @param  type The sequence type as noted in part 3 of the DICOM standard,
   *          i.e.\ "1,1C,2,2C or 3".
   *  @param  module Name of the module/macro this sequence is contained in.
   *          Used for error messages and can also be left empty.
   *  @return EC_Normal if successful, an error code otherwise
   */
  template <class Container>
  static void writeSingleItem(OFCondition& result,
                              const DcmTagKey& seqKey,
                              Container& source,
                              DcmItem& destination,
                              const OFString& type,
                              const OFString& module)
  {
    if ( result.good() )
    {
      // Delete old data
      destination.findAndDeleteElement(seqKey);

      /* If we have data, write it */
      OFCondition haveData = source.check(OFTrue /* Be quiet */);
      if (haveData.good())
      {
        DcmItem *localItem = NULL;
        // If item is NULL result.bad() is always true.
        result = destination.findOrCreateSequenceItem(seqKey, localItem, 0);
        if ( result.good() )
        {
          result = source.write(*localItem);
          // It can happen that check() returns OK but no elements have to be
          // written at all (e.g.\ if it contains only type 3 attributes). In that
          // case a sequence with a single empty item is written which must be removed
          // afterwards.
          if (result.good() && (localItem->card() == 0) )
          {
            destination.findAndDeleteElement(seqKey);
          }
        }
      }
      /* If we do not have data, insert empty for type 2 */
      else if (type == "2")
      {
          destination.insertEmptyElement(seqKey);
      }
      else if (type == "1C")
      {
        DCMIOD_TRACE("Skipping type 1C sequence " << seqKey << ": No data or incomplete data available");
      }
      /* Check outcome */
      checkSubSequence(result, destination, seqKey, "1", type, module);
    }
  }

  /** Writes given container into a DICOM item of a specific sequence.
   *  The sequence is created from scratch so that any old information
   *  will be lost.
   *  @param  result If writing is successful, result will contain EC_Normal.
   *          Otherwise an error code is set.
   *  @param  seqKey The tag key of the sequence to be written
   *  @param  source The source container to read from. Must support the
   *          write(DcmItem*) function.
   *  @param  destination The DICOM item that should hold the sequence
   *          (with a single item) in the end.
   *  @param  rule The rule for writing the given sequence
   *  @return EC_Normal if successful, an error code otherwise
   */
  template <class Container>
  static void writeSingleItem(OFCondition& result,
                              const DcmTagKey& seqKey,
                              Container& source,
                              DcmItem& destination,
                              IODRule *rule)
  {
    if (result.good())
    {
      if (rule == NULL)
      {
        DCMIOD_ERROR("Cannot write sequence " << seqKey << " (no rule supplied)");
        result = EC_CannotCheck;
      }
      writeSingleItem(result, seqKey, source, destination, rule->getType(), rule->getModule());
    }
  }


  /** Check whether a given sequence exists in a given item and whether it conforms to
   *  to its requirement type
   *  @param  result If sequence is valid, result is set to EC_Normal, otherwise
   *          to an error.
   *  @param  surroundingItem The item that should contain the given sequence.
   *  @param  seqKey The sequence to look for
   *  @param  cardinality Expected number of items (i.e.\ expected number of
   *          elements in source container). See DcmElement::checkVM() for a
   *          list of valid values.
   *  @param  type The sequence type as noted in part 3 of the DICOM standard,
   *          i.e.\ "1,1C,2,2C or 3".
   *  @param  module Name of the module/macro this sequence is contained in.
   *          Used for error messages and can also be left empty.
   *  @return EC_Normal if successful, an error code otherwise
   */
  static void checkSubSequence(OFCondition& result,
                               DcmItem& surroundingItem,
                               const DcmTagKey& seqKey,
                               const OFString& cardinality,
                               const OFString& type,
                               const OFString& module);

  /** Deletes all elements from given container and calls "delete" on each
   *  of them to clear memory.
   *  @param container  The container that should be cleared. Must contain
   *         pointers to objects that are allocated on the heap.
   */
  template<class Container>
  static void freeContainer(Container& container)
  {
    typename Container::iterator it = container.begin();
    while (it != container.end())
    {
      delete *it;
      it++;
    }
    container.clear();
  }

  /** Clones and copies all elements from source to destination container by
   *  copy constructing all elements.
   *  @param src  The container that should be copied. Must contain pointers
   *         to objects that are allocated on the heap, and that are
   *         copy-constructible
   *  @param dst  The container to copy the cloned elements to.
   */
  template<class Container>
  static void copyContainer(const Container& src, Container& dst)
  {
    typedef typename OFremove_pointer<typename Container::value_type>::type Element;
    typename Container::const_iterator it = src.begin();
    while (it != src.end())
    {
      if (*it != NULL)
      {
        Element *elem = new Element(**it);
        dst.push_back ( elem );
      }
      else
      {
        DCMIOD_ERROR("Could not copy element while copying container: Element is NULL, skipping");
      }
      it++;
    }
  }

  /** Function that takes a string representation of a tag key and
   *  converts it to a tag key instance if possible
   *  @param  keyString String of the format "(gggg,eeee)"
   *  @return The tag key if it could be parsed, DCM_UndefinedTagKey is returned
   *          instead (0xffff,0xffff)
   */
  static const DcmTagKey parseTagKey(const OFString& keyString);

  /** Decompress given dataset if possible. Decompression codecs have to be
   *  registered beforehand.
   *  @param  dset The dataset to decompress
   *  @return EC_Normal if decompression works, error otherwise
   */
  static OFCondition decompress(DcmDataset& dset);

  /** Create new Unique Identifier (UID)
    *  @param  level 0: instance level, 1: Series level, >=2: Study level.
    *          Calling always with level=0 is not an error but will result
    *          in unique values, too.
    *  @return The UID created.
    */
  static OFString createUID(const Uint8 level = 0);

private:

  // We only have static functions so we do not need an instance of
  // this class so far.

  /** Undefined default constructor (only static functions)
   */
  DcmIODUtil();

  /** Undefined destructor
   */
  ~DcmIODUtil();
};

#endif // IODUTIL_H
