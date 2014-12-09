
// testcomp.C

#include "testcomp.H"
#include "interpreter.H"
#include <iostream.h>
#include <fstream.h>
#include <strstream.h>
#include "execute.H"

RTTI_DEFINE_SUBCLASS(thingamadooberClass,Class)
RTTI_DEFINE_SUBCLASS(thingamadooberObject,Object)


Class *thingamajig, *thingamadoober;


void go();

void main(int argc,char *argv[])
{
  try
    {
      go();
    }
  catch(const LEXICAL_ERROR &le)
    {
      cout << "Lexical error on line " << le.GetEpsilonLineNum() <<
	" in file \"" << "\"" << endl;
    }
  catch(const READ_ERROR &)
    {
      cout << "Read error" << endl;
    }
  catch(const SYNTAX_ERROR &se)
    {
      cout << "Syntax error on line " << se.GetEpsilonLineNum() <<
	" in file \"" << "\"" << endl;
      if(se.GetDetails())
	cout << se.GetDetails() << endl;
      else cout << endl;
    }
  catch(const SEMANTIC_ERROR &se)
    {
      cout << "Semantic error on line " << se.GetEpsilonLineNum() <<
	" in file \"" << "\"" << endl;
      if(se.GetDetails())
	cout << se.GetDetails() << endl;
      else cout << endl;
    }
  catch(const STACK_UNDERFLOW &)
    {
      cout << "Stack underflow" << endl;
    }
  catch(const INTERNAL_ERROR &ie)
    {
      cout << "Internal error on line " << ie.GetLineNum() <<
	" in " << ie.GetFilename() << ": " << ie.GetReason() << endl;
    }
  catch(const RUN_TIME_ERROR &rte)
    {
      cout << "Run-time error: ";
      if(rte.GetDetails())
	cout << rte.GetDetails() << endl;
      else cout << "<no details available>" << endl;
    }
  catch(const EPSILON_EXCEPTION &e)
    {
      cout << "general exception caught in main()" << endl;
      if(e.GetFilename())
	cout << "file=" << e.GetFilename() << " line=" <<
	  e.GetLineNum() << endl;
      if(e.GetReason())
	cout << " reason=" << e.GetReason() << endl;
    }
  catch(const FILE_EXCEPTION &e)
    {
      cout << "file exception caught in main()" << endl;
      if(e.GetFilename())
	cout << "file=" << e.GetFilename() << " line=" <<
	  e.GetLineNum() << endl;
      if(e.GetReason())
	cout << "reason=" << e.GetReason() << endl;
    }
  catch(...)
    {
      cout << "Exception caught in main()" << endl;
    }
}



Object *hickup(RunTimeEnvironment &env)
{
  cout << "hickup!" << endl;
}



void registerBuiltInClasses(EpsilonInterpreter &interpreter)
{
  thingamajig=interpreter.createClass("thingamajig");
  interpreter.addInstanceMethod(thingamajig,"print:",
    "method thingamajig::print: p { cout << '[' << p << ']' }");
  interpreter.addInstanceMethod(thingamajig,"hickup",hickup);

  // ----------------------------------------------------------

  thingamadoober=new thingamadooberClass();
  interpreter.registerClass(thingamadoober);
  interpreter.addInstanceMethod(thingamadoober,"init:",
			&thingamadooberObject::init);
  interpreter.addInstanceMethod(thingamadoober,"hasMoreTokens",
			&thingamadooberObject::hasMoreTokens);
  interpreter.addInstanceMethod(thingamadoober,"nextToken",
			&thingamadooberObject::nextToken);
}



void registerBuiltInObjects(EpsilonInterpreter &interpreter)
{
  thingamadooberObject *hoojabob=
    new thingamadooberObject(thingamadoober,0);

  int la=interpreter.declareGlobalObject("hoojabob");
  interpreter.allocateGlobalSpace();
  interpreter.storeGlobalObject(hoojabob,la);
}



void go()
{
  EpsilonInterpreter interpreter;

  registerBuiltInClasses(interpreter);
  registerBuiltInObjects(interpreter);

  ifstream is("/home/bmajoros/Epsilon/testfile.eps");
  interpreter.parseCode(is);

  if(!interpreter.doesMainExist())
    {
      cout << "You have not defined main()!" << endl;
      return;
    }

  interpreter.executeMain();
}



thingamadooberClass::thingamadooberClass()
  : Class("thingamadoober",root_class)
{
}



Object *thingamadooberClass::Instantiate(int SubclassAttributes)
{
  return new thingamadooberObject(this,SubclassAttributes);
}



thingamadooberObject::thingamadooberObject(Class *c,int subclassAttributes)
  : Object(c,subclassAttributes), tokenizer(NULL)
{
  // CTOR
}



Object *thingamadooberObject::init(RunTimeEnvironment &env)
{
  // thingamadoober::init: src

  // Get self
  thingamadooberObject *self=
    DYNAMIC_CAST_PTR(thingamadooberObject,env.GetSelf());

  // Get parameter
  String_Object *src=
    DYNAMIC_CAST_PTR(String_Object,env.GetParameter(1));
  if(!src)
    throw RUN_TIME_ERROR(__FILE__,__LINE__,
      "thingamadoober::init: requires string parameter");

  // Perform operation
  self->init(src->GetValue());

  // Return result
  return self;
}



void thingamadooberObject::init(const StringObject &src)
{
  delete tokenizer;
  tokenizer=new StringTokenizer(src.AsCharArray(),"");
}



Object *thingamadooberObject::hasMoreTokens(RunTimeEnvironment &env)
{
  // thingamadoober::hasMoreTokens

  // Get self
  thingamadooberObject *self=
    DYNAMIC_CAST_PTR(thingamadooberObject,env.GetSelf());

  // Perform operation
  if(!self->tokenizer) return nil;
  bool hasMore=self->tokenizer->hasMoreTokens();

  // Return result
  return hasMore ? true_object : false_object;
}



Object *thingamadooberObject::nextToken(RunTimeEnvironment &env)
{
  // thingamadoober::nextToken

  // Get self
  thingamadooberObject *self=
    DYNAMIC_CAST_PTR(thingamadooberObject,env.GetSelf());

  // Perform operation
  if(!self->tokenizer) return nil;
  const char *nextTok=self->tokenizer->nextToken();

  // Return result
  return new String_Object(string_class,0,nextTok);
}
