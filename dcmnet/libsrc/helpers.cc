/*
 *
 *  Copyright (C) 2021, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were partly developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module: dcmnet
 *
 *  Author: Michael Onken
 *
 *  Purpose: Collection of helper functions
 *
 */

#include "dcmtk/dcmnet/helpers.h"
#include "dcmtk/dcmnet/lst.h"
#include "dcmtk/dcmnet/assoc.h"
#include "dcmtk/dcmnet/dulstruc.h"

void
destroyAssociatePDUPresentationContextList(LST_HEAD ** pcList)
{
    PRV_PRESENTATIONCONTEXTITEM
        * prvCtx;
    DUL_SUBITEM
        * subItem;

    if (*pcList == NULL)
        return;

    prvCtx = (PRV_PRESENTATIONCONTEXTITEM*)LST_Dequeue(pcList);
    while (prvCtx != NULL) {
        subItem = (DUL_SUBITEM*)LST_Dequeue(&prvCtx->transferSyntaxList);
        while (subItem != NULL) {
            free(subItem);
            subItem = (DUL_SUBITEM*)LST_Dequeue(&prvCtx->transferSyntaxList);
        }
        LST_Destroy(&prvCtx->transferSyntaxList);
        free(prvCtx);
        prvCtx = (PRV_PRESENTATIONCONTEXTITEM*)LST_Dequeue(pcList);
    }
    LST_Destroy(pcList);
}


void
destroyUserInformationLists(DUL_USERINFO * userInfo)
{
    PRV_SCUSCPROLE
        * role;

    role = (PRV_SCUSCPROLE*)LST_Dequeue(&userInfo->SCUSCPRoleList);
    while (role != NULL) {
        free(role);
        role = (PRV_SCUSCPROLE*)LST_Dequeue(&userInfo->SCUSCPRoleList);
    }
    LST_Destroy(&userInfo->SCUSCPRoleList);

    /* extended negotiation */
    delete userInfo->extNegList;
    userInfo->extNegList = NULL;

    /* user identity negotiation */
    delete userInfo->usrIdent;
    userInfo->usrIdent = NULL;
}
