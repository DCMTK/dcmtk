/*
 *
 *  Copyright (C) 1997-2010, OFFIS e.V.
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
 *  Module:  ofstd
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: OFConfigFileNode
 *
 *  Last Update:      $Author: uli $
 *  Update Date:      $Date: 2010-10-20 08:09:34 $
 *  CVS/RCS Revision: $Revision: 1.10 $
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

void OFConfigFileNode::print(STD_NAMESPACE ostream &out, unsigned int level)
{
  if (level > 0)
  {
    unsigned int i;
    for (i = 0; i < level; ++i) out << "[";
    out << keyword_;
    for (i = 0; i < level; ++i) out << "]";
    out << OFendl;

    // print subtree if present
    if (son_)
    {
      son_->print(out, level - 1);
      out << OFendl;
    }
  }
  else
  {
    out << keyword_ << " = ";
    const char *c = value_.c_str();
    while (*c)
    {
      if (*c == '\n') out << "\n    "; else out << *c;
      ++c;
    }
    out << OFendl;
  }

  // continue print on same level if present
  if (brother_) brother_->print(out, level);
}

/* ------------------------------------------------------------------------- */

OFConfigFileCursor::OFConfigFileCursor(const OFConfigFileCursor &source)
: array_(NULL)
, maxLevel_(source.maxLevel_)
{
  array_ = new OFConfigFileNodePtr[maxLevel_ + 1];
  for (unsigned int i = 0; i <= maxLevel_; i++)
    array_[i] = source.array_[i];
}

OFConfigFileCursor& OFConfigFileCursor::operator=(const OFConfigFileCursor &source)
{
  if (this != &source)
  {
    delete[] array_;
    maxLevel_ = source.maxLevel_;
    array_ = new OFConfigFileNodePtr[maxLevel_ + 1];
    for (unsigned int i = 0; i <= maxLevel_; i++)
      array_[i] = source.array_[i];
  }
  return *this;
}

void OFConfigFileCursor::clear()
{
  if (NULL == array_) array_ = new OFConfigFileNodePtr[maxLevel_ + 1];
  for (unsigned int i = 0; i <= maxLevel_; i++)
    array_[i] = NULL;
}

OFBool OFConfigFileCursor::section_valid(unsigned int level) const
{
  OFBool result=OFFalse;
  if ((level <= maxLevel_) && array_)
  {
    result = OFTrue;
    for (int i = maxLevel_; i >= OFstatic_cast(int, level); i--)
      result = result && (array_[i] != NULL);
  }
  return result;
}

void OFConfigFileCursor::set_section(
  unsigned int level,
  const char *key,
  OFConfigFileNode *anchor)
{
  int valid = (level <= maxLevel_) && array_;
  if (valid)
  {
    if (level < maxLevel_) valid = section_valid(level + 1); else valid = 1;
    if (valid)
    {
      first_section(level, anchor);
      while ((section_valid(level)) && array_[level] && (! array_[level]->match(key)))
        next_section(level);
    } else clear();
  } else clear();
}

void OFConfigFileCursor::first_section(
  unsigned int level,
  OFConfigFileNode *anchor)
{
  int valid = (level <= maxLevel_);
  if (valid)
  {
    if (level < maxLevel_) valid=section_valid(level + 1); else valid = 1;
    if (valid)
    {
      if (level < maxLevel_) array_[level] = array_[level + 1]->getSon();
      else array_[level] = anchor;
      for (int i = level - 1; i >= 0; i--) array_[i] = NULL;
    } else clear();
  } else clear();
}

void OFConfigFileCursor::next_section(unsigned int level)
{
  int valid = (level <= maxLevel_);
  if (valid)
  {
    valid = section_valid(level);
    if (valid)
    {
      array_[level] = array_[level]->getBrother();
      for (int i = level - 1; i >= 0; i--) array_[i] = NULL;
    } else clear();
  } else clear();
}

