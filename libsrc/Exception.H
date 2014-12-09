// ================================================
// Exception.H
//
// Bill Majoros -- 3/28/96
// ================================================

#ifndef INCL_MY_EXCEPTION_H
#define INCL_MY_EXCEPTION_H

#include "StringObject.H"


// This simulates a try-catch-finally block
#define FINALLY(Statements) \
  catch(...) \
    { \
      Statements \
      throw; \
    } \
  Statements



// ********************************************************
//                      class RootException
// ********************************************************

// RootException is the class at the root of my
// exception class hierarchy.

class RootException {
  StringObject Message;
protected:
  void SetMessage(const char *); // for my subclasses
public:
  virtual ~RootException() {}
  RootException(const StringObject &);
  RootException(const char * ="");
  RootException(const RootException &);
  const StringObject &GetMessage() const;
};



// *************************************************************
//                  class ArrayIndexException
// *************************************************************

// An ArrayIndexException is usu. thrown in an operator[]
// when an invalid index is used.

class ArrayIndexException : public RootException {
public:
  ArrayIndexException(unsigned Index,const char *MoreMessage=0);
};




// *************************************************************
//                     class FileErrorException
// *************************************************************

// All exceptions associated with file errors (i.e., difficulty
// opening/closing or reading/writing from/to a file) should
// be instances or subclasses of FileErrorException.

class FileErrorException : public RootException {
public:
  FileErrorException(const char *Filename,const char *Reason=0);
};




// *************************************************************
//                   class NotFoundException
// *************************************************************
class NotFoundException : public RootException {
public:
  NotFoundException(const StringObject &ThingNotFound);
};



#endif
