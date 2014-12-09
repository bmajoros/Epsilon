// =======================================
// Visitor.h
//
// Visitor class for traversing ASTs
//
//
// =======================================

#ifndef INCL_VISITOR_H
#define INCL_VISITOR_H

#include "ast.H"



// ****************************************
//	      class TreeVisitor
// ****************************************

// You can traverse ASTs by deriving a new class from
// TreeVisitor and passing an instance of your class
// to SyntaxForest::ReceiveVisitor(), which will call
// these virtual methods for each AST node in the tree:

class TreeVisitor 
{
public:
  virtual void Visit(AstExpr *n,void *p=NULL) 
   {}
  virtual void Visit(AstSend *n,void *p=NULL) 
   { Visit((AstExpr*)n,p); }
  virtual void Visit(AstCoalescedSend *n,void *p=NULL) 
   { Visit((AstSend*)n,p); }
  virtual void Visit(AstEquals *n,void *p=NULL) 
   { Visit((AstExpr*)n,p); }
  virtual void Visit(AstNew *n,void *p=NULL) 
   { Visit((AstExpr*)n,p); }
  virtual void Visit(AstIdent *n,void *p=NULL) 
   { Visit((AstExpr*)n,p); }
  virtual void Visit(AstClassName *n,void *p=NULL) 
   { Visit((AstIdent*)n,p); }
  virtual void Visit(AstSuper *n,void *p=NULL) 
   { Visit((AstIdent*)n,p); }
  virtual void Visit(AstLiteral *n,void *p=NULL) 
   { Visit((AstExpr*)n,p); }
  virtual void Visit(AstBlockLiteral *n,void *p=NULL) 
   { Visit((AstLiteral*)n,p); }
  virtual void Visit(AstCharLiteral *n,void *p=NULL) 
   { Visit((AstLiteral*)n,p); }
  virtual void Visit(AstStringLiteral *n,void *p=NULL) 
   { Visit((AstLiteral*)n,p); }
  virtual void Visit(AstFloatLiteral *n,void *p=NULL) 
   { Visit((AstLiteral*)n,p); }
  virtual void Visit(AstIntLiteral *n,void *p=NULL) 
   { Visit((AstLiteral*)n,p); }
  virtual void Visit(AstStmt *n,void *p=NULL) {}
  virtual void Visit(AstExprStmt *n,void *p=NULL) 
   { Visit((AstStmt*)n,p); }
  virtual void Visit(AstReturn *n,void *p=NULL) 
   { Visit((AstStmt*)n,p); }
  virtual void Visit(AstBind *n,void *p=NULL) 
   { Visit((AstStmt*)n,p); }
};


#endif
