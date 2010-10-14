/*
 *
 *  Copyright (C) 2000-2010, OFFIS e.V.
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
 *  Module: dcmsr
 *
 *  Author: Joerg Riesmeier
 *
 *  Purpose: Create sample structured reports using the dcmsr API
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2010-10-14 13:15:11 $
 *  CVS/RCS Revision: $Revision: 1.29 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/ofstd/ofstream.h"
#include "dcmtk/dcmsr/dsrdoc.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcfilefo.h"


// forward declarations
static void generate_ki(DSRDocument *doc, OFString &studyUID_ki);
static void generate_si(DSRDocument *doc, OFString &studyUID_ki);
static void generate_fk(DSRDocument *doc);
static void generate_lp(DSRDocument *doc);
static void generate_01(DSRDocument *doc, OFString &studyUID_01);
static void generate_02(DSRDocument *doc, OFString &studyUID_01);
static void generate_03(DSRDocument *doc);
static void generate_04(DSRDocument *doc);
static void generate_05(DSRDocument *doc);
static void generate_06(DSRDocument *doc);
static void generate_07(DSRDocument *doc);
static void generate_08(DSRDocument *doc);
static void generate_09(DSRDocument *doc);
static void generate_10(DSRDocument *doc);
static void generate_11(DSRDocument *doc);
static void generate_12(DSRDocument *doc);
static void generate_13(DSRDocument *doc);
static void generate_14(DSRDocument *doc);
static void generate_15(DSRDocument *doc);
static void generate_16(DSRDocument *doc);
static void generate_17(DSRDocument *doc);
static void generate_18(DSRDocument *doc);
static void generate_19(DSRDocument *doc);

// make all reports
const int num_reports = 24;
const char *all_reports[num_reports] = {"mkreport",
                                        "ki", "si", "fk", "lp",
                                        "01", "02", "03", "04", "05", "06", "07", "08",
                                        "09", "10", "11", "12", "13", "14", "15", "16", "17", "18", "19"};


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        COUT << "mkreport: Create DICOM SR documents" << OFendl;
        COUT << "----------------------------------------------------" << OFendl;
        COUT << " ki = IHE Year 2 key image note (empty)" << OFendl;
        COUT << " si = IHE Year 2 simple image report (empty)" << OFendl;
        COUT << " fk = Fake report, C. Iulius Caesar: De bello Gallico" << OFendl;
        COUT << " lp = Valid comprehensive report with loop/cycle" << OFendl;
        COUT << OFendl;
        COUT << " 01 = Consultation report (text only)" << OFendl;
        COUT << " 02 = Same as 01 but with NUM and PNAME items" << OFendl;
        COUT << " 03 = Very short report (text only)" << OFendl;
        COUT << " 04 = Text report with several sections (history)" << OFendl;
        COUT << " 05 = Text report with several blocks (discharge)" << OFendl;
        COUT << " 06 = Radiology report with image reference (dentist)" << OFendl;
        COUT << " 07 = Same as 06 with image/pstate reference" << OFendl;
        COUT << " 08 = Same as 06 with composite (pstate) reference" << OFendl;
        COUT << OFendl;
        COUT << " 09 = RSNA '95: Picker, CT, #3" << OFendl;
        COUT << " 10 = RSNA '95: Picker, MR, #4" << OFendl;
        COUT << " 11 = RSNA '95: Kodak, CR, #8" << OFendl;
        COUT << " 12 = RSNA '95: Acuson, US, #11" << OFendl;
        COUT << " 13 = RSNA '95: GE, CT, #17" << OFendl;
        COUT << " 14 = RSNA '95: GE, MR, #21" << OFendl;
        COUT << " 15 = RSNA '95: Siemens, MR, #26" << OFendl;
        COUT << " 16 = RSNA '95: Siemens, DS, #29" << OFendl;
        COUT << " 17 = RSNA '95: Siemens, DR, #31" << OFendl;
        COUT << " 18 = RSNA '95: Fuji, CR, #32" << OFendl;
        COUT << " 19 = RSNA '95: ATL, US, #36" << OFendl;
        COUT << "----------------------------------------------------" << OFendl;
        COUT << "all = create all abovementioned DICOM SR documents" << OFendl;
    } else {
        /* make sure data dictionary is loaded */
        if (!dcmDataDict.isDictionaryLoaded())
        {
            CERR << "Warning: no data dictionary loaded, "
                 << "check environment variable: "
                 << DCM_DICT_ENVIRONMENT_VARIABLE << OFendl;
        }

        DSRDocument *doc = new DSRDocument();
        if (doc != NULL)
        {
            OFString studyUID_ki, studyUID_01;
            OFBool writeFile = OFTrue;
            /* check whether to create all reports or only selected ones */
            const int count = (strcmp(argv[1], "all") == 0) ? num_reports : argc;
            const char **array = (strcmp(argv[1], "all") == 0) ? all_reports : OFconst_cast(const char **, argv);
            for (int i = 1; i < count; i++)
            {
                writeFile = OFTrue;
                if (strcmp(array[i], "ki") == 0)
                    generate_ki(doc, studyUID_ki);
                else if (strcmp(array[i], "si") == 0)
                    generate_si(doc, studyUID_ki);
                else if (strcmp(array[i], "fk") == 0)
                    generate_fk(doc);
                else if (strcmp(array[i], "lp") == 0)
                    generate_lp(doc);
                else if (strcmp(array[i], "01") == 0)
                    generate_01(doc, studyUID_01);
                else if (strcmp(array[i], "02") == 0)
                    generate_02(doc, studyUID_01);
                else if (strcmp(array[i], "03") == 0)
                    generate_03(doc);
                else if (strcmp(array[i], "04") == 0)
                    generate_04(doc);
                else if (strcmp(array[i], "05") == 0)
                    generate_05(doc);
                else if (strcmp(array[i], "06") == 0)
                    generate_06(doc);
                else if (strcmp(array[i], "07") == 0)
                    generate_07(doc);
                else if (strcmp(array[i], "08") == 0)
                    generate_08(doc);
                else if (strcmp(array[i], "09") == 0)
                    generate_09(doc);
                else if (strcmp(array[i], "10") == 0)
                    generate_10(doc);
                else if (strcmp(array[i], "11") == 0)
                    generate_11(doc);
                else if (strcmp(array[i], "12") == 0)
                    generate_12(doc);
                else if (strcmp(array[i], "13") == 0)
                    generate_13(doc);
                else if (strcmp(array[i], "14") == 0)
                    generate_14(doc);
                else if (strcmp(array[i], "15") == 0)
                    generate_15(doc);
                else if (strcmp(array[i], "16") == 0)
                    generate_16(doc);
                else if (strcmp(array[i], "17") == 0)
                    generate_17(doc);
                else if (strcmp(array[i], "18") == 0)
                    generate_18(doc);
                else if (strcmp(array[i], "19") == 0)
                    generate_19(doc);
                else {
                    writeFile = OFFalse;
                    CERR << "WARNING: unknown document identifier \"" << array[i] << "\" ... ignoring" << OFendl;
                }

                if (writeFile)
                {
                    COUT << OFString(79, '-') << OFendl;
                    COUT << "mkreport: report" << array[i] << ".dcm" << OFendl << OFendl;
                    doc->print(COUT, DSRTypes::PF_shortenLongItemValues);
                    COUT << OFendl;

                    DcmFileFormat *fileformat = new DcmFileFormat();
                    DcmDataset *dataset = NULL;
                    if (fileformat != NULL)
                        dataset = fileformat->getDataset();
                    if (dataset != NULL)
                    {
                        doc->getCodingSchemeIdentification().addPrivateDcmtkCodingScheme();
                        if (doc->write(*dataset).good())
                        {
                            OFString filename = "report";
                            filename += array[i];
                            filename += ".dcm";
                            fileformat->saveFile(filename.c_str(), EXS_LittleEndianExplicit);
                        } else
                            CERR << "ERROR: could not write SR document into dataset" << OFendl;
                    }
                }
            }
        }
        delete doc;
    }
    return 0;
}


static void generate_ki(DSRDocument *doc,
                        OFString &studyUID_ki)
{
    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
    doc->getStudyInstanceUID(studyUID_ki);
    doc->setStudyDescription("OFFIS Structured Reporting Templates");
    doc->setSeriesDescription("IHE Year 2 - Key Image Note");
    doc->setSpecificCharacterSetType(DSRTypes::CS_Latin1);

    doc->setPatientName("Last Name^First Name");
    doc->setPatientSex("O");
    doc->setManufacturer("OFFIS e.V.");
    doc->setReferringPhysicianName("Last Name^First Name");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.01", OFFIS_CODING_SCHEME_DESIGNATOR, "Document Title"));

    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Code, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Observation Context Mode"));
    doc->getTree().getCurrentContentItem().setCodeValue(DSRCodedEntryValue("IHE.03", OFFIS_CODING_SCHEME_DESIGNATOR, "DIRECT"));
    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_PName);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.04", OFFIS_CODING_SCHEME_DESIGNATOR, "Recording Observer's Name"));
    doc->getTree().getCurrentContentItem().setStringValue("Enter text");
    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Recording Observer's Organization Name"));
    doc->getTree().getCurrentContentItem().setStringValue("Enter text");
    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Code);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.06", OFFIS_CODING_SCHEME_DESIGNATOR, "Observation Context Mode"));
    doc->getTree().getCurrentContentItem().setCodeValue(DSRCodedEntryValue("IHE.07", OFFIS_CODING_SCHEME_DESIGNATOR, "PATIENT"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.11", OFFIS_CODING_SCHEME_DESIGNATOR, "Key Image Description"));
    doc->getTree().getCurrentContentItem().setStringValue("Enter text");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.10", OFFIS_CODING_SCHEME_DESIGNATOR, "Image Reference"));
    doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue("0", "0"));
}


