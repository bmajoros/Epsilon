// =======================================
// astprint.h
//
// Class for printing ASTs
//
//
// =======================================

#ifndef INCL_ASTPRINT_H
#define INCL_ASTPRINT_H

#include "visitor.H"
#include <iostream.h>



// ****************************************
//		      class AstPrinter
// ****************************************

// This class prints out the AST; it is used
// for debugging.

class AstPrinter : public TreeVisitor 
{
  ostream &os;
public:
  AstPrinter(ostream &os) : os(os) {}
  virtual void Visit(AstSend *n,void * =NULL);
  virtual void Visit(AstCoalescedSend *n,void * =NULL);
  virtual void Visit(AstEquals *n,void * =NULL);
  virtual void Visit(AstNew *n,void * =NULL);
  virtual void Visit(AstIdent *n,void * =NULL);
  virtual void Visit(AstClassName *n,void * =NULL);
  virtual void Visit(AstSuper *n,void * =NULL);
  virtual void Visit(AstBlockLiteral *n,void * =NULL);
  virtual void Visit(AstCharLiteral *n,void * =NULL);
  virtual void Visit(AstStringLiteral *n,void * =NULL);
  virtual void Visit(AstFloatLiteral *n,void * =NULL);
  virtual void Visit(AstIntLiteral *n,void * =NULL);
  virtual void Visit(AstExprStmt *n,void * =NULL);
  virtual void Visit(AstReturn *n,void * =NULL);
  virtual void Visit(AstBind *n,void * =NULL);
};



#endif
