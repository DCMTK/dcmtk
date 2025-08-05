/*
 *
 *  Copyright (C) 2024-2025, Open Connections GmbH
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
 *  Purpose: Class for managing the Palette Color Lookup Table Module
 *
 */

#ifndef MODPALETTECOLORLUT_H
#define MODPALETTECOLORLUT_H

#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmdata/dctagkey.h"
#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/dcmiod/iodrules.h"
#include "dcmtk/dcmiod/modbase.h"
#include "dcmtk/ofstd/ofcond.h"
#include "dcmtk/ofstd/oftypes.h"

/** Class representing the Palette Color Lookup Table Module:
 *
 * Red Palette Color Lookup Table​ Descriptor (0028,1101): (US or SS, 3, 1)
 * Green Palette Color Lookup Table​ Descriptor (0028,1102): (US or SS, 3, 1)
 * Blue Palette Color Lookup Table​ Descriptor (0028,1103): (US or SS, 3, 1)
 * Palette Color Lookup Table UID (0028,1199): (UI, 1, 3)
 * Red Palette Color Lookup Table​ Data​ (0028,1201): (OW, 1-n, 1C)
 * Green Palette Color Lookup Table​ Data​ (0028,1202): (OW, 1-n, 1C)
 * Blue Palette Color Lookup Table​ Data​ (0028,1203): (OW, 1-n, 1C)
 * Segmented Red Palette Color Lookup Table​ Data​ (0028,1221): (OW, 1-n, 1C)
 * Segmented Green Palette Color Lookup Table​ Data​ (0028,1222): (OW, 1-n, 1C)
 * Segmented Blue Palette Color Lookup Table​ Data​ (0028,1223): (OW, 1-n, 1C)
 */
class DCMTK_DCMIOD_EXPORT IODPaletteColorLUTModule : public IODModule
{

public:
    /** Constructor
     *  @param  item The item to be used for data storage. If NULL, the
     *          class creates an empty data container.
     *  @param  rules The rule set for this class. If NULL, the class creates
     *          one from scratch and adds its values.
     */
    IODPaletteColorLUTModule(OFshared_ptr<DcmItem> item, OFshared_ptr<IODRules> rules);

    /** Constructor
     */
    IODPaletteColorLUTModule();

    /** Destructor
     */
    virtual ~IODPaletteColorLUTModule();

    /** Clear all attributes from the data that are handled by this module.
     *  An attribute is considered belonging to the module if there are rules
     *  marked as belonging to this module via the rule's module name.
     */
    virtual void clearData();

    /** Resets rules to their original values
     */
    virtual void resetRules();

    /** Get name of module
     *  @return Name of the module ("PaletteColorLookupTableModule")
     */
    virtual OFString getName() const;

    /** Read attributes from given item into this class
     *  @param source  The source to read from
     *  @param clearOldData If OFTrue, old data is cleared before reading. Otherwise
     *         old data is overwritten (or amended)
     *  @result EC_Normal if reading was successful, error otherwise
     */
    virtual OFCondition read(DcmItem& source, const OFBool clearOldData = OFTrue);

    /** Write attributes from this class into given item
     *  @param  destination The item to write to
     *  @result EC_Normal if writing was successful, error otherwise
     */
    virtual OFCondition write(DcmItem& destination);

    // ---------------- Getters -----------------------------

    /** Returns the number of bits used per LUT data entry.
     *  If the number of bits is not the same for all LUTs, 0 is returned, indicating an error.
     *  @return The number of bits used per LUT data entry (8 or 16)
     */
    virtual Uint8 numBits();

    /** Get the Red Palette Color Lookup Table​ Descriptor
     *  @param  value The value of Red Palette Color Lookup Table​ Descriptor
     *  @param  pos The position of the value to be retrieved (0..2)
     *  @return EC_Normal if value is found, an error code otherwise
     */
    virtual OFCondition getRedPaletteColorLookupTableDescriptor(Uint16& value, const unsigned long pos = 0) const;

    /** Get the Green Palette Color Lookup Table​ Descriptor
     *  @param  value The value of Green Palette Color Lookup Table​ Descriptor
     *  @param  pos The position of the value to be retrieved (0..2)
     *  @return EC_Normal if value is found, an error code otherwise
     */
    virtual OFCondition getGreenPaletteColorLookupTableDescriptor(Uint16& value, const unsigned long pos = 0) const;

