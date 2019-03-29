/*
 *
 *  Copyright (C) 1997-2019, OFFIS e.V.
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
 *  Purpose: test program for C++ string class
 *
 *  This file is derived from a file with the following copyright statement:
 *  Copyright (C) 1994 Free Software Foundation
 *
 *  This file is part of the GNU ANSI C++ Library.  This library is free
 *  software; you can redistribute it and/or modify it under the terms of
 *  the GNU General Public License as published by the Free Software
 *  Foundation; either version 2, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this library; see the file COPYING.  If not, write to the Free
 *  Software Foundation, 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include "dcmtk/config/osconfig.h"     /* include OS specific configuration first */

#include "dcmtk/ofstd/ofstring.h"
#define OFTEST_OFSTD_ONLY
#include "dcmtk/ofstd/oftest.h"


static OFString X = "Hello";
#define Y_str "world\0How are you?"
static OFString Y(Y_str, sizeof(Y_str)/sizeof(char) - 1);
static OFString N = "123";
static OFString c;
static const char *s = ",";

OFTEST(ofstd_OFString_constructor)
{
  OFString x;
  OFCHECK_EQUAL(x, "");

  OFString y("Hell\0", 5);
  OFCHECK_EQUAL(y.length(), 5);
  OFCHECK_EQUAL(y[4], '\0');
  OFCHECK_EQUAL(y, OFString("Hell\0", 5));

  y = "Hello";
  OFCHECK_EQUAL(y, "Hello");

  if (y[y.length()-1] == 'o')
    y = y + '\n';
  OFCHECK_EQUAL(y, "Hello\n");

  y = "Hello";
  OFCHECK_EQUAL(y, "Hello");

  OFString a = y;
  OFCHECK_EQUAL(a, "Hello");
  OFCHECK_EQUAL(a, y);

  OFString b (a, 1, 2);
  OFCHECK_EQUAL(b, "el");

  char ch = '@';
  OFString z (2, ch);
  OFCHECK_EQUAL (z, "@@");

  OFString n ("20");
  OFCHECK_EQUAL(n, "20");

  int i = atoi(n.c_str ());
  OFCHECK_EQUAL(i, 20);

}

OFTEST(ofstd_OFString_concatenate)
{
#define CHECK_EQ(ofstr, c_str) \
  OFCHECK_EQUAL(ofstr, OFString(c_str, sizeof(c_str)/sizeof(char) - 1))

  OFString x = X;
  OFString y = Y;
  OFString z = x + y;
  CHECK_EQ(z, "Helloworld\0How are you?");

  x += y;
  CHECK_EQ(x, "Helloworld\0How are you?");

  y = Y;
  x = X;
  y.insert (0, x);
  CHECK_EQ(y, "Helloworld\0How are you?");

  y = Y;
  x = X;
  y.insert (5, x);
  CHECK_EQ(y, "worldHello\0How are you?");

  y = Y;
  x = X;
  x = x + y + x;
  CHECK_EQ(x, "Helloworld\0How are you?Hello");

  x.erase(5, 17);
  CHECK_EQ(x, "Hello?Hello");

  y = Y;
  x = X;
  x = y + x + x;
  CHECK_EQ(x, "world\0How are you?HelloHello");

  x = X;
  y = Y;
  z = x + s + ' ' + y.substr (y.find ('w'), 1) + y.substr (y.find ('w') + 1, y.find('\0') - y.find('w')) + ".";
  CHECK_EQ(z, "Hello, world\0.");
#undef CHECK_EQ
}

OFTEST(ofstd_OFString_compare)
{
  OFString x = X;
  OFString y = Y;
  OFString n = N;
  OFString z = x + y;

  OFCHECK(x != y);
  OFCHECK_EQUAL(x, "Hello");
  OFCHECK(x != z.substr (0, 4));
  OFCHECK(x.compare (y) < 0);
  OFCHECK(x.compare (z.substr (0, 6)) < 0);

  OFCHECK_EQUAL(x.find ("lo"), 3);
  OFCHECK_EQUAL(x.find ("l", 2), 2);
  OFCHECK_EQUAL(x.rfind ("l"), 3);

  x = "Moin";
  y = "Moin!";
  n = "Moin!";
  z = "Zahl";

  OFCHECK(x.compare(x) == 0);
  OFCHECK(x.compare(y) < 0);
  OFCHECK(x.compare(n) < 0);
  OFCHECK(x.compare(z) < 0);

  OFCHECK(y.compare(x) > 0);
  OFCHECK(y.compare(y) == 0);
  OFCHECK(y.compare(n) == 0);
  OFCHECK(y.compare(z) < 0);

  OFCHECK(z.compare(x) > 0);
  OFCHECK(z.compare(y) > 0);
  OFCHECK(z.compare(n) > 0);
  OFCHECK(z.compare(z) == 0);
}

