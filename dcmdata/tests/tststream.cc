/*
** 
** Author: Andreas Barth 	Created:  09.11.95
**                          Modified: 18.11.95
**
** Module: tststream.cc
** 
** Purpose:
** 	This file tests the DcmFileStream and DcmMemoryStream 
** 
** 
** Last Update:		$Author: andreas $
** Update Date:		$Date: 1996-04-12 13:19:12 $
** Source File:		$Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmdata/tests/Attic/tststream.cc,v $
** CVS/RCS Revision:	$Revision: 1.4 $
** Status:		$State: Exp $
**
** CVS/RCS Log at end of file
**
*/

#include "osconfig.h"    // make sure OS specific configuration is included first

#include <iostream.h>
#include <stdlib.h>
#include <string.h>

#include "dcstream.h"


#define BUFLEN 32
char buffer[BUFLEN];

void ReadFromFile(DcmBufferStream * stream, FILE * inFile)
{

    if (!stream -> EndOfStream())
    {
	stream -> ReleaseBuffer();
	unsigned long length = fread(buffer, 1, BUFLEN, inFile);
	stream -> SetBuffer(buffer, length);
		
	if (feof(inFile))
	{
	    cout << endl << "EOF reached" << endl;
	    stream -> SetEndOfStream();
	}
    }
}


void ReadTest(DcmStream * stream, FILE * inFile, BOOL putback)
{
    Uint32 ul[10];
    Sint32 sl[10];
    Uint16 us[10];
    Sint16 ss[10];
    Float32 fl[10];
    Float64 fd[10];
    char * uc;
    size_t uc_len;
    int times = putback ? 2 : 1;
    int k;


    for (k=0; k < times; k++)
    {
	if (putback && k == 0)
	    stream -> SetPutbackMark();

	while (stream -> Avail() < 10 * sizeof(Uint32))
	    ReadFromFile((DcmBufferStream *) stream, inFile);
		
	stream->ReadBytes(ul,10 * sizeof(Uint32));
	cout << endl << "ul: ";
	for (int i = 0; i < 10; i++)
	    cout << ul[i] << " ";
	cout << endl;

	if (putback && k == 0)
	    stream->Putback();
    }

    for (k=0; k < times; k++)
    {
	if (putback && k == 0)
	    stream -> SetPutbackMark();

	while (stream -> Avail() < 10 * sizeof(Sint32))
	    ReadFromFile((DcmBufferStream *) stream, inFile);

	stream->ReadBytes(sl,10 * sizeof(Sint32));
	cout << endl << "sl: " ;
	for (int i = 0; i < 10; i++)
	    cout << sl[i] << " ";
	cout << endl;

	if (putback && k == 0)
	    stream->Putback();
    }

    for (k=0; k < times; k++)
    {
	if (putback && k == 0)
	    stream -> SetPutbackMark();

	while (stream -> Avail() < 10 * sizeof(Uint16))
	    ReadFromFile((DcmBufferStream *) stream, inFile);

	stream->ReadBytes(us,10 * sizeof(Uint16));
	cout << endl << "us: " ;
	for (int i = 0; i < 10; i++)
	    cout << us[i] << " ";
	cout << endl;

	if (putback && k == 0)
	    stream->Putback();
    }

    for (k=0; k < times; k++)
    {
	if (putback && k == 0)
	    stream -> SetPutbackMark();

	while (stream -> Avail() < 10 * sizeof(Sint16))
	    ReadFromFile((DcmBufferStream *) stream, inFile);

	stream->ReadBytes(ss,10 * sizeof(Sint16));
	cout << endl << "ss: " ;
	for (int i = 0; i < 10; i++)
	    cout << ss[i] << " ";
	cout << endl;

	if (putback && k == 0)
	    stream->Putback();
    }

    for (k=0; k < times; k++)
    {
	if (putback && k == 0)
	    stream -> SetPutbackMark();

	while (stream -> Avail() < 10 * sizeof(Float32))
	    ReadFromFile((DcmBufferStream *) stream, inFile);

	stream->ReadBytes(fl,10 * sizeof(Float32));
	cout << endl << "fl: " ;
	for (int i = 0; i < 10; i++)
	    cout << fl[i] << " ";
	cout << endl;

	if (putback && k == 0)
	    stream->Putback();
    }

    for (k=0; k < times; k++)
    {
	if (putback && k == 0)
	    stream -> SetPutbackMark();

	while (stream -> Avail() < 10 * sizeof(Float64))
	    ReadFromFile((DcmBufferStream *) stream, inFile);

	stream->ReadBytes(fd,10 * sizeof(Float64));
	cout << endl << "fd: " ;
	for (int i = 0; i < 10; i++)
	    cout << fd[i] << " ";
	cout << endl;

	if (putback && k == 0)
	    stream->Putback();
    }

    for (k=0; k < times; k++)
    {
	if (putback && k == 0)
	    stream -> SetPutbackMark();

	while(stream -> Avail() < sizeof(size_t))
	    ReadFromFile((DcmBufferStream *) stream, inFile);

	stream->ReadBytes(&uc_len, sizeof(size_t));
	cout << endl << "uc_len: " ;
	cout << uc_len << endl;
	uc = new char[uc_len+1];

	if (putback && k == 0)
	    stream->Putback();
    }

    for (k=0; k < times; k++)
    {
	if (putback && k == 0)
	    stream -> SetPutbackMark();

	while(stream -> Avail() < uc_len)
	    ReadFromFile((DcmBufferStream *) stream, inFile);

	stream->ReadBytes(uc, uc_len);
	cout << endl << "uc: " ;
	uc[uc_len] = 0;
	cout << uc << endl;

	if (putback && k == 0)
	    stream->Putback();
    }

    if (stream->EndOfStream())
	cout << "End of Stream reached" << endl;
}


