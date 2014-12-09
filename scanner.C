// =======================================
// Scanner.cpp
//
// Token classes and scanning routines
//
//
// =======================================

#include "libsrc/typeinfo.H"
#include "scanner.H"
#include "except.H"
#include <ctype.h>
#include "libsrc/RTTI.H"

ostream &operator <<(ostream &os,Token &t)
	{
    os << "line #" << t.GetLineNum() << ": ";
	PrintTokenType(t.GetTag(),os);
	switch(t.GetTag())
		{
		case TOK_IDENT:				// identifier
		case TOK_STRING_LIT:		// "string literal"
		case TOK_CHAR_LIT:			// 'c'
		case TOK_INT_LIT:			// 32
		case TOK_FLOAT_LIT:			// 98.6
        	os << " (" << t.GetLexeme() << ")";
			break;
		}
	return os;
	}



// for debugging
void PrintTokenType(TokenType t,ostream &os)
	{
	switch(t)
		{
		case TOK_CLASS:				// class
			os << "TOK_CLASS"; break;
		case TOK_INCLUDE:			// include
			os << "TOK_INCLUDE"; break;
		case TOK_ATTRIBUTE:			// attribute
			os << "TOK_ATTRIBUTE"; break;
		case TOK_METHOD:				// method
			os << "TOK_METHOD"; break;
		case TOK_RETURN:				// return
			os << "TOK_RETURN"; break;
		case TOK_OBJECT:				// object
			os << "TOK_OBJECT"; break;
		case TOK_SELF:				// self
			os << "TOK_SELF"; break;
		case TOK_SUPER:				// super
			os << "TOK_SUPER"; break;
		case TOK_MAIN:				// main
			os << "TOK_MAIN"; break;
		case TOK_IDENT:				// identifier
			os << "TOK_IDENT"; break;
		case TOK_STRING_LIT:			// "string literal"
			os << "TOK_STRING_LIT"; break;
		case TOK_CHAR_LIT:				// 'c'
			os << "TOK_CHAR_LIT"; break;
		case TOK_INT_LIT:			// 32
			os << "TOK_INT_LIT"; break;
		case TOK_FLOAT_LIT:			// 98.6
			os << "TOK_FLOAT_LIT"; break;
		case TOK_COLON:				// :
			os << "TOK_COLON"; break;
		case TOK_DOUBLE_COLON:		// ::
			os << "TOK_DOUBLE_COLON"; break;
		case TOK_OPEN_BRACE:			// {
			os << "TOK_OPEN_BRACE"; break;
		case TOK_CLOSE_BRACE:		// }
			os << "TOK_CLOSE_BRACE"; break;
		case TOK_OPEN_BRACKET:		// [
			os << "TOK_OPEN_BRACKET"; break;
		case TOK_CLOSE_BRACKET:		// ]
			os << "TOK_CLOSE_BRACKET"; break;
		case TOK_OPEN_PAREN:			// (
			os << "TOK_OPEN_PAREN" ; break;
		case TOK_CLOSE_PAREN:		// )
			os << "TOK_CLOSE_PAREN"; break;
		case TOK_SEMICOLON:			// ;
			os << "TOK_SEMICOLON"; break;
		case TOK_BIND:				// bind
			os << "TOK_BIND"; break;
		case TOK_TO:				// to
			os << "TOK_TO"; break;
		case TOK_EQUAL:				// ==
			os << "TOK_EQUAL"; break;
		case TOK_COMMA:				// ,
			os << "TOK_COMMA"; break;
		case TOK_VERTICAL_BAR:		// |
			os << "TOK_VERTICAL_BAR"; break;
		case TOK_PERIOD:	// .
			os << "TOK_PERIOD"; break;
		case TOK_BINOP:		// + - * /
			os << "TOK_BINOP"; break;
		case TOK_EOF:					// end-of-file
			os << "TOK_EOF"; break;
		}
	}



