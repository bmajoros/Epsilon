// =======================================
// Parser.cpp
//
// Parser class and parsing routines
//
//
// =======================================

#include "libsrc/typeinfo.H"
#include "parser.H"
#include <strstream.h>
#include "identlst.H"
#include "class.H"
#include <stdlib.h>
#include "tempmgr.H"
#include "astprint.H"
#include <fstream.h>
#include "object.H"
#include "cmdline.H"
#include "libsrc/safecopy.H"
#include <string.h>



// ****************************************
//	   NamedStream methods
// ****************************************
NamedStream::NamedStream(char *name,TokenStream *strm)
	: Name(name), TheStream(strm)
	{
	// ctor

	// Note: name is kept by this object but is _not_ deleted
	// when this object is deleted!
	}



NamedStream::~NamedStream()
	{
	// do not delete Name here, because it is taken over by a client
	// before this NamedStream is deleted
	}



char *NamedStream::GetName() const
	{
	return Name;
	}



TokenStream *NamedStream::GetStream() const
	{
	return TheStream;
	}



// ****************************************
//	 FilenameRecord methods
// ****************************************
FilenameRecord::FilenameRecord(const char *Filename)
	: Filename(Duplicate(Filename))
	{
	// ctor
	}



FilenameRecord::~FilenameRecord()
	{
	delete [] Filename;
	}



char *FilenameRecord::GetFilename() const
	{
	return Filename;
	}



bool FilenameRecord::Equals(const char *key)
	{
	return !strcasecmp(key,Filename);
	}



// ****************************************
//	     Parser methods
// ****************************************

void Parser::ParseMethod(istream &is)
{
  CurrentStream=new TokenStream(&is);
  CurrentStreamName=Duplicate("<internal>");
  
  pp_method_defn();
  
  delete CurrentStream;
  CurrentStream=0;
  delete [] CurrentStreamName;
  CurrentStreamName=0;
}



MethodBody *Parser::GetMain()
{
  return MainBody;
}



int Parser::nextGlobalLexPos()
{
  return nextGlobalLP++;
}



int Parser::getGlobalARSize()
{
  return nextGlobalLP;
}



Parser::Parser()
  : MainDefined(false), MainBody(new EpsilonBody), 
    IsPushedBack(false), nextGlobalLP(1)
{
  // Enter global definitions (such as built-in classes
  // and objects) into the global symbol table
  DefineGlobals();
}



void Parser::PushBack(Token &t)
{
  PushedBack=t;
  IsPushedBack=true;
}



void Parser::Match(TokenType t)
{
  // This function is called when TokenType t is
  // expected, but its lexeme is not needed.  This
  // routine throws a SYNTAX_ERROR exception if
  // the expected token is not next in the stream.
  
  Token Tok;
  Match(t,Tok);
}



void Parser::Match(TokenType t,Token &Tok)
{
  // This function is called when TokenType t is
  // expected.  This routine throws a SYNTAX_ERROR
  // exception if the expected token is not next in
  // the stream.  The matched token is copied into
  // Tok.
  
  if(IsPushedBack)
    {
      Tok=PushedBack;
      IsPushedBack=false;
    }
  else
    *CurrentStream >> Tok;
  
  if(Tok.GetTag()!=t)
    {
      ostrstream os;
      os << "Expecting " << TokenTypeLabel(t) << ", but found "
	 << TokenTypeLabel(Tok.GetTag()) << ends;
      throw SYNTAX_ERROR(__FILE__,__LINE__,Tok.GetLineNum(),os.str());
    }
}



TokenType Parser::Peek()
{
  if(IsPushedBack) return PushedBack.GetTag();
  
  Token Tok;
  
  *CurrentStream >> Tok;
  CurrentStream->PushBack(Tok);
  
  return Tok.GetTag();
}



char *Parser::GetCurrentFilename() const
{
  return CurrentStreamName;
}



bool Parser::doesMainExist() 
{ 
  return MainDefined; 
}




void Parser::ParseEntireProgram(const char *fname)
{
  ifstream is(fname);
  if(!is)
    throw FILE_EXCEPTION(__FILE__,__LINE__,fname);

  AlreadyIncluded.Insert(new FilenameRecord(fname),fname);  
  CurrentStreamName=Duplicate(fname);

  ParseStream(is);
}



void Parser::ParseStream(istream &is)
{
  CurrentStream=new TokenStream(&is);

  // Invoke the top-level parsing routine
  pp_program();
}



void Parser::DefineGlobals()
{
  // Push global symbol table (we are about to enter the
  // "global scope")
  TheScopeStack.EnterScope();
}




EpsilonScopeStack &Parser::getScopeStack()
{
  return TheScopeStack;
}





// 			  ######################
// 			  # PARSING PROCEDURES #
// 			  ######################

void Parser::pp_program()
{
  // program -> component program
  // program -> TOK_EOF
  
  // *** This routine parses a file and all files included by that file.
  //     It is recursively invoked on each included file.
  
  while(1)
    {
      switch(Peek())
	{
	case TOK_INCLUDE:
	case TOK_CLASS:
	case TOK_METHOD:
	case TOK_MAIN:
	  pp_component();
	  continue;
	}
      Match(TOK_EOF);
      break;
    }
}



