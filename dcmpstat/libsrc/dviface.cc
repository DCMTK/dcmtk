/*
 *
 *  Copyright (C) 1998-99, OFFIS
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
 *  Module:  dcmpstat
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: DVPresentationState
 *
 *  Last Update:      $Author: meichel $
 *  Update Date:      $Date: 1998-11-27 14:50:38 $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dviface.h"
#include "dvpstat.h"


DVInterface::DVInterface()
  : pState()
{
}


DVInterface::DVInterface(const DVInterface& obj)
  : pState(obj.pState)
{
}


DVInterface::~DVInterface()
{
}


E_Condition DVInterface::loadImage(const char *studyUID,
                                   const char *seriesUID,
                                   const char *instanceUID)
{
    return EC_IllegalCall;
}


E_Condition DVInterface::loadImage(const char *imgName)
{
    E_Condition status = EC_IllegalCall;
    DcmFileFormat *image = NULL;
    if ((status = loadFileFormat(imgName, image)) == EC_Normal)
    {
        if (image != NULL)
        {
            DcmDataset *dataset = image->getDataset();
            if (dataset != NULL)
            {
                if ((status = pState.createFromImage(*dataset)) == EC_Normal)
                    status = pState.attachImage(image);
            } else
                status = EC_CorruptedData;
        } else
            status = EC_IllegalCall;
    }
    return status;
}


E_Condition DVInterface::loadPState(const char *studyUID,
                                    const char *seriesUID,
                                    const char *instanceUID)
{
    return EC_IllegalCall;
}


E_Condition DVInterface::loadPState(const char *pstName,
                                    const char *imgName)
{
    E_Condition status = EC_IllegalCall;
    DcmFileFormat *pstate = NULL;
    DcmFileFormat *image = NULL;
    if (((status = loadFileFormat(pstName, pstate)) == EC_Normal) &&
        ((status = loadFileFormat(imgName, image)) == EC_Normal))
    {
        if ((pstate != NULL) && (image != NULL))
        {
            DcmDataset *dataset = pstate->getDataset();
            if (dataset != NULL)
            {
                if ((status = pState.read(*dataset)) == EC_Normal)
                    status = pState.attachImage(image);
            } else
                status = EC_MemoryExhausted;
        } else
            status = EC_IllegalCall;
    }
    return status;
}


E_Condition DVInterface::savePState()
{
    return EC_IllegalCall;
}


E_Condition DVInterface::savePState(const char *filename)
{
    E_Condition status = EC_IllegalCall;
    DcmDataset *dataset = new DcmDataset();
    if (dataset != NULL)
    {
        if ((status = pState.write(*dataset)) == EC_Normal)
        {
        	DcmFileFormat *fileformat = new DcmFileFormat(dataset);
        	if (fileformat != NULL)
                status = saveFileFormat(filename, fileformat);
            else
                status = EC_MemoryExhausted;
        }
    } else
        status = EC_MemoryExhausted;
    return status;
}


void DVInterface::clear()
{
    pState.clear();
}


DVPresentationState &DVInterface::getCurrentPState()
{
    return pState;
}


E_Condition DVInterface::lockDatabase()
{
    return EC_IllegalCall;
}


E_Condition DVInterface::unlockDatabase()
{
    return EC_IllegalCall;
}



Uint32 DVInterface::getNumberOfStudies()
{
    return 0;
}


E_Condition DVInterface::selectStudy(Uint32 idx)
{
    return EC_IllegalCall;
}


const char *DVInterface::getStudyUID()
{
    return NULL;
}


const char *DVInterface::getStudyDescription()
{
    return NULL;
}


DVInterface::DVIFhierarchyStatus DVInterface::getStudyStatus() const
{
    return DVIF_objectIsNew;
}


Uint32 DVInterface::getNumberOfSeries()
{
    return 0;
}


E_Condition DVInterface::selectSeries(Uint32 idx)
{
    return EC_IllegalCall;
}


const char *DVInterface::getSeriesUID()
{
    return NULL;
}


DVInterface::DVIFhierarchyStatus DVInterface::getSeriesStatus() const
{
    return DVIF_objectIsNew;
}


OFBool DVInterface::isPresentationStateSeries()
{
    return OFFalse;
}


Uint32 DVInterface::getNumberOfInstances()
{
    return 0;
}


E_Condition DVInterface::selectInstance(Uint32 idx)
{
    return EC_IllegalCall;
}


const char *DVInterface::getInstanceUID()
{
    return NULL;
}


const char *DVInterface::getModality()
{
    return NULL;
}


DVInterface::DVIFhierarchyStatus DVInterface::getInstanceStatus() const
{
    return DVIF_objectIsNew;
}


OFBool DVInterface::isPresentationState()
{
    return OFFalse;
}


E_Condition DVInterface::instanceReviewed(const char *studyUID,
                                          const char *seriesUID,
                                          const char *instanceUID)
{
    return EC_IllegalCall;
}


E_Condition DVInterface::deleteInstance(const char *studyUID,
                                        const char *seriesUID,
                                        const char *instanceUID)
{
    return EC_IllegalCall;
}


E_Condition DVInterface::deleteSeries(const char *studyUID,
                                      const char *seriesUID)
{
    return EC_IllegalCall;
}


E_Condition DVInterface::deleteStudy(const char *studyUID)
{
    return EC_IllegalCall;
}


E_Condition DVInterface::sendIOD(const char *targetID,
                                 const char *studyUID,
                                 const char *seriesUID,
                                 const char *instanceUID)
{
    return EC_IllegalCall;
}


Uint32 DVInterface::getNumberOfTargets()
{
    return 0;
}


const char *DVInterface::getTargetID(Uint32 idx)
{
    return NULL;
}


const char *DVInterface::getTargetDescription(Uint32 idx)
{
    return NULL;
}


const char *DVInterface::getTargetDescription(const char *targetID)
{
    return NULL;
}


E_Condition DVInterface::loadFileFormat(const char *filename,
                                        DcmFileFormat *&fileformat)
{
    DcmFileStream stream(filename, DCM_ReadMode);
    if (!stream.Fail())
    {
        fileformat = new DcmFileFormat;
        if (fileformat != NULL)
        {
            E_Condition status;
            fileformat->transferInit();
            if ((status = fileformat->read(stream)) == EC_Normal)
                fileformat->transferEnd();
            return status;
        }
        return EC_MemoryExhausted;
    }
    return stream.GetError();
}


E_Condition DVInterface::saveFileFormat(const char *filename,
                                        DcmFileFormat *fileformat)
{
    DcmFileStream stream(filename, DCM_WriteMode);
    if (!stream.Fail())
    {
        if (fileformat != NULL)
        {
            E_Condition status;
            fileformat->transferInit();
            status = fileformat->write(stream, EXS_LittleEndianExplicit, EET_ExplicitLength, EGL_recalcGL, EPD_withoutPadding);
            fileformat->transferEnd();
            return status;
        }
        return EC_IllegalCall;
    }
    return stream.GetError();
}


/*
 *  CVS/RCS Log:
 *  $Log: dviface.cc,v $
 *  Revision 1.1  1998-11-27 14:50:38  meichel
 *  Initial Release.
 *
 *
 */