static void generate_si(DSRDocument *doc,
                        OFString &studyUID_ki)
{
    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
    if (!studyUID_ki.empty())
        doc->createNewSeriesInStudy(studyUID_ki);
    doc->setStudyDescription("OFFIS Structured Reporting Templates");
    doc->setSeriesDescription("IHE Year 2 - Simple Image Report");
    doc->setSpecificCharacterSetType(DSRTypes::CS_Latin1);

    doc->setPatientName("Last Name^First Name");
    doc->setPatientSex("O");
    doc->setManufacturer("OFFIS e.V.");
    doc->setReferringPhysicianName("Last Name^First Name");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.01", OFFIS_CODING_SCHEME_DESIGNATOR, "Document Title"));

    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Code, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Observation Context Mode"));
    doc->getTree().getCurrentContentItem().setCodeValue(DSRCodedEntryValue("IHE.03", OFFIS_CODING_SCHEME_DESIGNATOR, "DIRECT"));
    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_PName);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.04", OFFIS_CODING_SCHEME_DESIGNATOR, "Recording Observer's Name"));
    doc->getTree().getCurrentContentItem().setStringValue("Enter text");
    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Recording Observer's Organization Name"));
    doc->getTree().getCurrentContentItem().setStringValue("Enter text");
    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Code);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.06", OFFIS_CODING_SCHEME_DESIGNATOR, "Observation Context Mode"));
    doc->getTree().getCurrentContentItem().setCodeValue(DSRCodedEntryValue("IHE.07", OFFIS_CODING_SCHEME_DESIGNATOR, "PATIENT"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.08", OFFIS_CODING_SCHEME_DESIGNATOR, "Section Heading"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.09", OFFIS_CODING_SCHEME_DESIGNATOR, "Report Text"));
    doc->getTree().getCurrentContentItem().setStringValue("Enter text");

    doc->getTree().addContentItem(DSRTypes::RT_inferredFrom, DSRTypes::VT_Image, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.10", OFFIS_CODING_SCHEME_DESIGNATOR, "Image Reference"));
    doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue("0", "0"));

    doc->getTree().goUp();

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.10", OFFIS_CODING_SCHEME_DESIGNATOR, "Image Reference"));
    doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue("0", "0"));
}


static void generate_fk(DSRDocument *doc)
{
    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
    doc->createNewSeriesInStudy("1.2.276.0.7230010.3.4.1915765545.18030.917282194.11");
    doc->setStudyDescription("OFFIS Structured Reporting Templates");
    doc->setSeriesDescription("Fake Report, C. Iulius Caesar: De bello Gallico");

    doc->setPatientName("Caesar^Gaius Iulius");
    doc->setPatientSex("M");
    doc->setReferringPhysicianName("Augustus Caesar^Gaius Iulius Octavianus");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CH_0.1", OFFIS_CODING_SCHEME_DESIGNATOR, "De bello Gallico"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CH_1.1", OFFIS_CODING_SCHEME_DESIGNATOR, "Image of the Author"));
    doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue(UID_SecondaryCaptureImageStorage, "1.2.276.0.7230010.3.4.1915765545.18030.917282194.11.1.1", UID_GrayscaleSoftcopyPresentationStateStorage, "1.2.276.0.7230010.3.1.4.1707840890.221.974385531.18"));
    doc->getCurrentRequestedProcedureEvidence().addItem("1.2.276.0.7230010.3.4.1915765545.18030.917282194.11", "1.2.276.0.7230010.3.4.1915765545.18030.917282194.11.1", UID_SecondaryCaptureImageStorage, "1.2.276.0.7230010.3.4.1915765545.18030.917282194.11.1.1");
    doc->getCurrentRequestedProcedureEvidence().setRetrieveAETitle("DCMPSTATE");
    doc->getPertinentOtherEvidence().addItem("1.2.276.0.7230010.3.4.1915765545.18030.917282194.11", "1.2.276.0.7230010.3.1.4.1707840890.221.974385403.16", UID_GrayscaleSoftcopyPresentationStateStorage, "1.2.276.0.7230010.3.1.4.1707840890.221.974385531.18");
    doc->getPertinentOtherEvidence().setRetrieveAETitle("DCMPSTATE");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CH_2.1", OFFIS_CODING_SCHEME_DESIGNATOR, "Liber primus"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CH_3.1", OFFIS_CODING_SCHEME_DESIGNATOR, "I"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CH_4.1", OFFIS_CODING_SCHEME_DESIGNATOR, "1"));
    doc->getTree().getCurrentContentItem().setStringValue("Gallia est omnis divisa in partes tres, quarum unam incolunt Belgae, aliam Aquitani, tertiam, qui ipsorum lingua Celtae, nostra Galli appellantur.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CH_4.2", OFFIS_CODING_SCHEME_DESIGNATOR, "2"));
    doc->getTree().getCurrentContentItem().setStringValue("Hi omnes lingua, institutis, legibus inter se differunt. Gallos ab Aquitanis Garunna flumen, a Belgis Matrona et Sequana dividit.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CH_4.3", OFFIS_CODING_SCHEME_DESIGNATOR, "3"));
    doc->getTree().getCurrentContentItem().setStringValue("Horum omnium fortissimi sunt Belgae, propterea quod a cultu atque humanitate provinciae longissime absunt minimeque ad eos mercatores saepe commeant atque ea, quae ad effeminandos animos pertinent, important proximique sunt Germanis, qui trans Rhenum incolunt, quibuscum continenter bellum gerunt.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CH_4.4", OFFIS_CODING_SCHEME_DESIGNATOR, "4"));
    doc->getTree().getCurrentContentItem().setStringValue("Qua de causa Helvetii quoque reliquos Gallos virtute praecedunt, quod fere cotidianis proeliis cum Germanis contendunt, cum aut suis finibus eos prohibent aut ipsi in eorum finibus bellum gerunt.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CH_4.5", OFFIS_CODING_SCHEME_DESIGNATOR, "5"));
    doc->getTree().getCurrentContentItem().setStringValue("[Eorum una, pars, quam Gallos obtinere dictum est, initium capit a flumine Rhodano, continetur Garumna flumine, Oceano, finibus Belgarum, attingit etiam ab Sequanis et Helvetiis flumen Rhenum, vergit ad septentriones.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CH_4.6", OFFIS_CODING_SCHEME_DESIGNATOR, "6"));
    doc->getTree().getCurrentContentItem().setStringValue("Belgae ab extremis Galliae finibus oriuntur, pertinent ad inferiorem partem fluminis Rheni, spectant in septentrionem et orientem solem.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CH_4.7", OFFIS_CODING_SCHEME_DESIGNATOR, "7"));
    doc->getTree().getCurrentContentItem().setStringValue("Aquitania a Garumna flumine ad Pyrenaeos montes et eam partem Oceani quae est ad Hispaniam pertinet; spectat inter occasum solis et septentriones.]");

    doc->getTree().goUp();

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CH_3.2", OFFIS_CODING_SCHEME_DESIGNATOR, "II"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CH_4.1", OFFIS_CODING_SCHEME_DESIGNATOR, "1"));
    doc->getTree().getCurrentContentItem().setStringValue("Apud Helvetios longe nobilissimus fuit et ditissimus Orgetorix. Is M. Messalla M. Pisone consulibus regni cupiditate inductus coniurationem nobilitatis fecit et civitati persuasit, ut de finibus suis cum omnibus copiis exirent:");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CH_4.2", OFFIS_CODING_SCHEME_DESIGNATOR, "2"));
    doc->getTree().getCurrentContentItem().setStringValue("Perfacile esse, cum virtute omnibus praestarent, totius Galliae imperio potiri.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CH_4.3", OFFIS_CODING_SCHEME_DESIGNATOR, "3"));
    doc->getTree().getCurrentContentItem().setStringValue("Id hoc facilius iis persuasit, quod undique loci natura Helvetii continentur: una ex parte flumine Rheno latissimo atque altissimo, qui agrum Helvetium a Germanis dividit, altera ex parte monte Iura altissimo, qui est inter Sequanos et Helvetios, tertia lacu Lemanno et flumine Rhodano, qui provinciam nostram ab Helvetiis dividit.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CH_4.4", OFFIS_CODING_SCHEME_DESIGNATOR, "4"));
    doc->getTree().getCurrentContentItem().setStringValue("His rebus fiebat, ut et minus late vagarentur et minus facile finitimis bellum inferre possent; qua ex parte homines bellandi cupidi magno dolore afficiebantur. 5 Pro multitudine autem hominum et pro gloria belli atque fortitudinis angustos se fines habere arbitrabantur, qui in longitudinem milia passuum ducenta quadraginta, in latitudinem centum octoginta patebant.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CH_4.5", OFFIS_CODING_SCHEME_DESIGNATOR, "5"));
    doc->getTree().getCurrentContentItem().setStringValue("Pro multitudine autem hominum et pro gloria belli atque fortitudinis angustos se fines habere arbitrabantur, qui in longitudinem milia passuum ducenta quadraginta, in latitudinem centum octoginta patebant.");

    doc->completeDocument();
    doc->verifyDocument("Augustus Caesar^Gaius Iulius Octavianus", "SPQR");
}


