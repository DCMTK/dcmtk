/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Module:  dcmnet
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: 
 *    class DcmAssociationConfigurationFile
 * 
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-10-20 08:09:34 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmnet/dcasccff.h"
#include "dcmtk/dcmnet/dcasccfg.h" /* for class DcmAssociationConfiguration */
#include "dcmtk/dcmdata/dcerror.h"  /* for EC_IllegalCall */
#include "dcmtk/ofstd/ofconfig.h" /* for class OFConfigFile */

#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"


/* config file keys for the association configuration */

#define L2_EXTENDEDNEGOTIATION         "EXTENDEDNEGOTIATION"
#define L2_PRESENTATIONCONTEXTS        "PRESENTATIONCONTEXTS"
#define L2_PROFILES                    "PROFILES"
#define L2_SCPSCUROLESELECTION         "SCPSCUROLESELECTION"
#define L2_TRANSFERSYNTAXES            "TRANSFERSYNTAXES"

#define L0_EXTENDEDNEGOTIATION         "EXTENDEDNEGOTIATION"
#define L0_EXTENDEDNEGOTIATION_X       "EXTENDEDNEGOTIATION"
#define L0_PRESENTATIONCONTEXTS        "PRESENTATIONCONTEXTS"
#define L0_PRESENTATIONCONTEXT_X       "PRESENTATIONCONTEXT"
#define L0_ROLE_X                      "ROLE"
#define L0_SCPSCUROLESELECTION         "SCPSCUROLESELECTION"
#define L0_TRANSFERSYNTAX_X            "TRANSFERSYNTAX"


OFCondition DcmAssociationConfigurationFile::initialize(
  DcmAssociationConfiguration& cfg,
  const char *filename)
{
  if (!filename) return EC_IllegalCall;

  FILE *cfgfile = fopen(filename, "rb");
  if (cfgfile == NULL)
  {
    OFString s("unable to open configuration file: ");
    s += filename;
    return makeOFCondition(OFM_dcmnet, 1042, OF_error, s.c_str());    
  }

  OFConfigFile config(cfgfile);
  fclose(cfgfile);

  // parse transfer syntaxes
  OFCondition result = parseTransferSyntaxes(cfg, config);
  if (result.bad()) return result;

  // parse presentation contexts
  result = parsePresentationContexts(cfg, config);
  if (result.bad()) return result;

  // parse role selection items
  result = parseRoleSelectionItems(cfg, config);
  if (result.bad()) return result;

  // parse extended negotiation items
  result = parseExtendedNegotiationItems(cfg, config);
  if (result.bad()) return result;

  // parse profiles
  result = parseProfiles(cfg, config);

  return result;
}

OFCondition DcmAssociationConfigurationFile::parseTransferSyntaxes(
  DcmAssociationConfiguration& cfg,
  OFConfigFile& config)
{
  config.set_section(2, L2_TRANSFERSYNTAXES);
  if (! config.section_valid(2))
  {
    OFString s("cannot find section [[");
    s += L2_TRANSFERSYNTAXES;
    s += "]] in config file";
    return makeOFCondition(OFM_dcmnet, 1050, OF_error, s.c_str());      
  }

  char buf[64];
  unsigned int counter;
  OFBool found;
  const char *key = NULL;
  const char *value = NULL;
  OFCondition result = EC_Normal;

  config.first_section(1);
  while (config.section_valid(1))
  {
    key = config.get_keyword(1);
    counter = 0;
    found = OFTrue;    
    while (found)
    {
      sprintf(buf, "%s%u", L0_TRANSFERSYNTAX_X, ++counter);
      value = config.get_entry(buf);
      if (value)
      {
        result = cfg.addTransferSyntax(key, value);
        if (result.bad()) return result;
      } else found = OFFalse;
    }
    config.next_section(1);
  }

  return result;
}

OFCondition DcmAssociationConfigurationFile::parsePresentationContexts(
  DcmAssociationConfiguration& cfg,
  OFConfigFile& config)
{
  config.set_section(2, L2_PRESENTATIONCONTEXTS);
  if (! config.section_valid(2))
  {
    OFString s("cannot find section [[");
    s += L2_PRESENTATIONCONTEXTS;
    s += "]] in config file";
    return makeOFCondition(OFM_dcmnet, 1051, OF_error, s.c_str());      
  }

  char buf[64];
  unsigned int counter;
  OFBool found;
  const char *key = NULL;
  const char *value = NULL;
  OFString abstractSyntaxUID;
  OFString transferSyntaxKey;
  size_t separator = 0;
  size_t i;
  size_t len;
  unsigned char c;
  OFCondition result = EC_Normal;

  config.first_section(1);
  while (config.section_valid(1))
  {
    key = config.get_keyword(1);
    counter = 0;
    found = OFTrue;    
    while (found)
    {
      sprintf(buf, "%s%u", L0_PRESENTATIONCONTEXT_X, ++counter);
      value = config.get_entry(buf);
      if (value)
      {       
        // value must consist of abstract syntax and transfer syntax, separated by '\\'         
        abstractSyntaxUID = value;
        len = abstractSyntaxUID.size();
        separator = abstractSyntaxUID.find("\\");
        if (separator == OFString_npos)
        {
          OFString s("syntax error: missing '\\' in entry ");
          s += buf;
          s += " in config file";
          return makeOFCondition(OFM_dcmnet, 1052, OF_error, s.c_str());      
        }
        
        // do name mangling for transfer syntax key
        transferSyntaxKey.clear();
        i = separator;
        while (++i < len)
        {
          c = abstractSyntaxUID.at(i);
          if (! isspace(c)) transferSyntaxKey += (char) (toupper(c));
        }

        // finally cut abstract syntax name           
        abstractSyntaxUID.erase(separator);

        result = cfg.addPresentationContext(key, abstractSyntaxUID.c_str(), transferSyntaxKey.c_str());
        if (result.bad()) return result;

      } else found = OFFalse;
    }
    config.next_section(1);
  }

  return result;
}