void OFConfigFileCursor::orderedInsert(
  OFConfigFileNode *parent,
  OFConfigFileNode *&newnode)
{
  if (NULL == parent)
  {
    // parent must not be NULL, so this should never happen.
    // Just in case, avoid memory leak.
    delete newnode;
  }
  else
  {
    if (NULL == parent->getSon())
    {
      // parent node does not have any childs yet; we're the first one
      parent->setSon(newnode);
    }
    else
    {
      OFConfigFileNode *leftMostBrother = parent->getSon();
      if (! leftMostBrother->less(newnode->getKeyword()))
      {
        // need to insert at or before leftMostBrother
        if (leftMostBrother->match(newnode->getKeyword()))
        {
          // duplicate entry detected. Replace old entry.
          leftMostBrother->setValue(newnode->getValue());
          delete newnode;
          newnode = leftMostBrother;
        }
        else
        {
          // newnode is new leftmost entry
          newnode->setBrother(leftMostBrother);
          parent->setSon(newnode);
        }
      }
      else
      {
        // need to insert behind leftMostBrother.
        // skip all list entries where the keyword is less, except for the last one
        while (leftMostBrother->getBrother() && leftMostBrother->getBrother()->less(newnode->getKeyword()))
        {
          leftMostBrother = leftMostBrother->getBrother();
        }

        // now we're either at the last entry of the list, or the next entry is greater or equal.
        if (leftMostBrother->getBrother())
        {
          // we're not at the end of the list
          if (leftMostBrother->getBrother()->match(newnode->getKeyword()))
          {
            // duplicate entry detected. Replace old entry.
            leftMostBrother->getBrother()->setValue(newnode->getValue());
            delete newnode;
            newnode = leftMostBrother->getBrother();
          }
          else
          {
            // next entry is larger than newnode; insert here
            newnode->setBrother(leftMostBrother->getBrother());
            leftMostBrother->setBrother(newnode);
          }
        }
        else
        {
          // we're at the end of the list, i.e. newnode needs to be appended.
          leftMostBrother->setBrother(newnode);
        }
      }
    }
  }
}

void OFConfigFileCursor::insert(
  unsigned int level,
  OFConfigFileNode *&newnode,
  OFConfigFileNode *&anchor,
  OFBool orderedMode)
{
  // Implement option that causes entries to be sorted and identical entries to be replaced
  if (level == maxLevel_)
  {
    if (NULL == array_[maxLevel_])
    {
      // newnode is the first entry in the tree, store as anchor
      anchor = newnode;
    }
    else
    {
      if (orderedMode)
      {
        // we need to create the root node of the tree,
        // because orderedInsert() needs a parent.
        OFConfigFileNode rootNode("root");
        rootNode.setSon(anchor);
        orderedInsert(&rootNode, newnode);
        anchor = rootNode.getSon();  // anchor could have changed
        rootNode.setSon(NULL); // destruction of root node shall not destroy whole tree
      }
      else
      {
        array_[maxLevel_]->setBrother(newnode);
      }
    }

    // adjust cursor
    array_[maxLevel_] = newnode;
  }
  else
  {
    if (array_[level + 1])
    {
      if (NULL == array_[level + 1]->getSon())
      {
        // newnode is the first child node of the parent
        array_[level + 1]->setSon(newnode);
      }
      else
      {
        if (orderedMode)
        {
          orderedInsert(array_[level + 1], newnode);
        }
        else
        {
          if (NULL == array_[level]) array_[level] = array_[level + 1]->getSon();
          array_[level]->setBrother(newnode);
        }
      }

      // adjust cursor
      array_[level] = newnode;
    }
    else
    {
      // this should never happen unless the caller passes the wrong level, in which case
      // we at least avoid a memory leak
      delete newnode;
      newnode = NULL;
    }

  }
  if (level > 0) for (int j = level - 1; j >= 0; j--) array_[j] = NULL;
}

/* ------------------------------------------------------------------------- */

const char *OFConfigFile::get_keyword(unsigned int level)
{
  const char *result = NULL;
  int valid = (level <= maxLevel_);
  if ((valid) && (section_valid(level)))
    result = cursor_.getKeyword(level);
  return result;
}

const char *OFConfigFile::get_value()
{
  const char *result = NULL;
  int valid = section_valid(0);
  if (valid) result = cursor_.getValue(0);
  return result;
}

