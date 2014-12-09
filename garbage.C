// =======================================
// Garbage.cpp
//
// Definitions for garbage collection
//
//
// =======================================

#include "libsrc/typeinfo.H"
#include "garbage.H"
#include "rtstack.H"
#include "cmdline.H"

RTTI_DEFINE_SUBCLASS(Garbage,link_node)

// ****************************************
//			GarbageCollector methods
// ****************************************

GarbageCollector::GarbageCollector() : Threshold(GC_THRESHOLD),
	InUse(0), NumObjects(0)
	{
	// ctor
	}



void GarbageCollector::Manage(Garbage *g)
	{
	// g is being handed over to the GC so the GC can
	// monitor its accessibility and delete it when it
	// becomes inaccessible
	ObjectList.list_insert(g);
	++NumObjects;
	}



void GarbageCollector::CollectIfLow(RunTimeStack &s)
	{
	if(NumObjects > InUse+Threshold)
		{
		int OldNumObjects;
		/*		if(CmdLine->AreWeDebugging())
			{
			cout << "\n------------------------------------------------------"
				<< endl;;
			cout << "Performing garbage collection...." << endl;
			OldNumObjects=NumObjects;
			}*/

		PerformGC(s);

		/*		if(CmdLine->AreWeDebugging())
			{
			cout << "Garbage collection is complete.  " << endl;
			cout << OldNumObjects << " objects reduced to " << NumObjects
				<< " objects." << endl;
			cout << "------------------------------------------------------"
				<< endl;

			cout << "Press ENTER...";
			char junk[5];
			cin.getline(junk,4);
			cout << endl;
			}*/
		}
	}




void GarbageCollector::PerformGC(RunTimeStack &s)
	{
	// Perform mark-and-sweep garbage collection.  We
	// follow every pointer in every activation record
	// in the RunTimeStack, marking all Objects found.
	// Before marking an Object, we make sure it is not
	// already marked, so we don't enter an infinite loop.

	// Then we sweep once through the ObjectList.  For
	// each Object in the ObjectList, if it is not marked,
	// we remove it from the list and delete it; otherwise,
	// we unmark it so it is ready for the next round of
	// mark-and-sweep.

	Mark(s);
	Sweep(s);
	}



void GarbageCollector::Mark(RunTimeStack &s)
	{
	// Mark every object accessible through the run-time stack

	s.InitIterator();
	while(s.CanIterate())
		{
		ActivationRecord &ar=*s.Iterate();
		MarkAR(ar);
		}
	}



void GarbageCollector::MarkAR(ActivationRecord &ar)
	{
	// Mark all objects accessible through this AR

	// An AR is garbage, too, so mark it just like other garbage
	if(ar.IsMarked()) return; // ### I don't think this line is necessary
	ar.Mark(); 				  //     ...but this one is!

	int NumEntries=ar.GetNumEntries();
	for(int i=0 ; i<NumEntries ; i++)
		{
		Object *obj=ar.GetEntry(i);

		// obj might be NULL, because main's AR has
        // a NULL self value
		if(obj) MarkAccessibleFrom(obj);
		}
	}



void GarbageCollector::MarkAccessibleFrom(Object *obj)
	{
	// If this object is already marked, then just return,
	// to avoid entering an infinite loop
	if(obj->IsMarked()) return;

	// Mark this object right away, so that as we recurse onto
	// other objects, we will know if we come back to this object,
	// and won't recurse again (entering an infinite loop)
	obj->Mark();

	// Iterate through this object's attributes, recursing onto each
	int NumAttributes=obj->TotalAttributes();
	for(int i=0 ; i<NumAttributes ; i++)
		MarkAccessibleFrom(obj->GetAttribute(i));

	// Finally, if this object is a Block_Object, then we also have to
	// follow its static chain and recursively mark the AR thus accessed
	Block_Object *bo=DYNAMIC_CAST_PTR(Block_Object,obj);
	if(bo)
		MarkAR(*bo->GetStaticChain());
	}



void GarbageCollector::Sweep(RunTimeStack &s)
	{
	// Sweep away all objects not marked, and unmark those that
	// are marked

	// First, sweep through the ObjectList
	ObjectList.reset_seq();
	Garbage *g;
	while(g=DYNAMIC_CAST_PTR(Garbage,ObjectList.sequential()))
		{
		if(!g->IsMarked())
			{
			ObjectList.del_seq();
			--NumObjects;
			}
		else
			g->Unmark();
		}

	// Now we have to make another pass through the stack to unmark
	// all ARs still on the stack; otherwise, they won't be visited
	// during the next marking phase, and their objects will get
	// deleted out from under them
	s.InitIterator();
	while(s.CanIterate())
		{
		ActivationRecord &ar=*s.Iterate();
		ar.Unmark();
		}

	// Finally, store away NumObjects as InUse, because we were
	// unable to collect any of the remaining objects; therefore,
	// they must still be accessible and are probably "in use."
	InUse=NumObjects;
	}





