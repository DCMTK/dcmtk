/*
 *
 *  Copyright (C) 1993-2011, OFFIS e.V.
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
 *  Module:  dcmqrdb
 *
 *  Author:  Marco Eichelberg / Didier Lemoine / Gilles Mevel
 *
 *  Purpose: enums and structures used for the database index file
 *
 */

#ifndef DATAPRIV_H
#define DATAPRIV_H

#include "dcmtk/config/osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcmtk/dcmnet/dicom.h"
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcuid.h"
#include "dcmtk/dcmdata/dcdeftag.h"
#include "dcmtk/dcmqrdb/dcmqrdbi.h"

BEGIN_EXTERN_C
#ifdef HAVE_IO_H
#define access my_access    // Workaround to make Visual C++ Compiler happy!
#include <io.h>
#undef access
#endif
END_EXTERN_C

/** types of query keys
 */
enum DB_KEY_TYPE
{
    /// unique key
    UNIQUE_KEY,
    /// required key
    REQUIRED_KEY,
    /// optional key
    OPTIONAL_KEY
};

/** query models
 */
enum DB_QUERY_CLASS
{
    /// patient root Q/R model
    PATIENT_ROOT,
    /// study root Q/R model
    STUDY_ROOT,
    /// patient/study only Q/R model
    PATIENT_STUDY
};

/** types of database keys
 */
enum DB_KEY_CLASS
{
    /// a date entry
    DATE_CLASS,
    /// a time entry
    TIME_CLASS,
    /// a UID entry
    UID_CLASS,
    /// a string entry
    STRING_CLASS,
    /// an entry not belonging to any other class
    OTHER_CLASS
};


/** Level Strings
 */

#define PATIENT_LEVEL_STRING    "PATIENT"
#define STUDY_LEVEL_STRING      "STUDY"
#define SERIE_LEVEL_STRING      "SERIES"
#define IMAGE_LEVEL_STRING      "IMAGE"

/*
** Maximum size of things to put in db records.
** Some values will have vm>1 thus these maximums are
** intended to leave enough space for most common uses.
*/
#define AE_MAX_LENGTH   128     /* Application Entity    */
#define AS_MAX_LENGTH   32      /* Age String            */
#define AT_MAX_LENGTH   32      /* Attribute Tag         */
#define CS_MAX_LENGTH   128     /* Code String           */
#define DA_MAX_LENGTH   80      /* Date                  */
#define DS_MAX_LENGTH   128     /* Decimal String        */
#define DT_MAX_LENGTH   208     /* Date Time             */
#define FL_MAX_LENGTH   32      /* FLoating point single */
#define FD_MAX_LENGTH   64      /* Floating point Double */
#define IS_MAX_LENGTH   96      /* Integer String        */
#define LO_MAX_LENGTH   64      /* Long String           */
#define LT_MAX_LENGTH   10240   /* Long Text             */
#define PN_MAX_LENGTH   64      /* Person Name           */
#define SH_MAX_LENGTH   16      /* Short String          */
#define SL_MAX_LENGTH   32      /* Signed Long           */
#define SS_MAX_LENGTH   16      /* Signed Short          */
#define ST_MAX_LENGTH   1024    /* Short Text            */
#define TM_MAX_LENGTH   128     /* Time                  */
#define UI_MAX_LENGTH   64      /* Unique Identifier     */
#define UL_MAX_LENGTH   32      /* Unsigned Long         */
#define US_MAX_LENGTH   16      /* Unsigned Short        */
#define CS_LABEL_MAX_LENGTH 16  /* Code String - Presentation Label */
#define DESCRIPTION_MAX_LENGTH 128  /* Not related to any particular DICOM attribute */

#define DBC_MAXSTRING           256

#define MAX_MAX_STUDIES         DB_UpperMaxStudies
#define MAX_NUMBER_OF_IMAGES    10000
#define SIZEOF_IDXRECORD        (sizeof (IdxRecord))
#define SIZEOF_STUDYDESC        (sizeof (StudyDescRecord) * MAX_MAX_STUDIES)

/** this class provides a primitive interface for handling a flat DICOM element,
 *  similar to DcmElement, but only for use within the database module
 */
struct DCMTK_DCMQRDB_EXPORT DB_SmallDcmElmt
{
public:
    /// default constructor
    DB_SmallDcmElmt();

    /// pointer to the value field
    char* PValueField ;

    /// value length in bytes
    Uint32 ValueLength ;

    /// attribute tag
    DcmTagKey XTag ;

private:
    /// private undefined copy constructor
    DB_SmallDcmElmt(const DB_SmallDcmElmt& copy);
    /// private undefined copy assignment operator
    DB_SmallDcmElmt& operator=(const DB_SmallDcmElmt& copy);
};

/** this class provides a primitive interface for handling a list of flat DICOM elements,
 *  similar to DcmItem, but only for use within the database module
 */
