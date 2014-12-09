// =======================================
// TempMgr.cpp
//
// Allocation of temporaries for expression
// evaluation
//
//
// =======================================

#include "libsrc/typeinfo.H"
#include "tempmgr.H"
#include "except.H"


// ****************************************
//	    TemporaryPool methods
// ****************************************
LexicalAddress TemporaryPool::GetTemporary()
{
  for(int i=0 ; i<NumTemps ; i++)
    if(!InUse[i]) break;
  
  if(i>=NumTemps) NumTemps++;
  
  InUse[i]=true;
  return LexicalAddress(0,i+FirstTempPosition);
}



void TemporaryPool::ReleaseTemporary(const LexicalAddress &la)
{
  if(la.GetPosition()==-1) return;
  
  InUse[la.GetPosition()-FirstTempPosition]=false;
}



// ****************************************
//	  TemporaryAllocator methods
// ****************************************

/*
Each of these "visitor" methods executes an appropriate form of
the following algorithm, depending on the type of node (i.e., how
many children it has):
  AssignTemps(AstExpr &A,TempPool &P)
  {
     For each child C of A do:
        AssignTemps(C,P);
     For each child C of A do:
        P.ReleaseTemporary(C.Temporary);
     A.Temp:=P.GetTemporary();
   }
*/


void TemporaryAllocator::Visit(AstSend *s,void *releaseRecipTmp)
{
  // Visit the children
  s->GetRecipientNode()->ReceiveVisitor(this,(void*)1);
  linked_list &parms=s->GetParmNodes();
  parms.reset_seq();
  AstExpr *ThisParm;
  while(1)
    {
      link_node *ln=parms.sequential();
      if(!ln) break;
      ThisParm=DYNAMIC_CAST_PTR(AstExpr,ln);
      if(!ThisParm)
	throw INTERNAL_ERROR(__FILE__,__LINE__,
			     "Cannot cast parm list element to AstExpr*");
      ThisParm->ReceiveVisitor(this);
    }
  
  // Release children's temporaries
  if(releaseRecipTmp)
    ThePool.ReleaseTemporary(s->GetRecipientNode()->GetTemporary());
  parms.reset_seq();
  while(ThisParm=DYNAMIC_CAST_PTR(AstExpr,parms.sequential()))
    ThePool.ReleaseTemporary(ThisParm->GetTemporary());
  
  // Allocate a temporary for this node
  s->SetTemporary(ThePool.GetTemporary());
}



void TemporaryAllocator::Visit(AstCoalescedSend *s,void *releaseRecipTmp)
{
  // Visit the children
  s->GetPrevMsgNode()->ReceiveVisitor(this,NULL);
  linked_list &parms=s->GetParmNodes();
  parms.reset_seq();
  AstExpr *ThisParm;
  while(1)
    {
      link_node *ln=parms.sequential();
      if(!ln) break;
      ThisParm=DYNAMIC_CAST_PTR(AstExpr,ln);
      if(!ThisParm)
	throw INTERNAL_ERROR(__FILE__,__LINE__,
			     "Cannot cast parm list element to AstExpr*");
      ThisParm->ReceiveVisitor(this);
    }
  
  // Release children's temporaries
  if(releaseRecipTmp)
    ThePool.ReleaseTemporary(s->GetRecipientNode()->GetTemporary());
  parms.reset_seq();
  while(ThisParm=DYNAMIC_CAST_PTR(AstExpr,parms.sequential()))
    ThePool.ReleaseTemporary(ThisParm->GetTemporary());
  
  // Allocate a temporary for this node
  s->SetTemporary(ThePool.GetTemporary());
}



void TemporaryAllocator::Visit(AstEquals *e,void *releaseRecipTmp)
{
  // Visit the children
  e->GetLhs()->ReceiveVisitor(this,(void*)1);
  e->GetRhs()->ReceiveVisitor(this,(void*)1);
  
  // Release children's temporaries
  ThePool.ReleaseTemporary(e->GetLhs()->GetTemporary());
  ThePool.ReleaseTemporary(e->GetRhs()->GetTemporary());
  
  // Allocate a temporary for this node
  e->SetTemporary(ThePool.GetTemporary());
}



void TemporaryAllocator::Visit(AstNew *n,void *releaseRecipTmp)
{
  // Allocate a temporary for this node
  n->SetTemporary(ThePool.GetTemporary());
}



void TemporaryAllocator::Visit(AstClassName *n,void *releaseRecipTmp)
{
  // Allocate a temporary for this node
  n->SetTemporary(ThePool.GetTemporary());
}



void TemporaryAllocator::Visit(AstLiteral *l,void *releaseRecipTmp)
{
  // Allocate a temporary for this node
  l->SetTemporary(ThePool.GetTemporary());
  
  // Note:  For AstBlockLiteral, we do not recurse
  // onto the block's body, because the block will
  // be executed with its own activation record (AR),
  // so the block should manage its own temporaries
  // apart from the enclosing code's AR.
}



/*void TemporaryAllocator::Visit(AstSuper *s,void *releaseRecipTmp)
  {
  // Allocate a temporary for this node
  s->SetTemporary(ThePool.GetTemporary());
  }
  */



void TemporaryAllocator::Visit(AstIdent *ident,void *releaseRecipTmp)
{
  // DO NOT allocate a temporary for this node
  // (but mark its temporary address as (-1,-1)
  // so it is obvious that this node has no temporary)
  ident->SetTemporary(LexicalAddress(-1,-1));
}



void TemporaryAllocator::Visit(AstExprStmt *s,void *releaseRecipTmp)
{
  // Visit the children
  s->GetExpr()->ReceiveVisitor(this,(void*)1);
  
  // Release children's temporaries
  ThePool.ReleaseTemporary(s->GetExpr()->GetTemporary());
}



void TemporaryAllocator::Visit(AstReturn *r,void *releaseRecipTmp)
{
  // Visit the children
  r->GetExpr()->ReceiveVisitor(this,(void*)1);
  
  // Release children's temporaries
  ThePool.ReleaseTemporary(r->GetExpr()->GetTemporary());
}



void TemporaryAllocator::Visit(AstBind *b,void *releaseRecipTmp)
{
  // Visit the children
  b->GetRhs()->ReceiveVisitor(this,(void*)1);
  
  // Release children's temporaries
  ThePool.ReleaseTemporary(b->GetRhs()->GetTemporary());
}


