/*
 *
 *  Copyright (C) 1994-2010, OFFIS e.V.
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
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-11-02 15:31:09 $
 *  CVS/RCS Revision: $Revision: 1.150 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
#include "dcmtk/dcmdata/dcvrof.h"
#include "dcmtk/dcmdata/dcvrpn.h"
#include "dcmtk/dcmdata/dcvrsh.h"
#include "dcmtk/dcmdata/dcvrsl.h"
#include "dcmtk/dcmdata/dcvrss.h"
#include "dcmtk/dcmdata/dcvrst.h"
#include "dcmtk/dcmdata/dcvrtm.h"
#include "dcmtk/dcmdata/dcvrui.h"
#include "dcmtk/dcmdata/dcvrul.h"
#include "dcmtk/dcmdata/dcvrulup.h"
#include "dcmtk/dcmdata/dcvrus.h"
#include "dcmtk/dcmdata/dcvrut.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstring.h"
#include "dcmtk/ofstd/ofcast.h"
#include "dcmtk/ofstd/ofstd.h"


// ********************************


DcmItem::DcmItem()
  : DcmObject(ItemTag),
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
            elementList->insert(old.elementList->get()->clone(), ELP_next);
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
        elementList->insert(obj.elementList->get()->clone(), ELP_next);
      } while (obj.elementList->seek(ELP_next));
    }
  }
  return *this;
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


E_TransferSyntax DcmItem::checkTransferSyntax(DcmInputStream & inStream)
{
    E_TransferSyntax transferSyntax;
    Uint8 tagAndVR[6];

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
        /* in case we encounterd an invalid tag, we want to assume that the used transfer syntax */
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
        /* in case we encounterd a valid tag, we want to figure out, if it is an implicit or */
        /* explicit transfer syntax. Hence, check if the last 2 bytes represent a valid VR. */
        if (foundVR(&tagAndVR[4]))
        {
            /* having figured out that the last 2 bytes represent a valid */
            /* VR, we need to find out which of the two tags was valid */
            if (taglittle.error().bad())
            {
                /* if the litte endian tag was invalid, the transfer syntax is big endian explicit */
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
    /* dump information on a certain debug level */
    DCMDATA_DEBUG("DcmItem::checkTransferSyntax() TransferSyntax=\""
        << DcmXfer(transferSyntax).getXferName() << "\"");

    /* return determined transfer syntax */
    return transferSyntax;
}


// ********************************


void DcmItem::checkAndUpdateVR(DcmItem &item,
                               DcmTag &tag)
{
    /* handle special cases where the VR can be determined by some other element values */
    if (((tag == DCM_WaveformData) || (tag == DCM_WaveformPaddingValue)) && (tag.getEVR() == EVR_ox))
    {
        /* case 1 (WaveformData and others): see section 8.3 in PS 3.5 */
        Uint16 bitsAlloc;
        if (item.findAndGetUint16(DCM_WaveformBitsAllocated, bitsAlloc).good())
        {
            if (bitsAlloc == 8)
            {
                DCMDATA_DEBUG("setting undefined VR of " << tag.getTagName() << " " << tag << " to 'OB' because WaveformBitsAllocated "
                    << DCM_WaveformBitsAllocated << " has a value of 8");
                tag.setVR(EVR_OB);
            } else {
                DCMDATA_DEBUG("setting undefined VR of " << tag.getTagName() << " " << tag << " to 'OW' because WaveformBitsAllocated "
                    << DCM_WaveformBitsAllocated << " has a value that is different from 8");
                tag.setVR(EVR_OW);
            }
        }
    }
    else if (((tag == DCM_PixelPaddingValue) || (tag == DCM_PixelPaddingRangeLimit) ||
        (tag == DCM_HistogramFirstBinValue) || (tag == DCM_HistogramLastBinValue)) && (tag.getEVR() == EVR_xs))
    {
        /* case 2 (PixelPaddingValue and others): see section C.7.5.1 and C.11.5 in PS 3.3 */
        Uint16 pixelRep;
        if (item.findAndGetUint16(DCM_PixelRepresentation, pixelRep).good())
        {
            if (pixelRep == 0x0001)
            {
                DCMDATA_DEBUG("setting undefined VR of " << tag.getTagName() << " " << tag << " to 'SS' because PixelRepresentation "
                    << DCM_PixelRepresentation << " has a value of 1");
                tag.setVR(EVR_SS);
            } else {
                DCMDATA_DEBUG("setting undefined VR of " << tag.getTagName() << " " << tag << " to 'US' because PixelRepresentation "
                    << DCM_PixelRepresentation << " has a value that is different from 1");
                tag.setVR(EVR_US);
            }
        }
    }
    else if (((tag == DCM_OverlayData) || (tag == DCM_PixelData)) && (tag.getEVR() == EVR_ox))
    {
        /* case 3 (OverlayData and PixelData): see section 8.1.2 and 8.2 in PS 3.5 */
        DCMDATA_DEBUG("setting undefined VR of " << tag.getTagName() << " " << tag << " to 'OW'");
        tag.setVR(EVR_OW);
    }
    else if ((tag == DCM_RETIRED_CurveData) && (tag.getEVR() == EVR_ox))
    {
        /* case 4 (CurveData): see section A.1 in PS 3.5-2004 */
        DCMDATA_DEBUG("setting undefined VR of " << tag.getTagName() << " " << tag << " to 'OB'");
        tag.setVR(EVR_OB);
    }
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
        DcmTag delimItemTag(DCM_ItemDelimitationItem);
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
    /* XML start tag for "item" */
    out << "<item";
    /* cardinality (number of attributes) = 1..n */
    out << " card=\"" << card() << "\"";
    /* value length in bytes = 0..max (if not undefined) */
    if (getLengthField() != DCM_UndefinedLength)
        out << " len=\"" << getLengthField() << "\"";
    out << ">" << OFendl;
    /* write item content */
    if (!elementList->empty())
    {
        /* write content of all children */
        DcmObject *dO;
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            dO->writeXML(out, flags);
        } while (elementList->seek(ELP_next));
    }
    /* XML end tag for "item" */
    out << "</item>" << OFendl;
    /* always report success */
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
    Uint32 headersize = xferSyn.sizeofTagHeader(getVR());
    /* Length of item's content, i.e. contained elements */
    itemlen = getLength(xfer, enctype);
    /* Since the item's total length can exceed the maximum length of 32 bit, it is
     * always necessary to check for overflows. The approach taken is not elegant
     * but should work...
     */
    if ( (itemlen == DCM_UndefinedLength) || OFStandard::check32BitAddOverflow(itemlen, headersize) )
      return DCM_UndefinedLength;
    itemlen += xferSyn.sizeofTagHeader(getVR());
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
                        /* is not UL replace this element with one that has a UL datatype since */
                        /* group length elements are supposed to have this data type */
                        if (dO->getETag() == 0x0000 && dO->ident() != EVR_UL)
                        {
                            delete elementList->remove();
                            DcmTag tagUL(actGrp, 0x0000, EVR_UL);
                            DcmUnsignedLong *dUL = new DcmUnsignedLong(tagUL);
                            elementList->insert(dUL, ELP_prev);
                            dO = dUL;
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
                        }

                        /* in case we want to add padding elements and the current element is a */
                        /* padding element we want to remember the padding element so that the */
                        /* group length of this element can be stored later */
                        if (padenc == EPD_withPadding && actGrp == 0xfffc)
                            paddingGL = OFstatic_cast(DcmUnsignedLong *, dO);

                        /* if actGLElem contains a valid pointer it was set in one of the last iterations */
                        /* to the group lenght element of the last group. We need to write the current computed */
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

                /* create an array of a corresponding size and set the arrayfields */
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
    Uint32 numElems = exceededGroupLengthElems.size();
    for (Uint32 i=0; i < numElems; i++)
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
    DcmEVR nxtobj = EVR_UNKNOWN;
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
    /* occured while performing this check return this error */
    if (inStream.avail() < (xferSyn.isExplicitVR() ? 6u:4u))
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
    // check whether tag is private
    OFBool isPrivate = groupTag & 1;

    /* if the transfer syntax which was passed is an explicit VR syntax and if the current */
    /* item is not a delimitation item (note that delimitation items do not have a VR), go */
    /* ahead and read 2 bytes from inStream. These 2 bytes contain this item's VR value. */
    if (xferSyn.isExplicitVR() && newTag.getEVR() != EVR_na)
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
        }

        /* set the VR which was read in the above created tag object. */
        newTag.setVR(vr);

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
            }
        }
    }

    /* determine this item's VR */
    nxtobj = newTag.getEVR();

    /* the next thing we want to do is read the value in the length field from inStream. */
    /* determine if there is a corresponding amount of bytes (for the length field) still */
    /* available in inStream. If not, return an error. */
    if (inStream.avail() < xferSyn.sizeofTagHeader(nxtobj) - bytesRead)
    {
        inStream.putback();    // the UnsetPutbackMark is in readSubElement
        bytesRead = 0;
        l_error = EC_StreamNotifyClient;
        return l_error;
    }

    /* read the value in the length field. In some cases, it is 4 bytes wide, in other */
    /* cases only 2 bytes (see DICOM standard part 5, section 7.1.1) */
    if (xferSyn.isImplicitVR() || nxtobj == EVR_na)   //note that delimitation items don't have a VR
    {
        inStream.read(&valueLength, 4);            //length field is 4 bytes wide
        swapIfNecessary(gLocalByteOrder, byteOrder, &valueLength, 4, 4);
        bytesRead += 4;
    } else {                                       //the transfer syntax is explicit VR
        DcmVR vr(newTag.getEVR());
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
    }
    /* if the value in length is odd, print an error message */
    if ( (valueLength & 1) && (valueLength != DCM_UndefinedLength) )
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
        if (valueLength > remainingItemBytes)
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
    OFCondition l_error = newDicomElement(subElem, newTag, newLength, &privateCreatorCache, readAsUN);

    /* if no error occured and subElem does not equal NULL, go ahead */
    if (l_error.good() && subElem != NULL)
    {
        // inStream.UnsetPutbackMark(); // not needed anymore with new stream architecture

        /* insert the new element into the (sorted) element list and */
        /* assign information which was read from the instream to it */
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
                << " found twice in one dataset/item, ignoring second entry");
            delete subElem;
        }
    }
    /* else if an error occured, try to recover from this error */
    else if (l_error == EC_InvalidTag)
    {
        /* This is the second Putback operation on the putback mark in */
        /* readTagAndLength but it is impossible that both can be executed */
        /* without setting the Mark twice. */
        inStream.putback();
        DCMDATA_WARN("DcmItem: Parse error while parsing element " << newTag);
    }
    else if (l_error != EC_ItemEnd)
    {
        // inStream.UnsetPutbackMark(); // not needed anymore with new stream architecture
        DCMDATA_WARN("DcmItem: Parse error in sequence item, found " << newTag
            << " instead of an item delimiter");
    } else {
        // inStream.UnsetPutbackMark(); // not needed anymore with new stream architecture
    }

    /* return result value */
    return l_error;
}


