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
 */


// compile "dcmgpdir" with dcmjpeg support
#define BUILD_DCMGPDIR_AS_DCMMKDIR

// include full implementation of "dcmgpdir"
#include "../../dcmdata/apps/dcmgpdir.cc"