static void generate_lp(DSRDocument *doc)
{
    doc->createNewDocument(DSRTypes::DT_ComprehensiveSR);
    doc->setStudyDescription("OFFIS Structured Reporting Test");
    doc->setSeriesDescription("Valid report with loop/cycle");

    doc->setPatientName("Loop^Mr");
    doc->setPatientSex("M");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("TST.01", OFFIS_CODING_SCHEME_DESIGNATOR, "Document Title"));

    size_t node1 = doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("TST.02", OFFIS_CODING_SCHEME_DESIGNATOR, "First Paragraph"));
    doc->getTree().getCurrentContentItem().setStringValue("Some text.");

    size_t node2 = doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("TST.03", OFFIS_CODING_SCHEME_DESIGNATOR, "Second Paragraph"));
    doc->getTree().getCurrentContentItem().setStringValue("Some more text.");

    doc->getTree().addByReferenceRelationship(DSRTypes::RT_inferredFrom, node1);
    doc->getTree().gotoNode(node1);
    doc->getTree().addByReferenceRelationship(DSRTypes::RT_inferredFrom, node2);
}


static void generate_01(DSRDocument *doc, OFString &studyUID_01)
{
    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
    doc->setStudyDescription("OFFIS Structured Reporting Samples");
    doc->getStudyInstanceUID(studyUID_01);
    doc->setSeriesDescription("Basic Text Report");

    doc->setPatientName("Osterman^Phillip^B.");
    doc->setPatientBirthDate("19220909");
    doc->setPatientSex("M");
    doc->setReferringPhysicianName("Fukuda^Katherine M.^^^M. D.");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("DT.06", OFFIS_CODING_SCHEME_DESIGNATOR, "Consultation Report"));

    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_PName, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.04", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Name"));
    doc->getTree().getCurrentContentItem().setStringValue("Packer^David M.^^^M. D.");
    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Organization Name"));
    doc->getTree().getCurrentContentItem().setStringValue("Redlands Clinic");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_01", OFFIS_CODING_SCHEME_DESIGNATOR, "Description"));
    doc->getTree().getCurrentContentItem().setStringValue("This 78-year-old gentleman referred by Dr. Fukuda was also seen by Dr. Mason at the Redlands Clinic. He has been seen in the past by Dr. Klugman.\nThe patient developed a lesion in the concha of the left external ear. Recent biopsy confirmed this as being a squamous cell carcinoma. The patient has had a few other skin cancers.\nOf most significant past history is the fact that this patient has a leukemia that has been treated in standard fashion by Dr. Klugman. The patient was then transferred to the Redlands Clinic and by some experimental protocol which, I guess, includes some sort of lymphocyte electrophoresis, has been placed into remission. He is not currently on any antileukemia drugs and has responded extremely well to his medical management.\nOn examination, the patient is healthy in general appearance. There is a 1.5 cm lesion on the concha of the ear, which is seen well on photograph of the left external ear. There are numerous soft lymph nodes in both sides of the neck, which I presume are related to his leukemia.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_02", OFFIS_CODING_SCHEME_DESIGNATOR, "Diagnosis"));
    doc->getTree().getCurrentContentItem().setStringValue("Squamous cell carcinoma, relatively superficial, involving the skin of the left external ear, which is seen well on photograph of the left external ear.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_03", OFFIS_CODING_SCHEME_DESIGNATOR, "Treatment"));
    doc->getTree().getCurrentContentItem().setStringValue("The plan of treatment is as follows: 4500 rad, 15 treatment sessions, using 100 kV radiation.\nThe reason for treatment, expected acute reaction, and remote possibility of complication was discussed with this patient at some length, and he accepted therapy as outlined.");
}


static void generate_02(DSRDocument *doc, OFString &studyUID_01)
{
    doc->createNewDocument(DSRTypes::DT_EnhancedSR);
    if (!studyUID_01.empty())
        doc->createNewSeriesInStudy(studyUID_01);
    doc->setStudyDescription("OFFIS Structured Reporting Samples");
    doc->setSeriesDescription("Text Report with CODE, NUM and PNAME content items");

    doc->setPatientName("Osterman^Phillip B.");
    doc->setPatientBirthDate("19220909");
    doc->setPatientSex("M");
    doc->setReferringPhysicianName("Fukuda^Katherine M.^^^M. D.");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("DT.06", OFFIS_CODING_SCHEME_DESIGNATOR, "Consultation Report"));

    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_PName, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.04", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Name"));
    doc->getTree().getCurrentContentItem().setStringValue("Packer^David M.^^^M. D.");
    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Organization Name"));
    doc->getTree().getCurrentContentItem().setStringValue("Redlands Clinic");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setContinuityOfContent(DSRTypes::COC_Continuous);

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_01", OFFIS_CODING_SCHEME_DESIGNATOR, "Description"));
    doc->getTree().getCurrentContentItem().setStringValue("This 78-year-old gentleman referred by");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_PName);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_04", OFFIS_CODING_SCHEME_DESIGNATOR, "Referring Physician"));
    doc->getTree().getCurrentContentItem().setStringValue("Fukuda^^^Dr.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_01", OFFIS_CODING_SCHEME_DESIGNATOR, "Description"));
    doc->getTree().getCurrentContentItem().setStringValue("was also seen by");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_PName);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("OR.01", OFFIS_CODING_SCHEME_DESIGNATOR, "Physician"));
    doc->getTree().getCurrentContentItem().setStringValue("Mason^^^Dr.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_01", OFFIS_CODING_SCHEME_DESIGNATOR, "Description"));
    doc->getTree().getCurrentContentItem().setStringValue("at the");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Code);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_05", OFFIS_CODING_SCHEME_DESIGNATOR, "Hospital Name"));
    doc->getTree().getCurrentContentItem().setCodeValue(DSRCodedEntryValue("CODE_06", OFFIS_CODING_SCHEME_DESIGNATOR, "Redlands Clinic"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_01", OFFIS_CODING_SCHEME_DESIGNATOR, "Description"));
    doc->getTree().getCurrentContentItem().setStringValue(". He has been seen in the past by");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_PName);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("OR.01", OFFIS_CODING_SCHEME_DESIGNATOR, "Physician"));
    doc->getTree().getCurrentContentItem().setStringValue("Klugman^^^Dr.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_01", OFFIS_CODING_SCHEME_DESIGNATOR, "Description"));
    doc->getTree().getCurrentContentItem().setStringValue(".\nThe patient developed a lesion in the concha of the left external ear. Recent biopsy confirmed this as being a squamous cell carcinoma. The patient has had a few other skin cancers.\nOf most significant past history is the fact that this patient has a leukemia that has been treated in standard fashion by");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_PName);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("OR.01", OFFIS_CODING_SCHEME_DESIGNATOR, "Physician"));
    doc->getTree().getCurrentContentItem().setStringValue("Klugman^^^Dr.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_01", OFFIS_CODING_SCHEME_DESIGNATOR, "Description"));
    doc->getTree().getCurrentContentItem().setStringValue(". The patient was then transferred to the");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Code);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_05", OFFIS_CODING_SCHEME_DESIGNATOR, "Hospital Name"));
    doc->getTree().getCurrentContentItem().setCodeValue(DSRCodedEntryValue("CODE_06", OFFIS_CODING_SCHEME_DESIGNATOR, "Redlands Clinic"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_01", OFFIS_CODING_SCHEME_DESIGNATOR, "Description"));
    doc->getTree().getCurrentContentItem().setStringValue("and by some experimental protocol which, I guess, includes some sort of lymphocyte electrophoresis, has been placed into remission. He is not currently on any antileukemia drugs and has responded extremely well to his medical management.\nOn examination, the patient is healthy in general appearance. There is a");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Num);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("M-02550", "SNM3", "Diameter"));
    doc->getTree().getCurrentContentItem().setNumericValue(DSRNumericMeasurementValue("1.5", DSRCodedEntryValue("cm", "UCUM", "1.4", "centimeter")));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_01", OFFIS_CODING_SCHEME_DESIGNATOR, "Description"));
    doc->getTree().getCurrentContentItem().setStringValue("lesion on the concha of the ear, which is seen well on photograph of the left external ear. There are numerous soft lymph nodes in both sides of the neck, which I presume are related to his leukemia.");

    doc->getTree().goUp();

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_02", OFFIS_CODING_SCHEME_DESIGNATOR, "Diagnosis"));
    doc->getTree().getCurrentContentItem().setStringValue("Squamous cell carcinoma, relatively superficial, involving the skin of the left external ear, which is seen well on photograph of the left external ear.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_03", OFFIS_CODING_SCHEME_DESIGNATOR, "Treatment"));
    doc->getTree().getCurrentContentItem().setStringValue("The plan of treatment is as follows: 4500 rad, 15 treatment sessions, using 100 kV radiation.\nThe reason for treatment, expected acute reaction, and remote possibility of complication was discussed with this patient at some length, and he accepted therapy as outlined.");

    // add additional information on UCUM coding scheme (UID from CP 372)
    doc->getCodingSchemeIdentification().addItem("UCUM");
    doc->getCodingSchemeIdentification().setCodingSchemeUID("2.16.840.1.113883.6.8");
    doc->getCodingSchemeIdentification().setCodingSchemeName("Unified Code for Units of Measure");
    doc->getCodingSchemeIdentification().setCodingSchemeVersion("1.4");
    doc->getCodingSchemeIdentification().setResponsibleOrganization("Regenstrief Institute for Health Care, Indianapolis");
}


