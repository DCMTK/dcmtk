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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Implementation of class DcmDataset
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-11-12 12:16:11 $
 *  CVS/RCS Revision: $Revision: 1.52 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
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
  : DcmItem(ItemTag, DCM_UndefinedLength),
    Xfer(EXS_Unknown)
{
}


DcmDataset& DcmDataset::operator=(const DcmDataset& obj)
{
  if (this != &obj)
  {
    // copy parent's member variables
    DcmItem::operator=(obj);
    // copy DcmDataset's member variables
    Xfer = obj.Xfer;
  }
  return *this;
}


DcmDataset::DcmDataset(const DcmDataset &old)
  : DcmItem(old),
    Xfer(old.Xfer)
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
    setLengthField(DCM_UndefinedLength);
    return result;
}

DcmEVR DcmDataset::ident() const
{
    return EVR_dataset;
}


E_TransferSyntax DcmDataset::getOriginalXfer() const
{
    return Xfer;
}


void DcmDataset::removeInvalidGroups()
{
    DcmStack stack;
    DcmObject *object = NULL;
    /* iterate over all elements */
    while (nextObject(stack, OFTrue).good())
    {
        object = stack.top();
        /* delete invalid elements */
        if ((object->getTag().getGroup() == 0x0002) || !object->getTag().hasValidGroup())
        {
            stack.pop();
            /* remove element from dataset and free memory */
            delete OFstatic_cast(DcmItem *, stack.top())->remove(object);
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
    register E_TransferSyntax originalXfer = Xfer;
    if (newXfer == EXS_Unknown)
        return OFFalse;
    if (Xfer == EXS_Unknown)
        originalXfer = oldXfer;

    /* Check stream compression for this transfer syntax */
    DcmXfer xf(newXfer);
    if (xf.getStreamCompression() == ESC_unsupported)
        return OFFalse;

    return DcmItem::canWriteXfer(newXfer, originalXfer);
}


// ********************************


void DcmDataset::print(STD_NAMESPACE ostream&out,
                       const size_t flags,
                       const int level,
                       const char *pixelFileName,
                       size_t *pixelCounter)
{
    out << OFendl;
    if (flags & DCMTypes::PF_useANSIEscapeCodes)
        out << ANSI_ESCAPE_CODE_COMMENT;
    printNestingLevel(out, flags, level);
    out << "# Dicom-Data-Set" << OFendl;
    if (flags & DCMTypes::PF_useANSIEscapeCodes)
        out << ANSI_ESCAPE_CODE_COMMENT;
    printNestingLevel(out, flags, level);
    out << "# Used TransferSyntax: " << DcmXfer(Xfer).getXferName() << OFendl;
    if (flags & DCMTypes::PF_useANSIEscapeCodes)
        out << ANSI_ESCAPE_CODE_RESET;
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


OFCondition DcmDataset::writeXML(STD_NAMESPACE ostream&out,
                                 const size_t flags)
{
    OFString xmlString;
    DcmXfer xfer(Xfer);
    /* XML start tag for "data-set" */
    out << "<data-set xfer=\"" << xfer.getXferID() << "\"";
    out << " name=\"" << OFStandard::convertToMarkupString(xfer.getXferName(), xmlString) << "\"";
    if (flags & DCMTypes::XF_useDcmtkNamespace)
        out << " xmlns=\"" << DCMTK_XML_NAMESPACE_URI << "\"";
    out << ">" << OFendl;
    if (!elementList->empty())
    {
        /* write content of all children */
        DcmObject *dO;
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            dO->writeXML(out, flags & ~DCMTypes::XF_useDcmtkNamespace);
        } while (elementList->seek(ELP_next));
    }
    /* XML end tag for "data-set" */
    out << "</data-set>" << OFendl;
    /* always report success */
    return EC_Normal;
}


// ********************************


OFCondition DcmDataset::read(DcmInputStream &inStream,
                             const E_TransferSyntax xfer,
                             const E_GrpLenEncoding glenc,
                             const Uint32 maxReadLength)
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
                /* To support incorrectly encoded datasets detect the transfer syntax from the stream.  */
                /* This is possible for given unknown and plain big or little endian transfer syntaxes. */
                switch( xfer )
                {
                    case EXS_Unknown:
                    case EXS_LittleEndianImplicit:
                    case EXS_LittleEndianExplicit:
                    case EXS_BigEndianExplicit:
                    case EXS_BigEndianImplicit:
                        Xfer = checkTransferSyntax(inStream);
                        if ((xfer != EXS_Unknown) && (Xfer != xfer))
                            DCMDATA_WARN("DcmDataset: Wrong transfer syntax specified, detecting from dataset");
                        break;
                    default:
                        Xfer = xfer;
                }
            }
            else /* default behaviour */
            {
                /* If the transfer syntax which was passed equals EXS_Unknown we want to */
                /* determine the transfer syntax from the information in the stream itself. */
                /* If the transfer syntax is given, we want to use it. */
                if (xfer == EXS_Unknown)
                    Xfer = checkTransferSyntax(inStream);
                else
                    Xfer = xfer;
            }
            /* Check stream compression for this transfer syntax */
            DcmXfer xf(Xfer);
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
            errorFlag = DcmItem::read(inStream, Xfer, glenc, maxReadLength);
    }

    /* if the error flag shows ok or that the end of the stream was encountered, */
    /* we have read information for this particular data set or command; in this */
    /* case, we need to do something for the current dataset object */
    if (errorFlag.good() || errorFlag == EC_EndOfStream)
    {
        /* set the error flag to ok */
        errorFlag = EC_Normal;

        /* take care of group length (according to what is specified */
        /* in glenc) and padding elements (don't change anything) */
        computeGroupLengthAndPadding(glenc, EPD_noChange, Xfer);

        /* and set the transfer state to ERW_ready to indicate that the data set is complete */
        setTransferState(ERW_ready);
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
      /* or (if it's an unknown tranfer syntax) we use the one which is contained in this->Xfer. */
      E_TransferSyntax newXfer = oxfer;
      if (newXfer == EXS_Unknown)
        newXfer = Xfer;

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
          setTransferState(ERW_ready);
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
      newXfer = Xfer;

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
          setTransferState(ERW_ready);
      }
    }
  }
  return errorFlag;
}


