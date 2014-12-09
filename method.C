// =======================================
// method.cpp
//
// Method body representation
//
//
// =======================================

#include "libsrc/typeinfo.H"
#include "method.H"
#include "ast.H"
#include "object.H"
#include "symblrec.H"
#include "execute.H"



// ****************************************
//			  MethodBody methods
// ****************************************

MethodBody::MethodBody(int AR_size) : AR_size(AR_size)
	{
	// ctor
	}



void MethodBody::SetARsize(int s)
	{
	AR_size=s;
	}



int MethodBody::GetARsize() const
	{
	return AR_size;
	}



// ****************************************
//			    CppBody methods
// ****************************************

CppBody::CppBody(CppMethod f,int AR_size) : f(f), MethodBody(AR_size)
	{
	// ctor
	}



Object *CppBody::Call(RunTimeEnvironment &env)
	{
	Object *RetVal=(*f)(env);
	if(!env.AreWeReturning())
		{
		// We don't do this if env.AreWeReturning() returns true,
		// because that would mean that we executed an Epsilon
		// return statement, so this return value won't get
		// used anyway; besides, we would cause the Epsilon
		// return statement's value to be lost.
		env.Return(env.GetStack().PeekTop(),RetVal);
		}

	return RetVal; // This is actually ignored
	}



// ****************************************
//			  EpsilonBody methods
// ****************************************

EpsilonBody::EpsilonBody(SyntaxForest *f,int AR_size) : MyForest(f),
	MethodBody(AR_size)
	{
	// ctor
	}



void EpsilonBody::SetBody(SyntaxForest *sf)
	{
	MyForest=sf;
	}



Object *EpsilonBody::Call(RunTimeEnvironment &env)
	{
	// Precondition: My activation record has already been pushed, and
	//				 my parameters (including the implicit parameter
    //				 "self") have been stored in that AR.

	// Tell the SyntaxForest to execute
	MyForest->Execute(env);

	// Return the return-value (or nil if none)
	return MyForest->GetValue(env);
	}




