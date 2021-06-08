/*
 *
 *  Copyright (C) 2015-2021, Open Connections GmbH
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation are maintained by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmfg
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Factory class for creating functional groups
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmfg/fgctacquisitiondetails.h"
#include "dcmtk/dcmfg/fgctacquisitiontype.h"
#include "dcmtk/dcmfg/fgctadditionalxraysource.h"
#include "dcmtk/dcmfg/fgctexposure.h"
#include "dcmtk/dcmfg/fgctgeometry.h"
#include "dcmtk/dcmfg/fgctimageframetype.h"
#include "dcmtk/dcmfg/fgctposition.h"
#include "dcmtk/dcmfg/fgctreconstruction.h"
#include "dcmtk/dcmfg/fgcttabledynamics.h"
#include "dcmtk/dcmfg/fgctxraydetails.h"
#include "dcmtk/dcmfg/fgderimg.h"
#include "dcmtk/dcmfg/fgfact.h"
#include "dcmtk/dcmfg/fgfracon.h"
#include "dcmtk/dcmfg/fgframeanatomy.h"
#include "dcmtk/dcmfg/fgframevoilut.h"
#include "dcmtk/dcmfg/fgimagedatatype.h"
#include "dcmtk/dcmfg/fgirradiationeventid.h"
#include "dcmtk/dcmfg/fgparametricmapframetype.h"
#include "dcmtk/dcmfg/fgpixeltransform.h"
#include "dcmtk/dcmfg/fgpixmsr.h"
#include "dcmtk/dcmfg/fgplanor.h"
#include "dcmtk/dcmfg/fgplanorvol.h"
#include "dcmtk/dcmfg/fgplanpo.h"
#include "dcmtk/dcmfg/fgplanposvol.h"
#include "dcmtk/dcmfg/fgrealworldvaluemapping.h"
#include "dcmtk/dcmfg/fgseg.h"
#include "dcmtk/dcmfg/fgtemporalposition.h"
#include "dcmtk/dcmfg/fgusimagedescription.h"
#include "dcmtk/dcmiod/iodutil.h"

FGFactory* FGFactory::m_Instance = NULL;

FGFactory::FGFactory()
{
    // noting to do (private constructor)
}

FGFactory& FGFactory::instance()
{
    if (m_Instance == NULL)
    {
        m_Instance = new FGFactory();
    }
    return *m_Instance;
}

FGBase* FGFactory::create(const DcmFGTypes::E_FGType fgtype)
{
    switch (fgtype)
    {
        case DcmFGTypes::EFG_PIXELMEASURES:
            return new FGPixelMeasures();
            break;
        case DcmFGTypes::EFG_FRAMEANATOMY:
            return new FGFrameAnatomy();
            break;
        case DcmFGTypes::EFG_FRAMECONTENT:
            return new FGFrameContent();
            break;
        case DcmFGTypes::EFG_CTACQUISITIONDETAILS:
            return new FGCTAcquisitionDetails();
            break;
        case DcmFGTypes::EFG_CTACQUISITIONTYPE:
            return new FGCTAcquisitionType();
            break;
        case DcmFGTypes::EFG_CTADDITIONALXRAYSOURCE:
            return new FGCTAdditionalXRaySource();
            break;
        case DcmFGTypes::EFG_CTEXPOSURE:
            return new FGCTExposure();
            break;
        case DcmFGTypes::EFG_CTGEOMETRY:
            return new FGCTGeometry();
            break;
        case DcmFGTypes::EFG_CTIMAGEFRAMETYPE:
            return new FGCTImageFrameType();
            break;
        case DcmFGTypes::EFG_CTPOSITION:
            return new FGCTPosition();
            break;
        case DcmFGTypes::EFG_CTRECONSTRUCTION:
            return new FGCTReconstruction();
            break;
        case DcmFGTypes::EFG_CTTABLEDYNAMICS:
            return new FGCTTableDynamics();
            break;
        case DcmFGTypes::EFG_CTXRAYDETAILS:
            return new FGCTXRayDetails();
            break;
        case DcmFGTypes::EFG_IRRADIATIONEVENTIDENT:
            return new FGIrradiationEventIdentification();
            break;
        case DcmFGTypes::EFG_FRAMEVOILUTMETA: // Frame VOI LUT and Frame VOI LUT with LUT
            return new FGFrameVOILUT();
            break;
        case DcmFGTypes::EFG_PARAMETRICMAPFRAMETYPE:
            return new FGParametricMapFrameType;
            break;
        case DcmFGTypes::EFG_PLANEPOSPATIENT:
            return new FGPlanePosPatient();
            break;
        case DcmFGTypes::EFG_PLANEPOSITIONVOLUME:
            return new FGPlanePositionVolume();
            break;
        case DcmFGTypes::EFG_PLANEORIENTPATIENT:
            return new FGPlaneOrientationPatient();
            break;
        case DcmFGTypes::EFG_PLANEORIENTVOLUME:
            return new FGPlaneOrientationVolume();
        case DcmFGTypes::EFG_DERIVATIONIMAGE:
            return new FGDerivationImage();
            break;
        case DcmFGTypes::EFG_PIXELVALUETRANSMETA: // Pixel Value Transformation Macro or Identity Pixel Value
                                                  // Transformation Macro:
            return new FGPixelValueTransformation();
            break;
        case DcmFGTypes::EFG_IMAGEDATATYPE:
            return new FGImageDataType();
            break;
        case DcmFGTypes::EFG_REALWORLDVALUEMAPPING:
            return new FGRealWorldValueMapping();
            break;
        case DcmFGTypes::EFG_SEGMENTATION:
            return new FGSegmentation();
            break;
        case DcmFGTypes::EFG_TEMPORALPOSITION:
            return new FGTemporalPosition();
            break;
        case DcmFGTypes::EFG_USIMAGEDESCRIPTION:
            return new FGUSImageDescription();
            break;
        case DcmFGTypes::EFG_CARDIACSYNC:
        case DcmFGTypes::EFG_CONTRASTBOLUSUSAGE:
        case DcmFGTypes::EFG_PIXELINTENSITYRELLUT:
        case DcmFGTypes::EFG_FRAMEPIXELSHIFT:
        case DcmFGTypes::EFG_PATIENTORIENTINFRAME:
        case DcmFGTypes::EFG_FRAMEDISPLAYSHUTTER:
        case DcmFGTypes::EFG_RESPIRATORYSYNC:
        case DcmFGTypes::EFG_RADIOPHARAMAUSAGE:
        case DcmFGTypes::EFG_PATIENTPHYSIOSTATE:
        {
            DCMFG_DEBUG("Encountered functional group that is not explicitly supported yet:"
                        << DcmFGTypes::FGType2OFString(fgtype));
            return NULL;
            break;
        }
        default:
        {
            DCMFG_DEBUG("Encountered unknown functional group");
            break;
        }
    }
    return NULL;
}

FGBase* FGFactory::create(const DcmTagKey& fgSequenceKey)
{
    if (!DcmIODUtil::isSequenceTag(fgSequenceKey))
    {
        DCMFG_ERROR("Cannot create functional group from non-sequence tag");
        return NULL;
    }
    FGBase* fg                  = NULL;
    DcmFGTypes::E_FGType fgType = DcmFGTypes::tagKey2FGType(fgSequenceKey);
    if (fgType != DcmFGTypes::EFG_UNKNOWN)
    {
        fg = create(fgType);
    }
    /* if the sequence is know but there is no specific implementation, create generic functional group */
    if (fg == NULL)
    {
        DCMFG_DEBUG("Encountered unknown functional group, started by tag key: " << fgSequenceKey);
        fg = new FGUnknown(fgSequenceKey);
    }
    return fg;
}
