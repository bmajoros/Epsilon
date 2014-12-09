// ==================================================
// StringTokenizer.C
//
// Programmer     Date       Change
// ----------     ----       ------
// B. Majoros     2 /19/97   Creation
// ==================================================

#include "StringTokenizer.H"

// **************************************************
//             StringTokenizer methods
// **************************************************

StringTokenizer::StringTokenizer(const char *source,
  const char *whitespace)
  : source(source), whitespace(whitespace), p(source)
{
  // ctor
}



bool StringTokenizer::hasMoreTokens()
{
  skipWhiteSpace();
  return p && *p!='\0';
}



bool StringTokenizer::isWhiteSpace(char c)
{
  if(!*whitespace) return !isalpha(c);

  char *w=whitespace;
  while(*w)
    {
      if(c==*w) return true;
      ++w;
    }
  return false;
}


void StringTokenizer::skipWhiteSpace()
{
  while(*p && isWhiteSpace(*p))
    ++p;
}


const char *StringTokenizer::nextToken()
{
  skipWhiteSpace();
  
  char *q=buffer;
  while(*p && !isWhiteSpace(*p))
    {
      *q=*p;
      ++p, ++q;
    }
  
  *q='\0';

  return buffer;
}