void Parser::pp_component()
{
  // component -> include
  // component -> class_decl
  // component -> method_defn
  // component -> main_entry
  
  switch(Peek())
    {
    case TOK_INCLUDE:
      pp_include();
      break;
    case TOK_CLASS:
      pp_class_decl();
      break;
    case TOK_METHOD:
      pp_method_defn();
      break;
    case TOK_MAIN:
      pp_main_entry();
      break;
    }
}



void Parser::pp_include()
{
  // include -> TOK_INCLUDE TOK_STRING_LIT
  
  Match(TOK_INCLUDE);
  
  Token Filename;
  Match(TOK_STRING_LIT,Filename);
  
  // If this file has already been included, then do nothing
  if(AlreadyIncluded.Find(Filename.GetLexeme())) return;
  
  // Record the filename so that it won't be included again later
  AlreadyIncluded.Insert(new FilenameRecord(Filename.GetLexeme()),
			 Filename.GetLexeme());
  
  ifstream is(Filename.GetLexeme());
  if(is.bad())
    {
      strstream ss;
      ss << "Cannot open \"" << Filename.GetLexeme() << "\"" << ends;
      throw SYNTAX_ERROR(__FILE__,__LINE__,Filename.GetLineNum(),ss.str());
    }
  
  OldStreams.Push(new NamedStream(CurrentStreamName,CurrentStream));
  
  CurrentStream=new TokenStream(&is);
  CurrentStreamName=Duplicate(Filename.GetLexeme());
  
  pp_program();
  
  delete CurrentStream;
  delete [] CurrentStreamName;
  
  NamedStream *ns=OldStreams.Pop();
  CurrentStream=ns->GetStream();
  CurrentStreamName=ns->GetName();
  delete ns;
}



// *********************** CLASS DECLARATIONS ***********************

void Parser::pp_class_decl()
{
  // class_decl -> TOK_CLASS TOK_IDENT base_class TOK_OPEN_BRACE
  // 			     class_body TOK_CLOSE_BRACE
  
  
  Match(TOK_CLASS);
  
  // Get class name
  Token name;
  int LexicalDepth;
  Match(TOK_IDENT,name);
  
  // Get base-class
  Class *BaseClass=pp_base_class(); // defaults to Root if none specified
  
  // Create a Class to represent this new Epsilon class
  if(TheScopeStack.Find(name.GetLexeme(),LexicalDepth))
    throw SEMANTIC_ERROR(__FILE__,__LINE__,name.GetLineNum(),
			 "Class name redefined");
  Class *NewClass=new Class(Duplicate(name.GetLexeme()),BaseClass);
  TheScopeStack.Insert(new ClassNameNode(name.GetLexeme(),NewClass),
		       name.GetLexeme());
  
  // Create a metaclass for this class
  ostrstream os;
  os << "meta~" << name.GetLexeme() << ends; // for error reporting only
  Class *NewMetaclass=new Class(os.str(),BaseClass->GetMetaclass());
  
  // Parse the class body
  Match(TOK_OPEN_BRACE);
  pp_class_body(NewClass,NewMetaclass);
  Match(TOK_CLOSE_BRACE);
  
  // Create a Class_Object for this class, so it is a first-class object
  Class_Object *Representative=
    new Class_Object(NewMetaclass,NewClass,
		     NewMetaclass->TotalAttributes());
  
  // Inform the class about its representative
  NewClass->SetRepresentative(Representative);
}



Class *Parser::pp_base_class()
{
  // base_class -> TOK_COLON TOK_IDENT
  // base_class ->
  
  // Returns a pointer to a base Class, or NULL if none
  
  Class *cp=root_class;
  
  if(Peek()==TOK_COLON)
    {
      Match(TOK_COLON);
      
      Token tok;
      Match(TOK_IDENT,tok);
      int junk;
      SymbolNode *sn=
	TheScopeStack.Find(tok.GetLexeme(),junk);
      if(!sn)
	throw SEMANTIC_ERROR(__FILE__,__LINE__,tok.GetLineNum(),
			     "Undefined base class");
      ClassNameNode *cnn=DYNAMIC_CAST_PTR(ClassNameNode,sn);
      if(!cnn)
	throw SEMANTIC_ERROR(__FILE__,__LINE__,tok.GetLineNum(),
			     "Base type is not a class");
      cp=cnn->GetClass();
    }
  
  return cp;
}



void Parser::pp_class_body(Class *ThisClass,Class *NewMetaclass)
{
  // class_body -> member_decl more_class_body
  // class_body ->
  
  switch(Peek())
    {
    case TOK_ATTRIBUTE:
    case TOK_METHOD:
    case TOK_CLASS:
      pp_member_decl(ThisClass,NewMetaclass);
      pp_more_class_body(ThisClass,NewMetaclass);
    }
}



void Parser::pp_more_class_body(Class *ThisClass,Class *NewMetaclass)
{
  // more_class_body -> TOK_PERIOD member_decl more_class_body
  // more_class_body ->
  
  while(Peek()==TOK_PERIOD)
    {
      Match(TOK_PERIOD);
      pp_member_decl(ThisClass,NewMetaclass);
    }
}



