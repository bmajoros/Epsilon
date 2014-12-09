// =======================================
// AST.h
//
// Abstract syntax trees
//
//
// =======================================

#ifndef INCL_AST_H
#define INCL_AST_H

#include "libsrc/linked2.H"
#include "libsrc/safecopy.H"
#include "symblrec.H"
#include <iostream.h>
#include "libsrc/RTTI.H"

// forward declarations
class Object;
class Class;
class AstStmt;
class TreeVisitor;
class RunTimeStack;



/*        ++++++++++++++++++++++++++++++++++++++
	  INHERITANCE HIERARCHY FOR SYNTAX TREES
	  ++++++++++++++++++++++++++++++++++++++


	     	     AstStmt
			|
			|
			|
	------------------------------------
	|                    |             |
	|		     |		   |
	AstExprStmt      AstReturn	AstBind




		     AstExpr
			|
			|
			|
   --------------------------------------------------------------
   |		|		|		   |		|
   |		|		|		   |		|
AstSend	    AstEquals	   AstLiteral		AstNew	    AstIdent
				|                   		|
				|                       ----------------
				|                       |              |
       -----------------------------------------   AstClassName    AstSuper
       |         |          |        |         |
       |         |          |        |         |
AstCharLiteral	 |  AstSringLiteral  |	AstBlockLiteral
		 |                   |
                 |                   |
	    AstFloatLiteral 	AstIntLiteral


*/



// ****************************************
//	 class LexicalAddress
// ****************************************

// Denotes an object reference by specifying the
// lexical scope-nesting and the position within
// a pointer array, such as an activation record,
// or an object's attribute table

class LexicalAddress 
{
  int Depth, Position;
public:
  LexicalAddress(int Depth,int Position);
  LexicalAddress();
  int GetDepth() const;
  int GetPosition() const;
  void SetPosition(int i);
};



ostream &operator<<(ostream &os,const LexicalAddress &la);



// ****************************************
//	    class SyntaxForest
// ****************************************

// A sequence of executable statements

class SyntaxForest 
{
  linked_list MyStatements; // list of AstStmt objects
public:
  void Append(AstStmt *s);
  virtual void ReceiveVisitor(TreeVisitor *v,void * =NULL);
  virtual void Execute(RunTimeEnvironment &env);
  Object *GetValue(RunTimeEnvironment &env);
};



// ****************************************
//	       class AstExpr
// ****************************************

// Any arbitrary expression

class AstExpr : public link_node 
{
  LexicalAddress MyTemporary;
protected:
  void StoreInTemporary(Object *obj,RunTimeEnvironment &env);
public:
  RTTI_DECLARE_SUBCLASS(AstExpr,link_node)
  virtual ~AstExpr() {}
  virtual void ReceiveVisitor(TreeVisitor *v,void * =NULL);
  void SetTemporary(const LexicalAddress &temp);
  virtual LexicalAddress GetTemporary() const;
  virtual void Evaluate(RunTimeEnvironment &env)=0;
  virtual Object *GetValue(RunTimeEnvironment &env);
};



// ****************************************
//	      class AstSend
// ****************************************

// A message send

class AstSend : public AstExpr 
{
protected:
  AstExpr *Recipient;
  char *MessageName;
  linked_list Parameters; // list of AstExpr
  MethodBody *GetMethod(Object *);
public:
  RTTI_DECLARE_SUBCLASS(AstSend,link_node)
  AstSend(AstExpr *Recipient,const char *Msg);
  virtual ~AstSend();
  void AppendParm(AstExpr *e);
  void AppendToName(char *);
  virtual void ReceiveVisitor(TreeVisitor *v,void * =NULL);
  linked_list &GetParmNodes();
  AstExpr *GetRecipientNode();
  virtual void Evaluate(RunTimeEnvironment &env);
  const char *GetMessageName() const;
};



// ****************************************
//	class AstCoalescedSend
// ****************************************

// A message send

class AstCoalescedSend : public AstSend
{
  AstExpr *previousMsg;
public:
  RTTI_DECLARE_SUBCLASS(AstCoalescedSend,link_node)
  AstCoalescedSend(AstExpr *recipient,
		   const char *msg,AstExpr *previousMsg);
  virtual ~AstCoalescedSend();
  virtual void ReceiveVisitor(TreeVisitor *v,void * =NULL);
  virtual void Evaluate(RunTimeEnvironment &env);
  virtual AstExpr *GetPrevMsgNode() { return previousMsg; }
};



