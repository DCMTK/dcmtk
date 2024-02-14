/*
 *
 *  Copyright (C) 2016-2024, Open Connections GmbH
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
 *  Module: dcmpmap
 *
 *  Author: Michael Onken
 *
 *  Purpose: Class for managing the Parametric Map IOD
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmpmap/dpmparametricmapiod.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmdata/dcvrobow.h"
#include "dcmtk/dcmdata/dcvrfl.h"
#include "dcmtk/dcmdata/dcvrfd.h"
#include "dcmtk/dcmdata/dcuid.h"


// ----------------------------------------------------------------------------
// Class SetImagePixelModuleVisitor:
// Class for setting Image Pixel Module attributes based on pixel data type
// ----------------------------------------------------------------------------

struct DPMParametricMapIOD::SetImagePixelModuleVisitor
{
  SetImagePixelModuleVisitor(const Uint16 r,
                             const Uint16 c)
  : rows(r)
  , cols(c)
  {

  }

  template<typename T>
  OFCondition operator()( T& t )
  {
    if ( (rows == 0) || (cols == 0) )
    {
      DCMPMAP_ERROR("Rows/Cols must be non-zero but are : " << rows << "/" << cols);
      return DPM_InvalidDimensions;
    }

    t.setRows(rows);
    t.setColumns(cols);
    setSpecificValues(t);

    return EC_Normal;
  }

  template<typename T>
  void setSpecificValues(IODImagePixelModule<T>& p)
  {
    p.setBitsAllocated(16);
    p.setBitsStored(16);
    p.setHighBit(15);
    p.setSamplesPerPixel(1);
    p.setPhotometricInterpretation("MONOCHROME2");
    p.setPixelRepresentation(OFis_signed<T>::value ? 1 : 0);
  }

  void setSpecificValues(IODFloatingPointImagePixelModule& p)
  {
    // Avoid compiler warning about unused parameter "p"
    (void)p;
    // No specific values for 32 bit floating point data to be set
  }

  void setSpecificValues(IODDoubleFloatingPointImagePixelModule& p)
  {
    // Avoid compiler warning about unused parameter "p"
    (void)p;
    // No specific values for 64 bit floating point data to be set
  }
  // Members
  const Uint16 rows;
  const Uint16 cols;
};


struct DPMParametricMapIOD::ReadVisitor
{
  // Inner class that implements the specializations for different
  // pixel data types
  template<typename T>
  class DcmElementOf;

  ReadVisitor(DcmItem& srcItem,
              DPMParametricMapIOD& m)
  : item(srcItem)
  , map(m)
  {
    // Nothing to do
  }

  OFCondition operator()(OFmonostate)
  {
    return DPM_InvalidPixelData;
  }

  template<typename ImagePixel>
  OFCondition operator()(ImagePixel& pixel)
  {
    OFCondition result = map.readGeneric(item);
    if (result.good())
    {
      Uint16 rows, cols;
      Uint32 numFrames;
      size_t numBytesFrame = 0;
      rows = cols = 0;
      map.getRows(rows);
      map.getColumns(cols);
      numFrames = DcmIODUtil::limitMaxFrames(map.getFunctionalGroups().getNumberOfFrames(),
                                             "Functional groups implicate more than 2147483647 frames, only 2147483647 will be used");
      if (!rows || !cols || !numFrames)
      {
        DCMPMAP_ERROR("Rows (" << rows << "), Columns (" << cols << ") and Number of Frames (" << numFrames << ") must not be 0");
        return DPM_InvalidPixelInfo;
      }
      numBytesFrame = OFstatic_cast(size_t,rows) * cols * sizeof(typename ImagePixel::value_type);
      result = readSpecific(pixel, numFrames, numBytesFrame);
    }
    return result;
  }

  template<typename T>
  OFCondition readSpecific(IODImagePixelModule<T>& p,
                           const Uint32 numFrames,
                           const size_t numBytesFrame)
  {
    // Avoid compiler warning about unused parameter
    (void)p;
    unsigned long numTotalWords = 0;
    const Uint16* pixData = NULL;
    if (item.findAndGetUint16Array(DCM_PixelData, pixData, &numTotalWords).good())
    {
      if (numTotalWords == numBytesFrame * numFrames / 2 /* we compare to num words not num bytes */)
      {
        for (Uint32 n = 0; n < numFrames; n++)
        {
          DcmIODTypes::Frame* f = new DcmIODTypes::Frame;
          if (f)
          {
            f->length = numBytesFrame;
            f->pixData = new Uint8[f->length];
            memcpy(f->pixData, pixData + n*numBytesFrame/2, numBytesFrame);
            map.m_Frames.push_back(f);
          }
          else
          {
            return EC_MemoryExhausted;
          }
        }
      }
      else
      {
        DCMPMAP_ERROR("Invalid number of pixels: Expected " << numBytesFrame * numFrames / 2 << " pixels but Pixel Data has " << numTotalWords << " pixels");
        return DPM_InvalidPixelInfo;
      }
    }
    else
    {
      DCMPMAP_ERROR("No Pixel Data element found");
      return DPM_NoPixelData;
    }
    return EC_Normal;
  }

  OFCondition readSpecific(IODFloatingPointImagePixelModule& p,
                           const Uint32 numFrames,
                           const size_t numBytesFrame)
  {
    // Avoid compiler warning on unused parameter
    (void)p;
    unsigned long numTotalFloats = 0;
    const Float32* pixData = NULL;
    if (item.findAndGetFloat32Array(DCM_FloatPixelData, pixData, &numTotalFloats).good())
    {
      if (numTotalFloats == numBytesFrame * numFrames / 4 /* we compare to 32 bit floats not bytes */)
      {
        for (Uint32 n=0; n < numFrames; n++)
        {
          DcmIODTypes::Frame* f = new DcmIODTypes::Frame;
          if (f)
          {
            f->length = numBytesFrame;
            f->pixData = new Uint8[f->length];
            memcpy(f->pixData, pixData + n*numBytesFrame/4, numBytesFrame);
            map.m_Frames.push_back(f);
          }
          else
          {
            return EC_MemoryExhausted;
          }
        }
      }
      else
      {
        DCMPMAP_ERROR("Invalid number of pixels: Expected " << numBytesFrame * numFrames / 4 << " pixels but Float Pixel Data has " << numTotalFloats << " pixels");
        return DPM_InvalidPixelInfo;
      }
    }
    else
    {
      DCMPMAP_ERROR("No Float Pixel Data element found");
      return DPM_NoPixelData;
    }
    return EC_Normal;
  }

  OFCondition readSpecific(IODDoubleFloatingPointImagePixelModule& p,
                           const Uint32 numFrames,
                           const size_t numBytesFrame)
  {
    // Avoid compiler warning on unused parameter
    (void)p;
    unsigned long numTotalFloats = 0;
    const Float64* pixData = NULL;
    if (item.findAndGetFloat64Array(DCM_DoubleFloatPixelData, pixData, &numTotalFloats).good())
    {
      if (numTotalFloats == numBytesFrame * numFrames / 8 /* we compare to 64 bit floats not bytes */)
      {
        for (Uint16 n=0; n < numFrames; n++)
        {
          DcmIODTypes::Frame* f = new DcmIODTypes::Frame;
          if (f)
          {
            f->length = numBytesFrame;
            f->pixData = new Uint8[f->length];
            memcpy(f->pixData, pixData + n*numBytesFrame/8, numBytesFrame);
            map.m_Frames.push_back(f);
          }
          else
          {
            return EC_MemoryExhausted;
          }
        }
      }
      else
      {
        DCMPMAP_ERROR("Invalid number of pixels: Expected " << numBytesFrame * numFrames / 8 << " pixels but Double Float Pixel Data has " << numTotalFloats << " pixels");
        return DPM_InvalidPixelInfo;
      }
    }
    else
    {
      DCMPMAP_ERROR("No Double Float Pixel Data element found");
      return DPM_NoPixelData;
    }
    return EC_Normal;
  }

  // Members, i.e. parameters to operator()
  DcmItem& item;
  DPMParametricMapIOD& map;
};



