/*
 *
 *  Copyright (C) 2015-2024, Open Connections GmbH
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

#include "dcmtk/config/osconfig.h" /* make sure OS specific configuration is included first */
#include "dcmtk/dcmiod/modbase.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmiod/iodrules.h"
#include "dcmtk/dcmiod/iodutil.h"

IODComponent::IODComponent(const IODComponent& rhs)
    : m_Item(OFstatic_cast(DcmItem*, rhs.m_Item->clone()))
    , m_Rules(rhs.m_Rules->clone())
    , m_Parent(OFnullptr)
    , m_CheckValueOnWrite(rhs.m_CheckValueOnWrite)
{
}

IODComponent::IODComponent(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules, IODComponent* parent)
    : m_Item(item)
    , m_Rules(rules)
    , m_Parent(parent)
    , m_CheckValueOnWrite(OFTrue)
{
    if (!m_Item)
        m_Item.reset(new DcmItem());

    if (!m_Rules)
        m_Rules.reset(new IODRules());
}

IODComponent::IODComponent(IODComponent* parent)
    : m_Item()
    , m_Rules()
    , m_Parent(parent)
    , m_CheckValueOnWrite()
{
    m_Item.reset(new DcmItem());
    m_Rules.reset(new IODRules());
    if (parent)
    {
        m_CheckValueOnWrite = parent->getValueCheckOnWrite();
    }
}

IODComponent::~IODComponent()
{
    // Nothing to do for shared pointers
}

IODComponent& IODComponent::operator=(const IODComponent& rhs)
{
    if (&rhs != this)
    {
        m_Item.reset(OFstatic_cast(DcmItem*, rhs.m_Item->clone()));
        m_Rules.reset(rhs.m_Rules->clone());
        m_Parent = OFnullptr;
        m_CheckValueOnWrite = rhs.m_CheckValueOnWrite;
    }
    return *this;
}

void IODComponent::inventMissing()
{
    // Try to fill in default values
    OFVector<IODRule*> writeRules     = m_Rules->getByModule(getName());
    OFVector<IODRule*>::iterator rule = writeRules.begin();
    while (rule != writeRules.end())
    {
        OFString val = (*rule)->getDefaultValue();
        // We have a default value
        if (val.length() != 0)
        {
            // Check if element is not set
            DcmElement* elem = NULL;
            if (getData().findAndGetElement((*rule)->getTagKey(), elem).bad())
            {
                elem = DcmItem::newDicomElement((*rule)->getTagKey());
                if (elem == NULL)
                {
                    DCMIOD_ERROR("Could not allocate element " << (*rule)->getTagKey());
                }
                else
                {
                    elem->putString(val.c_str());
                    m_Item->insert(elem);
                }
            }
            else if (elem->getLength() == 0)
            {
                elem->putString(val.c_str());
            }
        }
        rule++;
    }
}

void IODComponent::clearData()
{
    OFVector<IODRule*> modRules       = m_Rules->getByModule(getName());
    OFVector<IODRule*>::iterator rule = modRules.begin();
    while (rule != modRules.end())
    {
        m_Item->findAndDeleteElement((*rule)->getTagKey());
        rule++;
    }
}

void IODComponent::makeOptional()
{
    OFVector<IODRule*> modRules       = m_Rules->getByModule(getName());
    OFVector<IODRule*>::iterator rule = modRules.begin();
    while (rule != modRules.end())
    {
        (*rule)->setType("3");
        rule++;
    }
}

OFCondition IODComponent::read(DcmItem& source, const OFBool clearOldData)
{
    // Debug
    DCMIOD_DEBUG("Reading component: " << getName());

    // Clear old data if desired
    if (clearOldData)
    {
        clearData();
    }

    // Do actual reading
    read(source, *m_Rules, *m_Item, getName());
    // We do not report errors here (only logger output)
    return EC_Normal;
}

