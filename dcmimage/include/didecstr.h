/*********************************************************************
** 
**  DicomDecimalString (Header)
**
**  author   : Joerg Riesmeier
**  created  : 15.12.96
**  modified : 25.04.97
**
*********************************************************************/


#ifndef __DIDECSTR_H
#define __DIDECSTR_H

#include "osconfig.h"
#include "dctypes.h"
#include "dcvrds.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

class DiDecimalString : public DcmDecimalString
{
 public: 
	inline void getValue(double &returnVal, unsigned long pos = 0, const char sep = '\\')
	{
		char *p;
		char *str = NULL;
		DcmDecimalString::getString(str);
		while (((p = strchr(str, sep)) != NULL) && (pos > 0))
		{
			str = p + 1;
			pos--;
		}
		if (pos == 0)
		{
			if (p == NULL)
				returnVal = atof(str);
			else
			{
				char *nstr = new char[p - str + 1];
				if (nstr != NULL)
				{
					*nstr = '\0';
					strncat(nstr, str, p - str);
					returnVal = atof(nstr);
					delete nstr;
				}
			}
		} 
	}
};


#endif
