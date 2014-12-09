
// interpreter.C

#pragma implementation "array.C"


#include "interpreter.H"
#include "libsrc/Random.H"
#include "scanner.H"
#include "parser.H"
#include "except.H"
#include "libsrc/linked2.H"
#include <fstream.h>
#include <strstream.h>
#include "class.H"
#include "ast.H"
#include "tempmgr.H"
#include "execute.H"
#include <new.h>
#include <stdlib.h>
#include "cmdline.H"
#include "libsrc/array.C"

ElasticArray<bool> *gcc_is_dum_1;



bool EpsilonInterpreter::doesMainExist()
{
  return parser.doesMainExist();
}



EpsilonInterpreter::EpsilonInterpreter()
{
  // ctor

  randomize();
  declareBuiltInTypes();
  declareBuiltInObjs();
}



Class *EpsilonInterpreter::createClass(const StringObject &name)
{
  Class *newClass=new Class(name.AsCharArray());
  registerClass(newClass);
  return newClass;
}



void EpsilonInterpreter::executeMain()
{
  // Get the AST for main
  MethodBody *main_body=parser.GetMain();
  
  // Call main
  env.GetStack().PushAR(main_body->GetARsize(),NULL,nil);
  main_body->Call(env);
  env.PopAR();
}



void EpsilonInterpreter::parseCode(istream &is)
{
  parser.ParseStream(is);
}



void EpsilonInterpreter::registerClass(Class *c)
{
  // This method creates a metaclass for the new class
  // and then makes the class publicly available in the
  // global scope

  // Create metaclass
  ostrstream os;
  os << "meta~" << c->GetName() << ends;
  c->setMetaClass(os.str());
  os.rdbuf()->freeze(0);

  // Add to symbol table
  c->declare(c->GetName(),parser.getScopeStack());
}



void EpsilonInterpreter::addAttribute(Class *c,const StringObject &name)
{
  c->AddAttribute(name.AsCharArray());
}



void EpsilonInterpreter::addInstanceMethod(Class *c,
					   const StringObject &name,
					   CppMethod body)
{
  c->addInstanceMethod(name.AsCharArray(),body);
}



void EpsilonInterpreter::addInstanceMethod(Class *c,
					   const StringObject &name,
					   const StringObject &body)
{
  c->addInstanceMethod(name.AsCharArray());
  strstream ss;
  ss << body.AsCharArray() << ends;
  parser.ParseMethod(ss);
}



int EpsilonInterpreter::declareGlobalObject(const StringObject &name)
{
  const int lexPos=parser.nextGlobalLexPos();

  ObjectNameNode *onn=
    new ObjectNameNode(name.AsCharArray(),OBJ_GLOBAL,lexPos);
  parser.getScopeStack().Insert(onn,name.AsCharArray());

  return lexPos;
}



void EpsilonInterpreter::declareBuiltInObjs()
{
  lpCin=declareGlobalObject("cin");
  lpCout=declareGlobalObject("cout");
  lpTrue=declareGlobalObject("true");
  lpFalse=declareGlobalObject("false");
  lpNil=declareGlobalObject("nil");
  lpEndl=declareGlobalObject("endl");
}



void EpsilonInterpreter::storeBuiltInObjs()
{
  true_object=new True_Object(true_class);
  false_object=new False_Object(false_class);
  nil=nil_class->Instantiate();

  storeGlobalObject(istream_class->Instantiate(),lpCin);
  storeGlobalObject(ostream_class->Instantiate(),lpCout);
  storeGlobalObject(true_object,lpTrue);
  storeGlobalObject(false_object,lpFalse);
  storeGlobalObject(nil,lpNil);
  storeGlobalObject(new Char_Object(char_class,0,'\n'),lpEndl);
}



void EpsilonInterpreter::allocateGlobalSpace()
{
  env.GetReadyToRun(parser.getGlobalARSize());
  storeBuiltInObjs();
}



void EpsilonInterpreter::storeGlobalObject(Object *obj,int lexicalPosition)
{
  env.StoreObject(LexicalAddress(0,lexicalPosition),
		  OBJ_LOCAL,
		  obj);
}



void EpsilonInterpreter::declare(Class *c,const StringObject &name)
{
  c->declare(name.AsCharArray(),parser.getScopeStack());
}




void EpsilonInterpreter::undeclare(Class *c,const StringObject &name)
{
  c->undeclare(name.AsCharArray(),parser.getScopeStack());
}




