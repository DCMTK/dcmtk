/*
 *
 *  Copyright (C) 1994-2019, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Gerd Ehlers
 *
 *  Purpose: class DcmItem
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#define INCLUDE_CCTYPE
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/dcmdata/dcitem.h"
#include "dcmtk/ofstd/ofdefine.h"     /* for memzero() */
#include "dcmtk/dcmdata/dcdeftag.h"   /* for name constants */
#include "dcmtk/dcmdata/dcistrma.h"   /* for class DcmInputStream */
#include "dcmtk/dcmdata/dcobject.h"
#include "dcmtk/dcmdata/dcostrma.h"   /* for class DcmOutputStream */
#include "dcmtk/dcmdata/dcovlay.h"
#include "dcmtk/dcmdata/dcpixel.h"
#include "dcmtk/dcmdata/dcsequen.h"
#include "dcmtk/dcmdata/dcswap.h"
#include "dcmtk/dcmdata/dcvr.h"
#include "dcmtk/dcmdata/dcvrae.h"
#include "dcmtk/dcmdata/dcvras.h"
#include "dcmtk/dcmdata/dcvrat.h"
#include "dcmtk/dcmdata/dcvrcs.h"
#include "dcmtk/dcmdata/dcvrda.h"
#include "dcmtk/dcmdata/dcvrds.h"
#include "dcmtk/dcmdata/dcvrdt.h"
#include "dcmtk/dcmdata/dcvrfd.h"
#include "dcmtk/dcmdata/dcvrfl.h"
#include "dcmtk/dcmdata/dcvris.h"
#include "dcmtk/dcmdata/dcvrlo.h"
#include "dcmtk/dcmdata/dcvrlt.h"
#include "dcmtk/dcmdata/dcvrobow.h"
#include "dcmtk/dcmdata/dcvrod.h"
#include "dcmtk/dcmdata/dcvrof.h"
#include "dcmtk/dcmdata/dcvrol.h"
#include "dcmtk/dcmdata/dcvrpn.h"
#include "dcmtk/dcmdata/dcvrsh.h"
#include "dcmtk/dcmdata/dcvrsl.h"
#include "dcmtk/dcmdata/dcvrss.h"
#include "dcmtk/dcmdata/dcvrst.h"
#include "dcmtk/dcmdata/dcvrtm.h"
#include "dcmtk/dcmdata/dcvruc.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmdata/dcvrul.h"
#include "dcmtk/dcmdata/dcvrulup.h"
#include "dcmtk/dcmdata/dcvrur.h"
#include "dcmtk/dcmdata/dcvrus.h"
#include "dcmtk/dcmdata/dcvrut.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/dcmdata/dcspchrs.h"   /* for class DcmSpecificCharacterSet */
#include "dcmtk/dcmdata/dcjson.h"

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/ofstd.h"

// ********************************


DcmItem::DcmItem()
  : DcmObject(DCM_ItemTag),
    elementList(NULL),
    lastElementComplete(OFTrue),
    fStartPosition(0),
    privateCreatorCache()
{
    elementList = new DcmList;
}


DcmItem::DcmItem(const DcmTag &tag,
                 const Uint32 len)
  : DcmObject(tag, len),
    elementList(NULL),
    lastElementComplete(OFTrue),
    fStartPosition(0),
    privateCreatorCache()
{
    elementList = new DcmList;
}


DcmItem::DcmItem(const DcmItem &old)
  : DcmObject(old),
    elementList(new DcmList),
    lastElementComplete(old.lastElementComplete),
    fStartPosition(old.fStartPosition),
    privateCreatorCache()
{
    if (!old.elementList->empty())
    {
        elementList->seek(ELP_first);
        old.elementList->seek(ELP_first);
        do
        {
            DcmObject *dO = old.elementList->get()->clone();
            elementList->insert(dO, ELP_next);
            // remember the parent
            dO->setParent(this);
        } while (old.elementList->seek(ELP_next));
    }
}


DcmItem& DcmItem::operator=(const DcmItem& obj)
{
    if (this != &obj)
    {
        // copy parent's member variables
        DcmObject::operator=(obj);

        // delete any existing elements
        elementList->deleteAllElements();

        // copy DcmItem's member variables
        lastElementComplete = obj.lastElementComplete;
        fStartPosition = obj.fStartPosition;
        if (!obj.elementList->empty())
        {
            elementList->seek(ELP_first);
            obj.elementList->seek(ELP_first);
            do
            {
                DcmObject *dO = obj.elementList->get()->clone();
                elementList->insert(dO, ELP_next);
                // remember the parent
                dO->setParent(this);
            } while (obj.elementList->seek(ELP_next));
        }
    }
    return *this;
}


int DcmItem::compare(const DcmItem& rhs) const
{
    if (this == &rhs)
        return 0;

    // cast away constness (dcmdata is not const correct...)
    DcmItem* myThis = NULL;
    DcmItem* myRhs = NULL;
    myThis = OFconst_cast(DcmItem*, this);
    myRhs =  OFconst_cast(DcmItem*, &rhs);

    // check length, i.e. number of elements in item
    unsigned long thisNumValues = myThis->card();
    unsigned long rhsNumValues = myRhs->card();
    if (thisNumValues < rhsNumValues)
    {
        return -1;
    }
    else if (thisNumValues > rhsNumValues)
    {
        return 1;
    }
    // iterate over all items and test equality
    for (unsigned long count = 0; count < thisNumValues; count++)
    {
        DcmElement* val = myThis->getElement(count);
        if (val)
        {
            DcmElement* rhsVal = myRhs->getElement(count);
            if (rhsVal)
            {
                int result = val->compare(*rhsVal);
                if (result != 0)
                {
                    return result;
                }
                // otherwise they are equal, continue comparison
            }
        }
    }

    // all values as well as VM equal: objects are equal
    return 0;
}


OFCondition DcmItem::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
      if (rhs.ident() != ident()) return EC_IllegalCall;
      *this = OFstatic_cast(const DcmItem &, rhs);
  }
  return EC_Normal;
}


DcmItem::~DcmItem()
{
    elementList->deleteAllElements();
    delete elementList;
}


// ********************************


OFBool DcmItem::foundVR(const Uint8* atposition)
{
    const Uint8 c1 = atposition[0];
    const Uint8 c2 = atposition[1];
    OFBool valid = OFFalse;

    if (isalpha(c1) && isalpha(c2))
    {
        Uint8 vrName[3];
        vrName[0] = c1;
        vrName[1] = c2;
        vrName[2] = '\0';

        /* is this VR name a standard VR descriptor */
        DcmVR vr( OFreinterpret_cast(const char*, &vrName[0] ) );
        valid = vr.isStandard();
    } else {
        /* cannot be a valid VR name since non-characters */
        valid = OFFalse;
    }
    return valid;
}


// ********************************


E_TransferSyntax DcmItem::checkTransferSyntax(DcmInputStream &inStream)
{
    E_TransferSyntax transferSyntax;
    Uint8 tagAndVR[6];

    /* we need 6 bytes, if there is less available we can't do much */
    if (inStream.avail() < 6)
        return EXS_LittleEndianExplicit;

    /* read 6 bytes from the input stream (try to read tag and VR (data type)) */
    inStream.mark();
    inStream.read(tagAndVR, 6);               // check tag & VR
    inStream.putback();

    /* create two tag variables (one for little, one for big */
    /* endian) in order to figure out, if there is a valid tag */
    const Uint8 c1 = tagAndVR[0];
    const Uint8 c2 = tagAndVR[1];
    const Uint8 c3 = tagAndVR[2];
    const Uint8 c4 = tagAndVR[3];
    const Uint16 t1 = OFstatic_cast(unsigned short, (c1 & 0xff) + ((c2 & 0xff) << 8));  // explicit little endian
    const Uint16 t2 = OFstatic_cast(unsigned short, (c3 & 0xff) + ((c4 & 0xff) << 8));  // conversion
    DcmTag taglittle(t1, t2);
    DcmTag tagbig(swapShort(t1), swapShort(t2));

    /* now we want to determine the transfer syntax which was used to code the information in the stream. */
    /* The decision is based on two questions: a) Did we encounter a valid tag? and b) Do the last 2 bytes */
    /* which were read from the stream represent a valid VR? In certain special cases, where the transfer */
    /* cannot be determined without doubt, we want to guess the most probable transfer syntax. */

    /* if both tag variables show an error, we encountered an invalid tag */
    if ((taglittle.error().bad()) && (tagbig.error().bad()))
    {
        /* in case we encountered an invalid tag, we want to assume that the used transfer syntax */
        /* is a little endian transfer syntax. Now we have to figure out, if it is an implicit or */
        /* explicit transfer syntax. Hence, check if the last 2 bytes represent a valid VR. */
        if (foundVR(&tagAndVR[4]))
        {
            /* if the last 2 bytes represent a valid VR, we assume that the used */
            /* transfer syntax is the little endian explicit transfer syntax. */
            transferSyntax = EXS_LittleEndianExplicit;
        } else {
            /* if the last 2 bytes did not represent a valid VR, we assume that the */
            /* used transfer syntax is the little endian implicit transfer syntax. */
            transferSyntax = EXS_LittleEndianImplicit;
        }
    }
    /* if at least one tag variable did not show an error, we encountered a valid tag */
    else
    {
        /* in case we encountered a valid tag, we want to figure out, if it is an implicit or */
        /* explicit transfer syntax. Hence, check if the last 2 bytes represent a valid VR. */
        if (foundVR(&tagAndVR[4]))
        {
            /* having figured out that the last 2 bytes represent a valid */
            /* VR, we need to find out which of the two tags was valid */
            if (taglittle.error().bad())
            {
                /* if the little endian tag was invalid, the transfer syntax is big endian explicit */
                transferSyntax = EXS_BigEndianExplicit;
            }
            else if (tagbig.error().bad())
            {
                /* if the big endian tag was invalid, the transfer syntax is little endian explicit */
                transferSyntax = EXS_LittleEndianExplicit;
            } else {
                /* if both tags were valid, we take a look at the group numbers. Since */
                /* group 0008 is much more probable than group 0800 for the first tag */
                /* we specify the following: */
                if ((taglittle.getGTag() > 0xff)&&(tagbig.getGTag() <= 0xff)) transferSyntax = EXS_BigEndianExplicit;
                else transferSyntax = EXS_LittleEndianExplicit;
            }
        } else {
            /* having figured out that the last 2 bytes do not represent a */
            /* valid VR, we need to find out which of the two tags was valid */
            if (taglittle.error().bad())
            {
                /* if the litte endian tag was invalid, the transfer syntax is big endian implicit */
                transferSyntax = EXS_BigEndianImplicit;
            }
            else if (tagbig.error().bad())
            {
                /* if the big endian tag was invalid, the transfer syntax is little endian implicit */
                transferSyntax = EXS_LittleEndianImplicit;
            } else {
                /* if both tags were valid, we take a look at the group numbers. Since */
                /* group 0008 is much more probable than group 0800 for the first tag */
                /* we specify the following: */
                if ((taglittle.getGTag() > 0xff)&&(tagbig.getGTag() <= 0xff)) transferSyntax = EXS_BigEndianImplicit;
                else transferSyntax = EXS_LittleEndianImplicit;
            }
        }
    }

    /* return determined transfer syntax */
    return transferSyntax;
}


// ********************************


void DcmItem::checkAndUpdateVR(DcmItem &item,
                               DcmTag &tag)
{
    /* handle special cases where the VR can be determined by some other element values */
    if (((tag == DCM_WaveformData) || (tag == DCM_WaveformPaddingValue) ||
        (tag == DCM_ChannelMinimumValue) || (tag == DCM_ChannelMaximumValue)) && (tag.getEVR() == EVR_ox))
    {
        /* case 1 (WaveformData and others): see section 8.3 in PS 3.5 */
        Uint16 bitsAlloc;
        if (item.findAndGetUint16(DCM_WaveformBitsAllocated, bitsAlloc).good())
        {
            if (bitsAlloc == 8)
            {
                DCMDATA_DEBUG("DcmItem::checkAndUpdateVR() setting undefined VR of "
                    << tag.getTagName() << " " << tag << " to 'OB' because WaveformBitsAllocated "
                    << DCM_WaveformBitsAllocated << " has a value of 8");
                tag.setVR(EVR_OB);
            } else {
                DCMDATA_DEBUG("DcmItem::checkAndUpdateVR() setting undefined VR of "
                    << tag.getTagName() << " " << tag << " to 'OW' because WaveformBitsAllocated "
                    << DCM_WaveformBitsAllocated << " has a value that is different from 8");
                tag.setVR(EVR_OW);
            }
        }
    }
    else if (((tag == DCM_PixelPaddingValue) || (tag == DCM_PixelPaddingRangeLimit) ||
        (tag == DCM_HistogramFirstBinValue) || (tag == DCM_HistogramLastBinValue) ||
        (tag == DCM_ZeroVelocityPixelValue) || (tag == DCM_SmallestImagePixelValue) ||
        (tag == DCM_LargestImagePixelValue) || (tag == DCM_SmallestPixelValueInSeries) ||
        (tag == DCM_LargestPixelValueInSeries) || (tag == DCM_LUTDescriptor) ||
        (tag == DCM_RealWorldValueLastValueMapped) || (tag == DCM_RealWorldValueFirstValueMapped)) &&
        (tag.getEVR() == EVR_xs))
    {
        /* case 2 (PixelPaddingValue and others): see section C.7.5.1, C.7.6.16.x and C.11.5 in PS 3.3 */
        Uint16 pixelRep;
        if (item.findAndGetUint16(DCM_PixelRepresentation, pixelRep).good())
        {
            if (pixelRep == 0x0001)
            {
                DCMDATA_DEBUG("DcmItem::checkAndUpdateVR() setting undefined VR of " << tag.getTagName()
                    << " " << tag << " to 'SS' because PixelRepresentation "
                    << DCM_PixelRepresentation << " has a value of 1");
                tag.setVR(EVR_SS);
            } else {
                DCMDATA_DEBUG("DcmItem::checkAndUpdateVR() setting undefined VR of " << tag.getTagName()
                    << " " << tag << " to 'US' because PixelRepresentation "
                    << DCM_PixelRepresentation << " has a value that is different from 1");
                tag.setVR(EVR_US);
            }
        }
    }
    else if (((tag.getBaseTag() == DCM_OverlayData) || (tag == DCM_PixelData)) && (tag.getEVR() == EVR_ox))
    {
        /* case 3 (OverlayData and PixelData): see section 8.1.2 and 8.2 in PS 3.5 */
        DCMDATA_DEBUG("DcmItem::checkAndUpdateVR() setting undefined VR of "
            << tag.getTagName() << " " << tag << " to 'OW'");
        tag.setVR(EVR_OW);
    }
    else if ((tag.getBaseTag() == DCM_RETIRED_CurveData) && (tag.getEVR() == EVR_ox))
    {
        /* case 4 (CurveData): see section A.1 in PS 3.5-2004 */
        DCMDATA_DEBUG("DcmItem::checkAndUpdateVR() setting undefined VR of "
            << tag.getTagName() << " " << tag << " to 'OB'");
        tag.setVR(EVR_OB);
    }
    /* currently unhandled:
     * - MappedPixelValue (0022,1452), US or SS
     * - RedPaletteColorLookupTableDescriptor (0028,1101), US or SS
     * - GreenPaletteColorLookupTableDescriptor (0028,1102), US or SS
     * - BluePaletteColorLookupTableDescriptor (0028,1103), US or SS
     * and some retired DICOM attributes as well as some DICONDE attributes
     */
}


// ********************************


DcmEVR DcmItem::ident() const
{
    return EVR_item;
}


unsigned long DcmItem::getVM()
{
    return 1;
}


unsigned long DcmItem::getNumberOfValues()
{
    return elementList->card();
}


unsigned long DcmItem::card() const
{
    return elementList->card();
}


// ********************************


void DcmItem::print(STD_NAMESPACE ostream &out,
                    const size_t flags,
                    const int level,
                    const char *pixelFileName,
                    size_t *pixelCounter)
{
    if (flags & DCMTypes::PF_showTreeStructure)
    {
        /* print item line with empty text */
        printInfoLine(out, flags, level);
        /* print item content */
        if (!elementList->empty())
        {
            DcmObject *dO;
            elementList->seek(ELP_first);
            do {
                dO = elementList->get();
                dO->print(out, flags, level + 1, pixelFileName, pixelCounter);
            } while (elementList->seek(ELP_next));
        }
    } else {
        /* print item start line */
        OFOStringStream oss;
        oss << "(Item with ";
        if (getLengthField() == DCM_UndefinedLength)
            oss << "undefined";
        else
            oss << "explicit";
        oss << " length #=" << card() << ")" << OFStringStream_ends;
        OFSTRINGSTREAM_GETSTR(oss, tmpString)
        printInfoLine(out, flags, level, tmpString);
        OFSTRINGSTREAM_FREESTR(tmpString)
        /* print item content */
        if (!elementList->empty())
        {
            DcmObject *dO;
            elementList->seek(ELP_first);
            do {
                dO = elementList->get();
                dO->print(out, flags, level + 1, pixelFileName, pixelCounter);
            } while (elementList->seek(ELP_next));
        }
        /* print item end line */
        DcmTag delimItemTag(DCM_ItemDelimitationItemTag);
        if (getLengthField() == DCM_UndefinedLength)
            printInfoLine(out, flags, level, "(ItemDelimitationItem)", &delimItemTag);
        else
            printInfoLine(out, flags, level, "(ItemDelimitationItem for re-encoding)", &delimItemTag);
    }
}


// ********************************