static void generate_03(DSRDocument *doc)
{
    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
    doc->setStudyDescription("OFFIS Structured Reporting Samples");
    doc->setSeriesDescription("Text Report with different sections");

    doc->setPatientName("Silverman^Elaine J.");
    doc->setPatientBirthDate("19811010");
    doc->setPatientSex("F");
    doc->setReferringPhysicianName("Cooper^Harold B.^^^M. D.");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("DT.01", OFFIS_CODING_SCHEME_DESIGNATOR, "Radiology Report"));

    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_PName, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.04", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Name"));
    doc->getTree().getCurrentContentItem().setStringValue("Skinner^Marian B.^^^M. D.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("SH.06", OFFIS_CODING_SCHEME_DESIGNATOR, "Findings"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_07", OFFIS_CODING_SCHEME_DESIGNATOR, "PA Chest"));
    doc->getTree().getCurrentContentItem().setStringValue("Upright PA view of the chest shows the lung fields are clear, without evidence of an active process. Heart size in normal.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.06", OFFIS_CODING_SCHEME_DESIGNATOR, "Impression"));
    doc->getTree().getCurrentContentItem().setStringValue("Negative chest.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_08", OFFIS_CODING_SCHEME_DESIGNATOR, "Abdomen"));
    doc->getTree().getCurrentContentItem().setStringValue("Flat and upright views of the abdomen show a normal gas pattern without evidence of obstruction or ileus. There are no calcifications or abnormal masses noted.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.06", OFFIS_CODING_SCHEME_DESIGNATOR, "Impression"));
    doc->getTree().getCurrentContentItem().setStringValue("Negative study.");
}


static void generate_04(DSRDocument *doc)
{
    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
    doc->setStudyDescription("OFFIS Structured Reporting Samples");
    doc->setSeriesDescription("Text Report with hierarchical structure");

    doc->setPatientName("Mars^Verna Marie^de");
    doc->setPatientBirthDate("19320810");
    doc->setPatientSex("F");
    doc->setPatientID("62789");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("SH.01", OFFIS_CODING_SCHEME_DESIGNATOR, "History"));

    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_PName, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.04", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Name"));
    doc->getTree().getCurrentContentItem().setStringValue("Struthers^Cortland M.^^^M. D.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_09", OFFIS_CODING_SCHEME_DESIGNATOR, "Chief Complaint"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_09", OFFIS_CODING_SCHEME_DESIGNATOR, "Chief Complaint"));
    doc->getTree().getCurrentContentItem().setStringValue("Prolapse and bleeding after each bowel movement for the past 3 - 4 months.");

    doc->getTree().goUp();

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_10", OFFIS_CODING_SCHEME_DESIGNATOR, "Present Illness"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_10", OFFIS_CODING_SCHEME_DESIGNATOR, "Present Illness"));
    doc->getTree().getCurrentContentItem().setStringValue("This 68 year-old white female says she usually has three bowel movements a day in small amounts, and there has been a recent change in the frequency, size, and type of bowel movement she has been having. She is also having some pain and irritation in this area. She has had no previous anorectal surgery or rectal infection. She denies any blood in the stool itself.");

    doc->getTree().goUp();

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_11", OFFIS_CODING_SCHEME_DESIGNATOR, "Past History"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_12", OFFIS_CODING_SCHEME_DESIGNATOR, "Illnesses"));
    doc->getTree().getCurrentContentItem().setStringValue("The patient had polio at age 8, from which she made a remarkable recovery. Apparently, she was paralyzed in both lower extremities but now has adequate use of these. She has had no other serious illnesses.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_13", OFFIS_CODING_SCHEME_DESIGNATOR, "Allergies"));
    doc->getTree().getCurrentContentItem().setStringValue("ALLERGIC TO PENICILLIN. She denies any other drug or food allergies.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_14", OFFIS_CODING_SCHEME_DESIGNATOR, "Medications"));
    doc->getTree().getCurrentContentItem().setStringValue("None");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_15", OFFIS_CODING_SCHEME_DESIGNATOR, "Operations"));
    doc->getTree().getCurrentContentItem().setStringValue("Herniorrhaphy, 25 years ago.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_16", OFFIS_CODING_SCHEME_DESIGNATOR, "Social"));
    doc->getTree().getCurrentContentItem().setStringValue("She does not smoke or drink. She lives with her husband, who is an invalid and for whom she provides care. She is a retired municipal court judge.");

    doc->getTree().goUp();

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_17", OFFIS_CODING_SCHEME_DESIGNATOR, "Family History"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_18", OFFIS_CODING_SCHEME_DESIGNATOR, "Family History"));
    doc->getTree().getCurrentContentItem().setStringValue("One brother died of cancer of the throat; another has cancer of the kidney.");
}


static void generate_05(DSRDocument *doc)
{
    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
    doc->setStudyDescription("OFFIS Structured Reporting Samples");
    doc->setSeriesDescription("Text Report with different sections");
    doc->setSpecificCharacterSetType(DSRTypes::CS_Latin1);

    doc->setPatientName("Silverman^Elaine J.");
    doc->setPatientBirthDate("19811010");
    doc->setPatientSex("F");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_19", OFFIS_CODING_SCHEME_DESIGNATOR, "Discharge Summary"));

    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_PName, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.04", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Name"));
    doc->getTree().getCurrentContentItem().setStringValue("Cooper^Harold B.^^^M. D.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_20", OFFIS_CODING_SCHEME_DESIGNATOR, "History of present Illness"));
    doc->getTree().getCurrentContentItem().setStringValue("This 19-year-old black female, nulligravida, was admitted to the hospital on June 14, 1999 with fever of 102°, left lower quadrant pain, vaginal discharge, constipation, and a tender left adnexal mass. Her past history and family history were unremarkable. Present pain had started 2 to 3 weeks prior (starting on May 30, 1999) and lasted for 6 days. She had taken contraceptive pills in the past, but had stopped because she was not sexually active.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_21", OFFIS_CODING_SCHEME_DESIGNATOR, "Physical Examination"));
    doc->getTree().getCurrentContentItem().setStringValue("She appeared well-developed and well-nourished, and in mild distress. The only positive physical findings were limited to the abdomen and pelvis. Her abdomen was mildly distended, and it was tender, especially in the lower left quadrant. At pelvic examination her cervix was tender on motion, and the uterus was of normal size, retroverted, and somewhat fixed. There was a tender cystic mass about 4 - 5 cm in the left adnexa. Rectal examination was negative.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_22", OFFIS_CODING_SCHEME_DESIGNATOR, "Admitting Diagnosis"));
    doc->getTree().getCurrentContentItem().setStringValue("1. Probable pelvic inflammatory disease (PID).\n2. Rule out ectopic pregnancy.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_23", OFFIS_CODING_SCHEME_DESIGNATOR, "Laboratory Data on Admission"));
    doc->getTree().getCurrentContentItem().setStringValue("Hb 8.8, Hct 26.5, WBC 8,100 with 80 segs and 18 lymphs. Sedimentation rate 100 mm in one hour. Sickle cell prep + (turned out to be a trait). Urinalysis normal. Electrolytes normal. SMA-12 normal. Chest x-ray negative, 2 hour UCG negative.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_24", OFFIS_CODING_SCHEME_DESIGNATOR, "Hospital Course and Treatment"));
    doc->getTree().getCurrentContentItem().setStringValue("Initially, she was given cephalothin 2 gm IV q6h, and kanamycin 0.5 gm IM b.i.d. Over the next 2 days the patient's condition improved. Her pain decreased, and her temperature came down to normal int he morning and spiked to 101° in the evening. Repeat CBC showed Hb 7.8, Hct 23.5. The pregnancy test was negative. On the second night following the admission, her temperature spiked to 104º. The patient was started on antituberculosis treatment, consisting of isoniazid 300 mg/day, ethambutol 600 mg b.i.d., and rifampin 600 mg daily. She became afebrile on the sixth postoperative day and was discharged on July 15, 1999, in good condition. She will be seen in the office in one week.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_25", OFFIS_CODING_SCHEME_DESIGNATOR, "Surgical Procedures"));
    doc->getTree().getCurrentContentItem().setStringValue("Biopsy of omentum for frozen section; culture specimens.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_26", OFFIS_CODING_SCHEME_DESIGNATOR, "Discharge Diagnosis"));
    doc->getTree().getCurrentContentItem().setStringValue("Genital tuberculosis.");
}