void Parser::pp_member_decl(Class *ThisClass,Class *NewMetaclass)
{
  // member_decl -> attribute
  // member_decl -> method_decl
  // member_decl -> TOK_CLASS static_method_decl
  
  switch(Peek())
    {
    case TOK_ATTRIBUTE:
      pp_attribute(ThisClass);
      break;
    case TOK_METHOD:
      pp_method_decl(ThisClass);
      break;
    case TOK_CLASS:
      Match(TOK_CLASS);
      pp_static_member_decl(NewMetaclass);
      break;
    default:
      throw SYNTAX_ERROR(__FILE__,__LINE__,
			 CurrentStream->GetLineNum(),"Expecting declaration");
    }
}



void Parser::pp_static_member_decl(Class *NewMetaclass)
{
  // static_method_decl -> attribute
  // static_method_decl -> method_decl
  
  switch(Peek())
    {
    case TOK_ATTRIBUTE:
      pp_attribute(NewMetaclass);
      break;
    case TOK_METHOD:
      pp_method_decl(NewMetaclass);
      break;
    default:
      throw SYNTAX_ERROR(__FILE__,__LINE__,
			 CurrentStream->GetLineNum(),"Expecting declaration");
    }
}



void Parser::pp_attribute(Class *ThisClass)
{
  // attribute -> TOK_ATTRIBUTE ident_list
  
  linked_list *attr_list;
  
  Match(TOK_ATTRIBUTE);
  attr_list=pp_ident_list();
  
  // Add each of the identifiers in the list to the class'
  // attribute table
  attr_list->reset_seq();
  IdentifierNode *n;
  while(n=DYNAMIC_CAST_PTR(IdentifierNode,attr_list->sequential()))
    ThisClass->AddAttribute(n->GetIdent());
  
  delete attr_list;
}



linked_list *Parser::pp_ident_list()
{
  // ident_list -> TOK_IDENT more_comma_idents
  
  // Caller: please delete my return value
  
  linked_list *L;
  
  Token name;
  Match(TOK_IDENT,name);
  L=pp_more_comma_idents();
  
  L->list_insert(new IdentifierNode(name.GetLexeme()));
  
  return L;
}



linked_list *Parser::pp_more_comma_idents()
{
  // more_comma_idents -> TOK_COMMA TOK_IDENT more_comma_idents
  // more_comma_idents ->
  
  // Caller: please delete my return value
  
  linked_list *L=new linked_list;
  while(Peek()==TOK_COMMA)
    {
      Match(TOK_COMMA);
      Token name;
      Match(TOK_IDENT,name);
      L->list_append(new IdentifierNode(name.GetLexeme()));
    }
  return L;
}



void Parser::pp_method_decl(Class *ThisClass)
{
  // method_decl -> TOK_METHOD method_name
  
  char *name;
  
  Match(TOK_METHOD);
  name=pp_method_name(0);
  
  ThisClass->AddMethod(name);
  delete [] name;
}



char *Parser::pp_method_name(linked_list *parms)
{
  // method_name -> TOK_IDENT long_name
  // method_name -> TOK_BINOP TOK_IDENT
  
  // Caller: please delete my return value.
  // parms is a linked_list in which I will put the
  // parameter names for this method, unless parms is NULL.
  // The list will be in the same order the parms occur
  // in the source file.
  
  Token name_tok;
  
  switch(Peek())
    {
    case TOK_BINOP:
      {
	Match(TOK_BINOP,name_tok);
	Token parm_tok;
	Match(TOK_IDENT,parm_tok);
	if(parms)
	  parms->list_insert(new IdentifierNode(parm_tok.GetLexeme()));
	return Duplicate(name_tok.GetLexeme());
      }
    default: // this default makes sure syntax errors are caught
    case TOK_IDENT:
      {
	char *rest_of_name;
	
	Match(TOK_IDENT,name_tok);
	rest_of_name=pp_long_name(parms);
	
	ostrstream os;
	os << name_tok.GetLexeme() << rest_of_name << ends;
	delete [] rest_of_name;
	
	return os.str();
      }
    }
}



char *Parser::pp_long_name(linked_list *parms)
{
  // long_name -> TOK_COLON TOK_IDENT longer_name
  // long_name ->
  
  // Caller: please delete my return value
  // parms is a linked_list in which I will put the
  // parameter names for this method, unless parms is NULL.
  // The list will be in the same order the parms occur
  // in the source file.
  
  ostrstream os;
  
  if(Peek()==TOK_COLON)
    {
      os << ':';
      Match(TOK_COLON);
      Token parm;
      Match(TOK_IDENT,parm);
      if(parms)
	parms->list_append(new IdentifierNode(parm.GetLexeme()));
      char *ln;
      ln=pp_longer_name(parms);
      os << ln;
      delete [] ln;
    }
  
  os << ends;
  return os.str();
}