    /** Get the Blue Palette Color Lookup Table​ Descriptor
     *  @param  value The value of Blue Palette Color Lookup Table​ Descriptor
     *  @param  pos The position of the value to be retrieved (0..2)
     *  @return EC_Normal if value is found, an error code otherwise
     */
    virtual OFCondition getBluePaletteColorLookupTableDescriptor(Uint16& value, const unsigned long pos = 0) const;

    /** Get the Palette Color Lookup Table UID
     *  @param  value Reference to variable in which the value should be stored
     *  @param  pos Index of the value to get (0..vm-1), -1 for all components
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getPaletteColorLookupTableUID(OFString& value, const signed long pos = 0) const;

    /** Get the Red Palette Color Lookup Table​ Data​ (16 bit version)
     *  @param  dataCopy Reference to variable in which a copy of the value should be stored
     *  @param  numEntries Number of entries in the lookup table
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRedPaletteColorLookupTableData(const Uint16*& dataCopy, unsigned long& numEntries);

    /** Get the Red Palette Color Lookup Table​ Data​ (8 bit version)
     *  @param  dataCopy Reference to variable in which a copy of the value should be stored
     *  @param  numEntries Number of entries in the lookup table
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getRedPaletteColorLookupTableData(const Uint8*& dataCopy, unsigned long& numEntries);

    /** Get the Green Palette Color Lookup Table​ Data​ (16 bit version)
     *  @param  dataCopy Reference to variable in which a copy of the value should be stored
     *  @param  numEntries Number of entries in the lookup table
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual
    OFCondition getGreenPaletteColorLookupTableData(const Uint16*& dataCopy, unsigned long& numEntries);

    /** Get the Green Palette Color Lookup Table​ Data​ (8 bit version)
     *  @param  dataCopy Reference to variable in which a copy of the value should be stored
     *  @param  numEntries Number of entries in the lookup table
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual
    OFCondition getGreenPaletteColorLookupTableData(const Uint8*& dataCopy, unsigned long& numEntries);

    /** Get the Blue Palette Color Lookup Table​ Data​ (16 bit version)
     *  @param  dataCopy Reference to variable in which a copy of the value should be stored
     *  @param  numEntries Number of entries in the lookup table
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual
    OFCondition getBluePaletteColorLookupTableData(const Uint16*& dataCopy, unsigned long& numEntries);


    /** Get the Blue Palette Color Lookup Table​ Data​ (8 bit version)
     *  @param  dataCopy Reference to variable in which a copy of the value should be stored
     *  @param  numEntries Number of entries in the lookup table
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual
    OFCondition getBluePaletteColorLookupTableData(const Uint8*& dataCopy, unsigned long& numEntries);


    /** Get the Segmented Red Palette Color Lookup Table​ Data​
     *  @param  dataCopy Reference to variable in which a copy of the value should be stored
     *  @param  numEntries Number of entries in the lookup table
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSegmentedRedPaletteColorLookupTableData(const Uint16*& dataCopy, unsigned long& numEntries);

    /** Get the Segmented Red Palette Color Lookup Table​ Data​ (8 bit version)
     *  @param  dataCopy Reference to variable in which a copy of the value should be stored
     *  @param  numEntries Number of entries in the lookup table
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSegmentedRedPaletteColorLookupTableData(const Uint8*& dataCopy, unsigned long& numEntries);

    /** Get the Segmented Green Palette Color Lookup Table​ Data​ (16 bit version)
     *  @param  dataCopy Reference to variable in which a copy of the value should be stored
     *  @param  numEntries Number of entries in the lookup table
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSegmentedGreenPaletteColorLookupTableData(const Uint16*& dataCopy, unsigned long& numEntries);

    /** Get the Segmented Green Palette Color Lookup Table​ Data​ (8 bit version)
     *  @param  dataCopy Reference to variable in which a copy of the value should be stored
     *  @param  numEntries Number of entries in the lookup table
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSegmentedGreenPaletteColorLookupTableData(const Uint8*& dataCopy, unsigned long& numEntries);

    /** Get the Segmented Blue Palette Color Lookup Table​ Data​ (16 bit version)
     *  @param  dataCopy Reference to variable in which a copy of the value should be stored
     *  @param  numEntries Number of entries in the lookup table
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSegmentedBluePaletteColorLookupTableData(const Uint16*& dataCopy, unsigned long& numEntries);

    /** Get the Segmented Blue Palette Color Lookup Table​ Data​ (8 bit version)
     *  @param  dataCopy Reference to variable in which a copy of the value should be stored
     *  @param  numEntries Number of entries in the lookup table
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getSegmentedBluePaletteColorLookupTableData(const Uint8*& dataCopy, unsigned long& numEntries);

    // ---------------- Setters -----------------------------

    /** Set the Red Palette Color Lookup Table​ Descriptor
     *  @param  value The value of Red Palette Color Lookup Table​ Descriptor
     *  @param  pos The position of the value to be set (0..2)
     *  @return EC_Normal if value is set, an error code otherwise
     */
    virtual OFCondition setRedPaletteColorLookupTableDescriptor(const Uint16& value, const unsigned long pos = 0);

