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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Implementation of class DcmDataset
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofstack.h"
#include "dcmtk/ofstd/ofstd.h"

#include "dcmtk/dcmdata/dcjson.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcxfer.h"
#include "dcmtk/dcmdata/dcvrus.h"
#include "dcmtk/dcmdata/dcpixel.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmdata/dcostrma.h"    /* for class DcmOutputStream */
#include "dcmtk/dcmdata/dcostrmf.h"    /* for class DcmOutputFileStream */
#include "dcmtk/dcmdata/dcistrma.h"    /* for class DcmInputStream */
#include "dcmtk/dcmdata/dcistrmf.h"    /* for class DcmInputFileStream */
#include "dcmtk/dcmdata/dcwcache.h"    /* for class DcmWriteCache */


// ********************************


DcmDataset::DcmDataset()
  : DcmItem(DCM_ItemTag, DCM_UndefinedLength),
    OriginalXfer(EXS_Unknown),
    // the default transfer syntax is explicit VR with local endianness
    CurrentXfer((gLocalByteOrder == EBO_BigEndian) ? EXS_BigEndianExplicit : EXS_LittleEndianExplicit)
{
}


DcmDataset& DcmDataset::operator=(const DcmDataset& obj)
{
  if (this != &obj)
  {
    // copy parent's member variables
    DcmItem::operator=(obj);
    // copy DcmDataset's member variables
    OriginalXfer = obj.OriginalXfer;
    CurrentXfer = obj.CurrentXfer;
  }
  return *this;
}


DcmDataset::DcmDataset(const DcmDataset &old)
  : DcmItem(old),
    OriginalXfer(old.OriginalXfer),
    CurrentXfer(old.CurrentXfer)
{
}


OFCondition DcmDataset::copyFrom(const DcmObject& rhs)
{
  if (this != &rhs)
  {
    if (rhs.ident() != ident()) return EC_IllegalCall;
    *this = OFstatic_cast(const DcmDataset &, rhs);
  }
  return EC_Normal;
}


DcmDataset::~DcmDataset()
{
}


// ********************************


OFCondition DcmDataset::clear()
{
    OFCondition result = DcmItem::clear();
    // TODO: should we also reset OriginalXfer and CurrentXfer?
    setLengthField(DCM_UndefinedLength);
    return result;
}

DcmEVR DcmDataset::ident() const
{
    return EVR_dataset;
}


E_TransferSyntax DcmDataset::getOriginalXfer() const
{
    return OriginalXfer;
}


E_TransferSyntax DcmDataset::getCurrentXfer() const
{
    return CurrentXfer;
}


void DcmDataset::updateOriginalXfer()
{
    DcmStack resultStack;
    /* Check for pixel data element on main dataset level only. */
    /* Icon images and other nested pixel data elements are not checked. */
    if (search(DCM_PixelData, resultStack, ESM_fromHere, OFFalse).good())
    {
        if (resultStack.top()->ident() == EVR_PixelData)
        {
            /* determine the transfer syntax of the original and current representation */
            E_TransferSyntax repType = EXS_Unknown;
            const DcmRepresentationParameter *repParam = NULL;
            DcmPixelData *pixelData = OFstatic_cast(DcmPixelData *, resultStack.top());
            pixelData->getOriginalRepresentationKey(OriginalXfer, repParam);
            pixelData->getCurrentRepresentationKey(repType, repParam);
            /* check whether we also need to change the current transfer syntax */
            if (repType == EXS_LittleEndianExplicit /* default */)
            {
                /* only change the value if not already uncompressed */
                if ((CurrentXfer != EXS_LittleEndianImplicit) &&
                    (CurrentXfer != EXS_LittleEndianExplicit) &&
                    (CurrentXfer != EXS_BigEndianExplicit))
                {
                    CurrentXfer = repType;
                }
            }
            else if (repType != EXS_Unknown)
            {
                CurrentXfer = repType;
            }
        } else {
            /* something is fishy with the pixel data element (wrong class) */
            DCMDATA_WARN("DcmDataset: Wrong class for pixel data element, cannot update original transfer syntax");
        }
    }
    /* if no pixel data was found, update only in case of unknown representation */
    else
    {
        if (OriginalXfer == EXS_Unknown)
        {
            /* this is also the default in DcmPixelData::getOriginalRepresentationKey() */
            OriginalXfer = EXS_LittleEndianExplicit;
        }
        if (CurrentXfer == EXS_Unknown)
        {
            /* this is also the default in DcmPixelData::getCurrentRepresentationKey() */
            CurrentXfer = EXS_LittleEndianExplicit;
        }
    }
}


