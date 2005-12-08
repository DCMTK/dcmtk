/*
 *
 *  Copyright (C) 1997-2005, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  ofstd
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: OFConfigFileNode
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2005-12-08 15:48:52 $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofconfig.h"
#include "dcmtk/ofstd/ofcast.h"

#define INCLUDE_CSTRING
#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"


OFConfigFileNode::OFConfigFileNode(const char *keyword)
: brother_(NULL)
, son_(NULL)
, keyword_(keyword)
, value_()
{
}

OFConfigFileNode::~OFConfigFileNode()
{
  // recursively delete tree
  delete son_;
  delete brother_;
}

/* ------------------------------------------------------------------------- */

OFConfigFileCursor::OFConfigFileCursor(const OFConfigFileCursor& source)
{
  for (int i=0; i<=OFConfigFile_MaxLevel; i++) ptr[i]=source.ptr[i];
}

OFConfigFileCursor& OFConfigFileCursor::operator=(const OFConfigFileCursor& source)
{
  if (this != &source)
  { 
    for (int i=0; i<=OFConfigFile_MaxLevel; i++) ptr[i]=source.ptr[i];
  }
  return *this;
}

void OFConfigFileCursor::clear()
{
  for (int i=0; i<= OFConfigFile_MaxLevel; i++) ptr[i]=NULL;
}

OFBool OFConfigFileCursor::section_valid(unsigned int level) const
{
  OFBool result=OFFalse;
  if (level <= OFConfigFile_MaxLevel)
  {
    result = OFTrue;
    for (int i = OFConfigFile_MaxLevel; i >= OFstatic_cast(int, level); i--)
      result = result && (ptr[i] != NULL);
  }
  return result;
}

void OFConfigFileCursor::set_section(
    unsigned int level, 
    const char *key,
    OFConfigFileNode *anchor)
{
  int valid=(level<=OFConfigFile_MaxLevel);
  if (valid)
  {
    if (level<OFConfigFile_MaxLevel) valid=section_valid(level+1); else valid=1;
    if (valid)
    {
      first_section(level, anchor);
      while ((section_valid(level))&&(! ptr[level]->match(key)))
        next_section(level);
    } else clear();
  } else clear();
}

void OFConfigFileCursor::first_section(
  unsigned int level,
  OFConfigFileNode *anchor)
{
  int valid= (level<=OFConfigFile_MaxLevel);
  if (valid)
  {
    if (level<OFConfigFile_MaxLevel) valid=section_valid(level+1); else valid=1;
    if (valid)
    {
      if (level<OFConfigFile_MaxLevel) ptr[level] = ptr[level+1]->getSon();
      else ptr[level] = anchor;
      for (int i=level-1; i>=0; i--) ptr[i]=NULL;
    } else clear();
  } else clear();
}

void OFConfigFileCursor::next_section(unsigned int level)
{
  int valid= (level<=OFConfigFile_MaxLevel);
  if (valid)
  {
    valid=section_valid(level);
    if (valid)
    {
      ptr[level] = ptr[level]->getBrother();
      for (int i=level-1; i>=0; i--) ptr[i]=NULL;
    } else clear();
  } else clear();
}

void OFConfigFileCursor::insert(
  unsigned int level,
  OFConfigFileNode *newnode,
  OFConfigFileNode *& anchor)
{
  if (level==OFConfigFile_MaxLevel)
  {
    if (ptr[OFConfigFile_MaxLevel])
      ptr[OFConfigFile_MaxLevel]->setBrother(newnode);
      else anchor = newnode;
    ptr[OFConfigFile_MaxLevel] = newnode;
  } 
  else 
  {
    if (ptr[level])
    {
      ptr[level]->setBrother(newnode);
      ptr[level] = newnode;
    } else {
      if (ptr[level+1])
      {
        ptr[level+1]->setSon(newnode);
        ptr[level] = newnode;
      }
    }
  }
  if (level > 0) for (int j=level-1; j>=0; j--) ptr[j]=NULL;
}

/* ------------------------------------------------------------------------- */