void WriteToFile(DcmBufferStream * stream, FILE * outFile)
{
    void * bufptr;
    Uint32 length;
    stream -> GetBuffer(bufptr, length);
    fwrite(bufptr, 1, (size_t)length, outFile);
}

void WriteTest(DcmStream * stream, FILE * outFile)
{
    Uint32 ul[10];
    Sint32 sl[10];
    Uint16 us[10];
    Sint16 ss[10];
    Float32 fl[10];
    Float64 fd[10];
    char * uc = "abcdefghijklmnop";
    size_t uc_len = strlen(uc);
    unsigned long length, t;
    int i;

    for (i=1; i < 10; i++)
    {
	ul[i] = Uint32(rand());
	sl[i] = Sint32(rand());
	us[i] = Uint16(rand());
	ss[i] = Sint16(rand());
	fl[i] = Float32(Float32(rand())/rand());
	fd[i] = Float64(Float64(rand())/rand());
    }


    for(length = 10 * sizeof(Uint32), t = 0;
	length != t; 
	t += stream -> TransferredBytes())
    {
	stream->WriteBytes(&((unsigned char *)ul)[t],length-t);
		
	if (stream -> Avail() == 0)
	{
	    WriteToFile((DcmBufferStream *) stream, outFile);
	    if (stream -> GetError() == EC_StreamNotifyClient)
		stream -> ClearError();
	}
    }
    cout << endl << "ul: " ;
    for (i = 0; i < 10; i++)
	cout << ul[i] << " ";
    cout << endl;


    for(length = 10 * sizeof(Sint32), t = 0;
	length != t; 
	t += stream -> TransferredBytes())
    {
	stream->WriteBytes(&((unsigned char *)sl)[t],length-t);
		
	if (stream -> Avail() == 0)
	{
	    WriteToFile((DcmBufferStream *) stream, outFile);
	    if (stream -> GetError() == EC_StreamNotifyClient)
		stream -> ClearError();
	}
    }
    cout << endl << "sl: " ;
    for (i = 0; i < 10; i++)
	cout << sl[i] << " ";
    cout << endl;


    for(length = 10 * sizeof(Uint16), t=0; 
	length != t; 
	t += stream -> TransferredBytes())
    {
	stream->WriteBytes(&((unsigned char *)us)[t],length-t);
		
	if (stream -> Avail() == 0)
	{
	    WriteToFile((DcmBufferStream *) stream, outFile);
	    if (stream -> GetError() == EC_StreamNotifyClient)
		stream -> ClearError();
	}
    }
    cout << endl << "us: " ;
    for (i = 0; i < 10; i++)
	cout << us[i] << " ";
    cout << endl;


    for(length = 10 * sizeof(Sint16), t = 0;
	length != t; 
	t += stream -> TransferredBytes())
    {
	stream->WriteBytes(&((unsigned char *)ss)[t],length-t);
		
	if (stream -> Avail() == 0)
	{
	    WriteToFile((DcmBufferStream *) stream, outFile);
	    if (stream -> GetError() == EC_StreamNotifyClient)
		stream -> ClearError();
	}
    }
    cout << endl << "ss: " ;
    for (i = 0; i < 10; i++)
	cout << ss[i] << " ";
    cout << endl;


    for(length = 10 * sizeof(Float32), t = 0;
	length != t; 
	t += stream -> TransferredBytes())
    {
	stream->WriteBytes(&((unsigned char *)fl)[t],length-t);
		
	if (stream -> Avail() == 0)
	{
	    WriteToFile((DcmBufferStream *) stream, outFile);
	    if (stream -> GetError() == EC_StreamNotifyClient)
		stream -> ClearError();
	}
    }
    cout << endl << "fl: " ;
    for (i = 0; i < 10; i++)
	cout << fl[i] << " ";
    cout << endl;


    for(length = 10 * sizeof(Float64), t = 0;
	length != t; 
	t += stream -> TransferredBytes())
    {
	stream->WriteBytes(&((unsigned char *)fd)[t],length-t);
		
	if (stream -> Avail() == 0)
	{
	    WriteToFile((DcmBufferStream *) stream, outFile);
	    if (stream -> GetError() == EC_StreamNotifyClient)
		stream -> ClearError();
	}
    }
    cout << endl << "fd: " ;
    for (i = 0; i < 10; i++)
	cout << fd[i] << " ";
    cout << endl;


    for(length = sizeof(size_t), t = 0; 
	t != length; 
	t += stream -> TransferredBytes())
    {
	stream->WriteBytes(&((unsigned char *)&uc_len)[t], length-t);
		
	if (stream -> Avail() == 0)
	{
	    WriteToFile((DcmBufferStream *) stream, outFile);
	    if (stream -> GetError() == EC_StreamNotifyClient)
		stream -> ClearError();
	}
    }

    cout << endl << "uc_len: " ;
    cout << uc_len << endl;

    for(length = uc_len, t = 0; 
	length != t; 
	t += stream -> TransferredBytes())
    {
	stream->WriteBytes(&uc[t], length-t);
		
	if (stream -> Avail() == 0)
	{
	    WriteToFile((DcmBufferStream *) stream, outFile);
	    if (stream -> GetError() == EC_StreamNotifyClient)
		stream -> ClearError();
	}
    }

    if (!stream -> Flush())
	WriteToFile((DcmBufferStream *) stream, outFile);

    cout << endl << "uc: " ;
    cout << uc << endl;
}


