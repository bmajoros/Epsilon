// =======================================
// Object.cpp
//
// Representations of Epsilon objects
//
//
// =======================================

#include "libsrc/typeinfo.H"
#include "object.H"
#include "rtstack.H"
#include "ast.H"
#include "except.H"
#include "execute.H"
#include <strstream.h>
#include <stdio.h>
#include "libsrc/Random.H"
#include <math.h>
//#include <conio.h>
//#include "math_funcs.H"


RTTI_DEFINE_SUBCLASS(Object,Garbage)
RTTI_DEFINE_SUBCLASS(Class_Object,Object)
RTTI_DEFINE_SUBCLASS(Array_Object,Object)
RTTI_DEFINE_SUBCLASS(Nil_Object,Object)
RTTI_DEFINE_SUBCLASS(Block_Object,Object)
RTTI_DEFINE_SUBCLASS(Int_Object,Object)
RTTI_DEFINE_SUBCLASS(Float_Object,Object)
RTTI_DEFINE_SUBCLASS(String_Object,Object)
RTTI_DEFINE_SUBCLASS(Char_Object,Object)
RTTI_DEFINE_SUBCLASS(True_Object,Object)
RTTI_DEFINE_SUBCLASS(False_Object,Object)
RTTI_DEFINE_SUBCLASS(IStream_Object,Object)
RTTI_DEFINE_SUBCLASS(OStream_Object,Object)
RTTI_DEFINE_SUBCLASS(IFStream_Object,IStream_Object)
RTTI_DEFINE_SUBCLASS(OFStream_Object,OStream_Object)
RTTI_DEFINE_SUBCLASS(StrTok_Object,Object)


extern Object *nil=0;
extern Object *true_object=0, *false_object=0;


int min(int a,int b) { return a<b ? a : b; }


// ****************************************
//		 Object methods
// ****************************************
Object::Object(Class *c,int NumAttr)
  : MyClass(c), MyAttributes(0)
{
  // ctor
  
  if(NumAttr)
    {
      MyAttributes=new Object*[NumAttr];
      for(int i=0 ; i<NumAttr ; i++)
	MyAttributes[i]=nil;
    }
}



int Object::TotalAttributes() const
{
  return MyClass->TotalAttributes();
}



void Object::SetClass(Class *c)
{
  MyClass=c;
}



Object *Object::isNil(RunTimeEnvironment &)
{
  // x isNil ifTrue: [...
  
  return false_object;
}



Object *Object::classOf(RunTimeEnvironment &env)
{
  // someObject classOf name displayOn: consoleOut;
  
  Object *self=env.GetSelf();
  return self->GetClass()->AsFirstClassObject();
}



Object::~Object()
{
  // dtor
  
  delete [] MyAttributes;
}



Object *Object::GetAttribute(int i) const
{
  Object *NonConstThis=(Object*)this;
  return (*NonConstThis)[i];
}



Object *&Object::operator[](int i)
{
  
  // ### This bounds checking probably slows it down alot...
  if(i<0 || i>=TotalAttributes())
    throw INTERNAL_ERROR(__FILE__,__LINE__,
			 "Invalid index in Object::operator[]");
  
  return MyAttributes[i];
}



Object *Object::error(RunTimeEnvironment &env)
{
  // self error: "use new: numElem"
  
  String_Object *msg=
    DYNAMIC_CAST_PTR(String_Object,env.GetParameter(1));
  if(!msg)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Second parameter to \"error:\" must be a string");
  
  throw RUN_TIME_ERROR(__FILE__,__LINE__,msg->GetValue().GetCopy());
}




// ****************************************
//	   Class_Object methods
// ****************************************
Class_Object::Class_Object(Class *MyClass,Class *Represented,int NumAttr)
  : Object(MyClass,NumAttr), Represented(Represented)
{
  // ctor
}



Class *Class_Object::WhoDoYouRepresent() const
{
  return Represented;
}



Object *Class_Object::New(RunTimeEnvironment &env)
{
  // SomeClass new
  
  Class_Object *self=
    DYNAMIC_CAST_PTR(Class_Object,env.GetSelf());
  return self->WhoDoYouRepresent()->Instantiate();
}



Object *Class_Object::name(RunTimeEnvironment &env)
{
  // Root name displayOn: consoleOut
  
  Class_Object *self=
    DYNAMIC_CAST_PTR(Class_Object,env.GetSelf());
  String_Object *so=
    DYNAMIC_CAST_PTR(String_Object,string_class->Instantiate());
  so->GetValue().CopyFrom(self->WhoDoYouRepresent()->GetName());
  return so;
}



Object *Class_Object::superClass(RunTimeEnvironment &env)
{
  // SomeClass superClass
  
  Class_Object *self=
    DYNAMIC_CAST_PTR(Class_Object,env.GetSelf());
  Class *TheSuperclass=self->WhoDoYouRepresent()->GetSuperClass();
  
  return TheSuperclass ? TheSuperclass->AsFirstClassObject() : nil;
}



Object *Class_Object::subClasses(RunTimeEnvironment &env)
{
  // SomeClass subClasses
  
  Class_Object *self=
    DYNAMIC_CAST_PTR(Class_Object,env.GetSelf());
  
  linked_list &sub_classes=self->WhoDoYouRepresent()->GetSubclasses();
  Array_Object *TheArray=
    new Array_Object(array_class,0,sub_classes.NumElements());
  
  sub_classes.reset_seq();
  Class *ThisClass;
  int Index=0;
  while(ThisClass=DYNAMIC_CAST_PTR(Class,sub_classes.sequential()))
    TheArray->SetArrayElement(Index++,ThisClass->AsFirstClassObject());
  
  return TheArray;
}



// ****************************************
//	     StrTok_Object methods
// ****************************************

StrTok_Object::StrTok_Object(Class *c,int subclassAttributes)
  : Object(c,subclassAttributes), tokenizer(NULL)
{
  // CTOR
}



Object *StrTok_Object::initSource(RunTimeEnvironment &env)
{
  // StringTokenizer::initSource: src

  // Get self
  StrTok_Object *self=
    DYNAMIC_CAST_PTR(StrTok_Object,env.GetSelf());

  // Get parameter
  String_Object *src=
    DYNAMIC_CAST_PTR(String_Object,env.GetParameter(1));
  if(!src)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
      "StringTokenizer::initSource: requires string parameter");

  // Perform operation
  self->initSource(src->GetValue());

  // Return result
  return self;
}



Object *StrTok_Object::initSourceDelim(RunTimeEnvironment &env)
{
  // StringTokenizer::initSource: src Delimiters: delim

  // Get self
  StrTok_Object *self=
    DYNAMIC_CAST_PTR(StrTok_Object,env.GetSelf());

  // Get parameters
  String_Object *src=
    DYNAMIC_CAST_PTR(String_Object,env.GetParameter(1));
  String_Object *delim=
    DYNAMIC_CAST_PTR(String_Object,env.GetParameter(2));
  if(!src || !delim)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
      "StringTokenizer::initSource:Delimiters: requires string parameters");

  // Perform operation
  self->initSourceDelim(src->GetValue(),delim->GetValue());

  // Return result
  return self;
}



