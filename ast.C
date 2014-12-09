// =======================================
// AST.cpp
//
// Abstract syntax trees
//
//
// =======================================

#include "libsrc/typeinfo.H"
#include "ast.H"
#include "tempmgr.H"
#include <strstream.h>
#include "execute.H"
#include "except.H"


RTTI_DEFINE_SUBCLASS(AstExpr,link_node)
RTTI_DEFINE_SUBCLASS(AstSend,AstExpr)
RTTI_DEFINE_SUBCLASS(AstCoalescedSend,AstSend)
RTTI_DEFINE_SUBCLASS(AstEquals,AstExpr)
RTTI_DEFINE_SUBCLASS(AstNew,AstExpr)
RTTI_DEFINE_SUBCLASS(AstIdent,AstExpr)
RTTI_DEFINE_SUBCLASS(AstClassName,AstIdent)
RTTI_DEFINE_SUBCLASS(AstSuper,AstIdent)
RTTI_DEFINE_SUBCLASS(AstLiteral,AstExpr)
RTTI_DEFINE_SUBCLASS(AstCharLiteral,AstLiteral)
RTTI_DEFINE_SUBCLASS(AstStringLiteral,AstLiteral)
RTTI_DEFINE_SUBCLASS(AstBlockLiteral,AstLiteral)
RTTI_DEFINE_SUBCLASS(AstFloatLiteral,AstLiteral)
RTTI_DEFINE_SUBCLASS(AstIntLiteral,AstLiteral)
RTTI_DEFINE_SUBCLASS(AstStmt,link_node)
RTTI_DEFINE_SUBCLASS(AstExprStmt,AstStmt)
RTTI_DEFINE_SUBCLASS(AstReturn,AstStmt)
RTTI_DEFINE_SUBCLASS(AstBind,AstStmt)



// ****************************************
//	   LexicalAddress methods
// ****************************************
LexicalAddress::LexicalAddress(int Depth,int Position) 
: Depth(Depth), Position(Position)
{
  // ctor
}



LexicalAddress::LexicalAddress() : Depth(-1), Position(-1)
{
  // ctor
}



int LexicalAddress::GetDepth() const
{
  return Depth;
}



int LexicalAddress::GetPosition() const
{
  return Position;
}



void LexicalAddress::SetPosition(int i)
{
  Position=i;
}



ostream &operator<<(ostream &os,const LexicalAddress &la)
{
  os<<"(: "<<la.GetDepth()<<' '<<la.GetPosition()<<')';
  return os;
}



// ****************************************
//	       SyntaxForest methods
// ****************************************
void SyntaxForest::Append(AstStmt *s)
{
  MyStatements.list_append(s);
}



void SyntaxForest::ReceiveVisitor(TreeVisitor *v,void *p)
{
  MyStatements.reset_seq();
  AstStmt *ThisStmt;
  while(ThisStmt=DYNAMIC_CAST_PTR(AstStmt,MyStatements.sequential()))
    ThisStmt->ReceiveVisitor(v,p);
}



void SyntaxForest::Execute(RunTimeEnvironment &env)
{
  MyStatements.reset_seq();
  AstStmt *ThisStmt;
  while(ThisStmt=DYNAMIC_CAST_PTR(AstStmt,MyStatements.sequential()))
    {
      ThisStmt->Execute(env);
      if(env.AreWeReturning())
	return; // An Epsilon "return" statement has been executed
    }
}



Object *SyntaxForest::GetValue(RunTimeEnvironment &env)
{
  // Since a block evaluates to its last statement (when that
  // statement is an AstExprStmt), we peek into the last
  // statement, check whether it is an AstExprStmt, and if
  // so, get its value and return it.  Otherwise, evaluate to nil.
  // We also return nil when the list of statements is empty.
  
  // If this SyntaxForest represents a method rather than a block,
  // then GetValue() is not to be used; when a return statement is
  // absent, we return self by default.
  
  // Finally, note that if a return statement occurs, the last statement
  // in the block may not be executed before control returns to the
  // caller.  This is fine, because in that case, this routine's return
  // value won't actually be used.  The fact that it is called at all
  // is also OK, because we will just retrieve the contents of an
  // uninitialized temporary, which is guaranteed to be nil.
  
  // Find the last statement
  AstStmt *last=DYNAMIC_CAST_PTR(AstStmt,MyStatements.GetLast());
  
  // See if it is an AstExprStmt
  AstExprStmt *expr_stmt=DYNAMIC_CAST_PTR(AstExprStmt,last);
  
  // If so, return that statement's value
  if(expr_stmt) return expr_stmt->GetValue(env);
  
  // Otherwise, return nil
  return nil;
}