struct DCMTK_DCMQRDB_EXPORT DB_ElementList
{
    /// default constructor
    DB_ElementList(): elem(), next(NULL) {}

    /// current list element
    DB_SmallDcmElmt elem ;

    /// pointer to next in list
    struct DB_ElementList *next ;

private:
    /// private undefined copy constructor
    DB_ElementList(const DB_ElementList& copy);
    /// private undefined copy assignment operator
    DB_ElementList& operator=(const DB_ElementList& copy);
};

struct DCMTK_DCMQRDB_EXPORT DB_UidList
{
    char *patient ;
    char *study ;
    char *serie ;
    char *image ;
    struct DB_UidList *next ;
};

struct DCMTK_DCMQRDB_EXPORT DB_CounterList
{
    int idxCounter ;
    struct DB_CounterList *next ;
};

struct DCMTK_DCMQRDB_EXPORT DB_FindAttr
{
    DcmTagKey tag ;
    DB_LEVEL level ;
    DB_KEY_TYPE keyAttr ;
    DB_KEY_CLASS keyClass ;

    /* to passify some C++ compilers */
    DB_FindAttr(const DcmTagKey& t, DB_LEVEL l, DB_KEY_TYPE kt, DB_KEY_CLASS kc)
        : tag(t), level(l), keyAttr(kt), keyClass(kc) { }
};

struct DCMTK_DCMQRDB_EXPORT DB_Private_Handle
{
    int pidx ;
    DB_ElementList *findRequestList ;
    DB_ElementList *findResponseList ;
    DB_LEVEL queryLevel ;
    char indexFilename[DBC_MAXSTRING+1] ;
    char storageArea[DBC_MAXSTRING+1] ;
    long maxBytesPerStudy ;
    long maxStudiesAllowed ;
    int idxCounter ;
    DB_CounterList *moveCounterList ;
    int NumberRemainOperations ;
    DB_QUERY_CLASS rootLevel ;
    DB_UidList *uidList ;

    DB_Private_Handle()
    : pidx(0)
    , findRequestList(NULL)
    , findResponseList(NULL)
    , queryLevel(STUDY_LEVEL)
//  , indexFilename()
//  , storageArea()
    , maxBytesPerStudy(0)
    , maxStudiesAllowed(0)
    , idxCounter(0)
    , moveCounterList(NULL)
    , NumberRemainOperations(0)
    , rootLevel(STUDY_ROOT)
    , uidList(NULL)
    {
    }
};

/** this struct defines the structure of each "Study Record" in the index.dat
 *  file maintained by this module. A Study Record is a direct binary copy
 *  of an instance of this struct.
 */
struct DCMTK_DCMQRDB_EXPORT StudyDescRecord
{
    /// Study Instance UID of the study described by this record
    char StudyInstanceUID [UI_MAX_LENGTH+1] ;

    /// combined size (in bytes) of all images of this study in the database
    long StudySize ;

    /// timestamp for last update of this study. Format: output of time(2) converted to double.
    double LastRecordedDate ;

    /// number of images of this study in the database
    int NumberofRegistratedImages ;
};

struct DCMTK_DCMQRDB_EXPORT ImagesofStudyArray
{
    int idxCounter ;
    double RecordedDate ;
    long ImageSize ;
};


/* the following constants define which array element
 * of the param[] array in the IdxRecord structure
 * is linked to which value field in the IdxRecord.
 * numbers must be continuous, starting with 0.
 *
 * The constant NBPARAMETERS must contain the number
 * of array elements to be referenced in param[]
 * (= highest index +1)
 */

#define RECORDIDX_PatientBirthDate                0
#define RECORDIDX_PatientSex                      1
#define RECORDIDX_PatientName                     2
#define RECORDIDX_PatientID                       3
#define RECORDIDX_PatientBirthTime                4
#define RECORDIDX_OtherPatientIDs                 5
#define RECORDIDX_OtherPatientNames               6
#define RECORDIDX_EthnicGroup                     7
#define RECORDIDX_NumberofPatientRelatedStudies   8
#define RECORDIDX_NumberofPatientRelatedSeries    9
#define RECORDIDX_NumberofPatientRelatedInstances 10
#define RECORDIDX_StudyDate                      11
#define RECORDIDX_StudyTime                      12
#define RECORDIDX_StudyID                        13
#define RECORDIDX_StudyDescription               14
#define RECORDIDX_NameOfPhysiciansReadingStudy   15
#define RECORDIDX_AccessionNumber                16
#define RECORDIDX_ReferringPhysicianName         17
#define RECORDIDX_ProcedureDescription           18
#define RECORDIDX_AttendingPhysiciansName        19
#define RECORDIDX_StudyInstanceUID               20
#define RECORDIDX_OtherStudyNumbers              21
#define RECORDIDX_AdmittingDiagnosesDescription  22
#define RECORDIDX_PatientAge                     23
#define RECORDIDX_PatientSize                    24
#define RECORDIDX_PatientWeight                  25
#define RECORDIDX_Occupation                     26
#define RECORDIDX_NumberofStudyRelatedSeries     27
#define RECORDIDX_NumberofStudyRelatedInstances  28
#define RECORDIDX_SeriesNumber                   29
#define RECORDIDX_SeriesInstanceUID              30
#define RECORDIDX_Modality                       31
#define RECORDIDX_ImageNumber                    32
#define RECORDIDX_SOPInstanceUID                 33
#define RECORDIDX_SeriesDate                     34
#define RECORDIDX_SeriesTime                     35
#define RECORDIDX_SeriesDescription              36
#define RECORDIDX_ProtocolName                   37
#define RECORDIDX_OperatorsName                  38
#define RECORDIDX_PerformingPhysicianName        39
#define RECORDIDX_PresentationLabel              40