    /** Set the Green Palette Color Lookup Table​ Descriptor
     *  @param  value The value of Green Palette Color Lookup Table​ Descriptor
     *  @param  pos The position of the value to be set (0..2)
     *  @return EC_Normal if value is set, an error code otherwise
     */
    virtual OFCondition setGreenPaletteColorLookupTableDescriptor(const Uint16& value, const unsigned long pos = 0);

    /** Set the Blue Palette Color Lookup Table​ Descriptor
     *  @param  value The value of Blue Palette Color Lookup Table​ Descriptor
     *  @param  pos The position of the value to be set (0..2)
     *  @return EC_Normal if value is set, an error code otherwise
     */
    virtual OFCondition setBluePaletteColorLookupTableDescriptor(const Uint16& value, const unsigned long pos = 0);

    /** Set the Palette Color Lookup Table UID
     *  @param  value The value of Palette Color Lookup Table UID
     *  @param  checkValue Check value for conformance with VR (UI) and VM (1) if OFTrue
     *  @return EC_Normal if value is set, an error code otherwise
     */
    virtual OFCondition setPaletteColorLookupTableUID(const OFString& value, const OFBool checkValue = OFTrue);

    /** Set the Red Palette Color Lookup Table​ Data​ (16 bit version)
     *  @param  value The value of Red Palette Color Lookup Table​ Data​
     *  @param  numEntries Number of entries in the lookup table
     *  @param  checkValue Check value for conformance with VR (OW) and VM (1-n) if OFTrue
     *  @return EC_Normal if value is set, an error code otherwise
     */
    virtual OFCondition
    setRedPaletteColorLookupTableData(const Uint16* value, const unsigned long numEntries, const OFBool checkValue = OFTrue);

    /** Set the Red Palette Color Lookup Table​ Data​ (8 bit version)
     *  @param  value The value of Red Palette Color Lookup Table​ Data​
     *  @param  numEntries Number of entries in the lookup table
     *  @param  checkValue Check value for conformance with VR (OW) and VM (1-n) if OFTrue
     *  @return EC_Normal if value is set, an error code otherwise
     */
    virtual OFCondition
    setRedPaletteColorLookupTableData(const Uint8* value, const unsigned long numEntries, const OFBool checkValue = OFTrue);


    /** Set the Green Palette Color Lookup Table​ Data​ (16 bit version)
     *  @param  value The value of Red Palette Color Lookup Table​ Data​
     *  @param  numEntries Number of entries in the lookup table
     *  @param  checkValue Check value for conformance with VR (OW) and VM (1-n) if OFTrue
     *  @return EC_Normal if value is set, an error code otherwise
     */

    virtual OFCondition
    setGreenPaletteColorLookupTableData(const Uint16* value, const unsigned long numEntries, const OFBool checkValue = OFTrue);

    /** Set the Green Palette Color Lookup Table​ Data​ (8 bit version)
     *  @param  value The value of Red Palette Color Lookup Table​ Data​
     *  @param  numEntries Number of entries in the lookup table
     *  @param  checkValue Check value for conformance with VR (OW) and VM (1-n) if OFTrue
     *  @return EC_Normal if value is set, an error code otherwise
     */

