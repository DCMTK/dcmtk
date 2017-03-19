/*
 *
 *  Copyright (C) 2002-2017, OFFIS e.V.
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
 *  Module:  dcmimage
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: class DcmQuantColorHashTable
 *
 */


#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmimage/diqthash.h"
#include "dcmtk/dcmdata/dcxfer.h"      /* for E_TransferSyntax */
#include "dcmtk/dcmimgle/dcmimage.h"    /* for DicomImage */


DcmQuantColorHashTable::DcmQuantColorHashTable()
: m_Table(DcmQuantHashSize, OFnullptr)
{

}


DcmQuantColorHashTable::~DcmQuantColorHashTable()
{
  for (table_iterator it = m_Table.begin(); it != m_Table.end(); delete *it++);
}


unsigned long DcmQuantColorHashTable::countEntries() const
{
  unsigned long result = 0;
  for (const_table_iterator it = m_Table.begin(); it != m_Table.end(); ++it)
    if (*it) result += OFstatic_cast(unsigned long, (*it)->size());
  return result;
}


unsigned long DcmQuantColorHashTable::createHistogram(DcmQuantHistogramItemPointer *& array)
{
  unsigned long numcolors = countEntries();
  array = new DcmQuantHistogramItemPointer[numcolors];
  if (array)
  {
    unsigned long counter = 0;
    for (table_iterator it = m_Table.begin(); it != m_Table.end(); ++it)
      if (*it) (*it)->moveto(array, counter, numcolors);
  }
  return numcolors;
}


unsigned long DcmQuantColorHashTable::addToHashTable(
  DicomImage& image,
  unsigned long newmaxval,
  unsigned long maxcolors)
{
  const unsigned long cols = image.getWidth();
  const unsigned long rows = image.getHeight();
  const unsigned long frames = image.getFrameCount();
  const int bits = sizeof(DcmQuantComponent)*8;

  unsigned long numcolors = 0;
  unsigned long j, k;
  const DcmQuantComponent *cp;
  DcmQuantPixel px;
  const void *data = NULL;

  // compute maxval
  unsigned long maxval = 0;
  for (int bb=0; bb < bits; bb++) maxval = (maxval << 1) | 1;

  DcmQuantScaleTable scaletable;
  scaletable.createTable(maxval, newmaxval);

  DcmQuantComponent r, g, b;

  for (unsigned long ff=0; ff<frames; ff++)
  {
    data = image.getOutputData(bits, ff, 0);
    if (data)
    {
      cp = OFstatic_cast(const DcmQuantComponent *, data);
      for (j = 0; j < rows; j++)
      {
        for (k = 0; k < cols; k++)
        {
          // get pixel
          r = *cp++;
          g = *cp++;
          b = *cp++;
          px.scale(r, g, b, scaletable);

          // lookup and increase if already in hash table
          numcolors += item(px).add(px);
          if (numcolors > maxcolors) return 0;
        }
      }
    }
  }
  return numcolors;
}
