//
// (C) Jan de Vaan 2007-2009, all rights reserved. See the accompanying "License.txt" for licensed use.
//

#ifndef CHARLS_DECODERSTATEGY
#define CHARLS_DECODERSTATEGY

#include <assert.h>
#include "streams.h"
#include "processline.h"

#if defined(i386) || defined(__i386__) || defined(_M_IX86) || defined(__amd64__) || defined(_M_X64)
#define ARCH_HAS_UNALIGNED_MEM_ACCESS /* TODO define this symbol for more architectures */
#define USE_X86_ASSEMBLY
#endif

#ifdef _MSC_VER
#include <intrin.h>

inline unsigned int byteswap(unsigned int x)
{
	return _byteswap_ulong(x);
}

inline unsigned long long byteswap(unsigned long long x)
{
	return _byteswap_uint64(x);
}
#elif defined(USE_X86_ASSEMBLY)
// We can use x86 inline assembly!
inline size_t byteswap(size_t x)
{
	asm("bswap %0" : "=r" (x) : "0" (x));
	return x;
}
#else
// This function byteswaps a 4-byte unsigned integer
inline unsigned int byteswap4(unsigned int x)
{
	return (((x & 0x000000ff) << 24) |
		((x & 0x0000ff00) <<  8) |
		((x & 0x00ff0000) >>  8) |
		((x & 0xff000000) >> 24));
}

inline size_t byteswap(size_t x)
{
	// The compiler should be able to optimize these ifs away
	if (sizeof(size_t) == 4)
		return byteswap4(x);
	if (sizeof(size_t) != 8)
		// This is not yet implemented!
		assert(false);

	// This variable should always be 32. The problem is that the following
	// code would cause a warning on 32-bit arches, even though it is never
	// executed. This is why we have to use this hack.
	const unsigned int shift = sizeof(size_t) * 8 / 2;

	unsigned int high = x >> shift;
	unsigned int low = x & 0xffffffff;

	size_t res = ((size_t) byteswap4(low)) << shift;
	res |= byteswap4(high);

	return res;
}
#endif

inline int is_big_endian()
{
	int i = 1;
	char *p = (char *) &i;

	// Little endian stores the least significant byte first in memory
	// (0x01 in our case), big endian stores it last.
	return (*p == 0);
}

inline size_t big_to_native_endian(size_t x)
{
	if (is_big_endian())
		// x already is in our current endian (=big)
		return x;
	// We are on little endian, we need to swap some bytes
	return byteswap(x);
}

class DecoderStrategy
{
public:
	DecoderStrategy(const JlsParamaters& info) :
		  _info(info),
	          _processLine(0),
		  _readCache(0),
		  _validBits(0),
		  _pbyteCompressed(0)
	  {
	  }

	  virtual ~DecoderStrategy()
	  {
		  delete _processLine;
	  }

	  virtual void SetPresets(const JlsCustomParameters& presets) = 0;
	  virtual size_t DecodeScan(void* pvoidOut, const Size& size, const void* pvoidIn, size_t cbyte, bool bCheck) = 0;

	  void Init(BYTE* pbyteCompressed, size_t cbyte)
	  {
		  _validBits = 0;
		  _readCache = 0;
		  _pbyteCompressed = pbyteCompressed;
		  _pbyteCompressedEnd = pbyteCompressed + cbyte;
		  _pbyteNextFF = FindNextFF();
		  MakeValid();
	  }

	  inlinehint void Skip(LONG length)
	  {
		  _validBits -= length;
		  _readCache = _readCache << length;
	  }


	  void OnLineBegin(LONG cpixel, void* ptypeBuffer, LONG pixelStride)
	  {}


	  void OnLineEnd(LONG cpixel, const void* ptypeBuffer, LONG pixelStride)
	  {
		  _processLine->NewLineDecoded(ptypeBuffer, cpixel, pixelStride);
	  }

	  typedef size_t bufType;

	  enum {
		  bufferbits = sizeof( bufType ) * 8
	  };