// ----------------------------------------------------------------------------
// Class WriteVisitor:
// Class for adding DICOM pixel data bulk element based on type pixel data type
// ----------------------------------------------------------------------------

// Generic implementation, template specialization is done in inner
// DcmElementOf class
struct DPMParametricMapIOD::WriteVisitor
{
  // Inner class that implements the specializations for different
  // pixel data types
  template<typename T>
  class DcmElementOf;

  // Constructor, sets parameters the visitor works on in operator()
  WriteVisitor(DcmItem& i,
               DPMParametricMapIOD& m)
  : item(i)
  , map(m)
  {
    // Nothing else to do
  }

  //
  OFCondition operator()(OFmonostate)
  {
    DCMPMAP_ERROR("Could not write pixel data: Invalid pixel data type, only 16 bit integer as well as 32 and 64 bit float data are handled");
    return DPM_NoPixelData;
  }

  template<typename ImagePixel>
  OFCondition operator()(ImagePixel& pixel)
  {
    // Avoid compiler warning about unused parameter "pixel"
    (void)pixel;
    // Input data is checked before
    Uint16 rows = 0;
    Uint16 cols = 0;
    map.getRows(rows);
    map.getColumns(cols);
    const size_t numFrames = map.m_Frames.size();
    const size_t numBytesFrame = map.m_Frames[0]->length;
    const size_t numPixelsFrame = rows * cols;
    // Creates the correct pixel data element, based on the image pixel module used.
    // I.e. For integer data, the "Pixel Data" element is used, i.e. the DcmElement type
    // instantiated by DcmElementOf is of DCMTK type DcmOtherByteOtherWord while for
    // IODFloatingPointImagePixelModule which has floating point pixel data, DcmFloatingPointSingle
    // is used.
    DcmElementOf<ImagePixel> element;
    // copy all frames into map's frame structure
    if (OFTypename ImagePixel::value_type* ptr = element.getData(numBytesFrame * numFrames))
    {
      for (size_t f = 0; f < numFrames; ++f)
      {
        memcpy(ptr, map.m_Frames[f]->pixData, numBytesFrame);
        ptr += numPixelsFrame;
      }
      OFCondition result = element.put(item);
      // Write generic (i.e. non pixel data-specific attributes)
      return result.good() ? map.writeGeneric(item) : result;
    }
    return EC_MemoryExhausted;
  }

