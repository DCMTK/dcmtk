/*
 *
 *  Copyright (C) 1996-2002, OFFIS
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
 *  Module:  dcmimage
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: class DcmQuantColorTable
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2002-11-27 14:16:58 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/libsrc/diqtctab.cc,v $
 *  CVS/RCS Revision: $Revision: 1.4 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"
#include "diqtctab.h"

#include "ofconsol.h"
#include "diqtpbox.h"
#include "dcerror.h"   /* for EC_IllegalCall */
#include "dcelem.h"    /* for DcmElement */
#include "dcitem.h"    /* for DcmItem */
#include "dcvrus.h"    /* for DcmUnsignedShort */
#include "dcvrobow.h"  /* for DcmOtherByteOtherWord */
#include "dcswap.h"    /* for swapIfNecessary() */
#include "dcdeftag.h"  /* for tag constants */
#include "dcuid.h"     /* for OFFIS_DCMTK_VERSION */

#define INCLUDE_CSTDIO
#include "ofstdinc.h"

/* ------------------------------------------------------------ */

// static comparison functions for qsort

static int redcompare(const void *x1, const void *x2) 
{
  return (int) (*(DcmQuantHistogramItemPointer *) x1)->getRed() 
       - (int) (*(DcmQuantHistogramItemPointer *) x2)->getRed();
}


static int greencompare(const void *x1, const void *x2) 
{
  return (int) (*(DcmQuantHistogramItemPointer *) x1)->getGreen() 
       - (int) (*(DcmQuantHistogramItemPointer *) x2)->getGreen();
}


static int bluecompare(const void *x1, const void *x2) 
{
  return (int) (*(DcmQuantHistogramItemPointer *) x1)->getBlue() 
       - (int) (*(DcmQuantHistogramItemPointer *) x2)->getBlue();
}

/* ------------------------------------------------------------ */

DcmQuantColorTable::DcmQuantColorTable()
: array(NULL)
, numColors(0)
, maxval(0)
{
}


DcmQuantColorTable::~DcmQuantColorTable()
{
  clear();
}


void DcmQuantColorTable::clear()
{
  if (array)
  {
  	for (unsigned long i=0; i < numColors; i++) delete array[i];
    delete[] array;
    array = NULL;
  }
  numColors = 0;
  maxval = 0;
}


OFCondition DcmQuantColorTable::computeHistogram(
  DicomImage& image, 
  unsigned long maxcolors)
{
  // reset object to initial state
  clear();

  // compute initial maxval
  maxval = (DcmQuantComponent) -1;  
  DcmQuantColorHashTable *htable = NULL;

  // attempt to make a histogram of the colors, unclustered.
  // If at first we don't succeed, lower maxval to increase color
  // coherence and try again.  This will eventually terminate.
  OFBool done = OFFalse;
  while (! done)
  {
    htable = new DcmQuantColorHashTable();
    numColors = htable->addToHashTable(image, maxval, maxcolors);
    if (numColors > 0) done = OFTrue;
    else
    {
      delete htable;
      maxval = maxval/2;
    }
  }

  numColors = htable->createHistogram(array);
  delete htable;
  return EC_Normal;
}


