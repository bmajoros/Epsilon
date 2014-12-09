// =======================================
// Object.h
//
// Representations of Epsilon objects
//
//
// =======================================

#ifndef INCL_OBJECT_H
#define INCL_OBJECT_H

#include "garbage.H"
#include "class.H"
#include "libsrc/StringObject.H"
#include <fstream.h>
#include "libsrc/StringTokenizer.H"


class ActivationRecord;
class AstBlockLiteral;
class Object;


// These are defined in object.cpp:
extern Object *nil;
extern Object *true_object, *false_object;


// ****************************************
//		class Object
// ****************************************

// An Object represents a single class instance
// in an Epsilon program.

class Object : public Garbage 
{
  friend class Array_Object; // needs to reallocate MyAttributes
  //	friend class Super_Object; // needs to inhibit polymorphism for []
  Class *MyClass;
  Object **MyAttributes;  // array of Object*
public:
  RTTI_DECLARE_SUBCLASS(Object,link_node)
  Object(Class *c,int NumAttr=0);
  virtual ~Object();
  virtual Object *&operator[](int i);
  virtual Object *GetAttribute(int i) const;
  virtual void SetAttribute(int i,Object *obj) { (*this)[i]=obj; }
  virtual Class *GetClass() const { return MyClass; }
  virtual void SetClass(Class *);
  virtual int TotalAttributes() const;
  
  // These are Root's methods
  static Object *isNil(RunTimeEnvironment &);
  static Object *classOf(RunTimeEnvironment &);
  static Object *error(RunTimeEnvironment &);
};



// ****************************************
//	   class StrTok_Object
// ****************************************
class StrTok_Object : public Object
{
  StringTokenizer *tokenizer;
  void initSource(const StringObject &src);
  void initSourceDelim(const StringObject &src,
		       const StringObject &delim);
public:
  RTTI_DECLARE_SUBCLASS(StrTok_Object,link_node)
  StrTok_Object(Class *,int subclassAttributes);

  // instance methods
  static Object *initSource(RunTimeEnvironment &);
  static Object *initSourceDelim(RunTimeEnvironment &);
  static Object *hasMoreTokens(RunTimeEnvironment &);
  static Object *nextToken(RunTimeEnvironment &);
};


// ****************************************
//	   class Class_Object
// ****************************************
class Class_Object : public Object 
{
  Class *Represented; // Which class I represent (as a first-class object)
public:
  RTTI_DECLARE_SUBCLASS(Class_Object,link_node)
  Class_Object(Class *MyClass,Class *Represented,int NumAttr=0);
  Class *WhoDoYouRepresent() const;
  
  // These are methods of Meta (class methods for all Epsilon classes)
  static Object *New(RunTimeEnvironment &);
  static Object *superClass(RunTimeEnvironment &);
  static Object *subClasses(RunTimeEnvironment &);
  static Object *name(RunTimeEnvironment &);
};



// ****************************************
//	      class Array_Object
// ****************************************

// A zero-based array

class Array_Object : public Object 
{
  int NumArrayElements;
  int FirstElementIndex();
public:
  RTTI_DECLARE_SUBCLASS(Array_Object,link_node)
  Array_Object(Class *,int SubclassAttr=0,int NumArrayElem=0);
  virtual int TotalAttributes() const;
  virtual void SetArrayElement(int i,Object *);
  virtual Object *GetArrayElement(int i);
  static Object *setSize(RunTimeEnvironment &);// causes reallocation
  static Object *getSize(RunTimeEnvironment &);
  static Object *at(RunTimeEnvironment &);
  static Object *atPut(RunTimeEnvironment &);
  static Object *copyFrom(RunTimeEnvironment &);
};




// ****************************************
//	      class Nil_Object
// ****************************************

// There is one Nil_Object instance in the
// system, and it is called nil.  It is the
// only built-in type that returns true to
// the isNil message.

class Nil_Object : public Object 
{
public:
  RTTI_DECLARE_SUBCLASS(Nil_Object,link_node)
  Nil_Object(Class *,int NumAttr=0);
  static Object *isNil(RunTimeEnvironment &);
  static Object *displayOn(RunTimeEnvironment &);
  static Object *hashValue(RunTimeEnvironment &);
};



// ****************************************
//	       class Block_Object
// ****************************************

// A Block_Object represents a block in the Epsilon
// source code, which can be sent messages, stored
// in data structures, or passed as arguments.  This
// class differs from the other subclasses of Object
// in that users cannot explicitly use the name
// "Block" to instantiate a Block_Object; they must
// use a block construct.

// (This class is called "BlockContext" in gnu smalltalk)