  // Members, i.e. parameters to operator()
  DcmItem& item;
  DPMParametricMapIOD& map;
};


// ----------------------------------------------------------------------------
// Class WriteVisitor:DcmElementOf<IODImagePixelModule<PixelType> >:
// Offers creation and insertion for Pixel Data element as used for
// int-based pixel data. So far, Uint8, Sint8, Uint16 and Sint16 pixel
// data is supported (relates to OB/OW data type for DICOM Pixel data)
// ----------------------------------------------------------------------------

template<typename PixelType>
class DPMParametricMapIOD::WriteVisitor::DcmElementOf<IODImagePixelModule<PixelType> >
{
public:

  DcmElementOf()
  : m_pElement(new DcmOtherByteOtherWord(DCM_PixelData))
  {
    // Nothing else to do
  }

  PixelType* getData(const size_t count);

  OFCondition put(DcmItem& itemPara)
  {
    OFCondition result = itemPara.insert(m_pElement.get(), OFTrue);
    if (result.good())
      m_pElement.release();
    return result;
  }

private:

  OFunique_ptr<DcmOtherByteOtherWord> m_pElement;
};


template<>
Uint16* DPMParametricMapIOD::WriteVisitor::DcmElementOf<IODImagePixelModule<Uint16> >::getData(const size_t size /* num total pixels */)
{
  if (m_pElement)
  if (m_pElement->setVR(EVR_OW).good())
  {
    Uint16* result;
    if (m_pElement->createUint16Array(OFstatic_cast(Uint32, size/2), result).good())
      return result;
  }
  return OFnullptr;
}


template<>
Sint16* DPMParametricMapIOD::WriteVisitor::DcmElementOf<IODImagePixelModule<Sint16> >::getData(const size_t size)
{
  if (m_pElement)
  if (m_pElement->setVR(EVR_OW).good())
  {
    Uint16* result;
    if (m_pElement->createUint16Array(OFstatic_cast(Uint32, size/2), result).good())
      return OFreinterpret_cast(Sint16*, result);
  }
  return OFnullptr;
}


// ----------------------------------------------------------------------------
// Class WriteVisitor:DcmElementOf<IODImagePixelModule<IODFloatingPointImagePixelModule> >:
// Offers creation and insertion for Pixel Data element as used for
// 32 bit float-based pixel data as used in Parametric Map IOD
// ----------------------------------------------------------------------------

