
// safecopy.h

#ifndef INCL_SAFECOPY_H
#define INCL_SAFECOPY_H


#include <string.h>




// safe_strcpy() allocates memory for the destination before
// copying the source into it, but it does NOT deallocate the
// memory currently allocated to the destination pointer first,
// if any.
inline void safe_strcpy(char *&To,const char *From)
	{
	To=new char[strlen(From)+1];
	strcpy(To,From);
	}



inline char *Duplicate(const char *p)
	{
	char *q;
	safe_strcpy(q,p);
	return q;
	}



#endif
