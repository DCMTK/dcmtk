/*
 *
 *  Copyright (C) 2003-2026, OFFIS e.V.
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
 *  Author:  Michael Onken
 *
 *  Purpose: Class for modifying DICOM files
 *
 */

#include "dcmtk/config/osconfig.h" // make sure OS specific configuration is included first

#include "dcmtk/ofstd/ofstream.h"   /* for OFIStringStream */

#include "dcmtk/dcmdata/dcistrmf.h" /* for class DcmInputFileStream */
#include "dcmtk/dcmdata/dcpath.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdict.h"
#include "dcmtk/dcmdata/dcdicent.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcpixel.h"
#include "dcmtk/dcmdata/dcvris.h"
#include "dcmtk/dcmdata/dcmetinf.h"
#include "dcmtk/ofstd/oflist.h"
#include "dcmtk/ofstd/ofstd.h"
#include "dcmtk/ofstd/oflimits.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/ofstd/ofstd.h"
#include "mdfdsman.h"
#include "mdfconen.h"

static OFLogger mdfdsmanLogger = OFLog::getLogger("dcmtk.dcmdata.mdfdsman");

MdfDatasetManager::MdfDatasetManager()
    : current_file("")
    , dfile(NULL)
    , dset(NULL)
    , ignore_un_modifies(OFFalse)
{
}

OFCondition MdfDatasetManager::loadFile(const char* file_name,
                                        const E_FileReadMode readMode,
                                        const E_TransferSyntax xfer,
                                        const OFBool createIfNecessary)
{
    OFCondition cond;
    // delete old dfile and free memory and reset current_file
    delete dfile;
    current_file = "";
    dfile        = new DcmFileFormat();
    dset         = dfile->getDataset();

    // load file into dfile if it exists
    OFLOG_INFO(mdfdsmanLogger, "Loading file into dataset manager: " << file_name);
    if (OFStandard::fileExists(file_name) || (strcmp(file_name, "-") == 0))
    {
        cond = dfile->loadFile(file_name, xfer, EGL_noChange, DCM_MaxReadLength, readMode);
    }
    // if it does not already exist, check whether it should be created
    else if (createIfNecessary)
    {
        OFLOG_DEBUG(mdfdsmanLogger, "File " << file_name << "does not exist, creating it as desired");
        cond = dfile->saveFile(file_name, EXS_LittleEndianExplicit /* might change later */);
    }
    // no file, we have an error
    else
        cond = makeOFCondition(OFM_dcmdata, 22, OF_error, "No such file or directory");

    // if there are errors:
    if (cond.bad())
    {
        dset = NULL;
    }
    // file successfully loaded into dfile
    else
    {
        // get dataset from file
        OFLOG_INFO(mdfdsmanLogger, "Getting dataset from loaded file: " << file_name);
        dset = dfile->getDataset();
        /* load also pixeldata into memory:
         * Without this command pixeldata wouldn't be included into the file,
         * that's saved after modifying, because original filename was renamed
         * meanwhile
         */
        cond = dset->loadAllDataIntoMemory();
        if (cond.good())
        {
            // save filename to member variable
            current_file = file_name;
        }
        if (cond.bad())
        {
            OFLOG_ERROR(mdfdsmanLogger,
                        "Unable to load all data into memory for file: " << file_name << ". Error: " << cond.text());
        }
    }
    return cond;
}

static DcmTagKey getTagKeyFromDictionary(OFString tag)
{
    DcmTagKey key(0xffff, 0xffff);
    const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
    const DcmDictEntry* dicent              = globalDataDict.findEntry(tag.c_str());
    // successful lookup in dictionary -> translate to tag and return
    if (dicent)
    {
        key = dicent->getKey();
    }
    dcmDataDict.rdunlock();
    return key;
}

static int readNextToken(const char* c, int& pos, DcmTagKey& key, Uint32& idx)
{
    OFString aString;
    int lpos = pos;
    int spos = 0;
    if (c[lpos] == '\0')
        return -1; // EOF
    if (c[lpos] == '.')
    {
        ++pos;
        return 3; // period
    }
    // look for item index between []
    if (c[lpos] == '[')
    {
        spos = ++lpos;
        while ((c[lpos] >= '0') && (c[lpos] <= '9'))
            ++lpos;
        if (c[lpos] != ']')
            return 0; // parse error
        unsigned long newindex = 0;
        if (1 != sscanf(c + spos, "%lu", &newindex))
            return 0; // parse error
        idx = OFstatic_cast(Uint32, newindex);
        pos = ++lpos;
        return 2; // index
    }
    // look for tag between ()
    if (c[lpos] == '(')
    {
        spos = ++lpos;
        while ((c[lpos] != ')') && (c[lpos] != '\0'))
            ++lpos;
        if (c[lpos] != ')')
            return 0; // parse error
        unsigned int group = 0;
        unsigned int elem  = 0;
        if (2 != sscanf(c + spos, "%x,%x", &group, &elem))
            return 0; // parse error
        key = DcmTagKey(OFstatic_cast(Uint16, group), OFstatic_cast(Uint16, elem));
        pos = ++lpos;
        return 1; // tag key
    }
    // so far no tag and no item index found. So check if it's a dictionary name
    spos = lpos;
    while (((c[lpos] >= 'a') && (c[lpos] <= 'z')) || ((c[lpos] >= 'A') && (c[lpos] <= 'Z'))
           || ((c[lpos] >= '0') && (c[lpos] <= '9')))
        ++lpos;
    aString.append(c + spos, (lpos - spos));
    key = getTagKeyFromDictionary(aString);
    // if key was found in dictionary, return 1 for tag key
    if ((key.getGroup() != 0xffff) && (key.getElement() != 0xffff))
    {
        pos = lpos;
        return 1; // tag key
    }
    // if no return command was processed this far, the token could not be parsed
    return 0; // parse error
}