char *TokenTypeLabel(TokenType t)
	{
	switch(t)
		{
		case TOK_CLASS:				// class
			return "keyword \"class\"";
		case TOK_INCLUDE:			// include
			return "keyword \"include\"";
		case TOK_ATTRIBUTE:			// attribute
			return "keyword \"attribute\"";
		case TOK_METHOD:				// method
			return "keyword \"method\"";
		case TOK_RETURN:				// return
			return "operator ^";
		case TOK_OBJECT:				// object
			return "keyword \"object\"";
		case TOK_SELF:				// self
			return "keyword \"self\"";
		case TOK_SUPER:				// super
			return "keyword \"super\"";
		case TOK_MAIN:				// main
			return "keyword \"main\"";
		case TOK_IDENT:				// identifier
			return "identifier";
		case TOK_STRING_LIT:			// "string literal"
			return "string literal";
		case TOK_CHAR_LIT:				// 'c'
			return "character literal";
		case TOK_INT_LIT:			// 32
			return "integer literal";
		case TOK_FLOAT_LIT:			// 98.6
			return "float literal";
		case TOK_COLON:				// :
			return "':'";
		case TOK_DOUBLE_COLON:		// ::
			return "operator ::";
		case TOK_OPEN_BRACE:			// {
			return "'{'";
		case TOK_CLOSE_BRACE:		// }
			return "'}'";
		case TOK_OPEN_BRACKET:		// [
			return "'['";
		case TOK_CLOSE_BRACKET:		// ]
			return "']'";
		case TOK_OPEN_PAREN:			// (
			return "'('" ;
		case TOK_CLOSE_PAREN:		// )
			return "')'";
		case TOK_SEMICOLON:			// ;
			return "';'";
		case TOK_BIND:				// bind
			return "keyword \"bind\"";
		case TOK_TO:				// to
			return "keyword \"to\"";
		case TOK_EQUAL:				// ==
			return "operator ==";
		case TOK_COMMA:				// ,
			return "comma";
		case TOK_VERTICAL_BAR:		// |
			return "'|'";
		case TOK_PERIOD:
			return "period";
		case TOK_BINOP:
			return "binary operator";
		case TOK_EOF:					// end-of-file
			return "end of file";
		}
	}



// ****************************************
//			    Token methods
// ****************************************
Token::Token(TokenType Tag,int LineNum,char *Lexeme)
	: Tag(Tag), Lexeme(Duplicate(Lexeme)), LineNum(LineNum)
	{
	// ctor
	}



Token::Token(const Token &t)
	: Tag(t.Tag), Lexeme(Duplicate(t.Lexeme)), LineNum(t.LineNum)
	{
	// copy ctor
	}



Token::~Token()
	{
	// dtor
	delete [] Lexeme;
	}



const Token &Token::operator=(const Token &t)
	{
	Tag=t.Tag;
	delete [] Lexeme;
	Lexeme=Duplicate(t.Lexeme);
	LineNum=t.LineNum;
	return *this;
	}



// ****************************************
//			 TokenStream methods
// ****************************************
TokenStream &TokenStream::operator>>(Token &t)
	{
	if(IsPushed)
		{
		// A token was recently pushed back...use that
		t=PushedBack;
		IsPushed=false;
		}
	else// otherwise, extract from istream
		GetToken(t);

	return *this;
	}



void TokenStream::PushBack(const Token &t)
	{
	// Push token back into token stream

	if(IsPushed)
		throw INTERNAL_ERROR(__FILE__,__LINE__,
			"Pushing multiple tokens back into stream");

	PushedBack=t;
	IsPushed=true;
	}



void TokenStream::ScanStringLit(Token &t)
	{
	// This method scans the rest of a string literal after
	// the opening quote has already been read

	int BufferIndex=0, ic;
	while(1)
		{
		ic=is->get();
		Buffer[BufferIndex]=STATIC_CAST(char,ic);
		if(IsEofChar(ic))
			throw LEXICAL_ERROR(__FILE__,__LINE__,LineNum);
		if(Buffer[BufferIndex]=='"') break;
		if(Buffer[BufferIndex]=='\\')
			{
			ic=is->get();
			if(IsEofChar(ic)) throw LEXICAL_ERROR(__FILE__,__LINE__,LineNum);
			Buffer[BufferIndex]=STATIC_CAST(char,ic);
			switch(Buffer[BufferIndex])
				{
				case 'n':
					Buffer[BufferIndex]='\n';
					break;
				case 't':
					Buffer[BufferIndex]='\t';
                    break;
				}
			}
		++BufferIndex;
		}
	Buffer[BufferIndex]='\0';
	t=Token(TOK_STRING_LIT,LineNum,Buffer);
	}



