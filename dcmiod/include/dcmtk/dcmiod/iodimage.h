/*
 *
 *  Copyright (C) 2015, Open Connections GmbH
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
 *  Module: dcmiod
 *
 *  Author: Michael Onken
 *
 *  Purpose: Class repres. image IOD by exposing common image IOD attributes
 *
 */

#ifndef IODIMAGE_H
#define IODIMAGE_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/iodcommn.h"
#include "dcmtk/dcmiod/modgeneralimage.h"
#include "dcmtk/dcmiod/modimagepixel.h"

/** Class for managing common image IOD attributes. At the moment support for
 *  the palette-related attributes and the smallest/largest image pixel value
 *  attributes is still missing. Also, pixel data must be handled separately.
 *  Modules supported on top of DcmIODCommon: General Image Module and
 *  Image Pixel Module.
 */
class DCMTK_DCMIOD_EXPORT DcmIODImage :
  public DcmIODCommon
{

public:

  /** Constructor
   */
  DcmIODImage();

  /** Get General Image Module
   *  @return Reference to General Image Module
   */
  IODGeneralImageModule& getGeneralImage();

  /** Get Image Pixel Module
   * @return Reference to Image Pixel Module
   */
  IODImagePixelModule& getImagePixel();

  /** Virtual Destructor
   */
  virtual ~DcmIODImage();

  /** Clear (removes) all attributes handled by the modules of this IOD.
   *  IOD Rules are not reset.
   */
  virtual void clearData();

  /** Read common image module attributes (all those handled by this class)
   *  from given item. Reads attributes from base class DcmIODCommon before.
   *  The current content is deleted even if the reading process fails.
   *  If the log stream is set and valid the reason for any error might be
   *  obtained from the error/warning output.
   *  @param  dataset  Reference to DICOM dataset from which the document
   *          should be read
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition read(DcmItem &dataset);

  /** Write current common image module's attributes to DICOM dataset. Also
   *  writes attributes of base class DcmIODCommon afterwards.
   *  @param  dataset Reference to DICOM dataset to which the current document
   *          should be written.  The dataset is not cleared before writing
   *          to it!
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition write(DcmItem &dataset);

private:

  /// General Image Module
  IODGeneralImageModule m_GeneralImage;

  /// Image Pixel Module
  IODImagePixelModule   m_ImagePixel;

};

#endif // IODIMAGE_H
