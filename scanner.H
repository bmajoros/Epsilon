// =======================================
// Scanner.h
//
// Token classes and scanning routines
//
//
// =======================================

#ifndef INCL_SCANNER_H
#define INCL_SCANNER_H

#include <iostream.h>


#define MAX_TOKEN_LEN 200


// ****************************************
//			    enum TokenType
// ****************************************

// I enumerate all of the types of tokens,
// or "terminals" in the grammar.

enum TokenType {
	TOK_CLASS,				// class
	TOK_INCLUDE,			// include
	TOK_ATTRIBUTE,			// attribute
	TOK_METHOD,				// method
	TOK_RETURN,				// return
	TOK_OBJECT,				// object
	TOK_SELF,				// self
	TOK_SUPER,				// super
	TOK_MAIN,				// main
	TOK_IDENT,				// identifier
	TOK_STRING_LIT,			// "string literal"
	TOK_CHAR_LIT,			// 'c'
	TOK_INT_LIT,			// 32
	TOK_FLOAT_LIT,			// 98.6
	TOK_COLON,				// :
	TOK_DOUBLE_COLON,		// ::
	TOK_OPEN_BRACE,			// {
	TOK_CLOSE_BRACE,		// }
	TOK_OPEN_BRACKET,		// [
	TOK_CLOSE_BRACKET,		// ]
	TOK_OPEN_PAREN,			// (
	TOK_CLOSE_PAREN,		// )
	TOK_SEMICOLON,			// ;
	TOK_BIND,				// bind
	TOK_TO,					// to
	TOK_EQUAL,				// ==
	TOK_COMMA,				// ,
	TOK_VERTICAL_BAR,		// |
	TOK_PERIOD,				// .
	TOK_BINOP,				// + - * /
	TOK_EOF					// end-of-file
	};



void PrintTokenType(TokenType t,ostream &os); // for debugging
char *TokenTypeLabel(TokenType t);


// ****************************************
//			    class Token
// ****************************************

// I represent a single syntactic unit in
// the source code, such as a keyword or
// operator.  I am produced by the Scanner
// and consumed by the Parser.

class Token {
	TokenType Tag;
	char *Lexeme;	// actual text from the source code
	int LineNum;
public:
	Token(TokenType Tag=TOK_EOF,int LineNum=0,char *Lexeme="");
	Token(const Token &);
	~Token();
	const Token &operator=(const Token &);
	TokenType GetTag() const { return Tag; }
	const char *GetLexeme() const { return Lexeme; }
    int GetLineNum() const { return LineNum; }
	};


ostream &operator <<(ostream &os,Token &t);



// ****************************************
//			  class TokenStream
// ****************************************

// I transform a stream of characters into
// a stream of tokens, which are consumed
// by the Parser.  I am sometimes referred
// to as the "scanner" because of what I do.
// I also keep track of the current line number
// in the input file.

class TokenStream {
	istream *is; // could be an ifstream or a strstream for all I know
	Token PushedBack; // Token that was pushed back via PushBack()
	bool IsPushed; // Has a token been pushed back? (can push only one)
	bool IsExtraPushed; // Has a second character been "pushed back" ?
	char ExtraPushed; // A second character that has been pushed back
	int LineNum; // current source code line number
	void GetToken(Token &);
	void ScanNumber(Token &);
	void ScanKeywordOrIdent(Token &);
	void ScanStringLit(Token &t);
	void ScanCharLit(Token &t);
	void ScanColonTokens(Token &t);
	void ScanEquals(Token &t);
    void ConsumeComment();
	char Buffer[MAX_TOKEN_LEN]; // for temporary, internal use
	bool IsEofChar(int ic) { return ic==EOF || ic==0; }
	void SkipWhitespace();
public:
	TokenStream(istream *i=0);
	void AttachTo(istream *i); // ifstream or strstream
	TokenStream &operator>>(Token &);
	void PushBack(const Token &t); // Push 1 token back into token stream
	int GetLineNum() const { return LineNum; }
	};



#endif