template<>
class DPMParametricMapIOD::WriteVisitor::DcmElementOf<IODFloatingPointImagePixelModule>
{
public:
  DcmElementOf()
  : m_pElement(new DcmFloatingPointSingleAlloc(DCM_FloatPixelData))
  {

  }

  Float32* getData(const size_t size)
  {
    if (m_pElement)
    {
      Float32* result;
      if (m_pElement->createFloat32Array(size, result).good())
        return result;
    }
    return OFnullptr;
  }

  OFCondition put(DcmItem& itemPara)
  {
    OFCondition result = itemPara.insert(m_pElement.get(), OFTrue);
    if (result.good())
      m_pElement.release();
    return result;
  }

private:

  struct DcmFloatingPointSingleAlloc : DcmFloatingPointSingle
  {
    DcmFloatingPointSingleAlloc(const DcmTagKey& tag)
    : DcmFloatingPointSingle(tag)
    {

    }

    OFCondition createFloat32Array(const size_t size,
                                   Float32*& result)
    {
       OFCondition status = createEmptyValue(OFstatic_cast(Uint32, size));
       result = OFreinterpret_cast(Float32*, getValue());
       return status;
    }
  };

  OFunique_ptr<DcmFloatingPointSingleAlloc> m_pElement;
};


// ----------------------------------------------------------------------------
// Class WriteVisitor:DcmElementOf<IODImagePixelModule<IODFloatingPointImagePixelModule> >:
// Offers creation and insertion for Pixel Data element as used for
// 64 bit float-based pixel data as used in Parametric Map IOD.
// ----------------------------------------------------------------------------

template<>
class DPMParametricMapIOD::WriteVisitor::DcmElementOf<IODDoubleFloatingPointImagePixelModule>
{
public:
  DcmElementOf()
  : m_pElement(new DcmFloatingPointDoubleAlloc(DCM_DoubleFloatPixelData))
  {

  }

  Float64* getData(const size_t size)
  {
    if (m_pElement)
    {
      Float64* result;
      if (m_pElement->createFloat64Array(size, result).good())
        return result;
    }
    return OFnullptr;
  }

  OFCondition put(DcmItem& itemPara)
  {
    OFCondition result = itemPara.insert(m_pElement.get(), OFTrue);
    if (result.good())
      m_pElement.release();
    return result;
  }

private:

  struct DcmFloatingPointDoubleAlloc : DcmFloatingPointDouble
  {
    DcmFloatingPointDoubleAlloc(const DcmTagKey& tag)
    : DcmFloatingPointDouble(tag)
    {

    }

    OFCondition createFloat64Array(const size_t size, Float64*& result)
    {
       OFCondition status = createEmptyValue(OFstatic_cast(Uint32, size));
       result = OFreinterpret_cast(Float64*, getValue());
       return status;
    }
  };

  OFunique_ptr<DcmFloatingPointDoubleAlloc> m_pElement;
};


// Helper "class" that returns Frames offering API to the pixel's frame bulk
// data by offering the dedicated data type, e.g. Float32 instead of the
// internally stored generic Uint8 array.
//
struct DPMParametricMapIOD::GetFramesVisitor
{
  GetFramesVisitor(DPMParametricMapIOD& m)
  : map(m)
  {

  }

  OFCondition operator()(OFmonostate)
  {
    return DPM_InvalidPixelData;
  }

  template<typename ImagePixel>
  Frames<OFTypename ImagePixel::value_type> operator()(ImagePixel&)
  {
    return Frames<OFTypename ImagePixel::value_type>(map);
  }

  DPMParametricMapIOD& map;
};


/** Helper "class" that returns Frames offering API to the pixel's frame bulk
 *  data by offering the dedicated data type, e.g. Float32 instead of the
 *  internally stored generic Uint8 array.
 */
struct DPMParametricMapIOD::ReadFramesVisitor
{
  ReadFramesVisitor(DcmItem& i,
                    DPMParametricMapIOD& m)
  : map(m), item(i)
  {
    // Nothing else to do
  }

  OFCondition operator()(OFmonostate)
  {
    return DPM_InvalidPixelData;
  }

  template<typename ImagePixel>
  OFCondition operator()(ImagePixel&)
  {
    return Frames<OFTypename ImagePixel::value_type>(map);
  }

