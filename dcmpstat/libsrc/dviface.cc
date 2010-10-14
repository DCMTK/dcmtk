/*
 *
 *  Copyright (C) 1998-2010, OFFIS e.V.
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
 *  Module:  dcmpstat
 *
 *  Author:  Joerg Riesmeier, Marco Eichelberg
 *
 *  Purpose: DVPresentationState
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:14:31 $
 *  CVS/RCS Revision: $Revision: 1.165 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcmtk/dcmpstat/dviface.h"

#include "dcmtk/dcmpstat/dvpsdef.h"   /* for constants */
#include "dcmtk/ofstd/ofstring.h"     /* for class OFString */
#include "dcmtk/ofstd/ofbmanip.h"     /* for OFBitmanipTemplate */
#include "dcmtk/ofstd/ofdatime.h"     /* for OFDateTime */
#include "dcmtk/ofstd/oflist.h"       /* for class OFList */
#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/ofstd/ofcast.h"

#include "dcmtk/dcmimgle/digsdfn.h"   /* for DiGSDFunction */
#include "dcmtk/dcmimgle/diciefn.h"   /* for DiCIELABFunction */
#include "dcmtk/dcmnet/diutil.h"      /* for DU_getStringDOElement */
#include "dcmtk/dcmpstat/dvpssp.h"    /* for class DVPSStoredPrint */
#include "dcmtk/dcmpstat/dvpshlp.h"   /* for class DVPSHelper */
#include "dcmtk/dcmimgle/dcmimage.h"  /* for class DicomImage */
#include "dcmtk/dcmpstat/dvsighdl.h"  /* for class DVSignatureHandler */
#include "dcmtk/dcmsign/dcsignat.h"   /* for class DcmSignature */
#include "dcmtk/dcmsr/dsrdoc.h"       /* for class DSRDocument */
#include "dcmtk/dcmsr/dsrcodvl.h"     /* for class DSRCodedEntryValue */
#include "dcmtk/oflog/fileap.h"       /* for log4cplus::FileAppender */

#include "dcmtk/dcmpstat/dvpsib.h"    /* for DVPSImageBoxContent, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsab.h"    /* for DVPSAnnotationContent, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsov.h"    /* for DVPSOverlay, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsgl.h"    /* for DVPSGraphicLayer, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsal.h"    /* for DVPSOverlayCurveActivationLayer, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsga.h"    /* for DVPSGraphicAnnotation, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpscu.h"    /* for DVPSCurve, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsvl.h"    /* for DVPSVOILUT, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsvw.h"    /* for DVPSVOIWindow, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsda.h"    /* for DVPSDisplayedArea, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpssv.h"    /* for DVPSSoftcopyVOI, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsrs.h"    /* for DVPSReferencedSeries, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpstx.h"    /* for DVPSTextObject, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsgr.h"    /* for DVPSGraphicObject, needed by MSVC5 with STL */
#include "dcmtk/dcmpstat/dvpsri.h"    /* for DVPSReferencedImage, needed by MSVC5 with STL */
#include "dcmtk/dcmqrdb/dcmqrdbi.h"   /* for DB_UpperMaxBytesPerStudy */
#include "dcmtk/dcmqrdb/dcmqrdbs.h"   /* for DcmQueryRetrieveDatabaseStatus */

#define INCLUDE_CSTDIO
#define INCLUDE_CCTYPE
#define INCLUDE_CMATH
#define INCLUDE_UNISTD
#include "dcmtk/ofstd/ofstdinc.h"

BEGIN_EXTERN_C
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>   /* for fork */
#endif
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>    /* for waitpid */
#endif
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>    /* for wait3 */
#endif
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h> /* for wait3 */
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>    /* for stat, fstat */
#endif
#ifdef HAVE_SYS_UTIME_H
#include <sys/utime.h>   /* for utime */
#endif
#ifdef HAVE_UTIME_H
#include <utime.h>       /* for utime */
#endif
END_EXTERN_C

#ifdef HAVE_WINDOWS_H
#include <windows.h>
#include <winbase.h>     /* for CreateProcess */
#endif

#ifdef WITH_OPENSSL
#include "dcmtk/dcmtls/tlstrans.h"
#include "dcmtk/dcmtls/tlslayer.h"

BEGIN_EXTERN_C
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/err.h>
END_EXTERN_C
#endif


DVInterface::DVInterface(const char *config_file, OFBool useLog)
: DVConfiguration(config_file)
, pPrint(NULL)
, pState(NULL)
, pReport(NULL)
, pSignatureHandler(NULL)
, pStoredPState(NULL)
, pDicomImage(NULL)
, pDicomPState(NULL)
, pHardcopyImage(NULL)
, printJobIdentifier()
, printJobCounter(0)
, configPath()
, databaseIndexFile()
, referenceTime(0)
, pHandle(NULL)
, lockingMode(OFFalse)
, idxCache()
, idxRec()
, idxRecPos(-1)
, imageInDatabase(OFFalse)
, minimumPrintBitmapWidth(0)
, minimumPrintBitmapHeight(0)
, maximumPrintBitmapWidth(0)
, maximumPrintBitmapHeight(0)
, maximumPrintPreviewWidth(0)
, maximumPrintPreviewHeight(0)
, maximumPreviewImageWidth(0)
, maximumPreviewImageHeight(0)
, currentPrinter()
, displayCurrentLUTID()
, printCurrentLUTID()
, printerMediumType()
, printerFilmDestination()
, printerFilmSessionLabel()
, printerNumberOfCopies(0)
, printerPriority()
, printerOwnerID()
, activateAnnotation(OFFalse)
, prependDateTime(OFTrue)
, prependPrinterName(OFTrue)
, prependLighting(OFTrue)
, annotationText()
{
#ifdef WITH_OPENSSL
    DcmSignature::initializeLibrary(); // initializes OpenSSL for dcmsign and dcmtls
#endif

    clearIndexRecord(idxRec, idxRecPos);
    if (config_file) configPath = config_file;

    /* initialize display transform (only on low-cost systems) */
    for (int i = DVPSD_first; i < DVPSD_max;i++)
        displayFunction[i] = NULL;
    if (!getGUIConfigEntryBool(L2_HIGHRESOLUTIONGRAPHICS, OFFalse))
    {
        const char *displayFunctionFile = getMonitorCharacteristicsFile();
        if (displayFunctionFile && (strlen(displayFunctionFile) > 0))
        {
            DiDisplayFunction *df = new DiGSDFunction(displayFunctionFile);
            if (df && (df->isValid()))
            {
                displayFunction[DVPSD_GSDF] = df;
                df = new DiCIELABFunction(displayFunctionFile);
                if (df && (df->isValid()))
                    displayFunction[DVPSD_CIELAB] = df;
            }
            else
            {
                if (df) delete df;
                DCMPSTAT_WARN("Unable to load monitor characterics file '" << displayFunctionFile << "', ignoring");
            }
        }
    }

    minimumPrintBitmapWidth   = getMinPrintResolutionX();
    minimumPrintBitmapHeight  = getMinPrintResolutionY();
    maximumPrintBitmapWidth   = getMaxPrintResolutionX();
    maximumPrintBitmapHeight  = getMaxPrintResolutionY();
    maximumPreviewImageWidth  = getMaxPreviewResolutionX();
    maximumPreviewImageHeight = getMaxPreviewResolutionY();

    pPrint = new DVPSStoredPrint(getDefaultPrintIllumination(), getDefaultPrintReflection(), getNetworkAETitle());
    pState = new DVPresentationState(OFstatic_cast(DiDisplayFunction **, displayFunction),
      minimumPrintBitmapWidth, minimumPrintBitmapHeight, maximumPrintBitmapWidth, maximumPrintBitmapHeight,
      maximumPreviewImageWidth, maximumPreviewImageHeight);
    pReport = new DSRDocument();
    pSignatureHandler = new DVSignatureHandler(*this);

    referenceTime = OFstatic_cast(unsigned long, time(NULL));
    /* initialize printJobIdentifier with a string comprising the current time */
    char buf[20];
    sprintf(buf, "%lu", referenceTime);
    printJobIdentifier = buf;
    /* initialize reference time with "yesterday" */
    if (referenceTime >= 86400) referenceTime -= 86400; // subtract one day
    setCurrentPrinter(getTargetID(0, DVPSE_printAny));

    if (useLog)
    {
        const char *filename = getLogFile();
        if (filename != NULL)
        {
            const char *directory = getLogFolder();
            OFString filepath;
            if (directory != NULL)
            {
                filepath = directory;
                filepath += PATH_SEPARATOR;
                filepath += filename;
            } else
                filepath = filename;

            // This will badly interact with oflog config files :(
            const char *pattern = "%D, Level %p, Module DCMPSTAT%n%m%n";
            OFauto_ptr<log4cplus::Layout> layout(new log4cplus::PatternLayout(pattern));
            log4cplus::SharedAppenderPtr logfile(new log4cplus::FileAppender(filepath, STD_NAMESPACE ios::app));
            // We can't use OFLog::getLogger() here because that doesn't let us
            // configure the object
            log4cplus::Logger log = log4cplus::Logger::getInstance("dcmtk.dcmpstat.logfile");

            logfile->setLayout(layout);
            log.addAppender(logfile);
            log.setLogLevel(getLogLevel());

            // All log messages to the logger "dcmtk.dcmpstat" are now written
            // to the log file <filepath>
        }
    }

    DCMPSTAT_LOGFILE("---------------------------\n--- Application started ---\n---------------------------");
}


DVInterface::~DVInterface()
{
    DCMPSTAT_INFO("Application terminated");
    delete pPrint;
    delete pState;
    delete pReport;
    delete pSignatureHandler;
    delete pStoredPState;
    delete pDicomImage;
    delete pDicomPState;
    delete pHardcopyImage;
    for (int i = DVPSD_first; i < DVPSD_max; i++)
      delete displayFunction[i];
    if (pHandle) releaseDatabase();
    // refresh database index file access time
    if (databaseIndexFile.length() > 0)
        // cast to char* required for gcc 2.5.8 on NeXTSTEP
        utime(OFconst_cast(char *, databaseIndexFile.c_str()), NULL);
}


OFCondition DVInterface::loadImage(const char *studyUID,
                                   const char *seriesUID,
                                   const char *instanceUID,
                                   OFBool changeStatus)
{
    OFCondition status = EC_IllegalCall;
    if (studyUID && seriesUID && instanceUID)
    {
        if (lockDatabase() == EC_Normal)
        {
            const char *filename = getFilename(studyUID, seriesUID, instanceUID);
            if (filename)
            {
                if ((status = loadImage(filename)) == EC_Normal)
                {
                    imageInDatabase = OFTrue;
                    if (changeStatus)
                        instanceReviewed(studyUID, seriesUID, instanceUID);
                }
            } else
                DCMPSTAT_LOGFILE("Load image from database failed: UIDs not in index file");
        } else
            DCMPSTAT_LOGFILE("Load image from database failed: could not lock index file");
    } else
        DCMPSTAT_LOGFILE("Load image from database failed: invalid UIDs");
   return status;
}


OFCondition DVInterface::loadImage(const char *imgName)
{
    OFCondition status = EC_IllegalCall;
    DcmFileFormat *image = NULL;
    DVPresentationState *newState = new DVPresentationState(OFstatic_cast(DiDisplayFunction **, displayFunction),
      minimumPrintBitmapWidth, minimumPrintBitmapHeight, maximumPrintBitmapWidth, maximumPrintBitmapHeight,
      maximumPreviewImageWidth, maximumPreviewImageHeight);
    if (newState==NULL)
    {
        DCMPSTAT_LOGFILE("Load image from file failed: memory exhausted");
        return EC_MemoryExhausted;
    }

    if ((status = DVPSHelper::loadFileFormat(imgName, image)) == EC_Normal)
    {
        if (image)
        {
            DcmDataset *dataset = image->getDataset();
            if (dataset)
            {
              if (EC_Normal == (status = newState->createFromImage(*dataset)))
                status = newState->attachImage(image, OFFalse);
              if (EC_Normal == status)
              {
                exchangeImageAndPState(newState, image);
                imageInDatabase = OFFalse;
              }
            } else status = EC_CorruptedData;
        } else status = EC_IllegalCall;
        if (status != EC_Normal)
            DCMPSTAT_LOGFILE("Load image from file failed: invalid data structures");
    } else
       DCMPSTAT_LOGFILE("Load image from file failed: could not read fileformat");
    if (status != EC_Normal)
    {
        delete newState;
        delete image;
    }
    return status;
}


OFCondition DVInterface::loadReferencedImage(size_t idx, OFBool changeStatus)
{
    OFCondition status = EC_IllegalCall;
    if ((pState != NULL) && (idx < pState->numberOfImageReferences()))
    {
        OFString ofstudyUID, ofseriesUID, ofsopclassUID, ofinstanceUID, offrames, aetitle, mediaID, mediaUID;
        if ((status = pState->getImageReference(idx, ofstudyUID, ofseriesUID, ofsopclassUID, ofinstanceUID,
            offrames, aetitle, mediaID, mediaUID)) == EC_Normal)
        {
            if (lockDatabase() == EC_Normal)
            {
                const char *filename = getFilename(ofstudyUID.c_str(), ofseriesUID.c_str(), ofinstanceUID.c_str());
                if (filename != NULL)
                {
                    DcmFileFormat *image = NULL;
                    if ((status = DVPSHelper::loadFileFormat(filename, image)) == EC_Normal)
                    {
                        status = EC_IllegalCall;
                        if (image != NULL)
                        {
                            DcmDataset *dataset = image->getDataset();
                            if (dataset != NULL)
                            {
                                if ((status = pState->attachImage(image, OFFalse)) == EC_Normal)
                                {
                                    exchangeImageAndPState(pState, image);      // do not exchange pState
                                    imageInDatabase = OFTrue;
                                    if (changeStatus)
                                        instanceReviewed(ofstudyUID.c_str(), ofseriesUID.c_str(), ofinstanceUID.c_str());
                                }
                            }
                        }
                        if (status != EC_Normal)
                            DCMPSTAT_LOGFILE("Load referenced image from file failed: invalid data structures");
                    } else {
                        status = EC_CorruptedData;
                        DCMPSTAT_LOGFILE("Load referenced image from file failed: could not read fileformat");
                    }
                    if (status != EC_Normal)
                        delete image;
                } else {
                    status = EC_IllegalCall;
                    DCMPSTAT_LOGFILE("Load referenced image from database failed: UIDs not in index file");
                }
            } else
                DCMPSTAT_LOGFILE("Load referenced image from database failed: could not lock index file");
        } else
            DCMPSTAT_LOGFILE("Load referenced image from database failed: internal error");
    } else
        DCMPSTAT_LOGFILE("Load referenced image from database failed: internal error");
    return status;
}


OFCondition DVInterface::loadPState(const char *studyUID,
                                    const char *seriesUID,
                                    const char *instanceUID,
                                    OFBool changeStatus)
{
    // determine the filename of the presentation state
    OFCondition status = lockDatabase();
    if (status != EC_Normal)
    {
        DCMPSTAT_LOGFILE("Load presentation state from database failed: could not lock index file");
        return status;
    }
    const char *filename = getFilename(studyUID, seriesUID, instanceUID);
    if (filename==NULL)
    {
        DCMPSTAT_LOGFILE("Load presentation state from database failed: UIDs not in index file");
        return EC_IllegalCall;
    }

    // load the presentation state
    DcmFileFormat *pstate = NULL;
    DVPresentationState *newState = new DVPresentationState(OFstatic_cast(DiDisplayFunction **, displayFunction),
      minimumPrintBitmapWidth, minimumPrintBitmapHeight, maximumPrintBitmapWidth, maximumPrintBitmapHeight,
      maximumPreviewImageWidth, maximumPreviewImageHeight);
    if (newState==NULL)
    {
        DCMPSTAT_LOGFILE("Load presentation state from database failed: memory exhausted");
        return EC_MemoryExhausted;
    }

    if ((EC_Normal == (status = DVPSHelper::loadFileFormat(filename, pstate)))&&(pstate))
    {
        DcmDataset *dataset = pstate->getDataset();
        if (dataset) status = newState->read(*dataset); else status = EC_CorruptedData;
    }
    if (status == EC_Normal)
    {
        // access the first image reference in the presentation state
        OFString ofstudyUID, ofseriesUID, ofsopclassUID, ofinstanceUID, offrames, aetitle, mediaID, mediaUID;
        status = newState->getImageReference(0, ofstudyUID, ofseriesUID, ofsopclassUID, ofinstanceUID,
          offrames, aetitle, mediaID, mediaUID);

        // we could do something fancy with the retrieve AE title and the storage media ID here,
        // but we just assume that the referenced image is in our local database.
        if (EC_Normal == status)
        {
            // determine the filename of the referenced image
            const char *imagefilename = NULL;
            imagefilename = getFilename(ofstudyUID.c_str(), ofseriesUID.c_str(), ofinstanceUID.c_str());

            // load the image file and attach it
            if (imagefilename)
            {
                DcmFileFormat *fimage = NULL;
                if ((EC_Normal == (status = DVPSHelper::loadFileFormat(imagefilename, fimage)))&&(fimage))
                {
                    DcmDataset *dataset = pstate->getDataset();
                    if (dataset)
                    {
                        status = newState->attachImage(fimage, OFFalse);
                        if (EC_Normal == status)
                        {
                          exchangeImageAndPState(newState, fimage, pstate);
                          imageInDatabase = OFTrue;
                          if (changeStatus)
                          {
                              // mark pstate and (first) image reviewed
                              instanceReviewed(studyUID, seriesUID, instanceUID);
                              instanceReviewed(ofstudyUID.c_str(), ofseriesUID.c_str(), ofinstanceUID.c_str());
                          }
                        }
                    } else status = EC_CorruptedData;
                }
                if (status!=EC_Normal)
                {
                    delete fimage;
                    DCMPSTAT_LOGFILE("Load presentation state from database failed: could not read image data");
                }
            } else {
                status = EC_IllegalCall;     // no valid image filename
                DCMPSTAT_LOGFILE("Load presentation state from database failed: referenced image not in index file");
            }
        }
    } else
        DCMPSTAT_LOGFILE("Load presentation state from database failed: could not read fileformat");
    if (status!=EC_Normal)
    {
        delete pstate;
        delete newState;
    }
    return status;
}


OFCondition DVInterface::loadPState(const char *pstName,
                                    const char *imgName)
{
    OFCondition status = EC_IllegalCall;
    DcmFileFormat *pstate = NULL;
    DcmFileFormat *image = pDicomImage;     // default: do not replace image if image filename is NULL
    DVPresentationState *newState = new DVPresentationState(OFstatic_cast(DiDisplayFunction **, displayFunction),
      minimumPrintBitmapWidth, minimumPrintBitmapHeight, maximumPrintBitmapWidth, maximumPrintBitmapHeight,
      maximumPreviewImageWidth, maximumPreviewImageHeight);
    if (newState==NULL)
    {
        DCMPSTAT_LOGFILE("Load presentation state from file failed: memory exhausted");
        return EC_MemoryExhausted;
    }

    if ((status = DVPSHelper::loadFileFormat(pstName, pstate)) == EC_Normal)
    {
        if ((imgName == NULL) || ((status = DVPSHelper::loadFileFormat(imgName, image)) == EC_Normal))
        {
            if ((pstate)&&(image))
            {
                DcmDataset *dataset = pstate->getDataset();
                if (dataset)
                {
                    if (EC_Normal == (status = newState->read(*dataset)))
                        status = newState->attachImage(image, OFFalse);
                    if (EC_Normal == status)
                    {
                        exchangeImageAndPState(newState, image, pstate);
                        imageInDatabase = OFFalse;
                    }
                } else status = EC_CorruptedData;
            } else status = EC_IllegalCall;
            if (status != EC_Normal)
                DCMPSTAT_LOGFILE("Load presentation state from file failed: invalid data structures");
        } else
            DCMPSTAT_LOGFILE("Load presentation state from file failed: could not load image");
    } else
        DCMPSTAT_LOGFILE("Load presentation state from file failed: could not read fileformat");
    if (status != EC_Normal)
    {
        delete newState;
        if (image != pDicomImage)
            delete image;
        delete pstate;
    }
    return status;
}


OFCondition DVInterface::loadStructuredReport(const char *studyUID,
                                              const char *seriesUID,
                                              const char *instanceUID,
                                              OFBool changeStatus)
{
    OFCondition status = EC_IllegalCall;
    if (studyUID && seriesUID && instanceUID)
    {
        if (lockDatabase() == EC_Normal)
        {
            const char *filename = getFilename(studyUID, seriesUID, instanceUID);
            if (filename)
            {
                if ((status = loadStructuredReport(filename)) == EC_Normal)
                {
                    if (changeStatus)
                        instanceReviewed(studyUID, seriesUID, instanceUID);
                }
            } else
                DCMPSTAT_LOGFILE("Load structured report from database failed: UIDs not in index file");
        } else
            DCMPSTAT_LOGFILE("Load structured report from database failed: could not lock index file");
    } else
        DCMPSTAT_LOGFILE("Load structured report from database failed: invalid UIDs");
   return status;
}


