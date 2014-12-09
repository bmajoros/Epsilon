// =======================================
// TempMgr.h
//
// Allocation of temporaries for expression
// evaluation
//
//
// =======================================

#ifndef INCL_TEMPMGR_H
#define INCL_TEMPMGR_H

#include "libsrc/array.H"
#include "visitor.H"



// ****************************************
//	    class TemporaryPool
// ****************************************

// The TemporaryPool is a container containing
// a bunch of temporaries, some of which are
// in use, and others of which are free.

class TemporaryPool 
{
  int NumTemps;
  int FirstTempPosition;
  ElasticArray<bool> InUse;
public:
  TemporaryPool(int FirstTempPosition) : NumTemps(0), InUse(10,10),
  FirstTempPosition(FirstTempPosition) {}
  LexicalAddress GetTemporary();
  void ReleaseTemporary(const LexicalAddress &);
  int GetNumTemps() const { return NumTemps; }
};



// ****************************************
//	   class TemporaryAllocator
// ****************************************

// A TemporaryAllocator is a "visitor" which
// traverses a SyntaxForest, allocating temporaries
// to each of the nodes that need them.  Each node
// calls a method on TemporaryAllocator, telling it
// the node's type.

class TemporaryAllocator : public TreeVisitor 
{
  TemporaryPool ThePool;
public:
  TemporaryAllocator(int FirstTempPosition) : ThePool(FirstTempPosition) {}
  virtual void Visit(AstSend *,void * =NULL);
  virtual void Visit(AstCoalescedSend *,void * =NULL);
  virtual void Visit(AstEquals *,void * =NULL);
  virtual void Visit(AstNew *,void * =NULL);
  virtual void Visit(AstIdent *,void * =NULL);
  virtual void Visit(AstClassName *n,void * =NULL);
  virtual void Visit(AstLiteral *,void * =NULL);
  virtual void Visit(AstExprStmt *,void * =NULL);
  virtual void Visit(AstReturn *,void * =NULL);
  virtual void Visit(AstBind *,void * =NULL);
  int GetNumTemps() const { return ThePool.GetNumTemps(); }
};



#endif