// ****************************************
//	       AstExpr methods
// ****************************************

void AstExpr::SetTemporary(const LexicalAddress &temp)
{
  MyTemporary=temp;
}



LexicalAddress AstExpr::GetTemporary() const
{
  return MyTemporary;
}



Object *AstExpr::GetValue(RunTimeEnvironment &env)
{
  return env.GetObject(MyTemporary,OBJ_LOCAL);
}



void AstExpr::ReceiveVisitor(TreeVisitor *v,void *p)
{
  v->Visit(this,p);
}



void AstExpr::StoreInTemporary(Object *obj,RunTimeEnvironment &env)
{
  env.StoreObject(MyTemporary,OBJ_LOCAL,obj);
}



// ****************************************
//		AstSend methods
// ****************************************

AstSend::AstSend(AstExpr *Recipient,const char *Msg)
  : Recipient(Recipient), MessageName(Duplicate(Msg))
{
  // ctor
}



AstSend::~AstSend()
{
  delete [] MessageName;
}



void AstSend::AppendParm(AstExpr *e)
{
  Parameters.list_append(e);
}



linked_list &AstSend::GetParmNodes()
{
  return Parameters;
}



AstExpr *AstSend::GetRecipientNode()
{
  return Recipient;
}



const char *AstSend::GetMessageName() const
{
  return MessageName;
}



void AstSend::AppendToName(char *p)
{
  ostrstream os;
  
  os << MessageName << p << ends;
  delete [] MessageName;
  MessageName=os.str();
}



void AstSend::ReceiveVisitor(TreeVisitor *v,void *p)
{
  v->Visit(this,p);
}



MethodBody *AstSend::GetMethod(Object *recipient)
{
  // This method gets the MethodBody which will handle the message-
  // send; it accounts for the case where we are sending a message
  // via the superclass (i.e., the use of the "super" keyword)
  
  // First, find the MethodNameNode for the message being sent
  MethodNameNode *mnn;
  AstSuper *super_node=DYNAMIC_CAST_PTR(AstSuper,Recipient);
  if(super_node)
    // The message is being sent to the "super" keyword, so don't
    // use polymorphism -- send it using the superclass of the
    // lexically-enclosing class for this statement
    mnn=super_node->GetSuperClass()->FindMethod(MessageName);
  else
    // Just a normal message send -- use polymorphism
    mnn=recipient->GetClass()->FindMethod(MessageName);
  
  // Signal an error if the message has no handler
  if(!mnn)
    {
      ostrstream os;
      
      Class_Object *co=DYNAMIC_CAST_PTR(Class_Object,recipient);
      if(co)
	os << "class \"" << co->WhoDoYouRepresent()->GetName()
	   << "\" did not understand " << MessageName << ends;
      else
	os << recipient->GetClass()->GetName() <<
	  " object did not understand " << MessageName << ends;
      throw RUN_TIME_ERROR(__FILE__,__LINE__,os.str());
    }
  
  // Make sure the method body has been defined
  if(!mnn->GetBody())
    {
      ostrstream os;
      os << "No body defined for method " <<
	recipient->GetClass()->GetName() <<	"::" << MessageName << ends;
      throw RUN_TIME_ERROR(__FILE__,__LINE__,os.str());
    }
  
  
  // Return the method which will handle this message
  return mnn->GetBody();
}