OFCondition DVInterface::loadStructuredReport(const char *filename)
{
    OFCondition status = EC_IllegalCall;
    DcmFileFormat *fileformat = NULL;
    DSRDocument *newReport = new DSRDocument();
    if (newReport == NULL)
    {
        DCMPSTAT_LOGFILE("Load structured report from file failed: memory exhausted");
        return EC_MemoryExhausted;
    }

    if ((status = DVPSHelper::loadFileFormat(filename, fileformat)) == EC_Normal)
    {
        if (fileformat)
        {
            DcmDataset *dataset = fileformat->getDataset();
            if (dataset)
            {
                if ((status = newReport->read(*dataset, DSRTypes::RF_readDigitalSignatures)) == EC_Normal)
                {
                    delete pReport;
                    pReport = newReport;
                    if (pSignatureHandler)
                    {
                        pSignatureHandler->updateDigitalSignatureInformation(*dataset, DVPSS_structuredReport, OFTrue);
                        /* check whether loaded report is 'finalized' (certain attributes are digitally signed) */
                        DcmAttributeTag tagList(DcmTag(0, 0) /* irrelevant value */);
                        tagList.putTagVal(DCM_SOPInstanceUID, 0);
                        tagList.putTagVal(DCM_VerifyingObserverSequence, 1);
                        tagList.putTagVal(DCM_InstanceCreationDate, 2);
                        tagList.putTagVal(DCM_InstanceCreationTime, 3);
                        tagList.putTagVal(DCM_InstanceCreatorUID, 4);
                        if (pSignatureHandler->attributesSigned(*dataset, tagList))
                            pReport->finalizeDocument();
                    }
                }
            } else
                status = EC_CorruptedData;
        } else
            status = EC_IllegalCall;
        if (status != EC_Normal)
            DCMPSTAT_LOGFILE("Load structured report from file failed: invalid data structures");
    } else
       DCMPSTAT_LOGFILE("Load structured report from file failed: could not read fileformat");
    if (status != EC_Normal)
        delete newReport;
    delete fileformat;
    return status;
}


OFCondition DVInterface::loadSRTemplate(const char *reportID)
{
  OFCondition result = EC_IllegalCall;
  if (reportID)
  {
     const char *srfile = getReportFilename(reportID);
     if (srfile)
     {
       OFString filename = getReportFolder(); // never NULL.
       filename += PATH_SEPARATOR;
       filename += srfile;
       result = loadStructuredReport(filename.c_str());
       if (result == EC_Normal)
       {
         if (pReport != NULL)
         {
           /* date/time is filled automatically if empty */
           pReport->setContentDate("");
           pReport->setContentTime("");
           /* generate new study/series/instance UID and fill date/time */
           pReport->createNewStudy();
         }
       } else
         DCMPSTAT_LOGFILE("Load structured reporting 'template' from file failed");
     }
  }
  return result;
}


OFCondition DVInterface::savePState(OFBool replaceSOPInstanceUID)
{
    // release database lock since we are using the DB module directly
    releaseDatabase();

    if (pState==NULL) return EC_IllegalCall;
    const char *instanceUID = NULL;
    if (replaceSOPInstanceUID) instanceUID=pState->createInstanceUID(); else instanceUID=pState->getInstanceUID();
    if (instanceUID==NULL) return EC_IllegalCall;

    DcmQueryRetrieveDatabaseStatus dbStatus(STATUS_Success);
    OFCondition result=EC_Normal;
    char imageFileName[MAXPATHLEN+1];

    DcmQueryRetrieveIndexDatabaseHandle dbhandle(getDatabaseFolder(), PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, result);
    if (result.bad())
    {
        DCMPSTAT_LOGFILE("Save presentation state to database failed: could not lock index file");
        return EC_IllegalCall;
    }

    if (dbhandle.makeNewStoreFileName(UID_GrayscaleSoftcopyPresentationStateStorage, instanceUID, imageFileName).good())
    {
        // now store presentation state as filename
        result = savePState(imageFileName, OFFalse);
        if (EC_Normal==result)
        {
            if (dbhandle.storeRequest(UID_GrayscaleSoftcopyPresentationStateStorage,
                instanceUID, imageFileName, &dbStatus).bad())
            {
                result = EC_IllegalCall;
                DCMPSTAT_LOGFILE("Save presentation state to database failed: could not register in index file");
                DCMPSTAT_DEBUG("Unable to register presentation state '" << imageFileName << "' in database");
            }
        }
    }
    if (pDicomImage != NULL)
    {
        DcmDataset *dset = pDicomImage->getDataset();
        if (dset != NULL)
        {
            DIC_UI sopClass;
            DIC_UI instanceUID2;
            DIC_UI seriesUID;
            DIC_UI studyUID;
            if (DU_getStringDOElement(dset, DCM_SOPClassUID, sopClass) &&
                    DU_getStringDOElement(dset, DCM_SOPInstanceUID, instanceUID2) &&
                    DU_getStringDOElement(dset, DCM_SeriesInstanceUID, seriesUID) &&
                    DU_getStringDOElement(dset, DCM_StudyInstanceUID, studyUID) &&
                ((!imageInDatabase) || (getSeriesStruct(studyUID, seriesUID, instanceUID2) == NULL)))
            {
                releaseDatabase();   /* avoid deadlocks */
                if (dbhandle.makeNewStoreFileName(sopClass, instanceUID2, imageFileName).good())
                {
                    // now store presentation state as filename
                    result = saveCurrentImage(imageFileName);
                    if (EC_Normal==result)
                    {
                        if (dbhandle.storeRequest(sopClass, instanceUID2, imageFileName, &dbStatus).bad())
                        {
                            result = EC_IllegalCall;
                            DCMPSTAT_LOGFILE("Save presentation state to database failed: could not register image in index file");
                            DCMPSTAT_DEBUG("Unable to register image '" << imageFileName << "' in database");
                        } else {
                            imageInDatabase = OFTrue;
                        }
                    }
                }
            }
        }
    }
    return result;
}


OFCondition DVInterface::savePState(const char *filename, OFBool replaceSOPInstanceUID, OFBool explicitVR)
{
    if (pState==NULL) return EC_IllegalCall;
    if (filename==NULL) return EC_IllegalCall;

    OFCondition status = EC_IllegalCall;
    DcmFileFormat *fileformat = new DcmFileFormat();
    DcmDataset *dataset = NULL;
    if (fileformat) dataset=fileformat->getDataset();

    if (dataset)
    {
        if ((status = pState->write(*dataset, replaceSOPInstanceUID)) == EC_Normal)
        {
          status = DVPSHelper::saveFileFormat(filename, fileformat, explicitVR);

          // replace the stored data for resetPresentationState()
          delete pDicomPState;
          pDicomPState = fileformat;
          fileformat = NULL; // make sure we don't delete fileformat later
          if (pSignatureHandler)
          {
            pSignatureHandler->updateDigitalSignatureInformation(*pDicomPState->getDataset(), DVPSS_presentationState, OFFalse);
          }
        }
        if (status != EC_Normal)
            DCMPSTAT_LOGFILE("Save presentation state to file failed: could not write fileformat");
    } else {
        DCMPSTAT_LOGFILE("Save presentation state to file failed: memory exhausted");
        status = EC_MemoryExhausted;
    }

    delete fileformat;
    return status;
}


OFCondition DVInterface::saveCurrentImage(const char *filename, OFBool explicitVR)
{
    if (filename==NULL) return EC_IllegalCall;
    if (pDicomImage==NULL) return EC_IllegalCall;
    OFCondition result = DVPSHelper::saveFileFormat(filename, pDicomImage, explicitVR);
    if (result != EC_Normal)
        DCMPSTAT_LOGFILE("Save image to file failed: could not write fileformat");
    return result;
}


OFCondition DVInterface::saveStructuredReport()
{
    // release database lock since we are using the DB module directly
    releaseDatabase();

    if (pReport == NULL)
        return EC_IllegalCall;
    OFString sopClassUID;
    if (pReport->getSOPClassUID(sopClassUID).length() == 0)
        return EC_IllegalCall;
    OFString instanceUID;
    if (pReport->getSOPInstanceUID(instanceUID).length() == 0)
        return EC_IllegalCall;

    DcmQueryRetrieveDatabaseStatus dbStatus(STATUS_Success);
    char filename[MAXPATHLEN+1];
    OFCondition result = EC_Normal;

    DcmQueryRetrieveIndexDatabaseHandle dbhandle(getDatabaseFolder(), PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, result);
    if (result.bad())
    {
        DCMPSTAT_LOGFILE("Save structured report to database failed: could not lock index file");
        return EC_IllegalCall;
    }

    if (dbhandle.makeNewStoreFileName(sopClassUID.c_str(), instanceUID.c_str(), filename).good())
    {
        // now store presentation state as filename
        result = saveStructuredReport(filename);
        if (EC_Normal == result)
        {
            if (dbhandle.storeRequest(sopClassUID.c_str(), instanceUID.c_str(), filename, &dbStatus).bad())
            {
                result = EC_IllegalCall;
                DCMPSTAT_LOGFILE("Save structured report to database failed: could not register in index file");
                DCMPSTAT_DEBUG("Unable to register structured report '" << filename << "' in database");
            }
        }
    }
    return result;
}


OFCondition DVInterface::saveStructuredReport(const char *filename, OFBool explicitVR)
{
    if (pReport==NULL) return EC_IllegalCall;
    if (filename==NULL) return EC_IllegalCall;

    OFCondition status = EC_IllegalCall;
    DcmFileFormat *fileformat = new DcmFileFormat();
    DcmDataset *dataset = NULL;
    if (fileformat) dataset=fileformat->getDataset();

    if (dataset)
    {
        /* always add information about private OFFIS DCMTK Coding Scheme */
        pReport->getCodingSchemeIdentification().addPrivateDcmtkCodingScheme();
        if ((status = pReport->write(*dataset)) == EC_Normal)
        {
            status = DVPSHelper::saveFileFormat(filename, fileformat, explicitVR);
            if (pSignatureHandler)
            {
              pSignatureHandler->updateDigitalSignatureInformation(*dataset, DVPSS_structuredReport, OFFalse);
            }
        }
        if (status != EC_Normal)
            DCMPSTAT_LOGFILE("Save structured report to file failed: could not write fileformat");
    } else {
        DCMPSTAT_LOGFILE("Save structured report to file failed: memory exhausted");
        status = EC_MemoryExhausted;
    }

    delete fileformat;
    return status;
}


OFCondition DVInterface::addImageReferenceToPState(const char *studyUID, const char *seriesUID, const char *instanceUID)
{
    OFCondition status = EC_IllegalCall;
    if (pState && studyUID && seriesUID && instanceUID)
    {
        OFString study = pState->getStudyUID();
        if (study == studyUID)
        {
            if (lockDatabase() == EC_Normal)
            {
                const char *filename = getFilename(studyUID, seriesUID, instanceUID);
                if (filename)
                {
                    DcmFileFormat *image = NULL;
                    if ((status = DVPSHelper::loadFileFormat(filename, image)) == EC_Normal)
                    {
                        status = EC_CorruptedData;
                        if (image)
                        {
                            DcmDataset *dataset = image->getDataset();
                            if (dataset)
                                status = pState->addImageReference(*dataset);
                        }
                        if (status != EC_Normal)
                            DCMPSTAT_LOGFILE("Add image reference to presentation state failed: invalid data structures");
                    } else
                        DCMPSTAT_LOGFILE("Add image reference to presentation state failed: could not read fileformat");
                    delete image;
                } else
                    DCMPSTAT_LOGFILE("Add image reference to presentation state failed: UIDs not in index file");
            } else
                DCMPSTAT_LOGFILE("Add image reference to presentation state failed: could not lock index file");
        } else
            DCMPSTAT_LOGFILE("Add image reference to presentation state failed: not the same study UID");
    } else
        DCMPSTAT_LOGFILE("Add image reference to presentation state failed: invalid UIDs");
    return status;
}


size_t DVInterface::getNumberOfImageReferences()
{
    if (pState != NULL)
        return pState->numberOfImageReferences();
    return 0;
}


OFCondition DVInterface::exchangeImageAndPState(DVPresentationState *newState, DcmFileFormat *image, DcmFileFormat *state)
{
    if (newState==NULL) return EC_IllegalCall;
    if (image==NULL) return EC_IllegalCall;
    if (pState != newState)
    {
        delete pState;
        delete pStoredPState;
        delete pDicomPState;
        pState = newState;
        pStoredPState = NULL;
        pDicomPState = state;
        if (pSignatureHandler)
        {
          if (pDicomPState) pSignatureHandler->updateDigitalSignatureInformation(*pDicomPState->getDataset(), DVPSS_presentationState, OFTrue);
          else pSignatureHandler->disableDigitalSignatureInformation(DVPSS_presentationState);
        }
    }
    if (pDicomImage != image)
    {
        delete pDicomImage;       // delete only if different
        pDicomImage = image;
        if (pSignatureHandler)
        {
          pSignatureHandler->updateDigitalSignatureInformation(*pDicomImage->getDataset(), DVPSS_image, OFTrue);
        }
    }
    return EC_Normal;
}


OFCondition DVInterface::resetPresentationState()
{
    DVPresentationState *newState = new DVPresentationState(displayFunction,
      minimumPrintBitmapWidth, minimumPrintBitmapHeight, maximumPrintBitmapWidth, maximumPrintBitmapHeight,
      maximumPreviewImageWidth, maximumPreviewImageHeight);
    if (newState==NULL) return EC_MemoryExhausted;

    OFCondition status = EC_Normal;
    if ((pDicomImage)&&(pDicomPState))
    {
        // both image and presentation state are present
        DcmDataset *dataset = pDicomPState->getDataset();
        if (dataset)
        {
            if (EC_Normal == (status = newState->read(*dataset))) status = newState->attachImage(pDicomImage, OFFalse);
            if (EC_Normal == status)
            {
                if (pState) delete pState;
                if (pStoredPState) delete pStoredPState;
                pState = newState;
                pStoredPState = NULL;                   // return to original pstate
            }
        } else status = EC_IllegalCall;
    }
    else if (pDicomImage)
    {
        // only image is present
        DcmDataset *dataset = pDicomImage->getDataset();
        if (dataset)
        {
            if (EC_Normal == (status = newState->createFromImage(*dataset))) status = newState->attachImage(pDicomImage, OFFalse);
            if (EC_Normal == status)
            {
                if (pState) delete pState;
                if (pStoredPState) delete pStoredPState;
                pState = newState;
                pStoredPState = NULL;
            }
        } else status = EC_IllegalCall;
    }
    if (EC_Normal != status) delete newState;
    return status;
}


OFCondition DVInterface::saveCurrentPStateForReset()
{
    OFCondition status = EC_IllegalCall;
    if (pState != NULL)
    {
        DcmFileFormat *fileformat = new DcmFileFormat();
        if (fileformat != NULL)
        {
            DcmDataset *dataset = fileformat->getDataset();
            if (dataset)
            {
                status = pState->write(*dataset, OFFalse);  // write current state to 'reset' dataset
                if (status == EC_Normal)
                {
                    delete pDicomPState;
                    pDicomPState = fileformat;
                    fileformat = NULL;                      // avoid deletion of pDicomPState a few lines later
                }
            } else status = EC_MemoryExhausted;
        } else status = EC_MemoryExhausted;
        delete fileformat;
    }
    return status;
}


Uint32 DVInterface::getNumberOfPStates()
{
    if (createPStateCache())
    {
        DVInstanceCache::ItemStruct *instance = getInstanceStruct();
        if ((instance != NULL) && ((instance->Type == DVPSI_image) || (instance->Type == DVPSI_hardcopyGrayscale)))
            return instance->List.size();
    }
    return 0;
}


OFCondition DVInterface::selectPState(Uint32 idx, OFBool changeStatus)
{
    if (createPStateCache())
    {
        DVInstanceCache::ItemStruct *instance = getInstanceStruct();
        if ((instance != NULL) && ((instance->Type == DVPSI_image) || (instance->Type == DVPSI_hardcopyGrayscale)))
        {
            OFListIterator(DVInstanceCache::ItemStruct *) iter = instance->List.begin();
            OFListIterator(DVInstanceCache::ItemStruct *) last = instance->List.end();
            while (iter != last)
            {
                if (idx == 0)
                {
                    DVInstanceCache::ItemStruct *pstate = (*iter);
                    if (pstate != NULL)
                    {
                        OFCondition status = EC_IllegalCall;
                        if (pDicomImage == NULL)
                            status = loadPState(pstate->Filename.c_str(), instance->Filename.c_str());
                        else
                            status = loadPState(pstate->Filename.c_str());
                        if ((status == EC_Normal) && changeStatus)
                            instanceReviewed(pstate->Pos);
                        return status;
                    }
                }
                idx--;
                ++iter;
            }
        }
    }
    return EC_IllegalCall;
}


const char *DVInterface::getPStateDescription(Uint32 idx)
{
    if (createPStateCache())
    {
        DVInstanceCache::ItemStruct *instance = getInstanceStruct();
        if ((instance != NULL) && ((instance->Type == DVPSI_image) || (instance->Type == DVPSI_hardcopyGrayscale)))
        {
            OFListIterator(DVInstanceCache::ItemStruct *) iter = instance->List.begin();
            OFListIterator(DVInstanceCache::ItemStruct *) last = instance->List.end();
            while (iter != last)
            {
                if (idx == 0)
                {
                    DVInstanceCache::ItemStruct *pstate = (*iter);
                    if (pstate != NULL)
                        return pstate->Description.c_str();
                }
                idx--;
                ++iter;
            }
        }
    }
    return NULL;
}


const char *DVInterface::getPStateLabel(Uint32 idx)
{
    if (createPStateCache())
    {
        DVInstanceCache::ItemStruct *instance = getInstanceStruct();
        if ((instance != NULL) && ((instance->Type == DVPSI_image) || (instance->Type == DVPSI_hardcopyGrayscale)))
        {
            OFListIterator(DVInstanceCache::ItemStruct *) iter = instance->List.begin();
            OFListIterator(DVInstanceCache::ItemStruct *) last = instance->List.end();
            while (iter != last)
            {
                if (idx == 0)
                {
                    DVInstanceCache::ItemStruct *pstate = (*iter);
                    if (pstate != NULL)
                        return pstate->Label.c_str();
                }
                idx--;
                ++iter;
            }
        }
    }
    return NULL;
}


OFCondition DVInterface::disablePState()
{
  OFCondition status = EC_IllegalCall;
  if ((pState != NULL) && (pStoredPState == NULL))
  {
    if (pDicomImage != NULL)
    {
      DcmDataset *dataset = pDicomImage->getDataset();
      if (dataset != NULL)
      {
        DVPresentationState *newState = new DVPresentationState(displayFunction,
          minimumPrintBitmapWidth, minimumPrintBitmapHeight, maximumPrintBitmapWidth, maximumPrintBitmapHeight,
          maximumPreviewImageWidth, maximumPreviewImageHeight);
        if (newState != NULL)
        {
          if ((status = newState->createFromImage(*dataset)) == EC_Normal)
          {
            if ((status = newState->attachImage(pDicomImage, OFFalse)) == EC_Normal)
            {
              pStoredPState = pState;
              pState = newState;
              return EC_Normal;
            }
          }
          delete newState;
        }
      }
    }
  }
  return status;
}


OFCondition DVInterface::enablePState()
{
    if ((pState != NULL) && (pStoredPState != NULL))
    {
        delete pState;
        pState = pStoredPState;
        pStoredPState = NULL;
        return EC_Normal;
    }
    return EC_IllegalCall;
}


DVStudyCache::ItemStruct *DVInterface::getStudyStruct(const char *studyUID,
                                                      const char *seriesUID)
{
    if (createIndexCache())
    {
        if (studyUID)
        {
            if (idxCache.isElem(studyUID))
            {
                DVStudyCache::ItemStruct *study = idxCache.getItem();
                if ((seriesUID == NULL) || (study->List.isElem(seriesUID)))
                    return study;
            }
        } else
            return idxCache.getItem();          // current study
    }
    return NULL;
}


DVSeriesCache::ItemStruct *DVInterface::getSeriesStruct(const char *studyUID,
                                                        const char *seriesUID,
                                                        const char *instanceUID)
{
    if ((studyUID && seriesUID) || (!studyUID && !seriesUID))
    {
        DVStudyCache::ItemStruct *study = getStudyStruct(studyUID, seriesUID);
        if (study != NULL)
        {
            DVSeriesCache::ItemStruct *series = study->List.getItem();
            if (series != NULL)
            {
                if ((instanceUID == NULL) || (series->List.isElem(instanceUID)))
                    return series;
            }
        }
    }
    return NULL;
}


DVInstanceCache::ItemStruct *DVInterface::getInstanceStruct(const char *studyUID,
                                                            const char *seriesUID,
                                                            const char *instanceUID)
{
    if ((studyUID && seriesUID && instanceUID) || (!studyUID && !seriesUID && !instanceUID))
    {
        DVSeriesCache::ItemStruct *series = getSeriesStruct(studyUID, seriesUID, instanceUID);
        if (series != NULL)
            return series->List.getItem();
    }
    return NULL;
}


const char *DVInterface::getFilename(const char *studyUID,
                                     const char *seriesUID,
                                     const char *instanceUID)
{
    DVSeriesCache::ItemStruct *series = getSeriesStruct(studyUID, seriesUID, instanceUID);
    if (series != NULL)
        return series->List.getFilename();
    return NULL;
}


OFCondition DVInterface::lockDatabase()
{
    if (pHandle) return EC_Normal; // may be called multiple times

    OFCondition result;
    pHandle = new DcmQueryRetrieveIndexDatabaseHandle(getDatabaseFolder(), PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, result);
    if (result.good())
    {
        lockingMode = OFFalse;
        if (pHandle->DB_lock(OFFalse).good())
        {
            if (databaseIndexFile.length() == 0)
                databaseIndexFile = pHandle->getIndexFilename();
            return EC_Normal;
        }
    }
    return EC_IllegalCall;
}


OFCondition DVInterface::lockExclusive()
{
    if (pHandle && lockingMode) return EC_Normal;
    OFCondition result = EC_Normal;
    if (pHandle == NULL) result = lockDatabase();
    if (result.good())
    {
        // we now have a shared lock.
        pHandle->DB_unlock();
        if (pHandle->DB_lock(OFTrue).good())
            lockingMode = OFTrue;
        else
            result = EC_IllegalCall;
    }
    return result;
}


OFCondition DVInterface::unlockExclusive()
{
    if (pHandle && lockingMode)
    {
        if (pHandle->DB_unlock().good())
        {
            delete pHandle;
            pHandle=NULL;
            lockingMode=OFFalse;
            clearIndexCache();
            return EC_Normal;
        }
    }
    return EC_IllegalCall;
}


