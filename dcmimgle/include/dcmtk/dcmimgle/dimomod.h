/*
 *
 *  Copyright (C) 1996-2011, OFFIS e.V.
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
 *  Purpose: DicomMonochromeModality (Header)
 *
 */


#ifndef DIMOMOD_H
#define DIMOMOD_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofcast.h"

#include "dcmtk/dcmimgle/diluptab.h"
#include "dcmtk/dcmimgle/diobjcou.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiDocument;
class DiInputPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to handle modality specific transformations (incl. modality LUT)
 */
class DCMTK_DCMIMGLE_EXPORT DiMonoModality
  : public DiObjectCounter
{

 public:

    /** constructor
     *
     ** @param  docu   pointer to dataset (encapsulated)
     *  @param  pixel  pointer to input pixel data
     */
    DiMonoModality(const DiDocument *docu,
                   DiInputPixel *pixel);

    /** constructor, rescale
     *
     ** @param  docu       pointer to dataset (encapsulated)
     *  @param  pixel      pointer to input pixel data
     *  @param  slope      rescale slope (<> 0)
     *  @param  intercept  rescale intercept
     */
    DiMonoModality(const DiDocument *docu,
                   DiInputPixel *pixel,
                   const double slope,
                   const double intercept);

    /** constructor, LUT
     *
     ** @param  docu         pointer to dataset (encapsulated)
     *  @param  pixel        pointer to input pixel data
     *  @param  data         element containing the modality LUT data
     *  @param  descriptor   element containing the modality LUT descriptor
     *  @param  explanation  element containing the modality LUT explanation (optional)
     */
    DiMonoModality(const DiDocument *docu,
                   DiInputPixel *pixel,
                   const DcmUnsignedShort &data,
                   const DcmUnsignedShort &descriptor,
                   const DcmLongString *explanation);

    /** constructor, no modality transform
     *
     ** @param  bits  number of bits per pixel
     */
    DiMonoModality(const int bits);

    /** destructor
     */
    virtual ~DiMonoModality();

    /** get integer representation
     *
     ** @return integer representation
     */
    inline EP_Representation getRepresentation() const
    {
        return Representation;
    }

    /** get minimum pixel value after modality transform
     *
     ** @return minimum pixel value
     */
    inline double getMinValue() const
    {
        return MinValue;
    }

    /** get maximum pixel value after modality transform
     *
     ** @return maximum pixel value
     */
    inline double getMaxValue() const
    {
        return MaxValue;
    }

    /** get number of bits describing the width of output data
     *
     ** @return number of bits (might be 0)
     */
    inline unsigned int getBits() const
    {
        return Bits;
    }

    /** get number of bits actually used to store the output data.
     *  (based on the range given by 'MinValue' and 'MaxValue')
     *
     ** @return number of used bits (might be 0)
     */
    inline unsigned int getUsedBits() const
    {
        return UsedBits;
    }

    /** get absolute (possible) minimum pixel value after modality transform
     *
     ** @return absolute minimum pixel value
     */
    inline double getAbsMinimum() const
    {
        return AbsMinimum;
    }

    /** get absolute (possible) maximum pixel value after modality transform
     *
     ** @return absolute maximum pixel value
     */
    inline double getAbsMaximum() const
    {
        return AbsMaximum;
    }

    /** get rescale intercept value
     *
     ** @return rescale intercept value
     */
    inline double getRescaleIntercept() const
    {
        return RescaleIntercept;
    }

    /** get rescale slope value
     *
     ** @return rescale slope value
     */
    inline double getRescaleSlope() const
    {
        return RescaleSlope;
    }

    /** get pointer to lookup table data
     *
     ** @return pointer to lookup table data or NULL if absent
     */
    inline const DiLookupTable *getTableData() const
    {
        return TableData;
    }

    /** get modality LUT explanation
     *
     ** @return modality LUT explanation or NULL if absent
     */
    inline const char *getExplanation() const
    {
        return (TableData != NULL) ? TableData->getExplanation() : OFstatic_cast(const char *, NULL);
    }

    /** check whether lookup table is present
     *
     ** @return true if lookup table is present, false otherwise
     */
    inline int hasLookupTable() const
    {
        return LookupTable;
    }

    /** check whether rescaling is present
     *
     ** @return true if rescaling is present, false otherwise
     */
    inline int hasRescaling() const
    {
        return Rescaling;
    }


 protected:

    /** initialize internal data structures and values
     *
     ** @param  docu   pointer to dataset (encapsulated)
     *  @param  pixel  pointer to input pixel data
     */
    int Init(const DiDocument *docu,
             DiInputPixel *pixel);

    /** check lookup table for validity (and possibly correct it)
     */
    void checkTable();

    /** check rescaling for validity (and possibly correct it)
     *
     ** @param  pixel  pointer to input pixel data
     */
    void checkRescaling(const DiInputPixel *pixel);

    /** determine integer representation used for the output data
     *
     ** @param  docu  pointer to dataset (encapsulated)
     */
    void determineRepresentation(const DiDocument *docu);


 private:

    /// integer representation
    EP_Representation Representation;

    /// minimum pixel value
    double MinValue;
    /// maximum pixel value
    double MaxValue;

    /// number of bits
    unsigned int Bits;
    /// number of used bits
    unsigned int UsedBits;

    /// absolute minimum pixel value
    double AbsMinimum;
    /// absolute maximum pixel value
    double AbsMaximum;

    /// rescale intercept
    double RescaleIntercept;
    /// rescale slope
    double RescaleSlope;

    /// status flag: lookup table present
    int LookupTable;
    /// status flag: rescaling present
    int Rescaling;

    /// pointer to modality lookup table
    DiLookupTable *TableData;

 // --- declarations to avoid compiler warnings

    DiMonoModality(const DiMonoModality &);
    DiMonoModality &operator=(const DiMonoModality &);
};


#endif
