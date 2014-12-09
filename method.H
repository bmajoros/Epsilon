// =======================================
// method.h
//
// Method body representation
//
//
// =======================================

#ifndef INCL_METHOD_H
#define INCL_METHOD_H

#include "libsrc/linked2.H"

class Object;
class SyntaxForest;
class RunTimeEnvironment;


/*			METHOD BODY INHERITANCE HIERARCHY


					  MethodBody
						  |
						  |
			   -----------------------
			   |                      |
			   |                      |
			CppBody				EpsilonBody


*/



// ****************************************
//			  class MethodBody
// ****************************************

// Abstract base class for objects that represent
// the body of a method

class MethodBody {
	int AR_size; // number of Object pointers in activation record
public:
	MethodBody(int AR_size);
	void SetARsize(int s);
	virtual Object *Call(RunTimeEnvironment &)=0;
	int GetARsize() const;
	};



// ****************************************
//			  typedef CppFunction
// ****************************************
typedef Object *(*CppMethod)(RunTimeEnvironment &);



// ****************************************
//			    class CppBody
// ****************************************

// A CppBody is a representation for method
// bodies that are defined by C++ functions

class CppBody : public MethodBody {
	CppMethod f;
public:
	CppBody(CppMethod f,int AR_size=0);
	virtual Object *Call(RunTimeEnvironment &env);
	};



// ****************************************
//			  class EpsilonBody
// ****************************************

// An EpsilonBody is a representation for
// method bodies that are defined by Epsilon
// syntax trees

class EpsilonBody : public MethodBody {
	SyntaxForest *MyForest;
public:
	EpsilonBody(SyntaxForest *f=0,int AR_size=0);
	void SetBody(SyntaxForest *sf);
	virtual Object *Call(RunTimeEnvironment &);
	};


#endif
