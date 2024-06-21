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
 *  Purpose: Class for managing attribute rules as found in DICOM modules
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/iodrules.h"
#include "dcmtk/dcmdata/dcelem.h"
#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/dcmdata/dctagkey.h"
#include "dcmtk/dcmiod/iodtypes.h"

IODRules::IODRules()
    : m_Rules()
{
    // nothing to do
}

IODRules* IODRules::clone()
{
    IODRules* newRules = new IODRules();
    if (newRules)
    {
        OFMap<DcmTagKey, IODRule*>::iterator it = m_Rules.begin();
        while (it != m_Rules.end())
        {
            if (it->second)
            {
                IODRule* newRule = it->second->clone();
                if (newRule)
                {
                    newRules->addRule(newRule);
                }
                else
                {
                    DCMIOD_WARN("Cannot create new IODRule, memory exhausted?");
                }
            }
            else
            {
                DCMIOD_WARN("Found NULL IODRule, cannot clone");
            }
            it++;
        }
    }
    else
    {
        DCMIOD_WARN("Cannot create new IODRules, memory exhausted?");
    }
    return newRules;
}

IODRule* IODRules::getByTag(const DcmTagKey& key) const
{
    IODRules::const_iterator it = m_Rules.find(key);
    if (it != m_Rules.end())
        return (*it).second;
    else
        return NULL;
}

IODRules::iterator IODRules::begin()
{
    return m_Rules.begin();
}

IODRules::iterator IODRules::end()
{
    return m_Rules.end();
}

void IODRules::clear()
{

    while (m_Rules.size() > 0)
    {
        IODRules::iterator it = m_Rules.begin();
        IODRule* rule         = (*it).second;
        m_Rules.erase(it);
        delete rule;
        rule = NULL;
    }
}

OFBool IODRules::addRule(IODRule* rule, const OFBool overwriteExisting)
{
    if (rule == NULL)
    {
        DCMIOD_ERROR("Cannot add IOD rule: NULL pointer");
        return OFFalse;
    }
    DcmTagKey key = rule->getTagKey();
    iterator it   = m_Rules.find(key);
    if (it != m_Rules.end())
    {
        if (overwriteExisting)
        {
            delete (*it).second;
            (*it).second = rule;
        }
        else
        {
            DCMIOD_DEBUG("IOD rule for tag " << key << " not inserted (already existing and overwriting disabled)");
            return OFFalse;
        }
    }
    else
    {
        m_Rules.insert(OFMake_pair(key, rule));
    }
    return OFTrue;
}

const OFVector<IODRule*> IODRules::getByModule(const OFString& moduleName)
{
    OFVector<IODRule*> result;
    IODRules::iterator it = m_Rules.begin();
    while (it != m_Rules.end())
    {
        if ((*it).second->getModule() == moduleName)
        {
            result.push_back((*it).second);
        }
        it++;
    }
    return result;
}

OFBool IODRules::deleteRule(const DcmTagKey key)
{
    IODRules::iterator it = m_Rules.find(key);
    if (it != m_Rules.end())
    {
        IODRule* rule = (*it).second;
        m_Rules.erase(it);
        delete rule;
        return OFTrue;
    }
    return OFFalse;
}

void IODRules::dump(STD_NAMESPACE ostream& out)
{
    IODRules::iterator it = m_Rules.begin();
    while (it != m_Rules.end())
    {
        out << (*it).first << ": Type \"" << (*it).second->getType() << "\", VM \"" << (*it).second->getType() << "\""
            << OFendl;
        it++;
    }
}

IODRules::~IODRules()
{
    clear();
}

IODRule::IODRule(const DcmTagKey& key,
                 const OFString& VM,
                 const OFString& type,
                 const OFString& module,
                 const DcmIODTypes::IOD_IE ie,
                 const OFString& defaultValue,
                 const OFString& privateCreator)
    : m_Key(key)
    , m_VM(VM)
    , m_Type(type)
    , m_Module(module)
    , m_IE(ie)
    , m_DefaultValue(defaultValue)
    , m_PrivateCreator(privateCreator)
{
    // nothing else to do
}

IODRule::~IODRule()
{
    // nothing to do
}

