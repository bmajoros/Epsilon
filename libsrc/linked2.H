
// ==========================================
//
//		LINKED2.h
//		List-related objects.
//
//
// ==========================================




#ifndef INCL_LINKED2_H
#define INCL_LINKED2_H

//#include "/pan/pan5/local/lib/g++-include/std/typeinfo.h"
#include "typeinfo.H"
#include <stdio.h>
#include <stdlib.h>
#include "RTTI.H"




// **************************************************
// *                                                *
// *                  Classes                       *
// *                                                *
// **************************************************

// ***************** class link_node *******************

// A simple node for a linked_list, Stack, etc...
// Use the smart_node for more sophisticated data
// structures such as Set, Bag, etc...

class link_node 
{
public:
  RTTI_DECLARE_ROOT(link_node)
  link_node *next;
  link_node(void);		// constructor
  virtual ~link_node(void); // destructor
};







// *************** class BaseList ****************

// A base class for linked list classes

class BaseList {
public:
	BaseList(void) : first(0) {};
	link_node *first;
	};






// **************** class linked_list ******************

// A linked list.  The major underlying class in
// Set, Stack, Queue, etc...

class linked_list : public BaseList {
protected:
	link_node *seq;		// next node to be output by sequential()
	link_node *shadow;   // always two behind seq

public:
	linked_list(void);
	virtual ~linked_list(void);
	virtual void destroy(void);	// destroy all nodes
	virtual void purge() { destroy(); }
	virtual link_node *sequential(void);	// returns the next in the list
	void set_seq(link_node *SetSeq);
	virtual link_node *previous(link_node *ThisNode);
		// returns pointer to previous node or NULL
	virtual void reset_seq(void);			// reset sequential() to first node
	virtual void list_insert(link_node *new_node);
		// insert new node at beginning of list
	virtual void list_append(link_node *new_node);	// append new_node onto list end
	virtual void insert_here(link_node *new_node, link_node *here);
		// insert new node before this node

	virtual void del_node(link_node *victim);	// delete victem node from list
	virtual void del_seq(void);	// delete the node last returned by sequential()
	virtual void LetGoOf(link_node *Victim); // remove but don't deallocate

	virtual int NumElements(void);	// total number of nodes
	virtual int IsEmpty() const { return !first; }
	virtual link_node *GetFirst() { return first; }
    virtual link_node *GetLast();
	virtual link_node *RemoveFirst() { link_node *n=first; LetGoOf(n); return n; }
    virtual int CanIterate() { return (int)seq; }
	};






// ************ class ListIterator *************

// An iterator for any linked_list object.
// Class T is the class of the objects in
// the list.

template <class T>
class ListIterator {
protected:
	BaseList *TheList;
	T *seq;

public:
	ListIterator();
	ListIterator(BaseList *ForList);
	void SetList(BaseList *ForList);
	void reset_seq();
	T *sequential();
	void set_seq(T *To) { seq=To; }
	};






// ************ class ListOf *************

// A linked list of some specific type of node.
// The template parameter specifies that type of
// node.  This type should be derived from
// link_node.
// Note that a ListOf appends more efficiently
// than a linked_list because of its "last" pointer.
// Note also that the ListOf can have arbitrarily
// many simultaneous iterators, while linked_list
// provides the machinery for exactly one iteration
// at any given time.

template <class NodeType>
class ListOf : protected BaseList {
	NodeType *last;
	ListIterator<NodeType> *Iter; // array of iterators
	NodeType *Successor(NodeType *n)
		{ return static_cast<NodeType*>(n->next); }
public:
	ListOf(int NumIters=0);
	virtual ~ListOf();
	NodeType *GetFirst() const { return static_cast<NodeType *>(first); }
	NodeType *GetLast() const { return static_cast<NodeType *>(last); }
	NodeType *RemoveFirst();
	void Append(NodeType *NewNode);
	void Insert(NodeType *NewNode);
	void InsertBefore(NodeType *NewNode,NodeType *BeforeThis);
	void InsertAfter(NodeType *NewNode,NodeType *AfterThis);
	void Delete(NodeType *Victim);
	void LetGoOf(NodeType *ThisNode);
	void Purge();
	long NumElements();
	ListIterator<NodeType> &GetIterator(int i) { return Iter[i]; }
	};





// *********************************************
//			   ListIterator<> methods
// *********************************************

template <class T>
ListIterator<T>::ListIterator(BaseList *ForList) : TheList(ForList),
	seq(0)
	{
	// ctor
	}






template <class T>
ListIterator<T>::ListIterator() : TheList(0), seq(0)
	{
	// ctor

	// NOTE:  If you use this constructor, you
	//        MUST call SetList to tell the iterator
	//		  which list it is to operate on. It is
	//		  advised that you just use the other
	//        constructor instead!
	}