void AstSend::Evaluate(RunTimeEnvironment &env)
{
  // Evaluate the recipient
  Recipient->Evaluate(env);
  if(env.AreWeReturning())
    return; // An Epsilon "return" statement has been executed
  Object *recipient=Recipient->GetValue(env);
  
  // Get the body of the method that will handle this message
  MethodBody *body=GetMethod(recipient);
  
  // Create an activation record (but do not push it yet)
  ActivationRecord *ar=new ActivationRecord(body->GetARsize(),nil);
  
  // Pass the implicit argument, "self"
  ar->SetEntry(0,recipient);
  
  // Evaluate the arguments (left-to-right), and store them in
  // the activation record
  Parameters.reset_seq();
  AstExpr *this_parm;
  int LexicalPosition=1;
  while(this_parm=DYNAMIC_CAST_PTR(AstExpr,Parameters.sequential()))
    {
      this_parm->Evaluate(env);
      if(env.AreWeReturning())
	return; // An Epsilon "return" statement has been executed
      Object *parm_value=this_parm->GetValue(env);
      ar->SetEntry(LexicalPosition,parm_value);
      ++LexicalPosition;
    }
  
  // Push the newly created activation record
  env.GetStack().PushAR(ar);
  
  // Call the method
  Object *ReturnValue=recipient; // return self by default
  body->Call(env);
  if(env.AreWeReturning())
    {
      // An Epsilon "return" statement has been executed
      if(env.GetReturnAR()==env.GetStack().PeekTop())
	{
	  // The "return" statement was a return from this function,
	  // so we must stop the stack unwinding from going any further
	  env.DoneReturning();
	  ReturnValue=env.GetReturnValue();
	}
    }
  
  // Pop the activation record
  env.PopAR();
  
  // Store the return value in a temporary
  StoreInTemporary(ReturnValue,env);
}




// ****************************************
//	 AstCoalescedSend methods
// ****************************************

AstCoalescedSend::AstCoalescedSend(AstExpr *Recipient,const char *Msg,
				   AstExpr *previousMsg)
  : AstSend(Recipient,Msg), previousMsg(previousMsg)
{
  // ctor
}



AstCoalescedSend::~AstCoalescedSend()
{
}



void AstCoalescedSend::ReceiveVisitor(TreeVisitor *v,void *p)
{
  v->Visit(this,p);
}



void AstCoalescedSend::Evaluate(RunTimeEnvironment &env)
{
  // Evaluate the previousMsg
  previousMsg->Evaluate(env);
  if(env.AreWeReturning())
    return; // An Epsilon "return" statement has been executed
  Object *recipient=Recipient->GetValue(env);

  // Get the body of the method that will handle this message
  MethodBody *body=GetMethod(recipient);
  
  // Create an activation record (but do not push it yet)
  ActivationRecord *ar=new ActivationRecord(body->GetARsize(),nil);
  
  // Pass the implicit argument, "self"
  ar->SetEntry(0,recipient);
  
  // Evaluate the arguments (left-to-right), and store them in
  // the activation record
  Parameters.reset_seq();
  AstExpr *this_parm;
  int LexicalPosition=1;
  while(this_parm=DYNAMIC_CAST_PTR(AstExpr,Parameters.sequential()))
    {
      this_parm->Evaluate(env);
      if(env.AreWeReturning())
	return; // An Epsilon "return" statement has been executed
      Object *parm_value=this_parm->GetValue(env);
      ar->SetEntry(LexicalPosition,parm_value);
      ++LexicalPosition;
    }
  
  // Push the newly created activation record
  env.GetStack().PushAR(ar);
  
  // Call the method
  Object *ReturnValue=recipient; // return self by default
  body->Call(env);
  if(env.AreWeReturning())
    {
      // An Epsilon "return" statement has been executed
      if(env.GetReturnAR()==env.GetStack().PeekTop())
	{
	  // The "return" statement was a return from this function,
	  // so we must stop the stack unwinding from going any further
	  env.DoneReturning();
	  ReturnValue=env.GetReturnValue();
	}
    }
  
  // Pop the activation record
  env.PopAR();
  
  // Store the return value in a temporary
  StoreInTemporary(ReturnValue,env);
}



// ****************************************
//	      AstEquals methods
// ****************************************
AstEquals::AstEquals(AstExpr *lhs,AstExpr *rhs)
  : lhs(lhs), rhs(rhs)
{
  // ctor
}



AstExpr *AstEquals::GetLhs()
{
  return lhs;
}



