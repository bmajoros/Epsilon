// =======================================
// Class.cpp
//
// Representations of Epsilon classes
//
//
// =======================================

#include "libsrc/typeinfo.H"
#include "class.H"
#include "object.H"
#include "parser.H"


RTTI_DEFINE_SUBCLASS(Class,link_node)
RTTI_DEFINE_SUBCLASS(Array_Class,Class)
RTTI_DEFINE_SUBCLASS(Nil_Class,Class)
RTTI_DEFINE_SUBCLASS(Int_Class,Class)
RTTI_DEFINE_SUBCLASS(Float_Class,Class)
RTTI_DEFINE_SUBCLASS(String_Class,Class)
RTTI_DEFINE_SUBCLASS(Char_Class,Class)
RTTI_DEFINE_SUBCLASS(IStream_Class,Class)
RTTI_DEFINE_SUBCLASS(OStream_Class,Class)
RTTI_DEFINE_SUBCLASS(IFStream_Class,Class)
RTTI_DEFINE_SUBCLASS(OFStream_Class,Class)
RTTI_DEFINE_SUBCLASS(StrTok_Class,Class)


// ******************* globals *******************
Int_Class *int_class=0;
Float_Class *float_class=0;
Char_Class *char_class=0;
String_Class *string_class=0;
Class *true_class=0, *false_class=0;
Class *block_class=0;
IStream_Class *istream_class=0;
OStream_Class *ostream_class=0;
IFStream_Class *ifstream_class=0;
OFStream_Class *ofstream_class=0;
Nil_Class *nil_class=0;
Array_Class *array_class=0;
Class *Meta=0, *root_class=0;
StrTok_Class *strtok_class=0;


// ****************************************
//	       Class methods
// ****************************************

int Class::detectArity(const char *methodName)
{
  int arity=0;

  // Detect operators (which are all binary)
  if(methodName[0] && !isalpha(methodName[0])) return 2;

  // Count colins in keyword selectors
  const char *p=methodName;
  while(*p!='\0')
    {
      if(*p==':')
	++arity;
      ++p;
    }

  return arity;
}


void Class::setMetaClass(const char *name)
{
  Class *metaClass=new Class(name,Meta);
  SetRepresentative(new Class_Object(metaClass,this));  
}



void Class::setMetaClass(Class *metaClass)
{
  SetRepresentative(new Class_Object(metaClass,this));  
}



void Class::addInstanceMethod(const char *name,CppMethod m,int arity)
{
  if(arity==ARITY_DETECT)
    arity=detectArity(name);
  AddMethod(name,m,arity+1);
}



void Class::addInstanceMethod(const char *name)
{
  AddMethod(name);
}



void Class::addClassMethod(const char *name,CppMethod m,int arity)
{
  if(arity==ARITY_DETECT)
    arity=detectArity(name);
  GetMetaclass()->AddMethod(name,m,arity+1);
}



void Class::addClassMethod(const char *name)
{
  GetMetaclass()->AddMethod(name);
}



void Class::declare(const char *name,EpsilonScopeStack &scopeStack)
{
  ClassNameNode *cnn=new ClassNameNode(name,this);
  scopeStack.Insert(cnn,name);
}



void Class::undeclare(const char *name,EpsilonScopeStack &scopeStack)
{
  scopeStack.Delete(name);
}



Class_Object *Class::AsFirstClassObject() const
{
  return Representative;
}



Class *Class::GetMetaclass() const
{
  return Representative->GetClass();
}



void Class::SetRepresentative(Class_Object *rep)
{
  Representative=rep;
}



int Class::TotalAttributes() const
{
  // Returns the total number of attributes in an instance of
  // this class, including the attributes of all base classes
  
  if(SuperClass)
    return SuperClass->TotalAttributes()+NumAttributes;
  else
    return NumAttributes;
}



Class::Class(const char *Name,Class *SuperClass) : SuperClass(SuperClass),
  Attributes(new ClassSymbolTable), Methods(new ClassSymbolTable),
  NumAttributes(0), Name(Name), Representative(0)
{
  // ctor
  
  // Register self as subclass of my SuperClass
  if(SuperClass) SuperClass->GetSubclasses().list_insert(this);
}



const char *Class::GetName() const
{
  return Name;
}



Class::~Class()
{
  // dtor
  
  delete Attributes;
  delete Methods;
}



void Class::AddAttribute(const char *Name)
{
  int LexicalPosition=TotalAttributes();
  ObjectNameNode *onn=new ObjectNameNode(Name,OBJ_ATTRIBUTE,
					 LexicalPosition);
  Attributes->Insert(onn,Name);
  ++NumAttributes;
}