#define TO_UCHAR(s) OFstatic_cast(unsigned char, (s))
OFCondition DcmAssociationConfigurationFile::parseRoleSelectionItems(
  DcmAssociationConfiguration& cfg,
  OFConfigFile& config)
{
  OFCondition result = EC_Normal;

  config.set_section(2, L2_SCPSCUROLESELECTION);
  if (! config.section_valid(2)) return result; // SCP/SCU role selection is optional, may be absent

  char buf[64];
  unsigned int counter;
  OFBool found;
  const char *key = NULL;
  const char *value = NULL;
  OFString abstractSyntaxUID;
  OFString roleKey;
  size_t separator = 0;
  size_t i;
  size_t len;
  char c;

  config.first_section(1);
  while (config.section_valid(1))
  {
    key = config.get_keyword(1);
    counter = 0;
    found = OFTrue;    
    while (found)
    {
      sprintf(buf, "%s%u", L0_ROLE_X, ++counter);
      value = config.get_entry(buf);
      if (value)
      {       
        // value must consist of abstract syntax and role, separated by '\\'         
        abstractSyntaxUID = value;
        len = abstractSyntaxUID.size();
        separator = abstractSyntaxUID.find("\\");
        if (separator == OFString_npos)
        {
          OFString s("syntax error: missing '\\' in entry ");
          s += buf;
          s += " in config file";
          return makeOFCondition(OFM_dcmnet, 1054, OF_error, s.c_str());      
        }
        
        // do name mangling for role key
        roleKey.clear();
        i = separator;
        while (++i < len)
        {
          c = abstractSyntaxUID.at(i);
          roleKey += (char) (toupper(TO_UCHAR(c)));
        }

        // finally cut abstract syntax name           
        abstractSyntaxUID.erase(separator);

        // check role key
        if (roleKey == "SCU")
        {
          result = cfg.addRole(key, abstractSyntaxUID.c_str(), ASC_SC_ROLE_SCU);
        }
        else if (roleKey == "SCP")
        {
          result = cfg.addRole(key, abstractSyntaxUID.c_str(), ASC_SC_ROLE_SCP);
        }
        else if (roleKey == "BOTH")
        {
          result = cfg.addRole(key, abstractSyntaxUID.c_str(), ASC_SC_ROLE_SCUSCP);
        }
        else
        {
          OFString s("syntax error: unknown role key in entry ");
          s += buf;
          s += " in config file";
          result = makeOFCondition(OFM_dcmnet, 1055, OF_error, s.c_str());      
        }

        if (result.bad()) return result;

      } else found = OFFalse;
    }
    config.next_section(1);
  }

  return result;
}


