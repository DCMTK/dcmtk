/*
 *
 *  Copyright (C) 1994-2005, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers
 *
 *  Purpose:
 *  This file contains the interface to routines which provide
 *  DICOM object encoding/decoding, search and lookup facilities.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2005-12-02 08:49:17 $
 *  CVS/RCS Revision: $Revision: 1.40 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#ifndef DCOBJECT_H
#define DCOBJECT_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofconsol.h"
#include "ofglobal.h"
#include "dcerror.h"
#include "dctypes.h"
#include "dcxfer.h"
#include "dctag.h"
#include "dclist.h"
#include "dcstack.h"


// forward declarations
class DcmOutputStream;
class DcmInputStream;


// Undefined Length Identifier now defined in dctypes.h

// Maxinum number of read bytes for a Value Element
const Uint32 DCM_MaxReadLength = 4096;

// Maximum length of tag and length in a DICOM element
const Uint32 DCM_TagInfoLength = 12;

// Optimum line length if not all data printed
const Uint32 DCM_OptPrintLineLength = 70;

// Optimum value length if not all data printed
const Uint32 DCM_OptPrintValueLength = 40;


/** This flags defines whether automatic correction should be applied to input
 *  data (e.g. stripping of padding blanks, removal of blanks in UIDs, etc).
 *  Default is enabled.
 */
extern OFGlobal<OFBool> dcmEnableAutomaticInputDataCorrection; /* default OFTrue */

/** This flag defines the handling of illegal odd-length attributes: If flag is
 *  true, odd lengths are respected (i.e. an odd number of bytes is read from
 *  the input stream.) After successful reading, padding to even number of bytes
 *  is enforced by adding a zero pad byte if dcmEnableAutomaticInputDataCorrection
 *  is true. Otherwise the odd number of bytes remains as read.
 *
 *  If flag is false, old (pre DCMTK 3.5.2) behaviour applies: The length field
 *  implicitly incremented and an even number of bytes is read from the stream.
 */
extern OFGlobal<OFBool> dcmAcceptOddAttributeLength; /* default OFTrue */

/** This flag defines how UN attributes with undefined length are treated
 *  by the parser when reading. The default is to expect the content of the
 *  UN element (up to and including the sequence delimitation item)
 *  to be encoded in Implicit VR Little Endian, as described in CP 246.
 *  DCMTK expects the attribute to be encoded like a DICOM sequence, i.e.
 *  the content of each item is parsed as a DICOM dataset.
 *  If the flag is disabled old (pre DCMTK 3.5.4) behaviour applies: The
 *  attribute is treated as if it was an Explicit VR SQ element.
 *
 *  Note that the flag only affects the read behaviour but not the write
 *  behaviour - DCMTK will never write UN elements with undefined length.
 */
extern OFGlobal<OFBool> dcmEnableCP246Support; /* default OFTrue */

/** DCMTK releases up to 3.5.3 created a non-conforming byte stream
 *  as input to the MAC algorithm when creating or verifying digital signatures
 *  including compressed pixel data (i.e. signatures including attribute
 *  (7FE0,0010) in an encapsulated transfer syntax). This has been fixed
 *  in DCMTK 3.5.4, but this flag allows to revert to the old behavior
 *  in order to create or verify signatures that are compatible with older
 *  releases. Default is "off" (OFFalse).
 */
extern OFGlobal<OFBool> dcmEnableOldSignatureFormat; /* default OFFalse */

/** This flag defines whether the transfer syntax for uncompressed datasets
 *  is detected automatically.  The automatic detection has been introduced
 *  since there are (incorrectly encoded) DICOM dataset stored with a
 *  different transfer syntax than specified in the meta header.
 */
extern OFGlobal<OFBool> dcmAutoDetectDatasetXfer; /* default OFFalse */

/** base class for all DICOM objects defined in 'dcmdata'
 */
class DcmObject
{

  public:

    /** constructor.
     *  Create new object from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmObject(const DcmTag &tag,
              const Uint32 len = 0);

    /** copy constructor
     *  @param obj item to be copied
     */
    DcmObject(const DcmObject &obj);

    /** destructor
     */
    virtual ~DcmObject();

    /** clone method
     *  @return deep copy of this object
     */
    virtual DcmObject *clone() const = 0;

    /** assignment operator
     *  @param obj object to be assigned/copied
     *  @return reference to this object
     */
    DcmObject &operator=(const DcmObject &obj);

    /** get type identifier (abstract)
     *  @return type identifier of this class
     */
    virtual DcmEVR ident() const = 0;

    // current value representation. If object was read from a stream
    // getVR returns the read value representation. It is possible that
    // this vr is not the same as mentioned in the data dictionary
    // (e.g. private tags, encapsulated data ...)
    inline DcmEVR getVR() const { return Tag.getEVR(); }

    inline OFBool isaString() const { return Tag.getVR().isaString(); }

    virtual OFBool isLeaf() const = 0;

    /** print object to a stream
     *  @param out output stream
     *  @param flags optional flag used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     *  @param pixelFileName not used
     *  @param pixelCounter not used
     */
    virtual void print(ostream &out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL) = 0;

