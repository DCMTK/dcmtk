/*
 *
 *  Copyright (C) 1997-2011, OFFIS e.V.
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
 *    classes: OFConfigFile
 *
 */

#ifndef OFCONFIG_H
#define OFCONFIG_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofstack.h"
#include "dcmtk/ofstd/ofstream.h"

#define INCLUDE_CSTDIO
#include "dcmtk/ofstd/ofstdinc.h"

/*
 *  Short description of configuration file structure:
 *    - The data in a configuration file have a tree structure.
 *      The tree has a depth defined at instantiation time (by default, OFConfigFile_MaxLevel), 
 *      not including the (imaginary) root node.
 *    - A level 0 entry (a leaf) has the form: KEYWORD = VALUE,
 *      where the keyword starts on row one of a line.
 *    - A level 1 entry has the form [KEYWORD]
 *    - A level 2 entry has the form [[KEYWORD]] (and so on).
 *    - Keywords may consist of:
 *        A..Z, a..z (which are converted to uppercase),
 *        0..9,
 *        '-'
 *    - Values can be any kind of ASCII text. Values may span multiple lines.
 *      To continue a value in the next line, the next line MUST start with
 *      (any amount of) whitespace, which is discarded when reading the value.
 *      linefeeds (converted to ASCII 10 if necessary) are kept in the
 *      value string. Empty lines are discarded (and also their linefeed).
 *    - The data must have a "clean" tree structure. This means that there
 *      MUST be a level 2 keyword before any level 1 keyword etc.
 *    - lines starting with the comment char (default  is "#") are interpreted 
 *      as comment lines.
 *
 */

#define OFConfigFile_MaxLevel 2
#define OFConfigFile_CommentChar '#'

class OFConfigFile;
class OFConfigFileNode;

typedef OFConfigFileNode *OFPConfigFileNode;


/** this class maintains one configuration file entry (key-value pair)
 *  and the links that comprise the configuration file tree structure.
 *  Internal use only.
 */
class DCMTK_OFSTD_EXPORT OFConfigFileNode
{
public:
  /** constructor.
   *  @param keyword configuration keyword, copied into this object
   */
  OFConfigFileNode(const char *keyword);

  /// destructor, recursively deletes whole tree
  ~OFConfigFileNode();

  /** return keyword as C string
   */
  const char *getKeyword() const
  {
    return keyword_.c_str();
  }

  /** return current value as C string
   */
  const char *getValue() const
  {
    return value_.c_str();
  }

  /** set value from C string
   *  @param c C string, must not be NULL
   */
  void setValue(const char *c)
  {
    value_ = c;
  }

  /** check if keyword matches given string
   *  @param c C string, must not be NULL
   *  @return true if equal, false otherwise
   */
  OFBool match(const char *c) const
  {
    return (keyword_ == c);
  }

  /** check if keyword compares "<" to given string
   *  @param c C string, must not be NULL
   *  @return true if equal, false otherwise
   */
  OFBool less(const char *c) const
  {
    return (keyword_ < c);
  }

  /** return pointer to next object in tree on same level
   *  @return pointer to next object, NULL if empty
   */
  OFConfigFileNode *getBrother() const
  {
    return brother_;
  }

  /** return pointer to next object in tree on lower level
   *  @return pointer to lower object, NULL if empty
   */
  OFConfigFileNode *getSon() const
  {
    return son_;
  }

  /** set pointer to next object in tree on same level
   *  @param brother pointer to next object in tree
   */
  void setBrother(OFConfigFileNode *brother)
  {
    brother_ = brother;
  }

  /** set pointer to next object in tree on lower level
   *  @param son pointer to next object in tree
   */
  void setSon(OFConfigFileNode *son)
  {
    son_ = son;
  }

  /** print the content of this node to an output stream
   *  @param out output stream to print to
   *  @param level tree depth (level)
   */
  void print(STD_NAMESPACE ostream& out, unsigned int level);

private:
  /// private undefined copy constructor
  OFConfigFileNode(const OFConfigFileNode& arg);

