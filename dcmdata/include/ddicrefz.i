/*
 *
 * FileName: ddicrefz.i
 *
 * Module: dcmcode
 *
 * Purpose:
 * This header file contains various table references (long form)
 *
 * Author: Peter Jensch / Andrew Hewett
 *
 * Last Update:    $Author: hewett $
 * Revision:    $Revision: 1.1 $
 * Status:    $State: Exp $
 *
 * $Id: ddicrefz.i,v 1.1 1995-11-02 18:00:02 hewett Exp $
 *
 * $Log: ddicrefz.i,v $
 * Revision 1.1  1995-11-02 18:00:02  hewett
 * Initial revision
 *
 *
 * Version:
 * 1.1  1993/08/10  13:46:47  Jensch/Hewett
 *                  Initial release
 */
/*
************************************************************************
**  Copyright (C) 1993/1994, OFFIS, Oldenburg University and CERIUM
**
**  This software and supporting documentation were
**  developed by 
**  
**    Institut OFFIS
**    Bereich Kommunikationssysteme
**    Westerstr. 10-12
**    26121 Oldenburg, Germany
**    
**    Fachbereich Informatik
**    Abteilung Prozessinformatik
**    Carl von Ozzietzky Universitaet Oldenburg 
**    Ammerlaender Heerstr. 114-118
**    26111 Oldenburg, Germany
**    
**    CERIUM
**    Laboratoire SIM
**    Faculte de Medecine
**    2 Avenue du Pr. Leon Bernard
**    35043 Rennes Cedex, France
**  
**  for CEN/TC251/WG4 as a contribution to the Radiological 
**  Society of North America (RSNA) 1993 Digital Imaging and 
**  Communications in Medicine (DICOM) Demonstration.
**  
**  THIS SOFTWARE IS MADE AVAILABLE, AS IS, AND NEITHER OFFIS,
**  OLDENBURG UNIVERSITY NOR CERIUM MAKE ANY WARRANTY REGARDING 
**  THE SOFTWARE, ITS PERFORMANCE, ITS MERCHANTABILITY OR 
**  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER 
**  DISEASES OR ITS CONFORMITY TO ANY SPECIFICATION.  THE 
**  ENTIRE RISK AS TO QUALITY AND PERFORMANCE OF THE SOFTWARE   
**  IS WITH THE USER. 
**  
**  Copyright of the software and supporting documentation
**  is, unless otherwise stated, jointly owned by OFFIS,
**  Oldenburg University and CERIUM and free access is hereby
**  granted as a license to use this software, copy this
**  software and prepare derivative works based upon this
**  software. However, any distribution of this software
**  source code or supporting documentation or derivative
**  works (source code and supporting documentation) must
**  include the three paragraphs of this copyright notice. 
** 
*/
/*
Structure of List descriptions (ModAttrT_Desc_...):

E_ListIsTagMember                     No List 0   0    0   NULL
E_ListIsTagTypeMember                 No List 0   0    0   NULL
E_ListIsTagMemberWithCond             No List 0   0    NoC ListC
E_ListIsTagTypeMemberWithCond         No List 0   0    NoC ListC
E_ListIsValxMember                    No List Tag Type 0   NULL
E_ListIsValxMemberWithAddVal          No List Tag Type 0   NULL
E_ListIsValxMemberAndCond             No List Tag Type 0   ListDesc
E_ListIsValxMemberWithAddValAndCond   No List Tag Type 0   ListDesc
*/
/* ModAttrTable references */
S_TagType ModAttrT_SOPUIDRefs[] =   /* Standard SOP Class UID/SOP Instance UID */
  {
  /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_1C},
  /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_1C}
  };

/* ModAttrTable references */
S_Tag ModAttrT_CodeRefs[] =  /* Code... commons */
  {
  /* (0008,0100) */  ET_CodeValue,
  /* (0008,0102) */  ET_CodingSchemeDesignator,
  /* (0008,0104) */  ET_CodeMeaning
  };

/* ModAttrTable references */
S_TagType ModAttrT_03C07011[] =   /* Patient Module Attributes */
  {
   /* (0010,0010) */ {ET_PatientsName,EType_2},
   /* (0010,0020) */ {ET_PatientID,EType_2},
   /* (0010,0030) */ {ET_PatientsBirthDate,EType_2},
   /* (0010,0032) */ {ET_PatientsBirthTime,EType_3},
   /* (0010,0040) */ {ET_PatientsSex,EType_2},
   /* (0010,1000) */ {ET_OtherPatientIDs,EType_3},
   /* (0010,1001) */ {ET_OtherPatientNames,EType_3},
   /* (0010,2160) */ {ET_EthnicGroup,EType_3},
   /* (0010,4000) */ {ET_PatientComments,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C07011 = {E_ListIsTagTypeMember,9,ModAttrT_03C07011, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C07011SQReferencedPatientSequence[] =  /* Patient Module Attributes */
  {
  /* (0008,1120) */ {ET_ReferencedPatientSequence,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C07011SQReferencedPatientSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_SOPUIDRefs, 0, 0,1,ModAttrT_03C07011SQReferencedPatientSequence};

/* ModAttrTable references */
S_List2Str ModAttrT_03C07011V1PatientsSex[] =   /* Patient Module Attributes */
  {
   {"F","female"},
   {"M","male"},
   {"O","other"}
  };
S_ListDesc1 ModAttrT_Desc_03C07011V1PatientsSex = {E_ListIsVal1MemberWithAddVal,3,ModAttrT_03C07011V1PatientsSex,ET_PatientsSex,EType_2,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C07021[] =   /* General Study Module Attributes */
  {
   /* (0008,0020) */ {ET_StudyDate,EType_2},
   /* (0008,0030) */ {ET_StudyTime,EType_2},
   /* (0008,0050) */ {ET_AccessionNumber,EType_2},
   /* (0008,0090) */ {ET_ReferringPhysiciansName,EType_2},
   /* (0008,1030) */ {ET_StudyDescription,EType_3},
   /* (0008,1060) */ {ET_NameofPhysiciansReadingStudy,EType_3},
   /* (0020,000D) */ {ET_StudyInstanceUID,EType_1},
   /* (0020,0010) */ {ET_StudyID,EType_2}
  };
S_ListDesc1 ModAttrT_Desc_03C07021 = {E_ListIsTagTypeMember,8,ModAttrT_03C07021, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C07021SQReferencedStudySequence[] =  /* General Study Module Attributes */
  {
  /* (0008,1110) */ {ET_ReferencedStudySequence,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C07021SQReferencedStudySequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_SOPUIDRefs, 0, 0,1,ModAttrT_03C07021SQReferencedStudySequence};

/* ModAttrTable references */
S_TagType ModAttrT_03C07022[] =   /* Patient Study Module Attributes */
  {
   /* (0008,1080) */ {ET_AdmittingDiagnosesDescription,EType_3},
   /* (0010,1010) */ {ET_PatientsAge,EType_3},
   /* (0010,1020) */ {ET_PatientsSize,EType_3},
   /* (0010,1030) */ {ET_PatientsWeight,EType_3},
   /* (0010,2180) */ {ET_Occupation,EType_3},
   /* (0010,21B0) */ {ET_AdditionalPatientHistory,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C07022 = {E_ListIsTagTypeMember,6,ModAttrT_03C07022, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C07031[] =   /* General Series Module Attributes */
  {
   /* (0008,0021) */ {ET_SeriesDate,EType_3},
   /* (0008,0031) */ {ET_SeriesTime,EType_3},
   /* (0008,0060) */ {ET_Modality,EType_1},
   /* (0008,103E) */ {ET_SeriesDescription,EType_3},
   /* (0008,1050) */ {ET_PerformingPhysiciansName,EType_3},
   /* (0008,1070) */ {ET_OperatorsName,EType_3},
   /* (0018,0015) */ {ET_BodyPartExamined,EType_3},
   /* (0018,1030) */ {ET_ProtocolName,EType_3},
   /* (0018,5100) */ {ET_PatientPosition,EType_2C},
   /* (0020,000E) */ {ET_SeriesInstanceUID,EType_1},
   /* (0020,0011) */ {ET_SeriesNumber,EType_2},
   /* (0020,0060) */ {ET_Laterality,EType_2C},
   /* (0028,0108) */ {ET_SmallestPixelValueinSeries,EType_3},
   /* (0028,0109) */ {ET_LargestPixelValueinSeries,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C07031 = {E_ListIsTagTypeMember,14,ModAttrT_03C07031, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C07031SQReferencedStudyComponentSequence[] =  /* General Series Module Attributes */
  {
  /* (0008,1111) */ {ET_ReferencedStudyComponentSequence,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C07031SQReferencedStudyComponentSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_SOPUIDRefs, 0, 0,1,ModAttrT_03C07031SQReferencedStudyComponentSequence};

/* ModAttrTable references */
char *ModAttrT_03C07031V1BodyPartExamined[] =   /* General Series Module Attributes */
  {
   "ABDOMEN",
   "ANKLE",
   "BREAST",
   "CHEST",
   "CLAVICLE",
   "COCCYX",
   "CSPINE",
   "ELBOW",
   "EXTREMITY",
   "FOOT",
   "HAND",
   "HIP",
   "KNEE",
   "LSPINE",
   "PELVIS",
   "SHOULDER",
   "SKULL",
   "SSPINE",
   "TSPINE"
  };
S_ListDesc1 ModAttrT_Desc_03C07031V1BodyPartExamined = {E_ListIsVal1Member,19,ModAttrT_03C07031V1BodyPartExamined,ET_BodyPartExamined,EType_3,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C07031V1PatientPosition[] =   /* General Series Module Attributes */
  {
   {"FFDL","Feet First-Decubitus Left"},
   {"FFDR","Feet First-Decubitus Right"},
   {"FFP","Feet First-Prone"},
   {"FFS","Feet First-Supine"},
   {"HFDL","Head First-Decubitus Left"},
   {"HFDR","Head First-Decubitus Right"},
   {"HFP","Head First-Prone"},
   {"HFS","Head First-Supine"}
  };
S_ListDesc1 ModAttrT_Desc_03C07031V1PatientPosition = {E_ListIsVal1MemberWithAddVal,8,ModAttrT_03C07031V1PatientPosition,ET_PatientPosition,EType_2C,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C07031V1Laterality[] =   /* General Series Module Attributes */
  {
   {"R","right"},
   {"L","left"}
  };
S_ListDesc1 ModAttrT_Desc_03C07031V1Laterality = {E_ListIsVal1MemberWithAddVal,2,ModAttrT_03C07031V1Laterality,ET_Laterality,EType_2C,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C08011[] =   /* CR Series Module Attributes */
  {
   /* (0018,0015) */ {ET_BodyPartExamined,EType_2},
   /* (0018,1160) */ {ET_FilterType,EType_3},
   /* (0018,1180) */ {ET_CollimatorgridName,EType_3},
   /* (0018,1190) */ {ET_FocalSpots,EType_3},
   /* (0018,1260) */ {ET_PlateType,EType_3},
   /* (0018,1261) */ {ET_PhosphorType,EType_3},
   /* (0018,5101) */ {ET_ViewPosition,EType_2}
  };
S_ListDesc1 ModAttrT_Desc_03C08011 = {E_ListIsTagTypeMember,7,ModAttrT_03C08011, 0, 0,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C08011V1BodyPartExamined[] =   /* CR Series Module Attributes */
  {
   "SKULL",
   "CSPINE",
   "TSPINE",
   "LSPINE",
   "SSPINE",
   "COCYX",
   "CHEST",
   "CLAVICLE",
   "BREAST",
   "ABDOMEN",
   "PELVIS",
   "HIP",
   "SHOULDER",
   "ELBOW",
   "KNEE",
   "ANKLE",
   "HAND",
   "FOOT",
   "EXTREMITY"
  };
S_ListDesc1 ModAttrT_Desc_03C08011V1BodyPartExamined = {E_ListIsVal1Member,19,ModAttrT_03C08011V1BodyPartExamined,ET_BodyPartExamined,EType_2,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C08011V1ViewPosition[] =   /* CR Series Module Attributes */
  {
   {"AP","Anterior/Posterior"},
   {"PA","Posterior/Anterior"},
   {"LL","Left Lateral"},
   {"RL","Right Lateral"},
   {"RLD","Right Lateral Decubitus"},
   {"LLD","Left Lateral Decubitus"},
   {"RLO","Right Lateral Oblique"},
   {"LLO","Left Lateral Oblique"}
  };
S_ListDesc1 ModAttrT_Desc_03C08011V1ViewPosition = {E_ListIsVal1MemberWithAddVal,8,ModAttrT_03C08011V1ViewPosition,ET_ViewPosition,EType_2,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C07051[] =   /* General Equipment Module Attributes */
  {
   /* (0008,0070) */ {ET_Manufacturer,EType_2},
   /* (0008,0080) */ {ET_InstitutionName,EType_3},
   /* (0008,0081) */ {ET_InstitutionAddress,EType_3},
   /* (0008,1010) */ {ET_StationName,EType_3},
   /* (0008,1040) */ {ET_InstitutionalDepartmentName,EType_3},
   /* (0008,1090) */ {ET_ManufacturersModelName,EType_3},
   /* (0018,1000) */ {ET_DeviceSerialNumber,EType_3},
   /* (0018,1020) */ {ET_SoftwareVersions,EType_3},
   /* (0018,1050) */ {ET_SpatialResolution,EType_3},
   /* (0018,1200) */ {ET_DateofLastCalibration,EType_3},
   /* (0018,1201) */ {ET_TimeofLastCalibration,EType_3},
   /* (0028,0120) */ {ET_PixelPaddingValue,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C07051 = {E_ListIsTagTypeMember,12,ModAttrT_03C07051, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C07061[] =   /* General Image Module Attributes */
  {
   /* (0008,0008) */ {ET_ImageType,EType_3},
   /* (0008,0022) */ {ET_AcquisitionDate,EType_3},
   /* (0008,0023) */ {ET_ImageDate,EType_2C},
   /* (0008,0032) */ {ET_AcquisitionTime,EType_3},
   /* (0008,0033) */ {ET_ImageTime,EType_2C},
   /* (0008,2111) */ {ET_DerivationDescription,EType_3},
   /* (0020,0012) */ {ET_AcquisitionNumber,EType_3},
   /* (0020,0013) */ {ET_ImageNumber,EType_2},
   /* (0020,0020) */ {ET_PatientOrientation,EType_2C},
   /* (0020,1002) */ {ET_ImagesinAcquisition,EType_3},
   /* (0020,4000) */ {ET_ImageComments,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C07061 = {E_ListIsTagTypeMember,11,ModAttrT_03C07061, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C07061SQReferencedImageSequence[] =  /* General Image Module Attributes */
  {
  /* (0008,1140) */ {ET_ReferencedImageSequence,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C07061SQReferencedImageSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_SOPUIDRefs, 0, 0,1,ModAttrT_03C07061SQReferencedImageSequence};

/* ModAttrTable references */
S_TagType ModAttrT_03C07061SQSourceImageSequence[] =  /* General Image Module Attributes */
  {
  /* (0008,2112) */ {ET_SourceImageSequence,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C07061SQSourceImageSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_SOPUIDRefs, 0, 0,1,ModAttrT_03C07061SQSourceImageSequence};

/* ModAttrTable references */
S_TagType ModAttrT_03C07063[] =   /* Image Pixel Module Attributes */
  {
   /* (0028,0002) */ {ET_SamplesperPixel,EType_1},
   /* (0028,0004) */ {ET_PhotometricInterpretation,EType_1},
   /* (0028,0006) */ {ET_PlanarConfiguration,EType_1C},
   /* (0028,0010) */ {ET_Rows0028,EType_1},
   /* (0028,0011) */ {ET_Columns0028,EType_1},
   /* (0028,0034) */ {ET_PixelAspectRatio,EType_1C},
   /* (0028,0100) */ {ET_BitsAllocated0028,EType_1},
   /* (0028,0101) */ {ET_BitsStored,EType_1},
   /* (0028,0102) */ {ET_HighBit,EType_1},
   /* (0028,0103) */ {ET_PixelRepresentation,EType_1},
   /* (0028,0106) */ {ET_SmallestImagePixelValue,EType_3},
   /* (0028,0107) */ {ET_LargestImagePixelValue,EType_3},
   /* (0028,1101) */ {ET_RedPaletteColorLookupTableDescriptor,EType_1C},
   /* (0028,1102) */ {ET_GreenPaletteColorLookupTableDescriptor,EType_1C},
   /* (0028,1103) */ {ET_BluePaletteColorLookupTableDescriptor,EType_1C},
   /* (0028,1201) */ {ET_RedPaletteColorLookupTableData,EType_1C},
   /* (0028,1202) */ {ET_GreenPaletteColorLookupTableData,EType_1C},
   /* (0028,1203) */ {ET_BluePaletteColorLookupTableData,EType_1C},
   /* (7FE0,0010) */ {ET_PixelData,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_03C07063 = {E_ListIsTagTypeMember,19,ModAttrT_03C07063, 0, 0,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C07063V1PhotometricInterpretation[] =   /* Image Pixel Module Attributes */
  {
   "MONOCHROME1",
   "MONOCHROME2",
   "PALETTE COLOR",
   "RGB",
   "HSV",
   "ARGB",
   "CMYK"
  };
S_ListDesc1 ModAttrT_Desc_03C07063V1PhotometricInterpretation = {E_ListIsVal1Member,7,ModAttrT_03C07063V1PhotometricInterpretation,ET_PhotometricInterpretation,EType_1,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C07063V1PlanarConfiguration[] =   /* Image Pixel Module Attributes */
  {
   {"0000","RGB R1G1B1..  HSV H1S1V1..  ARGB A1R1G1B1..  CMYK C1M1Y1K1.."},
   {"0001","RGB R1..G1..B1..  HSV H1..S1..V1..  ARGB A1..R1..G1..B1..  CMYK C1..M1..Y1..K1.."}
  };
S_ListDesc1 ModAttrT_Desc_03C07063V1PlanarConfiguration = {E_ListIsVal1MemberWithAddVal,2,ModAttrT_03C07063V1PlanarConfiguration,ET_PlanarConfiguration,EType_1C,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C07063V1PixelRepresentation[] =   /* Image Pixel Module Attributes */
  {
   {"0000H","unsigned integer"},
   {"0001H","2's complement"}
  };
S_ListDesc1 ModAttrT_Desc_03C07063V1PixelRepresentation = {E_ListIsVal1MemberWithAddVal,2,ModAttrT_03C07063V1PixelRepresentation,ET_PixelRepresentation,EType_1,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C07065[] =   /* Cine Module Attributes */
  {
   /* (0008,2142) */ {ET_StartTrim,EType_3},
   /* (0008,2143) */ {ET_StopTrim,EType_3},
   /* (0008,2144) */ {ET_RecommendedDisplayFrameRate,EType_3},
   /* (0018,0040) */ {ET_CineRate,EType_3},
   /* (0018,0072) */ {ET_EffectiveSeriesDuration,EType_3},
   /* (0018,1063) */ {ET_FrameTime,EType_1C},
   /* (0018,1065) */ {ET_FrameTimeVector,EType_1C},
   /* (0018,1066) */ {ET_FrameDelay,EType_3},
   /* (0018,1242) */ {ET_ActualFrameDuration,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C07065 = {E_ListIsTagTypeMember,9,ModAttrT_03C07065, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C08012[] =   /* CR Image Module Attributes */
  {
   /* (0018,0060) */ {ET_KVP,EType_3},
   /* (0018,1004) */ {ET_PlateID,EType_3},
   /* (0018,1110) */ {ET_DistanceSourcetoDetector,EType_3},
   /* (0018,1111) */ {ET_DistanceSourcetoPatient,EType_3},
   /* (0018,1150) */ {ET_ExposureTime,EType_3},
   /* (0018,1151) */ {ET_XrayTubeCurrent,EType_3},
   /* (0018,1152) */ {ET_Exposure,EType_3},
   /* (0018,1170) */ {ET_GeneratorPower,EType_3},
   /* (0018,1400) */ {ET_AcquisitionDeviceProcessingDescription,EType_3},
   /* (0018,1401) */ {ET_AcquisitionDeviceProcessingCode,EType_3},
   /* (0018,1402) */ {ET_CassetteOrientation,EType_3},
   /* (0018,1403) */ {ET_CassetteSize,EType_3},
   /* (0018,1404) */ {ET_ExposuresonPlate,EType_3},
   /* (0018,1405) */ {ET_RelativeXrayExposure,EType_3},
   /* (0018,6000) */ {ET_Sensitivity,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C08012 = {E_ListIsTagTypeMember,15,ModAttrT_03C08012, 0, 0,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C08012V1CassetteOrientation[] =   /* CR Image Module Attributes */
  {
   "LANDSCAPE",
   "PORTAIT"
  };
S_ListDesc1 ModAttrT_Desc_03C08012V1CassetteOrientation = {E_ListIsVal1Member,2,ModAttrT_03C08012V1CassetteOrientation,ET_CassetteOrientation,EType_3,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C08012V1CassetteSize[] =   /* CR Image Module Attributes */
  {
   "18CMX24CM",
   "8INX10IN",
   "24CMX30CM",
   "10INX12IN",
   "30CMX35CM",
   "30CMX40CM",
   "11INX14IN",
   "35CMX35CM",
   "35CMX43CM"
  };
S_ListDesc1 ModAttrT_Desc_03C08012V1CassetteSize = {E_ListIsVal1Member,9,ModAttrT_03C08012V1CassetteSize,ET_CassetteSize,EType_3,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C0902[] =   /* Overlay Plane Module Attributes */
  {
   /* (60xx,0010) */ {ET_Rows60xx,EType_1},
   /* (60xx,0011) */ {ET_Columns60xx,EType_1},
   /* (60xx,0040) */ {ET_OverlayType,EType_1},
   /* (60xx,0050) */ {ET_Origin,EType_1},
   /* (60xx,0100) */ {ET_BitsAllocated60xx,EType_1},
   /* (60xx,0102) */ {ET_BitPosition,EType_1},
   /* (60xx,1100) */ {ET_OverlayDescriptorGray,EType_3},
   /* (60xx,1101) */ {ET_OverlayDescriptorRed,EType_3},
   /* (60xx,1102) */ {ET_OverlayDescriptorGreen,EType_3},
   /* (60xx,1103) */ {ET_OverlayDescriptorBlue,EType_3},
   /* (60xx,1200) */ {ET_OverlaysGray,EType_3},
   /* (60xx,1201) */ {ET_OverlaysRed,EType_3},
   /* (60xx,1202) */ {ET_OverlaysGreen,EType_3},
   /* (60xx,1203) */ {ET_OverlaysBlue,EType_3},
   /* (60xx,1301) */ {ET_ROIArea,EType_3},
   /* (60xx,1302) */ {ET_ROIMean,EType_3},
   /* (60xx,1303) */ {ET_ROIStandardDeviation,EType_3},
   /* (60xx,3000) */ {ET_OverlayData,EType_1C}
  };
S_ListDesc1 ModAttrT_Desc_03C0902 = {E_ListIsTagTypeMember,18,ModAttrT_03C0902, 0, 0,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C0902V1OverlayType[] =   /* Overlay Plane Module Attributes */
  {
   {"G","Graphics"},
   {"R","ROL"}
  };
S_ListDesc1 ModAttrT_Desc_03C0902V1OverlayType = {E_ListIsVal1MemberWithAddVal,2,ModAttrT_03C0902V1OverlayType,ET_OverlayType,EType_1,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C1002[] =   /* Curve Module Attributes */
  {
   /* (50xx,0005) */ {ET_CurveDimensions,EType_1},
   /* (50xx,0010) */ {ET_NumberofPoints,EType_1},
   /* (50xx,0020) */ {ET_TypeofData,EType_1},
   /* (50xx,0022) */ {ET_CurveDescription,EType_3},
   /* (50xx,0030) */ {ET_AxisUnits,EType_3},
   /* (50xx,0040) */ {ET_AxisLabels,EType_3},
   /* (50xx,0103) */ {ET_DataValueRepresentation,EType_1},
   /* (50xx,0104) */ {ET_MinimumCoordinateValue,EType_3},
   /* (50xx,0105) */ {ET_MaximumCoordinateValue,EType_3},
   /* (50xx,0106) */ {ET_CurveRange,EType_3},
   /* (50xx,0110) */ {ET_CurveDataDescriptor,EType_1C},
   /* (50xx,0112) */ {ET_CoordinateStartValue,EType_1C},
   /* (50xx,0114) */ {ET_CoordinateStepValue,EType_1C},
   /* (50xx,3000) */ {ET_CurveData,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_03C1002 = {E_ListIsTagTypeMember,14,ModAttrT_03C1002, 0, 0,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C1002V1TypeofData[] =   /* Curve Module Attributes */
  {
   {"TAC","time activity curve"},
   {"PROF","image profile"},
   {"HIST","histogram"},
   {"ROI","polygraphic region of interest"},
   {"TABL","table of values"},
   {"FILT","filter kernel"},
   {"POLY","polyline"},
   {"ECG","ecg data"},
   {"PRESSURE","pressure data"},
   {"FLOW","flow data"},
   {"PHYSIO","physio data"},
   {"RESP","Respiration trace"}
  };
S_ListDesc1 ModAttrT_Desc_03C1002V1TypeofData = {E_ListIsVal1MemberWithAddVal,12,ModAttrT_03C1002V1TypeofData,ET_TypeofData,EType_1,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C1002V1AxisUnits[] =   /* Curve Module Attributes */
  {
   {"SEC","Seconds"},
   {"CNTS","Counts"},
   {"MM","millimeters"},
   {"PIXL","Pixels"},
   {"NONE","Unitless"},
   {"BPM","beats/min"},
   {"CM","centimeters"},
   {"CMS","centimeters/second"},
   {"CM2","cm**2"},
   {"CMS2","cm**2/second"},
   {"CM3","cm**3"},
   {"CM3S","cm**3/sec"},
   {"CMS2","cm/second**2"},
   {"DB","db"},
   {"DBS","db/second"},
   {"DEG","degrees"},
   {"GM","gram"},
   {"GMM2","gram/meter**2"},
   {"HZ","Hertz"},
   {"IN","inch"},
   {"KG","kg"},
   {"LMIN","liters/minute"},
   {"LMINM2","liters/minute/meter**2"},
   {"M2","meters**2"},
   {"MS2","meters/sec**2"},
   {"MLM2","millimeters/meter**2"},
   {"MILS","milliseconds"},
   {"MILV","millivolts"},
   {"MMHG","mmHg"},
   {"PCNT","percent"},
   {"LB","pound"}
  };
S_ListDesc1 ModAttrT_Desc_03C1002V1AxisUnits = {E_ListIsVal1MemberWithAddVal,31,ModAttrT_03C1002V1AxisUnits,ET_AxisUnits,EType_3,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C1002V1DataValueRepresentation[] =   /* Curve Module Attributes */
  {
   {"0000H","unsigned short"},
   {"0001H","signed short"},
   {"0002H","floating point single"},
   {"0003H","floating point double"},
   {"0004H","signed long"}
  };
S_ListDesc1 ModAttrT_Desc_03C1002V1DataValueRepresentation = {E_ListIsVal1MemberWithAddVal,5,ModAttrT_03C1002V1DataValueRepresentation,ET_DataValueRepresentation,EType_1,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C1002V1CurveDataDescriptor[] =   /* Curve Module Attributes */
  {
   {"0000H","Dimension component described using interval spacing"},
   {"0001H","Dimension component described using values"}
  };
S_ListDesc1 ModAttrT_Desc_03C1002V1CurveDataDescriptor = {E_ListIsVal1MemberWithAddVal,2,ModAttrT_03C1002V1CurveDataDescriptor,ET_CurveDataDescriptor,EType_1C,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C1101[] =   /* Modality LUT Module Attributes */
  {
   /* (0028,1052) */ {ET_RescaleIntercept,EType_1C},
   /* (0028,1053) */ {ET_RescaleSlope,EType_1C},
   /* (0028,1054) */ {ET_Rescaletype,EType_1C}
  };
S_ListDesc1 ModAttrT_Desc_03C1101 = {E_ListIsTagTypeMember,3,ModAttrT_03C1101, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C1101SQModalityLUTSequence[] =  /* Modality LUT Module Attributes  */
  {
   /* (0028,3002) */ {ET_LUTDescriptor,EType_1C},
   /* (0028,3003) */ {ET_LUTExplanation,EType_3},
   /* (0028,3004) */ {ET_ModalityLUTType,EType_1C},
   /* (0028,3006) */ {ET_LUTData,EType_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ03C1101SQModalityLUTSequence[] =  /* Modality LUT Module Attributes */
  {
   /* (0028,3000) */ {ET_ModalityLUTSequence,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C1101SQModalityLUTSequence = {E_ListIsTagTypeMemberWithCond,4,ModAttrT_03C1101SQModalityLUTSequence, 0, 0,1,ModAttrT_SQ03C1101SQModalityLUTSequence};

/* ModAttrTable references */
S_TagType ModAttrT_03C1102[] =   /* VOI LUT Module Attributes */
  {
   /* (0028,1050) */ {ET_WindowCenter,EType_3},
   /* (0028,1051) */ {ET_WindowWidth,EType_1C},
   /* (0028,1055) */ {ET_WindowCenter_WidthExplanation,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C1102 = {E_ListIsTagTypeMember,3,ModAttrT_03C1102, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C1102SQVOILUTSequence[] =  /* VOI LUT Module Attributes  */
  {
   /* (0028,3002) */ {ET_LUTDescriptor,EType_1C},
   /* (0028,3003) */ {ET_LUTExplanation,EType_3},
   /* (0028,3006) */ {ET_LUTData,EType_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ03C1102SQVOILUTSequence[] =  /* VOI LUT Module Attributes */
  {
   /* (0028,3010) */ {ET_VOILUTSequence,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C1102SQVOILUTSequence = {E_ListIsTagTypeMemberWithCond,3,ModAttrT_03C1102SQVOILUTSequence, 0, 0,1,ModAttrT_SQ03C1102SQVOILUTSequence};

/* ModAttrTable references */
S_TagType ModAttrT_03C1201[] =   /* SOP Common Module Attributes */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_1C},
   /* (0008,0012) */ {ET_InstanceCreationDate,EType_3},
   /* (0008,0013) */ {ET_InstanceCreationTime,EType_3},
   /* (0008,0014) */ {ET_InstanceCreatorUID,EType_3},
   /* (0008,0016) */ {ET_SOPClassUID,EType_1},
   /* (0008,0018) */ {ET_SOPInstanceUID,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_03C1201 = {E_ListIsTagTypeMember,6,ModAttrT_03C1201, 0, 0,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C1201V1SpecificCharacterSet[] =   /* SOP Common Module Attributes */
  {
   {"ISO-IR 100","Latin set 1"},
   {"ISO-IR 101","Latin set 2"},
   {"ISO-IR 109","Latin set 3"},
   {"ISO-IR 110","Latin set 4"},
   {"ISO-IR 126","Greek"},
   {"ISO-IR 127","Arabic"},
   {"ISO-IR 138","Hebrew"},
   {"ISO-IR 144","Cyrillic"},
   {"ISO-IR 148","Latin set 5"}
  };
S_ListDesc1 ModAttrT_Desc_03C1201V1SpecificCharacterSet = {E_ListIsVal1MemberWithAddVal,9,ModAttrT_03C1201V1SpecificCharacterSet,ET_SpecificCharacterSet,EType_1C,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C07041[] =   /* Frame Of Reference Module Attributes */
  {
   /* (0020,0052) */ {ET_FrameofReferenceUID,EType_1},
   /* (0020,1040) */ {ET_PositionReferenceIndicator,EType_2}
  };
S_ListDesc1 ModAttrT_Desc_03C07041 = {E_ListIsTagTypeMember,2,ModAttrT_03C07041, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C07062[] =   /* Image Plane Module Attributes */
  {
   /* (0018,0050) */ {ET_SliceThickness,EType_2},
   /* (0020,0032) */ {ET_ImagePositionPatient,EType_1},
   /* (0020,0037) */ {ET_ImageOrientationPatient,EType_1},
   /* (0020,1041) */ {ET_SliceLocation,EType_3},
   /* (0028,0030) */ {ET_PixelSpacing,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_03C07062 = {E_ListIsTagTypeMember,5,ModAttrT_03C07062, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C07064[] =   /* Contrast/Bolus Module Attributes */
  {
   /* (0018,0010) */ {ET_ContrastBolusAgent,EType_2},
   /* (0018,1040) */ {ET_ContrastBolusRoute,EType_3},
   /* (0018,1041) */ {ET_ContrastBolusVolume,EType_3},
   /* (0018,1042) */ {ET_ContrastBolusStartTime,EType_3},
   /* (0018,1043) */ {ET_ContrastBolusStopTime,EType_3},
   /* (0018,1044) */ {ET_ContrastBolusTotalDose,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C07064 = {E_ListIsTagTypeMember,6,ModAttrT_03C07064, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C08021[] =   /* CT Image Module Attributes */
  {
   /* (0008,0008) */ {ET_ImageType,EType_1},
   /* (0018,0022) */ {ET_ScanOptions,EType_3},
   /* (0018,0060) */ {ET_KVP,EType_2},
   /* (0018,0090) */ {ET_DataCollectionDiameter,EType_3},
   /* (0018,1100) */ {ET_ReconstructionDiameter,EType_3},
   /* (0018,1110) */ {ET_DistanceSourcetoDetector,EType_3},
   /* (0018,1111) */ {ET_DistanceSourcetoPatient,EType_3},
   /* (0018,1120) */ {ET_GantryDetectorTilt,EType_3},
   /* (0018,1130) */ {ET_TableHeight,EType_3},
   /* (0018,1140) */ {ET_RotationDirection,EType_3},
   /* (0018,1150) */ {ET_ExposureTime,EType_3},
   /* (0018,1151) */ {ET_XrayTubeCurrent,EType_3},
   /* (0018,1152) */ {ET_Exposure,EType_3},
   /* (0018,1160) */ {ET_FilterType,EType_3},
   /* (0018,1170) */ {ET_GeneratorPower,EType_3},
   /* (0018,1190) */ {ET_FocalSpots,EType_3},
   /* (0018,1210) */ {ET_ConvolutionKernel,EType_3},
   /* (0020,0012) */ {ET_AcquisitionNumber,EType_2},
   /* (0028,0002) */ {ET_SamplesperPixel,EType_1},
   /* (0028,0004) */ {ET_PhotometricInterpretation,EType_1},
   /* (0028,0100) */ {ET_BitsAllocated0028,EType_1},
   /* (0028,0101) */ {ET_BitsStored,EType_1},
   /* (0028,0102) */ {ET_HighBit,EType_1},
   /* (0028,1052) */ {ET_RescaleIntercept,EType_1},
   /* (0028,1053) */ {ET_RescaleSlope,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_03C08021 = {E_ListIsTagTypeMember,25,ModAttrT_03C08021, 0, 0,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C08021V3ImageType[] =   /* CT Image Module Attributes */
  {
   "AXIAL",
   "LOCALIZER"
  };
S_ListDesc1 ModAttrT_Desc_03C08021V3ImageType = {E_ListIsVal3Member,2,ModAttrT_03C08021V3ImageType,ET_ImageType,EType_1,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C08021V1RotationDirection[] =   /* CT Image Module Attributes */
  {
   {"CW","clockwise"},
   {"CC","counter clockwise"}
  };
S_ListDesc1 ModAttrT_Desc_03C08021V1RotationDirection = {E_ListIsVal1MemberWithAddVal,2,ModAttrT_03C08021V1RotationDirection,ET_RotationDirection,EType_3,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C08021V1SamplesperPixel[] =   /* CT Image Module Attributes */
  {
   "1"
  };
S_ListDesc1 ModAttrT_Desc_03C08021V1SamplesperPixel = {E_ListIsVal1Member,1,ModAttrT_03C08021V1SamplesperPixel,ET_SamplesperPixel,EType_1,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C08021V1PhotometricInterpretation[] =   /* CT Image Module Attributes */
  {
   "MONOCHROME1",
   "MONOCHROME2"
  };
S_ListDesc1 ModAttrT_Desc_03C08021V1PhotometricInterpretation = {E_ListIsVal1Member,2,ModAttrT_03C08021V1PhotometricInterpretation,ET_PhotometricInterpretation,EType_1,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C08021V1BitsAllocated0028[] =   /* CT Image Module Attributes */
  {
   "16"
  };
S_ListDesc1 ModAttrT_Desc_03C08021V1BitsAllocated0028 = {E_ListIsVal1Member,1,ModAttrT_03C08021V1BitsAllocated0028,ET_BitsAllocated0028,EType_1,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C08021V1BitsStored[] =   /* CT Image Module Attributes */
  {
   "12",
   "13",
   "14",
   "15",
   "16"
  };
S_ListDesc1 ModAttrT_Desc_03C08021V1BitsStored = {E_ListIsVal1Member,5,ModAttrT_03C08021V1BitsStored,ET_BitsStored,EType_1,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C08021V1HighBit[] =   /* CT Image Module Attributes */
  {
   "(0028,0101)-1"
  };
S_ListDesc1 ModAttrT_Desc_03C08021V1HighBit = {E_ListIsVal1Member,1,ModAttrT_03C08021V1HighBit,ET_HighBit,EType_1,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C08031[] =   /* MR Image Module Attributes */
  {
   /* (0008,0008) */ {ET_ImageType,EType_1},
   /* (0018,0020) */ {ET_ScanningSequence,EType_1},
   /* (0018,0021) */ {ET_SequenceVariant,EType_1},
   /* (0018,0022) */ {ET_ScanOptions,EType_2},
   /* (0018,0023) */ {ET_MRAcquisitionType,EType_2},
   /* (0018,0024) */ {ET_SequenceName,EType_3},
   /* (0018,0025) */ {ET_AngioFlag,EType_3},
   /* (0018,0080) */ {ET_RepetitionTime,EType_2C},
   /* (0018,0081) */ {ET_EchoTime,EType_2},
   /* (0018,0082) */ {ET_InversionTime,EType_2C},
   /* (0018,0083) */ {ET_NumberofAverages,EType_3},
   /* (0018,0084) */ {ET_ImagingFrequency,EType_3},
   /* (0018,0085) */ {ET_ImagedNucleus,EType_3},
   /* (0018,0086) */ {ET_EchoNumbers,EType_3},
   /* (0018,0087) */ {ET_MagneticFieldStrength,EType_3},
   /* (0018,0088) */ {ET_SpacingBetweenSlices,EType_3},
   /* (0018,0089) */ {ET_NumberofPhaseEncodingSteps,EType_3},
   /* (0018,0091) */ {ET_EchoTrainLength,EType_2},
   /* (0018,0093) */ {ET_PercentSampling,EType_3},
   /* (0018,0094) */ {ET_PercentPhaseFieldofView,EType_3},
   /* (0018,0095) */ {ET_PixelBandwidth,EType_3},
   /* (0018,1060) */ {ET_TriggerTime,EType_2C},
   /* (0018,1062) */ {ET_NominalInterval,EType_3},
   /* (0018,1080) */ {ET_BeatRejectionFlag,EType_3},
   /* (0018,1081) */ {ET_LowRRValue,EType_3},
   /* (0018,1082) */ {ET_HighRRValue,EType_3},
   /* (0018,1083) */ {ET_IntervalsAcquired,EType_3},
   /* (0018,1084) */ {ET_IntervalsRejected,EType_3},
   /* (0018,1085) */ {ET_PVCRejection,EType_3},
   /* (0018,1086) */ {ET_SkipBeats,EType_3},
   /* (0018,1088) */ {ET_HeartRate,EType_3},
   /* (0018,1090) */ {ET_CardiacNumberofImages,EType_3},
   /* (0018,1094) */ {ET_TriggerWindow,EType_3},
   /* (0018,1100) */ {ET_ReconstructionDiameter,EType_3},
   /* (0018,1250) */ {ET_ReceivingCoil,EType_3},
   /* (0018,1251) */ {ET_TransmittingCoil,EType_3},
   /* (0018,1310) */ {ET_AcquisitionMatrix,EType_3},
   /* (0018,1312) */ {ET_PhaseEncodingDirection,EType_3},
   /* (0018,1314) */ {ET_FlipAngle,EType_3},
   /* (0018,1315) */ {ET_VariableFlipAngleFlag,EType_3},
   /* (0018,1316) */ {ET_SAR,EType_3},
   /* (0018,1318) */ {ET_dBdt,EType_3},
   /* (0020,0100) */ {ET_TemporalPositionIdentifier,EType_3},
   /* (0020,0105) */ {ET_NumberofTemporalPositions,EType_3},
   /* (0020,0110) */ {ET_TemporalResolution,EType_3},
   /* (0028,0002) */ {ET_SamplesperPixel,EType_1},
   /* (0028,0004) */ {ET_PhotometricInterpretation,EType_1},
   /* (0028,0100) */ {ET_BitsAllocated0028,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_03C08031 = {E_ListIsTagTypeMember,48,ModAttrT_03C08031, 0, 0,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C08031V3ImageType[] =   /* MR Image Module Attributes */
  {
   "DENSITY MAP",
   "DIFFUSION MAP",
   "IMAGE ADDITION",
   "MODULUS SUBSTRACT",
   "MPR",
   "OTHER",
   "PHASE MAP",
   "PHASE SUBSTRACT",
   "PROJECTION IMAGE",
   "T1 MAP",
   "T2 MAP",
   "VELOCITY MAP"
  };
S_ListDesc1 ModAttrT_Desc_03C08031V3ImageType = {E_ListIsVal3Member,12,ModAttrT_03C08031V3ImageType,ET_ImageType,EType_1,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C08031V1ScanningSequence[] =   /* MR Image Module Attributes */
  {
   {"EP","Echo Planar"},
   {"GR","Gradient Recalled"},
   {"IR","Inversion Recovery"},
   {"RM","Research Mode"},
   {"SE","Spin Echo"}
  };
S_ListDesc1 ModAttrT_Desc_03C08031V1ScanningSequence = {E_ListIsVal1MemberWithAddVal,5,ModAttrT_03C08031V1ScanningSequence,ET_ScanningSequence,EType_1,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C08031V1SequenceVariant[] =   /* MR Image Module Attributes */
  {
   {"SK","segmented k-space"},
   {"MTC","magnetization transfer contrast"},
   {"SS","steady state"},
   {"TRSS","time reversed steady state"},
   {"SP","spoiled"},
   {"MP","MAG prepared"},
   {"OSP","oversampling phase"}
  };
S_ListDesc1 ModAttrT_Desc_03C08031V1SequenceVariant = {E_ListIsVal1MemberWithAddVal,7,ModAttrT_03C08031V1SequenceVariant,ET_SequenceVariant,EType_1,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C08031V1ScanOptions[] =   /* MR Image Module Attributes */
  {
   {"PER","Phase Encode Recordering"},
   {"RG","Respiratory Gating"},
   {"CG","Cardiac Gating"},
   {"PPG","Pripheral Pulse Gating"},
   {"FC","Flow Compensation"},
   {"PFF","Partial Fourier-Frequency"},
   {"PFP","Partial Fourier-Phase"},
   {"SP","Spatial Presaturation"},
   {"FS","Fat Saturation"}
  };
S_ListDesc1 ModAttrT_Desc_03C08031V1ScanOptions = {E_ListIsVal1MemberWithAddVal,9,ModAttrT_03C08031V1ScanOptions,ET_ScanOptions,EType_2,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C08031V1MRAcquisitionType[] =   /* MR Image Module Attributes */
  {
   {"2D","frequency x phase"},
   {"3D","frequency x phase x phase"}
  };
S_ListDesc1 ModAttrT_Desc_03C08031V1MRAcquisitionType = {E_ListIsVal1MemberWithAddVal,2,ModAttrT_03C08031V1MRAcquisitionType,ET_MRAcquisitionType,EType_2,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C08031V1AngioFlag[] =   /* MR Image Module Attributes */
  {
   {"Y","Image is Angio"},
   {"N","Image is not Angio"}
  };
S_ListDesc1 ModAttrT_Desc_03C08031V1AngioFlag = {E_ListIsVal1MemberWithAddVal,2,ModAttrT_03C08031V1AngioFlag,ET_AngioFlag,EType_3,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C08031V1BeatRejectionFlag[] =   /* MR Image Module Attributes */
  {
   {"Y","yes"},
   {"N","no"}
  };
S_ListDesc1 ModAttrT_Desc_03C08031V1BeatRejectionFlag = {E_ListIsVal1MemberWithAddVal,2,ModAttrT_03C08031V1BeatRejectionFlag,ET_BeatRejectionFlag,EType_3,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C08031V1PhaseEncodingDirection[] =   /* MR Image Module Attributes */
  {
   {"ROW","phase encoded in rows"},
   {"COL","phase encoded in columns"}
  };
S_ListDesc1 ModAttrT_Desc_03C08031V1PhaseEncodingDirection = {E_ListIsVal1MemberWithAddVal,2,ModAttrT_03C08031V1PhaseEncodingDirection,ET_PhaseEncodingDirection,EType_3,0,NULL};

S_ListDesc1 ModAttrT_Desc_03C08031V1VariableFlipAngleFlag = {E_ListIsVal1MemberWithAddVal,2,ModAttrT_03C08031V1BeatRejectionFlag,ET_VariableFlipAngleFlag,EType_3,0,NULL};

S_ListDesc1 ModAttrT_Desc_03C08031V1SamplesperPixel = {E_ListIsVal1Member,1,ModAttrT_03C08021V1SamplesperPixel,ET_SamplesperPixel,EType_1,0,NULL};

S_ListDesc1 ModAttrT_Desc_03C08031V1PhotometricInterpretation = {E_ListIsVal1Member,2,ModAttrT_03C08021V1PhotometricInterpretation,ET_PhotometricInterpretation,EType_1,0,NULL};

S_ListDesc1 ModAttrT_Desc_03C08031V1BitsAllocated0028 = {E_ListIsVal1Member,1,ModAttrT_03C08021V1BitsAllocated0028,ET_BitsAllocated0028,EType_1,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C08041[] =   /* NM Series Module Attributes */
  {
   /* (0008,0042) */ {ET_NuclearMedicineSeriesType,EType_2},
   /* (0018,0030) */ {ET_Radionuclide,EType_2},
   /* (0018,0031) */ {ET_Radiopharmaceutical,EType_3},
   /* (0018,0032) */ {ET_EnergyWindowCenterline,EType_3},
   /* (0018,0033) */ {ET_EnergyWindowTotalWidth,EType_3},
   /* (0018,0034) */ {ET_InterventionDrugName,EType_3},
   /* (0018,0035) */ {ET_InterventionDrugStartTime,EType_3},
   /* (0018,0072) */ {ET_EffectiveSeriesDuration,EType_3},
   /* (0018,1045) */ {ET_Syringecounts,EType_3},
   /* (0018,1061) */ {ET_TriggerSourceorType,EType_3},
   /* (0018,1070) */ {ET_RadionuclideRoute,EType_3},
   /* (0018,1071) */ {ET_RadionuclideVolume,EType_3},
   /* (0018,1072) */ {ET_RadionuclideStartTime,EType_3},
   /* (0018,1073) */ {ET_RadionuclideStopTime,EType_3},
   /* (0018,1074) */ {ET_RadionuclideTotalDose,EType_3},
   /* (0018,1120) */ {ET_GantryDetectorTilt,EType_3},
   /* (0018,1300) */ {ET_ScanVelocity,EType_2C},
   /* (0018,1301) */ {ET_WholeBodyTechnique,EType_2C},
   /* (0018,1302) */ {ET_ScanLength,EType_2C},
   /* (0020,0014) */ {ET_IsotopeNumber,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C08041 = {E_ListIsTagTypeMember,20,ModAttrT_03C08041, 0, 0,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C08041V1NuclearMedicineSeriesType[] =   /* NM Series Module Attributes */
  {
   "CONTINUOUS TOMO",
   "DYNAMIC",
   "GATED",
   "GATED TOMO",
   "STATIC",
   "STEP_SHOOT TOMO",
   "WHOLE BODY"
  };
S_ListDesc1 ModAttrT_Desc_03C08041V1NuclearMedicineSeriesType = {E_ListIsVal1Member,7,ModAttrT_03C08041V1NuclearMedicineSeriesType,ET_NuclearMedicineSeriesType,EType_2,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C08041V1WholeBodyTechnique[] =   /* NM Series Module Attributes */
  {
   {"1PS","on pass"},
   {"2PS","two pass"},
   {"PCN","patient contour following employed"},
   {"DIV","diverging collimator used"},
   {"MSP","multiple static images collected"}
  };
S_ListDesc1 ModAttrT_Desc_03C08041V1WholeBodyTechnique = {E_ListIsVal1MemberWithAddVal,5,ModAttrT_03C08041V1WholeBodyTechnique,ET_WholeBodyTechnique,EType_2C,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C08042[] =   /* NM Equipment Module Attributes */
  {
   /* (0018,1145) */ {ET_CenterofRotationOffset,EType_3},
   /* (0018,1147) */ {ET_FieldofViewShape,EType_3},
   /* (0018,1149) */ {ET_FieldofViewDimensions,EType_3},
   /* (0018,1180) */ {ET_CollimatorgridName,EType_3},
   /* (0018,1181) */ {ET_CollimatorType,EType_3},
   /* (0018,1182) */ {ET_FocalDistance,EType_3},
   /* (0018,1183) */ {ET_XFocusCenter,EType_3},
   /* (0018,1184) */ {ET_YFocusCenter,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C08042 = {E_ListIsTagTypeMember,8,ModAttrT_03C08042, 0, 0,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C08042V1FieldofViewShape[] =   /* NM Equipment Module Attributes */
  {
   "RECTANGLE",
   "ROUND",
   "HEXAGONAL"
  };
S_ListDesc1 ModAttrT_Desc_03C08042V1FieldofViewShape = {E_ListIsVal1Member,3,ModAttrT_03C08042V1FieldofViewShape,ET_FieldofViewShape,EType_3,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C08042V1CollimatorType[] =   /* NM Equipment Module Attributes */
  {
   {"ASTG","Astigmatic"},
   {"CONE","Cone-beam"},
   {"FANB","Fan-beam"},
   {"PARA","Parallel"},
   {"PINH","Pinhole"},
   {"SLNT","Slant hole"}
  };
S_ListDesc1 ModAttrT_Desc_03C08042V1CollimatorType = {E_ListIsVal1MemberWithAddVal,6,ModAttrT_03C08042V1CollimatorType,ET_CollimatorType,EType_3,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C07066[] =   /* Multi-frame Module Attributes */
  {
   /* (0028,0008) */ {ET_NumberofFrames,EType_1},
   /* (0028,0009) */ {ET_FrameIncrementPointer,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_03C07066 = {E_ListIsTagTypeMember,2,ModAttrT_03C07066, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C08043[] =   /* NM Image Module Attributes */
  {
   /* (0018,0070) */ {ET_CountsAccumulated,EType_2},
   /* (0018,0071) */ {ET_AcquisitionTerminationCondition,EType_3},
   /* (0018,1100) */ {ET_ReconstructionDiameter,EType_3},
   /* (0018,1110) */ {ET_DistanceSourcetoDetector,EType_3},
   /* (0018,1130) */ {ET_TableHeight,EType_3},
   /* (0018,1131) */ {ET_TableTraverse,EType_3},
   /* (0018,1141) */ {ET_AngularPosition,EType_3},
   /* (0018,1142) */ {ET_RadialPosition,EType_3},
   /* (0018,1210) */ {ET_ConvolutionKernel,EType_3},
   /* (0018,1242) */ {ET_ActualFrameDuration,EType_3},
   /* (0018,1243) */ {ET_CountRate,EType_3},
   /* (0018,5020) */ {ET_PreprocessingFunction,EType_3},
   /* (0018,5021) */ {ET_PostprocessingFunction,EType_3},
   /* (0020,0015) */ {ET_PhaseNumber,EType_3},
   /* (0020,0016) */ {ET_IntervalNumber,EType_3},
   /* (0020,0017) */ {ET_TimeSlotNumber,EType_3},
   /* (0020,0018) */ {ET_AngleNumber,EType_3},
   /* (0028,0009) */ {ET_FrameIncrementPointer,EType_1C},
   /* (0028,0031) */ {ET_ZoomFactor,EType_2},
   /* (0028,0032) */ {ET_ZoomCenter,EType_3},
   /* (0028,0051) */ {ET_CorrectedImage,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C08043 = {E_ListIsTagTypeMember,21,ModAttrT_03C08043, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C08043SQReferencedOverlaySequence[] =  /* NM Image Module Attributes */
  {
  /* (0008,1130) */ {ET_ReferencedOverlaySequence,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C08043SQReferencedOverlaySequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_SOPUIDRefs, 0, 0,1,ModAttrT_03C08043SQReferencedOverlaySequence};

/* ModAttrTable references */
S_TagType ModAttrT_03C08043SQReferencedCurveSequence[] =  /* NM Image Module Attributes */
  {
  /* (0008,1145) */ {ET_ReferencedCurveSequence,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C08043SQReferencedCurveSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_SOPUIDRefs, 0, 0,1,ModAttrT_03C08043SQReferencedCurveSequence};

/* ModAttrTable references */
S_List2Str ModAttrT_03C08043V1AcquisitionTerminationCondition[] =   /* NM Image Module Attributes */
  {
   {"CNTS","counts"},
   {"DENS","density"},
   {"MANU","manual"},
   {"OVFL","data overflow"},
   {"TIME","time"},
   {"TRIG","physiological trigger"}
  };
S_ListDesc1 ModAttrT_Desc_03C08043V1AcquisitionTerminationCondition = {E_ListIsVal1MemberWithAddVal,6,ModAttrT_03C08043V1AcquisitionTerminationCondition,ET_AcquisitionTerminationCondition,EType_3,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C08043V1CorrectedImage[] =   /* NM Image Module Attributes */
  {
   {"ATTN","attenuation corrected"},
   {"COR","center of rotation corrected"},
   {"DECY","decay corrected"},
   {"DTIM","dead time corrected"},
   {"NCO","non-circular orbit corrected"},
   {"SCAT","scatter corrected"},
   {"UNIF","flood corrected"}
  };
S_ListDesc1 ModAttrT_Desc_03C08043V1CorrectedImage = {E_ListIsVal1MemberWithAddVal,7,ModAttrT_03C08043V1CorrectedImage,ET_CorrectedImage,EType_3,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C08044[] =   /* NM SPECT Acquisition Image Module Attributes */
  {
   /* (0018,1100) */ {ET_ReconstructionDiameter,EType_3},
   /* (0018,1110) */ {ET_DistanceSourcetoDetector,EType_3},
   /* (0018,1130) */ {ET_TableHeight,EType_3},
   /* (0018,1131) */ {ET_TableTraverse,EType_3},
   /* (0018,1140) */ {ET_RotationDirection,EType_3},
   /* (0018,1141) */ {ET_AngularPosition,EType_3},
   /* (0018,1142) */ {ET_RadialPosition,EType_3},
   /* (0018,1143) */ {ET_ScanArc,EType_2C},
   /* (0018,1144) */ {ET_AngularStep,EType_1},
   /* (0018,1146) */ {ET_RotationOffset,EType_3},
   /* (0020,0018) */ {ET_AngleNumber,EType_3},
   /* (0028,0009) */ {ET_FrameIncrementPointer,EType_1},
   /* (0028,0031) */ {ET_ZoomFactor,EType_2},
   /* (0028,0032) */ {ET_ZoomCenter,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C08044 = {E_ListIsTagTypeMember,14,ModAttrT_03C08044, 0, 0,0,NULL};

S_ListDesc1 ModAttrT_Desc_03C08044V1RotationDirection = {E_ListIsVal1MemberWithAddVal,2,ModAttrT_03C08021V1RotationDirection,ET_RotationDirection,EType_3,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C08044V1FrameIncrementPointer[] =   /* NM SPECT Acquisition Image Module Attributes */
  {
   "00181144"
  };
S_ListDesc1 ModAttrT_Desc_03C08044V1FrameIncrementPointer = {E_ListIsVal1Member,1,ModAttrT_03C08044V1FrameIncrementPointer,ET_FrameIncrementPointer,EType_1,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C08045[] =   /* NM Multi-gated Acquisition Image Module Attributes */
  {
   /* (0018,1060) */ {ET_TriggerTime,EType_3},
   /* (0018,1061) */ {ET_TriggerSourceorType,EType_2},
   /* (0018,1062) */ {ET_NominalInterval,EType_3},
   /* (0018,1063) */ {ET_FrameTime,EType_2},
   /* (0018,1064) */ {ET_FramingType,EType_3},
   /* (0018,1080) */ {ET_BeatRejectionFlag,EType_3},
   /* (0018,1081) */ {ET_LowRRValue,EType_3},
   /* (0018,1082) */ {ET_HighRRValue,EType_3},
   /* (0018,1083) */ {ET_IntervalsAcquired,EType_3},
   /* (0018,1084) */ {ET_IntervalsRejected,EType_3},
   /* (0018,1085) */ {ET_PVCRejection,EType_3},
   /* (0018,1086) */ {ET_SkipBeats,EType_3},
   /* (0018,1088) */ {ET_HeartRate,EType_3},
   /* (0018,1090) */ {ET_CardiacNumberofImages,EType_3},
   /* (0028,0009) */ {ET_FrameIncrementPointer,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_03C08045 = {E_ListIsTagTypeMember,15,ModAttrT_03C08045, 0, 0,0,NULL};

S_ListDesc1 ModAttrT_Desc_03C08045V1BeatRejectionFlag = {E_ListIsVal1MemberWithAddVal,2,ModAttrT_03C08031V1BeatRejectionFlag,ET_BeatRejectionFlag,EType_3,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C08045V1FrameIncrementPointer[] =   /* NM Multi-gated Acquisition Image Module Attributes */
  {
   "00181063"
  };
S_ListDesc1 ModAttrT_Desc_03C08045V1FrameIncrementPointer = {E_ListIsVal1Member,1,ModAttrT_03C08045V1FrameIncrementPointer,ET_FrameIncrementPointer,EType_1,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C0903[] =   /* Multi-frame Overlay Module Attributes */
  {
   /* (60xx,0015) */ {ET_NumberofFramesinOverlay,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_03C0903 = {E_ListIsTagTypeMember,1,ModAttrT_03C0903, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C0901[] =   /* Overlay Identification Module Attributes */
  {
   /* (0008,0024) */ {ET_OverlayDate,EType_3},
   /* (0008,0034) */ {ET_OverlayTime,EType_3},
   /* (0020,0022) */ {ET_OverlayNumber,EType_2}
  };
S_ListDesc1 ModAttrT_Desc_03C0901 = {E_ListIsTagTypeMember,3,ModAttrT_03C0901, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C0901SQReferencedImageSequence[] =  /* Overlay Identification Module Attributes */
  {
  /* (0008,1140) */ {ET_ReferencedImageSequence,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C0901SQReferencedImageSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_SOPUIDRefs, 0, 0,1,ModAttrT_03C0901SQReferencedImageSequence};

/* ModAttrTable references */
S_TagType ModAttrT_03C1001[] =   /* Curve Identification Module Attributes */
  {
   /* (0008,0025) */ {ET_CurveDate,EType_3},
   /* (0008,0035) */ {ET_CurveTime,EType_3},
   /* (0020,0024) */ {ET_CurveNumber,EType_2}
  };
S_ListDesc1 ModAttrT_Desc_03C1001 = {E_ListIsTagTypeMember,3,ModAttrT_03C1001, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C1001SQReferencedOverlaySequence[] =  /* Curve Identification Module Attributes */
  {
  /* (0008,1130) */ {ET_ReferencedOverlaySequence,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C1001SQReferencedOverlaySequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_SOPUIDRefs, 0, 0,1,ModAttrT_03C1001SQReferencedOverlaySequence};

/* ModAttrTable references */
S_TagType ModAttrT_03C1001SQReferencedImageSequence[] =  /* Curve Identification Module Attributes */
  {
  /* (0008,1140) */ {ET_ReferencedImageSequence,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C1001SQReferencedImageSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_SOPUIDRefs, 0, 0,1,ModAttrT_03C1001SQReferencedImageSequence};

/* ModAttrTable references */
S_TagType ModAttrT_03C1001SQReferencedCurveSequence[] =  /* Curve Identification Module Attributes */
  {
  /* (0008,1145) */ {ET_ReferencedCurveSequence,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C1001SQReferencedCurveSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_SOPUIDRefs, 0, 0,1,ModAttrT_03C1001SQReferencedCurveSequence};

/* ModAttrTable references */
S_TagType ModAttrT_03C08051[] =   /* US Frame of Reference Module */
  {
   /* (0018,6018) */ {ET_RegionLocationMinX0,EType_1},
   /* (0018,601A) */ {ET_RegionLocationMinY0,EType_1},
   /* (0018,601C) */ {ET_RegionLocationMaxX1,EType_1},
   /* (0018,601E) */ {ET_RegionLocationMaxY1,EType_1},
   /* (0018,6020) */ {ET_ReferencePixelX0,EType_3},
   /* (0018,6022) */ {ET_ReferencePixelY0,EType_3},
   /* (0018,6024) */ {ET_PhysicalUnitsXDirection,EType_1},
   /* (0018,6026) */ {ET_PhysicalUnitsYDirection,EType_1},
   /* (0018,6028) */ {ET_ReferencePixelPhysicalValueX,EType_3},
   /* (0018,602A) */ {ET_ReferencePixelPhysicalValueY,EType_3},
   /* (0018,602C) */ {ET_PhysicalDeltaX,EType_1},
   /* (0018,602E) */ {ET_PhysicalDeltaY,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_03C08051 = {E_ListIsTagTypeMember,12,ModAttrT_03C08051, 0, 0,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C08051V1PhysicalUnitsXDirection[] =   /* US Frame of Reference Module */
  {
   {"0000H","None or not applicable"},
   {"0001H","Percent"},
   {"0002H","DB"},
   {"0003H","cm"},
   {"0004H","seconds"},
   {"0005H","hertz"},
   {"0006H","DB/seconds"},
   {"0007H","cm/sec"},
   {"0008H","cm**2"},
   {"0009H","cm**2/sec"},
   {"000AH","cm**3"},
   {"000BH","cm**3/sec"},
   {"000CH","degrees"}
  };
S_ListDesc1 ModAttrT_Desc_03C08051V1PhysicalUnitsXDirection = {E_ListIsVal1MemberWithAddVal,13,ModAttrT_03C08051V1PhysicalUnitsXDirection,ET_PhysicalUnitsXDirection,EType_1,0,NULL};

S_ListDesc1 ModAttrT_Desc_03C08051V1PhysicalUnitsYDirection = {E_ListIsVal1MemberWithAddVal,13,ModAttrT_03C08051V1PhysicalUnitsXDirection,ET_PhysicalUnitsYDirection,EType_1,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C08052[] =   /* US Region Calibration Module Attributes */
  {
   {ET_UNKNOWN,EType_UNKNOWN}
  };
S_ListDesc1 ModAttrT_Desc_03C08052 = {E_ListIsTagTypeMember,0,ModAttrT_03C08052, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C08052SQSequenceofUltrasoundRegions[] =  /* US Region Calibration Module Attributes  */
  {
   /* (0018,6012) */ {ET_RegionSpatialFormat,EType_1},
   /* (0018,6014) */ {ET_RegionDataType,EType_1},
   /* (0018,6016) */ {ET_RegionFlags,EType_1},
   /* (0018,6018) */ {ET_RegionLocationMinX0,EType_1},
   /* (0018,601A) */ {ET_RegionLocationMinY0,EType_1},
   /* (0018,601C) */ {ET_RegionLocationMaxX1,EType_1},
   /* (0018,601E) */ {ET_RegionLocationMaxY1,EType_1},
   /* (0018,6020) */ {ET_ReferencePixelX0,EType_3},
   /* (0018,6022) */ {ET_ReferencePixelY0,EType_3},
   /* (0018,6024) */ {ET_PhysicalUnitsXDirection,EType_1},
   /* (0018,6026) */ {ET_PhysicalUnitsYDirection,EType_1},
   /* (0018,6028) */ {ET_ReferencePixelPhysicalValueX,EType_3},
   /* (0018,602A) */ {ET_ReferencePixelPhysicalValueY,EType_3},
   /* (0018,602C) */ {ET_PhysicalDeltaX,EType_1},
   /* (0018,602E) */ {ET_PhysicalDeltaY,EType_1},
   /* (0018,6030) */ {ET_TransducerFrequency,EType_3},
   /* (0018,6032) */ {ET_PulseRepetitionFrequency,EType_3},
   /* (0018,6034) */ {ET_DopplerCorrectionAngle,EType_3},
   /* (0018,6036) */ {ET_SterringAngle,EType_3},
   /* (0018,6038) */ {ET_DopplerSampleVolumeXPosition,EType_3},
   /* (0018,603A) */ {ET_DopplerSampleVolumeYPosition,EType_3},
   /* (0018,603C) */ {ET_TMLinePositionX0,EType_3},
   /* (0018,603E) */ {ET_TMLinePositionY0,EType_3},
   /* (0018,6040) */ {ET_TMLinePositionX1,EType_3},
   /* (0018,6042) */ {ET_TMLinePositionY1,EType_3},
   /* (0018,6044) */ {ET_PixelComponentOrganization,EType_1C},
   /* (0018,6046) */ {ET_PixelComponentMask,EType_1C},
   /* (0018,6048) */ {ET_PixelComponentRangeStart,EType_1C},
   /* (0018,604A) */ {ET_PixelComponentRangeStop,EType_1C},
   /* (0018,604C) */ {ET_PixelComponentPhysicalUnits,EType_1C},
   /* (0018,604E) */ {ET_PixelComponentDataType,EType_1C},
   /* (0018,6050) */ {ET_NumberofTableBreakPoints,EType_1C},
   /* (0018,6052) */ {ET_TableofXBreakPoints,EType_1C},
   /* (0018,6054) */ {ET_TableofYBreakPoints,EType_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ03C08052SQSequenceofUltrasoundRegions[] =  /* US Region Calibration Module Attributes */
  {
   /* (0018,6011) */ {ET_SequenceofUltrasoundRegions,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_03C08052SQSequenceofUltrasoundRegions = {E_ListIsTagTypeMemberWithCond,34,ModAttrT_03C08052SQSequenceofUltrasoundRegions, 0, 0,1,ModAttrT_SQ03C08052SQSequenceofUltrasoundRegions};

/* ModAttrTable references */
S_List2Str ModAttrT_03C08052V1RegionSpatialFormat[] =   /* US Region Calibration Module Attributes */
  {
   {"0000H","None or not applicable"},
   {"0001H","2D tissue or flow"},
   {"0002H","M-Mode tissue or flow"},
   {"0003H","Spectral CW or PW Doppler"},
   {"0004H","Wave form"},
   {"0005H","Graphics"}
  };
S_ListDesc1 ModAttrT_Desc_03C08052V1RegionSpatialFormat = {E_ListIsVal1MemberWithAddVal,6,ModAttrT_03C08052V1RegionSpatialFormat,ET_RegionSpatialFormat,EType_1,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C08052V1RegionDataType[] =   /* US Region Calibration Module Attributes */
  {
   {"0000H","None or not applicable"},
   {"0001H","Tissue"},
   {"0002H","Color flow"},
   {"0003H","PW Spectral Doppler"},
   {"0004H","CW Spectral Doppler"},
   {"0005H","Doppler Mean Trace"},
   {"0006H","Doppler Mode Trace"},
   {"0007H","Doppler max Trace"},
   {"0008H","volume/dt trace"},
   {"0009H","volume/trace"},
   {"000AH","ECG trace"},
   {"000BH","Pulse trace"},
   {"000CH","Phonocardiogram trace"},
   {"000DH","Gray bar"},
   {"000EH","Color bar"},
   {"000FH","Integrated Backscatter"},
   {"0010H","Area trace"},
   {"0011H","area/dt"},
   {"0012H","Other Physiological input"}
  };
S_ListDesc1 ModAttrT_Desc_03C08052V1RegionDataType = {E_ListIsVal1MemberWithAddVal,19,ModAttrT_03C08052V1RegionDataType,ET_RegionDataType,EType_1,0,NULL};

S_ListDesc1 ModAttrT_Desc_03C08052V1PhysicalUnitsXDirection = {E_ListIsVal1MemberWithAddVal,13,ModAttrT_03C08051V1PhysicalUnitsXDirection,ET_PhysicalUnitsXDirection,EType_1,0,NULL};

S_ListDesc1 ModAttrT_Desc_03C08052V1PhysicalUnitsYDirection = {E_ListIsVal1MemberWithAddVal,13,ModAttrT_03C08051V1PhysicalUnitsXDirection,ET_PhysicalUnitsYDirection,EType_1,0,NULL};

S_ListDesc1 ModAttrT_Desc_03C08052V1PixelComponentPhysicalUnits = {E_ListIsVal1MemberWithAddVal,13,ModAttrT_03C08051V1PhysicalUnitsXDirection,ET_PixelComponentPhysicalUnits,EType_1C,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C08052V1PixelComponentDataType[] =   /* US Region Calibration Module Attributes */
  {
   {"0000H","None or not applicable"},
   {"0001H","Tissue"},
   {"0002H","Spectral Doppler"},
   {"0003H","Color Flow Velocity"},
   {"0004H","Color Flow Variance"},
   {"0005H","Color Flow Intensity"},
   {"0006H","Gray bar"},
   {"0007H","Color bar"},
   {"0008H","Integrated Backscatter"},
   {"0009H","Computed Border"}
  };
S_ListDesc1 ModAttrT_Desc_03C08052V1PixelComponentDataType = {E_ListIsVal1MemberWithAddVal,10,ModAttrT_03C08052V1PixelComponentDataType,ET_PixelComponentDataType,EType_1C,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C08053[] =   /* US Image Module Attributes */
  {
   /* (0008,0008) */ {ET_ImageType,EType_2},
   /* (0008,2120) */ {ET_StageName,EType_3},
   /* (0008,2122) */ {ET_StageNumber,EType_3},
   /* (0008,2124) */ {ET_NumberofStages,EType_2C},
   /* (0008,2128) */ {ET_ViewNumber,EType_3},
   /* (0008,2129) */ {ET_NumberofEventTimers,EType_3},
   /* (0008,212A) */ {ET_NumberofViewsinStage,EType_2C},
   /* (0008,2130) */ {ET_EventElapsedTimes,EType_3},
   /* (0008,2132) */ {ET_EventTimerNames,EType_3},
   /* (0008,2200) */ {ET_TransducerPosition,EType_3},
   /* (0008,2204) */ {ET_TransducerOrientation,EType_3},
   /* (0008,2208) */ {ET_AnatomicStructure,EType_3},
   /* (0018,1060) */ {ET_TriggerTime,EType_3},
   /* (0018,1062) */ {ET_NominalInterval,EType_3},
   /* (0018,1080) */ {ET_BeatRejectionFlag,EType_3},
   /* (0018,1081) */ {ET_LowRRValue,EType_3},
   /* (0018,1082) */ {ET_HighRRValue,EType_3},
   /* (0018,1088) */ {ET_HeartRate,EType_3},
   /* (0018,5000) */ {ET_OutputPower,EType_3},
   /* (0018,5010) */ {ET_TransducerData,EType_3},
   /* (0018,5012) */ {ET_FocusDepth,EType_3},
   /* (0018,5020) */ {ET_PreprocessingFunction,EType_3},
   /* (0018,5022) */ {ET_MechanicalIndex,EType_3},
   /* (0018,5024) */ {ET_ThermalIndex,EType_3},
   /* (0018,5026) */ {ET_CranialThermalIndex,EType_3},
   /* (0018,5027) */ {ET_SoftTissueThermalIndex,EType_3},
   /* (0018,5028) */ {ET_SoftTissuefocusThermalIndex,EType_3},
   /* (0018,5029) */ {ET_SoftTissuesurfaceThermalIndex,EType_3},
   /* (0018,5050) */ {ET_DepthofScanField,EType_3},
   /* (0018,5210) */ {ET_ImageTransformationMatrix,EType_3},
   /* (0018,5212) */ {ET_ImageTranslationVector,EType_3},
   /* (0018,6031) */ {ET_TransducerType,EType_3},
   /* (0028,0004) */ {ET_PhotometricInterpretation,EType_1},
   /* (0028,0009) */ {ET_FrameIncrementPointer,EType_1C},
   /* (0028,0103) */ {ET_PixelRepresentation,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_03C08053 = {E_ListIsTagTypeMember,35,ModAttrT_03C08053, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C08053SQReferencedOverlaySequence[] =  /* US Image Module Attributes */
  {
  /* (0008,1130) */ {ET_ReferencedOverlaySequence,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C08053SQReferencedOverlaySequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_SOPUIDRefs, 0, 0,1,ModAttrT_03C08053SQReferencedOverlaySequence};

/* ModAttrTable references */
S_TagType ModAttrT_03C08053SQReferencedCurveSequence[] =  /* US Image Module Attributes */
  {
  /* (0008,1145) */ {ET_ReferencedCurveSequence,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C08053SQReferencedCurveSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_SOPUIDRefs, 0, 0,1,ModAttrT_03C08053SQReferencedCurveSequence};

/* ModAttrTable references */
char *ModAttrT_03C08053V3ImageType[] =   /* US Image Module Attributes */
  {
   "ABDOMINAL",
   "BREAST",
   "CHEST",
   "ENDOCAVITARY",
   "ENDORECTAL",
   "ENDOVAGINAL",
   "EPICARDIAL",
   "FETAL HEART",
   "GYNECOLOGY",
   "INTRACARDIAC",
   "INTRAOPERATIVE",
   "INTRAVASCULAR",
   "MUSCULOSKELETAL",
   "NEONATAL HEAD",
   "OBSTETRICAL",
   "OPHTHALMIC",
   "PEDIATRIC",
   "PELVIC",
   "RETROPERITONEAL",
   "SCROTAL",
   "SMALL PARTS",
   "TEE",
   "THYROID",
   "TRANSCRANIAL",
   "TTE",
   "US BIOPSY",
   "VASCULAR"
  };
S_ListDesc1 ModAttrT_Desc_03C08053V3ImageType = {E_ListIsVal3Member,27,ModAttrT_03C08053V3ImageType,ET_ImageType,EType_2,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C08053V4ImageType[] =   /* US Image Module Attributes */
  {
   {"0001","2D Imaging"},
   {"0002","M-Mode"},
   {"0004","CW Doppler"},
   {"0008","PW Doppler"},
   {"0010","Color Doppler"},
   {"0020","Color M-Mode"},
   {"0040","3D Rendering"}
  };
S_ListDesc1 ModAttrT_Desc_03C08053V4ImageType = {E_ListIsVal4MemberWithAddVal,7,ModAttrT_03C08053V4ImageType,ET_ImageType,EType_2,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C08053V1TransducerPosition[] =   /* US Image Module Attributes */
  {
   "ANTERIOR",
   "AORTA",
   "APICAL",
   "ARTERY",
   "BASAL ESOPHAGUS",
   "BYPASS GRAFT",
   "CAUDAL",
   "CRANIAL",
   "DISTAL",
   "ENDORECTAL",
   "ENDOVAGINAL",
   "EPICARDIAL",
   "ESOPHAGUS",
   "GASTRIC",
   "INFERIOR",
   "L ATRIUM",
   "L CORONARY ART",
   "L LIQ",
   "L LOQ",
   "L PARASTERN",
   "L SUBCLAVIAN",
   "L UIQ",
   "L UOQ",
   "L VENTRICLELATERAL",
   "LEFT",
   "LEFT PARA",
   "LOWER ESOPHAGUS",
   "LSUPRACLAVICULAR",
   "MEDIAL",
   "MID ESOPHAGUS",
   "MIDLINE",
   "OCCIPITAL",
   "POSTERIOR",
   "PROXIMAL",
   "PULMONARY ART",
   "R ATRIUM",
   "R CORONARY ART",
   "R LIQ",
   "R LOQ",
   "R PARASTERN",
   "R SUBCLAVIAN",
   "R UIQ",
   "R UOQ",
   "R VENTRICLE",
   "RIGHT",
   "RIGHT PARA",
   "RSUPRACLAVICULAR",
   "SUBCLAVIAN",
   "SUBCOSTAL",
   "SUP STERN NOTCH",
   "SUPERIOR ",
   "SUPRACLAVICULAR",
   "SURFACE",
   "TRANSGASTRIC",
   "TRANSORBITAL",
   "TRANSTEMPORAL",
   "UPPER ESOPHAGUS",
   "VEIN"
  };
S_ListDesc1 ModAttrT_Desc_03C08053V1TransducerPosition = {E_ListIsVal1Member,58,ModAttrT_03C08053V1TransducerPosition,ET_TransducerPosition,EType_3,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C08053V1TransducerOrientation[] =   /* US Image Module Attributes */
  {
   "2 CHAMBER",
   "4 CHAMBER",
   "5 CHAMBER",
   "AXIAL",
   "CORONAL",
   "L PARASAG",
   "LONG AXIS",
   "LONGITUDINAL",
   "OBLIQUE",
   "R PARASAG",
   "SAGITTAL",
   "SHORT AXIS",
   "TRANSVERSE"
  };
S_ListDesc1 ModAttrT_Desc_03C08053V1TransducerOrientation = {E_ListIsVal1Member,13,ModAttrT_03C08053V1TransducerOrientation,ET_TransducerOrientation,EType_3,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C08053V1AnatomicStructure[] =   /* US Image Module Attributes */
  {
   "3RD VENTRICLE",
   "4 CH HEART",
   "4TH VENTRICLE",
   "ABD AO",
   "AC",
   "ADRENAL",
   "AMNIOTIC FLUID",
   "AO ANNULUS",
   "AO ARCH",
   "AO LFLT TIPS",
   "AO VALVE",
   "AORTA",
   "APPENDIX",
   "ASCENDING AO",
   "ASCENDING COLON",
   "ASD",
   "AZYGOS VEIN",
   "BASILAR ART",
   "BILE DUCT",
   "BLADDER",
   "BPD",
   "C SPINE",
   "CELIAC ART",
   "CENTRAL ZONE",
   "CEREB HEMISPHERE",
   "CEREBELLUM",
   "CERVIX",
   "COLON",
   "CORONARY SINUS",
   "CUL DE SAC",
   "DESCENDING COLON",
   "DIAGONAL",
   "DISTAL LAD",
   "DISTAL LCX",
   "DISTAL RCA",
   "DUODENUM",
   "ENDOMETRIUM",
   "FEMUR LENGTH",
   "FETAL ARMS",
   "FETAL DIGITS",
   "FETAL HEART",
   "FETAL LEGS",
   "FETAL POLE",
   "FUNDUS",
   "GALLBLADDER",
   "GEST SAC",
   "HC",
   "HEPATIC ART",
   "IAS",
   "ILEUM",
   "INF MES ART",
   "INF VENA CAVA",
   "INNOMINATE A",
   "INT ATRIAL SEPT",
   "INT VENT SEPT",
   "ISTHMUS",
   "IVS",
   "L ACA",
   "L ATRIAL APPEN ",
   "L ATRIUM",
   "L AXILLARY A",
   "L AXILLARY V",
   "L BRACHIAL A",
   "L BRACHIAL V",
   "L BREAST",
   "L BULB",
   "L CCA",
   "L CFA",
   "L CFV",
   "L CHOROID PLEXUS",
   "L COM ILIAC ART",
   "L COM ILIAC V",
   "L ECA",
   "L EPIDIDYMIS",
   "L EXT ILIAC A",
   "L EXT ILIAC V",
   "L EXT JUG V",
   "L EYE",
   "L FEMUR",
   "L FOOT",
   "L GERMINAL MATRX",
   "L GSV",
   "L HEPATIC V",
   "L HIP",
   "L HUMERUS",
   "L ICA",
   "L ILIAC A",
   "L ILIAC V",
   "L INT JUG V",
   "L KIDNEY",
   "L MAIN COR ART",
   "L MCA",
   "L OVARY",
   "L PCA ",
   "L POP A",
   "L POP V",
   "L PORTAL V",
   "L PROF FEM A",
   "L PROF FEM V",
   "L RADIAL A",
   "L RADIUS",
   "L RENAL ART",
   "L RENAL V",
   "L SAPH V",
   "L SAPH V",
   "L SEM VESICLE",
   "L SFA",
   "L SFV",
   "L SHOULDER",
   "L SPINE",
   "L SUBCLAVIAN A",
   "L SUBCLAVIAN V",
   "L SVC",
   "L TESTIS",
   "L THALAMUS",
   "L TIB FIB",
   "L ULNA",
   "L ULNAR ART",
   "L VENTRICLE",
   "L VERT",
   "LAD",
   "LAD D1",
   "LAD D2",
   "LAD D3",
   "LAD S1",
   "LAD S2",
   "LAT VENTRICLES",
   "LCX",
   "LCX DM1",
   "LCX DM2",
   "LIMA",
   "LIVER",
   "LLPV",
   "LOWER DESC AO",
   "LPA",
   "LPV",
   "LUPV",
   "LV APEX",
   "LV BASE",
   "LV CAVITY",
   "LV MID",
   "LVOT",
   "MAIN PA",
   "MID HEPATIC V",
   "MID LAD",
   "MID LCX",
   "MID RCA",
   "MITRAL ANNULUS",
   "MITRAL LFLT TIPS",
   "MITRAL VALVE",
   "MORRISONS POUCH",
   "OBTUSE MARGINAL",
   "OFD",
   "PANCREAS",
   "PENILE VESSELS",
   "PERINEUM",
   "PERIPHERAL ZONE",
   "PLACENTA",
   "PLV COR ART",
   "PORTAL V",
   "POST DESC ART",
   "POST LV BRANCH",
   "PROSTATE",
   "PROX CIRC",
   "PROX LAD",
   "PROX LCX",
   "PROX RCA",
   "PULM ANNULUS",
   "PULM VALVE",
   "PULM VEIN",
   "PV TIPS",
   "R ACA",
   "R ATRIUM",
   "R AXILLARY A",
   "R AXILLARY V",
   "R BRACHIAL A",
   "R BRACHIAL V",
   "R BREAST",
   "R BULB",
   "R CCA",
   "R CFA",
   "R CFV",
   "R CHOROID PLEXUS",
   "R COM ILIAC ART",
   "R COM ILIAC V",
   "R ECA",
   "R EPIDIDYMIS",
   "R EXT ILIAC A",
   "R EXT ILIAC V",
   "R EXT JUV V",
   "R EYE",
   "R FEMUR",
   "R FOOT",
   "R GERMINAL MATRX",
   "R GSV",
   "R HEPATIC V",
   "R HIP",
   "R HUMERUS",
   "R ICA",
   "R ILIAC A",
   "R ILIAC V",
   "R INT JUG V",
   "R KIDNEY",
   "R MCA",
   "R OVARY",
   "R PCAR POP A",
   "R POP V",
   "R PORTAL V",
   "R PROF FEM A",
   "R PROF FEM V",
   "R PV",
   "R RADIAL A",
   "R RADIUS",
   "R RENAL ",
   "R RENAL ART",
   "R SAPH V",
   "R SAPH V",
   "R SEM VESICLE",
   "R SFA",
   "R SFV",
   "R SHOULDER",
   "R SUBCLAVIAN A",
   "R SUBCLAVIAN V",
   "R TESTIS",
   "R THALAMUS",
   "R TIB FIB",
   "R ULNA",
   "R ULNAR ART",
   "R VENTRICLE",
   "R VERT",
   "RAA",
   "RCA",
   "RECTUM",
   "RETROPERITONEUM",
   "RIMA",
   "RLPV",
   "RPA",
   "RUPV",
   "RV CAVITY ",
   "RVOT",
   "SACRUM",
   "SAPH V GRAFT LAD",
   "SCROTUM",
   "SIGMOID",
   "SIGMOID",
   "SMA",
   "SMALL INTESTINE",
   "SMV",
   "SPLEEN",
   "SPLENIC ART",
   "SPLENIC V",
   "STOMACH",
   "SUP VENA CAVA",
   "SVG D",
   "SVG LAD",
   "SVG LCX",
   "SVG OM",
   "SVG PDA",
   "SVG PLV",
   "SVG RCA",
   "T SPINE",
   "TESTICLE",
   "THYROID",
   "TRANSITION ZONE",
   "TRANSVERSE COLON",
   "TRICUSPID ANN",
   "TRICUSPID TIPS",
   "TRICUSPID VALVE",
   "UMB CORD",
   "UPPER DESC AO",
   "UTERINE FUNDUS",
   "UTERUS",
   "VAGINA",
   "VSD",
   "YOLK SAC"
  };
S_ListDesc1 ModAttrT_Desc_03C08053V1AnatomicStructure = {E_ListIsVal1Member,275,ModAttrT_03C08053V1AnatomicStructure,ET_AnatomicStructure,EType_3,0,NULL};

S_ListDesc1 ModAttrT_Desc_03C08053V1BeatRejectionFlag = {E_ListIsVal1MemberWithAddVal,2,ModAttrT_03C08031V1BeatRejectionFlag,ET_BeatRejectionFlag,EType_3,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C08053V1TransducerType[] =   /* US Image Module Attributes */
  {
   "SECTOR_PHASED",
   "SECTOR_MECH",
   "SECTOR_ANNULAR",
   "LINEAR",
   "CURVED LINEAR",
   "SINGLE CRYSTAL",
   "SPLIT XTAL CWD",
   "IV_PHASED",
   "IV_ROT XTAL",
   "IV_ROT MIRROR",
   "ENDOCAV_PA",
   "ENDOCAV_MECH",
   "ENDOCAV_CLA",
   "ENDOCAV_AA",
   "ENDOCAV_LINEAR",
   "VECTOR_PHASED"
  };
S_ListDesc1 ModAttrT_Desc_03C08053V1TransducerType = {E_ListIsVal1Member,16,ModAttrT_03C08053V1TransducerType,ET_TransducerType,EType_3,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C08053V1PhotometricInterpretation[] =   /* US Image Module Attributes */
  {
   "MONOCHROME2",
   "PALETTE COLOR",
   "RGB",
   "ARGB"
  };
S_ListDesc1 ModAttrT_Desc_03C08053V1PhotometricInterpretation = {E_ListIsVal1Member,4,ModAttrT_03C08053V1PhotometricInterpretation,ET_PhotometricInterpretation,EType_1,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C08053V1FrameIncrementPointer[] =   /* US Image Module Attributes */
  {
   {"00180063","sequencing by Frame Time (0018,1063)"},
   {"00181065","sequencing by Frame Time Vector (0018,1065)"}
  };
S_ListDesc1 ModAttrT_Desc_03C08053V1FrameIncrementPointer = {E_ListIsVal1MemberWithAddVal,2,ModAttrT_03C08053V1FrameIncrementPointer,ET_FrameIncrementPointer,EType_1C,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C08053V1PixelRepresentation[] =   /* US Image Module Attributes */
  {
   {"0000H","unsigned integer"}
  };
S_ListDesc1 ModAttrT_Desc_03C08053V1PixelRepresentation = {E_ListIsVal1MemberWithAddVal,1,ModAttrT_03C08053V1PixelRepresentation,ET_PixelRepresentation,EType_1,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C1003[] =   /* Audio Module Attributes */
  {
   /* (50xx,2000) */ {ET_AudioType,EType_1},
   /* (50xx,2002) */ {ET_AudioSampleFormat,EType_1},
   /* (50xx,2004) */ {ET_NumberofChannels,EType_1},
   /* (50xx,2006) */ {ET_NumberofSamples,EType_1},
   /* (50xx,2008) */ {ET_SampleRate,EType_1},
   /* (50xx,200A) */ {ET_TotalTime,EType_1},
   /* (50xx,200C) */ {ET_AudioSampleData,EType_1},
   /* (50xx,200E) */ {ET_AudioComments,EType_X}
  };
S_ListDesc1 ModAttrT_Desc_03C1003 = {E_ListIsTagTypeMember,8,ModAttrT_03C1003, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C1003SQReferencedImageSequence[] =  /* Audio Module Attributes */
  {
  /* (0008,1140) */ {ET_ReferencedImageSequence,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C1003SQReferencedImageSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_SOPUIDRefs, 0, 0,1,ModAttrT_03C1003SQReferencedImageSequence};

/* ModAttrTable references */
S_List2Str ModAttrT_03C1003V1AudioType[] =   /* Audio Module Attributes */
  {
   {"0000H","none"},
   {"0001H","Doppler Audio"},
   {"0002H","Voice Audio"},
   {"0003H","Phono Audio"}
  };
S_ListDesc1 ModAttrT_Desc_03C1003V1AudioType = {E_ListIsVal1MemberWithAddVal,4,ModAttrT_03C1003V1AudioType,ET_AudioType,EType_1,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C1003V1AudioSampleFormat[] =   /* Audio Module Attributes */
  {
   {"0000H","16 bit 2's complement"},
   {"0001H","8 bit 2's complement"}
  };
S_ListDesc1 ModAttrT_Desc_03C1003V1AudioSampleFormat = {E_ListIsVal1MemberWithAddVal,2,ModAttrT_03C1003V1AudioSampleFormat,ET_AudioSampleFormat,EType_1,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C1003V1NumberofChannels[] =   /* Audio Module Attributes */
  {
   {"0000H","mono"},
   {"0001H","stereo"}
  };
S_ListDesc1 ModAttrT_Desc_03C1003V1NumberofChannels = {E_ListIsVal1MemberWithAddVal,2,ModAttrT_03C1003V1NumberofChannels,ET_NumberofChannels,EType_1,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C08061[] =   /* SC Image Equipment Module Attributes */
  {
   /* (0008,0060) */ {ET_Modality,EType_3},
   /* (0008,0064) */ {ET_ConversionType,EType_1},
   /* (0018,1010) */ {ET_SecondaryCaptureDeviceID,EType_3},
   /* (0018,1016) */ {ET_SecondaryCaptureDeviceManufacturer,EType_3},
   /* (0018,1018) */ {ET_SecondaryCaptureDeviceManufacturersModelName,EType_3},
   /* (0018,1019) */ {ET_SecondaryCaptureDeviceSoftwareVersions,EType_3},
   /* (0018,1022) */ {ET_VideoImageFormatAcquired,EType_3},
   /* (0018,1023) */ {ET_DigitalImageFormatAcquired,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C08061 = {E_ListIsTagTypeMember,8,ModAttrT_03C08061, 0, 0,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C08061V1ConversionType[] =   /* SC Image Equipment Module Attributes */
  {
   {"DF","Digitized Film"},
   {"DI","Digital Interface"},
   {"DV","Digitized Video"},
   {"WSD","Workstation"}
  };
S_ListDesc1 ModAttrT_Desc_03C08061V1ConversionType = {E_ListIsVal1MemberWithAddVal,4,ModAttrT_03C08061V1ConversionType,ET_ConversionType,EType_1,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C08062[] =   /* SC Image Module Attributes */
  {
   /* (0018,1012) */ {ET_DateofSecondaryCapture,EType_3},
   /* (0018,1014) */ {ET_TimeofSecondaryCapture,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C08062 = {E_ListIsTagTypeMember,2,ModAttrT_03C08062, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C0707[] =   /* Patient Summary Module Attributes */
  {
   /* (0010,0010) */ {ET_PatientsName,EType_2},
   /* (0010,0020) */ {ET_PatientID,EType_2}
  };
S_ListDesc1 ModAttrT_Desc_03C0707 = {E_ListIsTagTypeMember,2,ModAttrT_03C0707, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C0708[] =   /* Study Content Module Attributes */
  {
   /* (0020,000D) */ {ET_StudyInstanceUID,EType_1},
   /* (0020,0010) */ {ET_StudyID,EType_2}
  };
S_ListDesc1 ModAttrT_Desc_03C0708 = {E_ListIsTagTypeMember,2,ModAttrT_03C0708, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C0708SQReferencedSeriesSequence[] =  /* Study Content Module Attributes  */
  {
   /* (0008,0054) */ {ET_RetrieveAETitle,EType_2C},
   /* (0020,000E) */ {ET_SeriesInstanceUID,EType_1},
   /* (0088,0130) */ {ET_StorageMediaFilesetID,EType_2C},
   /* (0088,0140) */ {ET_StorageMediaFilesetUID,EType_2C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ03C0708SQReferencedSeriesSequence[] =  /* Study Content Module Attributes */
  {
   /* (0008,1115) */ {ET_ReferencedSeriesSequence,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_03C0708SQReferencedSeriesSequence = {E_ListIsTagTypeMemberWithCond,4,ModAttrT_03C0708SQReferencedSeriesSequence, 0, 0,1,ModAttrT_SQ03C0708SQReferencedSeriesSequence};

/* ModAttrTable references */
S_TagType ModAttrT_03C0708SQSQReferencedImageSequence[] =  /* Study Content Module Attributes  */
  {
   /* (0008,0054) */ {ET_RetrieveAETitle,EType_2C},
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_2},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_2},
   /* (0088,0130) */ {ET_StorageMediaFilesetID,EType_2C},
   /* (0088,0140) */ {ET_StorageMediaFilesetUID,EType_2C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQSQ03C0708SQSQReferencedImageSequence[] =  /* Study Content Module Attributes */
  {
  {ET_ReferencedSeriesSequence,EType_1},
  {ET_ReferencedImageSequence,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_03C0708SQSQReferencedImageSequence = {E_ListIsTagTypeMemberWithCond,5,ModAttrT_03C0708SQSQReferencedImageSequence, 0, 0,2,ModAttrT_SQSQ03C0708SQSQReferencedImageSequence};

/* ModAttrTable references */
S_TagType ModAttrT_03C1103[] =   /* LUT Identification Module Attributes */
  {
   /* (0020,0026) */ {ET_LookupTableNumber,EType_2}
  };
S_ListDesc1 ModAttrT_Desc_03C1103 = {E_ListIsTagTypeMember,1,ModAttrT_03C1103, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C1103SQReferencedImageSequence[] =  /* LUT Identification Module Attributes */
  {
  /* (0008,1140) */ {ET_ReferencedImageSequence,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_03C1103SQReferencedImageSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_SOPUIDRefs, 0, 0,1,ModAttrT_03C1103SQReferencedImageSequence};

/* ModAttrTable references */
S_TagType ModAttrT_03C0201[] =   /* Patient Relationship Module Attributes */
  {
   {ET_UNKNOWN,EType_UNKNOWN}
  };
S_ListDesc1 ModAttrT_Desc_03C0201 = {E_ListIsTagTypeMember,0,ModAttrT_03C0201, 0, 0,0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C0201SQReferencedStudySequence[] =  /* Patient Relationship Module Attributes  */
  {
   /* (0008,1150) */ ET_ReferencedSOPClassUID,
   /* (0008,1155) */ ET_ReferencedSOPInstanceUID
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C0201SQReferencedStudySequence[] =  /* Patient Relationship Module Attributes */
  {
   /* (0008,1110) */ ET_ReferencedStudySequence
  };
S_ListDesc1 ModAttrT_Desc_03C0201SQReferencedStudySequence = {E_ListIsTagMemberWithCond,2,ModAttrT_03C0201SQReferencedStudySequence, 0, 0,1,ModAttrT_SQ03C0201SQReferencedStudySequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C0201SQReferencedVisitSequence[] =  /* Patient Relationship Module Attributes  */
  {
   /* (0008,1150) */ ET_ReferencedSOPClassUID,
   /* (0008,1155) */ ET_ReferencedSOPInstanceUID
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C0201SQReferencedVisitSequence[] =  /* Patient Relationship Module Attributes */
  {
   /* (0008,1125) */ ET_ReferencedVisitSequence
  };
S_ListDesc1 ModAttrT_Desc_03C0201SQReferencedVisitSequence = {E_ListIsTagMemberWithCond,2,ModAttrT_03C0201SQReferencedVisitSequence, 0, 0,1,ModAttrT_SQ03C0201SQReferencedVisitSequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C0201SQReferencedPatientAliasSequence[] =  /* Patient Relationship Module Attributes  */
  {
   /* (0008,1150) */ ET_ReferencedSOPClassUID,
   /* (0008,1155) */ ET_ReferencedSOPInstanceUID
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C0201SQReferencedPatientAliasSequence[] =  /* Patient Relationship Module Attributes */
  {
   /* (0038,0004) */ ET_ReferencedPatientAliasSequence
  };
S_ListDesc1 ModAttrT_Desc_03C0201SQReferencedPatientAliasSequence = {E_ListIsTagMemberWithCond,2,ModAttrT_03C0201SQReferencedPatientAliasSequence, 0, 0,1,ModAttrT_SQ03C0201SQReferencedPatientAliasSequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C0202[] =   /* Patient Identification Module Attributes */
  {
   /* (0010,0010) */ ET_PatientsName,
   /* (0010,0020) */ ET_PatientID,
   /* (0010,0021) */ ET_IssuerofPatientID,
   /* (0010,1000) */ ET_OtherPatientIDs,
   /* (0010,1001) */ ET_OtherPatientNames,
   /* (0010,1005) */ ET_PatientsBirthName,
   /* (0010,1060) */ ET_PatientsMothersBirthName,
   /* (0010,1090) */ ET_MedicalRecordLocator
  };
S_ListDesc1 ModAttrT_Desc_03C0202 = {E_ListIsTagMember,8,ModAttrT_03C0202, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C0203[] =   /* Patient Demographic Module Attributes */
  {
   /* (0010,0030) */ ET_PatientsBirthDate,
   /* (0010,0032) */ ET_PatientsBirthTime,
   /* (0010,0040) */ ET_PatientsSex,
   /* (0010,1020) */ ET_PatientsSize,
   /* (0010,1030) */ ET_PatientsWeight,
   /* (0010,1040) */ ET_PatientsAddress,
   /* (0010,1080) */ ET_MilitaryRank,
   /* (0010,1081) */ ET_BranchofService,
   /* (0010,2150) */ ET_CountryofResidence,
   /* (0010,2152) */ ET_RegionofResidence,
   /* (0010,2154) */ ET_PatientsTelephoneNumbers,
   /* (0010,2160) */ ET_EthnicGroup,
   /* (0010,21F0) */ ET_PatientsReligiousPreference,
   /* (0010,4000) */ ET_PatientComments
  };
S_ListDesc1 ModAttrT_Desc_03C0203 = {E_ListIsTagMember,14,ModAttrT_03C0203, 0, 0, 0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C0203SQPatientsInsurancePlanCodeSequence[] =  /* Patient Demographic Module Attributes */
  {
  /*  */ {ET_PatientsInsurancePlanCodeSequence}
  };
S_ListDesc1 ModAttrT_Desc_03C0203SQPatientsInsurancePlanCodeSequence = {E_ListIsTagMemberWithCond,3,ModAttrT_CodeRefs, 0, 0,1,ModAttrT_03C0203SQPatientsInsurancePlanCodeSequence};

S_ListDesc1 ModAttrT_Desc_03C0203V1PatientsSex = {E_ListIsVal1MemberWithAddVal,3,ModAttrT_03C07011V1PatientsSex,ET_PatientsSex,EType_X,0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C0204[] =   /* Patient Medical Module Attributes */
  {
   /* (0010,2000) */ ET_MedicalAlerts,
   /* (0010,2110) */ ET_ContrastAllergies,
   /* (0010,21A0) */ ET_SmokingStatus,
   /* (0010,21B0) */ ET_AdditionalPatientHistory,
   /* (0010,21C0) */ ET_PregnancyStatus,
   /* (0010,21D0) */ ET_LastMenstrualDate,
   /* (0038,0050) */ ET_SpecialNeeds,
   /* (0038,0500) */ ET_PatientState
  };
S_ListDesc1 ModAttrT_Desc_03C0204 = {E_ListIsTagMember,8,ModAttrT_03C0204, 0, 0, 0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C0204V1SmokingStatus[] =   /* Patient Medical Module Attributes */
  {
   "NO",
   "UNKNOWN",
   "YES"
  };
S_ListDesc1 ModAttrT_Desc_03C0204V1SmokingStatus = {E_ListIsVal1Member,3,ModAttrT_03C0204V1SmokingStatus,ET_SmokingStatus,EType_X,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_03C0204V1PregnancyStatus[] =   /* Patient Medical Module Attributes */
  {
   {"0001","not pregnant"},
   {"0002","possibly pregnant"},
   {"0003","definitely pregnant"},
   {"0004","unknown"}
  };
S_ListDesc1 ModAttrT_Desc_03C0204V1PregnancyStatus = {E_ListIsVal1MemberWithAddVal,4,ModAttrT_03C0204V1PregnancyStatus,ET_PregnancyStatus,EType_X,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C0301[] =   /* Visit Relationship Module Attributes */
  {
   {ET_UNKNOWN,EType_UNKNOWN}
  };
S_ListDesc1 ModAttrT_Desc_03C0301 = {E_ListIsTagMember,0,ModAttrT_03C0301, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C0301SQReferencedStudySequence[] =  /* Visit Relationship Module Attributes  */
  {
   /* (0008,1150) */ ET_ReferencedSOPClassUID,
   /* (0008,1155) */ ET_ReferencedSOPInstanceUID
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C0301SQReferencedStudySequence[] =  /* Visit Relationship Module Attributes */
  {
   /* (0008,1110) */ ET_ReferencedStudySequence
  };
S_ListDesc1 ModAttrT_Desc_03C0301SQReferencedStudySequence = {E_ListIsTagMemberWithCond,2,ModAttrT_03C0301SQReferencedStudySequence, 0, 0,1,ModAttrT_SQ03C0301SQReferencedStudySequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C0301SQReferencedPatientSequence[] =  /* Visit Relationship Module Attributes  */
  {
   /* (0008,1150) */ ET_ReferencedSOPClassUID,
   /* (0008,1155) */ ET_ReferencedSOPInstanceUID
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C0301SQReferencedPatientSequence[] =  /* Visit Relationship Module Attributes */
  {
   /* (0008,1120) */ ET_ReferencedPatientSequence
  };
S_ListDesc1 ModAttrT_Desc_03C0301SQReferencedPatientSequence = {E_ListIsTagMemberWithCond,2,ModAttrT_03C0301SQReferencedPatientSequence, 0, 0,1,ModAttrT_SQ03C0301SQReferencedPatientSequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C0302[] =   /* Visit Identification Module Attributes */
  {
   /* (0008,0080) */ ET_InstitutionName,
   /* (0008,0081) */ ET_InstitutionAddress,
   /* (0038,0010) */ ET_AdmissionID,
   /* (0038,0011) */ ET_IssuerofAdmissionID
  };
S_ListDesc1 ModAttrT_Desc_03C0302 = {E_ListIsTagMember,4,ModAttrT_03C0302, 0, 0, 0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C0302SQInstitutionCodeSequence[] =  /* Visit Identification Module Attributes */
  {
  /*  */ {ET_InstitutionCodeSequence}
  };
S_ListDesc1 ModAttrT_Desc_03C0302SQInstitutionCodeSequence = {E_ListIsTagMemberWithCond,3,ModAttrT_CodeRefs, 0, 0,1,ModAttrT_03C0302SQInstitutionCodeSequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C0303[] =   /* Visit Status Module Attributes */
  {
   /* (0038,0008) */ ET_VisitStatusID,
   /* (0038,0300) */ ET_CurrentPatientLocation,
   /* (0038,0400) */ ET_PatientsInstitutionResidence,
   /* (0038,4000) */ ET_VisitComments
  };
S_ListDesc1 ModAttrT_Desc_03C0303 = {E_ListIsTagMember,4,ModAttrT_03C0303, 0, 0, 0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C0303V1VisitStatusID[] =   /* Visit Status Module Attributes */
  {
   "ADMITTED",
   "CREATED",
   "DISCHARGED",
   "SCHEDULED"
  };
S_ListDesc1 ModAttrT_Desc_03C0303V1VisitStatusID = {E_ListIsVal1Member,4,ModAttrT_03C0303V1VisitStatusID,ET_VisitStatusID,EType_X,0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C0304[] =   /* Visit Admission Module Attributes */
  {
   /* (0008,0090) */ ET_ReferringPhysiciansName,
   /* (0008,0092) */ ET_ReferringPhysiciansAddress,
   /* (0008,0094) */ ET_ReferringPhysiciansTelephoneNumbers,
   /* (0008,1080) */ ET_AdmittingDiagnosesDescription,
   /* (0038,0016) */ ET_RouteofAdmissions,
   /* (0038,0020) */ ET_AdmittingDate,
   /* (0038,0021) */ ET_AdmittingTime
  };
S_ListDesc1 ModAttrT_Desc_03C0304 = {E_ListIsTagMember,7,ModAttrT_03C0304, 0, 0, 0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C0304SQAdmittingDiagnosisCodeSequence[] =  /* Visit Admission Module Attributes */
  {
  /*  */ {ET_AdmittingDiagnosisCodeSequence}
  };
S_ListDesc1 ModAttrT_Desc_03C0304SQAdmittingDiagnosisCodeSequence = {E_ListIsTagMemberWithCond,3,ModAttrT_CodeRefs, 0, 0,1,ModAttrT_03C0304SQAdmittingDiagnosisCodeSequence};

/* ModAttrTable references */
char *ModAttrT_03C0304V1RouteofAdmissions[] =   /* Visit Admission Module Attributes */
  {
   "emergency",
   "normal"
  };
S_ListDesc1 ModAttrT_Desc_03C0304V1RouteofAdmissions = {E_ListIsVal1Member,2,ModAttrT_03C0304V1RouteofAdmissions,ET_RouteofAdmissions,EType_X,0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C0305[] =   /* Visit Discharge Module Attributes */
  {
   /* (0038,0030) */ ET_DischargeDate,
   /* (0038,0032) */ ET_DischargeTime,
   /* (0038,0040) */ ET_DischargeDiagnosisDescription
  };
S_ListDesc1 ModAttrT_Desc_03C0305 = {E_ListIsTagMember,3,ModAttrT_03C0305, 0, 0, 0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C0305SQDischargeDiagnosisCodeSequence[] =  /* Visit Discharge Module Attributes */
  {
  /*  */ {ET_DischargeDiagnosisCodeSequence}
  };
S_ListDesc1 ModAttrT_Desc_03C0305SQDischargeDiagnosisCodeSequence = {E_ListIsTagMemberWithCond,3,ModAttrT_CodeRefs, 0, 0,1,ModAttrT_03C0305SQDischargeDiagnosisCodeSequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C0306[] =   /* Visit Scheduling Module Attributes */
  {
   /* (0038,001A) */ ET_ScheduledAdmissionDate,
   /* (0038,001B) */ ET_ScheduledAdmissionTime,
   /* (0038,001C) */ ET_ScheduledDischargeDate,
   /* (0038,001D) */ ET_ScheduledDischargeTime,
   /* (0038,001E) */ ET_ScheduledPatientInstitutionResidence
  };
S_ListDesc1 ModAttrT_Desc_03C0306 = {E_ListIsTagMember,5,ModAttrT_03C0306, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C0401[] =   /* Study Relationship Module Attributes */
  {
   /* (0008,0050) */ ET_AccessionNumber,
   /* (0020,000D) */ ET_StudyInstanceUID
  };
S_ListDesc1 ModAttrT_Desc_03C0401 = {E_ListIsTagMember,2,ModAttrT_03C0401, 0, 0, 0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C0401SQReferencedResultsSequence[] =  /* Study Relationship Module Attributes */
  {
  /* (0008,1100) */ {ET_ReferencedResultsSequence,EType_X}
  };
S_ListDesc1 ModAttrT_Desc_03C0401SQReferencedResultsSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_SOPUIDRefs, 0, 0,1,ModAttrT_03C0401SQReferencedResultsSequence};

/* ModAttrTable references */
S_TagType ModAttrT_03C0401SQReferencedStudyComponentSequence[] =  /* Study Relationship Module Attributes */
  {
  /* (0008,1111) */ {ET_ReferencedStudyComponentSequence,EType_X}
  };
S_ListDesc1 ModAttrT_Desc_03C0401SQReferencedStudyComponentSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_SOPUIDRefs, 0, 0,1,ModAttrT_03C0401SQReferencedStudyComponentSequence};

/* ModAttrTable references */
S_TagType ModAttrT_03C0401SQReferencedPatientSequence[] =  /* Study Relationship Module Attributes */
  {
  /* (0008,1120) */ {ET_ReferencedPatientSequence,EType_X}
  };
S_ListDesc1 ModAttrT_Desc_03C0401SQReferencedPatientSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_SOPUIDRefs, 0, 0,1,ModAttrT_03C0401SQReferencedPatientSequence};

/* ModAttrTable references */
S_TagType ModAttrT_03C0401SQReferencedVisitSequence[] =  /* Study Relationship Module Attributes */
  {
  /* (0008,1125) */ {ET_ReferencedVisitSequence,EType_X}
  };
S_ListDesc1 ModAttrT_Desc_03C0401SQReferencedVisitSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_SOPUIDRefs, 0, 0,1,ModAttrT_03C0401SQReferencedVisitSequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C0402[] =   /* Study Identification Module Attributes */
  {
   /* (0020,0010) */ ET_StudyID,
   /* (0020,1070) */ ET_OtherStudyNumbers,
   /* (0032,0012) */ ET_StudyIDIssuer
  };
S_ListDesc1 ModAttrT_Desc_03C0402 = {E_ListIsTagMember,3,ModAttrT_03C0402, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C0403[] =   /* Study Classification Module Attributes */
  {
   /* (0032,000A) */ ET_StudyStatusID,
   /* (0032,000C) */ ET_StudyPriorityID,
   /* (0032,4000) */ ET_StudyComments
  };
S_ListDesc1 ModAttrT_Desc_03C0403 = {E_ListIsTagMember,3,ModAttrT_03C0403, 0, 0, 0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C0403V1StudyStatusID[] =   /* Study Classification Module Attributes */
  {
   "ARRIVED",
   "COMPLETED",
   "CREATED",
   "READ",
   "SCHEDULED",
   "STARTED",
   "VERIFIED"
  };
S_ListDesc1 ModAttrT_Desc_03C0403V1StudyStatusID = {E_ListIsVal1Member,7,ModAttrT_03C0403V1StudyStatusID,ET_StudyStatusID,EType_X,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C0403V1StudyPriorityID[] =   /* Study Classification Module Attributes */
  {
   "HIGH",
   "MED",
   "LOW"
  };
S_ListDesc1 ModAttrT_Desc_03C0403V1StudyPriorityID = {E_ListIsVal1Member,3,ModAttrT_03C0403V1StudyPriorityID,ET_StudyPriorityID,EType_X,0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C0404[] =   /* Study Scheduling Module Attributes */
  {
   /* (0032,1000) */ ET_ScheduledStudyStartDate,
   /* (0032,1001) */ ET_ScheduledStudyStartTime,
   /* (0032,1010) */ ET_ScheduledStudyStopDate,
   /* (0032,1011) */ ET_ScheduledStudyStopTime,
   /* (0032,1020) */ ET_ScheduledStudyLocation,
   /* (0032,1021) */ ET_ScheduledStudyLocationAETitles,
   /* (0032,1030) */ ET_ReasonforStudy,
   /* (0032,1032) */ ET_RequestingPhysician,
   /* (0032,1033) */ ET_RequestingService,
   /* (0032,1060) */ ET_RequestedProcedureDescription,
   /* (0032,1070) */ ET_RequestedContrastAgent
  };
S_ListDesc1 ModAttrT_Desc_03C0404 = {E_ListIsTagMember,11,ModAttrT_03C0404, 0, 0, 0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C0404SQRequestedProcedureCodeSequence[] =  /* Study Scheduling Module Attributes */
  {
  /*  */ {ET_RequestedProcedureCodeSequence}
  };
S_ListDesc1 ModAttrT_Desc_03C0404SQRequestedProcedureCodeSequence = {E_ListIsTagMemberWithCond,3,ModAttrT_CodeRefs, 0, 0,1,ModAttrT_03C0404SQRequestedProcedureCodeSequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C0405[] =   /* Study Acquisition Module Attributes */
  {
   /* (0008,0020) */ ET_StudyDate,
   /* (0008,0030) */ ET_StudyTime,
   /* (0020,1000) */ ET_SeriesinStudy,
   /* (0020,1004) */ ET_AcquisitioninStudy,
   /* (0032,0032) */ ET_StudyVerifiedDate,
   /* (0032,0033) */ ET_StudyVerifiedTime,
   /* (0032,1040) */ ET_StudyArrivalDate,
   /* (0032,1041) */ ET_StudyArrivalTime,
   /* (0032,1050) */ ET_StudyCompletionDate,
   /* (0032,1051) */ ET_StudyCompletionTime
  };
S_ListDesc1 ModAttrT_Desc_03C0405 = {E_ListIsTagMember,10,ModAttrT_03C0405, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C0406[] =   /* Study Read Module Attributes */
  {
   /* (0008,1060) */ ET_NameofPhysiciansReadingStudy,
   /* (0032,0034) */ ET_StudyReadDate,
   /* (0032,0035) */ ET_StudyReadTime
  };
S_ListDesc1 ModAttrT_Desc_03C0406 = {E_ListIsTagMember,3,ModAttrT_03C0406, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C0407[] =   /* Study Component Module Attributes */
  {
   /* (0020,000D) */ ET_StudyInstanceUID,
   /* (0020,0010) */ ET_StudyID
  };
S_ListDesc1 ModAttrT_Desc_03C0407 = {E_ListIsTagMember,2,ModAttrT_03C0407, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C0407SQReferencedSeriesSequence[] =  /* Study Component Module Attributes  */
  {
   /* (0008,0021) */ ET_SeriesDate,
   /* (0008,0031) */ ET_SeriesTime,
   /* (0008,0054) */ ET_RetrieveAETitle,
   /* (0020,000E) */ ET_SeriesInstanceUID,
   /* (0088,0130) */ ET_StorageMediaFilesetID,
   /* (0088,0140) */ ET_StorageMediaFilesetUID
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C0407SQReferencedSeriesSequence[] =  /* Study Component Module Attributes */
  {
   /* (0008,1115) */ ET_ReferencedSeriesSequence
  };
S_ListDesc1 ModAttrT_Desc_03C0407SQReferencedSeriesSequence = {E_ListIsTagMemberWithCond,6,ModAttrT_03C0407SQReferencedSeriesSequence, 0, 0,1,ModAttrT_SQ03C0407SQReferencedSeriesSequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C0407SQSQReferencedImageSequence[] =  /* Study Component Module Attributes  */
  {
   /* (0008,0054) */ ET_RetrieveAETitle,
   /* (0008,1150) */ ET_ReferencedSOPClassUID,
   /* (0008,1155) */ ET_ReferencedSOPInstanceUID,
   /* (0088,0130) */ ET_StorageMediaFilesetID,
   /* (0088,0140) */ ET_StorageMediaFilesetUID
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQSQ03C0407SQSQReferencedImageSequence[] =  /* Study Component Module Attributes */
  {
   ET_ReferencedSeriesSequence,
   ET_ReferencedImageSequence
  };
S_ListDesc1 ModAttrT_Desc_03C0407SQSQReferencedImageSequence = {E_ListIsTagMemberWithCond,5,ModAttrT_03C0407SQSQReferencedImageSequence, 0, 0,2,ModAttrT_SQSQ03C0407SQSQReferencedImageSequence};

/* ModAttrTable references */
S_TagType ModAttrT_03C0408[] =   /* Study Component Relationship Module Attributes */
  {
   {ET_UNKNOWN,EType_UNKNOWN}
  };
S_ListDesc1 ModAttrT_Desc_03C0408 = {E_ListIsTagMember,0,ModAttrT_03C0408, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C0408SQReferencedStudySequence[] =  /* Study Component Relationship Module Attributes  */
  {
   /* (0008,1150) */ ET_ReferencedSOPClassUID,
   /* (0008,1155) */ ET_ReferencedSOPInstanceUID
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C0408SQReferencedStudySequence[] =  /* Study Component Relationship Module Attributes */
  {
   /* (0008,1110) */ ET_ReferencedStudySequence
  };
S_ListDesc1 ModAttrT_Desc_03C0408SQReferencedStudySequence = {E_ListIsTagMemberWithCond,2,ModAttrT_03C0408SQReferencedStudySequence, 0, 0,1,ModAttrT_SQ03C0408SQReferencedStudySequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C0409[] =   /* Study Component Acquisition  Module Attributes */
  {
   /* (0008,0060) */ ET_Modality,
   /* (0008,1030) */ ET_StudyDescription,
   /* (0008,1050) */ ET_PerformingPhysiciansName,
   /* (0032,1055) */ ET_StudyComponentStatusID
  };
S_ListDesc1 ModAttrT_Desc_03C0409 = {E_ListIsTagMember,4,ModAttrT_03C0409, 0, 0, 0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C0409SQProcedureCodeSequence[] =  /* Study Component Acquisition  Module Attributes */
  {
  /*  */ {ET_ProcedureCodeSequence}
  };
S_ListDesc1 ModAttrT_Desc_03C0409SQProcedureCodeSequence = {E_ListIsTagMemberWithCond,3,ModAttrT_CodeRefs, 0, 0,1,ModAttrT_03C0409SQProcedureCodeSequence};

/* ModAttrTable references */
char *ModAttrT_03C0409V1StudyComponentStatusID[] =   /* Study Component Acquisition  Module Attributes */
  {
   "COMPLETED",
   "CREATED",
   "INCOMPLETE",
   "POSTINTERPRET",
   "VERIFIED"
  };
S_ListDesc1 ModAttrT_Desc_03C0409V1StudyComponentStatusID = {E_ListIsVal1Member,5,ModAttrT_03C0409V1StudyComponentStatusID,ET_StudyComponentStatusID,EType_X,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C0501[] =   /* Results Relationship Module Attributes */
  {
   {ET_UNKNOWN,EType_UNKNOWN}
  };
S_ListDesc1 ModAttrT_Desc_03C0501 = {E_ListIsTagMember,0,ModAttrT_03C0501, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C0501SQReferencedStudySequence[] =  /* Results Relationship Module Attributes  */
  {
   /* (0008,1150) */ ET_ReferencedSOPClassUID,
   /* (0008,1155) */ ET_ReferencedSOPInstanceUID
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C0501SQReferencedStudySequence[] =  /* Results Relationship Module Attributes */
  {
   /* (0008,1110) */ ET_ReferencedStudySequence
  };
S_ListDesc1 ModAttrT_Desc_03C0501SQReferencedStudySequence = {E_ListIsTagMemberWithCond,2,ModAttrT_03C0501SQReferencedStudySequence, 0, 0,1,ModAttrT_SQ03C0501SQReferencedStudySequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C0501SQReferencedInterpretationSequence[] =  /* Results Relationship Module Attributes  */
  {
   /* (0008,1150) */ ET_ReferencedSOPClassUID,
   /* (0008,1155) */ ET_ReferencedSOPInstanceUID
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C0501SQReferencedInterpretationSequence[] =  /* Results Relationship Module Attributes */
  {
   /* (4008,0050) */ ET_ReferencedInterpretationSequence
  };
S_ListDesc1 ModAttrT_Desc_03C0501SQReferencedInterpretationSequence = {E_ListIsTagMemberWithCond,2,ModAttrT_03C0501SQReferencedInterpretationSequence, 0, 0,1,ModAttrT_SQ03C0501SQReferencedInterpretationSequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C0502[] =   /* Results Identification Module Attributes */
  {
   /* (4008,0040) */ ET_ResultsID,
   /* (4008,0042) */ ET_ResultsIDIssuer
  };
S_ListDesc1 ModAttrT_Desc_03C0502 = {E_ListIsTagMember,2,ModAttrT_03C0502, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C0503[] =   /* Results Impression Module Attributes */
  {
   /* (4008,0300) */ ET_Impressions,
   /* (4008,4000) */ ET_ResultsComments
  };
S_ListDesc1 ModAttrT_Desc_03C0503 = {E_ListIsTagMember,2,ModAttrT_03C0503, 0, 0, 0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C0601[] =   /* Interpretation Relationship Module Attributes */
  {
   {ET_UNKNOWN,EType_UNKNOWN}
  };
S_ListDesc1 ModAttrT_Desc_03C0601 = {E_ListIsTagMember,0,ModAttrT_03C0601, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C0601SQReferencedResultsSequence[] =  /* Interpretation Relationship Module Attributes  */
  {
   /* (0008,1150) */ ET_ReferencedSOPClassUID,
   /* (0008,1155) */ ET_ReferencedSOPInstanceUID
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C0601SQReferencedResultsSequence[] =  /* Interpretation Relationship Module Attributes */
  {
   /* (0008,1100) */ ET_ReferencedResultsSequence
  };
S_ListDesc1 ModAttrT_Desc_03C0601SQReferencedResultsSequence = {E_ListIsTagMemberWithCond,2,ModAttrT_03C0601SQReferencedResultsSequence, 0, 0,1,ModAttrT_SQ03C0601SQReferencedResultsSequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C0602[] =   /* Interpretation Identification Module Attributes */
  {
   /* (4008,0200) */ ET_InterpretationID,
   /* (4008,0202) */ ET_InterpretationIDIssuer
  };
S_ListDesc1 ModAttrT_Desc_03C0602 = {E_ListIsTagMember,2,ModAttrT_03C0602, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C0603[] =   /* Interpretation State Module Attributes */
  {
   /* (4008,0210) */ ET_InterpretationTypeID,
   /* (4008,0212) */ ET_InterpretationStatusID
  };
S_ListDesc1 ModAttrT_Desc_03C0603 = {E_ListIsTagMember,2,ModAttrT_03C0603, 0, 0, 0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C0603V1InterpretationTypeID[] =   /* Interpretation State Module Attributes */
  {
   "REPORT",
   "AMENDMENT"
  };
S_ListDesc1 ModAttrT_Desc_03C0603V1InterpretationTypeID = {E_ListIsVal1Member,2,ModAttrT_03C0603V1InterpretationTypeID,ET_InterpretationTypeID,EType_X,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C0603V1InterpretationStatusID[] =   /* Interpretation State Module Attributes */
  {
   "CREATED",
   "RECORDED",
   "TRANSCRIBED",
   "APPROVED"
  };
S_ListDesc1 ModAttrT_Desc_03C0603V1InterpretationStatusID = {E_ListIsVal1Member,4,ModAttrT_03C0603V1InterpretationStatusID,ET_InterpretationStatusID,EType_X,0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C0604[] =   /* Interpretation Recording Module Attributes */
  {
   /* (4008,0100) */ ET_InterpretationRecordedDate,
   /* (4008,0101) */ ET_InterpretationRecordedTime,
   /* (4008,0102) */ ET_InterpretationRecorder,
   /* (4008,0103) */ ET_ReferencetoRecordedSound
  };
S_ListDesc1 ModAttrT_Desc_03C0604 = {E_ListIsTagMember,4,ModAttrT_03C0604, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C0605[] =   /* Interpretation Transcription Module Attributes */
  {
   /* (4008,0108) */ ET_InterpretationTranscriptionDate,
   /* (4008,0109) */ ET_InterpretationTranscriptionTime,
   /* (4008,010A) */ ET_InterpretationTranscriber,
   /* (4008,010B) */ ET_InterpretationText,
   /* (4008,010C) */ ET_InterpretationAuthor
  };
S_ListDesc1 ModAttrT_Desc_03C0605 = {E_ListIsTagMember,5,ModAttrT_03C0605, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C0606[] =   /* Interpretation Approval Module Attributes */
  {
   /* (4008,0115) */ ET_InterpretationDiagnosisDescription
  };
S_ListDesc1 ModAttrT_Desc_03C0606 = {E_ListIsTagMember,1,ModAttrT_03C0606, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C0606SQInterpretationApproverSequence[] =  /* Interpretation Approval Module Attributes  */
  {
   /* (4008,0112) */ ET_InterpretationApprovalDate,
   /* (4008,0113) */ ET_InterpretationApprovalTime,
   /* (4008,0114) */ ET_PhysicianApprovingInterpretation
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C0606SQInterpretationApproverSequence[] =  /* Interpretation Approval Module Attributes */
  {
   /* (4008,0111) */ ET_InterpretationApproverSequence
  };
S_ListDesc1 ModAttrT_Desc_03C0606SQInterpretationApproverSequence = {E_ListIsTagMemberWithCond,3,ModAttrT_03C0606SQInterpretationApproverSequence, 0, 0,1,ModAttrT_SQ03C0606SQInterpretationApproverSequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C0606SQResultsDistributionListSequence[] =  /* Interpretation Approval Module Attributes  */
  {
   /* (4008,0119) */ ET_DistributionName,
   /* (4008,011A) */ ET_DistributionAddress
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C0606SQResultsDistributionListSequence[] =  /* Interpretation Approval Module Attributes */
  {
   /* (4008,0118) */ ET_ResultsDistributionListSequence
  };
S_ListDesc1 ModAttrT_Desc_03C0606SQResultsDistributionListSequence = {E_ListIsTagMemberWithCond,2,ModAttrT_03C0606SQResultsDistributionListSequence, 0, 0,1,ModAttrT_SQ03C0606SQResultsDistributionListSequence};

/* ModAttrTable references */
S_TagType ModAttrT_03C0606SQDiagnosisCodeSequence[] =  /* Interpretation Approval Module Attributes */
  {
  /* (4008,0118) */ {ET_DiagnosisCodeSequence}
  };
S_ListDesc1 ModAttrT_Desc_03C0606SQDiagnosisCodeSequence = {E_ListIsTagMemberWithCond,3,ModAttrT_CodeRefs, 0, 0,1,ModAttrT_03C0606SQDiagnosisCodeSequence};

/* ModAttrTable references */
char *ModAttrT_03C0606V1_4008_0117CodingSchemeDesignator[] =   /* Interpretation Approval Module Attributes */
  {
   "ACR",
   "CE",
   "E",
   "19",
   "19C",
   "I10",
   "99zz",
   "LB",
   "RC",
   "SNM",
   "S3",
   "UML"
  };
S_ListDesc1 ModAttrT_Desc_03C0606V1_4008_0117CodingSchemeDesignator = {E_ListIsVal1MemberAndCond,12,ModAttrT_03C0606V1_4008_0117CodingSchemeDesignator,ET_CodingSchemeDesignator,EType_X,0,&ModAttrT_Desc_03C0606SQDiagnosisCodeSequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C1301[] =   /* Basic Film Session Presentation Module */
  {
   /* (2000,0010) */ ET_NumberOfCopies,
   /* (2000,0020) */ ET_PrintPriority,
   /* (2000,0030) */ ET_MediumType,
   /* (2000,0040) */ ET_FilmDestination,
   /* (2000,0050) */ ET_FilmSessionLabel,
   /* (2000,0060) */ ET_MemoryAllocation
  };
S_ListDesc1 ModAttrT_Desc_03C1301 = {E_ListIsTagMember,6,ModAttrT_03C1301, 0, 0, 0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C1301V1PrintPriority[] =   /* Basic Film Session Presentation Module */
  {
   "HIGH",
   "LOW",
   "MED"
  };
S_ListDesc1 ModAttrT_Desc_03C1301V1PrintPriority = {E_ListIsVal1Member,3,ModAttrT_03C1301V1PrintPriority,ET_PrintPriority,EType_X,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C1301V1MediumType[] =   /* Basic Film Session Presentation Module */
  {
   "PAPER",
   "CLEAR FILM",
   "BLUE FILM"
  };
S_ListDesc1 ModAttrT_Desc_03C1301V1MediumType = {E_ListIsVal1Member,3,ModAttrT_03C1301V1MediumType,ET_MediumType,EType_X,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C1301V1FilmDestination[] =   /* Basic Film Session Presentation Module */
  {
   "MAGAZINE",
   "PROCESSOR"
  };
S_ListDesc1 ModAttrT_Desc_03C1301V1FilmDestination = {E_ListIsVal1Member,2,ModAttrT_03C1301V1FilmDestination,ET_FilmDestination,EType_X,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C1302[] =   /* Basic Film Session Relationship Module */
  {
   {ET_UNKNOWN,EType_UNKNOWN}
  };
S_ListDesc1 ModAttrT_Desc_03C1302 = {E_ListIsTagMember,0,ModAttrT_03C1302, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C1302SQReferencedFilmBoxSequence[] =  /* Basic Film Session Relationship Module  */
  {
   /* (0008,1150) */ ET_ReferencedSOPClassUID,
   /* (0008,1155) */ ET_ReferencedSOPInstanceUID
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C1302SQReferencedFilmBoxSequence[] =  /* Basic Film Session Relationship Module */
  {
   /* (2000,0500) */ ET_ReferencedFilmBoxSequence
  };
S_ListDesc1 ModAttrT_Desc_03C1302SQReferencedFilmBoxSequence = {E_ListIsTagMemberWithCond,2,ModAttrT_03C1302SQReferencedFilmBoxSequence, 0, 0,1,ModAttrT_SQ03C1302SQReferencedFilmBoxSequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C1303[] =   /* Basic Film Box Presentation Module */
  {
   /* (2010,0010) */ ET_ImageDisplayFormat,
   /* (2010,0030) */ ET_AnnotationDisplayFormatID,
   /* (2010,0040) */ ET_FilmOrientation,
   /* (2010,0050) */ ET_FilmSizeID,
   /* (2010,0060) */ ET_MagnificationType,
   /* (2010,0080) */ ET_SmoothingType,
   /* (2010,0100) */ ET_BorderDensity,
   /* (2010,0110) */ ET_EmptyImageDensity,
   /* (2010,0120) */ ET_MinDensity,
   /* (2010,0130) */ ET_MaxDensity,
   /* (2010,0140) */ ET_Trim,
   /* (2010,0150) */ ET_ConfigurationInformation
  };
S_ListDesc1 ModAttrT_Desc_03C1303 = {E_ListIsTagMember,12,ModAttrT_03C1303, 0, 0, 0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C1303V1FilmOrientation[] =   /* Basic Film Box Presentation Module */
  {
   "PORTRAIT",
   "LANDSCAPE"
  };
S_ListDesc1 ModAttrT_Desc_03C1303V1FilmOrientation = {E_ListIsVal1Member,2,ModAttrT_03C1303V1FilmOrientation,ET_FilmOrientation,EType_X,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C1303V1FilmSizeID[] =   /* Basic Film Box Presentation Module */
  {
   "8INX10IN",
   "10INX14IN",
   "14INX14IN",
   "24CMX24CM",
   "10INX12IN",
   "11INX14IN",
   "14INX17IN",
   "24CMX30CM"
  };
S_ListDesc1 ModAttrT_Desc_03C1303V1FilmSizeID = {E_ListIsVal1Member,8,ModAttrT_03C1303V1FilmSizeID,ET_FilmSizeID,EType_X,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C1303V1MagnificationType[] =   /* Basic Film Box Presentation Module */
  {
   "REPLICATE",
   "BILINEAR",
   "CUBIC",
   "NONE"
  };
S_ListDesc1 ModAttrT_Desc_03C1303V1MagnificationType = {E_ListIsVal1Member,4,ModAttrT_03C1303V1MagnificationType,ET_MagnificationType,EType_X,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C1303V1BorderDensity[] =   /* Basic Film Box Presentation Module */
  {
   "BLACK",
   "WHITE"
  };
S_ListDesc1 ModAttrT_Desc_03C1303V1BorderDensity = {E_ListIsVal1Member,2,ModAttrT_03C1303V1BorderDensity,ET_BorderDensity,EType_X,0,NULL};

S_ListDesc1 ModAttrT_Desc_03C1303V1EmptyImageDensity = {E_ListIsVal1Member,2,ModAttrT_03C1303V1BorderDensity,ET_EmptyImageDensity,EType_X,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C1303V1Trim[] =   /* Basic Film Box Presentation Module */
  {
   "YES",
   "NO"
  };
S_ListDesc1 ModAttrT_Desc_03C1303V1Trim = {E_ListIsVal1Member,2,ModAttrT_03C1303V1Trim,ET_Trim,EType_X,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C1304[] =   /* Basic Film Box Relationship Module */
  {
   {ET_UNKNOWN,EType_UNKNOWN}
  };
S_ListDesc1 ModAttrT_Desc_03C1304 = {E_ListIsTagMember,0,ModAttrT_03C1304, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C1304SQReferencedFilmSessionSequence[] =  /* Basic Film Box Relationship Module  */
  {
   /* (0008,1150) */ ET_ReferencedSOPClassUID,
   /* (0008,1155) */ ET_ReferencedSOPInstanceUID
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C1304SQReferencedFilmSessionSequence[] =  /* Basic Film Box Relationship Module */
  {
   /* (2010,0500) */ ET_ReferencedFilmSessionSequence
  };
S_ListDesc1 ModAttrT_Desc_03C1304SQReferencedFilmSessionSequence = {E_ListIsTagMemberWithCond,2,ModAttrT_03C1304SQReferencedFilmSessionSequence, 0, 0,1,ModAttrT_SQ03C1304SQReferencedFilmSessionSequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C1304SQReferencedImageBoxSequence2010[] =  /* Basic Film Box Relationship Module  */
  {
   /* (0008,1150) */ ET_ReferencedSOPClassUID,
   /* (0008,1155) */ ET_ReferencedSOPInstanceUID
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C1304SQReferencedImageBoxSequence2010[] =  /* Basic Film Box Relationship Module */
  {
   /* (2010,0510) */ ET_ReferencedImageBoxSequence2010
  };
S_ListDesc1 ModAttrT_Desc_03C1304SQReferencedImageBoxSequence2010 = {E_ListIsTagMemberWithCond,2,ModAttrT_03C1304SQReferencedImageBoxSequence2010, 0, 0,1,ModAttrT_SQ03C1304SQReferencedImageBoxSequence2010};

/* ModAttrTable references */
S_Tag ModAttrT_03C1304SQReferencedBasicAnnotationBoxSequence[] =  /* Basic Film Box Relationship Module  */
  {
   /* (0008,1150) */ ET_ReferencedSOPClassUID,
   /* (0008,1155) */ ET_ReferencedSOPInstanceUID
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C1304SQReferencedBasicAnnotationBoxSequence[] =  /* Basic Film Box Relationship Module */
  {
   /* (2010,0520) */ ET_ReferencedBasicAnnotationBoxSequence
  };
S_ListDesc1 ModAttrT_Desc_03C1304SQReferencedBasicAnnotationBoxSequence = {E_ListIsTagMemberWithCond,2,ModAttrT_03C1304SQReferencedBasicAnnotationBoxSequence, 0, 0,1,ModAttrT_SQ03C1304SQReferencedBasicAnnotationBoxSequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C1305[] =   /* Image Box Pixel Presentation Module */
  {
   /* (2010,0060) */ ET_MagnificationType,
   /* (2010,0080) */ ET_SmoothingType,
   /* (2020,0010) */ ET_ImagePosition2020,
   /* (2020,0020) */ ET_Polarity,
   /* (2020,0030) */ ET_RequestedImageSize
  };
S_ListDesc1 ModAttrT_Desc_03C1305 = {E_ListIsTagMember,5,ModAttrT_03C1305, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C1305SQPreformattedGrayscaleImageSequence[] =  /* Image Box Pixel Presentation Module  */
  {
   /* (0028,0002) */ ET_SamplesperPixel,
   /* (0028,0004) */ ET_PhotometricInterpretation,
   /* (0028,0010) */ ET_Rows0028,
   /* (0028,0011) */ ET_Columns0028,
   /* (0028,0034) */ ET_PixelAspectRatio,
   /* (0028,0100) */ ET_BitsAllocated0028,
   /* (0028,0101) */ ET_BitsStored,
   /* (0028,0102) */ ET_HighBit,
   /* (0028,0103) */ ET_PixelRepresentation,
   /* (7FE0,0010) */ ET_PixelData
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C1305SQPreformattedGrayscaleImageSequence[] =  /* Image Box Pixel Presentation Module */
  {
   /* (2020,0110) */ ET_PreformattedGrayscaleImageSequence
  };
S_ListDesc1 ModAttrT_Desc_03C1305SQPreformattedGrayscaleImageSequence = {E_ListIsTagMemberWithCond,10,ModAttrT_03C1305SQPreformattedGrayscaleImageSequence, 0, 0,1,ModAttrT_SQ03C1305SQPreformattedGrayscaleImageSequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C1305SQPreformattedColorImageSequence[] =  /* Image Box Pixel Presentation Module  */
  {
   /* (0028,0002) */ ET_SamplesperPixel,
   /* (0028,0004) */ ET_PhotometricInterpretation,
   /* (0028,0006) */ ET_PlanarConfiguration,
   /* (0028,0010) */ ET_Rows0028,
   /* (0028,0011) */ ET_Columns0028,
   /* (0028,0034) */ ET_PixelAspectRatio,
   /* (0028,0100) */ ET_BitsAllocated0028,
   /* (0028,0101) */ ET_BitsStored,
   /* (0028,0102) */ ET_HighBit,
   /* (0028,0103) */ ET_PixelRepresentation,
   /* (7FE0,0010) */ ET_PixelData
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C1305SQPreformattedColorImageSequence[] =  /* Image Box Pixel Presentation Module */
  {
   /* (2020,0111) */ ET_PreformattedColorImageSequence
  };
S_ListDesc1 ModAttrT_Desc_03C1305SQPreformattedColorImageSequence = {E_ListIsTagMemberWithCond,11,ModAttrT_03C1305SQPreformattedColorImageSequence, 0, 0,1,ModAttrT_SQ03C1305SQPreformattedColorImageSequence};

S_ListDesc1 ModAttrT_Desc_03C1305V1_2020_0110SamplesperPixel = {E_ListIsVal1MemberAndCond,1,ModAttrT_03C08021V1SamplesperPixel,ET_SamplesperPixel,EType_X,0,&ModAttrT_Desc_03C1305SQPreformattedGrayscaleImageSequence};

/* ModAttrTable references */
char *ModAttrT_03C1305V1_2020_0111SamplesperPixel[] =   /* Image Box Pixel Presentation Module */
  {
   "3"
  };
S_ListDesc1 ModAttrT_Desc_03C1305V1_2020_0111SamplesperPixel = {E_ListIsVal1MemberAndCond,1,ModAttrT_03C1305V1_2020_0111SamplesperPixel,ET_SamplesperPixel,EType_X,0,&ModAttrT_Desc_03C1305SQPreformattedColorImageSequence};

S_ListDesc1 ModAttrT_Desc_03C1305V1_2020_0110PhotometricInterpretation = {E_ListIsVal1MemberAndCond,2,ModAttrT_03C08021V1PhotometricInterpretation,ET_PhotometricInterpretation,EType_X,0,&ModAttrT_Desc_03C1305SQPreformattedGrayscaleImageSequence};

/* ModAttrTable references */
char *ModAttrT_03C1305V1_2020_0111PhotometricInterpretation[] =   /* Image Box Pixel Presentation Module */
  {
   "RGB"
  };
S_ListDesc1 ModAttrT_Desc_03C1305V1_2020_0111PhotometricInterpretation = {E_ListIsVal1MemberAndCond,1,ModAttrT_03C1305V1_2020_0111PhotometricInterpretation,ET_PhotometricInterpretation,EType_X,0,&ModAttrT_Desc_03C1305SQPreformattedColorImageSequence};

/* ModAttrTable references */
char *ModAttrT_03C1305V1_2020_0110BitsAllocated0028[] =   /* Image Box Pixel Presentation Module */
  {
   "8",
   "16"
  };
S_ListDesc1 ModAttrT_Desc_03C1305V1_2020_0110BitsAllocated0028 = {E_ListIsVal1MemberAndCond,2,ModAttrT_03C1305V1_2020_0110BitsAllocated0028,ET_BitsAllocated0028,EType_X,0,&ModAttrT_Desc_03C1305SQPreformattedGrayscaleImageSequence};

/* ModAttrTable references */
char *ModAttrT_03C1305V1_2020_0111BitsAllocated0028[] =   /* Image Box Pixel Presentation Module */
  {
   "8"
  };
S_ListDesc1 ModAttrT_Desc_03C1305V1_2020_0111BitsAllocated0028 = {E_ListIsVal1MemberAndCond,1,ModAttrT_03C1305V1_2020_0111BitsAllocated0028,ET_BitsAllocated0028,EType_X,0,&ModAttrT_Desc_03C1305SQPreformattedColorImageSequence};

/* ModAttrTable references */
char *ModAttrT_03C1305V1_2020_0110BitsStored[] =   /* Image Box Pixel Presentation Module */
  {
   "8",
   "12"
  };
S_ListDesc1 ModAttrT_Desc_03C1305V1_2020_0110BitsStored = {E_ListIsVal1MemberAndCond,2,ModAttrT_03C1305V1_2020_0110BitsStored,ET_BitsStored,EType_X,0,&ModAttrT_Desc_03C1305SQPreformattedGrayscaleImageSequence};

S_ListDesc1 ModAttrT_Desc_03C1305V1_2020_0111BitsStored = {E_ListIsVal1MemberAndCond,1,ModAttrT_03C1305V1_2020_0111BitsAllocated0028,ET_BitsStored,EType_X,0,&ModAttrT_Desc_03C1305SQPreformattedColorImageSequence};

/* ModAttrTable references */
char *ModAttrT_03C1305V1_2020_0110HighBit[] =   /* Image Box Pixel Presentation Module */
  {
   "7",
   "11"
  };
S_ListDesc1 ModAttrT_Desc_03C1305V1_2020_0110HighBit = {E_ListIsVal1MemberAndCond,2,ModAttrT_03C1305V1_2020_0110HighBit,ET_HighBit,EType_X,0,&ModAttrT_Desc_03C1305SQPreformattedGrayscaleImageSequence};

/* ModAttrTable references */
char *ModAttrT_03C1305V1_2020_0111HighBit[] =   /* Image Box Pixel Presentation Module */
  {
   "7"
  };
S_ListDesc1 ModAttrT_Desc_03C1305V1_2020_0111HighBit = {E_ListIsVal1MemberAndCond,1,ModAttrT_03C1305V1_2020_0111HighBit,ET_HighBit,EType_X,0,&ModAttrT_Desc_03C1305SQPreformattedColorImageSequence};

/* ModAttrTable references */
char *ModAttrT_03C1305V1_2020_0110PixelRepresentation[] =   /* Image Box Pixel Presentation Module */
  {
   "0000H"
  };
S_ListDesc1 ModAttrT_Desc_03C1305V1_2020_0110PixelRepresentation = {E_ListIsVal1MemberAndCond,1,ModAttrT_03C1305V1_2020_0110PixelRepresentation,ET_PixelRepresentation,EType_X,0,&ModAttrT_Desc_03C1305SQPreformattedGrayscaleImageSequence};

S_ListDesc1 ModAttrT_Desc_03C1305V1_2020_0111PixelRepresentation = {E_ListIsVal1MemberAndCond,1,ModAttrT_03C1305V1_2020_0110PixelRepresentation,ET_PixelRepresentation,EType_X,0,&ModAttrT_Desc_03C1305SQPreformattedColorImageSequence};

S_ListDesc1 ModAttrT_Desc_03C1305V1MagnificationType = {E_ListIsVal1Member,4,ModAttrT_03C1303V1MagnificationType,ET_MagnificationType,EType_X,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C1305V1Polarity[] =   /* Image Box Pixel Presentation Module */
  {
   "NORMAL",
   "REVERSE"
  };
S_ListDesc1 ModAttrT_Desc_03C1305V1Polarity = {E_ListIsVal1Member,2,ModAttrT_03C1305V1Polarity,ET_Polarity,EType_X,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C1306[] =   /* Image Box Relationship Module */
  {
   {ET_UNKNOWN,EType_UNKNOWN}
  };
S_ListDesc1 ModAttrT_Desc_03C1306 = {E_ListIsTagMember,0,ModAttrT_03C1306, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C1306SQReferencedImageSequence[] =  /* Image Box Relationship Module  */
  {
   /* (0008,1150) */ ET_ReferencedSOPClassUID,
   /* (0008,1155) */ ET_ReferencedSOPInstanceUID,
   /* (0008,1160) */ ET_ReferencedFrameNumber
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C1306SQReferencedImageSequence[] =  /* Image Box Relationship Module */
  {
   /* (0008,1140) */ ET_ReferencedImageSequence
  };
S_ListDesc1 ModAttrT_Desc_03C1306SQReferencedImageSequence = {E_ListIsTagMemberWithCond,3,ModAttrT_03C1306SQReferencedImageSequence, 0, 0,1,ModAttrT_SQ03C1306SQReferencedImageSequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C1306SQReferencedImageOverlayBoxSequence[] =  /* Image Box Relationship Module  */
  {
   /* (0008,1150) */ ET_ReferencedSOPClassUID,
   /* (0008,1155) */ ET_ReferencedSOPInstanceUID,
   /* (0008,1160) */ ET_ReferencedFrameNumber
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C1306SQReferencedImageOverlayBoxSequence[] =  /* Image Box Relationship Module */
  {
   /* (2020,0130) */ ET_ReferencedImageOverlayBoxSequence
  };
S_ListDesc1 ModAttrT_Desc_03C1306SQReferencedImageOverlayBoxSequence = {E_ListIsTagMemberWithCond,3,ModAttrT_03C1306SQReferencedImageOverlayBoxSequence, 0, 0,1,ModAttrT_SQ03C1306SQReferencedImageOverlayBoxSequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C1306SQReferencedVOILUTBoxSequence[] =  /* Image Box Relationship Module  */
  {
   /* (0008,1150) */ ET_ReferencedSOPClassUID,
   /* (0008,1155) */ ET_ReferencedSOPInstanceUID
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C1306SQReferencedVOILUTBoxSequence[] =  /* Image Box Relationship Module */
  {
   /* (2020,0140) */ ET_ReferencedVOILUTBoxSequence
  };
S_ListDesc1 ModAttrT_Desc_03C1306SQReferencedVOILUTBoxSequence = {E_ListIsTagMemberWithCond,2,ModAttrT_03C1306SQReferencedVOILUTBoxSequence, 0, 0,1,ModAttrT_SQ03C1306SQReferencedVOILUTBoxSequence};

/* ModAttrTable references */
S_Tag ModAttrT_03C1307[] =   /* Basic Annotation Presentation Module */
  {
   /* (2030,0010) */ ET_AnnotationPosition,
   /* (2030,0020) */ ET_TextString
  };
S_ListDesc1 ModAttrT_Desc_03C1307 = {E_ListIsTagMember,2,ModAttrT_03C1307, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C1308[] =   /* Print Job Module */
  {
   /* (2000,0020) */ ET_PrintPriority,
   /* (2100,0020) */ ET_ExecutionStatus,
   /* (2100,0030) */ ET_ExecutionStatusInfo,
   /* (2100,0040) */ ET_CreationDate,
   /* (2100,0050) */ ET_CreationTime,
   /* (2100,0070) */ ET_Originator,
   /* (2110,0030) */ ET_PrinterName
  };
S_ListDesc1 ModAttrT_Desc_03C1308 = {E_ListIsTagMember,7,ModAttrT_03C1308, 0, 0, 0,NULL};

S_ListDesc1 ModAttrT_Desc_03C1308V1PrintPriority = {E_ListIsVal1Member,3,ModAttrT_03C1301V1PrintPriority,ET_PrintPriority,EType_X,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C1308V1ExecutionStatus[] =   /* Print Job Module */
  {
   "PENDING",
   "PRINTING",
   "DONE",
   "FAILURE"
  };
S_ListDesc1 ModAttrT_Desc_03C1308V1ExecutionStatus = {E_ListIsVal1Member,4,ModAttrT_03C1308V1ExecutionStatus,ET_ExecutionStatus,EType_X,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C1308V1ExecutionStatusInfo[] =   /* Print Job Module */
  {
   "QUEUED",
   "NO SUPPLY MGZ",
   "NO RECEIVE MGZ",
   "SUPPLY EMPTY",
   "RECEIVER FULL",
   "FILM JAM"
  };
S_ListDesc1 ModAttrT_Desc_03C1308V1ExecutionStatusInfo = {E_ListIsVal1Member,6,ModAttrT_03C1308V1ExecutionStatusInfo,ET_ExecutionStatusInfo,EType_X,0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C1309[] =   /* Printer Module */
  {
   /* (0008,0070) */ ET_Manufacturer,
   /* (0008,1090) */ ET_ManufacturersModelName,
   /* (0018,1000) */ ET_DeviceSerialNumber,
   /* (0018,1020) */ ET_SoftwareVersions,
   /* (0018,1200) */ ET_DateofLastCalibration,
   /* (0018,1201) */ ET_TimeofLastCalibration,
   /* (2110,0010) */ ET_PrinterStatus,
   /* (2110,0020) */ ET_PrinterStatusInfo,
   /* (2110,0030) */ ET_PrinterName
  };
S_ListDesc1 ModAttrT_Desc_03C1309 = {E_ListIsTagMember,9,ModAttrT_03C1309, 0, 0, 0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C1309V1PrinterStatus[] =   /* Printer Module */
  {
   "NORMAL",
   "WARNING",
   "FAILURE"
  };
S_ListDesc1 ModAttrT_Desc_03C1309V1PrinterStatus = {E_ListIsVal1Member,3,ModAttrT_03C1309V1PrinterStatus,ET_PrinterStatus,EType_X,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C1309V1PrinterStatusInfo[] =   /* Printer Module */
  {
   "SUPPLY EMPTY",
   "SUPPLY LOW",
   "RECEIVER FULL",
   "FILM JAM"
  };
S_ListDesc1 ModAttrT_Desc_03C1309V1PrinterStatusInfo = {E_ListIsVal1Member,4,ModAttrT_03C1309V1PrinterStatusInfo,ET_PrinterStatusInfo,EType_X,0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C1310[] =   /* Image Overlay Box Presentation Module */
  {
   /* (2040,0060) */ ET_OverlayMagnificationType,
   /* (2040,0070) */ ET_OverlaySmoothingType,
   /* (2040,0080) */ ET_OverlayForegroundDensity,
   /* (2040,0090) */ ET_OverlayMode,
   /* (2040,0100) */ ET_ThresholdDensity
  };
S_ListDesc1 ModAttrT_Desc_03C1310 = {E_ListIsTagMember,5,ModAttrT_03C1310, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C1310SQReferencedOverlayPlaneSequence[] =  /* Image Overlay Box Presentation Module  */
  {
   /* (0008,1150) */ ET_ReferencedSOPClassUID,
   /* (0008,1155) */ ET_ReferencedSOPInstanceUID,
   /* (2040,0011) */ ET_ReferencedOverlayPlaneGroups
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C1310SQReferencedOverlayPlaneSequence[] =  /* Image Overlay Box Presentation Module */
  {
   /* (2040,0010) */ ET_ReferencedOverlayPlaneSequence
  };
S_ListDesc1 ModAttrT_Desc_03C1310SQReferencedOverlayPlaneSequence = {E_ListIsTagMemberWithCond,3,ModAttrT_03C1310SQReferencedOverlayPlaneSequence, 0, 0,1,ModAttrT_SQ03C1310SQReferencedOverlayPlaneSequence};

S_ListDesc1 ModAttrT_Desc_03C1310V1OverlayMagnificationType = {E_ListIsVal1Member,4,ModAttrT_03C1303V1MagnificationType,ET_OverlayMagnificationType,EType_X,0,NULL};

S_ListDesc1 ModAttrT_Desc_03C1310V1OverlayForegroundDensity = {E_ListIsVal1Member,2,ModAttrT_03C1303V1BorderDensity,ET_OverlayForegroundDensity,EType_X,0,NULL};

/* ModAttrTable references */
char *ModAttrT_03C1310V1OverlayMode[] =   /* Image Overlay Box Presentation Module */
  {
   "REPLACE",
   "THRESHOLDREPLACE",
   "COMPLEMENT"
  };
S_ListDesc1 ModAttrT_Desc_03C1310V1OverlayMode = {E_ListIsVal1Member,3,ModAttrT_03C1310V1OverlayMode,ET_OverlayMode,EType_X,0,NULL};

S_ListDesc1 ModAttrT_Desc_03C1310V1ThresholdDensity = {E_ListIsVal1Member,2,ModAttrT_03C1303V1BorderDensity,ET_ThresholdDensity,EType_X,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_03C1311[] =   /* Image Overlay Box Relationship Module */
  {
   {ET_UNKNOWN,EType_UNKNOWN}
  };
S_ListDesc1 ModAttrT_Desc_03C1311 = {E_ListIsTagMember,0,ModAttrT_03C1311, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_03C1311SQReferencedImageBoxSequence2040[] =  /* Image Overlay Box Relationship Module  */
  {
   /* (0008,1150) */ ET_ReferencedSOPClassUID,
   /* (0008,1155) */ ET_ReferencedSOPInstanceUID
  };
/* ModAttrTable references */
S_Tag ModAttrT_SQ03C1311SQReferencedImageBoxSequence2040[] =  /* Image Overlay Box Relationship Module */
  {
   /* (2040,0500) */ ET_ReferencedImageBoxSequence2040
  };
S_ListDesc1 ModAttrT_Desc_03C1311SQReferencedImageBoxSequence2040 = {E_ListIsTagMemberWithCond,2,ModAttrT_03C1311SQReferencedImageBoxSequence2040, 0, 0,1,ModAttrT_SQ03C1311SQReferencedImageBoxSequence2040};

/* ModAttrTable references */
S_TagType ModAttrT_04C060112[] =   /* Patient Level Attributes for the Patient Root Query/Retrieve Information Model */
  {
   /* (0010,0010) */ {ET_PatientsName,EType_R},
   /* (0010,0020) */ {ET_PatientID,EType_U},
   /* (0010,0030) */ {ET_PatientsBirthDate,EType_O},
   /* (0010,0032) */ {ET_PatientsBirthTime,EType_O},
   /* (0010,0040) */ {ET_PatientsSex,EType_O},
   /* (0010,1000) */ {ET_OtherPatientIDs,EType_O},
   /* (0010,1001) */ {ET_OtherPatientNames,EType_O},
   /* (0010,2160) */ {ET_EthnicGroup,EType_O},
   /* (0010,4000) */ {ET_PatientComments,EType_O},
   /* (0020,1200) */ {ET_NumberofPatientRelatedStudies,EType_O},
   /* (0020,1202) */ {ET_NumberofPatientRelatedSeries,EType_O},
   /* (0020,1204) */ {ET_NumberofPatientRelatedImages,EType_O}
  };
S_ListDesc1 ModAttrT_Desc_04C060112 = {E_ListIsTagTypeMember,12,ModAttrT_04C060112, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04C060112SQReferencedPatientSequence[] =  /* Patient Level Attributes for the Patient Root Query/Retrieve Information Model */
  {
  /* (0008,1120) */ {ET_ReferencedPatientSequence,EType_O}
  };
S_ListDesc1 ModAttrT_Desc_04C060112SQReferencedPatientSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_SOPUIDRefs, 0, 0,1,ModAttrT_04C060112SQReferencedPatientSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04C060113[] =   /* Study Level Keys for the Patient Root Query/Retrieve Information Model */
  {
   /* (0008,0020) */ {ET_StudyDate,EType_R},
   /* (0008,0030) */ {ET_StudyTime,EType_R},
   /* (0008,0050) */ {ET_AccessionNumber,EType_R},
   /* (0008,0090) */ {ET_ReferringPhysiciansName,EType_O},
   /* (0008,1030) */ {ET_StudyDescription,EType_O},
   /* (0008,1060) */ {ET_NameofPhysiciansReadingStudy,EType_O},
   /* (0008,1080) */ {ET_AdmittingDiagnosesDescription,EType_O},
   /* (0010,1010) */ {ET_PatientsAge,EType_O},
   /* (0010,1020) */ {ET_PatientsSize,EType_O},
   /* (0010,1030) */ {ET_PatientsWeight,EType_O},
   /* (0010,2180) */ {ET_Occupation,EType_O},
   /* (0010,21B0) */ {ET_AdditionalPatientHistory,EType_O},
   /* (0020,000D) */ {ET_StudyInstanceUID,EType_U},
   /* (0020,0010) */ {ET_StudyID,EType_R},
   /* (0020,1070) */ {ET_OtherStudyNumbers,EType_O},
   /* (0020,1206) */ {ET_NumberofStudyRelatedSeries,EType_O},
   /* (0020,1208) */ {ET_NumberofStudyRelatedImages,EType_O},
   /* (4008,010C) */ {ET_InterpretationAuthor,EType_O}
  };
S_ListDesc1 ModAttrT_Desc_04C060113 = {E_ListIsTagTypeMember,18,ModAttrT_04C060113, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04C060113SQProcedureCodeSequence[] =  /* Study Level Keys for the Patient Root Query/Retrieve Information Model  */
  {
   /* (0008,0100) */ {ET_CodeValue,EType_O},
   /* (0008,0102) */ {ET_CodingSchemeDesignator,EType_O},
   /* (0008,0104) */ {ET_CodeMeaning,EType_O}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04C060113SQProcedureCodeSequence[] =  /* Study Level Keys for the Patient Root Query/Retrieve Information Model */
  {
   /* (0008,1032) */ {ET_ProcedureCodeSequence,EType_O}
  };
S_ListDesc1 ModAttrT_Desc_04C060113SQProcedureCodeSequence = {E_ListIsTagTypeMemberWithCond,3,ModAttrT_04C060113SQProcedureCodeSequence, 0, 0,1,ModAttrT_SQ04C060113SQProcedureCodeSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04C060113SQReferencedStudySequence[] =  /* Study Level Keys for the Patient Root Query/Retrieve Information Model  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_O},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_O}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04C060113SQReferencedStudySequence[] =  /* Study Level Keys for the Patient Root Query/Retrieve Information Model */
  {
   /* (0008,1110) */ {ET_ReferencedStudySequence,EType_O}
  };
S_ListDesc1 ModAttrT_Desc_04C060113SQReferencedStudySequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04C060113SQReferencedStudySequence, 0, 0,1,ModAttrT_SQ04C060113SQReferencedStudySequence};

/* ModAttrTable references */
S_TagType ModAttrT_04C060114[] =   /* Series Level Attributes for the Patient Root Query/Retrieve Information Model */
  {
   /* (0008,0021) */ {ET_SeriesDate,EType_O},
   /* (0008,0031) */ {ET_SeriesTime,EType_O},
   /* (0008,0060) */ {ET_Modality,EType_R},
   /* (0008,103E) */ {ET_SeriesDescription,EType_O},
   /* (0008,1070) */ {ET_OperatorsName,EType_O},
   /* (0018,0015) */ {ET_BodyPartExamined,EType_O},
   /* (0018,1030) */ {ET_ProtocolName,EType_O},
   /* (0018,5100) */ {ET_PatientPosition,EType_O},
   /* (0020,000E) */ {ET_SeriesInstanceUID,EType_U},
   /* (0020,0011) */ {ET_SeriesNumber,EType_R},
   /* (0020,0060) */ {ET_Laterality,EType_O},
   /* (0028,0108) */ {ET_SmallestPixelValueinSeries,EType_O},
   /* (0028,0109) */ {ET_LargestPixelValueinSeries,EType_O}
  };
S_ListDesc1 ModAttrT_Desc_04C060114 = {E_ListIsTagTypeMember,13,ModAttrT_04C060114, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04C060115[] =   /* Image Level Keys for the Patient Root Query/Retrieve Information Model */
  {
   /* (0008,0008) */ {ET_ImageType,EType_O},
   /* (0008,0018) */ {ET_SOPInstanceUID,EType_U},
   /* (0008,0022) */ {ET_AcquisitionDate,EType_O},
   /* (0008,0023) */ {ET_ImageDate,EType_O},
   /* (0008,0032) */ {ET_AcquisitionTime,EType_O},
   /* (0008,0033) */ {ET_ImageTime,EType_O},
   /* (0008,1140) */ {ET_ReferencedImageSequence,EType_O},
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_O},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_O},
   /* (0008,2111) */ {ET_DerivationDescription,EType_O},
   /* (0008,2112) */ {ET_SourceImageSequence,EType_O},
   /* (0018,0050) */ {ET_SliceThickness,EType_O},
   /* (0020,0012) */ {ET_AcquisitionNumber,EType_O},
   /* (0020,0013) */ {ET_ImageNumber,EType_R},
   /* (0020,0020) */ {ET_PatientOrientation,EType_O},
   /* (0020,0032) */ {ET_ImagePositionPatient,EType_O},
   /* (0020,0037) */ {ET_ImageOrientationPatient,EType_O},
   /* (0020,1002) */ {ET_ImagesinAcquisition,EType_O},
   /* (0020,1041) */ {ET_SliceLocation,EType_O},
   /* (0020,4000) */ {ET_ImageComments,EType_O},
   /* (0028,0002) */ {ET_SamplesperPixel,EType_O},
   /* (0028,0004) */ {ET_PhotometricInterpretation,EType_O},
   /* (0028,0006) */ {ET_PlanarConfiguration,EType_O},
   /* (0028,0010) */ {ET_Rows0028,EType_O},
   /* (0028,0011) */ {ET_Columns0028,EType_O},
   /* (0028,0030) */ {ET_PixelSpacing,EType_O},
   /* (0028,0034) */ {ET_PixelAspectRatio,EType_O},
   /* (0028,0100) */ {ET_BitsAllocated0028,EType_O},
   /* (0028,0101) */ {ET_BitsStored,EType_O},
   /* (0028,0102) */ {ET_HighBit,EType_O},
   /* (0028,0103) */ {ET_PixelRepresentation,EType_O},
   /* (0028,0106) */ {ET_SmallestImagePixelValue,EType_O},
   /* (0028,0107) */ {ET_LargestImagePixelValue,EType_O},
   /* (0028,1050) */ {ET_WindowCenter,EType_O},
   /* (0028,1051) */ {ET_WindowWidth,EType_O},
   /* (0028,1055) */ {ET_WindowCenter_WidthExplanation,EType_O},
   /* (0028,1101) */ {ET_RedPaletteColorLookupTableDescriptor,EType_O},
   /* (0028,1102) */ {ET_GreenPaletteColorLookupTableDescriptor,EType_O},
   /* (0028,1103) */ {ET_BluePaletteColorLookupTableDescriptor,EType_O},
   /* (0028,1201) */ {ET_RedPaletteColorLookupTableData,EType_O},
   /* (0028,1202) */ {ET_GreenPaletteColorLookupTableData,EType_O},
   /* (0028,1203) */ {ET_BluePaletteColorLookupTableData,EType_O},
   /* (0028,3000) */ {ET_ModalityLUTSequence,EType_O},
   /* (0028,3002) */ {ET_LUTDescriptor,EType_O},
   /* (0028,3003) */ {ET_LUTExplanation,EType_O},
   /* (7FE0,0010) */ {ET_PixelData,EType_O}
  };
S_ListDesc1 ModAttrT_Desc_04C060115 = {E_ListIsTagTypeMember,46,ModAttrT_04C060115, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04C060212[] =   /* Study Level Keys for the Study Root Query/Retrieve Information Model */
  {
   /* (0008,0020) */ {ET_StudyDate,EType_R},
   /* (0008,0030) */ {ET_StudyTime,EType_R},
   /* (0008,0050) */ {ET_AccessionNumber,EType_R},
   /* (0008,0090) */ {ET_ReferringPhysiciansName,EType_O},
   /* (0008,1030) */ {ET_StudyDescription,EType_O},
   /* (0008,1060) */ {ET_NameofPhysiciansReadingStudy,EType_O},
   /* (0008,1080) */ {ET_AdmittingDiagnosesDescription,EType_O},
   /* (0010,0010) */ {ET_PatientsName,EType_R},
   /* (0010,0020) */ {ET_PatientID,EType_R},
   /* (0010,0030) */ {ET_PatientsBirthDate,EType_O},
   /* (0010,0032) */ {ET_PatientsBirthTime,EType_O},
   /* (0010,0040) */ {ET_PatientsSex,EType_O},
   /* (0010,1000) */ {ET_OtherPatientIDs,EType_O},
   /* (0010,1001) */ {ET_OtherPatientNames,EType_O},
   /* (0010,1010) */ {ET_PatientsAge,EType_O},
   /* (0010,1020) */ {ET_PatientsSize,EType_O},
   /* (0010,1030) */ {ET_PatientsWeight,EType_O},
   /* (0010,2160) */ {ET_EthnicGroup,EType_O},
   /* (0010,2180) */ {ET_Occupation,EType_O},
   /* (0010,21B0) */ {ET_AdditionalPatientHistory,EType_O},
   /* (0010,4000) */ {ET_PatientComments,EType_O},
   /* (0020,000D) */ {ET_StudyInstanceUID,EType_U},
   /* (0020,0010) */ {ET_StudyID,EType_R},
   /* (0020,1070) */ {ET_OtherStudyNumbers,EType_O},
   /* (0020,1200) */ {ET_NumberofPatientRelatedStudies,EType_O},
   /* (0020,1202) */ {ET_NumberofPatientRelatedSeries,EType_O},
   /* (0020,1204) */ {ET_NumberofPatientRelatedImages,EType_O},
   /* (0020,1206) */ {ET_NumberofStudyRelatedSeries,EType_O},
   /* (0020,1208) */ {ET_NumberofStudyRelatedImages,EType_O},
   /* (4008,010C) */ {ET_InterpretationAuthor,EType_O}
  };
S_ListDesc1 ModAttrT_Desc_04C060212 = {E_ListIsTagTypeMember,30,ModAttrT_04C060212, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04C060212SQProcedureCodeSequence[] =  /* Study Level Keys for the Study Root Query/Retrieve Information Model  */
  {
   /* (0008,0100) */ {ET_CodeValue,EType_O},
   /* (0008,0102) */ {ET_CodingSchemeDesignator,EType_O},
   /* (0008,0104) */ {ET_CodeMeaning,EType_O}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04C060212SQProcedureCodeSequence[] =  /* Study Level Keys for the Study Root Query/Retrieve Information Model */
  {
   /* (0008,1032) */ {ET_ProcedureCodeSequence,EType_O}
  };
S_ListDesc1 ModAttrT_Desc_04C060212SQProcedureCodeSequence = {E_ListIsTagTypeMemberWithCond,3,ModAttrT_04C060212SQProcedureCodeSequence, 0, 0,1,ModAttrT_SQ04C060212SQProcedureCodeSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04C060212SQReferencedStudySequence[] =  /* Study Level Keys for the Study Root Query/Retrieve Information Model  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_O},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_O}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04C060212SQReferencedStudySequence[] =  /* Study Level Keys for the Study Root Query/Retrieve Information Model */
  {
   /* (0008,1110) */ {ET_ReferencedStudySequence,EType_O}
  };
S_ListDesc1 ModAttrT_Desc_04C060212SQReferencedStudySequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04C060212SQReferencedStudySequence, 0, 0,1,ModAttrT_SQ04C060212SQReferencedStudySequence};

/* ModAttrTable references */
S_TagType ModAttrT_04C060212SQReferencedPatientSequence[] =  /* Study Level Keys for the Study Root Query/Retrieve Information Model  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_O},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_O}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04C060212SQReferencedPatientSequence[] =  /* Study Level Keys for the Study Root Query/Retrieve Information Model */
  {
   /* (0008,1120) */ {ET_ReferencedPatientSequence,EType_O}
  };
S_ListDesc1 ModAttrT_Desc_04C060212SQReferencedPatientSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04C060212SQReferencedPatientSequence, 0, 0,1,ModAttrT_SQ04C060212SQReferencedPatientSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04E032[] =   /* Detached Patient Management SOP Class N-Get Attributes */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_3_1C},
   /* (0010,0010) */ {ET_PatientsName,EType_3_2},
   /* (0010,0020) */ {ET_PatientID,EType_3_2},
   /* (0010,0030) */ {ET_PatientsBirthDate,EType_3_2},
   /* (0010,0040) */ {ET_PatientsSex,EType_3_2},
   /* (0038,0004) */ {ET_ReferencedPatientAliasSequence,EType_3_2}
  };
S_ListDesc1 ModAttrT_Desc_04E032 = {E_ListIsTagTypeMember,6,ModAttrT_04E032, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04E032SQReferencedStudySequence[] =  /* Detached Patient Management SOP Class N-Get Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_3_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_3_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04E032SQReferencedStudySequence[] =  /* Detached Patient Management SOP Class N-Get Attributes */
  {
   /* (0008,1110) */ {ET_ReferencedStudySequence,EType_3_2}
  };
S_ListDesc1 ModAttrT_Desc_04E032SQReferencedStudySequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04E032SQReferencedStudySequence, 0, 0,1,ModAttrT_SQ04E032SQReferencedStudySequence};

/* ModAttrTable references */
S_TagType ModAttrT_04E032SQReferencedVisitSequence[] =  /* Detached Patient Management SOP Class N-Get Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_3_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_3_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04E032SQReferencedVisitSequence[] =  /* Detached Patient Management SOP Class N-Get Attributes */
  {
   /* (0008,1125) */ {ET_ReferencedVisitSequence,EType_3_2}
  };
S_ListDesc1 ModAttrT_Desc_04E032SQReferencedVisitSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04E032SQReferencedVisitSequence, 0, 0,1,ModAttrT_SQ04E032SQReferencedVisitSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04E0331[] =   /* Patient Notification Event Information (Patient Created 1) */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_n_1C},
   /* (0008,0012) */ {ET_InstanceCreationDate,EType_n_2},
   /* (0008,0013) */ {ET_InstanceCreationTime,EType_n_2},
   /* (0008,0014) */ {ET_InstanceCreatorUID,EType_n_2},
   /* (0010,0010) */ {ET_PatientsName,EType_n_2},
   /* (0010,0020) */ {ET_PatientID,EType_n_2},
   /* (0010,0030) */ {ET_PatientsBirthDate,EType_n_2},
   /* (0010,0040) */ {ET_PatientsSex,EType_n_2}
  };
S_ListDesc1 ModAttrT_Desc_04E0331 = {E_ListIsTagTypeMember,8,ModAttrT_04E0331, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04E0332[] =   /* Patient Notification Event Information (Patient Deleted 2) */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_n_1C},
   /* (0010,0010) */ {ET_PatientsName,EType_n_2},
   /* (0010,0020) */ {ET_PatientID,EType_n_2}
  };
S_ListDesc1 ModAttrT_Desc_04E0332 = {E_ListIsTagTypeMember,3,ModAttrT_04E0332, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04E0333[] =   /* Patient Notification Event Information (Patient Updated 3) */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_n_1C},
   /* (0010,0010) */ {ET_PatientsName,EType_n_2},
   /* (0010,0020) */ {ET_PatientID,EType_n_2}
  };
S_ListDesc1 ModAttrT_Desc_04E0333 = {E_ListIsTagTypeMember,3,ModAttrT_04E0333, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04E0421[] =   /* Detached Visit Management SOP Class N-Get Attributes */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_3_1C},
   /* (0008,0090) */ {ET_ReferringPhysiciansName,EType_3_2}
  };
S_ListDesc1 ModAttrT_Desc_04E0421 = {E_ListIsTagTypeMember,2,ModAttrT_04E0421, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04E0421SQReferencedStudySequence[] =  /* Detached Visit Management SOP Class N-Get Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_3_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_3_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04E0421SQReferencedStudySequence[] =  /* Detached Visit Management SOP Class N-Get Attributes */
  {
   /* (0008,1110) */ {ET_ReferencedStudySequence,EType_3_2}
  };
S_ListDesc1 ModAttrT_Desc_04E0421SQReferencedStudySequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04E0421SQReferencedStudySequence, 0, 0,1,ModAttrT_SQ04E0421SQReferencedStudySequence};

/* ModAttrTable references */
S_TagType ModAttrT_04E0421SQReferencedPatientSequence[] =  /* Detached Visit Management SOP Class N-Get Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_3_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_3_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04E0421SQReferencedPatientSequence[] =  /* Detached Visit Management SOP Class N-Get Attributes */
  {
   /* (0008,1120) */ {ET_ReferencedPatientSequence,EType_3_1}
  };
S_ListDesc1 ModAttrT_Desc_04E0421SQReferencedPatientSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04E0421SQReferencedPatientSequence, 0, 0,1,ModAttrT_SQ04E0421SQReferencedPatientSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04E0422[] =   /* Detached Visit Management SOP Class N-Set Attributes */
  {
   /* (0038,0008) */ {ET_VisitStatusID,EType_3_1},
   /* (0038,0300) */ {ET_CurrentPatientLocation,EType_3_1}
  };
S_ListDesc1 ModAttrT_Desc_04E0422 = {E_ListIsTagTypeMember,2,ModAttrT_04E0422, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04E0431[] =   /* Visit Notification Event Types and Attributes.(Visit Created 1) */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_n_1C},
   /* (0008,0012) */ {ET_InstanceCreationDate,EType_n_1},
   /* (0008,0013) */ {ET_InstanceCreationTime,EType_n_2},
   /* (0008,0014) */ {ET_InstanceCreatorUID,EType_n_2},
   /* (0038,0008) */ {ET_VisitStatusID,EType_n_1},
   /* (0038,0010) */ {ET_AdmissionID,EType_n_2}
  };
S_ListDesc1 ModAttrT_Desc_04E0431 = {E_ListIsTagTypeMember,6,ModAttrT_04E0431, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04E0431SQReferencedPatientSequence[] =  /* Visit Notification Event Types and Attributes.(Visit Created 1)  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_n_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_n_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04E0431SQReferencedPatientSequence[] =  /* Visit Notification Event Types and Attributes.(Visit Created 1) */
  {
   /* (0008,1120) */ {ET_ReferencedPatientSequence,EType_n_1}
  };
S_ListDesc1 ModAttrT_Desc_04E0431SQReferencedPatientSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04E0431SQReferencedPatientSequence, 0, 0,1,ModAttrT_SQ04E0431SQReferencedPatientSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04E0432[] =   /* Visit Notification Event Types and Attributes.(Visit Scheduled 2) */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_n_1C},
   /* (0038,001A) */ {ET_ScheduledAdmissionDate,EType_n_1},
   /* (0038,001B) */ {ET_ScheduledAdmissionTime,EType_n_2},
   /* (0038,001E) */ {ET_ScheduledPatientInstitutionResidence,EType_n_2}
  };
S_ListDesc1 ModAttrT_Desc_04E0432 = {E_ListIsTagTypeMember,4,ModAttrT_04E0432, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04E0432SQReferencedPatientSequence[] =  /* Visit Notification Event Types and Attributes.(Visit Scheduled 2)  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_n_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_n_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04E0432SQReferencedPatientSequence[] =  /* Visit Notification Event Types and Attributes.(Visit Scheduled 2) */
  {
   /* (0008,1120) */ {ET_ReferencedPatientSequence,EType_n_1}
  };
S_ListDesc1 ModAttrT_Desc_04E0432SQReferencedPatientSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04E0432SQReferencedPatientSequence, 0, 0,1,ModAttrT_SQ04E0432SQReferencedPatientSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04E0433[] =   /* Visit Notification Event Types and Attributes.(Patient Admitted 3) */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_n_1C},
   /* (0008,0090) */ {ET_ReferringPhysiciansName,EType_n_2},
   /* (0038,0008) */ {ET_VisitStatusID,EType_n_1},
   /* (0038,0016) */ {ET_RouteofAdmissions,EType_n_2},
   /* (0038,0020) */ {ET_AdmittingDate,EType_n_1},
   /* (0038,0021) */ {ET_AdmittingTime,EType_n_2}
  };
S_ListDesc1 ModAttrT_Desc_04E0433 = {E_ListIsTagTypeMember,6,ModAttrT_04E0433, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04E0433SQReferencedPatientSequence[] =  /* Visit Notification Event Types and Attributes.(Patient Admitted 3)  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_n_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_n_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04E0433SQReferencedPatientSequence[] =  /* Visit Notification Event Types and Attributes.(Patient Admitted 3) */
  {
   /* (0008,1120) */ {ET_ReferencedPatientSequence,EType_n_1}
  };
S_ListDesc1 ModAttrT_Desc_04E0433SQReferencedPatientSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04E0433SQReferencedPatientSequence, 0, 0,1,ModAttrT_SQ04E0433SQReferencedPatientSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04E0434[] =   /* Visit Notification Event Types and Attributes.(Patient Transferred 4) */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_n_1C},
   /* (0038,0008) */ {ET_VisitStatusID,EType_n_1},
   /* (0038,0300) */ {ET_CurrentPatientLocation,EType_n_1},
   /* (0038,0400) */ {ET_PatientsInstitutionResidence,EType_n_1}
  };
S_ListDesc1 ModAttrT_Desc_04E0434 = {E_ListIsTagTypeMember,4,ModAttrT_04E0434, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04E0434SQReferencedPatientSequence[] =  /* Visit Notification Event Types and Attributes.(Patient Transferred 4)  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_n_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_n_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04E0434SQReferencedPatientSequence[] =  /* Visit Notification Event Types and Attributes.(Patient Transferred 4) */
  {
   /* (0008,1120) */ {ET_ReferencedPatientSequence,EType_n_1}
  };
S_ListDesc1 ModAttrT_Desc_04E0434SQReferencedPatientSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04E0434SQReferencedPatientSequence, 0, 0,1,ModAttrT_SQ04E0434SQReferencedPatientSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04E0435[] =   /* Visit Notification Event Types and Attributes.(Patient Discharged 5) */
  {
   /* (0038,0008) */ {ET_VisitStatusID,EType_n_1},
   /* (0038,0030) */ {ET_DischargeDate,EType_n_2},
   /* (0038,0032) */ {ET_DischargeTime,EType_n_2}
  };
S_ListDesc1 ModAttrT_Desc_04E0435 = {E_ListIsTagTypeMember,3,ModAttrT_04E0435, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04E0435SQReferencedPatientSequence[] =  /* Visit Notification Event Types and Attributes.(Patient Discharged 5)  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_n_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_n_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04E0435SQReferencedPatientSequence[] =  /* Visit Notification Event Types and Attributes.(Patient Discharged 5) */
  {
   /* (0008,1120) */ {ET_ReferencedPatientSequence,EType_n_1}
  };
S_ListDesc1 ModAttrT_Desc_04E0435SQReferencedPatientSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04E0435SQReferencedPatientSequence, 0, 0,1,ModAttrT_SQ04E0435SQReferencedPatientSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04E0436[] =   /* Visit Notification Event Types and Attributes.(Visit Deleted 6) */
  {
   /* (0038,0010) */ {ET_AdmissionID,EType_n_2}
  };
S_ListDesc1 ModAttrT_Desc_04E0436 = {E_ListIsTagTypeMember,1,ModAttrT_04E0436, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04E0437[] =   /* Visit Notification Event Types and Attributes.(Visit Updated 7) */
  {
   {ET_UNKNOWN,EType_UNKNOWN}
  };
S_ListDesc1 ModAttrT_Desc_04E0437 = {E_ListIsTagTypeMember,0,ModAttrT_04E0437, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04F0321[] =   /* Detached Study Management SOP Class N-GET Attributes */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_3_1C},
   /* (0008,0050) */ {ET_AccessionNumber,EType_3_2},
   /* (0020,0010) */ {ET_StudyID,EType_3_2}
  };
S_ListDesc1 ModAttrT_Desc_04F0321 = {E_ListIsTagTypeMember,3,ModAttrT_04F0321, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04F0321SQReferencedResultsSequence[] =  /* Detached Study Management SOP Class N-GET Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_3_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_3_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04F0321SQReferencedResultsSequence[] =  /* Detached Study Management SOP Class N-GET Attributes */
  {
   /* (0008,1100) */ {ET_ReferencedResultsSequence,EType_3_2}
  };
S_ListDesc1 ModAttrT_Desc_04F0321SQReferencedResultsSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04F0321SQReferencedResultsSequence, 0, 0,1,ModAttrT_SQ04F0321SQReferencedResultsSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04F0321SQReferencedStudyComponentSequence[] =  /* Detached Study Management SOP Class N-GET Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_3_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_3_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04F0321SQReferencedStudyComponentSequence[] =  /* Detached Study Management SOP Class N-GET Attributes */
  {
   /* (0008,1111) */ {ET_ReferencedStudyComponentSequence,EType_3_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0321SQReferencedStudyComponentSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04F0321SQReferencedStudyComponentSequence, 0, 0,1,ModAttrT_SQ04F0321SQReferencedStudyComponentSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04F0321SQReferencedPatientSequence[] =  /* Detached Study Management SOP Class N-GET Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_3_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_3_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04F0321SQReferencedPatientSequence[] =  /* Detached Study Management SOP Class N-GET Attributes */
  {
   /* (0008,1120) */ {ET_ReferencedPatientSequence,EType_3_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0321SQReferencedPatientSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04F0321SQReferencedPatientSequence, 0, 0,1,ModAttrT_SQ04F0321SQReferencedPatientSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04F0321SQReferencedVisitSequence[] =  /* Detached Study Management SOP Class N-GET Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_3_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_3_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04F0321SQReferencedVisitSequence[] =  /* Detached Study Management SOP Class N-GET Attributes */
  {
   /* (0008,1125) */ {ET_ReferencedVisitSequence,EType_3_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0321SQReferencedVisitSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04F0321SQReferencedVisitSequence, 0, 0,1,ModAttrT_SQ04F0321SQReferencedVisitSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04F0322[] =   /* Detached Study Management SOP Class N-Set Attributes */
  {
   /* (0008,0020) */ {ET_StudyDate,EType_3_1},
   /* (0008,0030) */ {ET_StudyTime,EType_3_1},
   /* (0032,000A) */ {ET_StudyStatusID,EType_3_1},
   /* (0032,0032) */ {ET_StudyVerifiedDate,EType_3_1},
   /* (0032,0033) */ {ET_StudyVerifiedTime,EType_3_1},
   /* (0032,1040) */ {ET_StudyArrivalDate,EType_3_1},
   /* (0032,1041) */ {ET_StudyArrivalTime,EType_3_1},
   /* (0032,1050) */ {ET_StudyCompletionDate,EType_3_1},
   /* (0032,1051) */ {ET_StudyCompletionTime,EType_3_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0322 = {E_ListIsTagTypeMember,9,ModAttrT_04F0322, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04F0331[] =   /* Study Notification Event Information (Study Created 1) */
  {
   /* (0008,0012) */ {ET_InstanceCreationDate,EType_n_1},
   /* (0008,0013) */ {ET_InstanceCreationTime,EType_n_2},
   /* (0008,0014) */ {ET_InstanceCreatorUID,EType_n_2},
   /* (0032,000A) */ {ET_StudyStatusID,EType_n_2}
  };
S_ListDesc1 ModAttrT_Desc_04F0331 = {E_ListIsTagTypeMember,4,ModAttrT_04F0331, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04F0331SQReferencedPatientSequence[] =  /* Study Notification Event Information (Study Created 1)  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_n_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_n_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04F0331SQReferencedPatientSequence[] =  /* Study Notification Event Information (Study Created 1) */
  {
   /* (0008,1120) */ {ET_ReferencedPatientSequence,EType_n_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0331SQReferencedPatientSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04F0331SQReferencedPatientSequence, 0, 0,1,ModAttrT_SQ04F0331SQReferencedPatientSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04F0331SQReferencedVisitSequence[] =  /* Study Notification Event Information (Study Created 1)  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_n_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_n_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04F0331SQReferencedVisitSequence[] =  /* Study Notification Event Information (Study Created 1) */
  {
   /* (0008,1125) */ {ET_ReferencedVisitSequence,EType_n_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0331SQReferencedVisitSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04F0331SQReferencedVisitSequence, 0, 0,1,ModAttrT_SQ04F0331SQReferencedVisitSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04F0332[] =   /* Study Notification Event Information (Study Scheduled 2) */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_n_1C},
   /* (0032,1000) */ {ET_ScheduledStudyStartDate,EType_n_1},
   /* (0032,1001) */ {ET_ScheduledStudyStartTime,EType_n_1},
   /* (0032,1020) */ {ET_ScheduledStudyLocation,EType_n_2},
   /* (0032,1021) */ {ET_ScheduledStudyLocationAETitles,EType_n_2},
   /* (0032,1060) */ {ET_RequestedProcedureDescription,EType_n_2}
  };
S_ListDesc1 ModAttrT_Desc_04F0332 = {E_ListIsTagTypeMember,6,ModAttrT_04F0332, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04F0332SQReferencedPatientSequence[] =  /* Study Notification Event Information (Study Scheduled 2)  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_n_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_n_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04F0332SQReferencedPatientSequence[] =  /* Study Notification Event Information (Study Scheduled 2) */
  {
   /* (0008,1120) */ {ET_ReferencedPatientSequence,EType_n_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0332SQReferencedPatientSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04F0332SQReferencedPatientSequence, 0, 0,1,ModAttrT_SQ04F0332SQReferencedPatientSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04F0332SQReferencedVisitSequence[] =  /* Study Notification Event Information (Study Scheduled 2)  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_n_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_n_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04F0332SQReferencedVisitSequence[] =  /* Study Notification Event Information (Study Scheduled 2) */
  {
   /* (0008,1125) */ {ET_ReferencedVisitSequence,EType_n_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0332SQReferencedVisitSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04F0332SQReferencedVisitSequence, 0, 0,1,ModAttrT_SQ04F0332SQReferencedVisitSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04F0332SQRequestedProcedureCodeSequence[] =  /* Study Notification Event Information (Study Scheduled 2)  */
  {
   /* (0008,0100) */ {ET_CodeValue,EType_n_1C},
   /* (0008,0102) */ {ET_CodingSchemeDesignator,EType_n_1C},
   /* (0008,0104) */ {ET_CodeMeaning,EType_n_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04F0332SQRequestedProcedureCodeSequence[] =  /* Study Notification Event Information (Study Scheduled 2) */
  {
   /* (0032,1064) */ {ET_RequestedProcedureCodeSequence,EType_n_2}
  };
S_ListDesc1 ModAttrT_Desc_04F0332SQRequestedProcedureCodeSequence = {E_ListIsTagTypeMemberWithCond,3,ModAttrT_04F0332SQRequestedProcedureCodeSequence, 0, 0,1,ModAttrT_SQ04F0332SQRequestedProcedureCodeSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04F0333[] =   /* Study Notification Event Information (Patient Arrived 3) */
  {
   /* (0032,1040) */ {ET_StudyArrivalDate,EType_n_1},
   /* (0032,1041) */ {ET_StudyArrivalTime,EType_n_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0333 = {E_ListIsTagTypeMember,2,ModAttrT_04F0333, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04F0334[] =   /* Study Notification Event Information (Study Started 4) */
  {
   /* (0008,0020) */ {ET_StudyDate,EType_n_1},
   /* (0008,0030) */ {ET_StudyTime,EType_n_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0334 = {E_ListIsTagTypeMember,2,ModAttrT_04F0334, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04F0335[] =   /* Study Notification Event Information (Study Completed 5) */
  {
   /* (0032,1050) */ {ET_StudyCompletionDate,EType_n_1},
   /* (0032,1051) */ {ET_StudyCompletionTime,EType_n_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0335 = {E_ListIsTagTypeMember,2,ModAttrT_04F0335, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04F0335SQReferencedStudyComponentSequence[] =  /* Study Notification Event Information (Study Completed 5)  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_n_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_n_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04F0335SQReferencedStudyComponentSequence[] =  /* Study Notification Event Information (Study Completed 5) */
  {
   /* (0008,1111) */ {ET_ReferencedStudyComponentSequence,EType_n_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0335SQReferencedStudyComponentSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04F0335SQReferencedStudyComponentSequence, 0, 0,1,ModAttrT_SQ04F0335SQReferencedStudyComponentSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04F0336[] =   /* Study Notification Event Information (Study Verified 6) */
  {
   /* (0032,0032) */ {ET_StudyVerifiedDate,EType_n_1},
   /* (0032,0033) */ {ET_StudyVerifiedTime,EType_n_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0336 = {E_ListIsTagTypeMember,2,ModAttrT_04F0336, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04F0336SQReferencedStudyComponentSequence[] =  /* Study Notification Event Information (Study Verified 6)  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_n_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_n_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04F0336SQReferencedStudyComponentSequence[] =  /* Study Notification Event Information (Study Verified 6) */
  {
   /* (0008,1111) */ {ET_ReferencedStudyComponentSequence,EType_n_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0336SQReferencedStudyComponentSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04F0336SQReferencedStudyComponentSequence, 0, 0,1,ModAttrT_SQ04F0336SQReferencedStudyComponentSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04F0337[] =   /* Study Notification Event Information (Study Read 7) */
  {
   /* (0032,0034) */ {ET_StudyReadDate,EType_n_1},
   /* (0032,0035) */ {ET_StudyReadTime,EType_n_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0337 = {E_ListIsTagTypeMember,2,ModAttrT_04F0337, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04F0339[] =   /* Study Notification Event Information (Study Updated 9) */
  {
   {ET_UNKNOWN,EType_UNKNOWN}
  };
S_ListDesc1 ModAttrT_Desc_04F0339 = {E_ListIsTagTypeMember,0,ModAttrT_04F0339, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04F0421[] =   /* Study Component Management SOP Class N-CREATE Attributes */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_1C_1C},
   /* (0008,0060) */ {ET_Modality,EType_1_1},
   /* (0008,1030) */ {ET_StudyDescription,EType_2_2},
   /* (0020,0010) */ {ET_StudyID,EType_2_2},
   /* (0032,1055) */ {ET_StudyComponentStatusID,EType_1_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0421 = {E_ListIsTagTypeMember,5,ModAttrT_04F0421, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04F0421SQProcedureCodeSequence[] =  /* Study Component Management SOP Class N-CREATE Attributes  */
  {
   /* (0008,0100) */ {ET_CodeValue,EType_1_1C},
   /* (0008,0102) */ {ET_CodingSchemeDesignator,EType_1_1C},
   /* (0008,0104) */ {ET_CodeMeaning,EType_1_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04F0421SQProcedureCodeSequence[] =  /* Study Component Management SOP Class N-CREATE Attributes */
  {
   /* (0008,1032) */ {ET_ProcedureCodeSequence,EType_1_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0421SQProcedureCodeSequence = {E_ListIsTagTypeMemberWithCond,3,ModAttrT_04F0421SQProcedureCodeSequence, 0, 0,1,ModAttrT_SQ04F0421SQProcedureCodeSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04F0421SQReferencedStudySequence[] =  /* Study Component Management SOP Class N-CREATE Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_1_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_1_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04F0421SQReferencedStudySequence[] =  /* Study Component Management SOP Class N-CREATE Attributes */
  {
   /* (0008,1110) */ {ET_ReferencedStudySequence,EType_1_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0421SQReferencedStudySequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04F0421SQReferencedStudySequence, 0, 0,1,ModAttrT_SQ04F0421SQReferencedStudySequence};

/* ModAttrTable references */
S_TagType ModAttrT_04F0421SQReferencedSeriesSequence[] =  /* Study Component Management SOP Class N-CREATE Attributes  */
  {
   /* (0008,0021) */ {ET_SeriesDate,EType_2_2C},
   /* (0008,0031) */ {ET_SeriesTime,EType_2_2C},
   /* (0008,0054) */ {ET_RetrieveAETitle,EType_2C_1C},
   /* (0020,000E) */ {ET_SeriesInstanceUID,EType_2_2C},
   /* (0088,0130) */ {ET_StorageMediaFilesetID,EType_2C_1C},
   /* (0088,0140) */ {ET_StorageMediaFilesetUID,EType_2C_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04F0421SQReferencedSeriesSequence[] =  /* Study Component Management SOP Class N-CREATE Attributes */
  {
   /* (0008,1115) */ {ET_ReferencedSeriesSequence,EType_2_2}
  };
S_ListDesc1 ModAttrT_Desc_04F0421SQReferencedSeriesSequence = {E_ListIsTagTypeMemberWithCond,6,ModAttrT_04F0421SQReferencedSeriesSequence, 0, 0,1,ModAttrT_SQ04F0421SQReferencedSeriesSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04F0421SQSQReferencedImageSequence[] =  /* Study Component Management SOP Class N-CREATE Attributes  */
  {
   /* (0008,0054) */ {ET_RetrieveAETitle,EType_2C_1C},
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_2_2C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_2_2C},
   /* (0088,0130) */ {ET_StorageMediaFilesetID,EType_2C_1C},
   /* (0088,0140) */ {ET_StorageMediaFilesetUID,EType_2C_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQSQ04F0421SQSQReferencedImageSequence[] =  /* Study Component Management SOP Class N-CREATE Attributes */
  {
  {ET_ReferencedSeriesSequence,EType_2_2},
  {ET_ReferencedImageSequence,EType_2_2C}
  };
S_ListDesc1 ModAttrT_Desc_04F0421SQSQReferencedImageSequence = {E_ListIsTagTypeMemberWithCond,5,ModAttrT_04F0421SQSQReferencedImageSequence, 0, 0,2,ModAttrT_SQSQ04F0421SQSQReferencedImageSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04F0422[] =   /* Study Component Management SOP Class N-SET Attributes */
  {
   /* (0008,1030) */ {ET_StudyDescription,EType_3_1},
   /* (0020,0010) */ {ET_StudyID,EType_3_1},
   /* (0032,1055) */ {ET_StudyComponentStatusID,EType_3_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0422 = {E_ListIsTagTypeMember,3,ModAttrT_04F0422, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04F0422SQProcedureCodeSequence[] =  /* Study Component Management SOP Class N-SET Attributes  */
  {
   /* (0008,0100) */ {ET_CodeValue,EType_3_1C},
   /* (0008,0102) */ {ET_CodingSchemeDesignator,EType_3_1C},
   /* (0008,0104) */ {ET_CodeMeaning,EType_3_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04F0422SQProcedureCodeSequence[] =  /* Study Component Management SOP Class N-SET Attributes */
  {
   /* (0008,1032) */ {ET_ProcedureCodeSequence,EType_3_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0422SQProcedureCodeSequence = {E_ListIsTagTypeMemberWithCond,3,ModAttrT_04F0422SQProcedureCodeSequence, 0, 0,1,ModAttrT_SQ04F0422SQProcedureCodeSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04F0422SQReferencedSeriesSequence[] =  /* Study Component Management SOP Class N-SET Attributes  */
  {
   /* (0008,0021) */ {ET_SeriesDate,EType_3_1C},
   /* (0008,0031) */ {ET_SeriesTime,EType_3_1C},
   /* (0008,0054) */ {ET_RetrieveAETitle,EType_3_1C},
   /* (0020,000E) */ {ET_SeriesInstanceUID,EType_3_1C},
   /* (0088,0130) */ {ET_StorageMediaFilesetID,EType_3_1C},
   /* (0088,0140) */ {ET_StorageMediaFilesetUID,EType_3_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04F0422SQReferencedSeriesSequence[] =  /* Study Component Management SOP Class N-SET Attributes */
  {
   /* (0008,1115) */ {ET_ReferencedSeriesSequence,EType_3_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0422SQReferencedSeriesSequence = {E_ListIsTagTypeMemberWithCond,6,ModAttrT_04F0422SQReferencedSeriesSequence, 0, 0,1,ModAttrT_SQ04F0422SQReferencedSeriesSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04F0422SQSQReferencedImageSequence[] =  /* Study Component Management SOP Class N-SET Attributes  */
  {
   /* (0008,0054) */ {ET_RetrieveAETitle,EType_3_1C},
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_3_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_3_1C},
   /* (0088,0130) */ {ET_StorageMediaFilesetID,EType_3_1C},
   /* (0088,0140) */ {ET_StorageMediaFilesetUID,EType_3_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQSQ04F0422SQSQReferencedImageSequence[] =  /* Study Component Management SOP Class N-SET Attributes */
  {
  {ET_ReferencedSeriesSequence,EType_3_1},
  {ET_ReferencedImageSequence,EType_3_1C}
  };
S_ListDesc1 ModAttrT_Desc_04F0422SQSQReferencedImageSequence = {E_ListIsTagTypeMemberWithCond,5,ModAttrT_04F0422SQSQReferencedImageSequence, 0, 0,2,ModAttrT_SQSQ04F0422SQSQReferencedImageSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04F0423[] =   /* Study Component Management SOP Class N-Get Attributes */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_3_1C},
   /* (0008,0060) */ {ET_Modality,EType_3_1},
   /* (0008,1030) */ {ET_StudyDescription,EType_3_1},
   /* (0020,0010) */ {ET_StudyID,EType_3_2},
   /* (0032,1055) */ {ET_StudyComponentStatusID,EType_3_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0423 = {E_ListIsTagTypeMember,5,ModAttrT_04F0423, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04F0423SQProcedureCodeSequence[] =  /* Study Component Management SOP Class N-Get Attributes  */
  {
   /* (0008,0100) */ {ET_CodeValue,EType_3_1C},
   /* (0008,0102) */ {ET_CodingSchemeDesignator,EType_3_1C},
   /* (0008,0104) */ {ET_CodeMeaning,EType_3_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04F0423SQProcedureCodeSequence[] =  /* Study Component Management SOP Class N-Get Attributes */
  {
   /* (0008,1032) */ {ET_ProcedureCodeSequence,EType_3_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0423SQProcedureCodeSequence = {E_ListIsTagTypeMemberWithCond,3,ModAttrT_04F0423SQProcedureCodeSequence, 0, 0,1,ModAttrT_SQ04F0423SQProcedureCodeSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04F0423SQReferencedStudySequence[] =  /* Study Component Management SOP Class N-Get Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_3_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_3_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04F0423SQReferencedStudySequence[] =  /* Study Component Management SOP Class N-Get Attributes */
  {
   /* (0008,1110) */ {ET_ReferencedStudySequence,EType_3_1}
  };
S_ListDesc1 ModAttrT_Desc_04F0423SQReferencedStudySequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04F0423SQReferencedStudySequence, 0, 0,1,ModAttrT_SQ04F0423SQReferencedStudySequence};

/* ModAttrTable references */
S_TagType ModAttrT_04F0423SQReferencedSeriesSequence[] =  /* Study Component Management SOP Class N-Get Attributes  */
  {
   /* (0008,0021) */ {ET_SeriesDate,EType_3_1C},
   /* (0008,0031) */ {ET_SeriesTime,EType_3_1C},
   /* (0008,0054) */ {ET_RetrieveAETitle,EType_3_1C},
   /* (0020,000E) */ {ET_SeriesInstanceUID,EType_3_1C},
   /* (0088,0130) */ {ET_StorageMediaFilesetID,EType_3_1C},
   /* (0088,0140) */ {ET_StorageMediaFilesetUID,EType_3_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04F0423SQReferencedSeriesSequence[] =  /* Study Component Management SOP Class N-Get Attributes */
  {
   /* (0008,1115) */ {ET_ReferencedSeriesSequence,EType_3_2}
  };
S_ListDesc1 ModAttrT_Desc_04F0423SQReferencedSeriesSequence = {E_ListIsTagTypeMemberWithCond,6,ModAttrT_04F0423SQReferencedSeriesSequence, 0, 0,1,ModAttrT_SQ04F0423SQReferencedSeriesSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04F0423SQSQReferencedImageSequence[] =  /* Study Component Management SOP Class N-Get Attributes  */
  {
   /* (0008,0054) */ {ET_RetrieveAETitle,EType_3_1C},
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_3_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_3_1C},
   /* (0088,0130) */ {ET_StorageMediaFilesetID,EType_3_1C},
   /* (0088,0140) */ {ET_StorageMediaFilesetUID,EType_3_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQSQ04F0423SQSQReferencedImageSequence[] =  /* Study Component Management SOP Class N-Get Attributes */
  {
  {ET_ReferencedSeriesSequence,EType_3_2},
  {ET_ReferencedImageSequence,EType_3_1C}
  };
S_ListDesc1 ModAttrT_Desc_04F0423SQSQReferencedImageSequence = {E_ListIsTagTypeMemberWithCond,5,ModAttrT_04F0423SQSQReferencedImageSequence, 0, 0,2,ModAttrT_SQSQ04F0423SQSQReferencedImageSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04G032[] =   /* Detached Results Management SOP Class N-Get Attributes */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_3_1C},
   /* (4008,0040) */ {ET_ResultsID,EType_3_2},
   /* (4008,0300) */ {ET_Impressions,EType_3_2}
  };
S_ListDesc1 ModAttrT_Desc_04G032 = {E_ListIsTagTypeMember,3,ModAttrT_04G032, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04G032SQReferencedStudySequence[] =  /* Detached Results Management SOP Class N-Get Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_3_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_3_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04G032SQReferencedStudySequence[] =  /* Detached Results Management SOP Class N-Get Attributes */
  {
   /* (0008,1110) */ {ET_ReferencedStudySequence,EType_3_1}
  };
S_ListDesc1 ModAttrT_Desc_04G032SQReferencedStudySequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04G032SQReferencedStudySequence, 0, 0,1,ModAttrT_SQ04G032SQReferencedStudySequence};

/* ModAttrTable references */
S_TagType ModAttrT_04G032SQReferencedInterpretationSequence[] =  /* Detached Results Management SOP Class N-Get Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_3_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_3_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04G032SQReferencedInterpretationSequence[] =  /* Detached Results Management SOP Class N-Get Attributes */
  {
   /* (4008,0050) */ {ET_ReferencedInterpretationSequence,EType_3_2}
  };
S_ListDesc1 ModAttrT_Desc_04G032SQReferencedInterpretationSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04G032SQReferencedInterpretationSequence, 0, 0,1,ModAttrT_SQ04G032SQReferencedInterpretationSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04G0331[] =   /* Results Notification Event Types and Attributes (Results Created 1) */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_n_1C},
   /* (0008,0012) */ {ET_InstanceCreationDate,EType_n_1},
   /* (0008,0013) */ {ET_InstanceCreationTime,EType_n_2},
   /* (0008,0014) */ {ET_InstanceCreatorUID,EType_n_2},
   /* (4008,0040) */ {ET_ResultsID,EType_n_2}
  };
S_ListDesc1 ModAttrT_Desc_04G0331 = {E_ListIsTagTypeMember,5,ModAttrT_04G0331, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04G0331SQReferencedStudySequence[] =  /* Results Notification Event Types and Attributes (Results Created 1)  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_n_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_n_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04G0331SQReferencedStudySequence[] =  /* Results Notification Event Types and Attributes (Results Created 1) */
  {
   /* (0008,1110) */ {ET_ReferencedStudySequence,EType_n_1}
  };
S_ListDesc1 ModAttrT_Desc_04G0331SQReferencedStudySequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04G0331SQReferencedStudySequence, 0, 0,1,ModAttrT_SQ04G0331SQReferencedStudySequence};

/* ModAttrTable references */
S_TagType ModAttrT_04G0331SQReferencedInterpretationSequence[] =  /* Results Notification Event Types and Attributes (Results Created 1)  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_n_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_n_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04G0331SQReferencedInterpretationSequence[] =  /* Results Notification Event Types and Attributes (Results Created 1) */
  {
   /* (4008,0050) */ {ET_ReferencedInterpretationSequence,EType_n_2}
  };
S_ListDesc1 ModAttrT_Desc_04G0331SQReferencedInterpretationSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04G0331SQReferencedInterpretationSequence, 0, 0,1,ModAttrT_SQ04G0331SQReferencedInterpretationSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04G0332[] =   /* Results Notification Event Types and Attributes (Results Deleted 2) */
  {
   /* (4008,0040) */ {ET_ResultsID,EType_n_2}
  };
S_ListDesc1 ModAttrT_Desc_04G0332 = {E_ListIsTagTypeMember,1,ModAttrT_04G0332, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04G0333[] =   /* Results Notification Event Types and Attributes (Results Updated 3) */
  {
   {ET_UNKNOWN,EType_UNKNOWN}
  };
S_ListDesc1 ModAttrT_Desc_04G0333 = {E_ListIsTagTypeMember,0,ModAttrT_04G0333, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04G0421[] =   /* Detached Interpretation Management SOP Class N-Get Attributes */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_3_1C},
   /* (4008,010B) */ {ET_InterpretationText,EType_3_2},
   /* (4008,0200) */ {ET_InterpretationID,EType_3_2},
   /* (4008,0210) */ {ET_InterpretationTypeID,EType_3_2},
   /* (4008,0212) */ {ET_InterpretationStatusID,EType_3_2}
  };
S_ListDesc1 ModAttrT_Desc_04G0421 = {E_ListIsTagTypeMember,5,ModAttrT_04G0421, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04G0421SQReferencedResultsSequence[] =  /* Detached Interpretation Management SOP Class N-Get Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_3_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_3_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04G0421SQReferencedResultsSequence[] =  /* Detached Interpretation Management SOP Class N-Get Attributes */
  {
   /* (0008,1100) */ {ET_ReferencedResultsSequence,EType_3_1}
  };
S_ListDesc1 ModAttrT_Desc_04G0421SQReferencedResultsSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04G0421SQReferencedResultsSequence, 0, 0,1,ModAttrT_SQ04G0421SQReferencedResultsSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04G0422[] =   /* Detached Interpretation  Management SOP Class N-Set Attributes */
  {
   /* (4008,0212) */ {ET_InterpretationStatusID,EType_3_1}
  };
S_ListDesc1 ModAttrT_Desc_04G0422 = {E_ListIsTagTypeMember,1,ModAttrT_04G0422, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04G0431[] =   /* Interpretation Notification Event Information (Interpretation Created 1) */
  {
   /* (0008,0012) */ {ET_InstanceCreationDate,EType_n_1},
   /* (0008,0013) */ {ET_InstanceCreationTime,EType_n_2},
   /* (0008,0014) */ {ET_InstanceCreatorUID,EType_n_2},
   /* (4008,0210) */ {ET_InterpretationTypeID,EType_n_2},
   /* (4008,0212) */ {ET_InterpretationStatusID,EType_n_2}
  };
S_ListDesc1 ModAttrT_Desc_04G0431 = {E_ListIsTagTypeMember,5,ModAttrT_04G0431, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04G0431SQReferencedResultsSequence[] =  /* Interpretation Notification Event Information (Interpretation Created 1)  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_n_1C},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_n_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04G0431SQReferencedResultsSequence[] =  /* Interpretation Notification Event Information (Interpretation Created 1) */
  {
   /* (0008,1100) */ {ET_ReferencedResultsSequence,EType_n_1}
  };
S_ListDesc1 ModAttrT_Desc_04G0431SQReferencedResultsSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04G0431SQReferencedResultsSequence, 0, 0,1,ModAttrT_SQ04G0431SQReferencedResultsSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04G0432[] =   /* Interpretation Notification Event Information (Interpretation Recorded 2) */
  {
   /* (4008,0100) */ {ET_InterpretationRecordedDate,EType_n_1},
   /* (4008,0101) */ {ET_InterpretationRecordedTime,EType_n_2},
   /* (4008,0102) */ {ET_InterpretationRecorder,EType_n_2},
   /* (4008,0103) */ {ET_ReferencetoRecordedSound,EType_n_3},
   /* (4008,0210) */ {ET_InterpretationTypeID,EType_n_2},
   /* (4008,0212) */ {ET_InterpretationStatusID,EType_n_2}
  };
S_ListDesc1 ModAttrT_Desc_04G0432 = {E_ListIsTagTypeMember,6,ModAttrT_04G0432, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04G0433[] =   /* Interpretation Notification Event Information (Interpretation Transcribed 3) */
  {
   /* (4008,0108) */ {ET_InterpretationTranscriptionDate,EType_n_1},
   /* (4008,0109) */ {ET_InterpretationTranscriptionTime,EType_n_2},
   /* (4008,010C) */ {ET_InterpretationAuthor,EType_n_1},
   /* (4008,0210) */ {ET_InterpretationTypeID,EType_n_2},
   /* (4008,0212) */ {ET_InterpretationStatusID,EType_n_2}
  };
S_ListDesc1 ModAttrT_Desc_04G0433 = {E_ListIsTagTypeMember,5,ModAttrT_04G0433, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04G0434[] =   /* Interpretation Notification Event Information (Interpretation Approved 4) */
  {
   /* (4008,0115) */ {ET_InterpretationDiagnosisDescription,EType_n_2},
   /* (4008,0212) */ {ET_InterpretationStatusID,EType_n_2}
  };
S_ListDesc1 ModAttrT_Desc_04G0434 = {E_ListIsTagTypeMember,2,ModAttrT_04G0434, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04G0434SQInterpretationApproverSequence[] =  /* Interpretation Notification Event Information (Interpretation Approved 4)  */
  {
   /* (4008,0112) */ {ET_InterpretationApprovalDate,EType_n_1},
   /* (4008,0113) */ {ET_InterpretationApprovalTime,EType_n_2},
   /* (4008,0114) */ {ET_PhysicianApprovingInterpretation,EType_n_1}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04G0434SQInterpretationApproverSequence[] =  /* Interpretation Notification Event Information (Interpretation Approved 4) */
  {
   /* (4008,0111) */ {ET_InterpretationApproverSequence,EType_n_1}
  };
S_ListDesc1 ModAttrT_Desc_04G0434SQInterpretationApproverSequence = {E_ListIsTagTypeMemberWithCond,3,ModAttrT_04G0434SQInterpretationApproverSequence, 0, 0,1,ModAttrT_SQ04G0434SQInterpretationApproverSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04G0434SQDiagnosisCodeSequence[] =  /* Interpretation Notification Event Information (Interpretation Approved 4)  */
  {
   /* (0008,0100) */ {ET_CodeValue,EType_n_1C},
   /* (0008,0102) */ {ET_CodingSchemeDesignator,EType_n_1C},
   /* (0008,0104) */ {ET_CodeMeaning,EType_n_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04G0434SQDiagnosisCodeSequence[] =  /* Interpretation Notification Event Information (Interpretation Approved 4) */
  {
   /* (4008,0117) */ {ET_DiagnosisCodeSequence,EType_n_2}
  };
S_ListDesc1 ModAttrT_Desc_04G0434SQDiagnosisCodeSequence = {E_ListIsTagTypeMemberWithCond,3,ModAttrT_04G0434SQDiagnosisCodeSequence, 0, 0,1,ModAttrT_SQ04G0434SQDiagnosisCodeSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04G0436[] =   /* Interpretation Notification Event Information (Interpretation Updated 6) */
  {
   {ET_UNKNOWN,EType_UNKNOWN}
  };
S_ListDesc1 ModAttrT_Desc_04G0436 = {E_ListIsTagTypeMember,0,ModAttrT_04G0436, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04H04121[] =   /* Basic Film Session SOP Class N-CREATE Attributes */
  {
   /* (2000,0010) */ {ET_NumberOfCopies,EType_U_M},
   /* (2000,0020) */ {ET_PrintPriority,EType_U_M},
   /* (2000,0030) */ {ET_MediumType,EType_U_M},
   /* (2000,0040) */ {ET_FilmDestination,EType_U_M},
   /* (2000,0050) */ {ET_FilmSessionLabel,EType_U_U},
   /* (2000,0060) */ {ET_MemoryAllocation,EType_U_U}
  };
S_ListDesc1 ModAttrT_Desc_04H04121 = {E_ListIsTagTypeMember,6,ModAttrT_04H04121, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04H04124[] =   /* Basic Film Session SOP Class N-ACTION Attributes */
  {
   {ET_UNKNOWN,EType_UNKNOWN}
  };
S_ListDesc1 ModAttrT_Desc_04H04124 = {E_ListIsTagTypeMember,0,ModAttrT_04H04124, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04H04124SQReferencedPrintJobSequence[] =  /* Basic Film Session SOP Class N-ACTION Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_n_MC},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_n_MC}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04H04124SQReferencedPrintJobSequence[] =  /* Basic Film Session SOP Class N-ACTION Attributes */
  {
   /* (2100,0500) */ {ET_ReferencedPrintJobSequence,EType_n_MC}
  };
S_ListDesc1 ModAttrT_Desc_04H04124SQReferencedPrintJobSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04H04124SQReferencedPrintJobSequence, 0, 0,1,ModAttrT_SQ04H04124SQReferencedPrintJobSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04H04221[] =   /* Basic Film Box SOP Class N-CREATE Attributes */
  {
   /* (2010,0010) */ {ET_ImageDisplayFormat,EType_M_M},
   /* (2010,0030) */ {ET_AnnotationDisplayFormatID,EType_U_U},
   /* (2010,0040) */ {ET_FilmOrientation,EType_U_M},
   /* (2010,0050) */ {ET_FilmSizeID,EType_U_M},
   /* (2010,0060) */ {ET_MagnificationType,EType_U_M},
   /* (2010,0080) */ {ET_SmoothingType,EType_U_U},
   /* (2010,0100) */ {ET_BorderDensity,EType_U_U},
   /* (2010,0110) */ {ET_EmptyImageDensity,EType_U_U},
   /* (2010,0120) */ {ET_MinDensity,EType_U_U},
   /* (2010,0130) */ {ET_MaxDensity,EType_U_M},
   /* (2010,0140) */ {ET_Trim,EType_U_U},
   /* (2010,0150) */ {ET_ConfigurationInformation,EType_U_M}
  };
S_ListDesc1 ModAttrT_Desc_04H04221 = {E_ListIsTagTypeMember,12,ModAttrT_04H04221, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04H04221SQReferencedFilmSessionSequence[] =  /* Basic Film Box SOP Class N-CREATE Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_M_M},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_M_M}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04H04221SQReferencedFilmSessionSequence[] =  /* Basic Film Box SOP Class N-CREATE Attributes */
  {
   /* (2010,0500) */ {ET_ReferencedFilmSessionSequence,EType_M_M}
  };
S_ListDesc1 ModAttrT_Desc_04H04221SQReferencedFilmSessionSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04H04221SQReferencedFilmSessionSequence, 0, 0,1,ModAttrT_SQ04H04221SQReferencedFilmSessionSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04H04221SQReferencedImageBoxSequence2010[] =  /* Basic Film Box SOP Class N-CREATE Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_n_M},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_n_M}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04H04221SQReferencedImageBoxSequence2010[] =  /* Basic Film Box SOP Class N-CREATE Attributes */
  {
   /* (2010,0510) */ {ET_ReferencedImageBoxSequence2010,EType_n_M}
  };
S_ListDesc1 ModAttrT_Desc_04H04221SQReferencedImageBoxSequence2010 = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04H04221SQReferencedImageBoxSequence2010, 0, 0,1,ModAttrT_SQ04H04221SQReferencedImageBoxSequence2010};

/* ModAttrTable references */
S_TagType ModAttrT_04H04221SQReferencedBasicAnnotationBoxSequence[] =  /* Basic Film Box SOP Class N-CREATE Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_n_MC},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_n_MC}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04H04221SQReferencedBasicAnnotationBoxSequence[] =  /* Basic Film Box SOP Class N-CREATE Attributes */
  {
   /* (2010,0520) */ {ET_ReferencedBasicAnnotationBoxSequence,EType_n_MC}
  };
S_ListDesc1 ModAttrT_Desc_04H04221SQReferencedBasicAnnotationBoxSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04H04221SQReferencedBasicAnnotationBoxSequence, 0, 0,1,ModAttrT_SQ04H04221SQReferencedBasicAnnotationBoxSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04H04222[] =   /* Basic Film Box SOP Class N-SET Attributes */
  {
   /* (2010,0060) */ {ET_MagnificationType,EType_U_M},
   /* (2010,0080) */ {ET_SmoothingType,EType_U_U},
   /* (2010,0100) */ {ET_BorderDensity,EType_U_U},
   /* (2010,0110) */ {ET_EmptyImageDensity,EType_U_U},
   /* (2010,0120) */ {ET_MinDensity,EType_U_U},
   /* (2010,0130) */ {ET_MaxDensity,EType_U_M},
   /* (2010,0140) */ {ET_Trim,EType_U_U},
   /* (2010,0150) */ {ET_ConfigurationInformation,EType_U_M}
  };
S_ListDesc1 ModAttrT_Desc_04H04222 = {E_ListIsTagTypeMember,8,ModAttrT_04H04222, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04H04224[] =   /* Basic Film Box SOP Class N-ACTION Attributes */
  {
   {ET_UNKNOWN,EType_UNKNOWN}
  };
S_ListDesc1 ModAttrT_Desc_04H04224 = {E_ListIsTagTypeMember,0,ModAttrT_04H04224, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04H04224SQReferencedPrintJobSequence[] =  /* Basic Film Box SOP Class N-ACTION Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_n_MC},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_n_MC}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04H04224SQReferencedPrintJobSequence[] =  /* Basic Film Box SOP Class N-ACTION Attributes */
  {
   /* (2100,0500) */ {ET_ReferencedPrintJobSequence,EType_n_MC}
  };
S_ListDesc1 ModAttrT_Desc_04H04224SQReferencedPrintJobSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04H04224SQReferencedPrintJobSequence, 0, 0,1,ModAttrT_SQ04H04224SQReferencedPrintJobSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04H043121[] =   /* Basic Grayscale Image Box SOP Class N-SET Attributes */
  {
   /* (2010,0060) */ {ET_MagnificationType,EType_U_U},
   /* (2010,0080) */ {ET_SmoothingType,EType_U_U},
   /* (2020,0010) */ {ET_ImagePosition2020,EType_M_M},
   /* (2020,0020) */ {ET_Polarity,EType_U_M},
   /* (2020,0030) */ {ET_RequestedImageSize,EType_U_U}
  };
S_ListDesc1 ModAttrT_Desc_04H043121 = {E_ListIsTagTypeMember,5,ModAttrT_04H043121, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04H043121SQReferencedOverlaySequence[] =  /* Basic Grayscale Image Box SOP Class N-SET Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_MC_M},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_MC_M}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04H043121SQReferencedOverlaySequence[] =  /* Basic Grayscale Image Box SOP Class N-SET Attributes */
  {
   /* (0008,1130) */ {ET_ReferencedOverlaySequence,EType_U_MC}
  };
S_ListDesc1 ModAttrT_Desc_04H043121SQReferencedOverlaySequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04H043121SQReferencedOverlaySequence, 0, 0,1,ModAttrT_SQ04H043121SQReferencedOverlaySequence};

/* ModAttrTable references */
S_TagType ModAttrT_04H043121SQPreformattedGrayscaleImageSequence[] =  /* Basic Grayscale Image Box SOP Class N-SET Attributes  */
  {
   /* (0028,0002) */ {ET_SamplesperPixel,EType_M_M},
   /* (0028,0004) */ {ET_PhotometricInterpretation,EType_M_M},
   /* (0028,0010) */ {ET_Rows0028,EType_M_M},
   /* (0028,0011) */ {ET_Columns0028,EType_M_M},
   /* (0028,0034) */ {ET_PixelAspectRatio,EType_M_M},
   /* (0028,0100) */ {ET_BitsAllocated0028,EType_M_M},
   /* (0028,0101) */ {ET_BitsStored,EType_M_M},
   /* (0028,0102) */ {ET_HighBit,EType_M_M},
   /* (0028,0103) */ {ET_PixelRepresentation,EType_M_M},
   /* (7FE0,0010) */ {ET_PixelData,EType_M_M}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04H043121SQPreformattedGrayscaleImageSequence[] =  /* Basic Grayscale Image Box SOP Class N-SET Attributes */
  {
   /* (2020,0110) */ {ET_PreformattedGrayscaleImageSequence,EType_M_M}
  };
S_ListDesc1 ModAttrT_Desc_04H043121SQPreformattedGrayscaleImageSequence = {E_ListIsTagTypeMemberWithCond,10,ModAttrT_04H043121SQPreformattedGrayscaleImageSequence, 0, 0,1,ModAttrT_SQ04H043121SQPreformattedGrayscaleImageSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04H043221[] =   /* Basic Color Image Box SOP Class N-SET Attributes */
  {
   /* (2010,0060) */ {ET_MagnificationType,EType_U_U},
   /* (2010,0080) */ {ET_SmoothingType,EType_U_U},
   /* (2020,0010) */ {ET_ImagePosition2020,EType_M_M},
   /* (2020,0020) */ {ET_Polarity,EType_U_M},
   /* (2020,0030) */ {ET_RequestedImageSize,EType_U_U}
  };
S_ListDesc1 ModAttrT_Desc_04H043221 = {E_ListIsTagTypeMember,5,ModAttrT_04H043221, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04H043221SQReferencedOverlaySequence[] =  /* Basic Color Image Box SOP Class N-SET Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_MC_M},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_MC_M}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04H043221SQReferencedOverlaySequence[] =  /* Basic Color Image Box SOP Class N-SET Attributes */
  {
   /* (0008,1130) */ {ET_ReferencedOverlaySequence,EType_U_MC}
  };
S_ListDesc1 ModAttrT_Desc_04H043221SQReferencedOverlaySequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04H043221SQReferencedOverlaySequence, 0, 0,1,ModAttrT_SQ04H043221SQReferencedOverlaySequence};

/* ModAttrTable references */
S_TagType ModAttrT_04H043321[] =   /* Referenced Image Box SOP Class N-SET Attributes */
  {
   /* (2010,0060) */ {ET_MagnificationType,EType_U_U},
   /* (2010,0080) */ {ET_SmoothingType,EType_U_U},
   /* (2020,0010) */ {ET_ImagePosition2020,EType_M_M},
   /* (2020,0020) */ {ET_Polarity,EType_U_M},
   /* (2020,0030) */ {ET_RequestedImageSize,EType_U_U}
  };
S_ListDesc1 ModAttrT_Desc_04H043321 = {E_ListIsTagTypeMember,5,ModAttrT_04H043321, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04H043321SQReferencedOverlaySequence[] =  /* Referenced Image Box SOP Class N-SET Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_MC_M},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_MC_M},
   /* (0008,1160) */ {ET_ReferencedFrameNumber,EType_MC_M}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04H043321SQReferencedOverlaySequence[] =  /* Referenced Image Box SOP Class N-SET Attributes */
  {
   /* (0008,1130) */ {ET_ReferencedOverlaySequence,EType_U_MC}
  };
S_ListDesc1 ModAttrT_Desc_04H043321SQReferencedOverlaySequence = {E_ListIsTagTypeMemberWithCond,3,ModAttrT_04H043321SQReferencedOverlaySequence, 0, 0,1,ModAttrT_SQ04H043321SQReferencedOverlaySequence};

/* ModAttrTable references */
S_TagType ModAttrT_04H043321SQReferencedImageSequence[] =  /* Referenced Image Box SOP Class N-SET Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_M_M},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_M_M},
   /* (0008,1160) */ {ET_ReferencedFrameNumber,EType_MC_M}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04H043321SQReferencedImageSequence[] =  /* Referenced Image Box SOP Class N-SET Attributes */
  {
   /* (0008,1140) */ {ET_ReferencedImageSequence,EType_M_M}
  };
S_ListDesc1 ModAttrT_Desc_04H043321SQReferencedImageSequence = {E_ListIsTagTypeMemberWithCond,3,ModAttrT_04H043321SQReferencedImageSequence, 0, 0,1,ModAttrT_SQ04H043321SQReferencedImageSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04H043321SQReferencedVOILUTBoxSequence[] =  /* Referenced Image Box SOP Class N-SET Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_MC_M},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_MC_M}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04H043321SQReferencedVOILUTBoxSequence[] =  /* Referenced Image Box SOP Class N-SET Attributes */
  {
   /* (2020,0140) */ {ET_ReferencedVOILUTBoxSequence,EType_U_M}
  };
S_ListDesc1 ModAttrT_Desc_04H043321SQReferencedVOILUTBoxSequence = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04H043321SQReferencedVOILUTBoxSequence, 0, 0,1,ModAttrT_SQ04H043321SQReferencedVOILUTBoxSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04H04421[] =   /* Basic Annotation Box SOP Class N-SET Attributes */
  {
   /* (2030,0010) */ {ET_AnnotationPosition,EType_M_M},
   /* (2030,0020) */ {ET_TextString,EType_U_M}
  };
S_ListDesc1 ModAttrT_Desc_04H04421 = {E_ListIsTagTypeMember,2,ModAttrT_04H04421, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04H045211[] =   /* Print Job SOP Class N-EVENT-REPORT Attributes (PENDING 1) */
  {
   /* (2000,0050) */ {ET_FilmSessionLabel,EType_U_U},
   /* (2100,0030) */ {ET_ExecutionStatusInfo,EType_U_M},
   /* (2110,0030) */ {ET_PrinterName,EType_U_U}
  };
S_ListDesc1 ModAttrT_Desc_04H045211 = {E_ListIsTagTypeMember,3,ModAttrT_04H045211, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04H045212[] =   /* Print Job SOP Class N-EVENT-REPORT Attributes (PRINTING 2) */
  {
   /* (2000,0050) */ {ET_FilmSessionLabel,EType_U_U},
   /* (2100,0030) */ {ET_ExecutionStatusInfo,EType_U_M},
   /* (2110,0030) */ {ET_PrinterName,EType_U_U}
  };
S_ListDesc1 ModAttrT_Desc_04H045212 = {E_ListIsTagTypeMember,3,ModAttrT_04H045212, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04H045213[] =   /* Print Job SOP Class N-EVENT-REPORT Attributes (DONE 3) */
  {
   /* (2000,0050) */ {ET_FilmSessionLabel,EType_U_U},
   /* (2100,0030) */ {ET_ExecutionStatusInfo,EType_U_M},
   /* (2110,0030) */ {ET_PrinterName,EType_U_U}
  };
S_ListDesc1 ModAttrT_Desc_04H045213 = {E_ListIsTagTypeMember,3,ModAttrT_04H045213, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04H045214[] =   /* Print Job SOP Class N-EVENT-REPORT Attributes (FAILURE 4) */
  {
   /* (2000,0050) */ {ET_FilmSessionLabel,EType_U_U},
   /* (2100,0030) */ {ET_ExecutionStatusInfo,EType_U_M},
   /* (2110,0030) */ {ET_PrinterName,EType_U_U}
  };
S_ListDesc1 ModAttrT_Desc_04H045214 = {E_ListIsTagTypeMember,3,ModAttrT_04H045214, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04H04522[] =   /* Print Job SOP Class N-GET Attributes */
  {
   /* (2000,0020) */ {ET_PrintPriority,EType_U_M},
   /* (2100,0020) */ {ET_ExecutionStatus,EType_U_M},
   /* (2100,0030) */ {ET_ExecutionStatusInfo,EType_U_M},
   /* (2100,0040) */ {ET_CreationDate,EType_U_U},
   /* (2100,0050) */ {ET_CreationTime,EType_U_U},
   /* (2100,0070) */ {ET_Originator,EType_U_U},
   /* (2110,0030) */ {ET_PrinterName,EType_U_U}
  };
S_ListDesc1 ModAttrT_Desc_04H04522 = {E_ListIsTagTypeMember,7,ModAttrT_04H04522, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04H046212[] =   /* PRINTER SOP Class N-EVENT-REPORT Attributes (WARNING 2) */
  {
   /* (2110,0020) */ {ET_PrinterStatusInfo,EType_U_M},
   /* (2110,0030) */ {ET_PrinterName,EType_U_U}
  };
S_ListDesc1 ModAttrT_Desc_04H046212 = {E_ListIsTagTypeMember,2,ModAttrT_04H046212, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04H046213[] =   /* PRINTER SOP Class N-EVENT-REPORT Attributes (FAILURE 3) */
  {
   /* (2110,0020) */ {ET_PrinterStatusInfo,EType_U_M},
   /* (2110,0030) */ {ET_PrinterName,EType_U_U}
  };
S_ListDesc1 ModAttrT_Desc_04H046213 = {E_ListIsTagTypeMember,2,ModAttrT_04H046213, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04H04622[] =   /* PRINTER SOP Class N-GET Attributes */
  {
   /* (0008,0070) */ {ET_Manufacturer,EType_U_U},
   /* (0008,1090) */ {ET_ManufacturersModelName,EType_U_U},
   /* (0018,1000) */ {ET_DeviceSerialNumber,EType_U_U},
   /* (0018,1020) */ {ET_SoftwareVersions,EType_U_U},
   /* (0018,1200) */ {ET_DateofLastCalibration,EType_U_U},
   /* (0018,1201) */ {ET_TimeofLastCalibration,EType_U_U},
   /* (2110,0010) */ {ET_PrinterStatus,EType_U_M},
   /* (2110,0020) */ {ET_PrinterStatusInfo,EType_U_M},
   /* (2110,0030) */ {ET_PrinterName,EType_U_U}
  };
S_ListDesc1 ModAttrT_Desc_04H04622 = {E_ListIsTagTypeMember,9,ModAttrT_04H04622, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04H04721[] =   /* VOI LUT Box SOP Class N-CREATE Attributes */
  {
   /* (0028,1050) */ {ET_WindowCenter,EType_U_M},
   /* (0028,1051) */ {ET_WindowWidth,EType_U_M},
   /* (0028,1055) */ {ET_WindowCenter_WidthExplanation,EType_U_M}
  };
S_ListDesc1 ModAttrT_Desc_04H04721 = {E_ListIsTagTypeMember,3,ModAttrT_04H04721, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04H04721SQVOILUTSequence[] =  /* VOI LUT Box SOP Class N-CREATE Attributes  */
  {
   /* (0028,3002) */ {ET_LUTDescriptor,EType_MC_M},
   /* (0028,3003) */ {ET_LUTExplanation,EType_U_U},
   /* (0028,3006) */ {ET_LUTData,EType_MC_M}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04H04721SQVOILUTSequence[] =  /* VOI LUT Box SOP Class N-CREATE Attributes */
  {
   /* (0028,3010) */ {ET_VOILUTSequence,EType_U_M}
  };
S_ListDesc1 ModAttrT_Desc_04H04721SQVOILUTSequence = {E_ListIsTagTypeMemberWithCond,3,ModAttrT_04H04721SQVOILUTSequence, 0, 0,1,ModAttrT_SQ04H04721SQVOILUTSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04H04821[] =   /* Image Overlay Box SOP Class N-CREATE Attributes */
  {
   /* (2040,0060) */ {ET_OverlayMagnificationType,EType_U_U},
   /* (2040,0070) */ {ET_OverlaySmoothingType,EType_U_U},
   /* (2040,0080) */ {ET_OverlayForegroundDensity,EType_U_U},
   /* (2040,0090) */ {ET_OverlayMode,EType_U_U},
   /* (2040,0100) */ {ET_ThresholdDensity,EType_U_U}
  };
S_ListDesc1 ModAttrT_Desc_04H04821 = {E_ListIsTagTypeMember,5,ModAttrT_04H04821, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04H04821SQReferencedOverlayPlaneSequence[] =  /* Image Overlay Box SOP Class N-CREATE Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_MC_M},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_MC_M},
   /* (2040,0011) */ {ET_ReferencedOverlayPlaneGroups,EType_MC_M}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04H04821SQReferencedOverlayPlaneSequence[] =  /* Image Overlay Box SOP Class N-CREATE Attributes */
  {
   /* (2040,0010) */ {ET_ReferencedOverlayPlaneSequence,EType_U_M}
  };
S_ListDesc1 ModAttrT_Desc_04H04821SQReferencedOverlayPlaneSequence = {E_ListIsTagTypeMemberWithCond,3,ModAttrT_04H04821SQReferencedOverlayPlaneSequence, 0, 0,1,ModAttrT_SQ04H04821SQReferencedOverlayPlaneSequence};

/* ModAttrTable references */
S_TagType ModAttrT_04H04821SQReferencedImageBoxSequence2040[] =  /* Image Overlay Box SOP Class N-CREATE Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_M_M},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_M_M}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04H04821SQReferencedImageBoxSequence2040[] =  /* Image Overlay Box SOP Class N-CREATE Attributes */
  {
   /* (2040,0500) */ {ET_ReferencedImageBoxSequence2040,EType_M_M}
  };
S_ListDesc1 ModAttrT_Desc_04H04821SQReferencedImageBoxSequence2040 = {E_ListIsTagTypeMemberWithCond,2,ModAttrT_04H04821SQReferencedImageBoxSequence2040, 0, 0,1,ModAttrT_SQ04H04821SQReferencedImageBoxSequence2040};

/* ModAttrTable references */
S_TagType ModAttrT_04H04822[] =   /* Image Overlay Box SOP Class N-SET Attributes */
  {
   /* (2040,0060) */ {ET_OverlayMagnificationType,EType_U_U},
   /* (2040,0070) */ {ET_OverlaySmoothingType,EType_U_U},
   /* (2040,0080) */ {ET_OverlayForegroundDensity,EType_U_U},
   /* (2040,0090) */ {ET_OverlayMode,EType_U_U},
   /* (2040,0100) */ {ET_ThresholdDensity,EType_U_U}
  };
S_ListDesc1 ModAttrT_Desc_04H04822 = {E_ListIsTagTypeMember,5,ModAttrT_04H04822, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_04H04822SQReferencedOverlayPlaneSequence[] =  /* Image Overlay Box SOP Class N-SET Attributes  */
  {
   /* (0008,1150) */ {ET_ReferencedSOPClassUID,EType_MC_M},
   /* (0008,1155) */ {ET_ReferencedSOPInstanceUID,EType_MC_M},
   /* (2040,0011) */ {ET_ReferencedOverlayPlaneGroups,EType_MC_M}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ04H04822SQReferencedOverlayPlaneSequence[] =  /* Image Overlay Box SOP Class N-SET Attributes */
  {
   /* (2040,0010) */ {ET_ReferencedOverlayPlaneSequence,EType_U_M}
  };
S_ListDesc1 ModAttrT_Desc_04H04822SQReferencedOverlayPlaneSequence = {E_ListIsTagTypeMemberWithCond,3,ModAttrT_04H04822SQReferencedOverlayPlaneSequence, 0, 0,1,ModAttrT_SQ04H04822SQReferencedOverlayPlaneSequence};

/* ModAttrTable references */
S_Tag ModAttrT_07090311[] =   /* C-STORE-RQ Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0110) */ ET_MessageID,
   /* (0000,0700) */ ET_Priority,
   /* (0000,0800) */ ET_DataSetType0000,
   /* (0000,1000) */ ET_AffectedSOPInstanceUID,
   /* (0000,1030) */ ET_MoveOriginatorApplicationEntityTitle,
   /* (0000,1031) */ ET_MoveOriginatorMessageID
  };
S_ListDesc1 ModAttrT_Desc_07090311 = {E_ListIsTagMember,9,ModAttrT_07090311, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07090312[] =   /* C-STORE-RSP Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0120) */ ET_MessageIDBeingRespondedTo,
   /* (0000,0800) */ ET_DataSetType0000,
   /* (0000,0900) */ ET_Status,
   /* (0000,1000) */ ET_AffectedSOPInstanceUID
  };
S_ListDesc1 ModAttrT_Desc_07090312 = {E_ListIsTagMember,7,ModAttrT_07090312, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07090321[] =   /* C-FIND-RQ Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0110) */ ET_MessageID,
   /* (0000,0700) */ ET_Priority,
   /* (0000,0800) */ ET_DataSetType0000
  };
S_ListDesc1 ModAttrT_Desc_07090321 = {E_ListIsTagMember,6,ModAttrT_07090321, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07090322[] =   /* C-FIND-RSP Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0120) */ ET_MessageIDBeingRespondedTo,
   /* (0000,0800) */ ET_DataSetType0000,
   /* (0000,0900) */ ET_Status
  };
S_ListDesc1 ModAttrT_Desc_07090322 = {E_ListIsTagMember,6,ModAttrT_07090322, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07090323[] =   /* C-CANCEL-FIND-RQ Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0120) */ ET_MessageIDBeingRespondedTo,
   /* (0000,0800) */ ET_DataSetType0000
  };
S_ListDesc1 ModAttrT_Desc_07090323 = {E_ListIsTagMember,4,ModAttrT_07090323, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07090331[] =   /* C-GET-RQ Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0110) */ ET_MessageID,
   /* (0000,0700) */ ET_Priority,
   /* (0000,0800) */ ET_DataSetType0000
  };
S_ListDesc1 ModAttrT_Desc_07090331 = {E_ListIsTagMember,6,ModAttrT_07090331, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07090332[] =   /* C-GET-RSP Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0120) */ ET_MessageIDBeingRespondedTo,
   /* (0000,0800) */ ET_DataSetType0000,
   /* (0000,0900) */ ET_Status,
   /* (0000,1020) */ ET_NumberofRemainingSuboperations,
   /* (0000,1021) */ ET_NumberofCompletedSuboperations,
   /* (0000,1022) */ ET_NumberofFailedSuboperations,
   /* (0000,1023) */ ET_NumberofWarningSuboperations
  };
S_ListDesc1 ModAttrT_Desc_07090332 = {E_ListIsTagMember,10,ModAttrT_07090332, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07090333[] =   /* C-CANCEL-GET-RQ Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0120) */ ET_MessageIDBeingRespondedTo,
   /* (0000,0800) */ ET_DataSetType0000
  };
S_ListDesc1 ModAttrT_Desc_07090333 = {E_ListIsTagMember,4,ModAttrT_07090333, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07090341[] =   /* C-MOVE-RQ Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0110) */ ET_MessageID,
   /* (0000,0600) */ ET_MoveDestination,
   /* (0000,0700) */ ET_Priority,
   /* (0000,0800) */ ET_DataSetType0000
  };
S_ListDesc1 ModAttrT_Desc_07090341 = {E_ListIsTagMember,7,ModAttrT_07090341, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07090342[] =   /* C-MOVE-RSP Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0120) */ ET_MessageIDBeingRespondedTo,
   /* (0000,0800) */ ET_DataSetType0000,
   /* (0000,0900) */ ET_Status,
   /* (0000,1020) */ ET_NumberofRemainingSuboperations,
   /* (0000,1021) */ ET_NumberofCompletedSuboperations,
   /* (0000,1022) */ ET_NumberofFailedSuboperations,
   /* (0000,1023) */ ET_NumberofWarningSuboperations
  };
S_ListDesc1 ModAttrT_Desc_07090342 = {E_ListIsTagMember,10,ModAttrT_07090342, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07090343[] =   /* C-CANCEL-MOVE-RQ Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0120) */ ET_MessageIDBeingRespondedTo,
   /* (0000,0800) */ ET_DataSetType0000
  };
S_ListDesc1 ModAttrT_Desc_07090343 = {E_ListIsTagMember,4,ModAttrT_07090343, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07090351[] =   /* C-ECHO-RQ Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0110) */ ET_MessageID,
   /* (0000,0800) */ ET_DataSetType0000
  };
S_ListDesc1 ModAttrT_Desc_07090351 = {E_ListIsTagMember,5,ModAttrT_07090351, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07090352[] =   /* C-ECHO-RSP Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0120) */ ET_MessageIDBeingRespondedTo,
   /* (0000,0800) */ ET_DataSetType0000,
   /* (0000,0900) */ ET_Status
  };
S_ListDesc1 ModAttrT_Desc_07090352 = {E_ListIsTagMember,6,ModAttrT_07090352, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07100311[] =   /* N-EVENT-REPORT-RQ Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0110) */ ET_MessageID,
   /* (0000,0800) */ ET_DataSetType0000,
   /* (0000,1000) */ ET_AffectedSOPInstanceUID,
   /* (0000,1002) */ ET_EventTypeID
  };
S_ListDesc1 ModAttrT_Desc_07100311 = {E_ListIsTagMember,7,ModAttrT_07100311, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07100312[] =   /* N-EVENT-REPORT-RSP Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0120) */ ET_MessageIDBeingRespondedTo,
   /* (0000,0800) */ ET_DataSetType0000,
   /* (0000,0900) */ ET_Status,
   /* (0000,1000) */ ET_AffectedSOPInstanceUID,
   /* (0000,1002) */ ET_EventTypeID
  };
S_ListDesc1 ModAttrT_Desc_07100312 = {E_ListIsTagMember,8,ModAttrT_07100312, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07100321[] =   /* N-GET-RQ Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0003) */ ET_RequestedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0110) */ ET_MessageID,
   /* (0000,0800) */ ET_DataSetType0000,
   /* (0000,1001) */ ET_RequestedSOPInstanceUID,
   /* (0000,1005) */ ET_AttributeIdentifierList
  };
S_ListDesc1 ModAttrT_Desc_07100321 = {E_ListIsTagMember,7,ModAttrT_07100321, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07100322[] =   /* N-GET-RSP Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0120) */ ET_MessageIDBeingRespondedTo,
   /* (0000,0800) */ ET_DataSetType0000,
   /* (0000,0900) */ ET_Status,
   /* (0000,1000) */ ET_AffectedSOPInstanceUID
  };
S_ListDesc1 ModAttrT_Desc_07100322 = {E_ListIsTagMember,7,ModAttrT_07100322, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07100331[] =   /* N-SET-RQ Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0003) */ ET_RequestedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0110) */ ET_MessageID,
   /* (0000,0800) */ ET_DataSetType0000,
   /* (0000,1001) */ ET_RequestedSOPInstanceUID
  };
S_ListDesc1 ModAttrT_Desc_07100331 = {E_ListIsTagMember,6,ModAttrT_07100331, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07100332[] =   /* N-SET-RSP Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0120) */ ET_MessageIDBeingRespondedTo,
   /* (0000,0800) */ ET_DataSetType0000,
   /* (0000,0900) */ ET_Status,
   /* (0000,1000) */ ET_AffectedSOPInstanceUID
  };
S_ListDesc1 ModAttrT_Desc_07100332 = {E_ListIsTagMember,7,ModAttrT_07100332, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07100341[] =   /* N-ACTION-RQ Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0003) */ ET_RequestedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0110) */ ET_MessageID,
   /* (0000,0800) */ ET_DataSetType0000,
   /* (0000,1001) */ ET_RequestedSOPInstanceUID,
   /* (0000,1008) */ ET_ActionTypeID
  };
S_ListDesc1 ModAttrT_Desc_07100341 = {E_ListIsTagMember,7,ModAttrT_07100341, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07100342[] =   /* N-ACTION-RSP Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0120) */ ET_MessageIDBeingRespondedTo,
   /* (0000,0800) */ ET_DataSetType0000,
   /* (0000,0900) */ ET_Status,
   /* (0000,1000) */ ET_AffectedSOPInstanceUID,
   /* (0000,1008) */ ET_ActionTypeID
  };
S_ListDesc1 ModAttrT_Desc_07100342 = {E_ListIsTagMember,8,ModAttrT_07100342, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07100351[] =   /* N-CREATE-RQ Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0110) */ ET_MessageID,
   /* (0000,0800) */ ET_DataSetType0000,
   /* (0000,1000) */ ET_AffectedSOPInstanceUID
  };
S_ListDesc1 ModAttrT_Desc_07100351 = {E_ListIsTagMember,6,ModAttrT_07100351, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07100352[] =   /* N-CREATE-RSP Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0120) */ ET_MessageIDBeingRespondedTo,
   /* (0000,0800) */ ET_DataSetType0000,
   /* (0000,0900) */ ET_Status,
   /* (0000,1000) */ ET_AffectedSOPInstanceUID
  };
S_ListDesc1 ModAttrT_Desc_07100352 = {E_ListIsTagMember,7,ModAttrT_07100352, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07100361[] =   /* N-DELETE-RQ Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0003) */ ET_RequestedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0110) */ ET_MessageID,
   /* (0000,0800) */ ET_DataSetType0000,
   /* (0000,1001) */ ET_RequestedSOPInstanceUID
  };
S_ListDesc1 ModAttrT_Desc_07100361 = {E_ListIsTagMember,6,ModAttrT_07100361, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07100362[] =   /* N-DELETE-RSP Message Fields */
  {
   /* (0000,0000) */ ET_GroupLength0000,
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0100) */ ET_CommandField,
   /* (0000,0120) */ ET_MessageIDBeingRespondedTo,
   /* (0000,0800) */ ET_DataSetType0000,
   /* (0000,0900) */ ET_Status,
   /* (0000,1000) */ ET_AffectedSOPInstanceUID
  };
S_ListDesc1 ModAttrT_Desc_07100362 = {E_ListIsTagMember,7,ModAttrT_07100362, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0101[] =   /* Success */
  {
   /* (0000,0900) */ ET_Status
  };
S_ListDesc1 ModAttrT_Desc_07C0101 = {E_ListIsTagMember,1,ModAttrT_07C0101, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0201[] =   /* Pending */
  {
   /* (0000,0900) */ ET_Status
  };
S_ListDesc1 ModAttrT_Desc_07C0201 = {E_ListIsTagMember,1,ModAttrT_07C0201, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0301[] =   /* Cancel */
  {
   /* (0000,0900) */ ET_Status
  };
S_ListDesc1 ModAttrT_Desc_07C0301 = {E_ListIsTagMember,1,ModAttrT_07C0301, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0401[] =   /* Warning */
  {
   /* (0000,0900) */ ET_Status,
   /* (0000,0901) */ ET_OffendingElement,
   /* (0000,0902) */ ET_ErrorComment
  };
S_ListDesc1 ModAttrT_Desc_07C0401 = {E_ListIsTagMember,3,ModAttrT_07C0401, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0402[] =   /* Attribute List Error */
  {
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0900) */ ET_Status,
   /* (0000,1000) */ ET_AffectedSOPInstanceUID,
   /* (0000,1005) */ ET_AttributeIdentifierList
  };
S_ListDesc1 ModAttrT_Desc_07C0402 = {E_ListIsTagMember,4,ModAttrT_07C0402, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0501[] =   /* Error: Cannot Understand */
  {
   /* (0000,0900) */ ET_Status,
   /* (0000,0901) */ ET_OffendingElement,
   /* (0000,0902) */ ET_ErrorComment
  };
S_ListDesc1 ModAttrT_Desc_07C0501 = {E_ListIsTagMember,3,ModAttrT_07C0501, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0502[] =   /* Error: Data Set does not Match SOP Class */
  {
   /* (0000,0900) */ ET_Status,
   /* (0000,0901) */ ET_OffendingElement,
   /* (0000,0902) */ ET_ErrorComment
  };
S_ListDesc1 ModAttrT_Desc_07C0502 = {E_ListIsTagMember,3,ModAttrT_07C0502, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0503[] =   /* Failed */
  {
   /* (0000,0900) */ ET_Status,
   /* (0000,0901) */ ET_OffendingElement,
   /* (0000,0902) */ ET_ErrorComment
  };
S_ListDesc1 ModAttrT_Desc_07C0503 = {E_ListIsTagMember,3,ModAttrT_07C0503, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0504[] =   /* Refused: Move Destination Unknown */
  {
   /* (0000,0900) */ ET_Status,
   /* (0000,0902) */ ET_ErrorComment
  };
S_ListDesc1 ModAttrT_Desc_07C0504 = {E_ListIsTagMember,2,ModAttrT_07C0504, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0505[] =   /* Refused: Out of Resources */
  {
   /* (0000,0900) */ ET_Status,
   /* (0000,0902) */ ET_ErrorComment
  };
S_ListDesc1 ModAttrT_Desc_07C0505 = {E_ListIsTagMember,2,ModAttrT_07C0505, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0506[] =   /* Refused: SOP Class Not Supported */
  {
   /* (0000,0900) */ ET_Status,
   /* (0000,0902) */ ET_ErrorComment
  };
S_ListDesc1 ModAttrT_Desc_07C0506 = {E_ListIsTagMember,2,ModAttrT_07C0506, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0507[] =   /* Class-Instance Conflict */
  {
   /* (0000,0003) */ ET_RequestedSOPClassUID,
   /* (0000,0900) */ ET_Status,
   /* (0000,1001) */ ET_RequestedSOPInstanceUID
  };
S_ListDesc1 ModAttrT_Desc_07C0507 = {E_ListIsTagMember,3,ModAttrT_07C0507, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0508[] =   /* Duplicate SOP Instance */
  {
   /* (0000,0900) */ ET_Status,
   /* (0000,1000) */ ET_AffectedSOPInstanceUID
  };
S_ListDesc1 ModAttrT_Desc_07C0508 = {E_ListIsTagMember,2,ModAttrT_07C0508, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0509[] =   /* Duplicate Invocation */
  {
   /* (0000,0900) */ ET_Status
  };
S_ListDesc1 ModAttrT_Desc_07C0509 = {E_ListIsTagMember,1,ModAttrT_07C0509, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0510[] =   /* Invalid Argument Value */
  {
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0900) */ ET_Status,
   /* (0000,1000) */ ET_AffectedSOPInstanceUID,
   /* (0000,1002) */ ET_EventTypeID,
   /* (0000,1008) */ ET_ActionTypeID
  };
S_ListDesc1 ModAttrT_Desc_07C0510 = {E_ListIsTagMember,5,ModAttrT_07C0510, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0511[] =   /* Invalid Attribute Value */
  {
   /* (0000,0900) */ ET_Status
  };
S_ListDesc1 ModAttrT_Desc_07C0511 = {E_ListIsTagMember,1,ModAttrT_07C0511, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0512[] =   /* Invalid Object Instance */
  {
   /* (0000,0900) */ ET_Status,
   /* (0000,1000) */ ET_AffectedSOPInstanceUID
  };
S_ListDesc1 ModAttrT_Desc_07C0512 = {E_ListIsTagMember,2,ModAttrT_07C0512, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0513[] =   /* Missing Attribute */
  {
   /* (0000,0900) */ ET_Status,
   /* (0000,1005) */ ET_AttributeIdentifierList
  };
S_ListDesc1 ModAttrT_Desc_07C0513 = {E_ListIsTagMember,2,ModAttrT_07C0513, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0514[] =   /* Missing Attribute Value */
  {
   /* (0000,0900) */ ET_Status
  };
S_ListDesc1 ModAttrT_Desc_07C0514 = {E_ListIsTagMember,1,ModAttrT_07C0514, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0515[] =   /* Mistyped Argument */
  {
   /* (0000,0900) */ ET_Status
  };
S_ListDesc1 ModAttrT_Desc_07C0515 = {E_ListIsTagMember,1,ModAttrT_07C0515, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0516[] =   /* No Such Argument */
  {
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0900) */ ET_Status,
   /* (0000,1002) */ ET_EventTypeID,
   /* (0000,1008) */ ET_ActionTypeID
  };
S_ListDesc1 ModAttrT_Desc_07C0516 = {E_ListIsTagMember,4,ModAttrT_07C0516, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0517[] =   /* No Such Attribute */
  {
   /* (0000,0900) */ ET_Status,
   /* (0000,1005) */ ET_AttributeIdentifierList
  };
S_ListDesc1 ModAttrT_Desc_07C0517 = {E_ListIsTagMember,2,ModAttrT_07C0517, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0518[] =   /* No Such Event Type */
  {
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0900) */ ET_Status,
   /* (0000,1002) */ ET_EventTypeID
  };
S_ListDesc1 ModAttrT_Desc_07C0518 = {E_ListIsTagMember,3,ModAttrT_07C0518, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0519[] =   /* No Such Object Instance */
  {
   /* (0000,0900) */ ET_Status,
   /* (0000,1000) */ ET_AffectedSOPInstanceUID
  };
S_ListDesc1 ModAttrT_Desc_07C0519 = {E_ListIsTagMember,2,ModAttrT_07C0519, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0520[] =   /* No Such SOP Class */
  {
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0900) */ ET_Status
  };
S_ListDesc1 ModAttrT_Desc_07C0520 = {E_ListIsTagMember,2,ModAttrT_07C0520, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0521[] =   /* Processing Failure */
  {
   /* (0000,0002) */ ET_AffectedSOPClassUID,
   /* (0000,0900) */ ET_Status,
   /* (0000,0902) */ ET_ErrorComment,
   /* (0000,0903) */ ET_ErrorID,
   /* (0000,1000) */ ET_AffectedSOPInstanceUID
  };
S_ListDesc1 ModAttrT_Desc_07C0521 = {E_ListIsTagMember,5,ModAttrT_07C0521, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0522[] =   /* Resource Limitation */
  {
   /* (0000,0900) */ ET_Status
  };
S_ListDesc1 ModAttrT_Desc_07C0522 = {E_ListIsTagMember,1,ModAttrT_07C0522, 0, 0, 0,NULL};

/* ModAttrTable references */
S_Tag ModAttrT_07C0523[] =   /* Unrecognized Operation */
  {
   /* (0000,0900) */ ET_Status
  };
S_ListDesc1 ModAttrT_Desc_07C0523 = {E_ListIsTagMember,1,ModAttrT_07C0523, 0, 0, 0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_100701[] =   /* DICOM File Meta Information */
  {
   /* (0002,0000) */ {ET_GroupLength0002,EType_1},
   /* (0002,0001) */ {ET_FileMetaInformationVersion,EType_1},
   /* (0002,0002) */ {ET_MediaStoredSOPClassUID,EType_1},
   /* (0002,0003) */ {ET_MediaStoredSOPInstanceUID,EType_1},
   /* (0002,0010) */ {ET_TransferSyntaxUID,EType_1},
   /* (0002,0012) */ {ET_ImplementationClassUID,EType_1},
   /* (0002,0013) */ {ET_ImplementationVersionName,EType_3},
   /* (0002,0016) */ {ET_SourceApplicationEntityTitle,EType_3},
   /* (0002,0100) */ {ET_PrivateInformationCreatorUID,EType_3},
   /* (0002,0102) */ {ET_PrivateInformation,EType_1C}
  };
S_ListDesc1 ModAttrT_Desc_100701 = {E_ListIsTagTypeMember,10,ModAttrT_100701, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_10B0321[] =   /* File-set Identification Module */
  {
   /* (0004,1130) */ {ET_FilesetID,EType_2},
   /* (0004,1141) */ {ET_FileIDofFilesetDescriptorFile,EType_3},
   /* (0004,1142) */ {ET_FormatoftheFilesetDescriptorFile,EType_2C}
  };
S_ListDesc1 ModAttrT_Desc_10B0321 = {E_ListIsTagTypeMember,3,ModAttrT_10B0321, 0, 0,0,NULL};

/* ModAttrTable references */
char *ModAttrT_10B0321V1FormatoftheFilesetDescriptorFile[] =   /* File-set Identification Module */
  {
   "DICOM TEXT"
  };
S_ListDesc1 ModAttrT_Desc_10B0321V1FormatoftheFilesetDescriptorFile = {E_ListIsVal1Member,1,ModAttrT_10B0321V1FormatoftheFilesetDescriptorFile,ET_FormatoftheFilesetDescriptorFile,EType_2C,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_10B0322[] =   /* Directory Information Module */
  {
   /* (0004,1200) */ {ET_OffsetoftheFirstDirectoryRecordoftheRootDirectoryEntity,EType_1},
   /* (0004,1202) */ {ET_OffsetoftheLastDirectoryRecordoftheRootDirectoryEntity,EType_1},
   /* (0004,1212) */ {ET_FilesetConsistencyFlag,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_10B0322 = {E_ListIsTagTypeMember,3,ModAttrT_10B0322, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_10B0322SQDirectoryRecordSequence[] =  /* Directory Information Module  */
  {
   /* (0004,1400) */ {ET_OffsetoftheNextDirectoryRecord,EType_1C},
   /* (0004,1410) */ {ET_RecordInuseFlag,EType_1C},
   /* (0004,1420) */ {ET_OffsetofReferencedLowerLevelDirectoryEntity,EType_1C},
   /* (0004,1430) */ {ET_DirectoryRecordType,EType_1C},
   /* (0004,1432) */ {ET_PrivateRecordUID,EType_1C},
   /* (0004,1500) */ {ET_ReferencedFileID,EType_2C},
   /* (0004,1504) */ {ET_MRDRDirectoryRecordOffset,EType_1C},
   /* (0004,1510) */ {ET_ReferencedSOPClassUIDinFile,EType_1C},
   /* (0004,1511) */ {ET_ReferencedSOPInstanceUIDinFile,EType_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ10B0322SQDirectoryRecordSequence[] =  /* Directory Information Module */
  {
   /* (0004,1220) */ {ET_DirectoryRecordSequence,EType_2}
  };
S_ListDesc1 ModAttrT_Desc_10B0322SQDirectoryRecordSequence = {E_ListIsTagTypeMemberWithCond,9,ModAttrT_10B0322SQDirectoryRecordSequence, 0, 0,1,ModAttrT_SQ10B0322SQDirectoryRecordSequence};

/* ModAttrTable references */
S_List2Str ModAttrT_10B0322V1FilesetConsistencyFlag[] =   /* Directory Information Module */
  {
   {"0000H","no known inconsistencies"},
   {"FFFFH","The FSR or FSU shall assume that inconsistencies are present"}
  };
S_ListDesc1 ModAttrT_Desc_10B0322V1FilesetConsistencyFlag = {E_ListIsVal1MemberWithAddVal,2,ModAttrT_10B0322V1FilesetConsistencyFlag,ET_FilesetConsistencyFlag,EType_1,0,NULL};

/* ModAttrTable references */
S_List2Str ModAttrT_10B0322V1RecordInuseFlag[] =   /* Directory Information Module */
  {
   {"FFFFH","record is in use"},
   {"0000H","record is inactive"}
  };
S_ListDesc1 ModAttrT_Desc_10B0322V1RecordInuseFlag = {E_ListIsVal1MemberWithAddVal,2,ModAttrT_10B0322V1RecordInuseFlag,ET_RecordInuseFlag,EType_1C,0,NULL};

/* ModAttrTable references */
char *ModAttrT_10B0322V1DirectoryRecordType[] =   /* Directory Information Module */
  {
   "CURVE",
   "FILM BOX",
   "FILM SESSION",
   "IMAGE",
   "IMAGEBOX",
   "INTERPRETATION",
   "MODALITY LUT",
   "MRDR",
   "OVERLAY",
   "PATIENT",
   "PRINT QUEUE",
   "PRIVATE",
   "RESULTS",
   "SERIES",
   "STUDY",
   "STUDY COMPONENT",
   "TOPIC",
   "VISIT",
   "VOI LUT"
  };
S_ListDesc1 ModAttrT_Desc_10B0322V1DirectoryRecordType = {E_ListIsVal1Member,19,ModAttrT_10B0322V1DirectoryRecordType,ET_DirectoryRecordType,EType_1C,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_10B0501[] =   /* Patient Keys */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_1C},
   /* (0010,0010) */ {ET_PatientsName,EType_2},
   /* (0010,0020) */ {ET_PatientID,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_10B0501 = {E_ListIsTagTypeMember,3,ModAttrT_10B0501, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_10B0502[] =   /* Study Keys */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_1C},
   /* (0008,0020) */ {ET_StudyDate,EType_1},
   /* (0008,0030) */ {ET_StudyTime,EType_1},
   /* (0008,0050) */ {ET_AccessionNumber,EType_2},
   /* (0008,1030) */ {ET_StudyDescription,EType_2},
   /* (0020,000D) */ {ET_StudyInstanceUID,EType_1C},
   /* (0020,0010) */ {ET_StudyID,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_10B0502 = {E_ListIsTagTypeMember,7,ModAttrT_10B0502, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_10B0503[] =   /* Series Keys */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_1C},
   /* (0020,000E) */ {ET_SeriesInstanceUID,EType_1},
   /* (0020,0011) */ {ET_SeriesNumber,EType_1},
   /* (0088,0200) */ {ET_IconImage,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_10B0503 = {E_ListIsTagTypeMember,4,ModAttrT_10B0503, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_10B0504[] =   /* Image Keys */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_1C},
   /* (0020,0013) */ {ET_ImageNumber,EType_1},
   /* (0088,0200) */ {ET_IconImage,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_10B0504 = {E_ListIsTagTypeMember,3,ModAttrT_10B0504, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_10B0505[] =   /* Overlay Keys */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_1C},
   /* (0020,0022) */ {ET_OverlayNumber,EType_1},
   /* (0088,0200) */ {ET_IconImage,EType_3}
  };
S_ListDesc1 ModAttrT_Desc_10B0505 = {E_ListIsTagTypeMember,3,ModAttrT_10B0505, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_10B0506[] =   /* Modality LUT Keys */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_1C},
   /* (0020,0026) */ {ET_LookupTableNumber,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_10B0506 = {E_ListIsTagTypeMember,2,ModAttrT_10B0506, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_10B0507[] =   /* VOI LUT Keys */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_1C},
   /* (0020,0026) */ {ET_LookupTableNumber,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_10B0507 = {E_ListIsTagTypeMember,2,ModAttrT_10B0507, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_10B0508[] =   /* Curve Keys */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_1C},
   /* (0020,0024) */ {ET_CurveNumber,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_10B0508 = {E_ListIsTagTypeMember,2,ModAttrT_10B0508, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_10B0509[] =   /* Topic Keys */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_1C},
   /* (0088,0904) */ {ET_TopicTitle,EType_1},
   /* (0088,0906) */ {ET_TopicSubject,EType_2},
   /* (0088,0910) */ {ET_TopicAuthor,EType_2},
   /* (0088,0912) */ {ET_TopicKeyWords,EType_2}
  };
S_ListDesc1 ModAttrT_Desc_10B0509 = {E_ListIsTagTypeMember,5,ModAttrT_10B0509, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_10B0510[] =   /* Visit Keys */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_1C},
   /* (0008,0080) */ {ET_InstitutionName,EType_2},
   /* (0038,0010) */ {ET_AdmissionID,EType_2},
   /* (0038,0020) */ {ET_AdmittingDate,EType_2}
  };
S_ListDesc1 ModAttrT_Desc_10B0510 = {E_ListIsTagTypeMember,4,ModAttrT_10B0510, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_10B0511[] =   /* Result Keys */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_1C},
   /* (0008,0012) */ {ET_InstanceCreationDate,EType_2},
   /* (4008,0040) */ {ET_ResultsID,EType_2}
  };
S_ListDesc1 ModAttrT_Desc_10B0511 = {E_ListIsTagTypeMember,3,ModAttrT_10B0511, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_10B0512[] =   /* Interpretation Keys */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_1C},
   /* (4008,0108) */ {ET_InterpretationTranscriptionDate,EType_1},
   /* (4008,010C) */ {ET_InterpretationAuthor,EType_2},
   /* (4008,0115) */ {ET_InterpretationDiagnosisDescription,EType_2},
   /* (4008,0200) */ {ET_InterpretationID,EType_1},
   /* (4008,0210) */ {ET_InterpretationTypeID,EType_2},
   /* (4008,0212) */ {ET_InterpretationStatusID,EType_2}
  };
S_ListDesc1 ModAttrT_Desc_10B0512 = {E_ListIsTagTypeMember,7,ModAttrT_10B0512, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_10B0512SQDiagnosisCodeSequence[] =  /* Interpretation Keys  */
  {
   /* (0008,0100) */ {ET_CodeValue,EType_1C},
   /* (0008,0102) */ {ET_CodingSchemeDesignator,EType_1C},
   /* (0008,0104) */ {ET_CodeMeaning,EType_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ10B0512SQDiagnosisCodeSequence[] =  /* Interpretation Keys */
  {
   /* (4008,0117) */ {ET_DiagnosisCodeSequence,EType_2}
  };
S_ListDesc1 ModAttrT_Desc_10B0512SQDiagnosisCodeSequence = {E_ListIsTagTypeMemberWithCond,3,ModAttrT_10B0512SQDiagnosisCodeSequence, 0, 0,1,ModAttrT_SQ10B0512SQDiagnosisCodeSequence};

/* ModAttrTable references */
S_TagType ModAttrT_10B0513[] =   /* Study Component Keys */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_1C},
   /* (0008,1030) */ {ET_StudyDescription,EType_2},
   /* (0008,1050) */ {ET_PerformingPhysiciansName,EType_2}
  };
S_ListDesc1 ModAttrT_Desc_10B0513 = {E_ListIsTagTypeMember,3,ModAttrT_10B0513, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_10B0513SQProcedureCodeSequence[] =  /* Study Component Keys  */
  {
   /* (0008,0100) */ {ET_CodeValue,EType_1C},
   /* (0008,0102) */ {ET_CodingSchemeDesignator,EType_1C},
   /* (0008,0104) */ {ET_CodeMeaning,EType_1C}
  };
/* ModAttrTable references */
S_TagType ModAttrT_SQ10B0513SQProcedureCodeSequence[] =  /* Study Component Keys */
  {
   /* (0008,1032) */ {ET_ProcedureCodeSequence,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_10B0513SQProcedureCodeSequence = {E_ListIsTagTypeMemberWithCond,3,ModAttrT_10B0513SQProcedureCodeSequence, 0, 0,1,ModAttrT_SQ10B0513SQProcedureCodeSequence};

/* ModAttrTable references */
S_TagType ModAttrT_10B0514[] =   /* Print Queue Keys */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_1C},
   /* (2110,0030) */ {ET_PrinterName,EType_2},
   /* (2110,0099) */ {ET_PrintQueueID,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_10B0514 = {E_ListIsTagTypeMember,3,ModAttrT_10B0514, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_10B0515[] =   /* Film Session Keys */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_1C},
   /* (2000,0050) */ {ET_FilmSessionLabel,EType_2},
   /* (2100,0020) */ {ET_ExecutionStatus,EType_1C}
  };
S_ListDesc1 ModAttrT_Desc_10B0515 = {E_ListIsTagTypeMember,3,ModAttrT_10B0515, 0, 0,0,NULL};

/* ModAttrTable references */
char *ModAttrT_10B0515V1ExecutionStatus[] =   /* Film Session Keys */
  {
   "PENDING",
   "DONE"
  };
S_ListDesc1 ModAttrT_Desc_10B0515V1ExecutionStatus = {E_ListIsVal1Member,2,ModAttrT_10B0515V1ExecutionStatus,ET_ExecutionStatus,EType_1C,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_10B0516[] =   /* Basic Film Box Keys */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_1C},
   /* (2100,0020) */ {ET_ExecutionStatus,EType_1C}
  };
S_ListDesc1 ModAttrT_Desc_10B0516 = {E_ListIsTagTypeMember,2,ModAttrT_10B0516, 0, 0,0,NULL};

S_ListDesc1 ModAttrT_Desc_10B0516V1ExecutionStatus = {E_ListIsVal1Member,2,ModAttrT_10B0515V1ExecutionStatus,ET_ExecutionStatus,EType_1C,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_10B0517[] =   /* Basic Image Box Keys */
  {
   /* (0008,0005) */ {ET_SpecificCharacterSet,EType_1C},
   /* (2020,0010) */ {ET_ImagePosition2020,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_10B0517 = {E_ListIsTagTypeMember,2,ModAttrT_10B0517, 0, 0,0,NULL};

/* ModAttrTable references */
S_TagType ModAttrT_10B0519[] =   /* Multi-Referenced File Keys */
  {
   /* (0004,1600) */ {ET_NumberofReferences,EType_1}
  };
S_ListDesc1 ModAttrT_Desc_10B0519 = {E_ListIsTagTypeMember,1,ModAttrT_10B0519, 0, 0,0,NULL};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0201[] =     /* Patient Relationship Module Attributes */
  {
   &ModAttrT_Desc_03C0201,
   &ModAttrT_Desc_03C0201SQReferencedPatientAliasSequence,
   &ModAttrT_Desc_03C0201SQReferencedStudySequence,
   &ModAttrT_Desc_03C0201SQReferencedVisitSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0201 = {E_ListDesc2,4,ModAttrGroupT_03C0201};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0202[] =     /* Patient Identification Module Attributes */
  {
   &ModAttrT_Desc_03C0202
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0202 = {E_ListDesc2,1,ModAttrGroupT_03C0202};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0203[] =     /* Patient Demographic Module Attributes */
  {
   &ModAttrT_Desc_03C0203,
   &ModAttrT_Desc_03C0203SQPatientsInsurancePlanCodeSequence,
   &ModAttrT_Desc_03C0203V1PatientsSex
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0203 = {E_ListDesc2,3,ModAttrGroupT_03C0203};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0204[] =     /* Patient Medical Module Attributes */
  {
   &ModAttrT_Desc_03C0204,
   &ModAttrT_Desc_03C0204V1PregnancyStatus,
   &ModAttrT_Desc_03C0204V1SmokingStatus
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0204 = {E_ListDesc2,3,ModAttrGroupT_03C0204};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0301[] =     /* Visit Relationship Module Attributes */
  {
   &ModAttrT_Desc_03C0301,
   &ModAttrT_Desc_03C0301SQReferencedPatientSequence,
   &ModAttrT_Desc_03C0301SQReferencedStudySequence
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0301 = {E_ListDesc2,3,ModAttrGroupT_03C0301};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0302[] =     /* Visit Identification Module Attributes */
  {
   &ModAttrT_Desc_03C0302,
   &ModAttrT_Desc_03C0302SQInstitutionCodeSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0302 = {E_ListDesc2,2,ModAttrGroupT_03C0302};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0303[] =     /* Visit Status Module Attributes */
  {
   &ModAttrT_Desc_03C0303,
   &ModAttrT_Desc_03C0303V1VisitStatusID
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0303 = {E_ListDesc2,2,ModAttrGroupT_03C0303};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0304[] =     /* Visit Admission Module Attributes */
  {
   &ModAttrT_Desc_03C0304,
   &ModAttrT_Desc_03C0304SQAdmittingDiagnosisCodeSequence,
   &ModAttrT_Desc_03C0304V1RouteofAdmissions
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0304 = {E_ListDesc2,3,ModAttrGroupT_03C0304};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0305[] =     /* Visit Discharge Module Attributes */
  {
   &ModAttrT_Desc_03C0305,
   &ModAttrT_Desc_03C0305SQDischargeDiagnosisCodeSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0305 = {E_ListDesc2,2,ModAttrGroupT_03C0305};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0306[] =     /* Visit Scheduling Module Attributes */
  {
   &ModAttrT_Desc_03C0306
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0306 = {E_ListDesc2,1,ModAttrGroupT_03C0306};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0401[] =     /* Study Relationship Module Attributes */
  {
   &ModAttrT_Desc_03C0401,
   &ModAttrT_Desc_03C0401SQReferencedPatientSequence,
   &ModAttrT_Desc_03C0401SQReferencedResultsSequence,
   &ModAttrT_Desc_03C0401SQReferencedStudyComponentSequence,
   &ModAttrT_Desc_03C0401SQReferencedVisitSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0401 = {E_ListDesc2,5,ModAttrGroupT_03C0401};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0402[] =     /* Study Identification Module Attributes */
  {
   &ModAttrT_Desc_03C0402
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0402 = {E_ListDesc2,1,ModAttrGroupT_03C0402};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0403[] =     /* Study Classification Module Attributes */
  {
   &ModAttrT_Desc_03C0403,
   &ModAttrT_Desc_03C0403V1StudyPriorityID,
   &ModAttrT_Desc_03C0403V1StudyStatusID
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0403 = {E_ListDesc2,3,ModAttrGroupT_03C0403};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0404[] =     /* Study Scheduling Module Attributes */
  {
   &ModAttrT_Desc_03C0404,
   &ModAttrT_Desc_03C0404SQRequestedProcedureCodeSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0404 = {E_ListDesc2,2,ModAttrGroupT_03C0404};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0405[] =     /* Study Acquisition Module Attributes */
  {
   &ModAttrT_Desc_03C0405
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0405 = {E_ListDesc2,1,ModAttrGroupT_03C0405};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0406[] =     /* Study Read Module Attributes */
  {
   &ModAttrT_Desc_03C0406
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0406 = {E_ListDesc2,1,ModAttrGroupT_03C0406};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0407[] =     /* Study Component Module Attributes */
  {
   &ModAttrT_Desc_03C0407,
   &ModAttrT_Desc_03C0407SQReferencedSeriesSequence,
   &ModAttrT_Desc_03C0407SQSQReferencedImageSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0407 = {E_ListDesc2,3,ModAttrGroupT_03C0407};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0408[] =     /* Study Component Relationship Module Attributes */
  {
   &ModAttrT_Desc_03C0408,
   &ModAttrT_Desc_03C0408SQReferencedStudySequence
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0408 = {E_ListDesc2,2,ModAttrGroupT_03C0408};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0409[] =     /* Study Component Acquisition  Module Attributes */
  {
   &ModAttrT_Desc_03C0409,
   &ModAttrT_Desc_03C0409SQProcedureCodeSequence,
   &ModAttrT_Desc_03C0409V1StudyComponentStatusID
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0409 = {E_ListDesc2,3,ModAttrGroupT_03C0409};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0501[] =     /* Results Relationship Module Attributes */
  {
   &ModAttrT_Desc_03C0501,
   &ModAttrT_Desc_03C0501SQReferencedInterpretationSequence,
   &ModAttrT_Desc_03C0501SQReferencedStudySequence
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0501 = {E_ListDesc2,3,ModAttrGroupT_03C0501};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0502[] =     /* Results Identification Module Attributes */
  {
   &ModAttrT_Desc_03C0502
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0502 = {E_ListDesc2,1,ModAttrGroupT_03C0502};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0503[] =     /* Results Impression Module Attributes */
  {
   &ModAttrT_Desc_03C0503
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0503 = {E_ListDesc2,1,ModAttrGroupT_03C0503};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0601[] =     /* Interpretation Relationship Module Attributes */
  {
   &ModAttrT_Desc_03C0601,
   &ModAttrT_Desc_03C0601SQReferencedResultsSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0601 = {E_ListDesc2,2,ModAttrGroupT_03C0601};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0602[] =     /* Interpretation Identification Module Attributes */
  {
   &ModAttrT_Desc_03C0602
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0602 = {E_ListDesc2,1,ModAttrGroupT_03C0602};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0603[] =     /* Interpretation State Module Attributes */
  {
   &ModAttrT_Desc_03C0603,
   &ModAttrT_Desc_03C0603V1InterpretationStatusID,
   &ModAttrT_Desc_03C0603V1InterpretationTypeID
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0603 = {E_ListDesc2,3,ModAttrGroupT_03C0603};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0604[] =     /* Interpretation Recording Module Attributes */
  {
   &ModAttrT_Desc_03C0604
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0604 = {E_ListDesc2,1,ModAttrGroupT_03C0604};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0605[] =     /* Interpretation Transcription Module Attributes */
  {
   &ModAttrT_Desc_03C0605
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0605 = {E_ListDesc2,1,ModAttrGroupT_03C0605};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0606[] =     /* Interpretation Approval Module Attributes */
  {
   &ModAttrT_Desc_03C0606,
   &ModAttrT_Desc_03C0606SQDiagnosisCodeSequence,
   &ModAttrT_Desc_03C0606SQInterpretationApproverSequence,
   &ModAttrT_Desc_03C0606SQResultsDistributionListSequence,
   &ModAttrT_Desc_03C0606V1_4008_0117CodingSchemeDesignator
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0606 = {E_ListDesc2,5,ModAttrGroupT_03C0606};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C07011[] =     /* Patient Module Attributes */
  {
   &ModAttrT_Desc_03C07011,
   &ModAttrT_Desc_03C07011SQReferencedPatientSequence,
   &ModAttrT_Desc_03C07011V1PatientsSex
  };
S_ListDesc3 ModAttrGroupT_Desc_03C07011 = {E_ListDesc2,3,ModAttrGroupT_03C07011};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C07021[] =     /* General Study Module Attributes */
  {
   &ModAttrT_Desc_03C07021,
   &ModAttrT_Desc_03C07021SQReferencedStudySequence
  };
S_ListDesc3 ModAttrGroupT_Desc_03C07021 = {E_ListDesc2,2,ModAttrGroupT_03C07021};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C07022[] =     /* Patient Study Module Attributes */
  {
   &ModAttrT_Desc_03C07022
  };
S_ListDesc3 ModAttrGroupT_Desc_03C07022 = {E_ListDesc2,1,ModAttrGroupT_03C07022};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C07031[] =     /* General Series Module Attributes */
  {
   &ModAttrT_Desc_03C07031,
   &ModAttrT_Desc_03C07031SQReferencedStudyComponentSequence,
   &ModAttrT_Desc_03C07031V1BodyPartExamined,
   &ModAttrT_Desc_03C07031V1Laterality,
   &ModAttrT_Desc_03C07031V1PatientPosition
  };
S_ListDesc3 ModAttrGroupT_Desc_03C07031 = {E_ListDesc2,5,ModAttrGroupT_03C07031};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C07041[] =     /* Frame Of Reference Module Attributes */
  {
   &ModAttrT_Desc_03C07041
  };
S_ListDesc3 ModAttrGroupT_Desc_03C07041 = {E_ListDesc2,1,ModAttrGroupT_03C07041};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C07051[] =     /* General Equipment Module Attributes */
  {
   &ModAttrT_Desc_03C07051
  };
S_ListDesc3 ModAttrGroupT_Desc_03C07051 = {E_ListDesc2,1,ModAttrGroupT_03C07051};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C07061[] =     /* General Image Module Attributes */
  {
   &ModAttrT_Desc_03C07061,
   &ModAttrT_Desc_03C07061SQReferencedImageSequence,
   &ModAttrT_Desc_03C07061SQSourceImageSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_03C07061 = {E_ListDesc2,3,ModAttrGroupT_03C07061};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C07062[] =     /* Image Plane Module Attributes */
  {
   &ModAttrT_Desc_03C07062
  };
S_ListDesc3 ModAttrGroupT_Desc_03C07062 = {E_ListDesc2,1,ModAttrGroupT_03C07062};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C07063[] =     /* Image Pixel Module Attributes */
  {
   &ModAttrT_Desc_03C07063,
   &ModAttrT_Desc_03C07063V1PhotometricInterpretation,
   &ModAttrT_Desc_03C07063V1PixelRepresentation,
   &ModAttrT_Desc_03C07063V1PlanarConfiguration
  };
S_ListDesc3 ModAttrGroupT_Desc_03C07063 = {E_ListDesc2,4,ModAttrGroupT_03C07063};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C07064[] =     /* Contrast/Bolus Module Attributes */
  {
   &ModAttrT_Desc_03C07064
  };
S_ListDesc3 ModAttrGroupT_Desc_03C07064 = {E_ListDesc2,1,ModAttrGroupT_03C07064};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C07065[] =     /* Cine Module Attributes */
  {
   &ModAttrT_Desc_03C07065
  };
S_ListDesc3 ModAttrGroupT_Desc_03C07065 = {E_ListDesc2,1,ModAttrGroupT_03C07065};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C07066[] =     /* Multi-frame Module Attributes */
  {
   &ModAttrT_Desc_03C07066
  };
S_ListDesc3 ModAttrGroupT_Desc_03C07066 = {E_ListDesc2,1,ModAttrGroupT_03C07066};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0707[] =     /* Patient Summary Module Attributes */
  {
   &ModAttrT_Desc_03C0707
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0707 = {E_ListDesc2,1,ModAttrGroupT_03C0707};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0708[] =     /* Study Content Module Attributes */
  {
   &ModAttrT_Desc_03C0708,
   &ModAttrT_Desc_03C0708SQReferencedSeriesSequence,
   &ModAttrT_Desc_03C0708SQSQReferencedImageSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0708 = {E_ListDesc2,3,ModAttrGroupT_03C0708};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C08011[] =     /* CR Series Module Attributes */
  {
   &ModAttrT_Desc_03C08011,
   &ModAttrT_Desc_03C08011V1BodyPartExamined,
   &ModAttrT_Desc_03C08011V1ViewPosition
  };
S_ListDesc3 ModAttrGroupT_Desc_03C08011 = {E_ListDesc2,3,ModAttrGroupT_03C08011};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C08012[] =     /* CR Image Module Attributes */
  {
   &ModAttrT_Desc_03C08012,
   &ModAttrT_Desc_03C08012V1CassetteOrientation,
   &ModAttrT_Desc_03C08012V1CassetteSize
  };
S_ListDesc3 ModAttrGroupT_Desc_03C08012 = {E_ListDesc2,3,ModAttrGroupT_03C08012};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C08021[] =     /* CT Image Module Attributes */
  {
   &ModAttrT_Desc_03C08021,
   &ModAttrT_Desc_03C08021V1BitsAllocated0028,
   &ModAttrT_Desc_03C08021V1BitsStored,
   &ModAttrT_Desc_03C08021V1HighBit,
   &ModAttrT_Desc_03C08021V1PhotometricInterpretation,
   &ModAttrT_Desc_03C08021V1RotationDirection,
   &ModAttrT_Desc_03C08021V1SamplesperPixel,
   &ModAttrT_Desc_03C08021V3ImageType
  };
S_ListDesc3 ModAttrGroupT_Desc_03C08021 = {E_ListDesc2,8,ModAttrGroupT_03C08021};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C08031[] =     /* MR Image Module Attributes */
  {
   &ModAttrT_Desc_03C08031,
   &ModAttrT_Desc_03C08031V1AngioFlag,
   &ModAttrT_Desc_03C08031V1BeatRejectionFlag,
   &ModAttrT_Desc_03C08031V1BitsAllocated0028,
   &ModAttrT_Desc_03C08031V1MRAcquisitionType,
   &ModAttrT_Desc_03C08031V1PhaseEncodingDirection,
   &ModAttrT_Desc_03C08031V1PhotometricInterpretation,
   &ModAttrT_Desc_03C08031V1SamplesperPixel,
   &ModAttrT_Desc_03C08031V1ScanOptions,
   &ModAttrT_Desc_03C08031V1ScanningSequence,
   &ModAttrT_Desc_03C08031V1SequenceVariant,
   &ModAttrT_Desc_03C08031V1VariableFlipAngleFlag,
   &ModAttrT_Desc_03C08031V3ImageType
  };
S_ListDesc3 ModAttrGroupT_Desc_03C08031 = {E_ListDesc2,13,ModAttrGroupT_03C08031};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C08041[] =     /* NM Series Module Attributes */
  {
   &ModAttrT_Desc_03C08041,
   &ModAttrT_Desc_03C08041V1NuclearMedicineSeriesType,
   &ModAttrT_Desc_03C08041V1WholeBodyTechnique
  };
S_ListDesc3 ModAttrGroupT_Desc_03C08041 = {E_ListDesc2,3,ModAttrGroupT_03C08041};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C08042[] =     /* NM Equipment Module Attributes */
  {
   &ModAttrT_Desc_03C08042,
   &ModAttrT_Desc_03C08042V1CollimatorType,
   &ModAttrT_Desc_03C08042V1FieldofViewShape
  };
S_ListDesc3 ModAttrGroupT_Desc_03C08042 = {E_ListDesc2,3,ModAttrGroupT_03C08042};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C08043[] =     /* NM Image Module Attributes */
  {
   &ModAttrT_Desc_03C08043,
   &ModAttrT_Desc_03C08043SQReferencedCurveSequence,
   &ModAttrT_Desc_03C08043SQReferencedOverlaySequence,
   &ModAttrT_Desc_03C08043V1AcquisitionTerminationCondition,
   &ModAttrT_Desc_03C08043V1CorrectedImage
  };
S_ListDesc3 ModAttrGroupT_Desc_03C08043 = {E_ListDesc2,5,ModAttrGroupT_03C08043};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C08044[] =     /* NM SPECT Acquisition Image Module Attributes */
  {
   &ModAttrT_Desc_03C08044,
   &ModAttrT_Desc_03C08044V1FrameIncrementPointer,
   &ModAttrT_Desc_03C08044V1RotationDirection
  };
S_ListDesc3 ModAttrGroupT_Desc_03C08044 = {E_ListDesc2,3,ModAttrGroupT_03C08044};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C08045[] =     /* NM Multi-gated Acquisition Image Module Attributes */
  {
   &ModAttrT_Desc_03C08045,
   &ModAttrT_Desc_03C08045V1BeatRejectionFlag,
   &ModAttrT_Desc_03C08045V1FrameIncrementPointer
  };
S_ListDesc3 ModAttrGroupT_Desc_03C08045 = {E_ListDesc2,3,ModAttrGroupT_03C08045};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C08051[] =     /* US Frame of Reference Module */
  {
   &ModAttrT_Desc_03C08051,
   &ModAttrT_Desc_03C08051V1PhysicalUnitsXDirection,
   &ModAttrT_Desc_03C08051V1PhysicalUnitsYDirection
  };
S_ListDesc3 ModAttrGroupT_Desc_03C08051 = {E_ListDesc2,3,ModAttrGroupT_03C08051};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C08052[] =     /* US Region Calibration Module Attributes */
  {
   &ModAttrT_Desc_03C08052,
   &ModAttrT_Desc_03C08052SQSequenceofUltrasoundRegions,
   &ModAttrT_Desc_03C08052V1PhysicalUnitsXDirection,
   &ModAttrT_Desc_03C08052V1PhysicalUnitsYDirection,
   &ModAttrT_Desc_03C08052V1PixelComponentDataType,
   &ModAttrT_Desc_03C08052V1PixelComponentPhysicalUnits,
   &ModAttrT_Desc_03C08052V1RegionDataType,
   &ModAttrT_Desc_03C08052V1RegionSpatialFormat
  };
S_ListDesc3 ModAttrGroupT_Desc_03C08052 = {E_ListDesc2,8,ModAttrGroupT_03C08052};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C08053[] =     /* US Image Module Attributes */
  {
   &ModAttrT_Desc_03C08053,
   &ModAttrT_Desc_03C08053SQReferencedCurveSequence,
   &ModAttrT_Desc_03C08053SQReferencedOverlaySequence,
   &ModAttrT_Desc_03C08053V1AnatomicStructure,
   &ModAttrT_Desc_03C08053V1BeatRejectionFlag,
   &ModAttrT_Desc_03C08053V1FrameIncrementPointer,
   &ModAttrT_Desc_03C08053V1PhotometricInterpretation,
   &ModAttrT_Desc_03C08053V1PixelRepresentation,
   &ModAttrT_Desc_03C08053V1TransducerOrientation,
   &ModAttrT_Desc_03C08053V1TransducerPosition,
   &ModAttrT_Desc_03C08053V1TransducerType,
   &ModAttrT_Desc_03C08053V3ImageType,
   &ModAttrT_Desc_03C08053V4ImageType
  };
S_ListDesc3 ModAttrGroupT_Desc_03C08053 = {E_ListDesc2,13,ModAttrGroupT_03C08053};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C08061[] =     /* SC Image Equipment Module Attributes */
  {
   &ModAttrT_Desc_03C08061,
   &ModAttrT_Desc_03C08061V1ConversionType
  };
S_ListDesc3 ModAttrGroupT_Desc_03C08061 = {E_ListDesc2,2,ModAttrGroupT_03C08061};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C08062[] =     /* SC Image Module Attributes */
  {
   &ModAttrT_Desc_03C08062
  };
S_ListDesc3 ModAttrGroupT_Desc_03C08062 = {E_ListDesc2,1,ModAttrGroupT_03C08062};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0901[] =     /* Overlay Identification Module Attributes */
  {
   &ModAttrT_Desc_03C0901,
   &ModAttrT_Desc_03C0901SQReferencedImageSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0901 = {E_ListDesc2,2,ModAttrGroupT_03C0901};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0902[] =     /* Overlay Plane Module Attributes */
  {
   &ModAttrT_Desc_03C0902,
   &ModAttrT_Desc_03C0902V1OverlayType
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0902 = {E_ListDesc2,2,ModAttrGroupT_03C0902};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C0903[] =     /* Multi-frame Overlay Module Attributes */
  {
   &ModAttrT_Desc_03C0903
  };
S_ListDesc3 ModAttrGroupT_Desc_03C0903 = {E_ListDesc2,1,ModAttrGroupT_03C0903};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C1001[] =     /* Curve Identification Module Attributes */
  {
   &ModAttrT_Desc_03C1001,
   &ModAttrT_Desc_03C1001SQReferencedCurveSequence,
   &ModAttrT_Desc_03C1001SQReferencedImageSequence,
   &ModAttrT_Desc_03C1001SQReferencedOverlaySequence
  };
S_ListDesc3 ModAttrGroupT_Desc_03C1001 = {E_ListDesc2,4,ModAttrGroupT_03C1001};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C1002[] =     /* Curve Module Attributes */
  {
   &ModAttrT_Desc_03C1002,
   &ModAttrT_Desc_03C1002V1AxisUnits,
   &ModAttrT_Desc_03C1002V1CurveDataDescriptor,
   &ModAttrT_Desc_03C1002V1DataValueRepresentation,
   &ModAttrT_Desc_03C1002V1TypeofData
  };
S_ListDesc3 ModAttrGroupT_Desc_03C1002 = {E_ListDesc2,5,ModAttrGroupT_03C1002};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C1003[] =     /* Audio Module Attributes */
  {
   &ModAttrT_Desc_03C1003,
   &ModAttrT_Desc_03C1003SQReferencedImageSequence,
   &ModAttrT_Desc_03C1003V1AudioSampleFormat,
   &ModAttrT_Desc_03C1003V1AudioType,
   &ModAttrT_Desc_03C1003V1NumberofChannels
  };
S_ListDesc3 ModAttrGroupT_Desc_03C1003 = {E_ListDesc2,5,ModAttrGroupT_03C1003};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C1101[] =     /* Modality LUT Module Attributes */
  {
   &ModAttrT_Desc_03C1101,
   &ModAttrT_Desc_03C1101SQModalityLUTSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_03C1101 = {E_ListDesc2,2,ModAttrGroupT_03C1101};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C1102[] =     /* VOI LUT Module Attributes */
  {
   &ModAttrT_Desc_03C1102,
   &ModAttrT_Desc_03C1102SQVOILUTSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_03C1102 = {E_ListDesc2,2,ModAttrGroupT_03C1102};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C1103[] =     /* LUT Identification Module Attributes */
  {
   &ModAttrT_Desc_03C1103,
   &ModAttrT_Desc_03C1103SQReferencedImageSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_03C1103 = {E_ListDesc2,2,ModAttrGroupT_03C1103};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C1201[] =     /* SOP Common Module Attributes */
  {
   &ModAttrT_Desc_03C1201,
   &ModAttrT_Desc_03C1201V1SpecificCharacterSet
  };
S_ListDesc3 ModAttrGroupT_Desc_03C1201 = {E_ListDesc2,2,ModAttrGroupT_03C1201};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C1301[] =     /* Basic Film Session Presentation Module */
  {
   &ModAttrT_Desc_03C1301,
   &ModAttrT_Desc_03C1301V1FilmDestination,
   &ModAttrT_Desc_03C1301V1MediumType,
   &ModAttrT_Desc_03C1301V1PrintPriority
  };
S_ListDesc3 ModAttrGroupT_Desc_03C1301 = {E_ListDesc2,4,ModAttrGroupT_03C1301};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C1302[] =     /* Basic Film Session Relationship Module */
  {
   &ModAttrT_Desc_03C1302,
   &ModAttrT_Desc_03C1302SQReferencedFilmBoxSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_03C1302 = {E_ListDesc2,2,ModAttrGroupT_03C1302};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C1303[] =     /* Basic Film Box Presentation Module */
  {
   &ModAttrT_Desc_03C1303,
   &ModAttrT_Desc_03C1303V1BorderDensity,
   &ModAttrT_Desc_03C1303V1EmptyImageDensity,
   &ModAttrT_Desc_03C1303V1FilmOrientation,
   &ModAttrT_Desc_03C1303V1FilmSizeID,
   &ModAttrT_Desc_03C1303V1MagnificationType,
   &ModAttrT_Desc_03C1303V1Trim
  };
S_ListDesc3 ModAttrGroupT_Desc_03C1303 = {E_ListDesc2,7,ModAttrGroupT_03C1303};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C1304[] =     /* Basic Film Box Relationship Module */
  {
   &ModAttrT_Desc_03C1304,
   &ModAttrT_Desc_03C1304SQReferencedBasicAnnotationBoxSequence,
   &ModAttrT_Desc_03C1304SQReferencedFilmSessionSequence,
   &ModAttrT_Desc_03C1304SQReferencedImageBoxSequence2010
  };
S_ListDesc3 ModAttrGroupT_Desc_03C1304 = {E_ListDesc2,4,ModAttrGroupT_03C1304};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C1305[] =     /* Image Box Pixel Presentation Module */
  {
   &ModAttrT_Desc_03C1305,
   &ModAttrT_Desc_03C1305SQPreformattedColorImageSequence,
   &ModAttrT_Desc_03C1305SQPreformattedGrayscaleImageSequence,
   &ModAttrT_Desc_03C1305V1MagnificationType,
   &ModAttrT_Desc_03C1305V1Polarity,
   &ModAttrT_Desc_03C1305V1_2020_0110BitsAllocated0028,
   &ModAttrT_Desc_03C1305V1_2020_0110BitsStored,
   &ModAttrT_Desc_03C1305V1_2020_0110HighBit,
   &ModAttrT_Desc_03C1305V1_2020_0110PhotometricInterpretation,
   &ModAttrT_Desc_03C1305V1_2020_0110PixelRepresentation,
   &ModAttrT_Desc_03C1305V1_2020_0110SamplesperPixel,
   &ModAttrT_Desc_03C1305V1_2020_0111BitsAllocated0028,
   &ModAttrT_Desc_03C1305V1_2020_0111BitsStored,
   &ModAttrT_Desc_03C1305V1_2020_0111HighBit,
   &ModAttrT_Desc_03C1305V1_2020_0111PhotometricInterpretation,
   &ModAttrT_Desc_03C1305V1_2020_0111PixelRepresentation,
   &ModAttrT_Desc_03C1305V1_2020_0111SamplesperPixel
  };
S_ListDesc3 ModAttrGroupT_Desc_03C1305 = {E_ListDesc2,17,ModAttrGroupT_03C1305};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C1306[] =     /* Image Box Relationship Module */
  {
   &ModAttrT_Desc_03C1306,
   &ModAttrT_Desc_03C1306SQReferencedImageOverlayBoxSequence,
   &ModAttrT_Desc_03C1306SQReferencedImageSequence,
   &ModAttrT_Desc_03C1306SQReferencedVOILUTBoxSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_03C1306 = {E_ListDesc2,4,ModAttrGroupT_03C1306};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C1307[] =     /* Basic Annotation Presentation Module */
  {
   &ModAttrT_Desc_03C1307
  };
S_ListDesc3 ModAttrGroupT_Desc_03C1307 = {E_ListDesc2,1,ModAttrGroupT_03C1307};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C1308[] =     /* Print Job Module */
  {
   &ModAttrT_Desc_03C1308,
   &ModAttrT_Desc_03C1308V1ExecutionStatus,
   &ModAttrT_Desc_03C1308V1ExecutionStatusInfo,
   &ModAttrT_Desc_03C1308V1PrintPriority
  };
S_ListDesc3 ModAttrGroupT_Desc_03C1308 = {E_ListDesc2,4,ModAttrGroupT_03C1308};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C1309[] =     /* Printer Module */
  {
   &ModAttrT_Desc_03C1309,
   &ModAttrT_Desc_03C1309V1PrinterStatus,
   &ModAttrT_Desc_03C1309V1PrinterStatusInfo
  };
S_ListDesc3 ModAttrGroupT_Desc_03C1309 = {E_ListDesc2,3,ModAttrGroupT_03C1309};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C1310[] =     /* Image Overlay Box Presentation Module */
  {
   &ModAttrT_Desc_03C1310,
   &ModAttrT_Desc_03C1310SQReferencedOverlayPlaneSequence,
   &ModAttrT_Desc_03C1310V1OverlayForegroundDensity,
   &ModAttrT_Desc_03C1310V1OverlayMagnificationType,
   &ModAttrT_Desc_03C1310V1OverlayMode,
   &ModAttrT_Desc_03C1310V1ThresholdDensity
  };
S_ListDesc3 ModAttrGroupT_Desc_03C1310 = {E_ListDesc2,6,ModAttrGroupT_03C1310};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_03C1311[] =     /* Image Overlay Box Relationship Module */
  {
   &ModAttrT_Desc_03C1311,
   &ModAttrT_Desc_03C1311SQReferencedImageBoxSequence2040
  };
S_ListDesc3 ModAttrGroupT_Desc_03C1311 = {E_ListDesc2,2,ModAttrGroupT_03C1311};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04C060112[] =     /* Patient Level Attributes for the Patient Root Query/Retrieve Information Model */
  {
   &ModAttrT_Desc_04C060112,
   &ModAttrT_Desc_04C060112SQReferencedPatientSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04C060112 = {E_ListDesc2,2,ModAttrGroupT_04C060112};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04C060113[] =     /* Study Level Keys for the Patient Root Query/Retrieve Information Model */
  {
   &ModAttrT_Desc_04C060113,
   &ModAttrT_Desc_04C060113SQProcedureCodeSequence,
   &ModAttrT_Desc_04C060113SQReferencedStudySequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04C060113 = {E_ListDesc2,3,ModAttrGroupT_04C060113};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04C060114[] =     /* Series Level Attributes for the Patient Root Query/Retrieve Information Model */
  {
   &ModAttrT_Desc_04C060114
  };
S_ListDesc3 ModAttrGroupT_Desc_04C060114 = {E_ListDesc2,1,ModAttrGroupT_04C060114};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04C060115[] =     /* Image Level Keys for the Patient Root Query/Retrieve Information Model */
  {
   &ModAttrT_Desc_04C060115
  };
S_ListDesc3 ModAttrGroupT_Desc_04C060115 = {E_ListDesc2,1,ModAttrGroupT_04C060115};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04C060212[] =     /* Study Level Keys for the Study Root Query/Retrieve Information Model */
  {
   &ModAttrT_Desc_04C060212,
   &ModAttrT_Desc_04C060212SQProcedureCodeSequence,
   &ModAttrT_Desc_04C060212SQReferencedPatientSequence,
   &ModAttrT_Desc_04C060212SQReferencedStudySequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04C060212 = {E_ListDesc2,4,ModAttrGroupT_04C060212};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04E032[] =     /* Detached Patient Management SOP Class N-Get Attributes */
  {
   &ModAttrT_Desc_04E032,
   &ModAttrT_Desc_04E032SQReferencedStudySequence,
   &ModAttrT_Desc_04E032SQReferencedVisitSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04E032 = {E_ListDesc2,3,ModAttrGroupT_04E032};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04E0331[] =     /* Patient Notification Event Information (Patient Created 1) */
  {
   &ModAttrT_Desc_04E0331
  };
S_ListDesc3 ModAttrGroupT_Desc_04E0331 = {E_ListDesc2,1,ModAttrGroupT_04E0331};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04E0332[] =     /* Patient Notification Event Information (Patient Deleted 2) */
  {
   &ModAttrT_Desc_04E0332
  };
S_ListDesc3 ModAttrGroupT_Desc_04E0332 = {E_ListDesc2,1,ModAttrGroupT_04E0332};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04E0333[] =     /* Patient Notification Event Information (Patient Updated 3) */
  {
   &ModAttrT_Desc_04E0333
  };
S_ListDesc3 ModAttrGroupT_Desc_04E0333 = {E_ListDesc2,1,ModAttrGroupT_04E0333};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04E0421[] =     /* Detached Visit Management SOP Class N-Get Attributes */
  {
   &ModAttrT_Desc_04E0421,
   &ModAttrT_Desc_04E0421SQReferencedPatientSequence,
   &ModAttrT_Desc_04E0421SQReferencedStudySequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04E0421 = {E_ListDesc2,3,ModAttrGroupT_04E0421};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04E0422[] =     /* Detached Visit Management SOP Class N-Set Attributes */
  {
   &ModAttrT_Desc_04E0422
  };
S_ListDesc3 ModAttrGroupT_Desc_04E0422 = {E_ListDesc2,1,ModAttrGroupT_04E0422};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04E0431[] =     /* Visit Notification Event Types and Attributes.(Visit Created 1) */
  {
   &ModAttrT_Desc_04E0431,
   &ModAttrT_Desc_04E0431SQReferencedPatientSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04E0431 = {E_ListDesc2,2,ModAttrGroupT_04E0431};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04E0432[] =     /* Visit Notification Event Types and Attributes.(Visit Scheduled 2) */
  {
   &ModAttrT_Desc_04E0432,
   &ModAttrT_Desc_04E0432SQReferencedPatientSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04E0432 = {E_ListDesc2,2,ModAttrGroupT_04E0432};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04E0433[] =     /* Visit Notification Event Types and Attributes.(Patient Admitted 3) */
  {
   &ModAttrT_Desc_04E0433,
   &ModAttrT_Desc_04E0433SQReferencedPatientSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04E0433 = {E_ListDesc2,2,ModAttrGroupT_04E0433};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04E0434[] =     /* Visit Notification Event Types and Attributes.(Patient Transferred 4) */
  {
   &ModAttrT_Desc_04E0434,
   &ModAttrT_Desc_04E0434SQReferencedPatientSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04E0434 = {E_ListDesc2,2,ModAttrGroupT_04E0434};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04E0435[] =     /* Visit Notification Event Types and Attributes.(Patient Discharged 5) */
  {
   &ModAttrT_Desc_04E0435,
   &ModAttrT_Desc_04E0435SQReferencedPatientSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04E0435 = {E_ListDesc2,2,ModAttrGroupT_04E0435};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04E0436[] =     /* Visit Notification Event Types and Attributes.(Visit Deleted 6) */
  {
   &ModAttrT_Desc_04E0436
  };
S_ListDesc3 ModAttrGroupT_Desc_04E0436 = {E_ListDesc2,1,ModAttrGroupT_04E0436};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04E0437[] =     /* Visit Notification Event Types and Attributes.(Visit Updated 7) */
  {
   &ModAttrT_Desc_04E0437
  };
S_ListDesc3 ModAttrGroupT_Desc_04E0437 = {E_ListDesc2,1,ModAttrGroupT_04E0437};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04F0321[] =     /* Detached Study Management SOP Class N-GET Attributes */
  {
   &ModAttrT_Desc_04F0321,
   &ModAttrT_Desc_04F0321SQReferencedPatientSequence,
   &ModAttrT_Desc_04F0321SQReferencedResultsSequence,
   &ModAttrT_Desc_04F0321SQReferencedStudyComponentSequence,
   &ModAttrT_Desc_04F0321SQReferencedVisitSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04F0321 = {E_ListDesc2,5,ModAttrGroupT_04F0321};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04F0322[] =     /* Detached Study Management SOP Class N-Set Attributes */
  {
   &ModAttrT_Desc_04F0322
  };
S_ListDesc3 ModAttrGroupT_Desc_04F0322 = {E_ListDesc2,1,ModAttrGroupT_04F0322};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04F0331[] =     /* Study Notification Event Information (Study Created 1) */
  {
   &ModAttrT_Desc_04F0331,
   &ModAttrT_Desc_04F0331SQReferencedPatientSequence,
   &ModAttrT_Desc_04F0331SQReferencedVisitSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04F0331 = {E_ListDesc2,3,ModAttrGroupT_04F0331};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04F0332[] =     /* Study Notification Event Information (Study Scheduled 2) */
  {
   &ModAttrT_Desc_04F0332,
   &ModAttrT_Desc_04F0332SQReferencedPatientSequence,
   &ModAttrT_Desc_04F0332SQReferencedVisitSequence,
   &ModAttrT_Desc_04F0332SQRequestedProcedureCodeSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04F0332 = {E_ListDesc2,4,ModAttrGroupT_04F0332};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04F0333[] =     /* Study Notification Event Information (Patient Arrived 3) */
  {
   &ModAttrT_Desc_04F0333
  };
S_ListDesc3 ModAttrGroupT_Desc_04F0333 = {E_ListDesc2,1,ModAttrGroupT_04F0333};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04F0334[] =     /* Study Notification Event Information (Study Started 4) */
  {
   &ModAttrT_Desc_04F0334
  };
S_ListDesc3 ModAttrGroupT_Desc_04F0334 = {E_ListDesc2,1,ModAttrGroupT_04F0334};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04F0335[] =     /* Study Notification Event Information (Study Completed 5) */
  {
   &ModAttrT_Desc_04F0335,
   &ModAttrT_Desc_04F0335SQReferencedStudyComponentSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04F0335 = {E_ListDesc2,2,ModAttrGroupT_04F0335};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04F0336[] =     /* Study Notification Event Information (Study Verified 6) */
  {
   &ModAttrT_Desc_04F0336,
   &ModAttrT_Desc_04F0336SQReferencedStudyComponentSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04F0336 = {E_ListDesc2,2,ModAttrGroupT_04F0336};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04F0337[] =     /* Study Notification Event Information (Study Read 7) */
  {
   &ModAttrT_Desc_04F0337
  };
S_ListDesc3 ModAttrGroupT_Desc_04F0337 = {E_ListDesc2,1,ModAttrGroupT_04F0337};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04F0339[] =     /* Study Notification Event Information (Study Updated 9) */
  {
   &ModAttrT_Desc_04F0339
  };
S_ListDesc3 ModAttrGroupT_Desc_04F0339 = {E_ListDesc2,1,ModAttrGroupT_04F0339};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04F0421[] =     /* Study Component Management SOP Class N-CREATE Attributes */
  {
   &ModAttrT_Desc_04F0421,
   &ModAttrT_Desc_04F0421SQProcedureCodeSequence,
   &ModAttrT_Desc_04F0421SQReferencedSeriesSequence,
   &ModAttrT_Desc_04F0421SQReferencedStudySequence,
   &ModAttrT_Desc_04F0421SQSQReferencedImageSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04F0421 = {E_ListDesc2,5,ModAttrGroupT_04F0421};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04F0422[] =     /* Study Component Management SOP Class N-SET Attributes */
  {
   &ModAttrT_Desc_04F0422,
   &ModAttrT_Desc_04F0422SQProcedureCodeSequence,
   &ModAttrT_Desc_04F0422SQReferencedSeriesSequence,
   &ModAttrT_Desc_04F0422SQSQReferencedImageSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04F0422 = {E_ListDesc2,4,ModAttrGroupT_04F0422};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04F0423[] =     /* Study Component Management SOP Class N-Get Attributes */
  {
   &ModAttrT_Desc_04F0423,
   &ModAttrT_Desc_04F0423SQProcedureCodeSequence,
   &ModAttrT_Desc_04F0423SQReferencedSeriesSequence,
   &ModAttrT_Desc_04F0423SQReferencedStudySequence,
   &ModAttrT_Desc_04F0423SQSQReferencedImageSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04F0423 = {E_ListDesc2,5,ModAttrGroupT_04F0423};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04G032[] =     /* Detached Results Management SOP Class N-Get Attributes */
  {
   &ModAttrT_Desc_04G032,
   &ModAttrT_Desc_04G032SQReferencedInterpretationSequence,
   &ModAttrT_Desc_04G032SQReferencedStudySequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04G032 = {E_ListDesc2,3,ModAttrGroupT_04G032};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04G0331[] =     /* Results Notification Event Types and Attributes (Results Created 1) */
  {
   &ModAttrT_Desc_04G0331,
   &ModAttrT_Desc_04G0331SQReferencedInterpretationSequence,
   &ModAttrT_Desc_04G0331SQReferencedStudySequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04G0331 = {E_ListDesc2,3,ModAttrGroupT_04G0331};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04G0332[] =     /* Results Notification Event Types and Attributes (Results Deleted 2) */
  {
   &ModAttrT_Desc_04G0332
  };
S_ListDesc3 ModAttrGroupT_Desc_04G0332 = {E_ListDesc2,1,ModAttrGroupT_04G0332};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04G0333[] =     /* Results Notification Event Types and Attributes (Results Updated 3) */
  {
   &ModAttrT_Desc_04G0333
  };
S_ListDesc3 ModAttrGroupT_Desc_04G0333 = {E_ListDesc2,1,ModAttrGroupT_04G0333};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04G0421[] =     /* Detached Interpretation Management SOP Class N-Get Attributes */
  {
   &ModAttrT_Desc_04G0421,
   &ModAttrT_Desc_04G0421SQReferencedResultsSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04G0421 = {E_ListDesc2,2,ModAttrGroupT_04G0421};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04G0422[] =     /* Detached Interpretation  Management SOP Class N-Set Attributes */
  {
   &ModAttrT_Desc_04G0422
  };
S_ListDesc3 ModAttrGroupT_Desc_04G0422 = {E_ListDesc2,1,ModAttrGroupT_04G0422};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04G0431[] =     /* Interpretation Notification Event Information (Interpretation Created 1) */
  {
   &ModAttrT_Desc_04G0431,
   &ModAttrT_Desc_04G0431SQReferencedResultsSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04G0431 = {E_ListDesc2,2,ModAttrGroupT_04G0431};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04G0432[] =     /* Interpretation Notification Event Information (Interpretation Recorded 2) */
  {
   &ModAttrT_Desc_04G0432
  };
S_ListDesc3 ModAttrGroupT_Desc_04G0432 = {E_ListDesc2,1,ModAttrGroupT_04G0432};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04G0433[] =     /* Interpretation Notification Event Information (Interpretation Transcribed 3) */
  {
   &ModAttrT_Desc_04G0433
  };
S_ListDesc3 ModAttrGroupT_Desc_04G0433 = {E_ListDesc2,1,ModAttrGroupT_04G0433};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04G0434[] =     /* Interpretation Notification Event Information (Interpretation Approved 4) */
  {
   &ModAttrT_Desc_04G0434,
   &ModAttrT_Desc_04G0434SQDiagnosisCodeSequence,
   &ModAttrT_Desc_04G0434SQInterpretationApproverSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04G0434 = {E_ListDesc2,3,ModAttrGroupT_04G0434};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04G0436[] =     /* Interpretation Notification Event Information (Interpretation Updated 6) */
  {
   &ModAttrT_Desc_04G0436
  };
S_ListDesc3 ModAttrGroupT_Desc_04G0436 = {E_ListDesc2,1,ModAttrGroupT_04G0436};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04H04121[] =     /* Basic Film Session SOP Class N-CREATE Attributes */
  {
   &ModAttrT_Desc_04H04121
  };
S_ListDesc3 ModAttrGroupT_Desc_04H04121 = {E_ListDesc2,1,ModAttrGroupT_04H04121};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04H04124[] =     /* Basic Film Session SOP Class N-ACTION Attributes */
  {
   &ModAttrT_Desc_04H04124,
   &ModAttrT_Desc_04H04124SQReferencedPrintJobSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04H04124 = {E_ListDesc2,2,ModAttrGroupT_04H04124};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04H04221[] =     /* Basic Film Box SOP Class N-CREATE Attributes */
  {
   &ModAttrT_Desc_04H04221,
   &ModAttrT_Desc_04H04221SQReferencedBasicAnnotationBoxSequence,
   &ModAttrT_Desc_04H04221SQReferencedFilmSessionSequence,
   &ModAttrT_Desc_04H04221SQReferencedImageBoxSequence2010
  };
S_ListDesc3 ModAttrGroupT_Desc_04H04221 = {E_ListDesc2,4,ModAttrGroupT_04H04221};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04H04222[] =     /* Basic Film Box SOP Class N-SET Attributes */
  {
   &ModAttrT_Desc_04H04222
  };
S_ListDesc3 ModAttrGroupT_Desc_04H04222 = {E_ListDesc2,1,ModAttrGroupT_04H04222};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04H04224[] =     /* Basic Film Box SOP Class N-ACTION Attributes */
  {
   &ModAttrT_Desc_04H04224,
   &ModAttrT_Desc_04H04224SQReferencedPrintJobSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04H04224 = {E_ListDesc2,2,ModAttrGroupT_04H04224};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04H043121[] =     /* Basic Grayscale Image Box SOP Class N-SET Attributes */
  {
   &ModAttrT_Desc_04H043121,
   &ModAttrT_Desc_04H043121SQPreformattedGrayscaleImageSequence,
   &ModAttrT_Desc_04H043121SQReferencedOverlaySequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04H043121 = {E_ListDesc2,3,ModAttrGroupT_04H043121};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04H043221[] =     /* Basic Color Image Box SOP Class N-SET Attributes */
  {
   &ModAttrT_Desc_04H043221,
   &ModAttrT_Desc_04H043221SQReferencedOverlaySequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04H043221 = {E_ListDesc2,2,ModAttrGroupT_04H043221};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04H043321[] =     /* Referenced Image Box SOP Class N-SET Attributes */
  {
   &ModAttrT_Desc_04H043321,
   &ModAttrT_Desc_04H043321SQReferencedImageSequence,
   &ModAttrT_Desc_04H043321SQReferencedOverlaySequence,
   &ModAttrT_Desc_04H043321SQReferencedVOILUTBoxSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04H043321 = {E_ListDesc2,4,ModAttrGroupT_04H043321};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04H04421[] =     /* Basic Annotation Box SOP Class N-SET Attributes */
  {
   &ModAttrT_Desc_04H04421
  };
S_ListDesc3 ModAttrGroupT_Desc_04H04421 = {E_ListDesc2,1,ModAttrGroupT_04H04421};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04H045211[] =     /* Print Job SOP Class N-EVENT-REPORT Attributes (PENDING 1) */
  {
   &ModAttrT_Desc_04H045211
  };
S_ListDesc3 ModAttrGroupT_Desc_04H045211 = {E_ListDesc2,1,ModAttrGroupT_04H045211};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04H045212[] =     /* Print Job SOP Class N-EVENT-REPORT Attributes (PRINTING 2) */
  {
   &ModAttrT_Desc_04H045212
  };
S_ListDesc3 ModAttrGroupT_Desc_04H045212 = {E_ListDesc2,1,ModAttrGroupT_04H045212};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04H045213[] =     /* Print Job SOP Class N-EVENT-REPORT Attributes (DONE 3) */
  {
   &ModAttrT_Desc_04H045213
  };
S_ListDesc3 ModAttrGroupT_Desc_04H045213 = {E_ListDesc2,1,ModAttrGroupT_04H045213};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04H045214[] =     /* Print Job SOP Class N-EVENT-REPORT Attributes (FAILURE 4) */
  {
   &ModAttrT_Desc_04H045214
  };
S_ListDesc3 ModAttrGroupT_Desc_04H045214 = {E_ListDesc2,1,ModAttrGroupT_04H045214};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04H04522[] =     /* Print Job SOP Class N-GET Attributes */
  {
   &ModAttrT_Desc_04H04522
  };
S_ListDesc3 ModAttrGroupT_Desc_04H04522 = {E_ListDesc2,1,ModAttrGroupT_04H04522};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04H046212[] =     /* PRINTER SOP Class N-EVENT-REPORT Attributes (WARNING 2) */
  {
   &ModAttrT_Desc_04H046212
  };
S_ListDesc3 ModAttrGroupT_Desc_04H046212 = {E_ListDesc2,1,ModAttrGroupT_04H046212};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04H046213[] =     /* PRINTER SOP Class N-EVENT-REPORT Attributes (FAILURE 3) */
  {
   &ModAttrT_Desc_04H046213
  };
S_ListDesc3 ModAttrGroupT_Desc_04H046213 = {E_ListDesc2,1,ModAttrGroupT_04H046213};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04H04622[] =     /* PRINTER SOP Class N-GET Attributes */
  {
   &ModAttrT_Desc_04H04622
  };
S_ListDesc3 ModAttrGroupT_Desc_04H04622 = {E_ListDesc2,1,ModAttrGroupT_04H04622};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04H04721[] =     /* VOI LUT Box SOP Class N-CREATE Attributes */
  {
   &ModAttrT_Desc_04H04721,
   &ModAttrT_Desc_04H04721SQVOILUTSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04H04721 = {E_ListDesc2,2,ModAttrGroupT_04H04721};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04H04821[] =     /* Image Overlay Box SOP Class N-CREATE Attributes */
  {
   &ModAttrT_Desc_04H04821,
   &ModAttrT_Desc_04H04821SQReferencedImageBoxSequence2040,
   &ModAttrT_Desc_04H04821SQReferencedOverlayPlaneSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04H04821 = {E_ListDesc2,3,ModAttrGroupT_04H04821};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_04H04822[] =     /* Image Overlay Box SOP Class N-SET Attributes */
  {
   &ModAttrT_Desc_04H04822,
   &ModAttrT_Desc_04H04822SQReferencedOverlayPlaneSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_04H04822 = {E_ListDesc2,2,ModAttrGroupT_04H04822};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07090311[] =     /* C-STORE-RQ Message Fields */
  {
   &ModAttrT_Desc_07090311
  };
S_ListDesc3 ModAttrGroupT_Desc_07090311 = {E_ListDesc2,1,ModAttrGroupT_07090311};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07090312[] =     /* C-STORE-RSP Message Fields */
  {
   &ModAttrT_Desc_07090312
  };
S_ListDesc3 ModAttrGroupT_Desc_07090312 = {E_ListDesc2,1,ModAttrGroupT_07090312};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07090321[] =     /* C-FIND-RQ Message Fields */
  {
   &ModAttrT_Desc_07090321
  };
S_ListDesc3 ModAttrGroupT_Desc_07090321 = {E_ListDesc2,1,ModAttrGroupT_07090321};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07090322[] =     /* C-FIND-RSP Message Fields */
  {
   &ModAttrT_Desc_07090322
  };
S_ListDesc3 ModAttrGroupT_Desc_07090322 = {E_ListDesc2,1,ModAttrGroupT_07090322};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07090323[] =     /* C-CANCEL-FIND-RQ Message Fields */
  {
   &ModAttrT_Desc_07090323
  };
S_ListDesc3 ModAttrGroupT_Desc_07090323 = {E_ListDesc2,1,ModAttrGroupT_07090323};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07090331[] =     /* C-GET-RQ Message Fields */
  {
   &ModAttrT_Desc_07090331
  };
S_ListDesc3 ModAttrGroupT_Desc_07090331 = {E_ListDesc2,1,ModAttrGroupT_07090331};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07090332[] =     /* C-GET-RSP Message Fields */
  {
   &ModAttrT_Desc_07090332
  };
S_ListDesc3 ModAttrGroupT_Desc_07090332 = {E_ListDesc2,1,ModAttrGroupT_07090332};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07090333[] =     /* C-CANCEL-GET-RQ Message Fields */
  {
   &ModAttrT_Desc_07090333
  };
S_ListDesc3 ModAttrGroupT_Desc_07090333 = {E_ListDesc2,1,ModAttrGroupT_07090333};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07090341[] =     /* C-MOVE-RQ Message Fields */
  {
   &ModAttrT_Desc_07090341
  };
S_ListDesc3 ModAttrGroupT_Desc_07090341 = {E_ListDesc2,1,ModAttrGroupT_07090341};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07090342[] =     /* C-MOVE-RSP Message Fields */
  {
   &ModAttrT_Desc_07090342
  };
S_ListDesc3 ModAttrGroupT_Desc_07090342 = {E_ListDesc2,1,ModAttrGroupT_07090342};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07090343[] =     /* C-CANCEL-MOVE-RQ Message Fields */
  {
   &ModAttrT_Desc_07090343
  };
S_ListDesc3 ModAttrGroupT_Desc_07090343 = {E_ListDesc2,1,ModAttrGroupT_07090343};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07090351[] =     /* C-ECHO-RQ Message Fields */
  {
   &ModAttrT_Desc_07090351
  };
S_ListDesc3 ModAttrGroupT_Desc_07090351 = {E_ListDesc2,1,ModAttrGroupT_07090351};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07090352[] =     /* C-ECHO-RSP Message Fields */
  {
   &ModAttrT_Desc_07090352
  };
S_ListDesc3 ModAttrGroupT_Desc_07090352 = {E_ListDesc2,1,ModAttrGroupT_07090352};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07100311[] =     /* N-EVENT-REPORT-RQ Message Fields */
  {
   &ModAttrT_Desc_07100311
  };
S_ListDesc3 ModAttrGroupT_Desc_07100311 = {E_ListDesc2,1,ModAttrGroupT_07100311};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07100312[] =     /* N-EVENT-REPORT-RSP Message Fields */
  {
   &ModAttrT_Desc_07100312
  };
S_ListDesc3 ModAttrGroupT_Desc_07100312 = {E_ListDesc2,1,ModAttrGroupT_07100312};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07100321[] =     /* N-GET-RQ Message Fields */
  {
   &ModAttrT_Desc_07100321
  };
S_ListDesc3 ModAttrGroupT_Desc_07100321 = {E_ListDesc2,1,ModAttrGroupT_07100321};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07100322[] =     /* N-GET-RSP Message Fields */
  {
   &ModAttrT_Desc_07100322
  };
S_ListDesc3 ModAttrGroupT_Desc_07100322 = {E_ListDesc2,1,ModAttrGroupT_07100322};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07100331[] =     /* N-SET-RQ Message Fields */
  {
   &ModAttrT_Desc_07100331
  };
S_ListDesc3 ModAttrGroupT_Desc_07100331 = {E_ListDesc2,1,ModAttrGroupT_07100331};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07100332[] =     /* N-SET-RSP Message Fields */
  {
   &ModAttrT_Desc_07100332
  };
S_ListDesc3 ModAttrGroupT_Desc_07100332 = {E_ListDesc2,1,ModAttrGroupT_07100332};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07100341[] =     /* N-ACTION-RQ Message Fields */
  {
   &ModAttrT_Desc_07100341
  };
S_ListDesc3 ModAttrGroupT_Desc_07100341 = {E_ListDesc2,1,ModAttrGroupT_07100341};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07100342[] =     /* N-ACTION-RSP Message Fields */
  {
   &ModAttrT_Desc_07100342
  };
S_ListDesc3 ModAttrGroupT_Desc_07100342 = {E_ListDesc2,1,ModAttrGroupT_07100342};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07100351[] =     /* N-CREATE-RQ Message Fields */
  {
   &ModAttrT_Desc_07100351
  };
S_ListDesc3 ModAttrGroupT_Desc_07100351 = {E_ListDesc2,1,ModAttrGroupT_07100351};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07100352[] =     /* N-CREATE-RSP Message Fields */
  {
   &ModAttrT_Desc_07100352
  };
S_ListDesc3 ModAttrGroupT_Desc_07100352 = {E_ListDesc2,1,ModAttrGroupT_07100352};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07100361[] =     /* N-DELETE-RQ Message Fields */
  {
   &ModAttrT_Desc_07100361
  };
S_ListDesc3 ModAttrGroupT_Desc_07100361 = {E_ListDesc2,1,ModAttrGroupT_07100361};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07100362[] =     /* N-DELETE-RSP Message Fields */
  {
   &ModAttrT_Desc_07100362
  };
S_ListDesc3 ModAttrGroupT_Desc_07100362 = {E_ListDesc2,1,ModAttrGroupT_07100362};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0101[] =     /* Success */
  {
   &ModAttrT_Desc_07C0101
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0101 = {E_ListDesc2,1,ModAttrGroupT_07C0101};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0201[] =     /* Pending */
  {
   &ModAttrT_Desc_07C0201
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0201 = {E_ListDesc2,1,ModAttrGroupT_07C0201};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0301[] =     /* Cancel */
  {
   &ModAttrT_Desc_07C0301
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0301 = {E_ListDesc2,1,ModAttrGroupT_07C0301};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0401[] =     /* Warning */
  {
   &ModAttrT_Desc_07C0401
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0401 = {E_ListDesc2,1,ModAttrGroupT_07C0401};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0402[] =     /* Attribute List Error */
  {
   &ModAttrT_Desc_07C0402
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0402 = {E_ListDesc2,1,ModAttrGroupT_07C0402};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0501[] =     /* Error: Cannot Understand */
  {
   &ModAttrT_Desc_07C0501
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0501 = {E_ListDesc2,1,ModAttrGroupT_07C0501};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0502[] =     /* Error: Data Set does not Match SOP Class */
  {
   &ModAttrT_Desc_07C0502
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0502 = {E_ListDesc2,1,ModAttrGroupT_07C0502};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0503[] =     /* Failed */
  {
   &ModAttrT_Desc_07C0503
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0503 = {E_ListDesc2,1,ModAttrGroupT_07C0503};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0504[] =     /* Refused: Move Destination Unknown */
  {
   &ModAttrT_Desc_07C0504
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0504 = {E_ListDesc2,1,ModAttrGroupT_07C0504};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0505[] =     /* Refused: Out of Resources */
  {
   &ModAttrT_Desc_07C0505
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0505 = {E_ListDesc2,1,ModAttrGroupT_07C0505};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0506[] =     /* Refused: SOP Class Not Supported */
  {
   &ModAttrT_Desc_07C0506
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0506 = {E_ListDesc2,1,ModAttrGroupT_07C0506};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0507[] =     /* Class-Instance Conflict */
  {
   &ModAttrT_Desc_07C0507
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0507 = {E_ListDesc2,1,ModAttrGroupT_07C0507};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0508[] =     /* Duplicate SOP Instance */
  {
   &ModAttrT_Desc_07C0508
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0508 = {E_ListDesc2,1,ModAttrGroupT_07C0508};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0509[] =     /* Duplicate Invocation */
  {
   &ModAttrT_Desc_07C0509
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0509 = {E_ListDesc2,1,ModAttrGroupT_07C0509};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0510[] =     /* Invalid Argument Value */
  {
   &ModAttrT_Desc_07C0510
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0510 = {E_ListDesc2,1,ModAttrGroupT_07C0510};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0511[] =     /* Invalid Attribute Value */
  {
   &ModAttrT_Desc_07C0511
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0511 = {E_ListDesc2,1,ModAttrGroupT_07C0511};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0512[] =     /* Invalid Object Instance */
  {
   &ModAttrT_Desc_07C0512
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0512 = {E_ListDesc2,1,ModAttrGroupT_07C0512};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0513[] =     /* Missing Attribute */
  {
   &ModAttrT_Desc_07C0513
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0513 = {E_ListDesc2,1,ModAttrGroupT_07C0513};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0514[] =     /* Missing Attribute Value */
  {
   &ModAttrT_Desc_07C0514
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0514 = {E_ListDesc2,1,ModAttrGroupT_07C0514};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0515[] =     /* Mistyped Argument */
  {
   &ModAttrT_Desc_07C0515
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0515 = {E_ListDesc2,1,ModAttrGroupT_07C0515};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0516[] =     /* No Such Argument */
  {
   &ModAttrT_Desc_07C0516
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0516 = {E_ListDesc2,1,ModAttrGroupT_07C0516};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0517[] =     /* No Such Attribute */
  {
   &ModAttrT_Desc_07C0517
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0517 = {E_ListDesc2,1,ModAttrGroupT_07C0517};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0518[] =     /* No Such Event Type */
  {
   &ModAttrT_Desc_07C0518
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0518 = {E_ListDesc2,1,ModAttrGroupT_07C0518};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0519[] =     /* No Such Object Instance */
  {
   &ModAttrT_Desc_07C0519
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0519 = {E_ListDesc2,1,ModAttrGroupT_07C0519};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0520[] =     /* No Such SOP Class */
  {
   &ModAttrT_Desc_07C0520
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0520 = {E_ListDesc2,1,ModAttrGroupT_07C0520};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0521[] =     /* Processing Failure */
  {
   &ModAttrT_Desc_07C0521
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0521 = {E_ListDesc2,1,ModAttrGroupT_07C0521};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0522[] =     /* Resource Limitation */
  {
   &ModAttrT_Desc_07C0522
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0522 = {E_ListDesc2,1,ModAttrGroupT_07C0522};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_07C0523[] =     /* Unrecognized Operation */
  {
   &ModAttrT_Desc_07C0523
  };
S_ListDesc3 ModAttrGroupT_Desc_07C0523 = {E_ListDesc2,1,ModAttrGroupT_07C0523};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_100701[] =     /* DICOM File Meta Information */
  {
   &ModAttrT_Desc_100701
  };
S_ListDesc3 ModAttrGroupT_Desc_100701 = {E_ListDesc2,1,ModAttrGroupT_100701};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_10B0321[] =     /* File-set Identification Module */
  {
   &ModAttrT_Desc_10B0321,
   &ModAttrT_Desc_10B0321V1FormatoftheFilesetDescriptorFile
  };
S_ListDesc3 ModAttrGroupT_Desc_10B0321 = {E_ListDesc2,2,ModAttrGroupT_10B0321};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_10B0322[] =     /* Directory Information Module */
  {
   &ModAttrT_Desc_10B0322,
   &ModAttrT_Desc_10B0322SQDirectoryRecordSequence,
   &ModAttrT_Desc_10B0322V1DirectoryRecordType,
   &ModAttrT_Desc_10B0322V1FilesetConsistencyFlag,
   &ModAttrT_Desc_10B0322V1RecordInuseFlag
  };
S_ListDesc3 ModAttrGroupT_Desc_10B0322 = {E_ListDesc2,5,ModAttrGroupT_10B0322};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_10B0501[] =     /* Patient Keys */
  {
   &ModAttrT_Desc_10B0501
  };
S_ListDesc3 ModAttrGroupT_Desc_10B0501 = {E_ListDesc2,1,ModAttrGroupT_10B0501};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_10B0502[] =     /* Study Keys */
  {
   &ModAttrT_Desc_10B0502
  };
S_ListDesc3 ModAttrGroupT_Desc_10B0502 = {E_ListDesc2,1,ModAttrGroupT_10B0502};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_10B0503[] =     /* Series Keys */
  {
   &ModAttrT_Desc_10B0503
  };
S_ListDesc3 ModAttrGroupT_Desc_10B0503 = {E_ListDesc2,1,ModAttrGroupT_10B0503};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_10B0504[] =     /* Image Keys */
  {
   &ModAttrT_Desc_10B0504
  };
S_ListDesc3 ModAttrGroupT_Desc_10B0504 = {E_ListDesc2,1,ModAttrGroupT_10B0504};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_10B0505[] =     /* Overlay Keys */
  {
   &ModAttrT_Desc_10B0505
  };
S_ListDesc3 ModAttrGroupT_Desc_10B0505 = {E_ListDesc2,1,ModAttrGroupT_10B0505};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_10B0506[] =     /* Modality LUT Keys */
  {
   &ModAttrT_Desc_10B0506
  };
S_ListDesc3 ModAttrGroupT_Desc_10B0506 = {E_ListDesc2,1,ModAttrGroupT_10B0506};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_10B0507[] =     /* VOI LUT Keys */
  {
   &ModAttrT_Desc_10B0507
  };
S_ListDesc3 ModAttrGroupT_Desc_10B0507 = {E_ListDesc2,1,ModAttrGroupT_10B0507};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_10B0508[] =     /* Curve Keys */
  {
   &ModAttrT_Desc_10B0508
  };
S_ListDesc3 ModAttrGroupT_Desc_10B0508 = {E_ListDesc2,1,ModAttrGroupT_10B0508};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_10B0509[] =     /* Topic Keys */
  {
   &ModAttrT_Desc_10B0509
  };
S_ListDesc3 ModAttrGroupT_Desc_10B0509 = {E_ListDesc2,1,ModAttrGroupT_10B0509};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_10B0510[] =     /* Visit Keys */
  {
   &ModAttrT_Desc_10B0510
  };
S_ListDesc3 ModAttrGroupT_Desc_10B0510 = {E_ListDesc2,1,ModAttrGroupT_10B0510};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_10B0511[] =     /* Result Keys */
  {
   &ModAttrT_Desc_10B0511
  };
S_ListDesc3 ModAttrGroupT_Desc_10B0511 = {E_ListDesc2,1,ModAttrGroupT_10B0511};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_10B0512[] =     /* Interpretation Keys */
  {
   &ModAttrT_Desc_10B0512,
   &ModAttrT_Desc_10B0512SQDiagnosisCodeSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_10B0512 = {E_ListDesc2,2,ModAttrGroupT_10B0512};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_10B0513[] =     /* Study Component Keys */
  {
   &ModAttrT_Desc_10B0513,
   &ModAttrT_Desc_10B0513SQProcedureCodeSequence
  };
S_ListDesc3 ModAttrGroupT_Desc_10B0513 = {E_ListDesc2,2,ModAttrGroupT_10B0513};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_10B0514[] =     /* Print Queue Keys */
  {
   &ModAttrT_Desc_10B0514
  };
S_ListDesc3 ModAttrGroupT_Desc_10B0514 = {E_ListDesc2,1,ModAttrGroupT_10B0514};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_10B0515[] =     /* Film Session Keys */
  {
   &ModAttrT_Desc_10B0515,
   &ModAttrT_Desc_10B0515V1ExecutionStatus
  };
S_ListDesc3 ModAttrGroupT_Desc_10B0515 = {E_ListDesc2,2,ModAttrGroupT_10B0515};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_10B0516[] =     /* Basic Film Box Keys */
  {
   &ModAttrT_Desc_10B0516,
   &ModAttrT_Desc_10B0516V1ExecutionStatus
  };
S_ListDesc3 ModAttrGroupT_Desc_10B0516 = {E_ListDesc2,2,ModAttrGroupT_10B0516};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_10B0517[] =     /* Basic Image Box Keys */
  {
   &ModAttrT_Desc_10B0517
  };
S_ListDesc3 ModAttrGroupT_Desc_10B0517 = {E_ListDesc2,1,ModAttrGroupT_10B0517};

/* ModuleAttributeGroupTable references */
S_ListDesc2 ModAttrGroupT_10B0519[] =     /* Multi-Referenced File Keys */
  {
   &ModAttrT_Desc_10B0519
  };
S_ListDesc3 ModAttrGroupT_Desc_10B0519 = {E_ListDesc2,1,ModAttrGroupT_10B0519};

/* ModuleTable references */
S_ListDesc4 ModT_03A0203[] =     /* CR Image IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C07011,ETypeM_IEPatient}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07011,ETypeM_M}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_IEStudy}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_M}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07022,ETypeM_U}  /* Patient Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_IESeries}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_M}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C08011,ETypeM_M}  /* CR Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_IEEquipment}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_M}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C07061,ETypeM_IEImage}  /* General Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C07061,ETypeM_M}  /* General Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C07063,ETypeM_M}  /* Image Pixel Module Attributes */,
   {&ModAttrGroupT_Desc_03C07065,ETypeM_C}  /* Cine Module Attributes */,
   {&ModAttrGroupT_Desc_03C08012,ETypeM_M}  /* CR Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C0902,ETypeM_U}  /* Overlay Plane Module Attributes */,
   {&ModAttrGroupT_Desc_03C1002,ETypeM_U}  /* Curve Module Attributes */,
   {&ModAttrGroupT_Desc_03C1101,ETypeM_U}  /* Modality LUT Module Attributes */,
   {&ModAttrGroupT_Desc_03C1102,ETypeM_U}  /* VOI LUT Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_M}  /* SOP Common Module Attributes */
  };
S_ListDesc5 ModT_Desc_03A0203 = {E_ListDesc4,20,ModT_03A0203};

/* ModuleTable references */
S_ListDesc4 ModT_03A0303[] =     /* CT Image IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C07011,ETypeM_IEPatient}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07011,ETypeM_M}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_IEStudy}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_M}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07022,ETypeM_U}  /* Patient Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_IESeries}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_M}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07041,ETypeM_IEFrameOfReference}  /* Frame Of Reference Module Attributes */,
   {&ModAttrGroupT_Desc_03C07041,ETypeM_M}  /* Frame Of Reference Module Attributes */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_IEEquipment}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_M}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C07061,ETypeM_IEImage}  /* General Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C07061,ETypeM_M}  /* General Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C07062,ETypeM_M}  /* Image Plane Module Attributes */,
   {&ModAttrGroupT_Desc_03C07063,ETypeM_M}  /* Image Pixel Module Attributes */,
   {&ModAttrGroupT_Desc_03C07064,ETypeM_C}  /* Contrast/Bolus Module Attributes */,
   {&ModAttrGroupT_Desc_03C08021,ETypeM_M}  /* CT Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C0902,ETypeM_U}  /* Overlay Plane Module Attributes */,
   {&ModAttrGroupT_Desc_03C1102,ETypeM_U}  /* VOI LUT Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_M}  /* SOP Common Module Attributes */
  };
S_ListDesc5 ModT_Desc_03A0303 = {E_ListDesc4,20,ModT_03A0303};

/* ModuleTable references */
S_ListDesc4 ModT_03A0403[] =     /* MR Image IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C07011,ETypeM_IEPatient}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07011,ETypeM_M}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_IEStudy}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_M}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07022,ETypeM_U}  /* Patient Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_IESeries}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_M}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07041,ETypeM_IEFrameOfReference}  /* Frame Of Reference Module Attributes */,
   {&ModAttrGroupT_Desc_03C07041,ETypeM_M}  /* Frame Of Reference Module Attributes */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_IEEquipment}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_M}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C07061,ETypeM_IEImage}  /* General Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C07061,ETypeM_M}  /* General Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C07062,ETypeM_M}  /* Image Plane Module Attributes */,
   {&ModAttrGroupT_Desc_03C07063,ETypeM_M}  /* Image Pixel Module Attributes */,
   {&ModAttrGroupT_Desc_03C07064,ETypeM_C}  /* Contrast/Bolus Module Attributes */,
   {&ModAttrGroupT_Desc_03C08031,ETypeM_M}  /* MR Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C0902,ETypeM_U}  /* Overlay Plane Module Attributes */,
   {&ModAttrGroupT_Desc_03C1102,ETypeM_U}  /* VOI LUT Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_M}  /* SOP Common Module Attributes */
  };
S_ListDesc5 ModT_Desc_03A0403 = {E_ListDesc4,20,ModT_03A0403};

/* ModuleTable references */
S_ListDesc4 ModT_03A0503[] =     /* NM Image IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C07011,ETypeM_IEPatient}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07011,ETypeM_M}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_IEStudy}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_M}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07022,ETypeM_U}  /* Patient Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_IESeries}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_M}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C08041,ETypeM_M}  /* NM Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07041,ETypeM_IEFrameOfReference}  /* Frame Of Reference Module Attributes */,
   {&ModAttrGroupT_Desc_03C07041,ETypeM_U}  /* Frame Of Reference Module Attributes */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_IEEquipment}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_M}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C08042,ETypeM_U}  /* NM Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C07061,ETypeM_IEImageNoOtherIE}  /* General Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C07061,ETypeM_M}  /* General Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C07062,ETypeM_U}  /* Image Plane Module Attributes */,
   {&ModAttrGroupT_Desc_03C07063,ETypeM_M}  /* Image Pixel Module Attributes */,
   {&ModAttrGroupT_Desc_03C07065,ETypeM_C}  /* Cine Module Attributes */,
   {&ModAttrGroupT_Desc_03C07066,ETypeM_C}  /* Multi-frame Module Attributes */,
   {&ModAttrGroupT_Desc_03C08043,ETypeM_M}  /* NM Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C08044,ETypeM_C}  /* NM SPECT Acquisition Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C08045,ETypeM_C}  /* NM Multi-gated Acquisition Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C0902,ETypeM_U}  /* Overlay Plane Module Attributes */,
   {&ModAttrGroupT_Desc_03C0903,ETypeM_U}  /* Multi-frame Overlay Module Attributes */,
   {&ModAttrGroupT_Desc_03C1002,ETypeM_U}  /* Curve Module Attributes */,
   {&ModAttrGroupT_Desc_03C1102,ETypeM_U}  /* VOI LUT Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_M}  /* SOP Common Module Attributes */,
   {&ModAttrGroupT_Desc_03C0901,ETypeM_IEOverlayNoOtherIE}  /* Overlay Identification Module Attributes */,
   {&ModAttrGroupT_Desc_03C0901,ETypeM_M}  /* Overlay Identification Module Attributes */,
   {&ModAttrGroupT_Desc_03C0902,ETypeM_M}  /* Overlay Plane Module Attributes */,
   {&ModAttrGroupT_Desc_03C0903,ETypeM_M}  /* Multi-frame Overlay Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_M}  /* SOP Common Module Attributes */,
   {&ModAttrGroupT_Desc_03C1001,ETypeM_IECurveNoOtherIE}  /* Curve Identification Module Attributes */,
   {&ModAttrGroupT_Desc_03C1001,ETypeM_M}  /* Curve Identification Module Attributes */,
   {&ModAttrGroupT_Desc_03C1002,ETypeM_M}  /* Curve Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_M}  /* SOP Common Module Attributes */
  };
S_ListDesc5 ModT_Desc_03A0503 = {E_ListDesc4,36,ModT_03A0503};

/* ModuleTable references */
S_ListDesc4 ModT_03A0603[] =     /* US Image IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C07011,ETypeM_IEPatient}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07011,ETypeM_M}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_IEStudy}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_M}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07022,ETypeM_U}  /* Patient Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_IESeries}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_M}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07041,ETypeM_IEFrameOfReference}  /* Frame Of Reference Module Attributes */,
   {&ModAttrGroupT_Desc_03C07041,ETypeM_U}  /* Frame Of Reference Module Attributes */,
   {&ModAttrGroupT_Desc_03C08051,ETypeM_C}  /* US Frame of Reference Module */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_IEEquipment}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_M}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C07061,ETypeM_IEImageNoOtherIE}  /* General Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C07061,ETypeM_M}  /* General Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C07063,ETypeM_M}  /* Image Pixel Module Attributes */,
   {&ModAttrGroupT_Desc_03C07064,ETypeM_C}  /* Contrast/Bolus Module Attributes */,
   {&ModAttrGroupT_Desc_03C08052,ETypeM_U}  /* US Region Calibration Module Attributes */,
   {&ModAttrGroupT_Desc_03C08053,ETypeM_M}  /* US Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C0902,ETypeM_U}  /* Overlay Plane Module Attributes */,
   {&ModAttrGroupT_Desc_03C1102,ETypeM_U}  /* VOI LUT Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_M}  /* SOP Common Module Attributes */,
   {&ModAttrGroupT_Desc_03C1001,ETypeM_IECurveNoOtherIE}  /* Curve Identification Module Attributes */,
   {&ModAttrGroupT_Desc_03C1001,ETypeM_M}  /* Curve Identification Module Attributes */,
   {&ModAttrGroupT_Desc_03C1002,ETypeM_M}  /* Curve Module Attributes */,
   {&ModAttrGroupT_Desc_03C1003,ETypeM_U}  /* Audio Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_M}  /* SOP Common Module Attributes */
  };
S_ListDesc5 ModT_Desc_03A0603 = {E_ListDesc4,26,ModT_03A0603};

/* ModuleTable references */
S_ListDesc4 ModT_03A0703[] =     /* US Multi-frame Image IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C07011,ETypeM_IEPatient}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07011,ETypeM_M}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_IEStudy}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_M}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07022,ETypeM_U}  /* Patient Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_IESeries}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_M}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07041,ETypeM_IEFrameOfReference}  /* Frame Of Reference Module Attributes */,
   {&ModAttrGroupT_Desc_03C07041,ETypeM_U}  /* Frame Of Reference Module Attributes */,
   {&ModAttrGroupT_Desc_03C08051,ETypeM_C}  /* US Frame of Reference Module */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_IEEquipment}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_M}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C07061,ETypeM_IEImageNoOtherIE}  /* General Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C07061,ETypeM_M}  /* General Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C07063,ETypeM_M}  /* Image Pixel Module Attributes */,
   {&ModAttrGroupT_Desc_03C07064,ETypeM_C}  /* Contrast/Bolus Module Attributes */,
   {&ModAttrGroupT_Desc_03C07065,ETypeM_C}  /* Cine Module Attributes */,
   {&ModAttrGroupT_Desc_03C07066,ETypeM_M}  /* Multi-frame Module Attributes */,
   {&ModAttrGroupT_Desc_03C08052,ETypeM_U}  /* US Region Calibration Module Attributes */,
   {&ModAttrGroupT_Desc_03C08053,ETypeM_M}  /* US Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C1102,ETypeM_U}  /* VOI LUT Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_M}  /* SOP Common Module Attributes */,
   {&ModAttrGroupT_Desc_03C1001,ETypeM_IECurveNoOtherIE}  /* Curve Identification Module Attributes */,
   {&ModAttrGroupT_Desc_03C1001,ETypeM_M}  /* Curve Identification Module Attributes */,
   {&ModAttrGroupT_Desc_03C1002,ETypeM_M}  /* Curve Module Attributes */,
   {&ModAttrGroupT_Desc_03C1003,ETypeM_U}  /* Audio Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_M}  /* SOP Common Module Attributes */
  };
S_ListDesc5 ModT_Desc_03A0703 = {E_ListDesc4,27,ModT_03A0703};

/* ModuleTable references */
S_ListDesc4 ModT_03A0803[] =     /* SC Image IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C07011,ETypeM_IEPatient}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07011,ETypeM_M}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_IEStudy}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_M}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07022,ETypeM_U}  /* Patient Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_IESeries}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_M}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_IEEquipment}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_U}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C08061,ETypeM_M}  /* SC Image Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C07061,ETypeM_IEImage}  /* General Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C07061,ETypeM_M}  /* General Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C07063,ETypeM_M}  /* Image Pixel Module Attributes */,
   {&ModAttrGroupT_Desc_03C08062,ETypeM_M}  /* SC Image Module Attributes */,
   {&ModAttrGroupT_Desc_03C0902,ETypeM_U}  /* Overlay Plane Module Attributes */,
   {&ModAttrGroupT_Desc_03C1101,ETypeM_U}  /* Modality LUT Module Attributes */,
   {&ModAttrGroupT_Desc_03C1102,ETypeM_U}  /* VOI LUT Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_M}  /* SOP Common Module Attributes */
  };
S_ListDesc5 ModT_Desc_03A0803 = {E_ListDesc4,18,ModT_03A0803};

/* ModuleTable references */
S_ListDesc4 ModT_03A0903[] =     /* Standalone Overlay IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C07011,ETypeM_IEPatient}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07011,ETypeM_M}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_IEStudy}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_M}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07022,ETypeM_U}  /* Patient Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_IESeries}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_M}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_IEEquipment}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_M}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C0901,ETypeM_IEOverlay}  /* Overlay Identification Module Attributes */,
   {&ModAttrGroupT_Desc_03C0901,ETypeM_M}  /* Overlay Identification Module Attributes */,
   {&ModAttrGroupT_Desc_03C0902,ETypeM_M}  /* Overlay Plane Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_M}  /* SOP Common Module Attributes */
  };
S_ListDesc5 ModT_Desc_03A0903 = {E_ListDesc4,13,ModT_03A0903};

/* ModuleTable references */
S_ListDesc4 ModT_03A1003[] =     /* Standalone Curve IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C07011,ETypeM_IEPatient}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07011,ETypeM_M}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_IEStudy}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_M}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07022,ETypeM_U}  /* Patient Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_IESeries}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_M}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_IEEquipment}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_M}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C1001,ETypeM_IECurve}  /* Curve Identification Module Attributes */,
   {&ModAttrGroupT_Desc_03C1001,ETypeM_M}  /* Curve Identification Module Attributes */,
   {&ModAttrGroupT_Desc_03C1002,ETypeM_M}  /* Curve Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_M}  /* SOP Common Module Attributes */
  };
S_ListDesc5 ModT_Desc_03A1003 = {E_ListDesc4,13,ModT_03A1003};

/* ModuleTable references */
S_ListDesc4 ModT_03A1103[] =     /* Basic Study Descriptor IOD Modul Table */
  {
   {&ModAttrGroupT_Desc_03C0707,ETypeM_M}  /* Patient Summary Module Attributes */,
   {&ModAttrGroupT_Desc_03C0708,ETypeM_M}  /* Study Content Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_M}  /* SOP Common Module Attributes */
  };
S_ListDesc5 ModT_Desc_03A1103 = {E_ListDesc4,3,ModT_03A1103};

/* ModuleTable references */
S_ListDesc4 ModT_03A1203[] =     /* Standalone Modality IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C07011,ETypeM_IEPatient}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07011,ETypeM_M}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_IEStudy}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_M}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07022,ETypeM_U}  /* Patient Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_IESeries}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_M}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_IEEquipment}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_M}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C1101,ETypeM_IEModalityLUT}  /* Modality LUT Module Attributes */,
   {&ModAttrGroupT_Desc_03C1101,ETypeM_M}  /* Modality LUT Module Attributes */,
   {&ModAttrGroupT_Desc_03C1103,ETypeM_M}  /* LUT Identification Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_M}  /* SOP Common Module Attributes */
  };
S_ListDesc5 ModT_Desc_03A1203 = {E_ListDesc4,13,ModT_03A1203};

/* ModuleTable references */
S_ListDesc4 ModT_03A1303[] =     /* Standalone VOI LUTI IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C07011,ETypeM_IEPatient}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07011,ETypeM_M}  /* Patient Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_IEStudy}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07021,ETypeM_M}  /* General Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07022,ETypeM_U}  /* Patient Study Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_IESeries}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07031,ETypeM_M}  /* General Series Module Attributes */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_IEEquipment}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C07051,ETypeM_M}  /* General Equipment Module Attributes */,
   {&ModAttrGroupT_Desc_03C1102,ETypeM_IEVOILUT}  /* VOI LUT Module Attributes */,
   {&ModAttrGroupT_Desc_03C1102,ETypeM_M}  /* VOI LUT Module Attributes */,
   {&ModAttrGroupT_Desc_03C1103,ETypeM_M}  /* LUT Identification Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_M}  /* SOP Common Module Attributes */
  };
S_ListDesc5 ModT_Desc_03A1303 = {E_ListDesc4,13,ModT_03A1303};

/* ModuleTable references */
S_ListDesc4 ModT_03B0102[] =     /* Patient IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C0201,ETypeM_X}  /* Patient Relationship Module Attributes */,
   {&ModAttrGroupT_Desc_03C0202,ETypeM_X}  /* Patient Identification Module Attributes */,
   {&ModAttrGroupT_Desc_03C0203,ETypeM_X}  /* Patient Demographic Module Attributes */,
   {&ModAttrGroupT_Desc_03C0204,ETypeM_X}  /* Patient Medical Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_X}  /* SOP Common Module Attributes */
  };
S_ListDesc5 ModT_Desc_03B0102 = {E_ListDesc4,5,ModT_03B0102};

/* ModuleTable references */
S_ListDesc4 ModT_03B0202[] =     /* Visit IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C0301,ETypeM_X}  /* Visit Relationship Module Attributes */,
   {&ModAttrGroupT_Desc_03C0302,ETypeM_X}  /* Visit Identification Module Attributes */,
   {&ModAttrGroupT_Desc_03C0303,ETypeM_X}  /* Visit Status Module Attributes */,
   {&ModAttrGroupT_Desc_03C0304,ETypeM_X}  /* Visit Admission Module Attributes */,
   {&ModAttrGroupT_Desc_03C0305,ETypeM_X}  /* Visit Discharge Module Attributes */,
   {&ModAttrGroupT_Desc_03C0306,ETypeM_X}  /* Visit Scheduling Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_X}  /* SOP Common Module Attributes */
  };
S_ListDesc5 ModT_Desc_03B0202 = {E_ListDesc4,7,ModT_03B0202};

/* ModuleTable references */
S_ListDesc4 ModT_03B0302[] =     /* Study IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C0401,ETypeM_X}  /* Study Relationship Module Attributes */,
   {&ModAttrGroupT_Desc_03C0402,ETypeM_X}  /* Study Identification Module Attributes */,
   {&ModAttrGroupT_Desc_03C0403,ETypeM_X}  /* Study Classification Module Attributes */,
   {&ModAttrGroupT_Desc_03C0404,ETypeM_X}  /* Study Scheduling Module Attributes */,
   {&ModAttrGroupT_Desc_03C0405,ETypeM_X}  /* Study Acquisition Module Attributes */,
   {&ModAttrGroupT_Desc_03C0406,ETypeM_X}  /* Study Read Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_X}  /* SOP Common Module Attributes */
  };
S_ListDesc5 ModT_Desc_03B0302 = {E_ListDesc4,7,ModT_03B0302};

/* ModuleTable references */
S_ListDesc4 ModT_03B0402[] =     /* Study Component IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C0407,ETypeM_X}  /* Study Component Module Attributes */,
   {&ModAttrGroupT_Desc_03C0408,ETypeM_X}  /* Study Component Relationship Module Attributes */,
   {&ModAttrGroupT_Desc_03C0409,ETypeM_X}  /* Study Component Acquisition  Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_X}  /* SOP Common Module Attributes */
  };
S_ListDesc5 ModT_Desc_03B0402 = {E_ListDesc4,4,ModT_03B0402};

/* ModuleTable references */
S_ListDesc4 ModT_03B0502[] =     /* Results IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C0501,ETypeM_X}  /* Results Relationship Module Attributes */,
   {&ModAttrGroupT_Desc_03C0502,ETypeM_X}  /* Results Identification Module Attributes */,
   {&ModAttrGroupT_Desc_03C0503,ETypeM_X}  /* Results Impression Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_X}  /* SOP Common Module Attributes */
  };
S_ListDesc5 ModT_Desc_03B0502 = {E_ListDesc4,4,ModT_03B0502};

/* ModuleTable references */
S_ListDesc4 ModT_03B0602[] =     /* Interpretation IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C0601,ETypeM_X}  /* Interpretation Relationship Module Attributes */,
   {&ModAttrGroupT_Desc_03C0602,ETypeM_X}  /* Interpretation Identification Module Attributes */,
   {&ModAttrGroupT_Desc_03C0603,ETypeM_X}  /* Interpretation State Module Attributes */,
   {&ModAttrGroupT_Desc_03C0604,ETypeM_X}  /* Interpretation Recording Module Attributes */,
   {&ModAttrGroupT_Desc_03C0605,ETypeM_X}  /* Interpretation Transcription Module Attributes */,
   {&ModAttrGroupT_Desc_03C0606,ETypeM_X}  /* Interpretation Approval Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_X}  /* SOP Common Module Attributes */
  };
S_ListDesc5 ModT_Desc_03B0602 = {E_ListDesc4,7,ModT_03B0602};

/* ModuleTable references */
S_ListDesc4 ModT_03B0702[] =     /* Film Session IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C1201,ETypeM_X}  /* SOP Common Module Attributes */,
   {&ModAttrGroupT_Desc_03C1301,ETypeM_X}  /* Basic Film Session Presentation Module */,
   {&ModAttrGroupT_Desc_03C1302,ETypeM_X}  /* Basic Film Session Relationship Module */
  };
S_ListDesc5 ModT_Desc_03B0702 = {E_ListDesc4,3,ModT_03B0702};

/* ModuleTable references */
S_ListDesc4 ModT_03B0802[] =     /* Film Box IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C1201,ETypeM_X}  /* SOP Common Module Attributes */,
   {&ModAttrGroupT_Desc_03C1303,ETypeM_X}  /* Basic Film Box Presentation Module */,
   {&ModAttrGroupT_Desc_03C1304,ETypeM_X}  /* Basic Film Box Relationship Module */
  };
S_ListDesc5 ModT_Desc_03B0802 = {E_ListDesc4,3,ModT_03B0802};

/* ModuleTable references */
S_ListDesc4 ModT_03B0902[] =     /* Image Box IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C1201,ETypeM_X}  /* SOP Common Module Attributes */,
   {&ModAttrGroupT_Desc_03C1305,ETypeM_X}  /* Image Box Pixel Presentation Module */,
   {&ModAttrGroupT_Desc_03C1306,ETypeM_X}  /* Image Box Relationship Module */
  };
S_ListDesc5 ModT_Desc_03B0902 = {E_ListDesc4,3,ModT_03B0902};

/* ModuleTable references */
S_ListDesc4 ModT_03B1002[] =     /* Annotation IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C1201,ETypeM_X}  /* SOP Common Module Attributes */,
   {&ModAttrGroupT_Desc_03C1307,ETypeM_X}  /* Basic Annotation Presentation Module */
  };
S_ListDesc5 ModT_Desc_03B1002 = {E_ListDesc4,2,ModT_03B1002};

/* ModuleTable references */
S_ListDesc4 ModT_03B1102[] =     /* Print Job IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C1201,ETypeM_X}  /* SOP Common Module Attributes */,
   {&ModAttrGroupT_Desc_03C1308,ETypeM_X}  /* Print Job Module */
  };
S_ListDesc5 ModT_Desc_03B1102 = {E_ListDesc4,2,ModT_03B1102};

/* ModuleTable references */
S_ListDesc4 ModT_03B1202[] =     /* Printer IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C1201,ETypeM_X}  /* SOP Common Module Attributes */,
   {&ModAttrGroupT_Desc_03C1309,ETypeM_X}  /* Printer Module */
  };
S_ListDesc5 ModT_Desc_03B1202 = {E_ListDesc4,2,ModT_03B1202};

/* ModuleTable references */
S_ListDesc4 ModT_03B1302[] =     /* VOI LUT IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C1102,ETypeM_X}  /* VOI LUT Module Attributes */,
   {&ModAttrGroupT_Desc_03C1201,ETypeM_X}  /* SOP Common Module Attributes */
  };
S_ListDesc5 ModT_Desc_03B1302 = {E_ListDesc4,2,ModT_03B1302};

/* ModuleTable references */
S_ListDesc4 ModT_03B1402[] =     /* Image Overlay Box IOD Modules */
  {
   {&ModAttrGroupT_Desc_03C1201,ETypeM_X}  /* SOP Common Module Attributes */,
   {&ModAttrGroupT_Desc_03C1310,ETypeM_X}  /* Image Overlay Box Presentation Module */,
   {&ModAttrGroupT_Desc_03C1311,ETypeM_X}  /* Image Overlay Box Relationship Module */
  };
S_ListDesc5 ModT_Desc_03B1402 = {E_ListDesc4,3,ModT_03B1402};

/* ModuleTable references */
S_ListDesc4 ModT_04C06IM[] =     /* Information Models */
  {
   {&ModAttrGroupT_Desc_04C060112,ETypeM_X}  /* Patient Level Attributes for the Patient Root Query/Retrieve Information Model */,
   {&ModAttrGroupT_Desc_04C060113,ETypeM_X}  /* Study Level Keys for the Patient Root Query/Retrieve Information Model */,
   {&ModAttrGroupT_Desc_04C060114,ETypeM_X}  /* Series Level Attributes for the Patient Root Query/Retrieve Information Model */,
   {&ModAttrGroupT_Desc_04C060115,ETypeM_X}  /* Image Level Keys for the Patient Root Query/Retrieve Information Model */,
   {&ModAttrGroupT_Desc_04C060212,ETypeM_X}  /* Study Level Keys for the Study Root Query/Retrieve Information Model */
  };
S_ListDesc5 ModT_Desc_04C06IM = {E_ListDesc4,5,ModT_04C06IM};

/* ModuleTable references */
S_ListDesc4 ModT_04E031[] =     /* DIMSE-N Service Group Detached Patient Management SOP Class */
  {
   {&ModAttrGroupT_Desc_04E032,ETypeM_NGet_U_M}  /* Detached Patient Management SOP Class N-Get Attributes */,
   {&ModAttrGroupT_Desc_04E0331,ETypeM_NEventReportPatientCreated_1_M_M}  /* Patient Notification Event Information (Patient Created 1) */,
   {&ModAttrGroupT_Desc_04E0332,ETypeM_NEventReportPatientDeleted_2_M_M}  /* Patient Notification Event Information (Patient Deleted 2) */,
   {&ModAttrGroupT_Desc_04E0333,ETypeM_NEventReportPatientUpdated_3_M_M}  /* Patient Notification Event Information (Patient Updated 3) */
  };
S_ListDesc5 ModT_Desc_04E031 = {E_ListDesc4,4,ModT_04E031};

/* ModuleTable references */
S_ListDesc4 ModT_04E041[] =     /* DIMSE-N Service Group Detached Visit Management SOP Class */
  {
   {&ModAttrGroupT_Desc_04E0421,ETypeM_NGet_U_M}  /* Detached Visit Management SOP Class N-Get Attributes */,
   {&ModAttrGroupT_Desc_04E0422,ETypeM_NSet_U_M}  /* Detached Visit Management SOP Class N-Set Attributes */,
   {&ModAttrGroupT_Desc_04E0431,ETypeM_NEventReportVisitCreated_1_M_M}  /* Visit Notification Event Types and Attributes.(Visit Created 1) */,
   {&ModAttrGroupT_Desc_04E0432,ETypeM_NEventReportVisitScheduled_2_M_M}  /* Visit Notification Event Types and Attributes.(Visit Scheduled 2) */,
   {&ModAttrGroupT_Desc_04E0433,ETypeM_NEventReportPatientAdmitted_3_M_M}  /* Visit Notification Event Types and Attributes.(Patient Admitted 3) */,
   {&ModAttrGroupT_Desc_04E0434,ETypeM_NEventReportPatientTransferred_4_M_M}  /* Visit Notification Event Types and Attributes.(Patient Transferred 4) */,
   {&ModAttrGroupT_Desc_04E0435,ETypeM_NEventReportPatientDischarged_5_M_M}  /* Visit Notification Event Types and Attributes.(Patient Discharged 5) */,
   {&ModAttrGroupT_Desc_04E0436,ETypeM_NEventReportVisitDeleted_6_M_M}  /* Visit Notification Event Types and Attributes.(Visit Deleted 6) */,
   {&ModAttrGroupT_Desc_04E0437,ETypeM_NEventReportVisitUpdated_7_M_M}  /* Visit Notification Event Types and Attributes.(Visit Updated 7) */
  };
S_ListDesc5 ModT_Desc_04E041 = {E_ListDesc4,9,ModT_04E041};

/* ModuleTable references */
S_ListDesc4 ModT_04F031[] =     /* DIMSE-N Service Group Detached Study Management SOP CLass */
  {
   {&ModAttrGroupT_Desc_04F0321,ETypeM_NGet_U_M}  /* Detached Study Management SOP Class N-GET Attributes */,
   {&ModAttrGroupT_Desc_04F0322,ETypeM_NSet_U_M}  /* Detached Study Management SOP Class N-Set Attributes */,
   {&ModAttrGroupT_Desc_04F0331,ETypeM_NEventReportStudyCreated_1_M_M}  /* Study Notification Event Information (Study Created 1) */,
   {&ModAttrGroupT_Desc_04F0332,ETypeM_NEventReportStudyScheduled_2_M_M}  /* Study Notification Event Information (Study Scheduled 2) */,
   {&ModAttrGroupT_Desc_04F0333,ETypeM_NEventReportPatientArrived_3_M_M}  /* Study Notification Event Information (Patient Arrived 3) */,
   {&ModAttrGroupT_Desc_04F0334,ETypeM_NEventReportStudyStarted_4_M_M}  /* Study Notification Event Information (Study Started 4) */,
   {&ModAttrGroupT_Desc_04F0335,ETypeM_NEventReportStudyCompleted_5_M_M}  /* Study Notification Event Information (Study Completed 5) */,
   {&ModAttrGroupT_Desc_04F0336,ETypeM_NEventReportStudyVerified_6_M_M}  /* Study Notification Event Information (Study Verified 6) */,
   {&ModAttrGroupT_Desc_04F0337,ETypeM_NEventReportStudyRead_7_M_M}  /* Study Notification Event Information (Study Read 7) */,
   {&ModAttrGroupT_Desc_04F0339,ETypeM_NEventReportStudyUpdated_9_M_M}  /* Study Notification Event Information (Study Updated 9) */
  };
S_ListDesc5 ModT_Desc_04F031 = {E_ListDesc4,10,ModT_04F031};

/* ModuleTable references */
S_ListDesc4 ModT_04F041[] =     /* DIMSE-N Service Group Study Component Management SOP Class */
  {
   {&ModAttrGroupT_Desc_04F0421,ETypeM_NCreate_M_M}  /* Study Component Management SOP Class N-CREATE Attributes */,
   {&ModAttrGroupT_Desc_04F0422,ETypeM_NSet_U_M}  /* Study Component Management SOP Class N-SET Attributes */,
   {&ModAttrGroupT_Desc_04F0423,ETypeM_NGet_U_M}  /* Study Component Management SOP Class N-Get Attributes */
  };
S_ListDesc5 ModT_Desc_04F041 = {E_ListDesc4,3,ModT_04F041};

/* ModuleTable references */
S_ListDesc4 ModT_04G031[] =     /* DIMSE-N Service Group Detached Results Management SOP Class */
  {
   {&ModAttrGroupT_Desc_04G032,ETypeM_NGet_U_M}  /* Detached Results Management SOP Class N-Get Attributes */,
   {&ModAttrGroupT_Desc_04G0331,ETypeM_NEventReportResultsCreated_1_M_M}  /* Results Notification Event Types and Attributes (Results Created 1) */,
   {&ModAttrGroupT_Desc_04G0332,ETypeM_NEventReportResultsDeleted_2_M_M}  /* Results Notification Event Types and Attributes (Results Deleted 2) */,
   {&ModAttrGroupT_Desc_04G0333,ETypeM_NEventReportResultsUpdated_3_M_M}  /* Results Notification Event Types and Attributes (Results Updated 3) */
  };
S_ListDesc5 ModT_Desc_04G031 = {E_ListDesc4,4,ModT_04G031};

/* ModuleTable references */
S_ListDesc4 ModT_04G041[] =     /* DIMSE-N Service Group Detached Interpretation Management SOP Class */
  {
   {&ModAttrGroupT_Desc_04G0421,ETypeM_NGet_U_M}  /* Detached Interpretation Management SOP Class N-Get Attributes */,
   {&ModAttrGroupT_Desc_04G0422,ETypeM_NSet_U_M}  /* Detached Interpretation  Management SOP Class N-Set Attributes */,
   {&ModAttrGroupT_Desc_04G0431,ETypeM_NEventReportInterpretationCreated_1_M_M}  /* Interpretation Notification Event Information (Interpretation Created 1) */,
   {&ModAttrGroupT_Desc_04G0432,ETypeM_NEventReportInterpretationRecorded_2_M_M}  /* Interpretation Notification Event Information (Interpretation Recorded 2) */,
   {&ModAttrGroupT_Desc_04G0433,ETypeM_NEventReportInterpretationTranscribed_3_M_M}  /* Interpretation Notification Event Information (Interpretation Transcribed 3) */,
   {&ModAttrGroupT_Desc_04G0434,ETypeM_NEventReportInterpretationApproved_4_M_M}  /* Interpretation Notification Event Information (Interpretation Approved 4) */,
   {&ModAttrGroupT_Desc_04G0436,ETypeM_NEventReportInterpretationUpdated_6_M_M}  /* Interpretation Notification Event Information (Interpretation Updated 6) */
  };
S_ListDesc5 ModT_Desc_04G041 = {E_ListDesc4,7,ModT_04G041};

/* ModuleTable references */
S_ListDesc4 ModT_04H03221[] =     /* Basic Greyscale Print Management Meta SOP Class */
  {
   {NULL,ETypeM_UNKNOWN}
  };
S_ListDesc5 ModT_Desc_04H03221 = {E_ListDesc4,0,ModT_04H03221};

/* ModuleTable references */
S_ListDesc4 ModT_04H03222[] =     /* Basic Color Print Management Meta SOP Class */
  {
   {NULL,ETypeM_UNKNOWN}
  };
S_ListDesc5 ModT_Desc_04H03222 = {E_ListDesc4,0,ModT_04H03222};

/* ModuleTable references */
S_ListDesc4 ModT_04H03223[] =     /* Referenced Greysale Print Management Meta SOP Class */
  {
   {NULL,ETypeM_UNKNOWN}
  };
S_ListDesc5 ModT_Desc_04H03223 = {E_ListDesc4,0,ModT_04H03223};

/* ModuleTable references */
S_ListDesc4 ModT_04H03224[] =     /* Referenced Color Print Management Meta SOP Class */
  {
   {NULL,ETypeM_UNKNOWN}
  };
S_ListDesc5 ModT_Desc_04H03224 = {E_ListDesc4,0,ModT_04H03224};

/* ModuleTable references */
S_ListDesc4 ModT_04H0332[] =     /* List of optional SOP Classes */
  {
   {NULL,ETypeM_UNKNOWN}
  };
S_ListDesc5 ModT_Desc_04H0332 = {E_ListDesc4,0,ModT_04H0332};

/* ModuleTable references */
S_ListDesc4 ModT_04H041[] =     /* DIMSE Service Group Basic Film Session SOP Class */
  {
   {&ModAttrGroupT_Desc_04H04121,ETypeM_NCreate_M_M}  /* Basic Film Session SOP Class N-CREATE Attributes */,
   {&ModAttrGroupT_Desc_04H04124,ETypeM_NAction_Print_1_U_U}  /* Basic Film Session SOP Class N-ACTION Attributes */
  };
S_ListDesc5 ModT_Desc_04H041 = {E_ListDesc4,2,ModT_04H041};

/* ModuleTable references */
S_ListDesc4 ModT_04H042[] =     /* DIMSE Service Group Basic Film Box SOP Class */
  {
   {&ModAttrGroupT_Desc_04H04221,ETypeM_NCreate_M_M}  /* Basic Film Box SOP Class N-CREATE Attributes */,
   {&ModAttrGroupT_Desc_04H04222,ETypeM_NSet_U_U}  /* Basic Film Box SOP Class N-SET Attributes */,
   {&ModAttrGroupT_Desc_04H04224,ETypeM_NAction_Print_1_M_M}  /* Basic Film Box SOP Class N-ACTION Attributes */
  };
S_ListDesc5 ModT_Desc_04H042 = {E_ListDesc4,3,ModT_04H042};

/* ModuleTable references */
S_ListDesc4 ModT_04H04312[] =     /* DIMSE Service Group Basic Grayscale Image Box SOP Class */
  {
   {&ModAttrGroupT_Desc_04H043121,ETypeM_NSet_M_M}  /* Basic Grayscale Image Box SOP Class N-SET Attributes */
  };
S_ListDesc5 ModT_Desc_04H04312 = {E_ListDesc4,1,ModT_04H04312};

/* ModuleTable references */
S_ListDesc4 ModT_04H04322[] =     /* DIMSE Service Group Basic Color Image Box SOP Class */
  {
   {&ModAttrGroupT_Desc_04H043221,ETypeM_NSet_M_M}  /* Basic Color Image Box SOP Class N-SET Attributes */
  };
S_ListDesc5 ModT_Desc_04H04322 = {E_ListDesc4,1,ModT_04H04322};

/* ModuleTable references */
S_ListDesc4 ModT_04H04332[] =     /* DIMSE Service Group Referenced Image Box SOP Class */
  {
   {&ModAttrGroupT_Desc_04H043321,ETypeM_NSet_M_M}  /* Referenced Image Box SOP Class N-SET Attributes */
  };
S_ListDesc5 ModT_Desc_04H04332 = {E_ListDesc4,1,ModT_04H04332};

/* ModuleTable references */
S_ListDesc4 ModT_04H0442[] =     /* DIMSE Service Group Basic Annotation Box SOP Class */
  {
   {&ModAttrGroupT_Desc_04H04421,ETypeM_NSet_U_M}  /* Basic Annotation Box SOP Class N-SET Attributes */
  };
S_ListDesc5 ModT_Desc_04H0442 = {E_ListDesc4,1,ModT_04H0442};

/* ModuleTable references */
S_ListDesc4 ModT_04H045[] =     /* DIMSE Service Group Print Job SOP Class */
  {
   {&ModAttrGroupT_Desc_04H045211,ETypeM_NEventReportPending_1_M_M}  /* Print Job SOP Class N-EVENT-REPORT Attributes (PENDING 1) */,
   {&ModAttrGroupT_Desc_04H045212,ETypeM_NEventReportPrinting_2_M_M}  /* Print Job SOP Class N-EVENT-REPORT Attributes (PRINTING 2) */,
   {&ModAttrGroupT_Desc_04H045213,ETypeM_NEventReportDone_3_M_M}  /* Print Job SOP Class N-EVENT-REPORT Attributes (DONE 3) */,
   {&ModAttrGroupT_Desc_04H045214,ETypeM_NEventReportFailure_4_M_M}  /* Print Job SOP Class N-EVENT-REPORT Attributes (FAILURE 4) */,
   {&ModAttrGroupT_Desc_04H04522,ETypeM_NGet_U_M}  /* Print Job SOP Class N-GET Attributes */
  };
S_ListDesc5 ModT_Desc_04H045 = {E_ListDesc4,5,ModT_04H045};

/* ModuleTable references */
S_ListDesc4 ModT_04H046[] =     /* DIMSE Service Group PRINTER SOP Class */
  {
   {&ModAttrGroupT_Desc_04H046212,ETypeM_NEventReportWarning_2_M_M}  /* PRINTER SOP Class N-EVENT-REPORT Attributes (WARNING 2) */,
   {&ModAttrGroupT_Desc_04H046213,ETypeM_NEventReportFailure_3_M_M}  /* PRINTER SOP Class N-EVENT-REPORT Attributes (FAILURE 3) */,
   {&ModAttrGroupT_Desc_04H04622,ETypeM_NGet_U_M}  /* PRINTER SOP Class N-GET Attributes */
  };
S_ListDesc5 ModT_Desc_04H046 = {E_ListDesc4,3,ModT_04H046};

/* ModuleTable references */
S_ListDesc4 ModT_04H0472[] =     /* DIMSE Service Group VOI LUT SOP Class */
  {
   {&ModAttrGroupT_Desc_04H04721,ETypeM_NCreate_M_M}  /* VOI LUT Box SOP Class N-CREATE Attributes */
  };
S_ListDesc5 ModT_Desc_04H0472 = {E_ListDesc4,1,ModT_04H0472};

/* ModuleTable references */
S_ListDesc4 ModT_04H048[] =     /* DIMSE Service Group Image Overlay Box SOP Class */
  {
   {&ModAttrGroupT_Desc_04H04821,ETypeM_NCreate_M_M}  /* Image Overlay Box SOP Class N-CREATE Attributes */,
   {&ModAttrGroupT_Desc_04H04822,ETypeM_NSet_U_M}  /* Image Overlay Box SOP Class N-SET Attributes */
  };
S_ListDesc5 ModT_Desc_04H048 = {E_ListDesc4,2,ModT_04H048};

/* ModuleTable references */
S_ListDesc4 ModT_0707533[] =     /* DIMSE Services and Procedures */
  {
   {NULL,ETypeM_UNKNOWN}
  };
S_ListDesc5 ModT_Desc_0707533 = {E_ListDesc4,0,ModT_0707533};

/* ModuleTable references */
S_ListDesc4 ModT_070931[] =     /* DIMSE C-STORE */
  {
   {&ModAttrGroupT_Desc_07090311,ETypeM_X}  /* C-STORE-RQ Message Fields */,
   {&ModAttrGroupT_Desc_07090312,ETypeM_X}  /* C-STORE-RSP Message Fields */
  };
S_ListDesc5 ModT_Desc_070931 = {E_ListDesc4,2,ModT_070931};

/* ModuleTable references */
S_ListDesc4 ModT_070932[] =     /* DIMSE C-FIND */
  {
   {&ModAttrGroupT_Desc_07090321,ETypeM_X}  /* C-FIND-RQ Message Fields */,
   {&ModAttrGroupT_Desc_07090322,ETypeM_X}  /* C-FIND-RSP Message Fields */,
   {&ModAttrGroupT_Desc_07090323,ETypeM_X}  /* C-CANCEL-FIND-RQ Message Fields */
  };
S_ListDesc5 ModT_Desc_070932 = {E_ListDesc4,3,ModT_070932};

/* ModuleTable references */
S_ListDesc4 ModT_070933[] =     /* DIMSE C-GET */
  {
   {&ModAttrGroupT_Desc_07090331,ETypeM_X}  /* C-GET-RQ Message Fields */,
   {&ModAttrGroupT_Desc_07090332,ETypeM_X}  /* C-GET-RSP Message Fields */,
   {&ModAttrGroupT_Desc_07090333,ETypeM_X}  /* C-CANCEL-GET-RQ Message Fields */
  };
S_ListDesc5 ModT_Desc_070933 = {E_ListDesc4,3,ModT_070933};

/* ModuleTable references */
S_ListDesc4 ModT_070934[] =     /* DIMSE C-MOVE */
  {
   {&ModAttrGroupT_Desc_07090341,ETypeM_X}  /* C-MOVE-RQ Message Fields */,
   {&ModAttrGroupT_Desc_07090342,ETypeM_X}  /* C-MOVE-RSP Message Fields */,
   {&ModAttrGroupT_Desc_07090343,ETypeM_X}  /* C-CANCEL-MOVE-RQ Message Fields */
  };
S_ListDesc5 ModT_Desc_070934 = {E_ListDesc4,3,ModT_070934};

/* ModuleTable references */
S_ListDesc4 ModT_070935[] =     /* DIMSE C-ECHO */
  {
   {&ModAttrGroupT_Desc_07090351,ETypeM_X}  /* C-ECHO-RQ Message Fields */,
   {&ModAttrGroupT_Desc_07090352,ETypeM_X}  /* C-ECHO-RSP Message Fields */
  };
S_ListDesc5 ModT_Desc_070935 = {E_ListDesc4,2,ModT_070935};

/* ModuleTable references */
S_ListDesc4 ModT_071031[] =     /* DIMSE N-EVENT-REPORT */
  {
   {&ModAttrGroupT_Desc_07100311,ETypeM_X}  /* N-EVENT-REPORT-RQ Message Fields */,
   {&ModAttrGroupT_Desc_07100312,ETypeM_X}  /* N-EVENT-REPORT-RSP Message Fields */
  };
S_ListDesc5 ModT_Desc_071031 = {E_ListDesc4,2,ModT_071031};

/* ModuleTable references */
S_ListDesc4 ModT_071032[] =     /* DIMSE N-GET */
  {
   {&ModAttrGroupT_Desc_07100321,ETypeM_X}  /* N-GET-RQ Message Fields */,
   {&ModAttrGroupT_Desc_07100322,ETypeM_X}  /* N-GET-RSP Message Fields */
  };
S_ListDesc5 ModT_Desc_071032 = {E_ListDesc4,2,ModT_071032};

/* ModuleTable references */
S_ListDesc4 ModT_071033[] =     /* DIMSE N-SET */
  {
   {&ModAttrGroupT_Desc_07100331,ETypeM_X}  /* N-SET-RQ Message Fields */,
   {&ModAttrGroupT_Desc_07100332,ETypeM_X}  /* N-SET-RSP Message Fields */
  };
S_ListDesc5 ModT_Desc_071033 = {E_ListDesc4,2,ModT_071033};

/* ModuleTable references */
S_ListDesc4 ModT_071034[] =     /* DIMSE N-ACTION */
  {
   {&ModAttrGroupT_Desc_07100341,ETypeM_X}  /* N-ACTION-RQ Message Fields */,
   {&ModAttrGroupT_Desc_07100342,ETypeM_X}  /* N-ACTION-RSP Message Fields */
  };
S_ListDesc5 ModT_Desc_071034 = {E_ListDesc4,2,ModT_071034};

/* ModuleTable references */
S_ListDesc4 ModT_071035[] =     /* DIMSE N-CREATE */
  {
   {&ModAttrGroupT_Desc_07100351,ETypeM_X}  /* N-CREATE-RQ Message Fields */,
   {&ModAttrGroupT_Desc_07100352,ETypeM_X}  /* N-CREATE-RSP Message Fields */
  };
S_ListDesc5 ModT_Desc_071035 = {E_ListDesc4,2,ModT_071035};

/* ModuleTable references */
S_ListDesc4 ModT_071036[] =     /* DIMSE N-DELETE */
  {
   {&ModAttrGroupT_Desc_07100361,ETypeM_X}  /* N-DELETE-RQ Message Fields */,
   {&ModAttrGroupT_Desc_07100362,ETypeM_X}  /* N-DELETE-RSP Message Fields */
  };
S_ListDesc5 ModT_Desc_071036 = {E_ListDesc4,2,ModT_071036};

/* ModuleTable references */
S_ListDesc4 ModT_07CStatus[] =     /* DIMSE Status */
  {
   {&ModAttrGroupT_Desc_07C0101,ETypeM_X}  /* Success */,
   {&ModAttrGroupT_Desc_07C0201,ETypeM_X}  /* Pending */,
   {&ModAttrGroupT_Desc_07C0301,ETypeM_X}  /* Cancel */,
   {&ModAttrGroupT_Desc_07C0401,ETypeM_X}  /* Warning */,
   {&ModAttrGroupT_Desc_07C0402,ETypeM_X}  /* Attribute List Error */,
   {&ModAttrGroupT_Desc_07C0501,ETypeM_X}  /* Error: Cannot Understand */,
   {&ModAttrGroupT_Desc_07C0502,ETypeM_X}  /* Error: Data Set does not Match SOP Class */,
   {&ModAttrGroupT_Desc_07C0503,ETypeM_X}  /* Failed */,
   {&ModAttrGroupT_Desc_07C0504,ETypeM_X}  /* Refused: Move Destination Unknown */,
   {&ModAttrGroupT_Desc_07C0505,ETypeM_X}  /* Refused: Out of Resources */,
   {&ModAttrGroupT_Desc_07C0506,ETypeM_X}  /* Refused: SOP Class Not Supported */,
   {&ModAttrGroupT_Desc_07C0507,ETypeM_X}  /* Class-Instance Conflict */,
   {&ModAttrGroupT_Desc_07C0508,ETypeM_X}  /* Duplicate SOP Instance */,
   {&ModAttrGroupT_Desc_07C0509,ETypeM_X}  /* Duplicate Invocation */,
   {&ModAttrGroupT_Desc_07C0510,ETypeM_X}  /* Invalid Argument Value */,
   {&ModAttrGroupT_Desc_07C0511,ETypeM_X}  /* Invalid Attribute Value */,
   {&ModAttrGroupT_Desc_07C0512,ETypeM_X}  /* Invalid Object Instance */,
   {&ModAttrGroupT_Desc_07C0513,ETypeM_X}  /* Missing Attribute */,
   {&ModAttrGroupT_Desc_07C0514,ETypeM_X}  /* Missing Attribute Value */,
   {&ModAttrGroupT_Desc_07C0515,ETypeM_X}  /* Mistyped Argument */,
   {&ModAttrGroupT_Desc_07C0516,ETypeM_X}  /* No Such Argument */,
   {&ModAttrGroupT_Desc_07C0517,ETypeM_X}  /* No Such Attribute */,
   {&ModAttrGroupT_Desc_07C0518,ETypeM_X}  /* No Such Event Type */,
   {&ModAttrGroupT_Desc_07C0519,ETypeM_X}  /* No Such Object Instance */,
   {&ModAttrGroupT_Desc_07C0520,ETypeM_X}  /* No Such SOP Class */,
   {&ModAttrGroupT_Desc_07C0521,ETypeM_X}  /* Processing Failure */,
   {&ModAttrGroupT_Desc_07C0522,ETypeM_X}  /* Resource Limitation */,
   {&ModAttrGroupT_Desc_07C0523,ETypeM_X}  /* Unrecognized Operation */
  };
S_ListDesc5 ModT_Desc_07CStatus = {E_ListDesc4,28,ModT_07CStatus};

/* ModuleTable references */
S_ListDesc4 ModT_1007Hdr[] =     /* DICOM File Meta Information Header */
  {
   {&ModAttrGroupT_Desc_100701,ETypeM_X}  /* DICOM File Meta Information */
  };
S_ListDesc5 ModT_Desc_1007Hdr = {E_ListDesc4,1,ModT_1007Hdr};

/* ModuleTable references */
S_ListDesc4 ModT_10B031[] =     /* Basic Directory IOD Modules */
  {
   {&ModAttrGroupT_Desc_10B0321,ETypeM_M}  /* File-set Identification Module */,
   {&ModAttrGroupT_Desc_10B0322,ETypeM_U}  /* Directory Information Module */
  };
S_ListDesc5 ModT_Desc_10B031 = {E_ListDesc4,2,ModT_10B031};

/* ModuleTable references */
S_ListDesc4 ModT_10B042[] =     /* Relationship between Directory Records */
  {
   {&ModAttrGroupT_Desc_10B0501,ETypeM_X}  /* Patient Keys */,
   {&ModAttrGroupT_Desc_10B0502,ETypeM_X}  /* Study Keys */,
   {&ModAttrGroupT_Desc_10B0503,ETypeM_X}  /* Series Keys */,
   {&ModAttrGroupT_Desc_10B0504,ETypeM_X}  /* Image Keys */,
   {&ModAttrGroupT_Desc_10B0505,ETypeM_X}  /* Overlay Keys */,
   {&ModAttrGroupT_Desc_10B0506,ETypeM_X}  /* Modality LUT Keys */,
   {&ModAttrGroupT_Desc_10B0507,ETypeM_X}  /* VOI LUT Keys */,
   {&ModAttrGroupT_Desc_10B0508,ETypeM_X}  /* Curve Keys */,
   {&ModAttrGroupT_Desc_10B0509,ETypeM_X}  /* Topic Keys */,
   {&ModAttrGroupT_Desc_10B0510,ETypeM_X}  /* Visit Keys */,
   {&ModAttrGroupT_Desc_10B0511,ETypeM_X}  /* Result Keys */,
   {&ModAttrGroupT_Desc_10B0512,ETypeM_X}  /* Interpretation Keys */,
   {&ModAttrGroupT_Desc_10B0513,ETypeM_X}  /* Study Component Keys */,
   {&ModAttrGroupT_Desc_10B0514,ETypeM_X}  /* Print Queue Keys */,
   {&ModAttrGroupT_Desc_10B0515,ETypeM_X}  /* Film Session Keys */,
   {&ModAttrGroupT_Desc_10B0516,ETypeM_X}  /* Basic Film Box Keys */,
   {&ModAttrGroupT_Desc_10B0517,ETypeM_X}  /* Basic Image Box Keys */,
   {&ModAttrGroupT_Desc_10B0519,ETypeM_X}  /* Multi-Referenced File Keys */
  };
S_ListDesc5 ModT_Desc_10B042 = {E_ListDesc4,18,ModT_10B042};

/* ModuleMetaTable references */
S_ListDesc4 ModMetaT_04H03222[] =     /* Basic Color Print Management Meta SOP Class */
  {
   {(S_ListDesc3 *)&ModT_Desc_04H041,ETypeM_M_M}  /* DIMSE Service Group Basic Film Session SOP Class */,
   {(S_ListDesc3 *)&ModT_Desc_04H042,ETypeM_M_M}  /* DIMSE Service Group Basic Film Box SOP Class */,
   {(S_ListDesc3 *)&ModT_Desc_04H04322,ETypeM_M_M}  /* DIMSE Service Group Basic Color Image Box SOP Class */,
   {(S_ListDesc3 *)&ModT_Desc_04H046,ETypeM_M_M}  /* DIMSE Service Group PRINTER SOP Class */
  };
S_ListDesc5 ModMetaT_Desc_04H03222 = {E_ListMeta,4,ModMetaT_04H03222};

/* ModuleMetaTable references */
S_ListDesc4 ModMetaT_04H03224[] =     /* Referenced Color Print Management Meta SOP Class */
  {
   {(S_ListDesc3 *)&ModT_Desc_04H041,ETypeM_M_M}  /* DIMSE Service Group Basic Film Session SOP Class */,
   {(S_ListDesc3 *)&ModT_Desc_04H042,ETypeM_M_M}  /* DIMSE Service Group Basic Film Box SOP Class */,
   {(S_ListDesc3 *)&ModT_Desc_04H04332,ETypeM_M_M}  /* DIMSE Service Group Referenced Image Box SOP Class */,
   {(S_ListDesc3 *)&ModT_Desc_04H046,ETypeM_M_M}  /* DIMSE Service Group PRINTER SOP Class */
  };
S_ListDesc5 ModMetaT_Desc_04H03224 = {E_ListMeta,4,ModMetaT_04H03224};

/* ModuleMetaTable references */
S_ListDesc4 ModMetaT_04H03221[] =     /* Basic Greyscale Print Management Meta SOP Class */
  {
   {(S_ListDesc3 *)&ModT_Desc_04H041,ETypeM_M_M}  /* DIMSE Service Group Basic Film Session SOP Class */,
   {(S_ListDesc3 *)&ModT_Desc_04H042,ETypeM_M_M}  /* DIMSE Service Group Basic Film Box SOP Class */,
   {(S_ListDesc3 *)&ModT_Desc_04H04312,ETypeM_M_M}  /* DIMSE Service Group Basic Grayscale Image Box SOP Class */,
   {(S_ListDesc3 *)&ModT_Desc_04H046,ETypeM_M_M}  /* DIMSE Service Group PRINTER SOP Class */
  };
S_ListDesc5 ModMetaT_Desc_04H03221 = {E_ListMeta,4,ModMetaT_04H03221};

/* ModuleMetaTable references */
S_ListDesc4 ModMetaT_04H03223[] =     /* Referenced Greysale Print Management Meta SOP Class */
  {
   {(S_ListDesc3 *)&ModT_Desc_04H041,ETypeM_M_M}  /* DIMSE Service Group Basic Film Session SOP Class */,
   {(S_ListDesc3 *)&ModT_Desc_04H042,ETypeM_M_M}  /* DIMSE Service Group Basic Film Box SOP Class */,
   {(S_ListDesc3 *)&ModT_Desc_04H04332,ETypeM_M_M}  /* DIMSE Service Group Referenced Image Box SOP Class */,
   {(S_ListDesc3 *)&ModT_Desc_04H046,ETypeM_M_M}  /* DIMSE Service Group PRINTER SOP Class */,
   {(S_ListDesc3 *)&ModT_Desc_04H0472,ETypeM_M_M}  /* DIMSE Service Group VOI LUT SOP Class */
  };
S_ListDesc5 ModMetaT_Desc_04H03223 = {E_ListMeta,5,ModMetaT_04H03223};

/* ModuleMetaTable references */
S_ListDesc4 ModMetaT_0707533[] =     /* DIMSE Services and Procedures */
  {
   {(S_ListDesc3 *)&ModT_Desc_070931,ETypeM_nnn}  /* DIMSE C-STORE */,
   {(S_ListDesc3 *)&ModT_Desc_070932,ETypeM_nCM}  /* DIMSE C-FIND */,
   {(S_ListDesc3 *)&ModT_Desc_070933,ETypeM_MCM}  /* DIMSE C-GET */,
   {(S_ListDesc3 *)&ModT_Desc_070934,ETypeM_MCM}  /* DIMSE C-MOVE */,
   {(S_ListDesc3 *)&ModT_Desc_070935,ETypeM_nnn}  /* DIMSE C-ECHO */,
   {(S_ListDesc3 *)&ModT_Desc_071031,ETypeM_nnn}  /* DIMSE N-EVENT-REPORT */,
   {(S_ListDesc3 *)&ModT_Desc_071032,ETypeM_nnn}  /* DIMSE N-GET */,
   {(S_ListDesc3 *)&ModT_Desc_071033,ETypeM_nnn}  /* DIMSE N-SET */,
   {(S_ListDesc3 *)&ModT_Desc_071034,ETypeM_nnn}  /* DIMSE N-ACTION */,
   {(S_ListDesc3 *)&ModT_Desc_071035,ETypeM_nnn}  /* DIMSE N-CREATE */,
   {(S_ListDesc3 *)&ModT_Desc_071036,ETypeM_nnn}  /* DIMSE N-DELETE */
  };
S_ListDesc5 ModMetaT_Desc_0707533 = {E_ListMeta,11,ModMetaT_0707533};

/* ModuleMetaTable references */
S_ListDesc4 ModMetaT_04H0332[] =     /* List of optional SOP Classes */
  {
   {(S_ListDesc3 *)&ModT_Desc_04H0442,ETypeM_M_M}  /* DIMSE Service Group Basic Annotation Box SOP Class */,
   {(S_ListDesc3 *)&ModT_Desc_04H045,ETypeM_M_M}  /* DIMSE Service Group Print Job SOP Class */,
   {(S_ListDesc3 *)&ModT_Desc_04H048,ETypeM_M_M}  /* DIMSE Service Group Image Overlay Box SOP Class */
  };
S_ListDesc5 ModMetaT_Desc_04H0332 = {E_ListMeta,3,ModMetaT_04H0332};

/* UID and Table references, used for makro insertions and Module Attribute references */
S_TableRef TableRefs[] =
  {
   {"1.2.840.10008.1.1","VerificationSOPClass",NULL},
   {"1.2.840.10008.1.2","LittleEndianImplicit",NULL},
   {"1.2.840.10008.1.2.1","LittleEndianExplicit",NULL},
   {"1.2.840.10008.1.2.2","BigEndianExplicit",NULL},
   {"1.2.840.10008.1.2.4.50","JPEGProcess1BaselineDefaultTransferSyntaxforLossyJPEG8BitImageCompression",NULL},
   {"1.2.840.10008.1.2.4.51","JPEGProcess2_4ExtendedDefaultTransferSyntaxforLossyJPEG12BitImageCompressionProcess4only",NULL},
   {"1.2.840.10008.1.2.4.52","JPEGProcess3_5Extended",NULL},
   {"1.2.840.10008.1.2.4.53","JPEGProcess6_8SpectralSelectionNonHierarchical",NULL},
   {"1.2.840.10008.1.2.4.54","JPEGProcess7_9SpectralSelectionNonHierarchical",NULL},
   {"1.2.840.10008.1.2.4.55","JPEGProcess10_12FullProgressionNonHierarchical",NULL},
   {"1.2.840.10008.1.2.4.56","JPEGProcess11_13FullProgressionNonHierarchical",NULL},
   {"1.2.840.10008.1.2.4.57","JPEGProcess14LosslessNonHierarchical",NULL},
   {"1.2.840.10008.1.2.4.58","JPEGProcess15LosslessNonHierarchical",NULL},
   {"1.2.840.10008.1.2.4.59","JPEGProcess16_18ExtendedHierarchical",NULL},
   {"1.2.840.10008.1.2.4.60","JPEGProcess17_19ExtendedHierarchical",NULL},
   {"1.2.840.10008.1.2.4.61","JPEGProcess20_22SpectralSelectionHierarchical",NULL},
   {"1.2.840.10008.1.2.4.62","JPEGProcess21_23SpectralSelectionHierarchical",NULL},
   {"1.2.840.10008.1.2.4.63","JPEGProcess24_26FullProgressionHierarchical",NULL},
   {"1.2.840.10008.1.2.4.64","JPEGProcess25_27FullProgressionHierarchical",NULL},
   {"1.2.840.10008.1.2.4.65","JPEGProcess28LosslessHierarchical",NULL},
   {"1.2.840.10008.1.2.4.66","JPEGProcess29LosslessHierarchical",NULL},
   {"1.2.840.10008.1.2.4.70","JPEGProcess14LosslessHierarchicalFirstOrderPredictionSelectionValue1DefaultTransferSyntaxforLosslessJPEGImageCompression",NULL},
   {"1.2.840.10008.1.3.10","BasicDirectoryStorageSOPClass",NULL},
   {"1.2.840.10008.1.9","BasicStudyContentNotificationSOPClass",NULL},
   {"1.2.840.10008.3.1.1.1","DICOMApplicationContextName",NULL},
   {"1.2.840.10008.3.1.2.1.1","DetachedPatientManagementSOPClass",&ModT_Desc_04E031},
   {"1.2.840.10008.3.1.2.1.4","DetachedPatientManagementMetaSOPClass",NULL},
   {"1.2.840.10008.3.1.2.2.1","DetachedVisitManagementSOPClass",&ModT_Desc_04E041},
   {"1.2.840.10008.3.1.2.3.1","DetachedStudyManagementSOPClass",&ModT_Desc_04F031},
   {"1.2.840.10008.3.1.2.3.2","StudyComponentManagementSOPClass",&ModT_Desc_04F041},
   {"1.2.840.10008.3.1.2.5.1","DetachedResultsManagementSOPClass",&ModT_Desc_04G031},
   {"1.2.840.10008.3.1.2.5.4","DetachedResultsManagementMetaSOPClass",NULL},
   {"1.2.840.10008.3.1.2.5.5","DetachedStudyManagementMetaSOPClass",NULL},
   {"1.2.840.10008.3.1.2.6.1","DetachedInterpretationManagementSOPClass",&ModT_Desc_04G041},
   {"1.2.840.10008.5.1.1.1","BasicFilmSessionSOPClass",&ModT_Desc_04H041},
   {"1.2.840.10008.5.1.1.14","PrintJobSOPClass",&ModT_Desc_04H045},
   {"1.2.840.10008.5.1.1.15","BasicAnnotationBoxSOPClass",&ModT_Desc_04H0442},
   {"1.2.840.10008.5.1.1.16","PrinterSOPClass",&ModT_Desc_04H046},
   {"1.2.840.10008.5.1.1.17","PrinterSOPInstance",&ModT_Desc_04H046},
   {"1.2.840.10008.5.1.1.18","BasicColorPrintManagementMetaSOPClass",&ModMetaT_Desc_04H03222},
   {"1.2.840.10008.5.1.1.18.1","ReferencedColorPrintManagementMetaSOPClass",&ModMetaT_Desc_04H03224},
   {"1.2.840.10008.5.1.1.2","BasicFilmBoxSOPClass",&ModT_Desc_04H041},
   {"1.2.840.10008.5.1.1.22","VOILUTBoxSOPClass",&ModT_Desc_04H0472},
   {"1.2.840.10008.5.1.1.24","ImageOverlayBoxSOPClass",&ModT_Desc_04H048},
   {"1.2.840.10008.5.1.1.4","BasicGrayscaleImageBoxSOPClass",&ModT_Desc_04H04312},
   {"1.2.840.10008.5.1.1.4.1","BasicColorImageBoxSOPClass",&ModT_Desc_04H04322},
   {"1.2.840.10008.5.1.1.4.2","ReferencedImageBoxSOPClass",&ModT_Desc_04H04332},
   {"1.2.840.10008.5.1.1.9","BasicGrayscalePrintManagementMetaSOPClass",&ModMetaT_Desc_04H03221},
   {"1.2.840.10008.5.1.1.9.1","ReferencedGrayscalePrintManagementMetaSOPClass",&ModMetaT_Desc_04H03223},
   {"1.2.840.10008.5.1.4.1.1.1","ComputedRadiographyImageStorage",&ModT_Desc_03A0203},
   {"1.2.840.10008.5.1.4.1.1.10","StandaloneModalityLUTStorage",&ModT_Desc_03A1203},
   {"1.2.840.10008.5.1.4.1.1.11","StandaloneVOILUTStorage",&ModT_Desc_03A1303},
   {"1.2.840.10008.5.1.4.1.1.2","CTImageStorage",&ModT_Desc_03A0303},
   {"1.2.840.10008.5.1.4.1.1.3","UltrasoundMultiframeImageStorage",&ModT_Desc_03A0703},
   {"1.2.840.10008.5.1.4.1.1.4","MRImageStorage",&ModT_Desc_03A0403},
   {"1.2.840.10008.5.1.4.1.1.5","NuclearMedicineImageStorage",&ModT_Desc_03A0503},
   {"1.2.840.10008.5.1.4.1.1.6","UltrasoundImageStorage",&ModT_Desc_03A0603},
   {"1.2.840.10008.5.1.4.1.1.7","SecondaryCaptureImageStorage",&ModT_Desc_03A0803},
   {"1.2.840.10008.5.1.4.1.1.8","StandaloneOverlayStorage",&ModT_Desc_03A0903},
   {"1.2.840.10008.5.1.4.1.1.9","StandaloneCurveStorage",&ModT_Desc_03A1003},
   {"1.2.840.10008.5.1.4.1.2.1.1","FINDPatientRootQueryRetrieveInformationModel",NULL},
   {"1.2.840.10008.5.1.4.1.2.1.2","MOVEPatientRootQueryRetrieveInformationModel",NULL},
   {"1.2.840.10008.5.1.4.1.2.1.3","GETPatientRootQueryRetrieveInformationModel",NULL},
   {"1.2.840.10008.5.1.4.1.2.2.1","FINDStudyRootQueryRetrieveInformationModel",NULL},
   {"1.2.840.10008.5.1.4.1.2.2.2","MOVEStudyRootQueryRetrieveInformationModel",NULL},
   {"1.2.840.10008.5.1.4.1.2.2.3","GETStudyRootQueryRetrieveInformationModel",NULL},
   {"1.2.840.10008.5.1.4.1.2.3.1","FINDPatientStudyOnlyQueryRetrieveInformationModel",NULL},
   {"1.2.840.10008.5.1.4.1.2.3.2","MOVEPatientStudyOnlyQueryRetrieveInformationModel",NULL},
   {"1.2.840.10008.5.1.4.1.2.3.3","GETPatientStudyOnlyQueryRetrieveInformationModel",NULL},
   {"AnnotationIODModules","",&ModT_Desc_03B1002},
   {"BasicDirectoryIODModules","",&ModT_Desc_10B031},
   {"BasicStudyDescriptorIODModuleTable","",&ModT_Desc_03A1103},
   {"DICOMFileMetaInformationHeader","",&ModT_Desc_1007Hdr},
   {"DIMSEServicesandProcedures","",&ModMetaT_Desc_0707533},
   {"DIMSEStatus","",&ModT_Desc_07CStatus},
   {"FilmBoxIODModules","",&ModT_Desc_03B0802},
   {"FilmSessionIODModules","",&ModT_Desc_03B0702},
   {"ImageBoxIODModules","",&ModT_Desc_03B0902},
   {"ImageOverlayIODModules","",&ModT_Desc_03B1402},
   {"InformationModels","",&ModT_Desc_04C06IM},
   {"InterpretationIODModules","",&ModT_Desc_03B0602},
   {"ListofoptionalSOPClasses","",&ModMetaT_Desc_04H0332},
   {"PatientIODModules","",&ModT_Desc_03B0102},
   {"PrintJobIODModules","",&ModT_Desc_03B1102},
   {"PrinterIODModules","",&ModT_Desc_03B1202},
   {"RelationshipbetweenDirectoryRecords","",&ModT_Desc_10B042},
   {"ResultIODModules","",&ModT_Desc_03B0502},
   {"StudyComponentIODModules","",&ModT_Desc_03B0402},
   {"StudyIODModules","",&ModT_Desc_03B0302},
   {"VOILUTIODModules","",&ModT_Desc_03B1302},
   {"VisitIODModules","",&ModT_Desc_03B0202}
  };

short DIM_OF_TableRefs = 91;

