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
 *  Purpose: Track Set's Measurement from a Tractography Results IOD
 *
 */


#ifndef TRCMEASUREMENT_H
#define TRCMEASUREMENT_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/modbase.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmiod/iodmacro.h"   // for CodeSequenceMacro
#include "dcmtk/dcmtract/trctypes.h"


/** Class representing a Measurement within the Tractography Results IOD. A
 *  measurement provides measurement values for all tracks in the related
 *  track set. Measurement values can be defined for all points in a track,
 *  or only selected point indices.
 */
class DCMTK_DCMTRACT_EXPORT TrcMeasurement
: public IODComponent
{

public:

  /** Class that holds actual measurement values.
   */
  class DCMTK_DCMTRACT_EXPORT Values : public IODComponent
  {
    public:

      // Allow read/write functions in DcmIODUtil to access class internals
      friend class DcmIODUtil;

      /** Create TrcMeasurement::Values from minimal data.
       *  @param  dataValues The measurement values
       *  @param  numValues The number of measurement values. Must be equal
       *          to the numbers of data points in the related track.
       *  @param  values The measurement values
       *  @return EC_Normal if creating was successful, error otherwise
       */
      static OFCondition create(const Float32* dataValues,
                                const unsigned long numValues,
                                TrcMeasurement::Values*& values /* result */);

     /**  Create TrcMeasurement::Values from minimal data by providing
       *  measurement values as well as indices of those points those
       *  measurement values apply to.
       *  @param  dataValues The measurement values
       *  @param  numValues The number of measurement values, must be equal or
       *          less to number of data points in the related track.
       *  @param  trackPointIndices The indices of the points the measurement
       *          values apply to
       *  @param  values The measurement values
       *  @return EC_Normal if creating was successful, error otherwise
       */
      static OFCondition createWithIndices(const Float32* dataValues,
                                           const unsigned long numValues,
                                           const Uint32* trackPointIndices,
                                           TrcMeasurement::Values*& values /* result */);

     /**  Get measurement values and the point indices (if applicable)
       *  @param  dataValues Pointer to memory buffer with returned values
       *  @param  numValues The number of measurement values returned in
       *          dataValues
       *  @param  trackPointIndices The indices of the points the measurement
       *          values apply to. If there is a measurement value for each
       *          points in the track, NULL will be returned here.
       *  @return EC_Normal if measurement values could be retrieved, error
       *          otherwise.
       */
     virtual OFCondition get(const Float32*& dataValues,
                             unsigned long& numValues,
                             const Uint32*& trackPointIndices);

      /** Destructor, frees memory
      */
      virtual ~Values();

      /** Resets rules to their original values
       */
      virtual void resetRules();

      /** Get name of component
       *  @return Name of the module ("MeasurementValuesSequenceItem")
       */
      virtual OFString getName() const;

    protected:

      /** Protected constructor, use create() method to create a new track set
      */
      Values();
  };

   // Allow read/write functions in DcmIODUtil to access class internals
   friend class DcmIODUtil;

  /** Create TrcMeasurement from minimal data
   *  @param  measurementCode Code describing what is measured. Code must be
   *          taken from CID 7263 "Diffusion Tractography Measurement Types"
   *  @param  measurementUnitCodes Code describing the physical unit measured.
   *          Code must taken from CID 82 "Units of Measurement" (based on UCUM)
   *  @param  measurement The measurement, if it could be created, NULL otherwise
   *  @return EC_Normal if measurement could be created, error otherwise
   */
  static OFCondition create(const CodeSequenceMacro& measurementCode,
                            const CodeSequenceMacro& measurementUnitCodes,
                            TrcMeasurement*& measurement /* result */);

  /** Destructor, frees memory
   */
  virtual ~TrcMeasurement();

  /**  Get measurement data for a selected track
    *  @param  trackNumber The track whose measurement data should be retrieved
    *  @param  type Returns the type of measurement
    *  @param  units Returns the units of measurement
    *  @param  measurementData Returns the measurement data values
    *  @param  numMeasurementData Returns the number of measurement values in
    *          measurementData
    *  @param  pointIndices Returns the point indices the measurement data
    *          values refer to. If they refer to all points in the track,
    *          NULL is returned here
    *  @return EC_Normal if measurement could be retrieved, error otherwise.
    */
  virtual OFCondition get(const size_t trackNumber,
                          CodeSequenceMacro& type,
                          CodeSequenceMacro& units,
                          const Float32*& measurementData,
                          unsigned long& numMeasurementData,
                          const Uint32*& pointIndices);

  /** Set measurement values for a single track. Requires measurement value for
   *  all points in the track.
   *  @param  trackNumber The number of the track to set values for (0 for
   *          first track)
   *  @param  measurementValues The measurement values
   *  @param  numValues Number of measurement values in measurementValues, must
   *          be equal to number of values in track.
   *  @return EC_Normal if values could be set, error code otherwise
   */
  virtual OFCondition setTrackValues(const size_t trackNumber,
                                     const Float32* measurementValues,
                                     const unsigned long& numValues);

  /** Set measurement values for a single track. Requires list of indices to
   *  those points in track for which the measurement values are defined for.
   *  @param  trackNumber The number of the track to set values for (0 for
   *          first track)
   *  @param  measurementValues The measurement values
   *  @param  numValues Number of measurement values in measurementValues, must
   *          be equal or less to number of values in track.
   *  @param  pointIndices Indices of the points in the track that the
   *          measurement values apply to. Number of values must be equal to
   *          numValues.
   *  @return EC_Normal if values could be set, error code otherwise
   */
  virtual OFCondition setTrackValues(const size_t trackNumber,
                                     const Float32* measurementValues,
                                     const unsigned long& numValues,
                                     const Uint32* pointIndices);

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

  /** Get measurement values for all tracks
   *  @return Reference to measurement values for all tracks
   */
  virtual OFVector<TrcMeasurement::Values*>& getValues();

  /** Get reference to type of measurement code
   *  @return Reference to type of measurement code
   */
  virtual CodeSequenceMacro& getType();

  /** Get reference to measurement units code
   *  @return Reference to measurement units code
   */
  virtual CodeSequenceMacro& getUnits();

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of component
   *  @return Name of the module ("MeasurementsSequenceItem")
   */
  virtual OFString getName() const;

  /** Clear all data
   */
  virtual void clearData();

protected:

  /** Protected constructor, use create() method to create a new track set
   */
  TrcMeasurement();

  /** Check whether measurement values for all tracks are set
   *  @return OFTrue if values are completely present, OFFalse otherwise
   */
  virtual OFBool checkValuesComplete();

private:

  /// Single item from Concept Name Code Sequence (type of measurement)
  CodeSequenceMacro m_Type;

  /// Single item from Measurement Units Code Sequence (units of measurement)
  CodeSequenceMacro m_Units;

  /// Items from from Measurement Values Sequence
  OFVector<TrcMeasurement::Values*> m_Values;
};

#endif // TRCMEASUREMENT_H
