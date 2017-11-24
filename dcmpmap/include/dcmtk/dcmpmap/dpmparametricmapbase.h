/*
 *
 *  Copyright (C) 2016-2017, Open Connections GmbH
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
 *  Author: Jan Schlamelcher
 *
 *  Purpose: Generated base class representing the Parametric Map IOD
 *
 */

#ifndef DPMPARAMETRICMAPBASE_H
#define DPMPARAMETRICMAPBASE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmfg/fginterface.h"
#include "dcmtk/dcmfg/fgderimg.h"
#include "dcmtk/dcmfg/fgfracon.h"
#include "dcmtk/dcmfg/fgframeanatomy.h"
#include "dcmtk/dcmfg/fgframevoilut.h"
#include "dcmtk/dcmfg/fgpixeltransform.h"
#include "dcmtk/dcmfg/fgparametricmapframetype.h"
#include "dcmtk/dcmfg/fgpixmsr.h"
#include "dcmtk/dcmfg/fgplanor.h"
#include "dcmtk/dcmfg/fgplanpo.h"
#include "dcmtk/dcmfg/fgrealworldvaluemapping.h"
#include "dcmtk/dcmiod/iodimage.h"
#include "dcmtk/dcmiod/modacquisitioncontext.h"
#include "dcmtk/dcmiod/modcommoninstanceref.h"
#include "dcmtk/dcmiod/modenhequipment.h"
#include "dcmtk/dcmiod/modequipment.h"
#include "dcmtk/dcmiod/modgeneralimage.h"
#include "dcmtk/dcmiod/modgeneralseries.h"
#include "dcmtk/dcmiod/modgeneralstudy.h"
#include "dcmtk/dcmiod/modmultiframedimension.h"
#include "dcmtk/dcmiod/modmultiframefg.h"
#include "dcmtk/dcmiod/modpatient.h"
#include "dcmtk/dcmiod/modpatientstudy.h"
#include "dcmtk/dcmiod/modsopcommon.h"
#include "dcmtk/dcmiod/modimagepixel.h"
#include "dcmtk/dcmiod/modfloatingpointimagepixel.h"
#include "dcmtk/dcmpmap/dpmmodparametricmapimage.h"
#include "dcmtk/dcmpmap/dpmmodparametricmapseries.h"
#include "dcmtk/dcmpmap/dpmdef.h"

/** Class for managing the Parametric Map IOD attributes. Template parameter defines the type of pixel data
 *  to use: Unsigned or signed 16 bit integer, or 32 bit floating point, or 64 bit floating point data.
 */
class DCMTK_DCMPMAP_EXPORT DPMParametricMapBase
: public DcmIODImage
<
    IODImagePixelModule<Uint16>,
    IODImagePixelModule<Sint16>,
    IODFloatingPointImagePixelModule,
    IODDoubleFloatingPointImagePixelModule