OFBool OFConfigFile::get_bool_value(OFBool defaultvalue)
{
  OFBool result = defaultvalue;
  const char *val = get_value();
  if (val == NULL) return result;
  OFString pstring(val);
  OFString ostring;
  size_t len = pstring.length();
  unsigned char c;
  for (size_t i = 0; i<len; i++)
  {
    c = pstring[i];
    if ((c >= 'a') && (c <= 'z')) ostring += OFstatic_cast(char, toupper(c));
    else if ((c >= 'A') && (c <= 'Z')) ostring += c;
    else if ((c >= '0') && (c <= '9')) ostring += c;
    else if (c == '_') ostring += c;
  }

  if (ostring == "YES")  result = OFTrue; else
  if (ostring == "1")    result = OFTrue; else
  if (ostring == "TRUE") result = OFTrue; else
  if (ostring == "ON")   result = OFTrue; else
  if (ostring == "NO")   result = OFFalse; else
  if (ostring == "0")    result = OFFalse; else
  if (ostring == "FALSE")result = OFFalse; else
  if (ostring == "OFF")  result = OFFalse;
  return result;
}

void OFConfigFile::save_cursor()
{
  stack_.push(cursor_);
}

void OFConfigFile::restore_cursor()
{
  OFBool empty = stack_.empty();
  if (!empty)
  {
    cursor_ = stack_.top();
    stack_.pop();
  } else cursor_.clear();
}

void OFConfigFile::select_section(const char *key1, const char *key2, const char *key3)
{
  if (key3) set_section(3, key3);
  if (key2) set_section(2, key2);
  if (key1) set_section(1, key1);
}

const char *OFConfigFile::get_entry(const char *key0)
{
  const char *result = NULL;
  if (section_valid(1))
  {
    set_section(0, key0);
    result = get_value();
  }
  return result;
}

void OFConfigFile::store_char(char c)
{
  if (bufptr_ == OFstatic_cast(size_t, bufsize_))
  {
     char *oldbuf = buffer_;
     bufsize_ += 1024;
     buffer_ = new char[bufsize_];
     if (buffer_)
     {
       if (oldbuf)
       {
         strncpy(buffer_, oldbuf, bufptr_);
         delete[] oldbuf;
       }
       buffer_[bufptr_++] = c;
     } else {
       buffer_ = oldbuf;
       bufsize_ -= 1024;
     }
  } else buffer_[bufptr_++] = c;
}

char OFConfigFile::read_char(FILE *infile)
{
  char c;
  int done = 0;
  int handled = 0;
  int commentmode = 0;
  while (!done)
  {
    c = OFstatic_cast(char, getc(infile));
    handled = 0;
    if(!handled) if ((feof(infile)) || (ferror(infile)))
    {
      handled = 1;
      done = 1;
    }
    if(!handled) if ((c == 10) && (crfound_) && (isnewline_))
    {
      handled = 1;
      crfound_ = 0;
    }
    if(!handled) if ((c == 10) || (c == 13))
    {
      handled = 1;
      isnewline_ = 1;
      if (c == 13) crfound_ = 1; else crfound_ = 0;
      if (commentmode) commentmode = 0; else
      {
        done = 1;
        c = 10;
      }
    }
    if(!handled) if ((c == commentChar_) && (isnewline_))
    {
      handled = 1;
      commentmode = 1;
      isnewline_ = 0;
    }
    if(!handled) if (!commentmode)
    {
      handled = 1;
      done = 1;
      isnewline_ = 0;
      if (c == 0) c = ' '; // destroy 0 chars
    }
  } // done
  return c;
}

char OFConfigFile::read_keywordchar(FILE *infile)
{
  char c = 0;
  int done = 0;
  while ((!done) && (!feof(infile)) && (!ferror(infile)))
  {
    c = read_char(infile);
    if ((c != ' ') && (c != 9) && (c != 10)) done = 1;
  }
  if ((c > 96) && (c < 123)) c -= 32;
  return c;
}

