/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *  Update Date:      $Date: 1999-01-15 17:26:33 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dvpsconf.h"
#include "ofstring.h"

#include <string.h>
#include <ctype.h>   /* for toupper() */

class DVPSConfigNode
{
private:
  DVPSConfigNode *brother, *son;
  OFString keyword;
  OFString value;
  long linenumber;

  DVPSConfigNode();
  DVPSConfigNode(const DVPSConfigNode& arg);  
  ~DVPSConfigNode();
  DVPSConfigNode &operator=(const DVPSConfigNode& arg);
  
friend class DVPSConfig;
};

DVPSConfigNode::DVPSConfigNode()
: brother(NULL), son(NULL), keyword(), value(), linenumber(0)
{
}

DVPSConfigNode::~DVPSConfigNode()
{
  if (son) delete son;
  if (brother) delete brother;
}

void DVPSConfigCursor::invalidate()
{
  for (int i=0; i<= DVPSConfig_MaxLevel; i++) ptr[i]=NULL;
}


const char *DVPSConfig::get_keyword(unsigned int level)
{
  const char *result = NULL;
  int valid=(level<=DVPSConfig_MaxLevel);
  if ((valid)&&(section_valid(level)))
    result = cursor.ptr[level]->keyword.c_str();
  return result;
}

const char *DVPSConfig::get_value()
{
  const char *result=NULL;
  int valid=section_valid(0);
  if (valid) result=cursor.ptr[0]->value.c_str();
  return result;
}

OFBool DVPSConfig::get_bool_value(OFBool defaultvalue)
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
    if ((c>='a') && (c <= 'z')) ostring += (char)(toupper(c));
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

OFBool DVPSConfig::section_valid(unsigned int level)
{
  int result=0;
  if (level<=DVPSConfig_MaxLevel)
  {
    result = 1;
    for (int i=DVPSConfig_MaxLevel; i>=(int)level; i--)
      result = result && (cursor.ptr[i] != NULL);
  }
  if (result) return OFTrue; else return OFFalse;
}

void DVPSConfig::set_section(unsigned int level, const char *key)
{
  int valid=(level<=DVPSConfig_MaxLevel);
  if (valid)
  {
    if (level<DVPSConfig_MaxLevel) valid=section_valid(level+1); else valid=1;
    if (valid)
    {
      first_section(level);
      while ((section_valid(level))&&(cursor.ptr[level]->keyword != key))
        next_section(level);
    } else cursor.invalidate();
  } else cursor.invalidate();
}

void DVPSConfig::first_section(unsigned int level)
{
  int valid= (level<=DVPSConfig_MaxLevel);
  if (valid)
  {
    if (level<DVPSConfig_MaxLevel) valid=section_valid(level+1); else valid=1;
    if (valid)
    {
      if (level<DVPSConfig_MaxLevel) cursor.ptr[level] =
        cursor.ptr[level+1]->son;
      else cursor.ptr[level] = anchor;
      for (int i=level-1; i>=0; i--) cursor.ptr[i]=NULL;
    } else cursor.invalidate();
  } else cursor.invalidate();
}

void DVPSConfig::next_section(unsigned int level)
{
  int valid= (level<=DVPSConfig_MaxLevel);
  if (valid)
  {
    valid=section_valid(level);
    if (valid)
    {
      cursor.ptr[level] = cursor.ptr[level]->brother;
      for (int i=level-1; i>=0; i--) cursor.ptr[i]=NULL;
    } else cursor.invalidate();
  } else cursor.invalidate();
}

void DVPSConfig::save_cursor()
{
  stack.push(cursor);
}

void DVPSConfig::restore_cursor()
{
  OFBool empty = stack.empty();
  if (!empty) 
  {
    cursor = stack.top();
    stack.pop();
  } else cursor.invalidate();
}

void DVPSConfig::select_section(const char *key1, const char *key2)
{
  if (key2) set_section(2,key2);
  if ((key1)&&(section_valid(2))) set_section(1,key1);
}

const char *DVPSConfig::get_entry(const char *key0)
{
  const char *result=NULL;
  if (section_valid(1))
  {
    set_section(0,key0);
    result = get_value();
  }
  return result;
}

void DVPSConfig::store_char(char c)
{
  if (bufptr == (size_t)bufsize)
  {
     char *oldbuf=buffer;
     bufsize += 1024;
     buffer = new char[bufsize];
     if (buffer)
     {
       if (oldbuf) 
       {
         strncpy(buffer, oldbuf, bufptr);
         delete oldbuf;
       }
       buffer[bufptr++] = c;
     } else {
       buffer=oldbuf;
       bufsize -= 1024;
     }
  } else buffer[bufptr++] = c;
}

char DVPSConfig::read_char(FILE *infile)
{
  char c;
  int done=0;
  int handled=0;
  int commentmode=0;
  while (!done)
  {
    c = (char)(getc(infile));
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
      linenumber++;
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

char DVPSConfig::read_keywordchar(FILE *infile)
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

void DVPSConfig::read_entry(FILE *infile)
{
  char c;
  int done=0;
  int level=0;
  int valid=0;
  c = read_keywordchar(infile);
  long newlinenumber=linenumber;
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
      if (level>DVPSConfig_MaxLevel) level=DVPSConfig_MaxLevel;
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
    DVPSConfigNode *newnode = new DVPSConfigNode;
    if (newnode)
    {
      newnode->keyword = buffer;
      newnode->value = "";
      newnode->linenumber = newlinenumber;
      if (level==DVPSConfig_MaxLevel)
      {
        if (cursor.ptr[DVPSConfig_MaxLevel])
          cursor.ptr[DVPSConfig_MaxLevel]->brother = newnode;
          else anchor = newnode;
        cursor.ptr[DVPSConfig_MaxLevel] = newnode;
      } else {
        if (cursor.ptr[level])
        {
          cursor.ptr[level]->brother = newnode;
          cursor.ptr[level] = newnode;
        } else {
          if (cursor.ptr[level+1])
          {
            cursor.ptr[level+1]->son = newnode;
            cursor.ptr[level] = newnode;
          } 
        }
      }
      if (level>0) for (int j=level-1; j>=0; j--) cursor.ptr[j]=NULL;
    } // new node stored
    bufptr =0;
    // Read value field for level 0 keywords.
    if (level==0)
    {
      int skipws = 1;
      int justnewline =0;
      done = 0;
      while (!done)
      {
        c = read_char(infile);
        if ((!feof(infile))&&(!ferror(infile)))
        {
          if (c==10) 
          {
            if (!skipws) store_char(c);
            skipws=1;
            justnewline=1;
          }
          else if ((c==' ')||(c==9)) 
          {
            justnewline=0;
            if (!skipws) store_char(c); 
          }
          else
          {
            if (justnewline)
            {
              done=1;
              ungetc(c, infile);
            } else {
              store_char(c);
              skipws=0;
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
      newnode->value = buffer;
    }
    bufptr =0;
  } // something found
}

DVPSConfig::DVPSConfig(FILE *infile)
: stack()
, cursor()
, anchor(NULL)
, linenumber(1)
, isnewline(1)
, crfound(0)
, buffer(NULL)
, bufptr(0)
, bufsize(0)
{
  if (infile) while ((!feof(infile))&&(!ferror(infile))) read_entry(infile);
}

DVPSConfig::~DVPSConfig()
{
  if (anchor) delete anchor;
  if (buffer) delete buffer;
}

/*
 *  $Log: dvpsconf.cc,v $
 *  Revision 1.1  1999-01-15 17:26:33  meichel
 *  added configuration file facility (derived from dcmprint)
 *    and a sample config file.
 *
 *
 */