	  void MakeValid()
	  {
		  ASSERT(_validBits <=bufferbits - 8);

		  // Easy & fast: if there is no 0xFF byte in sight, we can read without bitstuffing
#ifndef ARCH_HAS_UNALIGNED_MEM_ACCESS
		  // We can only dereference properly aligned pointers. This
		  // checks if the pointer is properly aligned, if not we have
		  // to take the slow path.
		  const bufType needed_alignment_mask = sizeof(bufType) - 1;
#else
		  const bufType needed_alignment_mask = 0;
#endif
		  if (_pbyteCompressed < _pbyteNextFF && (((intptr_t)_pbyteCompressed) & needed_alignment_mask) == 0)
		  {
			  _readCache		 |= big_to_native_endian(*((bufType*)_pbyteCompressed)) >> _validBits;

			  int bytesToRead = (bufferbits - _validBits) >> 3;
			  _pbyteCompressed += bytesToRead;
			  _validBits += bytesToRead * 8;
			  ASSERT(_validBits >= bufferbits - 8);
			  return;
		  }

		  do
		  {
			  if (_pbyteCompressed >= _pbyteCompressedEnd)
			  {
				  if (_validBits <= 0)
					  throw JlsException(InvalidCompressedData);

				  return;
			  }

			  bufType valnew	  = _pbyteCompressed[0];

			  if (valnew == 0xFF)
			  {
				  // JPEG bitstream rule: no FF may be followed by 0x80 or higher
				 if (_pbyteCompressed == _pbyteCompressedEnd - 1 || (_pbyteCompressed[1] & 0x80) != 0)
				 {
					 if (_validBits <= 0)
						 throw JlsException(InvalidCompressedData);

					 return;
			     }
			  }

			  _readCache		 |= valnew << (bufferbits - 8  - _validBits);
			  _pbyteCompressed   += 1;
			  _validBits		 += 8;

			  if (valnew == 0xFF)
			  {
				  _validBits--;
			  }
		  }
		  while (_validBits < bufferbits - 8);

		  _pbyteNextFF = FindNextFF();
		  return;

	  }


	  BYTE* FindNextFF()
	  {
		  BYTE* pbyteNextFF =_pbyteCompressed;

		  while (pbyteNextFF < _pbyteCompressedEnd)
	      {
			  if (*pbyteNextFF == 0xFF)
			  {
				  break;
			  }
			  pbyteNextFF++;
		  }

		  return pbyteNextFF - (sizeof(bufType)-1);
	  }


	  BYTE* GetCurBytePos() const
	  {
		  LONG  cbitValid = _validBits;
		  BYTE* pbyteCompressed = _pbyteCompressed;

		  for (;;)
		  {
			  LONG cbitLast = pbyteCompressed[-1] == 0xFF ? 7 : 8;

			  if (cbitValid < cbitLast )
				  return pbyteCompressed;

			  cbitValid -= cbitLast;
			  pbyteCompressed--;
		  }
	  }


	  inlinehint LONG ReadValue(LONG length)
	  {
		  if (_validBits < length)
		  {
			  MakeValid();
		  }

		  ASSERT(length != 0 && length <= _validBits);
		  ASSERT(length < 32);
		  LONG result = LONG(_readCache >> (bufferbits - length));
		  Skip(length);
		  return result;
	  }


	  inlinehint LONG PeekByte()
	  {
		  if (_validBits < 8)
		  {
			  MakeValid();
		  }

		  return _readCache >> (bufferbits - 8);
	  }



	  inlinehint bool ReadBit()
	  {
		  if (_validBits <= 0)
		  {
			  MakeValid();
		  }

		  bool bSet = (_readCache & (bufType(1) << (bufferbits - 1))) != 0;
		  Skip(1);
		  return bSet;
	  }



	  inlinehint LONG Peek0Bits()
	  {
		  if (_validBits < 16)
		  {
			  MakeValid();
		  }
		  bufType valTest = _readCache;

		  for (LONG cbit = 0; cbit < 16; cbit++)
		  {
			  if ((valTest & (bufType(1) << (bufferbits - 1))) != 0)
				  return cbit;

			  valTest <<= 1;
		  }
		  return -1;
	  }



	  inlinehint LONG ReadHighbits()
	  {
		  LONG cbit = Peek0Bits();
		  if (cbit >= 0)
		  {
			  Skip(cbit + 1);
			  return cbit;
		  }
		  Skip(15);

		  for (LONG highbits = 15; ; highbits++)
		  {
			  if (ReadBit())
				  return highbits;
		  }
	  }


	  LONG ReadLongValue(LONG length)
	  {
		  if (length <= 24)
			  return ReadValue(length);

		  return (ReadValue(length - 24) << 24) + ReadValue(24);
	  }

protected:
	JlsParamaters _info;
	void* _ptypeUncompressed;
	ProcessLine* _processLine;

private:
	// decoding
	bufType _readCache;
	LONG _validBits;
	BYTE* _pbyteCompressed;
	BYTE* _pbyteNextFF;
	BYTE* _pbyteCompressedEnd;
};


#endif
