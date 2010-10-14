/*
 *
 *  Copyright (C) 1996-2010, OFFIS e.V.
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DicomOverlay (Header)
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:16:26 $
 *  CVS/RCS Revision: $Revision: 1.27 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DIOVLAY_H
#define DIOVLAY_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcast.h"

#include "dcmtk/dcmimgle/diobjcou.h"
#include "dcmtk/dcmimgle/diovdat.h"
#include "dcmtk/dcmimgle/diovpln.h"
#include "dcmtk/dcmimgle/diutils.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiDocument;

class DcmOverlayData;
class DcmLongString;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to handle a set of overlay planes
 */
class DiOverlay
  : public DiObjectCounter
{

 public:

    /** constructor
     *
     ** @param  docu    pointer to dataset (encapsulated)
     *  @param  alloc   bits allocated for image pixel data
     *  @param  stored  bits stored for image pixel data
     *  @param  high    high bit for image pixel data
     */
    DiOverlay(const DiDocument *docu = NULL,
              const Uint16 alloc = 0,
              const Uint16 stored = 0,
              const Uint16 high = 0);

    /** constructor, clip/scale
     *
     ** @param  overlay   pointer to reference overlay object
     *  @param  left_pos  x-coordinate of new overlay origin (offset for all planes)
     *  @param  top_pos   y-coordinate of new overlay origin (offset for all planes)
     *  @param  xfactor   scaling factor in x-direction
     *  @param  yfactor   scaling factor in y-direction
     */
    DiOverlay(const DiOverlay *overlay,
              const signed long left_pos,
              const signed long top_pos,
              const double xfactor,
              const double yfactor);

    /** constructor, flip
     *
     ** @param  overlay  pointer to reference overlay object
     *  @param  horz     flip horizontally if true
     *  @param  vert     flip vertically if true
     *  @param  columns  width of the surrounding image
     *  @param  rows     height of the surrounding image
     */
    DiOverlay(const DiOverlay *overlay,
              const int horz,
              const int vert,
              const Uint16 columns,
              const Uint16 rows);

    /** constructor, rotate
     *
     ** @param  overlay  pointer to reference overlay object
     *  @param  degree   angle by which the overlayplanes should be rotated
     *  @param  columns  width of the surrounding image
     *  @param  rows     height of the surrounding image
     */
    DiOverlay(const DiOverlay *overlay,
              const int degree,
              const Uint16 columns,
              const Uint16 rows);

    /** destructor
     */
    virtual ~DiOverlay();

    /** check whether specified plane is currently visible (not hidden)
     *
     ** @param  plane  index of plane (starting from 0) or group number (0x6000-0x60ff)
     *
     ** @return true if plane is visible, false otherwise
     */
    int isPlaneVisible(unsigned int plane);

    /** make specified plane visible
     *
     ** @param  plane  index of plane (starting from 0) or group number (0x6000-0x60ff)
     *
     ** @return status, true if successful, false otherwise
     */
    int showPlane(unsigned int plane);

    /** make specified plane visible and set parameters
     *
     ** @param  plane   index of plane (starting from 0) or group number (0x6000-0x60ff)
     *  @param  fore    foreground color for overlay plane (in percent: 0.0-1.0)
     *  @param  thresh  threshold value for overlay plane (0.0-1.0)
     *  @param  mode    new overlay mode (EMO_Default for stored mode)
     *
     ** @return status, true if successful, false otherwise
     */
    int showPlane(unsigned int plane,
                  const double fore,
                  const double thresh,
                  const EM_Overlay mode);

    /** make specified plane visible and set p-value.
     *  Only applicable for bitmap shutters.
     *
     ** @param  plane   index of plane (starting from 0) or group number (0x6000-0x60ff)
     *  @param  pvalue  p-value to be used for the overlay plane (0..65535)
     *
     ** @return status, true if successful, false otherwise
     */
    int showPlane(unsigned int plane,
                  const Uint16 pvalue);

    /** make all planes visible
     *
     ** @return status, true if successful, false otherwise
     */
    int showAllPlanes();

    /** make all planes visible
     *
     ** @param  fore    foreground color for overlay planes (in percent: 0.0-1.0)
     *  @param  thresh  threshold value for overlay planes (in percent: 0.0-1.0)
     *  @param  mode    new overlay mode (EMO_Default for stored mode)
     *
     ** @return status, true if successful, false otherwise
     */
    int showAllPlanes(const double fore,
                      const double thresh,
                      const EM_Overlay mode);

    /** make specified plane invisible
     *
     ** @param  plane  index of plane (starting from 0) or group number (0x6000-0x60ff)
     *
     ** @return status, true if successful, false otherwise
     */
    int hidePlane(unsigned int plane);

    /** make all planes invisible
     *
     ** @return status, true if successful, false otherwise
     */
    int hideAllPlanes();

    /** move plane to a new place
     *
     ** @param  plane     index of plane (starting from 0) or group number (0x6000-0x60ff)
     *  @param  left_pos  new x-coordinate of the overlay plane origin
     *  @param  top_pos   new y-coordinate of the overlay plane origin
     *
     ** @return status, true if successful, false otherwise
     */
    int placePlane(unsigned int plane,
                   const signed int left_pos,
                   const signed int top_pos);

    /** get group number of specified plane
     *
     ** @param  plane  index of plane (starting from 0) or group number (0x6000-0x60ff)
     *
     ** @return group number (0x6000-0x60ff) if successful, 0 otherwise
     */
    unsigned int getPlaneGroupNumber(unsigned int plane) const;

    /** get label of specified plane
     *
     ** @param  plane  index of plane (starting from 0) or group number (0x6000-0x60ff)
     *
     ** @return plane label if successful, NULL otherwise
     */
    const char *getPlaneLabel(unsigned int plane) const;

    /** get description of specified plane
     *
     ** @param  plane  index of plane (starting from 0) or group number (0x6000-0x60ff)
     *
     ** @return plane description if successful, NULL otherwise
     */
    const char *getPlaneDescription(unsigned int plane) const;

    /** get overlay mode of specified plane
     *
     ** @param  plane  index of plane (starting from 0) or group number (0x6000-0x60ff)
     *
     ** @return overlay mode if successful, EMO_Default otherwise
     */
    EM_Overlay getPlaneMode(unsigned int plane) const;

    /** get number of overlay planes
     *
     ** @return number of overlay planes
     */
    inline unsigned int getCount() const
    {
        return (Data != NULL) ? Data->Count : 0;
    }

    /** get current x-coordinate of overlay origin.
     *  This value is added to the origin of all overlay planes.
     *
     ** @return x-coordinate of overlay origin
     */
    inline signed long getLeft() const
    {
        return Left;
    }

    /** get current y-coordinate of overlay origin.
     *  This value is added to the origin of all overlay planes.
     *
     ** @return y-coordinate of overlay origin
     */
    inline signed long getTop() const
    {
        return Top;
    }

    /** check whether there are any overlay planes embedded in the pixel data
     *
     ** @return true if there are embedded planes, false otherwise
     */
    int hasEmbeddedData() const;

    /** add plane to list of additional overlay planes
     *
     ** @param  group        group number of plane to be added (0x6000-0x60ff).
     *                       If group number already exists in the list of additional planes
     *                       the old one is replaced. If the number exists in the list of
     *                       planes stored in the image dataset the new plane hides it.
     *  @param  left_pos     x-coordinate of the plane origin
     *  @param  top_pos      y-coordinate of the plane origin
     *  @param  columns      width of the overlay plane (in pixels)
     *  @param  rows         height of the overlay plane
     *  @param  data         element where the plane data is stored
     *  @param  label        element where the plane label is stored
     *  @param  description  element where the plane description is stored
     *  @param  mode         overlay plane mode
     *
     ** @return status, true if successful, false otherwise
     */
    int addPlane(const unsigned int group,
                 const signed int left_pos,
                 const signed int top_pos,
                 const unsigned int columns,
                 const unsigned int rows,
                 const DcmOverlayData &data,
                 const DcmLongString &label,
                 const DcmLongString &description,
                 const EM_Overlay mode);


    /** remove specified plane from list of additional overlay planes
     *
     ** @param  group  group number of plane to be removed (0x6000-0x60ff)
     *
     ** @return status, true if successful, false otherwise
     */
    int removePlane(const unsigned int group);

    inline DiOverlayPlane *getPlane(const unsigned int plane) const
    {
        return ((Data != NULL) && (Data->Planes != NULL) && (plane < Data->Count)) ? Data->Planes[plane] : OFstatic_cast(DiOverlayPlane *, NULL);
    }

    /** check whether specified plane exists
     *
     ** @param  plane    index of plane (starting from 0) or group number (0x6000-0x60ff)
     *  @param  visible  flag indicating whether plane should be visible or not (default: 0 = ignore)
     *
     ** @return status, true if such a plane exists, false otherwise
     */
    inline int hasPlane(unsigned int plane,
                        const int visible = 0) const
    {
        return (convertToPlaneNumber(plane, AdditionalPlanes) > 1) && (!visible || Data->Planes[plane]->isVisible());
    }

    /** get overlay plane data as an array of 1/8/16 bit values.
     *  Memory isn't handled internally and must therefore be deleted from calling program.
     *
     ** @param  frame     number of frame
     *  @param  plane     index of plane (starting from 0) or group number (0x6000-0x60ff)
     *  @param  left_pos  get x-coordinate of the plane origin
     *  @param  top_pos   get y-coordinate of the plane origin
     *  @param  width     get width of the overlay plane (in pixels)
     *  @param  height    get height of the overlay plane
     *  @param  mode      get overlay plane mode
     *  @param  columns   width of the surrounding image
     *  @param  rows      height of the surrounding image
     *  @param  bits      number of bits (stored) in the resulting array, default: 8
     *  @param  fore      foreground color used for the plane (default: 0xff = white, for 8 bits)
     *  @param  back      transparent background color (default: 0x00 = black)
     *
     ** @return pointer to pixel data if successful, NULL otherwise
     */
    void *getPlaneData(const unsigned long frame,
                       unsigned int plane,
                       unsigned int &left_pos,
                       unsigned int &top_pos,
                       unsigned int &width,
                       unsigned int &height,
                       EM_Overlay &mode,
                       const Uint16 columns,
                       const Uint16 rows,
                       const int bits = 8,
                       const Uint16 fore = 0xff,
                       const Uint16 back = 0x0);

    /** get overlay plane data as an array of 1/8/16 bit values.
     *  Memory isn't handled internally and must therefore be deleted from calling program.
     *
     ** @param  frame    number of frame
     *  @param  plane    index of plane (starting from 0) or group number (0x6000-0x60ff)
     *  @param  width    get width of the overlay plane (in pixels)
     *  @param  height   get height of the overlay plane
     *  @param  bits     number of bits (stored) in the resulting array, default: 8
     *  @param  fore     foreground color used for the plane (default: 0xff = white, for 8 bits)
     *  @param  back     transparent background color (default: 0x00 = black)
     *
     ** @return pointer to pixel data if successful, NULL otherwise
     */
    void *getFullPlaneData(const unsigned long frame,
                           unsigned int plane,
                           unsigned int &width,
                           unsigned int &height,
                           const int bits = 8,
                           const Uint16 fore = 0xff,
                           const Uint16 back = 0x0);

    /** create overlay plane data in (6xxx,3000) format.
     *  (1 bit allocated and stored, foreground color is 1, background color is 0,
     *   data is 16 bit padded - even length)
     *  Memory isn't handled internally and must therefore be deleted from calling program.
     *
     ** @param  buffer  stores pointer to overlay data (memory is allocated internally)
     *  @param  plane   number (0..15) or group number (0x60nn) of overlay plane
     *  @param  width   returns width of overlay plane (in pixels)
     *  @param  height  returns height of overlay plane (in pixels)
     *  @param  frames  returns number of frames (multiple overlay frames possible!)
     *
     ** @return number of bytes allocated for the 'buffer' if successful, 0 otherwise
     */
    unsigned long create6xxx3000PlaneData(Uint8 *&buffer,
                                          unsigned int plane,
                                          unsigned int &width,
                                          unsigned int &height,
                                          unsigned long &frames);

    /// constant defining the maximum number of overlay planes (16)
    static const unsigned int MaxOverlayCount;
    /// constant defining the group number of the first overlay plane (0x6000)
    static const unsigned int FirstOverlayGroup;


 protected:

    /** initialize new overlay managing object
     *
     ** @param  overlay  reference object used as a template
     *
     ** @return pointer to a new array of pixel data
     */
    Uint16 *Init(const DiOverlay *overlay);

    /** convert given plane or group number to a valid plane number
     *
     ** @param  plane  plane or group number to be converted
     *  @param  mode   true or false, see return value
     *
     ** @return status, true if successful, false otherwise.
     *          (1 = additional plane with the specified group number would be new,
     *           2 = additional plane with the specified group number already exists,
     *           3 = 'plane' is the number of a valid plane - only if 'mode' us false)
     */
    int convertToPlaneNumber(unsigned int &plane,
                             const int mode) const;

    /** convert five plane number to corresponding group number
     *
     ** @param  plane  index of plane (0..15)
     *
     ** @return group number
     */
    unsigned int convertToGroupNumber(const unsigned int plane) const
    {
        return FirstOverlayGroup + 2 * plane;
    }

    /** check whether given group number is valid.
     *  Valid means that the number is even and between 0x6000 and 0x60ff.
     *
     ** @param  group  group number to be checked
     *
     ** @return status, true if valid, false otherwise
     */
    int isValidGroupNumber(const unsigned int group) const;

    /** check whether the specified plane is valid. and determine maximum resolution
     *  and number of frames which are common for all overlay planes so far.
     *
     ** @param  plane  index of plane (0..15)
     *  @param  mode   if true the maximum number of frames is determined, otherwise not
     *
     ** @return status, true if successful, false otherwise
     */
    int checkPlane(const unsigned int plane,
                   const int mode = 1);


 private:

    /// left offset to all planes
    signed long Left;
    /// top offset to all planes
    signed long Top;
    /// maximum width of all planes
    Uint16 Width;
    /// maximum height of all planes
    Uint16 Height;
    /// maximum number of frames
    unsigned long Frames;

    /// status, if true planes are added later with addPlane()
    int AdditionalPlanes;

    /// points to overlay data (provides shared data)
    DiOverlayData *Data;

 // --- declarations to avoid compiler warnings

    DiOverlay(const DiOverlay &);
    DiOverlay &operator=(const DiOverlay &);
};


