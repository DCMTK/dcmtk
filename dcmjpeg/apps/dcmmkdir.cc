/*
 *
 *  Copyright (C) 2001-2010, OFFIS e.V.
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
 *  Module:  dcmjpeg
 *
 *  Authors: Joerg Riesmeier
 *
 *  Make a DICOMDIR according to the DICOM Part 11 Media Storage Application
 *  Profiles. Currently supports the following profiles:
 *  - General Purpose CD-R Interchange (STD-GEN-CD)
 *  - General Purpose Interchange on DVD-RAM Media (STD-GEN-DVD-RAM)
 *  - General Purpose DVD with Compression Interchange (STD-GEN-DVD-JPEG/J2K)
 *  - General Purpose USB and Flash Memory with Compression Interchange (STD-GEN-USB/MMC/CF/SD-JPEG/J2K)
 *  - General Purpose MIME Interchange (STD-GEN-MIME)
 *  - DVD Interchange with MPEG2 MP@ML (STD-DVD-MPEG2-MPML)
 *  - Basic Cardiac X-Ray Angiographic Studies on CD-R Media (STD-XABC-CD)
 *  - 1024 X-Ray Angiographic Studies on CD-R Media (STD-XA1K-CD)
 *  - 1024 X-Ray Angiographic Studies on DVD Media (STD-XA1K-DVD)
 *  - Dental Radiograph Interchange (STD-DEN-CD)
 *  - CT/MR Studies on various Media (STD-CTMR-xxxx)
 *  - Ultrasound Single Frame for Image Display (STD-US-ID-SF-xxxx)
 *  - Ultrasound Single Frame with Spatial Calibration (STD-US-SC-SF-xxxx)
 *  - Ultrasound Single Frame with Combined Calibration (STD-US-CC-SF-xxxx)
 *  - Ultrasound Single & Multi-Frame for Image Display (STD-US-ID-MF-xxxx)
 *  - Ultrasound Single & Multi-Frame with Spatial Calibration (STD-US-SC-MF-xxxx)
 *  - Ultrasound Single & Multi-Frame with Combined Calibration (STD-US-CC-MF-xxxx)
 *  - 12-lead ECG Interchange on Diskette (STD-WVFM-ECG-FD)
 *  - Hemodynamic Waveform Interchange on Diskette (STD-WVFM-HD-FD)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:13:38 $
 *  CVS/RCS Revision: $Revision: 1.5 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


// compile "dcmgpdir" with dcmjpeg support
#define BUILD_DCMGPDIR_AS_DCMMKDIR

// include full implementation of "dcmgpdir"
#include "../../dcmdata/apps/dcmgpdir.cc"


/*
 * CVS/RCS Log:
 * $Log: dcmmkdir.cc,v $
 * Revision 1.5  2010-10-14 13:13:38  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.4  2008-09-25 13:53:24  joergr
 * Updated comment header (list of supported profiles).
 *
 * Revision 1.3  2005/12/08 15:43:23  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.2  2004/01/16 14:28:01  joergr
 * Updated copyright header.
 *
 * Revision 1.1  2001/11/19 13:08:10  joergr
 * Moved dcmmkdir tool to dcmjpeg module.
 *
 *
 */
