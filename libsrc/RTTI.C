// ==================================================
// RTTI.C
//
// Programmer     Date       Change
// ----------     ----       ------
// B. Majoros     12/18/96   Creation
// ==================================================

#include "RTTI.H"
#include <strstream.h>


// **************************************************
//              ClassDescriptor methods
// **************************************************

ClassDescriptor::ClassDescriptor(ClassDescriptor *iSuperClass)
  : superClass(iSuperClass)
{
  // ctor
}


#include <iostream.h>
bool ClassDescriptor::DescendedFrom(ClassDescriptor *c)
{
  return bool(c==this || superClass && superClass->DescendedFrom(c));
}



// **************************************************
//                  BadCast methods
// **************************************************

BadCast::BadCast(const char *filename,int lineNum)
{
  // ctor

  ostrstream os;
  os << "Bad cast on line " << lineNum << " in file " << filename
    << ends;
  char *msg=os.str();
  SetMessage(msg);
  delete [] msg;
}



BadCast::BadCast(const BadCast &b)
{
  SetMessage(b.GetMessage().AsCharArray());
}



