/*
 *
 *  Copyright (C) 1998-2019, OFFIS e.V.
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
#include "dcmtk/oflog/fileap.h"       /* for dcmtk::log4cplus::FileAppender */

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
#include <openssl/ssl.h>
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
            OFunique_ptr<dcmtk::log4cplus::Layout> layout(new dcmtk::log4cplus::PatternLayout(pattern));
            dcmtk::log4cplus::SharedAppenderPtr logfile(new dcmtk::log4cplus::FileAppender(filepath, STD_NAMESPACE ios::app));
            // We can't use OFLog::getLogger() here because that doesn't let us
            // configure the object
            dcmtk::log4cplus::Logger log = dcmtk::log4cplus::Logger::getInstance("dcmtk.dcmpstat.logfile");

            logfile->setLayout(OFmove(layout));
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
    if (!databaseIndexFile.empty())
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

    if (dbhandle.makeNewStoreFileName(UID_GrayscaleSoftcopyPresentationStateStorage, instanceUID, imageFileName, sizeof(imageFileName)).good())
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
                DCMPSTAT_WARN("Unable to register presentation state '" << imageFileName << "' in database");
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
            if (DU_getStringDOElement(dset, DCM_SOPClassUID, sopClass, sizeof(sopClass)) &&
                    DU_getStringDOElement(dset, DCM_SOPInstanceUID, instanceUID2, sizeof(instanceUID2)) &&
                    DU_getStringDOElement(dset, DCM_SeriesInstanceUID, seriesUID, sizeof(seriesUID)) &&
                    DU_getStringDOElement(dset, DCM_StudyInstanceUID, studyUID, sizeof(studyUID)) &&
                ((!imageInDatabase) || (getSeriesStruct(studyUID, seriesUID, instanceUID2) == NULL)))
            {
                releaseDatabase();   /* avoid deadlocks */
                if (dbhandle.makeNewStoreFileName(sopClass, instanceUID2, imageFileName, sizeof(imageFileName)).good())
                {
                    // now store presentation state as filename
                    result = saveCurrentImage(imageFileName);
                    if (EC_Normal==result)
                    {
                        if (dbhandle.storeRequest(sopClass, instanceUID2, imageFileName, &dbStatus).bad())
                        {
                            result = EC_IllegalCall;
                            DCMPSTAT_LOGFILE("Save presentation state to database failed: could not register image in index file");
                            DCMPSTAT_WARN("Unable to register image '" << imageFileName << "' in database");
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
    if (pReport->getSOPClassUID(sopClassUID).bad() || sopClassUID.empty())
        return EC_IllegalCall;
    OFString instanceUID;
    if (pReport->getSOPInstanceUID(instanceUID).bad() || instanceUID.empty())
        return EC_IllegalCall;

    DcmQueryRetrieveDatabaseStatus dbStatus(STATUS_Success);
    char filename[MAXPATHLEN + 1];
    OFCondition result = EC_Normal;

    DcmQueryRetrieveIndexDatabaseHandle dbhandle(getDatabaseFolder(), PSTAT_MAXSTUDYCOUNT, PSTAT_STUDYSIZE, result);
    if (result.bad())
    {
        DCMPSTAT_LOGFILE("Save structured report to database failed: could not lock index file");
        return EC_IllegalCall;
    }

    if (dbhandle.makeNewStoreFileName(sopClassUID.c_str(), instanceUID.c_str(), filename, sizeof(filename)).good())
    {
        // now store presentation state as filename
        result = saveStructuredReport(filename);
        if (EC_Normal == result)
        {
            if (dbhandle.storeRequest(sopClassUID.c_str(), instanceUID.c_str(), filename, &dbStatus).bad())
            {
                result = EC_IllegalCall;
                DCMPSTAT_LOGFILE("Save structured report to database failed: could not register in index file");
                DCMPSTAT_WARN("Unable to register structured report '" << filename << "' in database");
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
            return OFstatic_cast(Uint32, instance->List.size());
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
            if (databaseIndexFile.empty())
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
  if (databaseIndexFile.empty())
  {
    if (pHandle == NULL)
    {
      lockDatabase(); // derives databaseIndexFile
      releaseDatabase();
    }
  }

  if (!databaseIndexFile.empty())
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
                            if (DSRTypes::sopClassUIDToDocumentType(record.SOPClassUID) != DSRTypes::DT_invalid)
                                type = DVPSI_structuredReport;
                            else if (strcmp(record.Modality, "PR") == 0)
                                type = DVPSI_presentationState;
                            else if (strcmp(record.Modality, "SR") == 0)
                                type = DVPSI_structuredReport;
                            else if (strcmp(record.Modality, "HC") == 0)
                                type =DVPSI_hardcopyGrayscale;
                            else if (strcmp(record.Modality, "STORED_PRINT") == 0)
                                type = DVPSI_storedPrint;
                            series->List.addItem(record.SOPInstanceUID,
                                                 counter,
                                                 OFstatic_cast(DVIFhierarchyStatus, record.hstat),
                                                 type,
                                                 record.ImageSize,
                                                 record.filename);
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
                                else if (strcmp(sopClassUID, idxRec.SOPClassUID) == 0)
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
    pHandle->DB_unlock();
    OFCondition result = pHandle->instanceReviewed(pos);
    pHandle->DB_lock(OFFalse);
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
        int i = 0;
        for (i = 0; i < PSTAT_MAXSTUDYCOUNT; i++)
        {
            if (strcmp(uid, study[i].StudyInstanceUID) == 0)
                return i;
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
                    int i = 0;
                    for (i = 0; i < PSTAT_MAXSTUDYCOUNT; i++)
                    {
                        if (strcmp(studyUID, study_desc[i].StudyInstanceUID) != 0)
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
  if (configPath.empty()) return EC_IllegalCall;

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
  if (configPath.empty()) return EC_IllegalCall;

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
  if (configPath.empty()) return EC_IllegalCall;

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
  if (configPath.empty()) return EC_IllegalCall;

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
  if (configPath.empty()) return EC_IllegalCall;

  OFStandard::initializeNetwork();

  OFCondition result = EC_Normal;
  T_ASC_Network *net=NULL;
  T_ASC_Parameters *params=NULL;
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
      sprintf(peerHost, "localhost:%d", OFstatic_cast(int, getQueryRetrievePort()));
      ASC_setPresentationAddresses(params, OFStandard::getHostName().c_str(), peerHost);

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

  OFStandard::shutdownNetwork();

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

  if (handle.makeNewStoreFileName(UID_SecondaryCaptureImageStorage, uid, imageFileName, sizeof(imageFileName)).good())
  {
     // now store presentation state as filename
     result = saveDICOMImage(imageFileName, pixelData, width, height, aspectRatio, OFTrue, uid);
     if (EC_Normal==result)
     {
       if (handle.storeRequest(UID_SecondaryCaptureImageStorage, uid, imageFileName, &dbStatus).bad())
       {
         result = EC_IllegalCall;
         DCMPSTAT_LOGFILE("Save image to database failed: could not register in index file");
         DCMPSTAT_WARN("Unable to register secondary capture image '" << imageFileName << "' in database");
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

  if (handle.makeNewStoreFileName(UID_RETIRED_HardcopyGrayscaleImageStorage, uid, imageFileName, sizeof(imageFileName)).good())
  {
     result = saveHardcopyGrayscaleImage(imageFileName, pixelData, width, height, aspectRatio, OFTrue, uid);
     if (EC_Normal==result)
     {
       if (handle.storeRequest(UID_RETIRED_HardcopyGrayscaleImageStorage, uid, imageFileName, &dbStatus).bad())
       {
         result = EC_IllegalCall;
         DCMPSTAT_LOGFILE("Save hardcopy grayscale image to database failed: could not register in index file");
         DCMPSTAT_WARN("Unable to register hardcopy grayscale image '" << imageFileName << "' in database");
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

  if (handle.makeNewStoreFileName(classUID, instanceUID, imageFileName, sizeof(imageFileName)).good())
  {
     // save image file
     result = DVPSHelper::saveFileFormat(imageFileName, &fileformat, OFTrue);
     if (EC_Normal==result)
     {
       if (handle.storeRequest(classUID, instanceUID, imageFileName, &dbStatus).bad())
       {
         result = EC_IllegalCall;
         DCMPSTAT_LOGFILE("Save fileformat to database failed: could not register in index file");
         DCMPSTAT_WARN("Unable to register file '" << imageFileName << "' in database");
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

  if (handle.makeNewStoreFileName(UID_RETIRED_StoredPrintStorage, uid, imageFileName, sizeof(imageFileName)).good())
  {
     // now store stored print object as filename
     result = saveStoredPrint(imageFileName, writeRequestedImageSize, OFTrue, uid);
     if (EC_Normal==result)
     {
       if (handle.storeRequest(UID_RETIRED_StoredPrintStorage, uid, imageFileName, &dbStatus).bad())
       {
         result = EC_IllegalCall;
         DCMPSTAT_LOGFILE("Save stored print to database failed: could not register in index file");
         DCMPSTAT_WARN("Unable to register stored print object '" << imageFileName << "' in database");
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
                if (displayFunction[DVPSD_GSDF] != NULL)
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
  if (configPath.empty()) return EC_IllegalCall;

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
  if (configPath.empty()) return EC_IllegalCall;

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
  if (configPath.empty()) return EC_IllegalCall;

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
  if (configPath.empty()) return EC_IllegalCall;

  OFStandard::initializeNetwork();

  OFCondition result = EC_Normal;
  T_ASC_Network *net=NULL;
  T_ASC_Parameters *params=NULL;
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
  DcmKeyFileFormat keyFileFormat = DCF_Filetype_PEM;
  if (! getTLSPEMFormat()) keyFileFormat = DCF_Filetype_ASN1;
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


          DcmTLSTransportLayer *tLayer = new DcmTLSTransportLayer(NET_REQUESTOR, tlsRandomSeedFile.c_str(), OFTrue);
          if (tLayer)
          {
            if (tlsCACertificateFolder) tLayer->addTrustedCertificateDir(tlsCACertificateFolder, keyFileFormat);
            if (tlsDHParametersFile.size() > 0) tLayer->setTempDHParameters(tlsDHParametersFile.c_str());
            tLayer->setPrivateKeyPasswd(tlsPrivateKeyPassword); // never prompt on console
            tLayer->setPrivateKeyFile(tlsPrivateKeyFile.c_str(), keyFileFormat);
            tLayer->setCertificateFile(tlsCertificateFile.c_str(), keyFileFormat);
            tLayer->setCertificateVerification(DCV_ignoreCertificate);

           // determine TLS profile
             OFString profileName;
            const char *profileNamePtr = getTargetTLSProfile(target);
            if (profileNamePtr) profileName = profileNamePtr;
            DcmTLSSecurityProfile tlsProfile = TSP_Profile_BCP195;  // default
            if (profileName == "BCP195-ND") tlsProfile = TSP_Profile_BCP195_ND;
            else if (profileName == "BCP195-EX") tlsProfile = TSP_Profile_BCP195_Extended;
            else if (profileName == "BCP195") tlsProfile = TSP_Profile_BCP195;
            else if (profileName == "AES") tlsProfile = TSP_Profile_AES;
            else if (profileName == "BASIC") tlsProfile = TSP_Profile_Basic;
            else if (profileName == "NULL") tlsProfile = TSP_Profile_IHE_ATNA_Unencrypted;

            // set TLS profile
            (void) tLayer->setTLSProfile(tlsProfile);

            // activate cipher suites
            (void) tLayer->activateCipherSuites();

            ASC_setTransportLayer(net, tLayer, 1);
          }
#else
          // we cannot shutdown a TLS process since we're compiled without OpenSSL support
          cond = EC_IllegalCall;
#endif
        }

        ASC_setAPTitles(params, getNetworkAETitle(), getTargetAETitle(target), NULL);
        sprintf(peerHost, "%s:%d", getTargetHostname(target), OFstatic_cast(int, getTargetPort(target)));
        ASC_setPresentationAddresses(params, OFStandard::getHostName().c_str(), peerHost);

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

  OFStandard::shutdownNetwork();

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
  if ((studyUID==NULL)||(seriesUID==NULL)||(instanceUID==NULL)||configPath.empty()) return EC_IllegalCall;
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
  OFString *password = OFstatic_cast(OFString *, userdata);
  int passwordSize = OFstatic_cast(int, password->length());
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
    BIO *in = BIO_new(BIO_s_file());
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
