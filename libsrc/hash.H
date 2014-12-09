// ================================
// Hash.h
//
// Hash table
//
// ================================

#ifndef INCL_HASH_H
#define INCL_HASH_H



int hashpjw(const char *s);



// *******************************************
//			  class SimpleHashTable
// *******************************************

// This class gives very basic support for hash
// tables.  Type T must be derived from link_node,
// and the TableSize should be a prime.

template <class T,int TableSize>
class SimpleHashTable 
{
  T *TheTable[TableSize];
public:
  SimpleHashTable();
  virtual ~SimpleHashTable();
  T *GetFirstAt(int HashValue) { return TheTable[HashValue]; }
  void Insert(T *t,int HashValue);
  void Delete(T *t,int HashValue);
  int HashString(const char *p);
};



// *******************************************
//			 SimpleHashTable methods
// *******************************************

template <class T,int TableSize>
SimpleHashTable<T,TableSize>::SimpleHashTable()
{
  for(int i=0 ; i<TableSize ; i++)
    TheTable[i]=0;
}



template <class T,int TableSize>
SimpleHashTable<T,TableSize>::~SimpleHashTable()
{
  for(int i=0 ; i<TableSize ; i++)
    {
      // Delete the ith list by traversing it
      
      T *ThisNode=TheTable[i];
      while(ThisNode)
	{
	  T *NextNode=(T*)ThisNode->next;
	  delete ThisNode;
	  ThisNode=NextNode;
	}
    }
}



template <class T,int TableSize>
void SimpleHashTable<T,TableSize>::Insert(T *t,int HashValue)
{
  t->next=TheTable[HashValue];
  TheTable[HashValue]=t;
}



template <class T,int TableSize>
void SimpleHashTable<T,TableSize>::Delete(T *t,int HashValue)
{
  if(TheTable[HashValue]==t)
    {
      TheTable[HashValue]=(T*)(t->next);
      delete t;
    }
  else
    {
      T *p=TheTable[HashValue];
      while(p && p->next != t) p=(T*)(p->next);
      if(p)
	{
	  p->next=t->next;
	  delete t;
	}
    }
}



template <class T,int TableSize>
int SimpleHashTable<T,TableSize>::HashString(const char *s)
{
  return hashpjw(s) % TableSize;
}




#endif
