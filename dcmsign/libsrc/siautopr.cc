/*
 *
 *  Copyright (C) 1998-2000, OFFIS
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
 *  Module: dcmsign
 *
 *  Author: Norbert Loxen, Marco Eichelberg
 *
 *  Purpose:
 *    classes: SiAuthorizationProfile
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2000-11-07 18:07:08 $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "osconfig.h"

#ifdef WITH_OPENSSL

#include "siautopr.h"
#include "dcdeftag.h"

OFBool SiAuthorizationProfile::isAllowableMACType(E_MACType macType) const
{
  OFBool result = OFFalse;
  switch (macType)
  {
    case EMT_RIPEMD160:
    /* case EMT_SHA1: */ // will be allowed later
      result = OFTrue;
      break;
    default:
      /* nothing */
      break;
  }
  return result;
}

OFBool SiAuthorizationProfile::isAllowableAlgorithmType(E_KeyType keyType) const
{
  OFBool result = OFFalse;
  switch (keyType)
  {
    case EKT_RSA:
      result = OFTrue;
      break;
    default:
      /* nothing */
      break;
  }
  return result;
}

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

OFBool SiAuthorizationProfile::attributeForbidden(const DcmTagKey& /* key */) const
{
  // we have no special restrictions for this profile
  return OFFalse;
}

OFBool SiAuthorizationProfile::isAllowableTransferSyntax(E_TransferSyntax xfer) const
{
  if (xfer == EXS_LittleEndianExplicit) return OFTrue;
  return OFFalse;
}

#else /* WITH_OPENSSL */

const int siautopr_cc_dummy_to_keep_linker_from_moaning = 0;

#endif

/*
 *  $Log: siautopr.cc,v $
 *  Revision 1.2  2000-11-07 18:07:08  joergr
 *  Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 *  Revision 1.1  2000/11/07 16:49:03  meichel
 *  Initial release of dcmsign module for DICOM Digital Signatures
 *
 *
 */