  /// private undefined copy assignment operator
  OFConfigFileNode &operator=(const OFConfigFileNode& arg);

  /// pointer to next object in tree on same level
  OFConfigFileNode *brother_;

  /// pointer to next object in tree on lower level
  OFConfigFileNode *son_;

  /// configuration keyword
  OFString keyword_;

  /// configuration value
  OFString value_;
};

/// pointer to a OFConfigFileNode object
typedef OFConfigFileNode *OFConfigFileNodePtr;

/** structure used by class OFConfigFile to store a cursor pointing
 *  to the currently selected section and entry in the config data
 */
class DCMTK_OFSTD_EXPORT OFConfigFileCursor
{
public:
  /** default constructor
   */
  OFConfigFileCursor(unsigned int maxLevel) 
  : array_(NULL)
  , maxLevel_(maxLevel)
  { 
  	clear(); 
  }

  /** copy constructor
   */
  OFConfigFileCursor(const OFConfigFileCursor& source);

  /** destructor
   */
  ~OFConfigFileCursor() 
  {
    delete[] array_;
  }

  /** assignment operator
   */
  OFConfigFileCursor& operator=(const OFConfigFileCursor& source);

  /// return object to default constructed state (invalid cursor)
  void clear();

  /** return keyword as C string.
   *  Precondition is that section_valid(level) return true.
   *  @param level cursor level
   */
  const char *getKeyword(unsigned int level) const
  {
    if ((level <= maxLevel_) && array_ && array_[level]) return array_[level]->getKeyword(); else return NULL;
  }

  /** return value as C string
   *  Precondition is that section_valid(level) return true.
   *  @param level cursor level
   */
  const char *getValue(unsigned int level) const
  {
    if ((level <= maxLevel_) && array_ && array_[level]) return array_[level]->getValue(); else return NULL;
  }

  /** checks if the cursor points to a valid location up to
   *  the given level
   *  @param level cursor level
   *  @return true if valid, false otherwise
   */
  OFBool section_valid(unsigned int level) const;

  /** sets cursor to the entry with keyword "key" at the given level.
   *  Cursor must be valid at the level higher than the one specified.
   *  If the keyword exists more than once, the first occurance is found.
   *  If the keyword does not exist, the cursor becomes invalid at the
   *  specified level. The cursor always becomes invalid at all levels
   *  lower than specified.
   *  @param level cursor level
   *  @param key key value
   *  @param anchor pointer to anchor node in config tree
   */
  void set_section(
    unsigned int level, 
    const char *key,
    OFConfigFileNode *anchor);
   
  /** sets cursor to the first entry at the given level (without
   *  changing the cursor position at higher levels).
   *  Cursor must be valid at the level higher than the one specified.
   *  If no section is found the cursor becomes invalid at the
   *  specified level. The cursor always becomes invalid at all levels
   *  lower than specified.
   *  @param level cursor level
   *  @param anchor pointer to anchor node in config tree
   */
  void first_section(
    unsigned int level,
    OFConfigFileNode *anchor);

  /** sets cursor to the next entry at the given level (without
   *  changing the cursor position at higher levels).
   *  Cursor must be valid at the level specified.
   *  If no section is found the cursor becomes invalid at the
   *  specified level. The cursor always becomes invalid at all levels
   *  lower than specified.
   *  @param level cursor level
   */
  void next_section(unsigned int level);

  /** inserts a new node into the tree at the current cursor position
   *  @param level level of insertion
   *  @param newnode node to be inserted, must not be NULL
   *  @param anchor pointer to anchor node of tree, may be modified
   *  @param orderedMode mode flag for processing configuration files.
   *    See documentation of OFConfigFile::orderedMode_
   */
  void insert(
    unsigned int level,
    OFConfigFileNode *& newnode,
    OFConfigFileNode *& anchor,
    OFBool orderedMode);