char *Parser::pp_longer_name(linked_list *parms)
{
  // longer_name -> TOK_IDENT TOK_COLON TOK_IDENT longer_name
  // longer_name ->
  
  // Caller: please delete my return value
  // parms is a linked_list in which I will put the
  // parameter names for this method, unless parms is NULL.
  // The list will be in the same order the parms occur
  // in the source file.
  
  ostrstream os;
  
  while(Peek()==TOK_IDENT)
    {
      Token name;
      Match(TOK_IDENT,name);
      os << name.GetLexeme() << ':';
      Match(TOK_COLON);
      Token parm;
      Match(TOK_IDENT,parm);
      if(parms)
	parms->list_append(new IdentifierNode(parm.GetLexeme()));
    }
  
  os << ends;
  return os.str();
}



// ******************** METHODS AND STATEMENTS *********************

void Parser::pp_method_defn()
{
  // method_defn -> TOK_METHOD TOK_IDENT TOK_DOUBLE_COLON
  // 			      method_name function_body
  
  Match(TOK_METHOD);
  
  TheScopeStack.EnterScope();
  
  // Read the class name and get its symbol-table record
  Token name;
  int LexicalDepth;
  Match(TOK_IDENT,name);
  SymbolNode *name_sn=TheScopeStack.Find(name.GetLexeme(),LexicalDepth);
  if(!name_sn)
    throw SEMANTIC_ERROR(__FILE__,__LINE__,name.GetLineNum(),
			 "Undefined class name");
  //if(typeid(*name_sn)!=typeid(ClassNameNode))
  if(DYNAMIC_CAST_PTR(ClassNameNode,name_sn)==NULL)
    throw SEMANTIC_ERROR(__FILE__,__LINE__,name.GetLineNum(),
			 "Non-class used for class name");
  ClassNameNode *cnn=DYNAMIC_CAST_PTR(ClassNameNode,name_sn);
  
  CurrentClass=cnn->GetClass(); // CurrentClass is a member of Parser
  
  Match(TOK_DOUBLE_COLON);
  linked_list parm_names;
  char *meth_name;
  meth_name=pp_method_name(&parm_names);
  MethodNameNode *mnn=CurrentClass->FindMethod(meth_name);
  if(!mnn)
    {
      // This method does not appear to belong to this class; however,
      // it may simply be that it is a CLASS METHOD rather than an
      // instance method
      
      mnn=CurrentClass->GetMetaclass()->FindMethod(meth_name);
      if(mnn)
	{
	  // We want pp_function_body() to look up attribute names
	  // in the metaclass' symbol table rather than the class'
	  // symbol table
	  CurrentClass=CurrentClass->GetMetaclass();
	}
      
      else
	throw SEMANTIC_ERROR(__FILE__,__LINE__,name.GetLineNum(),
			     "Method does not belong to class");
    }
  else
    {
      // This method was found in the class' symbol table, but we need
      // to make sure it isn't also in the metaclass' symbol table --
      // a method cannot be both an instance method and a class method
      // at the same time
      if(CurrentClass->GetMetaclass()->FindMethod(meth_name))
	throw SEMANTIC_ERROR(__FILE__,__LINE__,name.GetLineNum(),
	  "Method cannot be both a class method and an instance method");
    }
  
  // Enter parameter names into symbol table
  parm_names.reset_seq();
  IdentifierNode *in;
  while(in=DYNAMIC_CAST_PTR(IdentifierNode,parm_names.sequential()))
    {
      // Compute lexical position (we add 1, because AR[0] is
      // reserved for "self")
      int LexicalPosition=TheScopeStack.GetCurrentScopeSize()+1;
      
      ObjectNameNode *onn=
	new ObjectNameNode(in->GetIdent(),OBJ_LOCAL,LexicalPosition);
      TheScopeStack.Insert(onn,in->GetIdent());
    }
  
  // Parse the body and produce a SyntaxForest to represent it
  SyntaxForest *sf=pp_function_body();
  
  // Allocate temporaries to the nodes of the trees
  TemporaryAllocator TempMgr(TheScopeStack.GetCurrentScopeSize()+1);
  sf->ReceiveVisitor(&TempMgr);
  
  // Print out the syntax forest, for debugging purposes
  /*  if(CmdLine->AreWeDebugging())
    {
      AstPrinter printer(cout);
      sf->ReceiveVisitor(&printer);
      cout << "\npress enter...";
      cin.get();
      cout << endl;
    }*/
  
  // Compute size of activation record for this method based on the
  // number of temporaries and the number of local objects & parameters
  int AR_size=TempMgr.GetNumTemps()+TheScopeStack.GetCurrentScopeSize()+1;
  
  // Store the method body with its name in the symbol table
  mnn->SetBody(new EpsilonBody(sf,AR_size));
  
  TheScopeStack.LeaveScope();
}



