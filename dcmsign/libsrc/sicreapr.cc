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
 *    classes: SiCreatorProfile
 *
 */

#include "dcmtk/config/osconfig.h"

#ifdef WITH_OPENSSL

#include "dcmtk/dcmsign/sicreapr.h"
#include "dcmtk/dcmdata/dcdeftag.h"


OFBool SiCreatorProfile::attributeRequired(const DcmTagKey& key) const
{
  /* SOP Class and Instance UIDs */
  if (key == DCM_SOPClassUID) return OFTrue;
  if (key == DCM_SOPInstanceUID) return OFTrue;

  /* SOP Creation Date and Time, if present */
  if (key == DCM_InstanceCreationDate) return OFTrue;
  if (key == DCM_InstanceCreationTime) return OFTrue;

  /* Study and Series Instance UIDs */
  if (key == DCM_StudyInstanceUID) return OFTrue;
  if (key == DCM_SeriesInstanceUID) return OFTrue;

  /* Any attributes of the General Equipment module that are present */

  if (key == DCM_Manufacturer) return OFTrue;
  if (key == DCM_InstitutionName) return OFTrue;
  if (key == DCM_InstitutionAddress) return OFTrue;
  if (key == DCM_StationName) return OFTrue;
  if (key == DCM_InstitutionalDepartmentName) return OFTrue;
  if (key == DCM_ManufacturerModelName) return OFTrue;
  if (key == DCM_DeviceSerialNumber) return OFTrue;
  if (key == DCM_SoftwareVersions) return OFTrue;
  if (key == DCM_SpatialResolution) return OFTrue;
  if (key == DCM_DateOfLastCalibration) return OFTrue;
  if (key == DCM_TimeOfLastCalibration) return OFTrue;
  if (key == DCM_PixelPaddingValue) return OFTrue;

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

  return OFFalse;
}

#else /* WITH_OPENSSL */

int sicreapr_cc_dummy_to_keep_linker_from_moaning = 0;

#endif