class Block_Object : public Object 
{
  ActivationRecord *StaticChain;
  AstBlockLiteral *MyAstNode;
public:
  RTTI_DECLARE_SUBCLASS(Block_Object,link_node)
  Block_Object(ActivationRecord *StaticChain,AstBlockLiteral *,Class *,
	       int NumAttributes=0);
  ActivationRecord *GetStaticChain() const { return StaticChain; }
  Object *Invoke(RunTimeEnvironment &);
  static Object *evaluate(RunTimeEnvironment &);
  static Object *evaluateOn(RunTimeEnvironment &);
  static Object *evaluateOnAnd(RunTimeEnvironment &);
  static Object *evaluateOnAndAnd(RunTimeEnvironment &);
  static Object *whileTrue(RunTimeEnvironment &);
  static Object *whileFalse(RunTimeEnvironment &);
  static Object *until(RunTimeEnvironment &);
};



// ****************************************
//	     class Int_Object
// ****************************************
class Int_Object : public Object 
{
  int Value;
public:
  RTTI_DECLARE_SUBCLASS(Int_Object,link_node)
  Int_Object(Class *MyClass,int NumAttributes=0,int Value=0);
  void SetValue(int v) { Value=v; }
  int GetValue() const;
  static Object *plus(RunTimeEnvironment &);
  static Object *minus(RunTimeEnvironment &);
  static Object *times(RunTimeEnvironment &);
  static Object *dividedBy(RunTimeEnvironment &);
  static Object *modulus(RunTimeEnvironment &);
  static Object *displayOn(RunTimeEnvironment &);
  static Object *readFrom(RunTimeEnvironment &);
  static Object *upTo(RunTimeEnvironment &);
  static Object *downTo(RunTimeEnvironment &);
  static Object *greaterThan(RunTimeEnvironment &);
  static Object *lessThan(RunTimeEnvironment &);
  static Object *greaterEqual(RunTimeEnvironment &);
  static Object *lessEqual(RunTimeEnvironment &);
  static Object *equal(RunTimeEnvironment &);
  static Object *notEqual(RunTimeEnvironment &);
  static Object *asFloat(RunTimeEnvironment &);
  static Object *asString(RunTimeEnvironment &);
  static Object *asChar(RunTimeEnvironment &);
  static Object *hashValue(RunTimeEnvironment &);
  static Object *random(RunTimeEnvironment &);
  static Object *shiftLeft(RunTimeEnvironment &);
  static Object *shiftRight(RunTimeEnvironment &);
  static Object *bitOr(RunTimeEnvironment &);
  static Object *bitAnd(RunTimeEnvironment &);
  static Object *bitNot(RunTimeEnvironment &);
  static Object *timesDo(RunTimeEnvironment &);
};



// ****************************************
//	      class Float_Object
// ****************************************
class Float_Object : public Object 
{
  float Value;
public:
  RTTI_DECLARE_SUBCLASS(Float_Object,link_node)
  Float_Object(Class *MyClass,int NumAttributes=0,float Value=0);
  void SetValue(float v) { Value=v; }
  float GetValue() const;
  static Object *plus(RunTimeEnvironment &);
  static Object *minus(RunTimeEnvironment &);
  static Object *times(RunTimeEnvironment &);
  static Object *dividedBy(RunTimeEnvironment &);
  static Object *displayOn(RunTimeEnvironment &);
  static Object *greaterThan(RunTimeEnvironment &);
  static Object *lessThan(RunTimeEnvironment &);
  static Object *greaterEqual(RunTimeEnvironment &);
  static Object *lessEqual(RunTimeEnvironment &);
  static Object *equal(RunTimeEnvironment &);
  static Object *notEqual(RunTimeEnvironment &);
  static Object *asInt(RunTimeEnvironment &);
  static Object *asString(RunTimeEnvironment &);
  static Object *hashValue(RunTimeEnvironment &);
  static Object *misc_math_func(RunTimeEnvironment &env,
    double (*)(double),float MinVal=0,float MaxVal=0,
    char *FuncName="???");
  static Object *sin(RunTimeEnvironment &);
  static Object *acos(RunTimeEnvironment &);
  static Object *asin(RunTimeEnvironment &);
  static Object *atan(RunTimeEnvironment &);
  static Object *cos(RunTimeEnvironment &);
  static Object *tan(RunTimeEnvironment &);
  static Object *sqrt(RunTimeEnvironment &);
};



// ****************************************
//	   class String_Object
// ****************************************
class String_Object : public Object 
{
  StringObject Value;
  static int StrCmp(RunTimeEnvironment &);
public:
  RTTI_DECLARE_SUBCLASS(String_Object,link_node)
  String_Object(Class *MyClass,int NumAttributes=0,const char * ="");
  void SetValue(StringObject &v) { Value.CopyFrom(&v); }
  StringObject &GetValue() { return Value; }
  static Object *displayOn(RunTimeEnvironment &);
  static Object *readFrom(RunTimeEnvironment &);
  static Object *plus(RunTimeEnvironment &);
  static Object *lessThan(RunTimeEnvironment &);
  static Object *greaterThan(RunTimeEnvironment &);
  static Object *equal(RunTimeEnvironment &);
  static Object *length(RunTimeEnvironment &);
  static Object *at(RunTimeEnvironment &);
  static Object *atPut(RunTimeEnvironment &);
  static Object *asInt(RunTimeEnvironment &);
  static Object *asFloat(RunTimeEnvironment &);
  static Object *beginEnd(RunTimeEnvironment &);
  static Object *hashValue(RunTimeEnvironment &);
};



