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
 *  Purpose: Class for managing attribute rules as found in DICOM modules
 *
 */

#ifndef IODRULES_H
#define IODRULES_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofmap.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmdata/dctagkey.h"
#include "dcmtk/dcmiod/iodtypes.h"

#define INCLUDE_OSTREAM  // for debugging
#include "dcmtk/ofstd/ofstdinc.h"


class DcmItem;
class DcmTagKey;
class DcmElement;
class IODRule;


/** Class representing rules for a set of DICOM attributes, e.g.\ for
 *  all attributes of a module, macro or the like. At the moment
 *  attributes within sequences are not yet supported.
 */
class DCMTK_DCMIOD_EXPORT IODRules
{

public:

  /// iterator type to iterate over rule set
  typedef OFMap<DcmTagKey, IODRule*>::iterator iterator;

  /// const iterator type to iterate over rule set
  typedef OFMap<DcmTagKey, IODRule*>::const_iterator const_iterator;

  /** Constructor
   */
  IODRules();

  /** Returns deep copy of this object
   *  @return Deep copy of "this" object
   */
  IODRules* clone();

  /** Return iterator to first rule
   *  @return Iterator to the first rule
   */
  iterator begin();

  /** Return iterator to last rule
   *  @return Iterator to the last rule
   */
  iterator end();

  /** Adds rule to this rule set
   *  @param  rule The rule to add (ownership is transferred to this class)
   *  @param  overwriteExisting If OFTrue (default), an existing rule for the
   *          same attribute (tag) is overwritten, otherwise the rule is not
   *          taken over (and ownership stays with caller)
   *  @return OFTrue if rule could be added, error otherwise
   */
  virtual OFBool addRule(IODRule* rule,
                         const OFBool overwriteExisting = OFFalse);

  /** Delete rule by given tag key
   *  @param  key Tag of the attribute rule to be deleted
   *  @return OFTrue if a rule could be found and deleted, OFFalse otherwise
   */
  virtual OFBool deleteRule(const DcmTagKey key);

  /** Get all rules that are marked belonging to a specific module
   *  @param  moduleName The module name (e.g.\ "PatientModule")
   *  @return Vector with pointers to all the rules belonging to given module
   */
  const OFVector<IODRule*> getByModule(const OFString& moduleName);

  /** Get a rule by its tag
   *  @param  key The tag to find the rule for
   *  @return The rule, if found, NULL otherwise
   */
  IODRule* getByTag(const DcmTagKey& key) const;

  /** Clear all rules
   */
  virtual void clear();

  /** Dump rules to stream, useful for debugging
   *  @param  out The stream to dump to
   */
  virtual void dump(STD_NAMESPACE ostream &out);

  /** Destructor
   */
  virtual ~IODRules();

private:

  /// Map that holds all rules, accessible by their tag key
  OFMap<DcmTagKey, IODRule*> m_Rules;
};


/** Class representing a single rule, that refers to a single attribute.
 *  Each rule is uniquely identified by the attribute's tag key and includes
 *  information on the Value Multiplicity (VM) of the attribute, its type
 *  (1, 1C, 2,...), the module (e.g.\ "PatientModule") or structure (e.g.
 *  "CodeSequenceMacro") it belongs to, a default value and its private
 *  creator if it is a private attribute. On top, the Information Entity
 *  according to the DICOM Model of the Real World can be stored, e.g.
 *  "Patient" for the "PatientModule".
 */
class DCMTK_DCMIOD_EXPORT IODRule
{
public:

  /** Constructor to create a new attribute rule
   *  @param  key The tag key of the attribute
   *  @param  VM  The VM of the attribute (notation like in DICOM part 6,
   *          e.g.\ "1-n")
   *  @param  type The "IOD type" of the attribute.
   *          Allowed values "1", "1C", "2", "2C" and "3"
   *  @param  module The module or "component" this attribute belongs to
   *  @param  ie The Information Entity the module belongs to
   *  @param  defaultValue The default value (default: empty)
   *  @param  privateCreator The private creator of the attribute if attribute
   *          (default: No private creator)
   */
  IODRule(const DcmTagKey& key,
          const OFString& VM,
          const OFString& type,
          const OFString& module,
          const DcmIODTypes::IOD_IE ie,
          const OFString& defaultValue = "",
          const OFString& privateCreator = "");

  /** Return deep copy of this object
   *  @return Deep copy of "this" object
   */
  IODRule* clone();

  /** Get tag key of the attribute rule
   *  @return The tag key of the rule
   */
  virtual DcmTagKey getTagKey() const;

  /** Get private creator of the related attribute
   *  @return The private creator of the attribute
   */
  virtual OFString getPrivateCreator() const;

  /** The requirement type of the attribute (1, 1C, 2, 2C or 3)
   *  @return The requirement type of the attribute
   */
  virtual OFString getType() const;

  /** The VM of the attribute as noted in the DICOM standard part 6,
   *  e.g.\ "1-n" or "2"
   *  @return The VM
   */
  virtual OFString getVM() const;

  /** Get module (or component name) the related attribute belongs to
   *  @return The module or component name (e.g.\ "PatientModule" or
   *          "CodeSequenceMacro")
   */
  virtual OFString getModule() const;

  /** Get default value of this attribute
   *  @return The default value (might be empty)
   */
  virtual OFString getDefaultValue() const;

  /** Get Information Entity the related attribute belongs to
   *  @return The Information Entity (might be empty)
   */
  virtual DcmIODTypes::IOD_IE getIE() const;

  virtual OFBool setPrivateCreator(const OFString& val);

  virtual OFBool setType(const OFString& val);

  virtual OFBool setVM(const OFString& val);

  virtual OFBool setModule(const OFString& val);

  virtual OFBool setDefaultValue(const OFString& val);

  /** Check whether the given item fulfills the requirements of
   *  this rule, i.e.\ the related attribute is checked within the
   *  given item
   *  @param  item The item to check the attribute in
   *  @param  quiet If OFTrue, no error or warning messages will be produced
   *          but only the return code will indicate OK or error.
   *  @return EC_Normal if this rule is fulfilled, error otherwise
   */
  virtual OFCondition check(DcmItem& item,
                            const OFBool quiet = OFFalse);

  /** Virtual Destructor
   */
  virtual ~IODRule();

private:

  /// Private undefined default constructor
  IODRule();

  /// Private undefined copy constructor
  IODRule(const IODRule& rhs);

  /// Tag key
  DcmTagKey m_Key;

  /// VM, see DcmElement::checkVM() for permitted values (e.g.\ "1-n")
  OFString m_VM;

  /// Requirement type: 1, 1C, 2, 2C or 3
  OFString m_Type;

  /// Module (e.g.\ "PatientModule" or "component name", e.g.\ "CodeSequenceMacro"
  OFString m_Module;

  /// The Information Entity the related attribute belongs to according to the
  /// DICOM Model of the Real World, e.g.\ "Patient" for the attribute
  /// "Patient Name"
  DcmIODTypes::IOD_IE m_IE;

  /// Default value for attribute
  OFString m_DefaultValue;

  /// Private Creator (if private attribute)
  OFString m_PrivateCreator;
};

#endif // IODRULES_H
