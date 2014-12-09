// =======================================
// Class.h
//
// Representations of Epsilon classes
//
//
// =======================================

#ifndef INCL_CLASS_H
#define INCL_CLASS_H

#include "symblrec.H"


typedef ScopeStack<SymbolNode,41> EpsilonScopeStack;

class Object;
class Class_Object;


// ****************************************
//	   class ClassSymbolTable
// ****************************************
typedef HashSymbolTable<SymbolNode,31> ClassSymbolTable;

#define ARITY_DETECT -1


// ****************************************
//	         class Class
// ****************************************

// Instances of Class (and its subclasses)
// represent Epsilon classes, which can
// themselves be instantiated to produce
// Epsilon objects (which are represented by
// instances of Object).  Subclasses of Class
// instantiate subclasses of Object.  Class and
// its subclasses can be viewed as "metaclasses"
// because each Epsilon class is really just an
// instance of one of these (meta) classes.

class Class : public link_node 
{
  Class *SuperClass;
  linked_list SubClasses; // list of Class
  ClassSymbolTable *Attributes;
  ClassSymbolTable *Methods;
  int NumAttributes;  // Not including base-class attributes
  const char *Name;
  Class_Object *Representative; // Me, as a first-class object
  int detectArity(const char *methodName);
public:
  RTTI_DECLARE_SUBCLASS(Class,link_node)
  Class(const char *Name,Class *SuperClass=0);
  virtual ~Class();
  void AddAttribute(const char *Name);
  ObjectNameNode *FindAttribute(const char *Name);
  MethodNameNode *AddMethod(const char *Name);
  MethodNameNode *AddMethod(const char *Name,CppMethod f,int AR_size);
  MethodNameNode *AddMethod(const char *Name,SyntaxForest *,int AR_size=0);
  MethodNameNode *FindMethod(const char *Name);
  int TotalAttributes() const;
  Class *GetSuperClass() { return SuperClass; }
  linked_list &GetSubclasses() { return SubClasses; }
  virtual Object *Instantiate(int SubclassAttributes=0);
  void ReleaseSymbols() { delete Attributes; Attributes=0; }//### call this
  const char *GetName() const;
  void SetRepresentative(Class_Object *);
  Class_Object *AsFirstClassObject() const;
  Class *GetMetaclass() const;

  // New routines for easily adding built-in classes:
  void setMetaClass(const char *);
  void setMetaClass(Class *);
  void addInstanceMethod(const char *name,CppMethod body,
			 int arity=ARITY_DETECT);
  void addInstanceMethod(const char *name);
  void addClassMethod(const char *name,CppMethod body,
		      int arity=ARITY_DETECT);
  void addClassMethod(const char *name);
  void declare(const char *,EpsilonScopeStack &);
  void undeclare(const char *,EpsilonScopeStack &);
};



// ****************************************
//	     class StrTok_Class
// ****************************************
class StrTok_Class : public Class
{
public:
  RTTI_DECLARE_SUBCLASS(StrTok_Class,Class)
  StrTok_Class(Class *s) : Class("StringTokenizer",s) {}
  virtual Object *Instantiate(int SubclassAttributes=0);  
};



// ****************************************
//	     class Array_Class
// ****************************************
class Array_Class : public Class 
{
public:
  RTTI_DECLARE_SUBCLASS(Array_Class,link_node)
  Array_Class(Class *s) : Class("Array",s) {}
  virtual Object *Instantiate(int SubclassAttributes=0);
};



// ****************************************
//	      class Block_Class
// ****************************************

// This class no longer exists.  Block_Objects are of a class which is
// an instance of Class.  This is OK because we only create Block_Objects
// when an AstBlockLiteral node is executed; the Block_Object is created
// directly, rather than by sending block_class the Instantiate() message.



// ****************************************
//	      class Nil_Class
// ****************************************
class Nil_Class : public Class 
{
public:
  RTTI_DECLARE_SUBCLASS(Nil_Class,link_node)
  Nil_Class(Class *s) : Class("Nil",s) {}
  virtual Object *Instantiate(int SubclassAttributes=0);
};



// ****************************************
//	     class Int_Class
// ****************************************
class Int_Class : public Class 
{
public:
  RTTI_DECLARE_SUBCLASS(Int_Class,link_node)
  Int_Class(Class *s) : Class("Integer",s) {}
  virtual Object *Instantiate(int SubclassAttributes=0);
};



// ****************************************
//	      class Float_Class
// ****************************************
class Float_Class : public Class 
{
public:
  RTTI_DECLARE_SUBCLASS(Float_Class,link_node)
  Float_Class(Class *s) : Class("Float",s) {}
  virtual Object *Instantiate(int SubclassAttributes=0);
};



// ****************************************
//	    class String_Class
// ****************************************
class String_Class : public Class 
{
public:
  RTTI_DECLARE_SUBCLASS(String_Class,link_node)
  String_Class(Class *s) : Class("String",s) {}
  virtual Object *Instantiate(int SubclassAttributes=0);
};



// ****************************************
//	     class Char_Class
// ****************************************
class Char_Class : public Class 
{
public:
  RTTI_DECLARE_SUBCLASS(Char_Class,link_node)
  Char_Class(Class *s) : Class("Char",s) {}
  virtual Object *Instantiate(int SubclassAttributes=0);
};



// *** Bool_Class has been deleted, because it
// is not needed.  True and False (as in the
// two built-in Epsilon classes) are just instances
// of Class; they don't need any special data, so
// they don't need their own metaclasses.



// ****************************************
//	     class IStream_Class
// ****************************************
class IStream_Class : public Class 
{
public:
  RTTI_DECLARE_SUBCLASS(IStream_Class,link_node)
  IStream_Class(Class *s) : Class("istream",s) {}
  virtual Object *Instantiate(int SubclassAttributes=0);
};



// ****************************************
//	    class OStream_Class
// ****************************************
class OStream_Class : public Class 
{
public:
  RTTI_DECLARE_SUBCLASS(OStream_Class,link_node)
  OStream_Class(Class *s) : Class("ostream",s) {}
  virtual Object *Instantiate(int SubclassAttributes=0);
};



// ****************************************
//	    class IFStream_Class
// ****************************************
class IFStream_Class : public Class 
{
public:
  RTTI_DECLARE_SUBCLASS(IFStream_Class,link_node)
  IFStream_Class(Class *s) : Class("ifstream",s) {}
  virtual Object *Instantiate(int SubclassAttributes=0);
};



// ****************************************
//	   class OFStream_Class
// ****************************************
class OFStream_Class : public Class 
{
public:
  RTTI_DECLARE_SUBCLASS(OFStream_Class,link_node)
  OFStream_Class(Class *s) : Class("ofstream",s) {}
  virtual Object *Instantiate(int SubclassAttributes=0);
};



// ******************* globals *******************
extern Int_Class *int_class;
extern Float_Class *float_class;
extern Char_Class *char_class;
extern String_Class *string_class;
extern Class *true_class, *false_class;
extern Class *block_class;
extern IStream_Class *istream_class;
extern OStream_Class *ostream_class;
extern IFStream_Class *ifstream_class;
extern OFStream_Class *ofstream_class;
extern Nil_Class *nil_class;
extern Array_Class *array_class;
extern Class *Meta, *root_class;
extern StrTok_Class *strtok_class; // StringTokenizer

#endif
