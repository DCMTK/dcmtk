/*
**
** Author: Gerd Ehlers      27.03.94 -- Creation
**         Andreas Barth    27.11.95 -- Support of new Stream class
**
** Module: dcobject.cc
**
** Purpose:
** Implementation of the base class object
**
**
** Last Update:   $Author: andreas $
** Revision:      $Revision: 1.16 $
** Status:	  $State: Exp $
**
*/


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <iostream.h>

#include "dcobject.h"
#include "dcvr.h"
#include "dcxfer.h"
#include "dcswap.h"
#include "dcdebug.h"

/*
** Should automatic correction be applied to input data (e.g. stripping
** of padding blanks, removal of blanks in UIDs, etc).
*/
BOOL dcmEnableAutomaticInputDataCorrection = TRUE;



// ****** public methods **********************************


DcmObject::DcmObject(const DcmTag &tag, const Uint32 len)
{
  Tag = tag;    
  Length = len;
  errorFlag = EC_Normal;
  fTransferState = ERW_init;
  fTransferredBytes = 0;

#ifdef DEBUG
  testConstructDestruct = 1; // for debugging
#endif
}


// ********************************


DcmObject::DcmObject( const DcmObject& obj )
{
  Tag = obj.Tag;
  Length = obj.Length;
  errorFlag = obj.errorFlag;
  fTransferState = obj.fTransferState;
  fTransferredBytes = obj.fTransferredBytes;

#ifdef DEBUG
  testConstructDestruct = 1; // for debugging
#endif
}


// ********************************


DcmObject::~DcmObject()
{
#ifdef DEBUG
  if ( testConstructDestruct == 1 )                   // for debugging
    testConstructDestruct = 2; // for debugging
  else
    {
      debug(1, ( "Error: ~DcmObject called more than once (%d)",
	      testConstructDestruct ));

      cerr << "Error: ~DcmObject called more than once ("
	   << testConstructDestruct << ")" << endl;
      testConstructDestruct++;
    }
#endif
}


// ********************************


void DcmObject::transferInit()
{
  fTransferState = ERW_init;
  fTransferredBytes = 0;
}


// ********************************


void DcmObject::transferEnd()
{
  fTransferState = ERW_notInitialized;
}


// ********************************

DcmObject * DcmObject::nextInContainer(const DcmObject * /*obj*/)
{
    return NULL;
}

// ********************************

E_Condition DcmObject::nextObject(DcmStack & /*stack*/,
				  const BOOL /*intoSub*/)
{
    return EC_TagNotFound;
}

// ********************************

E_Condition DcmObject::search( const DcmTagKey &/*tag*/,
			       DcmStack &/*resultStack*/,
			       E_SearchMode /*mode*/,
			       BOOL /*searchIntoSub*/ )
{
  return EC_TagNotFound;
}


// ********************************


E_Condition DcmObject::searchErrors( DcmStack &resultStack )
{
  if ( errorFlag != EC_Normal )
    resultStack.push( this );
  return errorFlag;
}



// ***********************************************************
// ****** protected methods **********************************
// ***********************************************************


void DcmObject::printInfoLine(ostream & out, const BOOL showFullData,
			      const int level, char *info )
{
  printInfoLine(out, showFullData, level, Tag, Length, info );
}


// ********************************


void DcmObject::printInfoLine(ostream & out, const BOOL showFullData,
			      const int level, const DcmTag &tag, 
			      const Uint32 length,
			      char *info)
{
    DcmVR vr(tag.getVR());

    char output[100];
    for ( int i=1; i<level; i++)
	out << "    ";

    if (strlen(info) <= 38)
    {
	sprintf(output, "(%4.4x,%4.4x) %-5.5s %-38s #%6lu,%3lu",
		tag.getGTag(), tag.getETag(), vr.getVRName(), info,
		(unsigned long)length, getVM());
    }
    else 
    {
	sprintf(output, "(%4.4x,%4.4x) %-5.5s ",
		tag.getGTag(), tag.getETag(), vr.getVRName());
	if (!showFullData && DCM_OptPrintLineLength+10 <= strlen(info))
	    strcpy(&info[DCM_OptPrintLineLength],"...");
	out << output << info;
	sprintf(output, " #%6lu,%3lu", (unsigned long)length, getVM());
    }
    out << output << "  " << tag.getTagName() << endl;
}


// ********************************

E_Condition DcmObject::writeTag(DcmStream & outStream, const DcmTag & tag, 
				const E_TransferSyntax oxfer)
{
  DcmXfer outXfer(oxfer);
  const E_ByteOrder outByteOrder = outXfer.getByteOrder();
  if (outByteOrder == EBO_unknown)
      return EC_IllegalCall;

  Uint16 groupTag = tag.getGTag();		// 2 Byte Laenge; 
  swapIfNecessary(outByteOrder, gLocalByteOrder, &groupTag, 2, 2);
  outStream.WriteBytes(&groupTag, 2);
	
  Uint16 elementTag = tag.getETag();	// 2 Byte Laenge; 
  swapIfNecessary(outByteOrder, gLocalByteOrder, &elementTag, 2, 2);
  outStream.WriteBytes(&elementTag, 2);
  if (outStream.GetError() != EC_Normal)
    return outStream.GetError();
  else
    return EC_Normal;
}


E_Condition DcmObject::writeTagAndLength(DcmStream & outStream, 
					 const E_TransferSyntax oxfer,	
					 Uint32 & writtenBytes)	
{
  E_Condition l_error = outStream.GetError();
  if (l_error != EC_Normal)
    writtenBytes = 0;
  else
    {
      l_error = this -> writeTag(outStream, Tag, oxfer);
      writtenBytes = 4;

      DcmXfer oxferSyn(oxfer);
      const E_ByteOrder oByteOrder = oxferSyn.getByteOrder();
      if (oByteOrder == EBO_unknown)
	  return EC_IllegalCall;

      if (oxferSyn.isExplicitVR())
	{
	  // Umwandlung in gueltige VR
	  DcmVR myvr(this->ident()); // what VR should it be
	  // getValidEVR() will convert UN to OB if generation of UN disabled
	  DcmEVR vr = myvr.getValidEVR();
	  // convert to a valid string
	  const char *vrname = myvr.getValidVRName();
	  outStream.WriteBytes(vrname, 2);    // 2 Bytes of VR name 
	  writtenBytes += 2;

	  if (vr == EVR_OB || vr == EVR_OW || vr == EVR_SQ || vr == EVR_UN)
	    {
	      Uint16 reserved = 0;
	      outStream.WriteBytes(&reserved, 2); // 2 Byte Laenge
	      Uint32 valueLength = Length;
	      swapIfNecessary(oByteOrder, gLocalByteOrder, &valueLength, 4, 4);
	      outStream.WriteBytes(&valueLength, 4); // 4 Byte Laenge
	      writtenBytes += 6;
	    }
	  else
	    {
	      Uint16 valueLength = (Uint16)Length;
	      swapIfNecessary(oByteOrder, gLocalByteOrder, &valueLength, 2, 2);
	      outStream.WriteBytes(&valueLength, 2); // 2 Byte Laenge
	      writtenBytes += 2;
	    }

        } // if ( oxferSyn.isExplicitVR() )
      else
	{
	  Uint32 valueLength = Length;
	  swapIfNecessary(oByteOrder, gLocalByteOrder, &valueLength, 4, 4);
	  outStream.WriteBytes(&valueLength, 4); // 4 Byte Laenge
	  writtenBytes += 4;
	}
    }
  return l_error;
}


// ********************************

