/*
 *
 *  Copyright (C) 2024-2025, OFFIS e.V.
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
 *  Author:  Tingyan Xu, Marco Eichelberg
 *
 *  Purpose: Class for converting JSON DICOM documents to binary DICOM files
 *
 */

#ifndef DCJSONRD_H
#define DCJSONRD_H

class OFCondition;

#include "dcmtk/config/osconfig.h"      /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/oftypes.h"        /* for OFBool */
#include "dcmtk/dcmdata/dcdefine.h"     /* for DCMTK_DCMDATA_EXPORT */
#include "dcmtk/dcmdata/dcxfer.h"       /* for E_TransferSyntax */

#define JSMN_HEADER
#include "dcmtk/ofstd/ofjsmn.h"         /* for JSMN declarations  */

typedef jsmn_parser OFJsmnParser;
typedef jsmntok_t OFJsmnToken;
typedef OFJsmnToken *OFJsmnTokenPtr;

class DcmFileFormat;
class DcmElement;
class DcmTagKey;
class DcmTag;
class DcmSequenceOfItems;
class DcmItem;


/** input stream that reads from standard input
 */
class DCMTK_DCMDATA_EXPORT DcmJSONReader
{
public:
  /// constructor
  DcmJSONReader();

  /// destructor
  virtual ~DcmJSONReader();

  /// delete all internal buffers, but keep policy settings
  virtual void clear();

  /** read a JSON file and store the content in this object
   *  @param ifname name of the JSON file to be read
   *  @return EC_Normal upon success, an error code otherwise
   */
  virtual OFCondition readJSONFile(const char *ifname);

  /** read a JSON dataset from stdin and store the content in this object
   *  @return EC_Normal upon success, an error code otherwise
   */
  virtual OFCondition readJSONFromStdin();

  /** set the "ignore bulk data URI" policy
   *  @param value new policy, true = ignore bulk data URIs
   */
  virtual void setIgnoreBulkdataURIPolicy(OFBool value) { ignoreBulkdataURIPolicy_ = value; }

  /** set the "stop on error" policy
   *  @param value new policy, true = stop when encountering a parse error
   */
  virtual void setStopOnErrorPolicy(OFBool value) { stopOnErrorPolicy_ = value; }

  /** set the "ignore meta info" policy
   *  @param value new policy, true = ignore meta info elements in JSON dataset
   */
  virtual void setIgnoreMetaInfoPolicy(OFBool value) { ignoreMetaInfoPolicy_ = value; }

  /** set the array handling policy
   *  @param value new policy
   *    - -1: reject arrays with more than one dataset
   *    - 0: store arrays with multiple datasets as a private sequence
   *    - n > 0: select dataset n from the array, ignore all others   */
  virtual void setArrayHandlingPolicy(signed long value) { arrayHandlingPolicy_ = value; }

  /** set the transfer syntax for the dataset
   *  @param value transfer syntax
   */
  virtual void setTransferSyntax(E_TransferSyntax value) { xferSyntax_ = value; }

  /** set the transfer syntax for the dataset
   *  @param value transfer syntax
   */
  virtual E_TransferSyntax getTransferSyntax() const { return xferSyntax_; }

  /** parse the JSON file with the given filename
   *  @param fileformat DcmFileFormat instance to be populated with the parsed JSON content
   *  @param ifname name of the JSON file to be read
   *  @return EC_Normal upon success, an error code otherwise
   */
  virtual OFCondition readAndConvertJSONFile(
      DcmFileFormat& fileformat,
      const char *ifname);

  /** dump the token array produced by the JSMN parser to stderr
   */
  virtual OFCondition dumpJSONTokenArray();

private:

  /// null terminated character string containing the entire JSON dataset
  char *jsonDataset_;

  /// size of jsonDataset_ in bytes, not including the terminating null byte
  size_t jsonDatasetLen_;

  /// array of parsed JSON tokens
  OFJsmnTokenPtr tokenArray_;

  /// number of tokens in tokenArray_
  int tokenNumber_;

  /// policy for handling bulk data URIs (true = ignore)
  OFBool ignoreBulkdataURIPolicy_;

  /// policy for error handling (false = ignore)
  OFBool stopOnErrorPolicy_;

