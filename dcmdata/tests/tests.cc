/*
 *
 *  Copyright (C) 2011-2013, OFFIS e.V.
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
 *  Module:  dcmdata
 *
 *  Author:  Uli Schlachter
 *
 *  Purpose: main test program
 *
 */

#include "dcmtk/config/osconfig.h"

#include "dcmtk/ofstd/oftest.h"

OFTEST_REGISTER(dcmdata_partialElementAccess);
OFTEST_REGISTER(dcmdata_i2d_bmp);
OFTEST_REGISTER(dcmdata_checkStringValue);
OFTEST_REGISTER(dcmdata_determineVM);
OFTEST_REGISTER(dcmdata_getValueFromString);
OFTEST_REGISTER(dcmdata_pathAccess);
OFTEST_REGISTER(dcmdata_dateTime);
OFTEST_REGISTER(dcmdata_decimalString_1);
OFTEST_REGISTER(dcmdata_decimalString_2);
OFTEST_REGISTER(dcmdata_decimalString_3);
OFTEST_REGISTER(dcmdata_decimalString_4);
OFTEST_REGISTER(dcmdata_personName);
OFTEST_REGISTER(dcmdata_uniqueIdentifier_1);
OFTEST_REGISTER(dcmdata_uniqueIdentifier_2);
OFTEST_REGISTER(dcmdata_elementLength_EVR_AE);
OFTEST_REGISTER(dcmdata_elementLength_EVR_AS);
OFTEST_REGISTER(dcmdata_elementLength_EVR_AT);
OFTEST_REGISTER(dcmdata_elementLength_EVR_CS);
OFTEST_REGISTER(dcmdata_elementLength_EVR_DA);
OFTEST_REGISTER(dcmdata_elementLength_EVR_DS);
OFTEST_REGISTER(dcmdata_elementLength_EVR_DT);
OFTEST_REGISTER(dcmdata_elementLength_EVR_FD);
OFTEST_REGISTER(dcmdata_elementLength_EVR_FL);
OFTEST_REGISTER(dcmdata_elementLength_EVR_IS);
OFTEST_REGISTER(dcmdata_elementLength_EVR_LO);
OFTEST_REGISTER(dcmdata_elementLength_EVR_LT);
OFTEST_REGISTER(dcmdata_elementLength_EVR_OB);
OFTEST_REGISTER(dcmdata_elementLength_EVR_OD);
OFTEST_REGISTER(dcmdata_elementLength_EVR_OF);
OFTEST_REGISTER(dcmdata_elementLength_EVR_OW);
OFTEST_REGISTER(dcmdata_elementLength_EVR_OverlayData);
OFTEST_REGISTER(dcmdata_elementLength_EVR_PN);
OFTEST_REGISTER(dcmdata_elementLength_EVR_PixelData);
OFTEST_REGISTER(dcmdata_elementLength_EVR_SH);
OFTEST_REGISTER(dcmdata_elementLength_EVR_SL);
OFTEST_REGISTER(dcmdata_elementLength_EVR_SQ);
OFTEST_REGISTER(dcmdata_elementLength_EVR_SS);
OFTEST_REGISTER(dcmdata_elementLength_EVR_ST);
OFTEST_REGISTER(dcmdata_elementLength_EVR_TM);
OFTEST_REGISTER(dcmdata_elementLength_EVR_UI);
OFTEST_REGISTER(dcmdata_elementLength_EVR_UL);
OFTEST_REGISTER(dcmdata_elementLength_EVR_UN);
OFTEST_REGISTER(dcmdata_elementLength_EVR_UNKNOWN);
OFTEST_REGISTER(dcmdata_elementLength_EVR_UNKNOWN2B);
OFTEST_REGISTER(dcmdata_elementLength_EVR_US);
OFTEST_REGISTER(dcmdata_elementLength_EVR_UT);
OFTEST_REGISTER(dcmdata_elementLength_EVR_lt);
OFTEST_REGISTER(dcmdata_elementLength_EVR_na);
OFTEST_REGISTER(dcmdata_elementLength_EVR_ox);
OFTEST_REGISTER(dcmdata_elementLength_EVR_up);
OFTEST_REGISTER(dcmdata_elementLength_EVR_xs);
OFTEST_REGISTER(dcmdata_elementLength_pixelItem);
OFTEST_REGISTER(dcmdata_elementLength_pixelSequence);
OFTEST_REGISTER(dcmdata_elementParent);
OFTEST_REGISTER(dcmdata_parser_missingDelimitationItems);
OFTEST_REGISTER(dcmdata_parser_missingSequenceDelimitationItem_1);
OFTEST_REGISTER(dcmdata_parser_missingSequenceDelimitationItem_2);
OFTEST_REGISTER(dcmdata_parser_wrongDelimitationItemForSequence);
OFTEST_REGISTER(dcmdata_parser_wrongDelimitationItemForItem);
OFTEST_REGISTER(dcmdata_parser_explicitItemLengthTooLarge);
OFTEST_REGISTER(dcmdata_parser_oddLengthPartialValue_lastItem);
OFTEST_REGISTER(dcmdata_parser_oddLengthPartialValue_notLastItem);
OFTEST_REGISTER(dcmdata_parser_wrongExplicitVRinDataset_default);
OFTEST_REGISTER(dcmdata_parser_wrongExplicitVRinDataset_defaultVR_dictLen);
OFTEST_REGISTER(dcmdata_parser_wrongExplicitVRinDataset_dictVR_defaultLen);
OFTEST_REGISTER(dcmdata_parser_wrongExplicitVRinDataset_preferDataDict);
OFTEST_REGISTER(dcmdata_parser_undefinedLengthUNSequence);
OFTEST_REGISTER(dcmdata_readingDataDictionary);
OFTEST_REGISTER(dcmdata_usingDataDictionary);
OFTEST_REGISTER(dcmdata_specificCharacterSet_1);
OFTEST_REGISTER(dcmdata_specificCharacterSet_2);
OFTEST_REGISTER(dcmdata_specificCharacterSet_3);
OFTEST_REGISTER(dcmdata_specificCharacterSet_4);
OFTEST_REGISTER(dcmdata_attribute_filter);
OFTEST_MAIN("dcmdata")
