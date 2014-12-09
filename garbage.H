// =======================================
// Garbage.h
//
// Definitions for garbage collection
//
//
// =======================================

#ifndef INCL_GARBAGE_H
#define INCL_GARBAGE_H

#include "libsrc/linked2.H"
#include "libsrc/RTTI.H"


// Here we set the threshold for the GC; when the number
// of objects grows to this many more than the InUse
// value (estimate of # objects really in use), we initiate
// collection.
// ### It is currently set to 1000 objects; this should be made
// configurable in the future...
#define GC_THRESHOLD	1000



class RunTimeStack;
class ActivationRecord;
class Object;



// ****************************************
//			    class Garbage
// ****************************************

// Anything which can eventually be garbage-collected
// should be derived from Garbage.  Note, however,
// that just because an object is derived from Garbage
// doesn't mean that it can be garbage collected _yet_.

class Garbage : public link_node 
{
  bool Marked; // for "mark-and-sweep" garbage collection
public:
  RTTI_DECLARE_SUBCLASS(Garbage,link_node)
  Garbage() : Marked(false) {}
  virtual ~Garbage() {} // we will delete derived classes through base ptr
  void Mark() { Marked=true; }
  void Unmark() { Marked=false; }
  bool IsMarked() const { return Marked; }
};




// ****************************************
//			class GarbageCollector
// ****************************************

// The GarbageCollector maintains a list of all
// objects that can (eventually) be garbage-collected,
// and performs garbage collection on that list when
// instructed to.  The collection method used is the
// "mark-and-sweep" technique, which marks objects
// accessible from the run-time stack, and then
// sweeps through the ObjectList, deleting objects
// which are not marked.

class GarbageCollector 
{
  linked_list ObjectList; // list of Garbage objects
  unsigned NumObjects; // number of elements in ObjectList
  unsigned InUse; // estimate of # objects really in use (accessible)
  unsigned Threshold; // =GC_THRESHOLD
  void Mark(RunTimeStack &s);
  void Sweep(RunTimeStack &s);
  void MarkAR(ActivationRecord &ar);
  void MarkAccessibleFrom(Object *obj);
public:
  GarbageCollector();
  void Manage(Garbage *g);
  void PerformGC(RunTimeStack &s);
  void CollectIfLow(RunTimeStack &s);
};



#endif