void StrTok_Object::initSource(const StringObject &src)
{
  delete tokenizer;
  tokenizer=new StringTokenizer(src.AsCharArray(),"");
}



void StrTok_Object::initSourceDelim(const StringObject &src,
				    const StringObject &delim)
{
  delete tokenizer;
  tokenizer=new StringTokenizer(src.AsCharArray(),delim.AsCharArray());
}



Object *StrTok_Object::hasMoreTokens(RunTimeEnvironment &env)
{
  // StringTokenizer::hasMoreTokens

  // Get self
  StrTok_Object *self=
    DYNAMIC_CAST_PTR(StrTok_Object,env.GetSelf());

  // Perform operation
  if(!self->tokenizer) return nil;
  bool hasMore=self->tokenizer->hasMoreTokens();

  // Return result
  return hasMore ? true_object : false_object;
}



Object *StrTok_Object::nextToken(RunTimeEnvironment &env)
{
  // StringTokenizer::nextToken

  // Get self
  StrTok_Object *self=
    DYNAMIC_CAST_PTR(StrTok_Object,env.GetSelf());

  // Perform operation
  if(!self->tokenizer) return nil;
  const char *nextTok=self->tokenizer->nextToken();

  // Return result
  return new String_Object(string_class,0,nextTok);
}




// ****************************************
//	     Array_Object methods
// ****************************************
Array_Object::Array_Object(Class *c,int SubclassAttr,int NumArrayElem)
  : Object(c,SubclassAttr+NumArrayElem), NumArrayElements(NumArrayElem)
{
  // ctor
}



int Array_Object::TotalAttributes() const
{
  return MyClass->TotalAttributes()+NumArrayElements;
}



Object *Array_Object::setSize(RunTimeEnvironment &env)
{
  // anArray setSize: n;
  
  // causes reallocation; returns self
  
  // Get a pointer to the array object being resized
  Array_Object *self=DYNAMIC_CAST_PTR(Array_Object,env.GetSelf());
  
  // Get a pointer to the integer object specifying its new size
  Int_Object *new_size=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!new_size)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Array::setSize: requires an integer");
  
  // Get the value of the integer object
  int NewSize=new_size->GetValue();
  
  // The new total size for the attribute array is the new array size
  // plus the number of "real" attributes needed by subclasses of Array
  int NewTotalSize=self->MyClass->TotalAttributes()+NewSize;
  
  // Allocate the new attribute table
  Object **NewAttributes=new Object*[NewTotalSize];
  
  // Copy the old attributes into the new attributes table, but
  // no more than will fit!
  int NumToCopy=min(NewTotalSize,self->TotalAttributes());
  for(int i=0 ; i<NumToCopy ; i++) NewAttributes[i]=self->MyAttributes[i];
  
  // For any slots in the attribute table that were not filled in during
  // the previous step, initialize those slots to nil
  for( ; i<NewTotalSize ; i++) NewAttributes[i]=nil;
  
  // Delete the old attribute table
  delete [] self->MyAttributes;
  
  // Attach the new attribute table and store its size
  self->MyAttributes=NewAttributes;
  self->NumArrayElements=NewSize;
  
  return self;
}



Object *Array_Object::getSize(RunTimeEnvironment &env)
{
  Array_Object *self=DYNAMIC_CAST_PTR(Array_Object,env.GetSelf());
  return new Int_Object(int_class,0,self->NumArrayElements);
}



Object *Array_Object::at(RunTimeEnvironment &env)
{
  Array_Object *self=DYNAMIC_CAST_PTR(Array_Object,env.GetSelf());
  Int_Object *index_obj=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!index_obj)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Array::at: requires integer index");
  int index=index_obj->GetValue();
  if(index>=self->NumArrayElements || index<0)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Index out of range in Array::at:");
  return self->GetArrayElement(index);
}


void Array_Object::SetArrayElement(int i,Object *obj)
{
  SetAttribute(FirstElementIndex()+i,obj);
}



Object *Array_Object::GetArrayElement(int i)
{
  return GetAttribute(FirstElementIndex()+i);
}



int Array_Object::FirstElementIndex()
{
  return Object::TotalAttributes();
}



Object *Array_Object::atPut(RunTimeEnvironment &env)
{
  Array_Object *self=DYNAMIC_CAST_PTR(Array_Object,env.GetSelf());
  Int_Object *index_obj=
    DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!index_obj)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Array::at:put: requires integer index");
  Object *obj=env.GetParameter(2);
  
  int index=index_obj->GetValue();
  if(index>=self->NumArrayElements || index<0)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Index out of range in Array::at:put:");
  self->SetArrayElement(index,obj);
  
  return self;
}



Object *Array_Object::copyFrom(RunTimeEnvironment &env)
{
  Array_Object *self=DYNAMIC_CAST_PTR(Array_Object,env.GetSelf());
  Array_Object *parm=DYNAMIC_CAST_PTR(Array_Object,env.GetParameter(1));
  if(!parm)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Array::copyFrom: requires an array parameter");
  int NumToCopy=min(self->NumArrayElements,parm->NumArrayElements);
  for(int i=0 ; i<NumToCopy ; i++)
    self->SetArrayElement(i,parm->GetArrayElement(i));
  
  return self;
}






// ****************************************
//	      Nil_Object methods
// ****************************************
Nil_Object::Nil_Object(Class *c,int NumAttr)
  : Object(c,NumAttr)
{
  // ctor
}



Object *Nil_Object::isNil(RunTimeEnvironment &)
{
  // nil isNil ifTrue: [...
  
  return true_object;
}



Object *Nil_Object::hashValue(RunTimeEnvironment &)
{
  // nil hashValue
  
  return new Int_Object(int_class,0,0);
}