  // Members, i.e. parameters to operator()
  DPMParametricMapIOD& map;
  DcmItem& item;
};



template<typename PixelType>
DPMParametricMapIOD::Frames<PixelType>::Frames(DPMParametricMapIOD& map)
: m_Map(map)
{

}


template<typename PixelType>
OFCondition DPMParametricMapIOD::Frames<PixelType>::addFrame(PixelType* data,
                                                             const size_t numPixels,
                                                             const OFVector<FGBase*>& perFrameInformation)
{
  OFCondition result;
  if (data && numPixels)
  {
    if (!perFrameInformation.empty())
    {
      OFunique_ptr<DcmIODTypes::Frame> f(new DcmIODTypes::Frame);
      if (f)
      {
        f->length = numPixels * sizeof(PixelType);
        f->pixData = new Uint8[f->length];
        memcpy(f->pixData, data, f->length);
        m_Map.m_Frames.push_back(f.release());
        OFVector<FGBase*>::const_iterator fg = perFrameInformation.begin();
        while ( result.good() && (fg != perFrameInformation.end()) )
        {
          result = m_Map.m_FGInterface.addPerFrame(OFstatic_cast(Uint32, m_Map.m_Frames.size()), **fg);
          if (result.bad())
          {
            DCMPMAP_ERROR("Could not add functional group: " << DcmFGTypes::FGType2OFString((*fg)->getType()));
            break;
          }
          ++fg;
        }
      }
    }
  }
  // Clean up if necessary
  if (result.bad())
    m_Map.m_FGInterface.deleteFrame(OFstatic_cast(Uint32, m_Map.m_Frames.size()));
  return result;
}


template<typename PixelType>
PixelType* DPMParametricMapIOD::Frames<PixelType>::getFrame(const size_t frameNumber)
{
  if (frameNumber < m_Map.m_Frames.size())
  {
    return (PixelType*)(m_Map.m_Frames[frameNumber]->pixData);
  }
  return NULL;
}


// Protected constructor
DPMParametricMapIOD::DPMParametricMapIOD()
: DPMParametricMapBase()
, m_ContentIdentification()
, m_Frames()
{
}


// Protected constructor
template<typename ImagePixel>
DPMParametricMapIOD::DPMParametricMapIOD(OFin_place_type_t(ImagePixel))
: DPMParametricMapBase(OFin_place<ImagePixel>)
, m_ContentIdentification()
, m_Frames()
{
}


// Static method used to create a new Parametric Map IOD object
template<typename ImagePixel>
OFvariant<OFCondition,DPMParametricMapIOD> DPMParametricMapIOD::create(const OFString& modality,
                                                                       const OFString& seriesNumber,
                                                                       const OFString& instanceNumber,
                                                                       const Uint16 rows,
                                                                       const Uint16 columns,
                                                                       const IODEnhGeneralEquipmentModule::EquipmentInfo& equipmentInfo,
                                                                       const ContentIdentificationMacro& contentIdentification,
                                                                       const OFString& imageFlavor,
                                                                       const OFString& derivedPixelContrast,
                                                                       const DPMTypes::ContentQualification& contentQualification)
{
  DPMParametricMapIOD map(OFin_place<ImagePixel>);

  OFCondition status = SetImagePixelModuleVisitor(rows, columns)(*OFget<ImagePixel>(&map.getImagePixel()));
  if (status.bad())
    return status;

  status = map.init
  (
    modality,
    seriesNumber,
    instanceNumber,
    equipmentInfo,
    contentIdentification,
    imageFlavor,
    derivedPixelContrast,
    contentQualification
  );

  if(status.bad())
    return status;

  return map;
}


