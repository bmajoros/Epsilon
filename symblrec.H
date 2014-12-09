// =======================================
// symblrec.h
//
// Symbol-table entry records
//
//
// =======================================

#ifndef INCL_SYMBLREC_H
#define INCL_SYMBLREC_H

#include <string.h>
#include "libsrc/scopestk.H"
#include "libsrc/safecopy.H"
#include "method.H"
#include <iostream.h>



class Class;



/*	   INHERITANCE HIERARCHY FOR SYMBOL TABLE RECORDS


			 AbstractSymbolNode
			        |
				|
			    SymbolNode
				|
				|
	  -------------------------------------------------
	  |                     |                         |
	  |                     |                         |
ClassNameNode 		  ObjectNameNode	    MethodNameNode

*/



// ****************************************
//			  class SymbolNode
// ****************************************

// SymbolNode is an abstract symbol-table record

class SymbolNode : public AbstractSymbolNode 
{
protected:
  char *Name;
  SymbolNode(const char *Name);
public:
  RTTI_DECLARE_SUBCLASS(SymbolNode,link_node)
  virtual ~SymbolNode();
  virtual bool Equals(const char *Key);
};





// ****************************************
//          class ClassNameNode
// ****************************************

// A ClassNameNode stores the name of an
// Epsilon class

class ClassNameNode : public SymbolNode 
{
  Class *MyClass;
public:
  RTTI_DECLARE_SUBCLASS(ClassNameNode,link_node)
  ClassNameNode(const char *Name,Class *c);
  Class *GetClass();
};



// ****************************************
//           class MethodNameNode
// ****************************************

// A MethodNameNode stores the name of a
// method for a particular class

class MethodNameNode : public SymbolNode 
{
  MethodBody *MyBody;
public:
  RTTI_DECLARE_SUBCLASS(MethodNameNode,link_node)
  MethodNameNode(const char *Name);
  void SetBody(MethodBody *b);
  MethodBody *GetBody() const;
};



// ****************************************
//             enum StorageClass
// ****************************************

// Enum StorageClass specifies where an object
// is stored

enum StorageClass 
{
  OBJ_ATTRIBUTE,	// stored in object's attribute table
  OBJ_LOCAL,		// stored in run-time stack (includes parameters)
  OBJ_GLOBAL		// stored at very bottom of run-time stack
};


ostream &operator<<(ostream &os,const StorageClass &sc);



// ****************************************
//           class ObjectNameNode
// ****************************************

// An ObjectNameNode represents the name of
// a local object, parameter, object attribute,
// or built-in global object

class ObjectNameNode : public SymbolNode 
{
  StorageClass MyStorageClass;
  int LexicalPosition;
public:
  RTTI_DECLARE_SUBCLASS(ObjectNameNode,link_node)
  ObjectNameNode(const char *Name,StorageClass c,int LexicalPosition);
  int GetLexicalPosition() const;
  StorageClass GetStorageClass() const;
};



#endif
