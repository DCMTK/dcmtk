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
 *  Purpose: Statistics from a Tractography Results IOD
 *
 */


#ifndef TRCSTATISTIC_H
#define TRCSTATISTIC_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/modbase.h"
#include "dcmtk/dcmiod/iodutil.h"
#include "dcmtk/dcmiod/iodmacro.h"
#include "dcmtk/dcmtract/trctypes.h"

// Forward declarations

class CodeSequenceMacro;

/** Base class representing a statistic within the "Tractography Results" IOD. A
 *  statistic is of a certain type and applies to a specific region of interest.
 *  For each Track Set and for each track there may be one or more statistics
 *  defined. The specializations can be found in classes TrcTracksStatistic and
 *  TrcTrackSetStatistic.
 */
class DCMTK_DCMTRACT_EXPORT TrcStatistic
: public IODComponent
{

public:

  // Allow read/write functions in DcmIODUtil to access class internals
  friend class DcmIODUtil;

  /** Constructor
   */
  TrcStatistic();

  /** Destructor, frees memory
   */
  virtual ~TrcStatistic();

  /** Clears all data
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

  /** Write attributes from this class into given item
   *  @param  destination The item to write to
   *  @result EC_Normal if writing was successful, error otherwise
   */
  virtual OFCondition write(DcmItem& destination);

  /** Set descriptive information of statistic
   *  @param  typeCode The type of statistic, DICOM prescribes codes from
   *          CID 7263 "Diffusion Tractography Measurement Types"
   *  @param  typeModifierCode Defines the region of interest, DICOM prescribes
   *          code from CID 7464 "General Region of Interest Measurement
   *          Modifiers"
   *  @param  unitsCode The physical units of the statistic value, DICOM
   *          prescribes code from CID 82 "Units of Measurement" (UCUM).
   *  @return EC_Normal if setting was successful, error otherwise.
   */
  virtual OFCondition set(const CodeSequenceMacro& typeCode,
                          const CodeSequenceMacro& typeModifierCode,
                          const CodeSequenceMacro& unitsCode);

  /** Get descriptive information of statistic
   *  @param  typeCode Returns type of statistic
   *  @param  typeModifierCode Returns region of interest
   *  @param  unitsCode Returns physical units of the statistic value
   *  @return EC_Normal if getting was successful, error otherwise.
   */
  virtual OFCondition get(CodeSequenceMacro& typeCode,
                          CodeSequenceMacro& typeModifierCode,
                          CodeSequenceMacro& unitsCode);

  /** Resets rules to their original values
   */
  virtual void resetRules();

protected:

  /// Single item from Concept Name Code Sequence
  CodeSequenceMacro m_Type;

  /// Single item from Modifier Code Sequence
  CodeSequenceMacro m_TypeModifier;

  /// Single item from Unit Code Sequence
  CodeSequenceMacro m_Units;

  /// Floating Point Value(s), one for each Track in Track Set (for Track
  /// Statistic), or a single value in case of Track Set Statistic
  OFVector<Float32*> m_Values;
};


/** Class representing a statistic for Tracks in Track Set. Besides describing
 *  information like type of statistic, region o interest and units of the
 *  statistic values, the statistic defines a single statistic value for each
 *  track. Thus the number of statistic values must be identical to the number
 *  of tracks in the containing Track Set.
 */
class TrcTracksStatistic : public TrcStatistic
{
public:

  /** Constructor
   */
  TrcTracksStatistic();

  /** Destructor, frees memory
   */
  ~TrcTracksStatistic();

  /** Create track statistic by providing the required information
   *  @param  typeCode The value for which the statistic is a summary. DICOM
   *          prescribes code from CID 7263 "Diffusion Tractography Measurement
   *          Types"
   *  @param  typeModifierCode The region of interest of the statistic. DICOM
   *          prescribes code from CID 7464 "General Region of Interest
   *          Measurement Modifiers"
   *  @param  unitsCode The physical units of the statistic value. DICOM
   *          prescribes code from CID 82 "Units of Measurement".
   *  @param  statisticValues The statistic values
   *  @param  numValues The number of statistic values in statisticValues, must
   *          be equal to number of Tracks in the Track Set that this statistic
   *          applies to
   *  @param  statistic Returns the resulting TrcTracksStatistic object (NULL
   *          in case of any error)
   *  @return EC_Normal if creation was successful, error otherwise
   */
  static OFCondition create(const CodeSequenceMacro& typeCode,
                            const CodeSequenceMacro& typeModifierCode,
                            const CodeSequenceMacro& unitsCode,
                            const Float32* statisticValues,
                            const size_t numValues,
                            TrcTracksStatistic*& statistic /* result */);

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

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of component
   *  @return Name of the module ("TrackStatisticsSequenceItem")
   */
  virtual OFString getName() const;