AstExpr *AstEquals::GetRhs()
{
  return rhs;
}



void AstEquals::ReceiveVisitor(TreeVisitor *v,void *p)
{
  v->Visit(this,p);
}



void AstEquals::Evaluate(RunTimeEnvironment &env)
{
  lhs->Evaluate(env);
  if(env.AreWeReturning())
    return;// an Epsilon "return" statement has been executed
  
  rhs->Evaluate(env);
  if(env.AreWeReturning())
    return;// an Epsilon "return" statement has been executed
  
  Object *value;
  if(lhs->GetValue(env)==rhs->GetValue(env))
    value=true_object; // was true_class->Instantiate();
  else
    value=false_object; // was false_class->Instantiate();
  StoreInTemporary(value,env);
}



// ****************************************
//	       AstNew methods
// ****************************************
void AstNew::ReceiveVisitor(TreeVisitor *v,void *p)
{
  v->Visit(this,p);
}



void AstNew::Evaluate(RunTimeEnvironment &env)
{
  // This only actually runs the GC if memory is low:
  env.RunGarbageCollector();
  
  Object *value=TheClass->Instantiate();
  StoreInTemporary(value,env);
  env.RegisterGarbage(value);
}



// ****************************************
//	    AstIdent methods
// ****************************************
AstIdent::AstIdent(int LexicalDepth,int LexicalPosition,StorageClass c)
  : la(LexicalDepth,LexicalPosition), MyStorageClass(c)
{
  // ctor
}



void AstIdent::ReceiveVisitor(TreeVisitor *v,void *p)
{
  v->Visit(this,p);
}



void AstIdent::Evaluate(RunTimeEnvironment &env)
{
  // do nothing
}



Object *AstIdent::GetValue(RunTimeEnvironment &env)
{
  return env.GetObject(la,MyStorageClass);
}



void AstIdent::BindTo(Object *obj,RunTimeEnvironment &env)
{
  env.StoreObject(la,MyStorageClass,obj);
}



LexicalAddress AstIdent::GetLexicalAddress() const
{
  return la;
}



StorageClass AstIdent::GetStorageClass() const
{
  return MyStorageClass;
}



// ****************************************
// 	    AstClassName methods
// ****************************************
AstClassName::AstClassName(Class *TheClass)
  : TheClass(TheClass),
    AstIdent(0,0,OBJ_GLOBAL) // bogus parameters
{
  // ctor
}



void AstClassName::ReceiveVisitor(TreeVisitor *v,void *p)
{
  v->Visit(this,p);
}



Class *AstClassName::GetClass()
{
  return TheClass;
}



void AstClassName::Evaluate(RunTimeEnvironment &env)
{
  StoreInTemporary(TheClass->AsFirstClassObject(),env);
}



Object *AstClassName::GetValue(RunTimeEnvironment &env)
{
  return AstExpr::GetValue(env);// skip over AstIdent::GetValue()
}




// ****************************************
//	      AstSuper methods
// ****************************************
AstSuper::AstSuper(Class *SuperClass,int LexicalDepth)
  : TheSuperClass(SuperClass), AstIdent(LexicalDepth,0,OBJ_LOCAL)
{
  // ctor
}



Class *AstSuper::GetSuperClass()
{
  return TheSuperClass;
}




/*Object *AstSuper::GetValue(RunTimeEnvironment &env)
  {
  // Here we skip over AstIdent::GetValue and
  // jump up to AstExpr::GetValue, because whereas
  // AstIdent doesn't copy the denoted object into
  // a temporary, AstSuper creates an adapter
  // object and therefore needs to store that object
  // in a temporary.  AstExpr::GetValue gets the
  // object from the temporary.
  
  return AstExpr::GetValue(env);
  }
  */




void AstSuper::ReceiveVisitor(TreeVisitor *v,void *p)
{
  v->Visit(this,p);
}



/*void AstSuper::Evaluate(RunTimeEnvironment &env)
  {
  Object *self=AstIdent::GetValue(env);
  Object *super_obj=
  new Super_Object(self->GetClass()->GetSuperClass(),self);
  StoreInTemporary(super_obj,env);
  env.RegisterGarbage(super_obj);
  }
  */



