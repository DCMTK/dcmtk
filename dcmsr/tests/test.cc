#include "osconfig.h"

#include "dsrdoc.h"

#include "dcuid.h"
#include "dcfilefo.h"

#include <iostream.h>


#define WRITE
#define USE_PTR


E_Condition loadFileFormat(const char *filename,
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


E_Condition saveFileFormat(const char *filename, DcmFileFormat *fileformat)
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


int main()
{
    DSRDocument *doc = new DSRDocument(DSRTypes::DT_EnhancedSR);
    if (doc != NULL)
    {
        doc->setLogStream(&ofConsole);
#ifdef WRITE        
        OFString string;
        
        doc->setPatientsName("Gates^William^H");

        doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
        doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1111", "TEST", "Diagnosis"));
        DSRTypes::currentDateTime(string);
        doc->getTree().getCurrentContentItem().setObservationDateTime(string);

        doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container, DSRTypes::AM_belowCurrent);
        doc->getTree().getCurrentContentItem().setContinuityOfContent(DSRTypes::COC_Continuous);

        doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
        doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1234", OFFIS_CODING_SCHEME_DESIGNATOR, "Text Code"));
        doc->getTree().getCurrentContentItem().setStringValue("A mass of");

        doc->getTree().addContentItem(DSRTypes::RT_hasConceptMod, DSRTypes::VT_Code, DSRTypes::AM_belowCurrent);
        doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1234", OFFIS_CODING_SCHEME_DESIGNATOR, "Code"));
        doc->getTree().getCurrentContentItem().setCodeValue(DSRCodedEntryValue("2222", OFFIS_CODING_SCHEME_DESIGNATOR, "Sample Code 1"));

        doc->getTree().addContentItem(DSRTypes::RT_hasConceptMod, DSRTypes::VT_Code);
        doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1234", OFFIS_CODING_SCHEME_DESIGNATOR, "Code"));
        doc->getTree().getCurrentContentItem().setCodeValue(DSRCodedEntryValue("2222", OFFIS_CODING_SCHEME_DESIGNATOR, "Sample Code 2"));

        doc->getTree().goUp();

        doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Num);
        doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1234", OFFIS_CODING_SCHEME_DESIGNATOR, "Diameter"));
        doc->getTree().getCurrentContentItem().setNumericValue(DSRNumericMeasurementValue("3", DSRCodedEntryValue("cm", OFFIS_CODING_SCHEME_DESIGNATOR, "Length Unit")));

        doc->getTree().addContentItem(DSRTypes::RT_hasConceptMod, DSRTypes::VT_Code, DSRTypes::AM_belowCurrent);
        doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1234", OFFIS_CODING_SCHEME_DESIGNATOR, "Code"));
        doc->getTree().getCurrentContentItem().setCodeValue(DSRCodedEntryValue("2222", OFFIS_CODING_SCHEME_DESIGNATOR, "Sample Code"));

        doc->getTree().goUp();

        doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
        doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1234", OFFIS_CODING_SCHEME_DESIGNATOR, "Text Code"));
        doc->getTree().getCurrentContentItem().setStringValue("was detected.");

        doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container);

        doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
        doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1234", OFFIS_CODING_SCHEME_DESIGNATOR, "Text Code"));
        doc->getTree().getCurrentContentItem().setStringValue("A mass of");

        doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Num);
        doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1234", OFFIS_CODING_SCHEME_DESIGNATOR, "Diameter"));
/*
        doc->getTree().getCurrentContentItem().setNumericValue(DSRNumericMeasurementValue("3", DSRCodedEntryValue("cm", OFFIS_CODING_SCHEME_DESIGNATOR, "Length Unit")));
*/
        doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
        doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1234", OFFIS_CODING_SCHEME_DESIGNATOR, "Text Code"));
        doc->getTree().getCurrentContentItem().setStringValue("was detected.");

        doc->getTree().goUp();
        doc->getTree().goUp();

        doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
        doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1234", OFFIS_CODING_SCHEME_DESIGNATOR, "Code"));
        doc->getTree().getCurrentContentItem().setStringValue("Sample Text");

        doc->getTree().addContentItem(DSRTypes::RT_inferredFrom, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
        doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1234", OFFIS_CODING_SCHEME_DESIGNATOR, "Code"));
        doc->getTree().getCurrentContentItem().setStringValue("Inferred Sample Text");

        doc->getTree().addContentItem(DSRTypes::RT_hasProperties, DSRTypes::VT_SCoord);
        doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1234", OFFIS_CODING_SCHEME_DESIGNATOR, "SCoord Code"));
#ifdef USE_PTR
        DSRSpatialCoordinatesValue *scoordPtr = doc->getTree().getCurrentContentItem().getSpatialCoordinatesPtr();
        if (scoordPtr != NULL)
        {
            scoordPtr->setGraphicType(DSRTypes::GT_Circle);
            scoordPtr->getGraphicDataList().addItem(0, 0);
            scoordPtr->getGraphicDataList().addItem(255, 255);
        }
#else
        DSRSpatialCoordinatesValue spatialCoord(DSRTypes::GT_Circle);
        spatialCoord.getGraphicDataList().addItem(0, 0);
        spatialCoord.getGraphicDataList().addItem(255, 255);
        doc->getTree().getCurrentContentItem().setSpatialCoordinates(spatialCoord);
#endif

        doc->getTree().goUp();

        doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Composite);
        doc->getTree().getCurrentContentItem().setCompositeReference(DSRReferenceValue(UID_BasicTextSR, "9.8.7.6"));

        doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image);
