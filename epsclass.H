
// epsclass.H





OBSOLETE





#ifndef INCL_EPSCLASS_H
#define INCL_EPSCLASS_H

#include "class.H"
#include "parser.H"
#include "libsrc/StringObject.H"
#include <iostream.h>

class EpsilonClass
{
  Class *myClass;
  bool hasClassMembers;
  bool isRestricted;
  Parser &parser;
  StringObject name;

  void declare();
  void undeclare();
public:
  EpsilonClass(Class *,const StringObject &name,bool hasClassMembers,
	       bool isRestricted,Parser &);
  void addInstanceMethod(const StringObject &name,CppMethod body,
			 int arity=ARITY_DETECT);
  void addInstanceMethod(const StringObject &name,istream &body);
  void addClassMethod(const StringObject &name,CppMethod body,
		      int arity=ARITY_DETECT);
  void addClassMethod(const StringObject &name,istream &body);  
  void addAttribute(const StringObject &name);
};

#endif