// ********************************


OFCondition DcmItem::read(DcmInputStream & inStream,
                          const E_TransferSyntax xfer,
                          const E_GrpLenEncoding glenc,
                          const Uint32 maxReadLength)
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
        DcmTag newTag; OFBool readStopElem = OFFalse;
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
                else /* continue with normal case: parse rest of element */
                {
                    /* if there was an error while we were reading from the stream, terminate the while-loop */
                    /* (note that if the last element had been read from the instream in the last iteration, */
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
                    /* read the actual data value which belongs to this element */
                    /* (attribute) and insert this information into the elementList */
                    errorFlag = readSubElement(inStream, newTag, newValueLength, xfer, glenc, maxReadLength);
                    /* if reading was successful, we read the entire data value information */
                    /* for this element; hence lastElementComplete is true again */
                    if (errorFlag.good())
                        lastElementComplete = OFTrue;
                }
            } else
            {
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
                            << " encountered, skipping rest of dataset");
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
    /* modify the result value: two kinds of special error codes do not count as an error */
    if (errorFlag == EC_ItemEnd || errorFlag == EC_EndOfStream)
        errorFlag = EC_Normal;

    /* if at this point the error flag indicates success, the item has */
    /* been read completely; hence, set the transfer state to ERW_ready. */
    /* Note that all information for this element could be read from the */
    /* stream, the errorFlag is still set to EC_StreamNotifyClient. */
    if (errorFlag.good())
        setTransferState(ERW_ready);

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
                DcmTag delim(DCM_ItemDelimitationItem);
                errorFlag = writeTag(outStream, delim, oxfer);
                Uint32 delimLen = 0L;
                outStream.write(&delimLen, 4); // 4 bytes length
            }
            else
            {
                // Every subelement of the item is written but it
                // is not possible to write the delimination item into the buffer.
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

OFCondition DcmItem::writeSignatureFormat(
  DcmOutputStream &outStream,
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


// ********************************


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


unsigned long DcmItem::card() const
{
    return elementList->card();
}


// ********************************


OFCondition DcmItem::insert(DcmElement *elem,
                            OFBool replaceOld,
                            OFBool checkInsertOrder)
{
    /* intialize error flag with ok */
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
                    << " VR=\"" << DcmVR(elem->getVR()).getVRName() << "\" at beginning inserted");
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
        elementList->remove();          // removes element from list but does not delete it
    else
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
    DCMDATA_TRACE("DcmItem::verify() Element " << getTag()
        << " VR=\"" << DcmVR(getVR()).getVRName() << "\"");

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

// Support functions

OFCondition newDicomElement(DcmElement *&newElement,
                            const DcmTag &tag,
                            const Uint32 length)
{
    DcmTag newTag(tag);
    OFBool readAsUN = OFFalse;
    return newDicomElement(newElement, newTag, length, NULL, readAsUN);
}

DcmElement *newDicomElement(const DcmTag &tag,
                            const Uint32 length)
{
    DcmElement *newElement = NULL;
    newDicomElement(newElement, tag, length);
    return newElement;
}


// ********************************


OFCondition newDicomElement(DcmElement *&newElement,
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
        case EVR_FL :
            newElement = new DcmFloatingPointSingle(tag, length);
            break;
        case EVR_FD :
            newElement = new DcmFloatingPointDouble(tag, length);
            break;
        case EVR_OF :
            newElement = new DcmOtherFloat(tag, length);
            break;

        // sequences and items
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
            else if (((tag.getGTag() & 0xffe1) == 0x6000) && (tag.getETag() == 0x3000)) // DCM_OverlayData
                newElement = new DcmOverlayData(tag, length);
            else
                /* we don't know this element's real transfer syntax, so we just
                 * use the defaults of class DcmOtherByteOtherWord and let the
                 * application handle it.
                 */
                newElement = new DcmOtherByteOtherWord(tag, length);
            break;

        case EVR_lt :
            newElement = new DcmOtherByteOtherWord(tag, length);
            break;

        case EVR_OB :
        case EVR_OW :
            if (tag == DCM_PixelData)
                newElement = new DcmPixelData(tag, length);
            else if (((tag.getGTag() & 0xffe1) == 0x6000) && (tag.getETag() == 0x3000)) // DCM_OverlayData
                newElement = new DcmOverlayData(tag, length);
            else
                if (length == DCM_UndefinedLength)
                {
                    // The attribute is OB or OW but is encoded with undefined
                    // length.  Assume it is really a sequence so that we can
                    // catch the sequence delimitation item.
                    newElement = new DcmSequenceOfItems(tag, length);
                } else {
                    newElement = new DcmOtherByteOtherWord(tag, length);
                }
            break;

        // read unknown types as byte string:
        case EVR_UNKNOWN :
        case EVR_UNKNOWN2B :
        case EVR_UN :
        default :
            if (length == DCM_UndefinedLength)
            {
              // The attribute VR is UN with undefined length. Assume it is
              // really a sequence so that we can catch the sequence delimitation item.
              DcmVR sqVR(EVR_SQ); // on writing we will handle this element as SQ, not UN
              DcmTag newTag(tag.getXTag(), sqVR);
              newElement = new DcmSequenceOfItems(newTag, length, dcmEnableCP246Support.get());
            } else {
                // defined length UN element, treat like OB
                newElement = new DcmOtherByteOtherWord(tag, length);
            }
            break;
    }

    /* check for valid element pointer */
    if (l_error.good() && (newElement == NULL))
        l_error = EC_MemoryExhausted;

    /* return result value */
    return l_error;
}


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
** Simple tests for existance
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
    DcmElement *elem = NULL;
    Uint32 len = 0;
    DcmStack stack;

    OFCondition ec = search(key, stack, ESM_fromHere, searchIntoSub);
    elem = OFstatic_cast(DcmElement *, stack.top());
    if (ec.good() && elem != NULL)
        len = elem->getLength();

    return (ec.good()) && (len > 0);
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
            *count = elem->getLength() / sizeof(Uint8);
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
            *count = elem->getLength() / sizeof(Uint16);
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
            *count = elem->getLength() / sizeof(Sint16);
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
            *count = elem->getLength() / sizeof(Uint32);
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
            *count = elem->getLength() / sizeof(Sint32);
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
            *count = elem->getLength() / sizeof(Float32);
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
            *count = elem->getLength() / sizeof(Float64);
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
        case EVR_PN:
            elem = new DcmPersonName(tag);
            break;
        case EVR_OB:
        case EVR_OW:
            elem = new DcmOtherByteOtherWord(tag);
            break;
        case EVR_OF:
            elem = new DcmOtherFloat(tag);
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
        case EVR_UI:
            elem = new DcmUniqueIdentifier(tag);
            break;
        case EVR_UL:
            elem = new DcmUnsignedLong(tag);
            break;
        case EVR_US:
            elem = new DcmUnsignedShort(tag);
            break;
        case EVR_UT:
            elem = new DcmUnlimitedText(tag);
            break;
        default:
            status = EC_IllegalCall;
            break;
    }
    if (elem != NULL)
    {
        /* put value */
        status = elem->putString(value);
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
        case EVR_UI:
            elem = new DcmUniqueIdentifier(tag);
            break;
        case EVR_UT:
            elem = new DcmUnlimitedText(tag);
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
    DcmElement *elem = NULL;
    /* create new element */
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
                elem = new DcmPixelData(tag);
            else
                elem = new DcmPolymorphOBOW(tag);
            break;
        case EVR_xs:
            /* special handling */
            elem = new DcmUnsignedShort(DcmTag(tag, EVR_US));
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
    DcmElement *elem = NULL;
    /* create new element */
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
    DcmElement *elem = NULL;
    /* create new element */
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
    OFCondition status = EC_IllegalCall;
    /* create new element */
    if (tag.getEVR() == EVR_UL)
    {
        DcmElement *elem = new DcmUnsignedLong(tag);
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
    }
    return status;
}