static DcmItem* getItemFromPath(DcmItem& dataset, const char* location, OFString& message)
{
    DcmTagKey key;
    Uint32 idx             = 0;
    int pos                = 0;
    int token              = 0;
    int expected           = 1; // first expected is a tagkey
    OFBool finished        = OFFalse;
    DcmItem* result        = &dataset;
    DcmSequenceOfItems* sq = NULL;
    DcmStack stack;
    message.clear();
    do
    {
        token = readNextToken(location, pos, key, idx);
        if ((token != expected) && (token != -1))
        {
            message = message + "parse error in path '" + location + "'";
            return NULL;
        }
        if (token == -1)
        {
            if (!finished)
            {
                message = message + "Error: path '" + location + "' incomplete";
                return NULL;
            }
            return result;
        }
        if (token == 1)
        {
            // we have read a tag key
            stack.clear();
            if (EC_Normal != result->search(key, stack, ESM_fromHere, OFFalse))
            {
                message = message + " attribute not found in dataset (path is '" + location + "')";
                return NULL;
            }
            if (stack.top()->ident() == EVR_SQ)
            {
                sq = OFstatic_cast(DcmSequenceOfItems*, stack.top());
            }
            else
            {
                message = message + "attribute is not a sequence (path is '" + location + "')";
                return NULL;
            }
            expected = 2;
            finished = OFFalse;
        }
        else if (token == 2)
        {
            // we have read an index
            if (sq == NULL)
            {
                message = message + "sequence not found in path '" + location + "'";
                return NULL;
            }
            if (idx >= sq->card())
            {
                message = message + "cannot allocate item in sequence (path is '" + location + "')";
                return NULL;
            }
            result = sq->getItem(idx);
            if (result == NULL)
            {
                message = message + "item not found in path '" + location + "'";
                return NULL;
            }
            expected = 3;
            finished = OFTrue;
        }
        else if (token == 3)
        {
            // we have read a period
            expected = 1;
            finished = OFFalse;
        }
    } while (token > 0);
    return NULL;
}

static OFCondition splitTagPath(OFString& tag_path, DcmTagKey& key)
{
    OFString target_tag;
    unsigned int group, elem;
    size_t lpos, rpos;
    rpos = tag_path.size() - 1;
    lpos = rpos;
    if (tag_path[rpos] == ')')
    {
        // get opening '(' of target tag; if its not found -> return error
        while ((tag_path[lpos] != '(') && (lpos > 0))
            --lpos;
        if (tag_path[lpos] != '(')
            return makeOFCondition(OFM_dcmdata, 22, OF_error, "Invalid tag path!");
        // now lpos and rpos "point" to braces of target tag
        // copy target tag from tag path
        target_tag = tag_path.substr(lpos, rpos - lpos + 1);
        // delete target tag from path (inclusive trailing '.')
        tag_path.erase(lpos, tag_path.length() - lpos);
        // if there's a tag path left, remove the '.', too
        if (!tag_path.empty() && (tag_path[tag_path.length() - 1] == '.'))
            tag_path.erase(tag_path.length() - 1, 1);
        // parse target_tag into DcmTagKey
        if (2 != sscanf(target_tag.c_str(), "(%x,%x)", &group, &elem))
            return makeOFCondition(OFM_dcmdata, 22, OF_error, "Invalid target tag!");
        key = DcmTagKey(OFstatic_cast(Uint16, group), OFstatic_cast(Uint16, elem));
    }
    else
    // otherwise we could have a dictionary name
    {
        while ((lpos > 0)
               && (((tag_path[lpos] >= 'a') && (tag_path[lpos] <= 'z'))
                   || ((tag_path[lpos] >= 'A') && (tag_path[lpos] <= 'Z'))
                   || ((tag_path[lpos] >= '0') && (tag_path[lpos] <= '9'))))
            lpos--;
        target_tag = tag_path.substr(lpos, rpos - lpos + 1);
        if (target_tag[0] == '.')
            target_tag.erase(0, 1);
        tag_path.erase(lpos, tag_path.length() - lpos);
        key = getTagKeyFromDictionary(target_tag);
        if ((key.getGroup() == 0xffff) && (key.getElement() == 0xffff))
        {
            OFString message = target_tag;
            message.append(" not found in dictionary!");
            return makeOFCondition(OFM_dcmdata, 22, OF_error, message.c_str());
        }
    }
    return EC_Normal;
}

OFString MdfDatasetManager::stripPadding(OFString& value_string, const char padding, OFBool frontOnly)
{
    if (value_string.length() == 0)
        return value_string;
    // remove padding spaces at front and end
    size_t pos = value_string.find_first_not_of(padding);
    if (pos > 0)
    {
        OFLOG_TRACE(mdfdsmanLogger,
                    "Stripping " << pos << " padding \"" << padding
                    << "\" at the front of the string: " << value_string);
        value_string.erase(0, pos);
    }
    if (!frontOnly)
    {
        pos = value_string.find_last_not_of(padding);
        if (value_string.size() - pos - 1 > 0)
        {
            OFLOG_TRACE(mdfdsmanLogger,
                        "Stripping " << value_string.size() - pos - 1 << " padding \""
                        << padding << "\" at the end of the string: " << value_string);
            value_string.erase(pos + 1);
        }
    }
    return value_string;
}

/**
 * @brief Checks if the given DICOM path is valid and retrieves the last element.
 *
 * This function validates the provided DICOM path to ensure it does not contain any invalid groups
 * (groups 0, 2, 1, 3, 5, 7, and F are not allowed). It also checks if the path is not empty and
 * that the last element in the path is a leaf element (i.e., not a Sequence or Item).
 *
 * @param dcm_path Pointer to the DICOM path to be checked.
 * @param elem Reference to a pointer that will be set to the last element in the path if the path is valid.
 * @return OFCondition status code indicating the result of the validation:
 *         - EC_Normal: The path is valid and the last element is retrieved successfully.
 *         - EC_InvalidTag: The path is invalid or the last element is not a leaf element.
 */
static OFCondition checkPathAndGetLeafElem(DcmPath* dcm_path, DcmElement*& elem)
{
    // Checking the path. Whether it is empty or invalid.
    // The following groups are not allowed: 0, 2, also 1, 3, 5, 7 and F
    if (dcm_path->containsInvalidGroup() || dcm_path->containsGroup(0) || dcm_path->containsGroup(2))
    {
        OFLOG_ERROR(mdfdsmanLogger, "path contains invalid group: " << dcm_path->toString());
        return EC_InvalidTag;
    }

    DcmPathNode* lastElement = dcm_path->back();
    if (lastElement == NULL)
    {
        OFLOG_ERROR(mdfdsmanLogger, "last element in path is empty");
        return EC_InvalidTag;
    }
    DcmObject* obj = lastElement->m_obj;
    if (obj == NULL)
    {
        OFLOG_ERROR(mdfdsmanLogger, "last element in path is empty");
        return EC_InvalidTag;
    }
    // if object at the end is not a leaf, the comparison check fails
    if (!obj->isLeaf())
    {
        OFLOG_ERROR(mdfdsmanLogger, "the specified element is a nested element (Sequence or Item)");
        return EC_InvalidTag;
    }
    // if object can actually be casted to DcmElement is present
    elem = OFstatic_cast(DcmElement*, lastElement->m_obj);
    if (elem == NULL)
    {
        OFLOG_ERROR(mdfdsmanLogger, "last element in path is empty");
        return EC_InvalidTag;
    }
    return EC_Normal;
}