OFCondition DcmQuantColorTable::medianCut(
  DcmQuantColorTable& histogram, 
  unsigned long sum, 
  unsigned long theMaxval, 
  unsigned long numberOfColors,
  DcmLargestDimensionType largeType,
  DcmRepresentativeColorType repType)
{
  // reset object to initial state
  clear();

  // update maxval
  maxval = theMaxval;

  // initialize color table
  array = new DcmQuantHistogramItemPointer[numberOfColors];
  for (unsigned int xx=0; xx < numberOfColors; xx++) array[xx] = new DcmQuantHistogramItem();
  numColors = numberOfColors;

  register int i;
  register unsigned int bi;
  DcmQuantPixelBoxArray bv(numberOfColors);
  
  // Set up the initial box.
  bv[0].ind = 0;
  bv[0].colors = (int) histogram.getColors();
  bv[0].sum = sum;
  unsigned int boxes = 1;
  
  // Main loop: split boxes until we have enough.
  while ( boxes < numberOfColors )
  {
      register int indx, clrs;
      unsigned long sm;
      register int minr, maxr, ming, maxg, minb, maxb, v;
      unsigned long halfsum, lowersum;
      
      // Find the first splittable box.
      for ( bi = 0; bi < boxes; ++bi )
          if ( bv[bi].colors >= 2 )
          break;
      if ( bi == boxes )
          break;  /* ran out of colors! */
      indx = bv[bi].ind;
      clrs = bv[bi].colors;
      sm = bv[bi].sum;
      
      // Go through the box finding the minimum and maximum of each
      // component - the boundaries of the box.
      minr = maxr = histogram.array[indx]->getRed();
      ming = maxg = histogram.array[indx]->getGreen();
      minb = maxb = histogram.array[indx]->getBlue();
      for ( i = 1; i < clrs; ++i )
      {
          v = histogram.array[indx+i]->getRed();
          if ( v < minr ) minr = v;
          if ( v > maxr ) maxr = v;
          v = histogram.array[indx+i]->getGreen();
          if ( v < ming ) ming = v;
          if ( v > maxg ) maxg = v;
          v = histogram.array[indx+i]->getBlue();
          if ( v < minb ) minb = v;
          if ( v > maxb ) maxb = v;
      }
      
      /*
      ** Find the largest dimension, and sort by that component.  I have
      ** included two methods for determining the "largest" dimension;
      ** first by simply comparing the range in RGB space, and second
      ** by transforming into luminosities before the comparison.  
      */
      if (largeType == DcmLargestDimensionType_default)
      {
          if ( maxr - minr >= maxg - ming && maxr - minr >= maxb - minb )
              qsort((char*) &(histogram.array[indx]), clrs, sizeof(DcmQuantHistogramItemPointer), redcompare);
          else if ( maxg - ming >= maxb - minb )
              qsort((char*) &(histogram.array[indx]), clrs, sizeof(DcmQuantHistogramItemPointer), greencompare);
          else
              qsort((char*) &(histogram.array[indx]), clrs, sizeof(DcmQuantHistogramItemPointer), bluecompare);
       }
       else // DcmLargestDimensionType_luminance
       {         	
          double rl, gl, bl;
          DcmQuantPixel p;
          
          p.assign(maxr - minr, 0, 0);
          rl = p.luminance();
          
          p.assign(0, maxg - ming, 0);
          gl = p.luminance();
          
          p.assign(0, 0, maxb - minb);
          bl = p.luminance();
                    
          if ( rl >= gl && rl >= bl )
              qsort((char*) &(histogram.array[indx]), clrs, sizeof(DcmQuantHistogramItemPointer), redcompare );
          else if ( gl >= bl )
              qsort((char*) &(histogram.array[indx]), clrs, sizeof(DcmQuantHistogramItemPointer), greencompare );
          else
              qsort((char*) &(histogram.array[indx]), clrs, sizeof(DcmQuantHistogramItemPointer), bluecompare );
      }
      
      /*
      ** Now find the median based on the counts, so that about half the
      ** pixels (not colors, pixels) are in each subdivision.
      */
      lowersum = histogram.array[indx]->getValue();
      halfsum = sm / 2;
      for ( i = 1; i < clrs - 1; ++i )
      {
          if ( lowersum >= halfsum )
          break;
          lowersum += histogram.array[indx+i]->getValue();
      }
      
      // Split the box, and sort to bring the biggest boxes to the top.
      bv[bi].colors = i;
      bv[bi].sum = lowersum;
      bv[boxes].ind = indx + i;
      bv[boxes].colors = clrs - i;
      bv[boxes].sum = sm - lowersum;
      ++boxes;
      bv.sort(boxes);
  }
  
  /*
  ** Ok, we've got enough boxes.  Now choose a representative color for
  ** each box.  There are a number of possible ways to make this choice.
  ** One would be to choose the center of the box; this ignores any structure
  ** within the boxes.  Another method would be to average all the colors in
  ** the box - this is the method specified in Heckbert's paper.  A third
  ** method is to average all the pixels in the box.  
  */
  if (repType == DcmRepresentativeColorType_centerOfBox)
  {
      for ( bi = 0; bi < boxes; ++bi )
      {
          register int indx = bv[bi].ind;
          register int clrs = bv[bi].colors;
          register int minr, maxr, ming, maxg, minb, maxb, v;
      
          minr = maxr = histogram.array[indx]->getRed();
          ming = maxg = histogram.array[indx]->getGreen();
          minb = maxb = histogram.array[indx]->getBlue();        
          for ( i = 1; i < clrs; ++i )
          {
              v = histogram.array[indx+i]->getRed();
              minr = (minr < v ? minr : v);
              maxr = (minr > v ? minr : v);
              v = histogram.array[indx+i]->getGreen();
              ming = (ming < v ? ming : v);
              maxg = (ming > v ? ming : v);
              v = histogram.array[indx+i]->getBlue();
              minb = (minb < v ? minb : v);
              maxb = (minb > v ? minb : v);
          }
          array[bi]->assign(( minr + maxr ) / 2, ( ming + maxg ) / 2, ( minb + maxb ) / 2);
      }
  }
  else if (repType == DcmRepresentativeColorType_default)
  {
      for ( bi = 0; bi < boxes; ++bi )
      {
          register int indx = bv[bi].ind;
          register int clrs = bv[bi].colors;
          register long r = 0, g = 0, b = 0;
          
          for ( i = 0; i < clrs; ++i )
          {
              r += histogram.array[indx+i]->getRed();
              g += histogram.array[indx+i]->getGreen();
              b += histogram.array[indx+i]->getBlue();
          }
          r = r / clrs;
          g = g / clrs;
          b = b / clrs;
          array[bi]->assign((DcmQuantComponent)r, (DcmQuantComponent)g, (DcmQuantComponent)b);
      }
  }
  else // DcmRepresentativeColorType_averagePixels
  {
      for ( bi = 0; bi < boxes; ++bi )
      {
          register int indx = bv[bi].ind;
          register int clrs = bv[bi].colors;
          register unsigned long r = 0, g = 0, b = 0, sum = 0;
          
          for ( i = 0; i < clrs; ++i )
          {
              r += histogram.array[indx+i]->getRed()   * histogram.array[indx+i]->getValue();
              g += histogram.array[indx+i]->getGreen() * histogram.array[indx+i]->getValue();
              b += histogram.array[indx+i]->getBlue()  * histogram.array[indx+i]->getValue();
              sum += histogram.array[indx+i]->getValue();
          }
          r = r / sum;
          if ( r > maxval ) r = maxval;   /* avoid math errors */
          g = g / sum;
          if ( g > maxval ) g = maxval;
          b = b / sum;
          if ( b > maxval ) b = maxval;
          array[bi]->assign((DcmQuantComponent)r, (DcmQuantComponent)g, (DcmQuantComponent)b);
      }
  }
  
  // All done, now compute clusters
  computeClusters();
  return EC_Normal;
}