OFCondition DcmItem::writeXML(STD_NAMESPACE ostream &out,
                              const size_t flags)
{
    OFCondition l_error = EC_Normal;
    if (!(flags & DCMTypes::XF_useNativeModel))
    {
        /* XML start tag for "item" */
        out << "<item";
        /* cardinality (number of attributes) = 1..n */
        out << " card=\"" << card() << "\"";
        /* value length in bytes = 0..max (if not undefined) */
        if (getLengthField() != DCM_UndefinedLength)
            out << " len=\"" << getLengthField() << "\"";
        out << ">" << OFendl;
    }
    /* write item content */
    if (!elementList->empty())
    {
        /* write content of all children */
        DcmObject *dO;
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            l_error = dO->writeXML(out, flags);
        } while (l_error.good() && elementList->seek(ELP_next));
    }
    if (l_error.good())
    {
        if (!(flags & DCMTypes::XF_useNativeModel))
        {
            /* XML end tag for "item" */
            out << "</item>" << OFendl;
        }
    }
    return l_error;
}


// ********************************


OFCondition DcmItem::writeJson(STD_NAMESPACE ostream &out,
                               DcmJsonFormat &format)
{
    if (!elementList->empty())
    {
        // write content of all children
        out << "{" << format.newline();
        elementList->seek(ELP_first);
        OFCondition status = EC_Normal;
        status = elementList->get()->writeJson(out, format);
        while (status.good() && elementList->seek(ELP_next))
        {
            out << "," << format.newline();
            status = elementList->get()->writeJson(out, format);
        }
        out << format.newline() << format.indent() << "}";
        return status;
    }
    else
    {
        out << "{}" << format.newline();
    }
    return EC_Normal;
}

// ********************************


OFBool DcmItem::canWriteXfer(const E_TransferSyntax newXfer,
                             const E_TransferSyntax oldXfer)
{
    OFBool canWrite = OFTrue;
    if (newXfer == EXS_Unknown)
        canWrite = OFFalse;
    else if (!elementList->empty())
    {
        DcmObject *dO;
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            canWrite = dO->canWriteXfer(newXfer, oldXfer);
        } while (elementList->seek(ELP_next) && canWrite);
    }
    return canWrite;
}


// ********************************


Uint32 DcmItem::calcElementLength(const E_TransferSyntax xfer,
                                  const E_EncodingType enctype)
{
    Uint32 itemlen = 0;
    DcmXfer xferSyn(xfer);
    /* Length of item's start header */
    const Uint32 headersize = xferSyn.sizeofTagHeader(getVR());
    /* Length of item's content, i.e. contained elements */
    itemlen = getLength(xfer, enctype);
    /* Since the item's total length can exceed the maximum length of 32 bit, it is
     * always necessary to check for overflows. The approach taken is not elegant
     * but should work...
     */
    if ( (itemlen == DCM_UndefinedLength) || OFStandard::check32BitAddOverflow(itemlen, headersize) )
      return DCM_UndefinedLength;
    itemlen += headersize;
    if (enctype == EET_UndefinedLength) // add bytes for closing item tag marker if necessary
    {
      if (OFStandard::check32BitAddOverflow(itemlen, 8))
        return DCM_UndefinedLength;
      else
        itemlen += 8;
    }
    return itemlen;
}


// ********************************


Uint32 DcmItem::getLength(const E_TransferSyntax xfer,
                          const E_EncodingType enctype)
{
    Uint32 itemlen = 0;
    Uint32 sublen = 0;
    if (!elementList->empty())
    {
        DcmObject *dO;
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            sublen = dO->calcElementLength(xfer, enctype);
            /* explicit length: be sure that total size of contained elements fits into item's
               32 Bit length field. If not, switch encoding automatically to undefined
               length for this item. Nevertheless, any contained elements will be
               written with explicit length if possible.
             */
            if ( (enctype == EET_ExplicitLength) && OFStandard::check32BitAddOverflow(sublen, itemlen) )
            {
                if (dcmWriteOversizedSeqsAndItemsUndefined.get())
                {
                    DCMDATA_WARN("DcmItem: Explicit length of item exceeds 32-Bit length field, "
                        << "trying to encode with undefined length");
                }
                else
                {
                    DCMDATA_WARN("DcmItem: Explicit length of item exceeds 32-Bit length field, "
                        << "aborting write");
                    errorFlag = EC_SeqOrItemContentOverflow;
                }
                return DCM_UndefinedLength;
            }
            else
              itemlen += sublen;
        } while (elementList->seek(ELP_next));
    }
    return itemlen;
}


// ********************************


OFCondition DcmItem::computeGroupLengthAndPadding(const E_GrpLenEncoding glenc,
                                                  const E_PaddingEncoding padenc,
                                                  const E_TransferSyntax xfer,
                                                  const E_EncodingType enctype,
                                                  const Uint32 padlen,
                                                  const Uint32 subPadlen,
                                                  Uint32 instanceLength)
{
    /* if certain conditions are met, this is considered to be an illegal call. */
    if ((padenc == EPD_withPadding && (padlen % 2 || subPadlen % 2)) ||
        ((glenc == EGL_recalcGL || glenc == EGL_withGL ||
          padenc == EPD_withPadding) && xfer == EXS_Unknown))
        return EC_IllegalCall;

    /* if the caller specified that group length tags and padding */
    /* tags are not supposed to be changed, there is nothing to do. */
    if (glenc == EGL_noChange && padenc == EPD_noChange)
        return EC_Normal;

    /* if we get to this point, we need to do something. First of all, set the error indicator to normal. */
    OFCondition l_error = EC_Normal;
    /* collects group length elements that cannot be calculated due to length field overflows */
    OFList<DcmObject*> exceededGroupLengthElems;

    /* if there are elements in this item... */
    if (!elementList->empty())
    {
        /* initialize some variables */
        DcmObject *dO;
        OFBool beginning = OFTrue;
        Uint16 lastGrp = 0x0000;
        Uint16 actGrp;
        DcmUnsignedLong * actGLElem = NULL;
        DcmUnsignedLong * paddingGL = NULL;
        Uint32 grplen = 0;
        DcmXfer xferSyn(xfer);
        Uint32 sublen = 0;
        OFBool groupLengthExceeded = OFFalse;

        /* determine the current seek mode and set the list pointer to the first element */
        E_ListPos seekmode = ELP_next;
        elementList->seek(ELP_first);

        /* start a loop: we want to go through all elements as long as everything is okay */
        do
        {
            /* set the seek mode to "next" again, in case it has been modified in the last iteration */
            seekmode = ELP_next;

            /* get the current element and assign it to a local variable */
            dO = elementList->get();

            /* if the current element is a sequence, compute group length and padding for the sub sequence */
            if (dO->getVR() == EVR_SQ)
            {
                // add size of sequence header
                Uint32 templen = instanceLength + xferSyn.sizeofTagHeader(EVR_SQ);
                // call computeGroupLengthAndPadding for all contained items
                l_error = OFstatic_cast(DcmSequenceOfItems *, dO)->computeGroupLengthAndPadding
                    (glenc, padenc, xfer, enctype, subPadlen, subPadlen, templen);
            }

            /* if everything is ok so far */
            if (l_error.good())
            {
                /* in case one of the following two conditions is met */
                /*  (i) the caller specified that we want to add or remove group length elements and the current */
                /*      element's tag shows that it is a group length element (tag's element number equals 0x0000) */
                /*  (ii) the caller specified that we want to add or remove padding elements and the current */
                /*      element's tag shows that it is a padding element (tag is (0xfffc,0xfffc) */
                /* then we want to delete the current (group length or padding) element */
                if (((glenc ==  EGL_withGL || glenc == EGL_withoutGL) && dO->getETag() == 0x0000) ||
                    (padenc != EPD_noChange && dO->getTag() == DCM_DataSetTrailingPadding))
                {
                    delete elementList->remove();
                    seekmode = ELP_atpos; // remove advances 1 element forward -> make next seek() work
                    dO = NULL;
                }
                /* if the above mentioned conditions are not met but the caller specified that we want to add group */
                /* length tags for every group or that we want to recalculate values for existing group length tags */
                else  if (glenc == EGL_withGL || glenc == EGL_recalcGL)
                {
                    /* we need to determine the current element's group number */
                    actGrp = dO->getGTag();

                    /* and if the group number is different from the last remembered group number or */
                    /* if this id the very first element that is treated then we've found a new group */
                    if (actGrp != lastGrp || beginning) // new Group found
                    {
                        /* set beginning to false in order to specify that the */
                        /* very first element has already been treated */
                        beginning = OFFalse;

                        /* if the current element is a group length element and its data type */
                        /* is not UL replace this element with one that has a UL data type since */
                        /* group length elements are supposed to have this data type */
                        if (dO->getETag() == 0x0000 && dO->ident() != EVR_UL)
                        {
                            delete elementList->remove();
                            DcmTag tagUL(actGrp, 0x0000, EVR_UL);
                            DcmUnsignedLong *dUL = new DcmUnsignedLong(tagUL);
                            elementList->insert(dUL, ELP_prev);
                            dO = dUL;
                            // remember the parent
                            dO->setParent(this);
                            DCMDATA_WARN("DcmItem: Group Length with VR other than UL found, corrected");
                        }
                        /* if the above mentioned condition is not met but the caller specified */
                        /* that we want to add group length elements, we need to add such an element */
                        else if (glenc == EGL_withGL)
                        {
                            // Create GroupLength element
                            DcmTag tagUL(actGrp, 0x0000, EVR_UL);
                            DcmUnsignedLong *dUL = new DcmUnsignedLong(tagUL);
                            // insert new GroupLength element
                            elementList->insert(dUL, ELP_prev);
                            dO = dUL;
                            // remember the parent
                            dO->setParent(this);
                        }

                        /* in case we want to add padding elements and the current element is a */
                        /* padding element we want to remember the padding element so that the */
                        /* group length of this element can be stored later */
                        if (padenc == EPD_withPadding && actGrp == 0xfffc)
                            paddingGL = OFstatic_cast(DcmUnsignedLong *, dO);

                        /* if actGLElem contains a valid pointer it was set in one of the last iterations */
                        /* to the group length element of the last group. We need to write the current computed */
                        /* group length value to this element. Exception: If group length exceeds maximum possible */
                        /* value, than remove group length element instead of setting it */
                        if (actGLElem != NULL)
                        {
                            if (!groupLengthExceeded)
                            {
                                // do not use putUint32() in order to make sure that the resulting VM is really 1
                                actGLElem->putUint32Array(&grplen, 1);
                                DCMDATA_DEBUG("DcmItem::computeGroupLengthAndPadding() Length of Group 0x"
                                    << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
                                    << STD_NAMESPACE setw(4) << actGLElem->getGTag()
                                    << STD_NAMESPACE dec << STD_NAMESPACE setfill(' ')
                                    << " len=" << grplen);
                            }
                            else
                            {
                                DCMDATA_WARN("DcmItem: Group length of group 0x"
                                    << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
                                    << STD_NAMESPACE setw(4) << actGLElem->getGTag()
                                    << " exceeds 32-Bit length field. "
                                    << "Cannot calculate/write group length for this group.");
                                exceededGroupLengthElems.push_back(actGLElem);
                                groupLengthExceeded = OFFalse;
                            }
                        }

                        /* set the group length value to 0 since it is the beginning of the new group */
                        grplen = 0;

                        /* if the current element is a group length element, remember its address for later */
                        /* (we need to assign the group length value to this element in a subsequent iteration) */
                        /* in case the current element (at the beginning of the group) is not a group length */
                        /* element, set the actGLElem pointer to NULL. */
                        if (dO->getETag() == 0x0000)
                            actGLElem = OFstatic_cast(DcmUnsignedLong *, dO);
                        else
                            actGLElem = NULL;
                    }
                    /* if this is not a new group, calculate the element's length and add it */
                    /* to the currently computed group length value. If group length is larger */
                    /* than group length field permits, set flag to not add group length for this group */
                    else
                    {
                        sublen = dO->calcElementLength(xfer, enctype);
                        // test for 32-bit overflow return value
                        if ((sublen == DCM_UndefinedLength) || OFStandard::check32BitAddOverflow(sublen, grplen))
                        {
                            groupLengthExceeded = OFTrue;
                        }
                        else
                        {
                          grplen += sublen;
                        }
                    }

                    /* remember the current element's group number so that it is possible to */
                    /* figure out if a new group is treated in the following iteration */
                    lastGrp = actGrp;
                }
            }
        } while (l_error.good() && elementList->seek(seekmode));

        /* if there was no error and the caller specified that we want to add or recalculate */
        /* group length tags and if actGLElem has a valid value, we need to add the above */
        /* computed group length value to the last group's group length element. Exception: */
        /* If group length exceeds maximum possible value, remove group length element and */
        /* i.e. do not write it for this group. */
        if (l_error.good() && (glenc == EGL_withGL || glenc == EGL_recalcGL) && actGLElem)
        {
          if (groupLengthExceeded)
          {
            exceededGroupLengthElems.push_back(actGLElem);
          }
          else
          {
            actGLElem->putUint32(grplen);
          }
        }

        /* if the caller specified that we want to add padding elements and */
        /* if the length up to which shall be padded does not equal 0 we might */
        /* have to add a padding element */
        if (padenc == EPD_withPadding && padlen)
        {
            /* calculate how much space the entire padding element is supposed to occupy */
            Uint32 padding;
            if (ident() == EVR_dataset)
            {
                instanceLength += calcElementLength(xfer, enctype);
                padding = padlen - (instanceLength % padlen);
            } else
                padding = padlen - (getLength(xfer, enctype) % padlen);

            /* if now padding does not equal padlen we need to create a padding element. (if both values are equal */
            /* the element does have the exact required padlen length and does not need a padding element.) */
            if (padding != padlen)
            {
                /* Create new padding element */
                DcmOtherByteOtherWord * paddingEl = new DcmOtherByteOtherWord(DCM_DataSetTrailingPadding);

                /* calculate the length of the new element */
                Uint32 tmplen = paddingEl->calcElementLength(xfer, enctype);

                /* in case padding is smaller than the header of the padding element, we */
                /* need to increase padding (the value which specifies how much space the */
                /* entire padding element is supposed to occupy) until it is no longer smaller */
                while (tmplen > padding)
                    padding += padlen;

                /* determine the amount of bytes that have to be added to the */
                /* padding element so that it has the correct size */
                padding -= tmplen;

                /* create an array of a corresponding size and set the array fields */
                Uint8 * padBytes = new Uint8[padding];
                memzero(padBytes, size_t(padding));

                /* set information in the above created padding element (size and actual value) */
                paddingEl->putUint8Array(padBytes, padding);

                /* delete the above created array */
                delete[] padBytes;

                /* insert the padding element into this */
                insert(paddingEl);

                /* finally we need to update the group length for the padding element if it exists */
                if (paddingGL)
                {
                    Uint32 len;
                    paddingGL->getUint32(len);
                    len += paddingEl->calcElementLength(xfer, enctype);
                    paddingGL->putUint32(len);
                }
            }
        }
    }
    /* delete invalid group length elements from item. Cannot be done in */
    /* above while loop because then elementList iterator is invalidated */
    const size_t numElems = exceededGroupLengthElems.size();
    for (size_t i = 0; i < numElems; i++)
    {
        delete remove(exceededGroupLengthElems.front());
        exceededGroupLengthElems.pop_front();
    }

    return l_error;
}


// ********************************