OFCondition MdfDatasetManager::parsePathWithVM (OFString& path, DcmPathProcessor& proc, unsigned long& VM_index, OFBool& has_vm_index)
{
    VM_index = 0;
    has_vm_index = OFFalse;
    if (path.size() < 1)
        return EC_InvalidTag;

    OFCondition result = proc.findOrCreatePath(dset, path, OFFalse /*only find*/);

    // if the last char is ] and the path was not found,
    // it may be a condition with VM - cut off the [*] and recheck path.
    // e.g. "ImageType[1]" -> check "ImageType" instead and add the VM index later
    if (result.bad() && path[path.size() - 1] == ']')
    {
        size_t start_bracket  = path.rfind('[');
        OFString VM_index_str = path.substr(start_bracket + 1, path.size() - start_bracket - 2);
        path                  = path.substr(0, start_bracket);
        if (1 != sscanf(VM_index_str.c_str(), "%lu", &VM_index))
        {
            OFLOG_ERROR(mdfdsmanLogger, "the index in [] was not readable while parsing path");
            return EC_InvalidTag;
        }
        has_vm_index = OFTrue;
        OFLOG_TRACE(mdfdsmanLogger,
                    "Path not found. Using stripped new Path: " << path << " and VM index: " << VM_index);
        result = proc.findOrCreatePath(dset, path, OFFalse /*only find*/);
    }
    return result;
}


/**
 * Checks the existence of a value at the given multiplicity (VM) index.
 *
 * This function verifies whether a given VM index exists within the range of available values.
 * It performs an existence check based on the provided condition and returns the appropriate
 * status.
 *
 * @param condition The condition to be checked for existence.
 * @param VM_index The index of the value multiplicity to be checked.
 * @param number_of_values The total number of values available.
 * @return OFCondition status indicating the result of the check:
 *         - EC_Normal: The condition is met.
 *         - EC_PathConditionNotMet: The condition is not met.
 *         - EC_IllegalCall: The VM_index is negative (meaning not a specific
 *           value should be checked which is not handled by this method).
 */
OFCondition MdfDatasetManager::checkExistenceVM (const MdfCondition& condition, const unsigned long VM_index, const unsigned long number_of_values)
{
    OFBool indexExist = (VM_index < number_of_values);

    // operation is 'E'
    if (condition.isExistenceCheck(OFFalse))
    {
        if (indexExist) {
            OFLOG_TRACE(mdfdsmanLogger, "Checking Existence in VM (0 - "
                            << number_of_values - 1 << "): index " << VM_index
                            << " exists, condition passed");
            return EC_Normal;
        }
        OFLOG_TRACE(mdfdsmanLogger, "Checking Existence in VM (0 - "
                        << number_of_values - 1 << "): index " << VM_index
                        << " out of bound or no VM, condition failed");
        return EC_PathConditionNotMet;
    }
    if (indexExist) {
        OFLOG_TRACE(mdfdsmanLogger, "Checking Not Existence in VM (0 - "
                        << number_of_values - 1 << "): index " << VM_index
                        << " exists, condition failed");
        return EC_PathConditionNotMet;
    }
    OFLOG_TRACE(mdfdsmanLogger, "Checking Existence in VM (0 - "
                    << number_of_values - 1 << "): index " << VM_index
                    << "out of bound or no VM, condition passed");
    return EC_Normal;
}


OFCondition MdfDatasetManager::parsePath(DcmPathProcessor& proc, const MdfCondition& condition, unsigned long& vmIndex, OFBool& hasVMIndex, OFBool& conditionCheckDone)
{
    // find specified path
    OFCondition cond;
    OFString newTagPath = condition.path;
    newTagPath = stripPadding(newTagPath, ' ', OFFalse /*front and back*/);
    conditionCheckDone = OFFalse;
    // Value Multiplicity can be split from the path.
    vmIndex = 0;
    hasVMIndex= OFFalse;
    cond = parsePathWithVM(newTagPath, proc, vmIndex, hasVMIndex);
    if (cond.bad())
    {
        // EC_TagNotFound is special here due to existence checks.
        // If condition is !E, it passes if tag is not found; otherwise (E) it fails
        if (cond == EC_TagNotFound && condition.isExistenceCheck())
        {
            // operation is 'E' existence check
            if (condition.isExistenceCheck(OFFalse /*don't check for non-existence*/))
            {
                OFLOG_TRACE(mdfdsmanLogger, "Checking Existence: Tag does not exist, condition failed");
                conditionCheckDone = OFTrue;
                return EC_PathConditionNotMet;
            }
            // operation is !E non-Existence Check
            OFLOG_TRACE(mdfdsmanLogger, "Checking Non-Existence: Tag does not exist, condition passed");
            conditionCheckDone = OFTrue;
            return EC_Normal;
        }
        return cond;
    }
    // checking existence, tag is found; VM >= 1 will be checked later
    if (condition.isExistenceCheck() && !hasVMIndex)
    {
        // operation is 'E'
        if (condition.isExistenceCheck(OFFalse))
        {
            OFLOG_TRACE(mdfdsmanLogger, "Checking Existence: Tag exists, condition passed");
            conditionCheckDone = OFTrue;
            return EC_Normal;
        }
        OFLOG_TRACE(mdfdsmanLogger, "Checking Non-Existence: Tag exists, condition failed");
        conditionCheckDone = OFTrue;
        return EC_PathConditionNotMet;
    }

    return EC_Normal;
}

