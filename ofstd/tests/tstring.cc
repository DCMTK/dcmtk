/*
 *
 *  Module:  ofstd
 *
 *  Purpose: test programm for C++ string class
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-05-24 09:48:29 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/tests/tstring.cc,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


// Tests for the -*- C++ -*- string classes.
// Copyright (C) 1994 Free Software Foundation

// This file is part of the GNU ANSI C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the terms of
// the GNU General Public License as published by the Free Software
// Foundation; either version 2, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this library; see the file COPYING.  If not, write to the Free
// Software Foundation, 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.


#ifdef ORIGINAL_GNU_INCLUDES 
#include <std/string.h>
#include <iostream.h>
#include <std/cstdlib.h>
#include <std/cassert.h>
#endif

#undef HAVE_STD_STRING

#include "ofstring.h"
#include "ofconsol.h"

#define string OFString


string X = "Hello";
string Y = "world";
string N = "123";
string c;
const char *s = ",";

void decltest()
{
  string x;
  COUT << "an empty string:" << x << "\n";
  assert(x == "");

  string y = "Hello";
  COUT << "A string initialized to Hello:" << y << "\n";
  assert(y == "Hello");

  if (y[y.length()-1] == 'o')
	y = y + '\n';
  assert(y == "Hello\n");
  y = "Hello";

  string a = y;
  COUT << "A string initialized to previous string:" << a << "\n";
  assert(a == "Hello");
  assert(a == y);

  string b (a, 1, 2);
  COUT << "A string initialized to (previous string, 1, 2):" << b << "\n";
  assert(b == "el");

  char ch = '@';
  string z (1, ch);
  COUT << "A string initialized to @:" << z << "\n";
  assert (z == "@");

  string n ("20");
  COUT << "A string initialized to 20:" << n << "\n";
  assert(n == "20");

  int i = atoi(n.c_str ());
  double f = atof(n.c_str ());
  COUT << "n = " << n << " atoi(n) = " << i << " atof(n) = " << f << "\n";
  assert(i == 20);
  assert(f == 20);

}

void cattest()
{
  string x = X;
  string y = Y;
  string z = x + y;
  COUT << "z = x + y = " << z << "\n";
  assert(z == "Helloworld");

  x += y;
  COUT << "x += y; x = " << x << "\n";
  assert(x == "Helloworld");

  y = Y;
  x = X;
  y.insert (0, x);
  COUT << "y.insert (0, x); y = " << y << "\n";
  assert(y == "Helloworld");

  y = Y;
  x = X;
  x = x + y + x;
  COUT << "x = x + y + x; x = " << x << "\n";
  assert(x == "HelloworldHello");

  y = Y;
  x = X;
  x = y + x + x;
  COUT << "x = y + x + x; x = " << x << "\n";
  assert(x == "worldHelloHello");

  x = X;
  y = Y;
  z = x + s + ' ' + y.substr (y.find ('w'), 1) + y.substr (y.find ('w') + 1) + ".";
  COUT << "z = x + s +  + y.substr (y.find (w), 1) + y.substr (y.find (w) + 1) + . = " << z << "\n";
  assert(z == "Hello, world.");
}

void comparetest()
{  
  string x = X;
  string y = Y;
  string n = N;
  string z = x + y;

  assert(x != y);
  assert(x == "Hello");
  assert(x != z.substr (0, 4));
  assert(x.compare (y) < 0);
  assert(x.compare (z.substr (0, 6)) < 0);

  assert(x.find ("lo") == 3);
  assert(x.find ("l", 2) == 2);
  assert(x.rfind ("l") == 3);
}

void substrtest()
{
  string x = X;

  char ch = x[0];
  COUT << "ch = x[0] = " << ch << "\n";
  assert(ch == 'H');

  string z = x.substr (2, 3);
  COUT << "z = x.substr (2, 3) = " << z << "\n";
  assert(z == "llo");

  x.replace (2, 2, "r");
  COUT << "x.replace (2, 2, r); x = " << x << "\n";
  assert(x == "Hero");

  x = X;
  x.replace (0, 1, 1, 'j');
  COUT << "x.replace (0, 1, 'j'); x = " << x << "\n";
  assert(x == "jello");
}

void iotest()
{
  string z;
  COUT << "enter a word:";
  cin >> z;
  COUT << "word =" << z << " ";
  COUT << "length = " << z.length() << "\n";
}

void identitytest(string a, string b)
{
  string x = a;
  string y = b;
  x += b;
  y.insert (0, a);
  assert((a + b) == x);
  assert((a + b) == y);
  assert(x == y);
  
  assert((a + b + a) == (a + (b + a)));

  x.erase (x.rfind (b));
  assert(x == a);

  y.replace (0, y.rfind (b), b);
  assert(y == (b + b));
  y.replace (y.find (b), b.length (), a);
  assert(y == (a + b));
}

int main()
{
  decltest();
  cattest();
  comparetest();
  substrtest();
  identitytest(X, X);
  identitytest(X, Y);
  identitytest(X+Y+N+X+Y+N, "A string that will be used in identitytest but is otherwise just another useless string.");
  iotest();
  COUT << "\nEnd of test\n";
  return 0;
}


/*
**
** CVS/RCS Log:
** $Log: tstring.cc,v $
** Revision 1.4  2002-05-24 09:48:29  joergr
** Added "const" modifier to char pointer to avoid warnings reported by gcc
** 2.95.3 with additional options.
**
** Revision 1.3  2000/03/03 14:02:52  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.2  1998/11/27 12:42:10  joergr
** Added copyright message to source files and changed CVS header.
**
**
**
*/