OFCondition DcmItem::readTagAndLength(DcmInputStream &inStream,
                                      const E_TransferSyntax xfer,
                                      DcmTag &tag,
                                      Uint32 &length,
                                      Uint32 &bytesRead)
{
    OFCondition l_error = EC_Normal;
    Uint32 valueLength = 0;
    Uint16 groupTag = 0xffff;
    Uint16 elementTag = 0xffff;

    /* create a DcmXfer object based on the transfer syntax which was passed */
    DcmXfer xferSyn(xfer);

#ifdef DEBUG
    /* dump some information if required */
    DCMDATA_TRACE("DcmItem::readTagAndLength() TransferSyntax=\"" << xferSyn.getXferName() << "\"");
#endif

    /* bail out if at end of stream */
    if (inStream.eos())
        return EC_EndOfStream;

    /* check if either 4 (for implicit transfer syntaxes) or 6 (for explicit transfer */
    /* syntaxes) bytes are available in (i.e. can be read from) inStream. if an error */
    /* occurred while performing this check return this error */
    if (inStream.avail() < OFstatic_cast(offile_off_t, xferSyn.isExplicitVR() ? 6 : 4))
        return EC_StreamNotifyClient;

    /* determine the byte ordering of the transfer syntax which was passed; */
    /* if the byte ordering is unknown, this is an illegal call. */
    const E_ByteOrder byteOrder = xferSyn.getByteOrder();
    if (byteOrder == EBO_unknown)
        return EC_IllegalCall;

    /* read tag information (4 bytes) from inStream and create a corresponding DcmTag object */
    inStream.mark();
    inStream.read(&groupTag, 2);
    inStream.read(&elementTag, 2);
    swapIfNecessary(gLocalByteOrder, byteOrder, &groupTag, 2, 2);
    swapIfNecessary(gLocalByteOrder, byteOrder, &elementTag, 2, 2);
    // tag has been read
    bytesRead = 4;
    DcmTag newTag(groupTag, elementTag);
    DcmEVR newEVR = newTag.getEVR();
    // check whether tag is private
    OFBool isPrivate = groupTag & 1;

    /* if the transfer syntax which was passed is an explicit VR syntax and if the current */
    /* item is not a delimitation item (note that delimitation items do not have a VR), go */
    /* ahead and read 2 bytes from inStream. These 2 bytes contain this item's VR value. */
    if (xferSyn.isExplicitVR() && (newEVR != EVR_na))
    {
        char vrstr[3];
        vrstr[2] = '\0';

        /* read 2 bytes */
        inStream.read(vrstr, 2);

        /* create a corresponding DcmVR object */
        DcmVR vr(vrstr);

        /* if the VR which was read is not a standard VR, print a warning */
        if (!vr.isStandard())
        {
            OFOStringStream oss;
            oss << "DcmItem: Non-standard VR '"
                << ((OFstatic_cast(unsigned char, vrstr[0]) < 32) ? ' ' : vrstr[0])
                << ((OFstatic_cast(unsigned char, vrstr[1]) < 32) ? ' ' : vrstr[1]) << "' ("
                << STD_NAMESPACE hex << STD_NAMESPACE setfill('0')
                << STD_NAMESPACE setw(2) << OFstatic_cast(unsigned int, vrstr[0] & 0xff) << "\\"
                << STD_NAMESPACE setw(2) << OFstatic_cast(unsigned int, vrstr[1] & 0xff)
                << ") encountered while parsing element " << newTag << OFStringStream_ends;
            OFSTRINGSTREAM_GETSTR(oss, tmpString)
            /* encoding of this data element might be wrong, try to correct it */
            if (dcmAcceptUnexpectedImplicitEncoding.get())
            {
                DCMDATA_WARN(tmpString << ", trying again with Implicit VR Little Endian");
                /* put back read bytes to input stream ... */
                inStream.putback();
                bytesRead = 0;
                /* ... and retry with Implicit VR Little Endian transfer syntax */
                return readTagAndLength(inStream, EXS_LittleEndianImplicit, tag, length, bytesRead);
            } else {
                DCMDATA_WARN(tmpString << ", assuming " << (vr.usesExtendedLengthEncoding() ? "4" : "2")
                    << " byte length field");
            }
            OFSTRINGSTREAM_FREESTR(tmpString)

            /* workaround: handle known issue with non-standard VR "OX" for PixelData element */
            if ((newTag == DCM_PixelData) && (strncmp(vrstr, "OX", 2) == 0))
            {
                DCMDATA_WARN("DcmItem: Non-standard VR 'OX' is known to be wrongly used for PixelData " << newTag
                    << ", setting VR to 'OW'");
                vr.setVR(EVR_OW);
            }
        }

        /* the VR in the dataset might be wrong, so the user can decide to ignore it */
        if (dcmPreferVRFromDataDictionary.get() && (newEVR != EVR_UNKNOWN) && (newEVR != EVR_UNKNOWN2B))
        {
            if (newEVR != vr.getEVR())
            {
                /* ignore explicit VR in dataset if tag is defined in data dictionary */
                DCMDATA_DEBUG("DcmItem::readTagAndLength() ignoring explicit VR in data set ("
                    << vr.getVRName() << ") for element " << newTag
                    << ", using the one from data dictionary (" << newTag.getVRName() << ")");
            }
        } else {
            /* set the VR which was read in the above created tag object */
            newTag.setVR(vr);
        }

        if (!dcmPreferLengthFieldSizeFromDataDictionary.get() || newEVR == EVR_UNKNOWN || newEVR == EVR_UNKNOWN2B) {
            /* determine VR read from dataset, because this VR specifies the number of bytes for the length-field */
            newEVR = vr.getEVR();
        } else {
            /* use the VR from the dictionary for deciding the length of the length field */
        }

        /* increase counter by 2 */
        bytesRead += 2;
    }

    /* special handling for private elements */
    if (isPrivate && (newTag.getElement() >= 0x1000))
    {
        const char *pc = privateCreatorCache.findPrivateCreator(newTag);
        if (pc)
        {
            // we have a private creator for this element
            newTag.setPrivateCreator(pc);

            if (xferSyn.isImplicitVR())
            {
                // try to update VR from dictionary now that private creator is known
                newTag.lookupVRinDictionary();
                // also update the VR
                newEVR = newTag.getEVR();
            }
        }
    }

    /* the next thing we want to do is read the value in the length field from inStream. */
    /* determine if there is a corresponding amount of bytes (for the length field) still */
    /* available in inStream. If not, return an error. */
    if (inStream.avail() < OFstatic_cast(offile_off_t, xferSyn.sizeofTagHeader(newEVR) - bytesRead))
    {
        inStream.putback();    // the UnsetPutbackMark is in readSubElement
        bytesRead = 0;
        l_error = EC_StreamNotifyClient;
        return l_error;
    }

    /* read the value in the length field. In some cases, it is 4 bytes wide, in other */
    /* cases only 2 bytes (see DICOM standard part 5, section 7.1.1) */
    if (xferSyn.isImplicitVR() || newEVR == EVR_na) // note that delimitation items don't have a VR
    {
        inStream.read(&valueLength, 4);            // length field is 4 bytes wide
        swapIfNecessary(gLocalByteOrder, byteOrder, &valueLength, 4, 4);
        bytesRead += 4;
    } else {                                       // the transfer syntax is explicit VR
        DcmVR vr(newEVR);
        if (vr.usesExtendedLengthEncoding())
        {
            Uint16 reserved;
            inStream.read(&reserved, 2);           // 2 reserved bytes
            inStream.read(&valueLength, 4);        // length field is 4 bytes wide
            swapIfNecessary(gLocalByteOrder, byteOrder, &valueLength, 4, 4);
            bytesRead += 6;
        } else {
            Uint16 tmpValueLength;
            inStream.read(&tmpValueLength, 2);     // length field is 2 bytes wide
            swapIfNecessary(gLocalByteOrder, byteOrder, &tmpValueLength, 2, 2);
            bytesRead += 2;
            valueLength = tmpValueLength;
        }
        /* check whether value in length field is appropriate for this VR */
        const size_t vrSize = vr.getValueWidth();
        if ((vrSize > 1) && (valueLength % vrSize != 0))
        {
            /* warning is only reported for standard, fixed-size VRs that require more than 1 byte per value */
            DCMDATA_WARN("DcmItem: Length of element " << newTag << " is not a multiple of " << vrSize << " (VR=" << vr.getVRName() << ")");
        }
    }
    /* if the value in the length field is odd, print an error message */
    if ((valueLength & 1) && (valueLength != DCM_UndefinedLength))
    {
        DCMDATA_WARN("DcmItem: Length of element " << newTag << " is odd");
    }

    /* if desired, handle private attributes with maximum length as VR SQ */
    if (isPrivate && dcmReadImplPrivAttribMaxLengthAsSQ.get() && (valueLength == DCM_UndefinedLength))
    {
        /* re-set tag to be a sequence and also delete private creator cache */
        newTag.setVR(EVR_SQ);
        newTag.setPrivateCreator("");
    }

    /* if desired, check if length is greater than length of surrounding item */
    const Uint32 valueLengthItem = getLengthField();
    if ((ident() == EVR_item /* e.g. meta info would have length 0 */) &&
        (valueLengthItem != DCM_UndefinedLength /* this does not work in undefined length items */) &&
        (valueLength != DCM_UndefinedLength) /* Also, do not check sequences with undefined length 0xFFFFFFFF */
       )
    {
        const offile_off_t remainingItemBytes = valueLengthItem - (inStream.tell() - fStartPosition);
        /* is the explicit item length too small to cover the full value of the element value length to be read? */
        if (remainingItemBytes < 0)
        {
            DCMDATA_WARN("DcmItem: Explicit item length (" << valueLengthItem << " bytes) too large for the elements contained in the item");
            /* if the next tag is the sequence delimiter item, we can adapt to the situation */
            if (newTag.getXTag() == DCM_SequenceDelimitationItem)
            {
                DCMDATA_WARN("DcmItem: Sequence delimitation occurred before all bytes announced by explicit item length could be read");
                l_error = EC_PrematureSequDelimitationItem;
                /* rewind to start of sequence delimiter which is read in a regular way */
                /* by DcmSequenceOfItems later (if error is ignored in DcmItem::read()) */
                inStream.putback();
            }
        }
        else if (OFstatic_cast(offile_off_t, valueLength) > remainingItemBytes)
        {
            DCMDATA_WARN("DcmItem: Element " << newTag.getTagName() << " " << newTag
                << " larger (" << valueLength << ") than remaining bytes ("
                /* need to cast remainingItemBytes to unsigned long because VC6 cannot print offile_off_t (int64_t). */
                << OFstatic_cast(unsigned long, remainingItemBytes) << ") of surrounding item");
            l_error = EC_ElemLengthLargerThanItem;
        }
    }

    /* assign values to out parameter */
    length = valueLength;
    tag = newTag;

    /* return return value */
    return l_error;
}


// ********************************


OFCondition DcmItem::readSubElement(DcmInputStream &inStream,
                                    DcmTag &newTag,
                                    const Uint32 newLength,
                                    const E_TransferSyntax xfer,
                                    const E_GrpLenEncoding glenc,
                                    const Uint32 maxReadLength)
{
    DcmElement *subElem = NULL;

    /* create a new DcmElement* object with corresponding tag and */
    /* length; the object will be accessible through subElem */
    OFBool readAsUN = OFFalse;
    OFCondition l_error = DcmItem::newDicomElement(subElem, newTag, newLength, &privateCreatorCache, readAsUN);

    /* if no error occurred and subElem does not equal NULL, go ahead */
    if (l_error.good() && subElem != NULL)
    {
        // inStream.UnsetPutbackMark(); // not needed anymore with new stream architecture

        /* insert the new element into the (sorted) element list and */
        /* assign information which was read from the inStream to it */
        subElem->transferInit();
        /* we need to read the content of the attribute, no matter if */
        /* inserting the attribute succeeds or fails */
        l_error = subElem->read(inStream, (readAsUN ? EXS_LittleEndianImplicit : xfer), glenc, maxReadLength);
        // try to insert element into item. Note that
        // "elementList->insert(subElem, ELP_next)" would be faster,
        // but this is better since this insert-function creates a
        // sorted element list.
        // We insert the element even if subElem->read() reported an error
        // because otherwise I/O suspension would fail.
        OFCondition temp_error = insert(subElem, OFFalse, OFTrue);

        if (temp_error.bad())
        {
            // produce diagnostics
            DCMDATA_WARN("DcmItem: Element " << newTag
                << " found twice in one data set or item, ignoring second entry");
            delete subElem;
        }
    }
    /* else if an error occurred, try to recover from this error */
    else if (l_error == EC_InvalidTag)
    {
        /* This is the second putback operation on the putback mark in */
        /* readTagAndLength but it is impossible that both can be executed */
        /* without setting the Mark twice. */
        inStream.putback();
        DCMDATA_WARN("DcmItem: Parse error while parsing element " << newTag);
    }
    else if (l_error == EC_UndefinedLengthOBOW)
    {
        // do nothing
    }
    else if (l_error == EC_VOI_LUT_OBOW)
    {
        // do nothing
    }
    else if (l_error != EC_ItemEnd)
    {
        // inStream.UnsetPutbackMark(); // not needed anymore with new stream architecture

        // dump some information if required
        if (dcmIgnoreParsingErrors.get() || (dcmReplaceWrongDelimitationItem.get() && (l_error == EC_SequEnd)))
        {
            DCMDATA_WARN("DcmItem: Parse error in sequence item, found " << newTag
                << " instead of item delimiter " << DCM_ItemDelimitationItem);
        } else {
            DCMDATA_ERROR("DcmItem: Parse error in sequence item, found " << newTag
                << " instead of item delimiter " << DCM_ItemDelimitationItem);
        }
        // some systems use the wrong delimitation item at the end of a sequence
        if (dcmReplaceWrongDelimitationItem.get() && (l_error == EC_SequEnd))
        {
            DCMDATA_DEBUG("DcmItem::readSubItem() replacing wrong sequence delimiter "
                << DCM_SequenceDelimitationItem << " by item delimiter "
                << DCM_ItemDelimitationItem << " because it is expected here");
            l_error = EC_ItemEnd;
        } else {
            DCMDATA_DEBUG("DcmItem::readSubElement() cannot create Sub Element " << newTag);
            // treat this incorrect encoding as an error
            if (!dcmIgnoreParsingErrors.get())
                l_error = EC_ItemDelimitationItemMissing;
        }
    } else {
        // inStream.UnsetPutbackMark(); // not needed anymore with new stream architecture
    }

    /* dump some information if required */
    DCMDATA_TRACE("DcmItem::readSubItem() returns error = " << l_error.text());
    /* return result value */
    return l_error;
}


// ********************************


OFCondition DcmItem::read(DcmInputStream & inStream,
                          const E_TransferSyntax xfer,
                          const E_GrpLenEncoding glenc,
                          const Uint32 maxReadLength)
{
    return DcmItem::readUntilTag(inStream, xfer, glenc, maxReadLength, DCM_UndefinedTagKey);
}

OFCondition DcmItem::readUntilTag(DcmInputStream & inStream,
                                  const E_TransferSyntax xfer,
                                  const E_GrpLenEncoding glenc,
                                  const Uint32 maxReadLength,
                                  const DcmTagKey &stopParsingAtElement)
{
    /* check if this is an illegal call; if so set the error flag and do nothing, else go ahead */
    if (getTransferState() == ERW_notInitialized)
    {
        errorFlag = EC_IllegalCall;
        return errorFlag;
    }

    /* figure out if the stream reported an error */
    errorFlag = inStream.status();
    /* if the stream reported an error or if it is the end of the */
    /* stream, set the error flag correspondingly; else go ahead */
    if (errorFlag.good() && inStream.eos())
        errorFlag = EC_EndOfStream;
    else if (errorFlag.good() && getTransferState() != ERW_ready)
    {
        /* if the transfer state of this item is ERW_init, get its start */
        /* position in the stream and set the transfer state to ERW_inWork */
        if (getTransferState() == ERW_init)
        {
            fStartPosition = inStream.tell();  // start position of this item
            setTransferState(ERW_inWork);
        }
        DcmTag newTag;
        OFBool readStopElem = OFFalse;
        /* start a loop in order to read all elements (attributes) which are contained in the inStream */
        while (inStream.good() && (getTransferredBytes() < getLengthField() || !lastElementComplete) && !readStopElem)
        {
            /* initialize variables */
            Uint32 newValueLength = 0;
            Uint32 bytes_tagAndLen = 0;
            /* if the reading of the last element was complete, go ahead and read the next element */
            if (lastElementComplete)
            {
                /* read this element's tag and length information */
                /* (and possibly also VR information) from the inStream */
                errorFlag = readTagAndLength(inStream, xfer, newTag, newValueLength, bytes_tagAndLen);
                /* increase counter correspondingly */
                incTransferredBytes(bytes_tagAndLen);

                /* if desired, try to ignore parse error -> skip item */
                if ((errorFlag == EC_ElemLengthLargerThanItem) && dcmIgnoreParsingErrors.get())
                {
                    DCMDATA_WARN("DcmItem: Element " << newTag.getTagName() << " " << newTag
                        << " too large, trying to skip over rest of item");
                    /* we can call getLengthField because error does only occur for explicit length items */
                    const offile_off_t bytesToSkip = getLengthField() - bytes_tagAndLen;
                    if (bytesToSkip > inStream.avail()) // no chance to recover
                       break;
                    inStream.skip(bytesToSkip);
                    errorFlag = EC_Normal;
                }
                /* if desired, accept premature sequence delimitation item and continue as if item has been completely read. */
                /* The stream position has been rewound to the start position of the sequence end */
                /* delimiter tag in order to let DcmSequenceOfItems handle the delimiter in the reading routine. */
                else if ((errorFlag == EC_PrematureSequDelimitationItem) && dcmIgnoreParsingErrors.get())
                {
                     DCMDATA_WARN("DcmItem: Sequence delimitation occurred before all bytes announced by explicit item length could be read"
                         << ", trying to continue as if item was completely read");
                    errorFlag = EC_ItemEnd;  // make sure that error code leads to normal return from item reading loop
                    break; // we are completed with the item since sequence is closed
                }
                else /* continue with normal case: parse rest of element */
                {
                    /* if there was an error while we were reading from the stream, terminate the while-loop */
                    /* (note that if the last element had been read from the inStream in the last iteration, */
                    /* another iteration will be started, and of course then readTagAndLength(...) above will */
                    /* return that it encountered the end of the stream. It is only then (and here) when the */
                    /* while loop will be terminated.) */
                    if (errorFlag.bad())
                        break;
                    /* If we get to this point, we just started reading the first part */
                    /* of an element; hence, lastElementComplete is not longer true */
                    lastElementComplete = OFFalse;
                    /* in case of implicit VR, check whether the "default VR" is really appropriate */
                    if (DcmXfer(xfer).isImplicitVR())
                        checkAndUpdateVR(*this, newTag);

                    /* check if we want to stop parsing at this point, in the main dataset only */
                    if( (stopParsingAtElement != DCM_UndefinedTagKey) && (newTag.getXTag()>=stopParsingAtElement) && ident() == EVR_dataset)
                    {
                      lastElementComplete = OFTrue;
                      readStopElem = OFTrue;
                      DCMDATA_WARN("DcmItem: Element " << newTag.getTagName() << " " << newTag
                        << " encountered, skipping rest of dataset");
                    }
                    else
                    {
                      /* read the actual data value which belongs to this element */
                      /* (attribute) and insert this information into the elementList */
                      errorFlag = readSubElement(inStream, newTag, newValueLength, xfer, glenc, maxReadLength);
                      /* if reading was successful, we read the entire data value information */
                      /* for this element; hence lastElementComplete is true again */
                      if (errorFlag.good())
                        lastElementComplete = OFTrue;

                      /* in data or command sets, group 0x0001 to 0x0003, 0x0005, 0x0007 and 0xFFFF are not allowed. */
                      /* (we cannot check for group 0x0000 since we do not know whether we read a data or command set.)*/
                      if (!newTag.hasValidGroup() || (newTag.getGroup() == 0x0002))
                          DCMDATA_WARN("DcmItem: Invalid Element " << newTag << " found in data set");
                    }
                }
            } else {
                /* if lastElementComplete is false, we have only read the current element's */
                /* tag and length (and possibly VR) information as well as maybe some data */
                /* data value information. We need to continue reading the data value */
                /* information for this particular element. */
                errorFlag = elementList->get()->read(inStream, xfer, glenc, maxReadLength);
                /* if reading was successful, we read the entire information */
                /* for this element; hence lastElementComplete is true */
                if (errorFlag.good())
                    lastElementComplete = OFTrue;
            }
            /* remember how many bytes were read */
            setTransferredBytes(OFstatic_cast(Uint32, inStream.tell() - fStartPosition));
            if (errorFlag.good())
            {
                // If we completed one element, update the private tag cache.
                if (lastElementComplete)
                {
                    privateCreatorCache.updateCache(elementList->get());
                    // evaluate option for skipping rest of dataset
                    if ( (dcmStopParsingAfterElement.get() != DCM_UndefinedTagKey) &&
                         (dcmStopParsingAfterElement.get() == elementList->get()->getTag()) &&
                          ident() == EVR_dataset)
                    {
                        DCMDATA_WARN("DcmItem: Element " << newTag.getTagName() << " " << newTag
                            << " encountered, skipping rest of data set");
                        readStopElem = OFTrue;
                    }
                }
            } else
                break; // if some error was encountered terminate the while-loop
        } //while

        /* determine an appropriate result value; note that if the above called read function */
        /* encountered the end of the stream before all information for this element could be */
        /* read from the stream, the errorFlag has already been set to EC_StreamNotifyClient. */
        if (errorFlag.good())
        {
          // if stop element was read or end of stream occurs, tell parser end of stream is reached
          if (readStopElem || inStream.eos())
              errorFlag = EC_EndOfStream;
          // if all bytes could be read or last element read could not be completed, set to error
          else if ((getTransferredBytes() < getLengthField() || !lastElementComplete))
              errorFlag = EC_StreamNotifyClient;
        }
    } // else errorFlag
    /* modify the result value: three kinds of special error codes do not count as an error */
    if (errorFlag == EC_ItemEnd || errorFlag == EC_EndOfStream)
        errorFlag = EC_Normal;
    else if (errorFlag == EC_SequEnd)
    {
        if (dcmIgnoreParsingErrors.get())
        {
            /* do not treat the missing delimiter as an error */
            errorFlag = EC_Normal;
        } else
            errorFlag = EC_ItemDelimitationItemMissing;
    }

    /* if at this point the error flag indicates success, the item has */
    /* been read completely; hence, set the transfer state to ERW_ready. */
    /* Note that all information for this element could be read from the */
    /* stream, the errorFlag is still set to EC_StreamNotifyClient. */
    if (errorFlag.good())
        setTransferState(ERW_ready);

    /* dump information if required */
    DCMDATA_TRACE("DcmItem::read() returns error = " << errorFlag.text());

    /* return result value */
    return errorFlag;
}