OFCondition IODComponent::write(DcmItem& destination)
{
    // Debug
    DCMIOD_DEBUG("Writing component: " << getName());

    // Invent missing values
    inventMissing();

    // Start writing
    OFCondition result = EC_Normal;
    result             = write(*m_Item, *m_Rules, destination, getName(), m_CheckValueOnWrite);
    return result;
}

int IODComponent::compare(const IODComponent& rhs) const
{
    return this->m_Item.get()->compare(*(rhs.m_Item.get()));
}

OFCondition IODComponent::check(const OFBool quiet)
{
    OFCondition result;
    IODRules::iterator it = m_Rules->begin();
    while (it != m_Rules->end())
    {
        result = (*it).second->check(*m_Item, quiet);
        if (result.bad())
            break;
        else
            it++;
    }
    return result;
}

// static helper
OFCondition IODComponent::read(DcmItem& source, IODRules& rules, DcmItem& destination, const OFString& componentName)
{
    OFVector<IODRule*> modRules       = rules.getByModule(componentName);
    OFVector<IODRule*>::iterator rule = modRules.begin();
    while (rule != modRules.end())
    {
        // We do not read/copy sequences but only check them since they are not
        // handled within this component
        OFBool isSequence = (DcmTag((*rule)->getTagKey()).getEVR() == EVR_SQ);
        if (isSequence)
        {
            DcmElement* elem = NULL;
            OFCondition cond = source.findAndGetElement((*rule)->getTagKey(), elem);
            DcmIODUtil::checkElementValue(elem,
                                          (*rule)->getTagKey(),
                                          (*rule)->getVM(),
                                          (*rule)->getType(),
                                          cond,
                                          (*rule)->getModule().c_str(),
                                          dcmtk::log4cplus::WARN_LOG_LEVEL);
        }
        else // Normal attributes are checked and copied over into this IOD component
        {
            DcmElement* elem = NULL;
            DcmIODUtil::getAndCheckElementFromDataset(source, elem, *rule);
            if (elem != NULL)
            {
                OFCondition result = destination.insert(elem, OFTrue);
                if (result.bad())
                {
                    DCMIOD_ERROR("Cannot insert element with tag: " << (*rule)->getTagKey());
                    delete elem;
                }
            }
        }
        rule++;
    }
    return EC_Normal;
}

// static helper
OFCondition IODComponent::write(DcmItem& source, IODRules& rules, DcmItem& destination, const OFString& componentName, const OFBool checkValue)
{
    OFCondition result = EC_Normal;

    OFVector<IODRule*> writeRules     = rules.getByModule(componentName);
    OFVector<IODRule*>::iterator rule = writeRules.begin();
    while (rule != writeRules.end())
    {
        DcmElement* elem = NULL;
        source.findAndGetElement((*rule)->getTagKey(), elem, OFFalse /* only this level */, OFTrue /* create copy*/);
        DcmIODUtil::addElementToDataset(result, destination, elem, *rule, checkValue);
        rule++;
    }

    return result;
}

void IODComponent::setValueCheckOnWrite(const OFBool checkValue)
{
    m_CheckValueOnWrite = checkValue;
}

OFBool IODComponent::getValueCheckOnWrite() const
{
    return m_CheckValueOnWrite;
}

// -------- IODModule --------------

IODModule::IODModule()
    : IODComponent()
{
    // nothing to do, IODComponent does the work
}

IODModule::IODModule(const IODModule& rhs)
    : IODComponent(rhs.m_Item, rhs.m_Rules, rhs.m_Parent)
{
}

IODModule::IODModule(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules)
    : IODComponent(item, rules, NULL /* No parent for modules */)
{
    // nothing to do, IODComponent does the work
}

IODModule& IODModule::operator=(const IODModule& rhs)
{
    if (this != &rhs)
    {
        m_Item   = rhs.m_Item;
        m_Rules  = rhs.m_Rules;
        m_Parent = rhs.m_Parent;
    }
    return *this;
}