void Parser::pp_main_entry()
{
  // main -> TOK_MAIN function_body
  
  Match(TOK_MAIN);
  TheScopeStack.EnterScope();
  
  if(MainDefined)
    throw SEMANTIC_ERROR(__FILE__,__LINE__,CurrentStream->GetLineNum(),
			 "Redefinition of main");
  MainDefined=true;
  
  CurrentClass=0; // this code does not belong to any class
  
  SyntaxForest *main_body=pp_function_body();
  MainBody->SetBody(main_body);
  
  // Allocate temporaries to the nodes of the trees
  TemporaryAllocator TempMgr(TheScopeStack.GetCurrentScopeSize()+1);
  main_body->ReceiveVisitor(&TempMgr);
  
  // Print out the syntax forest, for debugging purposes
  /*	if(CmdLine->AreWeDebugging())
	{
	AstPrinter printer(cout);
	cout << "\nMAIN:" << endl;
	main_body->ReceiveVisitor(&printer);
	cout << "\npress enter...";
	cin.get();
	cout << endl;
        }*/
  
  // Compute size of activation record for main based on the
  // number of temporaries and the number of local objects & parameters
  int AR_size=TempMgr.GetNumTemps()+TheScopeStack.GetCurrentScopeSize()+1;
  MainBody->SetARsize(AR_size);
  
  TheScopeStack.LeaveScope();
}



SyntaxForest *Parser::pp_function_body()
{
  // function_body -> TOK_OPEN_BRACE stmt_list TOK_CLOSE_BRACE
  
  SyntaxForest *sf=new SyntaxForest;
  
  CurrentDepth=0; // We are currently at lexical depth 0
  
  Match(TOK_OPEN_BRACE);
  pp_stmt_list(sf);
  Match(TOK_CLOSE_BRACE);
  
  return sf;
}



bool Parser::CanBeginStatement(TokenType t)
{
  // This method returns true iff the given token
  // type can be the first token in a statement
  
  switch(t)
    {
    case TOK_OBJECT:
    case TOK_BIND:
    case TOK_RETURN:
    case TOK_OPEN_PAREN:
    case TOK_IDENT:
    case TOK_SELF:
    case TOK_SUPER:
    case TOK_OPEN_BRACKET:
    case TOK_INT_LIT:
    case TOK_FLOAT_LIT:
    case TOK_CHAR_LIT:
    case TOK_STRING_LIT:
      return true;
    }
  return false;
}



void Parser::pp_stmt_list(SyntaxForest *sf)
{
  // stmt_list -> statement more_statements
  // stmt_list ->
  
  if(CanBeginStatement(Peek()))
    {
      AstStmt *s;
      s=pp_statement();
      if(s) sf->Append(s);
      pp_more_statements(sf);
    }
}



void Parser::pp_more_statements(SyntaxForest *sf)
{
  // more_statements -> TOK_PERIOD statement more_statements
  // more_statements ->
  
  while(Peek()==TOK_PERIOD)
    {
      Match(TOK_PERIOD);
      AstStmt *s;
      s=pp_statement();
      if(s) sf->Append(s);
    }
}



AstStmt *Parser::pp_statement()
{
  // statement -> local_decl
  // statement -> assignment
  // statement -> expression
  // statement -> return_stmt
  
  // Returns an AstStmt, or NULL if the statement
  // is just a declaration
  
  switch(Peek())
    {
    case TOK_OBJECT:
      pp_local_decl();
      break;
    case TOK_BIND:
      return pp_assignment();
    case TOK_RETURN:
      return pp_return_stmt();
    case TOK_OPEN_PAREN:
    case TOK_IDENT:
    case TOK_SELF:
    case TOK_SUPER:
    case TOK_OPEN_BRACKET:
    case TOK_INT_LIT:
    case TOK_FLOAT_LIT:
    case TOK_CHAR_LIT:
    case TOK_STRING_LIT:
      {
	AstExpr *e=pp_expression();
	return new AstExprStmt(e);
      }
    default:
      // error
      throw SYNTAX_ERROR(__FILE__,__LINE__,
	CurrentStream->GetLineNum(),"Expecting statement");
    }
  return 0;
}



void Parser::pp_local_decl()
{
  // local_decl -> TOK_OBJECT ident_list
  
  linked_list *idents;
  
  Match(TOK_OBJECT);
  idents=pp_ident_list();
  
  idents->reset_seq();
  IdentifierNode *in;
  while(in=DYNAMIC_CAST_PTR(IdentifierNode,idents->sequential()))
    {
      // Compute lexical position (we add 1, because AR[0] is
      // reserved for "self")
      int LexicalPosition=TheScopeStack.GetCurrentScopeSize()+1;
      
      ObjectNameNode *onn=
	new ObjectNameNode(in->GetIdent(),OBJ_LOCAL,LexicalPosition);
      TheScopeStack.Insert(onn,in->GetIdent());
    }
}



AstStmt *Parser::pp_assignment()
{
  // assignment -> TOK_BIND ident_ref TOK_TO expression
  
  Match(TOK_BIND);
  AstIdent *NewIdent=pp_ident_ref();
  Match(TOK_TO);
  AstExpr *e=pp_expression();
  
  return new AstBind(NewIdent,e);
}



AstStmt *Parser::pp_return_stmt()
{
  // return_stmt -> TOK_RETURN expression
  
  Match(TOK_RETURN);
  AstExpr *e=pp_expression();
  
  return new AstReturn(e,CurrentDepth);
}



// ********************** EXPRESSIONS *********************

AstExpr *Parser::pp_expression()
{
  // expression -> bin_expr keyword_msg coalesced opt_comparison
  
  AstExpr *recipient=pp_bin_expr();
  linked_list Parms;
  char *msg_name=pp_keyword_msg(&Parms);
  AstExpr *snd=BuildSend(recipient,msg_name,&Parms);
  delete [] msg_name;
  AstExpr *coalesced=pp_coalesced(recipient,snd);
  return pp_opt_comparison(coalesced);
}