void DcmDataset::removeInvalidGroups(const OFBool cmdSet)
{
    DcmStack stack;
    DcmObject *object = NULL;
    /* check for data or command set */
    if (cmdSet)
    {
        /* iterate over all elements */
        while (nextObject(stack, OFTrue).good())
        {
            object = stack.top();
            /* in command sets, only group 0x0000 is allowed */
            if (object->getGTag() != 0x0000)
            {
                DCMDATA_DEBUG("DcmDataset::removeInvalidGroups() removing element "
                    << object->getTag() << " from command set");
                stack.pop();
                /* remove element from command set and free memory */
                delete OFstatic_cast(DcmItem *, stack.top())->remove(object);
            }
        }
    } else {
        /* iterate over all elements */
        while (nextObject(stack, OFTrue).good())
        {
            object = stack.top();
            /* in data sets, group 0x0000 to 0x0003, 0x0005, 0x0007 and 0xFFFF are not allowed */
            if ((object->getGTag() == 0x0000) || (object->getGTag() == 0x0002) ||
                !object->getTag().hasValidGroup())
            {
                DCMDATA_DEBUG("DcmDataset::removeInvalidGroups() removing element "
                    << object->getTag() << " from data set");
                stack.pop();
                /* remove element from data set and free memory */
                delete OFstatic_cast(DcmItem *, stack.top())->remove(object);
            }
            /* in sequence items, also group 0x0006 is not allowed */
            else if ((stack.card() > 2) && (object->getGTag() == 0x0006))
            {
                DCMDATA_DEBUG("DcmDataset::removeInvalidGroups() removing element "
                    << object->getTag() << " from sequence item");
                stack.pop();
                /* remove element from data set and free memory */
                delete OFstatic_cast(DcmItem *, stack.top())->remove(object);
            }
        }
    }
}


// ********************************


Uint32 DcmDataset::calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype)
{
    return DcmItem::getLength(xfer, enctype);
}


// ********************************


OFBool DcmDataset::canWriteXfer(const E_TransferSyntax newXfer,
                                const E_TransferSyntax oldXfer)
{
    if (newXfer == EXS_Unknown)
        return OFFalse;

    /* Check stream compression for this transfer syntax */
    DcmXfer xf(newXfer);
    if (xf.getStreamCompression() == ESC_unsupported)
        return OFFalse;

    return DcmItem::canWriteXfer(newXfer, (OriginalXfer == EXS_Unknown) ? oldXfer : OriginalXfer);
}


// ********************************


void DcmDataset::print(STD_NAMESPACE ostream &out,
                       const size_t flags,
                       const int level,
                       const char *pixelFileName,
                       size_t *pixelCounter)
{
    out << OFendl;
    if (flags & DCMTypes::PF_useANSIEscapeCodes)
        out << DCMDATA_ANSI_ESCAPE_CODE_COMMENT;
    printNestingLevel(out, flags, level);
    out << "# Dicom-Data-Set" << OFendl;
    if (flags & DCMTypes::PF_useANSIEscapeCodes)
        out << DCMDATA_ANSI_ESCAPE_CODE_COMMENT;
    printNestingLevel(out, flags, level);
    out << "# Used TransferSyntax: " << DcmXfer(CurrentXfer).getXferName();
    if (flags & DCMTypes::PF_useANSIEscapeCodes)
        out << DCMDATA_ANSI_ESCAPE_CODE_RESET;
    out << OFendl;
    if (!elementList->empty())
    {
        DcmObject *dO;
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            dO->print(out, flags, level + 1, pixelFileName, pixelCounter);
        } while (elementList->seek(ELP_next));
    }
}


// ********************************


