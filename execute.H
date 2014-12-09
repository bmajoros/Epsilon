// =======================================
// execute.h
//
// Execution environment
//
//
// =======================================

#ifndef INCL_EXECUTE_H
#define INCL_EXECUTE_H

#include "rtstack.H"



class LexicalAddress;


// ****************************************
//	 class RunTimeEnvironment
// ****************************************

// The RunTimeEnvironment encapsulates everything
// needed at run-time.

class RunTimeEnvironment 
{
  RunTimeStack TheStack;
  GarbageCollector GC;
  Object *&ObjectRef(const LexicalAddress &,StorageClass);
  bool Returning; // are we executing a return statement?
  ActivationRecord *ReturnFromAR; // AR to be popped by return statement
  Object *ReturnValue;
  ActivationRecord &GetARatDepth(int depth);
public:
  RunTimeEnvironment();
  void GetReadyToRun(int globalARSize);
  RunTimeStack &GetStack();
  void PopAR();
  Object *GetObject(const LexicalAddress &,StorageClass);
  Object *GetParameter(int LexicalPosition); // first parm is at pos 1
  Object *GetSelf();
  void StoreObject(const LexicalAddress &,StorageClass,Object *);
  void Return(ActivationRecord *From,Object *RetVal);
  bool AreWeReturning();
  ActivationRecord *GetReturnAR();
  void DoneReturning();
  Object *GetReturnValue() const;
  void RegisterGarbage(Object *);
  void RunGarbageCollector();
};



#endif