  /// policy for handling meta info elements in the JSON dataset (true = ignore)
  OFBool ignoreMetaInfoPolicy_;

  /** policy for handling arrays of multiple datasets.
   *    - -1: reject arrays with more than one dataset
   *    - 0: store arrays with multiple datasets as a private sequence
   *    - n > 0: select dataset n from the array, ignore all others
   */
  signed long arrayHandlingPolicy_;

  /// transfer syntax of the datset, default: LittleEndianExplicit
  E_TransferSyntax xferSyntax_;

  /** calculate the required number of tokens for the JSON dataset
   *  and allocate the token array accordingly
   */
  OFCondition reserveTokens();

  /** parse the JSON dataset that has be read into the buffer
   *  @return EC_Normal upon success, an error code otherwise
   */
  virtual OFCondition parseJSON();

  /** helper function to retrieve the content of the token.
   *  There is no check on boundaries!
   *  @param value the result string
   *  @param t token pointer
   */
  void getTokenContent(
      OFString& value,
      OFJsmnTokenPtr t);

  /** create DICOM element using the given tag and VR
   *  @param newElem pointer to newly created DICOM element returned in this parameter
   *  @param dcmTag attribute tag
   *  @param vr the string representation of the VR (if present)
   *  @return EC_Normal upon success, an error code otherwise
   */
  virtual OFCondition createElement(
      DcmElement *& newElem,
      DcmTag& dcmTag,
      const OFString& vr);

  /** extract DICOM tag from the given string
   *  @param keyToken token containing the string representation of the tag in form of "ggggeeee"
   *  @param tagkey stores the extracted DICOM tag
   *  @return EC_Normal upon success, an error code otherwise
   */
  virtual OFCondition extractTag(
      OFJsmnTokenPtr keyToken,
      DcmTagKey& tagkey);

  /** helper function processing escaped characters in JSON strings
   *  @param value containing the string. The string will be changed
   *  @return EC_Normal upon success, an error code otherwise
   */
  virtual OFCondition processJSONEscapeCharacters(OFString& value);

  /** parse the dataset part of an XML file containing a DICOM file or a DICOM dataset.
   *  @param dataset dataset stored in this parameter
   *  @param metaheader metaheader stored in this parameter
   *  @param current pointer to current JSMN Token
   *  @return EC_Normal upon success, an error code otherwise
   */
  virtual OFCondition parseDataSet(
      DcmItem* dataset,
      DcmItem* metaheader,
      OFJsmnTokenPtr& current);

  /** parse a DICOM sequence
   *  @param sequence DICOM Sequence
   *  @param current pointer to current token in the tokenArray
   *  @return EC_Normal upon success, an error code otherwise
   */
  virtual OFCondition parseSequence(
      DcmSequenceOfItems& sequence,
      OFJsmnTokenPtr& current);

  /** parse a DICOM element that is not a sequence
   *  @param dataset dataset stored in this parameter
   *  @param metaheader metaheader stored in this parameter
   *  @param current pointer to current JSMN Token
   *  @return EC_Normal upon success, an error code otherwise
   */
  virtual OFCondition parseElement(
      DcmItem* dataset,
      DcmItem* metaheader,
      OFJsmnTokenPtr& current);

  /** parse a value array from JSON to DICOM
   *  @param newElem Pointer to element where the value should be stored
   *  @param current pointer to current token in the tokenArray
   *  @return EC_Normal upon success, an error code otherwise
   */
  virtual OFCondition parseElementValueArray(
      DcmElement*& newElem,
      OFJsmnTokenPtr& current);

  /** parse a person name (PN value)
   *  @param value string containing the PN DICOM value
   *  @param current pointer to current token in the tokenArray
   *  @return EC_Normal upon success, an error code otherwise
   */
  virtual OFCondition parsePersonName(
      OFString& value,
      OFJsmnTokenPtr& current);

  /** store decoded inline binary value in a DICOM element
   *  @param element element into which the value will be inserted
   *  @param data buffer containing the inline binary data in big endian byte order
   *  @param length of the buffer
   *  @return EC_Normal upon success, an error code otherwise
   */
  virtual OFCondition storeInlineBinaryValue(
      DcmElement& element,
      Uint8 *data,
      size_t length);

};

#endif