#ifdef USE_PTR
        DSRImageReferenceValue *imagePtr = doc->getTree().getCurrentContentItem().getImageReferencePtr();
        if (imagePtr != NULL)
        {
            imagePtr->setValue(DSRImageReferenceValue(UID_CTImageStorage, "1.2.3.4.5.0", UID_GrayscaleSoftcopyPresentationStateStorage, "1.2.3.5.6.7"));
            imagePtr->getFrameList().addItem(5);
            imagePtr->getFrameList().addItem(2);
            imagePtr->getFrameList().addOnlyNewItem(5);
        }
#else
        DSRImageReferenceValue imageRef(UID_CTImageStorage, "1.2.3.4.5.0", UID_GrayscaleSoftcopyPresentationStateStorage, "1.2.3.5.6.7");
        imageRef.getFrameList().addItem(5);
        imageRef.getFrameList().addItem(2);
        imageRef.getFrameList().addOnlyNewItem(5);
        doc->getTree().getCurrentContentItem().setImageReference(imageRef);
#endif

        doc->getTree().addContentItem(DSRTypes::RT_hasConceptMod, DSRTypes::VT_Code, DSRTypes::AM_belowCurrent);
        doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1234", OFFIS_CODING_SCHEME_DESIGNATOR, "Code"));
        doc->getTree().getCurrentContentItem().setCodeValue(DSRCodedEntryValue("2222", OFFIS_CODING_SCHEME_DESIGNATOR, "Sample Code"));

        doc->getTree().goUp();

        doc->getTree().addContentItem(DSRTypes::RT_hasConceptMod, DSRTypes::VT_Code, DSRTypes::AM_belowCurrent);
        doc->getTree().getCurrentContentItem().setCodeValue(DSRCodedEntryValue("2222", OFFIS_CODING_SCHEME_DESIGNATOR, "Sample Code 2"));
        doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1234", OFFIS_CODING_SCHEME_DESIGNATOR, "Code"));

        doc->getTree().goUp();

        doc->getTree().addContentItem(DSRTypes::RT_hasConceptMod, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
        doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1234", OFFIS_CODING_SCHEME_DESIGNATOR, "Code"));
        doc->getTree().getCurrentContentItem().setStringValue("Sample Text 2");

        doc->getTree().addContentItem(DSRTypes::RT_hasProperties, DSRTypes::VT_Image, DSRTypes::AM_belowCurrent);
        doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1234", OFFIS_CODING_SCHEME_DESIGNATOR, "Key Image"));
        doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue(UID_CTImageStorage, "1.2.3.4"));
        doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue(UID_MRImageStorage, "1.2.3.4.0.1"));

        doc->getTree().addContentItem(DSRTypes::RT_hasProperties, DSRTypes::VT_Waveform);
#ifdef USE_PTR
        DSRWaveformReferenceValue *wavePtr = doc->getTree().getCurrentContentItem().getWaveformReferencePtr();
        if (wavePtr != NULL)
        {
            wavePtr->setValue(DSRWaveformReferenceValue(UID_HemodynamicWaveformStorage, "1.2.3.4.5"));
            wavePtr->getChannelList().addItem(5, 3);
            wavePtr->getChannelList().addItem(2, 0);
        }
#else
        DSRWaveformReferenceValue waveformRef();
        waveformRef.getChannelList().addItem(5, 3);
        waveformRef.getChannelList().addItem(2, 0);
        doc->getTree().getCurrentContentItem().setWaveformReference(waveformRef);
#endif

//        doc->getTree().gotoNode(5);
//        doc->getTree().removeCurrentContentItem();

        doc->createRevisedVersion();
        doc->createRevisedVersion();

        doc->verifyDocument("Riesmeier^Joerg", DSRCodedEntryValue("1705", OFFIS_CODING_SCHEME_DESIGNATOR, "JR"), "OFFIS e.V.");
        doc->verifyDocument("Schroeder^Gerhard", "Bundeskanzler");

        doc->completeDocument("This document is completed!");

        doc->print(cout);
        
        DcmFileFormat *fileformat = new DcmFileFormat();
        DcmDataset *dataset = NULL;
        if (fileformat)
            dataset = fileformat->getDataset();
        if (dataset)
        {
            if (doc->write(*dataset) == EC_Normal)
                saveFileFormat("test.dcm", fileformat);
            else
                cerr << "TEST ERROR: could not write SR document into dataset" << endl;
        }
#else
        DcmFileFormat *fileformat;
        if (loadFileFormat("test.dcm", fileformat) == EC_Normal)
        {
            DcmDataset *dataset = fileformat->getDataset();
            if (dataset != NULL)
            {
                if (doc->read(*dataset) == EC_Normal)
                    doc->print(cout);
                else
                    cerr << "TEST ERROR: could not read SR document from dataset" << endl;
             }
        } else
           cerr << "TEST ERROR: could not read fileformat" << endl;
#endif
    }
    delete doc;

    return 0;
}