    virtual OFCondition
    setGreenPaletteColorLookupTableData(const Uint8* value, const unsigned long numEntries, const OFBool checkValue = OFTrue);

    /** Set the Blue Palette Color Lookup Table​ Data​ (16 bit version)
     *  @param  value The value of Red Palette Color Lookup Table​ Data​
     *  @param  numEntries Number of entries in the lookup table
     *  @param  checkValue Check value for conformance with VR (OW) and VM (1-n) if OFTrue
     *  @return EC_Normal if value is set, an error code otherwise
     */
    virtual OFCondition
    setBluePaletteColorLookupTableData(const Uint16* value, const unsigned long numEntries, const OFBool checkValue = OFTrue);

    /** Set the Blue Palette Color Lookup Table​ Data​ (8 bit version).
     *  @param  value The value of Red Palette Color Lookup Table​ Data​
     *  @param  numEntries Number of entries in the lookup table
     *  @param  checkValue Check value for conformance with VR (OW) and VM (1-n) if OFTrue
     *  @return EC_Normal if value is set, an error code otherwise
     */
    virtual OFCondition
    setBluePaletteColorLookupTableData(const Uint8* value, const unsigned long numEntries, const OFBool checkValue = OFTrue);

    /** Set the Segmented Red Palette Color Lookup Table​ Data​ (16 bit version).
     *  @param  value The value of Red Palette Color Lookup Table​ Data​
     *  @param  numEntries Number of entries in the lookup table
     *  @param  checkValue Check value for conformance with VR (OW) and VM (1-n) if OFTrue
     *  @return EC_Normal if value is set, an error code otherwise
     */
    virtual OFCondition setSegmentedRedPaletteColorLookupTableData(const Uint16* value,
                                                           const unsigned long numEntries,
                                                           const OFBool checkValue = OFTrue);

    /** Set the Segmented Red Palette Color Lookup Table​ Data​ (8 bit version).
     *  @param  value The value of Red Palette Color Lookup Table​ Data​
     *  @param  numEntries Number of entries in the lookup table
     *  @param  checkValue Check value for conformance with VR (OW) and VM (1-n) if OFTrue
     *  @return EC_Normal if value is set, an error code otherwise
     */
    virtual OFCondition setSegmentedRedPaletteColorLookupTableData(const Uint8* value,
                                                           const unsigned long numEntries,
                                                           const OFBool checkValue = OFTrue);

    /** Set the Segmented Green Palette Color Lookup Table​ Data​ (16 bit version).
     *  @param  value The value of Red Palette Color Lookup Table​ Data​
     *  @param  numEntries Number of entries in the lookup table
     *  @param  checkValue Check value for conformance with VR (OW) and VM (1-n) if OFTrue
     *  @return EC_Normal if value is set, an error code otherwise
     */
    virtual OFCondition setSegmentedGreenPaletteColorLookupTableData(const Uint16* value,
                                                             const unsigned long numEntries,
                                                             const OFBool checkValue = OFTrue);

    /** Set the Segmented Green Palette Color Lookup Table​ Data​ (8 bit version)
     *  @param  value The value of Red Palette Color Lookup Table​ Data​
     *  @param  numEntries Number of entries in the lookup table
     *  @param  checkValue Check value for conformance with VR (OW) and VM (1-n) if OFTrue
     *  @return EC_Normal if value is set, an error code otherwise
     */
    virtual OFCondition setSegmentedGreenPaletteColorLookupTableData(const Uint8* value,
                                                             const unsigned long numEntries,
                                                             const OFBool checkValue = OFTrue);

    /** Set the Segmented Blue Palette Color Lookup Table​ Data​ (16 bit version).
     *  @param  value The value of Red Palette Color Lookup Table​ Data​
     *  @param  numEntries Number of entries in the lookup table
     *  @param  checkValue Check value for conformance with VR (OW) and VM (1-n) if OFTrue
     *  @return EC_Normal if value is set, an error code otherwise
     */
    virtual OFCondition setSegmentedBluePaletteColorLookupTableData(const Uint16* value,
                                                            const unsigned long numEntries,
                                                            const OFBool checkValue = OFTrue);