ObjectNameNode *Class::FindAttribute(const char *Name)
{
  // See if I have an attribute with this name
  ObjectNameNode *onn=
    DYNAMIC_CAST_PTR(ObjectNameNode,Attributes->Find(Name));
  
  // If not, and I have a superclass, then ask my superclass
  if(!onn && SuperClass)
    onn=SuperClass->FindAttribute(Name);
  
  return onn;
}



MethodNameNode *Class::AddMethod(const char *Name)
{
  MethodNameNode *mnn=new MethodNameNode(Name);
  Methods->Insert(mnn,Name);
  return mnn;
}



MethodNameNode *Class::AddMethod(const char *Name,CppMethod f,int AR_size)
{
  MethodNameNode *mnn=AddMethod(Name);
  mnn->SetBody(new CppBody(f,AR_size));
  return mnn;
}



MethodNameNode *Class::AddMethod(const char *Name,SyntaxForest *sf,
				 int AR_size)
{
  MethodNameNode *mnn=AddMethod(Name);
  mnn->SetBody(new EpsilonBody(sf,AR_size));
  return mnn;
}



MethodNameNode *Class::FindMethod(const char *Name)
{
  // See if I have a method with that name
  MethodNameNode *mnn=
    DYNAMIC_CAST_PTR(MethodNameNode,Methods->Find(Name));
  
  // If I don't, then maybe my superclass does (if I have one)
  if(!mnn && SuperClass)
    mnn=SuperClass->FindMethod(Name);
  
  return mnn;
}



Object *Class::Instantiate(int SubclassAttributes)
{
  // This is a request from one of my subclasses, asking
  // me to create an instance of myself, with enough
  // storage for my attributes _plus_ my subclass'
  // attributes.  We want the root classes to be the
  // ones to instantiate themselves, because things like
  // Integer and String need to have special data members
  // to hold their values; anything derived from these
  // classes must also have those special data members.
  // So if I have a superclass, I will forward the message
  // on to my super class, passing as an argument the total
  // amount of extra storage needed for me and my subclasses.
  
  int TotalStorageNeeded=NumAttributes+SubclassAttributes;
  if(SuperClass)
    {
      Object *obj=SuperClass->Instantiate(TotalStorageNeeded);
      obj->SetClass(this);
      return obj;
    }
  else
    return new Object(this,TotalStorageNeeded);
}



// ****************************************
//	    StrTok_Class methods
// ****************************************
Object *StrTok_Class::Instantiate(int SubclassAttributes)
{
  return new StrTok_Object(this,SubclassAttributes);
}




// ****************************************
//	     Array_Class methods
// ****************************************
Object *Array_Class::Instantiate(int SubclassAttributes)
{
  return new Array_Object(this,SubclassAttributes);
}



// ****************************************
//	      Nil_Class methods
// ****************************************
Object *Nil_Class::Instantiate(int SubclassAttributes)
{
  return new Nil_Object(this,SubclassAttributes);
}



// ****************************************
//	     Int_Class methods
// ****************************************
Object *Int_Class::Instantiate(int SubclassAttributes)
{
  return new Int_Object(this,SubclassAttributes);
}



// ****************************************
//	    Float_Class methods
// ****************************************
Object *Float_Class::Instantiate(int SubclassAttributes)
{
  return new Float_Object(this,SubclassAttributes);
}



// ****************************************
//	    String_Class methods
// ****************************************
Object *String_Class::Instantiate(int SubclassAttributes)
{
  return new String_Object(this,SubclassAttributes);
}



// ****************************************
//	    Char_Class methods
// ****************************************
Object *Char_Class::Instantiate(int SubclassAttributes)
{
  return new Char_Object(this,SubclassAttributes);
}



// ****************************************
//	   IStream_Class methods
// ****************************************
Object *IStream_Class::Instantiate(int SubclassAttributes)
{
  return new IStream_Object(this,SubclassAttributes);
}



// ****************************************
//	   OStream_Class methods
// ****************************************
Object *OStream_Class::Instantiate(int SubclassAttributes)
{
  return new OStream_Object(this,SubclassAttributes);
}



// ****************************************
//	   IFStream_Class methods
// ****************************************
Object *IFStream_Class::Instantiate(int SubclassAttributes)
{
  return new IFStream_Object(this,SubclassAttributes);
}



// ****************************************
//	   OFStream_Class methods
// ****************************************
Object *OFStream_Class::Instantiate(int SubclassAttributes)
{
  return new OFStream_Object(this,SubclassAttributes);
}