OFCondition DcmItem::putAndInsertSint32(const DcmTag& tag,
                                        const Sint32 value,
                                        const unsigned long pos,
                                        const OFBool replaceOld)
{
    OFCondition status = EC_IllegalCall;
    /* create new element */
    if (tag.getEVR() == EVR_SL)
    {
        DcmElement *elem = new DcmSignedLong(tag);
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
    }
    return status;
}


OFCondition DcmItem::putAndInsertFloat32(const DcmTag& tag,
                                         const Float32 value,
                                         const unsigned long pos,
                                         const OFBool replaceOld)
{
    OFCondition status = EC_IllegalCall;
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


OFCondition DcmItem::putAndInsertFloat64(const DcmTag& tag,
                                         const Float64 value,
                                         const unsigned long pos,
                                         const OFBool replaceOld)
{
    OFCondition status = EC_IllegalCall;
    /* create new element */
    if (tag.getEVR() == EVR_FD)
    {
        DcmElement *elem = new DcmFloatingPointDouble(tag);
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
    }
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
        case EVR_OF:
            elem = new DcmOtherFloat(tag);
            break;
        case EVR_OB:
        case EVR_OW:
            elem = new DcmOtherByteOtherWord(tag);
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
        case EVR_UI:
            elem = new DcmUniqueIdentifier(tag);
            break;
        case EVR_UL:
            elem = new DcmUnsignedLong(tag);
            break;
        case EVR_US:
            elem = new DcmUnsignedShort(tag);
            break;
        case EVR_UT:
            elem = new DcmUnlimitedText(tag);
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


/*
** CVS/RCS Log:
** $Log: dcitem.cc,v $
** Revision 1.150  2010-11-02 15:31:09  joergr
** Added special handling for data elements that are associated with different
** VRs (according to the data dictinary) when read with an implicit transfer
** syntax, e.g. PixelPaddingValue or WaveformData.
**
** Revision 1.149  2010-10-20 16:44:16  joergr
** Use type cast macros (e.g. OFstatic_cast) where appropriate.
**
** Revision 1.148  2010-10-14 13:14:08  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.147  2010-10-01 13:55:04  joergr
** Added new helper function findAndInsertCopyOfElement().
**
** Revision 1.146  2010-06-23 08:50:25  joergr
** Moved log output to another code line in order to avoid a possible NULL
** pointer dereference.
**
** Revision 1.145  2010-03-25 16:31:42  joergr
** Use return value of getTag() for stream output where possible.
** Made log messages more consistent within this module.
**
** Revision 1.144  2010-03-24 14:11:54  onken
** Fixed compilation problem introduced with recent assignment operator changes.
**
** Revision 1.143  2010-03-24 11:56:36  onken
** Fixed memory leak in assignment operators of DcmItem and DcmSequenceOfItems.
** Replaced all code occurences of cleaning all elements from internal lists to
** newly introduced function in DcmList.
**
** Revision 1.142  2009-12-04 16:54:54  joergr
** Sightly modified some log messages.
**
** Revision 1.141  2009-11-13 13:11:20  joergr
** Fixed minor issues in log output.
**
** Revision 1.140  2009-11-04 09:58:09  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.139  2009-09-28 13:30:59  joergr
** Moved general purpose definition file from module dcmdata to ofstd, and
** added new defines in order to make the usage easier.
**
** Revision 1.138  2009-08-31 15:21:57  joergr
** Fixed bug in group length computation which could cause a segmentation fault
** for incorrectly encoded DICOM datasets (with illegal group length elements).
**
** Revision 1.137  2009-08-07 14:35:49  joergr
** Enhanced isEmpty() method by checking whether the data element value consists
** of non-significant characters only.
**
** Revision 1.136  2009-05-15 09:20:09  joergr
** Fixed wrong comments in print() method.
**
** Revision 1.135  2009-04-20 16:02:09  joergr
** Fixed typo.
**
** Revision 1.134  2009-03-25 10:21:21  joergr
** Added new method isEmpty() to DICOM object, item and sequence class.
**
** Revision 1.133  2009-03-18 13:54:04  joergr
** Fixed "number overflow" issue with error message on non-standard VR.
**
** Revision 1.132  2009-03-05 14:08:05  onken
** Fixed typo.
**
** Revision 1.131  2009-03-05 13:35:07  onken
** Added checks for sequence and item lengths which prevents overflow in length
** field, if total length of contained items (or sequences) exceeds 32-bit
** length field. Also introduced new flag (default: enabled) for writing
** in explicit length mode, which allows for automatically switching encoding
** of only that very sequence/item to undefined length coding (thus permitting
** to actually write the file).
**
** Revision 1.130  2009-02-18 12:22:11  meichel
** Minor changes needed for VC6
**
** Revision 1.129  2009-02-16 16:09:36  onken
** Fixed bug that caused incorrect error message when parsing undefined length
** sequences inside defined length items.
**
** Revision 1.128  2009-02-11 13:16:36  onken
** Added global parser flag permitting to stop parsing after a specific
** element was parsed on dataset level (useful for removing garbage at
** end of file).
**
** Revision 1.127  2009-02-05 14:59:43  onken
** Make usage of global "ignore parsing errors" flag in case of elements
** being larger than rest of available input. However, if enabled, the
** parser ignores any elements coming after such an input-exceeding
** element. Minor code clarifications.
**
** Revision 1.126  2009-02-05 10:39:52  joergr
** Made new error messages more consistent with existing messages.
**
** Revision 1.125  2009-02-04 18:07:30  joergr
** Minor fixes and code restructuring (without changing the semantics).
** Fixed various type mismatches reported by MSVC introduced with OFFile class.
**
** Revision 1.124  2009-02-04 14:06:01  onken
** Changed parser to make use of the new error ignoring flag when parsing.
** Added check (makes use of new flag) that notes whether an element's value is
** specified larger than the surrounding item (applicable for explicit length
** coding).
**
** Revision 1.123  2009-01-29 15:35:32  onken
** Added global parsing option that allows for reading private attributes in
** implicit encoding having a maximum length to be read as sequences instead
** of relying on the dictionary.
**
** Revision 1.122  2009-01-06 16:27:03  joergr
** Reworked print() output format for option PF_showTreeStructure.
**
** Revision 1.121  2009-01-05 15:31:42  joergr
** Added global flag that allows for reading incorrectly encoded DICOM datasets
** where particular data elements are encoded with a differing transfer syntax
** (Implicit VR Little endian instead of Explicit VR encoding as declared).
**
** Revision 1.120  2008-12-12 11:44:41  onken
** Moved path access functions to separate classes
**
** Revision 1.119  2008-12-05 13:51:13  onken
** Introduced new error code number for specific findOrCreatePath() errors.
**
** Revision 1.118  2008-12-05 13:28:01  onken
** Splitted findOrCreatePath() function API for also offering a simple API
** for non-wildcard searches.
**
** Revision 1.117  2008-12-04 16:54:54  onken
** Changed findOrCreatePath() to also support wildcard as item numbers.
**
** Revision 1.116  2008-11-26 12:16:52  joergr
** Slightly changed behavior of newDicomElement(): return error code if new
** element could not be created (e.g. because memory is exhausted).
**
** Revision 1.115  2008-11-21 16:18:11  onken
** Changed implementation of findOrCreatePath() to make use of function
** newDicomElement() which also knows how to handle EVRs like ox correctly.
**
** Revision 1.114  2008-10-15 15:49:09  onken
** Fixed incorrect behaviour of findOrCreatePath() if sequence is the last
** attribute to be inserted.
**
** Revision 1.113  2008-10-15 12:31:24  onken
** Added findOrCreatePath() functions which allow for finding or creating a
** hierarchy of sequences, items and attributes according to a given "path"
** string.
**
** Revision 1.112  2008-09-24 13:32:10  joergr
** Fixed typo in comment.
**
** Revision 1.111  2008-07-17 10:31:31  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.110  2007-12-11 12:21:57  onken
** Corrected some putAndInsert functions that returned wrong error code
** (EC_MemoryExhausted) in some cases.
**
** Revision 1.109  2007-11-29 14:30:21  meichel
** Write methods now handle large raw data elements (such as pixel data)
**   without loading everything into memory. This allows very large images to
**   be sent over a network connection, or to be copied without ever being
**   fully in memory.
**
** Revision 1.108  2007/09/21 10:40:18  onken
** Changed foundVR() API and implementation to use Uint8* instead of char* to
** avoid calls to isalpha() with negative arguments (undef. behaviour/assertion)
**
** Revision 1.107  2007/06/29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.106  2007/06/08 15:18:47  joergr
** Fixed typo that caused MSVC to report an error.
**
** Revision 1.105  2007/06/08 15:12:56  joergr
** Added new helper functions insertSequenceItem(), findAndDeleteSequenceItem().
** Replaced helper function findAndCopyElement() by new optional parameter
** 'createCopy' in various findAndGetXXX() functions.
**
** Revision 1.104  2007/03/09 10:36:09  joergr
** Added support for missing VRs (SL, SS, UL, SS) to insertEmptyElement().
**
** Revision 1.103  2007/02/19 15:04:16  meichel
** Removed searchErrors() methods that are not used anywhere and added
**   error() methods only in the DcmObject subclasses where really used.
**
** Revision 1.102  2006/12/15 14:14:44  joergr
** Added new method that checks whether a DICOM object or element is affected
** by SpecificCharacterSet (0008,0005).
**
** Revision 1.101  2006/12/13 13:59:49  joergr
** Added new optional parameter "checkAllStrings" to method containsExtended
** Characters().
**
** Revision 1.100  2006/08/15 15:49:54  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.99  2006/05/11 08:49:18  joergr
** Moved checkForNonASCIICharacters() from application to library.
**
** Revision 1.98  2006/01/18 13:44:03  joergr
** Translated remaining German comment.
**
** Revision 1.97  2005/12/08 15:41:16  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.96  2005/11/28 15:53:13  meichel
** Renamed macros in dcdebug.h
**
** Revision 1.95  2005/11/15 18:28:04  meichel
** Added new global flag dcmEnableUnknownVRConversion that enables the automatic
**   re-conversion of defined length UN elements read in an explicit VR transfer
**   syntax, if the real VR is defined in the data dictionary. Default is OFFalse,
**   i.e. to retain the previous behavior.
**
** Revision 1.94  2005/11/15 16:59:25  meichel
** Added new pseudo VR type EVR_lt that is used for LUT Data when read in
**   implicit VR, which may be US, SS or OW. DCMTK always treats EVR_lt like OW.
**
** Revision 1.93  2005/11/07 16:59:26  meichel
** Cleaned up some copy constructors in the DcmObject hierarchy.
**
** Revision 1.92  2005/06/24 10:04:04  joergr
** Added support for internal VR "xs" to putAndInsertXXX() helper methods.
**
** Revision 1.91  2005/05/10 15:27:18  meichel
** Added support for reading UN elements with undefined length according
**   to CP 246. The global flag dcmEnableCP246Support allows to revert to the
**   prior behaviour in which UN elements with undefined length were parsed
**   like a normal explicit VR SQ element.
**
** Revision 1.90  2004/07/01 12:28:27  meichel
** Introduced virtual clone method for DcmObject and derived classes.
**
** Revision 1.89  2004/03/10 10:25:36  joergr
** Translated remaining German comments.
**
** Revision 1.88  2004/02/04 16:02:56  joergr
** Removed pointer declaration from parameter "resultStack" in method
** findAndGetElements(). Removed e-mail addresses from CVS log.
**
** Revision 1.87  2003/10/15 16:55:43  meichel
** Updated error messages for parse errors
**
** Revision 1.86  2003/10/08 10:25:00  joergr
** Added support for AT, OB, OF, OW, SL, SS, UL, US to putAndInsertString().
**
** Revision 1.85  2003/07/16 14:33:43  joergr
** Added new function findAndGetSequence().
** Adapted type casts to new-style typecast operators defined in ofcast.h.
**
** Revision 1.84  2003/06/26 09:17:29  onken
** Added commandline-application dcmodify.
**
** Revision 1.83  2003/06/02 17:25:28  joergr
** Added new helper function DcmItem::findAndCopyElement().
** Fixed bug in findAndDelete() implementation.
** Added explicit support for class DcmPixelData to putAndInsertUintXXArray().
** Changed implementation of findAndGetXXXArray() to avoid problems with MSVC5.
**
** Revision 1.82  2003/05/20 09:17:46  joergr
** Added new helper methods: findAndGetElement(), findAndGetUint32Array(),
** findAndGetSint32Array(), findAndGetFloat64Array(), findAndDeleteElement().
** Enhanced findAndGetSequenceItem() and findOrCreateSequenceItem() by checking
** the return value of ident() - avoids crashes when applied to non-sequence
** elements.
**
** Revision 1.81  2003/03/21 13:08:04  meichel
** Minor code purifications for warnings reported by MSVC in Level 4
**
** Revision 1.80  2002/12/09 09:30:52  wilkens
** Modified/Added doc++ documentation.
**
** Revision 1.79  2002/12/06 12:57:58  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Made source code formatting more consistent with other modules/files.
** Replaced some German comments by English translations.
**
** Revision 1.78  2002/11/27 12:06:48  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.77  2002/08/27 16:55:50  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.76  2002/08/02 15:06:33  joergr
** Fixed problems reported by Sun CC 2.0.1.
**
** Revision 1.75  2002/08/02 08:42:33  joergr
** Added optional 'pos' parameter to the putAndInsertXXX() methods.
**
** Revision 1.74  2002/07/23 14:21:33  meichel
** Added support for private tag data dictionaries to dcmdata
**
** Revision 1.73  2002/07/08 16:15:40  meichel
** Unknown undefined length attributes are now converted into SQ instead of UN.
**
** Revision 1.72  2002/07/08 14:44:39  meichel
** Improved dcmdata behaviour when reading odd tag length. Depending on the
**   global boolean flag dcmAcceptOddAttributeLength, the parser now either accepts
**   odd length attributes or implements the old behaviour, i.e. assumes a real
**   length larger by one.
**
** Revision 1.71  2002/07/04 16:35:31  joergr
** Fixed inconsistent formatting of the print() output.
**
** Revision 1.70  2002/06/26 15:49:59  joergr
** Added support for polymorp OB/OW value representation (e.g. pixel data) to
** putAndInsertUint8/16Array() methods.
**
** Revision 1.69  2002/05/29 09:59:37  meichel
** fixed follow-up problem in DcmItem caused by the modifications
**   committed 2002-05-17.
**
** Revision 1.68  2002/05/17 09:58:24  meichel
** fixed bug in DcmItem which caused the parser to fail if the same attribute
**   tag appeared twice within one dataset (which is illegal in DICOM anyway).
**   Added console warning if the attributes read are not in ascending order.
**
** Revision 1.67  2002/04/25 10:15:56  joergr
** Added support for XML output of DICOM objects.
**
** Revision 1.66  2002/04/16 13:43:17  joergr
** Added configurable support for C++ ANSI standard includes (e.g. streams).
**
** Revision 1.65  2002/04/11 12:28:00  joergr
** Enhanced documentation.
**
** Revision 1.64  2001/12/18 11:37:44  joergr
** Added helper method allowing to create and insert empty elements into an
** item/dataset.
**
** Revision 1.63  2001/11/16 15:55:02  meichel
** Adapted digital signature code to final text of supplement 41.
**
** Revision 1.62  2001/11/09 15:53:53  joergr
** Added new helper routines for managing sequences and items.
**
** Revision 1.61  2001/11/01 14:55:39  wilkens
** Added lots of comments.
**
** Revision 1.60  2001/10/10 15:19:51  joergr
** Changed parameter DcmTagKey to DcmTag in DcmItem::putAndInsert... methods
** to support elements which are not in the data dictionary (e.g. private
** extensions).
**
** Revision 1.59  2001/10/02 11:48:01  joergr
** Added functions to get/put 8 bit values/arrays from/to an item/dataset.
**
** Revision 1.58  2001/10/01 15:04:14  joergr
** Introduced new general purpose functions to get/put DICOM element values
** from/to an item/dataset - removed some old and rarely used functions.
** Added "#include <iomanip.h>" to keep gcc 3.0 quiet.
**
** Revision 1.57  2001/09/25 17:19:50  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.56  2001/06/01 15:49:05  meichel
** Updated copyright header
**
** Revision 1.55  2001/05/07 16:08:09  joergr
** Added support for VR=IS to method findIntegerNumber().
**
** Revision 1.54  2001/05/03 08:15:21  meichel
** Fixed bug in dcmdata sequence handling code that could lead to application
**   failure in rare cases during parsing of a correct DICOM dataset.
**
** Revision 1.53  2000/11/07 16:56:20  meichel
** Initial release of dcmsign module for DICOM Digital Signatures
**
** Revision 1.52  2000/04/14 15:55:05  meichel
** Dcmdata library code now consistently uses ofConsole for error output.
**
** Revision 1.51  2000/03/08 16:26:36  meichel
** Updated copyright header.
**
** Revision 1.50  2000/03/03 15:02:09  joergr
** Corrected bug related to padding of file and item size.
**
** Revision 1.49  2000/03/03 14:05:34  meichel
** Implemented library support for redirecting error messages into memory
**   instead of printing them to stdout/stderr for GUI applications.
**
** Revision 1.48  2000/02/29 11:49:29  meichel
** Removed support for VS value representation. This was proposed in CP 101
**   but never became part of the standard.
**
** Revision 1.47  2000/02/23 15:11:55  meichel
** Corrected macro for Borland C++ Builder 4 workaround.
**
** Revision 1.46  2000/02/10 10:52:19  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.45  2000/02/02 14:32:52  joergr
** Replaced 'delete' statements by 'delete[]' for objects created with 'new[]'.
**
** Revision 1.44  2000/02/01 10:12:07  meichel
** Avoiding to include <stdlib.h> as extern "C" on Borland C++ Builder 4,
**   workaround for bug in compiler header files.
**
** Revision 1.43  1999/03/31 09:25:30  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.42  1999/03/22 15:55:52  meichel
** New handling of unknown (unsupported) VRs encountered when reading explicit
**   VR data. If the VR string consists of uppercase letters, we assume a
**   "future DICOM VR" and decode it expecting an extended length field
**   (4 bytes). Otherwise, we assume an illegal VR string created by some old
**   equipment (i.e.) "??" and decode without extended length (2 bytes).
**
** Revision 1.41  1998/07/15 15:51:59  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.40  1998/01/14 15:23:42  hewett
** Added support for the VRs UT (Unlimited Text) and VS (Virtual String).
**
** Revision 1.39  1998/01/14 09:13:53  meichel
** Corrected bug: Overlay Data elements in the groups
**   6002-601f were handled by DcmOtherByteOtherWord
**   instead of the "polymorphous" DcmOverlayData class.
**
** Revision 1.38  1998/01/14 08:42:32  meichel
** Improved algorithm for auto-detection of transfer syntax
**   used when opening a DICOM file without metaheader.
**   Big endian datasets are now detected much more reliably.
**
** Revision 1.37  1997/11/07 08:52:18  meichel
** Corrected bug in the dcmdata read routines which caused incorrect reading
**   of datasets containing attributes with value representation "ox" (= OB or OW)
**   in the dicom dictionary other than PixelData and OverlayData.
**
** Revision 1.36  1997/09/22 14:50:53  hewett
** - Added 2 simple methods to test for the existance of an attribute
**   to DcmItem class (tagExists and tagExistsWithValue).  This code
**   was part of dcmgpdir.cc but is more generally useful.
** - Added 2 methods to find an attribute and retrieve numeric values
**   to DcmItem class (findIntegerNumber and findRealNumber).  The old
**   method findLong is now marked as obsolete and reimplemented using
**   findIntegerNumber.
**
** Revision 1.35  1997/09/12 13:44:53  meichel
** The algorithm introduced on 97.08.28 to detect incorrect odd-length
**   value fields falsely reported undefined length sequences and items
**   to be wrong. Corrected.
**
** Revision 1.34  1997/08/29 08:31:33  andreas
** - Added methods getOFString and getOFStringArray for all
**   string VRs. These methods are able to normalise the value, i. e.
**   to remove leading and trailing spaces. This will be done only if
**   it is described in the standard that these spaces are not relevant.
**   These methods do not test the strings for conformance, this means
**   especially that they do not delete spaces where they are not allowed!
**   getOFStringArray returns the string with all its parts separated by \
**   and getOFString returns only one value of the string.
**   CAUTION: Currently getString returns a string with trailing
**   spaces removed (if dcmEnableAutomaticInputDataCorrection == OFTrue) and
**   truncates the original string (since it is not copied!). If you rely on this
**   behaviour please change your application now.
**   Future changes will ensure that getString returns the original
**   string from the DICOM object (NULL terminated) inclusive padding.
**   Currently, if you call getOF... before calling getString without
**   normalisation, you can get the original string read from the DICOM object.
**
** Revision 1.33  1997/08/29 07:52:40  andreas
** - New error messages if length of an element is odd. Previously, no
**   error was reported. But the length is corrected by the method
**   newValueFiel and. so it was impossible for a checking utility to find
**   such an error in DICOM objects.
**
** Revision 1.32  1997/07/24 13:10:52  andreas
** - Removed Warnings from SUN CC 2.0.1
**
** Revision 1.31  1997/07/21 08:11:42  andreas
** - Support for CP 14. PixelData and OverlayData can have VR OW or OB
**   (depending on the transfer syntax). New internal value
**   representation (only for ident()) for OverlayData.
** - New environment for encapsulated pixel representations. DcmPixelData
**   can contain different representations and uses codecs to convert
**   between them. Codecs are derived from the DcmCodec class. New error
**   codes are introduced for handling of representations. New internal
**   value representation (only for ident()) for PixelData
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.30  1997/07/07 07:43:59  andreas
** - Changed type for Tag attribute in DcmObject from prointer to value
** - Changed parameter type DcmTag & to DcmTagKey & in all search functions
**   in DcmItem, DcmSequenceOfItems, DcmDirectoryRecord and DcmObject
** - Enhanced (faster) byte swapping routine. swapIfNecessary moved from
**   a method in DcmObject to a general function.
**
** Revision 1.29  1997/07/03 15:09:59  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.28  1997/06/06 09:55:29  andreas
** - corrected error: canWriteXfer returns false if the old transfer syntax
**   was unknown, which causes several applications to prohibit the writing
**   of dataset.
**
** Revision 1.27  1997/05/27 13:49:00  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.26  1997/05/16 08:13:49  andreas
** - Revised handling of GroupLength elements and support of
**   DataSetTrailingPadding elements. The enumeratio E_GrpLenEncoding
**   got additional enumeration values (for a description see dctypes.h).
**   addGroupLength and removeGroupLength methods are replaced by
**   computeGroupLengthAndPadding. To support Padding, the parameters of
**   element and sequence write functions changed.
** - Added a new method calcElementLength to calculate the length of an
**   element, item or sequence. For elements it returns the length of
**   tag, length field, vr field, and value length, for item and
**   sequences it returns the length of the whole item. sequence including
**   the Delimitation tag (if appropriate).  It can never return
**   UndefinedLength.
** - Deleted obsolete method DcmItem::calcHeaderLength because the
**   samce functionality is performed by DcmXfer::sizeofTagHeader
**
** Revision 1.25  1997/05/07 12:27:27  andreas
** Corrected error reading ItemDelimitationItem using explicit transfer syntaxes
**
** Revision 1.24  1997/04/30 16:32:50  andreas
** - Corrected Bug for reading of encapsulated pixel sequences
**
** Revision 1.23  1997/04/24 12:12:18  hewett
** Fixed DICOMDIR generation bug affecting the use of Unknown VR
** attributes (the file offsets were not being computed correctly).
**
** Revision 1.22  1997/04/18 08:10:49  andreas
** - Corrected debugging code
** - The put/get-methods for all VRs did not conform to the C++-Standard
**   draft. Some Compilers (e.g. SUN-C++ Compiler, Metroworks
**   CodeWarrier, etc.) create many warnings concerning the hiding of
**   overloaded get methods in all derived classes of DcmElement.
**   So the interface of all value representation classes in the
**   library are changed rapidly, e.g.
**   OFCondition get(Uint16 & value, const unsigned long pos);
**   becomes
**   OFCondition getUint16(Uint16 & value, const unsigned long pos);
**   All (retired) "returntype get(...)" methods are deleted.
**   For more information see dcmdata/include/dcelem.h
**
** Revision 1.21  1997/03/27 15:52:50  hewett
** Extended preliminary support for Unknown VR (UN) described in
** Supplement 14.  Attributes with undefined length should now
** be handled as a sequence.
**
** Revision 1.20  1997/03/26 17:15:57  hewett
** Added very preliminary support for Unknown VR (UN) described in
** Supplement 14.  WARNING: handling of unknown attributes with undefined
** length is not yet supported.
**
** Revision 1.19  1996/09/24 15:54:14  hewett
** Corrected erroneous setting of an error flag when inserting an
** attribute into an Item (via Item::insert(...)) and the attribute
** was already present.  Now the error flag is only set if replaceOld
** is OFFalse and an attribute already exists.
**
** Revision 1.18  1996/09/13 12:04:12  hewett
** Corrected missing () in function call (stack.card()) used in nextObject(...)
**
** Revision 1.17  1996/08/08 10:15:09  andreas
** Some more testing in nextObject
**
** Revision 1.16  1996/08/08 10:06:23  andreas
** Correct error for intoSub=OFFalse
**
** Revision 1.15  1996/08/05 08:46:12  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.14  1996/07/31 13:14:30  andreas
** - Minor corrections: error code for swapping to or from byteorder unknown
**                      correct read of dataset in fileformat
**
** Revision 1.13  1996/07/17 12:39:38  andreas
** new nextObject for DcmDataSet, DcmFileFormat, DcmItem, ...
**
** Revision 1.12  1996/04/29 15:08:14  hewett
** Replaced DcmItem::findInt(...) with the more general DcmItem::findLong(...).
**
** Revision 1.11  1996/04/27 14:04:55  hewett
** Eliminated compiler warnings when compiling without -DDEBUG.  Very
** minor corrections, mostly unused parameters and uninitialized variables.
**
** Revision 1.10  1996/04/16 16:04:54  andreas
** - const tag Parameter in newDicomElement
**
** Revision 1.9  1996/03/28 18:52:39  hewett
** Added 2 simple find&get methods (findString & findInt).
**
** Revision 1.8  1996/03/12 15:23:27  hewett
** When generating group length tags, the VR of a tag is now explicity
** set to be EVR_UL.  Group length tags not in the dictionary (e.g. for
** private groups) were getting coded incorrectly.
**
** Revision 1.7  1996/03/11 14:16:00  hewett
** Corrected error whereby explicit encoding was being recognised as implicit.
**
** Revision 1.6  1996/03/11 13:03:51  hewett
** Rearranged logic of DcmItem::checkTransferSyntax to make little-endian
** the default if both big and little endian are possible.
**
** Revision 1.5  1996/01/29 13:38:27  andreas
** - new put method for every VR to put value as a string
** - better and unique print methods
**
** Revision 1.4  1996/01/09 11:06:46  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
** Correct error in reading Item Delimitation Elements
**
** Revision 1.3  1996/01/05 13:27:37  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/
