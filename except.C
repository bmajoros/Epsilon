// =======================================
// Except.cpp
//
// Exception classes
//
//
// =======================================

#include "libsrc/typeinfo.H"
#include "except.H"



// ****************************************
//	    FILE_EXCEPTION methods
// ****************************************

FILE_EXCEPTION::FILE_EXCEPTION(const StringObject &sourceFile,
			       int LineNum,
			       const StringObject &filename)
  : EPSILON_EXCEPTION(sourceFile,LineNum,filename)
{
  // ctor
}



// ****************************************
//		   EPSILON_EXCEPTION methods
// ****************************************
EPSILON_EXCEPTION::EPSILON_EXCEPTION(const StringObject &Filename,
				     int LineNum,
				     const StringObject &Reason)
  : Filename(Filename), LineNum(LineNum),
    Reason(Reason)
{
  // ctor
}



EPSILON_EXCEPTION::EPSILON_EXCEPTION(const EPSILON_EXCEPTION &e)
  : Filename(e.Filename), LineNum(e.LineNum),
    Reason(e.Reason)
{
  // copy ctor
}