OFCondition DcmDataset::writeXML(STD_NAMESPACE ostream &out,
                                 const size_t flags)
{
    OFCondition l_error = EC_Normal;
    /* the Native DICOM Model as defined for Application Hosting needs special handling */
    if (flags & DCMTypes::XF_useNativeModel)
    {
        /* write XML start tag */
        out << "<NativeDicomModel xml:space=\"preserve\"";
        if (flags & DCMTypes::XF_useXMLNamespace)
            out << " xmlns=\"" << NATIVE_DICOM_MODEL_XML_NAMESPACE_URI << "\"";
        out << ">" << OFendl;
    } else {
        /* DCMTK-specific output format (default) */
        OFString xmlString;
        DcmXfer xfer(CurrentXfer);
        /* write XML start tag */
        out << "<data-set xfer=\"" << xfer.getXferID() << "\"";
        out << " name=\"" << OFStandard::convertToMarkupString(xfer.getXferName(), xmlString) << "\"";
        if (flags & DCMTypes::XF_useXMLNamespace)
            out << " xmlns=\"" << DCMTK_XML_NAMESPACE_URI << "\"";
        out << ">" << OFendl;
    }
    /* write dataset content */
    if (!elementList->empty())
    {
        /* write content of all children */
        DcmObject *dO;
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            l_error = dO->writeXML(out, flags & ~DCMTypes::XF_useXMLNamespace);
        } while (l_error.good() && elementList->seek(ELP_next));
    }
    if (l_error.good())
    {
        /* write XML end tag (depending on output format) */
        if (flags & DCMTypes::XF_useNativeModel)
        {
            out << "</NativeDicomModel>" << OFendl;
        } else {
            out << "</data-set>" << OFendl;
        }
    }
    return l_error;
}


// ********************************


OFCondition DcmDataset::writeJson(STD_NAMESPACE ostream &out,
                                  DcmJsonFormat &format)
{
    // write dataset content
    if (!elementList->empty())
    {
        elementList->seek(ELP_first);
        OFCondition status = EC_Normal;
        // write content of all children
        status = elementList->get()->writeJson(out, format);
        while (status.good() && elementList->seek(ELP_next))
        {
            out << "," << format.newline();
            status = elementList->get()->writeJson(out, format);
        }
        return status;
    }
    return EC_Normal;
}


// ********************************


OFCondition DcmDataset::read(DcmInputStream &inStream,
                             const E_TransferSyntax xfer,
                             const E_GrpLenEncoding glenc,
                             const Uint32 maxReadLength)
{
  return DcmDataset::readUntilTag(inStream, xfer, glenc, maxReadLength, DCM_UndefinedTagKey);
}

