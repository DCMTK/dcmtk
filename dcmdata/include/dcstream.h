/*
 *
 * Author: Gerd Ehlers      Created:  03-26-94
 *                          Modified: 07-17-95
 *
 * Module: dcstream.h
 *
 * Purpose:
 * Binary streams implmented with stdio
 *
 *
 * Last Update:   $Author: hewett $
 * Revision:      $Revision: 1.2 $
 * Status:	  $State: Exp $
 *
 */

#ifndef DCSTREAM_H
#define DCSTREAM_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include <stdio.h>
#include "dctypes.h"
#include "dcerror.h"
#include "dcxfer.h"


// ********************************************************
// *** input **********************************************
// ********************************************************

#define DCM_InFile 1
#define DCM_OutFile 2
#define DCM_NoCreateFile 4
#define DCM_BinaryFile 8
#define DCM_TruncFile 16


// Bis zu dieser Grenze werden Datenelemente (Value-Field) aus Dateien sofort
// gelesen. Der Rest wird erst bei einem Zugriff darauf nachgeladen. Stammen
// die Daten nicht aus einer Datei, so werden alle Datenelemente gelesen.
#define READ_MAX_BYTE 4096

#define MIN_BUFFER_LENGTH 200;


class iDicomStream 
{
private:
    FILE *      inFile;
    BOOL        fileOperationOK;  
    int         fileOperationErrno;
    BOOL	flagStreamFromFile;
    E_ByteOrder machineByteOrder;
    E_ByteOrder dataByteOrder;
    BOOL	mustSwap;
    char	*buffer;
    BOOL        EndOfFile;
    T_VR_UL     BufLen;
    T_VR_UL	startBuf;
    T_VR_UL	endBuf;
    T_VR_UL     numChars;
    T_VR_UL	_gcount;
    T_VR_UL     _gposition;

    iDicomStream& readFromFile(   char* val, int size );
    iDicomStream& readFromBuffer( char* val, int size );

protected:
    iDicomStream& readSw( char* val, int val_width, int times );

public:

    // CONSTUCTORS
    iDicomStream( const char *name,
                  int mode=DCM_InFile|DCM_NoCreateFile|DCM_BinaryFile,
                  int prot=0444 );

    iDicomStream( T_VR_UL bufferLength );
    iDicomStream( const iDicomStream &newiDicom );

    // DESTRUCTORS
    ~iDicomStream();

    // STREAMPOSITION
    long     tellg();
    inline T_VR_UL gcount() { return _gcount; }
    void seekg(long offset);

    // STATE HANDLING
    inline int rdstate() { return fileOperationErrno; }
    inline void clear() { fileOperationOK = TRUE; fileOperationErrno = 0; }
    int good();
    inline int fail() { return !good(); }
    int eof();
    T_VR_UL buffered();
    T_VR_UL avail();


    // BUFFER HANDLING
    E_Condition   fillBuffer( char *buf, T_VR_UL len );
    E_Condition   markBufferEOF();


    // UNGET Operations on File Streams
    iDicomStream& putback( char ch );
    iDicomStream& unget();

    // MISC
    T_VR_UL	  maximumLength()		{ return READ_MAX_BYTE; }
    BOOL	  fromFile()			{ return flagStreamFromFile; }
    void	  setDataByteOrder( E_TransferSyntax ts );
    void	  setDataByteOrder( E_ByteOrder bo );
    E_ByteOrder   getDataByteOrder()		{ return dataByteOrder; }


    // READ Operations

    iDicomStream& operator>> (char& val) {
        readSw( &val, sizeof(val), 1 );
	return *this;
    }
    iDicomStream& operator>> (unsigned char& val) {
        readSw( (char*)&val, sizeof(val), 1 );
	return *this;
    }
    iDicomStream& operator>> (T_VR_SS& val) {
        readSw( (char*)&val, sizeof(val), 1 );
	return *this;
    }
    iDicomStream& operator>> (T_VR_US& val) {
        readSw( (char*)&val, sizeof(val), 1 );
	return *this;
    }
    iDicomStream& operator>> (T_VR_SL& val) {
        readSw( (char*)&val, sizeof(val), 1 );
	return *this;
    }
    iDicomStream& operator>> (T_VR_UL& val) {
        readSw( (char*)&val, sizeof(val), 1 );
	return *this;
    }
    iDicomStream& operator>> (T_VR_FL& val) {
        readSw( (char*)&val, sizeof(val), 1 );
	return *this;
    }
    iDicomStream& operator>> (T_VR_FD& val) {
        readSw( (char*)&val, sizeof(val), 1 );
	return *this;
    }

    iDicomStream& read( char& c ) {
        readSw( &c, sizeof(c), 1 );
        return *this;
    }
    iDicomStream& read( unsigned char& c ) {
        readSw( (char*)&c, sizeof(c), 1 );
        return *this;
    }
    iDicomStream& read( char* val, int size ) {
        readSw( val, sizeof(*val), size );
        return *this;
    }
    iDicomStream& read( unsigned char* val, int size ) {
        readSw( (char*)val, sizeof(*val), size );
	return *this;
    }
    iDicomStream& read( T_VR_SS* val, int size ) {
        readSw( (char*)val, sizeof(*val), size );
	return *this;
    }
    iDicomStream& read( T_VR_US* val, int size ) {
        readSw( (char*)val, sizeof(*val), size );
	return *this;
    }
    iDicomStream& read( T_VR_SL* val, int size ) {
        readSw( (char*)val, sizeof(*val), size );
	return *this;
    }
    iDicomStream& read( T_VR_UL* val, int size ) {
        readSw( (char*)val, sizeof(*val), size );
	return *this;
    }
    iDicomStream& read( T_VR_FL* val, int size ) {
        readSw( (char*)val, sizeof(*val), size );
	return *this;
    }
    iDicomStream& read( T_VR_FD* val, int size ) {
        readSw( (char*)val, sizeof(*val), size );
	return *this;
    }
};