const char *OFConfigFile::get_keyword(unsigned int level)
{
  const char *result = NULL;
  int valid=(level<=OFConfigFile_MaxLevel);
  if ((valid)&&(section_valid(level)))
    result = cursor.getKeyword(level);
  return result;
}

const char *OFConfigFile::get_value()
{
  const char *result=NULL;
  int valid=section_valid(0);
  if (valid) result=cursor.getValue(0);
  return result;
}

OFBool OFConfigFile::get_bool_value(OFBool defaultvalue)
{
  OFBool result = defaultvalue;
  const char *val = get_value();
  if (val==NULL) return result;
  OFString pstring(val);
  OFString ostring;
  size_t len = pstring.length();
  char c;
  for (size_t i=0; i<len; i++)
  {
    c = pstring[i];
    if ((c>='a') && (c <= 'z')) ostring += OFstatic_cast(char, toupper(c));
    else if ((c>='A') && (c <= 'Z')) ostring += c;
    else if ((c>='0') && (c <= '9')) ostring += c;
    else if (c=='_')  ostring += c;
  }

  if (ostring=="YES")  result=OFTrue; else
  if (ostring=="1")    result=OFTrue; else
  if (ostring=="TRUE") result=OFTrue; else
  if (ostring=="ON")   result=OFTrue; else
  if (ostring=="NO")   result=OFFalse; else
  if (ostring=="0")    result=OFFalse; else
  if (ostring=="FALSE")result=OFFalse; else
  if (ostring=="OFF")  result=OFFalse;
  return result;
}

void OFConfigFile::save_cursor()
{
  stack.push(cursor);
}

void OFConfigFile::restore_cursor()
{
  OFBool empty = stack.empty();
  if (!empty)
  {
    cursor = stack.top();
    stack.pop();
  } else cursor.clear();
}

void OFConfigFile::select_section(const char *key1, const char *key2)
{
  if (key2) set_section(2,key2);
  if ((key1)&&(section_valid(2))) set_section(1,key1);
}

const char *OFConfigFile::get_entry(const char *key0)
{
  const char *result=NULL;
  if (section_valid(1))
  {
    set_section(0,key0);
    result = get_value();
  }
  return result;
}

void OFConfigFile::store_char(char c)
{
  if (bufptr == OFstatic_cast(size_t, bufsize))
  {
     char *oldbuf=buffer;
     bufsize += 1024;
     buffer = new char[bufsize];
     if (buffer)
     {
       if (oldbuf)
       {
         strncpy(buffer, oldbuf, bufptr);
         delete[] oldbuf;
       }
       buffer[bufptr++] = c;
     } else {
       buffer=oldbuf;
       bufsize -= 1024;
     }
  } else buffer[bufptr++] = c;
}

char OFConfigFile::read_char(FILE *infile)
{
  char c;
  int done=0;
  int handled=0;
  int commentmode=0;
  while (!done)
  {
    c = OFstatic_cast(char, getc(infile));
    handled=0;
    if(!handled) if ((feof(infile))||(ferror(infile)))
    {
      handled=1;
      done=1;
    }
    if(!handled) if ((c==10)&&(crfound)&&(isnewline))
    {
      handled=1;
      crfound=0;
    }
    if(!handled) if ((c==10)||(c==13))
    {
      handled=1;
      isnewline=1;
      if (c==13) crfound=1; else crfound=0;
      if (commentmode) commentmode=0; else
      {
        done=1;
        c=10;
      }
    }
    if(!handled) if ((c=='#')&&(isnewline))
    {
      handled=1;
      commentmode=1;
      isnewline=0;
    }
    if(!handled) if (!commentmode)
    {
      handled=1;
      done=1;
      isnewline=0;
      if (c==0) c=' '; // destroy 0 chars
    }
  } // done
  return c;
}

char OFConfigFile::read_keywordchar(FILE *infile)
{
  char c=0;
  int done=0;
  while ((!done)&&(!feof(infile))&&(!ferror(infile)))
  {
    c = read_char(infile);
    if ((c != ' ')&&(c != 9)&&(c != 10)) done=1;
  }
  if ((c>96)&&(c<123)) c -= 32;
  return c;
}