void TokenStream::ScanCharLit(Token &t)
	{
	// This method scans a character literal after the opening
	// single-quote has already been read

	int ic=is->get();
	if(STATIC_CAST(char,ic)=='\\')
		{
		ic=is->get();
		switch(STATIC_CAST(char,ic))
			{
			case 'n':
				ic=STATIC_CAST(int,'\n');
				break;
			case 't':
				ic=STATIC_CAST(int,'\t');
				break;
			case 'r':
				ic=STATIC_CAST(int,'\r');
				break;
			case '\\':
				break;
			default:
				throw LEXICAL_ERROR(__FILE__,__LINE__,LineNum);
			}
		}
	Buffer[0]=STATIC_CAST(char,ic);
	ic=is->get();
	char c=STATIC_CAST(char,ic);
	if(IsEofChar(ic) || c!='\'')
		throw LEXICAL_ERROR(__FILE__,__LINE__,LineNum);
	Buffer[1]='\0';
	t=Token(TOK_CHAR_LIT,LineNum,Buffer);
	}



void TokenStream::ScanColonTokens(Token &t)
	{
	// This method scans the two tokens beginning with a colon
	// (: and ::).  The first colon has already been read
	// before this method was called

	int ic=is->get();
	char c=STATIC_CAST(char,ic);
	if(IsEofChar(ic)) throw LEXICAL_ERROR(__FILE__,__LINE__,LineNum);
	if(c==':')
		t=Token(TOK_DOUBLE_COLON,LineNum); // ::
	else
		{
		is->putback(c);
		t=Token(TOK_COLON,LineNum); // :
		}
	}



void TokenStream::ScanEquals(Token &t)
	{
	// This method scans the == token, but it assumes that the
	// first = character has already been read before this method
	// was called

	int ic=is->get();
	char c=STATIC_CAST(char,ic);
	if(IsEofChar(ic)) throw LEXICAL_ERROR(__FILE__,__LINE__,LineNum);
	if(c=='=')
		t=Token(TOK_EQUAL,LineNum); // ==
	else
		{
		is->putback(c);
		throw LEXICAL_ERROR(__FILE__,__LINE__,LineNum);
		}
	}



void TokenStream::ConsumeComment()
	{
	// This method consumes the rest of a comment after the initial
	// slash (/) character has been read

	int ic=is->get();
	char c=STATIC_CAST(char,ic);
	if(IsEofChar(ic)) throw LEXICAL_ERROR(__FILE__,__LINE__,LineNum);
	if(c=='/')
		{
		while(c!='\n' && c!='\r' && !IsEofChar(ic))
			{
			ic=is->get();
			c=STATIC_CAST(char,ic);
			}
		if(c=='\r' || c=='\n') ++LineNum;
		}
	else
		{
		is->putback(c);
		throw LEXICAL_ERROR(__FILE__,__LINE__,LineNum);
		}
	}