>
{

public:

  /** Virtual Destructor
   */
  virtual ~DPMParametricMapBase();

  // -------------------- loading and saving ---------------------

  /** Save current object to given filename
   *  @param  filename The file to write to
   *  @param  writeXfer The transfer syntax to be used
   *  @return EC_Normal if writing was successful, error otherwise.
   */
  virtual OFCondition saveFile(const OFString& filename,
                               const E_TransferSyntax writeXfer = EXS_LittleEndianExplicit);

  /** Write current object to given item
   *  @param  dataset The item to write to
   *  @return EC_Normal if writing was successful, error otherwise.
   */
  OFCondition writeDataset(DcmItem& dataset);

  /** Perform some basic checking. This method is also invoked when
   *  writing the object to a DICOM dataset or file.
   *  @return OFTrue, if no errors were found, OFFalse otherwise.
   */
  virtual OFBool check();

  /** Get access to functional groups. This is meant for reading data from
   *  functional groups that are not actively managed, i.e.\ made accessible by
   *  DcmSegmentation. In rare cases, however, it makes sense to access it
   *  for writing too, e.g.\ in order to add Stacks; use with care!
   *  @return Reference to the functional groups
   */
  virtual FGInterface& getFunctionalGroups();

  /** Get Patient Module
   *  @return a reference to the IOD Patient Module
   */
  virtual IODPatientModule& getIODPatientModule();

  /** Get General Study Module
   *  @return a reference to the IOD General Study Module
   */
  virtual IODGeneralStudyModule& getIODGeneralStudyModule();

  /** Get Patient Study Module
   *  @return a reference to the IOD Patient Study Module
   */
  virtual IODPatientStudyModule& getIODPatientStudyModule();

  /** Get General Series Module
   *  @return a reference to the IOD General Series Module
   */
  virtual IODGeneralSeriesModule& getIODGeneralSeriesModule();

  /** Get Parametric Map Series Module
   *  @return a reference to the Parametric Map Series Module
   */
  virtual DPMParametricMapSeriesModule& getDPMParametricMapSeriesModule();

  /** Get General Equipment Module
   *  @return a reference to the General Equipment Module
   */
  virtual IODGeneralEquipmentModule& getIODGeneralEquipmentModule();

  /** Get Enhanced General Equipment Module
   *  @return a reference to the Enhanced General Equipment Module
   */
  virtual IODEnhGeneralEquipmentModule& getIODEnhGeneralEquipmentModule();

  /** Get General Image Module
   *  @return a reference to the General Image Module
   */
  virtual IODGeneralImageModule& getIODGeneralImageModule();

  /** Get Parametric Map Image Module
   *  @return a reference to the Parametric Map Image Module
   */
  virtual DPMParametricMapImageModule& getDPMParametricMapImageModule();

  /** Get Multi-frame Functional Groups Module
   *  @return a reference to the Multi-frame Functional Groups Module
   */
  virtual IODMultiFrameFGModule& getIODMultiFrameFGModule();

  /** Get Multi-frame Dimension Module
   *  @return a reference to the Multi-frame Dimension Module
   */
  virtual IODMultiframeDimensionModule& getIODMultiframeDimensionModule();

  /** Get Acquisition Context Module
   *  @return a reference to the Acquisition Context Module
   */
  virtual IODAcquisitionContextModule& getIODAcquisitionContextModule();

  /** Get Common Instance Reference Module
   *  @return a reference to the Common Instance Reference Module
   */
  virtual IODCommonInstanceReferenceModule& getIODCommonInstanceReferenceModule();

  /** Get SOP Common Module
   *  @return a reference to the SOP Common Module
   */
  virtual IODSOPCommonModule& getIODSOPCommonModule();

protected:

  /** Constructor; not public since object should be create via factory
   *  method in derived class.
   */
  DPMParametricMapBase();

   /** Constructor; not public since object should be create via factory
    *  method in derived class.
    */
  template<typename ImagePixel>
  DPMParametricMapBase(OFin_place_type_t(ImagePixel));

  /** Read Parametric Map attributes (all those handled by this class)
   *  from given item. Reads attributes from base class DcmIODCommon before.
   *  The current content is deleted even if the reading process fails.
   *  If the log stream is set and valid the reason for any error might be
   *  obtained from the error/warning output.
   *  @param  dataset  Reference to DICOM dataset from which the document
   *          should be read
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition read(DcmItem &dataset);

  /** Write current Parametric Map's attributes to DICOM dataset. Also
   *  writes attributes of base class DcmIODCommon afterwards.
   *  @param  dataset Reference to DICOM dataset to which the current document
   *          should be written.  The dataset is not cleared before writing
   *          to it!
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition write(DcmItem &dataset);

  /** Clear (removes) all attributes handled by the modules of this IOD.
   *  IOD Rules are not reset.
   */
  virtual void clearData();

  /** Loads file
   *  @param  dcmff The file format to load into
   *  @param  filename The filename of the file to load
   *  @param  dset Pointer to dataset after loading
   *  @return EC_Normal if loading was successful, error otherwise
   */
  static OFCondition loadFile(DcmFileFormat& dcmff,
                              const OFString& filename,
                              DcmDataset*& dset);

  /// Multi-frame Functional Groups high level interface
  FGInterface m_FGInterface;

  /// Parametric Map Series Module
  DPMParametricMapSeriesModule m_DPMParametricMapSeriesModule;

  /// Enhanced General Equipment Module
  IODEnhGeneralEquipmentModule m_IODEnhGeneralEquipmentModule;

  /// Parametric Map Image Module
  DPMParametricMapImageModule m_DPMParametricMapImageModule;

  /// Multi-frame Functional Groups Module
  IODMultiFrameFGModule m_IODMultiFrameFGModule;

  /// Multi-frame Dimension Module
  IODMultiframeDimensionModule m_IODMultiframeDimensionModule;

  /// Acquisition Context Module
  IODAcquisitionContextModule m_IODAcquisitionContextModule;

  /// Common Instance Reference Module
  IODCommonInstanceReferenceModule m_IODCommonInstanceReferenceModule;

};

#endif // DPMPARAMETRICMAPBASE_H