AstExpr *Parser::pp_coalesced(AstExpr *recipient,AstExpr *previousMsg)
{
  // coalesced -> TOK_SEMI postfix_unary_msgs keyword_msg coalesced
  // coalesced -> 

  while(Peek()==TOK_SEMICOLON)
    {
      bool useCoalesced=true;
      Match(TOK_SEMICOLON);
      
      linked_list idents;
      pp_postfix_unary_msgs(&idents);
      
      AstExpr *e=recipient;
      if(!idents.IsEmpty()) 
	{
	  // We have to build a "ladder" of message sends for the
	  // unary postfix selectors
	  
	  idents.reset_seq();
	  IdentifierNode *in;
	  while(in=DYNAMIC_CAST_PTR(IdentifierNode,idents.sequential()))
	    {
	      if(useCoalesced)
		{
		  e=new AstCoalescedSend(e,in->GetIdent(),previousMsg);
		  useCoalesced=false;
		}
	      else
		  e=new AstSend(e,in->GetIdent());
	    }
	}

      // handle keyword selector, if any
      linked_list Parms;
      char *msg_name=pp_keyword_msg(&Parms);
      if(useCoalesced)
	{
	  previousMsg=BuildCoalescedSend(e,msg_name,&Parms,previousMsg);
	  useCoalesced=false;
	}
      else
	previousMsg=BuildSend(e,msg_name,&Parms);
    }

  return previousMsg;
}



AstExpr *Parser::BuildSend(AstExpr *recipient,char *msg_name,
			   linked_list *Parms)
{
  // This method builds an AstSend node for a "keyword-message"
  // (a message taking parameters)
  
  if(Parms->IsEmpty()) return recipient;
  
  AstSend *send=new AstSend(recipient,msg_name);
  while(!Parms->IsEmpty())
    {
      AstExpr *exp=DYNAMIC_CAST_PTR(AstExpr,Parms->RemoveFirst());
      send->AppendParm(exp);
    }
  return send;
}



AstExpr *Parser::BuildCoalescedSend(AstExpr *recipient,char *msg_name,
				    linked_list *Parms,
				    AstExpr *previousMsg)
{
  // This method builds an AstSend node for a "keyword-message"
  // (a message taking parameters)
  
  if(Parms->IsEmpty()) //return recipient;
    Match(TOK_IDENT); // signal an error!
  
  AstSend *send=new AstCoalescedSend(recipient,msg_name,previousMsg);
  while(!Parms->IsEmpty())
    {
      AstExpr *exp=DYNAMIC_CAST_PTR(AstExpr,Parms->RemoveFirst());
      send->AppendParm(exp);
    }
  return send;
}



AstExpr *Parser::pp_opt_comparison(AstExpr *lhs)
{
  // opt_comparison -> TOK_EQUALS expression
  // opt_comparison ->
  
  if(Peek()==TOK_EQUAL)
    {
      Match(TOK_EQUAL);
      AstExpr *rhs=pp_expression();
      return new AstEquals(lhs,rhs);
    }
  return lhs;
}



char *Parser::pp_keyword_msg(linked_list *Parms)
{
  // keyword_msg -> TOK_IDENT TOK_COLON primary keyword_msg
  // keyword_msg ->
  
  // A "keyword messsge" is a message taking parameters
  
  // This method returns the name of the message, and fills the
  // Parms list with parameters, which are AstExpr's.
  // Caller: please delete my return value!
  
  ostrstream os;
  while(Peek()==TOK_IDENT)
    {
      Token tok;
      Match(TOK_IDENT,tok);
      Match(TOK_COLON);
      os << tok.GetLexeme() << ':';
      Parms->list_append(pp_bin_expr());
    }
  
  os << ends;
  return os.str();
}



void Parser::pp_postfix_unary_msgs(linked_list *L)
{
  // postfix_unary_msgs -> TOK_IDENT more_idents
  // postfix_unary_msgs ->
  
  // These are the productions that make the grammar LL(2) rather
  // than LL(1); an LL(1) parser wouldn't be able to decide whether
  // to apply the first or second production based on only the next
  // symbol, because if the next symbol is a TOK_IDENT, we don't
  // know whether postfix_unary_msgs should generate that TOK_IDENT, or
  // if it should be generated at the beginning of keyword_msg.  So
  // we use 2 symbols of look-ahead here to see whether the TOK_IDENT
  // is followed by a TOK_COLON; if so, we let keyword_msg have the
  // TOK_IDENT.  We use Parser::PushBack() to put back the first
  // look-ahead token (TOK_IDENT) after looking at it; the second
  // look-ahead token needn't be pushed back, becaused it is peeked-at
  // rather than being actually extracted.
  
  while(Peek()==TOK_IDENT)
    {
      Token name;
      Match(TOK_IDENT,name);
      if(Peek()==TOK_COLON)
	{
	  PushBack(name);
	  return;
	}
      L->list_append(new IdentifierNode(name.GetLexeme()));
    }
}