static void generate_06(DSRDocument *doc)
{
    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
    doc->createNewSeriesInStudy("1.2.276.0.7230010.3.1.4.123456");
    doc->setStudyDescription("OFFIS Structured Reporting Samples");
    doc->setSeriesDescription("Report with Image Reference");

    doc->setPatientName("Russel^William");
    doc->setPatientBirthDate("19900808");
    doc->setPatientSex("M");
    doc->setPatientID("4523");
    doc->setReferringPhysicianName("Smythe^John^^Dr.");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("DT.01", OFFIS_CODING_SCHEME_DESIGNATOR, "Radiology Report"));

    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_PName, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.04", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Name"));
    doc->getTree().getCurrentContentItem().setStringValue("Ruprecht^A.^^^Professor & Director");
    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Organization Name"));
    doc->getTree().getCurrentContentItem().setStringValue("University of Iowa");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_27", OFFIS_CODING_SCHEME_DESIGNATOR, "Teeth Present"));
    doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue(UID_SecondaryCaptureImageStorage, "1.2.276.0.7230010.3.1.4.123456.1.1"));
    doc->getCurrentRequestedProcedureEvidence().addItem("1.2.276.0.7230010.3.1.4.123456", "1.2.276.0.7230010.3.1.4.123456.1", UID_SecondaryCaptureImageStorage, "1.2.276.0.7230010.3.1.4.123456.1.1");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_28", OFFIS_CODING_SCHEME_DESIGNATOR, "Orthodontic/Pediatric Assessment"));
    doc->getTree().getCurrentContentItem().setStringValue("The dental age and chronological age appear to coincide. The occlusal development is within the range of normal, except for the missing teeth noted above.");

    /* reference to "missing teeth" (SCOORD)? */

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_29", OFFIS_CODING_SCHEME_DESIGNATOR, "Other"));
    doc->getTree().getCurrentContentItem().setStringValue("The borders of the portrayed paranasal sinuses appear to be intact; there is no evidence of pathosis in these sinuses.\nThe airway appears patent, but there is evidence of marked adenoidal hyperplasia.\nPart of the first and second cervical vertebrae are depicted. No gross abnormalities are seen. There is a normal width of prevertebral soft tissue.\nThe generalized bone pattern and jaw morphology are within the range of normal.");
}


static void generate_07(DSRDocument *doc)
{
    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
    doc->createNewSeriesInStudy("1.2.276.0.7230010.3.1.4.123456");
    doc->setStudyDescription("OFFIS Structured Reporting Samples");
    doc->setSeriesDescription("Report with Image and Presentation State Reference");

    doc->setPatientName("Russel^William");
    doc->setPatientBirthDate("19900808");
    doc->setPatientSex("M");
    doc->setPatientID("4523");
    doc->setReferringPhysicianName("Smythe^John^^Dr.");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("DT.01", OFFIS_CODING_SCHEME_DESIGNATOR, "Radiology Report"));

    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_PName, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.04", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Name"));
    doc->getTree().getCurrentContentItem().setStringValue("Ruprecht^A.^^^Professor & Director");
    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Organization Name"));
    doc->getTree().getCurrentContentItem().setStringValue("University of Iowa");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_27", OFFIS_CODING_SCHEME_DESIGNATOR, "Teeth Present"));
    doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue(UID_SecondaryCaptureImageStorage, "1.2.276.0.7230010.3.1.4.123456.1.2", UID_GrayscaleSoftcopyPresentationStateStorage, "1.2.276.0.7230010.3.1.4.123456.2.2"));
    doc->getCurrentRequestedProcedureEvidence().addItem("1.2.276.0.7230010.3.1.4.123456", "1.2.276.0.7230010.3.1.4.123456.1", UID_SecondaryCaptureImageStorage, "1.2.276.0.7230010.3.1.4.123456.1.2");
    doc->getCurrentRequestedProcedureEvidence().addItem("1.2.276.0.7230010.3.1.4.123456", "1.2.276.0.7230010.3.1.4.123456.2", UID_GrayscaleSoftcopyPresentationStateStorage, "1.2.276.0.7230010.3.1.4.123456.2.2");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_28", OFFIS_CODING_SCHEME_DESIGNATOR, "Orthodontic/Pediatric Assessment"));
    doc->getTree().getCurrentContentItem().setStringValue("The dental age and chronological age appear to coincide. The occlusal development is within the range of normal, except for the missing teeth noted above.");

    /* reference to "missing teeth" (SCOORD)? */

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_29", OFFIS_CODING_SCHEME_DESIGNATOR, "Other"));
    doc->getTree().getCurrentContentItem().setStringValue("The borders of the portrayed paranasal sinuses appear to be intact; there is no evidence of pathosis in these sinuses.\nThe airway appears patent, but there is evidence of marked adenoidal hyperplasia.\nPart of the first and second cervical vertebrae are depicted. No gross abnormalities are seen. There is a normal width of prevertebral soft tissue.\nThe generalized bone pattern and jaw morphology are within the range of normal.");
}


static void generate_08(DSRDocument *doc)
{
    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
    doc->createNewSeriesInStudy("1.2.276.0.7230010.3.1.4.123456");
    doc->setStudyDescription("OFFIS Structured Reporting Samples");
    doc->setSeriesDescription("Report with Presentation State Reference");

    doc->setPatientName("Russel^William");
    doc->setPatientBirthDate("19900808");
    doc->setPatientSex("M");
    doc->setPatientID("4523");
    doc->setReferringPhysicianName("Smythe^John^^Dr.");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("DT.01", OFFIS_CODING_SCHEME_DESIGNATOR, "Radiology Report"));

    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_PName, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.04", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Name"));
    doc->getTree().getCurrentContentItem().setStringValue("Ruprecht^A.^^^Professor & Director");
    doc->getTree().addContentItem(DSRTypes::RT_hasObsContext, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IHE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Observer Organization Name"));
    doc->getTree().getCurrentContentItem().setStringValue("University of Iowa");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Composite);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_27", OFFIS_CODING_SCHEME_DESIGNATOR, "Teeth Present"));
    doc->getTree().getCurrentContentItem().setCompositeReference(DSRCompositeReferenceValue(UID_GrayscaleSoftcopyPresentationStateStorage, "1.2.276.0.7230010.3.1.4.123456.2.2"));
    doc->getCurrentRequestedProcedureEvidence().addItem("1.2.276.0.7230010.3.1.4.123456", "1.2.276.0.7230010.3.1.4.123456.2", UID_GrayscaleSoftcopyPresentationStateStorage, "1.2.276.0.7230010.3.1.4.123456.2.2");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_28", OFFIS_CODING_SCHEME_DESIGNATOR, "Orthodontic/Pediatric Assessment"));
    doc->getTree().getCurrentContentItem().setStringValue("The dental age and chronological age appear to coincide. The occlusal development is within the range of normal, except for the missing teeth noted above.");

    /* reference to "missing teeth" (SCOORD)? */

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("CODE_29", OFFIS_CODING_SCHEME_DESIGNATOR, "Other"));
    doc->getTree().getCurrentContentItem().setStringValue("The borders of the portrayed paranasal sinuses appear to be intact; there is no evidence of pathosis in these sinuses.\nThe airway appears patent, but there is evidence of marked adenoidal hyperplasia.\nPart of the first and second cervical vertebrae are depicted. No gross abnormalities are seen. There is a normal width of prevertebral soft tissue.\nThe generalized bone pattern and jaw morphology are within the range of normal.");
}


