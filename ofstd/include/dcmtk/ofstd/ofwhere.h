/*
 *  This code is derived from the "whereami" library,
 *  https://github.com/gpakosz/whereami,
 *  dual licensed under the WTFPL v2 and MIT licenses without any warranty.
 *  The MIT license is reproduced below.
 *
 *  Copyright (c) 2024 Gregory Pakosz
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the “Software”),
 *  to deal in the Software without restriction, including without limitation
 *  the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *  and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 *  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 *  OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE
 *  OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *  Purpose: Class for various helper functions
 *
 */

#ifndef OFWHERE_H
#define OFWHERE_H

#include "dcmtk/config/osconfig.h"   /* make sure OS specific configuration is included first */
#include "dcmtk/ofstd/ofdefine.h"    /* for DCMTK_OFSTD_EXPORT */

BEGIN_EXTERN_C

/**
 * Returns the path to the current executable.
 *
 * Usage:
 *  - first call `int length = OFgetExecutablePath(NULL, 0, NULL);` to retrieve the length of the path
 *  - allocate the destination buffer with `path = (char*)malloc(length + 1);`
 *  - call `OFgetExecutablePath(path, length, NULL)` again to retrieve the path
 *  - add a terminal NUL character with `path[length] = '\0';`
 *
 * @param out destination buffer, optional
 * @param capacity destination buffer capacity
 * @param dirname_length optional recipient for the length of the dirname part
 *   of the path. Available only when `capacity` is large enough to retrieve the
 *   path.
 *
 * @return the length of the executable path on success (without a terminal NUL character), otherwise `-1`
 */
DCMTK_OFSTD_EXPORT
int OFgetExecutablePath(char* out, int capacity, int* dirname_length);

END_EXTERN_C

#endif // #ifndef OFWHERE_H
