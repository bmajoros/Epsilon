// =============================================
// array.C
// Expandable array objects
//
// Ported to Solaris on 3/27/96
// =============================================

#pragma interface

#ifndef INCL_ARRAY_C
#define INCL_ARRAY_C

#include "typeinfo.H"
#include <stdio.h>
#include "array.H"
//#include "Exception.H"


// =========================================================================
// ======================== PointerTable methods ===========================
// =========================================================================

template <class ElementType>
PointerTable<ElementType>::PointerTable(int TableSize)
  : Next(0), TheData(new ElementType*[TableSize])
{
  // ctor
  
  int f;
  for(f=0 ; f<TableSize ; f++)
    TheData[f]=0;
}



template <class ElementType>
PointerTable<ElementType> *PointerTable<ElementType>::GetSuccessor()
{
  return Next;
}



template <class ElementType>
void PointerTable<ElementType>::SetSuccessor(PointerTable<ElementType> *s)
{
  Next=s;
}



template <class ElementType>
ElementType *&PointerTable<ElementType>::operator[](int i)
{
  return TheData[i];
}



template <class ElementType>
PointerTable<ElementType>::PointerTable(void) 
  : Next(0), TheData(0)
{
  // ctor
}



template <class ElementType>
PointerTable<ElementType>::~PointerTable() 
{
  // dtor

  // Note that this destructor does not delete the data tables.
  // You must do that _before_ calling this destructor by
  // sending me the "Purge" message.  This is necessary, because
  // I do not know how many pointers are in my pointer table;
  // you must pass that as a parameter to the Purge() method.
  // (It would be nice to do it here, but destructors cannot
  // take parameters, and each pointer table should not store
  // its size).

  delete [] TheData;
}



template <class ElementType>
void PointerTable<ElementType>::Purge(int NumPointers)
{
  // Call this method just before deleting a PointerTable.
  // "NumPointers" is the size of the pointer table; i.e.,
  // the number of pointers to data tables.  Do not use this
  // for any other purpose than preparing for destruction,
  // because this method does not set the data table pointers
  // to NULL after deleting them (that would be unnecessary
  // when all you're going to do next is delete the pointer
  // table in the destructor).

  int i;
  for(i=0 ; i<NumPointers ; ++i)
    delete [] TheData[i];
}



template <class ElementType>
void PointerTable<ElementType>::Initialize(int TableSize)
{
  int f;
  
  Next=0;
  TheData = new ElementType*[TableSize];
  
  for(f=0 ; f<TableSize ; f++)
    TheData[f]=0;
}




// =========================================================================
// ======================== ElasticArray methods ===========================
// =========================================================================

template <class ElementType>
ElasticArray<ElementType>::ElasticArray(int iPointerSize,int iDataSize)
  : PointerSize(iPointerSize), DataSize(iDataSize)
{
  // ctor
  
  pTable.Initialize(PointerSize);
  pTable[0] = new ElementType[DataSize];
  Size=DataSize;
}





template <class ElementType>
ElasticArray<ElementType>::ElasticArray(void) 
{
  // ctor
  
  // NOTE:  If you use this constructor, then you MUST call Initialize()
  // 	    before actually using the ElasticArray!
}




template <class ElementType>
void ElasticArray<ElementType>::Initialize(int iPointerSize,int iDataSize)
{
  PointerSize=iPointerSize;
  DataSize=iDataSize;

  pTable.Initialize(PointerSize);
  pTable[0] = new ElementType[DataSize];
  Size=DataSize;
}





template <class ElementType>
ElasticArray<ElementType>::~ElasticArray()
{
  // dtor
  
  PointerTable<ElementType> *ThisNode=pTable.GetSuccessor();
  while(ThisNode)
    {
      PointerTable<ElementType> *NextNode=ThisNode->GetSuccessor();
      ThisNode->Purge(PointerSize);
      delete ThisNode;
      ThisNode=NextNode;
    }
}




template<class ElementType>
const ElementType &ElasticArray<ElementType>::operator[](int Index) const
{
  // This is a const index operation, which does not change any of the
  // internal representation of this ElasticArray.  It can be called
  // only if the Index is such that no expansion of the ElasticArray
  // will be necessary (otherwise, the internal state would have to change).

/*  if(Index>=Size)
    throw ArrayIndexException(Index," in const ElasticArray<>");*/

  ElasticArray<ElementType> &self=(ElasticArray<ElementType>&) *this;
  return self[Index];
}



template <class ElementType>
ElementType &ElasticArray<ElementType>::operator [](int Index)
{
  int WhichArray, LocalIndex, WhichTable, WhichPointer, f, g;
  PointerTable<ElementType> *ThisTable;
  
  WhichArray=Index / DataSize;
  LocalIndex=Index % DataSize;
  WhichTable=WhichArray / PointerSize;
  WhichPointer=WhichArray % PointerSize;
  
  if(Index < Size)
    { // the index is valid
      ThisTable=&pTable;
      for(f=1 ; f<=WhichTable ; f++)
	ThisTable=ThisTable->GetSuccessor();
      return ((*ThisTable)[WhichPointer])[LocalIndex];
    }
  
  else
    { // the index is invalid...must expand until this Index IS valid
      
      // find the last pointer table in the list
      ThisTable=&pTable;
      for(f=1 ; f<=WhichTable ; f++)
	if(ThisTable->GetSuccessor())
	  ThisTable=ThisTable->GetSuccessor();
	else
	  break;
      
      // this pointer table may be only partially filled...
      // fill in the rest (but only fill in the rest of this 
      // pointer-table if more pointer tables are about to be 
      // concatenated after it in the list)
      if(f<=WhichTable)
	for(g=(Size / DataSize) % PointerSize ; 
	    g && (g<PointerSize) ; g++)
	  (*ThisTable)[g] = new ElementType[DataSize];
      
      // allocate and initialize (fill in) as many additional 
      // pointer tables as are needed
      for( ; f<=WhichTable ; f++)
	{
	  ThisTable->SetSuccessor(
             new PointerTable<ElementType>(PointerSize));
	  
	  ThisTable=ThisTable->GetSuccessor();
	  
	  if(f<WhichTable) // if more tables will be appended to this one
	    for(g=0 ; g<PointerSize ; g++) // fill in all pointer slots
	      (*ThisTable)[g] = new ElementType[DataSize];
	  
	  else // this is the last table to be added
	    {
	      for(g=0 ; g<=WhichPointer ; g++)
		(*ThisTable)[g] = new ElementType[DataSize];
	      
	      // update size before returning
	      Size = WhichTable*PointerSize*DataSize + 
		// data in full tables
		(WhichPointer+1)*DataSize; // + partial table at end
	      
	      return ((*ThisTable)[WhichPointer])[LocalIndex];
	    }
	}
      
      // if execution reaches this point, then the preceding for-loop
      // was not executed -- therefore, we had enough tables before,
      // but the last table did not have quite enough entries...we need
      // only add some more entries
      for(g=(Size / DataSize) % PointerSize ; g<=WhichPointer ; g++)
	{
	  Size+=DataSize;  // update Size
	  (*ThisTable)[g] = new ElementType[DataSize];
	}
      
      return ((*ThisTable)[WhichPointer])[LocalIndex];
    }
}


#endif
