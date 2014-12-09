
// testcomp.H

#ifndef INCL_TESTCOMP_H
#define INCL_TESTCOMP_H

#include "class.H"
#include "object.H"
#include "/home/bmajoros/c++/libsrc/StringTokenizer.H"


class thingamadooberClass : public Class
{
public:
  RTTI_DECLARE_SUBCLASS(thingamadooberClass,link_node)
  thingamadooberClass();
  virtual Object *Instantiate(int SubclassAttributes);

  // class methods
};



class thingamadooberObject : public Object
{
  StringTokenizer *tokenizer;
  void init(const StringObject &src);
public:
  RTTI_DECLARE_SUBCLASS(thingamadooberObject,link_node)
  thingamadooberObject(Class *,int subclassAttributes);

  // instance methods
  static Object *init(RunTimeEnvironment &);
  static Object *hasMoreTokens(RunTimeEnvironment &);
  static Object *nextToken(RunTimeEnvironment &);
};


#endif