IODRule* IODRule::clone()
{
    return new IODRule(m_Key, m_VM, m_Type, m_Module, m_IE, m_DefaultValue, m_PrivateCreator);
}

OFString IODRule::getPrivateCreator() const
{
    return m_PrivateCreator;
}

DcmTagKey IODRule::getTagKey() const
{
    return m_Key;
}

OFString IODRule::getModule() const
{
    return m_Module;
}

OFString IODRule::getType() const
{
    return m_Type;
}

OFString IODRule::getVM() const
{
    return m_VM;
}

OFString IODRule::getDefaultValue() const
{
    return m_DefaultValue;
}

DcmIODTypes::IOD_IE IODRule::getIE() const
{
    return m_IE;
}

OFBool IODRule::setType(const OFString& val)
{
    if ((val != "1") && (val != "1C") && (val != "2") && (val != "2C") && (val != "3"))
    {
        return OFFalse;
    }
    m_Type = val;
    return OFTrue;
}

OFBool IODRule::setModule(const OFString& val)
{
    if (val.empty())
    {
        return OFFalse;
    }
    m_Module = val;
    return OFTrue;
}

OFBool IODRule::setPrivateCreator(const OFString& val)
{
    if (val.empty())
    {
        return OFFalse;
    }
    m_PrivateCreator = val;
    return OFTrue;
}

OFBool IODRule::setDefaultValue(const OFString& val)
{
    if (val.empty())
    {
        return OFFalse;
    }
    m_DefaultValue = val;
    return OFTrue;
}

OFBool IODRule::setVM(const OFString& val)
{
    // One could check whether this is a valid VM string...
    m_VM = val;
    return OFTrue;
}

OFCondition IODRule::check(DcmItem& item, const OFBool quiet)
{
    OFCondition result;
    const OFString tagName = DcmTag(m_Key).getTagName();
    DcmElement* elem       = NULL;
    OFCondition searchCond = item.findAndGetElement(m_Key, elem, OFFalse /* only this level */);
    /* NB: type 1C and 2C cannot be checked, assuming to be optional */
    if (((m_Type == "1") || (m_Type == "2")) && searchCond.bad())
    {
        if (!quiet)
            DCMIOD_WARN(tagName << " " << m_Key << " absent in " << m_Module << " (type " << m_Type << ")");
        result = EC_MissingAttribute;
    }
    else if ((elem == NULL) || elem->isEmpty(OFTrue /*normalize*/))
    {
        /* however, type 1C should never be present with empty value */
        if (((m_Type == "1") || (m_Type == "1C")) && searchCond.good())
        {
            if (!quiet)
                DCMIOD_WARN(tagName << " " << m_Key << " empty in " << m_Module << " (type " << m_Type << ")");
            result = EC_MissingValue;
        }
    }
    else
    {
        const OFCondition checkResult = elem->checkValue(m_VM, OFTrue /*oldFormat*/);
        if (checkResult == EC_InvalidCharacter)
        {
            if (!quiet)
                DCMIOD_WARN(tagName << " " << m_Key << " contains invalid character(s) in " << m_Module);
            result = checkResult;
        }
        else if (checkResult == EC_ValueRepresentationViolated)
        {
            if (!quiet)
                DCMIOD_WARN(tagName << " " << m_Key << " violates VR definition in " << m_Module);
            result = checkResult;
        }
        else if (checkResult == EC_ValueMultiplicityViolated)
        {
            const OFString vmText = (elem->getVR() == EVR_SQ) ? " #items" : " VM";
            if (!quiet)
                DCMIOD_WARN(tagName << " " << m_Key << vmText << " != " << m_VM << " in " << m_Module);
            result = checkResult;
        }
        else if (checkResult == EC_MaximumLengthViolated)
        {
            if (!quiet)
                DCMIOD_WARN(tagName << " " << m_Key << " violates maximum VR length in " << m_Module);
            result = checkResult;
        }
        else if (checkResult.bad())
        {
            if (!quiet)
                DCMIOD_DEBUG("INTERNAL ERROR while checking value of " << tagName << " " << m_Key << " in "
                                                                       << m_Module);
            result = EC_InternalError;
        }
    }
    return result;
}
