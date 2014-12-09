
// IdentList.C

#include "identlst.H"

RTTI_DEFINE_SUBCLASS(IdentifierNode,link_node)


IdentifierNode::IdentifierNode(const char *Ident) 
: Ident(Duplicate(Ident)) 
{
  // ctor
}



IdentifierNode::~IdentifierNode() 
{ 
  delete [] Ident; 
}



const char *IdentifierNode::GetIdent() const 
{ 
  return Ident; 
}