OFCondition MdfDatasetManager::checkStringCondition(const MdfCondition& condition, DcmElement*& elem, const OFBool has_VM_index, OFString& elemValue, OFString& cmpValue, int& compareResult)
{
    OFCondition err;

    // VM > 1 and no index specified -> compare full value
    if (elem->getNumberOfValues() > 1 && !has_VM_index)
    {
        OFLOG_DEBUG(mdfdsmanLogger,
                    "The value has VM > 1 but condition didn't specify an index. The whole value string will "
                    "be compared");
        char* stringVal;
        elem->getString(stringVal);
        // if its null, then elemValue is already empty
        if (stringVal != NULL)
            elemValue = stringVal;
    }
    DcmEVR evr = elem->getTag().getEVR();

    // if other than existence and equality, there may be weird results
    if (!condition.isEqualityCheck()
        // exclude date strings (DA has fixed format of 8 chars consisting of numbers and can be compared)
        && !(evr == EVR_DA)
    )
    {
        OFLOG_DEBUG(mdfdsmanLogger, "Comparing strings using <, <=, > or >= may deliver unexpected results.");
    }

    OFLOG_TRACE(mdfdsmanLogger,
                "The string comparison values are (operation: " << condition.operation << " )\n\""
                                                                << elemValue << "\" (element) \n\""
                                                                << cmpValue << "\" (input)");
    compareResult = elemValue.compare(cmpValue);
    // If the tag is an UID, it could have failed since a UID name has been provided instead
    if (evr == EVR_UI && condition.isEqualityCheck() && compareResult != 0)
    {
        elemValue = dcmFindNameOfUID(elemValue.c_str(), elemValue.c_str());
        OFLOG_TRACE(mdfdsmanLogger,
                    "The element is an UI. Trying to find the name\n\"" << elemValue << "\" (element)");
        compareResult = elemValue.compare(cmpValue);
    }
    return EC_Normal;
}

OFCondition MdfDatasetManager::createCompareElement(DcmElement*& orig, DcmElement*& created, OFString& value)
{
    OFCondition result;

    // OB, OW and UN use hexadecimal string representation (e.g. "01\00\dd" for OB,
    // "0000\7777" for OW). UN is stored as DcmOtherByteOtherWord internally (OB-like).
    // Skip decimal normalization and integer validation for these VRs and pass the
    // value directly to putString().
    DcmEVR evr = orig->getTag().getEVR();
    if (evr == EVR_OB || evr == EVR_OW || evr == EVR_UN || evr == EVR_ox)
    {
        if (value.empty())
        {
            OFLOG_ERROR(mdfdsmanLogger, "empty value");
            return EC_InvalidValue;
        }
        created = DcmItem::newDicomElement(orig->getTag());
        result = created->putString(value.c_str());
        if (result.bad())
        {
            OFLOG_ERROR(mdfdsmanLogger, "failed to parse hexadecimal value for " << DcmVR(evr).getVRName());
            delete created;
            created = NULL;
            return result;
        }
        return EC_Normal;
    }

    // first char may be plus or minus, cut it off for stripping the front padding,
    // and add it back in later ("-") if needed
    int sign = 1;
    if (value[0] == '-' || value[0] == '+')
    {
        sign = (value[0] == '-') ? -1 : 1;
        value.erase(0, 1);
    }
    if (value.empty())
    {
        OFLOG_ERROR(mdfdsmanLogger, "empty value");
        return EC_InvalidValue;
    }

    stripPadding(value, '0', OFTrue);

    // if the number was 0 (or something like 0000), we must add back in one 0
    if (value.empty()) value = "0";

    if (value[0] == '.') // it's a .* number, add one 0 at the start
    {
        value.insert(0, 1, '0');
    }

    // put the minus sign back in
    if (sign == -1)
        value = '-' + value;

    // check whether integer string valid in general (e.g. respects range)
    DcmVR vr = orig->getTag().getVR();
    if (vr.isInteger())
    {
        // reject floating point values for integer VRs
        size_t pos = value.find('.');
        size_t e_pos = value.find('e');
        if (e_pos == OFString_npos)
            e_pos = value.find('E');
        if (pos != OFString_npos && value.find_first_not_of('0', pos) != e_pos)
        {
            OFLOG_ERROR(mdfdsmanLogger,
                "value \"" << value
                << "\" seems to be a floating point number instead of integer as in VR "
                << vr.getVRName());
            return EC_InvalidValue;
        }

        result = checkIntegerValidity(value, sign, vr);
        if (result.bad())
            return result;
    }

    // create Element
    created = DcmItem::newDicomElement(orig->getTag());
    // checking for bounds - round trip: string to VR to string
    result = created->putString(value.c_str());
    if (result.bad())
    {
        OFLOG_ERROR(mdfdsmanLogger, "unexpected error in condition");
        delete created;
        created = NULL;
        return result;
    }
    return EC_Normal;
}

OFCondition MdfDatasetManager::checkNumberCondition(const MdfCondition& condition,
                                                    DcmElement*& elem,
                                                    OFString& elemValue,
                                                    OFString& cmpValue,
                                                    int& compareResult)
{
    OFCondition result;
    DcmElement *cmpElemRaw, *tmpMElemRaw;

    result = createCompareElement(elem, cmpElemRaw, cmpValue);
    if (result.bad()) return result;
    OFunique_ptr<DcmElement> cmpElem(cmpElemRaw);

    // create a temporary element with the original element value.
    // If a VM index is specified, this creates a new element containing only that value.
    result = createCompareElement(elem, tmpMElemRaw, elemValue);
    if (result.bad()) return result;
    OFunique_ptr<DcmElement> tmpMElem(tmpMElemRaw);

    OFLOG_TRACE(mdfdsmanLogger,
        "The comparison Values are: (element) \"" << elemValue << "\" " << condition.operation << " (input) \""
        << cmpValue << "\"");

    compareResult = tmpMElem->compare(*cmpElem);

    return EC_Normal;
}