  /** dummy comparison operator, needed by MSVC5 with STL.
   *  @return always false
   */
  OFBool operator<(const OFConfigFileCursor& /* arg */) const
  {
    return OFFalse;
  }

  /** dummy comparison operator, needed by MSVC5 with STL.
   *  @return always true
   */
  OFBool operator==(const OFConfigFileCursor& /* arg */) const
  {
    return OFTrue;
  }

private:

 /** helper method that is called by insert() in ordered mode
  *  to insert a new node at a given tree level (i.e. within a list
  *  of nodes that share the same parent node) in alphabetical order,
  *  replacing any older node with identical keyword.
  *  @param parent  parent node of new new to be inserted
  *  @param newnode pointer to new node to be inserted
  */
  void orderedInsert(
    OFConfigFileNode *parent,
    OFConfigFileNode *&newnode);

  /// the cursor is an array of pointers to OFConfigFileNode objects
  OFConfigFileNodePtr *array_;

  /// depth of tree, i.e. number of entries in array_
  unsigned int maxLevel_;  
};


/** class for reading and evaluating configuration files.
 *  An object of this class reads upon construction a text
 *  config file and offers methods allowing to browse the
 *  configuration data
 */
class DCMTK_OFSTD_EXPORT OFConfigFile
{
public:

  /** constructor.
   *  @param infile file from which the configuration data is to be read.
   *  @param maxLevel depth of the tree maintained in this config file, default 2
   *  @param commentChar character to start comment lines, default '#'
   *  @param orderedMode mode flag for processing configuration files.
   *    See documentation of member variable orderedMode_
   */
  OFConfigFile(
    FILE *infile, 
    unsigned int maxLevel = OFConfigFile_MaxLevel,
    char commentChar = OFConfigFile_CommentChar,
    OFBool orderedMode = OFFalse);

  /** destructor
   */
  virtual ~OFConfigFile();

  /** load configuration file
   *  @param infile configuration file, must already be open for reading.
   */
  void loadFile(FILE *infile);
  
  /** gets the name of the keyword at the specified level
   *  in the cursor path. Cursor must be valid at the specified level.
   *  @param level cursor level
   *  @return keyword if valid, NULL otherwise.
   */
  const char *get_keyword(unsigned int level);
  
  /** gets the value for the current entry (level 0 keyword).
   *  Cursor must point to a valid entry (i.e. be valid at level 0)
   *  @return value if valid, NULL otherwise.
   */
  const char *get_value();
  
  /** gets the value for the current entry and interprets it as
   *  a boolean value. The keywords "yes", "no", "on", "off", "1", "0", 
   *  "true" and false" are recognised in upper, lower and mixed case.
   *  If the value is different from any recognised keyword, the specified
   *  default is returned
   *  Cursor must point to a valid entry (i.e. be valid at level 0)
   *  @param defaultvalue default to be returned if no keyword is recognised
   *    of if the cursor is invalid.
   *  @return boolean value.
   */
  OFBool get_bool_value(OFBool defaultvalue);
  
  /** checks if the cursor points to a valid entry in the
   *  config data tree up to the the specified level.
   *  @param level tree level (0 for leaf)
   *  @return OFTrue if valid at the specified level.
   */
  OFBool section_valid(unsigned int level) const
  {
    return cursor_.section_valid(level);
  }
  
  /** sets cursor to the entry with keyword "key" at the given level.
   *  Cursor must be valid at the level higher than the one specified.
   *  If the keyword exists more than once, the first occurance is found.
   *  If the keyword does not exist, the cursor becomes invalid at the
   *  specified level. The cursor always becomes invalid at all levels
   *  lower than specified.
   *  @param level cursor level
   *  @param key key value
   */
  void set_section(unsigned int level, const char *key)
  {
    cursor_.set_section(level, key, anchor_);
  }
   
  /** sets cursor to the first entry at the given level (without
   *  changing the cursor position at higher levels).
   *  Cursor must be valid at the level higher than the one specified.
   *  If no section is found the cursor becomes invalid at the
   *  specified level. The cursor always becomes invalid at all levels
   *  lower than specified.
   *  @param level cursor level
   */
  void first_section(unsigned int level)
  {
    cursor_.first_section(level, anchor_);
  }