// ********************************************************
// *** output *********************************************
// ********************************************************



class oDicomStream 
{
private:
    FILE *      outFile;
    BOOL        fileOperationOK;
    int         fileOperationErrno;
    BOOL	flagStreamIntoFile;
    E_ByteOrder machineByteOrder;
    E_ByteOrder dataByteOrder;
    BOOL	mustSwap;
    char	*buffer;
    BOOL        EndOfFile;
    T_VR_UL	BufLen;
    T_VR_UL	startBuf;
    T_VR_UL	endBuf;
    T_VR_UL     numChars;
    T_VR_UL	_pcount;
    T_VR_UL     _pposition;

    oDicomStream& writeIntoFile(   const char* val, int size );
    oDicomStream& writeIntoBuffer( const char* val, int size );

protected:
    oDicomStream& writeSw( const char* val, int val_width, int times );

public:

    // CONSTUCTORS
    oDicomStream( const char *name,
                  int mode=DCM_OutFile|DCM_BinaryFile|DCM_TruncFile,
		  int prot=0664 );


    oDicomStream( T_VR_UL bufferLength );
    oDicomStream( const oDicomStream &newoDicom );

    // DESDTRUCTORS
    ~oDicomStream();

    // CLOSING Stream
    void close();


    // STREAMPOSITION
    long   tellp();
    inline T_VR_UL pcount() { return _pcount; }


    // STATE HANDLING
    inline int rdstate() { return fileOperationErrno; }
    inline void clear() { fileOperationOK = TRUE; fileOperationErrno = 0; }
    int 	good();
    inline int fail() { return !good(); }
    T_VR_UL     buffered();
    T_VR_UL     avail();
    BOOL	intoFile()		    { return flagStreamIntoFile; }

    // BUFFER HANDLING
    E_Condition readBuffer( char* buf, T_VR_UL size, T_VR_UL *realsize );
    E_Condition markBufferEOF();


    // BYTE ORDERING
    void	setDataByteOrder( E_TransferSyntax ts );
    void	setDataByteOrder( E_ByteOrder bo );
    E_ByteOrder getDataByteOrder()	    { return dataByteOrder; }


    // WRITE Operations
    oDicomStream& operator<< (char val) {
	writeSw( (char*)&val, 1, 1 );
	return *this;
    }
    oDicomStream& operator<< (unsigned char val) {
	writeSw( (char*)&val, 1, 1 );
	return *this;
    }
    oDicomStream& operator<< (T_VR_SS val) {
	writeSw( (char*)&val, sizeof(val), 1 );
	return *this;
    }
    oDicomStream& operator<< (T_VR_US val) {
	writeSw( (char*)&val, sizeof(val), 1 );
	return *this;
    }
    oDicomStream& operator<< (T_VR_SL val) {
	writeSw( (char*)&val, sizeof(val), 1 );
	return *this;
    }
    oDicomStream& operator<< (T_VR_UL val) {
	writeSw( (char*)&val, sizeof(val), 1 );
	return *this;
    }
    oDicomStream& operator<< (T_VR_FL val) {
	writeSw( (char*)&val, sizeof(val), 1 );
	return *this;
    }
    oDicomStream& operator<< (T_VR_FD val) {
	writeSw( (char*)&val, sizeof(val), 1 );
	return *this;
    }

    oDicomStream& write( const char c ) {
	writeSw( &c, 1, 1 );
	return *this;
    }
    oDicomStream& write( const unsigned char c ) {
	writeSw( (const char*)&c, 1, 1 );
	return *this;
    }
    oDicomStream& write( const char* val, int size ) {
	writeSw( (const char*)val, 1, size );
	return *this;
    }
    oDicomStream& write( const unsigned char* val, int size ) {
	writeSw( (const char*)val, 1, size );
	return *this;
    }
    oDicomStream& write( const T_VR_SS* val, int size ) {
	writeSw( (const char*)val, sizeof(*val), size );
	return *this;
    }
    oDicomStream& write( const T_VR_US* val, int size ) {
	writeSw( (const char*)val, sizeof(*val), size );
	return *this;
    }
    oDicomStream& write( const T_VR_SL* val, int size ) {
	writeSw( (const char*)val, sizeof(*val), size );
	return *this;
    }
    oDicomStream& write( const T_VR_UL* val, int size ) {
	writeSw( (const char*)val, sizeof(*val), size );
	return *this;
    }
    oDicomStream& write( const T_VR_FL* val, int size ) {
	writeSw( (const char*)val, sizeof(*val), size );
	return *this;
    }
    oDicomStream& write( const T_VR_FD* val, int size ) {
	writeSw( (const char*)val, sizeof(*val), size );
	return *this;
    }
};



#endif // DCSTREAM_H