// ********************************


OFCondition DcmDataset::loadFile(const char *filename,
                                 const E_TransferSyntax readXfer,
                                 const E_GrpLenEncoding groupLength,
                                 const Uint32 maxReadLength)
{
    OFCondition l_error = EC_IllegalParameter;
    /* check parameters first */
    if ((filename != NULL) && (strlen(filename) > 0))
    {
        /* open file for input */
        DcmInputFileStream fileStream(filename);

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
                l_error = read(fileStream, readXfer, groupLength, maxReadLength);
                transferEnd();
            }
        }
    }
    return l_error;
}


OFCondition DcmDataset::saveFile(const char *fileName,
                                 const E_TransferSyntax writeXfer,
                                 const E_EncodingType encodingType,
                                 const E_GrpLenEncoding groupLength,
                                 const E_PaddingEncoding padEncoding,
                                 const Uint32 padLength,
                                 const Uint32 subPadLength)
{
    DcmWriteCache wcache;
    OFCondition l_error = EC_IllegalParameter;
    /* check parameters first */
    if ((fileName != NULL) && (strlen(fileName) > 0))
    {
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
    OFStack<DcmStack> pixelStack;

    DcmStack resultStack;
    resultStack.push(this);
    while(search(DCM_PixelData, resultStack, ESM_afterStackTop, OFTrue).good() && l_error.good())
    {

        if (resultStack.top()->ident() == EVR_PixelData)
        {
            DcmPixelData * pixelData = OFstatic_cast(DcmPixelData *, resultStack.top());
            if (!pixelData->canChooseRepresentation(repType, repParam))
                l_error = EC_CannotChangeRepresentation;
            pixelStack.push(resultStack);
        }
        else
            l_error = EC_CannotChangeRepresentation;
    }
    if (l_error.good())
    {
        while(pixelStack.size() && l_error.good())
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
    }
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


/*
** CVS/RCS Log:
** $Log: dcdatset.cc,v $
** Revision 1.52  2010-11-12 12:16:11  joergr
** Output ANSI escape codes at the beginnig of each line in order to make sure
** that always the correct color is used in case of truncated multi-line output.
**
** Revision 1.51  2010-10-29 10:57:21  joergr
** Added support for colored output to the print() method.
**
** Revision 1.50  2010-10-20 16:44:16  joergr
** Use type cast macros (e.g. OFstatic_cast) where appropriate.
**
** Revision 1.49  2010-10-14 13:14:07  joergr
** Updated copyright header. Added reference to COPYRIGHT file.
**
** Revision 1.48  2009-12-04 16:55:33  joergr
** Sightly modified some log messages.
**
** Revision 1.47  2009-11-13 13:11:20  joergr
** Fixed minor issues in log output.
**
** Revision 1.46  2009-11-04 09:58:09  uli
** Switched to logging mechanism provided by the "new" oflog module
**
** Revision 1.45  2009-08-25 12:54:57  joergr
** Added new methods which remove all data elements with an invalid group number
** from the meta information header, dataset and/or fileformat.
**
** Revision 1.44  2008-07-17 10:31:31  onken
** Implemented copyFrom() method for complete DcmObject class hierarchy, which
** permits setting an instance's value from an existing object. Implemented
** assignment operator where necessary.
**
** Revision 1.43  2007-11-29 14:30:20  meichel
** Write methods now handle large raw data elements (such as pixel data)
**   without loading everything into memory. This allows very large images to
**   be sent over a network connection, or to be copied without ever being
**   fully in memory.
**
** Revision 1.42  2007/06/29 14:17:49  meichel
** Code clean-up: Most member variables in module dcmdata are now private,
**   not protected anymore.
**
** Revision 1.41  2006/08/15 15:49:54  meichel
** Updated all code in module dcmdata to correctly compile when
**   all standard C++ classes remain in namespace std.
**
** Revision 1.40  2005/12/08 15:40:59  meichel
** Changed include path schema for all DCMTK header files
**
** Revision 1.39  2005/12/02 08:51:44  joergr
** Changed macro NO_XFER_DETECTION_FOR_DATASETS into a global option that can
** be enabled and disabled at runtime.
**
** Revision 1.38  2005/12/01 09:57:26  joergr
** Added support for DICOM objects where the dataset is stored with a different
** transfer syntax than specified in the meta header.  The new behaviour can be
** disabled by compiling with the macro NO_XFER_DETECTION_FOR_DATASETS defined.
**
** Revision 1.37  2005/11/28 15:53:13  meichel
** Renamed macros in dcdebug.h
**
** Revision 1.36  2005/11/11 10:31:58  meichel
** Added explicit DcmDataset::clear() implementation.
**
** Revision 1.35  2005/11/07 17:22:33  meichel
** Implemented DcmFileFormat::clear(). Now the loadFile methods in class
**   DcmFileFormat and class DcmDataset both make sure that clear() is called
**   before new data is read, allowing for a re-use of the object.
**
** Revision 1.34  2004/02/04 16:19:15  joergr
** Adapted type casts to new-style typecast operators defined in ofcast.h.
** Removed acknowledgements with e-mail addresses from CVS log.
**
** Revision 1.33  2003/04/01 14:57:20  joergr
** Added support for XML namespaces.
**
** Revision 1.32  2002/12/09 09:30:49  wilkens
** Modified/Added doc++ documentation.
**
** Revision 1.31  2002/12/06 13:09:26  joergr
** Enhanced "print()" function by re-working the implementation and replacing
** the boolean "showFullData" parameter by a more general integer flag.
** Made source code formatting more consistent with other modules/files.
**
** Revision 1.30  2002/11/27 12:06:43  meichel
** Adapted module dcmdata to use of new header file ofstdinc.h
**
** Revision 1.29  2002/08/27 16:55:44  meichel
** Initial release of new DICOM I/O stream classes that add support for stream
**   compression (deflated little endian explicit VR transfer syntax)
**
** Revision 1.28  2002/07/10 11:47:45  meichel
** Added workaround for memory leak in handling of compressed representations
**   Conditional compilation with PIXELSTACK_MEMORY_LEAK_WORKAROUND #defined.
**
** Revision 1.27  2002/05/14 08:20:53  joergr
** Renamed some element names.
**
** Revision 1.26  2002/04/25 10:14:12  joergr
** Added support for XML output of DICOM objects.
**
** Revision 1.25  2002/04/16 13:43:15  joergr
** Added configurable support for C++ ANSI standard includes (e.g. streams).
**
** Revision 1.24  2002/04/11 12:27:11  joergr
** Added new methods for loading and saving DICOM files.
**
** Revision 1.23  2001/11/01 14:55:35  wilkens
** Added lots of comments.
**
** Revision 1.22  2001/09/26 15:49:29  meichel
** Modified debug messages, required by OFCondition
**
** Revision 1.21  2001/09/25 17:19:47  meichel
** Adapted dcmdata to class OFCondition
**
** Revision 1.20  2001/06/01 15:48:59  meichel
** Updated copyright header
**
** Revision 1.19  2001/05/03 08:15:21  meichel
** Fixed bug in dcmdata sequence handling code that could lead to application
**   failure in rare cases during parsing of a correct DICOM dataset.
**
** Revision 1.18  2000/11/07 16:56:18  meichel
** Initial release of dcmsign module for DICOM Digital Signatures
**
** Revision 1.17  2000/04/14 16:07:26  meichel
** Dcmdata library code now consistently uses ofConsole for error output.
**
** Revision 1.16  2000/03/08 16:26:30  meichel
** Updated copyright header.
**
** Revision 1.15  2000/02/10 10:52:16  joergr
** Added new feature to dcmdump (enhanced print method of dcmdata): write
** pixel data/item value fields to raw files.
**
** Revision 1.14  1999/03/31 09:25:19  meichel
** Updated copyright header in module dcmdata
**
** Revision 1.13  1998/07/15 15:51:47  joergr
** Removed several compiler warnings reported by gcc 2.8.1 with
** additional options, e.g. missing copy constructors and assignment
** operators, initialization of member variables in the body of a
** constructor instead of the member initialization list, hiding of
** methods by use of identical names, uninitialized member variables,
** missing const declaration of char pointers. Replaced tabs by spaces.
**
** Revision 1.12  1997/07/21 08:16:43  andreas
** - New environment for encapsulated pixel representations. DcmPixelData
**   can contain different representations and uses codecs to convert
**   between them. Codecs are derived from the DcmCodec class. New error
**   codes are introduced for handling of representations. New internal
**   value representation (only for ident()) for PixelData
** - new copy constructor for DcmStack
** - Replace all boolean types (BOOLEAN, CTNBOOLEAN, DICOM_BOOL, BOOL)
**   with one unique boolean type OFBool.
**
** Revision 1.11  1997/07/03 15:09:53  andreas
** - removed debugging functions Bdebug() and Edebug() since
**   they write a static array and are not very useful at all.
**   Cdebug and Vdebug are merged since they have the same semantics.
**   The debugging functions in dcmdata changed their interfaces
**   (see dcmdata/include/dcdebug.h)
**
** Revision 1.10  1997/06/06 09:55:28  andreas
** - corrected error: canWriteXfer returns false if the old transfer syntax
**   was unknown, which causes several applications to prohibit the writing
**   of dataset.
**
** Revision 1.9  1997/05/27 13:48:57  andreas
** - Add method canWriteXfer to class DcmObject and all derived classes.
**   This method checks whether it is possible to convert the original
**   transfer syntax to an new transfer syntax. The check is used in the
**   dcmconv utility to prohibit the change of a compressed transfer
**   syntax to a uncompressed.
**
** Revision 1.8  1997/05/16 08:23:52  andreas
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
**
** Revision 1.7  1996/08/05 08:46:08  andreas
** new print routine with additional parameters:
**         - print into files
**         - fix output length for elements
** corrected error in search routine with parameter ESM_fromStackTop
**
** Revision 1.6  1996/04/25 17:08:04  hewett
** Removed out-of-date comment about RESOLVE_AMBIGOUS_VR_OF_PIXELDATA.
**
** Revision 1.5  1996/03/13 14:44:23  hewett
** The DcmDataset::resolveAmbiguous() method was setting the VR of
** PixelData to OW if a non-encapsulated transfer syntax was in use.
** This should only be done if the transfer syntax is implicit.  Any
** explicit transfer syntax will carry the VR with the data.
** Solution: Delete the code in DcmDataset::resolveAmbiguous().
** The VR of PixelData is being correctly set in
** cmItem::readSubElement(...) according to Correction Proposal 14.
**
** Revision 1.4  1996/01/09 11:06:43  andreas
** New Support for Visual C++
** Correct problems with inconsistent const declarations
** Correct error in reading Item Delimitation Elements
**
** Revision 1.3  1996/01/05 13:27:33  andreas
** - changed to support new streaming facilities
** - unique read/write methods for file and block transfer
** - more cleanups
**
*/