OFCondition DVInterface::releaseDatabase()
{
    if (pHandle == NULL) return EC_Normal;
    OFCondition cond = pHandle->DB_unlock();
    if (cond.good())
    {
      delete pHandle;
      pHandle = NULL;
      clearIndexCache();
    }
    return cond;
}


void DVInterface::resetDatabaseReferenceTime()
{
    // set index file modification time to "yesterday" to make sure
    // we notice any change even if different processes have minor
    // date/time differences (i.e. over NFS)
#ifdef HAVE_DECLARATION_STRUCT_UTIMBUF
    struct utimbuf utime_buf;
    utime_buf.actime  = OFstatic_cast(time_t, referenceTime);
    utime_buf.modtime = OFstatic_cast(time_t, referenceTime);
    if (0 != utime(databaseIndexFile.c_str(), &utime_buf))
#else
    // some old platforms use the prototype int utime(char *file, time_t timep[])
    time_t utime_buf[2];
    utime_buf[0]  = OFstatic_cast(time_t, referenceTime);
    utime_buf[1] = OFstatic_cast(time_t, referenceTime);
    if (0 != utime(OFconst_cast(char *, databaseIndexFile.c_str()), utime_buf))
#endif
    {
      DCMPSTAT_WARN("Cannot set database index file modification time");
    } else {
      struct stat stat_buf;
      if (0 == stat(databaseIndexFile.c_str(), &stat_buf))
      {
        referenceTime = OFstatic_cast(unsigned long, stat_buf.st_mtime);
      }
    }
}


OFBool DVInterface::newInstancesReceived()
{
  if (databaseIndexFile.length() == 0)
  {
    if (pHandle == NULL)
    {
      lockDatabase(); // derives databaseIndexFile
      releaseDatabase();
    }
  }

  if (databaseIndexFile.length() > 0)
  {
    struct stat stat_buf;
    if (0== stat(databaseIndexFile.c_str(), &stat_buf))
    {
      if (OFstatic_cast(unsigned long, stat_buf.st_mtime) == referenceTime) return OFFalse;
    }

    resetDatabaseReferenceTime();
  }
  return OFTrue; // default
}


void DVInterface::clearIndexCache()
{
    idxCache.clear();
    clearIndexRecord(idxRec, idxRecPos);
}


OFBool DVInterface::createIndexCache()
{
    if (lockDatabase() == EC_Normal)
    {
        if (idxCache.empty())
        {
            int counter = 0;
            pHandle->DB_IdxInitLoop(&counter);
            IdxRecord record;
            while (pHandle->DB_IdxGetNext(&counter, &record).good())
            {
                if (!idxCache.isElem(record.StudyInstanceUID))
                    idxCache.addItem(record.StudyInstanceUID);
                DVStudyCache::ItemStruct *study = idxCache.getItem();
                if (study != NULL)
                {
                    if (!study->List.isElem(record.SeriesInstanceUID))
                        study->List.addItem(record.SeriesInstanceUID);
                    DVSeriesCache::ItemStruct *series = study->List.getItem();
                    if (series != NULL)
                    {
                        if (!series->List.isElem(record.SOPInstanceUID))
                        {
                            DVPSInstanceType type = DVPSI_image;
                            if (record.Modality != NULL)
                            {
                                if (strcmp(record.Modality, "PR") == 0)
                                    type = DVPSI_presentationState;
                                if (strcmp(record.Modality, "SR") == 0)
                                    type = DVPSI_structuredReport;
                                else if (strcmp(record.Modality, "HC") == 0)
                                    type =DVPSI_hardcopyGrayscale;
                                else if (strcmp(record.Modality, "STORED_PRINT") == 0)
                                    type = DVPSI_storedPrint;
                            }
                            series->List.addItem(record.SOPInstanceUID, counter, record.hstat, type, record.ImageSize, record.filename);
                            if (series->Type == DVPSI_image)
                                series->Type = type;                // series contains only one type of instances
                        }
                    }
                }
            }
            updateStatusCache();
        }
        return OFTrue;
    }
    return OFFalse;
}


OFBool DVInterface::createPStateCache()
{
    DVStudyCache::ItemStruct *study = getStudyStruct();
    if (study != NULL)
    {
        DVSeriesCache::ItemStruct *series = study->List.getItem();
        if (series != NULL)
        {
            DVInstanceCache::ItemStruct *instance = series->List.getItem();
            if ((instance != NULL) && ((instance->Type == DVPSI_image) || (instance->Type == DVPSI_hardcopyGrayscale)))
            {
                if (!instance->Checked)                             // is current instance already checked?
                {
                    if (instance->List.empty())
                    {
                        OFString seriesUID = series->UID;
                        OFString instanceUID = instance->UID;
                        if (study->List.gotoFirst())
                        {
                            do { /* for all series */
                                if (study->List.getType() == DVPSI_presentationState)
                                {
                                    series = study->List.getItem();
                                    if (series != NULL)
                                    {
                                        if (series->List.gotoFirst())
                                        {
                                            do { /* for all instances */
                                                if (series->List.getType() == DVPSI_presentationState)
                                                {
                                                    DcmFileFormat *pstate = NULL;
                                                    if ((DVPSHelper::loadFileFormat(series->List.getFilename(), pstate) == EC_Normal) && pstate)
                                                    {
                                                        DcmDataset *dataset = pstate->getDataset();
                                                        DVPSReferencedSeries_PList plist;
                                                        if (dataset && (plist.read(*dataset) == EC_Normal) && plist.isValid())
                                                        {
                                                            if (plist.findImageReference(seriesUID.c_str(), instanceUID.c_str()))
                                                            {
                                                                DVInstanceCache::ItemStruct *reference = series->List.getItem();
                                                                if (reference != NULL)
                                                                {
                                                                    DcmStack stack;
                                                                    if (dataset->search(DCM_ContentDescription, stack, ESM_fromHere, OFFalse) == EC_Normal)
                                                                    {
                                                                        char *value = NULL;
                                                                        if ((*OFstatic_cast(DcmLongString *, stack.top())).getString(value) == EC_Normal)
                                                                            reference->Description = value;
                                                                    }
                                                                    stack.clear();
                                                                    if (dataset->search(DCM_ContentLabel, stack, ESM_fromHere, OFFalse) == EC_Normal)
                                                                    {
                                                                        char *value = NULL;
                                                                        if ((*OFstatic_cast(DcmLongString *, stack.top())).getString(value) == EC_Normal)
                                                                            reference->Label = value;
                                                                    }
                                                                    instance->List.push_back(reference);
                                                                }
                                                            }
                                                        }
                                                    }
                                                    delete pstate;
                                                }
                                            } while (series->List.gotoNext());
                                        }
                                        series->List.reset();                    // set iterator to old position
                                    }
                                }
                            } while (study->List.gotoNext());
                        }
                        study->List.reset();                                     // set iterator to old position
                    }
                    instance->Checked = OFTrue;                                  // do not check twice
                }
                return OFTrue;
            }
        }
    }
    return OFFalse;
}


void DVInterface::clearIndexRecord(IdxRecord &record,
                                   int &recpos)
{
    OFBitmanipTemplate<Uint8>::zeroMem(OFreinterpret_cast(Uint8 *, &record), sizeof(idxRec));
    recpos = -1;
}


OFBool DVInterface::readIndexRecord(const int pos,
                                    IdxRecord &record,
                                    int *oldpos)
{
    if (lockDatabase() == EC_Normal)
    {
        if ((oldpos != NULL) && (pos == *oldpos))                      // record already read
            return OFTrue;
        if (pHandle->DB_IdxRead(pos, &record).good())
        {
            if (oldpos != NULL)
                *oldpos = pos;
            return OFTrue;
        }
    }
    return OFFalse;
}


void DVInterface::updateStatusCache()
{
    idxCache.updateStatus();
}


Uint32 DVInterface::getNumberOfStudies()
{
    if (createIndexCache())
        return idxCache.getCount();
    return 0;
}


Uint32 DVInterface::getNumberOfSeries()
{
    DVStudyCache::ItemStruct *study = getStudyStruct();
    if (study != NULL)
        return study->List.getCount();
    return 0;
}


Uint32 DVInterface::getNumberOfInstances()
{
    DVSeriesCache::ItemStruct *series = getSeriesStruct();
    if (series != NULL)
        return series->List.getCount();
    return 0;
}


OFCondition DVInterface::selectStudy(Uint32 idx)
{
    if (createIndexCache())
    {
        if (idxCache.gotoItem(idx))
        {
            DVStudyCache::ItemStruct *study = idxCache.getItem();
            if (study->List.gotoItem(0))
            {
                DVSeriesCache::ItemStruct *series = study->List.getItem();
                if (series != NULL)
                {
                    if (series->List.gotoItem(0))
                    {
                        if (readIndexRecord(series->List.getPos(), idxRec, &idxRecPos))
                            return EC_Normal;
                    }
                }
            }
        }
    }
    return EC_IllegalCall;
}


OFCondition DVInterface::selectStudy(const char *studyUID)
{
    if (studyUID)
    {
        if (createIndexCache())
        {
            if (idxCache.isElem(studyUID))
            {
                DVStudyCache::ItemStruct *study = idxCache.getItem();
                if (study->List.gotoItem(0))
                {
                    DVSeriesCache::ItemStruct *series = study->List.getItem();
                    if (series != NULL)
                    {
                        if (series->List.gotoItem(0))
                        {
                            if (readIndexRecord(series->List.getPos(), idxRec, &idxRecPos))
                                return EC_Normal;
                        }
                    }
                }
            }
        }
    }
    return EC_IllegalCall;
}


OFCondition DVInterface::selectSeries(Uint32 idx)
{
    DVStudyCache::ItemStruct *study = getStudyStruct();
    if (study != NULL)
    {
        if (study->List.gotoItem(idx))
        {
            DVSeriesCache::ItemStruct *series = study->List.getItem();
            if (series != NULL)
            {
                if (series->List.gotoItem(0))
                {
                    if (readIndexRecord(series->List.getPos(), idxRec, &idxRecPos))
                        return EC_Normal;
                }
            }
        }
    }
    return EC_IllegalCall;
}


OFCondition DVInterface::selectSeries(const char *seriesUID)
{
    if (seriesUID)
    {
        DVStudyCache::ItemStruct *study = getStudyStruct();
        if (study != NULL)
        {
            if (study->List.isElem(seriesUID))
            {
                DVSeriesCache::ItemStruct *series = study->List.getItem();
                if (series != NULL)
                {
                    if (series->List.gotoItem(0))
                    {
                        if (readIndexRecord(series->List.getPos(), idxRec, &idxRecPos))
                            return EC_Normal;
                    }
                }
            }
        }
    }
    return EC_IllegalCall;
}


OFCondition DVInterface::selectInstance(Uint32 idx)
{
    DVSeriesCache::ItemStruct *series = getSeriesStruct();
    if (series != NULL)
    {
        if (series->List.gotoItem(idx))
        {
            if (readIndexRecord(series->List.getPos(), idxRec, &idxRecPos))
                return EC_Normal;
        }
    }
    return EC_IllegalCall;
}


OFCondition DVInterface::selectInstance(const char *instanceUID)
{
    if (instanceUID)
    {
        DVSeriesCache::ItemStruct *series = getSeriesStruct();
        if (series != NULL)
        {
            if (series->List.isElem(instanceUID))
            {
                if (readIndexRecord(series->List.getPos(), idxRec, &idxRecPos))
                    return EC_Normal;
            }
        }
    }
    return EC_IllegalCall;
}


OFCondition DVInterface::selectInstance(const char *instanceUID, const char *sopClassUID)
{
    if (instanceUID)
    {
        if (createIndexCache() && idxCache.gotoFirst())
        {
            DVStudyCache::ItemStruct *study = NULL;
            DVSeriesCache::ItemStruct *series = NULL;
            do {  /* for all studies */
                study = idxCache.getItem();
                if ((study != NULL) && study->List.gotoFirst())
                {
                    do {  /* for all series */
                        series = study->List.getItem();
                        if ((series != NULL) && series->List.isElem(instanceUID))
                        {
                            if (readIndexRecord(series->List.getPos(), idxRec, &idxRecPos))
                            {
                                if (sopClassUID == NULL)
                                    return EC_Normal;
                                else if ((idxRec.SOPClassUID != NULL) && (strcmp(sopClassUID, idxRec.SOPClassUID) == 0))
                                    return EC_Normal;
                            }
                        }
                    } while (study->List.gotoNext());
                }
            } while (idxCache.gotoNext());
        }
    }
    return EC_IllegalCall;
}


OFCondition DVInterface::selectInstance(const char *studyUID, const char *seriesUID, const char *instanceUID)
{
    if (studyUID && seriesUID && instanceUID)
    {
        if (createIndexCache())
        {
            if (idxCache.isElem(studyUID))
            {
                DVStudyCache::ItemStruct *study = idxCache.getItem();
                if (study->List.isElem(seriesUID))
                {
                    DVSeriesCache::ItemStruct *series = study->List.getItem();
                    if (series != NULL)
                    {
                        if (series->List.isElem(instanceUID))
                        {
                            if (readIndexRecord(series->List.getPos(), idxRec, &idxRecPos))
                                return EC_Normal;
                        }
                    }
                }
            }
        }
    }
    return EC_IllegalCall;
}


DVIFhierarchyStatus DVInterface::getStudyStatus()
{
    return idxCache.getStatus();
}


DVIFhierarchyStatus DVInterface::getSeriesStatus()
{
    DVStudyCache::ItemStruct *study = idxCache.getItem();
    if (study != NULL)
        return study->List.getStatus();
    return DVIF_objectIsNew;
}


DVIFhierarchyStatus DVInterface::getInstanceStatus()
{
    DVStudyCache::ItemStruct *study = idxCache.getItem();
    if (study != NULL)
    {
        DVSeriesCache::ItemStruct *series = study->List.getItem();
        if (series != NULL)
            return series->List.getStatus();
    }
    return DVIF_objectIsNew;
}


DVPSInstanceType DVInterface::getSeriesType()
{
    DVStudyCache::ItemStruct *study = idxCache.getItem();
    if (study != NULL)
        return study->List.getType();
    return DVPSI_image;
}


DVPSInstanceType DVInterface::getInstanceType()
{
    DVStudyCache::ItemStruct *study = idxCache.getItem();
    if (study != NULL)
    {
        DVSeriesCache::ItemStruct *series = study->List.getItem();
        if (series != NULL)
            return series->List.getType();
    }
    return DVPSI_image;
}


const char *DVInterface::getStudyUID()
{
    return idxRec.StudyInstanceUID;
}


const char *DVInterface::getSeriesUID()
{
    return idxRec.SeriesInstanceUID;
}


const char *DVInterface::getSOPClassUID()
{
    return idxRec.SOPClassUID;
}


const char *DVInterface::getInstanceUID()
{
    return idxRec.SOPInstanceUID;
}


const char *DVInterface::getStudyDescription()
{
    return idxRec.StudyDescription;
}


const char *DVInterface::getStudyDate()
{
    return idxRec.StudyDate;
}


const char *DVInterface::getStudyTime()
{
    return idxRec.StudyTime;
}


const char *DVInterface::getReferringPhysiciansName()
{
    return idxRec.ReferringPhysicianName;
}


const char *DVInterface::getAccessionNumber()
{
    return idxRec.AccessionNumber;
}


const char *DVInterface::getNameOfPhysiciansReadingStudy()
{
    return idxRec.NameOfPhysiciansReadingStudy;
}


const char *DVInterface::getPatientName()
{
    return idxRec.PatientName;
}


const char *DVInterface::getPatientID()
{
    return idxRec.PatientID;
}


const char *DVInterface::getPatientBirthDate()
{
    return idxRec.PatientBirthDate;
}


const char *DVInterface::getPatientSex()
{
    return idxRec.PatientSex;
}


const char *DVInterface::getPatientBirthTime()
{
    return idxRec.PatientBirthTime;
}


const char *DVInterface::getOtherPatientNames()
{
    return idxRec.OtherPatientNames;
}


const char *DVInterface::getOtherPatientID()
{
    return idxRec.OtherPatientIDs;
}


const char *DVInterface::getEthnicGroup()
{
    return idxRec.EthnicGroup;
}


const char *DVInterface::getSeriesDescription()
{
    return idxRec.SeriesDescription;
}


const char *DVInterface::getSeriesNumber()
{
    return idxRec.SeriesNumber;
}


const char *DVInterface::getSeriesDate()
{
    return idxRec.SeriesDate;
}


const char *DVInterface::getSeriesTime()
{
    return idxRec.SeriesTime;
}


const char *DVInterface::getSeriesPerformingPhysiciansName()
{
    return idxRec.PerformingPhysicianName;
}


const char *DVInterface::getSeriesProtocolName()
{
    return idxRec.ProtocolName;
}


const char *DVInterface::getSeriesOperatorsName()
{
    return idxRec.OperatorsName;
}


const char *DVInterface::getModality()
{
    return idxRec.Modality;
}


const char *DVInterface::getImageNumber()
{
    return idxRec.ImageNumber;
}


const char *DVInterface::getFilename()
{
    return idxRec.filename;
}


const char *DVInterface::getInstanceDescription()
{
    return idxRec.InstanceDescription;
}


const char *DVInterface::getPresentationLabel()
{
    return idxRec.PresentationLabel;
}


OFCondition DVInterface::instanceReviewed(int pos)
{
    lockDatabase();
    OFBool wasNew = newInstancesReceived();
    if (pHandle == NULL) return EC_IllegalCall;
    OFCondition result = pHandle->instanceReviewed(pos);
    if (!wasNew) resetDatabaseReferenceTime();
    releaseDatabase();
    return result;
}


OFCondition DVInterface::instanceReviewed(const char *studyUID,
                                          const char *seriesUID,
                                          const char *instanceUID)
{
    OFCondition result = EC_IllegalCall;
    DVInstanceCache::ItemStruct *instance = getInstanceStruct(studyUID, seriesUID, instanceUID);
    if (instance != NULL)
    {
        if (instance->Status == DVIF_objectIsNotNew)
            result = EC_Normal;
        else
            result = instanceReviewed(instance->Pos);
    }
    return result;
}


int DVInterface::findStudyIdx(StudyDescRecord *study,
                              const char *uid)
{
    if ((study != NULL) && (uid != NULL))
    {
        register int i = 0;
        for (i = 0; i < PSTAT_MAXSTUDYCOUNT; i++)
        {
            if ((study[i].StudyInstanceUID != NULL) &&
                (strcmp(uid, study[i].StudyInstanceUID) == 0))
            {
                return i;
            }
        }
    }
    return -1;
}


int DVInterface::deleteImageFile(const char *filename)
{
    if ((filename != NULL) && (pHandle != NULL))
    {
        const char *pos;
        if (((pos = strrchr(filename, OFstatic_cast(int, PATH_SEPARATOR))) == NULL) ||   // check whether image file resides in index.dat directory
            (strncmp(filename, pHandle->getStorageArea(), pos - filename) == 0))
        {
//            DB_deleteImageFile((/*const */char *)filename);
            if (unlink(filename) == 0)
                return 1;                                                 // image file has been deleted
        }
        return 2;                                                         // image file has not been deleted
    }
    return 0;                                                             // given filename is invalid
}


OFCondition DVInterface::deleteStudy(const char *studyUID)
{
    DVStudyCache::ItemStruct *study = getStudyStruct(studyUID);
    if (study != NULL)
    {
        OFCondition result = EC_IllegalCall;
        OFBool wasNew = OFTrue;
        if (lockExclusive() == EC_Normal)
        {
            wasNew = newInstancesReceived();
            if (study->List.gotoFirst())
            {
                StudyDescRecord *study_desc = OFstatic_cast(StudyDescRecord *, malloc(SIZEOF_STUDYDESC));
                if (study_desc != NULL)
                {
                    if (pHandle->DB_GetStudyDesc(study_desc).good())
                    {
                        int idx = findStudyIdx(study_desc, studyUID);
                        if (idx >= 0)
                        {
                            do /* for all series */
                            {
                                DVSeriesCache::ItemStruct *series = study->List.getItem();
                                if (series != NULL)
                                {
                                    if (series->List.gotoFirst())
                                    {
                                        do /* for all instances */
                                        {
                                            pHandle->DB_IdxRemove(series->List.getPos());
                                            deleteImageFile(series->List.getFilename());
                                        } while (series->List.gotoNext());
                                    }
                                }
                            } while (study->List.gotoNext());
                            study_desc[idx].NumberofRegistratedImages = 0;
                            study_desc[idx].StudySize = 0;
                            pHandle->DB_StudyDescChange(study_desc);
                        }
                    }
                    free(study_desc);
                }
            }
        }
        unlockExclusive();
        if (!wasNew)
            resetDatabaseReferenceTime();
        return result;
    }
    return EC_IllegalCall;
}


OFCondition DVInterface::deleteSeries(const char *studyUID,
                                      const char *seriesUID)
{
    DVSeriesCache::ItemStruct *series = getSeriesStruct(studyUID, seriesUID);
    if (series != NULL)
    {
        OFCondition result = EC_IllegalCall;
        OFBool wasNew = OFTrue;
        if (lockExclusive() == EC_Normal)
        {
            wasNew = newInstancesReceived();
            if (series->List.gotoFirst())
            {
                StudyDescRecord *study_desc = OFstatic_cast(StudyDescRecord *, malloc(SIZEOF_STUDYDESC));
                if (study_desc != NULL)
                {
                    if (pHandle->DB_GetStudyDesc(study_desc).good())
                    {
                        int idx = findStudyIdx(study_desc, studyUID);
                        if (idx >= 0)
                        {
                            do /* for all images */
                            {
                                pHandle->DB_IdxRemove(series->List.getPos());
                                if (study_desc[idx].NumberofRegistratedImages > 0)
                                {
                                    study_desc[idx].NumberofRegistratedImages--;
                                    study_desc[idx].StudySize -= series->List.getImageSize();
                                }
                                deleteImageFile(series->List.getFilename());
                            } while (series->List.gotoNext());
                            pHandle->DB_StudyDescChange(study_desc);
                        }
                    }
                    free(study_desc);
                }
            }
        }
        unlockExclusive();
        if (!wasNew)
            resetDatabaseReferenceTime();
        return result;
    }
    return EC_IllegalCall;
}



