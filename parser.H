// =======================================
// Parser.h
//
// Parser class and parsing routines
//
//
// =======================================

#ifndef INCL_PARSER_H
#define INCL_PARSER_H

#include "libsrc/typeinfo.H"
#include "scanner.H"
#include "ast.H"
#include "libsrc/linked2.H"
#include "except.H"
#include "symblrec.H"



// ****************************************
//	       class NamedStream
// ****************************************

// A NamedStream is a TokenStream which has a
// filename; we do this so that error messages
// can report the file in which an error occurred,
// in addition to the line number.

class NamedStream 
{
  char *Name;
  TokenStream *TheStream;
public:
  NamedStream(char *,TokenStream *);
  virtual ~NamedStream();
  char *GetName() const;
  TokenStream *GetStream() const;
};



// ****************************************
//	      class StreamStack
// ****************************************

// A StreamStack is a stack of TokenStreams.  It is
// used for including files in a nested way, without
// forgetting the files that included them.

typedef ListStack<NamedStream*> StreamStack;




// ****************************************
//	   class EpsilonScopeStack
// ****************************************

// An EpsilonScopeStack is a stack of symbol tables.
// We stack symbol tables in order to keep track of
// nesting levels of identifier declarations.

typedef ScopeStack<SymbolNode,41> EpsilonScopeStack;




// ****************************************
//	   class FilenameRecord
// ****************************************

// A FilenameRecord represents a file that has
// already been included (and should therefore
// not be included again)

class FilenameRecord : public AbstractSymbolNode 
{
  char *Filename;
public:
  FilenameRecord(const char *Filename);
  virtual ~FilenameRecord();
  char *GetFilename() const;
  virtual bool Equals(const char *);
};



// ****************************************
//	       class FilenameSet
// ****************************************

// A FilenameSet is a set of filenames that have
// already been included.  We keep track of the files
// that have already been included so we do not include
// a file more than once, which would result in multiple-
// definition errors.

typedef HashSymbolTable<FilenameRecord,41> FilenameSet;



// forward declaration
class Class;



// ****************************************
//		class Parser
// ****************************************

// I parse an input stream by discovering the
// syntactic structure of the stream's contents.
// I create SyntaxTrees which represent that
// structure.  I automatically handle included
// files, by maintaining token streams on a stack.

class Parser 
{
  // Data members
  TokenStream *CurrentStream;
  char *CurrentStreamName;
  StreamStack OldStreams;
  EpsilonScopeStack TheScopeStack;
  EpsilonBody *MainBody; // The body for main
  bool MainDefined; // has main been defined yet?
  FilenameSet AlreadyIncluded; // files already included
  Token PushedBack;
  bool IsPushedBack;
  int nextGlobalLP; // next lexical position for global objects
  
  // Temporary parsing variables
  Class *CurrentClass; // The class whose method we are parsing (0 for main)
  int CurrentDepth; // Current lexical depth of parse position
  
  // Misc routines
  void Match(TokenType t);
  void Match(TokenType t,Token &Tok);
  TokenType Peek();
  void PushBack(Token &);
  void DefineGlobals();
  bool CanBeginStatement(TokenType);
  
  // =============================================================
  // ==== Parsing procedures: (each represents a nonterminal) ====
  // =============================================================
  
  // ************************* TOP LEVEL *************************
  void pp_program(); // parses a file (even just an include file)
  void pp_component();
  void pp_include();
  
  // ********************* CLASS DECLARATIONS ********************
  void pp_class_decl();
  Class *pp_base_class();
  void pp_class_body(Class *NewClass,Class *NewMetaclass);
  void pp_more_class_body(Class *NewClass,Class *NewMetaclass);
  void pp_member_decl(Class *NewClass,Class *NewMetaclass);
  void pp_static_member_decl(Class *);
  void pp_attribute(Class *);
  linked_list *pp_ident_list();
  linked_list *pp_more_comma_idents();
  void pp_method_decl(Class *);
  char *pp_method_name(linked_list *parms);
  char *pp_long_name(linked_list *parms);
  char *pp_longer_name(linked_list *parms);
  
  // ******************** METHODS AND STATEMENTS *****************
  void pp_method_defn();
  void pp_main_entry();
  SyntaxForest *pp_function_body();
  void pp_stmt_list(SyntaxForest *);
  void pp_more_statements(SyntaxForest *);
  AstStmt *pp_statement();
  void pp_local_decl();
  AstStmt *pp_assignment();
  AstStmt *pp_return_stmt();
  
  // ************************* EXPRESSIONS ***********************
  AstExpr *pp_expression();
  AstExpr *pp_opt_comparison(AstExpr *);
  AstExpr *pp_coalesced(AstExpr *recipient,AstExpr *previousMsg);
  AstExpr *BuildSend(AstExpr *recipient,char *msg_name,linked_list *Parms);
  AstExpr *BuildCoalescedSend(AstExpr *recipient,char *msg_name,
			      linked_list *Parms,AstExpr *previousMsg);
  char *pp_keyword_msg(linked_list *Parms);
  void pp_postfix_unary_msgs(linked_list *);
  AstExpr *pp_bin_expr();
  AstExpr *pp_opt_bin_op(AstExpr *);
  AstExpr *pp_primary();
  AstExpr *pp_atom();
  AstExpr *pp_literal();
  AstExpr *pp_block();
  int pp_block_parms();
  void pp_block_parm();
  int pp_more_block_parms();
  AstIdent *pp_ident_ref();
  
public:
  Parser();
  void ParseEntireProgram(const char *fname);
  void ParseStream(istream &);
  MethodBody *GetMain();
  char *GetCurrentFilename() const;
  void ParseMethod(istream &);
  EpsilonScopeStack &getScopeStack();
  bool doesMainExist();
  int nextGlobalLexPos();
  int getGlobalARSize();
};



#endif