OFCondition MdfDatasetManager::checkIntegerValidity(OFString& cmpValue, const int sign, const DcmVR& vr)
{
    if (vr.isUnsigned())
    {
        if (sign < 0)
        {
            OFLOG_ERROR(mdfdsmanLogger,
                "the value is of unsigned type and the given value is out of bound (negative) in VR "
                << vr.getVRName());
            return EC_InvalidValue;
        }

        Uint64 ullValue = 0, limitValue = 0;
        // Use OFIStringStream for portable C++98-compatible 64-bit parsing
        {
            OFIStringStream iss(cmpValue.c_str());
            iss >> ullValue;
            if (iss.fail())
            {
                OFLOG_ERROR(mdfdsmanLogger, "the value could not be parsed as Uint64");
                return EC_InvalidValue;
            }
        }
        if (vr.getValueWidth() == sizeof(Uint16)) // Short, OW, OB
            limitValue = (Uint64)OFnumeric_limits<Uint16>::max();
        else if (vr.getValueWidth() == sizeof(Uint32)) // Long, OL
            limitValue = (Uint64)OFnumeric_limits<Uint32>::max();
        else if (vr.getValueWidth() == sizeof(Uint64)) // Long Long, OV
            limitValue = (Uint64)OFnumeric_limits<Uint64>::max();
        if (ullValue > limitValue)
        {
            OFLOG_ERROR(mdfdsmanLogger,
                "the given value \"" << ullValue << "\" is out of bound in VR " << vr.getVRName());
            return EC_InvalidValue;
        }
        return EC_Normal;
    }

    Sint64 sllValue = 0, limitValue = 0;
    // Use OFIStringStream for portable C++98-compatible 64-bit parsing
    {
        OFIStringStream iss(cmpValue.c_str());
        iss >> sllValue;
        if (iss.fail())
        {
            OFLOG_ERROR(mdfdsmanLogger, "the value could not be parsed as Sint64");
            return EC_InvalidValue;
        }
    }
    Sint64 minValue = 0;
    if (vr.getValueWidth() == sizeof(Sint16)) // Short
    {
        limitValue = (Sint64)OFnumeric_limits<Sint16>::max();
        minValue = (Sint64)OFnumeric_limits<Sint16>::min();
    }
    else if (vr.getValueWidth() == sizeof(Sint32)) // Long
    {
        limitValue = (Sint64)OFnumeric_limits<Sint32>::max();
        minValue = (Sint64)OFnumeric_limits<Sint32>::min();
    }
    else if (vr.getValueWidth() == sizeof(Sint64)) // Long Long
    {
        limitValue = (Sint64)OFnumeric_limits<Sint64>::max();
        minValue = (Sint64)OFnumeric_limits<Sint64>::min();
    }
    else
    {
        // IS (Integer String) has sizeof(char) value width but stores
        // integers in the range of Sint32 per DICOM standard
        limitValue = (Sint64)OFnumeric_limits<Sint32>::max();
        minValue = (Sint64)OFnumeric_limits<Sint32>::min();
    }
    if (sllValue < minValue || sllValue > limitValue)
    {
        OFLOG_ERROR(mdfdsmanLogger,
            "the given value \"" << sllValue << "\" is out of bound in VR " << vr.getVRName());
        return EC_InvalidValue;
    }
    return EC_Normal;
}

static OFCondition evaluateCompareResult(const MdfCondition& condition, const int& compareResult)
{
    // interpret the result
    OFBool result = OFTrue;
    if (condition.isEqualityCheck())
    {
        if (condition.isEqualityCheck(OFFalse)) // operation == "="
            result = compareResult == 0;
        else // operation == "!="
            result = compareResult != 0;
    }
    else if (condition.isLesserCheck()) // operation == "<"
        result = compareResult < 0;
    else if (condition.isLesserCheck(OFTrue)) // operation == "<="
        result = compareResult <= 0;
    else if (condition.isGreaterCheck()) // operation == ">"
        result = compareResult > 0;
    else if (condition.isGreaterCheck(OFTrue)) // operation == ">="
        result = compareResult >= 0;

    if (!result)
        return EC_PathConditionNotMet;
    return EC_Normal;
}


OFCondition MdfDatasetManager::checkCondition(const MdfCondition& condition)
{
    // if no file loaded: return an error
    if (dfile == NULL)
        return makeOFCondition(OFM_dcmdata, 22, OF_error, "No file loaded yet!");

    DcmPathProcessor proc;
    unsigned long vmIndex = 0;
    OFList<DcmPath*> resultPaths;

    // Find the element denoted by the path and perform simple
    // existence checks (E and !E) on elements (not values)
    OFBool condCheckComplete = OFFalse;
    OFBool hasVmIndex = OFFalse;
    OFCondition err = parsePath(proc, condition, vmIndex, hasVmIndex, condCheckComplete);

    if (condCheckComplete || err.bad())
        return err;

    // Validity check
    Uint32 numResultPaths = proc.getResults(resultPaths);
    if (numResultPaths == 0)
        return EC_IllegalCall;

    // At this point, we know that
    // - path is valid
    // - simple existence checks (E and !E) on elements (not values) are already done and handled above.
    // I.e. from this point on we we only care about element values

    // Iterate over all paths that were found
    // and check the condition on the element values
    OFListIterator(DcmPath*) resultPath = resultPaths.begin();
    while (resultPath != resultPaths.end())
    {
        OFString elemValue;
        DcmElement* elem;

        // check if the path is valid and the last element is a leaf element
        // (sequences or items do not make sense for value comparisons)
        err = checkPathAndGetLeafElem(*resultPath, elem);
        if (err.bad())
            return err;

        // check for existence (of value) condition before looking at the more complex ones
        unsigned long numElemValues = elem->getNumberOfValues();
        if (condition.isExistenceCheck())
            return checkExistenceVM(condition, vmIndex, numElemValues);

        // bail out if user tries to compare an element based on a DCMTK-internal helper VR
        DcmVR vr = elem->getTag().getVR();
        if (vr.isForInternalUseOnly())
        {
            OFLOG_ERROR(mdfdsmanLogger, "Unexpected VR for condition check.");
            return EC_InvalidVR;
        }
        // make sure that check does not address element value that is out of bounds
        int compareResult = 0;
        // numElemValues may be 0 if the value of the item is empty
        if (numElemValues > 0 && vmIndex >= numElemValues)
        {
            OFLOG_ERROR(mdfdsmanLogger,
                "specified element in VM (0 - " << numElemValues - 1 << "): " << vmIndex
                << ", index out of bound");
            return EC_TagNotFound;
        }
        // based on desired vm index, get full value string or value from certain index
        if (!hasVmIndex)
            elem->getOFStringArray(elemValue, 0);
        else
            elem->getOFString(elemValue, vmIndex);

        // normalize user-provided value for comparison
        OFString condValue = condition.value;
        stripPadding(condValue, ' ');
        OFLOG_TRACE(mdfdsmanLogger, "Passing \"" << condValue << "\" into the compare with VR: " << vr.getVRName());

        DcmEVR evr = elem->getTag().getEVR();
        // compare number.
        // UN is included here even though isaNumber() is false for it: UN is
        // stored as DcmOtherByteOtherWord internally and must go through the
        // numeric path so that createCompareElement() can handle its hex format.
        // The string path would call getString() which returns raw binary data
        // for DcmOtherByteOtherWord, making string comparison meaningless.
        if (vr.isaNumber() || evr == EVR_UN)
            err = checkNumberCondition(condition, elem, elemValue, condValue, compareResult);
        // compare string
        else if (vr.isaString()
            // AT is not a string but should be compared as such
            || evr == EVR_AT)
            err = checkStringCondition(condition, elem, hasVmIndex, elemValue, condValue, compareResult);
        // give up otherwise
        else { // no specifications defined. no compare possible
            OFLOG_ERROR(mdfdsmanLogger, "Not a String and not a Number, no Compares possible: " << vr.getVRName());
            return EC_InvalidVR;
        }
        // bail out if comparison itself failed (e.g. invalid value for VR)
        if (err.bad())
            return err;
        // evaluate the result of the value comparison (i.e. whether it means passed or failed)
        err = evaluateCompareResult(condition, compareResult);
        if (err.bad())
        {
            OFLOG_DEBUG(mdfdsmanLogger,
                        "Condition failed: (element) " << elemValue << " is not " << condition.operation << " (input) "
                                                       << condValue);
            return err;
        }
        OFLOG_DEBUG(mdfdsmanLogger,
                    "Condition passed: (element) " << elemValue << " " << condition.operation << " (input) " << condValue);
        resultPath++;
    }
    return EC_Normal;
}