OFCondition DcmAssociationConfigurationFile::parseExtendedNegotiationItems(
  DcmAssociationConfiguration& cfg,
  OFConfigFile& config)
{
  OFCondition result = EC_Normal;

  config.set_section(2, L2_EXTENDEDNEGOTIATION);
  if (! config.section_valid(2)) return result; // extended negotiation is optional, may be absent

  char buf[64];
  unsigned char raw[1024];
  size_t rawSize = 0;
  unsigned int counter;
  OFBool found;
  const char *key = NULL;
  const char *value = NULL;
  OFString abstractSyntaxUID;
  OFString tempString;
  size_t separator = 0;
  size_t i;
  size_t len;
  unsigned short us = 0;
  OFBool parseError = OFFalse;
  char c;

  config.first_section(1);
  while (config.section_valid(1))
  {
    key = config.get_keyword(1);
    counter = 0;
    found = OFTrue;    
    while (found)
    {
      sprintf(buf, "%s%u", L0_EXTENDEDNEGOTIATION_X, ++counter);
      value = config.get_entry(buf);
      if (value)
      {       
        // value must consist of abstract syntax and extended negotiation data, separated by '\\'
        abstractSyntaxUID = value;
        len = abstractSyntaxUID.size();
        separator = abstractSyntaxUID.find("\\");
        if (separator == OFString_npos)
        {
          OFString s("syntax error: missing '\\' in entry ");
          s += buf;
          s += " in config file";
          return makeOFCondition(OFM_dcmnet, 1057, OF_error, s.c_str());      
        }

        if (len - separator > 2048)
        {
          OFString s("extended negotiation data too long, cannot handle > 1024 bytes in entry ");
          s += buf;
          s += " in config file";
          return makeOFCondition(OFM_dcmnet, 1058, OF_error, s.c_str());      
        }
        
        // do hex to binary conversion for raw data
        tempString.clear();
        i = separator + 1;
        rawSize = 0;
        while (i < len)
        {
          tempString = abstractSyntaxUID.substr(i, 2);
          parseError = OFTrue;
          if (tempString.size() == 2)
          {
            c = tempString[0];
            if (((c >= '0') && (c <= '9'))||((c >= 'a') && (c <= 'f'))||((c >= 'A') && (c <= 'F')))
            {
              c = tempString[1];
              if (((c >= '0') && (c <= '9'))||((c >= 'a') && (c <= 'f'))||((c >= 'A') && (c <= 'F')))
              {
                if (1 == sscanf(tempString.c_str(), "%hx", &us))
                {
                  raw[rawSize++] = (unsigned char) us;
                  parseError = OFFalse;
                }
              }
            }
          }

          if (parseError)
          {
            OFString s("parse error in extended negotiation data in entry ");
            s += buf;
            s += " in config file";
            return makeOFCondition(OFM_dcmnet, 1059, OF_error, s.c_str());      
          }
          i += 2;
        }

        // finally cut abstract syntax name           
        abstractSyntaxUID.erase(separator);

        result = cfg.addExtendedNegotiation(key, abstractSyntaxUID.c_str(), raw, (Uint32) rawSize);
        if (result.bad()) return result;

      } else found = OFFalse;
    }
    config.next_section(1);
  }

  return result;
}


OFCondition DcmAssociationConfigurationFile::parseProfiles(
  DcmAssociationConfiguration& cfg,
  OFConfigFile& config)
{
  config.set_section(2, L2_PROFILES);
  if (! config.section_valid(2))
  {
    OFString s("cannot find section [[");
    s += L2_PROFILES;
    s += "]] in config file";
    return makeOFCondition(OFM_dcmnet, 1060, OF_error, s.c_str());      
  }

  const char *key = NULL;
  const char *context = NULL;
  const char *role = NULL;
  const char *extneg = NULL;
  OFString scontext;
  OFString srole;
  OFString sextneg;
  const char *c;
  OFCondition result = EC_Normal;

  config.first_section(1);
  while (config.section_valid(1))
  {
    key = config.get_keyword(1);
    context = config.get_entry(L0_PRESENTATIONCONTEXTS);
    if (! context)
    {
      OFString s("no ");
      s += L0_PRESENTATIONCONTEXTS;
      s += "  entry in section ";
      s += key;
      s += " in config file";
      return makeOFCondition(OFM_dcmnet, 1061, OF_error, s.c_str());      
    }
    role = config.get_entry(L0_SCPSCUROLESELECTION);
    extneg = config.get_entry(L0_EXTENDEDNEGOTIATION);

    // do name mangling for presentation context key
    c = context;
    scontext.clear();
    while (*c)
    {
      if (! isspace(TO_UCHAR(*c))) scontext += (char) (toupper(TO_UCHAR(*c)));
      ++c;
    }
    context = scontext.c_str();

    // do name mangling for role selection key
    if (role)
    {
      c = role;
      srole.clear();
      while (*c)
      {
        if (! isspace(TO_UCHAR(*c))) srole += (char) (toupper(TO_UCHAR(*c)));
        ++c;
      }
      role = srole.c_str();
    }

    // do name mangling for extended negotiation key
    if (extneg)
    {
      c = extneg;
      sextneg.clear();
      while (*c)
      {
        if (! isspace(TO_UCHAR(*c))) sextneg += (char) (toupper(TO_UCHAR(*c)));
        ++c;
      }
      extneg = sextneg.c_str();
    }

    result = cfg.addProfile(key, context, role, extneg);
    if (result.bad()) return result;
    config.next_section(1);
  }

  return result;
}
#undef TO_UCHAR


/*
 * CVS/RCS Log
 * $Log: dcasccff.cc,v $
 * Revision 1.7  2010-10-20 08:09:34  uli
 * Also made sure toupper() and tolower() get a valid argument.
 *
 * Revision 1.6  2010-10-20 07:41:35  uli
 * Made sure isalpha() & friends are only called with valid arguments.
 *
 * Revision 1.5  2010-10-14 13:14:27  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.4  2005/12/08 15:44:26  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.3  2003/08/11 18:31:46  joergr
 * Included "ctype" header file required for gcc 3.2.3 on Debian Linux.
 *
 * Revision 1.2  2003/06/18 11:53:42  meichel
 * Fixed bug in association configuration file parser
 *
 * Revision 1.1  2003/06/10 14:30:15  meichel
 * Initial release of class DcmAssociationConfiguration and support
 *   classes. This class maintains a list of association negotiation
 *   profiles that can be addressed by symbolic keys. The profiles may
 *   be read from a configuration file.
 *
 *
 */
