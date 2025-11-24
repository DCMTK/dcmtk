/*
 *
 *  Copyright (C) 2015-2025, Open Connections GmbH
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
 *  Purpose: Class for managing common types needed by dcmiod module
 *
 */

#ifndef IODTYPES_H
#define IODTYPES_H

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmiod/ioddef.h"
#include "dcmtk/oflog/oflog.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/dcmdata/dcerror.h"

// ----------------------------------------------------------------------------
// Define the loggers for this module
// ----------------------------------------------------------------------------

extern DCMTK_DCMIOD_EXPORT OFLogger DCM_dcmiodLogger;

#define DCMIOD_TRACE(msg) OFLOG_TRACE(DCM_dcmiodLogger, msg)
#define DCMIOD_DEBUG(msg) OFLOG_DEBUG(DCM_dcmiodLogger, msg)
#define DCMIOD_INFO(msg) OFLOG_INFO(DCM_dcmiodLogger, msg)
#define DCMIOD_WARN(msg) OFLOG_WARN(DCM_dcmiodLogger, msg)
#define DCMIOD_ERROR(msg) OFLOG_ERROR(DCM_dcmiodLogger, msg)
#define DCMIOD_FATAL(msg) OFLOG_FATAL(DCM_dcmiodLogger, msg)

// ----------------------------------------------------------------------------
// Error constants
// ----------------------------------------------------------------------------

extern DCMTK_DCMIOD_EXPORT const OFConditionConst IOD_EC_WrongSOPClass;
extern DCMTK_DCMIOD_EXPORT const OFConditionConst IOD_EC_MissingAttribute;
extern DCMTK_DCMIOD_EXPORT const OFConditionConst IOD_EC_MissingSequenceData;
extern DCMTK_DCMIOD_EXPORT const OFConditionConst IOD_EC_InvalidDimensions;
extern DCMTK_DCMIOD_EXPORT const OFConditionConst IOD_EC_CannotInsertFrame;
extern DCMTK_DCMIOD_EXPORT const OFConditionConst IOD_EC_InvalidPixelData;
extern DCMTK_DCMIOD_EXPORT const OFConditionConst IOD_EC_InvalidObject;
extern DCMTK_DCMIOD_EXPORT const OFConditionConst IOD_EC_CannotDecompress;
extern DCMTK_DCMIOD_EXPORT const OFConditionConst IOD_EC_NoSuchRule;
extern DCMTK_DCMIOD_EXPORT const OFConditionConst IOD_EC_InvalidLaterality;
extern DCMTK_DCMIOD_EXPORT const OFConditionConst IOD_EC_InvalidElementValue;
extern DCMTK_DCMIOD_EXPORT const OFConditionConst IOD_EC_InvalidReference;
extern DCMTK_DCMIOD_EXPORT const OFConditionConst IOD_EC_ReferencesOmitted;
extern DCMTK_DCMIOD_EXPORT const OFConditionConst IOD_EC_InvalidColorPalette;

/** Class that wraps some constant definitions in the context of IODs
 */
class DCMTK_DCMIOD_EXPORT DcmIODTypes
{

public:
    /** Base class to store pixel data for frames. Right now 8 and 16 bit are supported.
     *  Per default, the class will release memory in the destructor.
     */
    struct FrameBase
    {
        /// Destructor
        FrameBase() {}
        /** Returns pixel data size in bytes
         *  @return Size of pixel data in bytes
         */
        virtual size_t getLengthInBytes() const = 0;

        /** Get pointer to pixel data
         *  @return Pointer to pixel data
         */
        virtual void* getPixelData() const = 0;

        /** Get bytes used per pixel
         *  @return Bytes per pixel (right now 8 or 16)
         */
        virtual Uint8 bytesPerPixel() const = 0;

        /** Get value at given index as 8 bit value
         *  @param byteVal The value at the given index
         *  @param index The index to get the value from
         *  @return EC_Normal if successful, EC_IllegalCall if index is out of bounds
         */
        virtual OFCondition getUint8AtIndex(Uint8 &byteVal, const size_t index) const =0;

