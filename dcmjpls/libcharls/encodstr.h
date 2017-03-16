// 
// (C) Jan de Vaan 2007-2010, all rights reserved. See the accompanying "License.txt" for licensed use. 
// 

#ifndef CHARLS_ENCODERSTRATEGY
#define CHARLS_ENCODERSTRATEGY

#define INCLUDE_NEW
#include "dcmtk/ofstd/ofstdinc.h"

#include "dcmtk/ofstd/ofmem.h"
#include "dcmtk/ofstd/ofbmanip.h"
#include "procline.h"
#include "decodstr.h"

// Implements encoding to stream of bits. In encoding mode JpegLsCodec inherits from EncoderStrategy

class EncoderStrategy
{

public:
  explicit EncoderStrategy(const JlsParameters& info) :
     _qdecoder(OFnullptr),
     _info(info),
     _processLine(OFnullptr),
     valcurrent(0),
     bitpos(0),
     _isFFWritten(false),
     _bytesWritten(0)
    
  {
  }

  virtual ~EncoderStrategy() 
  {
  }

  LONG PeekByte();
  
  void OnLineBegin(LONG cpixel, void* ptypeBuffer, LONG pixelStride)
  {
    _processLine->NewLineRequested(ptypeBuffer, cpixel, pixelStride);
  }

  void OnLineEnd(LONG /*cpixel*/, void* /*ptypeBuffer*/, LONG /*pixelStride*/) { }

    virtual void SetPresets(const JlsCustomParameters& presets) = 0;
    
  virtual size_t EncodeScan(const void* rawData, BYTE **ptr, size_t *size, size_t offset, bool compare) = 0;

protected:

  void Init(BYTE **ptr, size_t *size, size_t offset)
  {
    bitpos = 32;
    valcurrent = 0;
    _position = ptr;
    _size = size;
    _current_offset = offset;
  }


  void AppendToBitStream(LONG value, LONG length)
  {
          ASSERT(length < 32 && length >= 0);

          ASSERT((_qdecoder.get() == NULL) || (length == 0 && value == 0) ||( _qdecoder->ReadLongValue(length) == value));

#ifndef NDEBUG
    if (length < 32)
    {
          int mask = (1 << (length)) - 1;
          ASSERT((value | mask) == mask);
    }
#endif

     bitpos -= length;
     if (bitpos >= 0)
     {
             valcurrent = valcurrent | (value << bitpos);
             return;
     }

     valcurrent |= value >> -bitpos;
     Flush();
   
     // A second flush may be required if extra marker-detect bits were needed and not all bits could be written.
     if (bitpos < 0)
     {
           valcurrent |= value >> -bitpos;
           Flush();
     }

     ASSERT(bitpos >=0);
     valcurrent |= value << bitpos;
  }

  void EndScan()
  {
    Flush();

    // if a 0xff was written, Flush() will force one unset bit anyway
    if (_isFFWritten)
      AppendToBitStream(0, (bitpos - 1) % 8);
    else
      AppendToBitStream(0, bitpos % 8);
    
    Flush();
    ASSERT(bitpos == 0x20);
  }

  void Flush()
  {
    for (LONG i = 0; i < 4; ++i)
    {
      if (bitpos >= 32)
        break;

      if (_isFFWritten)
      {
        // JPEG-LS requirement (T.87, A.1) to detect markers: after a xFF value a single 0 bit needs to be inserted.
        write(BYTE(valcurrent >> 25));
        valcurrent = valcurrent << 7;
        bitpos += 7;
        _isFFWritten = false;
      }
      else
      {
        write(BYTE(valcurrent >> 24));
        _isFFWritten = (*_position)[_current_offset - 1] == 0xFF;
        valcurrent = valcurrent << 8;     
        bitpos += 8;
      }

    }
    
  }

  size_t GetLength() 
  { 
    return _bytesWritten - (bitpos -32)/8; 
  }


  inlinehint void AppendOnesToBitStream(LONG length)
  {
    AppendToBitStream((1 << length) - 1, length); 
  }


  OFunique_ptr<DecoderStrategy> _qdecoder; 

protected:
  JlsParameters _info;
  OFunique_ptr<ProcessLine> _processLine;
private:
  static BYTE *re_alloc(BYTE *old_ptr, size_t *old_size)
  {
    size_t new_size = *old_size * 2;
#ifdef HAVE_STD__NOTHROW
    BYTE *new_ptr = new(std::nothrow) BYTE[new_size];
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

  void write(BYTE value)
  {
    if (_current_offset == *_size) {
      *_position = re_alloc(*_position, _size);
    }

    (*_position)[_current_offset] = value;
    ++_current_offset;
    ++_bytesWritten;
  }

  unsigned int valcurrent;
  LONG bitpos;
  
  // encoding
  BYTE **_position;
  size_t *_size;
  size_t _current_offset;
  bool _isFFWritten;
  size_t _bytesWritten;

};

#endif