#define NBPARAMETERS                             41


/** this class manages an instance entry of the index file.
 *  Each instance/image record within the index.dat file is
 *  a direct (binary) copy of this structure.
 */
struct DCMTK_DCMQRDB_EXPORT IdxRecord
{
    /// default constructor
    IdxRecord();

    char    filename                        [DBC_MAXSTRING+1] ;
    char    SOPClassUID                     [UI_MAX_LENGTH+1] ;
    double  RecordedDate ;
    int     ImageSize ;

    DB_SmallDcmElmt param                   [NBPARAMETERS] ;

    char    PatientBirthDate                [DA_MAX_LENGTH+1] ;
    char    PatientSex                      [CS_MAX_LENGTH+1] ;
    char    PatientName                     [PN_MAX_LENGTH+1] ;
    char    PatientID                       [LO_MAX_LENGTH+1] ;
    char    PatientBirthTime                [TM_MAX_LENGTH+1] ;
    char    OtherPatientIDs                 [LO_MAX_LENGTH+1] ;
    char    OtherPatientNames               [PN_MAX_LENGTH+1] ;
    char    EthnicGroup                     [SH_MAX_LENGTH+1] ;
    char    NumberofPatientRelatedStudies   [IS_MAX_LENGTH+1] ;
    char    NumberofPatientRelatedSeries    [IS_MAX_LENGTH+1] ;
    char    NumberofPatientRelatedInstances [IS_MAX_LENGTH+1] ;

    char    StudyDate                       [DA_MAX_LENGTH+1] ;
    char    StudyTime                       [TM_MAX_LENGTH+1] ;
    char    StudyID                         [CS_MAX_LENGTH+1] ;
    char    StudyDescription                [LO_MAX_LENGTH+1] ;
    char    NameOfPhysiciansReadingStudy    [PN_MAX_LENGTH+1] ;

    char    AccessionNumber                 [CS_MAX_LENGTH+1] ;
    char    ReferringPhysicianName          [PN_MAX_LENGTH+1] ;
    char    ProcedureDescription            [LO_MAX_LENGTH+1] ;
    char    AttendingPhysiciansName         [PN_MAX_LENGTH+1] ;
    char    StudyInstanceUID                [UI_MAX_LENGTH+1] ;
    char    OtherStudyNumbers               [IS_MAX_LENGTH+1] ;
    char    AdmittingDiagnosesDescription   [LO_MAX_LENGTH+1] ;
    char    PatientAge                      [AS_MAX_LENGTH+1] ;
    char    PatientSize                     [DS_MAX_LENGTH+1] ;
    char    PatientWeight                   [DS_MAX_LENGTH+1] ;
    char    Occupation                      [SH_MAX_LENGTH+1] ;
    char    NumberofStudyRelatedSeries      [IS_MAX_LENGTH+1] ;
    char    NumberofStudyRelatedInstances   [IS_MAX_LENGTH+1] ;

    char    SeriesNumber                    [IS_MAX_LENGTH+1] ;
    char    SeriesInstanceUID               [UI_MAX_LENGTH+1] ;
    char    Modality                        [CS_MAX_LENGTH+1] ;

    char    ImageNumber                     [IS_MAX_LENGTH+1] ;
    char    SOPInstanceUID                  [UI_MAX_LENGTH+1] ;

    char    SeriesDate                      [DA_MAX_LENGTH+1] ;
    char    SeriesTime                      [TM_MAX_LENGTH+1] ;
    char    SeriesDescription               [LO_MAX_LENGTH+1] ;
    char    ProtocolName                    [LO_MAX_LENGTH+1] ;
    char    OperatorsName                   [PN_MAX_LENGTH+1] ;
    char    PerformingPhysicianName         [PN_MAX_LENGTH+1] ;
    char    PresentationLabel               [CS_LABEL_MAX_LENGTH+1] ;

    DVIFhierarchyStatus hstat;

    // Not related to any particular DICOM attribute !
    char    InstanceDescription             [DESCRIPTION_MAX_LENGTH+1] ;

private:
    /* undefined */ IdxRecord(const IdxRecord& copy);
    /* undefined */ IdxRecord& operator=(const IdxRecord& copy);
};


#endif