void OFConfigFile::read_entry(FILE *infile)
{
  char c;
  int done=0;
  int level=0;
  int valid=0;
  c = read_keywordchar(infile);
  if ((!feof(infile))&&(!ferror(infile)))
  {
    if (c=='[')
    {  // this is a higher level keyword
      level++;
      while (!done)
      {
        c = read_keywordchar(infile);
        valid = (!feof(infile))&&(!ferror(infile));
        if ((valid)&&(c=='[')) level++; else done=1;
      }
      if (valid)
      {  // Now we will read the keyword name
        ungetc(c, infile);
        done = 0;
        while (!done)
        {
          c = read_keywordchar(infile);
          valid = (!feof(infile))&&(!ferror(infile));
          if (valid)
          {
            if (((c>='A')&&(c<='Z'))||((c>='0')&&(c<='9'))||(c=='-')||(c=='_'))
            store_char(c);
            else if (c==']') done=1;
          } else done=1;
        }
      }
      if (valid)
      { // Now we will read the ']'
        int declevel = level-1;
        done = (declevel==0);
        while (!done)
        {
          c = read_keywordchar(infile);
          valid = (!feof(infile))&&(!ferror(infile));
          if (valid)
          {
            if (c==']') declevel--;
            done = (declevel==0);
          } else done=1;
        }
      }
      if (level>OFConfigFile_MaxLevel) level=OFConfigFile_MaxLevel;
    }
    else
    { // this is a level 0 keyword
      ungetc(c, infile);
      done = 0;
      while (!done)
      {
        c = read_keywordchar(infile);
        valid = (!feof(infile))&&(!ferror(infile));
        if (valid)
        {
          if (((c>='A')&&(c<='Z'))||((c>='0')&&(c<='9'))||(c=='-')||(c=='_')) store_char(c);
          else if (c=='=') done=1;
        } else done=1;
      }
    } // keyword read
    // Store the keyword.
    store_char(0);
    OFConfigFileNode *newnode = new OFConfigFileNode(buffer);
    if (newnode) cursor.insert(level, newnode, anchor); // new node stored
    bufptr =0;
    // Read value field for level 0 keywords.
    if (level==0)
    {
      int skipwhite = 1;
      int justnewline =0;
      done = 0;
      while (!done)
      {
        c = read_char(infile);
        if ((!feof(infile))&&(!ferror(infile)))
        {
          if (c==10)
          {
            if (!skipwhite) store_char(c);
            skipwhite=1;
            justnewline=1;
          }
          else if ((c==' ')||(c==9))
          {
            justnewline=0;
            if (!skipwhite) store_char(c);
          }
          else
          {
            if (justnewline)
            {
              done=1;
              ungetc(c, infile);
            } else {
              store_char(c);
              skipwhite=0;
            }
          }
        } else done=1;
      }
      store_char(0);
    }
    if ((bufptr>0)&&(newnode))
    {
      // remove trailing linefeeds
      while ((bufptr>0)&&((buffer[bufptr-1]==10)||(buffer[bufptr-1]==0)))
        buffer[--bufptr]=0;
      newnode->setValue(buffer);
    }
    bufptr =0;
  } // something found
}

OFConfigFile::OFConfigFile(FILE *infile)
: stack()
, cursor()
, anchor(NULL)
, isnewline(1)
, crfound(0)
, buffer(NULL)
, bufptr(0)
, bufsize(0)
{
  if (infile) while ((!feof(infile))&&(!ferror(infile))) read_entry(infile);
}

OFConfigFile::~OFConfigFile()
{
  delete anchor;
  delete[] buffer;
}

/*
 *  $Log: ofconfig.cc,v $
 *  Revision 1.4  2005-12-08 15:48:52  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.3  2003/07/09 13:58:04  meichel
 *  Adapted type casts to new-style typecast operators defined in ofcast.h
 *
 *  Revision 1.2  2003/05/12 12:54:55  meichel
 *  Fixed off-by-one bug in for loop
 *
 *  Revision 1.1  2003/04/29 10:14:08  meichel
 *  Moved configuration file parser from module dcmpstat to ofstd and renamed
 *    class to OFConfigFile. Cleaned up implementation (no more friend declarations).
 *
 *
 */