OFCondition DcmDataset::readUntilTag(DcmInputStream &inStream,
                                     const E_TransferSyntax xfer,
                                     const E_GrpLenEncoding glenc,
                                     const Uint32 maxReadLength,
                                     const DcmTagKey &stopParsingAtElement)
{
    /* check if the stream variable reported an error */
    errorFlag = inStream.status();
    /* if the stream did not report an error but the stream */
    /* is empty, set the error flag correspondingly */
    if (errorFlag.good() && inStream.eos())
        errorFlag = EC_EndOfStream;
    /* else if the stream did not report an error but the transfer */
    /* state does not equal ERW_ready, go ahead and do something */
    else if (errorFlag.good() && getTransferState() != ERW_ready)
    {
        /* if the transfer state is ERW_init, go ahead and check the transfer syntax which was passed */
        if (getTransferState() == ERW_init)
        {
            if (dcmAutoDetectDatasetXfer.get())
            {
                DCMDATA_DEBUG("DcmDataset::read() automatic detection of transfer syntax is enabled");
                /* To support incorrectly encoded datasets detect the transfer syntax from the stream.  */
                /* This is possible for given unknown and plain big or little endian transfer syntaxes. */
                switch (xfer)
                {
                    case EXS_Unknown:
                    case EXS_LittleEndianImplicit:
                    case EXS_LittleEndianExplicit:
                    case EXS_BigEndianExplicit:
                    case EXS_BigEndianImplicit:
                        DCMDATA_DEBUG("DcmDataset::read() trying to detect transfer syntax of uncompressed data set");
                        OriginalXfer = checkTransferSyntax(inStream);
                        if ((xfer != EXS_Unknown) && (OriginalXfer != xfer))
                            DCMDATA_WARN("DcmDataset: Wrong transfer syntax specified, detecting from data set");
                        break;
                    default:
                        DCMDATA_DEBUG("DcmDataset::read() data set seems to be compressed, so transfer syntax is not detected");
                        OriginalXfer = xfer;
                        break;
                }
            }
            else /* default behavior */
            {
                /* If the transfer syntax which was passed equals EXS_Unknown we want to */
                /* determine the transfer syntax from the information in the stream itself. */
                /* If the transfer syntax is given, we want to use it. */
                if (xfer == EXS_Unknown)
                {
                    DCMDATA_DEBUG("DcmDataset::read() trying to detect transfer syntax of data set (because it is unknown)");
                    OriginalXfer = checkTransferSyntax(inStream);
                } else
                    OriginalXfer = xfer;
            }
            /* dump information on debug level */
            DCMDATA_DEBUG("DcmDataset::read() TransferSyntax=\""
                << DcmXfer(OriginalXfer).getXferName() << "\"");
            CurrentXfer = OriginalXfer;
            /* check stream compression for this transfer syntax */
            DcmXfer xf(OriginalXfer);
            E_StreamCompression sc = xf.getStreamCompression();
            switch (sc)
            {
                case ESC_none:
                  // nothing to do
                  break;
                case ESC_unsupported:
                  // stream compressed transfer syntax that we cannot create; bail out.
                  if (errorFlag.good())
                      errorFlag = EC_UnsupportedEncoding;
                  break;
                default:
                  // supported stream compressed transfer syntax, install filter
                  errorFlag = inStream.installCompressionFilter(sc);
                  break;
            }
        }
        /* pass processing the task to class DcmItem */
        if (errorFlag.good())
            errorFlag = DcmItem::readUntilTag(inStream, OriginalXfer, glenc, maxReadLength, stopParsingAtElement);

    }

    /* if the error flag shows ok or that the end of the stream was encountered, */
    /* we have read information for this particular data set or command; in this */
    /* case, we need to do something for the current dataset object */
    if (errorFlag.good() || errorFlag == EC_EndOfStream)
    {
        /* perform some final checks on dataset level */
        errorFlag = doPostReadChecks();

        if (errorFlag.good())
        {
            /* set the error flag to ok */
            errorFlag = EC_Normal;

            /* take care of group length (according to what is specified */
            /* in glenc) and padding elements (don't change anything) */
            computeGroupLengthAndPadding(glenc, EPD_noChange, OriginalXfer);

            /* and set the transfer state to ERW_ready to indicate that the data set is complete */
            setTransferState(ERW_ready);
        }
    }

    /* dump information if required */
    DCMDATA_TRACE("DcmDataset::read() returns error = " << errorFlag.text());

    /* return result flag */
    return errorFlag;
}


// ********************************


OFCondition DcmDataset::write(
      DcmOutputStream &outStream,
      const E_TransferSyntax oxfer,
      const E_EncodingType enctype /* = EET_UndefinedLength */,
      DcmWriteCache *wcache)
{
    return write(outStream, oxfer, enctype, wcache, EGL_recalcGL);
}