void TokenStream::GetToken(Token &t)
	{
	int ic;
	while(1)
		{
		SkipWhitespace();

		// Check for end-of-file
		if(!IsExtraPushed)
			{
			ic=is->get();
			if(IsEofChar(ic))
				{
				t=Token(TOK_EOF,LineNum);
				return;
				}
			is->putback(STATIC_CAST(char,ic));
			}

		// Read first character
		if(IsExtraPushed)
			{
			Buffer[0]=ExtraPushed;
			IsExtraPushed=false;
			}
		else
			Buffer[0]=STATIC_CAST(char,is->get());

		switch(Buffer[0])
			{
			case '"': // string literal
				ScanStringLit(t);
				break;
			case '\'': // character literal
				ScanCharLit(t);
				break;
			case ':': // could be : or :: or :=
				ScanColonTokens(t);
				break;
			case '=': // ==
				ScanEquals(t);
				break;
			case '/': // comment
				ic=is->peek();
				if(STATIC_CAST(char,ic)=='/')
					{
					ConsumeComment();
					continue;
					}
				else
					{
					Buffer[1]='\0';
					t=Token(TOK_BINOP,LineNum,Buffer);
					}
				break;
			default:  // could be integer, float, keyword, or identifier
				if(isdigit(Buffer[0]))
					ScanNumber(t);
				else
					ScanKeywordOrIdent(t);
				break;
			case '{':
				t=Token(TOK_OPEN_BRACE,LineNum);
				break;
			case '.':
				t=Token(TOK_PERIOD,LineNum);
				break;
			case '^':
				t=Token(TOK_RETURN,LineNum);
				break;
			case '}':
				t=Token(TOK_CLOSE_BRACE,LineNum);
				break;
			case '[':
				t=Token(TOK_OPEN_BRACKET,LineNum);
				break;
			case ']':
				t=Token(TOK_CLOSE_BRACKET,LineNum);
				break;
			case '(':
				t=Token(TOK_OPEN_PAREN,LineNum);
				break;
			case ')':
				t=Token(TOK_CLOSE_PAREN,LineNum);
				break;
			case ';':
				t=Token(TOK_SEMICOLON,LineNum);
				break;
			case ',':
				t=Token(TOK_COMMA,LineNum);
				break;
			case '|': // could be | or ||
				ic=is->peek();
				if(STATIC_CAST(char,ic)=='|')
					{
					is->get(Buffer[1]);
					Buffer[2]='\0';
					t=Token(TOK_BINOP,LineNum,Buffer);
					}
				else
					t=Token(TOK_VERTICAL_BAR,LineNum);
				break;
			case '<':
				ic=is->peek();
				switch(STATIC_CAST(char,ic))
					{
					case '<':
					case '=':
						is->get(Buffer[1]);
						Buffer[2]='\0';
						t=Token(TOK_BINOP,LineNum,Buffer);
						break;
					default:
						Buffer[1]='\0';
						t=Token(TOK_BINOP,LineNum,Buffer);
						break;
					}
				break;
			case '>':
				ic=is->peek();
				switch(STATIC_CAST(char,ic))
					{
					case '>':
					case '=':
						is->get(Buffer[1]);
						Buffer[2]='\0';
						t=Token(TOK_BINOP,LineNum,Buffer);
						break;
					default:
						Buffer[1]='\0';
						t=Token(TOK_BINOP,LineNum,Buffer);
						break;
					}
				break;
			case '&':
				ic=is->peek();
				if(STATIC_CAST(char,ic)=='&')
					{
					is->get(Buffer[1]);
					Buffer[2]='\0';
					t=Token(TOK_BINOP,LineNum,Buffer);
					}
				else throw LEXICAL_ERROR(__FILE__,__LINE__,LineNum);
				break;
			case '+':
			case '*':
			case '-':
			case '%':
				Buffer[1]='\0';
				t=Token(TOK_BINOP,LineNum,Buffer);
				break;
			}
		break;
		}
	}