AstExpr *Parser::pp_bin_expr()
{
  // bin_expr -> primary opt_bin_op
  
  return pp_opt_bin_op(pp_primary());
}



AstExpr *Parser::pp_opt_bin_op(AstExpr *FirstPrimary)
{
  // opt_bin_op -> TOK_BINOP primary opt_bin_op
  // opt_bin_op ->
  
  Token op;
  AstExpr *lhs=FirstPrimary;
  while(Peek()==TOK_BINOP)
    {
      Match(TOK_BINOP,op);
      AstSend *snd=new AstSend(lhs,op.GetLexeme());
      snd->AppendParm(pp_primary());
      lhs=snd;
    }
  
  return lhs;
}



AstExpr *Parser::pp_primary()
{
  // primary -> atom postfix_unary_msgs
  
  // A  "primary" is an atom followed by zero or more postfix
  // unary message-sends
  
  AstExpr *recipient=pp_atom();
  
  linked_list idents;
  pp_postfix_unary_msgs(&idents);
  
  if(idents.IsEmpty()) return recipient;
  
  // Otherwise, we have to build a "ladder" of message sends
  // (because the atom is followed by a list of postfix unary
  // messages)
  
  idents.reset_seq();
  IdentifierNode *in;
  AstExpr *e=recipient;
  while(in=DYNAMIC_CAST_PTR(IdentifierNode,idents.sequential()))
    e=new AstSend(e,in->GetIdent());
  return e;
}



AstExpr *Parser::pp_atom()
{
  // atom -> TOK_OPEN_PAREN expression TOK_CLOSE_PAREN
  // atom -> literal
  // atom -> ident_ref
  // atom -> TOK_SELF
  // atom -> TOK_SUPER
  // atom -> block
  // atom -> new_expr
  
  switch(Peek())
    {
    case TOK_OPEN_PAREN:
      {
	Match(TOK_OPEN_PAREN);
	AstExpr *e=pp_expression();
	Match(TOK_CLOSE_PAREN);
	return e;
      }
    case TOK_BINOP: // for unary '-' with int or float literals
    case TOK_INT_LIT:
    case TOK_FLOAT_LIT:
    case TOK_CHAR_LIT:
    case TOK_STRING_LIT:
      return pp_literal();
    case TOK_IDENT:
      return pp_ident_ref();
    case TOK_SELF:
      {
	Token tok;
	Match(TOK_SELF,tok);
	if(!CurrentClass)
	  throw SEMANTIC_ERROR(__FILE__,__LINE__,tok.GetLineNum(),
			       "keyword 'self' may not appear in main");
	return new AstIdent(CurrentDepth,0,OBJ_LOCAL);
      }
    case TOK_SUPER:
      {
	Token tok;
	Match(TOK_SUPER,tok);
	if(!CurrentClass)
	  throw SEMANTIC_ERROR(__FILE__,__LINE__,tok.GetLineNum(),
			       "keyword 'super' may not appear in main");
	Class *SuperClass=CurrentClass->GetSuperClass();
	if(!SuperClass)
	  throw SEMANTIC_ERROR(__FILE__,__LINE__,tok.GetLineNum(),
			       "Class has no super class");
	return new AstSuper(SuperClass,CurrentDepth);
      }
    case TOK_OPEN_BRACKET:
      return pp_block();
    default:
      throw SYNTAX_ERROR(__FILE__,__LINE__,
			 CurrentStream->GetLineNum(),"Expecting primary");
    }
}



AstIdent *Parser::pp_ident_ref()
{
  // ident_ref -> TOK_IDENT
  
  // An identifier has been embedded right here in the source code;
  // we must compute the lexical address of the denoted object.  To
  // do this, we first look in the scope stack to see if it is from
  // a local declaration.  If not, then we ask the class whose method
  // we are currently parsing if it is an attribute.  However, even
  // if we do find it in the scope stack, if it is marked as a global
  // object, then we still check to see if it is an attribute, because
  // local declarations hide attribute names, but attribute names
  // hide global names, just like in C++.
  
  // This method also parses class names, which evaluate to the
  // first-class object representing that class.
  
  Token name;
  Match(TOK_IDENT,name);
  int LexicalDepth;
  SymbolNode *sn=
    TheScopeStack.Find(name.GetLexeme(),LexicalDepth);
  if(!sn)
    {
      // Not in the scope stack...maybe it's an attribute...
      if(CurrentClass)
	sn=CurrentClass->FindAttribute(name.GetLexeme());
      if(!sn)
	{
	  ostrstream os;
	  os << "Undefined identifier \"" << name.GetLexeme() <<
	    "\"" << ends;
	  throw SEMANTIC_ERROR(__FILE__,__LINE__,name.GetLineNum(),
			       os.str());
	}
      LexicalDepth=CurrentDepth;
    }
  ObjectNameNode *onn=DYNAMIC_CAST_PTR(ObjectNameNode,sn);
  if(!onn)
    {
      // See if it's a class name
      ClassNameNode *cnn=DYNAMIC_CAST_PTR(ClassNameNode,sn);
      if(cnn)
	return new AstClassName(cnn->GetClass());
      
      throw SEMANTIC_ERROR(__FILE__,__LINE__,name.GetLineNum(),
			   "Identifier does not name an object");
    }
  
  // If we found it in the global name space, let's make sure
  // the programmer didn't override the name as an object
  // attribute:
  if(onn->GetStorageClass()==OBJ_GLOBAL && CurrentClass)
    {
      sn=CurrentClass->FindAttribute(name.GetLexeme());
      if(sn)
	{
	  onn=DYNAMIC_CAST_PTR(ObjectNameNode,sn);
	  LexicalDepth=CurrentDepth;
	}
    }
  
  return new AstIdent(LexicalDepth,onn->GetLexicalPosition(),
		      onn->GetStorageClass());
}



