/*
 *
 *  Copyright (C) 1997-99, OFFIS
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
 *  Module:  ofstd
 *
 *  Author:  Andreas Barth
 *
 *  Purpose: test programm for classes OFList and OFListIterator
 *
 *  Last Update:      $Author: joergr $
 *  Update Date:      $Date: 1998-11-27 12:42:10 $
 *  Source File:      $Source: /export/gitmirror/dcmtk-git/../dcmtk-cvs/dcmtk/ofstd/tests/tstlist.cc,v $
 *  CVS/RCS Revision: $Revision: 1.2 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */


#include <iostream.h>
#include "ofalgo.h"
#include "oflist.h"

void print(OFList<int> & l)
{
    OFListIterator(int) e(l.end());
    cout << "Werte der List: ";
    for (OFListIterator(int) i(l.begin()); i != e; ++i)
	cout << *i << " ";
    cout << endl;
}

int main()
{
    OFList<int> l;
    l.push_front(1);
    l.push_front(2);
    l.push_back(3);
    l.push_back(4);
    l.push_front(5);

    print(l);

    OFListIterator(int) a(l.begin());
    ++a; ++a;
    cout << "2xa++ :Was ist jetzt a? " << *a << endl;
    l.insert(a, 50);
    l.insert(a, 10, 100);

    OFListIterator(int) e(l.end());

    cout << "Finde 50: "; 
    if (OFFind(OFListIterator(int), int, l.begin(), e, 50) != e)
	cout << "gefunden, wie erwartet\n";
    else
	cout << "nicht gefunden, Fehler\n"; 

    cout << "Ausgabe der Liste l\n";
    print(l);

    cout << "Kopiere Liste l in l1,\n";
    OFList<int> l1(l);
    cout << "Ausgabe der List l1\n";
    print(l1);

    cout << "Loesche alle 100er (nacheinander) aus l" << endl;

    OFListIterator(int) del;
    a = l.begin();
    while(a != e)
    {
	del = a++;
	if (*del == 100)
	{
	    l.erase(del);
	    cout << "Ein 100er Element geloescht\n";
	    print(l);
	}
    }


    cout << "Anzahl Elemente in l: " << l.size() << endl;
    cout << "Loesche Werte mit pop_front aus l: ";
    while(!l.empty())
    {
	cout << l.front() << " ";
	l.pop_front();
    }
    cout << endl;

    cout << "Loesche letztes Element aus l1: " << l1.back() 
	 << " und gebe l aus:\n";
    l1.pop_back();
    print(l1);

    cout << "Suche 1 in l1: " << endl;
    a = OFFind(OFIterator<int>, int, l1.begin(), l1.end(), 1);
    if ( a == l1.end())
	cout << "Fehler, 1 nicht gefunden\n";
    else
	cout << "1 gefunden (wie erwartet)\n";

    cout << "Loesche alle Elemente hinter ab der  1 und gebe Ergebnis aus.\n";
    l1.erase(a, l1.end());
    print(l1);
    cout << "Loesche die 50 und geben restliche List aus\n";
    l1.remove(50);
    print(l1);
    
    cout << " Aufruf des Destruktors \n";
}


/*
**
** CVS/RCS Log:
** $Log: tstlist.cc,v $
** Revision 1.2  1998-11-27 12:42:10  joergr
** Added copyright message to source files and changed CVS header.
**
**
**
*/