OFCondition DcmDataset::write(DcmOutputStream &outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype,
                              DcmWriteCache *wcache,
                              const E_GrpLenEncoding glenc,
                              const E_PaddingEncoding padenc,
                              const Uint32 padlen,
                              const Uint32 subPadlen,
                              Uint32 instanceLength)
{
  /* if the transfer state of this is not initialized, this is an illegal call */
  if (getTransferState() == ERW_notInitialized)
    errorFlag = EC_IllegalCall;
  else
  {
    /* check if the stream reported an error so far; if not, we can go ahead and write some data to it */
    errorFlag = outStream.status();

    if (errorFlag.good() && getTransferState() != ERW_ready)
    {
      /* Determine the transfer syntax which shall be used. Either we use the one which was passed, */
      /* or (if it's an unknown transfer syntax) we use the one which is contained in OriginalXfer. */
      E_TransferSyntax newXfer = oxfer;
      if (newXfer == EXS_Unknown)
        newXfer = OriginalXfer;

      /* if this function was called for the first time for the dataset object, the transferState is still */
      /* set to ERW_init. In this case, we need to take care of group length and padding elements according */
      /* to the strategies which are specified in glenc and padenc. Additionally, we need to set the element */
      /* list pointer of this data set to the fist element and we need to set the transfer state to ERW_inWork */
      /* so that this scenario will only be executed once for this data set object. */
      if (getTransferState() == ERW_init)
      {

        /* Check stream compression for this transfer syntax */
        DcmXfer xf(newXfer);
        E_StreamCompression sc = xf.getStreamCompression();
        switch (sc)
        {
          case ESC_none:
            // nothing to do
            break;
          case ESC_unsupported:
            // stream compressed transfer syntax that we cannot create; bail out.
            if (errorFlag.good())
              errorFlag = EC_UnsupportedEncoding;
            break;
          default:
            // supported stream compressed transfer syntax, install filter
            errorFlag = outStream.installCompressionFilter(sc);
            break;
        }

        /* take care of group length and padding elements, according to what is specified in glenc and padenc */
        computeGroupLengthAndPadding(glenc, padenc, newXfer, enctype, padlen, subPadlen, instanceLength);
        elementList->seek(ELP_first);
        setTransferState(ERW_inWork);
      }

      /* if the transfer state is set to ERW_inWork, we need to write the information which */
      /* is included in this data set's element list into the buffer which was passed. */
      if (getTransferState() == ERW_inWork)
      {
        // Remember that elementList->get() can be NULL if buffer was full after
        // writing the last item but before writing the sequence delimitation.
        if (!elementList->empty() && (elementList->get() != NULL))
        {
          /* as long as everything is ok, go through all elements of this data */
          /* set and write the corresponding information to the buffer */
          DcmObject *dO;
          do
          {
            dO = elementList->get();
            errorFlag = dO->write(outStream, newXfer, enctype, wcache);
          } while (errorFlag.good() && elementList->seek(ELP_next));
        }

        /* if all the information in this has been written to the */
        /* buffer set this data set's transfer state to ERW_ready */
        if (errorFlag.good())
        {
          setTransferState(ERW_ready);
          CurrentXfer = newXfer;
        }
      }
    }
  }

  /* return the corresponding result value */
  return errorFlag;
}


// ********************************


OFCondition DcmDataset::writeSignatureFormat(DcmOutputStream &outStream,
                                             const E_TransferSyntax oxfer,
                                             const E_EncodingType enctype,
                                             DcmWriteCache *wcache)
{
  if (getTransferState() == ERW_notInitialized)
    errorFlag = EC_IllegalCall;
  else
  {
    E_TransferSyntax newXfer = oxfer;
    if (newXfer == EXS_Unknown)
      newXfer = OriginalXfer;

    errorFlag = outStream.status();
    if (errorFlag.good() && getTransferState() != ERW_ready)
    {
      if (getTransferState() == ERW_init)
      {
        computeGroupLengthAndPadding(EGL_recalcGL, EPD_noChange, newXfer, enctype, 0, 0, 0);
        elementList->seek(ELP_first);
        setTransferState(ERW_inWork);
      }
      if (getTransferState() == ERW_inWork)
      {
        // elementList->get() can be NULL if buffer was full after
        // writing the last item but before writing the sequence delimitation.
        if (!elementList->empty() && (elementList->get() != NULL))
        {
          DcmObject *dO;
          do {
            dO = elementList->get();
            errorFlag = dO->writeSignatureFormat(outStream, newXfer, enctype, wcache);
          } while (errorFlag.good() && elementList->seek(ELP_next));
        }
        if (errorFlag.good())
        {
          setTransferState(ERW_ready);
          CurrentXfer = newXfer;
        }
      }
    }
  }
  return errorFlag;
}


// ********************************


OFCondition DcmDataset::loadFile(const OFFilename &fileName,
                                 const E_TransferSyntax readXfer,
                                 const E_GrpLenEncoding groupLength,
                                 const Uint32 maxReadLength)
{
  return DcmDataset::loadFileUntilTag(fileName, readXfer, groupLength, maxReadLength, DCM_UndefinedTagKey);
}

