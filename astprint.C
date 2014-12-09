// =======================================
// astprint.h
//
// Class for printing ASTs
//
//
// =======================================

#include "libsrc/typeinfo.H"
#include "astprint.H"
#include "class.H"



// ****************************************
//		      AstPrinter methods
// ****************************************

void AstPrinter::Visit(AstSend *n,void *)
	{
	os << "(AstSend [temp=" << n->GetTemporary() << "] ";
	n->GetRecipientNode()->ReceiveVisitor(this);
	os << ' ' << n->GetMessageName() << ' ';

	linked_list &p=n->GetParmNodes();
	p.reset_seq();
	AstExpr *e;
	while(e=DYNAMIC_CAST_PTR(AstExpr,p.sequential()))
		{
		e->ReceiveVisitor(this);
		os << ' ';
		}
	os << ")";
	}



void AstPrinter::Visit(AstCoalescedSend *n,void *)
{
  os << "(AstCoalescedSend [temp=" << n->GetTemporary() << "] ";
  n->GetPrevMsgNode()->ReceiveVisitor(this);
  os << ' ' << n->GetMessageName() << ' ';

  linked_list &p=n->GetParmNodes();
  p.reset_seq();
  AstExpr *e;
  while(e=DYNAMIC_CAST_PTR(AstExpr,p.sequential()))
    {
      e->ReceiveVisitor(this);
      os << ' ';
    }
  os << ")";
}



void AstPrinter::Visit(AstEquals *n,void *)
	{
	os << "(AstEquals [temp=" << n->GetTemporary() << "] ";
	n->GetLhs()->ReceiveVisitor(this);
	os << ' ';
	n->GetRhs()->ReceiveVisitor(this);
	os << ")";
	}



void AstPrinter::Visit(AstNew *n,void *)
	{
	os << "(AstNew [temp=" << n->GetTemporary() << "] <some class>)";
	}



void AstPrinter::Visit(AstIdent *n,void *)
	{
	os << "(Ident [temp=" << n->GetTemporary() << "] ";
	LexicalAddress la=n->GetLexicalAddress();
	StorageClass sc=n->GetStorageClass();
	os << la << ' ' << sc << ')';
	}



void AstPrinter::Visit(AstClassName *n,void *)
	{
	os << "(ClassName [temp=" << n->GetTemporary() << "] ";
	os << n->GetClass()->GetName() << ')';
	}



void AstPrinter::Visit(AstSuper *n,void *)
	{
	os << "(Super [temp=" << n->GetTemporary() << "] ";
	LexicalAddress la=n->GetLexicalAddress();
	StorageClass sc=n->GetStorageClass();
	os << la << ' ' << sc << ')';
	}



void AstPrinter::Visit(AstBlockLiteral *n,void *)
	{
	os << "(AstBlockLiteral [temp=" << n->GetTemporary() <<
		"]parms=" << n->GetNumParms() <<
		" locals=" << n->GetNumLocals() << ' ';
	n->GetStatements().ReceiveVisitor(this);
	os << ')';
	}



void AstPrinter::Visit(AstCharLiteral *n,void *)
	{
	os << "('" << n->GetChar() << '\'' << "[temp="
		<< n->GetTemporary() << "]) ";
	}



void AstPrinter::Visit(AstStringLiteral *n,void *)
	{
	os << "(\"" << n->GetString() << "\" " << "[temp="
		<< n->GetTemporary() << "]) ";
	}



void AstPrinter::Visit(AstFloatLiteral *n,void *)
	{
	os << "(" << n->GetFloat() << "[temp=" << n->GetTemporary() << "]) ";
	}



void AstPrinter::Visit(AstIntLiteral *n,void *)
	{
	os << "(" << n->GetInt() << "[temp=" << n->GetTemporary() << "]) ";
	}



void AstPrinter::Visit(AstExprStmt *n,void *)
	{
	n->GetExpr()->ReceiveVisitor(this);
	os << ';' << endl;
	}



void AstPrinter::Visit(AstReturn *n,void *)
	{
	os << "return ";
	n->GetExpr()->ReceiveVisitor(this);
	os << ';' << endl;
	}



void AstPrinter::Visit(AstBind *n,void *)
	{
	os << "bind ";
	n->GetLhs()->ReceiveVisitor(this);
	os << " to ";
	n->GetRhs()->ReceiveVisitor(this);
	os << ';' << endl;
	}