#endif


/*
 *
 * CVS/RCS Log:
 * $Log: diovlay.h,v $
 * Revision 1.27  2010-10-14 13:16:26  joergr
 * Updated copyright header. Added reference to COPYRIGHT file.
 *
 * Revision 1.26  2010-03-01 09:08:47  uli
 * Removed some unnecessary include directives in the headers.
 *
 * Revision 1.25  2008-11-18 11:01:28  joergr
 * Fixed issue with incorrectly encoded overlay planes (wrong values for
 * OverlayBitsAllocated and OverlayBitPosition).
 *
 * Revision 1.24  2005/12/08 16:48:01  meichel
 * Changed include path schema for all DCMTK header files
 *
 * Revision 1.23  2003/12/08 19:32:14  joergr
 * Adapted type casts to new-style typecast operators defined in ofcast.h.
 * Removed leading underscore characters from preprocessor symbols (reserved
 * symbols). Updated CVS header.
 *
 * Revision 1.22  2003/06/12 15:08:34  joergr
 * Fixed inconsistent API documentation reported by Doxygen.
 *
 * Revision 1.21  2002/12/09 13:32:55  joergr
 * Renamed parameter/local variable to avoid name clashes with global
 * declaration left and/or right (used for as iostream manipulators).
 *
 * Revision 1.20  2001/09/28 13:09:59  joergr
 * Added method to extract embedded overlay planes from pixel data and store
 * them in group (6xxx,3000) format.
 *
 * Revision 1.19  2001/06/01 15:49:49  meichel
 * Updated copyright header
 *
 * Revision 1.18  2001/05/14 09:49:18  joergr
 * Added support for "1 bit output" of overlay planes; useful to extract
 * overlay planes from the pixel data and store them separately in the dataset.
 *
 * Revision 1.17  2000/03/08 16:24:22  meichel
 * Updated copyright header.
 *
 * Revision 1.16  1999/10/20 10:34:04  joergr
 * Enhanced method getOverlayData to support 12 bit data for print.
 *
 * Revision 1.15  1999/09/17 12:46:05  joergr
 * Added/changed/completed DOC++ style comments in the header files.
 * Splitted file diovlay.h into two files (one for each class).
 *
 * Revision 1.14  1999/08/25 16:41:54  joergr
 * Added new feature: Allow clipping region to be outside the image
 * (overlapping).
 *
 * Revision 1.13  1999/05/03 11:09:30  joergr
 * Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 * Revision 1.12  1999/04/29 16:46:46  meichel
 * Minor code purifications to keep DEC cxx 6 quiet.
 *
 * Revision 1.11  1999/03/24 17:20:19  joergr
 * Added/Modified comments and formatting.
 *
 * Revision 1.10  1999/03/22 08:52:17  joergr
 * Added parameter to specify (transparent) background color for method
 * getOverlayData().
 *
 * Revision 1.9  1999/02/08 12:39:50  joergr
 * Corrected some typos and formatting.
 *
 * Revision 1.8  1999/02/03 17:33:51  joergr
 * Added support for calibration according to Barten transformation (incl.
 * a DISPLAY file describing the monitor characteristic).
 *
 * Revision 1.7  1998/12/23 13:21:29  joergr
 * Changed parameter type (long to int) to avoid warning reported by MSVC5.
 *
 * Revision 1.6  1998/12/23 11:36:28  joergr
 * Changed order of parameters for addOverlay() and getOverlayData().
 * Corrected bug concerning additional overlay planes.
 *
 * Revision 1.5  1998/12/22 14:35:30  joergr
 * Added method to check whether plane is visible, to get plane mode and to
 * remove all planes. Set 'value' used for getOverlay/PlaneData().
 * Changed meaning of return values (differentiate between different value
 * for 'true').
 *
 * Revision 1.4  1998/12/16 16:37:50  joergr
 * Added method to export overlay planes (create 8-bit bitmap).
 * Implemented flipping and rotation of overlay planes.
 *
 * Revision 1.3  1998/12/14 17:27:35  joergr
 * Added methods to add and remove additional overlay planes (still untested).
 * Added methods to support overlay labels and descriptions.
 *
 * Revision 1.2  1998/11/30 12:24:26  joergr
 * Removed wrong 'inline' from method declaration (reported by MSVC5).
 *
 * Revision 1.1  1998/11/27 15:42:39  joergr
 * Added copyright message.
 * Added method to detach pixel data if it is no longer needed.
 * Replaced delete by delete[] for array types.
 * Added methods and constructors for flipping and rotating, changed for
 * scaling and clipping.
 *
 * Revision 1.5  1998/07/01 08:39:26  joergr
 * Minor changes to avoid compiler warnings (gcc 2.8.1 with additional
 * options), e.g. add copy constructors.
 *
 * Revision 1.4  1998/05/11 14:53:25  joergr
 * Added CVS/RCS header to each file.
 *
 *
 */