Object *Nil_Object::displayOn(RunTimeEnvironment &env)
{
  // nil displayOn: cout
  
  OStream_Object *rhs=DYNAMIC_CAST_PTR(OStream_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Nil::displayOn: applied to illegal object");
  rhs->GetStream() << "nil";
  
  return rhs;
}



// ****************************************
//	     Block_Object methods
// ****************************************
Block_Object::Block_Object(ActivationRecord *StaticChain,AstBlockLiteral
			   *ast,Class *c,int NumAttributes)
  : StaticChain(StaticChain), MyAstNode(ast), Object(c,NumAttributes)
{
  // ctor
}



Object *Block_Object::Invoke(RunTimeEnvironment &env)
{
  // Pushes an activation record and calls this block.
  
  int AR_size=MyAstNode->GetARsize();
  env.GetStack().PushAR(AR_size,this,nil);
  MyAstNode->GetStatements().Execute(env);
  Object *ReturnValue=MyAstNode->GetStatements().GetValue(env);
  env.PopAR();
  
  return ReturnValue;
}



Object *Block_Object::whileTrue(RunTimeEnvironment &env)
{
  // [ x lessThan: y ] whileTrue: [ bind x to (x plus: 10) ]
  
  Block_Object *self=DYNAMIC_CAST_PTR(Block_Object,env.GetSelf());
  Block_Object *parm=DYNAMIC_CAST_PTR(Block_Object,env.GetParameter(1));
  if(!parm)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Parameter to Block::whileTrue: must be a block");
  
  while(1)
    {
      // Evaluate myself
      Object *ReturnValue=self->Invoke(env);
      if(env.AreWeReturning()) break;
      
      // If I evaluated to false, then go no further
      if(ReturnValue==false_object) break;
      
      // Otherwise, execute the parameter block
      parm->Invoke(env);
      if(env.AreWeReturning()) break;
    }
  
  return nil;
}


Object *Block_Object::until(RunTimeEnvironment &env)
{
  // [ bind x to (x plus: 10) ] until: [ x lessThan: y ]
  
  Block_Object *self=DYNAMIC_CAST_PTR(Block_Object,env.GetSelf());
  Block_Object *parm=DYNAMIC_CAST_PTR(Block_Object,env.GetParameter(1));
  if(!parm)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Parameter to Block::until: must be a block");
  
  while(1)
    {
      // Evaluate myself
      self->Invoke(env);
      if(env.AreWeReturning()) break;
      
      // Execute the "until" block
      Object *ReturnValue=parm->Invoke(env);
      if(env.AreWeReturning()) break;
      
      // If it evaluated to true, then go no further
      if(ReturnValue==true_object) break;
    }
  
  return nil;
}




Object *Block_Object::whileFalse(RunTimeEnvironment &env)
{
  // [ x lessThan: y ] whileFalse: [ bind x to (x minus: 10) ];
  
  Block_Object *self=DYNAMIC_CAST_PTR(Block_Object,env.GetSelf());
  Block_Object *parm=DYNAMIC_CAST_PTR(Block_Object,env.GetParameter(1));
  if(!parm)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Parameter to Block::whileFalse: must be a block");
  
  while(1)
    {
      // Evaluate myself
      Object *ReturnValue=self->Invoke(env);
      if(env.AreWeReturning()) break;
      
      // If I evaluated to true, then go no further
      if(ReturnValue==true_object) break;
      
      // Otherwise, execute the parameter block
      parm->Invoke(env);
      if(env.AreWeReturning()) break;
    }
  
  return nil;
}



Object *Block_Object::evaluate(RunTimeEnvironment &env)
{
  // [ msg displayOn: aStream ] evaluate
  
  Block_Object *self=DYNAMIC_CAST_PTR(Block_Object,env.GetSelf());
  return self->Invoke(env);
}



Object *Block_Object::evaluateOn(RunTimeEnvironment &env)
{
  // [ :a | a displayOn: aStream; ] evaluateOn: 6;
  
  Block_Object *self=DYNAMIC_CAST_PTR(Block_Object,env.GetSelf());
  if(self->MyAstNode->GetNumParms()<1)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Too many parameters to block");
  Object *Parm1=env.GetParameter(1);
  int AR_size=self->MyAstNode->GetARsize();
  env.GetStack().PushAR(AR_size,self,nil);
  env.GetStack().PeekTop()->SetEntry(1,Parm1);
  self->MyAstNode->GetStatements().Execute(env);
  Object *ReturnValue=self->MyAstNode->GetStatements().GetValue(env);
  env.PopAR();
  return ReturnValue;
}



Object *Block_Object::evaluateOnAnd(RunTimeEnvironment &env)
{
  // [ :x :y | return new Point setX: x setY: y ] evaluateOn: 2 and: 3
  
  Block_Object *self=DYNAMIC_CAST_PTR(Block_Object,env.GetSelf());
  Object *Parm1=env.GetParameter(1);
  Object *Parm2=env.GetParameter(2);
  if(self->MyAstNode->GetNumParms()<2)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Too many parameters to block");
  int AR_size=self->MyAstNode->GetARsize();
  env.GetStack().PushAR(AR_size,self,nil);
  env.GetStack().PeekTop()->SetEntry(1,Parm1);
  env.GetStack().PeekTop()->SetEntry(2,Parm2);
  self->MyAstNode->GetStatements().Execute(env);
  Object *ReturnValue=self->MyAstNode->GetStatements().GetValue(env);
  env.PopAR();
  return ReturnValue;
}



Object *Block_Object::evaluateOnAndAnd(RunTimeEnvironment &env)
{
  // [ :x :y :z | ... ] evaluateOn: 3 and: 7 and: 10
  
  Block_Object *self=DYNAMIC_CAST_PTR(Block_Object,env.GetSelf());
  Object *Parm1=env.GetParameter(1);
  Object *Parm2=env.GetParameter(2);
  Object *Parm3=env.GetParameter(3);
  if(self->MyAstNode->GetNumParms()<3)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Too many parameters to block");
  int AR_size=self->MyAstNode->GetARsize();
  env.GetStack().PushAR(AR_size,self,nil);
  env.GetStack().PeekTop()->SetEntry(1,Parm1);
  env.GetStack().PeekTop()->SetEntry(2,Parm2);
  env.GetStack().PeekTop()->SetEntry(3,Parm3);
  self->MyAstNode->GetStatements().Execute(env);
  Object *ReturnValue=self->MyAstNode->GetStatements().GetValue(env);
  env.PopAR();
  return ReturnValue;
}



// ****************************************
//			  Int_Object methods
// ****************************************
Int_Object::Int_Object(Class *MyClass,int NumAttributes,int Value)
  : Object(MyClass,NumAttributes), Value(Value)
{
  // ctor
}



int Int_Object::GetValue() const
{
  return Value;
}



Object *Int_Object::hashValue(RunTimeEnvironment &env)
{
  // 123 hashValue
  
  return DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
}




Object *Int_Object::random(RunTimeEnvironment &env)
{
  // 6 random // evaluates to random # between 0 and 5
  
  Int_Object *self=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  return new Int_Object(int_class,0,::RandomNumber(self->Value));
}




Object *Int_Object::plus(RunTimeEnvironment &env)
{
  // 6 + 4
  
  Int_Object *lhs=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  Int_Object *rhs=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Integer::+ applied to illegal object");
  return new Int_Object(int_class,0,lhs->Value+rhs->Value);
}



Object *Int_Object::minus(RunTimeEnvironment &env)
{
  // 6 - 4
  
  Int_Object *lhs=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  Int_Object *rhs=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Integer::- applied to illegal object");
  return new Int_Object(int_class,0,lhs->Value-rhs->Value);
}



Object *Int_Object::times(RunTimeEnvironment &env)
{
  // 6 * 4
  
  Int_Object *lhs=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  Int_Object *rhs=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Integer::* applied to illegal object");
  return new Int_Object(int_class,0,lhs->Value*rhs->Value);
}



Object *Int_Object::dividedBy(RunTimeEnvironment &env)
{
  // 6 / 4
  
  Int_Object *lhs=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  Int_Object *rhs=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Integer::/ applied to illegal object");
  if(rhs->Value==0)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Division by zero in Integer::/");
  return new Int_Object(int_class,0,lhs->Value/rhs->Value);
}



Object *Int_Object::modulus(RunTimeEnvironment &env)
{
  // 6 % 4
  
  Int_Object *lhs=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  Int_Object *rhs=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Integer::% applied to illegal object");
  if(rhs->Value==0)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Division by zero in Integer::%");
  return new Int_Object(int_class,0,lhs->Value%rhs->Value);
}



Object *Int_Object::displayOn(RunTimeEnvironment &env)
{
  // 6 displayOn: cout
  
  Int_Object *lhs=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  OStream_Object *rhs=DYNAMIC_CAST_PTR(OStream_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Integer::displayOn: applied to illegal object");
  rhs->GetStream() << lhs->Value;
  
  return rhs;
}



Object *Int_Object::downTo(RunTimeEnvironment &env)
{
  // 10 downTo: 1 do: [ :a | a printOn: cout]
  
  Int_Object *self=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  Int_Object *to=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!to)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
	 "Parameter #1 of Integer::downTo:do: must be an integer");
  Block_Object *block=DYNAMIC_CAST_PTR(Block_Object,env.GetParameter(2));
  if(!block)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
	   "Parameter #2 of Integer::downTo:do: must be a block");
  
  for(int i=self->Value ; i>= to->Value ; i--)
    {
      env.GetStack().PushAR(2,block,nil);
      Int_Object *obj=new Int_Object(int_class,0,i);
      env.GetStack().PeekTop()->SetEntry(1,obj);
      Block_Object::evaluateOn(env);
      env.PopAR();
      if(env.AreWeReturning()) break;
    }
  
  return nil;
}