static void generate_09(DSRDocument *doc)
{
    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
    doc->createNewSeriesInStudy("2.16.840.1.113662.2.1.53544936282433.12345.336.16650");
    doc->setStudyDescription("OFFIS Structured Reporting Samples");
    doc->setSeriesDescription("RSNA '95, Picker, CT");

    doc->setPatientName("Smith^Harold");
    doc->setPatientSex("M");
    doc->setPatientID("PIKR750000");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("DT.04", OFFIS_CODING_SCHEME_DESIGNATOR, "CT Report"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Request"));
    doc->getTree().getCurrentContentItem().setStringValue("CT: Spiral Angiography of the Thorax");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.01", OFFIS_CODING_SCHEME_DESIGNATOR, "History"));
    doc->getTree().getCurrentContentItem().setStringValue("46-year-old male presented to the emergency room with parasternal and epigastric pain. A chest X-ray showed widening of the mediastinum. R/O thoracic aneurysm");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Finding"));
    doc->getTree().getCurrentContentItem().setStringValue("There is an aneurysm of the entire thoracic aorta measuring up to 6.2 cm in diameter. Also, there is an intimal flap creating two lumens which is evident on all views extending from the aortic base through the ascending aorta, the aortic arch, and the descending aorta including that portion of the abdomen which is on the films. There are no pulmonary masses identified. The emergency room was immediately notified on this finding and a copy of the films were given to the emergency room so that they could accompany the patient.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.08", OFFIS_CODING_SCHEME_DESIGNATOR, "Conclusion"));
    doc->getTree().getCurrentContentItem().setStringValue("Aneurysm of thoracic aorta with dissection affecting ascending and descending aorta.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IR.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Best illustration of finding"));
    doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue(UID_CTImageStorage, "2.16.840.1.113662.2.1.12345.19950126.112629.1900"));
    doc->getCurrentRequestedProcedureEvidence().addItem("2.16.840.1.113662.2.1.53544936282433.12345.336.16650", "2.16.840.1.113662.2.1.53544936282433.12345.336.1665.9990", UID_CTImageStorage, "2.16.840.1.113662.2.1.12345.19950126.112629.1900");
}


static void generate_10(DSRDocument *doc)
{
    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
    doc->createNewSeriesInStudy("2.16.840.1.113662.4.8796818069641.798806497.93296077602350.10");
    doc->setStudyDescription("OFFIS Structured Reporting Samples");
    doc->setSeriesDescription("RSNA '95, Picker, MR");

    doc->setPatientName("Walz^John^R");
    doc->setPatientSex("M");
    doc->setPatientBirthDate("19781024");
    doc->setPatientID("PIKR752962");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("DT.05", OFFIS_CODING_SCHEME_DESIGNATOR, "MR Report"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Request"));
    doc->getTree().getCurrentContentItem().setStringValue("MRI: Knee");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.01", OFFIS_CODING_SCHEME_DESIGNATOR, "History"));
    doc->getTree().getCurrentContentItem().setStringValue("16 year old with right knee pain after an injury playing basketball.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("SH.06", OFFIS_CODING_SCHEME_DESIGNATOR, "Findings"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Finding"));
    doc->getTree().getCurrentContentItem().setStringValue("The bony structures are intact and normally aligned. There is bruising of the medial femoral condyle with some intrasubstance injury to the medial collateral ligament. The lateral collateral ligament in intact. The anterior cruciate ligament is irregular and slightly lax suggesting a partial tear. It does not appear to be completely torn. The posterior cruciate ligament is intact. The suprapatellar tendons are normal.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Finding"));
    doc->getTree().getCurrentContentItem().setStringValue("There is a tear of the posterior limb of the medial meniscus which communicates with the superior articular surface. The lateral meniscus is intact. There is a Baker's cyst and moderate joint effusion.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Finding"));
    doc->getTree().getCurrentContentItem().setStringValue("Internal derangement of the right knee with marked injury and with partial tear of the ACL; there is a tear of the posterior limb of the medial meniscus. There is a Baker's Cyst and joint effusion and intrasubstance injury to the medial collateral ligament.");

    doc->getTree().goUp();

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IR.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Best illustration of finding"));
    doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue(UID_MRImageStorage, "2.16.840.1.113662.4.8796818069641.806010667.274350678564784069"));
    doc->getCurrentRequestedProcedureEvidence().addItem("2.16.840.1.113662.4.8796818069641.798806497.93296077602350.10", "2.16.840.1.113662.4.8796818069641.806010667.284225018829304176", UID_MRImageStorage, "2.16.840.1.113662.4.8796818069641.806010667.274350678564784069");
}


static void generate_11(DSRDocument *doc)
{
    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
    doc->createNewSeriesInStudy("1.2.840.113654.2.4.4.3.4.119950730134200");
    doc->setStudyDescription("OFFIS Structured Reporting Samples");
    doc->setSeriesDescription("RSNA '95, Kodak, CR");

    doc->setPatientName("Gamage^Mary");
    doc->setPatientSex("F");
    doc->setPatientBirthDate("19950210");
    doc->setPatientID("KHIS001");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("DT.01", OFFIS_CODING_SCHEME_DESIGNATOR, "Radiology Report"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Request"));
    doc->getTree().getCurrentContentItem().setObservationDateTime("199507301342");
    doc->getTree().getCurrentContentItem().setStringValue("Portable chest");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.01", OFFIS_CODING_SCHEME_DESIGNATOR, "History"));
    doc->getTree().getCurrentContentItem().setStringValue("R/O Pneumothorax s/p chest tube insertion");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Finding"));
    doc->getTree().getCurrentContentItem().setStringValue("Over the interval, there has been placement of a left sided chest tube with significant decrease in the left pleural effusion. There is still a persistent pneumothorax although it likely is smaller as well. The right-sided pneumothorax is slightly bigger.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.06", OFFIS_CODING_SCHEME_DESIGNATOR, "Impression"));
    doc->getTree().getCurrentContentItem().setStringValue("Left chest tube insertion with significant decrease in left pleural effusion and pneumothorax.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IR.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Best illustration of finding"));
    doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue(UID_ComputedRadiographyImageStorage, "1.2.840.113654.2.4.4.3.6.1.119950730.1807310414"));
    doc->getCurrentRequestedProcedureEvidence().addItem("1.2.840.113654.2.4.4.3.4.119950730134200", "1.2.840.113654.2.4.4.3.5.119950730134200", UID_ComputedRadiographyImageStorage, "1.2.840.113654.2.4.4.3.6.1.119950730.1807310414");
}


static void generate_12(DSRDocument *doc)
{
    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
    doc->createNewSeriesInStudy("1.2.840.113680.3.103.775.2873347909.282313");
    doc->setStudyDescription("OFFIS Structured Reporting Samples");
    doc->setSeriesDescription("RSNA '95, Acuson, US");

    doc->setPatientName("Napper^Margret");
    doc->setPatientSex("F");
    doc->setPatientBirthDate("19500420");
    doc->setPatientID("ACN000001");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("DT.03", OFFIS_CODING_SCHEME_DESIGNATOR, "Ultrasound Report"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Request"));
    doc->getTree().getCurrentContentItem().setStringValue("Ultrasound: Right lower quadrant");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.01", OFFIS_CODING_SCHEME_DESIGNATOR, "History"));
    doc->getTree().getCurrentContentItem().setStringValue("Right lower quadrant pain r/o appendicitis");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("SH.06", OFFIS_CODING_SCHEME_DESIGNATOR, "Findings"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Finding"));
    doc->getTree().getCurrentContentItem().setStringValue("A retrocecal appendix is present, and markedly enlarged to a diameter of 4.5 mm. There is a central area of decreased echogenicity, possibly representing fluid within the appendix, and there is a somewhat eccentrically placed fluid collection near the appendiceal tip which may represent a very early perforation. There is inflammation of peri-appendiceal fat and a marked increase in blood flow seen on Doppler imaging.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Finding"));
    doc->getTree().getCurrentContentItem().setStringValue("The liver, spleen, kidneys, gallbladder, and pancreas are normal.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Finding"));
    doc->getTree().getCurrentContentItem().setStringValue("Thickening of appendix with surrounding inflammation consistent with appendicitis.");

    doc->getTree().goUp();

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IR.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Best illustration of finding"));
    doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue(UID_RETIRED_UltrasoundImageStorage, "1.2.840.113680.3.103.775.2873347909.282313.2"));
    doc->getCurrentRequestedProcedureEvidence().addItem("1.2.840.113680.3.103.775.2873347909.282313", "1.2.840.113680.3.103.775.2873347909.282313", UID_RETIRED_UltrasoundImageStorage, "1.2.840.113680.3.103.775.2873347909.282313.2");
}