    inline OFCondition error() const { return errorFlag; }

    inline E_TransferState transferState() const { return fTransferState; }
    virtual void transferInit(void);
    virtual void transferEnd(void);

    inline Uint16 getGTag() const { return Tag.getGTag(); }
    inline Uint16 getETag() const { return Tag.getETag(); }
    inline const DcmTag &getTag() const { return Tag; }
    inline void setGTag(Uint16 gtag) { Tag.setGroup(gtag); }

    virtual OFCondition setVR(DcmEVR /*vr*/) { return EC_IllegalCall; }
    virtual unsigned long getVM() = 0;

    // calculate length of Dicom element
    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype) = 0;

    // returns value length
    virtual Uint32 getLength(const E_TransferSyntax xfer = EXS_LittleEndianImplicit,
                             const E_EncodingType enctype = EET_UndefinedLength) = 0;

    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
                                const E_TransferSyntax oldXfer) = 0;

    virtual OFCondition read(DcmInputStream &inStream,
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength) = 0;

    /** write object to a stream (abstract)
     *  @param outStream DICOM output stream
     *  @param oxfer output transfer syntax
     *  @param enctype encoding types (undefined or explicit length)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmOutputStream &outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype = EET_UndefinedLength) = 0;

    /** write object in XML format to a stream
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, always returns EC_Illegal Call
     */
    virtual OFCondition writeXML(ostream &out,
                                 const size_t flags = 0);

    /** special write method for creation of digital signatures (abstract)
     *  @param outStream DICOM output stream
     *  @param oxfer output transfer syntax
     *  @param enctype encoding types (undefined or explicit length)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeSignatureFormat(DcmOutputStream &outStream,
					                         const E_TransferSyntax oxfer,
					                         const E_EncodingType enctype = EET_UndefinedLength) = 0;

    /** returns true if the current object may be included in a digital signature
     *  @return true if signable, false otherwise
     */
    virtual OFBool isSignable() const;

    /** returns true if the object contains an element with Unknown VR at any nesting level
     *  @return true if the object contains an element with Unknown VR, false otherwise
     */
    virtual OFBool containsUnknownVR() const;

    virtual OFCondition clear() = 0;
    virtual OFCondition verify(const OFBool autocorrect = OFFalse) = 0;

    virtual DcmObject *nextInContainer(const DcmObject *obj);

    virtual OFCondition nextObject(DcmStack &stack,
                                   const OFBool intoSub);

    virtual OFCondition search(const DcmTagKey &xtag,             // in
                               DcmStack &resultStack,             // inout
                               E_SearchMode mode = ESM_fromHere,  // in
                               OFBool searchIntoSub = OFTrue);    // in

    virtual OFCondition searchErrors(DcmStack &resultStack);      // inout

    virtual OFCondition loadAllDataIntoMemory() = 0;


 protected:

    /** print line indentation, e.g. a couple of spaces for each nesting level.
     *  Depending on the value of 'flags' other visualizations are also possible.
     *  @param out output stream
     *  @param flags used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     */
    void printNestingLevel(ostream &out,
                           const size_t flags,
                           const int level);

    /** print beginning of the info line.
     *  The default output is tag and value representation, though other
     *  visualizations are possible depending on the value of 'flags'.
     *  @param out output stream
     *  @param flags used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     *  @param tag optional tag used to print the data element information
     */
    void printInfoLineStart(ostream &out,
                            const size_t flags,
                            const int level,
                            DcmTag *tag = NULL);

    /** print end of the info line.
     *  The default output is length, value multiplicity and tag name, though
     *  other visualizations are possible depending on the value of 'flags'.
     *  @param out output stream
     *  @param flags used to customize the output (see DCMTypes::PF_xxx)
     *  @param printedLength number of characters printed after line start.
     *    Used for padding purposes.
     *  @param tag optional tag used to print the data element information
     */
    void printInfoLineEnd(ostream &out,
                          const size_t flags,
                          const unsigned long printedLength = 0xffffffff /*no padding*/,
                          DcmTag *tag = NULL);

    /** print given text with element information.
     *  Calls printInfoLineStart() and printInfoLineEnd() to frame the
     *  'info' text.
     *  @param out output stream
     *  @param flags used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     *  @param info text to be printed
     *  @param tag optional tag used to print the data element information
     */
    virtual void printInfoLine(ostream &out,
                               const size_t flags,
                               const int level = 0,
                               const char *info = NULL,
                               DcmTag *tag = NULL);

    static OFCondition writeTag(DcmOutputStream &outStream,
                                const DcmTag &tag,
                                const E_TransferSyntax oxfer); // in

    virtual OFCondition writeTagAndLength(DcmOutputStream &outStream,
                                          const E_TransferSyntax oxfer, // in
                                          Uint32 &writtenBytes) const;  // out

    /** return the number of bytes needed to serialize the
     *  tag, VR and length information of the current object using the given
     *  transfer syntax.
     *  @param oxfer The transfer syntax used for encoding
     *  @return number of bytes, may be 8 or 12 depending on VR and transfer syntax.
     */
    virtual Uint32 getTagAndLengthSize(const E_TransferSyntax oxfer) const;

    /* member variables */
    DcmTag Tag;
    Uint32 Length;
    E_TransferState fTransferState;
    OFCondition errorFlag;  // defined after fTransferState to workaround
                            // memory layout problem with Borland C++
    Uint32 fTransferredBytes;
 }; // class DcmObject