OFTEST(ofstd_OFString_copy)
{
  OFString x = X;
  size_t copied;
  char buffer[256];

#define CHECK_EQ(c_str, buffer, copied) \
  OFCHECK_EQUAL(OFString(c_str, sizeof(c_str)/sizeof(char) - 1), OFString(buffer, copied))

  copied = x.copy(&buffer[0], sizeof(buffer));
  OFCHECK_EQUAL(copied, 5);
  CHECK_EQ("Hello", buffer, copied);

  copied = x.copy(&buffer[0], 4);
  OFCHECK_EQUAL(copied, 4);
  CHECK_EQ("Hell", buffer, copied);

  copied = x.copy(&buffer[0], 4, 1);
  OFCHECK_EQUAL(copied, 4);
  CHECK_EQ("ello", buffer, copied);

  copied = x.copy(&buffer[0], 4, 2);
  OFCHECK_EQUAL(copied, 3);
  CHECK_EQ("llo", buffer, copied);

  copied = x.copy(&buffer[0], 0);
  OFCHECK_EQUAL(copied, 0);

#undef CHECK_EQ
}

OFTEST(ofstd_OFString_substr)
{
  OFString x = X;

  char ch = x[0];
  OFCHECK_EQUAL(ch, 'H');

  OFString z = x.substr (2, 3);
  OFCHECK_EQUAL(z, "llo");

  x.replace (2, 2, "r");
  OFCHECK_EQUAL(x, "Hero");

  x = X;
  x.replace (0, 1, 1, 'j');
  OFCHECK_EQUAL(x, "jello");
}


OFTEST(ofstd_OFString_reserve)
{
  OFString z;

  OFCHECK(z.empty());

  z.reserve(5);
  OFCHECK_EQUAL(z.length(), 0);
  OFCHECK(z.capacity() >= 5);
  OFCHECK(z.empty());

  z.assign(20, 'z');
  OFCHECK_EQUAL(z, "zzzzzzzzzzzzzzzzzzzz");
  OFCHECK_EQUAL(z.length(), 20);
  OFCHECK(z.capacity() >= 20);
  OFCHECK(!z.empty());

  z.reserve(5);
  OFCHECK_EQUAL(z, "zzzzzzzzzzzzzzzzzzzz");
  OFCHECK_EQUAL(z.length(), 20);
  OFCHECK(z.capacity() >= 20);
  OFCHECK(!z.empty());

  z.resize(5);
  OFCHECK_EQUAL(z, "zzzzz");
  OFCHECK_EQUAL(z.length(), 5);
  OFCHECK(z.capacity() >= 5);
  OFCHECK(!z.empty());

  OFString xxx("xxx");
  z.swap(xxx);
  OFCHECK_EQUAL(z, "xxx");
  OFCHECK_EQUAL(z.length(), 3);
  OFCHECK(z.capacity() >= 3);
  OFCHECK(!z.empty());

  z.clear();
  OFCHECK_EQUAL(z, "");
  OFCHECK_EQUAL(z.length(), 0);
  OFCHECK(z.empty());
}

static void identitytest(OFString a, OFString b)
{
  OFString x = a;
  OFString y = b;
  x += b;
  y.insert (0, a);
  OFCHECK_EQUAL((a + b), x);
  OFCHECK_EQUAL((a + b), y);
  OFCHECK_EQUAL(x, y);

  OFCHECK_EQUAL((a + b + a), (a + (b + a)));

  x.erase (x.rfind (b));
  OFCHECK_EQUAL(x, a);

  y.replace (0, y.rfind (b), b);
  OFCHECK_EQUAL(y, (b + b));
  y.replace (y.find (b), b.length (), a);
  OFCHECK_EQUAL(y, (a + b));
}

OFTEST(ofstd_OFString_identity_1)
{
  identitytest(X, X);
}

OFTEST(ofstd_OFString_identity_2)
{
  identitytest(X, Y);
}

OFTEST(ofstd_OFString_identity_3)
{
  identitytest(X+Y+N+X+Y+N, "A string that will be used in identitytest but is otherwise just another useless string.");
}