Object *Int_Object::upTo(RunTimeEnvironment &env)
{
  // 1 upTo: 10 do: [ :a | a printOn: cout]
  
  Int_Object *self=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  Int_Object *to=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!to)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
	"Parameter #1 of Integer::upTo:do: must be an integer");
  Block_Object *block=DYNAMIC_CAST_PTR(Block_Object,env.GetParameter(2));
  if(!block)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
	 "Parameter #2 of Integer::upTo:do: must be a block");
  
  for(int i=self->Value ; i<= to->Value ; i++)
    {
      env.GetStack().PushAR(2,block,nil);
      Int_Object *obj=new Int_Object(int_class,0,i);
      env.GetStack().PeekTop()->SetEntry(1,obj);
      Block_Object::evaluateOn(env);
      env.PopAR();
      if(env.AreWeReturning()) break;
    }
  
  return nil;
}



Object *Int_Object::timesDo(RunTimeEnvironment &env)
{
  // 10 timesDo: [ ... ]
  
  Int_Object *self=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  Block_Object *block=DYNAMIC_CAST_PTR(Block_Object,env.GetParameter(1));
  if(!block)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Integer::timesDo: requires a block");
  
  for(int i=1 ; i<=self->Value ; i++)
    {
      env.GetStack().PushAR(1,block,nil);
      Block_Object::evaluate(env);
      env.PopAR();
      if(env.AreWeReturning()) break;
    }
  
  return nil;
}



Object *Int_Object::greaterThan(RunTimeEnvironment &env)
{
  // 6 > 4
  
  Int_Object *lhs=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  Int_Object *rhs=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Integer::> applied to illegal object");
  return lhs->Value > rhs->Value ? true_object : false_object;
}



Object *Int_Object::lessThan(RunTimeEnvironment &env)
{
  // 6 < 4
  
  Int_Object *lhs=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  Int_Object *rhs=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Integer::< applied to illegal object");
  return lhs->Value < rhs->Value ? true_object : false_object;
}



Object *Int_Object::greaterEqual(RunTimeEnvironment &env)
{
  // 6 >= 4
  
  Int_Object *lhs=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  Int_Object *rhs=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Integer::>= applied to illegal object");
  return lhs->Value >= rhs->Value ? true_object : false_object;
}



Object *Int_Object::lessEqual(RunTimeEnvironment &env)
{
  // 6 <= 4
  
  Int_Object *lhs=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  Int_Object *rhs=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Integer::<= applied to illegal object");
  return lhs->Value <= rhs->Value ? true_object : false_object;
}



Object *Int_Object::equal(RunTimeEnvironment &env)
{
  // 6 equal: 4
  
  Int_Object *lhs=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  Int_Object *rhs=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Integer::equal: applied to illegal object");
  return lhs->Value == rhs->Value ? true_object : false_object;
}



Object *Int_Object::notEqual(RunTimeEnvironment &env)
{
  // 6 notEqual: 4
  
  Int_Object *lhs=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  Int_Object *rhs=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Integer::notEqual: applied to illegal object");
  return lhs->Value != rhs->Value ? true_object : false_object;
}



Object *Int_Object::asFloat(RunTimeEnvironment &env)
{
  // 6 asFloat
  
  Int_Object *self=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  return new Float_Object(float_class,0,(float)(self->Value));
}



Object *Int_Object::asString(RunTimeEnvironment &env)
{
  // (123 asString equal: "123") ifTrue: [...
  
  Int_Object *self=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  strstream ss;
  ss << self->Value << ends;
  String_Object *so=
    DYNAMIC_CAST_PTR(String_Object,string_class->Instantiate());
  char *p=ss.str();
  so->SetValue(StringObject(p));
  delete [] p;
  return so;
}



Object *Int_Object::asChar(RunTimeEnvironment &env)
{
  // (32 asChar equal: ' ') ifTrue: [...
  
  Int_Object *self=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  return new Char_Object(char_class,0,char(self->Value));
}



Object *Int_Object::shiftLeft(RunTimeEnvironment &env)
{
  // 6 << 2
  
  Int_Object *self=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  Int_Object *amt=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!amt)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Integer::<< requires an integer amount");
  self->Value <<= amt->Value;
  return self;
}



Object *Int_Object::shiftRight(RunTimeEnvironment &env)
{
  // 6 >> 2
  
  Int_Object *self=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  Int_Object *amt=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!amt)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Integer::>> requires an integer amount");
  self->Value >>= amt->Value;
  return self;
}



Object *Int_Object::bitOr(RunTimeEnvironment &env)
{
  // 1 || 3
  
  Int_Object *self=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  Int_Object *with=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!with)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Integer::|| requires an integer parameter");
  
  return new Int_Object(int_class,0,self->Value | with->Value);
}



Object *Int_Object::bitAnd(RunTimeEnvironment &env)
{
  // 1 && 2
  
  Int_Object *self=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  Int_Object *with=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!with)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Integer::&& requires an integer parameter");
  
  return new Int_Object(int_class,0,self->Value & with->Value);
}



Object *Int_Object::bitNot(RunTimeEnvironment &env)
{
  // 6 bitNot
  Int_Object *self=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  return new Int_Object(int_class,0,~self->Value);
}



