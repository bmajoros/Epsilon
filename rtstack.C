// ============================================
// rtstack.cpp
//
// RunTimeStack
//
//
// ============================================

#include "libsrc/typeinfo.H"
#include "rtstack.H"
#include "ast.H"
#include "except.H"


// This next line is a workaround for a bug in the C++
// compiler which sometimes causes linker errors due
// to not properly instantiating templates
static ListStack<ActivationRecord*> force_template_instantiation;



// ****************************************
//			ActivationRecord methods
// ****************************************

Object *ActivationRecord::GetSelf() const
	{
	return Entries[0];
	}




ActivationRecord::ActivationRecord(int NumEntries,Object *nil)
	: Entries(new Object*[NumEntries]),	NumEntries(NumEntries)
	{
	// ctor

	// Initialize all entries to point to the nil object
	for(int i=0 ; i<NumEntries ; i++)
		Entries[i]=nil;
	}



ActivationRecord::~ActivationRecord()
	{
	// dtor

	delete [] Entries;
	}



Object *&ActivationRecord::operator[](int i)
	{

	// ### This bounds checking probably slows it down alot...
	if(i<0 || i>=NumEntries)
		throw INTERNAL_ERROR(__FILE__,__LINE__,
			"Invalid index in ActivationRecord::operator[]");

    return Entries[i];
	}



Object *ActivationRecord::GetEntry(int i) const
	{
	return Entries[i];
	}



void ActivationRecord::SetEntry(int i,Object *to)
	{
	Entries[i]=to;
	}



int ActivationRecord::GetNumEntries() const
	{
	return NumEntries;
	}



// ****************************************
//			 RunTimeStack methods
// ****************************************

ActivationRecord *RunTimeStack::GetGlobalAR()
	{
	// Returns a pointer to the AR on the bottom of the stack,
	// which holds global objects

	ActivationRecord *Bottom=0;
	TheStack.InitIterator();
	while(TheStack.CanIterate())
		Bottom=TheStack.Iterate();
	return Bottom;
	}



void RunTimeStack::PushAR(int AR_size,Object *self,Object *nil)
	{
	// Create a new activation record
	ActivationRecord *ar=new ActivationRecord(AR_size,nil);

	// Set the 0th slot to "self"
	ar->SetEntry(0,self);

	// Push onto stack
	TheStack.Push(ar);
	}



void RunTimeStack::PushAR(ActivationRecord *ar)
	{
	TheStack.Push(ar);
	}



void RunTimeStack::PopAR(GarbageCollector &GC)
	{
	// Pop the top AR off the stack
	ActivationRecord *ar=TheStack.Pop();

	// Hand it over to the GC so it can be deleted when
	// it becomes inaccessible (at the moment, there may
	// exist some block which has a "static chain" pointing
	// to it)
	GC.Manage(ar);
	}



ActivationRecord *RunTimeStack::PeekTop()
	{
	return TheStack.PeekTop();
	}



bool RunTimeStack::IsEmpty()
	{
	return TheStack.IsEmpty();
	}



void RunTimeStack::InitIterator()
	{
	TheStack.InitIterator();
	}



bool RunTimeStack::CanIterate()
	{
	return TheStack.CanIterate();
	}



ActivationRecord *RunTimeStack::Iterate()
	{
	return TheStack.Iterate();
	}


