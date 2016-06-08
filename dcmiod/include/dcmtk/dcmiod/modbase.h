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
 *  Purpose: Abstract base class for IOD Modules or other attribute collections
 *
 */

#ifndef MODBASE_H
#define MODBASE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmiod/ioddef.h"
#include "dcmtk/dcmiod/iodrules.h"

/** Class for managing sets of dicom attributes (e.g.\ macros and modules).
 *  The data is hold in a container (DcmItem) that can be shared with other
 *  components, i.e.\ different modules that hold their data in a common
 *  container. In order to know which attributes in the container should be
 *  actually handled by a component, the component also has a set of rules, which
 *  can also be shared between different components (e.g.\ all modules of an IOD
 *  can share the same set of rules). For each attribute there is one rule in
 *  the rule set, denoting the requirement type (1,2,3,1C,2C), the VM, and
 *  besides others also the name of the component that the attribute belongs
 *  to (e.g.\ "PatientModule"). Since the component knows its own name, it can
 *  decide which attributes in the data container it is responsible for.
 *  This class is meant to support nested substructures but so far only writes
 *  attributes on a single level (including sequences). Also, in this context
 *  the class carries a parent relationship which is not used at the moment.
 */
class DCMTK_DCMIOD_EXPORT IODComponent
{

public:

  /** Constructor
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          an empty rule set.
   *  @param  parent The parent of the IOD component (NULL if none or unknown)
   */
  IODComponent(OFshared_ptr<DcmItem> item,
               OFshared_ptr<IODRules> rules,
               IODComponent* parent = NULL);

  /** Constructor, creates rules and item from scratch.
   *  @param  parent The parent of the IOD component (NULL if none or unknown)
   */
  IODComponent(IODComponent* parent = NULL);

  /** Assignment operator, copies contained item and rule set from rhs to
   *  "this" attribute set. Produces clone copy, i.e.\ the contained item
   *  and the rule set is copied. The parent component is set to NULL.
   *  @param  rhs The IODComponent to be assigned
   *  @return Reference to this module
   */
  IODComponent& operator=(const IODComponent& rhs);

  /** Copy constructor, copies reference to contained item and
   *  rule set to "this" attribute set.
   *  @param  rhs The component to be assigned
   */
  IODComponent(const IODComponent& rhs);

  /** Virtual Destructor
   */
  virtual ~IODComponent();

  /** Clear all attributes from the data that are handled by this module.
   *  An attribute is considered belonging to the module if there are rules
   *  marked as belonging to this module via the rule's module name.
   */
  void clearData();

  /** Set missing values by inventing "default values". Automatically
   *  called during write() in IODComponent; does nothing in this base
   *  class implementation but can be overwritten by derived classes if
   *  default values are desired.
   */
  virtual void inventMissing();

  /** Resets rules to their original values
   */
  virtual void resetRules() =0;

  /** Get rules handled by this module
   *  @return The rules
   */
  OFshared_ptr<IODRules> getRules()
  {
    return m_Rules;
  }

  /** Make component optional by turning all attributes requirement types of it
   *  to type 3. In order to reset to the attribute's original types,
   *  resetRules() can be used.
   */
  virtual void makeOptional();

  /** Get name of component
   *  @return Name of the module
   */
  virtual OFString getName() const =0;

  /** Get the data handled by this module
   *  @return The item containing the data of this module
   */
  DcmItem& getData()
  {
    return *m_Item;
  }

  /** Read attributes from given item into this class
   *  @param source  The source to read from
   *  @param clearOldData If OFTrue, old data is cleared before reading. Otherwise
   *         old data is overwritten (or amended)
   *  @result EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write attributes from this class into given item
   *  @param  destination The item to write to
   *  @result EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& destination);

  /** Check whether this component's data satisfies the underlying
   *  rules
   *  @param  quiet If OFTrue, not error / warning messages will be produced. Only
   *                the returned error code will indicate error or OK. Per default,
   *                logging output is produced (OFFalse).
   *  @result EC_Normal if rules are satisfied, error otherwise
   */
  virtual OFCondition check(const OFBool quiet = OFFalse);

  /** Comparison operator for IOD Components
   *  @param  rhs The right hand side of the comparison
   *  @return 0, if the given object is equal to this object, other value otherwise
   */
  virtual int compare(const IODComponent& rhs) const;

  /** Static helper function that reads attributes from given
   *  item into destination item, as determined by the provided
   *  rules and component name. The rules are only applied when reading
   *  from the source (which may result in warning messages on the logger),
   *  but if they could be found they are taken over into the destination
   *  item no matter whether the element validates against the rule.
   *  @param  source The item to read from
   *  @param  rules  The rules that provide the attributes and requirements
   *          for these attributes
   *  @param  destination The destination to write to
   *  @param  componentName The name of the module/component to write
   *  @result EC_Normal if reading was successful, error otherwise
   */
  static OFCondition  read(DcmItem& source,
                           IODRules& rules,
                           DcmItem& destination,
                           const OFString& componentName);

  /** Static helper function that writes attributes from given
   *  item into destination item, as determined by the provided
   *  rules and component name. The rules are only applied when writing
   *  to the destination (which may result in warning messages on the logger,
   *  and the whole call returning with an error). During reading from the
   *  source item the elements read are not validated against the rules.
   *  @param  source The item to read from
   *  @param  rules  The rules that provide the attributes and requirements
   *          for these attributes
   *  @param  destination The destination to write to
   *  @param  componentName The name of the module/component to write
   *  @result EC_Normal if reading was successful, error otherwise
   */
  static OFCondition write(DcmItem& source,
                           IODRules& rules,
                           DcmItem& destination,
                           const OFString& componentName);
protected:

    /// Shared pointer to the data handled by this class. The item may contain
    /// more attributes than this class is actually responsible for
    OFshared_ptr<DcmItem> m_Item;

    /// Rules describing the attributes governed by this class
    OFshared_ptr<IODRules> m_Rules;

    /// The parent component (may be NULL) of this class
    IODComponent* m_Parent;
};


/** The class IODModule is an IODComponent without parent component since
 *  a module does always belong to the top level dataset.
 */
class DCMTK_DCMIOD_EXPORT IODModule : public IODComponent
{
public:

  /** Constructor. Similar to the one of IODComponent but no parent
   *  can be defined since a module is always at top level.
   *  @param  item The item to be used for data storage. If NULL, the
   *          class creates an empty data container.
   *  @param  rules The rule set for this class. If NULL, the class creates
   *          an empty rule set.
   */
  IODModule(OFshared_ptr<DcmItem> item,
            OFshared_ptr<IODRules> rules);

  /** Constructor. Creates new empty data container and new empty
   *  ruleset. No parent component is defined (since a module is always
   *  on top level.
   */
  IODModule();

  ~IODModule() {};
};

#endif //MODBASE_H

