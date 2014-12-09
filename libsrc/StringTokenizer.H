// ==================================================
// StringTokenizer.H
//
// Programmer     Date       Change
// ----------     ----       ------
// B. Majoros     2 /19/97   Creation
// ==================================================

#ifndef INCL_STRING_TOKENIZER_H
#define INCL_STRING_TOKENIZER_H

#include "/home/bmajoros/c++/libsrc/bool.H"

// **************************************************
//               class StringTokenizer
// **************************************************

// Like the StringTokenizer in Java.  Specify the
// whitespace characters in the constructor, or
// omit the parameter altogether to get the default,
// which is space, tab, and newline/CR.  If you just
// want words made up of alphabetic characters, give
// an empty string ("") for the whitespace characters.
// Then anything not alphabetic will be considered
// whitespace.  This is faster than specifying all
// the non-alphabetic characters manually, because it
// relies on isalpha(), which I assume is a constant-
// time function.

class StringTokenizer {
  const char * const source;
  const char * const whitespace;
  const char *p;
  char buffer[300];
  bool isWhiteSpace(char);
  void skipWhiteSpace();
public:
  StringTokenizer(const char *source,const char *whitespace=" \t\n\r");
  bool hasMoreTokens();
  const char *nextToken();
};


#endif
