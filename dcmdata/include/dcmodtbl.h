/*
 * 
 * Author: Gerd Ehlers      Created:  05-29-94
 *                          Modified: 02-07-95
 * 
 * Module: dcmodtbl.h
 * 
 * Purpose:
 * Interface of class DcmModuleTable
 * 
 * 
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:        $State: Exp $
 *
 */

#ifndef DCMODTBL_H
#define DCMODTBL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "dctypes.h"
#include "dcdicenu.h"

typedef enum {
    E_ListDesc1, E_ListDesc2, E_ListDesc4, E_ListMeta,
    E_ListIsTagMember,
    E_ListIsTagTypeMember,
    E_ListIsTagMemberWithCond,
    E_ListIsTagTypeMemberWithCond,
    E_ListIsVal1Member,
    E_ListIsVal2Member,
    E_ListIsVal3Member,
    E_ListIsVal4Member,
    E_ListIsVal1MemberWithAddVal,
    E_ListIsVal2MemberWithAddVal,
    E_ListIsVal3MemberWithAddVal,
    E_ListIsVal4MemberWithAddVal,
    E_ListIsVal1MemberAndCond,
    E_ListIsVal1MemberWithAddValAndCond
} EList;


typedef enum {
    EType_UNKNOWN,

    EType_1,   EType_1C,   EType_2,     EType_2C,  EType_3,
    EType_M_M, EType_MC_M,
    EType_n_M, EType_n_MC,
    EType_U_M, EType_U_MC, EType_U_U,
    EType_1_1, EType_1_1C, EType_1C_1C,
    EType_2_2, EType_2_2C, EType_2C_1C,
    EType_3_1, EType_3_1C, EType_3_2,   EType_3_3,
    EType_n_1, EType_n_1C, EType_n_2,   EType_n_3,
    EType_C,   EType_M,    EType_O,     EType_R,   EType_U,
    EType_X
} ETypeAttr;


typedef enum {
    ETypeM_UNKNOWN,

    ETypeM_C, ETypeM_M, ETypeM_U, ETypeM_X,

    ETypeM_IECurve,
    ETypeM_IECurveNoOtherIE,
    ETypeM_IEEquipment,
    ETypeM_IEFrameOfReference,
    ETypeM_IEImage,
    ETypeM_IEImageNoOtherIE,
    ETypeM_IEModalityLUT,
    ETypeM_IEOverlay,
    ETypeM_IEOverlayNoOtherIE,
    ETypeM_IEPatient,
    ETypeM_IESeries,
    ETypeM_IEStudy,
    ETypeM_IEVOILUT,

    ETypeM_NAction_Print_1_M_M,
    ETypeM_NAction_Print_1_U_U,
    ETypeM_NCreate_M_M,

    ETypeM_NEventReportStudyCreated_1_M_M,
    ETypeM_NEventReportStudyScheduled_2_M_M,
    ETypeM_NEventReportPatientArrived_3_M_M,
    ETypeM_NEventReportStudyStarted_4_M_M,
    ETypeM_NEventReportStudyCompleted_5_M_M,
    ETypeM_NEventReportStudyVerified_6_M_M,
    ETypeM_NEventReportStudyRead_7_M_M,
    ETypeM_NEventReportStudyDeleted_8_M_M,
    ETypeM_NEventReportStudyUpdated_9_M_M,

    ETypeM_NEventReportPatientCreated_1_M_M,
    ETypeM_NEventReportPatientDeleted_2_M_M,
    ETypeM_NEventReportPatientUpdated_3_M_M,

    ETypeM_NEventReportVisitCreated_1_M_M,
    ETypeM_NEventReportVisitScheduled_2_M_M,
    ETypeM_NEventReportPatientAdmitted_3_M_M,
    ETypeM_NEventReportPatientTransferred_4_M_M,
    ETypeM_NEventReportPatientDischarged_5_M_M,
    ETypeM_NEventReportVisitDeleted_6_M_M,
    ETypeM_NEventReportVisitUpdated_7_M_M,

    ETypeM_NEventReportResultsCreated_1_M_M,
    ETypeM_NEventReportResultsDeleted_2_M_M,
    ETypeM_NEventReportResultsUpdated_3_M_M,

    ETypeM_NEventReportInterpretationCreated_1_M_M,
    ETypeM_NEventReportInterpretationRecorded_2_M_M,
    ETypeM_NEventReportInterpretationTranscribed_3_M_M,
    ETypeM_NEventReportInterpretationApproved_4_M_M,
    ETypeM_NEventReportInterpretationUpdated_6_M_M,

    ETypeM_NEventReportPending_1_M_M,
    ETypeM_NEventReportPrinting_2_M_M,
    ETypeM_NEventReportDone_3_M_M,
    ETypeM_NEventReportFailure_4_M_M,

    ETypeM_NEventReportWarning_2_M_M,
    ETypeM_NEventReportFailure_3_M_M,

    ETypeM_NGet_U_M,

    ETypeM_NSet_M_M,
    ETypeM_NSet_U_M,
    ETypeM_NSet_U_U,

    ETypeM_M_M,

    ETypeM_MCM,
    ETypeM_nCM,
    ETypeM_nnn
} ETypeMod;