Object *Int_Object::readFrom(RunTimeEnvironment &env)
{
  // x readFrom: cin
  
  Int_Object *lhs=DYNAMIC_CAST_PTR(Int_Object,env.GetSelf());
  IStream_Object *rhs=
    DYNAMIC_CAST_PTR(IStream_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Integer::readFrom: applied to illegal object");
  
  rhs->GetStream() >> lhs->Value;
  
  return lhs;
}



// ****************************************
//			 Float_Object methods
// ****************************************
Float_Object::Float_Object(Class *MyClass,int NumAttributes,float Value)
  : Object(MyClass,NumAttributes), Value(Value)
{
  // ctor
}



float Float_Object::GetValue() const
{
  return Value;
}



Object *Float_Object::hashValue(RunTimeEnvironment &env)
{
  // 3.14 hashValue
  
  Float_Object *self=DYNAMIC_CAST_PTR(Float_Object,env.GetSelf());
  return new Int_Object(int_class,0,(int)(self->Value));
}



Object *Float_Object::asInt(RunTimeEnvironment &env)
{
  Float_Object *self=DYNAMIC_CAST_PTR(Float_Object,env.GetSelf());
  return new Int_Object(int_class,0,(int)(self->Value));
}



Object *Float_Object::asString(RunTimeEnvironment &env)
{
  // (3.14 asString equal: "3.14") ifTrue: [...
  
  Float_Object *self=DYNAMIC_CAST_PTR(Float_Object,env.GetSelf());
  strstream ss;
  ss << self->Value << ends;
  String_Object *so=
    DYNAMIC_CAST_PTR(String_Object,string_class->Instantiate());
  char *p=ss.str();
  so->SetValue(StringObject(p));
  delete [] p;
  return so;
}




Object *Float_Object::plus(RunTimeEnvironment &env)
{
  // 6.5 + 4.5
  
  Float_Object *lhs=DYNAMIC_CAST_PTR(Float_Object,env.GetSelf());
  Float_Object *rhs=DYNAMIC_CAST_PTR(Float_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Float::+ applied to illegal object");
  return new Float_Object(float_class,0,lhs->Value+rhs->Value);
}



Object *Float_Object::minus(RunTimeEnvironment &env)
{
  // 6.0 - 4.0
  
  Float_Object *lhs=DYNAMIC_CAST_PTR(Float_Object,env.GetSelf());
  Float_Object *rhs=DYNAMIC_CAST_PTR(Float_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Float::- applied to illegal object");
  return new Float_Object(float_class,0,lhs->Value-rhs->Value);
}



Object *Float_Object::times(RunTimeEnvironment &env)
{
  // 6.0 * 4.0
  
  Float_Object *lhs=DYNAMIC_CAST_PTR(Float_Object,env.GetSelf());
  Float_Object *rhs=DYNAMIC_CAST_PTR(Float_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Float::* applied to illegal object");
  return new Float_Object(float_class,0,lhs->Value*rhs->Value);
}



Object *Float_Object::dividedBy(RunTimeEnvironment &env)
{
  // 6.0 / 4.0
  
  Float_Object *lhs=DYNAMIC_CAST_PTR(Float_Object,env.GetSelf());
  Float_Object *rhs=DYNAMIC_CAST_PTR(Float_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Float::/ applied to illegal object");
  if(rhs->Value==0)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Division by zero in Float::/");
  return new Float_Object(float_class,0,lhs->Value/rhs->Value);
}



Object *Float_Object::misc_math_func(RunTimeEnvironment &env,
				     double (*f)(double),
				     float MinVal,
				     float MaxVal,
				     char *FuncName)
{
  Float_Object *self=DYNAMIC_CAST_PTR(Float_Object,env.GetSelf());
  if(MinVal<MaxVal)
    if(self->Value < MinVal || self->Value > MaxVal)
      {
	ostrstream os;
	os << "Math error: " << FuncName << " of " <<
	  self->Value << ends;
	throw RUN_TIME_ERROR(__FILE__,__LINE__,os.str());
      }
  return new Float_Object(float_class,0,(*f)(self->Value));
}



Object *Float_Object::sin(RunTimeEnvironment &env)
{
  return misc_math_func(env,::sin); // ###
}



Object *Float_Object::acos(RunTimeEnvironment &env)
{
  return misc_math_func(env,::acos,-1,1,"acos"); // ###
}



Object *Float_Object::asin(RunTimeEnvironment &env)
{
  return misc_math_func(env,::asin,-1,1,"asin"); // ###
}



Object *Float_Object::atan(RunTimeEnvironment &env)
{
  return misc_math_func(env,::atan); // ###
}



Object *Float_Object::cos(RunTimeEnvironment &env)
{
  return misc_math_func(env,::cos); // ###
}



Object *Float_Object::tan(RunTimeEnvironment &env)
{
  return misc_math_func(env,::tan); // ###
}



Object *Float_Object::sqrt(RunTimeEnvironment &env)
{
  return misc_math_func(env,::sqrt); // ###
}



Object *Float_Object::displayOn(RunTimeEnvironment &env)
{
  // 6 displayOn: cout
  
  Float_Object *lhs=DYNAMIC_CAST_PTR(Float_Object,env.GetSelf());
  OStream_Object *rhs=DYNAMIC_CAST_PTR(OStream_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Float::displayOn: applied to illegal object");
  rhs->GetStream() << lhs->Value;
  
  return rhs;
}



Object *Float_Object::greaterThan(RunTimeEnvironment &env)
{
  // 6 > 4
  
  Float_Object *lhs=DYNAMIC_CAST_PTR(Float_Object,env.GetSelf());
  Float_Object *rhs=DYNAMIC_CAST_PTR(Float_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Float::> applied to illegal object");
  return lhs->Value > rhs->Value ? true_object : false_object;
}



Object *Float_Object::lessThan(RunTimeEnvironment &env)
{
  // 6 < 4
  
  Float_Object *lhs=DYNAMIC_CAST_PTR(Float_Object,env.GetSelf());
  Float_Object *rhs=DYNAMIC_CAST_PTR(Float_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Float::< applied to illegal object");
  return lhs->Value < rhs->Value ? true_object : false_object;
}



Object *Float_Object::greaterEqual(RunTimeEnvironment &env)
{
  // 6 >= 4
  
  Float_Object *lhs=DYNAMIC_CAST_PTR(Float_Object,env.GetSelf());
  Float_Object *rhs=DYNAMIC_CAST_PTR(Float_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Float::>= applied to illegal object");
  return lhs->Value >= rhs->Value ? true_object : false_object;
}



Object *Float_Object::lessEqual(RunTimeEnvironment &env)
{
  // 6 <= 4
  
  Float_Object *lhs=DYNAMIC_CAST_PTR(Float_Object,env.GetSelf());
  Float_Object *rhs=DYNAMIC_CAST_PTR(Float_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Float::<= applied to illegal object");
  return lhs->Value <= rhs->Value ? true_object : false_object;
}



Object *Float_Object::equal(RunTimeEnvironment &env)
{
  // 6 equal: 4
  
  Float_Object *lhs=DYNAMIC_CAST_PTR(Float_Object,env.GetSelf());
  Float_Object *rhs=DYNAMIC_CAST_PTR(Float_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Float::equal: applied to illegal object");
  return lhs->Value == rhs->Value ? true_object : false_object;
}



Object *Float_Object::notEqual(RunTimeEnvironment &env)
{
  // 6 notEqual: 4
  
  Float_Object *lhs=DYNAMIC_CAST_PTR(Float_Object,env.GetSelf());
  Float_Object *rhs=DYNAMIC_CAST_PTR(Float_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Float::notEqual: applied to illegal object");
  return lhs->Value != rhs->Value ? true_object : false_object;
}




// ****************************************
//	    String_Object methods
// ****************************************
String_Object::String_Object(Class *MyClass,int NumAttributes,
			     const char *iValue)
  : Object(MyClass,NumAttributes), Value(iValue)
{
  // ctor
}



Object *String_Object::hashValue(RunTimeEnvironment &env)
{
  // "hello, world" hashValue
  
  String_Object *self=DYNAMIC_CAST_PTR(String_Object,env.GetSelf());
  char *p=self->Value.GetCopy();
  int hash_value=hashpjw(p);
  delete [] p;
  
  return new Int_Object(int_class,0,hash_value);
}




Object *String_Object::displayOn(RunTimeEnvironment &env)
{
  // "hello, world" displayOn: consoleOut;
  
  String_Object *self=DYNAMIC_CAST_PTR(String_Object,env.GetSelf());
  OStream_Object *os=DYNAMIC_CAST_PTR(OStream_Object,env.GetParameter(1));
  if(!os)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "String::displayOn: applied to illegal object");
  
  char *p=self->Value.GetCopy();
  os->GetStream() << p;
  delete [] p;
  
  return os;
}



Object *String_Object::plus(RunTimeEnvironment &env)
{
  // "hello, " + "world"
  
  String_Object *lhs=DYNAMIC_CAST_PTR(String_Object,env.GetSelf());
  String_Object *rhs=DYNAMIC_CAST_PTR(String_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Operand of String::+ must be a string");
  
  StringObject so;
  so.CopyFrom(&lhs->Value);
  so.Append(&rhs->Value);
  
  String_Object *RetVal=new String_Object(string_class,0);
  RetVal->SetValue(so);
  return RetVal;
}



int String_Object::StrCmp(RunTimeEnvironment &env)
{
  // This method performs a strcmp on self and the first parameter
  
  String_Object *lhs=DYNAMIC_CAST_PTR(String_Object,env.GetSelf());
  String_Object *rhs=DYNAMIC_CAST_PTR(String_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Operand of string comparison must be a string");
  
  char *cp_lhs=lhs->Value.GetCopy();
  char *cp_rhs=rhs->Value.GetCopy();
  
  int cmp=strcmp(cp_lhs,cp_rhs);
  
  delete [] cp_lhs;
  delete [] cp_rhs;
  
  return cmp;
}




Object *String_Object::lessThan(RunTimeEnvironment &env)
{
  // (thisString lessThan: thatString) ifTrue: [...
  
  return StrCmp(env)<0 ? true_object : false_object;
}



Object *String_Object::greaterThan(RunTimeEnvironment &env)
{
  // (thisString greaterThan: thatString) ifTrue: [...
  
  return StrCmp(env)>0 ? true_object : false_object;
}



Object *String_Object::equal(RunTimeEnvironment &env)
{
  // (thisString equal: thatString) ifTrue: [...
  
  return StrCmp(env)==0 ? true_object : false_object;
}



Object *String_Object::length(RunTimeEnvironment &env)
{
  // "hello, world" length
  
  String_Object *self=DYNAMIC_CAST_PTR(String_Object,env.GetSelf());
  int str_len=self->Value.GetLength();
  return new Int_Object(int_class,0,str_len);
}



Object *String_Object::at(RunTimeEnvironment &env)
{
  // "hello, world" at: 3
  
  String_Object *self=DYNAMIC_CAST_PTR(String_Object,env.GetSelf());
  Int_Object *oIndex=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!oIndex)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "String::at: requires an integer index");
  
  int Index=oIndex->GetValue();
  if(Index<0 || Index>=self->Value.GetLength())
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "invalid index in String::at:");
  
  char *copy=self->Value.GetCopy();
  char RetVal=copy[Index];
  delete [] copy;
  
  return new Char_Object(char_class,0,RetVal);
}



Object *String_Object::atPut(RunTimeEnvironment &env)
{
  // "hello, Bill" at: 7 put: 'J'
  
  String_Object *self=DYNAMIC_CAST_PTR(String_Object,env.GetSelf());
  Int_Object *oIndex=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!oIndex)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "String::at:put: requires an integer index");
  int Index=oIndex->GetValue();
  if(Index<0 || Index>=self->Value.GetLength())
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "invalid index in String::at:put:");
  
  Char_Object *oPut=DYNAMIC_CAST_PTR(Char_Object,env.GetParameter(2));
  if(!oPut)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "String::at:put: requires a Char");
  
  self->Value[Index]=oPut->GetValue();
  
  return self;
}



Object *String_Object::asInt(RunTimeEnvironment &env)
{
  // "123" asInt
  
  String_Object *self=DYNAMIC_CAST_PTR(String_Object,env.GetSelf());
  char *p=self->Value.GetCopy();
  int val=atoi(p);
  delete [] p;
  return new Int_Object(int_class,0,val);
}



Object *String_Object::asFloat(RunTimeEnvironment &env)
{
  // "3.14" asFloat
  
  String_Object *self=DYNAMIC_CAST_PTR(String_Object,env.GetSelf());
  char *p=self->Value.GetCopy();
  float val=atof(p);
  delete [] p;
  return new Float_Object(float_class,0,val);
}



Object *String_Object::readFrom(RunTimeEnvironment &env)
{
  // new String readFrom: consoleIn;
  
  String_Object *self=DYNAMIC_CAST_PTR(String_Object,env.GetSelf());
  IStream_Object *is=DYNAMIC_CAST_PTR(IStream_Object,env.GetParameter(1));
  if(!is)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "String::readFrom: applied to illegal object");
  
  char Buffer[256];
  is->GetStream().getline(Buffer,255);
  self->Value.CopyFrom(Buffer);
  
  return is;
}



Object *String_Object::beginEnd(RunTimeEnvironment &env)
{
  // ("Bobcat" begin: 3 end: 5) displayOn: consoleOut
  
  String_Object *self=DYNAMIC_CAST_PTR(String_Object,env.GetSelf());
  Int_Object *from=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  Int_Object *to=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(2));
  if(!from || !to)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Parameter to String::begin:end: must be an integer");
  int iFrom=from->GetValue();
  int iTo=to->GetValue();
  if(iTo<iFrom || iFrom<0)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Attempt to access an invalid substring");
  int SubStringLength=iTo-iFrom;
  char *SubString=new char[SubStringLength+1];
  SubString[SubStringLength]='\0';
  char *p=self->Value.GetCopy();
  for(int i=iFrom ; i<=iTo ; i++)
    SubString[i-iFrom]=p[i];
  delete [] p;
  String_Object *so=
    DYNAMIC_CAST_PTR(String_Object,string_class->Instantiate());
  so->SetValue(StringObject(SubString));
  delete [] SubString;
  return so;
}



// ****************************************
//	    Char_Object methods
// ****************************************
Char_Object::Char_Object(Class *MyClass,int NumAttributes,char Value)
  : Object(MyClass,NumAttributes), Value(Value)
{
  // ctor
}



void Char_Object::SetValue(char v)
{
  Value=v;
}



char Char_Object::GetValue() const
{
  return Value;
}



Object *Char_Object::displayOn(RunTimeEnvironment &env)
{
  // 'a' displayOn: consoleOut;
  
  Char_Object *self=DYNAMIC_CAST_PTR(Char_Object,env.GetSelf());
  OStream_Object *os=DYNAMIC_CAST_PTR(OStream_Object,env.GetParameter(1));
  if(!os)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Char::displayOn: applied to illegal object");
  
  os->GetStream() << self->Value;
  
  return os;
}



Object *Char_Object::hashValue(RunTimeEnvironment &env)
{
  // 'c' hashValue
  
  Char_Object *self=DYNAMIC_CAST_PTR(Char_Object,env.GetSelf());
  return new Int_Object(int_class,0,(int)(self->Value));
}



Object *Char_Object::readFrom(RunTimeEnvironment &env)
{
  // new Char readFrom: consoleIn;
  
  Char_Object *self=DYNAMIC_CAST_PTR(Char_Object,env.GetSelf());
  IStream_Object *is=DYNAMIC_CAST_PTR(IStream_Object,env.GetParameter(1));
  if(!is)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Char::readFrom: applied to illegal object");
  
  is->GetStream().get(self->Value);
  
  return is;
}



Object *Char_Object::lessThan(RunTimeEnvironment &env)
{
  // (thisChar < thatChar) ifTrue: [...
  
  Char_Object *self=DYNAMIC_CAST_PTR(Char_Object,env.GetSelf());
  Char_Object *parm=DYNAMIC_CAST_PTR(Char_Object,env.GetParameter(1));
  if(!parm)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Char::< requires a Char parameter");
  
  return self->Value < parm->Value ? true_object : false_object;
}



Object *Char_Object::greaterThan(RunTimeEnvironment &env)
{
  // (thisChar > thatChar) ifTrue: [...
  
  Char_Object *self=DYNAMIC_CAST_PTR(Char_Object,env.GetSelf());
  Char_Object *parm=DYNAMIC_CAST_PTR(Char_Object,env.GetParameter(1));
  if(!parm)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Char::> requires a Char parameter");
  
  return self->Value > parm->Value ? true_object : false_object;
}



Object *Char_Object::equal(RunTimeEnvironment &env)
{
  // (thisChar equal: thatChar) ifTrue: [...
  
  Char_Object *self=DYNAMIC_CAST_PTR(Char_Object,env.GetSelf());
  Char_Object *parm=DYNAMIC_CAST_PTR(Char_Object,env.GetParameter(1));
  if(!parm)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "Char::equal: requires a Char parameter");
  
  return self->Value == parm->Value ? true_object : false_object;
}



Object *Char_Object::asString(RunTimeEnvironment &env)
{
  // someString plus: (thisChar asString)
  
  Char_Object *self=DYNAMIC_CAST_PTR(Char_Object,env.GetSelf());
  String_Object *so=
    DYNAMIC_CAST_PTR(String_Object,string_class->Instantiate());
  
  char Buffer[2];
  Buffer[0]=self->Value;
  Buffer[1]='\0';
  
  so->SetValue(StringObject(Buffer));
  return so;
}



Object *Char_Object::ascii(RunTimeEnvironment &env)
{
  // 'c' ascii
  
  Char_Object *self=DYNAMIC_CAST_PTR(Char_Object,env.GetSelf());
  return new Int_Object(int_class,0,int(self->Value));
}



// ****************************************
//	   IStream_Object methods
// ****************************************
IStream_Object::IStream_Object(Class *MyClass,int NumAttributes)
  : Object(MyClass,NumAttributes)
{
  // ctor
}



Object *IStream_Object::getch(RunTimeEnvironment &env)
{
  // cin getch
  
  int i=getchar();
  char c=(char)i;
  
  return new Char_Object(char_class,0,c);
}                                      



// ****************************************
//	    OStream_Object methods
// ****************************************
OStream_Object::OStream_Object(Class *MyClass,int NumAttributes)
  : Object(MyClass,NumAttributes)
{
  // ctor
}



Object *OStream_Object::nl(RunTimeEnvironment &env)
{
  OStream_Object *self=DYNAMIC_CAST_PTR(OStream_Object,env.GetSelf());
  self->GetStream() << endl;
  
  return self;
}



// ****************************************
//	   IFStream_Object methods
// ****************************************
IFStream_Object::IFStream_Object(Class *MyClass,int NumAttributes)
  : IStream_Object(MyClass,NumAttributes)
{
  // ctor
}



Object *IFStream_Object::open(RunTimeEnvironment &env)
{
  // new IFStream open: "somefile.dat";
  
  IFStream_Object *self=DYNAMIC_CAST_PTR(IFStream_Object,env.GetSelf());
  String_Object *so_fname=DYNAMIC_CAST_PTR(String_Object,env.GetParameter(1));
  if(!so_fname)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "ifstream::open: requires a String");
  char *fname=so_fname->GetValue().GetCopy();
  self->Value.open(fname,ios::in);
  delete [] fname;
  
  return self;
}



Object *IFStream_Object::close(RunTimeEnvironment &env)
{
  // some_stream close;
  
  IFStream_Object *self=DYNAMIC_CAST_PTR(IFStream_Object,env.GetSelf());
  self->Value.close();
  
  return self;
}



Object *IFStream_Object::eof(RunTimeEnvironment &env)
{
  // some_stream eof ifTrue: [...
  
  IFStream_Object *self=DYNAMIC_CAST_PTR(IFStream_Object,env.GetSelf());
  return self->Value.eof() ? true_object : false_object;
}



Object *IFStream_Object::bad(RunTimeEnvironment &env)
{
  // some_stream bad ifTrue: [...
  
  IFStream_Object *self=DYNAMIC_CAST_PTR(IFStream_Object,env.GetSelf());
  return self->Value.bad() ? true_object : false_object;
}



Object *IFStream_Object::position(RunTimeEnvironment &env)
{
  // some_stream position: 0
  
  IFStream_Object *self=DYNAMIC_CAST_PTR(IFStream_Object,env.GetSelf());
  Int_Object *pos=DYNAMIC_CAST_PTR(Int_Object,env.GetParameter(1));
  if(!pos)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "ifstream::position: requires an integer");
  self->Value.seekg(pos->GetValue(),ios::beg);
  
  return self;
}



Object *IFStream_Object::putBack(RunTimeEnvironment &env)
{
  // aStream putBack: 'c'
  
  IFStream_Object *self=DYNAMIC_CAST_PTR(IFStream_Object,env.GetSelf());
  Char_Object *c=DYNAMIC_CAST_PTR(Char_Object,env.GetParameter(1));
  if(!c)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "ifstream::putback: requires an character");
  self->Value.putback(c->GetValue());
  
  return self;
}



