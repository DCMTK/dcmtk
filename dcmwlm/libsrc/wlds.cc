/*
 *
 *  Copyright (C) 1996-2001, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmwlm
 *
 *  Author:  Thomas Wilkens
 *
 *  Purpose: (Partially) abstract class for connecting to an arbitrary data source.
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 2002-01-08 17:02:55 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/dcmwlm/libsrc/wlds.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

// ----------------------------------------------------------------------------

#include "osconfig.h"  // specific configuration for operating system

#include "dicom.h"     // for DIC_NODENAME etc. used in "wltypdef.h"
#include "wltypdef.h"  // for type definitions
#include "oftypes.h"   // for OFBool
#include "dcdatset.h"  // for DcmDataset
#include "dcvrat.h"    // for DcmAttributTag
#include "dcvrlo.h"    // for DcmLongString
#include "dcdict.h"    // for global variable dcmDataDict
#include "dcdeftag.h"  // for DCM_OffendingElement, ...
#include "dcsequen.h"  // for DcmSequenceOfItems

#include "wlds.h"

// ----------------------------------------------------------------------------

WlmDataSource::WlmDataSource( OFConsole *logStreamv, const OFBool verbosev )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Constructor.
// Parameters   : logStreamv - [in] Stream that can be used to dump information.
//                verbosev   - [in] Verbose mode.
// Return Value : none.
  : objectStatus( WLM_STATUS_UNKNOWN ), failOnInvalidQuery( OFTrue ), calledApplicationEntityTitle( NULL ),
    verbose( verbosev ), identifiers( NULL ), objlist( NULL ), errorElements( NULL ), offendingElements( NULL ),
    errorComment( NULL ), foundUnsupportedOptionalKey( OFFalse ), readLockSetOnDataSource( OFFalse ),
    logStream( logStreamv )
{
  char msg[200];

  // Make sure data dictionary is loaded.
  if( !dcmDataDict.isDictionaryLoaded() )
  {
    sprintf( msg, "Warning: no data dictionary loaded, check environment variable: %s\n", DCM_DICT_ENVIRONMENT_VARIABLE );
    DumpMessage( msg );
    objectStatus = WLM_STATUS_INIT_FAILED;
  }
  else
  {
    // Initialize member variables.
    objlist = new DcmList();
    identifiers = new DcmDataset();
    offendingElements = new DcmAttributeTag( DCM_OffendingElement, 0 );
    errorElements = new DcmAttributeTag( DCM_OffendingElement, 0 );
    errorComment = new DcmLongString( DCM_ErrorComment, 0 );

    // Indicate that the initialization was successful
    objectStatus = WLM_STATUS_OK;
  }
}

// ----------------------------------------------------------------------------

WlmDataSource::~WlmDataSource()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Destructor.
// Parameters   : none.
// Return Value : none.
{
  // free memory
  if( calledApplicationEntityTitle != NULL ) delete calledApplicationEntityTitle;
  ClearObjectList();
  delete objlist;
  ClearDataset(identifiers);
  delete identifiers;
  delete offendingElements;
  delete errorElements;
  delete errorComment;
}

// ----------------------------------------------------------------------------

OFBool WlmDataSource::IsObjectStatusOk()
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Returns OFTrue if the object status is okay.
// Parameters   : none.
// Return Value : OFTrue - The object status is ok.
//                OFFalse - The object status is not ok.
{
  if( objectStatus == WLM_STATUS_OK )
    return OFTrue;
  else
    return OFFalse;
}

// ----------------------------------------------------------------------------

void WlmDataSource::SetCalledApplicationEntityTitle( char *value )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Sets the member variable that specifies called application entity title.
// Parameters   : value - Value for calledApplicationEntityTitle.
// Return Value : none.
{
  if( value != NULL )
  {
    calledApplicationEntityTitle = new char[ strlen( value ) + 1 ];
    strcpy( calledApplicationEntityTitle, value );
  }
}

// ----------------------------------------------------------------------------

void WlmDataSource::SetFailOnInvalidQuery( OFBool value )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Set member variable that determines if a call should fail on an invalid query.
// Parameters   : value - Value for failOnInvalidQuery.
// Return Value : none.
{
  failOnInvalidQuery = value;
}

// ----------------------------------------------------------------------------

void WlmDataSource::SetVerbose( OFBool value )
// Date         : December 10, 2001
// Author       : Thomas Wilkens
// Task         : Set member variable that determines if information shall be dumped at processing time or not.
// Parameters   : value - Value for verbose.
// Return Value : none.
{
  verbose = value;
}

// ----------------------------------------------------------------------------

void WlmDataSource::ClearObjectList()
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function removes all elements from member variable objlist. This variable
//                will be used to capture all records that match the specified search mask.
// Parameters   : none.
// Return Value : none.
{
  // If the list is defined and there are elements in the list
  if( objlist && objlist->card() )
  {
    // go through the list of elements, starting at the first element
    DcmObject *removeObj = NULL;
    objlist->seek(ELP_first);
    for( unsigned long i=0; i<objlist->card(); i++ )
    {
      // and remove all elements
      removeObj = objlist->remove();
      if( removeObj )
        delete removeObj;
    }
  }
}

// ----------------------------------------------------------------------------

void WlmDataSource::ClearDataset( DcmDataset *idents )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function removes all elements from the given DcmDataset object.
// Parameters   : idents - [in] pointer to object which shall be cleared.
// Return Value : none.
{
  // If the given pointer is valid and refers to a non-empty structure
  if( idents && (idents->card()>0) )
  {
    // clear structure
    idents->clear();
  }
}

// ----------------------------------------------------------------------------

OFBool WlmDataSource::CheckIdentifiers( DcmDataset *idents )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function checks if all elements in the search mask are supported matching key
//                attributes. It returns OFTrue if this is the case, OFFalse if this is not the case.
// Parameters   : idents - [in] Contains the search mask.
// Return Value : OFTrue  - No invalid matching key attribute was found in the search mask.
//                OFFalse - At least one invalid matching key attribute was found in the search mask.
{
  char msg[200];

  // ### wilkens: Beim Aktualisieren dieser Funktion muß beachtet werden, daß in der Suchmaske
  // ### nicht notwendigerweise alle "required" matching key attributes vorhanden sein müssen
  // ### und auch vorhandene required matching key attributes müssen nicht notwendigerweise einen
  // ### Wert besitzen (d.h. sie können in der Suchmaske auch einen Unbekannt-Wert enthalten.
  // ### (siehe 2001er Standard, Teil 4, Abschnitt K.2.2.1.1.1)

  // ### Außerdem: Was ist mit einer leeren Suchmaske? Wenn diese kein Element enthält, dann
  // ### wird ja auch am Ende was leeres zurückgeliefert. Diesen Sonderfall sollten wir mal
  // ### abfangen. Dazu steht auch bestimmt was im Standard.

  // Initialize counter that counts invalid elements in the search mask. This
  // variable will later be used to determine the return value for this function.
  int invalidMatchingKeyCount = 0;

  // remember the number of data elements in the search mask 
  unsigned long cids = idents->card();

  // dump some information if required 
  if( verbose )
    DumpMessage( "Checking the search mask." );

  // this member variable indicates if we encountered an unsupported 
  // optional key attribute in the search mask; initialize it with false. 
  foundUnsupportedOptionalKey = OFFalse;

  // start a loop; go through all data elements in the search mask, for each element, do some checking 
  for( unsigned long i=0; i<cids ; i++ )
  {
    // determine current element 
    DcmElement *element = idents->getElement(i);

    // determine current element's length 
    Uint32 length = element->getLength();

    // determine current element's tag 
    DcmTag tag( element->getTag() );

    // if the current element is NOT a sequence 
    if( element->ident() != EVR_SQ )
    {
      // figure out if the current element refers to an attribute which 
      // is a supported matching key attribute. Use level 0 for this check. 
      if( IsSupportedMatchingKeyAttribute( tag.getXTag(), 0 ) )
      {
        // if the current element is a supported matching key attribute, do nothing (everything is ok) 
      }
      // if current element refers to an attribute which is not a supported 
      // matching key attribute, figure out if the current element refers to an 
      // attribute which is a supported return key attribute. Use level 0 for this check. 
      else if( IsSupportedReturnKeyAttribute( tag.getXTag(), 0 ) )
      {
        // if the current element refers to a supported return key attribute
        // everything is okay as long as this attribute does not contain a
        // value. In general, according to the DICOM standard it does not make
        // sense that a return key attribute which is NOT a matching key attribute
        // contains a value. Hence, we specify that in such a case (i.e. if the
        // current element's length does not equal 0) we've found an unsupported
        // optional key attribute. Note that we do not want to remember this attribute
        // in the list of error elements.
        if (length != 0)
        {
          // ### wilkens: Ich lasse das mal hier so stehen, aber wir müssen nochmal im Standard
          // ### schauen, ob das hier wirklich so gemacht werden soll. In diesem Fall haben wir
          // ### erkannt, dass ein nicht-Sequenz Attribut - welches nur Return Key Attribute ist
          // ### und nicht Matching Key Attribute - in der Suchmaske einen Wert enthält. Dies
          // ### macht ja so ohnehin keinen Sinn. Welches Verhalten schlägt der Standard an dieser
          // ### Stelle vor?  Ich würde sagen: Fehlermeldung ausgeben und Matching abbrechen.
          foundUnsupportedOptionalKey = OFTrue;
        }
      }
      // if current element does neither refer to a supported matching 
      // key attribute nor does it refer to a supported return key attribute, 
      // we've found an unsupported optional key attribute; we want to remember 
      // this attribute in the list of error elements. 
      else
      {
        foundUnsupportedOptionalKey = OFTrue;
        PutErrorElements( tag );
        // ### wilkens: Auch hier: Wie sollen wir vorgehen, wenn wir in der Suchmaske ein Element
        // ### gefunden haben, das von der gegenwärtigen Implementierung nicht unterstützt wird?
        // ### Der Standard sagt, daß manche Attribute unbedingt unterstützt werden müssen (manche
        // ### von diesen nur wenn eine bestimmte Bedingung erfüllt ist), andere sind optional.
        // ### Um herauszufinden, welchen unbedingt unterstützt werden müssen, dient der Type eines
        // ### Attributes, der entweder 1, 1C, 2, 2C oder 3 sein kann. Wenn wir ein Attribut in der
        // ### Suchmaske finden, das nicht unterstützt wird, wollen wir zumindest sicher eine
        // ### Warnmeldung ausgeben.
      }

      // check if the current element meets certain conditions (at the moment 
      // we only check if the element's data type and value go together) 
      // ### wilkens: Diese Methode sollte umbenannt werden, da wir nicht nur die MatchingKeys prüfen,
      // ### sondern alle gegebenen Attribute. Oder wir verschieben sie nach oben, wo sichergestellt
      // ### ist, daß es sich um einen supprted MatchingKeyAttribut handelt.
      if( !CheckMatchingKey( element ) )
      {
        // if there is a problem with the current element, increase the corresponding counter 
        invalidMatchingKeyCount++;
      }
    }
    // or if the current element IS a sequence 
    else
    {
      // figure out if the current sequence element refers to an attribute which 
      // is a supported matching key attribute. Use level 0 for this check. 
      if( IsSupportedMatchingKeyAttribute( tag.getXTag(), 0 ) )
      {
        // if the current element is a supported matching key attribute, check its length 
        if( length != 0 )
        {
          // if length does not equal 0, check inside the sequence 

          // remember that elements is a sequence of items 
          DcmSequenceOfItems *sequits = (DcmSequenceOfItems*)element;

          // check the cardinality of the sequence; note that we may 
          // not have more than 1 item in a sequence item whithin the search mask
          // ### wilkens: Hier wird geprüft, ob wir eventuell mehr
          // ### als ein Item in einer Sequenz in der Suchmaske haben. Diese Prüfung wird in
          // ### WlmDataSourceFiles::StartFindRequest nicht gemacht. Für den Fall, daß eine
          // ### ungültige Suchmaske trotzdem bearbeitet wird, sollte man eine entsprechende
          // ### Prüfung dort noch einfügen.
          if( sequits->card() > 1 )
          {
            PutOffendingElements(tag);
            errorComment->putString("More than 1 item in sequence.");
            sprintf( msg, "WlmDataSource::CheckIdentifiers : Error: More than one item in sequence %s (search mask) encountered.", tag.getTagName() );
            DumpMessage( msg );
            invalidMatchingKeyCount++;
          }
          // ### wilkens: Außerdem darf eine Sequenz auch nicht 0 Items haben, das muß hier
          // ### auch überprüft werden (oder ist das eventuell schon mit dem noch einzufügenden
          // ### else-Zweig der if-Anweisung "if( length != 0 )" erledigt? Könnte sein.)

          // get the first item 
          DcmItem *item = sequits->getItem(0);

          // determine the cardinality of this item 
          unsigned long cit = item->card();

          // go through all elements of this item 
          for( unsigned long k=0 ; k<cit ; k++ )
          {
            // ### wilkens: hier geht es wieder von vorne los, wir müssen die Funktion irgendwie rekursiv
            // ### aufrufen um eine allgemeine Schachtelungstiefe von Sequenzen zu berücksichtigen. (auch
            // ### wenn momentan nur eine Schachtelungstiefe von 2 vorhanden sein kann.)

            // determine the current element
            DcmElement *elementseq = item->getElement(k);

            // determine the length of the current element 
            Uint32 length1 = elementseq->getLength();

            // determine the current element's tag 
            DcmTag tagseq( elementseq->getTag() );

            // figure out if the current element refers to an attribute which 
            // is a supported matching key attribute. Use level 1 for this check. 
            if( IsSupportedMatchingKeyAttribute( tagseq.getXTag(), 1 ) )
            {
              // if the current element is a supported matching key attribute, do nothing (everything is ok) 
            }
            // if current element refers to an attribute which is not a supported 
            // matching key attribute, figure out if the current element refers to an 
            // attribute which is a supported return key attribute. Use level 1 for this check. 
            else if( IsSupportedReturnKeyAttribute( tagseq.getXTag(), 1 ) )
            {
              // if the current element refers to a supported return key attribute
              // everything is okay as long as this attribute does not contain a
              // value. In general, according to the DICOM standard it does not make
              // sense that a return key attribute which is NOT a matching key attribute
              // contains a value. Hence, we specify that in such a case (i.e. if the
              // current element's length does not equal 0) we've found an unsupported
              // optional key attribute. Note that we do not want to remember this attribute
              // in the list of error elements.
              if( length1 != 0 )
              {
                // ### wilkens: Ich lasse das mal hier so stehen, aber wir müssen nochmal im Standard
                // ### schauen, ob das hier wirklich so gemacht werden soll. In diesem Fall haben wir
                // ### erkannt, dass ein nicht-Sequenz Attribut - welches nur Return Key Attribute ist
                // ### und nicht Matching Key Attribute - in der Suchmaske einen Wert enthält. Dies
                // ### macht ja so ohnehin keinen Sinn. Welches Verhalten schlägt der Standard an dieser
                // ### Stelle vor? Ich würde sagen: Fehlermeldung ausgeben, Matching abbrechen.
                foundUnsupportedOptionalKey = OFTrue;
              }
            }
            // if current element does neither refer to a supported matching 
            // key attribute nor does it refer to a supported return key attribute, 
            // we've found an unsupported optional key attribute; we want to remember 
            // this attribute in the list of error elements. 
            else
            {
              foundUnsupportedOptionalKey = OFTrue;
              PutErrorElements(tagseq);
            }

            // check if the current element meets certain conditions (at the moment 
            // we only check if the element's data type and value go together) 
            if( !CheckMatchingKey( elementseq ) )
            {
              // if there is a problem with the current element, increase the corresponding counter 
              invalidMatchingKeyCount++;
            }
          }

#ifdef DELETE_UNEXPECTED_ELEMENTS_FROM_IDENTIFIER
          // ### wilkens: Was ist hiermit? Soll das immer gemacht werden? Es macht Sinn, da solche
          // ### Attribute in der Suchmaske am Ende nur Schwierigkeiten machen. Ich glaube, die
          // ### Frage muß entschieden werden, wenn ich die Matchroutine durchgesehen habe.
          // ### Nach dem Durchsehen der Matchroutine: Das hier sollte nicht gemacht werden, da
          // ### der SCU sicherlich alle zu uns übertragenen Attribute wieder bei sich in der
          // ### Rückgabe erwartet.
          // remove unexpected (error) elements from this item 
          unsigned long d = errorElements->getVM();
          for( unsigned long e=0 ; e<d ; e++ )
          {
            DcmTag errortag;
            errortag = errorElements->get(e);
            delete item->remove(errortag);
          }
#endif
        }
        // ### wilkens: Die Laenge einer Sequenz in der Suchmaske darf laut Standard niemals 0 sein, da
        // ### eine Sequenz in der Suchmaske immer genau ein Item beinhalten soll (das seinerseits jedoch
        // ### leer sein kann (siehe 2001er Standard, Teil 4, Abschnitt C.2.2.2.6). Wir müssen also hier
        // ### einen else-Zweig anfügen, der eine Warnung ausgibt, wenn die Länge einer Sequenz 0 ist.
        // ### (Oder welches andere Verhalten soll implementiert werden? Fehlermeldung und Matching abbrechen?)
        // ### (die Philips Modalität im Herzkatheterlabor der Städtischen Kliniken überträgt solche Sequenzen
        // ### mit Länge 0!) (ACHTUNG! Hier müssen wir aufpassen, was in dcmtk passiert, wenn eine
        // ### Sequenz mit genau einem leeren Item übertragen wird! Wird dieses Item wegoptimiert?
        // ### Das mal genau überprüfen! Insbesondere müssen wir genau checken, worauf sich length
        // ### hier in bezug auf die Sequenz bezieht!
      }
      // if current sequence element refers to an attribute which is not a supported 
      // matching key attribute, figure out if the current sequence element refers to an 
      // attribute which is a supported return key attribute. Use level 0 for this check. 
      else if( IsSupportedReturnKeyAttribute( tag.getXTag(), 0 ) )
      {
        // if this is the case and the current element's length does 
        // not equal 0, we've found an unsupported optional key attribute 
        if( length != 0 )
        {
          // ### wilkens: Auch hier lasse ich es mal so stehen, aber wir müssen auch nochmal im Standard
          // ### schauen, ob das hier wirklich so gemacht werden soll. In diesem Fall haben wir
          // ### erkannt, dass ein Sequenz Attribut - welches nur Return Key Attribute ist
          // ### und nicht Matching Key Attribute - in der Suchmaske eine Länge > 0 enthält. Dies
          // ### kann Sinn machen, und das aktuell implementierte Verhalten ist dann falsch.

          // ### wilkens (später): In der Tat macht das Sinn. Es kann ja sein, das der SCU Daten anfordern
          // ### möchte, die in innerhalb der Sequenz definierten Attributen stehen. Auch hier müssen
          // ### wir also in die Sequenz hineingehen, und schauen, was für Attribute vorhanden sind.
          // ### Allerdings dürfen hier keine Matching Key Attribute mehr vorkommen, ansonsten wäre die
          // ### Sequenz ja auch ein Matching Key Attribut. Die aktuelle Implementierung muß also
          // ### verändert werden.
          foundUnsupportedOptionalKey = OFTrue;
        }
      }
      // if current element does neither refer to a supported matching key
      // attribute nor does it refer to a supported return key attribute, 
      // we've found an unsupported optional key attribute; we want to remember 
      // this attribute in the list of error elements. 
      else
      {
        foundUnsupportedOptionalKey = OFTrue;
        PutErrorElements( tag );
        // ### wilkens: Auch hier: Wie sollen wir vorgehen, wenn wir was gefunden haben, was
        // ### von der gegenwärtigen Implementierung nicht unterstützt wird? Was sagt der Standard?
        // ### Momentan führt das lediglich zu einem bestimmten PENDING_WARNING anstatt einem PENDING
        // ### im result status.
      }
    }
  }

#ifdef DELETE_UNEXPECTED_ELEMENTS_FROM_IDENTIFIER
  // ### wilkens: Was ist hiermit? Soll das immer gemacht werden? Es macht Sinn, da solche
  // ### Attribute in der Suchmaske am Ende nur Schwierigkeiten machen. Ich glaube, die
  // ### Frage muß entschieden werden, wenn ich die Matchroutine durchgesehen habe.
  // ### Nach dem Durchsehen der Matchroutine: Das hier sollte nicht gemacht werden, da
  // ### der SCU sicherlich alle zu uns übertragenen Attribute wieder bei sich in der
  // ### Rückgabe erwartet.

  // remove unexpected (error) elements from this item 
  unsigned long d = errorElements->getVM();
  for( unsigned long h=0 ; h<d ; h++ )
  {
    DcmTag errortag;
    errortag = errorElements->get(h);
    delete idents->remove(errortag);
  }
#endif

  // if there was more than 1 error, override the error comment 
  if( invalidMatchingKeyCount > 1 )
    errorComment->putString("Syntax error in 1 or more matching keys");

  // return OFTrue or OFFalse depending on the number of invalid matching attributes 
  return( invalidMatchingKeyCount == 0 );
}

// ----------------------------------------------------------------------------

void WlmDataSource::PutOffendingElements( DcmTagKey &tag )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function inserts the tag of an offending element into the
//                corresponding member variable, unless this tag is already con-
//                tained in this variable.
// Parameters   : tag - [in] The tag that shall be inserted.
// Return Value : none.
{
  DcmTagKey errortag;

  // determine how many offending elements there have been so far
  unsigned long d = offendingElements->getVM();

  // if this is the first one, insert it at position 0
  if( d==0 )
  {
    offendingElements->putTagVal(tag, 0);
  }
  // if this is not the first one, insert it at the end
  // but only if it is not yet contained.
  else
  {
    OFBool tagFound = OFFalse;
    for( unsigned long j=0 ; j<d && !tagFound ; j++ )
    {
      offendingElements->getTagVal( errortag, j );
      if( errortag == tag )
        tagFound = OFTrue;
    }

    if( !tagFound )
      offendingElements->putTagVal( tag, d );
  }
}

// ----------------------------------------------------------------------------

void WlmDataSource::PutErrorElements( DcmTagKey &tag )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function inserts the tag of an error element into the
//                corresponding member variable, without checking if it is already
//                contained in this variable.
// Parameters   : tag - [in] The tag that shall be inserted.
// Return Value : none.
{
  // insert tag
  errorElements->putTagVal( tag, errorElements->getVM() );
}

// ----------------------------------------------------------------------------

OFBool WlmDataSource::CheckMatchingKey( DcmElement *elem )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function checks if the passed element's value only uses characters
//                which are part of the element's data type's character repertoire.
//                In general, note that the checking needs to be more comprehensive here.
//                The checks here only cover a few of the problems which can be encountered.
// Parameters   : elem - [in] Element which shall be checked.
// Return Value : OFTrue  - The given element's value only uses characters which are part of
//                          the element's data type's character repertoire.
//                OFFalse - The given element's value does not only use characters which are part of
//                          the element's data type's character repertoire.
{
// ### wilkens: Sind das alle Datentypen, die vorkommen können? Soll
// ### das Prüfen noch erweitert werden?? hmm ich hab mal geschaut:
// ### Es kann auch noch mindestens LO, PN, SH und AE vorkommen. Und SQ,
// ### aber das ist ja hier nicht relevant.
  OFBool ok = OFTrue;
  char *val = NULL;

  switch( elem->ident() )
  {
    case EVR_DA:
      val = GetStringValue( elem );
      // although normally DA is 8 bytes fixed length, in a query it
      // can be a date range (with a range matching symbol '-') and
      // thus need space padding (max 18 bytes).
      if( !CheckCharSet( val, "0123456789.- " ) )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoir for DA");
        ok = OFFalse;
      }
      break;

    case EVR_TM:
      val = GetStringValue( elem );
      // although normally TM is max 16 bytes length, in a query it
      // can be up to 33 bytes (with a range matching symbol '-') and
      // thus need space padding.
      if( !CheckCharSet( val, "0123456789.:- " ) )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoir for TM");
        ok = OFFalse;
      }
      break;

    case EVR_CS:
      val = GetStringValue( elem );
      // possible wildcards & space padding
      if( !CheckCharSet( val, "*.ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 _" ) )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoir for CS");
        ok = OFFalse;
      }
      break;

    case EVR_AS:
      val = GetStringValue( elem );
      // possible wildcards & space padding
      if( !CheckCharSet( val, "*.0123456789DWMY " ) )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoir for AS");
        ok = OFFalse;
      }
      break;

    case EVR_DS:
      val = GetStringValue( elem );
      // possible space padding
      if( !CheckCharSet( val, "0123456789+-Ee. " ) )
      {
        DcmTag tag(elem->getTag());
        PutOffendingElements(tag);
        errorComment->putString("Invalid Character Repertoir for DS");
        ok = OFFalse;
      }
      break;

    case EVR_IS:
      val = GetStringValue( elem );
      // possible space padding
      if( !CheckCharSet( val, "0123456789+-. " ) )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoir for IS");
        ok = OFFalse;
      }
      break;

    case EVR_UI:
      val = GetStringValue( elem );
      // possible multiple values (spaces are not allowed)
      if( !CheckCharSet( val, "0123456789.\\" ) )
      {
        DcmTag tag( elem->getTag() );
        PutOffendingElements( tag );
        errorComment->putString("Invalid Character Repertoir for UI");
        ok = OFFalse;
      }
      break;

    default:
      break;
  }

  return ok;
}

// ----------------------------------------------------------------------------

OFBool WlmDataSource::CheckCharSet( const char *s, const char *charset )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function returns true if all the characters of s can be found
//                in the string charset.
// Parameters   : s       - [in] String which shall be checked.
//                charset - [in] Possible character set for s. (valid pointer expected.)
// Return Value : 
{
  OFBool result = OFTrue;

  // check parameter
  if( s == NULL )
  {
    return OFTrue;
  }

  // return true if all the characters of s can be found in the string charset.
  int s_len = strlen( s );
  int charset_len = strlen( charset );
  for( int i=0 ; i<s_len && result ; i++ )
  {
    OFBool isSetMember = OFFalse;
    for( int j=0 ; !isSetMember && j<charset_len ; j++ )
    {
      if( s[i] == charset[j] )
        isSetMember = OFTrue;
    }

    if( !isSetMember )
      result = OFFalse;
  }

  return( result );
}

// ----------------------------------------------------------------------------

char *WlmDataSource::GetStringValue( DcmElement *elem )
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function returns the value of the given DICOM string element (attribute).
//                If the element does not refer to a string attribute, a NULL pointer is returned.
// Parameters   : elem - [in] The DICOM element.
// Return Value : The value of the given DICOM (string) element or NULL.
{
  char *val = NULL;

  if( elem && elem->isaString() )
    elem->getString( val );

  return val;
}

// ----------------------------------------------------------------------------

DcmAttributeTag *WlmDataSource::GetOffendingElements()
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function returns offending elements from member variable offendingElements
//                if there are any.
// Parameters   : none.
// Return Value : Pointer to offending elements or NULL if there are none.
{
  if( offendingElements->getLength() == 0 )
    return NULL;
  else
    return offendingElements;
}

// ----------------------------------------------------------------------------

DcmLongString *WlmDataSource::GetErrorComments()
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function returns error comments from member variable errorComment
//                if there are any.
// Parameters   : none.
// Return Value : Pointer to error comments or NULL if there are none.
{
  if( errorComment->getLength() == 0 )
    return NULL;
  else
    return errorComment;
}

// ----------------------------------------------------------------------------

WlmDataSourceStatusType WlmDataSource::CancelFindRequest()
// Date         : 1995
// Author       : Andrew Hewett
// Task         : This function handles a C-CANCEL Request during the processing of a C-FIND Request.
//                In detail, the member variable objlist is cleared.
// Parameters   : none.
// Return Value : WLM_CANCEL.
{
  // clear objlist
  ClearObjectList();

  // return WLM_CANCEL
  return WLM_CANCEL;
}

// ----------------------------------------------------------------------------

void WlmDataSource::DumpMessage( const char *message )
// Date         : 1995
// Author       : Jörg Riesmeier
// Task         : This function dumps the given information on a stream.
//                Used for dumping information in normal, debug and verbose mode.
// Parameters   : message - [in] The message to dump.
// Return Value : none.
{
  if( logStream != NULL && message != NULL )
  {
    logStream->lockCout();
    logStream->getCout() << message << endl;
    logStream->unlockCout();
  }
}

// ----------------------------------------------------------------------------

/*
** CVS Log
** $Log: wlds.cc,v $
** Revision 1.2  2002-01-08 17:02:55  joergr
** Added preliminary database support using OTL interface library (modified by
** MC/JR on 2001-12-21).
**
** Revision 1.1  2002/01/08 16:32:46  joergr
** Added new module "dcmwlm" developed by Thomas Wilkens (initial release for
** Windows, dated 2001-12-20).
**
**
*/