OFCondition DPMParametricMapIOD::init(const OFString& modality,
                                      const OFString& seriesNumber,
                                      const OFString& instanceNumber,
                                      const IODEnhGeneralEquipmentModule::EquipmentInfo& equipmentInfo,
                                      const ContentIdentificationMacro& contentIdentification,
                                      const OFString& imageFlavor,
                                      const OFString& derivedPixelContrast,
                                      const DPMTypes::ContentQualification& contentQualification)
{
  if (!equipmentInfo.isDataComplete())
  {
    DCMPMAP_ERROR("Equipment info incomplete");
    return DPM_InvalidAttributeValue;
  }

  OFCondition status = OFconst_cast(ContentIdentificationMacro*, &contentIdentification)->check();
  if (status.bad())
  {
    DCMPMAP_ERROR("Content identification is invalid: " << status.text());
    return DPM_InvalidAttributeValue;
  }

  status = OFconst_cast(ContentIdentificationMacro*, &contentIdentification)->check();
  if (imageFlavor.empty() || derivedPixelContrast.empty())
  {
    DCMPMAP_ERROR
    (
      "Image Type value 3 (Image Flavor) and 4 (Derived Pixel Contrast) must be set but are "
      << imageFlavor << "/" << derivedPixelContrast
    );
    return DPM_InvalidAttributeValue;
  }

  if (!DPMTypes::cqValid(contentQualification))
  {
    DCMPMAP_ERROR("Content Qualification not set");
    return DPM_InvalidAttributeValue;
  }

  // All data is valid, copy to object
  status = getIODEnhGeneralEquipmentModule().set(equipmentInfo);
  if (status.bad())
  {
    DCMPMAP_ERROR("Error setting equipment info: " << status.text());
    return DPM_InvalidAttributeValue;
  }

  m_ContentIdentification = contentIdentification;
  OFStringStream ss;
  ss << "DERIVED\\PRIMARY\\" << imageFlavor << "\\" << derivedPixelContrast;
  OFSTRINGSTREAM_GETOFSTRING(ss, imgType);

  status = getIODGeneralImageModule().setImageType(imgType);
  if (status.bad())
  {
    DCMPMAP_ERROR("Error setting Image Type: " << status.text());
    return DPM_InvalidAttributeValue;
  }

  status = m_DPMParametricMapSeriesModule.setModality(modality);
  if (status.bad())
  {

    DCMPMAP_ERROR("Could not set modality value \"" << modality << "\": " << status.text());
    return DPM_InvalidAttributeValue;
  }

  status = m_DPMParametricMapSeriesModule.setSeriesNumber(seriesNumber);
  if (status.bad())
  {
    DCMPMAP_ERROR("Could not set Series Number: " << status.text());
    return DPM_InvalidAttributeValue;
  }

  status = m_IODMultiFrameFGModule.setInstanceNumber(instanceNumber);
  if (status.bad())
    return status;

  m_DPMParametricMapImageModule.setContentQualification(DPMTypes::cq2Str(contentQualification));
  m_DPMParametricMapImageModule.setRecognizableVisualFeatures("YES");
  m_DPMParametricMapImageModule.setBurnedInAnnotation("NO");
  m_DPMParametricMapImageModule.setLossyImageCompression("00");
  m_DPMParametricMapImageModule.setPresentationLUTShape("IDENTITY");
  getSOPCommon().setSOPClassUID(UID_ParametricMapStorage);
  DcmIODUtil::setContentDateAndTimeNow(m_IODMultiFrameFGModule);

  return EC_Normal;
}


OFCondition DPMParametricMapIOD::read(DcmItem& dataset)
{
  return OFvisit<OFCondition>(ReadVisitor(dataset,*this),getImagePixel());

}


OFCondition DPMParametricMapIOD::write(DcmItem& dataset)
{
  if (!check())
    return DPM_InvalidAttributeValue;
  return OFvisit<OFCondition>(WriteVisitor(dataset,*this),getImagePixel());
}


OFvariant<OFCondition,DPMParametricMapIOD*> DPMParametricMapIOD::loadDataset(DcmDataset& dataset)
{
  OFString sopClass;
  DPMParametricMapIOD *map = NULL;
  if (dataset.findAndGetOFStringArray(DCM_SOPClassUID, sopClass).good())
  {
    if (sopClass == UID_ParametricMapStorage)
    {
      DcmElement *elem = NULL;
      if (dataset.findAndGetElement(DCM_PixelData, elem).good())
      {
        Uint16 pr = 0;
        if (dataset.findAndGetUint16(DCM_PixelRepresentation, pr).good())
        {
          if (pr == 0)
          {
            map = new DPMParametricMapIOD(OFin_place<IODImagePixelModule<Uint16> >);
          }
          else
          {
            map = new DPMParametricMapIOD(OFin_place<IODImagePixelModule<Sint16> >);
          }
        }
        else
        {
          DCMPMAP_WARN("Pixel Data element found but no Pixel Representation set, assuming 16 bit unsigned integer data");
          map = new DPMParametricMapIOD(OFin_place<IODImagePixelModule<Uint16> >);
        }
      }
      else if (dataset.findAndGetElement(DCM_FloatPixelData, elem).good())
      {
        map = new DPMParametricMapIOD(OFin_place<IODFloatingPointImagePixelModule>);
      }
      else if (dataset.findAndGetElement(DCM_DoubleFloatPixelData, elem).good())
      {
        map = new DPMParametricMapIOD(OFin_place<IODDoubleFloatingPointImagePixelModule>);
      }
      else
      {
        return DPM_NoPixelData;
      }
      if (map == NULL)
      {
        return EC_MemoryExhausted;
      }
    }
    else
    {
      DCMPMAP_ERROR("Invalid SOP Class: " << sopClass << ", only Parametric Map Storage (1.2.840.10008.5.1.4.1.1.30) supported");
      return DPM_InvalidSOPClass;
    }
  }
  OFCondition result = OFvisit<OFCondition>(ReadVisitor(dataset, *map), map->getImagePixel());
  if (result.good())
    return map;
  else
    return result;
}