// ********************************


OFCondition DcmItem::write(DcmOutputStream &outStream,
                           const E_TransferSyntax oxfer,
                           const E_EncodingType enctype,
                           DcmWriteCache *wcache)
{
  if (getTransferState() == ERW_notInitialized)
    errorFlag = EC_IllegalCall;
  else
  {
    errorFlag = outStream.status();
    if (errorFlag.good() && getTransferState() != ERW_ready)
    {
      if (getTransferState() == ERW_init)
      {
        if (outStream.avail() >= 8)
        {
          if (enctype == EET_ExplicitLength)
            setLengthField(getLength(oxfer, enctype));
          else
            setLengthField(DCM_UndefinedLength);
          if (errorFlag == EC_SeqOrItemContentOverflow)
            return errorFlag;
          errorFlag = writeTag(outStream, getTag(), oxfer);
          Uint32 valueLength = getLengthField();
          DcmXfer outXfer(oxfer);
          const E_ByteOrder oByteOrder = outXfer.getByteOrder();
          if (oByteOrder == EBO_unknown) return EC_IllegalCall;
          swapIfNecessary(oByteOrder, gLocalByteOrder, &valueLength, 4, 4);
          outStream.write(&valueLength, 4); // 4 bytes length
          elementList->seek(ELP_first);
          setTransferState(ERW_inWork);
        } else
          errorFlag = EC_StreamNotifyClient;
      }
      if (getTransferState() == ERW_inWork)
      {
        // elementList->get() can be NULL if buffer was full after
        // writing the last item but before writing the sequence delimitation.
        if (!elementList->empty() && (elementList->get() != NULL))
        {
          DcmObject *dO = NULL;
          do
          {
              dO = elementList->get();
              if (dO->transferState() != ERW_ready)
                errorFlag = dO->write(outStream, oxfer, enctype, wcache);
          } while (errorFlag.good() && elementList->seek(ELP_next));
        }
        if (errorFlag.good())
        {
          setTransferState(ERW_ready);
          if (getLengthField() == DCM_UndefinedLength)
          {
            if (outStream.avail() >= 8)
            {
                // write Item delimitation
                DcmTag delim(DCM_ItemDelimitationItemTag);
                errorFlag = writeTag(outStream, delim, oxfer);
                Uint32 delimLen = 0L;
                outStream.write(&delimLen, 4); // 4 bytes length
            }
            else
            {
                // Every subelement of the item is written but it
                // is not possible to write the delimitation item into the buffer.
                errorFlag = EC_StreamNotifyClient;
                setTransferState(ERW_inWork);
            }
          }
        }
      }
    }
  }
  return errorFlag;
}


// ********************************


OFCondition DcmItem::writeSignatureFormat(DcmOutputStream &outStream,
                                          const E_TransferSyntax oxfer,
                                          const E_EncodingType enctype,
                                          DcmWriteCache *wcache)
{
  if (getTransferState() == ERW_notInitialized)
    errorFlag = EC_IllegalCall;
  else
  {
    errorFlag = outStream.status();
    if (errorFlag.good() && getTransferState() != ERW_ready)
    {
      if (getTransferState() == ERW_init)
      {
        if (outStream.avail() >= 4)
        {
          if (enctype == EET_ExplicitLength)
            setLengthField(getLength(oxfer, enctype));
          else
            setLengthField(DCM_UndefinedLength);
          errorFlag = writeTag(outStream, getTag(), oxfer);
          /* we don't write the item length */
          elementList->seek(ELP_first);
          setTransferState(ERW_inWork);
        } else
          errorFlag = EC_StreamNotifyClient;
      }
      if (getTransferState() == ERW_inWork)
      {
        // elementList->get() can be NULL if buffer was full after
        // writing the last item but before writing the sequence delimitation.
        if (!elementList->empty() && (elementList->get() != NULL))
        {
          DcmObject *dO = NULL;
          do
          {
            dO = elementList->get();
            if (dO->transferState() != ERW_ready)
              errorFlag = dO->writeSignatureFormat(outStream, oxfer, enctype, wcache);
          } while (errorFlag.good() && elementList->seek(ELP_next));
        }
        if (errorFlag.good())
        {
          setTransferState(ERW_ready);
          /* we don't write an item delimitation even if the item has undefined length */
        }
      }
    }
  }
  return errorFlag;
}


// ********************************


OFBool DcmItem::isNested() const
{
    OFBool nested = OFFalse;
    if (getParent() != NULL)
    {
        // check for surrounding structure of sequence of items
        const DcmEVR parentIdent = getParent()->ident();
        if ((parentIdent == EVR_SQ) || (parentIdent == EVR_pixelSQ))
            nested = OFTrue;
    }
    return nested;
}


DcmItem *DcmItem::getParentItem()
{
    DcmItem *parentItem = NULL;
    if (getParent() != NULL)
    {
        // make sure that the direct parent has the correct type
        const DcmEVR parentIdent = getParent()->ident();
        if ((parentIdent == EVR_SQ) || (parentIdent == EVR_pixelSQ))
        {
            DcmObject *parent = getParent()->getParent();
            if (parent != NULL)
            {
                // make sure that it is really a class derived from DcmItem
                switch (parent->ident())
                {
                    case EVR_metainfo:
                    case EVR_dataset:
                    case EVR_item:
                    case EVR_dirRecord:
                        parentItem = OFreinterpret_cast(DcmItem *, parent);
                        break;
                    default:
                        DCMDATA_DEBUG("DcmItem::getParentItem() Parent object has wrong class identifier: "
                            << OFstatic_cast(int, parent->ident())
                            << " (" << DcmVR(parent->ident()).getVRName() << ")");
                        break;
                }
            }
        // When our parent is a fileformat, we should be a dataset or a metainfo.
        // In these cases, there really is no parent item, so no message.
        } else if (parentIdent != EVR_fileFormat) {
            DCMDATA_DEBUG("DcmItem::getParentItem() Direct parent object is not a sequence element");
        }
    }
    return parentItem;
}


// ********************************


void DcmItem::transferInit()
{
    DcmObject::transferInit();
    fStartPosition = 0;
    lastElementComplete = OFTrue;
    privateCreatorCache.clear();
    if (!elementList->empty())
    {
        elementList->seek(ELP_first);
        do {
            elementList->get()->transferInit();
        } while (elementList->seek(ELP_next));
    }
}


void DcmItem::transferEnd()
{
    DcmObject::transferEnd();
    privateCreatorCache.clear();
    if (!elementList->empty())
    {
        elementList->seek(ELP_first);
        do {
            elementList->get()->transferEnd();
        } while (elementList->seek(ELP_next));
    }
}


// ********************************


OFCondition DcmItem::insert(DcmElement *elem,
                            OFBool replaceOld,
                            OFBool checkInsertOrder)
{
    /* initialize error flag with ok */
    errorFlag = EC_Normal;
    /* do something only if the pointer which was passed does not equal NULL */
    if (elem != NULL)
    {
        DcmElement *dE;
        E_ListPos seekmode = ELP_last;
        /* iterate through elementList (from the last element to the first) */
        do {
            /* get current element from elementList */
            dE = OFstatic_cast(DcmElement *, elementList->seek(seekmode));
            /* if there is no element, i.e. elementList is empty */
            if (dE == NULL)
            {
                /* insert new element at the beginning of elementList */
                elementList->insert(elem, ELP_first);
                if (checkInsertOrder)
                {
                    // check if we have inserted at the end of the list
                    if (elem != OFstatic_cast(DcmElement *, elementList->seek(ELP_last)))
                    {
                        // produce diagnostics
                        DCMDATA_WARN("DcmItem: Dataset not in ascending tag order, at element " << elem->getTag());
                    }
                }
                /* dump some information if required */
                DCMDATA_TRACE("DcmItem::insert() Element " << elem->getTag()
                    << " VR=\"" << DcmVR(elem->getVR()).getVRName() << "\" inserted at beginning");
                /* check whether the new element already has a parent */
                if (elem->getParent() != NULL)
                {
                    DCMDATA_DEBUG("DcmItem::insert() Element " << elem->getTag() << " already has a parent: "
                      << elem->getParent()->getTag() << " VR=" << DcmVR(elem->getParent()->getVR()).getVRName());
                }
                /* remember the parent (i.e. the surrounding item/dataset) */
                elem->setParent(this);
                /* terminate do-while-loop */
                break;
            }
            /* else if the new element's tag is greater than the current element's tag */
            /* (i.e. we have found the position where the new element shall be inserted) */
            else if (elem->getTag() > dE->getTag())
            {
                /* insert the new element after the current element */
                elementList->insert(elem, ELP_next);
                if (checkInsertOrder)
                {
                    // check if we have inserted at the end of the list
                    if (elem != OFstatic_cast(DcmElement *, elementList->seek(ELP_last)))
                    {
                        // produce diagnostics
                        DCMDATA_WARN("DcmItem: Dataset not in ascending tag order, at element " << elem->getTag());
                    }
                }
                /* dump some information if required */
                DCMDATA_TRACE("DcmItem::insert() Element " << elem->getTag()
                    << " VR=\"" << DcmVR(elem->getVR()).getVRName() << "\" inserted");
                /* check whether the new element already has a parent */
                if (elem->getParent() != NULL)
                {
                    DCMDATA_DEBUG("DcmItem::insert() Element " << elem->getTag() << " already has a parent: "
                        << elem->getParent()->getTag() << " VR=" << DcmVR(elem->getParent()->getVR()).getVRName());
                }
                /* remember the parent (i.e. the surrounding item/dataset) */
                elem->setParent(this);
                /* terminate do-while-loop */
                break;
            }
            /* else if the current element and the new element show the same tag */
            else if (elem->getTag() == dE->getTag())
            {
                /* if new and current element are not identical */
                if (elem != dE)
                {
                    /* if the current (old) element shall be replaced */
                    if (replaceOld)
                    {
                        /* remove current element from list */
                        DcmObject *remObj = elementList->remove();

                        /* now the following holds: remObj == dE and elementList */
                        /* points to the element after the former current element. */

                        /* if the pointer to the removed object does not */
                        /* equal NULL (the usual case), delete this object */
                        /* and dump some information if required */
                        if (remObj != NULL)
                        {
                            /* dump some information if required */
                            DCMDATA_TRACE("DcmItem::insert() Element " << remObj->getTag()
                                << " VR=\"" << DcmVR(remObj->getVR()).getVRName()
                                << "\" p=" << OFstatic_cast(void *, remObj) << " removed and deleted");
                            delete remObj;
                        }
                        /* insert the new element before the current element */
                        elementList->insert(elem, ELP_prev);
                        /* dump some information if required */
                        DCMDATA_TRACE("DcmItem::insert() Element " << elem->getTag()
                            << " VR=\"" << DcmVR(elem->getVR()).getVRName()
                            << "\" p=" << OFstatic_cast(void *, elem) << " replaced older one");
                        /* check whether the new element already has a parent */
                        if (elem->getParent() != NULL)
                        {
                            DCMDATA_DEBUG("DcmItem::insert() Element " << elem->getTag() << " already has a parent: "
                                << elem->getParent()->getTag() << " VR=" << DcmVR(elem->getParent()->getVR()).getVRName());
                        }
                        /* remember the parent (i.e. the surrounding item/dataset) */
                        elem->setParent(this);
                    }   // if (replaceOld)
                    /* or else, i.e. the current element shall not be replaced by the new element */
                    else {
                        /* set the error flag correspondingly; we do not */
                        /* allow two elements with the same tag in elementList */
                        errorFlag = EC_DoubledTag;
                    }   // if (!replaceOld)
                }   // if (elem != dE)
                /* if the new and the current element are identical, the caller tries to insert */
                /* one element twice. Most probably an application error. */
                else {
                    errorFlag = EC_DoubledTag;
                }
                /* terminate do-while-loop */
                break;
            }
            /* set the seek mode to "get the previous element" */
            seekmode = ELP_prev;
        } while (dE);
    }
    /* if the pointer which was passed equals NULL, this is an illegal call */
    else
        errorFlag = EC_IllegalCall;
    /* return result value */
    return errorFlag;
}


// ********************************


DcmElement *DcmItem::getElement(const unsigned long num)
{
    errorFlag = EC_Normal;
    DcmElement *elem;
    elem = OFstatic_cast(DcmElement *, elementList->seek_to(num));
    /* reads element from list */
    if (elem == NULL)
        errorFlag = EC_IllegalCall;
    return elem;
}


// ********************************


DcmObject *DcmItem::nextInContainer(const DcmObject *obj)
{
    if (!obj)
        return elementList->get(ELP_first);
    else
    {
        if (elementList->get() != obj)
        {
            for(DcmObject * search_obj = elementList->seek(ELP_first);
                search_obj && search_obj != obj;
                search_obj = elementList->seek(ELP_next)
               ) {
                /* do nothing, just keep iterating */
            }
        }
        return elementList->seek(ELP_next);
    }
}


// ********************************


OFCondition DcmItem::nextObject(DcmStack &stack,
                                const OFBool intoSub)
{
    OFCondition l_error = EC_Normal;
    DcmObject * container = NULL;
    DcmObject * obj = NULL;
    DcmObject * result = NULL;
    OFBool examSub = intoSub;

    if (stack.empty())
    {
        stack.push(this);
        examSub = OFTrue;
    }

    obj = stack.top();
    if (obj->isLeaf() || !intoSub)
    {
        stack.pop();
        if (stack.card() > 0)
        {
            container = stack.top();
            result = container->nextInContainer(obj);
        }
    } else if (examSub)
        result = obj->nextInContainer(NULL);

    if (result)
        stack.push(result);
    else if (intoSub)
        l_error = nextUp(stack);
    else
        l_error = EC_SequEnd;

    return l_error;
}


// ********************************


DcmElement *DcmItem::remove(const unsigned long num)
{
    errorFlag = EC_Normal;
    DcmElement *elem;
    elem = OFstatic_cast(DcmElement *, elementList->seek_to(num));
    // read element from list
    if (elem != NULL)
    {
        elementList->remove();          // removes element from list but does not delete it
        elem->setParent(NULL);          // forget about the parent
    } else
        errorFlag = EC_IllegalCall;
    return elem;
}


// ********************************


DcmElement *DcmItem::remove(DcmObject *elem)
{
    errorFlag = EC_IllegalCall;
    if (!elementList->empty() && elem != NULL)
    {
        DcmObject *dO;
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            if (dO == elem)
            {
                elementList->remove();     // removes element from list but does not delete it
                elem->setParent(NULL);     // forget about the parent
                errorFlag = EC_Normal;
                break;
            }
        } while (elementList->seek(ELP_next));
    }
    if (errorFlag == EC_IllegalCall)
        return NULL;
    else
        return OFstatic_cast(DcmElement *, elem);
}