    /** Set the Segmented Blue Palette Color Lookup Table​ Data​ (8 bit version).
     *  @param  value The value of Red Palette Color Lookup Table​ Data​
     *  @param  numEntries Number of entries in the lookup table
     *  @param  checkValue Check value for conformance with VR (OW) and VM (1-n) if OFTrue
     *  @return EC_Normal if value is set, an error code otherwise
     */
    virtual OFCondition setSegmentedBluePaletteColorLookupTableData(const Uint8* value,
                                                            const unsigned long numEntries,
                                                            const OFBool checkValue = OFTrue);

    // ---------------- Convenience Setters -----------------------------

    /** Set the Palette Color Lookup Table​ Data​ (red, green, blue, 8 or 16 bit).
     *  @tparam T The data type of the color lookup table data. (Uint16 or Uint8)
     *  @param  copyRed The value of Red Palette Color Lookup Table​ Data​
     *  @param  copyGreen The value of Green Palette Color Lookup Table​ Data​
     *  @param  copyBlue The value of Blue Palette Color Lookup Table​ Data​
     *  @param  numEntries Number of entries in each of the lookup tables
     *  @param  checkValue Check value for conformance with VR (OW) and VM (1-n) if OFTrue
     *  @return EC_Normal if value is set, an error code otherwise
     */
    template <typename T> OFCondition setPaletteColorLookupTableData(const T* copyRed,
                                               const T* copyGreen,
                                               const T* copyBlue,
                                               const unsigned long numEntries,
                                               const OFBool checkValue = OFTrue);


    /** Sets the red, green, and blue data for a segmented palette color lookup table.
     *  The data arrays for red, green, and blue should be of type T and have a length of numEntries.
     *  \tparam T The data type of the color lookup table data. (Uint16 or Uint8)
     *  \param copyRedData Pointer to the array containing the red data.
     *  \param copyGreenData Pointer to the array containing the green data.
     *  \param copyBlueData Pointer to the array containing the blue data.
     *  \param numEntries The number of entries in the lookup table.
     *  \param checkValue Flag indicating whether to check the input values for validity. Default is true.
     *  \return EC_Normal if successful, error otherwise.
     */
    template <typename T> OFCondition setSegmentedPaletteColorLookupTableData(const T* copyRedData,
                                                        const T* copyGreenData,
                                                        const T* copyBlueData,
                                                        const unsigned long numEntries,
                                                        const OFBool checkValue = OFTrue);

    /** Sets all three values of the Red Palette Color Lookup Table Descriptor.
     *  @param numEntries The number of entries in the lookup table.
     *  @param firstValueMapped The first value mapped in the lookup table.
     *  @param numBitsPerEntry The number of bits per entry in the lookup table.
     *
     *  @return EC_Normal if value is set, an error code otherwise
     */
     virtual OFCondition setRedPaletteColorLookupTableDescriptor(const Uint16 numEntries,
                                                        const Uint16 firstValueMapped,
                                                        const Uint8 numBitsPerEntry);

    /** Sets all three values of the Green Palette Color Lookup Table Descriptor.
     *  @param numEntries The number of entries in the lookup table.
     *  @param firstValueMapped The first value mapped in the lookup table.
     *  @param numBitsPerEntry The number of bits per entry in the lookup table.
     *
     *  @return EC_Normal if value is set, an error code otherwise
     */
    virtual OFCondition setGreenPaletteColorLookupTableDescriptor(const Uint16 numEntries,
                                                          const Uint16 firstValueMapped,
                                                          const Uint8 numBitsPerEntry);

    /** Sets the blue Palette Color Lookup Table Descriptor.
     *  @param numEntries The number of entries in the lookup table.
     *  @param firstValueMapped The first value mapped in the lookup table.
     *  @param numBitsPerEntry The number of bits per entry in the lookup table.
     *
     *  @return EC_Normal if value is set, an error code otherwise
     */
    virtual OFCondition setBluePaletteColorLookupTableDescriptor(const Uint16 numEntries,
                                                         const Uint16 firstValueMapped,
                                                         const Uint8 numBitsPerEntry);

protected:

