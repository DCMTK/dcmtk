/*
 *
 *  Copyright (C) 2001-2004, OFFIS
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
 *  Module:  dcmjpeg
 *
 *  Authors: Joerg Riesmeier
 *
 *  Make a DICOMDIR according to the DICOM Part 11 Media Storage Application
 *  Profiles. Currently supports the following profiles:
 *  - General Purpose CD-R Interchange (STD-GEN-CD)
 *  - General Purpose Interchange on DVD-RAM Media (STD-GEN-DVD-RAM)
 *  - No profile (NONE), like STD-GEN-xxxx without restricting the transfer syntax
 *  - Basic Cardiac X-Ray Angiographic Studies on CD-R Media (STD-XABC-CD)
 *  - 1024 X-Ray Angiographic Studies on CD-R Media (STD-XA1K-CD)
 *  - CT/MR Studies on xxxx Media (STD-CTMR-xxxx)
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
 *  Update Date:      $Date: 2004-01-16 14:28:01 $
 *  CVS/RCS Revision: $Revision: 1.2 $
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
 * Revision 1.2  2004-01-16 14:28:01  joergr
 * Updated copyright header.
 *
 * Revision 1.1  2001/11/19 13:08:10  joergr
 * Moved dcmmkdir tool to dcmjpeg module.
 *
 *
 */
