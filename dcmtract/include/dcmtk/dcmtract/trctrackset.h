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
 *  Purpose: Class representing a Track Set from a Tractography Results IOD
 *
 */


#ifndef TRCTRACKSET_H
#define TRCTRACKSET_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/modbase.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmiod/iodmacro.h"   // for CodeSequenceMacro and CodeWithModifiers
#include "dcmtk/dcmtract/trctypes.h"


// Forward declarations
class TrcTracksStatistic;
class TrcTrackSetStatistic;
class TrcTrack;
class TrcMeasurement;

/** Class representing a Track Set within the Tractography Results IOD. A Track
 *  Set is a set of tracks (really), and might define a measurements and
 *  statistics which then must be populated for all tracks in the set.
 *
 */
class DCMTK_DCMTRACT_EXPORT TrcTrackSet
: public IODComponent
{

public:

  // Allow read/write functions in DcmIODUtil to access class internals
  friend class DcmIODUtil;

  /** Create TrcTrackSet object by proving required data
   *  @param  trackSetLabel Label for the Track Set (256 chars max, VR=LO)
   *  @param  trackSetDescription Description for the Track Set (2^32-2 bytes
   *          maximum, VR=UT)
   *  @param  trackSetAnatomy Code for the anatomy the Track Set covers. DICOM
   *          proposes CID 7710 "Tractography Anatomic Sites" (Baseline)
   *  @param  trackSetDiffusionModelCode Code for diffusion model used for Track
   *          Set. DICOM prescribes code from CID 7261 "Diffusion Model Value
   *          Types"
   *  @param  trackSetAlgorithmIdentification Algorithm used in order to
   *          create Track Set. DICOM prescribes code from CID 7262 "Diffusion
   *          Tractography Algorithm Families".
   *  @param  trackSet Returns the created Track Set, if successfull (NULL
   *          otherwise)
   *  @return EC_Normal if creation was successful, error otherwise
   */
  static OFCondition create(const OFString& trackSetLabel,
                            const OFString& trackSetDescription,
                            const CodeWithModifiers& trackSetAnatomy,
                            const CodeSequenceMacro& trackSetDiffusionModelCode,
                            const CodeSequenceMacro& trackSetAlgorithmIdentification,
                            TrcTrackSet*& trackSet /* result */);

  /** Destructor, frees memory
   */
  virtual ~TrcTrackSet();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Set missing values by inventing "default values". Automatically
   *  called during write() by IODComponent.
   */
  virtual void inventMissing();

  /** Get name of component
   *  @return Name of the module ("TrackSetSequenceItem")
   */
  virtual OFString getName() const;

  /** Clear all data
   */
  virtual void clearData();

  /** Read attributes from given item into this class
   *  @param source  The source to read from
   *  @param clearOldData If OFTrue, old data is cleared before reading. Otherwise
   *         old data is overwritten (or amended)
   *  @result EC_Normal if reading was successful, error otherwise
   */
  virtual OFCondition read(DcmItem& source,
                           const OFBool clearOldData = OFTrue);

  /** Get Tracks of this Track Set
   *  @return Reference to Tracks of this Track Set
   */
  virtual const OFVector<TrcTrack*>& getTracks();

  /** Write attributes from this class into given item
   *  @param  destination The item to write to
   *  @result EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& destination);

   /** Get Track Set Number (should be 1-n)
    *  @param  value Reference to variable in which the value should be stored
    *  @param  pos Index of the value to get (0..vm-1)
    *  @return EC_Normal if successful, an error code otherwise
    */
  virtual OFCondition getTrackSetNumber(Uint16 &value,
                                        const unsigned long pos = 0) const;

  /** Get Track Set Label
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getTrackSetLabel(OFString &value,
                                       const signed long pos = 0) const;

  /** Get Track Set Description
   *  @param  value Reference to variable in which the value should be stored
   *  @param  pos Index of the value to get (0..vm-1), -1 for all components
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getTrackSetDescription(OFString &value,
                                             const signed long pos = 0) const;

  /** Get reference to Track Set Anatomical Type Code
   *  @return Reference to the code
   */
  virtual CodeWithModifiers& getTrackSetAnatomy();

  /** Get copy of Recommended Display CIELab Value
   *  @param L Returns CIELab L value
   *  @param a Returns CIELab a value
   *  @param b Returns CIELab b value
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getRecommendedDisplayCIELabValue(Uint16& L,
                                                       Uint16& a,
                                                       Uint16& b);

  /** Get Recommended Line Thickness
   *  @param value Reference to the variable in which the value should be stored
   *  @param pos Index of the value to get (0..vm-1)
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition getRecommendedLineThickness(Float32& value,
                                                  const unsigned long pos = 0);
  /** Get Laterality.
   *  @param laterality Reference to the variable in which the value should be stored.
   *         If value in dataset is not set at all, LAT_UNKNOWN is returned. If
   *         a value is present that is not covered by the standard LAT_ERROR
   *         is returned.
   *  @return EC_Normal if successful (i.e. not LAT_ERROR), error code otherwise
   */
  virtual OFCondition getLaterality(TrcTypes::E_TrackSetLaterality& laterality);


  /** Find measurements of a specific type. They can be retrieved afterwards by
   *  using getMeasurement().
   *  @param  type The measurement type to look for
   *  @param  measurementNumbers Returns the measurement indexes that are
   *          of specified type.
   *  @return Always returns EC_Normal (for now).
   */
  virtual OFCondition findMeasurementsByType(const CodeSequenceMacro& type,
                                            OFVector<size_t>& measurementNumbers);

  /** Get measurement by index. The index can be identified by using
   *  findMeasurementsByType() or getMeasurementInfos()
   *  @param  measurementIndex The index of the measurement (starting from 0)
   *  @param  measurement Returns a reference to the measurement requested
   *  @return Returns EC_Normal if measurement exists, TRC_EC_NoSuchMeasurement
   *          otherwise
   */
  virtual OFCondition getMeasurement(const size_t measurementIndex,
                                     const TrcMeasurement*& measurement);

  /** Get descriptive information of all measurements
   *  @param typesAndUnits  Returns vector of pairs, which each pair holding
   *         a code for type of measurement and its physical units.
   */
  virtual void getMeasurementInfos(OFVector<OFPair<CodeSequenceMacro, CodeSequenceMacro> >& typesAndUnits);

  /** Get access to all statistics defined for the Tracks in this Track Set
   *  @return Returns reference to all Track statistics
   */
  virtual const OFVector<TrcTracksStatistic*>& getTrackStatistics();

  /** Get access to all statistics defined for the Tracks in this Track Set
   *  @return Returns reference to all Track statistics
   */
  virtual const OFVector<TrcTrackSetStatistic*>& getTrackSetStatistics();

  /** Get access to diffusion acquisition code
   *  @returns Returns reference to diffusion acquisition code
   */
  virtual CodeSequenceMacro& getDiffusionAcquisitionCode();

  /** Get access to diffusion model code used
   *  @return Returns reference to diffusion model code
   */
  virtual CodeSequenceMacro& getDiffusionModelCode();

  /** Get access to tracking algorithm identification codes
   *  @return Reference to tracking algorithm identification codes
   */
  virtual OFVector<CodeSequenceMacro*>& getTrackingAlgorithmIdentification();

  /** Returns number of Tracks in Track Set
   *  @return Number of Tracks in Track Set
   */
  size_t getNumberOfTracks();

  /** Returns number of Track Set Statistics in Track Set
   *  @return Number of Track Set Statistics in Track Set
   */
  size_t getNumberOfTrackSetStatistics();

  /** Returns number of Track Statistics in Track Set
   *  @return Number of Track Statistics in Track Set
   */
  size_t getNumberOfTrackStatistics();

  /** Returns number of Measurements in Track Set
   *  @return Number of Measurements in Track Set
   */
  size_t getNumberOfMeasurements();

  /** Add track to Track Set
   *  @param  pointData  Array of points (x,y,z coordinate for each point)
   *  @param  numPoints Number of points in pointData (i.e. number of point
   *          coordinates divided by 3)
   *  @param  recommendedCIELabColors Recommended display color(s) as CIELab
   *          values, i.e. L,a,b component(s). There are 3 modes:
   *          1) Single color: All points in track will be colored in that color
   *          2) No color (NULL): Color is not defined on a per-track basis but
   *          must be defined on Track Set level
   *          3) One color per point: Colors for all points are provided
   *          separately.
   *  @param  numColors  Number of colors defined in recommendedCIELabColors
   *          (must be either 0, 1 or numPoints)
   *  @param  result Returns the resulting Track if creation was successful,
   *          error otherwise
   */
  virtual OFCondition addTrack(const Float32* pointData,
                               const size_t numPoints,
                               const Uint16* recommendedCIELabColors,
                               const size_t numColors,
                               TrcTrack*& result);

  /** Add measurement for this Track Set
   *  @param  measurementCode The type of measurement. DICOM prescribes a code
   *          from  CID 7263 "Diffusion Tractography Measurement Types"
   *  @param  measurementUnitCodes The physical units of the measured value.
   *          DICOM prescribes a code from CID 82 "Units of Measurement".
   *  @param  measurement The created measurement, if successful, NULL otherwise
   *  @return EC_Normal if successful, error otherwise
   */
  virtual OFCondition addMeasurement(const CodeSequenceMacro& measurementCode,
                                     const CodeSequenceMacro& measurementUnitCodes,
                                     TrcMeasurement*& measurement /* result */);

  /** Add Track Statistic, i.e. statistic that is defined once and where a single
   *  value must be provided for all tracks.
   *  @param  typeCode The type of statistic, DICOM prescribes code from CID
   *          7263 "Diffusion Tractography Measurement Types".
   *  @param  typeModifierCode The region of interest of this statistic. DICOM
   *          prescribes code from CID 7464 "General Region of Interest
   *          Measurement Modifiers"
   *  @param  unitsCode The physical units of the measured value. DICOM
   *          prescribes a code from CID 82 "Units of Measurement".
   *  @param  values The statistical values, one for each track
   *  @param  numValues The number of values in statisticValues (must be equal to
   *          number of tracks in this Track Set)
   *  @param  statistic Returns the created statistic, if successful. NULL
   *          otherwise
   *  @return Returns EC_Normal if successful, error otherwise
   */
  virtual OFCondition addTrackStatistics(const CodeSequenceMacro& typeCode,
                                         const CodeSequenceMacro& typeModifierCode,
                                         const CodeSequenceMacro& unitsCode,
                                         const Float32* values,
                                         const size_t numValues,
                                         TrcTracksStatistic*& statistic /* result */);

  /** Add Track Set Statistic, i.e. a statistical value computed for the whole
   *  Track Set.
   *  @param  typeCode The type of statistic, DICOM prescribes code from CID
   *          7263 "Diffusion Tractography Measurement Types".
   *  @param  typeModifierCode The region of interest of this statistic. DICOM
   *          prescribes code from CID 7464 "General Region of Interest
   *          Measurement Modifiers"
   *  @param  unitsCode The physical units of the measured value. DICOM
   *          prescribes a code from CID 82 "Units of Measurement".
   *  @param  value The statistical value
   *  @param  statistic Returns the created statistic, if successful. NULL
   *          otherwise
   */
  virtual OFCondition addTrackSetStatistic(const CodeSequenceMacro& typeCode,
                                           const CodeSequenceMacro& typeModifierCode,
                                           const CodeSequenceMacro& unitsCode,
                                           const Float64 value,
                                           TrcTrackSetStatistic*& statistic /* result */);

  /** Set Track Set Label
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (LO) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setTrackSetLabel(const OFString &value,
                                       const OFBool checkValue = OFTrue);

  /** Set Track Set Description
   *  @param  value Value to be set (single value only) or "" for no value
   *  @param  checkValue Check 'value' for conformance with VR (UT) and VM (1)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setTrackSetDescription(const OFString &value,
                                             const OFBool checkValue = OFTrue);

  /** Set Recommended Display CIELab Value
   *  @param  L CIELab L component value
   *  @param  a CIELab a component value
   *  @param  b CIELab b component value
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setRecommendedDisplayCIELabValue(const Uint16 L,
                                                       const Uint16 a,
                                                       const Uint16 b);

  /** Set Recommended Line Thickness
   *  @param  value Value to be set.  The units shall be the same as the units
   *          of the coordinate system in which the point coordinates are specified.
   *  @param  checkValue Check 'value' for conformance with VR (FL)
   *          if enabled
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setRecommendedLineThickness(const Float32& value,
                                                  const OFBool checkValue = OFTrue);

  /** Set Laterality
   *  @param  value Value to be set
   *  @param  checkValue Check 'value' for conformance if enabled. Parameter is
   *          not used but present for consistency with other functions.
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition setLaterality(const TrcTypes::E_TrackSetLaterality value,
                                    const OFBool checkValue = OFTrue);

protected:

  /** Protected constructor, use create() method to create a new Track Set
   */
  TrcTrackSet();

  /** Reads Track statistics from item into dedicated structures
   *  @param  source The item to read from, must contain Track Set Statistics
   *          Sequence
   *  @return EC_Normal if reading was successful, error otherwise
   */
  OFCondition readTrackStatistics(DcmItem& source);

  /** Reads Track Set statistics from item into dedicated structures
   *  @param  source The item to read from, must contain Track Statistics
   *          Sequence
   *  @return EC_Normal if reading was successful, error otherwise
   */
  OFCondition readTrackSetStatistics(DcmItem& source);

  OFCondition readMeasurements(DcmItem& source);

  /** Reads Tracks from item into dedicated structures
   *  @param  source The item to read from, must contain Track Sequence
   *  @return EC_Normal if reading was successful, error otherwise
   */
  OFCondition readTracks(DcmItem& source);

  /** Write Track Statistics to given item
   *  @param  result Returns EC_Normal if writing was successful, error otherwise
   *          or if result has not been EC_Normal when calling
   *  @param  destination The destination item to write to. The Track Statistic
   *          Sequence will be written by this method.
   */
  void writeTrackStatistics(OFCondition& result,
                            DcmItem& destination);

  /** Write Track Statistics to given item
   *  @param  result Returns EC_Normal if writing was successful, error otherwise
   *          or if result has not been EC_Normal when calling
   *  @param  destination The destination item to write to. The Track Set
   *          Statistic Sequence will be written by this method.
   */
  void writeTrackSetStatistics(OFCondition& result,
                               DcmItem& destination);

  /** Write Measurements to given item
   *  @param  result Returns EC_Normal if writing was successful, error otherwise
   *          or if result has not been EC_Normal when calling
   *  @param  destination The destination item to write to. The Measurements
   *          Sequence will be written by this method.
   *  @return Same as parameter "result"
   */
  OFCondition writeMeasurements(OFCondition& result,
                                DcmItem& destination);

  /** Write Tracks to given item
   *  @param  result Returns EC_Normal if writing was successful, error otherwise
   *          or if result has not been EC_Normal when calling
   *  @param  destination The destination item to write to. The Tracks Sequence
   *          will be written by this method.
   *  @return Same as parameter "result"
   */
  OFCondition writeTracks(OFCondition& result,
                          DcmItem& destination);

  /** Check whether measurements have data for all tracks
   *  @return EC_Normal measurement data is complete, TRC_EC_MeasurementDataMissing
   *          otherwise
   */
  OFCondition checkMeasurements();

private:

  /// Data for all tracks (i.e. content of Track Sequence)
  OFVector<TrcTrack*> m_Tracks;

  /// Anatomy code for this Track Set
  CodeWithModifiers m_Anatomy;

  /// Measurements for this Track Set
  OFVector<TrcMeasurement*> m_Measurements;

  /// Statistics for Tracks in this Track Set
  OFVector<TrcTracksStatistic*> m_TrackStatistics;

  /// Statistics for Track Set
  OFVector<TrcTrackSetStatistic*> m_TrackSetStatistics;

  /// Code for diffusion acquisition used for this Track Set
  CodeSequenceMacro m_DiffusionAcquisitionCode;

  /// Code for diffusion model used for this Track Set
  CodeSequenceMacro m_DiffusionModelCode;

  /// Codes for algorithms used to create this Track Set
  OFVector<CodeSequenceMacro*> m_TrackingAlgorithmIdentification;

};

#endif // TRCTRACKSET_H