Object *IFStream_Object::peek(RunTimeEnvironment &env)
{
  // 32 equal: (aStream peek)
  
  IFStream_Object *self=DYNAMIC_CAST_PTR(IFStream_Object,env.GetSelf());
  return new Int_Object(int_class,0,self->Value.peek());
}



istream &IFStream_Object::GetStream()
{
  return Value;
}



// ****************************************
//	   OFStream_Object methods
// ****************************************
OFStream_Object::OFStream_Object(Class *MyClass,int NumAttributes) :
  OStream_Object(MyClass,NumAttributes)
{
  // ctor
}



Object *OFStream_Object::open(RunTimeEnvironment &env)
{
  // new OFStream open: "somefile.dat"
  
  OFStream_Object *self=DYNAMIC_CAST_PTR(OFStream_Object,env.GetSelf());
  String_Object *so_fname=DYNAMIC_CAST_PTR(String_Object,env.GetParameter(1));
  if(!so_fname)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "ofstream::open: requires a String");
  char *fname=so_fname->GetValue().GetCopy();
  self->Value.open(fname,ios::out);
  delete [] fname;
  
  return self;
}



Object *OFStream_Object::close(RunTimeEnvironment &env)
{
  // some_stream close
  
  OFStream_Object *self=DYNAMIC_CAST_PTR(OFStream_Object,env.GetSelf());
  self->Value.close();
  
  return self;
}