AstExpr *Parser::pp_literal()
{
  // literal -> TOK_INT_LIT
  // literal -> TOK_BINOP literal
  // literal -> TOK_FLOAT_LIT
  // literal -> TOK_CHAR_LIT
  // literal -> TOK_STRING_LIT
  
  Token tok;
  
  switch(Peek())
    {
    case TOK_BINOP:
      {
	Match(TOK_BINOP,tok);
	if(strcmp(tok.GetLexeme(),"-"))
	  throw SYNTAX_ERROR(__FILE__,__LINE__,tok.GetLineNum(),
			     "Only '-' may be used as a unary operator");
	AstExpr *lit=pp_literal();
	
	// Try integer
	AstIntLiteral *int_lit=DYNAMIC_CAST_PTR(AstIntLiteral,lit);
	if(int_lit)
	  {
	    int_lit->Negate();
	    return int_lit;
	  }
	
	// Try float
	AstFloatLiteral *float_lit=DYNAMIC_CAST_PTR(AstFloatLiteral,lit);
	if(float_lit)
	  {
	    float_lit->Negate();
	    return float_lit;
	  }
	
	// Error
	throw SYNTAX_ERROR(__FILE__,__LINE__,tok.GetLineNum(),
			   "Unary '-' may be applied only to numeric literals");
      }
    case TOK_INT_LIT:
      Match(TOK_INT_LIT,tok);
      return new AstIntLiteral(atoi(tok.GetLexeme()));
    case TOK_FLOAT_LIT:
      Match(TOK_FLOAT_LIT,tok);
      return new AstFloatLiteral(atof(tok.GetLexeme()));
    case TOK_CHAR_LIT:
      Match(TOK_CHAR_LIT,tok);
      return new AstCharLiteral(tok.GetLexeme()[0]);
    default: // this default is to supress compiler warnings
    case TOK_STRING_LIT:
      Match(TOK_STRING_LIT,tok);
      return new AstStringLiteral(tok.GetLexeme());
    }
}



AstExpr *Parser::pp_block()
{
  // block -> TOK_OPEN_BRACKET block_parms stmt_list TOK_CLOSE_BRACKET
  
  TheScopeStack.EnterScope();
  
  SyntaxForest *sf=new SyntaxForest;
  
  Match(TOK_OPEN_BRACKET);
  ++CurrentDepth;
  
  int NumParms=pp_block_parms();
  pp_stmt_list(sf);
  
  Match(TOK_CLOSE_BRACKET);
  --CurrentDepth;
  
  // Allocate temporaries to the nodes of the trees
  TemporaryAllocator TempMgr(TheScopeStack.GetCurrentScopeSize()+1);
  sf->ReceiveVisitor(&TempMgr);
  
  // Compute size of activation record for this method based on the
  // number of temporaries and the number of local objects & parameters
  int AR_size=TempMgr.GetNumTemps()+TheScopeStack.GetCurrentScopeSize()+1;
  int NumLocals=AR_size-NumParms;
  
  TheScopeStack.LeaveScope();
  
  AstBlockLiteral *bl=new AstBlockLiteral(sf,NumParms,NumLocals);
  return bl;
}



int Parser::pp_block_parms()
{
  // block_parms -> block_parm more_block_parms TOK_VERTICAL_BAR
  // block_parms ->
  
  // Returns the number of parameters
  
  int NumParms=0;
  
  if(Peek()==TOK_COLON)
    {
      pp_block_parm();
      NumParms=pp_more_block_parms()+1;
      Match(TOK_VERTICAL_BAR);
    }
  
  return NumParms;
}



void Parser::pp_block_parm()
{
  // block_parm -> TOK_COLON TOK_IDENT
  
  Match(TOK_COLON);
  
  Token tok;
  Match(TOK_IDENT,tok);
  
  // We compute lexical position by adding 1 to the number of
  // names already in the symbol table, because the 0th AR slot
  // is reserved for "self" (which in this case is a pointer to
  // the block object)
  int LexicalPosition=TheScopeStack.GetCurrentScopeSize()+1;
  
  ObjectNameNode *onn=new ObjectNameNode(tok.GetLexeme(),
					 OBJ_LOCAL,LexicalPosition);
  TheScopeStack.Insert(onn,tok.GetLexeme());
}



int Parser::pp_more_block_parms()
{
  // more_block_parms -> block_parm more_block_parms
  // more_block_parms ->
  
  // Returns the number of parameters
  
  int NumParms=0;
  while(Peek()==TOK_COLON)
    {
      pp_block_parm();
      ++NumParms;
    }
  return NumParms;
}