typedef   ETag    S_Tag;


typedef struct {
    ETag          XTag;
    ETypeAttr     Type;
} S_TagType;


typedef struct {
    char           *Value;
    char           *Name;
} S_List2Str;


typedef struct {
    EList         ListType;
    short         NoOf;
    void          *List_P;     /* S_TagType | S_Tag */
    ETag          XTag;
    ETypeAttr     Type;
    short         NoOfC;
    void          *ListC_P;    /* S_TagType | S_Tag | S_ListDesc1 */
} S_ListDesc1;


/*
typedef struct {
    S_ListDesc1   *AGT_P;
} S_ListDesc2;
*/


typedef S_ListDesc1* S_ListDesc2;


typedef struct {
    EList         ListType;
    short         NoOf;
    S_ListDesc2   *AGT_P;
} S_ListDesc3;


typedef struct {
    S_ListDesc3   *MGT_P;
    ETypeMod      TypeMG;
} S_ListDesc4;


typedef struct {
    EList         ListType;
    short         NoOf;
    S_ListDesc4   *ModT_P;
} S_ListDesc5;


typedef struct {
    char          *Value;
    char          *Symbol;
    S_ListDesc5   *ModD_P;
} S_TableRef;


// ************************************************************


class DcmModuleTable {
    short       TableRefsIndex;

    S_ListDesc5 *ModuleTableDescriptor_P;
    EList        ModuleTableDescriptor_ListType;           // == E_ListDesc4

    S_ListDesc4 *ModuleTable_P;
    short        ModuleTable_MaxNum;
    short        ModuleTable_Num;

    S_ListDesc3 *ModuleAttributeGroupDescriptor_P;
    EList        ModuleAttributeGroupDescriptor_ListType;  // == E_ListDesc2

    S_ListDesc2 *ModuleAttributeGroup_P;
    short        ModuleAttributeGroup_MaxNum;
    short        ModuleAttributeGroup_Num;

    S_ListDesc1 *ModuleAttributeDescriptor_P;
    EList        ModuleAttributeDescriptor_ListType;       // variable

    void        *ModuleAttribute_P;      // *char[] | S_List2Str[] | S_TagType[]
    short        ModuleAttribute_MaxNum;
    short        ModuleAttribute_Num;

    void        *ModuleAttributeCond_P;  // *char[] | S_List2Str[] | S_TagType[]
    short        ModuleAttributeCond_MaxNum;
    short        ModuleAttributeCond_Num;

    E_Condition errorFlag;

public:
    DcmModuleTable( char *moduleEntry );
    DcmModuleTable( const DcmModuleTable &newtable );
    ~DcmModuleTable();

    char*        getNameTableValue();
    char*        getNameTableSymbol();

    short        getModuleTableMaxNum()         { return ModuleTable_MaxNum; }
    E_Condition  setModuleTable(                  short numberOfEntry );
    S_ListDesc3* getModuleTable_MAGD(             short numberOfEntry );
    ETypeMod     getModuleTableETypeMod(          short numberOfEntry );

    short        getModuleAttributeGroupMaxNum()
                                           { return ModuleAttributeGroup_MaxNum; }
    E_Condition  setModuleAttributeGroup(         short numberOfEntry );
    S_ListDesc1* getModuleAttributeGroup_MAD(     short numberOfEntry );

    EList        getModuleAttributeDescriptor_ListType()
                                  { return ModuleAttributeDescriptor_ListType; }
    ETag         getModuleAttributeDescriptor_ETag();
    ETypeAttr    getModuleAttributeDescriptor_ETypeAttr();

    short        getModuleAttributeMaxNum()     { return ModuleAttribute_MaxNum; }
    E_Condition  setModuleAttribute(              short numberOfEntry );
    char*        getModuleAttribute_str(          short numberOfEntry );
    S_List2Str*  getModuleAttribute_List2Str(     short numberOfEntry );
    S_TagType*   getModuleAttribute_TagType(      short numberOfEntry );

    short        getModuleAttributeCondMaxNum() { return ModuleAttribute_MaxNum; }
    E_Condition  setModuleAttributeCond(          short numberOfEntry );
    char*        getModuleAttributeCond_str(      short numberOfEntry );
    S_List2Str*  getModuleAttributeCond_List2Str( short numberOfEntry );
    S_TagType*   getModuleAttributeCond_TagType(  short numberOfEntry );

    E_Condition  error()                        { return errorFlag; }
};


// ************************************************************


#endif // DCMODTBL_H