OFCondition MdfDatasetManager::modifyOrInsertPath(OFString tag_path,
                                                  const OFString& value,
                                                  const OFBool only_modify,
                                                  const OFBool update_metaheader,
                                                  const OFBool ignore_missing_tags,
                                                  const OFBool no_reservation_checks)
{
    // if no file loaded: return an error
    if (dfile == NULL)
        return makeOFCondition(OFM_dcmdata, 22, OF_error, "No file loaded yet!");

    // Find or create path and perform basic validity checks
    DcmPathProcessor proc;
    OFCondition result = findOrCreateValidPath(proc, tag_path, value, only_modify, ignore_missing_tags, no_reservation_checks);
    if (result.bad())
        return result;

    // Now deal with the value
    // start modifying element value as desired
    OFList<DcmPath*> resultPaths;
    proc.getResults(resultPaths);
    OFListIterator(DcmPath*) resultPath = resultPaths.begin();
    DcmPathNode* lastElement = NULL;
    while (resultPath != resultPaths.end())
    {
        lastElement = (*resultPath)->back();
        if (lastElement == NULL)
            return EC_IllegalCall;
        // if this is a sequence or item, we cannot insert an empty value.
        // findOrCreateValidPath() would have bailed out earlier if a non-empty value would have been provided.
        // We can report success for all paths in the result, since the VR will be the same in all of them.
        if (!lastElement->m_obj->isLeaf() || lastElement->m_obj->ident() == EVR_item)
            return EC_Normal;
        // if tag is already present, start modify operation
        DcmElement* elem = OFstatic_cast(DcmElement*, lastElement->m_obj);
        if (elem == NULL)
            return EC_IllegalCall;
        // Check if pixel data insertion can be performed (and report error if result.bad())
        result = checkPixelDataInsertion(elem);
        if (result.good())
        {
            result = startModify(elem, value);
        }
        if (result.bad())
            return result;
        if (update_metaheader)
            deleteRelatedMetaheaderTag(elem->getTag());
        resultPath++;
    }
    return EC_Normal;
}

OFCondition MdfDatasetManager::modifyOrInsertFromFile(OFString tag_path,
                                                      const OFString& filename,
                                                      const OFBool only_modify,
                                                      const OFBool update_metaheader,
                                                      const OFBool ignore_missing_tags,
                                                      const OFBool no_reservation_checks)
{
    // if no file loaded: return an error
    if (dfile == NULL)
        return makeOFCondition(OFM_dcmdata, 22, OF_error, "No file loaded yet!");

    // first, perform some basic checks on the specified file(name)
    if (filename.empty())
        return makeOFCondition(OFM_dcmdata, 22, OF_error, "No filename specified to read value from!");
    if (!OFStandard::fileExists(filename))
        return makeOFCondition(OFM_dcmdata, 22, OF_error, "File to read value from does not exist!");
    if (!OFStandard::isReadable(filename))
        return makeOFCondition(OFM_dcmdata, 22, OF_error, "File to read value from is not readable!");

    // Find or create path and perform basic validity checks
    DcmPathProcessor proc;
    OFCondition result = findOrCreateValidPath(proc, tag_path, filename, only_modify, ignore_missing_tags, no_reservation_checks);
    if (result.bad())
        return result;

    // start modifying element value as desired
    OFList<DcmPath*> resultPaths;
    proc.getResults(resultPaths);
    OFListIterator(DcmPath*) resultPath = resultPaths.begin();
    DcmPathNode* lastElement;
    while (resultPath != resultPaths.end())
    {
        lastElement = (*resultPath)->back();
        if (lastElement == NULL)
            return EC_IllegalCall;
        // if tag is already present, start modify operation
        DcmElement* elem = OFstatic_cast(DcmElement*, lastElement->m_obj);
        if (elem == NULL)
            return EC_IllegalCall;
        // Check if pixel data insertion can be performed (and report error if result.bad())
        result = checkPixelDataInsertion(elem);
        if (result.good())
        {
            // check whether VR is "unknown"
            DcmEVR vr = elem->getTag().getEVR();
            if (ignore_un_modifies && ((vr == EVR_UN) || (vr == EVR_UNKNOWN) || (vr == EVR_UNKNOWN2B)))
            {
                OFLOG_WARN(mdfdsmanLogger, "will not write value to attribute having VR=UN: " << elem->getTag());
                return EC_Normal;
            }
            // create stream object for binary file
            DcmInputFileStream fileStream(filename.c_str());
            result = fileStream.status();
            if (result.good())
            {
                const size_t fileLen = OFStandard::getFileSize(filename);
                if (fileLen & 1)
                    return makeOFCondition(
                        OFM_dcmdata, 22, OF_error, "Cannot insert/modify value with odd length from file!");
                // read element value from binary file (requires even length)
                result = elem->createValueFromTempFile(
                    fileStream.newFactory(), OFstatic_cast(Uint32, fileLen), EBO_LittleEndian);
            }
            if (result.bad())
                return result;
            if (update_metaheader)
                deleteRelatedMetaheaderTag(elem->getTag());
        }
        resultPath++;
    }
    return EC_Normal;
}

