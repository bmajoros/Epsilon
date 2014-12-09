// ==================================================================
// StringObject.C
// Classes that support character strings
//
// Ported to Solaris on 3/27/96
// ==================================================================

#include "typeinfo.H"
#include "StringObject.H"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <iostream.h>
#include <strstream.h>
//#include "Exception.H"
#include "hash.H"


ostream &operator<<(ostream &os,const StringObject &aString)
  {
    aString.PrintOn(os);
    return os;
  }



istream &operator>>(istream &is,StringObject &aString)
{
  aString.GetLineFrom(is);
  return is;
}



// ******************************************************************
//                         StringObject methods
// ******************************************************************


StringObject::operator int() const
{
  // computes hash value

  return hashpjw(TheData);
}



bool StringObject::IsSubstring(const StringObject &s) const
{
  // This method returns true iff s is a substring of this StringObject.

  int LastIndex=GetLength()-1;
  int i;
  for(i=0 ; i<=LastIndex ; ++i)
    if(!strcmp(AsCharArray(),s.AsCharArray()))
      return true;
  return false;
}



bool StringObject::IsPrefixOf(const StringObject &s) const
{
  return s.SubstringIndex(*this)==0;
}



int StringObject::SubstringIndex(const StringObject &s) const
{
  // This method returns the zero-based index of substring s
  // within this string, or -1 if s is not a substring of this.

  int LastIndex=GetLength()-1;
  int i;
  for(i=0 ; i<=LastIndex ; ++i)
    if(!strcmp(AsCharArray(),s.AsCharArray()))
      return i;
  return -1;
}



StringObject StringObject::GetSubstring(int start,int end) const
{
  // This method returns a string the contents of which were taken
  // from the characters between and including positions "start" and
  // "end" in this StringObject.

  // Allocate a temporary buffer to copy the substring characters into
  int newStringLength=end-start+1;
  char *tempArray=new char[newStringLength+1];

  // Copy the substring characters into the temporary buffer
  const StringObject &self=*this;
  int source=start, dest=0;
  for( ; source<=end ; ++source, ++dest)
    tempArray[dest]=self[source];
  tempArray[newStringLength]='\0';
    
  // Convert the temporary buffer into a true StringObject
  StringObject returnValue=tempArray;
  delete [] tempArray;

  // Return the substring
  return returnValue;
}



void StringObject::tolower()
{
  // This method converts all upper-case characters occurring in this
  // string to lowercase.

  // Get a pointer to the first character in the string
  char *p=GetTheData();

  // Consider each character in turn, up to the terminating NULL
  for( ; *p ; ++p)
      *p=::tolower(*p);
}



void StringObject::toupper()
{
  // This method converts all lower-case characters occurring in this
  // string to uppercase.

  // Get a pointer to the first character in the string
  char *p=GetTheData();

  // Consider each character in turn, up to the terminating NULL
  for( ; *p ; ++p)
      *p=::toupper(*p);
}



void StringObject::DeleteAll(char victim)
{
  // This method deletes all occurrences of victim in this string by shifting
  // all characters to the right of each instances one space to the left.

  // Algorithm:
  //   Conceptually, we scan through the string from left to right, pushing
  //   non-victim characters into a queue, which is then emptied back out
  //   into the original array.  In truth, we eliminate the queue and simply
  //   maintain two pointers; one pointer to the character under consideration
  //   for "pushing into the queue," and another pointer to the next free
  //   location in the (original) array where the "pushed" character can be
  //   stored.

  char *dest=GetTheData(), *src=GetTheData();
  while(*src) // until NULL is reached
    {
      if(*src!=victim) // skip victim, but copy all others
	{
	  *dest=*src; // copy
	  ++dest; // advance pointer to next free slot
	}
      ++src;
    }
  *dest=*src; // copy the NULL
}



void StringObject::ReplaceAll(char preImage,char image)
{
  // This method replaces all occurrences of preImage in this string
  // with image

  // Get a pointer to the first character
  char *p=GetTheData();

  // Consider each character in turn
  for( ; *p ; ++p)
    if(*p==preImage)
      *p=image;
}



StringObject StringObject::operator+(const StringObject &s) const
{
  // This method forms the concatenation of this object with s and
  // returns that composite string, without changing this string
  // nor s.  This, together with the ctor StringObject(const char *),
  // allows expressions of the form:
  //    StringObject() + "Hello, " + "world!"
  // which evaluates to a StringObject containing "Hello, world!".

  StringObject r=*this;
  r.Append(&s);
  return r;
}



StringObject StringObject::operator+(char c) const
{
  ostrstream os;
  os << AsCharArray() << c << ends;
  StringObject r(os.str());
  os.rdbuf()->freeze(0);
  return r;
}



StringObject StringObject::operator+(int i) const
{
  ostrstream os;
  os << AsCharArray() << i << ends;
  StringObject r(os.str());
  os.rdbuf()->freeze(0);
  return r;
}



StringObject StringObject::operator+(long i) const
{
  ostrstream os;
  os << AsCharArray() << i << ends;
  StringObject r(os.str());
  os.rdbuf()->freeze(0);
  return r;
}