Object *OFStream_Object::bad(RunTimeEnvironment &env)
{
  // some_stream bad ifTrue: [...
  
  OFStream_Object *self=DYNAMIC_CAST_PTR(OFStream_Object,env.GetSelf());
  return self->Value.bad() ? true_object : false_object;
}



ostream &OFStream_Object::GetStream()
{
  return Value;
}




// ****************************************
//	      True_Object methods
// ****************************************
True_Object::True_Object(Class *MyClass,int NumAttributes)
  : Object(MyClass,NumAttributes)
{
  // ctor
}



Object *True_Object::displayOn(RunTimeEnvironment &env)
{
  // true displayOn: cout
  
  OStream_Object *rhs=DYNAMIC_CAST_PTR(OStream_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "True::displayOn: applied to illegal object");
  rhs->GetStream() << "true";
  
  return rhs;
}



Object *True_Object::ifTrue(RunTimeEnvironment &env)
{
  // true ifTrue: [...
  
  Block_Object *block=DYNAMIC_CAST_PTR(Block_Object,env.GetParameter(1));
  if(!block)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "flow-of-control construct requires a block");
  env.GetStack().PushAR(1,block,nil);
  Block_Object::evaluate(env);
  env.PopAR();
  
  return nil;
}



Object *True_Object::ifTrueElse(RunTimeEnvironment &env)
{
  // true ifTrue: [...] else: [...]
  
  return ifTrue(env);
}




