
#include "libsrc/typeinfo.H"
#include <strstream.h>
#include <iostream.h>


void main(int argc,char *argv[])
{
  ostrstream os;
cout << "1" << endl;
  os << "this seems to work" << ends;
cout << "2" << endl;
  strstream str;
cout << "3" << endl;
  str << "but this is crashing" << ends;
cout << "4" << endl;  
}