  /** sets cursor to the next entry at the given level (without
   *  changing the cursor position at higher levels).
   *  Cursor must be valid at the level specified.
   *  If no section is found the cursor becomes invalid at the
   *  specified level. The cursor always becomes invalid at all levels
   *  lower than specified.
   *  @param level cursor level
   */
  void next_section(unsigned int level)
  {
    cursor_.next_section(level);
  }
  
  /** puts the current cursor position on a cursor stack.
   */
  void save_cursor();
  
  /** restores a previously stored cursor position from the cursor stack.
   */
  void restore_cursor();
  
  /** sets the cursor to a different section. This "shortcut" method allows
   *  to specify multiple section levels at the same time.
   *  The cursor becomes invalid when the section is not found or if parameters
   *  for more level then present in the tree are specified.
   *  @param key1 level 1 section key, i.e. [KEY]
   *  @param key2 level 2 section key, i.e. [[KEY]]. If omitted, section 2 remains unchanged.
   *  @param key2 level 3 section key, i.e. [[[KEY]]]. If omitted, section 3 remains unchanged.
   */
  void select_section(
    const char *key1,
    const char *key2=NULL,
    const char *key3=NULL);
  
  /** sets the cursor to the given level 0 keyword and returns
   *  the string value assigned to this keyword.
   *  @param key0 level 0 keyword
   *  @return value string if found, NULL otherwise
   */
  const char *get_entry(const char *key0);

  /** print the content of the configuration to an output stream
   *  @param out output stream to print to
   */
  void print(STD_NAMESPACE ostream& out);

private:  

  /** reads the next character from the input file,
   *  maintains the current line number and filters out
   *  comments.
   *  @param infile the file to be read
   *  @return next character
   */
  char read_char(FILE *infile);

  /** reads the next non-whitespace character from
   *  the input file and returns as uppercase character.
   *  @param infile the file to be read
   *  @return next non-whitespace character
   */
  char read_keywordchar(FILE *infile);
  
  /** reads a complete entry from the config file.
   *  An entry can either be a section heading or
   *  an entry of the form key=value. The entry is inserted
   *  into the config data tree managed by this object.
   *  @param infile the file to be read
   */
  void read_entry(FILE *infile);
  
  /** writes a character to the string buffer maintained in
   *  "buffer". Automatically resizes buffer if necessary.
   *  @param c character to be stored
   */
  void store_char(char c);

  /** private undefined copy constructor
   */
  OFConfigFile(const OFConfigFile&);
  
  /** private undefined assignment operator
   */
  OFConfigFile& operator=(const OFConfigFile&);


  /// stack of cursor positions that can be saved and restored
  OFStack<OFConfigFileCursor> stack_;

  /// current cursor position
  OFConfigFileCursor cursor_;

  /// anchor to data tree
  OFConfigFileNode *anchor_;

  /// flag indicating whether newline during file read
  int isnewline_;

  /// flag indicating whether CR was read during file read
  int crfound_;

  /// buffer during file read
  char *buffer_;

  /// index into buffer during file read
  size_t bufptr_;

  /// buffer size during file read
  long bufsize_;

  /// depth of tree, i.e. number of entries in array_
  unsigned int maxLevel_;  

  /// character starting comment lines  
  char commentChar_;
  
  /** mode flag for reading configuration file. If false, new entries to the
   *  tree containing the configuration file are just appended and not sorted.
   *  This allows multiple entries with the same keyword to be present, only the
   *  first of which will be found by search-by-keyword methods such as set_section,
   *  select_section or get_entry. In ordered mode, the tree is maintained in sorted
   *  order and a new entry will replace an older entry with identical keyword.
   *  This permits multiple configuration files to "overlay" each other but is
   *  somewhat slower when reading the configuration file.
   */
  OFBool orderedMode_;
};

#endif
