/*
** 
** Author: Andreas Barth 	Created:  09.11.95
**                          Modified: 18.11.95
**
** Module: tstbuf.cc
** 
** Purpose:
** 	This file tests the DcmBuffer 
** 
** 
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-01-05 13:31:38 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/tests/Attic/tstbuf.cc,v $
** CVS/RCS Revision:	$Revision: 1.1 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    // make sure OS specific configuration is included first
#include <iostream.h>
#include <stdio.h>
#include "dcbuf.h"

#define BUFLEN 100
#define READLEN 40 

int main(int argc, char * argv[])
{
	if (argc != 2)
	{
		cerr << "usage: " << argv[0] << " ASCII-File\n";
		return 1;
	}

	cout << "READ TEST" << endl;
	cout << "=========" << endl;
	

	FILE * inFile = fopen(argv[1], "r");

	if (!inFile)
	{
		cerr << argv[0] << ": Cannot open " << argv[1] << endl;
		return 1;
	}


	unsigned char inputBuf[BUFLEN];
	DcmMemoryBuffer buf;
	unsigned long length;
	unsigned char rdBuf[READLEN+1];

	while (!feof(inFile))
	{
		buf.Release();
		length = fread(inputBuf, 1, BUFLEN, inFile);
//		buf.CopyIntoBuffer(inputBuf, length, BUFLEN);
		buf.SetBuffer(inputBuf, length, BUFLEN);
		while(buf.Read(rdBuf, READLEN))
		{
			rdBuf[READLEN] = 0;
			cout << rdBuf;
		}
	}

	buf.SetEndOfBuffer();

	while(buf.Read(rdBuf, READLEN))
	{
		rdBuf[READLEN] = 0;
		cout << rdBuf;
	}
		

	if (!buf.EndOfBuffer())
	{
		const unsigned long avail = buf.Read(rdBuf, buf.AvailRead());
		rdBuf[avail] = 0;
		cout << rdBuf  << endl;
	}	

	fclose(inFile);

	cout << "PUTBACK TEST" << endl;
	cout << "=========" << endl;
	
	inFile = fopen(argv[1], "r");

	if (!inFile)
	{
		cerr << argv[0] << ": Cannot open " << argv[1] << endl;
		return 1;
	}

	
	while (!feof(inFile))
	{
		buf.Release();
		length = fread(inputBuf, 1, BUFLEN, inFile);
//		buf.CopyIntoBuffer(inputBuf, length, BUFLEN);
		buf.SetBuffer(inputBuf, length, BUFLEN);
		buf.SetPutbackMark();
		while(buf.Read(rdBuf, READLEN))
		{
			buf.Putback((unsigned char *)"abcdef",6);
			rdBuf[READLEN] = 0;
			cout << rdBuf;
		}
	}

	buf.SetEndOfBuffer();

	while(buf.Read(rdBuf, READLEN))
	{
		rdBuf[READLEN] = 0;
		cout << rdBuf;
	}
		

	if (!buf.EndOfBuffer())
	{
		const unsigned long avail = buf.Read(rdBuf, buf.AvailRead());
		rdBuf[avail] = 0;
		cout << rdBuf  << endl;
	}	

	cout << "WRITE TEST" << endl;
	cout << "==========" << endl;
	

	DcmMemoryBuffer wBuf(BUFLEN);
    unsigned char wContent[BUFLEN+1];
	wContent[BUFLEN]=0;
	
	
	unsigned char number[10];
	int len, realLen;
	

	for (int i = 0; i < 1322; i++)
	{
		sprintf((char *)number, "%d ", i);
		len = strlen((char *)number);
		if ((realLen = wBuf.Write(number, len)) != len)
		{
			wBuf.CopyFromBuffer(wContent);
			cout << wContent;
			wBuf.Write(&number[realLen], len-realLen);
		}
	}
	
	realLen = wBuf.GetLength();
	wBuf.CopyFromBuffer(wContent);
	wContent[realLen]=0;
	cout << wContent << endl;
		
}
	
			
/*
** CVS/RCS Log:
** $Log: tstbuf.cc,v $
** Revision 1.1  1996-01-05 13:31:38  andreas
** - new streaming facilities and test routines for blocks and buffers
** - unique read/write methods for block and file transfer
** - more cleanups
**
**
*/





