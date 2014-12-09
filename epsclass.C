
// epsclass.C

#include "epsclass.H"
#include <strstream.h>



OBSOLETE





EpsilonClass::EpsilonClass(Class *c,const StringObject &name,
			   bool hasClassMembers,bool isRestricted,
			   Parser &parser)
  : myClass(c), hasClassMembers(hasClassMembers), parser(parser),
    isRestricted(isRestricted), name(name)
{
  // ctor

  // Create & attach a metaclass
  if(hasClassMembers)
    {
      ostrstream os;
      os << "meta~" << name << ends;
      myClass->setMetaClass(os.str());
      os.rdbuf()->freeze(0);
    }
  else
    myClass->setMetaClass(Meta);


  if(!isRestricted) declare();
}



void EpsilonClass::declare()
{
  myClass->declare(name.AsCharArray(),parser.getScopeStack());
}



void EpsilonClass::undeclare()
{
  myClass->undeclare(name.AsCharArray(),parser.getScopeStack());
}



void EpsilonClass::addInstanceMethod(const StringObject &name,CppMethod body,
				     int arity)
{
  myClass->addInstanceMethod(name.AsCharArray(),body,arity);
}



void EpsilonClass::addInstanceMethod(const StringObject &name,istream &body)
{
  if(isRestricted) declare();    

  myClass->addInstanceMethod(name.AsCharArray());
  parser.ParseMethod(body);

  if(isRestricted) undeclare();
}



void EpsilonClass::addClassMethod(const StringObject &name,CppMethod body,
				  int arity)
{
  myClass->addClassMethod(name.AsCharArray(),body,arity);
}



void EpsilonClass::addClassMethod(const StringObject &name,istream &body)
{
  if(isRestricted) declare();    

  myClass->addClassMethod(name.AsCharArray());
  parser.ParseMethod(body);

  if(isRestricted) undeclare();
}



void EpsilonClass::addAttribute(const StringObject &name)
{
  myClass->AddAttribute(name.AsCharArray());
}