template <class T>
void ListIterator<T>::SetList(BaseList *ForList)
	{
	// Tells the iterator which list it is to
	// operate on.
	TheList=ForList;
	}







template <class T>
void ListIterator<T>::reset_seq()
	{
	if(TheList) seq=(T*)(TheList->first);
	}




template <class T>
T *ListIterator<T>::sequential()
	{
	T *retval=seq;
	if(seq) seq=(T *)(seq->next);

	return retval;
	}







// *********************************************
//				ListOf<> methods
// *********************************************


template <class NodeType>
void ListOf<NodeType>::LetGoOf(NodeType *OldNode)
	{
	// Removes the OldNode from the list, leaving the
	// rest of the members intact.  OldNode will NOT be
	// deleted.

	if(OldNode==GetFirst())
		{
		first=Successor(OldNode);
		if(!first) last=0;
		}
	else
		{
		// Need to find OldNode's predecessor, and redirect links around
		// OldNode

		NodeType *Previous=GetFirst();
		while(Previous && Successor(Previous)!=OldNode)
			Previous=Successor(Previous);
		if(!Previous) return;	// not in the list!

		Previous->next=OldNode->next;
		if(OldNode==GetLast()) last=Previous;
		}
	}





template <class NodeType>
void ListOf<NodeType>::Delete(NodeType *Victim)
	{
	LetGoOf(Victim);
	delete Victim;
	}





template <class NodeType>
void ListOf<NodeType>::Append(NodeType *NewNode) {

	// Appends the NewNode onto the end of the list

	if(!first) // appending the first member
		first=last=NewNode;
	else
		{
		last->next=NewNode;
		last=NewNode;
		}
	NewNode->next=0;
	}




template <class NodeType>
void ListOf<NodeType>::Insert(NodeType *NewNode) {

	// Inserts the NewNode at the beginning of the list

	if(!first) 	// inserting the first element
		{
		first=last=NewNode;
        NewNode->next=0;
		}
	else
		{
		NewNode->next=first;
		first=NewNode;
		}
	}






template <class NodeType>
void ListOf<NodeType>::Purge(void)
	{
	NodeType *ThisNode, *PrevNode;

	// Destroys the contents of the list, but does NOT
	// destroy the actual ListOf object

	ThisNode=GetFirst();
	while(ThisNode)
		{
		PrevNode=ThisNode;
		ThisNode=Successor(ThisNode);
		delete PrevNode;
		}

	first=last=0;
	}




template <class NodeType>
ListOf<NodeType>::ListOf(int NumIters) : BaseList(), last(0),
	Iter(0)
	{
	int f;

	// ctor

	// NumIters is the number of ListIterators
	// to be included in the object

	if(NumIters)
		{
		Iter=new ListIterator<NodeType>[NumIters];
		for(f=0 ; f<NumIters ; f++) Iter[f].SetList(this);
		}
	}




template <class NodeType>
ListOf<NodeType>::~ListOf(void)
	{
	// dtor
	Purge();
	delete [] Iter;
	}




template <class NodeType>
void ListOf<NodeType>::InsertBefore(NodeType *NewNode,NodeType *BeforeThis)
	{
	NodeType *ThisNode;

	// Insert NewNode to the left of BeforeThis in the list

	if(BeforeThis==GetFirst() || !first)
		{
		NewNode->next=first;
		first=NewNode;
		}
	else
		{
		ThisNode=GetFirst();
		while(Successor(ThisNode) && Successor(ThisNode)!=BeforeThis)
			ThisNode=Successor(ThisNode);

		NewNode->next=ThisNode->next;
		ThisNode->next=NewNode;
		}
	}





template <class NodeType>
void ListOf<NodeType>::InsertAfter(NodeType *NewNode,NodeType *AfterThis)
	{
	// Inserts NewNode after AfterThis in the list.

	if(AfterThis==GetLast()) last=NewNode;

	NewNode->next=AfterThis->next;
	AfterThis->next=NewNode;
	}



template <class NodeType>
NodeType *ListOf<NodeType>::RemoveFirst()
	{
	NodeType *p=GetFirst();
	if(p) LetGoOf(p);
    return p;
	}




template <class NodeType>
long ListOf<NodeType>::NumElements()
	{
	NodeType *ThisNode=GetFirst();
	int retval=0;

	// Returns the number of elements in the list.
	// This number is determined by explicitly
	// counting list elements at invocation.

	while(ThisNode)
		{
		retval++;
		ThisNode=Successor(ThisNode);
		}

	return retval;
	}







#endif