OFCondition MdfDatasetManager::modifyAllTags(OFString tag_path,
                                             const OFString& value,
                                             const OFBool update_metaheader,
                                             int& count,
                                             const OFBool ignore_missing_tags)
{
    // if no file loaded: return an error
    if (dfile == NULL)
        return makeOFCondition(OFM_dcmdata, 22, OF_error, "No file loaded yet!");
    DcmTagKey key;
    OFCondition result;

    // split tag_path into the path itself and the target tag
    result = splitTagPath(tag_path, key);
    if (result.bad())
        return result;

    // check whether tag with this group can be modified
    if (!key.hasValidGroup())
        return makeOFCondition(OFM_dcmdata, 22, OF_error, "Invalid group number!");

    // modify metaheader if necessary; not necessary if not a main level tag
    if (update_metaheader && tag_path.empty())
        deleteRelatedMetaheaderTag(key);
    // this stack will hold result of element search
    DcmStack result_stack;
    DcmObject* elem;
    // get references to all matching tags in dataset and store them in stack
    OFLOG_DEBUG(mdfdsmanLogger, "looking for occurrences of: " << key.toString());
    result = dset->findAndGetElements(key, result_stack);
    // if there are elements found, modify metaheader if necessary
    OFLOG_DEBUG(mdfdsmanLogger, "found " << result_stack.card() << " occurrences");
    // as long there are matching elements left on the stack
    while (result_stack.card() > 0 && result.good())
    {
        // get the top element
        elem = result_stack.pop();
        // if user gives e.g. a sequence delimiter,don't try to change it!
        if (elem->isLeaf())
        {
            // and put new value to element
            OFLOG_DEBUG(mdfdsmanLogger, "accessing existing tag for modify operation");
            result = startModify(OFstatic_cast(DcmElement*, elem), value);
            if (result.good())
                count++;
        }
        // if user gave "unchangeable" tag:
        else
            result = makeOFCondition(OFM_dcmdata, 22, OF_error, "Unable to modify tag!");
    }
    // if desired, handle "tag not found" as being OK
    if (ignore_missing_tags && (result == EC_TagNotFound))
        return EC_Normal;
    return result;
}

OFCondition MdfDatasetManager::deleteTag(OFString tag_path, const OFBool all_tags, const OFBool ignore_missing_tags)
{

    // if no file loaded: return an error
    if (dfile == NULL)
        return makeOFCondition(OFM_dcmdata, 22, OF_error, "No file loaded yet!");

    OFCondition result;
    if (all_tags)
    {
        // split tag path into item path and target tag
        DcmTagKey key;
        result = splitTagPath(tag_path, key);
        if (result.bad())
            return result; // error parsing tag path

        // if group is 0 or 2: abort; deletion of tags with 1,3,5,7 should be allowed
        if ((key.getGroup() == 0) || (key.getGroup() == 2))
            return makeOFCondition(OFM_dcmdata, 22, OF_error, "Cannot delete tags with group 0000 or 0002!");

        // if tag path still contains characters, user wants to modify item tag
        if (!tag_path.empty())
        {
            OFString error;
            DcmItem* item = NULL;
            item          = getItemFromPath(*dset, tag_path.c_str(), error);
            if (item != NULL)
                result = item->findAndDeleteElement(key, all_tags, all_tags);
            else
                return makeOFCondition(OFM_dcmdata, 22, OF_error, error.c_str());
        }
        // other user specified single tag without path
        else
        {
            result = dset->findAndDeleteElement(key, all_tags, all_tags);
        }
        if (ignore_missing_tags && (result == EC_TagNotFound))
            return EC_Normal;
        return result;
    }

    // do not delete all tags -> use path functions
    DcmPathProcessor pathProc;

    Uint32 numDeletions = 0;
    result              = pathProc.findOrDeletePath(dset, tag_path, numDeletions);
    if (ignore_missing_tags && (result == EC_TagNotFound))
        return EC_Normal;
    return result;
}

OFCondition MdfDatasetManager::deletePrivateData()
{
    // if no file loaded : return an error
    if (dfile == NULL)
        return makeOFCondition(OFM_dcmdata, 22, OF_error, "No file loaded yet!");

    DcmStack stack;
    DcmObject* dobj = NULL;
    DcmTagKey tag;
    OFCondition status = dset->nextObject(stack, OFTrue);
    while (status.good())
    {
        dobj = stack.top();
        tag  = dobj->getTag();
        if (tag.getGroup() & 1) // private tag ?
        {
            stack.pop();
            delete OFstatic_cast(DcmItem*, (stack.top()))->remove(dobj);
        }
        status = dset->nextObject(stack, OFTrue);
    }
    return EC_Normal;
}

OFCondition MdfDatasetManager::generateAndInsertUID(const DcmTagKey& uidKey)
{
    // if no file loaded : return an error
    if (dfile == NULL)
        return makeOFCondition(OFM_dcmdata, 22, OF_error, "No file loaded yet!");

    OFCondition result;
    char uid[100];
    if (uidKey == DCM_StudyInstanceUID)
        dcmGenerateUniqueIdentifier(uid, SITE_STUDY_UID_ROOT);
    else if (uidKey == DCM_SeriesInstanceUID)
        dcmGenerateUniqueIdentifier(uid, SITE_SERIES_UID_ROOT);
    else if (uidKey == DCM_SOPInstanceUID)
    {
        dcmGenerateUniqueIdentifier(uid, SITE_INSTANCE_UID_ROOT);
        // force meta-header to refresh SOP Class/Instance UIDs.
        DcmItem* meta_info = dfile->getMetaInfo();
        if (meta_info)
        {
            delete meta_info->remove(DCM_MediaStorageSOPInstanceUID);
        }
    }
    else
        return EC_IllegalCall;
    result = dset->putAndInsertString(uidKey, uid);
    return result;
}

OFCondition MdfDatasetManager::saveFile(const char* file_name,
                                        E_TransferSyntax opt_xfer,
                                        E_EncodingType opt_enctype,
                                        E_GrpLenEncoding opt_glenc,
                                        E_PaddingEncoding opt_padenc,
                                        OFCmdUnsignedInt opt_filepad,
                                        OFCmdUnsignedInt opt_itempad,
                                        OFBool opt_dataset)
{
    // if no file loaded: return an error
    if (dfile == NULL)
        return makeOFCondition(OFM_dcmdata, 22, OF_error, "No file loaded yet!");

    OFCondition result;
    /* check whether transfer syntax is possible */
    if ((opt_xfer == EXS_Unknown) || (dfile->canWriteXfer(opt_xfer)))
    {
        /* check whether pixel data is compressed */
        if (opt_dataset && DcmXfer(opt_xfer).isPixelDataCompressed())
        {
            OFLOG_WARN(mdfdsmanLogger, "encapsulated pixel data requires file format, ignoring --write-dataset");
            opt_dataset = OFFalse;
        }
        /* be sure that we have a known transfer syntax. The original xfer can
         * be unknown if the dataset was created in memory, or if the originally
         * loaded file does not contain attributes which could make sense if
         * it is a template file that should be processed with dcmodify.
         */
        if ((dfile->getDataset()->getOriginalXfer() == EXS_Unknown) && (opt_xfer == EXS_Unknown))
        {
            opt_xfer = EXS_LittleEndianExplicit;
        }
        /* write DICOM file. We deliberately use write mode EWM_fileformat here because
         * EWM_createNewMeta would break the "--no-meta-uid" command line option
         */
        result = dfile->saveFile(file_name,
                                 opt_xfer,
                                 opt_enctype,
                                 opt_glenc,
                                 opt_padenc,
                                 OFstatic_cast(Uint32, opt_filepad),
                                 OFstatic_cast(Uint32, opt_itempad),
                                 (opt_dataset) ? EWM_dataset : EWM_fileformat);
    }
    else
    {
        OFLOG_DEBUG(mdfdsmanLogger,
                    "no conversion to transfer syntax " << DcmXfer(opt_xfer).getXferName() << " possible!");
        result = EC_CannotChangeRepresentation;
    }
    // save file
    OFLOG_INFO(mdfdsmanLogger, "Saving current dataset to file: " << file_name);
    return result;
}

