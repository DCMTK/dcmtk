/*
 *
 *  Copyright (C) 2001-2003, OFFIS
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
 *  Module:  dcmimage
 *
 *  Author:  Alexander Haderer
 *
 *  Purpose: Implements PNG interface for plugable image formats
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 2003-02-11 13:18:38 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmimage/include/Attic/dipipng.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef __DIPIPNG_H
#define __DIPIPNG_H

#include "osconfig.h"

#ifdef WITH_LIBPNG

#include "diplugin.h"

class DiImage;

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
  E_pngFileMetainfo,
};

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
     *  @param inter metainfo type
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
 * Revision 1.1  2003-02-11 13:18:38  meichel
 * Added PNG export option to dcm2pnm and dcmj2pnm
 *
 *
 */
