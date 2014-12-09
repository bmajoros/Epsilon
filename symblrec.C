// =======================================
// symblrec.cpp
//
// Symbol-table entry records
//
//
// =======================================

#include "libsrc/typeinfo.H"
#include "symblrec.H"



RTTI_DEFINE_SUBCLASS(AbstractSymbolNode,link_node)
RTTI_DEFINE_SUBCLASS(SymbolNode,AbstractSymbolNode)
RTTI_DEFINE_SUBCLASS(ClassNameNode,SymbolNode)
RTTI_DEFINE_SUBCLASS(ObjectNameNode,SymbolNode)
RTTI_DEFINE_SUBCLASS(MethodNameNode,SymbolNode)


// ****************************************
//			  SymbolNode methods
// ****************************************

SymbolNode::SymbolNode(const char *Name) : Name(Duplicate(Name))
	{
	// ctor
	}



SymbolNode::~SymbolNode()
	{
	delete [] Name;
	}



bool SymbolNode::Equals(const char *Key)
	{
	return !strcmp(Key,Name);
	}



// ****************************************
//          ClassNameNode methods
// ****************************************

ClassNameNode::ClassNameNode(const char *Name,Class *c)
	: SymbolNode(Name), MyClass(c)
	{
	// ctor
	}



Class *ClassNameNode::GetClass()
	{
	return MyClass;
	}



// ****************************************
//           MethodNameNode methods
// ****************************************

MethodNameNode::MethodNameNode(const char *Name)
	: SymbolNode(Name), MyBody(0)
	{
	// ctor
	}



void MethodNameNode::SetBody(MethodBody *b)
	{
	MyBody=b;
	}



MethodBody *MethodNameNode::GetBody() const
	{
	return MyBody;
	}



// ****************************************
//           StorageClass operator
// ****************************************
ostream &operator<<(ostream &os,const StorageClass &sc)
	{
	switch(sc)
		{
		case OBJ_ATTRIBUTE: os << "OBJ_ATTRIBUTE"; break;
		case OBJ_LOCAL: os << "OBJ_LOCAL"; break;
		case OBJ_GLOBAL: os << "OBJ_GLOBAL"; break;
		}
	return os;
	}



// ****************************************
//          ObjectNameNode methods
// ****************************************

ObjectNameNode::ObjectNameNode(const char *Name,StorageClass c,int
	LexicalPosition) : SymbolNode(Name), MyStorageClass(c),
	LexicalPosition(LexicalPosition)
	{
	// ctor
	}



int ObjectNameNode::GetLexicalPosition() const
	{
	return LexicalPosition;
	}



StorageClass ObjectNameNode::GetStorageClass() const
	{
	return MyStorageClass;
	}