OFCondition DcmDataset::loadFileUntilTag(const OFFilename &fileName,
                                 const E_TransferSyntax readXfer,
                                 const E_GrpLenEncoding groupLength,
                                 const Uint32 maxReadLength,
                                 const DcmTagKey &stopParsingAtElement)
{
    OFCondition l_error = EC_InvalidFilename;
    /* check parameters first */
    if (!fileName.isEmpty())
    {
        /* open file for input */
        DcmInputFileStream fileStream(fileName);

        /* check stream status */
        l_error = fileStream.status();

        if (l_error.good())
        {
            /* clear this object */
            l_error = clear();
            if (l_error.good())
            {
                /* read data from file */
                transferInit();
                l_error = readUntilTag(fileStream, readXfer, groupLength, maxReadLength, stopParsingAtElement);
                transferEnd();
            }
        }
    }
    return l_error;
}


OFCondition DcmDataset::saveFile(const OFFilename &fileName,
                                 const E_TransferSyntax writeXfer,
                                 const E_EncodingType encodingType,
                                 const E_GrpLenEncoding groupLength,
                                 const E_PaddingEncoding padEncoding,
                                 const Uint32 padLength,
                                 const Uint32 subPadLength)
{
    OFCondition l_error = EC_InvalidFilename;
    /* check parameters first */
    if (!fileName.isEmpty())
    {
        DcmWriteCache wcache;
        /* open file for output */
        DcmOutputFileStream fileStream(fileName);

        /* check stream status */
        l_error = fileStream.status();
        if (l_error.good())
        {
            /* write data to file */
            transferInit();
            l_error = write(fileStream, writeXfer, encodingType, &wcache, groupLength, padEncoding, padLength, subPadLength);
            transferEnd();
        }
    }
    return l_error;
}


// ********************************


OFCondition DcmDataset::chooseRepresentation(const E_TransferSyntax repType,
                                             const DcmRepresentationParameter *repParam)
{
    OFCondition l_error = EC_Normal;
    OFBool pixelDataEncountered = OFFalse;
    OFStack<DcmStack> pixelStack;
    DcmXfer torep(repType);
    DcmXfer fromrep(CurrentXfer);

    DcmStack resultStack;
    resultStack.push(this);

    // check if we are attempting to compress but the image contains
    // floating point or double floating point pixel data, which our codecs don't support.
    if ((tagExists(DCM_FloatPixelData, OFTrue) || tagExists(DCM_DoubleFloatPixelData, OFTrue)) &&
         (fromrep.isEncapsulated() || torep.isEncapsulated()))
    {
        DCMDATA_ERROR("DcmDataset: Unable to compress/decompress floating point pixel data, cannot change representation");
        l_error = EC_CannotChangeRepresentation;
        return l_error;
    }

    // check if we are attempting to convert a dataset containing
    // a pixel data URL. In that case we only continue if the target
    // transfer syntax also uses a pixel data URL.
    if (tagExists(DCM_PixelDataProviderURL, OFTrue))
    {
      if (! torep.isReferenced())
      {
        DCMDATA_ERROR("DcmDataset: Unable to compress image containing a pixel data provider URL, cannot change representation");
        l_error = EC_CannotChangeRepresentation;
        return l_error;
      }
    }

    // Now search for all PixelData elements in this dataset
    while (search(DCM_PixelData, resultStack, ESM_afterStackTop, OFTrue).good() && l_error.good())
    {
        pixelDataEncountered = OFTrue;
        if (resultStack.top()->ident() == EVR_PixelData)
        {
            DcmPixelData *pixelData = OFstatic_cast(DcmPixelData *, resultStack.top());
            if (!pixelData->canChooseRepresentation(repType, repParam))
                l_error = EC_CannotChangeRepresentation;
            pixelStack.push(resultStack);
        } else {
            /* something is fishy with the pixel data element (wrong class) */
            DCMDATA_ERROR("DcmDataset: Wrong class for pixel data element, cannot change representation");
            l_error = EC_CannotChangeRepresentation;
        }
    }

    // If there are no pixel data elements in the dataset, issue a warning
    if (! pixelDataEncountered)
    {
      if (torep.isEncapsulated() && ! fromrep.isEncapsulated())
      {
        DCMDATA_WARN("DcmDataset: No pixel data present, nothing to compress");
      }
      if (! torep.isEncapsulated() && fromrep.isEncapsulated())
      {
        DCMDATA_WARN("DcmDataset: No pixel data present, nothing to decompress");
      }
    }

    // then call the method doing the real work for all pixel data elements found
    while (l_error.good() && (pixelStack.size() > 0))
    {
        l_error = OFstatic_cast(DcmPixelData *, pixelStack.top().top())->
            chooseRepresentation(repType, repParam, pixelStack.top());

#ifdef PIXELSTACK_MEMORY_LEAK_WORKAROUND
        // on certain platforms there seems to be a memory leak
        // at this point since for some reason pixelStack.pop does
        // not completely destruct the DcmStack object taken from the stack.
        // The following work-around should solve this issue.
        pixelStack.top().clear();
#endif

        pixelStack.pop();
    }

    // store current transfer syntax (if conversion was successful)
    if (l_error.good())
        CurrentXfer = repType;
    return l_error;
}