int main(int argc, char * argv[])
{

    char filename[80];

    if(argc != 2)
    {
	cerr << "usage: " << argv[0] << " FilenameGroup" << endl;
	return 1;
    }

    cout << endl << "WRITE-Test with file" << endl;
    cout << "====================" << endl << endl;

    strcpy(filename, argv[1]);
    strcat(filename, ".1");
    DcmFileStream * writeFileStream = 
	new DcmFileStream(filename, FALSE);

    WriteTest(writeFileStream, NULL);

    delete writeFileStream;


    cout << endl << "READ-Test with file" << endl;
    cout << "====================" << endl << endl;

    DcmFileStream * readFileStream = 
	new DcmFileStream(filename, TRUE);
   
    ReadTest(readFileStream, NULL, FALSE);

    delete readFileStream;


    cout << endl << "PUTBACK-Test with file" << endl;
    cout << "====================" << endl << endl;

    DcmFileStream * putbackFileStream = 
	new DcmFileStream(filename, TRUE);

    ReadTest(putbackFileStream, NULL, TRUE);

    delete putbackFileStream;


    cout << endl << "WRITE-Test with buffer" << endl;
    cout << "======================" << endl << endl;

    strcpy(filename, argv[1]);
    strcat(filename, ".2");
    FILE * outFile = fopen(filename, "wb");

    DcmBufferStream * writeBufStream = 
	new DcmBufferStream(buffer, BUFLEN, FALSE);

    WriteTest(writeBufStream, outFile);
		
    delete writeBufStream;
    fclose(outFile);


    cout << endl << "READ-Test with buffer" << endl;
    cout << "=====================" << endl << endl;

    FILE * inFile = fopen(filename, "rb");

    DcmBufferStream * readBufStream = 
	new DcmBufferStream(TRUE);
   
    ReadTest(readBufStream, inFile, FALSE);

    delete readBufStream;
    fclose(inFile);


    cout << endl << "PUTBACK-Test with file" << endl;
    cout << "====================" << endl << endl;

    inFile = fopen(filename, "rb");

    DcmBufferStream * putbackBufStream = 
	new DcmBufferStream(TRUE);

    ReadTest(putbackBufStream, inFile, TRUE);
		  
	
    delete putbackBufStream;
    fclose(inFile);
}


/*
** CVS/RCS Log:
** $Log: tststream.cc,v $
** Revision 1.4  1996-04-12 13:19:12  andreas
** Minor changes to support DEC ALPHA and DEC MIPS
**
** Revision 1.3  1996/01/05 13:31:40  andreas
** - new streaming facilities and test routines for blocks and buffers
** - unique read/write methods for block and file transfer
** - more cleanups
**
**
*/