// ****************************************
//	      AstLiteral methods
// ****************************************
void AstLiteral::ReceiveVisitor(TreeVisitor *v,void *p)
{
  v->Visit(this,p);
}



// ****************************************
//	   AstCharLiteral methods
// ****************************************
char AstCharLiteral::GetChar() const
{
  return TheChar;
}



AstCharLiteral::AstCharLiteral(char c) : TheChar(c)
{
  // ctor
}



void AstCharLiteral::ReceiveVisitor(TreeVisitor *v,void *p)
{
  v->Visit(this,p);
}



void AstCharLiteral::Evaluate(RunTimeEnvironment &env)
{
  // Evaluate to a Char_Object
  Char_Object *obj=
    DYNAMIC_CAST_PTR(Char_Object,char_class->Instantiate());
  obj->SetValue(TheChar);
  
  // Store the Char_Object in a temporary
  StoreInTemporary(obj,env);
  env.RegisterGarbage(obj);
}



// ****************************************
//	  AstStringLiteral methods
// ****************************************
AstStringLiteral::AstStringLiteral(const char *s)
  : TheString(Duplicate(s))
{
  // ctor
}



AstStringLiteral::~AstStringLiteral()
{
  delete [] TheString;
}



const char *AstStringLiteral::GetString() const
{
  return TheString;
}



void AstStringLiteral::ReceiveVisitor(TreeVisitor *v,void *p)
{
  v->Visit(this,p);
}



void AstStringLiteral::Evaluate(RunTimeEnvironment &env)
{
  // Evaluate to an String_Object
  String_Object *obj=
    DYNAMIC_CAST_PTR(String_Object,string_class->Instantiate());
  obj->SetValue(TheString);
  
  // Store the String_Object in a temporary
  StoreInTemporary(obj,env);
  env.RegisterGarbage(obj);
}



// ****************************************
//	   AstBlockLiteral methods
// ****************************************
AstBlockLiteral::AstBlockLiteral(SyntaxForest *f,int NumParms,int NumLocals)
  : MyStatements(f), NumParms(NumParms), NumLocals(NumLocals)
{
  // ctor
}



AstBlockLiteral::~AstBlockLiteral()
{
  delete MyStatements;
}



int AstBlockLiteral::GetNumParms() const
{
  return NumParms;
}



int AstBlockLiteral::GetNumLocals() const
{
  return NumLocals;
}



int AstBlockLiteral::GetARsize() const
{
  return NumParms+NumLocals;
}



SyntaxForest &AstBlockLiteral::GetStatements()
{
  return *MyStatements;
}



void AstBlockLiteral::ReceiveVisitor(TreeVisitor *v,void *p)
{
  v->Visit(this,p);
}



void AstBlockLiteral::Evaluate(RunTimeEnvironment &env)
{
  Object *obj;
  obj=new Block_Object(env.GetStack().PeekTop(),this,block_class);
  
  StoreInTemporary(obj,env);
  env.RegisterGarbage(obj);
}



// ****************************************
//	  AstFloatLiteral methods
// ****************************************

AstFloatLiteral::AstFloatLiteral(float f)
  : TheFloat(f)
{
  // ctor
}



void AstFloatLiteral::Negate()
{
  TheFloat=-TheFloat;
}




float AstFloatLiteral::GetFloat() const
{
  return TheFloat;
}



void AstFloatLiteral::ReceiveVisitor(TreeVisitor *v,void *p)
{
  v->Visit(this,p);
}



void AstFloatLiteral::Evaluate(RunTimeEnvironment &env)
{
  // Evaluate to a Float_Object
  Float_Object *obj=
    DYNAMIC_CAST_PTR(Float_Object,float_class->Instantiate());
  obj->SetValue(TheFloat);
  
  // Store the Float_Object in a temporary
  StoreInTemporary(obj,env);
  env.RegisterGarbage(obj);
}



// ****************************************
//	    AstIntLiteral methods
// ****************************************

AstIntLiteral::AstIntLiteral(int i)
  : TheInt(i)
{
  // ctor
}



void AstIntLiteral::Negate()
{
  TheInt=-TheInt;
}