        /** Get value at given index as 16 bit value
         *  @param shortVal The value at the given index
         *  @param index The index to get the value from
         *  @return EC_Normal if successful, EC_IllegalCall if index is out of bounds
         */
        virtual OFCondition getUint16AtIndex(Uint16 &shortVal, const size_t index) const =0;

        /** Set whether Frame class should release memory (default) or whether it will
         *  be released externally.
         *  @param release OFTrue if Frame should release memory, OFFalse otherwise
         */
        virtual void setReleaseMemory(OFBool release) = 0;

        /** Print frame data to string (for debugging purposes)
         *  @return String representation of frame data
         */
        virtual OFString print() const = 0;

        /** Deconstructor, frees frame data if not disabled via setReleaseMemory() method
         */
        virtual ~FrameBase() {}
    };

    /** Struct representing a single frame
     */
    template<typename PixelType>
    class Frame : public FrameBase
    {
    public:

        /** Default constructor
         */
        Frame() : m_pixData(NULL), m_numPixels(0), m_releaseMemory(OFTrue) {}

        /** Constructor that creates pixel data of the given size (amount of pixels)
         *  @param numPixels Number of pixels to allocate
         */
        Frame(const size_t numPixels) : m_pixData(NULL), m_numPixels(numPixels), m_releaseMemory(OFTrue)
        {
            m_pixData = new PixelType[numPixels];
        }

        /** Constructor that takes over pixel data for managing
         *  @param pixelData Pointer to pixel data that is afterwards managed by this class
         *  @param sizeInBytes Size of pixel data in bytes
         */
        Frame(PixelType* pixelData, const size_t sizeInBytes) : m_pixData(pixelData), m_numPixels(0), m_releaseMemory(OFTrue)
        {
            m_numPixels = sizeInBytes / sizeof(PixelType);
        }

        /** Copy constructor, copies pixel data
         *  @param rhs Frame to copy
         */
        Frame(const Frame& rhs)
        {
            delete[] m_pixData;
            m_pixData = new PixelType[rhs.m_numPixels];
            memcpy(m_pixData, rhs.m_pixData, rhs.m_numPixels);
            m_numPixels = rhs.m_numPixels;
            m_releaseMemory = rhs.m_releaseMemory;
        };

        /** Assignment constructor, copies pixel data
         *  @param rhs Frame to copy from
         *  @return Reference to this object
         */
        Frame& operator=(const Frame& rhs)
        {
            if (this != &rhs)
            {
                delete[] m_pixData;
                m_pixData = new PixelType[rhs.m_numPixels];
                memcpy(m_pixData, rhs.m_pixData, rhs.m_numPixels);
                m_numPixels = rhs.m_numPixels;
                m_releaseMemory = rhs.m_releaseMemory;
            }
            return *this;
        }

        /** Set whether pixel data should be release by this class (default)
         *  or is managed externally.
         *  @param release OFTrue if memory should be released, OFFalse otherwise
         */
        virtual void setReleaseMemory(OFBool release)
        {
            m_releaseMemory = release;
        }

        /** Get size of pixel data in bytes
         *  @return Size of pixel data in bytes
         */
        virtual size_t getLengthInBytes() const
        {
            return m_numPixels * bytesPerPixel(); // PixelType is always 1 or 2 bytes
        }

        /** Returns pointer to pixel data (untyped)
         *  @return Pointer to pixel data
         */
        virtual void* getPixelData() const
        {
            return m_pixData;
        }

        /** Returns pointer to pixel data (typed)
         *  @return Pointer to pixel data
         */
        virtual PixelType* getPixelDataTyped() const
        {
            return m_pixData;
        }

        /** Returns number of pixels (not necessarily bytes) in the frame
         *  @return Number of pixels
         */
        virtual Uint8 bytesPerPixel() const
        {
            return sizeof(PixelType);
        }

