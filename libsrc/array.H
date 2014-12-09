// =============================================
// array.H
// Expandable array objects
//
// Ported to Solaris on 3/27/96
// =============================================

#pragma interface


#ifndef INCL_ARRAY_H
#define INCL_ARRAY_H



// class PointerTable is used by ElasticArray; don't use it directly.
// Use ElasticArray (see below).

template <class ElementType>
class PointerTable {
  ElementType **TheData;
  PointerTable<ElementType> *Next;
public:
  PointerTable(int TableSize);
  PointerTable(void);
  void Initialize(int TableSize);
  virtual ~PointerTable();
  void Purge(int NumPointers); // Call this before deleting a PointerTable!
  ElementType *&operator[](int);
  PointerTable<ElementType> *GetSuccessor();
  void SetSuccessor(PointerTable<ElementType> *);
};



// An ElasticArray can grow arbitrarily large; when it is given
// an index which is too large, it simply expands until it is big
// enough to satisfy the request.  It has three advantages:
//
//   1. Its maximum size is bounded only by the machine (or swap space)
//   2. Under normal conditions, it operates in constant time
//   3. Even though it has a virtually infinite capacity, it takes
//      up very little space when only few indices are actually used
//
// It has these disadvantages:
//   1. Each indexing operation is slower than an indexing operation
//      for a "built-in" array in C++ (but is still constant-time)
//   2. It allocates objects in chunks, so for large objects, there
//      is some space overhead for the unused objects in a chunk
//      (but this overhead is less than if you used large C++ arrays)
//
// In order for an ElasticArray to work efficiently, you must pick the 
// 'pointer-size' and 'data-size' properly (they get passed to the
// constructor).  To do this, you need to understand the basic
// architecture of the data structure.
//
// An ElasticArray has a pointer-table, which is an array of pointers.
// Each of these pointers points to a data-table, which holds the
// actual elements of the ElasticArray.  The ElasticArray starts
// out with a single pointer-table and a single data-table, so the
// initial capacity of the ElasticArray is equal to data-size
// (data-size is the number of elements in a data-table, and pointer-
// size is the number of pointers in a pointer-table).  As the 
// ElasticArray grows, more data-tables are added; thus, the 
// ElasticArray grows in spurts (i.e., by the size of the data-table).
//
// Thus, if only a few data-tables are truly needed, only that many
// will be allocated.  With C++ arrays, you have to allocate the
// whole array in one big chunk, even if you only ever use part of
// the array.  
//
// When the pointer-table becomes full (that is, when all of its
// pointers point to data-tables which are full), another pointer-table
// is appended onto the end of the existing pointer-table.  This can
// continue arbitrarily, which allows the ElasticArray to grow without
// limit.  However, since the pointer-tables are linked into a list,
// the ElasticArray operates in linear time whenever more than one
// pointer-table is used.
//
// Thus, you should choose the pointer-size and data-size such that
// it is highly probable that the ElasticArray will never need
// more than one pointer-table; that is, you should pick these two
// numbers such that pointer-size * data-size is large enough
// that no index should ever exceed this product.  If an occasional
// index does exceed pointer-size * data-size, that's OK, but
// the ElasticArray operates in linear time for all such large indices.

template <class ElementType>
class ElasticArray {
protected:
  PointerTable<ElementType> pTable;
  int PointerSize;  // size of pointer-tables
  int DataSize;     // size of data arrays
  int Size;
public:
  ElasticArray(int iPointerSize,int iDataSize);
  ElasticArray(void);
  virtual void Initialize(int iPointerSize,int iDataSize);
  virtual ~ElasticArray();
  virtual ElementType &operator [](int Index);
  virtual const ElementType &operator[](int Index) const;
};




#endif
