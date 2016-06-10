/*
 *
 *  Copyright (C) 2016, Open Connections GmbH
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
 *  Module:  dcmtract
 *
 *  Author:  Michael Onken
 *
 *  Purpose: Class representing a Track from a Tractography Results IOD
 *
 */


#ifndef TCRTRACK_H
#define TCRTRACK_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/modbase.h"
#include "dcmtk/dcmtract/trctypes.h"


/** Class representing a Track of of the "Tractography Results" IOD. This
 *  is the data contained in the items of the Track Sequence.
 */
class DCMTK_DCMTRACT_EXPORT TrcTrack
: public IODComponent
{

public:

  // Allow read/write functions in DcmIODUtil to access class internals
  friend class DcmIODUtil;

  /** Create TrcTrack object from required data
   *  @param  trackDataPoints The data points of the track. Each point consists
   *          of x,y and z coordinate.
   *  @param  numPoints The number of data points (i.e. number of coordinates
   *          divided by 3!)
   *  @param  colors Optional parameter that defines the recommended display
   *          colors for all points. Each color is defined in CIELab space,
   *          which means "L", "a" and "b" value must be defined for each color.
   *          In DICOM, the value range must be 0 <= value <= 65535.
   *  @param  numColors The number of colors in the colors parameter (i.e. number
   *          of L,a,b components divided by 3!). Alternatively, a single
   *          CIELab color can be provided, independent from the number of
   *          points in the track. In that case the color is used to color
   *          all points in the track. See also
   *         setRecommendedDisplayCIELabValues() for more explanations.
   *  @param  trackSet Returns the resulting TrcTrack object (NULL in case of
   *          any error)
   *  @return EC_Normal if Track could be created, error otherwise
   */
  static OFCondition create(const Float32* trackDataPoints,
                            const size_t numPoints,
                            const Uint16* colors,
                            const size_t numColors,
                            TrcTrack*& trackSet /* result */);

  /** Destructor, frees memory
   */
  virtual ~TrcTrack();

  /** Get name of component
   *  @return Name of the module ("TrackSequenceItem")
   */
  virtual OFString getName() const;

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Clear all data
   */
  virtual void clearData();

  /** Check whether this component's data satisfies the underlying
   *  rules
   *  @param  quiet If OFTrue, not error / warning messages will be produced. Only
   *                the returned error code will indicate error or OK. Per default,
   *                logging output is produced (OFFalse).
   *  @result EC_Normal if rules are satisfied, error otherwise
   */
  virtual OFCondition check(const OFBool quiet = OFFalse);

  /** Read attributes from given item into this class
   *  @param source  The source to read from
   *  @param clearOldData If OFTrue, old data is cleared before reading. Otherwise
   *         old data is overwritten (or amended)
   *  @result EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Write attributes from this class into given item
   *  @param  destination The item to write to
   *  @result EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& destination);

   /** Get Track Data
    *  @param data  Pointer to the point data
    *  @return Returns number of points in the track or 0 if no points could
    *          be found.
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual size_t getTrackData(const Float32*& data) const;

  /** Get Number of data points
   */
  virtual size_t getNumDataPoints();

   /** Get Recommended Display CIELab Value. This will only return a result
    *  if a single value is defined to color the Track. Otherwise
    *  getRecommendedDisplayCIELabValueList() must be used.
    *  @param  L Returns L component of CIELab color
    *  @param  a Returns a component of CIELab color
    *  @param  b Returns b component of CIELab color
    *  @return EC_Normal if successful, an error code otherwise
    */
   virtual OFBool getRecommendedDisplayCIELabValue(Uint16& L,
                                                   Uint16& a,
                                                   Uint16& b);

   /** Get Recommended Display CIELab Value List. This will only return a result
    *  if one color per data point is defined.
    *  @param  colors Returns list of colors, each consisting of
    *          L,a,b components
    *  @return Number of colors (i.e. number of components divided
    *          by 3!). If no colors are defined for data points, 0 is returned.
    */
   size_t getRecommendedDisplayCIELabValueList(const Uint16*& colors);

   /** Returns the coloring mode, i.e. whether color is defined for all points
    *  in the track separately, or defined by a single color for the track, or
    *  if the track does not provide color information in which case the Track
    *  Set must provide a color (note that this is not checked, though).
    *  @return The coloring mode
    */
   TrcTypes::E_TrackColorMode getRecommendedDisplayCIELabMode();

  /** Set Track Data
   *  @param  trackDataPoints Array of values to be set, all points
   *          after another with each point consisting of x, y and z component.
   *  @param  numPoints Number of points in array (one point consists
   *          of x, y, and z coordinates)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setTrackData(const Float32* trackDataPoints,
                                   const size_t numPoints);

  /** Set Recommended Display CIELab Value (List)
   *  Depending on how many colors are provided, different DICOM is produced
   *  when writing the Track later:
   *  0: No color information for this Track is stored (i.e. neither "Recommended
   *     Display CIELab Value" is set nor "Recommended Display CIELab Value List".
   *     Thus there must be a color definition on Track Set level instead.
   *  1: The given color (consisting of L,a,b component) is recommended to be
   *     used for coloring every point in this track. This color will got to the
   *     element "Recommended Display CIELab value" for this particular track. In
   *     that case, the related Track Set must not have any color recommendation.
   *  >1: The number of colors must be equal to the number of data points. Each
   *     data point color is separately specified. This setting will lead to the
   *     element "Recommended Display CIELab Value List" to be written later.
   *     The related Track Set must not contain any color recommendation at all
   *     in that case.
   *  Note that if the third option (>1) is chosen, it must be provided for
   *  every track in the related Track Set!
   *  @param  colors Array of color values to be set, all colors one
   *          after another with each color consisting of L, a and b component.
   *  @param  numColors Number of colors in array (one point consists
   *          of x, y, and z coordinates).
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setRecommendedDisplayCIELabValues(const Uint16* colors,
                                                       const size_t numColors);

protected:

  /** Protected constructor, use create() method to create a new track set
   */
  TrcTrack();

  /** Cut off point data in the end if the number of components is not dividable
   *  by 3 (each point must have x,y and z component)
   *  @return OFTrue if situation is ok (after fixing if applicable), otherwise
   *          OFFalse (e.g. in case of an internal error)
   */
  OFBool fixPointCoordinatesDataVM();

};

#endif // TCRTRACK_H