OFCondition DVInterface::deleteInstance(const char *studyUID,
                                        const char *seriesUID,
                                        const char *instanceUID)
{
    DVSeriesCache::ItemStruct *series = getSeriesStruct(studyUID, seriesUID, instanceUID);
    if (series != NULL)
    {
        OFCondition result = EC_IllegalCall;
        OFBool wasNew = OFTrue;
        if (lockExclusive() == EC_Normal)
        {
            wasNew = newInstancesReceived();
            pHandle->DB_IdxRemove(series->List.getPos());
            StudyDescRecord *study_desc = OFstatic_cast(StudyDescRecord *, malloc(SIZEOF_STUDYDESC));
            if (study_desc != NULL)
            {
                if (pHandle->DB_GetStudyDesc(study_desc).good())
                {
                    register int i = 0;
                    for (i = 0; i < PSTAT_MAXSTUDYCOUNT; i++)
                    {
                        if ((study_desc[i].StudyInstanceUID != NULL) &&
                            (strcmp(studyUID, study_desc[i].StudyInstanceUID) != 0))
                        {
                            if (study_desc[i].NumberofRegistratedImages > 0)
                            {
                                study_desc[i].NumberofRegistratedImages--;
                                study_desc[i].StudySize -= series->List.getImageSize();
                                pHandle->DB_StudyDescChange(study_desc);
                            }
                            break;
                        }
                    }
                    free(study_desc);
                    result = EC_Normal;
                }
                deleteImageFile(series->List.getFilename());
            }
        }
        unlockExclusive();
        if (!wasNew)
            resetDatabaseReferenceTime();
        return result;
    }
    return EC_IllegalCall;
}


OFBool DVInterface::isDisplayTransformPossible(DVPSDisplayTransform transform)
{
    if (transform == DVPSD_none)
        return OFFalse;
    return (displayFunction[transform] != NULL);
}


OFCondition DVInterface::setAmbientLightValue(double value)
{
    OFCondition result = EC_IllegalCall;
    for (int i = DVPSD_first; i < DVPSD_max; i++)
    {
        if ((displayFunction[i] != NULL) && (displayFunction[i]->setAmbientLightValue(value)))
            result = EC_Normal;         // at least one display function has been valid
    }
    return result;
}


OFCondition DVInterface::getAmbientLightValue(double &value)
{
    if (displayFunction[DVPSD_first] != NULL)
    {
        value = displayFunction[DVPSD_first]->getAmbientLightValue();
        return EC_Normal;
    }
    return EC_IllegalCall;
}


OFCondition DVInterface::sendIOD(const char * targetID,
                                 const char * studyUID,
                                 const char * seriesUID,
                                 const char * instanceUID)
{
  if ((targetID==NULL)||(studyUID==NULL)) return EC_IllegalCall;
  const char *sender_application = getSenderName();
  if (sender_application==NULL) return EC_IllegalCall;
  if (configPath.length()==0) return EC_IllegalCall;

  DVPSHelper::cleanChildren(); // clean up old child processes before creating new ones

#ifdef HAVE_FORK
  // Unix version - call fork() and execl()
  pid_t pid = fork();
  if (pid < 0)
  {
    // fork failed - return error code
    return EC_IllegalCall;
  } else if (pid > 0)
  {
    // we are the parent process
    return EC_Normal;
  } else {
    // we are the child process
    if (execl(sender_application, sender_application, configPath.c_str(),
            targetID, studyUID, seriesUID, instanceUID, NULL) < 0)
    {
      DCMPSTAT_ERROR("Unable to execute '" << sender_application << "'");
    }
    // if execl succeeds, this part will not get executed.
    // if execl fails, there is not much we can do except bailing out.
    abort();
  }
#else
  // Windows version - call CreateProcess()

  // initialize startup info
  PROCESS_INFORMATION procinfo;
  STARTUPINFO sinfo;
  OFBitmanipTemplate<char>::zeroMem((char *)&sinfo, sizeof(sinfo));
  sinfo.cb = sizeof(sinfo);
  char commandline[4096];
  if (seriesUID && instanceUID) sprintf(commandline, "%s %s %s %s %s %s", sender_application, configPath.c_str(),
      targetID, studyUID, seriesUID, instanceUID);
  else if (seriesUID) sprintf(commandline, "%s %s %s %s %s", sender_application, configPath.c_str(), targetID,
      studyUID, seriesUID);
  else sprintf(commandline, "%s %s %s %s", sender_application, configPath.c_str(), targetID, studyUID);
#ifdef DEBUG
  if (CreateProcess(NULL, commandline, NULL, NULL, 0, 0, NULL, NULL, &sinfo, &procinfo))
#else
  if (CreateProcess(NULL, commandline, NULL, NULL, 0, DETACHED_PROCESS, NULL, NULL, &sinfo, &procinfo))
#endif
  {
    return EC_Normal;
  } else {
    DCMPSTAT_ERROR("Unable to execute '" << sender_application << "'");
  }

#endif
  return EC_IllegalCall;
}


OFCondition DVInterface::startReceiver()
{
  const char *receiver_application = getReceiverName();
  if (receiver_application==NULL) return EC_IllegalCall;
  if (configPath.length()==0) return EC_IllegalCall;

  OFCondition result = EC_Normal;
  DCMPSTAT_LOGFILE("Starting network receiver processes ...");

  Uint32 numberOfReceivers = getNumberOfTargets(DVPSE_receiver);
  for (Uint32 i=0; i < numberOfReceivers; i++)
  {
    DVPSHelper::cleanChildren(); // clean up old child processes before creating new ones
#ifdef HAVE_FORK
    // Unix version - call fork() and execl()
    pid_t pid = fork();
    if (pid < 0)
    {
      // fork failed - set error code
      result = EC_IllegalCall;
    } else if (pid > 0)
    {
      // we are the parent process, continue loop
    } else {
      // we are the child process
      if (execl(receiver_application, receiver_application, configPath.c_str(), getTargetID(i, DVPSE_receiver), NULL) < 0)
      {
          DCMPSTAT_ERROR("Unable to execute '" << receiver_application << "'");
      }
      // if execl succeeds, this part will not get executed.
      // if execl fails, there is not much we can do except bailing out.
      abort();
    }
#else
    // Windows version - call CreateProcess()
    // initialize startup info
    PROCESS_INFORMATION procinfo;
    STARTUPINFO sinfo;
    OFBitmanipTemplate<char>::zeroMem((char *)&sinfo, sizeof(sinfo));
    sinfo.cb = sizeof(sinfo);
    char commandline[4096];
    sprintf(commandline, "%s %s %s", receiver_application, configPath.c_str(), getTargetID(i, DVPSE_receiver));
#ifdef DEBUG
    if (CreateProcess(NULL, commandline, NULL, NULL, 0, 0, NULL, NULL, &sinfo, &procinfo))
#else
    if (CreateProcess(NULL, commandline, NULL, NULL, 0, DETACHED_PROCESS, NULL, NULL, &sinfo, &procinfo))
#endif
    {
      // continue loop
    } else {
        DCMPSTAT_ERROR("Unable to execute '" << receiver_application << "'");
        result = EC_IllegalCall;
    }
#endif
  }
  return result;
}

OFCondition DVInterface::terminateReceiver()
{
  const char *receiver_application = getReceiverName();
  if (receiver_application==NULL) return EC_IllegalCall;
  if (configPath.length()==0) return EC_IllegalCall;

  OFCondition result = EC_Normal;
  DCMPSTAT_LOGFILE("Terminating network receiver processes ...");

  DVPSHelper::cleanChildren(); // clean up old child processes before creating new ones
#ifdef HAVE_FORK
  // Unix version - call fork() and execl()
  pid_t pid = fork();
  if (pid < 0)
  {
    // fork failed - set error code
    result = EC_IllegalCall;
  } else if (pid > 0)
  {
    // we are the parent process, continue loop
  } else {
    // we are the child process
    if (execl(receiver_application, receiver_application, configPath.c_str(), "--terminate", NULL) < 0)
    {
        DCMPSTAT_ERROR("Unable to execute '" << receiver_application << "'");
    }
    // if execl succeeds, this part will not get executed.
    // if execl fails, there is not much we can do except bailing out.
    abort();
  }
#else
  // Windows version - call CreateProcess()
  // initialize startup info
  PROCESS_INFORMATION procinfo;
  STARTUPINFO sinfo;
  OFBitmanipTemplate<char>::zeroMem((char *)&sinfo, sizeof(sinfo));
  sinfo.cb = sizeof(sinfo);
  char commandline[4096];
  sprintf(commandline, "%s %s %s", receiver_application, configPath.c_str(), "--terminate");
#ifdef DEBUG
  if (CreateProcess(NULL, commandline, NULL, NULL, 0, 0, NULL, NULL, &sinfo, &procinfo))
#else
  if (CreateProcess(NULL, commandline, NULL, NULL, 0, DETACHED_PROCESS, NULL, NULL, &sinfo, &procinfo))
#endif
  {
    // continue loop
  } else {
      DCMPSTAT_ERROR("Unable to execute '" << receiver_application << "'");
      result = EC_IllegalCall;
  }
#endif
  return result;
}


OFCondition DVInterface::startQueryRetrieveServer()
{
  const char *server_application = getQueryRetrieveServerName();
  if (server_application==NULL) return EC_IllegalCall;
  if (configPath.length()==0) return EC_IllegalCall;

  OFString config_filename = getQueryRetrieveServerName();
  config_filename += ".cfg";
  if (getQueryRetrieveAutoCreateConfigFile())
    createQueryRetrieveServerConfigFile(config_filename.c_str());

  DCMPSTAT_LOGFILE("Starting query/retrieve server process ...");

  DVPSHelper::cleanChildren(); // clean up old child processes before creating new ones

  Sint32 timeout = getQueryRetrieveTimeout();

#ifdef HAVE_FORK
  // Unix version - call fork() and execl()
  pid_t pid = fork();
  if (pid < 0)
  {
    // fork failed - return error code
    return EC_IllegalCall;
  } else if (pid > 0)
  {
    // we are the parent process
    return EC_Normal;
  } else {
    // we are the child process
    if (timeout > 0)
    {
      char str_timeout[20];
      sprintf(str_timeout, "%lu", OFstatic_cast(unsigned long, timeout));
      execl(server_application, server_application, "-c", config_filename.c_str(), "--allow-shutdown",
        "--timeout", str_timeout, NULL);
    }
    else
    {
      execl(server_application, server_application, "-c", config_filename.c_str(), "--allow-shutdown", NULL);
    }

    DCMPSTAT_ERROR("Unable to execute '" << server_application << "'");

    // if execl succeeds, this part will not get executed.
    // if execl fails, there is not much we can do except bailing out.
    abort();
  }
#else
  // Windows version - call CreateProcess()
  // initialize startup info
  PROCESS_INFORMATION procinfo;
  STARTUPINFO sinfo;
  OFBitmanipTemplate<char>::zeroMem((char *)&sinfo, sizeof(sinfo));
  sinfo.cb = sizeof(sinfo);
  char commandline[4096];

  if (timeout > 0)
  {
    sprintf(commandline, "%s -c %s --allow-shutdown --timeout %lu",
      server_application, config_filename.c_str(), (unsigned long) timeout);
  }
  else
  {
    sprintf(commandline, "%s -c %s --allow-shutdown", server_application, config_filename.c_str());
  }

#ifdef DEBUG
  if (CreateProcess(NULL, commandline, NULL, NULL, 0, 0, NULL, NULL, &sinfo, &procinfo))
#else
  if (CreateProcess(NULL, commandline, NULL, NULL, 0, DETACHED_PROCESS, NULL, NULL, &sinfo, &procinfo))
#endif
  {
    return EC_Normal;
  } else {
      DCMPSTAT_ERROR("Unable to execute '" << server_application << "'");
  }
#endif
  return EC_IllegalCall;
}

OFCondition DVInterface::terminateQueryRetrieveServer()
{
  if (getQueryRetrieveServerName()==NULL) return EC_IllegalCall;
  if (configPath.length()==0) return EC_IllegalCall;

#ifdef HAVE_GUSI_H
  GUSISetup(GUSIwithSIOUXSockets);
  GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef HAVE_WINSOCK_H
  WSAData winSockData;
  /* we need at least version 1.1 */
  WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
  WSAStartup(winSockVersionNeeded, &winSockData);
#endif

  OFCondition result = EC_Normal;
  T_ASC_Network *net=NULL;
  T_ASC_Parameters *params=NULL;
  DIC_NODENAME localHost;
  DIC_NODENAME peerHost;
  T_ASC_Association *assoc=NULL;

  DCMPSTAT_LOGFILE("Terminating query/retrieve server process ...");

  OFCondition cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 30, &net);
  if (cond.good())
  {
    cond = ASC_createAssociationParameters(&params, DEFAULT_MAXPDU);
    if (cond.good())
    {
      ASC_setAPTitles(params, getNetworkAETitle(), getQueryRetrieveAETitle(), NULL);
      gethostname(localHost, sizeof(localHost) - 1);
      sprintf(peerHost, "localhost:%d", OFstatic_cast(int, getQueryRetrievePort()));
      ASC_setPresentationAddresses(params, localHost, peerHost);

      const char* transferSyntaxes[] = { UID_LittleEndianImplicitTransferSyntax };
      cond = ASC_addPresentationContext(params, 1, UID_PrivateShutdownSOPClass, transferSyntaxes, 1);
      if (cond.good())
      {
        cond = ASC_requestAssociation(net, params, &assoc);
        if (cond.good()) ASC_abortAssociation(assoc); // tear down association if necessary
        ASC_dropAssociation(assoc);
        ASC_destroyAssociation(&assoc);
      }
    } else result = EC_IllegalCall;
    ASC_dropNetwork(&net);
  } else result = EC_IllegalCall;

#ifdef HAVE_WINSOCK_H
  WSACleanup();
#endif

  return result;
}

OFCondition DVInterface::createQueryRetrieveServerConfigFile(const char *filename)
{
  STD_NAMESPACE ofstream output(filename);
  if (output)
  {
    DCMPSTAT_LOGFILE("Creating configuration file for query/retrieve server");
    output << "# ATTENTION: This file has been created automatically and will" << OFendl;
    output << "#            be re-created each time the query/retrieve server" << OFendl;
    output << "#            is started.  To avoid that manual changes to this" << OFendl;
    output << "#            file are destroyed, the flag AutoCreateConfigFile" << OFendl;
    output << "#            in the configuration file '" << configPath << "' has to be" << OFendl;
    output << "#            switched off." << OFendl;
    output << OFendl;
    output << "NetworkType     = \"tcp\"" << OFendl;
    output << "NetworkTCPPort  = " << getQueryRetrievePort() << OFendl;
    output << "MaxPDUSize      = " << getQueryRetrieveMaxPDU() << OFendl;
    output << "MaxAssociations = " << getQueryRetrieveMaxAssociations() << OFendl;
    output << "Display         = \"no\"" << OFendl;
    output << OFendl;
    output << "HostTable BEGIN" << OFendl;
    const char *aet = NULL;
    const char *name = NULL;
    const Uint32 count = getNumberOfTargets();
    for (Uint32 i = 0; i < count; i++)
    {
      const char *id = getTargetID(i);
      if (id != NULL)
      {
          aet = getTargetAETitle(id);
          name = getTargetHostname(id);
          if ((aet != NULL) && (name != NULL))
            output << id << " = (" << aet << ", " << name << ", " << getTargetPort(id) << ")" << OFendl;
      }
    }
    output << "HostTable END" << OFendl;
    output << OFendl;
    output << "AETable BEGIN" << OFendl;
    output << getQueryRetrieveAETitle() << "\t" << getDatabaseFolder() << "\tR\t(";
    output << PSTAT_MAXSTUDYCOUNT << ", " << PSTAT_STUDYSIZE / 1024 / 1024 << "mb)\tANY" << OFendl;
    output << "AETable END" << OFendl;
    return EC_Normal;
  }
  DCMPSTAT_LOGFILE("Could not create configuration file for query/retrieve server");
  return EC_IllegalCall;
}

OFCondition DVInterface::saveDICOMImage(
  const char *filename,
  const void *pixelData,
  unsigned long width,
  unsigned long height,
  double aspectRatio,
  OFBool explicitVR,
  const char *instanceUID)
{
    if ((width<1)||(width > 0xFFFF)) return EC_IllegalCall;
    if ((height<1)||(height > 0xFFFF)) return EC_IllegalCall;
    if (pixelData == NULL) return EC_IllegalCall;
    if (filename == NULL) return EC_IllegalCall;
    if (aspectRatio == 0.0) return EC_IllegalCall;

    Uint16 columns = OFstatic_cast(Uint16, width);
    Uint16 rows = OFstatic_cast(Uint16, height);
    OFCondition status = EC_Normal;
    DcmFileFormat *fileformat = new DcmFileFormat();
    DcmDataset *dataset = NULL;
    if (fileformat) dataset=fileformat->getDataset();
    char newuid[70];

    if (dataset)
    {
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_PatientName);
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_PatientID);
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_PatientBirthDate);
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_PatientSex);
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_StudyDate);
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_StudyTime);
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_ReferringPhysicianName);
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_StudyID);
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_AccessionNumber);
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_SeriesNumber);
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_InstanceNumber);
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_PatientOrientation);
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_SOPClassUID, UID_SecondaryCaptureImageStorage);
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_Modality, "OT");
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_ConversionType, "WSD");
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_PhotometricInterpretation, "MONOCHROME2");
      dcmGenerateUniqueIdentifier(newuid);
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_SOPInstanceUID, (instanceUID ? instanceUID : newuid));
      dcmGenerateUniqueIdentifier(newuid, SITE_SERIES_UID_ROOT);
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_SeriesInstanceUID, newuid);
      dcmGenerateUniqueIdentifier(newuid, SITE_STUDY_UID_ROOT);
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_StudyInstanceUID, newuid);
      if (EC_Normal==status) status = DVPSHelper::putUint16Value(dataset, DCM_SamplesPerPixel, 1);
      if (EC_Normal==status) status = DVPSHelper::putUint16Value(dataset, DCM_Rows, rows);
      if (EC_Normal==status) status = DVPSHelper::putUint16Value(dataset, DCM_Columns, columns);
      if (EC_Normal==status) status = DVPSHelper::putUint16Value(dataset, DCM_BitsAllocated, 8);
      if (EC_Normal==status) status = DVPSHelper::putUint16Value(dataset, DCM_BitsStored, 8);
      if (EC_Normal==status) status = DVPSHelper::putUint16Value(dataset, DCM_HighBit, 7);
      if (EC_Normal==status) status = DVPSHelper::putUint16Value(dataset, DCM_PixelRepresentation, 0);
      if ((EC_Normal==status)&&(aspectRatio != 1.0))
      {
        sprintf(newuid, "%ld\\%ld", 1000L, OFstatic_cast(long, aspectRatio*1000.0));
        status = DVPSHelper::putStringValue(dataset, DCM_PixelAspectRatio, newuid);
      }
      DcmPolymorphOBOW *pxData = new DcmPolymorphOBOW(DCM_PixelData);
      if (pxData)
      {
        status = pxData->putUint8Array(OFstatic_cast(Uint8 *, OFconst_cast(void *, pixelData)), OFstatic_cast(unsigned long, width*height));
        if (EC_Normal==status) status = dataset->insert(pxData, OFTrue /*replaceOld*/); else delete pxData;
      } else status = EC_MemoryExhausted;

      if (status != EC_Normal)
        DCMPSTAT_LOGFILE("Save image to file failed: invalid data structures");

      if (EC_Normal == status)
      {
        status = DVPSHelper::saveFileFormat(filename, fileformat, explicitVR);
        if (status != EC_Normal)
          DCMPSTAT_LOGFILE("Save image to file failed: could not write fileformat");
      }
    } else {
      status = EC_MemoryExhausted;
      DCMPSTAT_LOGFILE("Save image to file failed: memory exhausted");
    }

    delete fileformat;
    return status;
}


OFCondition DVInterface::saveDICOMImage(
  const void *pixelData,
  unsigned long width,
  unsigned long height,
  double aspectRatio)
{
  // release database lock since we are using the DB module directly
  releaseDatabase();

  char uid[100];
  dcmGenerateUniqueIdentifier(uid);

  DcmQueryRetrieveDatabaseStatus dbStatus(STATUS_Success);
  char imageFileName[MAXPATHLEN+1];

  OFCondition result = EC_Normal;
  DcmQueryRetrieveIndexDatabaseHandle handle(getDatabaseFolder(), PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, result);
  if (result.bad())
  {
    DCMPSTAT_LOGFILE("Save image to database failed: could not lock index file");
    return result;
  }

  if (handle.makeNewStoreFileName(UID_SecondaryCaptureImageStorage, uid, imageFileName).good())
  {
     // now store presentation state as filename
     result = saveDICOMImage(imageFileName, pixelData, width, height, aspectRatio, OFTrue, uid);
     if (EC_Normal==result)
     {
       if (handle.storeRequest(UID_SecondaryCaptureImageStorage, uid, imageFileName, &dbStatus).bad())
       {
         result = EC_IllegalCall;
         DCMPSTAT_LOGFILE("Save image to database failed: could not register in index file");
         DCMPSTAT_DEBUG("Unable to register secondary capture image '" << imageFileName << "' in database");
       }
     }
  }
  return result;
}


