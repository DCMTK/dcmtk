//
// (C) Jan de Vaan 2007-2010, all rights reserved. See the accompanying "License.txt" for licensed use.
//

#include "config.h"
#include "util.h"
#include "streams.h"
#include "header.h"

#include "decodstr.h"
#include "encodstr.h"
#include "context.h"
#include "ctxtrmod.h"
#include "lokuptbl.h"

signed char* JlsContext::_tableC = CreateTableC();

// As defined in the JPEG-LS standard

// used to determine how large runs should be encoded at a time.
const int J[32]			= {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 9, 10, 11, 12, 13, 14, 15};

// Default bin sizes for JPEG-LS statistical modeling. Can be overriden at compression time, however this is rarely done.
const int BASIC_T1		= 3;
const int BASIC_T2		= 7;
const int BASIC_T3		= 21;

#include "lltraits.h"
#include "deftrait.h"

#include "scan.h"

signed char QuantizeGratientOrg(const Presets& preset, LONG NEAR, LONG Di)
{
	if (Di <= -preset.T3) return  -4;
	if (Di <= -preset.T2) return  -3;
	if (Di <= -preset.T1) return  -2;
	if (Di < -NEAR)  return  -1;
	if (Di <=  NEAR) return   0;
	if (Di < preset.T1)   return   1;
	if (Di < preset.T2)   return   2;
	if (Di < preset.T3)   return   3;

	return  4;
}



OFVector<signed char> CreateQLutLossless(LONG cbit)
{
	Presets preset = ComputeDefault((1 << cbit) - 1, 0);
	LONG range = preset.MAXVAL + 1;

	OFVector<signed char> lut(range * 2);

	for (LONG diff = -range; diff < range; diff++)
	{
		lut[range + diff] = QuantizeGratientOrg(preset, 0,diff);
	}
	return lut;
}

// Lookup tables to replace code with lookup tables.
// To avoid threading issues, all tables are created when the program is loaded.


// Lookup table: decode symbols that are smaller or equal to 8 bit (16 tables for each value of k)
CTable decodingTables[16] = { InitTable(0), InitTable(1), InitTable(2), InitTable(3),
							 InitTable(4), InitTable(5), InitTable(6), InitTable(7),
							 InitTable(8), InitTable(9), InitTable(10), InitTable(11),
							 InitTable(12), InitTable(13), InitTable(14),InitTable(15) };


// Lookup tables: sample differences to bin indexes.
OFVector<signed char> rgquant8Ll = CreateQLutLossless(8);
OFVector<signed char> rgquant10Ll = CreateQLutLossless(10);
OFVector<signed char> rgquant12Ll = CreateQLutLossless(12);
OFVector<signed char> rgquant16Ll = CreateQLutLossless(16);




template<class STRATEGY>
OFauto_ptr<STRATEGY> JlsCodecFactory<STRATEGY>::GetCodec(const JlsParameters& info, const JlsCustomParameters& presets)
{
	STRATEGY* pstrategy = NULL;
	if (presets.RESET != 0 && presets.RESET != BASIC_RESET)
	{
		DefaultTraitsT<BYTE,BYTE> traits((1 << info.bitspersample) - 1, info.allowedlossyerror);
		traits.MAXVAL = presets.MAXVAL;
		traits.RESET = presets.RESET;
		pstrategy = new JlsCodec<DefaultTraitsT<BYTE, BYTE>, STRATEGY>(traits, info);
	}
	else
	{
		pstrategy = GetCodecImpl(info);
	}

	if (pstrategy != NULL)
	{
		pstrategy->SetPresets(presets);
	}
	return OFauto_ptr<STRATEGY>(pstrategy);
}



template<class TRAITS, class STRATEGY>
STRATEGY* CreateCodec(const TRAITS& t, const STRATEGY*,const JlsParameters& info)
{
	return new JlsCodec<TRAITS, STRATEGY>(t, info);
}


template<class STRATEGY>
STRATEGY* JlsCodecFactory<STRATEGY>::GetCodecImpl(const JlsParameters& info)
{
	STRATEGY* s = 0;

	if (info.ilv == ILV_SAMPLE && info.components != 3)
		return NULL;

#ifndef DISABLE_SPECIALIZATIONS

	// optimized lossless versions common formats
	if (info.allowedlossyerror == 0)
	{
		if (info.ilv == ILV_SAMPLE)
		{
			if (info.bitspersample == 8)
				return CreateCodec(LosslessTraitsT<Triplet<BYTE>,8>(), s, info);
		}
		else
		{
			switch (info.bitspersample)
			{
				case  8: return CreateCodec(LosslessTraitsT<BYTE,    8>(), s, info);
				case 12: return CreateCodec(LosslessTraitsT<USHORT, 12>(), s, info);
				case 16: return CreateCodec(LosslessTraitsT<USHORT, 16>(), s, info);
			}
		}
	}

#endif

	int maxval = (1 << info.bitspersample) - 1;

	if (info.bitspersample <= 8)
	{
		if (info.ilv == ILV_SAMPLE)
			return CreateCodec(DefaultTraitsT<BYTE,Triplet<BYTE> >(maxval, info.allowedlossyerror), s, info);

		return CreateCodec(DefaultTraitsT<BYTE, BYTE>((1 << info.bitspersample) - 1, info.allowedlossyerror), s, info);
	}
	else if (info.bitspersample <= 16)
	{
		if (info.ilv == ILV_SAMPLE)
			return CreateCodec(DefaultTraitsT<USHORT,Triplet<USHORT> >(maxval, info.allowedlossyerror), s, info);

		return CreateCodec(DefaultTraitsT<USHORT, USHORT>(maxval, info.allowedlossyerror), s, info);
	}
	return NULL;
}


template class JlsCodecFactory<DecoderStrategy>;
template class JlsCodecFactory<EncoderStrategy>;