// ****************************************
//	    class Char_Object
// ****************************************
class Char_Object : public Object 
{
  char Value;
public:
  RTTI_DECLARE_SUBCLASS(Char_Object,link_node)
  Char_Object(Class *MyClass,int NumAttributes=0,char Value=' ');
  void SetValue(char v);
  char GetValue() const;
  static Object *displayOn(RunTimeEnvironment &);
  static Object *lessThan(RunTimeEnvironment &);
  static Object *greaterThan(RunTimeEnvironment &);
  static Object *equal(RunTimeEnvironment &);
  static Object *asString(RunTimeEnvironment &);
  static Object *ascii(RunTimeEnvironment &);
  static Object *readFrom(RunTimeEnvironment &);
  static Object *hashValue(RunTimeEnvironment &);
};



// ****************************************
//	     class True_Object
// ****************************************

// My instances respond positively to messages.

class True_Object : public Object 
{
public:
  RTTI_DECLARE_SUBCLASS(True_Object,link_node)
  True_Object(Class *MyClass,int NumAttributes=0);
  static Object *ifTrue(RunTimeEnvironment &);
  static Object *ifTrueElse(RunTimeEnvironment &);
  static Object *ifFalse(RunTimeEnvironment &);
  static Object *ifFalseElse(RunTimeEnvironment &);
  static Object *and(RunTimeEnvironment &);
  static Object *or(RunTimeEnvironment &);
  static Object *not(RunTimeEnvironment &);
  static Object *displayOn(RunTimeEnvironment &);
};



// ****************************************
//	    class False_Object
// ****************************************

// My instances respond negatively to messages.

class False_Object : public Object 
{
public:
  RTTI_DECLARE_SUBCLASS(False_Object,link_node)
  False_Object(Class *MyClass,int NumAttributes=0);
  static Object *ifTrue(RunTimeEnvironment &);
  static Object *ifTrueElse(RunTimeEnvironment &);
  static Object *ifFalse(RunTimeEnvironment &);
  static Object *ifFalseElse(RunTimeEnvironment &);
  static Object *and(RunTimeEnvironment &);
  static Object *or(RunTimeEnvironment &);
  static Object *not(RunTimeEnvironment &);
  static Object *displayOn(RunTimeEnvironment &);
};



// ****************************************
//	   class IStream_Object
// ****************************************

// Not a user-accessible type.  Exists only so that
// we can give the user access to cin (consoleIn)

class IStream_Object : public Object 
{
public:
  RTTI_DECLARE_SUBCLASS(IStream_Object,link_node)
  IStream_Object(Class *MyClass,int NumAttributes=0);
  virtual istream &GetStream() { return cin; }
  static Object *getch(RunTimeEnvironment &);
};



// ****************************************
//	   class OStream_Object
// ****************************************

// Not a user-accessible type.  Exists only so we
// can give the user access to cout (consoleOut)

class OStream_Object : public Object 
{
public:
  RTTI_DECLARE_SUBCLASS(OStream_Object,link_node)
  OStream_Object(Class *MyClass,int NumAttributes=0);
  virtual ostream &GetStream() { return cout; }
  static Object *nl(RunTimeEnvironment &);
};



// ****************************************
//	    class IFStream_Object
// ****************************************
class IFStream_Object : public IStream_Object 
{
  ifstream Value;
public:
  RTTI_DECLARE_SUBCLASS(IFStream_Object,link_node)
  IFStream_Object(Class *MyClass,int NumAttributes=0);
  static Object *open(RunTimeEnvironment &);
  static Object *close(RunTimeEnvironment &);
  static Object *eof(RunTimeEnvironment &);
  static Object *bad(RunTimeEnvironment &);
  static Object *position(RunTimeEnvironment &);// seekg
  static Object *putBack(RunTimeEnvironment &);
  static Object *peek(RunTimeEnvironment &);
  virtual istream &GetStream();
};



// ****************************************
//	    class OFStream_Object
// ****************************************
class OFStream_Object : public OStream_Object 
{
  ofstream Value;
public:
  RTTI_DECLARE_SUBCLASS(OFStream_Object,link_node)
  OFStream_Object(Class *MyClass,int NumAttributes=0);
  static Object *open(RunTimeEnvironment &);
  static Object *close(RunTimeEnvironment &);
  static Object *bad(RunTimeEnvironment &);
  virtual ostream &GetStream();
};



#endif