OFCondition MdfDatasetManager::saveFile()
{
    // save file without changing any parameters
    return saveFile(current_file.c_str(), EXS_Unknown, EET_UndefinedLength, EGL_recalcGL, EPD_noChange, 0, 0, OFFalse);
}

OFCondition MdfDatasetManager::startModify(DcmElement* elem, const OFString& value)
{
    OFCondition result;
    DcmEVR vr = elem->getTag().getEVR();
    if (ignore_un_modifies && ((vr == EVR_UN) || (vr == EVR_UNKNOWN) || (vr == EVR_UNKNOWN2B)))
    {
        OFLOG_WARN(mdfdsmanLogger, "will not write value to attribute having VR=UN: " << elem->getTag().toString());
        return EC_Normal;
    }
    // start putString function being defined on all VRs
    result = elem->putString(value.c_str());
    return result;
}

void MdfDatasetManager::deleteRelatedMetaheaderTag(const DcmTagKey& key)
{
    DcmItem* meta_info = dfile->getMetaInfo();
    if (meta_info)
    {
        if (key == DCM_SOPInstanceUID)
            delete meta_info->remove(DCM_MediaStorageSOPInstanceUID);
        else if (key == DCM_SOPClassUID)
            delete meta_info->remove(DCM_MediaStorageSOPClassUID);
    }
}

DcmDataset* MdfDatasetManager::getDataset()
{
    return dset;
}

DcmFileFormat* MdfDatasetManager::getFileFormat()
{
    return dfile;
}

OFString MdfDatasetManager::getFilename() const
{
    return current_file;
}

OFBool MdfDatasetManager::isTagInDictionary(const DcmTagKey& search_key)
{
    const DcmDataDictionary& globalDataDict = dcmDataDict.rdlock();
    const DcmDictEntry* dicent              = globalDataDict.findEntry(search_key, NULL);
    // successful lookup in dictionary -> translate to tag and return
    dcmDataDict.rdunlock();
    if (dicent)
        return OFTrue;
    else
        return OFFalse;
}

void MdfDatasetManager::setModifyUNValues(OFBool modifyUNValues)
{
    ignore_un_modifies = !modifyUNValues;
}

OFCondition MdfDatasetManager::checkPixelDataInsertion(DcmElement* elem)
{
    if (elem->ident() == EVR_PixelData)
    {
        DcmPixelData* pix = OFstatic_cast(DcmPixelData*, elem);
        if (pix)
        {
            E_TransferSyntax ts                        = EXS_Unknown;
            const DcmRepresentationParameter* dontCare = NULL;
            pix->getCurrentRepresentationKey(ts, dontCare);
            DcmXfer xfer(ts);
            if (xfer.usesEncapsulatedFormat())
            {
                OFLOG_ERROR(mdfdsmanLogger, "Cannot replace encapsulated Pixel Data (not implemented)");
                return EC_IllegalParameter;
            }
        }
        else
        {
            OFLOG_WARN(mdfdsmanLogger, "Unexpected error while casting Pixel Data element, trying to ignore");
        }
    }
    return EC_Normal;
}


OFCondition MdfDatasetManager::findOrCreateValidPath(DcmPathProcessor& proc,
                                                     OFString tag_path,
                                                     const OFString& value_or_filename,
                                                     const OFBool only_modify,
                                                     const OFBool ignore_missing_tags,
                                                     const OFBool no_reservation_checks)
{
    // find or create specified path
    proc.checkPrivateReservations(!no_reservation_checks);
    OFCondition result = proc.findOrCreatePath(dset, tag_path, !only_modify /*create if desired*/);
    // if desired, handle tag not found as being not an error
    if ((result == EC_TagNotFound) && only_modify && ignore_missing_tags)
        return EC_Normal;
    if (result.bad())
        return result;
    OFList<DcmPath*> result_paths;
    Uint32 numResultPaths = proc.getResults(result_paths);
    if (numResultPaths == 0)
        return EC_IllegalCall;

    // general validity checking; must only be done for one result
    OFListIterator(DcmPath*) resultPath = result_paths.begin();
    // verify that groups 0 (invalid) and 2 (meta header) were not used
    if ((*resultPath)->containsGroup(0) || (*resultPath)->containsGroup(2))
        return makeOFCondition(OFM_dcmdata, 22, OF_error, "Cannot insert/modify tags with group 0000 or 0002!");
    // also - according to the standard -  groups 1,3,5,7,FF are illegal
    if ((*resultPath)->containsInvalidGroup())
        return makeOFCondition(OFM_dcmdata, 22, OF_error, "Groups 0001,0003,0005,0007,FFFF are illegal!");

    DcmPathNode* lastElement = (*resultPath)->back();
    if (lastElement == NULL)
        return EC_IllegalCall;
    DcmObject* obj = lastElement->m_obj;
    if (obj == NULL)
        return EC_IllegalCall;
    // if object at the end is not a leaf, the insertion is completed (or must fail)
    if (!obj->isLeaf())
    {
        // if user specified a value to be inserted into non-leaf element, return error
        if (!value_or_filename.empty())
            return makeOFCondition(OFM_dcmdata, 22, OF_error, "Cannot put value into non-leaf elements!");
        // non-leaf elements (items/sequences) cannot just be modified
        if (only_modify)
            return makeOFCondition(OFM_dcmdata, 22, OF_error, "Cannot modify non-leaf elements!");
    }

    return EC_Normal;
}



MdfDatasetManager::~MdfDatasetManager()
{
    // cleanup
    delete dfile;
}
