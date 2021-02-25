// 
// (C) Jan de Vaan 2007-2010, all rights reserved. See the accompanying "License.txt" for licensed use. 
// 
#ifndef CHARLS_STREAMS
#define CHARLS_STREAMS

#include "dcmtk/ofstd/ofvector.h"
#include "dcmtk/ofstd/ofbmanip.h"
#include "util.h"



// This file defines JPEG-LS streams: The header and the actual pixel data. Header markers have fixed length, the pixeldata not. 



class JpegSegment;

enum JPEGLS_ColorXForm
{
	// default (RGB)
	COLORXFORM_NONE = 0,	

	// Color transforms as defined by HP
	COLORXFORM_HP1,
	COLORXFORM_HP2,
	COLORXFORM_HP3,

	// Defined by HP but not supported by CharLS
	COLORXFORM_RGB_AS_YUV_LOSSY,
	COLORXFORM_MATRIX
};
	
//
// JLSOutputStream: minimal implementation to write JPEG header streams
//
class JLSOutputStream
{
	friend class JpegMarkerSegment;
	friend class JpegImageDataSegment;

public:
	JLSOutputStream();
	virtual ~JLSOutputStream();

	void Init(Size size, LONG bitsPerSample, LONG ccomp);
	void AddScan(const void* compareData, const JlsParameters* pparams);
	void AddLSE(const JlsCustomParameters* pcustom);
	void AddColorTransform(int i);
	size_t GetBytesWritten()
		{ return _cbytesWritten; }

	size_t Write(BYTE **ptr, size_t *size, size_t offset);

	BYTE **get_pos() { return _position; }

	size_t *get_size() { return _size; }

	size_t get_offset() { return _current_offset; }

	void EnableCompare(bool bCompare) 
	{ _bCompare = bCompare; }
private:
	void WriteByte(BYTE val)
	{ 
		ASSERT(!_bCompare || (*_position)[_current_offset] == val);
		
		if (_current_offset == *_size) {
			*_position = re_alloc(*_position, _size);
		}

		(*_position)[_current_offset++] = val;

		_cbytesWritten++;
	}

	void WriteBytes(const OFVector<BYTE>& rgbyte)
	{
		for (size_t i = 0; i < rgbyte.size(); ++i)
		{
			WriteByte(rgbyte[i]);
		}		
	}

	void WriteWord(USHORT val)
	{
		WriteByte(BYTE(val / 0x100));
		WriteByte(BYTE(val % 0x100));
	}

	void seek(size_t n)
	{
		_cbytesWritten += n;
		_current_offset += n;
	}

	bool _bCompare;

private:
	static BYTE *re_alloc(BYTE *old_ptr, size_t *old_size)
	{
		size_t new_size = *old_size * 2;
#ifdef HAVE_STD__NOTHROW
		BYTE *new_ptr = new BYTE[new_size];
#else
		BYTE *new_ptr = new BYTE[new_size];
#endif
		if (new_ptr == NULL) {
			throw alloc_fail();
		}

		OFBitmanipTemplate<BYTE>::copyMem(old_ptr, new_ptr, *old_size);

		delete[] old_ptr;

		*old_size = new_size;

		return new_ptr;
	}

	BYTE **_position;
	size_t *_size;
	size_t _current_offset;
	size_t _cbytesWritten;
	LONG _icompLast;
	OFVector<JpegSegment*> _segments;
};


//
// JLSInputStream: minimal implementation to read JPEG header streams
//
class JLSInputStream
{
public:
	JLSInputStream(const BYTE* pdata, size_t cbyteLength);

	size_t GetBytesRead()
		{ return _cbyteOffset; }

	const JlsParameters& GetMetadata() const
		{ return _info; } 

	const JlsCustomParameters& GetCustomPreset() const
	{ return _info.custom; } 

	void Read(void* pvoid, size_t cbyteAvailable);
	void ReadHeader();
	
	void EnableCompare(bool bCompare)
		{ _bCompare = bCompare;	}

	void SetInfo(JlsParameters* info) { _info = *info; }

	void SetRect(JlsRect rect) { _rect = rect; }

private:
	void ReadPixels(void* pvoid, size_t cbyteAvailable);
	void ReadScan(void*);	
	void ReadStartOfScan();
	void ReadPresetParameters();
	void ReadComment();
	void ReadStartOfFrame();
	BYTE ReadByte();
	int ReadWord();
	void ReadNBytes(OFVector<char>& dst, int byteCount);

	// JFIF
	void ReadJfif();
	// Color Transform Application Markers & Code Stream (HP extension)
	void ReadColorSpace();
	void ReadColorXForm();
	
private:
	const BYTE* _pdata;
	size_t _cbyteOffset;
	size_t _cbyteLength;
	bool _bCompare;
	JlsParameters _info;
	JlsRect _rect;
};




#endif