// ********************************


DcmElement *DcmItem::remove(const DcmTagKey &tag)
{
    errorFlag = EC_TagNotFound;
    DcmObject *dO = NULL;
    if (!elementList->empty())
    {
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            if (dO->getTag() == tag)
            {
                elementList->remove();     // removes element from list but does not delete it
                dO->setParent(NULL);       // forget about the parent
                errorFlag = EC_Normal;
                break;
            }
        } while (elementList->seek(ELP_next));
    }

    if (errorFlag == EC_TagNotFound)
        return NULL;
    else
        return OFstatic_cast(DcmElement *, dO);
}


// ********************************


OFCondition DcmItem::clear()
{
    errorFlag = EC_Normal;
    // remove all elements from item and delete them from memory
    elementList->deleteAllElements();
    setLengthField(0);

    return errorFlag;
}


OFBool DcmItem::isEmpty(const OFBool /*normalize*/)
{
    return elementList->empty();
}


// ********************************


OFCondition DcmItem::verify(const OFBool autocorrect)
{
    errorFlag = EC_Normal;
    if (!elementList->empty())
    {
        DcmObject *dO;
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            if (dO->verify(autocorrect).bad())
                errorFlag = EC_CorruptedData;
        } while (elementList->seek(ELP_next));
    }
    if (autocorrect)
        setLengthField(getLength());
    return errorFlag;
}


// ********************************

// Precondition: elementList is non-empty!
// Result:       - return EC_Normal;
//                 push element pointer on resultStack
//               - return EC_TagNotFound;
//                 resultStack unmodified
// Search again: push pointer of sub-element on resultStack and
//               start sub-search

OFCondition DcmItem::searchSubFromHere(const DcmTagKey &tag,
                                       DcmStack &resultStack,
                                       OFBool searchIntoSub)
{
    DcmObject *dO;
    OFCondition l_error = EC_TagNotFound;
    if (!elementList->empty())
    {
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            if (searchIntoSub)
            {
                resultStack.push(dO);
                if (dO->getTag() == tag)
                    l_error = EC_Normal;
                else
                    l_error = dO->search(tag, resultStack, ESM_fromStackTop, OFTrue);
                if (l_error.bad())
                    resultStack.pop();
            } else {
                if (dO->getTag() == tag)
                {
                    resultStack.push(dO);
                    l_error = EC_Normal;
                }
            }
        } while (l_error.bad() && elementList->seek(ELP_next));
        if (l_error==EC_Normal && dO->getTag()==tag)
        {
            DCMDATA_TRACE("DcmItem::searchSubFromHere() Element " << tag << " found");
        }
    }
    return l_error;
}


// ********************************


OFCondition DcmItem::search(const DcmTagKey &tag,
                            DcmStack &resultStack,
                            E_SearchMode mode,
                            OFBool searchIntoSub)
{
    DcmObject *dO = NULL;
    OFCondition l_error = EC_TagNotFound;
    if (mode == ESM_afterStackTop && resultStack.top() == this)
    {
        l_error = searchSubFromHere(tag, resultStack, searchIntoSub);
    }
    else if (!elementList->empty())
    {
        if (mode == ESM_fromHere || resultStack.empty())
        {
            resultStack.clear();
            l_error = searchSubFromHere(tag, resultStack, searchIntoSub);
        }
        else if (mode == ESM_fromStackTop)
        {
            dO = resultStack.top();
            if (dO == this)
                l_error = searchSubFromHere(tag, resultStack, searchIntoSub);
            else
            {   // gehe direkt zu Sub-Baum und suche dort weiter
                l_error = dO->search(tag, resultStack, mode, searchIntoSub);
// The next two lines destroy the stack->so delete them
//                if (l_error.bad()) // raeumt nur die oberste Stackebene
//                    resultStack.pop();      // ab; der Rest ist unveraendert
            }
        }
        else if (mode == ESM_afterStackTop && searchIntoSub)
        {
            // resultStack enthaelt Zielinformationen:
            // - stelle Zustand der letzen Suche in den einzelnen Suchroutinen
            //   wieder her
            // - finde Position von dO in Baum-Struktur
            //   1. suche eigenen Stack-Eintrag
            //      - bei Fehlschlag Suche beenden
            //   2. nehme naechsthoeheren Eintrag dnO
            //   3. stelle eigene Liste auf Position von dnO
            //   4. starte Suche ab dnO

            unsigned long i = resultStack.card();
            while (i > 0 && (dO = resultStack.elem(i-1)) != this)
            {
                i--;
            }
            if (dO != this && resultStack.card() > 0)
            {                            // oberste Ebene steht nie in resultStack
                i = resultStack.card()+1;// zeige jetzt auf hoechste Ebene+1
                dO = this;               // Treffer der hoechsten Ebene!
            }
            if (dO == this)
            {
                if (i == 1)                   // habe resultStack.top() gefunden
                    l_error = EC_TagNotFound; // markiere als kein Treffer, s.o.
                else                          //   siehe oben
                {
                    E_SearchMode submode = mode;
                    OFBool searchNode = OFTrue;
                    DcmObject *dnO;
                    dnO = resultStack.elem(i - 2); // Knoten der naechsten Ebene
                    elementList->seek(ELP_first);
                    do {
                        dO = elementList->get();
                        searchNode = searchNode ? (dO != dnO) : OFFalse;
                        if (!searchNode)
                        {                             // suche jetzt weiter
                            if (submode == ESM_fromStackTop)
                                resultStack.push(dO); // Stack aktualisieren
                            if (submode == ESM_fromStackTop && dO->getTag() == tag)
                                l_error = EC_Normal;
                            else
                                l_error = dO->search(tag, resultStack, submode, OFTrue);
                            if (l_error.bad())
                                resultStack.pop();
                            else
                                break;
                            submode = ESM_fromStackTop; // ab hier normale Suche
                        }
                    } while (elementList->seek(ELP_next));
                }
            } else
                l_error = EC_IllegalCall;
        } // (mode == ESM_afterStackTop
        else
            l_error = EC_IllegalCall;
    }
    return l_error;
}


// ********************************


OFCondition DcmItem::loadAllDataIntoMemory()
{
    OFCondition l_error = EC_Normal;
    if (!elementList->empty())
    {
        elementList->seek(ELP_first);
        do {
            OFCondition err = EC_Normal;
            DcmObject *dO = elementList->get();
            if ((err = dO->loadAllDataIntoMemory()).bad())
                l_error = err;
        } while (elementList->seek(ELP_next));
    }
    return l_error;
}


// ********************************


void DcmItem::compactElements(const Uint32 maxLength)
{
    DcmStack stack;
    DcmObject *object = NULL;
    /* iterate over all elements */
    while (nextObject(stack, OFTrue).good())
    {
        object = stack.top();
        // compact element if maximum length is exceeded
        if (object->isLeaf() && (object->getLength() > maxLength))
            OFstatic_cast(DcmElement *, object)->compact();
    }
}


// ********************************

// Support functions


OFCondition nextUp(DcmStack &stack)
{
    DcmObject *oldContainer = stack.pop();
    if (oldContainer->isLeaf())
        return EC_IllegalCall;
    else if (!stack.empty())
    {
        DcmObject *container = stack.top();
        DcmObject *result = container->nextInContainer(oldContainer);
        if (result)
        {
            stack.push(result);
            return EC_Normal;
        }
        else
            return nextUp(stack);
    }
    return EC_TagNotFound;
}


/*
** Simple tests for existence
*/

OFBool DcmItem::tagExists(const DcmTagKey &key,
                          OFBool searchIntoSub)
{
    DcmStack stack;

    OFCondition ec = search(key, stack, ESM_fromHere, searchIntoSub);
    return (ec.good());
}


OFBool DcmItem::tagExistsWithValue(const DcmTagKey &key,
                                   OFBool searchIntoSub)
{
    DcmStack stack;
    OFBool result = OFFalse;

    if (search(key, stack, ESM_fromHere, searchIntoSub).good())
    {
        DcmElement *elem = OFstatic_cast(DcmElement *, stack.top());
        if (elem != NULL)
            result = !(elem->isEmpty());
    }

    return result;
}


// ********************************

/* --- findAndGet functions: find an element and get it or the value, respectively --- */

OFCondition DcmItem::findAndGetElement(const DcmTagKey &tagKey,
                                       DcmElement *&element,
                                       const OFBool searchIntoSub,
                                       const OFBool createCopy)
{
    DcmStack stack;
    /* find the element */
    OFCondition status = search(tagKey, stack, ESM_fromHere, searchIntoSub);
    if (status.good())
    {
        element = OFstatic_cast(DcmElement *, stack.top());
        /* should never happen but ... */
        if (element == NULL)
            status = EC_CorruptedData;
        else if (createCopy)
        {
            /* create a copy of the element */
            element = OFstatic_cast(DcmElement *, element->clone());
            if (element == NULL)
                status = EC_MemoryExhausted;
        }
    } else {
        /* reset element pointer */
        element = NULL;
    }
    return status;
}


OFCondition DcmItem::findAndGetElements(const DcmTagKey &tagKey,
                                        DcmStack &resultStack)
{
    OFCondition status = EC_TagNotFound;
    DcmStack stack;
    DcmObject *object = NULL;
    /* iterate over all elements */
    while (nextObject(stack, OFTrue).good())
    {
        /* get element */
        object = stack.top();
        if (object->getTag() == tagKey)
        {
            /* add it to the result stack */
            resultStack.push(object);
            status = EC_Normal;
        }
    }
    return status;
}


OFCondition DcmItem::findAndGetString(const DcmTagKey& tagKey,
                                      const char *&value,
                                      const OFBool searchIntoSub)
{
    DcmElement *elem;
    /* find the element */
    OFCondition status = findAndGetElement(tagKey, elem, searchIntoSub);
    if (status.good())
    {
        /* get the value */
        status = elem->getString(OFconst_cast(char *&, value));
    }
    /* reset value */
    if (status.bad())
        value = NULL;
    return status;
}


OFCondition DcmItem::findAndGetString(const DcmTagKey& tagKey,
                                      const char *&value,
                                      Uint32 &length,
                                      const OFBool searchIntoSub)
{
    DcmElement *elem;
    /* find the element */
    OFCondition status = findAndGetElement(tagKey, elem, searchIntoSub);
    if (status.good())
    {
        /* get the value */
        status = elem->getString(OFconst_cast(char *&, value), length);
    }
    /* reset values */
    if (status.bad())
    {
        value = NULL;
        length = 0;
    }
    return status;
}


OFCondition DcmItem::findAndGetOFString(const DcmTagKey& tagKey,
                                        OFString &value,
                                        const unsigned long pos,
                                        const OFBool searchIntoSub)
{
    DcmElement *elem;
    /* find the element */
    OFCondition status = findAndGetElement(tagKey, elem, searchIntoSub);
    if (status.good())
    {
        /* get the value */
        status = elem->getOFString(value, pos);
    }
    /* reset value */
    if (status.bad())
        value.clear();
    return status;
}


OFCondition DcmItem::findAndGetOFStringArray(const DcmTagKey& tagKey,
                                             OFString &value,
                                             const OFBool searchIntoSub)
{
    DcmElement *elem;
    /* find the element */
    OFCondition status = findAndGetElement(tagKey, elem, searchIntoSub);
    if (status.good())
    {
        /* get the value */
        status = elem->getOFStringArray(value);
    }
    /* reset value */
    if (status.bad())
        value.clear();
    return status;
}


OFCondition DcmItem::findAndGetUint8(const DcmTagKey& tagKey,
                                     Uint8 &value,
                                     const unsigned long pos,
                                     const OFBool searchIntoSub)
{
    DcmElement *elem;
    /* find the element */
    OFCondition status = findAndGetElement(tagKey, elem, searchIntoSub);
    if (status.good())
    {
        /* get the value */
        status = elem->getUint8(value, pos);
    }
    /* reset value */
    if (status.bad())
        value = 0;
    return status;
}


OFCondition DcmItem::findAndGetUint8Array(const DcmTagKey& tagKey,
                                          const Uint8 *&value,
                                          unsigned long *count,
                                          const OFBool searchIntoSub)
{
    DcmElement *elem;
    /* find the element */
    OFCondition status = findAndGetElement(tagKey, elem, searchIntoSub);
    if (status.good())
    {
        /* get the value */
        Uint8 *array = NULL;
        status = elem->getUint8Array(array);
        value = array;
    }
    /* set optional count parameter */
    if (count != NULL)
    {
        if (status.good())
            *count = elem->getLength() / OFstatic_cast(unsigned long, sizeof(Uint8));
        else
            *count = 0;
    }
    /* reset value */
    if (status.bad())
        value = NULL;
    return status;
}


OFCondition DcmItem::findAndGetUint16(const DcmTagKey& tagKey,
                                      Uint16 &value,
                                      const unsigned long pos,
                                      const OFBool searchIntoSub)
{
    DcmElement *elem;
    /* find the element */
    OFCondition status = findAndGetElement(tagKey, elem, searchIntoSub);
    if (status.good())
    {
        /* get the value */
        status = elem->getUint16(value, pos);
    }
    /* reset value */
    if (status.bad())
        value = 0;
    return status;
}


OFCondition DcmItem::findAndGetUint16Array(const DcmTagKey& tagKey,
                                           const Uint16 *&value,
                                           unsigned long *count,
                                           const OFBool searchIntoSub)
{
    DcmElement *elem;
    /* find the element */
    OFCondition status = findAndGetElement(tagKey, elem, searchIntoSub);
    if (status.good())
    {
        /* get the value */
        Uint16 *array = NULL;
        status = elem->getUint16Array(array);
        value = array;
    }
    /* set optional count parameter */
    if (count != NULL)
    {
        if (status.good())
            *count = elem->getLength() / OFstatic_cast(unsigned long, sizeof(Uint16));
        else
            *count = 0;
    }
    /* reset value */
    if (status.bad())
        value = NULL;
    return status;
}


OFCondition DcmItem::findAndGetSint16(const DcmTagKey& tagKey,
                                      Sint16 &value,
                                      const unsigned long pos,
                                      const OFBool searchIntoSub)
{
    DcmElement *elem;
    /* find the element */
    OFCondition status = findAndGetElement(tagKey, elem, searchIntoSub);
    if (status.good())
    {
        /* get the value */
        status = elem->getSint16(value, pos);
    }
    /* reset value */
    if (status.bad())
        value = 0;
    return status;
}


OFCondition DcmItem::findAndGetSint16Array(const DcmTagKey& tagKey,
                                           const Sint16 *&value,
                                           unsigned long *count,
                                           const OFBool searchIntoSub)
{
    DcmElement *elem;
    /* find the element */
    OFCondition status = findAndGetElement(tagKey, elem, searchIntoSub);
    if (status.good())
    {
        /* get the value */
        Sint16 *array = NULL;
        status = elem->getSint16Array(array);
        value = array;
    }
    /* set optional count parameter */
    if (count != NULL)
    {
        if (status.good())
            *count = elem->getLength() / OFstatic_cast(unsigned long, sizeof(Sint16));
        else
            *count = 0;
    }
    /* reset value */
    if (status.bad())
        value = NULL;
    return status;
}


OFCondition DcmItem::findAndGetUint32(const DcmTagKey& tagKey,
                                      Uint32 &value,
                                      const unsigned long pos,
                                      const OFBool searchIntoSub)
{
    DcmElement *elem;
    /* find the element */
    OFCondition status = findAndGetElement(tagKey, elem, searchIntoSub);
    if (status.good())
    {
        /* get the value */
        status = elem->getUint32(value, pos);
    }
    /* reset value */
    if (status.bad())
        value = 0;
    return status;
}


OFCondition DcmItem::findAndGetUint32Array(const DcmTagKey& tagKey,
                                           const Uint32 *&value,
                                           unsigned long *count,
                                           const OFBool searchIntoSub)
{
    DcmElement *elem;
    /* find the element */
    OFCondition status = findAndGetElement(tagKey, elem, searchIntoSub);
    if (status.good())
    {
        /* get the value */
        Uint32 *array = NULL;
        status = elem->getUint32Array(array);
        value = array;
    }
    /* set optional count parameter */
    if (count != NULL)
    {
        if (status.good())
            *count = elem->getLength() / OFstatic_cast(unsigned long, sizeof(Uint32));
        else
            *count = 0;
    }
    /* reset value */
    if (status.bad())
        value = NULL;
    return status;
}


OFCondition DcmItem::findAndGetSint32(const DcmTagKey& tagKey,
                                      Sint32 &value,
                                      const unsigned long pos,
                                      const OFBool searchIntoSub)
{
    DcmElement *elem;
    /* find the element */
    OFCondition status = findAndGetElement(tagKey, elem, searchIntoSub);
    if (status.good())
    {
        /* get the value */
        status = elem->getSint32(value, pos);
    }
    /* reset value */
    if (status.bad())
        value = 0;
    return status;
}


OFCondition DcmItem::findAndGetSint32Array(const DcmTagKey& tagKey,
                                           const Sint32 *&value,
                                           unsigned long *count,
                                           const OFBool searchIntoSub)
{
    DcmElement *elem;
    /* find the element */
    OFCondition status = findAndGetElement(tagKey, elem, searchIntoSub);
    if (status.good())
    {
        /* get the value */
        Sint32 *array = NULL;
        status = elem->getSint32Array(array);
        value = array;
    }
    /* set optional count parameter */
    if (count != NULL)
    {
        if (status.good())
            *count = elem->getLength() / OFstatic_cast(unsigned long, sizeof(Sint32));
        else
            *count = 0;
    }
    /* reset value */
    if (status.bad())
        value = NULL;
    return status;
}