// ****************************************
//	     class AstEquals
// ****************************************

// A comparison, using the == operator

class AstEquals : public AstExpr 
{
  AstExpr *lhs, *rhs;
public:
  RTTI_DECLARE_SUBCLASS(AstEquals,link_node)
  AstEquals(AstExpr *lhs,AstExpr *rhs);
  virtual void ReceiveVisitor(TreeVisitor *v,void * =NULL);
  AstExpr *GetLhs();
  AstExpr *GetRhs();
  virtual void Evaluate(RunTimeEnvironment &env);
};



// ****************************************
//		class AstNew
// ****************************************

// An application of the new operator for creating
// an instance of a class

class AstNew : public AstExpr 
{
  Class *TheClass;
public:
  RTTI_DECLARE_SUBCLASS(AstNew,link_node)
  AstNew(Class *c) : TheClass(c) {}
  virtual void ReceiveVisitor(TreeVisitor *v,void * =NULL);
  virtual void Evaluate(RunTimeEnvironment &env);
};



// ****************************************
//	      class AstIdent
// ****************************************

// An identifier reference, which is represented
// as the lexical address, or "debruin notation,"
// indicating the location of the denoted object

class AstIdent : public AstExpr 
{
  LexicalAddress la;
  StorageClass MyStorageClass;
public:
  RTTI_DECLARE_SUBCLASS(AstIdent,link_node)
  AstIdent(int LexicalDepth,int LexicalPosition,StorageClass c);
  virtual void ReceiveVisitor(TreeVisitor *v,void * =NULL);
  virtual void Evaluate(RunTimeEnvironment &env);
  LexicalAddress GetLexicalAddress() const;
  StorageClass GetStorageClass() const;
  Object *GetValue(RunTimeEnvironment &env);
  void BindTo(Object *,RunTimeEnvironment &env);
};



// ****************************************
//	     class AstClassName
// ****************************************
class AstClassName : public AstIdent 
{
  Class *TheClass;
public:
  RTTI_DECLARE_SUBCLASS(AstClassName,link_node)
  AstClassName(Class *TheClass);
  virtual void ReceiveVisitor(TreeVisitor *v,void * =NULL);
  Class *GetClass();
  virtual void Evaluate(RunTimeEnvironment &env);
  Object *GetValue(RunTimeEnvironment &env);
};



// ****************************************
//	      class AstSuper
// ****************************************

// AstSuper represents the "super" keyword.
// Messages sent to the "super" keyword are sent
// via the superclass of the class owning the
// method in which the "super" keyword (lexically)
// occurs.  However, passing "super" to a method is
// the same as passing "self."

class AstSuper : public AstIdent 
{
  Class *TheSuperClass;
public:
  RTTI_DECLARE_SUBCLASS(AstSuper,link_node)
  AstSuper(Class *SuperClass,int LexicalDepth);
  virtual void ReceiveVisitor(TreeVisitor *v,void * =NULL);
  Class *GetSuperClass();
};



// ****************************************
//	     class AstLiteral
// ****************************************

// Any literal occurring in the text of the
// source code, such as 3.14, or a block

class AstLiteral : public AstExpr 
{
public:
  RTTI_DECLARE_SUBCLASS(AstLiteral,link_node)
  virtual void ReceiveVisitor(TreeVisitor *v,void * =NULL);
  virtual void Evaluate(RunTimeEnvironment &env)=0;
};



// ****************************************
//	   class AstCharLiteral
// ****************************************

// A character litaral, such as 'a'

class AstCharLiteral : public AstLiteral 
{
  char TheChar;
public:
  RTTI_DECLARE_SUBCLASS(AstCharLiteral,link_node)
  AstCharLiteral(char c);
  virtual void ReceiveVisitor(TreeVisitor *v,void * =NULL);
  virtual void Evaluate(RunTimeEnvironment &env);
  char GetChar() const;
};



// ****************************************
//	   class AstStringLiteral
// ****************************************

// A string literal, such as "hello, world"