  /** Set Track statistic
   *  @param  typeCode The value for which the statistic is a summary. DICOM
   *          prescribes code from CID 7263 "Diffusion Tractography Measurement
   *          Types"
   *  @param  typeModifierCode The region of interest of the statistic. DICOM
   *          prescribes code from CID 7464 "General Region of Interest
   *          Measurement Modifiers"
   *  @param  unitsCode The physical units of the statistic value. DICOM
   *          prescribes code from CID 82 "Units of Measurement".
   *  @param  statisticValues The statistic values
   *  @param  numValues The number of statistic values in statisticValues, must
   *          equal the number of tracks in containing Track Set
   *  @return EC_Normal if setting was successful, error otherwise
   */
  virtual OFCondition set(const CodeSequenceMacro& typeCode,
                          const CodeSequenceMacro& typeModifierCode,
                          const CodeSequenceMacro& unitsCode,
                          const Float32* statisticValues,
                          const size_t numValues);

  /** Get Track statistic information
   *  @param  typeCode Returns value for which the statistic is a summary.
   *  @param  typeModifierCode Returns region of interest of the statistic.
   *  @param  unitsCode Returns physical units of the statistic value.
   *  @param  statisticValues Returns statistic values
   *  @param  numValues Returns number of statistic values in statisticValues,
   *          should be equal to number of tracks in containing Track Set
   *  @return EC_Normal if data could be retrieved, error otherwise
   */
  virtual OFCondition get(CodeSequenceMacro& typeCode,
                          CodeSequenceMacro& typeModifierCode,
                          CodeSequenceMacro& unitsCode,
                          const Float32*& statisticValues,
                          unsigned long& numValues);
};

/** Class representing a Track Set statistic, i.e. a single statistic value that
 *  is amended by information about type of statistic, region of interest it as
 *  well as physical unit of the value. A Track Set can define 0 or more
 *  statistics.
 */
class TrcTrackSetStatistic : public TrcStatistic
{
public:

  /** Constructor
   */
  TrcTrackSetStatistic();

  /** Destructor, frees memory
   */
  virtual ~TrcTrackSetStatistic();

  /** Create track statistic by providing the required information
   *  @param  typeCode The value for which the statistic is a summary. DICOM
   *          prescribes code from CID 7263 "Diffusion Tractography Measurement
   *          Types"
   *  @param  typeModifierCode The region of interest of the statistic. DICOM
   *          prescribes code from CID 7464 "General Region of Interest
   *          Measurement Modifiers"
   *  @param  unitsCode The physical units of the statistic value. DICOM
   *          prescribes code from CID 82 "Units of Measurement".
   *  @param  statisticValue The statistic value
   *  @param  statistic Returns the resulting TrcTrackSetStatistic object (NULL
   *          in case of any error)
   *  @return EC_Normal if creation was successful, error otherwise
   */
  static OFCondition create(const CodeSequenceMacro& typeCode,
                            const CodeSequenceMacro& typeModifierCode,
                            const CodeSequenceMacro& unitsCode,
                            const Float64 statisticValue,
                            TrcTrackSetStatistic*& statistic /* result */);

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

  /** Resets rules to their original values
   */
  virtual void resetRules();

  /** Get name of component
   *  @return Name of the module ("TrackSetStatisticsSequenceItem")
   */
  virtual OFString getName() const;

  /** Set Track Set statistic
   *  @param  typeCode The value for which the statistic is a summary. DICOM
   *          prescribes code from CID 7263 "Diffusion Tractography Measurement
   *          Types"
   *  @param  typeModifierCode The region of interest of the statistic. DICOM
   *          prescribes code from CID 7464 "General Region of Interest
   *          Measurement Modifiers"
   *  @param  unitsCode The physical units of the statistic value. DICOM
   *          prescribes code from CID 82 "Units of Measurement".
   *  @param  statisticValue The statistic value
   *  @return EC_Normal if setting was successful, error otherwise
   */
  virtual OFCondition set(const CodeSequenceMacro& typeCode,
                          const CodeSequenceMacro& typeModifierCode,
                          const CodeSequenceMacro& unitsCode,
                          const Float64 statisticValue);

  /** Get Track Set statistic information
   *  @param  typeCode Returns value for which the statistic is a summary.
   *  @param  typeModifierCode Returns region of interest of the statistic.
   *  @param  unitsCode Returns physical units of the statistic value.
   *  @param  statisticValue Returns the statistic value
   *  @return EC_Normal if data could be retrieved, error otherwise
   */
  virtual OFCondition get(CodeSequenceMacro& typeCode,
                          CodeSequenceMacro& typeModifierCode,
                          CodeSequenceMacro& unitsCode,
                          Float64& statisticValue);

};


#endif // TRCSTATISTIC_H