static void generate_13(DSRDocument *doc)
{
    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
    doc->createNewSeriesInStudy("1.2.840.113674.514.212.200");
    doc->setStudyDescription("OFFIS Structured Reporting Samples");
    doc->setSeriesDescription("RSNA '95, GE, CT");

    doc->setPatientName("Wilkins^Charles");
    doc->setPatientSex("M");
    doc->setPatientID("GE0514");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("DT.04", OFFIS_CODING_SCHEME_DESIGNATOR, "CT Report"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Request"));
    doc->getTree().getCurrentContentItem().setStringValue("CT: Abdomen");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.01", OFFIS_CODING_SCHEME_DESIGNATOR, "History"));
    doc->getTree().getCurrentContentItem().setStringValue("Patient is a 45 year old male with abnormal liver function test. Ultrasound evaluation demonstrated a 3 cm lesion in the medial aspect of the right lobe of the liver. The question was raised regarding potential hemangioma.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.03", OFFIS_CODING_SCHEME_DESIGNATOR, "Procedure"));
    doc->getTree().getCurrentContentItem().setStringValue("Serial imaging was obtained in the upper abdomen with the administration of oral and intravenous contrast.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Finding"));
    doc->getTree().getCurrentContentItem().setStringValue("The examination demonstrates a well circumscribed 3 cm lesion present within the medial aspect of the inferior right liver lobe. Initial evaluation demonstrates lack of contrast enhancement. Subsequent imaging (not shown) demonstrated typical contrast enhancement pattern of a benign hemangioma of the liver. The remaining contrast enhancement pattern in the liver is normal. There is normal appearance of the adrenal glands, spleen, kidneys, and pancreas. There is no evidence of liver metastasis. 3 cm nodule present in the inferior medial aspect of right liver lobe. Contrast enhancement pattern consistent with the diagnosis of hemangioma.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IR.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Best illustration of finding"));
    doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue(UID_CTImageStorage, "1.2.840.113674.950809132337081.100"));
    doc->getCurrentRequestedProcedureEvidence().addItem("1.2.840.113674.514.212.200", "1.2.840.113674.514.212.82.300", UID_CTImageStorage, "1.2.840.113674.950809132337081.100");
}


static void generate_14(DSRDocument *doc)
{
    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
    doc->createNewSeriesInStudy("1.2.840.113674.1140.196.200");
    doc->setStudyDescription("OFFIS Structured Reporting Samples");
    doc->setSeriesDescription("RSNA '95, GE, MR");

    doc->setPatientName("Tyson^Bradley");
    doc->setPatientSex("M");
    doc->setPatientID("GE1140");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("DT.05", OFFIS_CODING_SCHEME_DESIGNATOR, "MR Report"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Request"));
    doc->getTree().getCurrentContentItem().setStringValue("Rule out internal derangement.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.03", OFFIS_CODING_SCHEME_DESIGNATOR, "Procedure"));
    doc->getTree().getCurrentContentItem().setStringValue("Sagittal T1 and T2 weighted images and fast spin echo and coronal T2 weighted images are acquired.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Finding"));
    doc->getTree().getCurrentContentItem().setStringValue("Within the posterior horn of the medial meniscus there is minimal increase in signal intensity; however, this does not extend to a joint surface and therefore does not represent a tear. The lateral meniscus is normal. The anterior and posterior cruciate ligaments are visualized and are normal. There is a joint diffusion present with a small superior joint plicae. No interarticular loose bodies are identified. There is no focal marrow edema. The collateral ligaments appear normal.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.06", OFFIS_CODING_SCHEME_DESIGNATOR, "Opinion"));
    doc->getTree().getCurrentContentItem().setStringValue("Joint diffusion with superior joint plicae. Minimal increased signal within the posterior horn of medial meniscus consistent with intermeniscal degeneration. No evidence of meniscal tear.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IR.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Best illustration of finding"));
    doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue(UID_MRImageStorage, "1.2.840.113674.950809133202037.100"));
    doc->getCurrentRequestedProcedureEvidence().addItem("1.2.840.113674.1140.196.200", "1.2.840.113674.1140.196.180.300", UID_MRImageStorage, "1.2.840.113674.950809133202037.100");
}


static void generate_15(DSRDocument *doc)
{
    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
    doc->createNewSeriesInStudy("1.3.12.2.1107.5.8.1.123456789.199507271758050705910");
    doc->setStudyDescription("OFFIS Structured Reporting Samples");
    doc->setSeriesDescription("RSNA '95, Siemens, MR");

    doc->setPatientName("Neubauer^Anna");
    doc->setPatientSex("F");
    doc->setPatientBirthDate("19511104");
    doc->setPatientID("SMS511104");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("DT.05", OFFIS_CODING_SCHEME_DESIGNATOR, "MR Report"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Request"));
    doc->getTree().getCurrentContentItem().setStringValue("MRI: Pelvis");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Finding"));
    doc->getTree().getCurrentContentItem().setStringValue("Visualization of a cervix carcinoma, almost completely permeating into the anterior and posterior labia and extending into the parametrium on the right side. No indication of bladder or rectal involvement. The pelvic wall is free. The tumor does not extend caudally to the vagina. In the vicinity of the left iliac vessel an approximately 1.5 cm round structure can be seen in today's examination, most probably representing the left ovary. If this has been removed because of the ectopic pregnancy, then it is a pathologically enlarged lymph node. There is an ovarian cyst on the right with individual solid contents. Cervical carcinoma permeating into the anterior and posterior labia and extending into the parametrium on the right side.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IR.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Best illustration of finding"));
    doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue(UID_MRImageStorage, "1.3.12.2.1107.5.8.1.123456789.199507271758050707765"));
    doc->getCurrentRequestedProcedureEvidence().addItem("1.3.12.2.1107.5.8.1.123456789.199507271758050705910", "1.3.12.2.1107.5.8.1.123456789.199507271758050706635", UID_MRImageStorage, "1.3.12.2.1107.5.8.1.123456789.199507271758050707765");
}


static void generate_16(DSRDocument *doc)
{
    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
    doc->createNewSeriesInStudy("1.3.12.2.1107.5.8.1.123456789.199507271807160007134");
    doc->setStudyDescription("OFFIS Structured Reporting Samples");
    doc->setSeriesDescription("RSNA '95, Siemens, DS");

    doc->setPatientName("Schmidt^Anna");
    doc->setPatientSex("F");
    doc->setPatientBirthDate("19450102");
    doc->setPatientID("SMS450102");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("DT.01", OFFIS_CODING_SCHEME_DESIGNATOR, "Radiology Report"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Request"));
    doc->getTree().getCurrentContentItem().setStringValue("MRI Angiography: Renal");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.03", OFFIS_CODING_SCHEME_DESIGNATOR, "Procedure"));
    doc->getTree().getCurrentContentItem().setStringValue("Puncture of the Right Common femoral artery and advancement of 4F pigtail catheter via a 0.035 inch guide wire into the abdominal aorta at the level of the renal arteries. Three DSA studies with injections of 25 ml Ultravist 300.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Finding"));
    doc->getTree().getCurrentContentItem().setStringValue("Right Renal artery: Normal location, single vessel supply, high-grade renal artery stenosis approximately 5 mm distal from the origin of the aorta. Left Renal artery: Single vessel supply, significantly larger organ size, no apparent stenosis.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.07", OFFIS_CODING_SCHEME_DESIGNATOR, "Recommendation"));
    doc->getTree().getCurrentContentItem().setStringValue("Documented single hemodynamic resultant stenosis of the right renal artery, Angioplasty suggested.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IR.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Best illustration of finding"));
    doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue(UID_SecondaryCaptureImageStorage, "1.3.12.2.1107.5.8.1.123456789.199507271807160009128"));
    doc->getCurrentRequestedProcedureEvidence().addItem("1.3.12.2.1107.5.8.1.123456789.199507271807160007134", "1.3.12.2.1107.5.8.1.123456789.199507271807160007847", UID_SecondaryCaptureImageStorage, "1.3.12.2.1107.5.8.1.123456789.199507271807160009128");
}


static void generate_17(DSRDocument *doc)
{
    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
    doc->setSpecificCharacterSetType(DSRTypes::CS_Latin1);
    doc->createNewSeriesInStudy("1.3.12.2.1107.5.8.1.123456789.199507271803030520282");
    doc->setStudyDescription("OFFIS Structured Reporting Samples");
    doc->setSeriesDescription("RSNA '95, Siemens, DR");

    doc->setPatientName("Offenmüller^Peter");
    doc->setPatientSex("M");
    doc->setPatientBirthDate("19280302");
    doc->setPatientID("SMS280302");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("DT.01", OFFIS_CODING_SCHEME_DESIGNATOR, "Radiology Report"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Request"));
    doc->getTree().getCurrentContentItem().setStringValue("ERCP");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.01", OFFIS_CODING_SCHEME_DESIGNATOR, "History"));
    doc->getTree().getCurrentContentItem().setStringValue("67 year old male patient, clinical status: two month Jaundice. Query space occupying lesion of the bile duct.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("SH.06", OFFIS_CODING_SCHEME_DESIGNATOR, "Findings"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Finding"));
    doc->getTree().getCurrentContentItem().setStringValue("The pancreaticus major as well as a small accessory duct are well contrasted. Discrete variations in caliber are visible in the region of the pancreatic duct suggestive of chronic inflammatory changes. Variations in caliber are also visible in the multiple small branches. No indications of contrast media depots suggestive of cysts or pseudocysts. No signs of intraluminal concrements.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Finding"));
    doc->getTree().getCurrentContentItem().setStringValue("Well contrasted bile duct and left and right hepatic ducts. Here there are smooth contours without indication of changes in diameter or stenosis. There are no signs of intraluminal concrements. The discrete changes of caliber in the pancreaticus major, accessory ducts and multiple small branches are suggestive of chronic inflammatory changes.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Finding"));
    doc->getTree().getCurrentContentItem().setStringValue("Normal appearance of the bile duct, cystic duct and gall bladder with good drainage of the contrast medium into the duodenum after the endoscopic examination.");

    doc->getTree().goUp();

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IR.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Best illustration of finding"));
    doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue(UID_SecondaryCaptureImageStorage, "1.3.12.2.1107.5.8.1.123456789.199507271803030521934"));
    doc->getCurrentRequestedProcedureEvidence().addItem("1.3.12.2.1107.5.8.1.123456789.199507271803030520282", "1.3.12.2.1107.5.8.1.123456789.199507271803030521007", UID_SecondaryCaptureImageStorage, "1.3.12.2.1107.5.8.1.123456789.199507271803030521934");
}


