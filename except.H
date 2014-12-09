// =======================================
// Except.h
//
// Exception classes
//
//
// =======================================

#ifndef INCL_EXCEPT_H
#define INCL_EXCEPT_H

#include "libsrc/typeinfo.H"
#include "libsrc/safecopy.H"
#include "libsrc/StringObject.H"


// ****************************************
//	   class EPSILON_EXCEPTION
// ****************************************

// All exceptions are derived from this class,
// except those introduced in library code
// (such as STACK_UNDERFLOW)

class EPSILON_EXCEPTION 
{
  StringObject Filename, Reason;
  int LineNum;
public:
  EPSILON_EXCEPTION(const StringObject &Filename,int LineNum,
		    const StringObject &Reason);
  EPSILON_EXCEPTION(const EPSILON_EXCEPTION &);
  const StringObject &GetFilename() const { return Filename; }
  int GetLineNum() const { return LineNum; }
  const StringObject &GetReason() const { return Reason; }
};



// ****************************************
//	    class FILE_EXCEPTION
// ****************************************

class FILE_EXCEPTION : public EPSILON_EXCEPTION
{
public:
  FILE_EXCEPTION(const StringObject &SourceFile,int LineNum,
		 const StringObject &filename="");
};



// ****************************************
//	    class INTERNAL_ERROR
// ****************************************

// An internal error is an error that occurs
// as a result of a bug in my interpeter

class INTERNAL_ERROR : public EPSILON_EXCEPTION 
{
public:
  INTERNAL_ERROR(const StringObject &Filename,int LineNum,
		 const StringObject &Reason)
    : EPSILON_EXCEPTION(Filename,LineNum,Reason) {}
};



// ****************************************
//	       class READ_ERROR
// ****************************************

// A read error is any error associated with
// reading data from a stream

class READ_ERROR : public EPSILON_EXCEPTION 
{
public:
  READ_ERROR(const StringObject &Filename,int LineNum)
    : EPSILON_EXCEPTION(Filename,LineNum,
			"Error reading from input stream") {}
};



// ****************************************
//	      class LEXICAL_ERROR
// ****************************************

// A lexical error is a sequence of characters in
// the Epsilon source code from which the scanner
// cannot form a valid token when it tries

class LEXICAL_ERROR : public EPSILON_EXCEPTION 
{
  int EpsilonLineNum;
public:
  LEXICAL_ERROR(const StringObject &Filename,int CPlusPlusLineNum,
		int EpsilonLineNum)
    : EPSILON_EXCEPTION(Filename,CPlusPlusLineNum,"Lexical error"),
      EpsilonLineNum(EpsilonLineNum) {}
  int GetEpsilonLineNum() const { return EpsilonLineNum; }
};



// ****************************************
//	      class SYNTAX_ERROR
// ****************************************

// A syntax error is an error in the Epsilon
// source code which violates the grammar rules

class SYNTAX_ERROR : public EPSILON_EXCEPTION 
{
  int EpsilonLineNum;
  StringObject Details;
public:
  SYNTAX_ERROR(const StringObject &Filename,int CPlusPlusLineNum,
	       int EpsilonLineNum,const StringObject &Details="")
    : EPSILON_EXCEPTION(Filename,CPlusPlusLineNum,"Syntax error"),
      EpsilonLineNum(EpsilonLineNum), Details(Details) {}
  SYNTAX_ERROR(const SYNTAX_ERROR &se)
    : EpsilonLineNum(se.EpsilonLineNum), Details(se.Details),
      EPSILON_EXCEPTION(se.GetFilename(),se.GetLineNum(),"Syntax error") {}
  virtual ~SYNTAX_ERROR() {}
  int GetEpsilonLineNum() const { return EpsilonLineNum; }
  const StringObject &GetDetails() const { return Details; }
};



// ****************************************
//	     class SEMANTIC_ERROR
// ****************************************

// A semantic error is any error in the Epsilon source code
// which is not a lexical nor parse error

class SEMANTIC_ERROR : public EPSILON_EXCEPTION 
{
  int EpsilonLineNum;
  StringObject Details;
public:
  SEMANTIC_ERROR(const StringObject &Filename,int CPlusPlusLineNum,
		 int EpsilonLineNum,const StringObject &Details="")
    : EPSILON_EXCEPTION(Filename,CPlusPlusLineNum,"Semantic error"),
      EpsilonLineNum(EpsilonLineNum), Details(Details) {}
  SEMANTIC_ERROR(const SEMANTIC_ERROR &se)
    : EpsilonLineNum(se.EpsilonLineNum), Details(se.Details),
      EPSILON_EXCEPTION(se.GetFilename(),se.GetLineNum(),"Semantic error")
  {}
  virtual ~SEMANTIC_ERROR() {}
  int GetEpsilonLineNum() const { return EpsilonLineNum; }
  const StringObject &GetDetails() const { return Details; }
};



// ****************************************
//	     class RUN_TIME_ERROR
// ****************************************

// Any error which occurs at run time as a result
// of a bug in the user's program

class RUN_TIME_ERROR : public EPSILON_EXCEPTION 
{
  StringObject Details;
public:
  RUN_TIME_ERROR(const StringObject &Filename,int CPlusPlusLineNum,
		 const StringObject &Details="")
    : EPSILON_EXCEPTION(Filename,CPlusPlusLineNum,"Run-time error"),
      Details(Details) {}
  RUN_TIME_ERROR(const RUN_TIME_ERROR &re)
    : Details(re.Details),
      EPSILON_EXCEPTION(re.GetFilename(),re.GetLineNum(),"Run-time error")
  {}
  virtual ~RUN_TIME_ERROR() {}
  const StringObject &GetDetails() const { return Details; }
};




#endif