void EpsilonInterpreter::declareBuiltInTypes()
{
  // Meta --------------------------------------------------------
  Meta=new Class("Meta");
  addInstanceMethod(Meta,"new",&Class_Object::New);
  addInstanceMethod(Meta,"superClass",&Class_Object::superClass);
  addInstanceMethod(Meta,"subClasses",&Class_Object::subClasses);
  addInstanceMethod(Meta,"error:",&Object::error);
  addInstanceMethod(Meta,"name",&Class_Object::name);
  addInstanceMethod(Meta,"isNil",&Object::isNil);
  
  // Root --------------------------------------------------------
  root_class=new Class("Root");
  root_class->setMetaClass(Meta);
  addInstanceMethod(root_class,"isNil",
		    &Object::isNil);
  addInstanceMethod(root_class,"classOf",
		    &Object::classOf);
  addInstanceMethod(root_class,"error:",
		    &Object::error);
  declare(root_class,"Root");

  // Integer ----------------------------------------------------
  int_class=new Int_Class(root_class);
  int_class->setMetaClass(Meta);
  addInstanceMethod(int_class,"+",&Int_Object::plus);
  addInstanceMethod(int_class,"-",&Int_Object::minus);
  addInstanceMethod(int_class,"*",&Int_Object::times);
  addInstanceMethod(int_class,"/",&Int_Object::dividedBy);
  addInstanceMethod(int_class,"displayOn:",&Int_Object::displayOn);
  addInstanceMethod(int_class,"readFrom:",&Int_Object::readFrom);
  addInstanceMethod(int_class,"%",&Int_Object::modulus);
  addInstanceMethod(int_class,">",&Int_Object::greaterThan);
  addInstanceMethod(int_class,"<",&Int_Object::lessThan);
  addInstanceMethod(int_class,">=",&Int_Object::greaterEqual);
  addInstanceMethod(int_class,"<=",&Int_Object::lessEqual);
  addInstanceMethod(int_class,"equal:",&Int_Object::equal);
  addInstanceMethod(int_class,"notEqual:",&Int_Object::notEqual);
  addInstanceMethod(int_class,"asFloat",&Int_Object::asFloat);
  addInstanceMethod(int_class,"asString",&Int_Object::asString);
  addInstanceMethod(int_class,"asChar",&Int_Object::asChar);
  addInstanceMethod(int_class,"upTo:do:",&Int_Object::upTo);
  addInstanceMethod(int_class,"downTo:do:",&Int_Object::downTo);
  addInstanceMethod(int_class,"hashValue",&Int_Object::hashValue);
  addInstanceMethod(int_class,"random",&Int_Object::random);
  addInstanceMethod(int_class,"<<",&Int_Object::shiftLeft);
  addInstanceMethod(int_class,">>",&Int_Object::shiftRight);
  addInstanceMethod(int_class,"||",&Int_Object::bitOr);
  addInstanceMethod(int_class,"&&",&Int_Object::bitAnd);
  addInstanceMethod(int_class,"bitNot",&Int_Object::bitNot);
  addInstanceMethod(int_class,"timesDo:",&Int_Object::timesDo);
  declare(int_class,"Integer");
  
  // Float -----------------------------------------------------
  float_class=new Float_Class(root_class);
  float_class->setMetaClass(Meta);
  addInstanceMethod(float_class,"+",&Float_Object::plus);
  addInstanceMethod(float_class,"-",&Float_Object::minus);
  addInstanceMethod(float_class,"*",&Float_Object::times);
  addInstanceMethod(float_class,"/",&Float_Object::dividedBy);
  addInstanceMethod(float_class,"displayOn:",&Float_Object::displayOn);
  addInstanceMethod(float_class,">",&Float_Object::greaterThan);
  addInstanceMethod(float_class,"<",&Float_Object::lessThan);
  addInstanceMethod(float_class,">=",&Float_Object::greaterEqual);
  addInstanceMethod(float_class,"<=",&Float_Object::lessEqual);
  addInstanceMethod(float_class,"equal:",&Float_Object::equal);
  addInstanceMethod(float_class,"notEqual:",&Float_Object::notEqual);
  addInstanceMethod(float_class,"asInt",&Float_Object::asInt);
  addInstanceMethod(float_class,"asString",&Float_Object::asString);
  addInstanceMethod(float_class,"hashValue",&Float_Object::hashValue);
  addInstanceMethod(float_class,"sin",&Float_Object::sin);
  addInstanceMethod(float_class,"acos",&Float_Object::acos);
  addInstanceMethod(float_class,"asin",&Float_Object::asin);
  addInstanceMethod(float_class,"atan",&Float_Object::atan);
  addInstanceMethod(float_class,"cos",&Float_Object::cos);
  addInstanceMethod(float_class,"tan",&Float_Object::tan);
  addInstanceMethod(float_class,"sqrt",&Float_Object::sqrt);
  declare(float_class,"Float");
  
  // True (unnamed class) ------------------------------------------
  true_class=new Class("True",root_class);
  true_class->setMetaClass(Meta);
  addInstanceMethod(true_class,"ifTrue:",&True_Object::ifTrue);
  addInstanceMethod(true_class,"ifTrue:else:",&True_Object::ifTrueElse);
  addInstanceMethod(true_class,"ifFalse:",&True_Object::ifFalse);
  addInstanceMethod(true_class,"ifFalse:else:",
				&True_Object::ifFalseElse);
  addInstanceMethod(true_class,"&&",&True_Object::and);
  addInstanceMethod(true_class,"||",&True_Object::or);
  addInstanceMethod(true_class,"not",&True_Object::not);
  addInstanceMethod(true_class,"displayOn:",&True_Object::displayOn);
  
  // False (unnamed class) ----------------------------------------
  false_class=new Class("False",root_class);
  false_class->setMetaClass(Meta);
  addInstanceMethod(false_class,"ifTrue:",&False_Object::ifTrue);
  addInstanceMethod(false_class,"ifTrue:else:",
				 &False_Object::ifTrueElse);
  addInstanceMethod(false_class,"ifFalse:",&False_Object::ifFalse);
  addInstanceMethod(false_class,"ifFalse:else:",
				 &False_Object::ifFalseElse);
  addInstanceMethod(false_class,"&&",&False_Object::and);
  addInstanceMethod(false_class,"||",&False_Object::or);
  addInstanceMethod(false_class,"not",&False_Object::not);
  addInstanceMethod(false_class,"displayOn:",&False_Object::displayOn);
  
  // Array -------------------------------------------------------
  array_class=new Array_Class(root_class);
  array_class->setMetaClass(Meta);
  declare(array_class,"Array");
  addInstanceMethod(array_class,"setSize:",&Array_Object::setSize);
  addInstanceMethod(array_class,"getSize",&Array_Object::getSize);
  addInstanceMethod(array_class,"at:",&Array_Object::at);
  addInstanceMethod(array_class,"at:put:",&Array_Object::atPut);
  addInstanceMethod(array_class,"copyFrom:",&Array_Object::copyFrom);
  addInstanceMethod(array_class,"do:",
		    "method Array::do: b"
		    "{  0 upTo: self getSize-1 do:"
		    "   [:i | b evaluateOn: (self at: i)]"
		    "}"
		    );
  addInstanceMethod(array_class,"displayOn:",
		    "method Array::displayOn: s"
		    "{ "
		    "   s<<\"Array [ \"."
		    "   self do:"
		    "      [:e | s<<e<<' ' ]."
		    "   ^s<<']'"
		    "}"
		    );

  // String --------------------------------------------------------
  string_class=new String_Class(root_class);
  string_class->setMetaClass(Meta);
  declare(string_class,"String");
  addInstanceMethod(string_class,"+",&String_Object::plus);
  addInstanceMethod(string_class,"displayOn:",
		    &String_Object::displayOn);
  addInstanceMethod(string_class,"readFrom:",&String_Object::readFrom);
  addInstanceMethod(string_class,"<",&String_Object::lessThan);
  addInstanceMethod(string_class,">",&String_Object::greaterThan);
  addInstanceMethod(string_class,"equal:",&String_Object::equal);
  addInstanceMethod(string_class,"length",&String_Object::length);
  addInstanceMethod(string_class,"at:",&String_Object::at);
  addInstanceMethod(string_class,"asInt",&String_Object::asInt);
  addInstanceMethod(string_class,"asFloat",&String_Object::asFloat);
  addInstanceMethod(string_class,"at:put:",&String_Object::atPut);
  addInstanceMethod(string_class,"begin:end:",&String_Object::beginEnd);
  addInstanceMethod(string_class,"hashValue",&String_Object::hashValue);
  addInstanceMethod(string_class,"do:",
		    "method String::do: b"
		    "{  0 upTo: self length - 1 do:"
		    "      [:i | b evaluateOn: (self at: i)]"
		    "}"
		    );

  // Char --------------------------------------------------------
  char_class=new Char_Class(root_class);
  char_class->setMetaClass(Meta);
  declare(char_class,"Char");
  addInstanceMethod(char_class,"displayOn:",&Char_Object::displayOn);
  addInstanceMethod(char_class,"readFrom:",&Char_Object::readFrom);
  addInstanceMethod(char_class,"<",&Char_Object::lessThan);
  addInstanceMethod(char_class,">",&Char_Object::greaterThan);
  addInstanceMethod(char_class,"equal:",&Char_Object::equal);
  addInstanceMethod(char_class,"asString",&Char_Object::asString);
  addInstanceMethod(char_class,"ascii",&Char_Object::ascii);
  addInstanceMethod(char_class,"hashValue",&Char_Object::hashValue);

  // Block (unnamed class) ----------------------------------------
  block_class=new Class("Block",root_class);
  block_class->setMetaClass(Meta);
  addInstanceMethod(block_class,"evaluate",&Block_Object::evaluate);
  addInstanceMethod(block_class,"evaluateOn:",
				 &Block_Object::evaluateOn);
  addInstanceMethod(block_class,"evaluateOn:and:",
				 &Block_Object::evaluateOnAnd);
  addInstanceMethod(block_class,"evaluateOn:and:and:",
				 &Block_Object::evaluateOnAndAnd);
  addInstanceMethod(block_class,"whileTrue:",
				 &Block_Object::whileTrue);
  addInstanceMethod(block_class,"whileFalse:",
				 &Block_Object::whileFalse);
  addInstanceMethod(block_class,"until:",&Block_Object::until);
  
  // Nil (unnamed class) --------------------------------------------
  nil_class=new Nil_Class(root_class);
  nil_class->setMetaClass(Meta);
  addInstanceMethod(nil_class,"isNil",&Nil_Object::isNil);
  addInstanceMethod(nil_class,"displayOn:",&Nil_Object::displayOn);
  addInstanceMethod(nil_class,"hashValue",&Nil_Object::hashValue);

  // IStream (unnamed class) ----------------------------------------
  istream_class=new IStream_Class(root_class);
  istream_class->setMetaClass(Meta);
  declare(istream_class,"istream");
  addInstanceMethod(istream_class,"getch",&IStream_Object::getch);
  addInstanceMethod(istream_class,">>",
		    "method istream::>> x"
		    "{"
		    "   x readFrom: self"
		    "}"
		    );
  undeclare(istream_class,"istream");

  // OStream (unnamed class) ----------------------------------------
  ostream_class=new OStream_Class(root_class);
  ostream_class->setMetaClass(Meta);
  declare(ostream_class,"ostream");
  addInstanceMethod(ostream_class,"nl",&OStream_Object::nl);
  addInstanceMethod(ostream_class,"<<",
		    "method ostream::<< x"
		    "{"
		    "   x displayOn: self"
		    "}"
		    );

  undeclare(ostream_class,"ostream");

  // IFStream ------------------------------------------------------
  ifstream_class=new IFStream_Class(root_class);
  ifstream_class->setMetaClass(Meta);
  declare(ifstream_class,"ifstream");
  addInstanceMethod(ifstream_class,"open:",&IFStream_Object::open);
  addInstanceMethod(ifstream_class,"close",&IFStream_Object::close);
  addInstanceMethod(ifstream_class,"eof",&IFStream_Object::eof);
  addInstanceMethod(ifstream_class,"bad",&IFStream_Object::bad);
  addInstanceMethod(ifstream_class,"position:",
		    &IFStream_Object::position);
  addInstanceMethod(ifstream_class,"putBack:",
		    &IFStream_Object::putBack);
  addInstanceMethod(ifstream_class,"peek",&IFStream_Object::peek);
  addInstanceMethod(ifstream_class,">>",
		    "method ifstream::>> x"
		    "{"
		    "  x readFrom: self"
		    "}"
		    );

  // OFStream ------------------------------------------------------
  ofstream_class=new OFStream_Class(root_class);
  ofstream_class->setMetaClass(Meta);
  declare(ofstream_class,"ofstream");
  addInstanceMethod(ofstream_class,"nl",&OStream_Object::nl);
  addInstanceMethod(ofstream_class,"open:",&OFStream_Object::open);
  addInstanceMethod(ofstream_class,"close",&OFStream_Object::close);
  addInstanceMethod(ofstream_class,"bad",&OFStream_Object::bad);
  addInstanceMethod(ofstream_class,"<<",
		    "method ofstream::<< x"
		    "{"
		    "   x displayOn: self"
		    "}"
		    );

  // StringTokenizer -----------------------------------------------
  strtok_class=new StrTok_Class(root_class);
  strtok_class->setMetaClass(Meta);
  declare(strtok_class,"StringTokenizer");
  addInstanceMethod(strtok_class,"initSource:",
		    &StrTok_Object::initSource);
  addInstanceMethod(strtok_class,"initSource:Delimiters:",
		    &StrTok_Object::initSourceDelim);
  addInstanceMethod(strtok_class,"hasMoreTokens",
		    &StrTok_Object::hasMoreTokens);
  addInstanceMethod(strtok_class,"nextToken",
		    &StrTok_Object::nextToken);
}



