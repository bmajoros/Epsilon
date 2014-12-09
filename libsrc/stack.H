// ================================
// Stack.h
//
// Stacks
//
// ================================

#ifndef INCL_STACK_H
#define INCL_STACK_H

#include "/home/bmajoros/SgmlTalk/libsrc/linked2.H"



// *******************************************
//			 class STACK_UNDERFLOW
// *******************************************
class STACK_UNDERFLOW {
	};




// *******************************************
//			     class ListStack
// *******************************************

// A ListStack can stack anything; even primitive
// data types; the elements need not be derived from
// a special type.  It allows clients to access its
// list representation directly, so clients can
// search through the entire stack without having
// to pop anything in the process.  This is useful
// for a stack of symbol tables; if a symbol is not
// found in the top symbol table, you can search
// the tables lower in the stack because they represent
// enclosing scopes.  The iteration mechanism
// provided by ListStack iterates down the stack,
// from the top to the bottom, for this very purpose.

template <class T>
class ListStack {
	linked_list TheList;
	class StackNode : public link_node {
		T MyElement;
		StackNode *next;
	public:
		StackNode(T t) : MyElement(t), next(0) {}
		operator T() { return MyElement; }
		};
public:
	void Push(T t);
	T Pop(); // throws STACK_UNDERFLOW if stack is empty
	bool IsEmpty();
	T PeekTop(); // throws STACK_UNDERFLOW if stack is empty

	linked_list &AsList();
	void InitIterator();
	T Iterate(); // throws STACK_UNDERFLOW if you iterate too far
	bool CanIterate();
	};



// *******************************************
//			    ListStack methods
// *******************************************

template <class T>
void ListStack<T>::InitIterator()
	{
	TheList.reset_seq();
	}



template <class T>
T ListStack<T>::Iterate()
	{
	link_node *ln=TheList.sequential();
	if(!ln) throw STACK_UNDERFLOW();

	StackNode *sn=(StackNode*)(ln);
	return *sn;
	}



template <class T>
bool ListStack<T>::CanIterate()
	{
    return TheList.CanIterate();
	}



template <class T>
T ListStack<T>::PeekTop()
	{
	if(TheList.IsEmpty()) throw STACK_UNDERFLOW();
	return *(StackNode*)(TheList.GetFirst());
	}




template <class T>
void ListStack<T>::Push(T t)
	{
	TheList.list_insert(new StackNode(t));
	}



template <class T>
bool ListStack<T>::IsEmpty()
	{
	return TheList.IsEmpty();
	}



template <class T>
linked_list &ListStack<T>::AsList()
	{
	return TheList;
	}



template <class T>
T ListStack<T>::Pop()
	{
	if(TheList.IsEmpty())
		throw STACK_UNDERFLOW();
	StackNode *n=(StackNode*)(TheList.RemoveFirst());
	T t=*n;
	delete n;
	return t;
	}



#endif
