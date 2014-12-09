// ======================================
// ScopeStk.h
//
// Scope stacks (stacks of symbol tables)
//
// ======================================


#ifndef INCL_SCOPESTK_H
#define INCL_SCOPESTK_H

#include "/home/bmajoros/SgmlTalk/libsrc/symtab.H"
#include "/home/bmajoros/SgmlTalk/libsrc/stack.H"



// *******************************************
//			   class ScopeStack
// *******************************************
template <class T,int HashTableSize>
class ScopeStack {
	typedef HashSymbolTable<T,HashTableSize> SymbolTable;
	ListStack<SymbolTable*> TheStack;
public:
	T *Find(const char *Key,int &LexicalDepth);
	void Insert(T *t,const char *Key); // throws STACK_UNDERFLOW if stack is empty
	void Delete(const char *Key);
	void EnterScope();
	int GetCurrentScopeSize() /*const*/; // throws STACK_UNDERFLOW if stack is empty
	void LeaveScope(); // throws STACK_UNDERFLOW if stack is empty
	};


template <class T,int HashTableSize>
T *ScopeStack<T,HashTableSize>::Find(const char *Key,int &LexicalDepth)
	{
	// Searches the scope stack from top to bottom
	// for the first instance of a symbol matchin
	// Key.  If found, LexicalDepth is set to
	// the lexical depth of the symbol, where symbols
	// in the top symbol table have lexical depth of
	// 0.  If not found, LexicalDepth is set to the
	// number of symbol tables in the stack.

	LexicalDepth=0;
	TheStack.InitIterator();
	while(TheStack.CanIterate())
		{
		SymbolTable *symtab=TheStack.Iterate();
		T *tp=symtab->Find(Key);
		if(tp) return tp;
		++LexicalDepth;
		}
	return 0;
	}



template <class T,int HashTableSize>
void ScopeStack<T,HashTableSize>::Delete(const char *Key)
	{
	// Deletes the innermost declaration of Key.

	TheStack.InitIterator();
	while(TheStack.CanIterate())
		{
		SymbolTable *symtab=TheStack.Iterate();
		if(symtab->Find(Key))
			{
            symtab->Delete(Key);
			break;
			}
		}

	}



template <class T,int HashTableSize>
void ScopeStack<T,HashTableSize>::Insert(T *t,const char *Key)
	{
	TheStack.PeekTop()->Insert(t,Key);
	}



template <class T,int HashTableSize>
int ScopeStack<T,HashTableSize>::GetCurrentScopeSize() //const
	{
	return TheStack.PeekTop()->GetNumEntries();
	}




template <class T,int HashTableSize>
void ScopeStack<T,HashTableSize>::EnterScope()
	{
	TheStack.Push(new SymbolTable);
	}



template <class T,int HashTableSize>
void ScopeStack<T,HashTableSize>::LeaveScope()
	{
	delete TheStack.Pop();
	}
    


#endif