Object *True_Object::ifFalse(RunTimeEnvironment &env)
{
  // true ifFalse: [...
  
  // do nothing
  
  return nil;
}



Object *True_Object::ifFalseElse(RunTimeEnvironment &env)
{
  // true ifFalse: [...] else: [...]
  
  Block_Object *block=DYNAMIC_CAST_PTR(Block_Object,env.GetParameter(2));
  if(!block)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "flow-of-control construct requires a block");
  env.GetStack().PushAR(1,block,nil);
  Block_Object::evaluate(env);
  env.PopAR();
  
  return nil;
}




Object *True_Object::and(RunTimeEnvironment &env)
{
  // true && true
  
  Object *rhs=env.GetParameter(1);
  if(rhs->GetClass()==true_class)
    return true_object;
  if(rhs->GetClass()==false_class)
    return false_object;
  throw RUN_TIME_ERROR(__FILE__,__LINE__,
		       "Parameter #2 of && must be boolean");
}



Object *True_Object::or(RunTimeEnvironment &env)
{
  // true || true
  
  Object *rhs=env.GetParameter(1);
  if(rhs->GetClass()==true_class || rhs->GetClass()==false_class)
    return true_object;
  throw RUN_TIME_ERROR(__FILE__,__LINE__,
		       "Parameter #2 of || must be boolean");
}



Object *True_Object::not(RunTimeEnvironment &env)
{
  // true not
  
  return false_object;
}



// ****************************************
//	   False_Object methods
// ****************************************
False_Object::False_Object(Class *MyClass,int NumAttributes)
  : Object(MyClass,NumAttributes)
{
  // ctor
}



Object *False_Object::displayOn(RunTimeEnvironment &env)
{
  // true displayOn: consoleOut
  
  OStream_Object *rhs=
    DYNAMIC_CAST_PTR(OStream_Object,env.GetParameter(1));
  if(!rhs)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "False::displayOn: applied to illegal object");
  rhs->GetStream() << "false";
  
  return rhs;
}



Object *False_Object::ifTrue(RunTimeEnvironment &env)
{
  // false ifTrue: [...
  
  // do nothing
  
  return nil;
}



Object *False_Object::ifTrueElse(RunTimeEnvironment &env)
{
  // false ifTrue: [...] else: [...]
  
  Block_Object *block=DYNAMIC_CAST_PTR(Block_Object,env.GetParameter(2));
  if(!block)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "flow-of-control construct requires a block");
  env.GetStack().PushAR(1,block,nil);
  Block_Object::evaluate(env);
  env.PopAR();
  
  return nil;
}




Object *False_Object::ifFalse(RunTimeEnvironment &env)
{
  // false ifFalse: [...
  
  Block_Object *block=DYNAMIC_CAST_PTR(Block_Object,env.GetParameter(1));
  if(!block)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
			 "flow-of-control construct requires a block");
  env.GetStack().PushAR(1,block,nil);
  Block_Object::evaluate(env);
  env.PopAR();
  
  return nil;
}



Object *False_Object::ifFalseElse(RunTimeEnvironment &env)
{
  // false ifFalse: [...] else: [...]
  
  return ifFalse(env);
}



Object *False_Object::and(RunTimeEnvironment &env)
{
  // false && false
  
  Object *rhs=env.GetParameter(1);
  if(rhs->GetClass()==true_class || rhs->GetClass()==false_class)
    return false_object;
  throw RUN_TIME_ERROR(__FILE__,__LINE__,
		       "Parameter #2 of && must be boolean");
}



Object *False_Object::or(RunTimeEnvironment &env)
{
  // false || false
  
  Object *rhs=env.GetParameter(1);
  if(rhs->GetClass()==true_class)
    return true_object;
  if(rhs->GetClass()==false_class)
    return false_object;
  throw RUN_TIME_ERROR(__FILE__,__LINE__,
		       "Parameter #2 of || must be boolean");
}



Object *False_Object::not(RunTimeEnvironment &env)
{
  // false not
  
  return true_object;
}



