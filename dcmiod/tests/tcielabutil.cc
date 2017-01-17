/*
 *
 *  Copyright (C) 2017, OFFIS e.V.
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
 *  Module:  dcmiod
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Tests for dcmiod's color conversion functionalities
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/oftest.h"
#include "dcmtk/dcmiod/cielabutil.h"
#define INCLUDE_CSTDLIB               // for srand()
#define INCLUDE_CTIME                 // for time() initialization of srand()
#include "dcmtk/ofstd/ofstdinc.h"

OFTEST(dcmiod_tcielabutil)
{
  // Result variables for testing
  double r1, r2, r3;
  r1=r2=r3 = 0.0;

  // Test RGB -> CIELab conversion with medium values
  IODCIELabUtil::rgb2Lab(r1, r2, r3 /* L,a,b */, 0.5, 0.5, 0.5);
  OFCHECK( fabs(53.388 - r1) < 0.001 );
  OFCHECK( fabs(0.006 - r2) < 0.001 );
  OFCHECK( fabs(-0.010 - r3) < 0.001 );

  // Test RGB -> CIEXYZ conversion with medium values
  r1=r2=r3 = 0;
  IODCIELabUtil::rgb2Xyz(r1, r2, r3 /* X,Y,Z */, 0.5, 0.5, 0.5);
  OFCHECK( fabs(0.203 - r1) < 0.001 );
  OFCHECK( fabs(0.214 - r2) < 0.001 );
  OFCHECK( fabs(0.233 - r3) < 0.001 );

  // Test RGB -> CIELab conversion with minimum value
  r1=r2=r3 = 0;
  IODCIELabUtil::rgb2Lab(r1, r2, r3 /* L,a,b */, 0, 0, 0);
  OFCHECK( fabs(0 - r1) < 0.001 );
  OFCHECK( fabs(0 - r2) < 0.001 );
  OFCHECK( fabs(0 - r3) < 0.001 );

  // Test RGB -> XYZ conversion with minimum values
  r1=r2=r3 = 0;
  IODCIELabUtil::rgb2Xyz(r1, r2, r3 /* X,Y,Z */, 0, 0, 0);
  OFCHECK( fabs(0 - r1) < 0.001 );
  OFCHECK( fabs(0 - r2) < 0.001 );
  OFCHECK( fabs(0 - r3) < 0.001 );

  // Test DICOM CIELab to "normalized" CIELab conversion with maximum values
  r1=r2=r3 = 0;
  IODCIELabUtil::dicomlab2Lab(r1, r2, r3, 65535, 65535, 65535 );
  OFCHECK( fabs(100 - r1) < 0.001 );
  OFCHECK( fabs(127 - r2) < 0.001 );
  OFCHECK( fabs(127 - r3) < 0.001 );

  // Test DICOM CIELab to "normalized" CIELab conversion with minimum values
  r1=r2=r3 = 0;
  IODCIELabUtil::dicomlab2Lab(r1, r2, r3, 0, 0, 0 );
  OFCHECK( fabs(0 - r1) < 0.001 );
  OFCHECK( fabs(-128 - r2) < 0.001 );
  OFCHECK( fabs(-128 - r3) < 0.001 );

  // Convert between colorspaces back and forth and check whether deviation after
  // roundtrip is less than around 1 promille

  // Initialize random numbers
  srand (OFstatic_cast(unsigned int, time (NULL)));

  // We do 1000 runs
  for (size_t numRun = 0; numRun< 1000; numRun++)
  {
    // i1, i2, i3: Input for conversion
    // r1, r2, r3: Results of conversion
    // o1, o2, o3: Output of inverse conversion (should equal i1, i2, i3)
    double i1, i2, i3, o1, o2, o3;
    i1 = i2 = i3 = r1 = r2 = r3 = o1 = o2 = o3 = 0.0;

    // Roundtrip RGB -> CIELab -> RGB
    i1 = (double)rand() / (double)RAND_MAX;
    i2 = (double)rand() / (double)RAND_MAX;
    i3 = (double)rand() / (double)RAND_MAX;
    IODCIELabUtil::rgb2Lab(r1, r2, r3, i1, i2, i3);
    IODCIELabUtil::lab2Rgb(o1, o2, o3, r1, r2, r3);
    OFCHECK( fabs(i1 - o1) < 0.001 );
    OFCHECK( fabs(i2 - o2) < 0.001 );
    OFCHECK( fabs(i3 - o3) < 0.001 );

    // Roundtrip CIELab -> DICOM CIELab -> CIELab
    i1 = i2 = i3 = r1 = r2 = r3 = o1 = o2 = o3 = 0.0;
    i1 = ((double)rand() / ((double)RAND_MAX)) * 100.0;
    i2 = ((double)rand() / ((double)RAND_MAX)) * 255.0 - 128;
    i3 = ((double)rand() / ((double)RAND_MAX)) * 255.0 - 128;
    IODCIELabUtil::lab2DicomLab(r1, r2, r3, i1, i2, i3);
    IODCIELabUtil::dicomlab2Lab(o1, o2, o3, r1, r2, r3);
    OFCHECK( fabs(i1 - o1) < 0.001 );
    OFCHECK( fabs(i2 - o2) < 0.001 );
    OFCHECK( fabs(i3 - o3) < 0.001 );

    // Roundtrip RGB -> CIEXYZ -> CIELab -> dicomCIELab -> RGB
    i1 = i2 = i3 = r1 = r2 = r3 = o1 = o2 = o3 = 0.0;
    i1 = ((double)rand() / ((double)RAND_MAX));
    i2 = ((double)rand() / ((double)RAND_MAX));
    i3 = ((double)rand() / ((double)RAND_MAX));
    IODCIELabUtil::rgb2Xyz(r1, r2, r3, i1, i2, i3);
    IODCIELabUtil::xyz2Lab(o1, o2, o3, r1, r2, r3);
    IODCIELabUtil::lab2DicomLab(r1, r2, r3, o1, o2, o3);
    IODCIELabUtil::dicomLab2RGB(o1, o2, o3, r1, r2, r3);

    OFCHECK( fabs(i1 - o1) < 0.001 );
    OFCHECK( fabs(i2 - o2) < 0.001 );
    OFCHECK( fabs(i3 - o3) < 0.001 );

  }
}
