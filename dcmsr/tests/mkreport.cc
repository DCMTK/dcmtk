#include "osconfig.h"

#include "dsrdoc.h"

#include "dcuid.h"
#include "dcfilefo.h"

#include <iostream.h>


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


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << "mkreport: Create DICOM SR documents" << endl;
        cout << "ki = IHE Year 2 key image note (empty)" << endl;
        cout << "si = IHE Year 2 simple image report (empty)" << endl;
        cout << "01 = Consultation Report (text only)" << endl;
        cout << "02 = Same as 01 but with NUM and PNAME items" << endl;
        cout << "03 = Very short report (text only)" << endl;
        cout << "04 = Text report with several sections (history)" << endl;
        cout << "05 = Text report with several blocks (discharge)" << endl;
        cout << "06 = Radilogy report with image reference (dentist)" << endl;
    } else {
        DSRDocument *doc = new DSRDocument();
        if (doc != NULL)
        {
            OFBool writeFile = OFTrue;
            doc->setLogStream(&ofConsole);
            for (int i = 1; i < argc; i++)
            {
                writeFile = OFTrue;
                if (strcmp(argv[i], "ki") == 0)
                {
                    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
                    doc->setSpecificCharacterSetType(DSRTypes::CS_Latin1);

                    doc->setPatientsName("IHE^Year 2^Key^Image^Note");
                    doc->setPatientsSex("O");
                    doc->setManufacturer("Kuratorium OFFIS e.V.");
                    doc->setReferringPhysiciansName("Riesmeier^Jörg");

                    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("0000", OFFIS_CODING_SCHEME_DESIGNATOR, "Document Title"));

                    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Code, DSRTypes::AM_belowCurrent);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1000", OFFIS_CODING_SCHEME_DESIGNATOR, "Observation Context Mode"));
                    doc->getTree().getCurrentContentItem().setCodeValue(DSRCodedEntryValue("1010", OFFIS_CODING_SCHEME_DESIGNATOR, "DIRECT"));
                    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_PName);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1001", OFFIS_CODING_SCHEME_DESIGNATOR, "Recording Observer's Name"));
                    doc->getTree().getCurrentContentItem().setStringValue("Enter text");
                    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1002", OFFIS_CODING_SCHEME_DESIGNATOR, "Recording Observer's Organization Name"));
                    doc->getTree().getCurrentContentItem().setStringValue("Enter text");
                    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Code);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1003", OFFIS_CODING_SCHEME_DESIGNATOR, "Observation Context Mode"));
                    doc->getTree().getCurrentContentItem().setCodeValue(DSRCodedEntryValue("1030", OFFIS_CODING_SCHEME_DESIGNATOR, "PATIENT"));

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1100", OFFIS_CODING_SCHEME_DESIGNATOR, "Key Image Description"));
                    doc->getTree().getCurrentContentItem().setStringValue("Enter text");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1200", OFFIS_CODING_SCHEME_DESIGNATOR, "Image Reference"));
                    doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue("0", "0"));

                } else if (strcmp(argv[i], "si") == 0)
                {
                    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
                    doc->setSpecificCharacterSetType(DSRTypes::CS_Latin1);

                    doc->setPatientsName("IHE^Year 2^Simple^Image^Report");
                    doc->setPatientsSex("O");
                    doc->setManufacturer("Kuratorium OFFIS e.V.");
                    doc->setReferringPhysiciansName("Riesmeier^Jörg");

                    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("0000", OFFIS_CODING_SCHEME_DESIGNATOR, "Document Title"));

                    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Code, DSRTypes::AM_belowCurrent);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1000", OFFIS_CODING_SCHEME_DESIGNATOR, "Observation Context Mode"));
                    doc->getTree().getCurrentContentItem().setCodeValue(DSRCodedEntryValue("1010", OFFIS_CODING_SCHEME_DESIGNATOR, "DIRECT"));
                    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_PName);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1001", OFFIS_CODING_SCHEME_DESIGNATOR, "Recording Observer's Name"));
                    doc->getTree().getCurrentContentItem().setStringValue("Enter text");
                    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1002", OFFIS_CODING_SCHEME_DESIGNATOR, "Recording Observer's Organization Name"));
                    doc->getTree().getCurrentContentItem().setStringValue("Enter text");
                    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Code);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1003", OFFIS_CODING_SCHEME_DESIGNATOR, "Observation Context Mode"));
                    doc->getTree().getCurrentContentItem().setCodeValue(DSRCodedEntryValue("1030", OFFIS_CODING_SCHEME_DESIGNATOR, "PATIENT"));

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1110", OFFIS_CODING_SCHEME_DESIGNATOR, "Section Heading"));

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1120", OFFIS_CODING_SCHEME_DESIGNATOR, "Report Text"));
                    doc->getTree().getCurrentContentItem().setStringValue("Enter text");

                    doc->getTree().addContentItem(DSRTypes::RT_inferredFrom, DSRTypes::VT_Image, DSRTypes::AM_belowCurrent);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1200", OFFIS_CODING_SCHEME_DESIGNATOR, "Image Reference"));
                    doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue("0", "0"));

                    doc->getTree().goUp();

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1200", OFFIS_CODING_SCHEME_DESIGNATOR, "Image Reference"));
                    doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue("0", "0"));

                } else if (strcmp(argv[i], "01") == 0)
                {
                    doc->createNewDocument(DSRTypes::DT_BasicTextSR);

                    doc->setPatientsName("Osterman^Phillip^B.");
                    doc->setPatientsBirthDate("19220909");
                    doc->setPatientsSex("M");
                    doc->setReferringPhysiciansName("Fukuda^Katherine^M.");

                    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1111", OFFIS_CODING_SCHEME_DESIGNATOR, "Consultation Report"));

                    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_PName, DSRTypes::AM_belowCurrent);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1112", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Name"));
                    doc->getTree().getCurrentContentItem().setStringValue("Packer^David^M.");
                    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1113", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Organization Name"));
                    doc->getTree().getCurrentContentItem().setStringValue("Redlands Clinic");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2222", OFFIS_CODING_SCHEME_DESIGNATOR, "Description"));
                    doc->getTree().getCurrentContentItem().setStringValue("This 78-year-old gentleman referred by Dr. Fukuda was also seen by Dr. Mason at the Redlands Clinic. He has been seen in the past by Dr. Klugman.\nThe patient developed a lesion in the concha of the left external ear. Recent biopsy confirmed this as being a squamous cell carcinoma. The patient has had a few other skin cancers.\nOf most significant past history is the fact that this patient has a leukemia that has been treated in standard fashion by Dr. Klugman. The patient was then transferred to the Redlands Clinic and by some experimental protocol which, I guess, includes some sort of lymphocyte electrophoresis, has been placed into remission. He is not currently on any antileukemia drugs and has responded extremely well to his medical management.\nOn examination, the patient is healthy in general appearance. There is a 1.5 cm lesion on the concha of the ear, which is seen well on photograph of the left external ear. There are numerous soft lymph nodes in both sides of the neck, which I presume are related to his leukemia.");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2223", OFFIS_CODING_SCHEME_DESIGNATOR, "Diagnosis"));
                    doc->getTree().getCurrentContentItem().setStringValue("Squamous cell carcinoma, relatively superficial, involving the skin of the left external ear, which is seen well on photograph of the left external ear.");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2224", OFFIS_CODING_SCHEME_DESIGNATOR, "Treatment"));
                    doc->getTree().getCurrentContentItem().setStringValue("The plan of treatment is as follows: 4500 rad, 15 treatment sessions, using 100 kV radiation.\nThe reason for treatment, expected acute reaction, and remote possibility of complication was discussed with this patient at some length, and he accepted therapy as outlined.");

                } else if (strcmp(argv[i], "02") == 0)
                {
                    doc->createNewDocument(DSRTypes::DT_EnhancedSR);

                    doc->setPatientsName("Osterman^Phillip^B.");
                    doc->setPatientsBirthDate("19220909");
                    doc->setPatientsSex("M");
                    doc->setReferringPhysiciansName("Fukuda^Katherine^M.");

                    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1111", OFFIS_CODING_SCHEME_DESIGNATOR, "Consultation Report"));

                    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_PName, DSRTypes::AM_belowCurrent);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1112", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Name"));
                    doc->getTree().getCurrentContentItem().setStringValue("Packer^David^M.");
                    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1113", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Organization Name"));
                    doc->getTree().getCurrentContentItem().setStringValue("Redlands Clinic");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container);
                    doc->getTree().getCurrentContentItem().setContinuityOfContent(DSRTypes::COC_Continuous);

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2222", OFFIS_CODING_SCHEME_DESIGNATOR, "Description"));
                    doc->getTree().getCurrentContentItem().setStringValue("This 78-year-old gentleman referred by");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_PName);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2223", OFFIS_CODING_SCHEME_DESIGNATOR, "Referring Physician"));
                    doc->getTree().getCurrentContentItem().setStringValue("Dr. Fukuda");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2222", OFFIS_CODING_SCHEME_DESIGNATOR, "Description"));
                    doc->getTree().getCurrentContentItem().setStringValue("was also seen by");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_PName);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2224", OFFIS_CODING_SCHEME_DESIGNATOR, "Physician"));
                    doc->getTree().getCurrentContentItem().setStringValue("Dr. Mason");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2222", OFFIS_CODING_SCHEME_DESIGNATOR, "Description"));
                    doc->getTree().getCurrentContentItem().setStringValue("at the");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Code);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2225", OFFIS_CODING_SCHEME_DESIGNATOR, "Hospital Name"));
                    doc->getTree().getCurrentContentItem().setCodeValue(DSRCodedEntryValue("2230", OFFIS_CODING_SCHEME_DESIGNATOR, "Redlands Clinic"));

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2222", OFFIS_CODING_SCHEME_DESIGNATOR, "Description"));
                    doc->getTree().getCurrentContentItem().setStringValue(". He has been seen in the past by");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_PName);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2224", OFFIS_CODING_SCHEME_DESIGNATOR, "Physician"));
                    doc->getTree().getCurrentContentItem().setStringValue("Dr. Klugman");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2222", OFFIS_CODING_SCHEME_DESIGNATOR, "Description"));
                    doc->getTree().getCurrentContentItem().setStringValue(".\nThe patient developed a lesion in the concha of the left external ear. Recent biopsy confirmed this as being a squamous cell carcinoma. The patient has had a few other skin cancers.\nOf most significant past history is the fact that this patient has a leukemia that has been treated in standard fashion by");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_PName);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2224", OFFIS_CODING_SCHEME_DESIGNATOR, "Physician"));
                    doc->getTree().getCurrentContentItem().setStringValue("Dr. Klugman");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2222", OFFIS_CODING_SCHEME_DESIGNATOR, "Description"));
                    doc->getTree().getCurrentContentItem().setStringValue(". The patient was then transferred to the");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Code);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2225", OFFIS_CODING_SCHEME_DESIGNATOR, "Hospital Name"));
                    doc->getTree().getCurrentContentItem().setCodeValue(DSRCodedEntryValue("2230", OFFIS_CODING_SCHEME_DESIGNATOR, "Redlands Clinic"));

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2222", OFFIS_CODING_SCHEME_DESIGNATOR, "Description"));
                    doc->getTree().getCurrentContentItem().setStringValue("and by some experimental protocol which, I guess, includes some sort of lymphocyte electrophoresis, has been placed into remission. He is not currently on any antileukemia drugs and has responded extremely well to his medical management.\nOn examination, the patient is healthy in general appearance. There is a");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Num);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("3333", OFFIS_CODING_SCHEME_DESIGNATOR, "Diameter"));
                    doc->getTree().getCurrentContentItem().setNumericValue(DSRNumericMeasurementValue("1.5", DSRCodedEntryValue("cm", "UCUM", "1.4", "centimeter")));

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2222", OFFIS_CODING_SCHEME_DESIGNATOR, "Description"));
                    doc->getTree().getCurrentContentItem().setStringValue("lesion on the concha of the ear, which is seen well on photograph of the left external ear. There are numerous soft lymph nodes in both sides of the neck, which I presume are related to his leukemia.");

                    doc->getTree().goUp();

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2223", OFFIS_CODING_SCHEME_DESIGNATOR, "Diagnosis"));
                    doc->getTree().getCurrentContentItem().setStringValue("Squamous cell carcinoma, relatively superficial, involving the skin of the left external ear, which is seen well on photograph of the left external ear.");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2224", OFFIS_CODING_SCHEME_DESIGNATOR, "Treatment"));
                    doc->getTree().getCurrentContentItem().setStringValue("The plan of treatment is as follows: 4500 rad, 15 treatment sessions, using 100 kV radiation.\nThe reason for treatment, expected acute reaction, and remote possibility of complication was discussed with this patient at some length, and he accepted therapy as outlined.");

                } else if (strcmp(argv[i], "03") == 0)
                {
                    doc->createNewDocument(DSRTypes::DT_BasicTextSR);

                    doc->setPatientsName("Silverman^Elaine^J.");
                    doc->setPatientsBirthDate("19811010");
                    doc->setPatientsSex("F");
                    doc->setReferringPhysiciansName("Cooper^Harold^B.");

                    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1111", OFFIS_CODING_SCHEME_DESIGNATOR, "Radiology Report"));

                    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_PName, DSRTypes::AM_belowCurrent);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1112", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Name"));
                    doc->getTree().getCurrentContentItem().setStringValue("Skinner^Marian^B.");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container, DSRTypes::AM_belowCurrent);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1111", OFFIS_CODING_SCHEME_DESIGNATOR, "Findings"));

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2222", OFFIS_CODING_SCHEME_DESIGNATOR, "PA Chest"));
                    doc->getTree().getCurrentContentItem().setStringValue("Upright PA view of the chest shows the lung fields are clear, without evidence of an active process. Heart size in normal.");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2223", OFFIS_CODING_SCHEME_DESIGNATOR, "Impression"));
                    doc->getTree().getCurrentContentItem().setStringValue("Negative chest.");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2224", OFFIS_CODING_SCHEME_DESIGNATOR, "Abdomen"));
                    doc->getTree().getCurrentContentItem().setStringValue("Flat and upright views of the abdomen show a normal gas pattern without evidence of obstruction or ileus. There are no calcifications or abnormal masses noted.");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2225", OFFIS_CODING_SCHEME_DESIGNATOR, "Impression"));
                    doc->getTree().getCurrentContentItem().setStringValue("Negative study.");

                } else if (strcmp(argv[i], "04") == 0)
                {
                    doc->createNewDocument(DSRTypes::DT_BasicTextSR);

                    doc->setPatientsName("Mars^de^Verna^Marie");
                    doc->setPatientsBirthDate("193208108");
                    doc->setPatientsSex("F");
                    doc->setPatientID("62789");

                    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1111", OFFIS_CODING_SCHEME_DESIGNATOR, "History"));

                    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_PName, DSRTypes::AM_belowCurrent);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1112", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Name"));
                    doc->getTree().getCurrentContentItem().setStringValue("Struthers^Cortland^M.");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1111", OFFIS_CODING_SCHEME_DESIGNATOR, "Chief Complaint"));

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2225", OFFIS_CODING_SCHEME_DESIGNATOR, "Chief Complaint"));
                    doc->getTree().getCurrentContentItem().setStringValue("Prolapse and bleeding after each bowel movement for the past 3 - 4 months.");

                    doc->getTree().goUp();

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1111", OFFIS_CODING_SCHEME_DESIGNATOR, "Present Illness"));

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2225", OFFIS_CODING_SCHEME_DESIGNATOR, "Present Illness"));
                    doc->getTree().getCurrentContentItem().setStringValue("This 68 year-old white female says she usually has three bowel movements a day in small amounts, and there has been a recent change in the frequency, size, and type of bowel movement she has been having. She is also having some pain and irritation in this area. She has had no previous anorectal surgery or rectal infection. She denies any blood in the stool itself.");

                    doc->getTree().goUp();

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1111", OFFIS_CODING_SCHEME_DESIGNATOR, "Past History"));

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2225", OFFIS_CODING_SCHEME_DESIGNATOR, "Illnesses"));
                    doc->getTree().getCurrentContentItem().setStringValue("The patient had polio at age 8, from which she made a remarkable recovery. Apparently, she was paralyzed in both lower extremities but now has adequate use of these. She has had no other serious illnesses.");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2225", OFFIS_CODING_SCHEME_DESIGNATOR, "Allergies"));
                    doc->getTree().getCurrentContentItem().setStringValue("ALLERGIC TO PENICILLIN. She denies any other drug or food allergies.");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2225", OFFIS_CODING_SCHEME_DESIGNATOR, "Medications"));
                    doc->getTree().getCurrentContentItem().setStringValue("None");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2225", OFFIS_CODING_SCHEME_DESIGNATOR, "Operations"));
                    doc->getTree().getCurrentContentItem().setStringValue("Herniorrhaphy, 25 years ago.");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2225", OFFIS_CODING_SCHEME_DESIGNATOR, "Social"));
                    doc->getTree().getCurrentContentItem().setStringValue("She does not smoke or drink. She lives with her husband, who is an invalid and for whom she provides care. She is a retired municipal court judge.");

                    doc->getTree().goUp();

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1111", OFFIS_CODING_SCHEME_DESIGNATOR, "Family History"));

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2225", OFFIS_CODING_SCHEME_DESIGNATOR, "Family History"));
                    doc->getTree().getCurrentContentItem().setStringValue("One brother died of cancer of the throat; another has cancer of the kidney.");
 
                } else if (strcmp(argv[i], "05") == 0)
                {
                    doc->createNewDocument(DSRTypes::DT_BasicTextSR);

                    doc->setPatientsName("Silverman^Elaine^J.");
                    doc->setPatientsBirthDate("19811010");
                    doc->setPatientsSex("F");

                    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1111", OFFIS_CODING_SCHEME_DESIGNATOR, "Discharge Summary"));

                    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_PName, DSRTypes::AM_belowCurrent);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1112", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Name"));
                    doc->getTree().getCurrentContentItem().setStringValue("Cooper^Harold^B.");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2222", OFFIS_CODING_SCHEME_DESIGNATOR, "History of present Illness"));
                    doc->getTree().getCurrentContentItem().setStringValue("This 19-year-old black female, nulligravida, was admitted to the hospital on June 14, 1999 with fever of 102°, left lower quadrant pain, vaginal discharge, constipation, and a tender left adnexal mass. Her past history and family history were unremarkable. Present pain had started 2 to 3 weeks prior (starting on May 30, 1999) and lasted for 6 days. She had taken contraceptive pills in the past, but had stopped because she was not sexually active.");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2223", OFFIS_CODING_SCHEME_DESIGNATOR, "Physical Examination"));
                    doc->getTree().getCurrentContentItem().setStringValue("She appeared well-developed and well-nourished, and in mild distress. The only positive physical findings were limited to the abdomen and pelvis. Her abdomen was mildly distended, and it was tender, especially in the lower left quadrant. At pelvic examination her cervix was tender on motion, and the uterus was of normal size, retroverted, and somewhat fixed. There was a tender cystic mass about 4 - 5 cm in the left adnexa. Rectal examination was negative.");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2224", OFFIS_CODING_SCHEME_DESIGNATOR, "Admitting Diagnosis"));
                    doc->getTree().getCurrentContentItem().setStringValue("1. Probable pelvic inflammatory disease (PID).\n2. Rule out ectopic pregnancy.");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2225", OFFIS_CODING_SCHEME_DESIGNATOR, "Laboratory Data on Admission"));
                    doc->getTree().getCurrentContentItem().setStringValue("Hb 8.8, Hct 26.5, WBC 8,100 with 80 segs and 18 lymphs. Sedimentation rate 100 mm in one hour. Sickle cell prep + (turned out to be a trait). Urinalysis normal. Electrolytes normal. SMA-12 normal. Chest x-ray negative, 2 hour UCG negative.");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2226", OFFIS_CODING_SCHEME_DESIGNATOR, "HospitalCourse and Treatment"));
                    doc->getTree().getCurrentContentItem().setStringValue("Initially, she was given cephalothin 2 gm IV q6h, and kanamycin 0.5 gm IM b.i.d. Over the next 2 days the patient's condition improved. Her pain decreased, and her temperature came down to normal int he morning and spiked to 101º in the evening. Repeat CBC showed Hb 7.8, Hct 23.5. The pregnancy test was negative. On the second night following the admission, her temperature spiked to 104º. The patient was started on antituberculosis treatment, consisting of isoniazid 300 mg/day, ethambutol 600 mg b.i.d., and rifampin 600 mg daily. She became afebrile on the sixth postoperative day and was discharged on July 15, 1999, in good condition. She will be seen in the office in one week.");
 
                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2227", OFFIS_CODING_SCHEME_DESIGNATOR, "Surgical Procedures"));
                    doc->getTree().getCurrentContentItem().setStringValue("Biopsy of omentum for frozen section; culture specimens.");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2228", OFFIS_CODING_SCHEME_DESIGNATOR, "Discharge Diagnosis"));
                    doc->getTree().getCurrentContentItem().setStringValue("Genital tuberculosis.");
                
                } else if (strcmp(argv[i], "06") == 0)
                {
                    doc->createNewDocument(DSRTypes::DT_BasicTextSR);

                    doc->setPatientsName("Russel^William");
                    doc->setPatientsBirthDate("19900808");
                    doc->setPatientsSex("M");
                    doc->setPatientID("4523");
                    doc->setReferringPhysiciansName("Smythe^John");

                    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1111", OFFIS_CODING_SCHEME_DESIGNATOR, "Radiology Report"));

                    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_PName, DSRTypes::AM_belowCurrent);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1112", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Name"));
                    doc->getTree().getCurrentContentItem().setStringValue("Ruprecht^A.");
                    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1113", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Organization Name"));
                    doc->getTree().getCurrentContentItem().setStringValue("University of Iowa");

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("1200", OFFIS_CODING_SCHEME_DESIGNATOR, "Teeth Present"));
                    doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue(UID_SecondaryCaptureImageStorage, "1.2.276.0.7230010.3.1.4.123456.1.1"));

                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2222", OFFIS_CODING_SCHEME_DESIGNATOR, "Orthodontic/Pediatric Assessment"));
                    doc->getTree().getCurrentContentItem().setStringValue("The dental age and chronological age appear to coincide. The occlusal development is within the range of normal, except for the missing teeth noted above.");

                    /* reference to "missing teeth" (SCOORD)? */
                    
                    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
                    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("2229", OFFIS_CODING_SCHEME_DESIGNATOR, "Other"));
                    doc->getTree().getCurrentContentItem().setStringValue("The borders of the portrayed paranasal sinuses appear to be intact; there is no evidence of pathosis in these sinuses.\nThe airway appears patent, but there is evidence of marked adenoidal hyperplasia.\nPart of the first and second cervical vertebrae are depicted. No gross abnormalities are seen. There is a normal width of prevertebral soft tissue.\nThe generalized bone pattern and jaw morphology are within the range of normal.");

                } else {
                    writeFile = OFFalse;
                    cerr << "WARNING: unknown document identifier \"" << argv[i] << "\" ... ignoring" << endl;
                }

                if (writeFile)
                {
                    cout << OFString(79, '-') << endl;
                    cout << "mkreport: report" << argv[i] << ".dcm" << endl << endl;
                    doc->print(cout, DSRTypes::PF_shortenLongItemValues);
                    cout << endl;
    
                    DcmFileFormat *fileformat = new DcmFileFormat();
                    DcmDataset *dataset = NULL;
                    if (fileformat)
                        dataset = fileformat->getDataset();
                    if (dataset)
                    {
                        if (doc->write(*dataset) == EC_Normal)
                        {
                            OFString filename = "report";
                            filename += argv[i];
                            filename += ".dcm";
                            saveFileFormat(filename.c_str(), fileformat);
                        } else
                            cerr << "ERROR: could not write SR document into dataset" << endl;
                    }
                }
            }
        }
        delete doc;
    }
    return 0;
}
