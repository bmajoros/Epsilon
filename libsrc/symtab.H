// ================================
// SymTab.h
//
// Symbol tables
//
// ================================

#ifndef INCL_SYMTAB_H
#define INCL_SYMTAB_H

#include "/home/bmajoros/SgmlTalk/libsrc/hash.H"
#include "/home/bmajoros/SgmlTalk/libsrc/linked2.H"


// *******************************************
//			class AbstractSymbolNode
// *******************************************
class AbstractSymbolNode : public link_node 
{
public:
  RTTI_DECLARE_SUBCLASS(AbstractSymbolNode,link_node)
  virtual ~AbstractSymbolNode() {}
  virtual bool Equals(const char *Key)=0;
};



// *******************************************
//			  class HashSymbolTable
// *******************************************

// Type T must be derived from AbstractSymbolNode,
// and TableSize should be prime.

template <class T,int TableSize>
class HashSymbolTable 
{
  SimpleHashTable<T,TableSize> hashTable;
  int NumEntries;
public:
  HashSymbolTable() : NumEntries(0) {}
  T *Find(const char *Key);
  void Insert(T *t,const char *Key);
  void Delete(const char *Key);
  int GetNumEntries() const { return NumEntries; }
};



// *******************************************
//			 HashSymbolTable methods
// *******************************************

template <class T,int TableSize>
T *HashSymbolTable<T,TableSize>::Find(const char *Key)
	{
	int hashValue=hashTable.HashString(Key);
	T *t=hashTable.GetFirstAt(hashValue);
	while(t)
		{
		if(t->Equals(Key)) return t;
		t=(T*)(t->next);
		}
	return t;
	}



template <class T,int TableSize>
void HashSymbolTable<T,TableSize>::Delete(const char *Key)
	{
	int hashValue=hashTable.HashString(Key);
	T *t=Find(Key);
	if(t) hashTable.Delete(t,hashValue);
	}



template <class T,int TableSize>
void HashSymbolTable<T,TableSize>::Insert(T *t,const char *Key)
	{
	int hashValue=hashTable.HashString(Key);
	hashTable.Insert(t,hashValue);
	++NumEntries;
	}



#endif
