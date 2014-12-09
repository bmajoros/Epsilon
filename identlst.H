// =======================================
// IdentLst.h
//
// Lists of identifiers
//
//
// =======================================

#ifndef INCL_IDENT_LIST
#define INCL_IDENT_LIST

#include "libsrc/safecopy.H"
#include "libsrc/linked2.H"



// ****************************************
//			class IdentifierNode
// ****************************************

// A node in an identifier list (linked_list)

class IdentifierNode : public link_node 
{
  char *Ident;
public:
  RTTI_DECLARE_SUBCLASS(IdentifierNode,link_node)
  IdentifierNode(const char *Ident);
  virtual ~IdentifierNode();
  const char *GetIdent() const;
};


#endif