        /** Get value at given index as 8 bit value
         *  @param byteVal The value at the given index
         *  @param index The index to get the value from
         *  @return EC_Normal if successful, EC_IllegalCall if index is out of bounds
         */
        virtual OFCondition getUint8AtIndex(Uint8 &byteVal, const size_t index) const
        {
            if (index >= m_numPixels) {
                return EC_IllegalCall;
            }
            // Since sizeof(PixelType) is known during compile time an
            // we would require C++11 to mark the condition as constexpr
            // we disable the related warning in general.
#include DCMTK_DIAGNOSTIC_PUSH
#include DCMTK_DIAGNOSTIC_IGNORE_VISUAL_STUDIO_CONSTANT_EXPRESSION_WARNING
// Add range check for 16-bit to 8-bit conversion
            if (sizeof(PixelType) == 2 && m_pixData[index] > 255) {
                DCMIOD_ERROR("Value in the frame is too large to be cast to 8 bits");
                return EC_IllegalCall;
            }
            byteVal = static_cast<Uint8>(m_pixData[index]);
            return EC_Normal;
        }
#include DCMTK_DIAGNOSTIC_POP

        /** Get value at given index as 16 bit value
         *  @param shortVal The value at the given index
         *  @param index The index to get the value from
         *  @return EC_Normal if successful, EC_IllegalCall if index is out of bounds
         */
        virtual OFCondition getUint16AtIndex(Uint16 &shortVal, const size_t index) const
        {
            if (index >= m_numPixels) {
                return EC_IllegalCall;
            }
            shortVal = static_cast<Uint16>(m_pixData[index]);
            return EC_Normal;
        }

        /** Print frame data to string (for debugging purposes)
         *  @return String representation of frame data
         */
        virtual OFString print() const
        {
            OFStringStream ss;
            ss << "Frame with " << m_numPixels << " bytes:\n";
            for (size_t i = 0; i < m_numPixels; i++)
            {
                ss << STD_NAMESPACE hex << OFstatic_cast(Uint16, m_pixData[i]) << " ";
            }
            ss << "\n";
            return ss.str().c_str();
        }

        /// Array for the pixel data bytes
        PixelType* m_pixData;
        /// Number of pixels in the frame
        size_t m_numPixels;
        // Denote whether to release memory in destructor
        OFBool m_releaseMemory;

        /** Destructor, frees memory if not disabled via setReleaseMemory() method
         */
        ~Frame()
        {
            if (m_releaseMemory)
            {
                delete[] m_pixData;
                m_pixData = NULL;
            }
        }
    };

    /** IOD Information Entities (incomplete list, extended as needed)
     */
    enum IOD_IE
    {
        /// Undefined Information Entity (i.e.\ no value set)
        IE_UNDEFINED,
        /// Patient Entity
        IE_PATIENT,
        /// Study Entity
        IE_STUDY,
        /// Series Entity
        IE_SERIES,
        /// Frame of Reference Entity
        IE_FOR,
        /// Equipment Entity
        IE_EQUIPMENT,
        /// Image Entity
        IE_IMAGE,
        //// Meta Entity: Instance covering image, waveform, etc.
        IE_INSTANCE
    };

    /** Enumerated values for attribute "Laterality"
     */
    enum IOD_LATERALITY
    {
        /// Undefined (e.g.\ value not set)
        LATERALITY_UNDEFINED,
        /// Left body part
        LATERALITY_L,
        /// Right body part
        LATERALITY_R
    };

    /** Enhanced US Image Module: Image Type (first value)
     */
    enum IOD_ENHUSIMAGETYPE
    {
        /// Unknown
        IMAGETYPE_UNKNOWN,
        /// ORIGINAL
        IMAGETYPE_ORIGINAL,
        /// DERIVED
        IMAGETYPE_DERIVED
    };

private:
    /** Private undefined default constructor
     */
    DcmIODTypes() {};

    /** Private undefined destructor
     */
    ~DcmIODTypes() {};
};


#endif // IODTYPES_H