#endif // DCOBJECT_H


/*
 * CVS/RCS Log:
 * $Log: dcobject.h,v $
 * Revision 1.40  2005-12-02 08:49:17  joergr
 * Changed macro NO_XFER_DETECTION_FOR_DATASETS into a global option that can
 * be enabled and disabled at runtime.
 *
 * Revision 1.39  2005/11/24 12:50:57  meichel
 * Fixed bug in code that prepares a byte stream that is fed into the MAC
 *   algorithm when creating or verifying a digital signature. The previous
 *   implementation was non-conformant when signatures included compressed
 *   (encapsulated) pixel data because the item length was included in the byte
 *   stream, while it should not. The global variable dcmEnableOldSignatureFormat
 *   and a corresponding command line option in dcmsign allow to re-enable the old
 *   implementation.
 *
 * Revision 1.38  2005/05/10 15:27:14  meichel
 * Added support for reading UN elements with undefined length according
 *   to CP 246. The global flag dcmEnableCP246Support allows to revert to the
 *   prior behaviour in which UN elements with undefined length were parsed
 *   like a normal explicit VR SQ element.
 *
 * Revision 1.37  2004/07/01 12:28:25  meichel
 * Introduced virtual clone method for DcmObject and derived classes.
 *
 * Revision 1.36  2004/04/27 09:21:01  wilkens
 * Fixed a bug in dcelem.cc which occurs when one is serializing a dataset
 * (that contains an attribute whose length value is coded with 2 bytes) into
 * a given buffer. Although the number of available bytes in the buffer was
 * sufficient, the dataset->write(...) method would always return
 * EC_StreamNotifyClient to indicate that there are not sufficient bytes
 * available in the buffer. This code modification fixes the problem.
 *
 * Revision 1.35  2003/06/12 13:33:21  joergr
 * Fixed inconsistent API documentation reported by Doxygen.
 *
 * Revision 1.34  2002/12/06 12:49:11  joergr
 * Enhanced "print()" function by re-working the implementation and replacing
 * the boolean "showFullData" parameter by a more general integer flag.
 * Added doc++ documentation.
 * Made source code formatting more consistent with other modules/files.
 *
 * Revision 1.33  2002/08/27 16:55:35  meichel
 * Initial release of new DICOM I/O stream classes that add support for stream
 *   compression (deflated little endian explicit VR transfer syntax)
 *
 * Revision 1.32  2002/08/20 12:18:35  meichel
 * Changed parameter list of loadFile and saveFile methods in class
 *   DcmFileFormat. Removed loadFile and saveFile from class DcmObject.
 *
 * Revision 1.31  2002/07/08 14:45:20  meichel
 * Improved dcmdata behaviour when reading odd tag length. Depending on the
 *   global boolean flag dcmAcceptOddAttributeLength, the parser now either accepts
 *   odd length attributes or implements the old behaviour, i.e. assumes a real
 *   length larger by one.
 *
 * Revision 1.30  2002/04/25 09:38:47  joergr
 * Added support for XML output of DICOM objects.
 *
 * Revision 1.29  2002/04/11 12:23:46  joergr
 * Added new methods for loading and saving DICOM files.
 *
 * Revision 1.28  2001/11/16 15:54:39  meichel
 * Adapted digital signature code to final text of supplement 41.
 *
 * Revision 1.27  2001/09/25 17:19:27  meichel
 * Adapted dcmdata to class OFCondition
 *
 * Revision 1.26  2001/06/01 15:48:41  meichel
 * Updated copyright header
 *
 * Revision 1.25  2000/11/07 16:56:07  meichel
 * Initial release of dcmsign module for DICOM Digital Signatures
 *
 * Revision 1.24  2000/04/14 16:02:39  meichel
 * Global flag dcmEnableAutomaticInputDataCorrection now derived from OFGlobal
 *   and, thus, safe for use in multi-thread applications.
 *
 * Revision 1.23  2000/03/08 16:26:16  meichel
 * Updated copyright header.
 *
 * Revision 1.22  2000/03/03 14:05:24  meichel
 * Implemented library support for redirecting error messages into memory
 *   instead of printing them to stdout/stderr for GUI applications.
 *
 * Revision 1.21  2000/02/10 10:50:52  joergr
 * Added new feature to dcmdump (enhanced print method of dcmdata): write
 * pixel data/item value fields to raw files.
 *
 * Revision 1.20  2000/02/01 10:12:02  meichel
 * Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
 *   workaround for bug in compiler header files.
 *
 * Revision 1.19  1999/03/31 09:24:42  meichel
 * Updated copyright header in module dcmdata
 *
 *
 */