void TokenStream::ScanNumber(Token &t)
	{
	// Could be an int or a float literal.  The first digit (or '-')
	// is in Buffer[0].

	int i=1;
	int ic=32;
	while(!IsEofChar(ic))
		{
		ic=is->get();
		Buffer[i]=STATIC_CAST(char,ic);
		if(is->bad()) throw READ_ERROR(__FILE__,__LINE__);
		if(!isdigit(Buffer[i])) break;
		++i;
		if(i>=MAX_TOKEN_LEN)
			throw LEXICAL_ERROR(__FILE__,__LINE__,LineNum);
		}
	if(IsEofChar(ic))
		{
		Buffer[i]='\0';
		t=Token(TOK_INT_LIT,LineNum,Buffer);
		return;
		}

	if(Buffer[i]=='.') // it's a float literal or an int at end of line
		{
		++i;
		if(i>=MAX_TOKEN_LEN)
			throw LEXICAL_ERROR(__FILE__,__LINE__,LineNum);

		ic=is->get();
		Buffer[i]=STATIC_CAST(char,ic);
		if(!isdigit(Buffer[i])) // period was not followed by more digits...
			{
			is->putback(Buffer[i]);
			IsExtraPushed=true; // because we can't push back two chars
			ExtraPushed='.';
			Buffer[i-1]='\0';
			t=Token(TOK_INT_LIT,LineNum,Buffer);
			return;
			}
		if(!IsEofChar(ic)) ++i;

		while(!IsEofChar(ic))
			{
			ic=is->get();
			Buffer[i]=STATIC_CAST(char,ic);
			if(is->bad()) throw READ_ERROR(__FILE__,__LINE__);
			if(!isdigit(Buffer[i])) break;
			++i;
			if(i>=MAX_TOKEN_LEN)
				throw LEXICAL_ERROR(__FILE__,__LINE__,LineNum);
			}
		if(IsEofChar(ic))
			{
			Buffer[i]='\0';
			t=Token(TOK_FLOAT_LIT,LineNum,Buffer);
			return;
			}

		is->putback(Buffer[i]);
		Buffer[i]='\0';
		t=Token(TOK_FLOAT_LIT,LineNum,Buffer);
		}
	else 	// it's an int literal
		{
		is->putback(Buffer[i]);
		Buffer[i]='\0';
		t=Token(TOK_INT_LIT,LineNum,Buffer);
		}
	}



void TokenStream::ScanKeywordOrIdent(Token &t)
	{
	// Could be a keyword or identifier.  The first letter is
	// in Buffer[0].

	// Make sure that Buffer[0] is a valid character
	if(!isalnum(Buffer[0]) && Buffer[0]!='_')
		throw LEXICAL_ERROR(__FILE__,__LINE__,LineNum);

	int i=1;
	int ic=32;
	while(!IsEofChar(ic))
		{
		ic=is->get();
		Buffer[i]=STATIC_CAST(char,ic);
		if(is->bad()) throw READ_ERROR(__FILE__,__LINE__);
		if(!isalnum(Buffer[i]) && Buffer[i]!='_') break;
		++i;
		if(i>=MAX_TOKEN_LEN)
			throw LEXICAL_ERROR(__FILE__,__LINE__,LineNum);
		}
	if(!IsEofChar(ic))
		is->putback(Buffer[i]);
	Buffer[i]='\0';

	// Look for string in list of keywords (binary search)
	char *KeywordArray[] = {
		"attribute",	"bind",			"class",
		"include",		"main",			"method",	//"new",
		"object",		"self",			"super", 	"to"
		};
	TokenType TokenArray[] = {
		TOK_ATTRIBUTE,	TOK_BIND, 		TOK_CLASS,
		TOK_INCLUDE,	TOK_MAIN,		TOK_METHOD,	//TOK_NEW,
		TOK_OBJECT,		TOK_SELF,		TOK_SUPER, 	TOK_TO
		};

	int From=0;
	int To=sizeof(KeywordArray)/sizeof(KeywordArray[0])-1;
	int Middle=To/2;
	while(From<=To)
		{
		int sc=strcmp(Buffer,KeywordArray[Middle]);
		if(!sc)
			{
			t=Token(TokenArray[Middle],LineNum);
			return;
			}
		if(sc<0) To=Middle-1;
		else From=Middle+1;
		Middle=(From+To)/2;
		}

	// Otherwise, it is an identifier
	t=Token(TOK_IDENT,LineNum,Buffer);
	}



void TokenStream::SkipWhitespace()
	{
	// This method skips whitespace and keeps count of newlines
	// in the process

	if(IsExtraPushed) return;

	int ic;
	char c=' ';
	while(isspace(c))
		{
		ic=is->get();
		if(IsEofChar(ic)) return;
		c=char(ic);
		if(c=='\r' || c=='\n') ++LineNum;
		}
	is->putback(c);
	}



TokenStream::TokenStream(istream *i)
	: is(i), IsPushed(false), LineNum(1), IsExtraPushed(false)
	{
	// ctor
	}



void TokenStream::AttachTo(istream *i)
	{
	is=i;
	LineNum=1;
	}