void OFConfigFile::read_entry(FILE *infile)
{
  char c;
  int done = 0;
  int level = 0;
  int valid = 0;
  c = read_keywordchar(infile);
  if ((!feof(infile)) && (!ferror(infile)))
  {
    if (c == '[')
    {  // this is a higher level keyword
      level++;
      while (!done)
      {
        c = read_keywordchar(infile);
        valid = (!feof(infile)) && (!ferror(infile));
        if ((valid) && (c == '[')) level++; else done = 1;
      }
      if (valid)
      {  // Now we will read the keyword name
        ungetc(c, infile);
        done = 0;
        while (!done)
        {
          c = read_keywordchar(infile);
          valid = (!feof(infile)) && (!ferror(infile));
          if (valid)
          {
            if (((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) || (c == '-') || (c == '_'))
            store_char(c);
            else if (c == ']') done = 1;
          } else done = 1;
        }
      }
      if (valid)
      { // Now we will read the ']'
        int declevel = level - 1;
        done = (declevel == 0);
        while (!done)
        {
          c = read_keywordchar(infile);
          valid = (!feof(infile)) && (!ferror(infile));
          if (valid)
          {
            if (c == ']') declevel--;
            done = (declevel == 0);
          } else done = 1;
        }
      }
      if (level > OFstatic_cast(int, maxLevel_)) level = maxLevel_;
    }
    else
    { // this is a level 0 keyword
      ungetc(c, infile);
      done = 0;
      while (!done)
      {
        c = read_keywordchar(infile);
        valid = (!feof(infile)) && (!ferror(infile));
        if (valid)
        {
          if (((c >= 'A') && (c <= 'Z')) || ((c >= '0') && (c <= '9')) || (c == '-') || (c == '_')) store_char(c);
          else if (c == '=') done = 1;
        } else done = 1;
      }
    } // keyword read

    // Store the keyword.
    store_char(0);
    OFConfigFileNode *newnode = new OFConfigFileNode(buffer_);
    if (newnode) cursor_.insert(level, newnode, anchor_, orderedMode_); // new node stored
    bufptr_ = 0;
    // Read value field for level 0 keywords.
    if (level == 0)
    {
      int skipwhite = 1;
      int justnewline = 0;
      done = 0;
      while (!done)
      {
        c = read_char(infile);
        if ((!feof(infile)) && (!ferror(infile)))
        {
          if (c == 10)
          {
            if (!skipwhite) store_char(c);
            skipwhite = 1;
            justnewline = 1;
          }
          else if ((c == ' ') || (c == 9))
          {
            justnewline = 0;
            if (!skipwhite) store_char(c);
          }
          else
          {
            if (justnewline)
            {
              done = 1;
              ungetc(c, infile);
            } else {
              store_char(c);
              skipwhite = 0;
            }
          }
        } else done = 1;
      }
      store_char(0);
    }
    if ((bufptr_ > 0) && (newnode))
    {
      // remove trailing linefeeds
      while ((bufptr_ > 0) && ((buffer_[bufptr_ - 1] == 10) || (buffer_[bufptr_ - 1] == 0)))
        buffer_[--bufptr_] = 0;
      newnode->setValue(buffer_);
    }
    bufptr_ = 0;
  } // something found
}

void OFConfigFile::print(STD_NAMESPACE ostream &out)
{
  if (anchor_) anchor_->print(out, maxLevel_);
}

void OFConfigFile::loadFile(FILE *infile)
{
  if (infile) while ((!feof(infile)) && (!ferror(infile))) read_entry(infile);
}

OFConfigFile::OFConfigFile(
  FILE *infile,
  unsigned int maxLevel,
  char commentChar,
  OFBool orderedMode)
: stack_()
, cursor_(maxLevel)
, anchor_(NULL)
, isnewline_(1)
, crfound_(0)
, buffer_(NULL)
, bufptr_(0)
, bufsize_(0)
, maxLevel_(maxLevel)
, commentChar_(commentChar)
, orderedMode_(orderedMode)
{
  loadFile(infile);
}

OFConfigFile::~OFConfigFile()
{
  delete anchor_;
  delete[] buffer_;
}


/*
 *  $Log: ofconfig.cc,v $
 *  Revision 1.10  2010-10-20 08:09:34  uli
 *  Also made sure toupper() and tolower() get a valid argument.
 *
 *  Revision 1.9  2010-10-14 13:14:53  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.8  2010-01-06 14:58:37  uli
 *  Fix a use-after-free bug in OFConfigFile on invalid config files.
 *
 *  Revision 1.7  2009-02-13 13:11:23  joergr
 *  Added initialization of member variables to class OFConfigFileCursor in order
 *  to avoid compiler warnings (reported by gcc with additional flags).
 *
 *  Revision 1.6  2008-04-16 09:37:30  meichel
 *  class OFConfigFile now supports an ordered mode where multiple
 *    configuration files can be loaded and can replace entries of other.
 *    Also added function to print content of configuration in reloadable format.
 *
 *  Revision 1.5  2008-04-15 15:46:30  meichel
 *  class OFConfigFile now supports flexible tree depths and configurable
 *    comment characters and can, therefore, fully replace the equivalent
 *    code in module dcmprint.
 *
 *  Revision 1.4  2005/12/08 15:48:52  meichel
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
