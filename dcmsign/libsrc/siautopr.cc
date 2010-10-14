/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *  Module: dcmsign
 *
 *  Author: Norbert Loxen, Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiAuthorizationProfile
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:38 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/siautopr.h"
#include "dcmtk/dcmdata/dcdeftag.h"

OFBool SiAuthorizationProfile::attributeRequired(const DcmTagKey& key) const
{
  /* SOP Class and Instance UIDs */
  if (key == DCM_SOPClassUID) return OFTrue;
  if (key == DCM_SOPInstanceUID) return OFTrue;

  /* Study and Series Instance UIDs */
  if (key == DCM_StudyInstanceUID) return OFTrue;
  if (key == DCM_SeriesInstanceUID) return OFTrue;
  
  /* Any overlay data present */
  if ((key.getGroup() >= 0x6000) && (key.getGroup() < 0x6020) && ((key.getGroup() & 0x0001) == 0)) return OFTrue;
  
  /* Any image data present - we assume this means the Image Pixel Module and not just PixelData */
  if (key.getGroup() == 0x0028) 
  {
    Uint16 elem = key.getElement();
    if (elem == 0x0002) return OFTrue; // DCM_SamplesPerPixel
    if (elem == 0x0004) return OFTrue; // DCM_PhotometricInterpretation
    if (elem == 0x0006) return OFTrue; // DCM_PlanarConfiguration                   
    if (elem == 0x0010) return OFTrue; // DCM_Rows
    if (elem == 0x0011) return OFTrue; // DCM_Columns
    if (elem == 0x0034) return OFTrue; // DCM_PixelAspectRatio                      
    if (elem == 0x0100) return OFTrue; // DCM_BitsAllocated
    if (elem == 0x0101) return OFTrue; // DCM_BitsStored                            
    if (elem == 0x0102) return OFTrue; // DCM_HighBit                               
    if (elem == 0x0103) return OFTrue; // DCM_PixelRepresentation                   
    if (elem == 0x0106) return OFTrue; // DCM_SmallestImagePixelValue               
    if (elem == 0x0107) return OFTrue; // DCM_LargestImagePixelValue                
    if (elem == 0x1101) return OFTrue; // DCM_RedPaletteColorLookupTableDescriptor  
    if (elem == 0x1102) return OFTrue; // DCM_GreenPaletteColorLookupTableDescriptor
    if (elem == 0x1103) return OFTrue; // DCM_BluePaletteColorLookupTableDescriptor 
    if (elem == 0x1201) return OFTrue; // DCM_RedPaletteColorLookupTableData        
    if (elem == 0x1202) return OFTrue; // DCM_GreenPaletteColorLookupTableData      
    if (elem == 0x1203) return OFTrue; // DCM_BluePaletteColorLookupTableData       
  }
  if (key == DCM_PixelData) return OFTrue;

  /* Any attributes whose values are verifiable by the technician or physician 
   * (e.g., their values are displayed to the technician or physician)
   * This is obviously application dependent. We don't handle that here.
   */  
  return OFFalse;    
}

#else /* WITH_OPENSSL */

int siautopr_cc_dummy_to_keep_linker_from_moaning = 0;

#endif

/*
 *  $Log: siautopr.cc,v $
 *  Revision 1.7  2010-10-14 13:14:38  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.6  2005-12-08 15:47:18  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.5  2002/12/16 12:57:49  meichel
 *  Minor modification to shut up linker on MacOS X when compiling
 *    without OpenSSL support
 *
 *  Revision 1.4  2001/11/16 15:50:52  meichel
 *  Adapted digital signature code to final text of supplement 41.
 *
 *  Revision 1.3  2001/06/01 15:50:52  meichel
 *  Updated copyright header
 *
 *  Revision 1.2  2000/11/07 18:07:08  joergr
 *  Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 *  Revision 1.1  2000/11/07 16:49:03  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

