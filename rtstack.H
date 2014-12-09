// ============================================
// rtstack.h
//
// RunTimeStack
//
//
// ============================================

#ifndef INCL_RTSTACK_H
#define INCL_RTSTACK_H

#include "garbage.H"
#include "object.H"
#include "symblrec.H"


class AstSend;



// ****************************************
//			class ActivationRecord
// ****************************************

// An ActivationRecord is a record on the RunTimeStack
// representing a single invocation of a method.  It
// contains local variables, parameters, and temporaries
// for the method.  The "next" pointer (inherited from
// link_node indirectly through Garbage) is referred to
// in compiler terminology as the "dynamic chain,"
// because it represents a dynamic relationship between
// functions; i.e., the call trace.

class ActivationRecord : public Garbage {
	Object **Entries;
	int NumEntries;
public:
	ActivationRecord(int NumEntries,Object *nil);
	~ActivationRecord();
	Object *&operator[](int i);
	Object *GetEntry(int i) const;
	Object *GetSelf() const;
	void SetEntry(int i,Object *to);
	int GetNumEntries() const;
	};



// ****************************************
//			  class AR_stack
// ****************************************
typedef ActivationRecord * ARptr;
typedef ListStack<ARptr> AR_stack; // linker complained about this template



// ****************************************
//			  class RunTimeStack
// ****************************************

// The RunTimeStack is a stack of activation
// records, which hold local variables, parameters,
// and temporaries for the current method
// invocation.

class RunTimeStack {
	AR_stack TheStack;
	ActivationRecord &GetARatDepth(int depth);
public:
	// For normal use:
	void PushAR(int AR_size,Object *self,Object *nil);
	void PushAR(ActivationRecord *);
	void PopAR(GarbageCollector &GC);
	ActivationRecord *PeekTop();
	ActivationRecord *GetGlobalAR();
	bool IsEmpty();

	// For garbage collection:
	void InitIterator();
	bool CanIterate();
	ActivationRecord *Iterate();
	};



#endif