StringObject StringObject::operator+(double d) const
{
  ostrstream os;
  os << AsCharArray() << d << ends;
  StringObject r(os.str());
  os.rdbuf()->freeze(0);
  return r;
}



bool StringObject::GetLineFrom(istream &is) // throw(FileErrorException)
{
  // Reads an entire line from the input stream into this StringObject,
  // or returns false if end-of-file, or throws a FileErrorException if
  // an error has occurred.

  delete [] TheData;

  char Buffer[400];
  is.getline(Buffer,sizeof(Buffer));
//  if(is.bad())
//    throw FileErrorException("<don't know filename");
  if(is.eof()) 
    return false;

  TheData=new char[strlen(Buffer)+1];
  strcpy(TheData,Buffer);
  return true;
}



char *StringObject::GetTheData() const 
{ 
  return TheData; 
}



int StringObject::GetSize() const 
{ 
  return Size; 
}



const char *StringObject::AsCharArray() const 
{ 
  return TheData; 
}



int StringObject::IsEqual(const StringObject *s) const
{ 
  return !strcmp(TheData,s->TheData); 
}



bool StringObject::IsEqual(const char *p) const
{
  return !strcmp(TheData,p);
}



bool StringObject::operator==(const StringObject &s) const 
{ 
  return IsEqual(&s); 
}



char &StringObject::operator[](unsigned i)
{ 
  return  (i<Size) ? TheData[i] : TheData[0]; 
}



const char &StringObject::operator[](unsigned i) const
{
  StringObject &Self=(StringObject&) *this;
  return Self[i];
}



StringObject &StringObject::operator=(const StringObject &OtherString)
{ 
  CopyFrom(&OtherString); 
  return *this; 
}



int StringObject::GetDiskSize() const 
{ 
  return GetLength() + sizeof(int); 
}



void StringObject::PrintOn(ostream &os) const
  { 
    os << TheData; 
  }



StringObject::StringObject(const StringObject &OtherString) : TheData(0)
	{
	// copy ctor

	CopyFrom(&OtherString);
	}



StringObject::StringObject() : TheData(0)
	{
	// ctor

	CopyFrom(""); // if we let TheData=NULL, it causes a crash later on
	}




StringObject::StringObject(const char *iString) : TheData(0)
	{
	// ctor

	CopyFrom(iString);
	}




StringObject::~StringObject()
	{
	// dtor

	  delete [] TheData;
	}



StringObject *StringObject::Duplicate() const
	{
	  return new StringObject(TheData);
	}



bool StringObject::IsEmpty() const
{
  return GetLength()==0 ? true : false;
}



int StringObject::GetLength() const
{
  return TheData ? strlen(TheData) : 0;
}



void StringObject::Append(const char *otherString)
{
  char *NewBuffer;
  int NewSize;
  
  // allocate new buffer
  NewSize=GetLength() + strlen(otherString) + 1;
  NewBuffer=new char[NewSize];
  
  // fill in data in new buffer
  sprintf(NewBuffer,"%s%s",TheData,otherString);
  
  // replace the old buffer with the new buffer
  delete [] TheData;
  TheData=NewBuffer;
  Size=NewSize;
}



void StringObject::Append(const StringObject *OtherString)
{
  Append(OtherString->TheData);
}



char *StringObject::GetCopy() const
	{
	char *RetVal;

	RetVal=new char[GetLength()+1];
	strcpy(RetVal,TheData);

	return RetVal;
	}





/*
BOOL StringObject::SaveSelf(HFILE hFile)
	{
	int StrLen;

	// Writes string to disk (size followed by string, not
	// including NULL character).  Returns FALSE on error.

	// write string length
	StrLen=GetLength();
	if(_lwrite(hFile,&StrLen,sizeof(StrLen))==HFILE_ERROR)
   	return FALSE;

	// write the data
	if(StrLen)
		if(_lwrite(hFile,TheData,StrLen)==HFILE_ERROR)
			return FALSE;

   return TRUE;
	}




BOOL StringObject::LoadSelf(HFILE hFile)
	{
   // read string length
	if(_lread(hFile,&Size,sizeof(Size)) < sizeof(Size))
   	return FALSE;
   Size++; // NULL char not counted

	// Allocate string space
	delete [] TheData;
	TheData=new char[Size];

	// Read in the string
	if(_lread(hFile,TheData,Size-1) < Size-1)
		return FALSE;
	TheData[Size-1]='\0';

   return TRUE;
	}
*/




void StringObject::CopyFrom(const StringObject *OtherString)
	{
	  CopyFrom(OtherString->TheData);
	}




void StringObject::CopyFrom(const char *iString)
	{
	delete [] TheData;

	if(!iString)
	  {
	    TheData=0;
	    return;
	  }

	Size=strlen(iString)+1;
	TheData=new char[Size];
   	strcpy(TheData,iString);
	}



int StringObject::atoi() const
{
  return ::atoi(AsCharArray());
}