OFvariant<OFCondition,DPMParametricMapIOD*> DPMParametricMapIOD::loadFile(const OFString& filename)
{
  DcmFileFormat dcmff;
  OFCondition cond = dcmff.loadFile(filename.c_str());
  OFvariant<OFCondition,DPMParametricMapIOD*> result;
  if (cond.good())
  {
     result = loadDataset( *(dcmff.getDataset()) );
     return result;
  }
  else
  {
    return cond;
  }
}


DPMParametricMapIOD::~DPMParametricMapIOD()
{
  DcmIODUtil::freeContainer(m_Frames);
  // nothing else to do for now
}


OFCondition DPMParametricMapIOD::addForAllFrames(const FGBase& group)
{
  return m_FGInterface.addShared(group);
}


OFBool DPMParametricMapIOD::check()
{
  OFString val;
  m_DPMParametricMapImageModule.getRecognizableVisualFeatures(val);
  if (DPMTypes::rvfValid(DPMTypes::str2Rvf(val)))
  {
    m_DPMParametricMapImageModule.getContentQualification(val);
    if (DPMTypes::cqValid(DPMTypes::str2Cq(val)))
    {
      if (!m_Frames.empty())
      {
        return OFTrue;
      }
      else
      {
        DCMPMAP_ERROR("No frame data defined in Parametric Map");
      }
    }
    else
    {
      DCMPMAP_ERROR("Invalid value for Recognizable Visual Features: " << val);
    }
  }
  else
  {
    DCMPMAP_ERROR("Invalid value for Content Qualification" << val);
  }
  return OFFalse;
}


OFCondition DPMParametricMapIOD::getColumns(Uint16& cols)
{
  return getImagePixel().getColumns(cols);
}


OFCondition DPMParametricMapIOD::getRows(Uint16& rows)
{
  return getImagePixel().getRows(rows);
}


size_t DPMParametricMapIOD::getNumberOfFrames() const
{
  return OFconst_cast(DPMParametricMapIOD*,this)->m_FGInterface.getNumberOfFrames();
}


OFCondition DPMParametricMapIOD::setRecognizableVisualFeatures(const DPMTypes::RecognizableVisibleFeatures yesOrNo)
{
  return DPMTypes::rvfValid(yesOrNo)
  ?
    m_DPMParametricMapImageModule.setRecognizableVisualFeatures(DPMTypes::rvf2Str(yesOrNo))
  :
    DPM_InvalidAttributeValue
  ;
}


OFCondition DPMParametricMapIOD::readGeneric(DcmItem& dataset)
{
  OFCondition result = DPMParametricMapBase::read(dataset);
  if (result.good())
    result = m_ContentIdentification.read(dataset);
  return result;
}


OFCondition DPMParametricMapIOD::writeGeneric(DcmItem& dataset)
{
  getFrameOfReference().ensureFrameOfReferenceUID();
  m_IODMultiFrameFGModule.setNumberOfFrames(DcmIODUtil::limitMaxFrames(m_Frames.size(), "Maximum number of frames exceeded, will write 2147483647"));
  OFCondition result = m_ContentIdentification.write(dataset);
  if (result.good())
  {
    result = DPMParametricMapBase::write(dataset);
  }
  return result;
}