OFCondition DcmItem::findAndGetLongInt(const DcmTagKey& tagKey,
                                       long int &value,
                                       const unsigned long pos,
                                       const OFBool searchIntoSub)
{
    DcmElement *elem;
    /* find the element */
    OFCondition status = findAndGetElement(tagKey, elem, searchIntoSub);
    if (status.good())
    {
        /* distinguish supported VRs */
        switch (elem->ident())
        {
            case EVR_UL:
            case EVR_OL:
            case EVR_up:
                Uint32 ul;
                status = elem->getUint32(ul, pos);
                value = OFstatic_cast(long int, ul);
                break;
            case EVR_SL:
            case EVR_IS:
                Sint32 sl;
                status = elem->getSint32(sl, pos);
                value = OFstatic_cast(long int, sl);
                break;
            case EVR_US:
            case EVR_xs:
            case EVR_lt:
                Uint16 us;
                status = elem->getUint16(us, pos);
                value = OFstatic_cast(long int, us);
                break;
            case EVR_SS:
                Sint16 ss;
                status = elem->getSint16(ss, pos);
                value = OFstatic_cast(long int, ss);
                break;
            default:
                status = EC_IllegalCall;
                break;
        }
    }
    /* reset value */
    if (status.bad())
        value = 0;
    return status;
}


OFCondition DcmItem::findAndGetFloat32(const DcmTagKey& tagKey,
                                       Float32 &value,
                                       const unsigned long pos,
                                       const OFBool searchIntoSub)
{
    DcmElement *elem;
    /* find the element */
    OFCondition status = findAndGetElement(tagKey, elem, searchIntoSub);
    if (status.good())
    {
        /* get the value */
        status = elem->getFloat32(value, pos);
    }
    /* reset value */
    if (status.bad())
        value = 0;
    return status;
}


OFCondition DcmItem::findAndGetFloat32Array(const DcmTagKey& tagKey,
                                            const Float32 *&value,
                                            unsigned long *count,
                                            const OFBool searchIntoSub)
{
    DcmElement *elem;
    /* find the element */
    OFCondition status = findAndGetElement(tagKey, elem, searchIntoSub);
    if (status.good())
    {
        /* get the value */
        Float32 *array = NULL;
        status = elem->getFloat32Array(array);
        value = array;
    }
    /* set optional count parameter */
    if (count != NULL)
    {
        if (status.good())
            *count = elem->getLength() / OFstatic_cast(unsigned long, sizeof(Float32));
        else
            *count = 0;
    }
    /* reset value */
    if (status.bad())
        value = NULL;
    return status;
}


OFCondition DcmItem::findAndGetFloat64(const DcmTagKey& tagKey,
                                       Float64 &value,
                                       const unsigned long pos,
                                       const OFBool searchIntoSub)
{
    DcmElement *elem;
    /* find the element */
    OFCondition status = findAndGetElement(tagKey, elem, searchIntoSub);
    if (status.good())
    {
        /* get the value */
        status = elem->getFloat64(value, pos);
    }
    /* reset value */
    if (status.bad())
        value = 0;
    return status;
}


OFCondition DcmItem::findAndGetFloat64Array(const DcmTagKey& tagKey,
                                            const Float64 *&value,
                                            unsigned long *count,
                                            const OFBool searchIntoSub)
{
    DcmElement *elem;
    /* find the element */
    OFCondition status = findAndGetElement(tagKey, elem, searchIntoSub);
    if (status.good())
    {
        /* get the value */
        Float64 *array = NULL;
        status = elem->getFloat64Array(array);
        value = array;
    }
    /* set optional count parameter */
    if (count != NULL)
    {
        if (status.good())
            *count = elem->getLength() / OFstatic_cast(unsigned long, sizeof(Float64));
        else
            *count = 0;
    }
    /* reset value */
    if (status.bad())
        value = NULL;
    return status;
}


OFCondition DcmItem::findAndGetSequence(const DcmTagKey &seqTagKey,
                                        DcmSequenceOfItems *&sequence,
                                        const OFBool searchIntoSub,
                                        const OFBool createCopy)
{
    DcmStack stack;
    /* find the element */
    OFCondition status = search(seqTagKey, stack, ESM_fromHere, searchIntoSub);
    if (status.good())
    {
        DcmElement *delem = OFstatic_cast(DcmElement *, stack.top());
        /* should never happen but ... */
        if (delem == NULL)
            status = EC_CorruptedData;
        /* check for correct VR */
        else if ((delem->ident() == EVR_SQ) || (delem->ident() == EVR_pixelSQ))
        {
            sequence = OFstatic_cast(DcmSequenceOfItems *, delem);
            /* create a copy of the sequence? */
            if (createCopy)
            {
                sequence = OFstatic_cast(DcmSequenceOfItems *, sequence->clone());
                if (sequence == NULL)
                    status = EC_MemoryExhausted;
            }
        } else
            status = EC_InvalidVR;
    }
    if (status.bad())
    {
        /* reset sequence pointer */
        sequence = NULL;
    }
    return status;
}


OFCondition DcmItem::findAndGetSequenceItem(const DcmTagKey &seqTagKey,
                                            DcmItem *&item,
                                            const signed long itemNum,
                                            const OFBool createCopy)
{
    DcmStack stack;
    /* find sequence */
    OFCondition status = search(seqTagKey, stack, ESM_fromHere, OFFalse /*searchIntoSub*/);
    if (status.good())
    {
        /* get element */
        DcmElement *delem = OFstatic_cast(DcmElement *, stack.top());
        if (delem != NULL)
        {
            /* check VR */
            if ((delem->ident() == EVR_SQ) || (delem->ident() == EVR_pixelSQ))
            {
                DcmSequenceOfItems *sequence = OFstatic_cast(DcmSequenceOfItems *, delem);
                const unsigned long count = sequence->card();
                /* empty sequence? */
                if (count > 0)
                {
                    /* get last item */
                    if (itemNum == -1)
                        item = sequence->getItem(count - 1);
                    /* get specified item */
                    else if ((itemNum >= 0) && (OFstatic_cast(unsigned long, itemNum) < count))
                        item = sequence->getItem(OFstatic_cast(unsigned long, itemNum));
                    /* invalid item number */
                    else
                        status = EC_IllegalParameter;
                    /* create a copy of the item? */
                    if (createCopy)
                    {
                        if (status.good() && (item != NULL))
                        {
                            item = OFstatic_cast(DcmItem *, item->clone());
                            if (item == NULL)
                                status = EC_MemoryExhausted;
                        }
                    }
                } else
                    status = EC_IllegalParameter;
            } else
                status = EC_InvalidVR;
        } else
            status = EC_CorruptedData;
    }
    /* reset item value */
    if (status.bad())
        item = NULL;
    else if (item == NULL)
        status = EC_IllegalCall;
    return status;
}


// ********************************

/* --- findOrCreate functions: find an element or create a new one --- */

OFCondition DcmItem::findOrCreateSequenceItem(const DcmTag& seqTag,
                                              DcmItem *&item,
                                              const signed long itemNum)
{
    DcmStack stack;
    /* find sequence */
    OFCondition status = search(seqTag, stack, ESM_fromHere, OFFalse /*searchIntoSub*/);
    DcmSequenceOfItems *sequence = NULL;
    /* sequence found? */
    if (status.good())
    {
        /* get element */
        DcmElement *delem = OFstatic_cast(DcmElement *, stack.top());
        if (delem != NULL)
        {
            /* check VR */
            if ((delem->ident() == EVR_SQ) || (delem->ident() == EVR_pixelSQ))
                sequence = OFstatic_cast(DcmSequenceOfItems *, delem);
            else
                status = EC_InvalidVR;
        } else
            status = EC_CorruptedData;
    } else {
        /* create new sequence element */
        sequence = new DcmSequenceOfItems(seqTag);
        if (sequence != NULL)
        {
            /* insert into item/dataset */
            status = insert(sequence, OFTrue /*replaceOld*/);
            if (status.bad())
                delete sequence;
        } else
            status = EC_MemoryExhausted;
    }
    if (status.good())
    {
        if (sequence != NULL)
        {
            const unsigned long count = sequence->card();
            /* existing item? */
            if ((count > 0) && (itemNum >= -1) && (itemNum < OFstatic_cast(signed long, count)))
            {
                if (itemNum == -1)
                {
                    /* get last item */
                    item = sequence->getItem(count - 1);
                } else {
                    /* get specified item */
                    item = sequence->getItem(OFstatic_cast(unsigned long, itemNum));
                }
            /* create new item(s) */
            } else {
                unsigned long i = 0;
                /* create empty trailing items if required */
                const unsigned long itemCount = (itemNum > OFstatic_cast(signed long, count)) ? (itemNum - count + 1) : 1;
                while ((i < itemCount) && status.good())
                {
                    item = new DcmItem();
                    if (item != NULL)
                    {
                        /* append new item to end of sequence */
                        status = sequence->append(item);
                        if (status.bad())
                            delete item;
                    } else
                        status = EC_MemoryExhausted;
                    i++;
                }
            }
        } else
            status = EC_IllegalCall;
    }
    /* reset item value */
    if (status.bad())
        item = NULL;
    else if (item == NULL)
        status = EC_IllegalCall;
    return status;
}


// ********************************


/* --- findAndXXX functions: find an element and do something with it --- */

OFCondition DcmItem::findAndInsertCopyOfElement(const DcmTagKey &tagKey,
                                                DcmItem *destItem,
                                                const OFBool replaceOld)
{
    OFCondition status = EC_IllegalParameter;
    if (destItem != NULL)
    {
        DcmElement *delem = NULL;
        /* get copy of element from current dataset */
        status = findAndGetElement(tagKey, delem, OFFalse /*searchIntoSub*/, OFTrue /*createCopy*/);
        if (status.good())
        {
            /* ... and insert it into the destination dataset */
            status = destItem->insert(delem, replaceOld);
            if (status.bad())
                delete delem;
        }
    }
    return status;
}


OFCondition DcmItem::findAndDeleteElement(const DcmTagKey &tagKey,
                                          const OFBool allOccurrences,
                                          const OFBool searchIntoSub)
{
    OFCondition status = EC_TagNotFound;
    DcmStack stack;
    DcmObject *object = NULL;
    OFBool intoSub = OFTrue;
    /* iterate over all elements */
    while (nextObject(stack, intoSub).good())
    {
        /* get element */
        object = stack.top();
        if (object->getTag() == tagKey)
        {
            stack.pop();
            /* remove element from dataset and free memory */
            delete OFstatic_cast(DcmItem *, stack.top())->remove(object);
            status = EC_Normal;
            /* delete only the first element? */
            if (!allOccurrences)
                break;
        }
        intoSub = searchIntoSub || allOccurrences;
    }
    return status;
}


OFCondition DcmItem::findAndDeleteSequenceItem(const DcmTagKey &seqTagKey,
                                               const signed long itemNum)
{
    DcmStack stack;
    /* find sequence */
    OFCondition status = search(seqTagKey, stack, ESM_fromHere, OFFalse /*searchIntoSub*/);
    if (status.good())
    {
        /* get element */
        DcmElement *delem = OFstatic_cast(DcmElement *, stack.top());
        if (delem != NULL)
        {
            /* check VR */
            if ((delem->ident() == EVR_SQ) || (delem->ident() == EVR_pixelSQ))
            {
                DcmSequenceOfItems *sequence = OFstatic_cast(DcmSequenceOfItems *, delem);
                const unsigned long count = sequence->card();
                /* last item? */
                if (itemNum == -1)
                    delete sequence->remove(count - 1);
                /* valid item? */
                else if ((itemNum >= 0) && (OFstatic_cast(unsigned long, itemNum) < count))
                    delete sequence->remove(OFstatic_cast(unsigned long, itemNum));
                else
                    status = EC_IllegalParameter;
            } else
                status = EC_InvalidVR;
        } else
            status = EC_CorruptedData;
    }
    return status;
}


// ********************************

/* --- putAndInsert functions: put value and insert new element --- */

OFCondition DcmItem::putAndInsertString(const DcmTag& tag,
                                        const char *value,
                                        const OFBool replaceOld)
{
    /* determine length of the string value */
    const Uint32 length = (value != NULL) ? OFstatic_cast(Uint32, strlen(value)) : 0;
    /* call the real function */
    return putAndInsertString(tag, value, length, replaceOld);
}