OFCondition DVInterface::saveHardcopyGrayscaleImage(
  const char *filename,
  const void *pixelData,
  unsigned long width,
  unsigned long height,
  double aspectRatio,
  OFBool explicitVR,
  const char *instanceUID)
{
    if (pState == NULL) return EC_IllegalCall;
    if (pPrint == NULL) return EC_IllegalCall;

    if ((width<1)||(width > 0xFFFF)) return EC_IllegalCall;
    if ((height<1)||(height > 0xFFFF)) return EC_IllegalCall;
    if (pixelData == NULL) return EC_IllegalCall;
    if (filename == NULL) return EC_IllegalCall;
    if (aspectRatio == 0.0) return EC_IllegalCall;

    Uint16 columns = OFstatic_cast(Uint16, width);
    Uint16 rows = OFstatic_cast(Uint16, height);
    OFCondition status = EC_Normal;
    DcmFileFormat *fileformat = new DcmFileFormat();
    DcmDataset *dataset = NULL;
    if (fileformat) dataset=fileformat->getDataset();
    char newuid[70];
    OFString aString;
    OFString theInstanceUID;

    if (dataset)
    {
      // write patient module
      if (EC_Normal==status) status = pState->writeHardcopyImageAttributes(*dataset);
      // write general study and general series module
      if (EC_Normal==status) status = pPrint->writeHardcopyImageAttributes(*dataset);

      // Hardcopy Equipment Module
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_RETIRED_HardcopyDeviceManufacturer, "OFFIS");
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_RETIRED_HardcopyDeviceSoftwareVersion, OFFIS_DTK_IMPLEMENTATION_VERSION_NAME);

      // General Image Module
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_InstanceNumber);
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_PatientOrientation);
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_ImageType, "DERIVED\\SECONDARY");
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_DerivationDescription, "Hardcopy rendered using Presentation State");
      // source image sequence is written in pState->writeHardcopyImageAttributes().

      // SOP Common Module
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_SOPClassUID, UID_RETIRED_HardcopyGrayscaleImageStorage);
      dcmGenerateUniqueIdentifier(newuid);
      theInstanceUID = (instanceUID ? instanceUID : newuid);
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_SOPInstanceUID, theInstanceUID.c_str());
      DVPSHelper::currentDate(aString);
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_InstanceCreationDate, aString.c_str());
      DVPSHelper::currentTime(aString);
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_InstanceCreationTime, aString.c_str());

      // Hardcopy Grayscale Image Module
      if (EC_Normal==status) status = DVPSHelper::putStringValue(dataset, DCM_PhotometricInterpretation, "MONOCHROME2");
      if (EC_Normal==status) status = DVPSHelper::putUint16Value(dataset, DCM_SamplesPerPixel, 1);
      if (EC_Normal==status) status = DVPSHelper::putUint16Value(dataset, DCM_Rows, rows);
      if (EC_Normal==status) status = DVPSHelper::putUint16Value(dataset, DCM_Columns, columns);
      if (EC_Normal==status) status = DVPSHelper::putUint16Value(dataset, DCM_BitsAllocated, 16);
      if (EC_Normal==status) status = DVPSHelper::putUint16Value(dataset, DCM_BitsStored, 12);
      if (EC_Normal==status) status = DVPSHelper::putUint16Value(dataset, DCM_HighBit, 11);
      if (EC_Normal==status) status = DVPSHelper::putUint16Value(dataset, DCM_PixelRepresentation, 0);
      if ((EC_Normal==status)&&(aspectRatio != 1.0))
      {
        sprintf(newuid, "%ld\\%ld", 1000L, OFstatic_cast(long, aspectRatio*1000.0));
        status = DVPSHelper::putStringValue(dataset, DCM_PixelAspectRatio, newuid);
      }

      DcmPolymorphOBOW *pxData = new DcmPolymorphOBOW(DCM_PixelData);
      if (pxData)
      {
        status = pxData->putUint16Array(OFstatic_cast(Uint16 *, OFconst_cast(void *, pixelData)), OFstatic_cast(unsigned long, width*height));
        if (EC_Normal==status) status = dataset->insert(pxData, OFTrue /*replaceOld*/); else delete pxData;
      } else status = EC_MemoryExhausted;

      // add Presentation LUT to hardcopy file if present, making it a Standard Extended SOP Class
      if ((EC_Normal==status)&&(pState->getPresentationLUT() == DVPSP_table))
      {
        status = pState->writePresentationLUTforPrint(*dataset);
      }

      if (status != EC_Normal)
        DCMPSTAT_LOGFILE("Save hardcopy grayscale image to file failed: invalid data structures");

      // save image file
      if (EC_Normal == status)
      {
        status = DVPSHelper::saveFileFormat(filename, fileformat, explicitVR);
        if (status != EC_Normal)
          DCMPSTAT_LOGFILE("Save hardcopy grayscale image to file failed: could not write fileformat");
      }
    } else {
      status = EC_MemoryExhausted;
      DCMPSTAT_LOGFILE("Save hardcopy grayscale image to file failed: memory exhausted");
    }

    if (EC_Normal == status)
    {
      OFString reqImageTmp;
      const char *reqImageSize = NULL;
      DVPSPresentationLUT *presLUT = pState->getPresentationLUTData();

      if (EC_Normal == pState->getPrintBitmapRequestedImageSize(reqImageTmp)) reqImageSize = reqImageTmp.c_str();
      /* we don't pass the patient ID (available as pState->getPatientID()) here because then
       * we could end up with multiple images being part of one study and one series, but having
       * different patient IDs. This might confuse archives using the patient root query model.
       */
      status = pPrint->addImageBox(getNetworkAETitle(), theInstanceUID.c_str(), reqImageSize, NULL, presLUT, pState->isMonochrome1Image());
    }

    delete fileformat;
    return status;
}


OFCondition DVInterface::saveHardcopyGrayscaleImage(
  const void *pixelData,
  unsigned long width,
  unsigned long height,
  double aspectRatio)
{
  // release database lock since we are using the DB module directly
  releaseDatabase();

  char uid[100];
  dcmGenerateUniqueIdentifier(uid);

  DcmQueryRetrieveDatabaseStatus dbStatus(STATUS_Success);
  char imageFileName[MAXPATHLEN+1];

  OFCondition result=EC_Normal;
  DcmQueryRetrieveIndexDatabaseHandle handle(getDatabaseFolder(), PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, result);
  if (result.bad())
  {
    DCMPSTAT_LOGFILE("Save hardcopy grayscale image to database failed: could not lock index file");
    return result;
  }

  if (handle.makeNewStoreFileName(UID_RETIRED_HardcopyGrayscaleImageStorage, uid, imageFileName).good())
  {
     result = saveHardcopyGrayscaleImage(imageFileName, pixelData, width, height, aspectRatio, OFTrue, uid);
     if (EC_Normal==result)
     {
       if (handle.storeRequest(UID_RETIRED_HardcopyGrayscaleImageStorage, uid, imageFileName, &dbStatus).bad())
       {
         result = EC_IllegalCall;
         DCMPSTAT_LOGFILE("Save hardcopy grayscale image to database failed: could not register in index file");
         DCMPSTAT_DEBUG("Unable to register hardcopy grayscale image '" << imageFileName << "' in database");
       }
     }
  }
  return result;
}


OFCondition DVInterface::saveFileFormatToDB(DcmFileFormat &fileformat)
{
  // release database lock since we are using the DB module directly
  releaseDatabase();

  // get SOP class and instance UID from dataset
  char *classUID = NULL;
  char *instanceUID = NULL;
  DcmStack stack;
  DcmDataset *dset = fileformat.getDataset();
  if (dset)
  {
    if (EC_Normal == dset->search(DCM_SOPInstanceUID, stack, ESM_fromHere, OFFalse))
    {
      OFstatic_cast(DcmElement *, stack.top())->getString(instanceUID);
    }
    stack.clear();
    if (EC_Normal == dset->search(DCM_SOPClassUID, stack, ESM_fromHere, OFFalse))
    {
      OFstatic_cast(DcmElement *, stack.top())->getString(classUID);
    }
  }
  if ((instanceUID==NULL)||(classUID==NULL)) return EC_IllegalCall;

  DcmQueryRetrieveDatabaseStatus dbStatus(STATUS_Success);
  char imageFileName[MAXPATHLEN+1];

  OFCondition result=EC_Normal;
  DcmQueryRetrieveIndexDatabaseHandle handle(getDatabaseFolder(), PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, result);
  if (result.bad())
  {
    DCMPSTAT_LOGFILE("Save fileformat to database failed: could not lock index file");
    return result;
  }

  if (handle.makeNewStoreFileName(classUID, instanceUID, imageFileName).good())
  {
     // save image file
     result = DVPSHelper::saveFileFormat(imageFileName, &fileformat, OFTrue);
     if (EC_Normal==result)
     {
       if (handle.storeRequest(classUID, instanceUID, imageFileName, &dbStatus).bad())
       {
         result = EC_IllegalCall;
         DCMPSTAT_LOGFILE("Save fileformat to database failed: could not register in index file");
         DCMPSTAT_DEBUG("Unable to register file '" << imageFileName << "' in database");
       }
     }
  }
  return result;
}


OFCondition DVInterface::loadStoredPrint(const char *studyUID, const char *seriesUID, const char *instanceUID, OFBool changeStatus)
{
    OFCondition status = EC_IllegalCall;
    if (studyUID && seriesUID && instanceUID)
    {
        if (lockDatabase() == EC_Normal)
        {
            const char *filename = getFilename(studyUID, seriesUID, instanceUID);
            if (filename)
            {
                if ((status = loadStoredPrint(filename)) == EC_Normal)
                {
                    if (changeStatus)
                        instanceReviewed(studyUID, seriesUID, instanceUID);
                }
            } else
                DCMPSTAT_LOGFILE("Load stored print from database failed: UIDs not in index file");
        } else
            DCMPSTAT_LOGFILE("Load stored print from database failed: could not lock index file");
    } else
        DCMPSTAT_LOGFILE("Load stored print from database failed: invalid UIDs");
    return status;
}


OFCondition DVInterface::loadStoredPrint(const char *filename)
{
    OFCondition status = EC_IllegalCall;
    DcmFileFormat *fileformat = NULL;
    DVPSStoredPrint *print = new DVPSStoredPrint(getDefaultPrintIllumination(), getDefaultPrintReflection());
    if (print==NULL)
    {
        DCMPSTAT_LOGFILE("Load stored print from file failed: memory exhausted");
        return EC_MemoryExhausted;
    }

    if ((status = DVPSHelper::loadFileFormat(filename, fileformat)) == EC_Normal)
    {
        if (fileformat)
        {
            DcmDataset *dataset = fileformat->getDataset();
            if (dataset)
            {
                if (EC_Normal == (status = print->read(*dataset)))
                {
                    delete pPrint;
                    pPrint = print;
                    clearFilmSessionSettings();
                }
            } else status = EC_CorruptedData;
            delete fileformat;
        } else status = EC_IllegalCall;
        if (status != EC_Normal)
            DCMPSTAT_LOGFILE("Load stored print from file failed: invalid data structures");
    } else
        DCMPSTAT_LOGFILE("Load stored print from file failed: could not read fileformat");
    if (status != EC_Normal)
    {
        delete print;
    }
    return status;
}


OFCondition DVInterface::saveStoredPrint(
  const char *filename,
  OFBool writeRequestedImageSize,
  OFBool explicitVR,
  const char *instanceUID)
{
    if (pState == NULL) return EC_IllegalCall;
    if (pPrint == NULL) return EC_IllegalCall;
    if (filename == NULL) return EC_IllegalCall;

    OFCondition status = EC_Normal;
    DcmFileFormat *fileformat = new DcmFileFormat();
    DcmDataset *dataset = NULL;
    if (fileformat)
        dataset = fileformat->getDataset();

    char newuid[70];
    char buf[32];

    /* set annotation if active */
    if (activateAnnotation)
    {
      OFString text;
      OFString dummy;
      if (prependDateTime)
      {
        OFDateTime::getCurrentDateTime().getISOFormattedDateTime(text, OFFalse /*showSeconds*/);
      }
      if (prependPrinterName)
      {
        text += currentPrinter;
        text += " ";
      }
      if (prependLighting)
      {
        sprintf(buf, "%d/%d ", pPrint->getPrintIllumination(), pPrint->getPrintReflectedAmbientLight());
        text += buf;
      }
      text += annotationText;
      if (text.size() >64) text.erase(64); // limit to max annotation length

      if (getTargetPrinterSupportsAnnotationBoxSOPClass(currentPrinter.c_str()))
      {
        const char *displayformat = getTargetPrinterAnnotationDisplayFormatID(currentPrinter.c_str(), dummy);
        Uint16 position = getTargetPrinterAnnotationPosition(currentPrinter.c_str());
        pPrint->setSingleAnnotation(displayformat, text.c_str(), position);
      } else pPrint->deleteAnnotations();
      if (getTargetPrinterSessionLabelAnnotation(currentPrinter.c_str()))
      {
        status = setPrinterFilmSessionLabel(text.c_str());
      }
    } else {
      pPrint->deleteAnnotations();
    }

    if (dataset)
    {
      if (instanceUID) status = pPrint->setInstanceUID(instanceUID); else
      {
        dcmGenerateUniqueIdentifier(newuid);
        status = pPrint->setInstanceUID(newuid);
      }
      if (EC_Normal == status) status = pPrint->write(*dataset, writeRequestedImageSize, OFTrue, OFTrue, OFFalse);

      // save file
      if (EC_Normal == status) status = DVPSHelper::saveFileFormat(filename, fileformat, explicitVR);

      if (status != EC_Normal)
        DCMPSTAT_LOGFILE("Save stored print to file failed: could not write fileformat");
    } else {
      DCMPSTAT_LOGFILE("Save stored print to file failed: memory exhausted");
      status = EC_MemoryExhausted;
    }

    delete fileformat;
    return status;
}

OFCondition DVInterface::saveStoredPrint(OFBool writeRequestedImageSize)
{
  // release database lock since we are using the DB module directly
  releaseDatabase();

  char uid[100];
  dcmGenerateUniqueIdentifier(uid);

  DcmQueryRetrieveDatabaseStatus dbStatus(STATUS_Success);
  char imageFileName[MAXPATHLEN+1];
  OFCondition result=EC_Normal;
  DcmQueryRetrieveIndexDatabaseHandle handle(getDatabaseFolder(), PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, result);
  if (result.bad())
  {
    DCMPSTAT_LOGFILE("Save stored print to database failed: could not lock index file");
    return result;
  }

  if (handle.makeNewStoreFileName(UID_RETIRED_StoredPrintStorage, uid, imageFileName).good())
  {
     // now store stored print object as filename
     result = saveStoredPrint(imageFileName, writeRequestedImageSize, OFTrue, uid);
     if (EC_Normal==result)
     {
       if (handle.storeRequest(UID_RETIRED_StoredPrintStorage, uid, imageFileName, &dbStatus).bad())
       {
         result = EC_IllegalCall;
         DCMPSTAT_LOGFILE("Save stored print to database failed: could not register in index file");
         DCMPSTAT_DEBUG("Unable to register stored print object '" << imageFileName << "' in database");
       }
     }
  }
  return result;
}

size_t DVInterface::getNumberOfPrintPreviews()
{
  if (pPrint != NULL)
    return pPrint->getNumberOfImages();
  return 0;
}

OFCondition DVInterface::loadPrintPreview(size_t idx, OFBool printLUT, OFBool changeStatus)
{
  OFCondition status = EC_IllegalCall;
  if ((pPrint != NULL) && (maximumPrintPreviewWidth > 0) && (maximumPrintPreviewHeight > 0))
  {
    const char *studyUID;
    const char *seriesUID;
    const char *instanceUID;
    if ((status = pPrint->getImageReference(idx, studyUID, seriesUID, instanceUID)) == EC_Normal)
    {
      status = EC_IllegalCall;
      const char *filename = getFilename(studyUID, seriesUID, instanceUID);
      if (filename)
      {
        DicomImage *image = new DicomImage(filename);
        if (image != NULL)
        {
          if (image->getStatus() == EIS_Normal)
          {
            unsigned long width = maximumPrintPreviewWidth;
            unsigned long height = maximumPrintPreviewHeight;
            /* consider aspect ratio of the image and the display */
            double ratio = image->getWidthHeightRatio();
            const double mpWidth = getMonitorPixelWidth();
            const double mpHeight = getMonitorPixelHeight();
            if ((mpWidth > 0) && (mpHeight > 0))
              ratio *= (mpWidth / mpHeight);
            if (ratio == 0.0)
              ratio = 1.0;
            if (OFstatic_cast(double, image->getWidth()) / OFstatic_cast(double, width * ratio) <
                OFstatic_cast(double, image->getHeight()) / OFstatic_cast(double, height))
            {
              width = 0;
            } else
              height = 0;
            image->setWidthHeightRatio(ratio);
            pHardcopyImage = image->createScaledImage(width, height, 0 /*interpolate*/, 1 /*aspect ratio*/);
            if (pHardcopyImage != NULL)
            {
              if (pHardcopyImage->getStatus() == EIS_Normal)
              {
                /* set display function for calibrated output */
                if (displayFunction != NULL)
                  pHardcopyImage->setDisplayFunction(displayFunction[DVPSD_GSDF]);
                /* adapt polarity if necessary */
                const char *polarity = pPrint->getImagePolarity(idx);
                if ((polarity != NULL) && (strcmp(polarity, "REVERSE") == 0))
                  pHardcopyImage->setPolarity(EPP_Reverse);
                /* set (print/display) presentation LUT */
                DVPSPresentationLUT *plut = pPrint->getPresentationLUT();   // first check whether there's a global one
                if (plut == NULL)
                  plut = pPrint->getImagePresentationLUT(idx);              // ... then check for an image box specific
                if (plut != NULL)
                {
                  pHardcopyImage->setHardcopyParameters(pPrint->getMinDensityValue(), pPrint->getMaxDensityValue(),
                      pPrint->getPrintReflectedAmbientLight(), pPrint->getPrintIllumination());
                  plut->activate(pHardcopyImage, printLUT);
                }
                status = EC_Normal;
                if (changeStatus)
                    instanceReviewed(studyUID, seriesUID, instanceUID);
              } else
                unloadPrintPreview();
            } else
              DCMPSTAT_LOGFILE("Load hardcopy grayscale image for print preview failed: memory exhausted");
          } else
            DCMPSTAT_LOGFILE("Load hardcopy grayscale image for print preview failed: could not read image");
          delete image;
        } else
          DCMPSTAT_LOGFILE("Load hardcopy grayscale image for print preview failed: memory exhausted");
      } else
        DCMPSTAT_LOGFILE("Load hardcopy grayscale image for print preview failed: UIDs not in index file");
    }
  }
  return status;
}

void DVInterface::unloadPrintPreview()
{
  delete pHardcopyImage;
  pHardcopyImage = NULL;
}

unsigned long DVInterface::getPrintPreviewSize()
{
  unsigned long result = 0;
  unsigned long width;
  unsigned long height;
  if (getPrintPreviewWidthHeight(width, height) == EC_Normal)
    result = width * height;
  return result;
}

void DVInterface::setMaxPrintPreviewWidthHeight(unsigned long width, unsigned long height)
{
  if ((width != maximumPrintPreviewWidth) || (height != maximumPrintPreviewHeight))
  {
    unloadPrintPreview();
    maximumPrintPreviewWidth = width;
    maximumPrintPreviewHeight = height;
  }
}

OFCondition DVInterface::getPrintPreviewWidthHeight(unsigned long &width, unsigned long &height)
{
  OFCondition result = EC_IllegalCall;
  if (pHardcopyImage != NULL)
  {
    width = pHardcopyImage->getWidth();
    height = pHardcopyImage->getHeight();
    if ((width > 0) && (height > 0))
      result = EC_Normal;
  } else {
    width = 0;
    height = 0;
  }
  return result;
}

OFCondition DVInterface::getPrintPreviewBitmap(void *bitmap, unsigned long size)
{
  OFCondition status = EC_IllegalCall;
  if ((pHardcopyImage != NULL) && (bitmap != NULL) && (size > 0))
  {
    if (pHardcopyImage->getOutputData(bitmap, size, 8 /*bits*/))
      status = EC_Normal;
  }
  return status;
}

OFCondition DVInterface::setCurrentPrinter(const char *targetID)
{
  if (targetID == NULL) return EC_IllegalCall;
  if (getTargetHostname(targetID) == NULL) return EC_IllegalCall; // Printer seems to be unknown
  activateAnnotation = getTargetPrinterSupportsAnnotation(targetID);
  if (pPrint != NULL)
  {
    pPrint->setPrinterName(targetID);
    pPrint->setDestination(getTargetAETitle(targetID));
  }
  currentPrinter = targetID;
  return EC_Normal;
}

const char *DVInterface::getCurrentPrinter()
{
  return currentPrinter.c_str();
}

OFCondition DVInterface::setPrinterMediumType(const char *value)
{
  if (value) printerMediumType = value; else printerMediumType.clear();
  return EC_Normal;
}

const char *DVInterface::getPrinterMediumType()
{
  return printerMediumType.c_str();
}

OFCondition DVInterface::setPrinterFilmDestination(const char *value)
{
  if (value) printerFilmDestination = value; else printerFilmDestination.clear();
  return EC_Normal;
}

const char *DVInterface::getPrinterFilmDestination()
{
  return printerFilmDestination.c_str();
}

OFCondition DVInterface::setPrinterFilmSessionLabel(const char *value)
{
  if (value) printerFilmSessionLabel = value; else printerFilmSessionLabel.clear();
  return EC_Normal;
}

const char *DVInterface::getPrinterFilmSessionLabel()
{
  return printerFilmSessionLabel.c_str();
}

OFCondition DVInterface::setPrinterPriority(const char *value)
{
  if (value) printerPriority = value; else printerPriority.clear();
  return EC_Normal;
}