    /** Returns a copy of the data for the given tag. A copy of the data is returned into the given
     *  pointer and the number of entries is returned.
     *  @param dataTag The tag of the data to be copied (i.e. Red/Green/Blue/PaletteColorLookupTableData)
     *  @param lutData The pointer that will be set to the copied data
     *  @param num8BitEntries The number of entries in the lookup table
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getUint8DataCopy(const DcmTagKey& dataTag, const Uint8*& lutData, unsigned long& num8BitEntries);

    /** Put the given 8 bit data into the given tag.
     *  @param dataTag The tag of the data to be copied into (i.e. Red/Green/Blue/PaletteColorLookupTableData)
     *  @param lutData The data to be copied
     *  @param num8BitEntries The number of entries in the lookup table
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putUint8Data(const DcmTagKey& dataTag, const Uint8* lutData, const unsigned long num8BitEntries);

    /** Returns a 16 bit copy of the data for the given tag. The data is returned into the given
     *  pointer and the number of entries is returned.
     *  @param dataTag The tag of the data to be copied to (i.e. Red/Green/Blue/PaletteColorLookupTableData)
     *  @param lutData The pointer that will be set to the copied data
     *  @param numEntries The number of entries in the data
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getUint16DataCopy(const DcmTagKey& dataTag, const Uint16*& lutData, unsigned long& numEntries);

    /** Returns a reference of the data from the given tag and the number of entries in the data
     *  @param dataTag The tag of the data to be copied to (i.e. Red/Green/Blue/PaletteColorLookupTableData)
     *  @param lutData The pointer that will be set to the data
     *  @param numEntries The number of entries in the data
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getUint16Data(const DcmTagKey& dataTag, const Uint16*& lutData, unsigned long& numEntries);

    /** Check given LUT for consistency with descriptor.
     *  @param descriptorTag The tag of the descriptor to be checked (i.e. Red/Green/BluePaletteColorLookupTableDescriptor)
     *  @param dataTag The tag of the data to be checked (i.e. Red/Green/BluePaletteColorLookupTableData).
     *         Should of course match the descriptor "color".
     *   @return OFTrue if LUT is consistent, OFFalse otherwise
     */
    virtual OFBool checkLUT(const DcmTagKey& descriptorTag,
                            const DcmTagKey& dataTag);

    /** Check basic consistency rules of all palette descriptors (not their data).
     *  @param isError If OFTrue, errors are reported, otherwise warnings
     *  @return OFTrue if all descriptors are consistent, OFFalse otherwise
     */
    virtual OFBool checkDescriptorConsistency(const OFBool& isError);

    /** Check basic consistency of all palette data with their descriptors.
     *  @param isError If OFTrue, errors are reported, otherwise warnings
     *  @return OFTrue if all data is consistent, OFFalse otherwise
     */
    virtual OFBool checkDataConsistency(const OFBool& isError);

    /** Check that either segmented or unsegmented LUT data is used, not both.
     *  @param isError If OFTrue, errors are reported, otherwise warnings
     *  @param isSegmented Returns OFTrue if segmented LUT data is used, OFFalse otherwise
     *  @return OFTrue if LUT data is consistent, OFFalse otherwise
     */
    virtual OFBool checkSegmentConsistency(const OFBool& isError, OFBool& isSegmented);

    /** Report error/warning for given descriptor tag.
     *  @param descriptorTag The tag of the descriptor to be reported (i.e. Red/Green/BluePaletteColorLookupTableDescriptor)
     *  @param message The message to be reported
     *  @param isError If OFTrue, an error is reported, otherwise a warning
     */
    virtual void reportLUTError(const DcmTagKey& descriptorTag,
                        const OFString& message,
                        const OFBool& isError);

    /** Get number of LUT data entries for given descriptor tag.
     *  @param dataTag The tag of the data to be checked (i.e. Red/Green/BluePaletteColorLookupTableDescriptor)
     *  @param result The number of entries in the data
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition numEntriesForData(const DcmTagKey& dataTag, Uint16& result);

private:

    /// The module's name ("PaletteColorLookupTableModule")
    static const OFString m_ModuleName;

};

#endif // MODPALETTECOLORLUT_H
