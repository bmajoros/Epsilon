// ================================
// hash.C
//
// Hash table
//
// Ported to Solaris on 3/27/96
// ================================

#include "typeinfo.H"
#include "hash.H"


int hashpjw(const char *s)
	{
	// This is essentially hashpjw(), from the Dragon Book.
	// You must compute modulo the table size before using
        // the hash value that it computes.

	const char *p;
	unsigned h=0, g;
	for(p=s ; *p!='\0' ; p++)
		{
		h = (h<<4) + *p;
		g = h & 0xf000;//0000;
		if(g)
			{
			h=h^(g>>24);
			h=h^g;
			}
		}
	return h;
	}