static void generate_18(DSRDocument *doc)
{
    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
    doc->createNewSeriesInStudy("1.2.392.200036.9125.0.198811291108.7");
    doc->setStudyDescription("OFFIS Structured Reporting Samples");
    doc->setSeriesDescription("RSNA '95, Fuji, CR");

    doc->setPatientName("Tanaka^Hanako");
    doc->setPatientSex("M");
    doc->setPatientBirthDate("19250824");
    doc->setPatientID("FUJI00001");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("DT.01", OFFIS_CODING_SCHEME_DESIGNATOR, "Radiology Report"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Request"));
    doc->getTree().getCurrentContentItem().setStringValue("Upper GI - double contrast stomach");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.01", OFFIS_CODING_SCHEME_DESIGNATOR, "History"));
    doc->getTree().getCurrentContentItem().setStringValue("Gastric pain.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.03", OFFIS_CODING_SCHEME_DESIGNATOR, "Procedure"));
    doc->getTree().getCurrentContentItem().setStringValue("Double contrast technique of the stomach was performed.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("SH.06", OFFIS_CODING_SCHEME_DESIGNATOR, "Findings"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Finding"));
    doc->getTree().getCurrentContentItem().setStringValue("Demonstrated is a gastric wall lesion along the greater curvature of the stomach. Radiating gastric folds are seen extending into the lesion without significant surrounding edema. No additional abnormalities are detected. Given the lack of surrounding edema of this single lesion within the stomach the primary diagnosis of gastric carcinoma remains the main differential diagnosis and must be excluded. Gastric ulcer, although a possibility, is thought to be less likely given the lack of surrounding edema.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Finding"));
    doc->getTree().getCurrentContentItem().setStringValue("Gastric wall mass lesion along greater curvature of the stomach. Pattern compatible with gastric carcinoma.");

    doc->getTree().goUp();

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IR.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Best illustration of finding"));
    doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue(UID_ComputedRadiographyImageStorage, "1.2.392.200036.9125.0.19950720093509"));
    doc->getCurrentRequestedProcedureEvidence().addItem("1.2.392.200036.9125.0.198811291108.7", "1.2.392.200036.9125.0.198811291108.7", UID_ComputedRadiographyImageStorage, "1.2.392.200036.9125.0.19950720093509");
}


static void generate_19(DSRDocument *doc)
{
    doc->createNewDocument(DSRTypes::DT_BasicTextSR);
    doc->createNewSeriesInStudy("1.2.840.113663.19950725.1.0");
    doc->setStudyDescription("OFFIS Structured Reporting Samples");
    doc->setSeriesDescription("RSNA '95, ATL, US");

    doc->setPatientName("Smith^Mary");
    doc->setPatientSex("F");
    doc->setPatientID("ATL000001");

    doc->getTree().addContentItem(DSRTypes::RT_isRoot, DSRTypes::VT_Container);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("DT.03", OFFIS_CODING_SCHEME_DESIGNATOR, "Ultrasound Report"));

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text, DSRTypes::AM_belowCurrent);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Request"));
    doc->getTree().getCurrentContentItem().setStringValue("Breast ultrasound");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.01", OFFIS_CODING_SCHEME_DESIGNATOR, "History"));
    doc->getTree().getCurrentContentItem().setStringValue("Assess palpable mass detected on mammography in the upper outer quadrant of breast in patient with intermittent bloody breast discharge.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Text);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("RE.05", OFFIS_CODING_SCHEME_DESIGNATOR, "Finding"));
    doc->getTree().getCurrentContentItem().setStringValue("The mass is identified in the upper outer quadrant on the breast corresponding to the size and location of the palpable mass. There are internal echoes indicating that this is a solid mass. Hyperechoic foci are present likely indicating that calcifications are present (confirmed on magnification mammography). In the retroareolar region there was identified an intraductal mass with proximal mild ductal dilatation. This is consistent with an intraductal pappiloma.\n1. Solid breast mass with calcifications.\n2. Intraductal pappiloma.");

    doc->getTree().addContentItem(DSRTypes::RT_contains, DSRTypes::VT_Image);
    doc->getTree().getCurrentContentItem().setConceptName(DSRCodedEntryValue("IR.02", OFFIS_CODING_SCHEME_DESIGNATOR, "Best illustration of finding"));
    doc->getTree().getCurrentContentItem().setImageReference(DSRImageReferenceValue(UID_RETIRED_UltrasoundImageStorage, "1.2.840.113663.19950725083017.0.0.0"));
    doc->getCurrentRequestedProcedureEvidence().addItem("1.2.840.113663.19950725.1.0", "1.2.840.113663.19950725.1.0.0", UID_RETIRED_UltrasoundImageStorage, "1.2.840.113663.19950725083017.0.0.0");
}


/*
 *  CVS/RCS Log:
 *  $Log: mkreport.cc,v $
 *  Revision 1.29  2010-10-14 13:15:11  joergr
 *  Updated copyright header. Added reference to COPYRIGHT file.
 *
 *  Revision 1.28  2010-08-09 13:27:21  joergr
 *  Updated data dictionary to 2009 edition of the DICOM standard. From now on,
 *  the official "keyword" is used for the attribute name which results in a
 *  number of minor changes (e.g. "PatientsName" is now called "PatientName").
 *
 *  Revision 1.27  2009-10-13 14:57:52  uli
 *  Switched to logging mechanism provided by the "new" oflog module.
 *
 *  Revision 1.26  2006-08-15 16:40:03  meichel
 *  Updated the code in module dcmsr to correctly compile when
 *    all standard C++ classes remain in namespace std.
 *
 *  Revision 1.25  2005/12/08 15:48:27  meichel
 *  Changed include path schema for all DCMTK header files
 *
 *  Revision 1.24  2004/01/05 14:38:05  joergr
 *  Removed acknowledgements with e-mail addresses from CVS log.
 *
 *  Revision 1.23  2003/10/09 17:48:35  joergr
 *  Added identification information on UCUM coding scheme (see CP 372).
 *
 *  Revision 1.22  2003/09/10 13:19:05  joergr
 *  Replaced PrivateCodingSchemeUID by new CodingSchemeIdenticationSequence as
 *  required by CP 324.
 *
 *  Revision 1.21  2003/08/07 14:19:12  joergr
 *  Adapted for use of OFListConstIterator, needed for compiling with HAVE_STL.
 *
 *  Revision 1.20  2002/05/14 08:17:47  joergr
 *  Added new command line option to create "all" reports with one call.
 *
 *  Revision 1.19  2002/05/07 13:02:12  joergr
 *  Added support for the Current Requested Procedure Evidence Sequence and the
 *  Pertinent Other Evidence Sequence to the dcmsr module.
 *
 *  Revision 1.18  2002/04/16 13:51:59  joergr
 *  Added configurable support for C++ ANSI standard includes (e.g. streams).
 *
 *  Revision 1.17  2002/04/11 13:06:08  joergr
 *  Use the new loadFile() and saveFile() routines from the dcmdata library.
 *
 *  Revision 1.16  2001/12/14 10:37:53  joergr
 *  Re-structured test program to "co-operate" with gcc on Irix 5.
 *
 *  Revision 1.15  2001/10/10 15:31:46  joergr
 *  Additonal adjustments for new OFCondition class.
 *
 *  Revision 1.14  2001/09/28 14:15:01  joergr
 *  Replaced "cerr" by "CERR". Replaced "cout" by "COUT".
 *
 *  Revision 1.13  2001/09/26 13:19:11  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.12  2001/09/26 13:04:34  meichel
 *  Adapted dcmsr to class OFCondition
 *
 *  Revision 1.11  2001/07/02 12:58:04  joergr
 *  Replaced non-standard characters in report "05" and "15".
 *
 *  Revision 1.10  2001/06/13 13:44:47  joergr
 *  Added check for data dictionary to command line tool.
 *
 *  Revision 1.9  2001/06/13 10:09:55  joergr
 *  Added SpecificCharacterSet attribute to report "05".
 *
 *  Revision 1.8  2001/03/28 09:05:44  joergr
 *  Added new sample report (valid structured report with cycle/loop).
 *
 */