const char *DVInterface::getPrinterPriority()
{
  return printerPriority.c_str();
}

OFCondition DVInterface::setPrinterOwnerID(const char *value)
{
  if (value) printerOwnerID = value; else printerOwnerID.clear();
  return EC_Normal;
}

const char *DVInterface::getPrinterOwnerID()
{
  return printerOwnerID.c_str();
}

OFCondition DVInterface::setPrinterNumberOfCopies(unsigned long value)
{
  printerNumberOfCopies = value;
  return EC_Normal;
}

unsigned long DVInterface::getPrinterNumberOfCopies()
{
  return printerNumberOfCopies;
}

OFCondition DVInterface::selectDisplayPresentationLUT(const char *lutID)
{
  OFCondition result = EC_IllegalCall;
  if (lutID && pState)
  {
     const char *lutfile = getLUTFilename(lutID);
     if (lutfile)
     {
       OFString filename = getLUTFolder(); // never NULL.
       filename += PATH_SEPARATOR;
       filename += lutfile;
       DcmFileFormat *fileformat = NULL;
       if ((result = DVPSHelper::loadFileFormat(filename.c_str(), fileformat)) == EC_Normal)
       {
         if (fileformat)
         {
           DcmDataset *dataset = fileformat->getDataset();
           if (dataset)
             result = pState->setPresentationLookupTable(*dataset);
           else
             result = EC_IllegalCall;
           if (EC_Normal == result)
             displayCurrentLUTID = lutID;
           else
             displayCurrentLUTID.clear();
         } else result = EC_IllegalCall;
         if (result != EC_Normal)
           DCMPSTAT_LOGFILE("Load display presentation LUT from file: invalid data structures");
       } else
         DCMPSTAT_LOGFILE("Load display presentation LUT from file: could not read fileformat");
       delete fileformat;
     } else
       DCMPSTAT_LOGFILE("Load display presentation LUT from file: not specified in config file");
  }
  return result;
}

const char *DVInterface::getDisplayPresentationLUTID()
{
  return displayCurrentLUTID.c_str();
}

OFCondition DVInterface::selectPrintPresentationLUT(const char *lutID)
{
  OFCondition result = EC_IllegalCall;
  if (lutID && pPrint)
  {
     const char *lutfile = getLUTFilename(lutID);
     if (lutfile)
     {
       OFString filename = getLUTFolder(); // never NULL.
       filename += PATH_SEPARATOR;
       filename += lutfile;
       DcmFileFormat *fileformat = NULL;
       if ((result = DVPSHelper::loadFileFormat(filename.c_str(), fileformat)) == EC_Normal)
       {
         if (fileformat)
         {
           DcmDataset *dataset = fileformat->getDataset();
           if (dataset)
             result = pPrint->setPresentationLookupTable(*dataset);
           else
             result = EC_IllegalCall;
           if (EC_Normal == result)
             printCurrentLUTID = lutID;
           else
             printCurrentLUTID.clear();
         } else result = EC_IllegalCall;
         if (result != EC_Normal)
           DCMPSTAT_LOGFILE("Load print presentation LUT from file: invalid data structures");
       } else
         DCMPSTAT_LOGFILE("Load print presentation LUT from file: could not read fileformat");
       delete fileformat;
     } else
       DCMPSTAT_LOGFILE("Load print presentation LUT from file: not specified in config file");
  }
  return result;
}

const char *DVInterface::getPrintPresentationLUTID()
{
  return printCurrentLUTID.c_str();
}

OFCondition DVInterface::spoolPrintJob(OFBool deletePrintedImages)
{
  if (pPrint==NULL) return EC_IllegalCall;
  if (currentPrinter.size()==0) return EC_IllegalCall;

  OFCondition result = saveStoredPrint(getTargetPrinterSupportsRequestedImageSize(currentPrinter.c_str()));
  if (EC_Normal == result)
  {
    result = spoolStoredPrintFromDB(pPrint->getStudyInstanceUID(), pPrint->getSeriesInstanceUID(), pPrint->getSOPInstanceUID());
  }
  if ((EC_Normal == result) && deletePrintedImages) result = pPrint->deleteSpooledImages();
  return result;
}

OFCondition DVInterface::startPrintSpooler()
{
  const char *spooler_application = getSpoolerName();
  if (spooler_application==NULL) return EC_IllegalCall;
  if (configPath.length()==0) return EC_IllegalCall;

  const char *printer = NULL;
  unsigned long sleepingTime = getSpoolerSleep();
  if (sleepingTime==0) sleepingTime=1; // default
  char sleepStr[20];
  sprintf(sleepStr, "%lu", sleepingTime);
  OFBool detailedLog = getDetailedLog();

  OFCondition result = EC_Normal;
  DCMPSTAT_LOGFILE("Starting print spooler process ...");

  DVPSHelper::cleanChildren(); // clean up old child processes before creating new ones

  Uint32 numberOfPrinters = getNumberOfTargets(DVPSE_printAny);
  if (numberOfPrinters > 0) for (Uint32 i=0; i < numberOfPrinters; i++)
  {
        printer = getTargetID(i, DVPSE_printAny);

#ifdef HAVE_FORK
    // Unix version - call fork() and execl()
    pid_t pid = fork();
    if (pid < 0) result = EC_IllegalCall; // fork failed - return error code
    else if (pid==0)
    {
      // we are the child process

      if (detailedLog)
      {
        if (execl(spooler_application, spooler_application, "--verbose", "--dump", "--spool", printJobIdentifier.c_str(),
          "--printer", printer, "--config", configPath.c_str(), "--sleep", sleepStr, NULL) < 0)
        {
          DCMPSTAT_ERROR("Unable to execute '" << spooler_application << "'");
        }
      } else {
        if (execl(spooler_application, spooler_application, "--spool", printJobIdentifier.c_str(),
          "--printer", printer, "--config", configPath.c_str(), "--sleep", sleepStr, NULL) < 0)
        {
          DCMPSTAT_ERROR("Unable to execute '" << spooler_application << "'");
        }
      }

      // if execl succeeds, this part will not get executed.
      // if execl fails, there is not much we can do except bailing out.
      abort();
    }
#else
    // Windows version - call CreateProcess()
    // initialize startup info
    PROCESS_INFORMATION procinfo;
    STARTUPINFO sinfo;
    OFBitmanipTemplate<char>::zeroMem((char *)&sinfo, sizeof(sinfo));
    sinfo.cb = sizeof(sinfo);
    char commandline[4096];
    if (detailedLog)
    {
      sprintf(commandline, "%s --verbose --dump --spool %s --printer %s --config %s --sleep %s", spooler_application,
        printJobIdentifier.c_str(), printer, configPath.c_str(), sleepStr);
    } else {
      sprintf(commandline, "%s --spool %s --printer %s --config %s --sleep %s", spooler_application,
        printJobIdentifier.c_str(), printer, configPath.c_str(), sleepStr);
    }
#ifdef DEBUG
    if (0 == CreateProcess(NULL, commandline, NULL, NULL, 0, 0, NULL, NULL, &sinfo, &procinfo))
#else
    if (0 == CreateProcess(NULL, commandline, NULL, NULL, 0, DETACHED_PROCESS, NULL, NULL, &sinfo, &procinfo))
#endif
    {
      DCMPSTAT_ERROR("Unable to execute '" << spooler_application << "'");
      result = EC_IllegalCall;
    }
#endif
  }
  return result;
}

OFCondition DVInterface::createPrintJobFilenames(const char *printer, OFString& tempname, OFString& jobname)
{
  tempname.clear();
  jobname.clear();
  if (printer==NULL) return EC_IllegalCall;
  char buf[20];

  sprintf(buf, "%04lu", printJobCounter++);
  jobname =  getSpoolFolder();
  jobname += PATH_SEPARATOR;
  jobname += printJobIdentifier;
  jobname += '_';
  jobname += printer;
  jobname += '_';
  jobname += buf;
  tempname = jobname;
  jobname += PRINTJOB_SUFFIX;
  tempname += PRINTJOB_TEMP_SUFFIX;
  return EC_Normal;
}

OFCondition DVInterface::terminatePrintSpooler()
{
  if (getSpoolerName()==NULL) return EC_IllegalCall;
  if (configPath.length()==0) return EC_IllegalCall;

  DVPSHelper::cleanChildren(); // clean up old child processes before creating new ones
  OFString spoolFilename;
  OFString tempFilename;
  const char *prt = NULL;

  DCMPSTAT_LOGFILE("Terminating print spooler process ...");

  Uint32 numberOfPrinters = getNumberOfTargets(DVPSE_printAny);
  if (numberOfPrinters > 0) for (Uint32 i=0; i < numberOfPrinters; i++)
  {
    prt = getTargetID(i, DVPSE_printAny);
    if (EC_Normal != createPrintJobFilenames(prt, tempFilename, spoolFilename)) return EC_IllegalCall;
    FILE *outf = fopen(tempFilename.c_str(),"wb");
    if (outf)
    {
      OFString timeString;
      OFDateTime::getCurrentDateTime().getISOFormattedDateTime(timeString);
      fprintf(outf,"#\n# print job created %s\n", timeString.c_str());
      fprintf(outf,"# target printer: [%s]\n#\n", (prt ? prt : "none"));
      fprintf(outf,"terminate\n");
      fclose(outf);
      if (0 != rename(tempFilename.c_str(), spoolFilename.c_str()))
      {
        DCMPSTAT_ERROR("Unable to activate spooler termination request '" << spoolFilename.c_str() << "'");
        return EC_IllegalCall;
      }
    } else {
      DCMPSTAT_ERROR("Unable to create spooler termination request '" << tempFilename.c_str() << "'");
      return EC_IllegalCall;
    }
  }
  return EC_Normal;
}

OFCondition DVInterface::startPrintServer()
{
  const char *application = getPrintServerName();
  if (application==NULL) return EC_IllegalCall;
  if (configPath.length()==0) return EC_IllegalCall;

  const char *printer = NULL;
  OFBool detailedLog = getDetailedLog();

  OFCondition result = EC_Normal;
  DCMPSTAT_LOGFILE("Starting print server process ...");

  DVPSHelper::cleanChildren(); // clean up old child processes before creating new ones

  Uint32 numberOfPrinters = getNumberOfTargets(DVPSE_printLocal);
  if (numberOfPrinters > 0) for (Uint32 i=0; i < numberOfPrinters; i++)
  {
    printer = getTargetID(i, DVPSE_printLocal);

#ifdef HAVE_FORK
    // Unix version - call fork() and execl()
    pid_t pid = fork();
    if (pid < 0) result = EC_IllegalCall; // fork failed - return error code
    else if (pid==0)
    {
      // we are the child process

      if (detailedLog)
      {
        if (execl(application, application, "--logfile", "--verbose", "--dump", "--printer", printer, "--config",
            configPath.c_str(), NULL) < 0)
        {
          DCMPSTAT_ERROR("Unable to execute '" << application << "'");
        }
      } else {
        if (execl(application, application, "--logfile", "--printer", printer, "--config", configPath.c_str(), NULL) < 0)
        {
          DCMPSTAT_ERROR("Unable to execute '" << application << "'");
        }
      }

      // if execl succeeds, this part will not get executed.
      // if execl fails, there is not much we can do except bailing out.
      abort();
    }
#else
    // Windows version - call CreateProcess()
    // initialize startup info
    PROCESS_INFORMATION procinfo;
    STARTUPINFO sinfo;
    OFBitmanipTemplate<char>::zeroMem((char *)&sinfo, sizeof(sinfo));
    sinfo.cb = sizeof(sinfo);
    char commandline[4096];
    if (detailedLog)
    {
      sprintf(commandline, "%s --logfile --verbose --dump --printer %s --config %s", application, printer, configPath.c_str());
    } else {
      sprintf(commandline, "%s --logfile --printer %s --config %s", application, printer, configPath.c_str());
    }
#ifdef DEBUG
    if (0 == CreateProcess(NULL, commandline, NULL, NULL, 0, 0, NULL, NULL, &sinfo, &procinfo))
#else
    if (0 == CreateProcess(NULL, commandline, NULL, NULL, 0, DETACHED_PROCESS, NULL, NULL, &sinfo, &procinfo))
#endif
    {
      DCMPSTAT_ERROR("Unable to execute '" << application << "'");
      result = EC_IllegalCall;
    }
#endif
  }
  return result;    // result of last process only
}

OFCondition DVInterface::terminatePrintServer()
{
  if (getPrintServerName()==NULL) return EC_IllegalCall;
  if (configPath.length()==0) return EC_IllegalCall;

#ifdef HAVE_GUSI_H
  GUSISetup(GUSIwithSIOUXSockets);
  GUSISetup(GUSIwithInternetSockets);
#endif

#ifdef HAVE_WINSOCK_H
  WSAData winSockData;
  /* we need at least version 1.1 */
  WORD winSockVersionNeeded = MAKEWORD( 1, 1 );
  WSAStartup(winSockVersionNeeded, &winSockData);
#endif

  OFCondition result = EC_Normal;
  T_ASC_Network *net=NULL;
  T_ASC_Parameters *params=NULL;
  DIC_NODENAME localHost;
  DIC_NODENAME peerHost;
  T_ASC_Association *assoc=NULL;
  const char *target = NULL;
  OFBool useTLS = OFFalse;

  DCMPSTAT_LOGFILE("Terminating print server process ...");

#ifdef WITH_OPENSSL
  /* TLS directory */
  const char *current = NULL;
  const char *tlsFolder = getTLSFolder();
  if (tlsFolder==NULL) tlsFolder = ".";

  /* key file format */
  int keyFileFormat = SSL_FILETYPE_PEM;
  if (! getTLSPEMFormat()) keyFileFormat = SSL_FILETYPE_ASN1;
#endif

  Uint32 numberOfPrinters = getNumberOfTargets(DVPSE_printLocal);
  if (numberOfPrinters > 0) for (Uint32 i=0; i < numberOfPrinters; i++)
  {
    target = getTargetID(i, DVPSE_printLocal);
    useTLS = getTargetUseTLS(target);

    OFCondition cond = ASC_initializeNetwork(NET_REQUESTOR, 0, 30, &net);
    if (cond.good())
    {
      cond = ASC_createAssociationParameters(&params, DEFAULT_MAXPDU);
      if (cond.good())
      {
        if (useTLS)
        {
#ifdef WITH_OPENSSL
          /* certificate file */
          OFString tlsCertificateFile(tlsFolder);
          tlsCertificateFile += PATH_SEPARATOR;
          current = getTargetCertificate(target);
          if (current) tlsCertificateFile += current; else tlsCertificateFile += "sitecert.pem";

          /* private key file */
          OFString tlsPrivateKeyFile(tlsFolder);
          tlsPrivateKeyFile += PATH_SEPARATOR;
          current = getTargetPrivateKey(target);
          if (current) tlsPrivateKeyFile += current; else tlsPrivateKeyFile += "sitekey.pem";

          /* private key password */
          const char *tlsPrivateKeyPassword = getTargetPrivateKeyPassword(target);

          /* DH parameter file */
          OFString tlsDHParametersFile;
          current = getTargetDiffieHellmanParameters(target);
          if (current)
          {
            tlsDHParametersFile = tlsFolder;
            tlsDHParametersFile += PATH_SEPARATOR;
            tlsDHParametersFile += current;
          }

          /* random seed file */
          OFString tlsRandomSeedFile(tlsFolder);
          tlsRandomSeedFile += PATH_SEPARATOR;
          current = getTargetRandomSeed(target);
          if (current) tlsRandomSeedFile += current; else tlsRandomSeedFile += "siteseed.bin";

          /* CA certificate directory */
          const char *tlsCACertificateFolder = getTLSCACertificateFolder();
          if (tlsCACertificateFolder==NULL) tlsCACertificateFolder = ".";

          /* ciphersuites */
#if OPENSSL_VERSION_NUMBER >= 0x0090700fL
          OFString tlsCiphersuites(TLS1_TXT_RSA_WITH_AES_128_SHA ":" SSL3_TXT_RSA_DES_192_CBC3_SHA);
#else
          OFString tlsCiphersuites(SSL3_TXT_RSA_DES_192_CBC3_SHA);
#endif
          Uint32 tlsNumberOfCiphersuites = getTargetNumberOfCipherSuites(target);
          if (tlsNumberOfCiphersuites > 0)
          {
            tlsCiphersuites.clear();
            OFString currentSuite;
            const char *currentOpenSSL;
            for (Uint32 ui=0; ui<tlsNumberOfCiphersuites; ui++)
            {
              getTargetCipherSuite(target, ui, currentSuite);
              if (NULL != (currentOpenSSL = DcmTLSTransportLayer::findOpenSSLCipherSuiteName(currentSuite.c_str())))
              {
                if (tlsCiphersuites.length() > 0) tlsCiphersuites += ":";
                tlsCiphersuites += currentOpenSSL;
              }
            }
          }

          DcmTLSTransportLayer *tLayer = new DcmTLSTransportLayer(DICOM_APPLICATION_REQUESTOR, tlsRandomSeedFile.c_str());
          if (tLayer)
          {
            if (tlsCACertificateFolder) tLayer->addTrustedCertificateDir(tlsCACertificateFolder, keyFileFormat);
            if (tlsDHParametersFile.size() > 0) tLayer->setTempDHParameters(tlsDHParametersFile.c_str());
            tLayer->setPrivateKeyPasswd(tlsPrivateKeyPassword); // never prompt on console
            tLayer->setPrivateKeyFile(tlsPrivateKeyFile.c_str(), keyFileFormat);
            tLayer->setCertificateFile(tlsCertificateFile.c_str(), keyFileFormat);
            tLayer->setCipherSuites(tlsCiphersuites.c_str());
            tLayer->setCertificateVerification(DCV_ignoreCertificate);
            ASC_setTransportLayer(net, tLayer, 1);
          }
#else
          // we cannot shutdown a TLS process since we're compiled without OpenSSL support
          cond = EC_IllegalCall;
#endif
        }

        ASC_setAPTitles(params, getNetworkAETitle(), getTargetAETitle(target), NULL);
        gethostname(localHost, sizeof(localHost) - 1);
        sprintf(peerHost, "%s:%d", getTargetHostname(target), OFstatic_cast(int, getTargetPort(target)));
        ASC_setPresentationAddresses(params, localHost, peerHost);

        if (cond.good()) cond = ASC_setTransportLayerType(params, useTLS);

        const char* transferSyntaxes[] = { UID_LittleEndianImplicitTransferSyntax };
        if (cond.good()) cond = ASC_addPresentationContext(params, 1, UID_PrivateShutdownSOPClass, transferSyntaxes, 1);
        if (cond.good())
        {
          cond = ASC_requestAssociation(net, params, &assoc);
          if (cond.good()) ASC_abortAssociation(assoc); // tear down association if necessary
          ASC_dropAssociation(assoc);
          ASC_destroyAssociation(&assoc);
        }
      } else result = EC_IllegalCall;
      ASC_dropNetwork(&net);
    } else result = EC_IllegalCall;
  }

#ifdef HAVE_WINSOCK_H
  WSACleanup();
#endif

  return result;    // result of last process only
}

OFCondition DVInterface::addToPrintHardcopyFromDB(const char *studyUID, const char *seriesUID, const char *instanceUID)
{
  OFCondition result = EC_IllegalCall;

  if (pPrint)
  {
    if (studyUID && seriesUID && instanceUID)
    {
      if (EC_Normal == (result = lockDatabase()))
      {
        DcmUniqueIdentifier sopclassuid(DCM_SOPClassUID);
        const char *filename = getFilename(studyUID, seriesUID, instanceUID);
        if (filename)
        {
          DcmFileFormat *ff = NULL;
          if ((result = DVPSHelper::loadFileFormat(filename, ff)) == EC_Normal)
          {
            if (ff)
            {
              DcmDataset *dataset = ff->getDataset();
              if (dataset)
              {
                DcmStack stack;
                DVPSPresentationLUT presentationLUT;
                if (EC_Normal != presentationLUT.read(*dataset, OFFalse)) presentationLUT.setType(DVPSP_identity);
                    result = dataset->search(sopclassuid.getTag(), stack, ESM_fromHere, OFFalse);
                if (EC_Normal == result)
                {
                  char *sopclass = NULL;
                  sopclassuid = *OFstatic_cast(DcmUniqueIdentifier *, stack.top());
                  if (EC_Normal == sopclassuid.getString(sopclass))
                    result = pPrint->addImageBox(getNetworkAETitle(), studyUID, seriesUID,
                      sopclass, instanceUID, NULL, NULL, &presentationLUT, OFFalse);
                  else
                    result = EC_IllegalCall;
                }
              } else result = EC_CorruptedData;
            } else result = EC_IllegalCall;
            if (result != EC_Normal)
                DCMPSTAT_LOGFILE("Load hardcopy grayscale image from file failed: invalid data structures");
          } else
            DCMPSTAT_LOGFILE("Load hardcopy grayscale image from file failed: could not read fileformat");
          if (ff) delete ff;
        } else {
          result = EC_IllegalCall;
          DCMPSTAT_LOGFILE("Load hardcopy grayscale image from database failed: UIDs not in index file");
        }
      } else
        DCMPSTAT_LOGFILE("Load hardcopy grayscale image from database failed: could not lock index file");
    } else
      DCMPSTAT_LOGFILE("Load hardcopy grayscale image from database failed: invalid UIDs");
  }

  releaseDatabase();
  return result;
}