class AstStringLiteral : public AstLiteral 
{
  char *TheString;
public:
  RTTI_DECLARE_SUBCLASS(AstStringLiteral,link_node)
  AstStringLiteral(const char *s);
  virtual ~AstStringLiteral();
  virtual void ReceiveVisitor(TreeVisitor *v,void * =NULL);
  virtual void Evaluate(RunTimeEnvironment &env);
  const char *GetString() const;
};



// ****************************************
//	    class AstBlockLiteral
// ****************************************

// A block literal, such as [ :a | a m: 3;]

class AstBlockLiteral : public AstLiteral 
{
  SyntaxForest *MyStatements;
  int NumParms;  // number of parameters to the block
  int NumLocals; // includes locals, self, and temporaries, but not parms
public:
  RTTI_DECLARE_SUBCLASS(AstBlockLiteral,link_node)
  AstBlockLiteral(SyntaxForest *f,int NumParms,int NumLocals);
  virtual ~AstBlockLiteral();
  virtual void ReceiveVisitor(TreeVisitor *v,void * =NULL);
  virtual void Evaluate(RunTimeEnvironment &env);
  int GetNumParms() const;
  int GetNumLocals() const;
  int GetARsize() const;
  SyntaxForest &GetStatements();
};



// ****************************************
//	     class AstFloatLiteral
// ****************************************

// A floating-point literal, such as 3.14

class AstFloatLiteral : public AstLiteral 
{
  float TheFloat;
public:
  RTTI_DECLARE_SUBCLASS(AstFloatLiteral,link_node)
  AstFloatLiteral(float f);
  virtual void ReceiveVisitor(TreeVisitor *v,void * =NULL);
  virtual void Evaluate(RunTimeEnvironment &env);
  float GetFloat() const;
  void Negate();
};



// ****************************************
//	   class AstIntLiteral
// ****************************************

// An integer literal, such as 4

class AstIntLiteral : public AstLiteral 
{
  int TheInt;
public:
  RTTI_DECLARE_SUBCLASS(AstIntLiteral,link_node)
  AstIntLiteral(int i);
  virtual void ReceiveVisitor(TreeVisitor *v,void * =NULL);
  virtual void Evaluate(RunTimeEnvironment &env);
  int GetInt() const;
  void Negate();
};



// ****************************************
//	       class AstStmt
// ****************************************

// A single statement, which is either an assignment,
// a return statement, or an expression.

class AstStmt : public link_node 
{
public:
  RTTI_DECLARE_SUBCLASS(AstStmt,link_node)
  virtual ~AstStmt() {}
  virtual void ReceiveVisitor(TreeVisitor *v,void * =NULL);
  virtual void Execute(RunTimeEnvironment &env)=0;
};



// ****************************************
//	     class AstExprStmt
// ****************************************

// A statement which consists of an expression
// to be evaluated

class AstExprStmt : public AstStmt 
{
  AstExpr *MyExpression;
public:
  RTTI_DECLARE_SUBCLASS(AstExprStmt,link_node)
  AstExprStmt(AstExpr *e);
  AstExpr *GetExpr();
  Object *GetValue(RunTimeEnvironment &env);
  virtual void ReceiveVisitor(TreeVisitor *v,void * =NULL);
  virtual void Execute(RunTimeEnvironment &env);
};



// ****************************************
//	    class AstReturn
// ****************************************

// A return statement (which is very hard to
// execute, because it may require unwinding
// the stack to the _lexically_ enclosing scope)

class AstReturn : public AstStmt 
{
  AstExpr *MyExpression;
  int NestingLevel; // lexical nesting level of this statement
public:
  RTTI_DECLARE_SUBCLASS(AstReturn,link_node)
  AstReturn(AstExpr *e,int NestingLevel);
  virtual void ReceiveVisitor(TreeVisitor *v,void * =NULL);
  AstExpr *GetExpr();
  virtual void Execute(RunTimeEnvironment &env);
};



// ****************************************
//	     class AstBind
// ****************************************

// An assignment statement

class AstBind : public AstStmt 
{
  AstIdent *lhs;
  AstExpr *rhs;
public:
  RTTI_DECLARE_SUBCLASS(AstBind,link_node)
  AstBind(AstIdent *lhs,AstExpr *rhs);
  virtual void ReceiveVisitor(TreeVisitor *v,void * =NULL);
  AstExpr *GetRhs();
  AstIdent *GetLhs();
  virtual void Execute(RunTimeEnvironment &env);
};



#endif
