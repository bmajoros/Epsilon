
// epsilon.C

#include "testcomp.H"
#include "interpreter.H"
#include <iostream.h>
#include <fstream.h>
#include <strstream.h>
#include "execute.H"
#include "cmdline.H"


void go(const StringObject &);


void main(int argc,char *argv[])
{
  try
    {
      // Process command line and get filename of source file
      CmdLine=new CommandLine(argc,argv);
      char Buffer[200];
      if(CmdLine->GetFilename())
	strcpy(Buffer,CmdLine->GetFilename());
      else
	{
	  cout << "Filename: ";
	  cin >> Buffer;
	}

      // Try to execute the source file
      go(Buffer);
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


void registerBuiltInClasses(EpsilonInterpreter &interpreter)
{
  // 1. Create & register your classes using 
  //       Class *c=interpreter.createClass("<name>");
  //       (automatically registered)
  //    or
  //       Class *c=new DerivedFromClass(...);
  //       (not automatically registered; you must do it)
  //       interpreter.registerClass(c);

  // 2. Add instance methods & class methods to your class:
  //       interpreter.addInstanceMethod(c,"<methodName>",
  //          &cPlusPlusFunction);
  //    or
  //       interpreter.addInstanceMethod(c,"<methodName>",
  //          "<epsilon source code>");
}


void registerBuiltInObjects(EpsilonInterpreter &interpreter)
{
  // Declare your global objects here using
  //    int lexAddr=interpreter.declareGlobalObject("<name>");
  const int laArgs=interpreter.declareGlobalObject("args");

  // Allow the interpreter to allocate space at the bottom of
  // the run-time-stack for all global objects
  interpreter.allocateGlobalSpace();

  // Store your global objects on the stack using
  //     interpreter.storeGlobalObject(<object>,lexAddr);

  const int argc=CmdLine->getProgArgc();
  Array_Object *objArgs=new Array_Object(array_class,0,argc);
  char **argv=CmdLine->getProgArgv();
  int i;
  for(i=0 ; i<argc ; ++i)
    {
      String_Object *arg=
	new String_Object(string_class,0,argv[i]);
      objArgs->SetArrayElement(i,arg);
    }
  interpreter.storeGlobalObject(objArgs,laArgs);
}



void go(const StringObject &filename)
{
  EpsilonInterpreter interpreter;

  registerBuiltInClasses(interpreter);
  registerBuiltInObjects(interpreter);

  ifstream is(filename.AsCharArray());
  if(!is.good())
    throw FILE_EXCEPTION(__FILE__,__LINE__,filename);

  interpreter.parseCode(is);

  if(!interpreter.doesMainExist())
    {
      cout << "You have not defined main()!" << endl;
      return;
    }

  interpreter.executeMain();
}