void DcmQuantColorTable::computeClusters()
{
  unsigned long i;
  unsigned long j;
  unsigned long k=0;
  int cluster;
  int newdist;
  int r1, g1, b1;
  int r2, g2, b2;

  // initialize clusters
  for (i = 0; i < numColors; ++i) array[i]->setValue(2000000000);

  for (i = 0; i < numColors-1; ++i)
  {
    cluster = array[i]->getValue();
    r1 = (int) array[i]->getRed();
    g1 = (int) array[i]->getGreen();
    b1 = (int) array[i]->getBlue();

    for (j = i+1; j < numColors; ++j)
    {
      // compute euclidean distance between i and j
      r2 = r1 - (int) array[j]->getRed();
      g2 = g1 - (int) array[j]->getGreen();
      b2 = b1 - (int) array[j]->getBlue();
      newdist = (r2*r2 + g2*g2 + b2*b2)/2;
      if (newdist < cluster)
      {
        cluster = newdist;
        k = j;
      }
    }
    array[i]->setValue(cluster);
    array[k]->setValue(cluster);
  }    
}


OFCondition DcmQuantColorTable::write(
  DcmItem& target, 
  OFBool writeAsOW,
  OFBool write16BitEntries)
{
  if (numColors == 0) return EC_IllegalCall;

  // if we're using 16 bit per sample anyway, force 16 bit palette entries
  if (sizeof(DcmQuantComponent) > 1) write16BitEntries = OFTrue;

  OFCondition result = EC_Normal;
  if (array)
  {
    // create palette color LUT descriptor
  	Uint16 descriptor[3];
    descriptor[0] = (numColors > 65535) ? 0 : (Uint16) numColors; // number of entries
    descriptor[1] = 0; // first pixel value mapped
    descriptor[2] = write16BitEntries ? 16 : 8; // bits per entry, must be 8 or 16.

    // if we're writing a 16-bit LUT with 64K entries, we must write as OW because
    // otherwise the US length field will overflow when writing in explicit VR!
    if ((descriptor[0] == 0) && write16BitEntries) writeAsOW = OFTrue;

    DcmElement *elem;
    if (result.good())
    {      
      elem = new DcmUnsignedShort(DCM_RedPaletteColorLookupTableDescriptor);
      if (elem)
      {
        result = elem->putUint16Array(descriptor, 3);
        if (result.good()) result = target.insert(elem, OFTrue);
        if (result.bad()) delete elem;
      } else result = EC_MemoryExhausted;
    }

    if (result.good())
    {      
      elem = new DcmUnsignedShort(DCM_GreenPaletteColorLookupTableDescriptor);
      if (elem)
      {
        result = elem->putUint16Array(descriptor, 3);
        if (result.good()) result = target.insert(elem, OFTrue);
        if (result.bad()) delete elem;
      } else result = EC_MemoryExhausted;
    }

    if (result.good())
    {      
      elem = new DcmUnsignedShort(DCM_BluePaletteColorLookupTableDescriptor);
      if (elem)
      {
        result = elem->putUint16Array(descriptor, 3);
        if (result.good()) result = target.insert(elem, OFTrue);
        if (result.bad()) delete elem;
      } else result = EC_MemoryExhausted;
    }

    // now create the LUT content
    if (result.good())
    {
      Uint16* rLUT = NULL;
      Uint16* gLUT = NULL;
      Uint16* bLUT = NULL;
      unsigned long numWords = 0;
      double factor = 1.0;
      if (write16BitEntries)
      {
      	numWords = numColors;
        rLUT = new Uint16[numWords];
        gLUT = new Uint16[numWords];
        bLUT = new Uint16[numWords];
        factor = 65535.0 / maxval;
        if (rLUT && gLUT && bLUT)
        {
          for (unsigned long i=0; i<numColors; i++)
          {
            rLUT[i] = (Uint16) ((double) array[i]->getRed() * factor);
            gLUT[i] = (Uint16) ((double) array[i]->getGreen() * factor);
            bLUT[i] = (Uint16) ((double) array[i]->getBlue() * factor);

            // if the source data is only 8 bits per entry, replicate high and low bytes
            if (sizeof(DcmQuantComponent) == 1)
            {
              rLUT[i] |= rLUT[i] << 8;
              gLUT[i] |= gLUT[i] << 8;
              bLUT[i] |= bLUT[i] << 8;
            }
          }
        } else result = EC_MemoryExhausted;
      }
      else
      {
        // number of Uint16 words needed to store numColors Uint8 values plus padding
      	numWords = (numColors+1)/2;
        rLUT = new Uint16[numWords];
        gLUT = new Uint16[numWords];
        bLUT = new Uint16[numWords];
        rLUT[numWords-1] = 0;
        gLUT[numWords-1] = 0;
        bLUT[numWords-1] = 0;
        factor = 255.0 / maxval;
        if (rLUT && gLUT && bLUT)
        {
          Uint8 *rLUT8 = (Uint8 *)rLUT;
          Uint8 *gLUT8 = (Uint8 *)gLUT;
          Uint8 *bLUT8 = (Uint8 *)bLUT;
          for (unsigned long i=0; i<numColors; i++)
          {
            rLUT8[i] = (Uint8) ((double) array[i]->getRed() * factor);
            gLUT8[i] = (Uint8) ((double) array[i]->getGreen() * factor);
            bLUT8[i] = (Uint8) ((double) array[i]->getBlue() * factor);
          }
          // we have written the byte array in little endian order, now swap to US/OW if neccessary
          swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, rLUT, numWords*sizeof(Uint16), sizeof(Uint16));
          swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, gLUT, numWords*sizeof(Uint16), sizeof(Uint16));
          swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, bLUT, numWords*sizeof(Uint16), sizeof(Uint16));
        } else result = EC_MemoryExhausted;
      }

      // the LUT data is prepared, now create the corresponding DICOM elements
      if (result.good())
      {     
      	if (writeAsOW) elem = new DcmOtherByteOtherWord(DcmTag(DCM_RedPaletteColorLookupTableData, EVR_OW));
        else elem = new DcmUnsignedShort(DcmTag(DCM_RedPaletteColorLookupTableData, EVR_US));
        if (elem)
        {
          result = elem->putUint16Array(rLUT, numWords);
          if (result.good()) result = target.insert(elem, OFTrue);
          if (result.bad()) delete elem;
        } else result = EC_MemoryExhausted;
      }

      if (result.good())
      {      
      	if (writeAsOW) elem = new DcmOtherByteOtherWord(DcmTag(DCM_GreenPaletteColorLookupTableData, EVR_OW));
        else elem = new DcmUnsignedShort(DcmTag(DCM_GreenPaletteColorLookupTableData, EVR_US));
        if (elem)
        {
          result = elem->putUint16Array(gLUT, numWords);
          if (result.good()) result = target.insert(elem, OFTrue);
          if (result.bad()) delete elem;
        } else result = EC_MemoryExhausted;
      }

      if (result.good())
      {      
      	if (writeAsOW) elem = new DcmOtherByteOtherWord(DcmTag(DCM_BluePaletteColorLookupTableData, EVR_OW));
        else elem = new DcmUnsignedShort(DcmTag(DCM_BluePaletteColorLookupTableData, EVR_US));
        if (elem)
        {
          result = elem->putUint16Array(bLUT, numWords);
          if (result.good()) result = target.insert(elem, OFTrue);
          if (result.bad()) delete elem;
        } else result = EC_MemoryExhausted;
      }
      delete[] rLUT;
      delete[] gLUT;
      delete[] bLUT;
    }
  } else result = EC_IllegalCall;

  return result;
}

void DcmQuantColorTable::setDescriptionString(OFString& str) const
{
  char buf[100];

  sprintf(buf, "Converted to PALETTE COLOR %lu/0/%u with DCMTK %s", 
    (numColors > 65535) ? 0 : numColors, (sizeof(DcmQuantComponent) == 1) ? 8 : 16,
    OFFIS_DCMTK_VERSION);

  str = buf;
}


/*
 *
 * CVS/RCS Log:
 * $Log: diqtctab.cc,v $
 * Revision 1.4  2002-11-27 14:16:58  meichel
 * Adapted module dcmimage to use of new header file ofstdinc.h
 *
 * Revision 1.3  2002/08/20 12:20:24  meichel
 * Adapted code to new loadFile and saveFile methods, thus removing direct
 *   use of the DICOM stream classes.
 *
 * Revision 1.2  2002/05/15 09:53:33  meichel
 * Minor corrections to avoid warnings on Sun CC 2.0.1
 *
 * Revision 1.1  2002/01/25 13:32:10  meichel
 * Initial release of new color quantization classes and
 *   the dcmquant tool in module dcmimage.
 *
 *
 */