OFBool DcmDataset::hasRepresentation(const E_TransferSyntax repType,
                                     const DcmRepresentationParameter *repParam)
{
    OFBool result = OFTrue;
    DcmStack resultStack;

    while(search(DCM_PixelData, resultStack, ESM_afterStackTop, OFTrue).good() && result)
    {
        if (resultStack.top()->ident() == EVR_PixelData)
        {
            DcmPixelData *pixelData = OFstatic_cast(DcmPixelData *, resultStack.top());
            result = pixelData->hasRepresentation(repType, repParam);
        }
        else
            result = OFFalse;
    }
    return result;
}


void DcmDataset::removeAllButCurrentRepresentations()
{
    DcmStack resultStack;

    while(search(DCM_PixelData, resultStack, ESM_afterStackTop, OFTrue).good())
    {
        if (resultStack.top()->ident() == EVR_PixelData)
        {
            DcmPixelData *pixelData = OFstatic_cast(DcmPixelData *, resultStack.top());
            pixelData->removeAllButCurrentRepresentations();
        }
    }
}


void DcmDataset::removeAllButOriginalRepresentations()
{
    DcmStack resultStack;

    while(search(DCM_PixelData, resultStack, ESM_afterStackTop, OFTrue).good())
    {
        if (resultStack.top()->ident() == EVR_PixelData)
        {
            DcmPixelData *pixelData = OFstatic_cast(DcmPixelData *, resultStack.top());
            pixelData->removeAllButOriginalRepresentations();
        }
    }
}


// ********************************


OFCondition DcmDataset::doPostReadChecks()
{
  DcmElement* pixData = NULL;
  DcmXfer xf(OriginalXfer);
  OFCondition result = EC_Normal;
  if (findAndGetElement(DCM_PixelData, pixData).good())
  {
      Uint32 valueLength = pixData->getLengthField();
      if (xf.isEncapsulated())
      {
          if (valueLength != DCM_UndefinedLength)
          {
              if (dcmUseExplLengthPixDataForEncTS.get() == OFFalse /* default case */)
              {
                  /* length of top level dataset's Pixel Data is explicitly */
                  /* defined but we have a transfer syntax requiring */
                  /* encapsulated pixel data (always encoded with undefined */
                  /* length). Print and return an error. */
                  DCMDATA_ERROR("Found explicit length Pixel Data in top level "
                  << "dataset with transfer syntax " << xf.getXferName()
                  << ": Only undefined length permitted");
                  result = EC_PixelDataExplLengthIllegal;
              }
              else
              {
                  /* Only print warning if requested by related OFGlobal, */
                  /* and behave like as we have the same case as for an */
                  /* icon image, which is always uncompressed (see above). */
                  DCMDATA_WARN("Found explicit length Pixel Data in top level "
                  << "dataset with transfer syntax " << xf.getXferName()
                  << ": Only undefined length permitted (ignored on explicit request)");
              }
          }
      }
  }

  return result;
}