OFCondition DVInterface::spoolStoredPrintFromDB(const char *studyUID, const char *seriesUID, const char *instanceUID)
{
  if ((studyUID==NULL)||(seriesUID==NULL)||(instanceUID==NULL)||(configPath.length()==0)) return EC_IllegalCall;
  OFString spoolFilename;
  OFString tempFilename;
  const char *prt = getCurrentPrinter();
  if (EC_Normal != createPrintJobFilenames(prt, tempFilename, spoolFilename)) return EC_IllegalCall;

  FILE *outf = fopen(tempFilename.c_str(),"wb");
  if (outf)
  {
    OFString timeString;
    OFDateTime::getCurrentDateTime().getISOFormattedDateTime(timeString);
    fprintf(outf, "#\n# print job created %s\n", timeString.c_str());
    fprintf(outf, "# target printer: [%s]\n#\n", (prt ? prt : "none"));
    fprintf(outf, "study        %s\nseries       %s\ninstance     %s\n", studyUID, seriesUID, instanceUID);
    if (printerMediumType.size() >0)       fprintf(outf,"mediumtype   %s\n", printerMediumType.c_str());
    if (printerFilmDestination.size() >0)  fprintf(outf,"destination  %s\n", printerFilmDestination.c_str());
    if (printerFilmSessionLabel.size() >0) fprintf(outf,"label        %s\n", printerFilmSessionLabel.c_str());
    if (printerPriority.size() >0)         fprintf(outf,"priority     %s\n", printerPriority.c_str());
    if (printerOwnerID.size() >0)          fprintf(outf,"owner_id     %s\n", printerOwnerID.c_str());
    if (printerNumberOfCopies >0)          fprintf(outf,"copies       %lu\n", printerNumberOfCopies);

    fclose(outf);
    if (0 != rename(tempFilename.c_str(), spoolFilename.c_str()))
    {
      DCMPSTAT_ERROR("Unable to activate print job '" << spoolFilename.c_str() << "'");
      return EC_IllegalCall;
    }
  } else {
    DCMPSTAT_ERROR("Unable to create print job '" << tempFilename.c_str() << "'");
    return EC_IllegalCall;
  }
  return EC_Normal;
}

OFCondition DVInterface::printSCUcreateBasicFilmSession(DVPSPrintMessageHandler& printHandler, OFBool plutInSession)
{
  if (!pPrint) return EC_IllegalCall;
  OFCondition result = EC_Normal;
  DcmDataset dset;
  DcmElement *delem = NULL;
  char buf[20];

  if ((EC_Normal==result)&&(printerMediumType.size() > 0))
  {
    delem = new DcmCodeString(DCM_MediumType);
    if (delem) result = delem->putString(printerMediumType.c_str()); else result=EC_IllegalCall;
    if (EC_Normal==result) result = dset.insert(delem, OFTrue /*replaceOld*/);
  }

  if ((EC_Normal==result)&&(printerFilmDestination.size() > 0))
  {
    delem = new DcmCodeString(DCM_FilmDestination);
    if (delem) result = delem->putString(printerFilmDestination.c_str()); else result=EC_IllegalCall;
    if (EC_Normal==result) result = dset.insert(delem, OFTrue /*replaceOld*/);
  }

  if ((EC_Normal==result)&&(printerFilmSessionLabel.size() > 0))
  {
    delem = new DcmLongString(DCM_FilmSessionLabel);
    if (delem) result = delem->putString(printerFilmSessionLabel.c_str()); else result=EC_IllegalCall;
    if (EC_Normal==result) result = dset.insert(delem, OFTrue /*replaceOld*/);
  }

  if ((EC_Normal==result)&&(printerPriority.size() > 0))
  {
    delem = new DcmCodeString(DCM_PrintPriority);
    if (delem) result = delem->putString(printerPriority.c_str()); else result=EC_IllegalCall;
    if (EC_Normal==result) result = dset.insert(delem, OFTrue /*replaceOld*/);
  }

  if ((EC_Normal==result)&&(printerOwnerID.size() > 0))
  {
    delem = new DcmShortString(DCM_OwnerID);
    if (delem) result = delem->putString(printerOwnerID.c_str()); else result=EC_IllegalCall;
    if (EC_Normal==result) result = dset.insert(delem, OFTrue /*replaceOld*/);
  }

  if ((EC_Normal==result)&&(printerNumberOfCopies > 0))
  {
    sprintf(buf, "%lu", printerNumberOfCopies);
    delem = new DcmIntegerString(DCM_NumberOfCopies);
    if (delem) result = delem->putString(buf); else result=EC_IllegalCall;
    if (EC_Normal==result) result = dset.insert(delem, OFTrue /*replaceOld*/);
  }

  if (EC_Normal==result) result = pPrint->printSCUcreateBasicFilmSession(printHandler, dset, plutInSession);
  return result;
}

void DVInterface::clearFilmSessionSettings()
{
  printerMediumType.clear();
  printerFilmDestination.clear();
  printerFilmSessionLabel.clear();
  printerPriority.clear();
  printerOwnerID.clear();
  printerNumberOfCopies = 0;
  return;
}

void DVInterface::setAnnotationText(const char *value)
{
  if (value) annotationText=value; else annotationText.clear();
  return;
}

OFCondition DVInterface::startExternalApplication(const char *application, const char *filename)
{
  if ((filename==NULL)||(application==NULL)) return EC_IllegalCall;
  DVPSHelper::cleanChildren(); // clean up old child processes before creating new ones

#ifdef HAVE_FORK
  // Unix version - call fork() and execl()
  pid_t pid = fork();
  if (pid < 0) return EC_IllegalCall; // fork failed - return error code
  else if (pid > 0) return EC_Normal; // we are the parent process
  else
  {
    // we are the child process
    if (execl(application, application, filename, NULL) < 0)
    {
      DCMPSTAT_ERROR("Unable to execute '" << application << "'");
    }
    // if execl succeeds, this part will not get executed.
    // if execl fails, there is not much we can do except bailing out.
    abort();
  }
#else
  // Windows version - call CreateProcess()

  // initialize startup info
  PROCESS_INFORMATION procinfo;
  STARTUPINFO sinfo;
  OFBitmanipTemplate<char>::zeroMem((char *)&sinfo, sizeof(sinfo));
  sinfo.cb = sizeof(sinfo);
  char commandline[4096];
  sprintf(commandline, "%s %s", application, filename);
#ifdef DEBUG
  if (CreateProcess(NULL, commandline, NULL, NULL, 0, 0, NULL, NULL, &sinfo, &procinfo))
#else
  if (CreateProcess(NULL, commandline, NULL, NULL, 0, DETACHED_PROCESS, NULL, NULL, &sinfo, &procinfo))
#endif
  {
    return EC_Normal;
  } else {
    DCMPSTAT_ERROR("Unable to execute '" << application << "'");
  }
#endif
  return EC_IllegalCall;
}

OFCondition DVInterface::dumpIOD(const char *filename)
{
  OFCondition result = startExternalApplication(getDumpToolName(), filename);
  if (result != EC_Normal)
    DCMPSTAT_LOGFILE("Dump IOD failed: could not start dump application");
  return result;
}

OFCondition DVInterface::dumpIOD(const char *studyUID, const char *seriesUID, const char *instanceUID)
{
  OFCondition result = EC_IllegalCall;
  if (studyUID && seriesUID && instanceUID)
  {
    if (EC_Normal == (result = lockDatabase()))
    {
      const char *filename = getFilename(studyUID, seriesUID, instanceUID);
      if (filename)
        result = dumpIOD(filename);
      else
      {
        result = EC_IllegalCall;
        DCMPSTAT_LOGFILE("Dump IOD from database failed: could not lock index file");
      }
    } else
      DCMPSTAT_LOGFILE("Dump IOD from database failed: UIDs not in index file");
  } else
    DCMPSTAT_LOGFILE("Dump IOD from database failed: invalid UIDs");
  return result;
}

OFCondition DVInterface::checkIOD(const char *filename)
{
  OFCondition result = startExternalApplication(getCheckToolName(), filename);
  if (result != EC_Normal)
    DCMPSTAT_LOGFILE("Check IOD failed: could not start evaluator application");
  return result;
}

OFCondition DVInterface::checkIOD(const char *studyUID, const char *seriesUID, const char *instanceUID)
{
  OFCondition result = EC_IllegalCall;
  if (studyUID && seriesUID && instanceUID)
  {
    if (EC_Normal == (result = lockDatabase()))
    {
      const char *filename = getFilename(studyUID, seriesUID, instanceUID);
      if (filename)
        result = checkIOD(filename);
      else
      {
        result = EC_IllegalCall;
        DCMPSTAT_LOGFILE("Check IOD from database failed: could not lock index file");
      }
    } else
      DCMPSTAT_LOGFILE("Check IOD from database failed: UIDs not in index file");
  } else
    DCMPSTAT_LOGFILE("Check IOD from database failed: invalid UIDs");
  return result;
}

#ifdef WITH_OPENSSL

/* buf     : buffer to write password into
 * size    : length of buffer in bytes
 * rwflag  : nonzero if the password will be used as a new password, i.e. user should be asked to repeat the password
 * userdata: arbitrary pointer that can be set with SSL_CTX_set_default_passwd_cb_userdata()
 * returns : number of bytes written to password buffer, -1 upon error
 */
extern "C" int DVInterfacePasswordCallback(char *buf, int size, int rwflag, void *userdata);

int DVInterfacePasswordCallback(char *buf, int size, int /* rwflag */, void *userdata)
{
  if (userdata == NULL) return -1;
  OFString *password = (OFString *)userdata;
  int passwordSize = password->length();
  if (passwordSize > size) passwordSize = size;
  strncpy(buf, password->c_str(), passwordSize);
  return passwordSize;
}

#endif


#ifdef WITH_OPENSSL
OFBool DVInterface::verifyUserPassword(const char *userID, const char *passwd)
#else
OFBool DVInterface::verifyUserPassword(const char * /*userID*/, const char * /*passwd*/)
#endif
{
  OFBool result = OFFalse;
#ifdef WITH_OPENSSL
  OFString filename;
  OFString privateKeyPasswd;
  if (passwd) privateKeyPasswd = passwd;
  OFBool isPEMFormat = getTLSPEMFormat();
  const char *userKey = getUserPrivateKey(userID);
  if (userKey == NULL)
    DCMPSTAT_LOGFILE("Cannot verify user password: unknown user or undefined private key file");
  else
  {
    const char *userDir = getUserCertificateFolder();
    if (userDir)
    {
      filename = userDir;
      filename += PATH_SEPARATOR;
    }
    filename += userKey;

    /* attempt to load the private key with the given password*/
    EVP_PKEY *pkey = NULL;
    BIO *in = BIO_new(BIO_s_file_internal());
    if (in)
    {
      if (BIO_read_filename(in, filename.c_str()) > 0)
      {
        if (isPEMFormat)
        {
          pkey = PEM_read_bio_PrivateKey(in, NULL, DVInterfacePasswordCallback, &privateKeyPasswd);
          if (pkey) result = OFTrue;
        } else {
          // ASN.1/DER encoded keys are never encrypted, thus no callback here.
          pkey = d2i_PrivateKey_bio(in, NULL);
          if (pkey) result = OFTrue;
        }
      } else
        DCMPSTAT_LOGFILE("Cannot verify user password: private key file not found");
      BIO_free(in);
    }
    if (pkey) EVP_PKEY_free(pkey);
  }
#else
  DCMPSTAT_LOGFILE("Cannot verify user password: not compiled with OpenSSL support");
#endif
  return result;
}

#ifdef WITH_OPENSSL
OFCondition DVInterface::verifyAndSignStructuredReport(const char *userID, const char *passwd, DVPSVerifyAndSignMode mode)
#else
OFCondition DVInterface::verifyAndSignStructuredReport(const char *userID, const char * /*passwd*/, DVPSVerifyAndSignMode mode)
#endif
{
  OFCondition result = EC_IllegalCall;
  if ((pReport != NULL) && (userID != NULL))
  {
    OFString userName(getUserDICOMName(userID));
    OFString userOrg(getUserOrganization(userID));
    OFString userCV, userCSD, userCSV, userCM;
    DSRCodedEntryValue userCode(getUserCodeValue(userID, userCV), getUserCodingSchemeDesignator(userID, userCSD),
                                getUserCodingSchemeVersion(userID, userCSV), getUserCodeMeaning(userID, userCM));
    /* verify document */
    if (pReport->verifyDocument(userName, userCode, userOrg) == EC_Normal)
    {
      if ((mode == DVPSY_verifyAndSign) || (mode == DVPSY_verifyAndSign_finalize))
      {
#ifdef WITH_OPENSSL
        if (pSignatureHandler)
        {
          DcmStack stack;
          DcmItem dataset;
          if (pReport->write(dataset, &stack) == EC_Normal)
          {
            DcmAttributeTag tagList(DcmTag(0, 0) /* irrelevant value */);
            if (mode == DVPSY_verifyAndSign)
            {
              /* do not sign particular attributes */
              tagList.putTagVal(DCM_SOPInstanceUID, 0);
              tagList.putTagVal(DCM_VerifyingObserverSequence, 1);
              tagList.putTagVal(DCM_InstanceCreationDate, 2);
              tagList.putTagVal(DCM_InstanceCreationTime, 3);
              tagList.putTagVal(DCM_InstanceCreatorUID, 4);
            }
            else if (mode == DVPSY_verifyAndSign_finalize)
            {
              /* always sign the entire document */
              stack.clear();
            }
            /* if no item is marked, sign entire dataset */
            if (stack.empty())
              stack.push(&dataset);
            /* digitally sign document */
            if (pSignatureHandler->createSignature(dataset, stack, tagList, userID, passwd) == EC_Normal)
            {
              DSRDocument *newReport = new DSRDocument();
              if (newReport != NULL)
              {
                if (newReport->read(dataset, DSRTypes::RF_readDigitalSignatures) == EC_Normal)
                {
                  /* replace report in memory */
                  delete pReport;
                  pReport = newReport;
                  pSignatureHandler->updateDigitalSignatureInformation(dataset, DVPSS_structuredReport, OFFalse /* onRead? */);
                  if (mode == DVPSY_verifyAndSign_finalize)
                    result = pReport->finalizeDocument();
                  else
                    result = EC_Normal;
                }
              } else
                result = EC_MemoryExhausted;
            }
          }
        }
#else
        DCMPSTAT_LOGFILE("Cannot sign structured report: not compiled with OpenSSL support");
#endif
      } else
        result= EC_Normal;
    }
  }
  return result;
}

const char *DVInterface::getCurrentSignatureValidationHTML(DVPSObjectType objtype) const
{
  return pSignatureHandler->getCurrentSignatureValidationHTML(objtype);
}

const char *DVInterface::getCurrentSignatureValidationOverview() const
{
  return pSignatureHandler->getCurrentSignatureValidationOverview();
}

DVPSSignatureStatus DVInterface::getCurrentSignatureStatus(DVPSObjectType objtype) const
{
  return pSignatureHandler->getCurrentSignatureStatus(objtype);
}

DVPSSignatureStatus DVInterface::getCombinedImagePStateSignatureStatus() const
{
  return pSignatureHandler->getCombinedImagePStateSignatureStatus();
}

unsigned long DVInterface::getNumberOfCorrectSignatures(DVPSObjectType objtype) const
{
  return pSignatureHandler->getNumberOfCorrectSignatures(objtype);
}

unsigned long DVInterface::getNumberOfUntrustworthySignatures(DVPSObjectType objtype) const
{
  return pSignatureHandler->getNumberOfUntrustworthySignatures(objtype);
}

unsigned long DVInterface::getNumberOfCorruptSignatures(DVPSObjectType objtype) const
{
  return pSignatureHandler->getNumberOfCorruptSignatures(objtype);
}

void DVInterface::disableImageAndPState()
{
  pSignatureHandler->disableImageAndPState();
}