int AstIntLiteral::GetInt() const
{
  return TheInt;
}



void AstIntLiteral::ReceiveVisitor(TreeVisitor *v,void *p)
{
  v->Visit(this,p);
}



void AstIntLiteral::Evaluate(RunTimeEnvironment &env)
{
  // Evaluate to an Int_Object
  Int_Object *obj=
    DYNAMIC_CAST_PTR(Int_Object,int_class->Instantiate());
  obj->SetValue(TheInt);
  
  // Store the Int_Object in a temporary
  StoreInTemporary(obj,env);
  env.RegisterGarbage(obj);
}



// ****************************************
//		AstStmt methods
// ****************************************
void AstStmt::ReceiveVisitor(TreeVisitor *v,void *p)
{
  v->Visit(this,p);
}



// ****************************************
//	       AstExprStmt methods
// ****************************************

AstExprStmt::AstExprStmt(AstExpr *e)
  : MyExpression(e)
{
  // ctor
}



AstExpr *AstExprStmt::GetExpr()
{
  return MyExpression;
}



Object *AstExprStmt::GetValue(RunTimeEnvironment &env)
{
  return MyExpression->GetValue(env);
}



void AstExprStmt::ReceiveVisitor(TreeVisitor *v,void *p)
{
  v->Visit(this,p);
}



void AstExprStmt::Execute(RunTimeEnvironment &env)
{
  MyExpression->Evaluate(env);
}



// ****************************************
//	      AstReturn methods
// ****************************************
AstReturn::AstReturn(AstExpr *e,int NestingLevel)
  : MyExpression(e), NestingLevel(NestingLevel)
{
  // ctor
}



AstExpr *AstReturn::GetExpr()
{
  return MyExpression;
}



void AstReturn::ReceiveVisitor(TreeVisitor *v,void *p)
{
  v->Visit(this,p);
}



void AstReturn::Execute(RunTimeEnvironment &env)
{
  // To execute a return statement, we must tell the RunTimeEnvironment
  // to go into "returning" mode; it must know which AR is associated
  // with the function that is returning, because we will end up
  // unwinding the stack until we pop that AR.
  
  // Now, if this return statement is inside a block, then the AR
  // associated with the function that is returning is _not_ the AR
  // on the top of the stack; that would be the AR for the block
  // containing the return statement, which is not what we want.  The
  // AR that we need is the one representing the function containing
  // this block.  However, this block could be nested several blocks
  // deep, so we need to know the block's nesting level, and we follow
  // that many static chains to get to the proper AR.
  
  // Finally, there is the case where the return statement is not inside
  // a block, but just inside a function.  In that case, the desired AR
  // _is_ the AR on top of the stack, so that case is trivial, and is
  // in fact entirely subsumed by the code that handles the harder case.
  
  // First, however, we must evaluate the expression being returned
  // (and since that may cause some other return statement to execute,
  // check it for returning)
  MyExpression->Evaluate(env);
  if(env.AreWeReturning())
    return;// an Epsilon "return" statement has been executed
  
  // Now perform the return
  ActivationRecord *ar=env.GetStack().PeekTop();
  for(int i=0 ; i<NestingLevel ; i++)
    {
      // Follow the next static chain
      Block_Object *this_block=
	DYNAMIC_CAST_PTR(Block_Object,ar->GetSelf());
      ar=this_block->GetStaticChain();
    }
  env.Return(ar,MyExpression->GetValue(env));
}




// ****************************************
//	      AstBind methods
// ****************************************
AstBind::AstBind(AstIdent *lhs,AstExpr *rhs)
  : lhs(lhs), rhs(rhs)
{
  // ctor
}



AstExpr *AstBind::GetRhs()
{
  return rhs;
}



AstIdent *AstBind::GetLhs()
{
  return lhs;
}



void AstBind::ReceiveVisitor(TreeVisitor *v,void *p)
{
  v->Visit(this,p);
}



void AstBind::Execute(RunTimeEnvironment &env)
{
  rhs->Evaluate(env);
  if(env.AreWeReturning())
    return;// an Epsilon "return" statement has been executed
  
  lhs->BindTo(rhs->GetValue(env),env);
}



