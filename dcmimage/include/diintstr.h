/*********************************************************************
** 
**  DicomIntegerString (Header)
**
**  author   : Joerg Riesmeier
**  created  : 15.12.96
**  modified : 25.04.97
**
*********************************************************************/


#ifndef __DIINTSTR_H
#define __DIINTSTR_H

#include "osconfig.h"
#include "dcvris.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiIntegerString : public DcmIntegerString
{
 public:
	inline void getValue(Sint32 &returnVal, unsigned long pos = 0, const char sep = '\\')
	{
		char *p;
		char *str = NULL;
		DcmIntegerString::getString(str);
		while (((p = strchr(str, sep)) != NULL) && (pos > 0))
		{
			str = p + 1;
			pos--;
		}
		if (pos == 0)
		{
			if (p == NULL)
				returnVal = atol(str);
			else
			{
				char *nstr = new char[p - str + 1];
				if (nstr != NULL)
				{
					*nstr = '\0';
					strncat(nstr, str, p - str);
					returnVal = atol(nstr);
					delete nstr;
				} 
			}
		} 
	}
};


#endif
