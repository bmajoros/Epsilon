
// interpreter.H

#ifndef INCL_INTERPRETER_H
#define INCL_INTERPRETER_H

#include "parser.H"
#include "execute.H"

class EpsilonInterpreter
{
  Parser parser;
  RunTimeEnvironment env;
  int lpCin, lpCout, lpTrue, lpFalse, lpNil, lpEndl;

  void declareBuiltInObjs();
  void storeBuiltInObjs();
  void declareBuiltInTypes();
  void declare(Class *,const StringObject &name);
  void undeclare(Class *,const StringObject &name);
public:
  EpsilonInterpreter();
  void parseCode(istream &);
  void executeMain();
  bool doesMainExist();

  Class *createClass(const StringObject &name);
  void registerClass(Class *);
  void addAttribute(Class *,const StringObject &name);
  void addInstanceMethod(Class *,const StringObject &name,
			 CppMethod body);
  void addInstanceMethod(Class *,const StringObject &name,
			 const StringObject &body);

  int declareGlobalObject(const StringObject &);
  void allocateGlobalSpace();
  void storeGlobalObject(Object *,int lexicalPosition);
};

#endif