/*
 *  CVS/RCS Log:
 *  $Log: dviface.cc,v $
 *  Revision 1.165  2010-10-14 13:14:31  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.164  2010-09-24 13:32:58  joergr
 *  Compared names of SOP Class UIDs with 2009 edition of the DICOM standard. The
 *  resulting name changes are mainly caused by the fact that the corresponding
 *  SOP Class is now retired.
 *
 *  Revision 1.163  2010-08-09 13:21:56  joergr
 *  Updated data dictionary to 2009 edition of the DICOM standard. From now on,
 *  the official "keyword" is used for the attribute name which results in a
 *  number of minor changes (e.g. "PatientsName" is now called "PatientName").
 *
 *  Revision 1.162  2009-12-15 14:50:49  uli
 *  Fixes some issues with --logfile and the config's log options.
 *
 *  Revision 1.161  2009-12-15 12:34:40  uli
 *  Re-added and fixed the command line option --logfile.
 *
 *  Revision 1.160  2009-12-11 16:16:59  joergr
 *  Removed old command line option --logfile when executing external processes.
 *  Slightly modified log messages and log levels.
 *
 *  Revision 1.159  2009-12-09 13:10:55  uli
 *  Fixed include path which was still using an old file name.
 *
 *  Revision 1.158  2009-11-24 14:12:58  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.157  2009-10-13 14:57:49  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.156  2008-04-30 12:38:43  meichel
 *  Fixed compile errors due to changes in attribute tag names
 *
 *  Revision 1.155  2006/08/15 16:57:01  meichel
 *  Updated the code in module dcmpstat to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.154  2005/12/08 15:46:15  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.153  2005/11/23 16:10:34  meichel
 *  Added support for AES ciphersuites in TLS module. All TLS-enabled
 *    tools now support the "AES TLS Secure Transport Connection Profile".
 *
 *  Revision 1.152  2005/11/16 14:58:24  meichel
 *  Set association timeout in ASC_initializeNetwork to 30 seconds. This improves
 *    the responsiveness of the tools if the peer blocks during assoc negotiation.
 *
 *  Revision 1.151  2005/04/04 10:11:59  meichel
 *  Module dcmpstat now uses the dcmqrdb API instead of imagectn for maintaining
 *    the index database
 *
 *  Revision 1.150  2004/08/03 11:43:18  meichel
 *  Headers libc.h and unistd.h are now included via ofstdinc.h
 *
 *  Revision 1.149  2004/02/13 11:49:36  joergr
 *  Adapted code for changed tag names (e.g. PresentationLabel -> ContentLabel).
 *
 *  Revision 1.148  2004/02/04 15:57:48  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.147  2003/12/19 14:57:04  meichel
 *  Completed support for terminating TLS-based print server processes
 *
 *  Revision 1.146  2003/12/19 13:49:57  meichel
 *  DVInterface::terminatePrintServer now also correctly terminates
 *    TLS-based print server processes.
 *
 *  Revision 1.145  2003/11/03 10:56:07  joergr
 *  Modified static type casts on DVPSLogMessageLevel variables to compile with
 *  gcc 2.95.
 *
 *  Revision 1.144  2003/09/18 11:52:18  joergr
 *  Call addPrivateDcmtkCodingScheme() when saving a structured report.
 *  Fixed wrong "assert" (pointer check) statement in saveStructuredReport().
 *  Adapted type casts to new-style typecast operators defined in ofcast.h.
 *
 *  Revision 1.143  2003/06/04 12:30:28  meichel
 *  Added various includes needed by MSVC5 with STL
 *
 *  Revision 1.142  2003/04/29 10:13:56  meichel
 *  Moved configuration file parser from module dcmpstat to ofstd and renamed
 *    class to OFConfigFile. Cleaned up implementation (no more friend declarations).
 *
 *  Revision 1.141  2002/12/20 14:51:58  wilkens
 *  Modified name clash resulting in a compiler error on Solaris 2.5.1 using
 *  compiler SC 2.0.1.
 *
 *  Revision 1.140  2002/11/29 13:16:32  meichel
 *  Introduced new command line option --timeout for controlling the
 *    connection request timeout.
 *
 *  Revision 1.139  2002/11/27 15:48:05  meichel
 *  Adapted module dcmpstat to use of new header file ofstdinc.h
 *
 *  Revision 1.138  2002/04/16 14:02:20  joergr
 *  Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 *  Revision 1.137  2002/04/11 13:13:43  joergr
 *  Replaced direct call of system routines by new standard date and time
 *  functions.
 *
 *  Revision 1.136  2002/01/08 10:37:34  joergr
 *  Corrected spelling of function dcmGenerateUniqueIdentifier().
 *  Changed prefix of UIDs created for series and studies (now using constants
 *  SITE_SERIES_UID_ROOT and SITE_STUDY_UID_ROOT which are supposed to be used
 *  in these cases).
 *
 *  Revision 1.135  2001/11/28 13:56:50  joergr
 *  Check return value of DcmItem::insert() statements where appropriate to
 *  avoid memory leaks when insert procedure fails.
 *
 *  Revision 1.134  2001/10/12 13:46:54  meichel
 *  Adapted dcmpstat to OFCondition based dcmnet module (supports strict mode).
 *
 *  Revision 1.133  2001/09/26 15:36:21  meichel
 *  Adapted dcmpstat to class OFCondition
 *
 *  Revision 1.132  2001/06/05 10:30:55  joergr
 *  Replaced some #ifdef _WIN32 statements by #ifdef HAVE_WINDOWS_H or #ifdef
 *  __CYGWIN__ respectively to reflect the fact that the latest Cygwin/gcc
 *  version does not define _WIN32 any more.
 *
 *  Revision 1.131  2001/05/10 16:44:53  joergr
 *  Added dcmsr as a standard library to dcmpstat (removed preprecessor #ifdef).
 *
 *  Revision 1.130  2001/05/07 16:04:47  joergr
 *  Adapted read SR method call to new parameter scheme (integer flag instead of
 *  boolean mode).
 *
 *  Revision 1.129  2001/02/23 14:57:55  joergr
 *  Update signature status when signing a structured report (not only when
 *  saving/storing the report).
 *
 *  Revision 1.128  2001/02/23 13:31:10  joergr
 *  Changed behaviour of method verifyAndSignStructuredReport() with 'finalize'.
 *  Now the entire document is always signed independently from the tree items
 *  marked.
 *
 *  Revision 1.127  2001/01/29 17:34:41  joergr
 *  Added method to verify and digitally sign structured reports.
 *
 *  Revision 1.126  2001/01/29 14:55:46  meichel
 *  Added new methods for creating signatures and checking the signature
 *    status in module dcmpstat.
 *
 *  Revision 1.125  2001/01/25 15:18:09  meichel
 *  Added initial support for verification of digital signatures
 *    in presentation states, images and structured reports to module dcmpstat.
 *
 *  Revision 1.124  2000/12/13 13:30:22  joergr
 *  Added explicit typecast to keep gcc 2.5.8 (NeXTSTEP) quiet.
 *
 *  Revision 1.123  2000/12/13 13:24:23  meichel
 *  Removed unused local variables
 *
 *  Revision 1.122  2000/12/11 18:18:24  joergr
 *  Removed name of (conditionally) unused method parameters to avoid compiler
 *  warnings (SunCC 2.0.1).
 *
 *  Revision 1.121  2000/12/08 12:46:35  joergr
 *  Separated module dcmsr from dcmpstat (use #define WITH_DCMSR to re-include
 *  it - probably also requires modification of makefiles).
 *
 *  Revision 1.120  2000/11/20 13:22:41  joergr
 *  Fixed minor bugs (string related memory problems when used with JNI).
 *
 *  Revision 1.119  2000/11/14 16:35:21  joergr
 *  Added creation of new UIDs and setting of content date/time when starting
 *  a new SR document from a "template".
 *
 *  Revision 1.118  2000/11/14 13:25:59  meichel
 *  Imagectn was always invoked in debug mode from class DVInterface
 *    on Unix platforms. Fixed.
 *
 *  Revision 1.117  2000/11/13 15:50:45  meichel
 *  Added dcmpstat support methods for creating image references
 *    in SR documents.
 *
 *  Revision 1.116  2000/11/13 11:52:43  meichel
 *  Added support for user logins and certificates.
 *
 *  Revision 1.115  2000/11/13 10:43:20  joergr
 *  Added support for Structured Reporting "templates".
 *
 *  Revision 1.114  2000/11/10 16:21:17  meichel
 *  Fixed problem with DICOMscope being unable to shut down receiver processes
 *    that are operating with TLS encryption by adding a special shutdown mode to
 *    dcmpsrcv.
 *
 *  Revision 1.113  2000/10/16 11:46:45  joergr
 *  Added support for new structured reports.
 *  Added method allowing to select an instance by instance UID and SOP class
 *  UID (without series and study UID). Required for composite references in
 *  DICOM SR.
 *
 *  Revision 1.112  2000/10/10 12:24:39  meichel
 *  Added extensions for IPC message communication
 *
 *  Revision 1.111  2000/08/31 15:56:14  joergr
 *  Switched off interpolation for scaling of print preview images (this caused
 *  problems with "scrambled" presentation LUTs in stored print objects).
 *  Correct bug: pixel aspect ratio and photometric interpretation were ignored
 *  for print preview.
 *
 *  Revision 1.110  2000/07/18 16:05:08  joergr
 *  Moved method convertODtoLum/PValue from class DVInterface to DVPSStoredPrint
 *  and corrected implementation.
 *  Changed behaviour of methods getMin/MaxDensityValue (return default value if
 *  attribute empty/absent).
 *
 *  Revision 1.109  2000/07/17 14:48:21  joergr
 *  Added support for presentation states referencing to hardcopy grayscale
 *  images.
 *
 *  Revision 1.108  2000/07/17 12:05:29  joergr
 *  Added methods to select objects from the database directly.
 *
 *  Revision 1.107  2000/07/14 17:10:10  joergr
 *  Added changeStatus parameter to all methods loading instances from the
 *  database.
 *
 *  Revision 1.106  2000/07/14 11:59:05  joergr
 *  Fixed bug in getNumberOfPStates(study,series,instance) method.
 *
 *  Revision 1.105  2000/07/12 12:52:00  joergr
 *  Fixed bug in loadPrintPreview routine.
 *
 *  Revision 1.104  2000/07/07 14:15:13  joergr
 *  Added support for LIN OD presentation LUT shape.
 *
 *  Revision 1.103  2000/07/06 09:41:17  joergr
 *  Added flag to loadPrintPreview() method allowing to choose how to interpret
 *  the presentation LUT (hardcopy or softcopy definition).
 *
 *  Revision 1.102  2000/07/05 12:32:21  joergr
 *  Added check whether external processes were actually started before
 *  terminating them.
 *  Fixed bug concerning the termination of external processes.
 *
 *  Revision 1.101  2000/07/05 09:00:02  joergr
 *  Added new log output messages.
 *
 *  Revision 1.100  2000/07/04 16:06:28  joergr
 *  Added support for overriding the presentation LUT settings made for the
 *  image boxes.
 *  Added new log output messages.
 *
 *  Revision 1.99  2000/06/22 10:46:47  joergr
 *  Fixed bug creating config file for Q/R server when Q/R server name was not
 *  specified in application config file.
 *
 *  Revision 1.98  2000/06/21 15:41:00  meichel
 *  Added DICOMscope support for calling the Presentation State Checker.
 *
 *  Revision 1.97  2000/06/09 10:14:55  joergr
 *  Added method to get number of presentation states referencing an image
 *  (specified by the three UIDs).
 *
 *  Revision 1.96  2000/06/08 17:38:01  joergr
 *  Corrected bug and added log messages in addImageReferenceToPState().
 *  Added method convertODtoLum().
 *
 *  Revision 1.95  2000/06/07 14:25:38  joergr
 *  Added configuration file entry "LogLevel" to filter log messages.
 *  Added flag to constructor specifying whether the general log file should be
 *  used (default: off).
 *  Added missing transformations (polarity, GSDF, presentation LUT, aspect
 *  ratio) to print preview rendering.
 *  Added log message output to I/O routines.
 *
 *  Revision 1.94  2000/06/07 13:17:26  meichel
 *  added binary and textual log facilities to Print SCP.
 *
 *  Revision 1.93  2000/06/06 09:43:25  joergr
 *  Moved configuration file entry "LogDirectory" from "[PRINT]" to new
 *  (more general) section "[APPLICATION]".
 *
 *  Revision 1.92  2000/06/05 16:24:27  joergr
 *  Implemented log message methods.
 *  Added method allowing to specify the current presentation state to be used
 *  for resetting the pstate.
 *
 *  Revision 1.91  2000/06/02 16:00:55  meichel
 *  Adapted all dcmpstat classes to use OFConsole for log and error output
 *
 *  Revision 1.90  2000/06/02 13:54:35  joergr
 *  Implemented start/terminatePrintServer methods.
 *
 *  Revision 1.89  2000/05/31 12:58:13  meichel
 *  Added initial Print SCP support
 *
 *  Revision 1.88  2000/05/31 07:56:20  joergr
 *  Added support for Stored Print attributes Originator and Destination
 *  application entity title.
 *
 *  Revision 1.87  2000/05/30 14:22:13  joergr
 *  Renamed some variables to avoid compiler warnings (reported by gcc 2.9x with
 *  additional compiler flags).
 *
 *  Revision 1.86  2000/05/30 13:56:23  joergr
 *  Renamed GrayscaleHardcopy to HardcopyGrayscale (which is the correct term
 *  according to the DICOM standard).
 *  Added support for multi-frame images and multiple references from a single
 *  presentation to a number of images.
 *  Removed methods which were already marked as "retired".
 *  Added support for the folowwing new features:
 *    - start/terminate query/retrieve server
 *    - load stored print objects
 *    - create print preview from hardcopy grayscale images
 *
 *  Revision 1.85  2000/03/08 16:29:00  meichel
 *  Updated copyright header.
 *
 *  Revision 1.84  2000/02/29 12:16:19  meichel
 *  Fixed bug in dcmpstat library that caused Monochrome1 images
 *    to be printed inverse if a Presentation LUT was applied.
 *
 *  Revision 1.83  1999/11/25 11:41:10  joergr
 *  Changed config file entry "HighEndSystem" to "HighResolutionGraphics".
 *
 *  Revision 1.82  1999/11/18 18:23:06  meichel
 *  Corrected various memory leaks. DcmFileFormat can be instantiated
 *    with a DcmDataset* as a parameter, but in this case the dataset is
 *    copied and not taken over by the DcmFileFormat. The pointer must
 *    be freed explicitly by the caller.
 *
 *  Revision 1.81  1999/11/03 13:05:33  meichel
 *  Added support for transmitting annotations in the film session label.
 *    Added support for dump tool launched from DVInterface.
 *
 *  Revision 1.80  1999/10/21 15:31:45  joergr
 *  Fixed bug in method addToPrintHardcopyFromDB().
 *
 *  Revision 1.79  1999/10/20 10:54:13  joergr
 *  Added support for a down-scaled preview image of the current DICOM image
 *  (e.g. useful for online-windowing or print preview).
 *  Corrected bug concerning the minimum and maximum print bitmap size (first
 *  presentation state created in the constructor of DVInterface never used the
 *  correct values from the config file).
 *
 *  Revision 1.78  1999/10/19 16:24:56  meichel
 *  Corrected handling of MONOCHROME1 images when used with P-LUTs
 *
 *  Revision 1.77  1999/10/19 14:48:21  meichel
 *  added support for the Basic Annotation Box SOP Class
 *    as well as access methods for Max Density and Min Density.
 *
 *  Revision 1.76  1999/10/13 14:11:59  meichel
 *  Added config file entries and access methods
 *    for user-defined VOI presets, log directory, verbatim logging
 *    and an explicit list of image display formats for each printer.
 *
 *  Revision 1.75  1999/10/13 06:44:17  joergr
 *  Fixed bug in get/setAmbientLightValue()
 *
 *  Revision 1.74  1999/10/07 17:21:56  meichel
 *  Reworked management of Presentation LUTs in order to create tighter
 *    coupling between Softcopy and Print.
 *
 *  Revision 1.73  1999/09/27 10:41:56  meichel
 *  Print interface now copies current printer name, avoids JNI problems.
 *
 *  Revision 1.72  1999/09/24 15:24:32  meichel
 *  Added support for CP 173 (Presentation LUT clarifications)
 *
 *  Revision 1.71  1999/09/23 17:37:15  meichel
 *  Added support for Basic Film Session options to dcmpstat print code.
 *
 *  Revision 1.70  1999/09/17 14:33:50  meichel
 *  Completed print spool functionality including Supplement 22 support
 *
 *  Revision 1.69  1999/09/15 17:43:31  meichel
 *  Implemented print job dispatcher code for dcmpstat, adapted dcmprtsv
 *    and dcmpsprt applications.
 *
 *  Revision 1.68  1999/09/13 15:19:13  meichel
 *  Added implementations for a number of further print API methods.
 *
 *  Revision 1.67  1999/09/10 12:46:53  meichel
 *  Added implementations for a number of print API methods.
 *
 *  Revision 1.66  1999/09/10 09:36:28  joergr
 *  Added support for CIELAB display function. New methods to handle display
 *  functions. Old methods are marked as retired and should be removed asap.
 *
 *  Revision 1.64  1999/09/08 17:11:43  joergr
 *  Added support for new instance types in database (grayscale hardcopy and
 *  stored print).
 *
 *  Revision 1.63  1999/09/08 16:41:41  meichel
 *  Moved configuration file evaluation to separate class.
 *
 *  Revision 1.62  1999/09/01 16:15:06  meichel
 *  Added support for requested image size to print routines
 *
 *  Revision 1.61  1999/08/31 16:54:46  meichel
 *  Added new sample application that allows to create simple print jobs.
 *
 *  Revision 1.60  1999/08/31 14:02:08  meichel
 *  Added print related config file methods
 *
 *  Revision 1.59  1999/08/27 15:57:48  meichel
 *  Added methods for saving hardcopy images and stored print objects
 *    either in file or in the local database.
 *
 *  Revision 1.58  1999/07/22 16:39:54  meichel
 *  Adapted dcmpstat data structures and API to supplement 33 letter ballot text.
 *
 *  Revision 1.57  1999/07/14 12:03:43  meichel
 *  Updated data dictionary for supplement 29, 39, 33_lb, CP packet 4 and 5.
 *    Corrected dcmtk applications for changes in attribute name constants.
 *
 *  Revision 1.56  1999/05/05 14:26:21  joergr
 *  Modified parameter of CreateProcess call to avoid creation of new command
 *  line window under Windows.
 *  Added optional parameter to method loadPState (from database) to change
 *  instance reviewed flag for pstate and image.
 *
 *  Revision 1.55  1999/05/04 16:05:49  joergr
 *  Added releaseDatabase to savePState to avoid deadlocks.
 *  Change status of variable imageInDatabase in savePState to avoid unnecessary
 *  saving of (probabaly large) image files.
 *
 *  Revision 1.54  1999/05/04 10:53:08  meichel
 *  Added test for struct utimbuf declaration, absent on some platforms
 *
 *  Revision 1.53  1999/05/03 14:15:58  joergr
 *  Enhanced check in savePState() method whether image file is already stored
 *  in database.
 *
 *  Revision 1.52  1999/05/03 11:01:36  joergr
 *  Minor code purifications to keep Sun CC 2.0.1 quiet.
 *
 *  Revision 1.51  1999/04/29 15:26:14  joergr
 *  Added PresentationLabel to index file.
 *
 *  Revision 1.50  1999/04/28 17:00:17  joergr
 *  Removed additional declaration of local variable (hides first declaration)
 *  to avoid compiler warnings reported by gcc 2.7.2.1 (Linux).
 *
 *  Revision 1.49  1999/04/27 11:25:31  joergr
 *  Added new entry to index file: Presentation Description.
 *  Enhanced savePState() method: now image file is also added to index file
 *  and stored in image directory (if not already there).
 *
 *  Revision 1.48  1999/03/22 09:52:40  meichel
 *  Reworked data dictionary based on the 1998 DICOM edition and the latest
 *    supplement versions. Corrected dcmtk applications for minor changes
 *    in attribute name constants.
 *
 *  Revision 1.47  1999/03/03 13:29:33  joergr
 *  Added methods to get and set ambient light value (re: Barten transformation).
 *  Moved method 'isBartenTransformPossible()' from presentation state class to
 *  interface class.
 *
 *  Revision 1.46  1999/03/02 13:38:17  joergr
 *  Corrected typo (E_Normal instead of EC_Normal).
 *
 *  Revision 1.45  1999/03/02 13:02:20  joergr
 *  Added parameter to selectPState() specifying whether to change the review
 *  status of the loaded presentation state.
 *
 *  Revision 1.44  1999/02/27 16:59:20  joergr
 *  Changed implementation of deleteImageFile (imagectn method doesn't function
 *  under Window NT).
 *  Removed bug in createPStateCache (cache was reported invalid on second call).
 *  Modified method selectPState (image file is now implicitly loaded if
 *  necessary).
 *
 *  Revision 1.43  1999/02/25 18:44:08  joergr
 *  Renamed methods enable/disablePState().
 *  Performed some modifications in the implementation of enable/disablePState
 *  to avoid dmalloc warnings (not yet finished).
 *
 *  Revision 1.42  1999/02/24 20:23:05  joergr
 *  Added methods to get a list of presentation states referencing the
 *  currently selected image.
 *  Added support for exchanging current presentation state (load from file)
 *  without deleting the current image.
 *  Report an error when loading a presentation state and the referenced image
 *  file is absent.
 *  Removed bug concerning newInstancesReceived (Windows NT behaves different to
 *  Unix when closing/unlocking a file).
 *
 *  Revision 1.41  1999/02/23 11:45:24  joergr
 *  Added check whether new instances have been received before resetting
 *  database reference time (affects delete and instance reviewed methods).
 *
 *  Revision 1.40  1999/02/22 14:21:59  joergr
 *  Added deletion of image files (depending on directory where the file is
 *  stored).
 *  Reset reference time for file modification checking after the index file
 *  has been changed internally (delete and change status methods).
 *  Removed debug messages when creating and clearing index cache.
 *
 *  Revision 1.39  1999/02/19 19:15:21  joergr
 *  Corrected bug in disablePresentationState().
 *
 *  Revision 1.38  1999/02/19 19:03:04  joergr
 *  Added methods to disable and (re-)enable PresentationStates.
 *  Added (private) helper methods to reduce redundant lines of code.
 *  Removed bug concerning method newInstancesReceived (databaseFilename was
 *  never set).
 *  Implemented main part of delete methods (image files are not yet deleted).
 *  Removed implicit application of a shared lock to the database file when
 *  unlock an exclusive lock.
 *
 *  Revision 1.37  1999/02/19 09:48:27  joergr
 *  Added method getFilename() to get filename of currently selected instance.
 *  Modified implementation of instanceReviewed.
 *
 *  Revision 1.36  1999/02/18 18:48:01  joergr
 *  Re-implemented methods to access index file (delete methods are still
 *  missing).
 *  Removed parameter 'deletefile' from delete methods. This parameter is
 *  not necessary because the decision whether a images file is deleted only
 *  depends on the directory where the file is stored (see comments).
 *
 *  Revision 1.35  1999/02/18 11:07:28  meichel
 *  Added new parameter explicitVR to interface methods savePState,
 *    saveDICOMImage.  Allows to choose between explicit VR and implicit VR
 *    little endian format.  Added new method saveCurrentImage that allows to
 *    save the current image to file.
 *
 *  Revision 1.34  1999/02/17 12:46:10  vorwerk
 *  bug fixed in strippidxarray.
 *
 *  Revision 1.33  1999/02/17 10:05:33  meichel
 *  Moved creation of Display Function object from DVPresentationState to
 *    DVInterface to avoid unnecessary re-reads.
 *
 *  Revision 1.32  1999/02/16 16:36:08  meichel
 *  Added method newInstancesReceived() to DVInterface class.
 *
 *  Revision 1.31  1999/02/12 10:04:13  vorwerk
 *  added cache , changed delete methods.
 *
 *  Revision 1.30  1999/02/10 16:01:40  meichel
 *  Fixed memory leak in dviface.cc - Config file contents were never deleted.
 *
 *  Revision 1.29  1999/02/09 15:58:10  meichel
 *  Implemented methods that save images and presentation states in the DB.
 *
 *  Revision 1.28  1999/02/08 10:52:35  meichel
 *  Updated documentation of dviface.h in Doc++ style.
 *    Removed dummy parameter from constructor.
 *
 *  Revision 1.27  1999/02/05 17:45:37  meichel
 *  Added config file entry for monitor characteristics file.  Monitor charac-
 *    teristics are passed to dcmimage if present to activate Barten transform.
 *
 *  Revision 1.26  1999/02/05 12:45:12  vorwerk
 *  actualised version: comments see previous version.
 *
 *  Revision 1.24  1999/01/29 16:01:02  meichel
 *  Reworked index file handle acquisition and locking code.
 *
 *  Revision 1.23  1999/01/29 09:51:28  vorwerk
 *  locking bug removed.
 *
 *  Revision 1.22  1999/01/28 15:27:23  vorwerk
 *  Exclusive and shared locking mechanism in all browser-methods added.
 *
 *  Revision 1.21  1999/01/27 15:31:28  vorwerk
 *  record deletion bug removed. Errorhandling for indexfiles with length zero added.
 *
 *  Revision 1.20  1999/01/27 14:59:26  meichel
 *  Implemented DICOM network receive application "dcmpsrcv" which receives
 *    images and presentation states and stores them in the local database.
 *
 *  Revision 1.19  1999/01/25 18:18:22  meichel
 *  Defined private SOP class UID for network receiver
 *    shutdown function. Cleanup up some code.
 *
 *  Revision 1.18  1999/01/25 16:48:37  vorwerk
 *  Bug in getaninstance removed. getNumberOfSeries and getNumberOfInstances results
 *  are correct now. getStudyStatus bug removed. Error handling routines added.
 *
 *  Revision 1.17  1999/01/25 13:05:57  meichel
 *  Implemented DVInterface::startReceiver()
 *    and several config file related methods.
 *
 *  Revision 1.16  1999/01/20 19:25:30  meichel
 *  Implemented sendIOD method which creates a separate process for trans-
 *    mitting images from the local database to a remote communication peer.
 *
 *  Revision 1.15  1999/01/19 15:13:41  vorwerk
 *  Additional methods for attributes in the indexfile added.
 *  Method getFilename implemented.
 *
 *  Revision 1.14  1999/01/18 17:30:48  meichel
 *  minor syntax purifications to keep VC++ happy
 *
 *  Revision 1.13  1999/01/18 16:15:17  vorwerk
 *  Bug in isPresentationstateSeries() corrected.
 *
 *  Revision 1.12  1999/01/15 17:27:17  meichel
 *  added DVInterface method resetPresentationState() which allows to reset a
 *    presentation state to the initial state (after loading).
 *
 *  Revision 1.11  1999/01/14 17:50:27  meichel
 *  added new method saveDICOMImage() to class DVInterface.
 *    Allows to store a bitmap as a DICOM image.
 *
 *  Revision 1.10  1999/01/14 16:19:46  vorwerk
 *  getNumberOfSeries bug corrected. Error handling in deletion and reviewed methods added.
 *
 *  Revision 1.8  1999/01/11 10:10:18  vorwerk
 *  error handling for getNumberofStudies and selectStudy implemented.
 *
 *  Revision 1.7  1999/01/07 16:40:04  vorwerk
 *  getSeriesDescription implemented
 *
 *  Revision 1.5  1999/01/04 13:28:11  vorwerk
 *  line inserted
 *
 *  Revision 1.4  1999/01/04 13:10:30  vorwerk
 *  getSeriesPerformingPhysicainsName() changed in getSeriesPerformingPhysiciansName()
 *
 *  Revision 1.3  1998/12/22 17:57:13  meichel
 *  Implemented Presentation State interface for overlays,
 *    VOI LUTs, VOI windows, curves. Added test program that
 *    allows to add curve data to DICOM images.
 *
 *  Revision 1.2  1998/12/22 15:52:45  vorwerk
 *  - browser methods implemented
 *  - methods added for index.dat
 *
 *  Revision 1.1  1998/11/27 14:50:38  meichel
 *  Initial Release.
 *
 *
 */
