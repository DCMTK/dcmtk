/*
 *
 *  Copyright (C) 2003-2010, OFFIS e.V.
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
 *  Module:  dcmimage
 *
 *  Author:  Alexander Haderer
 *
 *  Purpose: Implements PNG interface for plugable image formats
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:29 $
 *  CVS/RCS Revision: $Revision: 1.7 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIPIPNG_H
#define DIPIPNG_H

#include "dcmtk/config/osconfig.h"

#ifdef WITH_LIBPNG

#include "dcmtk/ofstd/ofstring.h"

#include "dcmtk/dcmimgle/diplugin.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiImage;


/*--------------------*
 *  type definitions  *
 *--------------------*/

/** describes the different types of interlace supported by
 *  the PNG plugin.  Enumeration depends on capabilities
 *  of underlying PNG library (libpng).
 */
enum DiPNGInterlace
{
  /// Adam7
  E_pngInterlaceAdam7,

  /// no interlace
  E_pngInterlaceNone
};

/** describes the different types of metainfo written to
 *  the png file
 */
enum DiPNGMetainfo
{
  /// no meta information
  E_pngNoMetainfo,

  /// file time, creator
  E_pngFileMetainfo
};


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Implementation of a PNG plugin for the dcmimgle/dcmimage library
 */
class DiPNGPlugin
  : public DiPluginFormat
{

  public:

    /** constructor
     */
    DiPNGPlugin();

    /** destructor
     */
    virtual ~DiPNGPlugin();

    /** write given image to a file stream (PNG format)
     *  @param image pointer to DICOM image object to be written
     *  @param stream stream to which the image is written (open in binary mode!)
     *  @param frame index of frame used for output (default: first frame = 0)
     *  @return true if successful, false otherwise
     */
    virtual int write(DiImage *image,
                      FILE *stream,
                      const unsigned long frame = 0) const;

    /** set interlace type for PNG creation
     *  @param inter interlace type
     */
    void setInterlaceType(DiPNGInterlace inter);

    /** set metainfo type for PNG creation
     *  @param minfo metainfo type
     */
    void setMetainfoType(DiPNGMetainfo minfo);

    /** get version information of the PNG library.
     *  Typical output format: "LIBPNG, Version 3.5.7"
     *  @return name and version number of the PNG library
     */
    static OFString getLibraryVersionString();


 private:

    /// PNG interlace type
    DiPNGInterlace interlaceType;

    /// PNG metainfo type
    DiPNGMetainfo metainfoType;
};

#endif
#endif


/*
 * CVS/RCS Log:
 * $Log: dipipng.h,v $
 * Revision 1.7  2010-10-14 13:16:29  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.6  2010-03-01 09:08:46  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.5  2005-12-08 16:01:42  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.4  2004/04/07 12:06:28  joergr
 * Removed comma at end of enumerator list.
 *
 * Revision 1.3  2003/12/23 12:09:24  joergr
 * Updated documentation to get rid of doxygen warnings.
 *
 * Revision 1.2  2003/12/17 18:18:08  joergr
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols).
 *
 * Revision 1.1  2003/02/11 13:18:38  meichel
 * Added PNG export option to dcm2pnm and dcmj2pnm
 *
 *
 */
