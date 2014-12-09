// ======================================================
// RTTI.H
//
// The macros in this file provide a method of simulating
// RTTI (Run-Time Type Information) when your compiler
// does not support RTTI directly.
//
// Programmer     Date       Change
// ----------     ----       ------
// B. Majoros     12/18/96   Creation
// B. Majoros     1/14/97    Use virtual method to access
//                           classDescriptor
// ======================================================

#ifndef INCL_RTTI_H
#define INCL_RTTI_H

//#include "/home/bmajoros/c++/libsrc/bool.H"
#include "Exception.H"


/* ---------------------------------------------------------
                     INTRODUCTION

  RTTI (Run-Time Type Information) has been officially added 
  to the C++ language, but it is not yet available in all
  implementations of the language.  RTTI provides, among
  other things, the ability to use the dynamic_cast<>()
  construct so that the validity of a cast can be checked
  at run time.  dynamic_cast<Alpha*>(&beta) evaluates to
  a pointer to beta of type Alpha* iff beta is an object
  of type Alpha, or any type derived from Alpha.  Otherwise,
  the dynamic_cast evaluates to NULL.  The macros in this
  file provide a simple, transparent way to simulate RTTI
  in systems which do not provide it directly.  Be careful
  to use the macros correctly, or the RTTI may work 
  incorrectly, without giving any compile-errors!  

  Use 
         Alpha *alpha=DYNAMIC_CAST_PTR(Alpha,(Beta*) b) 
  instead of 
         Alpha *alpha=dynamic_cast<Alpha*>((Beta *) b).  

  Alternatively, for references, use:
         Alpha &alpha=DYNAMIC_CAST_REF(Alpha,(Beta&) b) 
  instead of 
         Alpha &alpha=dynamic_cast<Alpha&>((Beta &) b).  

  When you upgrade your compiler, you can redefine all these 
  macros so that they use the real RTTI implementation rather 
  than the simulated one provided here.  

  
  



              HOW TO USE THE RTTI MACROS

* NOTE: Pay particular attention to the declaration
        of Beta below; RTTI_DECLARE_SUBCLASS _must_
        have the name of the subclass and the _root_
        class, _not_ the superclass!

1. Your .H file:

  class Root {
  public:
    RTTI_DECLARE_ROOT(Root)
  };

  class Alpha : public Root {
  public:
    RTTI_DECLARE_SUBCLASS(Alpha,Root)
  };


  class Beta : public Alpha {
  public:
    RTTI_DECLARE_SUBCLASS(Beta,Root) // <-- use "Root" here!!!
  };

2. Your .C file:

  RTTI_DEFINE_ROOT(Root)
  RTTI_DEFINE_SUBCLASS(Alpha,Root)
  RTTI_DEFINE_SUBCLASS(Beta,Alpha)


                       HOW RTTI WORKS

  Each class has a static ClassDescriptor object, which
  takes up only the space of a single pointer.  The Class-
  Descriptor of a class actually has a pointer pointing to
  the ClassDescriptor of its superclass.  Multiple-inheritance
  is NOT supported.  The ClassDescriptor of the root class in
  your hierarchy has a NULL pointer (i.e., no superclass).

  Each object has a virtual method which returns a pointer to 
  the ClassDescriptor of that object's class.  

  DYNAMIC_CAST_PTR(Alpha,&beta) expands to Alpha::dynamic_cast(&beta), 
  which sends the DescendedFrom() message to beta's class' 
  ClassDescriptor, passing Alpha's ClassDescriptor as a parameter.  
  DescendedFrom() checks whether beta's class' ClassDescriptor is 
  equal to Alpha's ClassDescriptor.  If so, then the cast succeeds.  
  Otherwise, a recursive call is made to see if beta's class' superclass 
  is DescendedFrom Alpha's ClassDescriptor.  Thus, the message 
  propagates up the inheritance tree until either the root class is 
  reached (failure), or a match is found (success).

--------------------------------------------------------------
 */


// **************************************************
//                class ClassDescriptor
// **************************************************

// This class is used by the RTTI macros (below).  Don't
// use this class directly.

class ClassDescriptor {
  ClassDescriptor *superClass;
public:
  ClassDescriptor(ClassDescriptor *iSuperClass);
  bool DescendedFrom(ClassDescriptor *);
};




// **************************************************
//                   class BadCast
// **************************************************

class BadCast : public RootException {
public:
  BadCast(const char *filename,int lineNum);
  BadCast(const BadCast &);
};



// Don't use this directly
#define RTTI_CLASS_DESCRIPTOR \
   static ClassDescriptor classDescriptor; 

// Don't use this directly
#define RTTI_DYNAMIC_CAST(className,rootClass) \
   static className *dynamicCastPtr(rootClass *r) \
   { \
      if(r==NULL) return NULL; \
      return \
      	r->getDescriptor()->DescendedFrom(&className::classDescriptor) ? \
	   (className*)r : NULL; \
   } \
   static className &dynamicCastRef(rootClass *r,const char *filename, \
      int line) \
   { \
      if(r && r->getDescriptor()->DescendedFrom(&className::classDescriptor)) \
	return (className&) *r; \
      throw BadCast(filename,line); \
   } \
   virtual ClassDescriptor *getDescriptor() { return &classDescriptor; }

// Put one of these in the public section of your class (in .H file)
#define RTTI_DECLARE_SUBCLASS(className,rootClass) \
   RTTI_CLASS_DESCRIPTOR \
   RTTI_DYNAMIC_CAST(className,rootClass)
#define RTTI_DECLARE_ROOT(className) \
   RTTI_DECLARE_SUBCLASS(className,className);

// This is obsolete.  Provided for backward compatibility.
#define RTTI_CONSTRUCTOR(className) 

// Put one of these in the .C file
#define RTTI_DEFINE_SUBCLASS(subclass,superclass) \
   ClassDescriptor subclass::classDescriptor(&superclass::classDescriptor);
#define RTTI_DEFINE_ROOT(className) \
   ClassDescriptor className::classDescriptor(NULL);

// Use this instead of the new C++ construct, 
// dynamic_cast<className*>(&object) or dynamic_cast<className&>(object)
#define DYNAMIC_CAST_PTR(className,object) \
   className::dynamicCastPtr(object)
#define DYNAMIC_CAST_REF(className,object) \
   className::dynamicCastRef(&object,__FILE__,__LINE__)

#define STATIC_CAST(type,expr)         (type)(expr)
#define CONST_CAST(type,expr)          (type)(expr)
#define REINTERPRET_CAST(type,expr)    (type)(void*)(expr)

#endif
