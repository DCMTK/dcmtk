/*
 * 
 * Author: Gerd Ehlers      Created:  05-29-94
 *                          Modified: 02-07-95
 * 
 * Module: dcmodtbl.cc
 * 
 * Purpose:
 * Implemantation of class DcmModuleTable
 * 
 * 
 * Last Update:	  $Author: hewett $
 * Revision:      $Revision: 1.1 $
 * Status:        $State: Exp $
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <string.h>

#include "dcmodtbl.h"
#include "dcdebug.h"

extern "C" {
#include "ddicrefz.ix"  // "ddicrefz.ix", weil "ddicrefz.i" sehr gross ist
}




extern S_TableRef TableRefs[];
extern short DIM_OF_TableRefs;



// ************************************************************


DcmModuleTable::DcmModuleTable( char *moduleEntry )
{
Bdebug((6, "dcmodule:DcmModuleTable(moduleEntry=[%s])", moduleEntry ));

    errorFlag = EC_Normal;
    short i = 0;
    while ((i < DIM_OF_TableRefs)
            && (strcmp(TableRefs[i].Value, moduleEntry) != 0))
        i++;
    if ((i < DIM_OF_TableRefs)
         && (strcmp(TableRefs[i].Value, moduleEntry) == 0))
    {
        TableRefsIndex = i;
        ModuleTableDescriptor_P = TableRefs[i].ModD_P;
debug(( 7, "found index i=[%d] in first column", i ));

    }
    else
    {
        i = 0;
        while ((i < DIM_OF_TableRefs)
                && (strcmp(TableRefs[i].Symbol, moduleEntry) != 0))
            i++;
        if ((i < DIM_OF_TableRefs)
             && (strcmp(TableRefs[i].Symbol, moduleEntry) == 0))
        {
            TableRefsIndex = i;
            ModuleTableDescriptor_P = TableRefs[i].ModD_P;
debug(( 7, "found index i=[%d] in second column", i ));

        }
        else
        {
            TableRefsIndex = -1;
            errorFlag = EC_IllegalCall;
            ModuleTableDescriptor_P = (S_ListDesc5*)NULL;
debug(( 7, "no index found" ));

        }
    }

    if (    ModuleTableDescriptor_P != (S_ListDesc5*)NULL
         && ModuleTableDescriptor_P->ListType == E_ListDesc4 )
    {
        ModuleTableDescriptor_ListType = ModuleTableDescriptor_P->ListType;
        ModuleTable_MaxNum = ModuleTableDescriptor_P->NoOf;
        ModuleTable_P = ModuleTableDescriptor_P->ModT_P;
debug(( 6, "ModuleTable reference found: ModuleTable_P is valid" ));

    }
    else
    {
        ModuleTableDescriptor_ListType = E_ListDesc4;  // Es gibt kein UNDEFINED
        ModuleTable_MaxNum = 0;
        ModuleTable_P = (S_ListDesc4*)NULL;
    }

    ModuleAttributeGroupDescriptor_P = (S_ListDesc3*)NULL;
    ModuleAttributeGroupDescriptor_ListType = E_ListDesc2; // kein UNDEFINED
    ModuleAttributeGroup_MaxNum = 0;
    ModuleAttributeGroup_P = (S_ListDesc2*)NULL;
    ModuleAttributeDescriptor_P = (S_ListDesc1*)NULL;
    ModuleAttributeDescriptor_ListType = E_ListDesc1; //Es gibt kein UNDEFINED
    ModuleAttribute_MaxNum = 0;
    ModuleAttribute_P = NULL;
    ModuleAttributeCond_MaxNum = 0;
    ModuleAttributeCond_P = NULL;

Edebug(());

}


// ************************************************************


DcmModuleTable::DcmModuleTable( const DcmModuleTable &newtable )
{
Bdebug((6, "dcmodule:DcmModuleTable(DcmModuleTable&)" ));

    TableRefsIndex           = newtable.TableRefsIndex;

    ModuleTableDescriptor_P  = newtable.ModuleTableDescriptor_P;
    ModuleTableDescriptor_ListType
                             = newtable.ModuleTableDescriptor_ListType;

    ModuleTable_P            = newtable.ModuleTable_P;
    ModuleTable_MaxNum       = newtable.ModuleTable_MaxNum;
    ModuleTable_Num          = newtable.ModuleTable_Num;

    ModuleAttributeGroupDescriptor_P
                             = newtable.ModuleAttributeGroupDescriptor_P;
    ModuleAttributeGroupDescriptor_ListType
                             = newtable.ModuleAttributeGroupDescriptor_ListType;

    ModuleAttributeGroup_P   = newtable.ModuleAttributeGroup_P;
    ModuleAttributeGroup_MaxNum
                             = newtable.ModuleAttributeGroup_MaxNum;
    ModuleAttributeGroup_Num = newtable.ModuleAttributeGroup_Num;

    ModuleAttributeDescriptor_P
                             = newtable.ModuleAttributeDescriptor_P;
    ModuleAttributeDescriptor_ListType
                             = newtable.ModuleAttributeDescriptor_ListType;

    ModuleAttribute_P        = newtable.ModuleAttribute_P;
    ModuleAttribute_MaxNum   = newtable.ModuleAttribute_MaxNum;
    ModuleAttribute_Num      = newtable.ModuleAttribute_Num;

    ModuleAttributeCond_P    = newtable.ModuleAttributeCond_P;
    ModuleAttributeCond_MaxNum
                             = newtable.ModuleAttributeCond_MaxNum;
    ModuleAttributeCond_Num  = newtable.ModuleAttributeCond_Num;

    errorFlag                = newtable.errorFlag;

Edebug(());

}


// ************************************************************


DcmModuleTable::~DcmModuleTable()
{
Bdebug((6, "dcmodule:DcmModuleTable::~DcmModuleTable()" ));
Edebug(());

}


// ************************************************************


char* DcmModuleTable::getNameTableValue()
{
    if ( TableRefsIndex >= 0 && TableRefsIndex < DIM_OF_TableRefs )
    {
        errorFlag = EC_Normal;
        return TableRefs[ TableRefsIndex ].Value;
    }
    else
    {
        errorFlag = EC_IllegalCall;
        return "";
    }
}


// ************************************************************


char* DcmModuleTable::getNameTableSymbol()
{
    if ( TableRefsIndex >= 0 && TableRefsIndex < DIM_OF_TableRefs )
    {
        errorFlag = EC_Normal;
        return TableRefs[ TableRefsIndex ].Symbol;
    }
    else
    {
        errorFlag = EC_IllegalCall;
        return "";
    }
}


// ************************************************************


E_Condition DcmModuleTable::setModuleTable( short numberOfEntry )
{
Bdebug((5, "dcmodule:DcmModuleTable::setModuleTable(numberOfEntry=[%d])",
           numberOfEntry ));

    errorFlag = EC_Normal;
    S_ListDesc4 *ModT = (S_ListDesc4*)NULL;
    if ( ModuleTable_P != (S_ListDesc4*)NULL
         && numberOfEntry >= 0
         && numberOfEntry < getModuleTableMaxNum() )
    {
        ModuleTable_Num = numberOfEntry;
        ModT = &ModuleTable_P[ numberOfEntry ];
        ModuleAttributeGroupDescriptor_P = ModT->MGT_P;

        if (    ModuleAttributeGroupDescriptor_P != (S_ListDesc3*)NULL
             && ModuleAttributeGroupDescriptor_P->ListType == E_ListDesc2 )
        {
            ModuleAttributeGroupDescriptor_ListType
                                 = ModuleAttributeGroupDescriptor_P->ListType;
            ModuleAttributeGroup_MaxNum = ModuleAttributeGroupDescriptor_P->NoOf;
            ModuleAttributeGroup_P = ModuleAttributeGroupDescriptor_P->AGT_P;
debug(( 4, "valid ModuleAttributeGroupDescriptor_P found." ));

        }
        else
        {
            ModuleAttributeGroupDescriptor_ListType = E_ListDesc2; // kein UNDEFINED
            ModuleAttributeGroup_MaxNum = 0;
            ModuleAttributeGroup_P = (S_ListDesc2*)NULL;
        }
    }
    else
    {
        ModuleTable_Num = 0;
        ModuleAttributeGroupDescriptor_P = (S_ListDesc3*)NULL;
        ModuleAttributeGroupDescriptor_ListType = E_ListDesc2; // kein UNDEFINED
        ModuleAttributeGroup_MaxNum = 0;
        ModuleAttributeGroup_P = (S_ListDesc2*)NULL;
        errorFlag = EC_IllegalCall;
    }
Edebug(());

    return errorFlag;
}


// ************************************************************


S_ListDesc3* DcmModuleTable::getModuleTable_MAGD( short numberOfEntry )
{
    S_ListDesc3 *ModAGD = (S_ListDesc3*)NULL;
    errorFlag = setModuleTable( numberOfEntry );
    if ( errorFlag == EC_Normal )
        ModAGD = ModuleTable_P[ numberOfEntry ].MGT_P;
    return ModAGD;
}


// ************************************************************


ETypeMod DcmModuleTable::getModuleTableETypeMod( short numberOfEntry )
{
    ETypeMod ETypeM = ETypeM_UNKNOWN;
    errorFlag = setModuleTable( numberOfEntry );
    if ( errorFlag == EC_Normal )
        ETypeM = ModuleTable_P[ numberOfEntry ].TypeMG;
    return ETypeM;
}


// ************************************************************


E_Condition DcmModuleTable::setModuleAttributeGroup( short numberOfEntry )
{
Bdebug((5, "dcmodule:DcmModuleTable::setModuleAttributeGroup(numberOfEntry=[%d])",
           numberOfEntry ));

    errorFlag = EC_Normal;
    S_ListDesc2 *AttrG = (S_ListDesc2*)NULL;
    if ( ModuleAttributeGroup_P != (S_ListDesc2*)NULL
         && numberOfEntry >= 0
         && numberOfEntry < getModuleAttributeGroupMaxNum() )
    {
        ModuleAttributeGroup_Num = numberOfEntry;
        AttrG = &ModuleAttributeGroup_P[ numberOfEntry ];
//        ModuleAttributeDescriptor_P = AttrG->AGT_P;
        ModuleAttributeDescriptor_P = *AttrG;

        if ( ModuleAttributeDescriptor_P != (S_ListDesc1*)NULL )
        {
            ModuleAttributeDescriptor_ListType
                                     = ModuleAttributeDescriptor_P->ListType;
            ModuleAttribute_MaxNum = ModuleAttributeDescriptor_P->NoOf;
            ModuleAttribute_P = ModuleAttributeDescriptor_P->List_P;
            ModuleAttributeCond_MaxNum = ModuleAttributeDescriptor_P->NoOfC;
            ModuleAttributeCond_P = ModuleAttributeDescriptor_P->ListC_P;
debug(( 5, "valid ModuleAttributeDescriptor_P found." ));

        }
        else
        {
            ModuleAttributeDescriptor_ListType = E_ListDesc1; //Es gibt kein UNDEFINED
            ModuleAttribute_MaxNum = 0;
            ModuleAttribute_P = NULL;
            ModuleAttributeCond_MaxNum = 0;
            ModuleAttributeCond_P = NULL;
        }
    }
    else
    {
        ModuleAttributeGroup_Num = 0;
        ModuleAttributeDescriptor_P = (S_ListDesc1*)NULL;
        ModuleAttributeDescriptor_ListType = E_ListDesc1; //Es gibt kein UNDEFINED
        ModuleAttribute_MaxNum = 0;
        ModuleAttribute_P = NULL;
        ModuleAttributeCond_MaxNum = 0;
        ModuleAttributeCond_P = NULL;
        errorFlag = EC_IllegalCall;
    }
Edebug(());

    return errorFlag;
}


// ************************************************************


S_ListDesc1* DcmModuleTable::getModuleAttributeGroup_MAD( short numberOfEntry )
{
    S_ListDesc1 *AttrD = (S_ListDesc1*)NULL;
    errorFlag = setModuleAttributeGroup( numberOfEntry );
    if ( errorFlag == EC_Normal )
//        AttrD = ModuleAttributeGroup_P[ numberOfEntry ].AGT_P;
        AttrD = ModuleAttributeGroup_P[ numberOfEntry ];
    return AttrD;
}


// ************************************************************


ETag DcmModuleTable::getModuleAttributeDescriptor_ETag()
{
    errorFlag = EC_Normal;
    ETag xtag = ET_UNKNOWN;
    if ( ModuleAttributeDescriptor_P != NULL )
    {
        xtag = ModuleAttributeDescriptor_P->XTag;
    }
    else
    {
        errorFlag = EC_IllegalCall;
    }
    return xtag;
}


// ************************************************************


ETypeAttr DcmModuleTable::getModuleAttributeDescriptor_ETypeAttr()
{
    errorFlag = EC_Normal;
    ETypeAttr etype = EType_UNKNOWN;
    if ( ModuleAttributeDescriptor_P != NULL )
    {
        etype = ModuleAttributeDescriptor_P->Type;
    }
    else
    {
        errorFlag = EC_IllegalCall;
    }
    return etype;
}


// ************************************************************


E_Condition DcmModuleTable::setModuleAttribute( short numberOfEntry )
{
Bdebug((5, "dcmodule:DcmModuleTable::setModuleAttribute(numberOfEntry=[%d])",
           numberOfEntry ));

    errorFlag = EC_Normal;
    if ( ModuleAttribute_P != NULL
         && numberOfEntry >= 0
         && numberOfEntry < getModuleAttributeMaxNum() )
    {
        ModuleAttribute_Num = numberOfEntry;
debug(( 3, "ModuleAttribute[%d] set.", numberOfEntry ));

    }
    else
    {
        ModuleAttribute_Num = 0;
        errorFlag = EC_IllegalCall;
    }
Edebug(());

    return errorFlag;
}


// ************************************************************


char* DcmModuleTable::getModuleAttribute_str( short numberOfEntry )
{
    char *AttrStr = (char*)NULL;
    errorFlag = setModuleAttribute( numberOfEntry );
    if (    errorFlag == EC_Normal
         && getModuleAttributeDescriptor_ListType() == E_ListIsVal1Member
       )
        AttrStr = ((char**)ModuleAttribute_P)[ numberOfEntry ];
    return AttrStr;
}


// ************************************************************


S_List2Str* DcmModuleTable::getModuleAttribute_List2Str( short numberOfEntry )
{                                     // ..._List2Str(no)->Value  (char*)
                                      // ..._List2Str(no)->Name   (char*)
    S_List2Str *Attr = (S_List2Str*)NULL;
    errorFlag = setModuleAttribute( numberOfEntry );
    if (    errorFlag == EC_Normal
         && getModuleAttributeDescriptor_ListType()
                                                == E_ListIsVal1MemberWithAddVal
       )
        Attr = &((S_List2Str*)ModuleAttribute_P)[ numberOfEntry ];
    return Attr;
}


// ************************************************************


S_TagType* DcmModuleTable::getModuleAttribute_TagType( short numberOfEntry )
{                                     // ..._TagType(no)->Xtag   (ETag)
                                      // ..._TagType(no)->Type   (ETypeAttr)
    S_TagType *Attr = (S_TagType*)NULL;
    errorFlag = setModuleAttribute( numberOfEntry );
    if (    errorFlag == EC_Normal
         && (    getModuleAttributeDescriptor_ListType()
                                               == E_ListIsTagTypeMember
              || getModuleAttributeDescriptor_ListType()
                                               == E_ListIsTagTypeMemberWithCond
            )
       )
        Attr = &((S_TagType*)ModuleAttribute_P)[ numberOfEntry ];
    return Attr;
}


// ************************************************************


E_Condition DcmModuleTable::setModuleAttributeCond( short numberOfEntry )
{
    errorFlag = EC_Normal;
    if ( ModuleAttributeCond_P != NULL
         && numberOfEntry >= 0
         && numberOfEntry < getModuleAttributeCondMaxNum() )
    {
        ModuleAttributeCond_Num = numberOfEntry;

    }
    else
    {
        ModuleAttributeCond_Num = 0;
        errorFlag = EC_IllegalCall;
    }
    return errorFlag;
}


// ************************************************************


char* DcmModuleTable::getModuleAttributeCond_str( short numberOfEntry )
{
    char *AttrStr = (char*)NULL;
    errorFlag = setModuleAttributeCond( numberOfEntry );
    if (    errorFlag == EC_Normal
         && getModuleAttributeDescriptor_ListType() == E_ListIsVal1Member
       )
        AttrStr = ((char**)ModuleAttributeCond_P)[ numberOfEntry ];
    return AttrStr;
}


// ************************************************************


S_List2Str* DcmModuleTable::getModuleAttributeCond_List2Str( short numberOfEntry )
{                                     // ..._List2Str(no)->Value  (char*)
                                      // ..._List2Str(no)->Name   (char*)
    S_List2Str *Attr = (S_List2Str*)NULL;
    errorFlag = setModuleAttributeCond( numberOfEntry );
    if (    errorFlag == EC_Normal
         && getModuleAttributeDescriptor_ListType()
                                                == E_ListIsVal1MemberWithAddVal
       )
        Attr = &((S_List2Str*)ModuleAttributeCond_P)[ numberOfEntry ];
    return Attr;
}


// ************************************************************


S_TagType* DcmModuleTable::getModuleAttributeCond_TagType( short numberOfEntry )
{                                     // ..._TagType(no)->Xtag   (ETag)
                                      // ..._TagType(no)->Type   (ETypeAttr)
    S_TagType *Attr = (S_TagType*)NULL;
    errorFlag = setModuleAttributeCond( numberOfEntry );
    if (    errorFlag == EC_Normal
         && (    getModuleAttributeDescriptor_ListType()
                                               == E_ListIsTagTypeMember
              || getModuleAttributeDescriptor_ListType()
                                               == E_ListIsTagTypeMemberWithCond
            )
       )
        Attr = &((S_TagType*)ModuleAttributeCond_P)[ numberOfEntry ];
    return Attr;
}


// ************************************************************