DPMTypes::ContentQualification DPMParametricMapIOD::getContentQualification()
{
  OFString val;
  m_DPMParametricMapImageModule.getContentQualification(val);
  return DPMTypes::str2Cq(val);
}


DPMTypes::RecognizableVisibleFeatures DPMParametricMapIOD::getRecognizableVisualFeatures()
{
  OFString val;
  DPMParametricMapIOD::m_DPMParametricMapImageModule.getRecognizableVisualFeatures(val);
  return DPMTypes::str2Rvf(val);
}


DPMParametricMapIOD::FramesType DPMParametricMapIOD::getFrames()
{
  return OFvisit<FramesType>(GetFramesVisitor(*this),getImagePixel());
}


OFCondition DPMParametricMapIOD::decompress(DcmDataset& dset)
{
  DcmXfer xfer = dset.getOriginalXfer();
  OFCondition result;
  // If the original transfer syntax could have been lossy, print warning
  if (dset.hasRepresentation(EXS_LittleEndianExplicit, NULL))
  {
    if ( xfer.isEncapsulated() && (xfer != EXS_RLELossless) && (xfer != EXS_DeflatedLittleEndianExplicit) )
    {
      DCMPMAP_WARN("Dataset has been compressed using a (possibly) lossy compression scheme (ignored)");
    }
  }
  // If the original transfer is encapsulated and we do not already have an uncompressed version, decompress or reject the file
  else if (xfer.isEncapsulated())
  {
    // RLE compression is fine (truly lossless). Deflated is handled internally by DCMTK.
    if (xfer == EXS_RLELossless)
    {
      DCMPMAP_DEBUG("DICOM file is RLE-compressed, converting to uncompressed transfer syntax first");
      result = DcmIODUtil::decompress(dset);
    }
    else // We do not accept any transfer syntax that could be lossy compressed
    {
      DCMPMAP_ERROR("Transfer syntax " << xfer.getXferName() << " uses lossy compression, not supported for Parametric Map objects!");
      result = IOD_EC_CannotDecompress;
    }
  }
  return result;
}

// Explicit instantiations using all permitted Parametric Map data types
template class DPMParametricMapIOD::Frames<Uint16>;
template class DPMParametricMapIOD::Frames<Sint16>;
template class DPMParametricMapIOD::Frames<Float32>;
template class DPMParametricMapIOD::Frames<Float64>;

#ifdef __SUNPRO_CC
// the SunPro compiler would complain that the DCMTK_DCMPMAP_EXPORT attribute cannot be applied
// to an explicit template instantiation.

// Helper macro to not to write the same thing multiple times
#define INSTANTIATE_CREATE(T) template OFvariant<OFCondition,DPMParametricMapIOD>\
DPMParametricMapIOD::create<T >(const OFString& modality,\
                                const OFString& seriesNumber,\
                                const OFString& instanceNumber,\
                                const Uint16 rows,\
                                const Uint16 columns,\
                                const IODEnhGeneralEquipmentModule::EquipmentInfo& equipmentInfo,\
                                const ContentIdentificationMacro& contentIdentification,\
                                const OFString& imageFlavor,\
                                const OFString& derivedPixelContrast,\
                                const DPMTypes::ContentQualification& contentQualification)

#else

// Helper macro to not to write the same thing multiple times
#define INSTANTIATE_CREATE(T) template DCMTK_DCMPMAP_EXPORT OFvariant<OFCondition,DPMParametricMapIOD>\
DPMParametricMapIOD::create<T >(const OFString& modality,\
                                const OFString& seriesNumber,\
                                const OFString& instanceNumber,\
                                const Uint16 rows,\
                                const Uint16 columns,\
                                const IODEnhGeneralEquipmentModule::EquipmentInfo& equipmentInfo,\
                                const ContentIdentificationMacro& contentIdentification,\
                                const OFString& imageFlavor,\
                                const OFString& derivedPixelContrast,\
                                const DPMTypes::ContentQualification& contentQualification)

#endif

// Instantiate all four permitted create() methods
INSTANTIATE_CREATE(IODImagePixelModule<Uint16>);
INSTANTIATE_CREATE(IODImagePixelModule<Sint16>);
INSTANTIATE_CREATE(IODFloatingPointImagePixelModule);
INSTANTIATE_CREATE(IODDoubleFloatingPointImagePixelModule);