OFCondition DcmItem::putAndInsertString(const DcmTag& tag,
                                        const char *value,
                                        const Uint32 length,
                                        const OFBool replaceOld)
{
    OFCondition status = EC_Normal;
    /* create new element */
    DcmElement *elem = NULL;
    switch(tag.getEVR())
    {
        case EVR_AE:
            elem = new DcmApplicationEntity(tag);
            break;
        case EVR_AS:
            elem = new DcmAgeString(tag);
            break;
        case EVR_AT:
            elem = new DcmAttributeTag(tag);
            break;
        case EVR_CS:
            elem = new DcmCodeString(tag);
            break;
        case EVR_DA:
            elem = new DcmDate(tag);
            break;
        case EVR_DS:
            elem = new DcmDecimalString(tag);
            break;
        case EVR_DT:
            elem = new DcmDateTime(tag);
            break;
        case EVR_FL:
            elem = new DcmFloatingPointSingle(tag);
            break;
        case EVR_FD:
            elem = new DcmFloatingPointDouble(tag);
            break;
        case EVR_IS:
            elem = new DcmIntegerString(tag);
            break;
        case EVR_LO:
            elem = new DcmLongString(tag);
            break;
        case EVR_LT:
            elem = new DcmLongText(tag);
            break;
        case EVR_OB:
        case EVR_OW:
            elem = new DcmOtherByteOtherWord(tag);
            break;
        case EVR_OD:
            elem = new DcmOtherDouble(tag);
            break;
        case EVR_OF:
            elem = new DcmOtherFloat(tag);
            break;
        case EVR_OL:
            elem = new DcmOtherLong(tag);
            break;
        case EVR_PN:
            elem = new DcmPersonName(tag);
            break;
        case EVR_SH:
            elem = new DcmShortString(tag);
            break;
        case EVR_SL:
            elem = new DcmSignedLong(tag);
            break;
        case EVR_SS:
            elem = new DcmSignedShort(tag);
            break;
        case EVR_ST:
            elem = new DcmShortText(tag);
            break;
        case EVR_TM:
            elem = new DcmTime(tag);
            break;
        case EVR_UC:
            elem = new DcmUnlimitedCharacters(tag);
            break;
        case EVR_UI:
            elem = new DcmUniqueIdentifier(tag);
            break;
        case EVR_UL:
            elem = new DcmUnsignedLong(tag);
            break;
        case EVR_UR:
            elem = new DcmUniversalResourceIdentifierOrLocator(tag);
            break;
        case EVR_US:
            elem = new DcmUnsignedShort(tag);
            break;
        case EVR_UT:
            elem = new DcmUnlimitedText(tag);
            break;
        case EVR_UNKNOWN:
            /* Unknown VR, e.g. tag not found in data dictionary */
            status = EC_UnknownVR;
            break;
        case EVR_SV:
        case EVR_UV:
        case EVR_OV:
            // TODO: requires new VR classes
            DCMDATA_ERROR("DcmItem: Support for new VR=" << tag.getVRName() << " not yet implemented");
            status = EC_NotYetImplemented;
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* put value */
        status = elem->putString(value, length);
        /* insert into dataset/item */
        if (status.good())
            status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else if (status.good())
        status = EC_MemoryExhausted;
    return status;
}


OFCondition DcmItem::putAndInsertOFStringArray(const DcmTag& tag,
                                               const OFString &value,
                                               const OFBool replaceOld)
{
    OFCondition status = EC_Normal;
    /* create new element */
    DcmElement *elem = NULL;
    switch(tag.getEVR())
    {
        case EVR_AE:
            elem = new DcmApplicationEntity(tag);
            break;
        case EVR_AS:
            elem = new DcmAgeString(tag);
            break;
        case EVR_CS:
            elem = new DcmCodeString(tag);
            break;
        case EVR_DA:
            elem = new DcmDate(tag);
            break;
        case EVR_DS:
            elem = new DcmDecimalString(tag);
            break;
        case EVR_DT:
            elem = new DcmDateTime(tag);
            break;
        case EVR_IS:
            elem = new DcmIntegerString(tag);
            break;
        case EVR_LO:
            elem = new DcmLongString(tag);
            break;
        case EVR_LT:
            elem = new DcmLongText(tag);
            break;
        case EVR_PN:
            elem = new DcmPersonName(tag);
            break;
        case EVR_SH:
            elem = new DcmShortString(tag);
            break;
        case EVR_ST:
            elem = new DcmShortText(tag);
            break;
        case EVR_TM:
            elem = new DcmTime(tag);
            break;
        case EVR_UC:
            elem = new DcmUnlimitedCharacters(tag);
            break;
        case EVR_UI:
            elem = new DcmUniqueIdentifier(tag);
            break;
        case EVR_UR:
            elem = new DcmUniversalResourceIdentifierOrLocator(tag);
            break;
        case EVR_UT:
            elem = new DcmUnlimitedText(tag);
            break;
        case EVR_UNKNOWN:
            /* Unknown VR, e.g. tag not found in data dictionary */
            status = EC_UnknownVR;
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* put value */
        status = elem->putOFStringArray(value);
        /* insert into dataset/item */
        if (status.good())
            status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else if (status.good())
        status = EC_MemoryExhausted;
    return status;
}


OFCondition DcmItem::putAndInsertUint8Array(const DcmTag& tag,
                                            const Uint8 *value,
                                            const unsigned long count,
                                            const OFBool replaceOld)
{
    OFCondition status = EC_Normal;
    /* create new element */
    DcmElement *elem = NULL;
    switch(tag.getEVR())
    {
        case EVR_OB:
            elem = new DcmOtherByteOtherWord(tag);
            break;
        case EVR_ox:
            /* special handling for Pixel Data */
            if (tag == DCM_PixelData)
            {
                elem = new DcmPixelData(tag);
                if (elem != NULL)
                    elem->setVR(EVR_OB);
            } else
                elem = new DcmPolymorphOBOW(tag);
            break;
        case EVR_UNKNOWN:
            /* Unknown VR, e.g. tag not found in data dictionary */
            status = EC_UnknownVR;
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* put value */
        status = elem->putUint8Array(value, count);
        /* insert into dataset/item */
        if (status.good())
            status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else if (status.good())
        status = EC_MemoryExhausted;
    return status;
}


OFCondition DcmItem::putAndInsertUint16(const DcmTag& tag,
                                        const Uint16 value,
                                        const unsigned long pos,
                                        const OFBool replaceOld)
{
    OFCondition status = EC_Normal;
    /* create new element */
    DcmElement *elem = NULL;
    switch(tag.getEVR())
    {
        case EVR_US:
            elem = new DcmUnsignedShort(tag);
            break;
        case EVR_lt:
        case EVR_xs:
            /* special handling */
            elem = new DcmUnsignedShort(DcmTag(tag, EVR_US));
            break;
        case EVR_UNKNOWN:
            /* Unknown VR, e.g. tag not found in data dictionary */
            status = EC_UnknownVR;
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* put value */
        status = elem->putUint16(value, pos);
        /* insert into dataset/item */
        if (status.good())
            status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else if (status.good())
        status = EC_MemoryExhausted;
    return status;
}


OFCondition DcmItem::putAndInsertUint16Array(const DcmTag& tag,
                                             const Uint16 *value,
                                             const unsigned long count,
                                             const OFBool replaceOld)
{
    OFCondition status = EC_Normal;
    /* create new element */
    DcmElement *elem = NULL;
    switch(tag.getEVR())
    {
        case EVR_AT:
            elem = new DcmAttributeTag(tag);
            break;
        case EVR_lt:
        case EVR_OW:
            elem = new DcmOtherByteOtherWord(tag);
            break;
        case EVR_US:
            elem = new DcmUnsignedShort(tag);
            break;
        case EVR_ox:
            /* special handling */
            if (tag == DCM_PixelData)
            {
                elem = new DcmPixelData(tag);
                if (elem != NULL) elem->setVR(EVR_OW);
            }
            else
                elem = new DcmPolymorphOBOW(tag);
            break;
        case EVR_xs:
            /* special handling */
            elem = new DcmUnsignedShort(DcmTag(tag, EVR_US));
            break;
        case EVR_UNKNOWN:
            /* Unknown VR, e.g. tag not found in data dictionary */
            status = EC_UnknownVR;
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* put value */
        status = elem->putUint16Array(value, count);
        /* insert into dataset/item */
        if (status.good())
            status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else if (status.good())
        status = EC_MemoryExhausted;
    return status;
}


OFCondition DcmItem::putAndInsertSint16(const DcmTag& tag,
                                        const Sint16 value,
                                        const unsigned long pos,
                                        const OFBool replaceOld)
{
    OFCondition status = EC_Normal;
    /* create new element */
    DcmElement *elem = NULL;
    switch(tag.getEVR())
    {
        case EVR_SS:
            elem = new DcmSignedShort(tag);
            break;
        case EVR_lt:
        case EVR_xs:
            /* special handling */
            elem = new DcmSignedShort(DcmTag(tag, EVR_SS));
            break;
        case EVR_UNKNOWN:
            /* Unknown VR, e.g. tag not found in data dictionary */
            status = EC_UnknownVR;
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* put value */
        status = elem->putSint16(value, pos);
        /* insert into dataset/item */
        if (status.good())
            status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else if (status.good())
        status = EC_MemoryExhausted;
    return status;
}


OFCondition DcmItem::putAndInsertSint16Array(const DcmTag& tag,
                                             const Sint16 *value,
                                             const unsigned long count,
                                             const OFBool replaceOld)
{
    OFCondition status = EC_Normal;
    /* create new element */
    DcmElement *elem = NULL;
    switch(tag.getEVR())
    {
        case EVR_SS:
            elem = new DcmSignedShort(tag);
            break;
        case EVR_lt:
        case EVR_xs:
            /* special handling */
            elem = new DcmSignedShort(DcmTag(tag, EVR_SS));
            break;
        case EVR_UNKNOWN:
            /* Unknown VR, e.g. tag not found in data dictionary */
            status = EC_UnknownVR;
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* put value */
        status = elem->putSint16Array(value, count);
        /* insert into dataset/item */
        if (status.good())
            status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else if (status.good())
        status = EC_MemoryExhausted;
    return status;
}


OFCondition DcmItem::putAndInsertUint32(const DcmTag& tag,
                                        const Uint32 value,
                                        const unsigned long pos,
                                        const OFBool replaceOld)
{
    OFCondition status = EC_Normal;
    /* create new element */
    DcmElement *elem = NULL;
    switch(tag.getEVR())
    {
        case EVR_UL:
            elem = new DcmUnsignedLong(tag);
            break;
        case EVR_OL:
            elem = new DcmOtherLong(tag);
            break;
        case EVR_UNKNOWN:
            /* Unknown VR, e.g. tag not found in data dictionary */
            status = EC_UnknownVR;
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* put value */
        status = elem->putUint32(value, pos);
        /* insert into dataset/item */
        if (status.good())
            status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else if (status.good())
        status = EC_MemoryExhausted;
    return status;
}


OFCondition DcmItem::putAndInsertUint32Array(const DcmTag& tag,
                                             const Uint32 *value,
                                             const unsigned long count,
                                             const OFBool replaceOld)
{
    OFCondition status = EC_Normal;
    /* create new element */
    DcmElement *elem = NULL;
    switch(tag.getEVR())
    {
        case EVR_OL:
            elem = new DcmOtherLong(tag);
            break;
        case EVR_UL:
            elem = new DcmUnsignedLong(tag);
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* put value */
        status = elem->putUint32Array(value, count);
        /* insert into dataset/item */
        if (status.good())
            status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else if (status.good())
        status = EC_MemoryExhausted;
    return status;
}


OFCondition DcmItem::putAndInsertSint32(const DcmTag& tag,
                                        const Sint32 value,
                                        const unsigned long pos,
                                        const OFBool replaceOld)
{
    OFCondition status = EC_Normal;
    /* create new element */
    DcmElement *elem = NULL;
    switch(tag.getEVR())
    {
        case EVR_SL:
            elem = new DcmSignedLong(tag);
            break;
        case EVR_UNKNOWN:
            /* Unknown VR, e.g. tag not found in data dictionary */
            status = EC_UnknownVR;
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* put value */
        status = elem->putSint32(value, pos);
        /* insert into dataset/item */
        if (status.good())
            status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else if (status.good())
        status = EC_MemoryExhausted;
    return status;
}


OFCondition DcmItem::putAndInsertFloat32(const DcmTag& tag,
                                         const Float32 value,
                                         const unsigned long pos,
                                         const OFBool replaceOld)
{
    OFCondition status = EC_Normal;
    /* create new element */
    DcmElement *elem = NULL;
    switch(tag.getEVR())
    {
        case EVR_FL:
            elem = new DcmFloatingPointSingle(tag);
            break;
        case EVR_OF:
            elem = new DcmOtherFloat(tag);
            break;
        case EVR_UNKNOWN:
            /* Unknown VR, e.g. tag not found in data dictionary */
            status = EC_UnknownVR;
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* put value */
        status = elem->putFloat32(value, pos);
        /* insert into dataset/item */
        if (status.good())
            status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else if (status.good())
        status = EC_MemoryExhausted;
    return status;
}


OFCondition DcmItem::putAndInsertFloat32Array(const DcmTag& tag,
                                              const Float32 *value,
                                              const unsigned long count,
                                              const OFBool replaceOld)
{
    OFCondition status = EC_Normal;
    /* create new element */
    DcmElement *elem = NULL;
    switch(tag.getEVR())
    {
        case EVR_FL:
            elem = new DcmFloatingPointSingle(tag);
            break;
        case EVR_OF:
            elem = new DcmOtherFloat(tag);
            break;
        case EVR_UNKNOWN:
            /* Unknown VR, e.g. tag not found in data dictionary */
            status = EC_UnknownVR;
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* put value */
        status = elem->putFloat32Array(value, count);
        /* insert into dataset/item */
        if (status.good())
            status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else if (status.good())
        status = EC_MemoryExhausted;
    return status;
}


OFCondition DcmItem::putAndInsertFloat64(const DcmTag& tag,
                                         const Float64 value,
                                         const unsigned long pos,
                                         const OFBool replaceOld)
{
    OFCondition status = EC_Normal;
    /* create new element */
    DcmElement *elem = NULL;
    switch(tag.getEVR())
    {
        case EVR_FD:
            elem = new DcmFloatingPointDouble(tag);
            break;
        case EVR_OD:
            elem = new DcmOtherDouble(tag);
            break;
        case EVR_UNKNOWN:
            /* Unknown VR, e.g. tag not found in data dictionary */
            status = EC_UnknownVR;
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* put value */
        status = elem->putFloat64(value, pos);
        /* insert into dataset/item */
        if (status.good())
            status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else if (status.good())
        status = EC_MemoryExhausted;
    return status;
}


OFCondition DcmItem::putAndInsertFloat64Array(const DcmTag& tag,
                                              const Float64 *value,
                                              const unsigned long count,
                                              const OFBool replaceOld)
{
    OFCondition status = EC_Normal;
    /* create new element */
    DcmElement *elem = NULL;
    switch(tag.getEVR())
    {
        case EVR_FD:
            elem = new DcmFloatingPointDouble(tag);
            break;
        case EVR_OD:
            elem = new DcmOtherDouble(tag);
            break;
        case EVR_UNKNOWN:
            /* Unknown VR, e.g. tag not found in data dictionary */
            status = EC_UnknownVR;
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* put value */
        status = elem->putFloat64Array(value, count);
        /* insert into dataset/item */
        if (status.good())
            status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else if (status.good())
        status = EC_MemoryExhausted;
    return status;
}


OFCondition DcmItem::putAndInsertTagKey(const DcmTag& tag,
                                        const DcmTagKey &value,
                                        const unsigned long pos,
                                        const OFBool replaceOld)
{
    OFCondition status = EC_Normal;
    /* create new element */
    DcmElement *elem = NULL;
    switch(tag.getEVR())
    {
        case EVR_AT:
            elem = new DcmAttributeTag(tag);
            break;
        case EVR_UNKNOWN:
            /* Unknown VR, e.g. tag not found in data dictionary */
            status = EC_UnknownVR;
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* put value */
        status = elem->putTagVal(value, pos);
        /* insert into dataset/item */
        if (status.good())
            status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else if (status.good())
        status = EC_MemoryExhausted;
    return status;
}


// ********************************


OFCondition DcmItem::insertEmptyElement(const DcmTag& tag,
                                        const OFBool replaceOld)
{
    OFCondition status = EC_Normal;
    /* create new element */
    DcmElement *elem = NULL;
    switch(tag.getEVR())
    {
        case EVR_AE:
            elem = new DcmApplicationEntity(tag);
            break;
        case EVR_AS:
            elem = new DcmAgeString(tag);
            break;
        case EVR_AT:
            elem = new DcmAttributeTag(tag);
            break;
        case EVR_CS:
            elem = new DcmCodeString(tag);
            break;
        case EVR_DA:
            elem = new DcmDate(tag);
            break;
        case EVR_DS:
            elem = new DcmDecimalString(tag);
            break;
        case EVR_DT:
            elem = new DcmDateTime(tag);
            break;
        case EVR_FL:
            elem = new DcmFloatingPointSingle(tag);
            break;
        case EVR_FD:
            elem = new DcmFloatingPointDouble(tag);
            break;
        case EVR_IS:
            elem = new DcmIntegerString(tag);
            break;
        case EVR_LO:
            elem = new DcmLongString(tag);
            break;
        case EVR_LT:
            elem = new DcmLongText(tag);
            break;
        case EVR_OB:
        case EVR_OW:
            elem = new DcmOtherByteOtherWord(tag);
            break;
        case EVR_OD:
            elem = new DcmOtherDouble(tag);
            break;
        case EVR_OF:
            elem = new DcmOtherFloat(tag);
            break;
        case EVR_OL:
            elem = new DcmOtherLong(tag);
            break;
        case EVR_PN:
            elem = new DcmPersonName(tag);
            break;
        case EVR_SH:
            elem = new DcmShortString(tag);
            break;
        case EVR_SL:
            elem = new DcmSignedLong(tag);
            break;
        case EVR_SQ:
            elem = new DcmSequenceOfItems(tag);
            break;
        case EVR_SS:
            elem = new DcmSignedShort(tag);
            break;
        case EVR_ST:
            elem = new DcmShortText(tag);
            break;
        case EVR_TM:
            elem = new DcmTime(tag);
            break;
        case EVR_UC:
            elem = new DcmUnlimitedCharacters(tag);
            break;
        case EVR_UI:
            elem = new DcmUniqueIdentifier(tag);
            break;
        case EVR_UL:
            elem = new DcmUnsignedLong(tag);
            break;
        case EVR_UR:
            elem = new DcmUniversalResourceIdentifierOrLocator(tag);
            break;
        case EVR_US:
            elem = new DcmUnsignedShort(tag);
            break;
        case EVR_UT:
            elem = new DcmUnlimitedText(tag);
            break;
        case EVR_PixelData:
            elem = new DcmPixelData(tag);
            // set VR to OW to make sure that we never write/send the internal VR
            if (elem) elem->setVR(EVR_OW);
            break;
        case EVR_OverlayData:
            elem = new DcmOverlayData(tag);
            // set VR to OW to make sure that we never write/send the internal VR
            if (elem) elem->setVR(EVR_OW);
            break;
        case EVR_UNKNOWN:
            /* Unknown VR, e.g. tag not found in data dictionary */
            status = EC_UnknownVR;
            break;
        case EVR_SV:
        case EVR_UV:
        case EVR_OV:
            // TODO: requires new VR classes
            DCMDATA_ERROR("DcmItem: Support for new VR=" << tag.getVRName() << " not yet implemented");
            status = EC_NotYetImplemented;
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* insert new element into dataset/item */
        status = insert(elem, replaceOld);
        /* could not be inserted, therefore, delete it immediately */
        if (status.bad())
            delete elem;
    } else if (status.good())
        status = EC_MemoryExhausted;
    return status;
}


OFCondition DcmItem::insertSequenceItem(const DcmTag &seqTag,
                                        DcmItem *item,
                                        const signed long itemNum)
{
    OFCondition status = EC_IllegalParameter;
    if (item != NULL)
    {
        DcmStack stack;
        /* find sequence */
        status = search(seqTag, stack, ESM_fromHere, OFFalse /*searchIntoSub*/);
        DcmSequenceOfItems *sequence = NULL;
        /* sequence found? */
        if (status.good())
        {
            /* get element */
            DcmElement *delem = OFstatic_cast(DcmElement *, stack.top());
            if (delem != NULL)
            {
                /* check VR */
                if ((delem->ident() == EVR_SQ) || (delem->ident() == EVR_pixelSQ))
                    sequence = OFstatic_cast(DcmSequenceOfItems *, delem);
                else
                    status = EC_InvalidVR;
            } else
                status = EC_CorruptedData;
        } else {
            /* create new sequence element */
            sequence = new DcmSequenceOfItems(seqTag);
            if (sequence != NULL)
            {
                /* insert into item/dataset */
                status = insert(sequence, OFTrue /*replaceOld*/);
                if (status.bad())
                    delete sequence;
            } else
                status = EC_MemoryExhausted;
        }
        if (status.good())
        {
            if (sequence != NULL)
            {
                const unsigned long count = sequence->card();
                /* 'itemNum' specifies and existing item? */
                if ((count > 0) && (itemNum >= -1) && (itemNum < OFstatic_cast(signed long, count)))
                {
                    if (itemNum == -1)
                    {
                        /* insert given item before last entry */
                        status = sequence->insert(item, count - 1, OFTrue /*before*/);
                    } else {
                        /* insert given item before specified entry */
                        status = sequence->insert(item, OFstatic_cast(unsigned long, itemNum), OFTrue /*before*/);
                    }
                /* create empty item(s) and append */
                } else {
                    DcmItem *newItem = NULL;
                    unsigned long i = 0;
                    /* create empty trailing items if required */
                    const unsigned long itemCount = (itemNum > OFstatic_cast(signed long, count)) ? (itemNum - count) : 0;
                    while ((i < itemCount) && status.good())
                    {
                        newItem = new DcmItem();
                        if (newItem != NULL)
                        {
                            /* append new item to end of sequence */
                            status = sequence->append(newItem);
                            if (status.bad())
                                delete newItem;
                        } else
                            status = EC_MemoryExhausted;
                        i++;
                    }
                    /* append given item to end of sequence */
                    status = sequence->append(item);
                }
            } else
                status = EC_IllegalCall;
        }
    }
    return status;
}


// ********************************


OFBool DcmItem::containsUnknownVR() const
{
    if (!elementList->empty())
    {
        elementList->seek(ELP_first);
        do {
            if (elementList->get()->containsUnknownVR())
                return OFTrue;
        } while (elementList->seek(ELP_next));
    }
    return OFFalse;
}


OFBool DcmItem::containsExtendedCharacters(const OFBool checkAllStrings)
{
    if (!elementList->empty())
    {
        elementList->seek(ELP_first);
        do {
            if (elementList->get()->containsExtendedCharacters(checkAllStrings))
                return OFTrue;
        } while (elementList->seek(ELP_next));
    }
    return OFFalse;
}


OFBool DcmItem::isAffectedBySpecificCharacterSet() const
{
    if (!elementList->empty())
    {
        elementList->seek(ELP_first);
        do {
            if (elementList->get()->isAffectedBySpecificCharacterSet())
                return OFTrue;
        } while (elementList->seek(ELP_next));
    }
    return OFFalse;
}


// ********************************


void DcmItem::updateSpecificCharacterSet(OFCondition &status,
                                         const DcmSpecificCharacterSet &converter)
{
    const OFString encoding = converter.getDestinationEncoding();
    if (status.good())
    {
        // check whether the attribute Specific Character Set (0008,0005) should be present at all
        if (checkForSpecificCharacterSet())
        {
            const OFString toCharset = converter.getDestinationCharacterSet();
            // check for default character set (ASCII), also make sure that the value "ISO_IR 6" is never used
            // in a dataset; open question: should we also check for non-ASCII characters in the element value?
            if (toCharset.empty() || (toCharset == "ISO_IR 6"))
            {
                // delete Specific Character Set (0008,0005) data element (type 1C)
                if (findAndDeleteElement(DCM_SpecificCharacterSet, OFFalse /*allOccurrences*/, OFFalse /*searchIntoSub*/).good())
                {
                    DCMDATA_DEBUG("DcmItem::convertCharacterSet() deleted element SpecificCharacterSet "
                        << DCM_SpecificCharacterSet << " during the conversion to " << encoding << " encoding");
                }
            } else {
                DCMDATA_DEBUG("DcmItem::convertCharacterSet() updating value of element SpecificCharacterSet "
                    << DCM_SpecificCharacterSet << " to '" << toCharset << "'");
                // update/set value of Specific Character Set (0008,0005) if needed
                status = putAndInsertOFStringArray(DCM_SpecificCharacterSet, toCharset);
            }
        } else {
            // otherwise delete it (if present)
            if (findAndDeleteElement(DCM_SpecificCharacterSet, OFFalse /*allOccurrences*/, OFFalse /*searchIntoSub*/).good())
            {
                DCMDATA_WARN("DcmItem: Deleted element SpecificCharacterSet " << DCM_SpecificCharacterSet
                    << " during the conversion to " << encoding << " encoding");
            }
        }
    } else {
        // an error occurred in a previous processing step
        DCMDATA_WARN("DcmItem: An error occurred during the conversion to " << encoding << " encoding, "
            << "the value of SpecificCharacterSet " << DCM_SpecificCharacterSet << " is not updated");
    }
}


OFCondition DcmItem::convertCharacterSet(const OFString &fromCharset,
                                         const OFString &toCharset,
                                         const size_t flags,
                                         const OFBool updateCharset)
{
    OFCondition status = EC_Normal;
    // if the item is empty, there is nothing to do
    if (!elementList->empty())
    {
        DcmSpecificCharacterSet converter;
        // create a new character set converter
        DCMDATA_DEBUG("DcmItem::convertCharacterSet() creating a new character set converter for '"
            << fromCharset << "'" << (fromCharset.empty() ? " (ASCII)" : "") << " to '"
            << toCharset << "'" << (toCharset.empty() ? " (ASCII)" : ""));
        // select source and destination character set
        status = converter.selectCharacterSet(fromCharset, toCharset);
        if (status.good())
        {
            unsigned cflags = 0;
            /* pass flags to underlying implementation */
            if (flags & DCMTypes::CF_discardIllegal)
                cflags |= OFCharacterEncoding::DiscardIllegalSequences;
            if (flags & DCMTypes::CF_transliterate)
                cflags |= OFCharacterEncoding::TransliterateIllegalSequences;
            if (cflags > 0)
                status = converter.setConversionFlags(cflags);
            if (status.good())
            {
                // convert all affected element values in the item
                status = convertCharacterSet(converter);
                if (updateCharset)
                {
                    // update the Specific Character Set (0008,0005) element
                    updateSpecificCharacterSet(status, converter);
                }
            }
        }
    }
    return status;
}


OFCondition DcmItem::convertCharacterSet(const OFString &toCharset,
                                         const size_t flags,
                                         const OFBool ignoreCharset)
{
    OFString fromCharset;
    // check whether this item can contain the attribute SpecificCharacterSet (0008,0005)
    if (checkForSpecificCharacterSet() && !ignoreCharset)
    {
        // determine value of Specific Character Set (0008,0005) if present in this item
        findAndGetOFStringArray(DCM_SpecificCharacterSet, fromCharset, OFFalse /*searchIntoSub*/);
    }
    // do the real work, if Specific Character Set is missing or empty use the default (ASCII)
    return convertCharacterSet(fromCharset, toCharset, flags, !ignoreCharset /*updateCharset*/);
}


OFCondition DcmItem::convertCharacterSet(DcmSpecificCharacterSet &converter)
{
    OFCondition status = EC_Normal;
    // if the item is empty, there is nothing to do
    if (!elementList->empty())
    {
        // iterate over all data elements in this item and convert the strings
        elementList->seek(ELP_first);
        do {
            status = elementList->get()->convertCharacterSet(converter);
        } while (status.good() && elementList->seek(ELP_next));
    }
    return status;
}


OFCondition DcmItem::convertToUTF8()
{
    // the DICOM defined term "ISO_IR 192" is used for "UTF-8"
    return convertCharacterSet("ISO_IR 192", 0 /*flags*/);
}


// ********************************


DcmElement* DcmItem::newDicomElement(const DcmTagKey& tag,
                                     const char *privateCreator)
{
  DcmTag temp(tag, privateCreator);
  DcmElement* elem = NULL;
  OFBool readAsUN = OFFalse;
  newDicomElement(
      elem,
      temp,
      0,          // Length
      NULL,       // Private creator
      readAsUN);  // read as VR UN (result ignored)
  return elem;
}


OFCondition DcmItem::newDicomElementWithVR(DcmElement*& newElement, const DcmTag& tag)
{
  DcmTag temp(tag);
  OFBool readAsUN = OFFalse;
  return newDicomElement(
      newElement,
      temp,
      0,          // Length
      NULL,       // Private creator
      readAsUN);  // read as VR UN (result ignored)
}


OFCondition DcmItem::newDicomElement(DcmElement*& newElement,
                                     const DcmTagKey& tag,
                                     const char *privateCreator)
{
    DcmTag temp(tag, privateCreator);
    newElement = NULL;
    OFBool readAsUN = OFFalse;
    return newDicomElement(
        newElement,
        temp,
        0,          // Length
        NULL,       // Private creator
        readAsUN);  // read as VR UN (result ignored)
}


OFCondition DcmItem::newDicomElement(DcmElement *&newElement,
                                     DcmTag &tag,
                                     const Uint32 length,
                                     DcmPrivateTagCache *privateCreatorCache,
                                     OFBool& readAsUN)
{
    /* initialize variables */
    OFCondition l_error = EC_Normal;
    newElement = NULL;
    DcmEVR evr = tag.getEVR();
    readAsUN = OFFalse;

    /* revert UN elements with finite length back to known VR if possible */
    if ((evr == EVR_UN) && (length != DCM_UndefinedLength) && dcmEnableUnknownVRConversion.get())
    {
        /* look up VR in data dictionary */
        DcmTag newTag(tag.getGroup(), tag.getElement());

        /* special handling for private elements */
        if (privateCreatorCache && (newTag.getGroup() & 1) && (newTag.getElement() >= 0x1000))
        {
            const char *pc = privateCreatorCache->findPrivateCreator(newTag);
            if (pc != NULL)
            {
                // we have a private creator for this element
                newTag.setPrivateCreator(pc);
                newTag.lookupVRinDictionary();
            }
        }

        /* update VR for tag, set "readAsUN" flag that makes sure the element value
         * is read in Little Endian Implicit VR (i.e. the UN encoding)
         */
        if (newTag.getEVR() != EVR_UNKNOWN)
        {
            tag.setVR(newTag.getVR());
            evr = tag.getEVR();
            readAsUN = OFTrue;
        }
    }

    /* depending on the VR of the tag which was passed, create the new object */
    switch (evr)
    {
        // byte strings:
        case EVR_AE :
            newElement = new DcmApplicationEntity(tag, length);
            break;
        case EVR_AS :
            newElement = new DcmAgeString(tag, length);
            break;
        case EVR_CS :
            newElement = new DcmCodeString(tag, length);
            break;
        case EVR_DA :
            newElement = new DcmDate(tag, length);
            break;
        case EVR_DS :
            newElement = new DcmDecimalString(tag, length);
            break;
        case EVR_DT :
            newElement = new DcmDateTime(tag, length);
            break;
        case EVR_IS :
            newElement = new DcmIntegerString(tag, length);
            break;
        case EVR_TM :
            newElement = new DcmTime(tag, length);
            break;
        case EVR_UI :
            newElement = new DcmUniqueIdentifier(tag, length);
            break;
        case EVR_UR:
            newElement = new DcmUniversalResourceIdentifierOrLocator(tag, length);
            break;

        // character strings:
        case EVR_LO :
            newElement = new DcmLongString(tag, length);
            break;
        case EVR_LT :
            newElement = new DcmLongText(tag, length);
            break;
        case EVR_PN :
            newElement = new DcmPersonName(tag, length);
            break;
        case EVR_SH :
            newElement = new DcmShortString(tag, length);
            break;
        case EVR_ST :
            newElement = new DcmShortText(tag, length);
            break;
        case EVR_UC:
            newElement = new DcmUnlimitedCharacters(tag, length);
            break;
        case EVR_UT:
            newElement = new DcmUnlimitedText(tag, length);
            break;

        // dependent on byte order:
        case EVR_AT :
            newElement = new DcmAttributeTag(tag, length);
            break;
        case EVR_SS :
            newElement = new DcmSignedShort(tag, length);
            break;
        case EVR_xs : // according to DICOM standard
        case EVR_US :
            newElement = new DcmUnsignedShort(tag, length);
            break;
        case EVR_SL :
            newElement = new DcmSignedLong(tag, length);
            break;
        case EVR_up : // for (0004,eeee) according to DICOM standard
        case EVR_UL :
            {
                // generate tag with VR from dictionary!
                DcmTag ulupTag(tag.getXTag());
                if (ulupTag.getEVR() == EVR_up)
                    newElement = new DcmUnsignedLongOffset(ulupTag, length);
                else
                    newElement = new DcmUnsignedLong(tag, length);
            }
            break;
        case EVR_OL :
            newElement = new DcmOtherLong(tag, length);
            break;
        case EVR_SV :
        case EVR_UV :
        case EVR_OV :
            // TODO: requires new VR classes; also need to add support for OV with undefined length
            DCMDATA_WARN("DcmItem: Support for new VR=" << tag.getVRName() << " not yet implemented, treating as UN/OB");
            // until dedicated support is available, treat as OB
            newElement = new DcmOtherByteOtherWord(tag, length);
            break;
        case EVR_FL :
            newElement = new DcmFloatingPointSingle(tag, length);
            break;
        case EVR_FD :
            newElement = new DcmFloatingPointDouble(tag, length);
            break;
        case EVR_OF :
            newElement = new DcmOtherFloat(tag, length);
            break;
        case EVR_OD :
            newElement = new DcmOtherDouble(tag, length);
            break;

        // sequences and items:
        case EVR_SQ :
            newElement = new DcmSequenceOfItems(tag, length);
            break;
        case EVR_na :
            if (tag.getXTag() == DCM_Item)
                l_error = EC_InvalidTag;
            else if (tag.getXTag() == DCM_SequenceDelimitationItem)
                l_error = EC_SequEnd;
            else if (tag.getXTag() == DCM_ItemDelimitationItem)
                l_error = EC_ItemEnd;
            else
                l_error = EC_InvalidTag;
            break;

        // pixel sequences (EVR_pixelSQ) are handled through class DcmPixelData
        // and should never appear here.

        // unclear 8 or 16 bit:
        case EVR_ox :
            if (tag == DCM_PixelData)
                newElement = new DcmPixelData(tag, length);
            else if (tag.getBaseTag() == DCM_OverlayData)
                newElement = new DcmOverlayData(tag, length);
            else
                /* we don't know this element's real transfer syntax, so we just
                 * use the defaults of class DcmOtherByteOtherWord and let the
                 * application handle it.
                 */
                newElement = new DcmOtherByteOtherWord(tag, length);
            break;

        // This case should only occur if we encounter an element with an invalid
        // "Pi" VR. Make sure this does not cause problems later on
        case EVR_PixelData :
            newElement = new DcmPixelData(tag, length);
            // set VR to OW to make sure that we never write/send the internal VR
            if (newElement) newElement->setVR(EVR_OW);
            break;

        // This case should only occur if we encounter an element with an invalid
        // "Ov" VR. Make sure this does not cause problems later on
        case EVR_OverlayData :
            newElement = new DcmOverlayData(tag, length);
            // set VR to OW to make sure that we never write/send the internal VR
            if (newElement) newElement->setVR(EVR_OW);
            break;

        case EVR_lt :
            newElement = new DcmOtherByteOtherWord(tag, length);
            break;

        case EVR_OB :
        case EVR_OW :
            if (tag == DCM_PixelData)
                newElement = new DcmPixelData(tag, length);
            else if (tag.getBaseTag() == DCM_OverlayData)
                newElement = new DcmOverlayData(tag, length);
            else if ((tag == DCM_VOILUTSequence) && (length != DCM_UndefinedLength))
            {
                // this is an incorrectly encoded VOI LUT Sequence.
                // Real-world examples of this issue have been reported in 2016.
                if (dcmConvertVOILUTSequenceOWtoSQ.get())
                {
                  // Silently fix the error by interpreting as a sequence.
                  DcmTag newTag(tag);
                  newTag.setVR(DcmVR(EVR_SQ)); // on writing we will handle this element as SQ, not OB/OW
                  newElement = new DcmSequenceOfItems(newTag, length);
                } else {

                    if (dcmIgnoreParsingErrors.get())
                    {
                        // ignore parse error, keep VR unchanged
                        DCMDATA_WARN("DcmItem: VOI LUT Sequence with VR=OW and explicit length encountered.");
                        newElement = new DcmOtherByteOtherWord(tag, length);
                    }
                    else
                    {
                        // bail out with an error
                        DCMDATA_ERROR("DcmItem: VOI LUT Sequence with VR=OW and explicit length encountered.");
                        l_error = EC_VOI_LUT_OBOW;
                    }
                }
            }
            else
                if (length == DCM_UndefinedLength)
                {
                    // The attribute is OB or OW but is encoded with undefined
                    // length, and it is not Pixel Data. This is illegal.
                    if (dcmConvertUndefinedLengthOBOWtoSQ.get())
                    {
                        // Assume that this is in fact a sequence so that we can
                        // catch the sequence delimitation item.
                        DcmTag newTag(tag);
                        newTag.setVR(DcmVR(EVR_SQ)); // on writing we will handle this element as SQ, not OB/OW
                        newElement = new DcmSequenceOfItems(newTag, length);
                    } else {
                        if (dcmIgnoreParsingErrors.get())
                        {
                            // ignore parse error, keep VR unchanged
                            OFCondition tempcond = EC_UndefinedLengthOBOW;
                            DCMDATA_WARN("DcmItem: Parse error in " << tag << ": " << tempcond.text());
                            newElement = new DcmSequenceOfItems(tag, length);
                        }
                        else
                        {
                            // bail out with an error
                            l_error = EC_UndefinedLengthOBOW;
                            DCMDATA_ERROR("DcmItem: Parse error in " << tag << ": " << l_error.text());
                        }
                    }
                } else {
                    newElement = new DcmOtherByteOtherWord(tag, length);
                }
            break;

        // read unknown types as byte string:
        case EVR_UNKNOWN :
        case EVR_UNKNOWN2B :
        case EVR_UN :
            if (length == DCM_UndefinedLength)
            {
                // The attribute VR is UN with undefined length. Assume it is really
                // a sequence so that we can catch the sequence delimitation item.
                DcmTag newTag(tag);
                newTag.setVR(DcmVR(EVR_SQ)); // on writing we will handle this element as SQ, not UN
                if (dcmEnableCP246Support.get())
                {
                    DCMDATA_WARN("Found element " << newTag << " with VR UN and undefined length, "
                        << "reading a sequence with transfer syntax LittleEndianImplicit (CP-246)");
                } else {
                    DCMDATA_WARN("Found element " << newTag << " with VR UN and undefined length");
                }
                newElement = new DcmSequenceOfItems(newTag, length, dcmEnableCP246Support.get());
            } else {
                // defined length UN element, treat like OB
                newElement = new DcmOtherByteOtherWord(tag, length);
            }
            break;

        // these types should never occur
        case EVR_item :
        case EVR_metainfo :
        case EVR_dataset :
        case EVR_fileFormat :
        case EVR_dicomDir :
        case EVR_dirRecord :
        case EVR_pixelSQ :
        case EVR_pixelItem :
            l_error = EC_IllegalCall;
            break;

        // we deliberately have no default clause to make sure a warning is raised
        // when an DcmEVR enum is not explicitly handled here
    }

    /* check for valid element pointer */
    if (l_error.good() && (newElement == NULL))
        l_error = EC_MemoryExhausted;

    /* return result value */
    return l_error;
}
