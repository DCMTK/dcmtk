/*
 *
 *  Copyright (C) 1998-2000, OFFIS
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
 *  Module: dcmpstat
 *
 *  Author: Marco Eichelberg
 *
 *  Purpose:
 *    classes: DVPSConfig
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2000-03-08 16:28:49 $
 *  CVS/RCS Revision: $Revision: 1.3 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#ifndef __DVPSCONF_H__
#define __DVPSCONF_H__

#include "osconfig.h"    /* make sure OS specific configuration is included first */

class DVPSConfig;

#include <stdio.h>

#include "ofstring.h"
#include "ofstack.h"

#define DVPSConfig_MaxLevel 2

/*
 *  Short description of configuration file structure:
 *    - The data in a configuration file have a tree structure.
 *      The tree has a fixed depth of (DVPSConfig_MaxLevel), not including
 *      the (imaginary) root node.
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
 *    - lines starting with "#" are interpreted as comment lines.
 *
 */

class DVPSConfigNode;
typedef DVPSConfigNode *DVPSPConfigNode;

/** structure used by class DVPSConfig to store a cursor pointing
 *  to the currently selected section and entry in the config data
 */
class DVPSConfigCursor
{
public:
  /** default constructor
   */
  DVPSConfigCursor() {invalidate();}

  /** copy constructor
   */
  DVPSConfigCursor(const DVPSConfigCursor& source);

  /** destructor
   */
  ~DVPSConfigCursor() {}

  /** assignment operator
   */
  DVPSConfigCursor& operator=(const DVPSConfigCursor& source);

private:
  DVPSPConfigNode ptr[DVPSConfig_MaxLevel +1];
  void invalidate();
  
  friend class DVPSConfig;
};


/** class for reading and evaluating configuration files.
 *  An object of this class reads upon construction a text
 *  config file and offers methods allowing to browse the
 *  configuration data
 */
class DVPSConfig
{
public:

  /** constructor.
   *  @param infile file from which the configuration data is to be read.
   */
  DVPSConfig(FILE *infile);
  /** destructor
   */
  virtual ~DVPSConfig();

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
   *  config data tree on the specified level.
   *  @param level tree level (0 for leaf)
   *  @return OFTrue if valid at the specified level.
   */
  OFBool section_valid(unsigned int level);
  
  /** sets cursor to the entry with keyword "key" at the given level.
   *  Cursor must be valid at the level higher than the one specified.
   *  If the keyword exists more than once, the first occurance is found.
   *  If the keyword does not exist, the cursor becomes invalid at the
   *  specified level. The cursor always becomes invalid at all levels
   *  lower than specified.
   *  @param level cursor level
   *  @param key key value
   */
  void  set_section(unsigned int level, const char *key);
   
  /** sets cursor to the first entry at the given level (without
   *  changing the cursor position at higher levels).
   *  Cursor must be valid at the level higher than the one specified.
   *  If no section is found the cursor becomes invalid at the
   *  specified level. The cursor always becomes invalid at all levels
   *  lower than specified.
   *  @param level cursor level
   */
  void  first_section(unsigned int level);

  /** sets cursor to the next entry at the given level (without
   *  changing the cursor position at higher levels).
   *  Cursor must be valid at the level specified.
   *  If no section is found the cursor becomes invalid at the
   *  specified level. The cursor always becomes invalid at all levels
   *  lower than specified.
   *  @param level cursor level
   */
  void  next_section(unsigned int level);
  
  /** puts the current cursor position on a cursor stack.
   */
  void  save_cursor();
  
  /** restores a previously stored cursor position from the cursor stack.
   */
  void  restore_cursor();
  
  /** sets the cursor to a different section. This "shortcut" method allows
   *  to specify both section levels at the same time.
   *  The cursor becomes invalid when the section is not found.
   *  @param key1 level 1 section key, i.e. [KEY]
   *  @param key2 level 2 section key, i.e. [[KEY]]. If omitted, section 2 remains unchanged.
   */
  void select_section(
    const char *key1,
    const char *key2=NULL);
  
  /** sets the cursor to the given level 0 keyword and returns
   *  the string value assigned to this keyword.
   *  @param key0 level 0 keyword
   *  @return value string if found, NULL otherwise
   */
  const char *get_entry(const char *key0);

private:  
  /// stack of cursor positions that can be saved and restored
  OFStack<DVPSConfigCursor> stack;
  /// current cursor position
  DVPSConfigCursor cursor;
  /// anchor to data tree
  DVPSConfigNode *anchor;
  /// linenumber during file read
  long linenumber;
  /// flag indicating whether newline during file read
  int isnewline;
  /// flag indicating whether CR was read during file read
  int crfound;
  /// buffer during file read
  char *buffer;
  /// index into buffer during file read
  size_t bufptr;
  /// buffer size during file read
  long bufsize;

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
  DVPSConfig(const DVPSConfig&);
  
  /** private undefined assignment operator
   */
  DVPSConfig& operator=(const DVPSConfig&);
  
};

#endif /* __DVPSCONF_H__ */

/*
 *  $Log: dvpsconf.h,v $
 *  Revision 1.3  2000-03-08 16:28:49  meichel
 *  Updated copyright header.
 *
 *  Revision 1.2  1999/04/30 16:40:47  meichel
 *  Minor code purifications to keep Sun CC 2.0.1 quiet
 *
 *  Revision 1.1  1999/01/15 17:26:29  meichel
 *  added configuration file facility (derived from dcmprint)
 *    and a sample config file.
 *
 *
 */

