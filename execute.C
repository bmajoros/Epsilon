// =======================================
// execute.cpp
//
// Execution environment
//
//
// =======================================

#include "libsrc/typeinfo.H"
#include "execute.H"
#include "class.H"
#include "object.H"
#include "ast.H"


// ****************************************
//	 RunTimeEnvironment methods
// ****************************************
void RunTimeEnvironment::RegisterGarbage(Object *obj)
	{
	GC.Manage(obj);
	}



void RunTimeEnvironment::RunGarbageCollector()
	{
	GC.CollectIfLow(TheStack);
	}



void RunTimeEnvironment::GetReadyToRun(int globalARSize)
{
  // Push activation record for main
  TheStack.PushAR(globalARSize,0,nil);
}



void RunTimeEnvironment::DoneReturning()
	{
	Returning=false;
	}




void RunTimeEnvironment::Return(ActivationRecord *From,Object *RetVal)
	{
	ReturnFromAR=From;
	Returning=true;
	ReturnValue=RetVal;
	}



Object *RunTimeEnvironment::GetReturnValue() const
	{
	return ReturnValue;
	}
    


bool RunTimeEnvironment::AreWeReturning()
	{
	return Returning;
	}



ActivationRecord *RunTimeEnvironment::GetReturnAR()
	{
    return ReturnFromAR;
	}




RunTimeEnvironment::RunTimeEnvironment() : Returning(false)
	{
	// ctor
	}



ActivationRecord &RunTimeEnvironment::GetARatDepth(int depth)
	{
	// Start at the top of the stack...
	ActivationRecord *ar=TheStack.PeekTop();

	// ...and follow the static chains down to the desired AR
	for(int i=0 ; i<depth ; i++)
		{
		Block_Object *bo=
		  DYNAMIC_CAST_PTR(Block_Object,ar->GetEntry(0));
		ar=bo->GetStaticChain();
		}

	return *ar;
	}




Object *&RunTimeEnvironment::ObjectRef(const LexicalAddress &la,
	StorageClass s)
	{
	switch(s)
		{
		case OBJ_LOCAL:
			{
			ActivationRecord &ar=GetARatDepth(la.GetDepth());
			return ar[la.GetPosition()];
			}
		case OBJ_ATTRIBUTE:
			{
			ActivationRecord &ar=GetARatDepth(la.GetDepth());
			Object &self=*ar[0];
			return self[la.GetPosition()];
			}
		case OBJ_GLOBAL:
		default:
			{
			ActivationRecord &global_ar=*TheStack.GetGlobalAR();
			return global_ar[la.GetPosition()];
			}
		}
	}



Object *RunTimeEnvironment::GetObject(const LexicalAddress &la,
	StorageClass s)
	{
	return ObjectRef(la,s);
	}



Object *RunTimeEnvironment::GetParameter(int LexicalPosition)
	{
    return GetObject(LexicalAddress(0,LexicalPosition),OBJ_LOCAL);
	}



Object *RunTimeEnvironment::GetSelf()
	{
    return TheStack.PeekTop()->GetSelf();
	}



void RunTimeEnvironment::StoreObject(const LexicalAddress &la,StorageClass s,
	Object *obj)
	{
	ObjectRef(la,s)=obj;
	}



RunTimeStack &RunTimeEnvironment::GetStack()
	{
	return TheStack;
	}



void RunTimeEnvironment::PopAR()
	{
	TheStack.PopAR(GC);
	}



