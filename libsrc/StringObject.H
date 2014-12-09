// ==================================================================
// StringObject.H
// Classes that support character strings
//
// Ported to Solaris on 3/27/96
// ==================================================================

#ifndef INCL_STRING_OBJ_H
#define INCL_STRING_OBJ_H

#include <string.h>


class ostream;
class istream;
//class FileErrorException;



// ******************************************************************
//                          class StringObject
// ******************************************************************

class StringObject {
public:
  StringObject(); // default ctor
  StringObject(const char *iString); // ctor
  StringObject(const StringObject &OtherString); // copy ctor
  virtual ~StringObject(); // dtor
  virtual char &operator[](unsigned i);
  virtual const char &operator[](unsigned i) const;
  virtual StringObject *Duplicate() const;
  virtual int GetLength() const; // strlen
  virtual bool IsEmpty() const;
  virtual int GetDiskSize() const;
  virtual void Append(const StringObject *OtherString);
  virtual void Append(const char *);
  virtual void CopyFrom(const StringObject *OtherString);
  virtual void CopyFrom(const char *iString);
  virtual StringObject &operator=(const StringObject &OtherString);
  virtual char *GetCopy() const;
  virtual const char *AsCharArray() const;
  virtual int IsEqual(const StringObject *s) const;
  virtual bool IsEqual(const char *) const;
  virtual bool operator==(const StringObject &s) const;
  virtual void PrintOn(ostream &) const;
  virtual bool GetLineFrom(istream &); // throw(FileErrorException)
  virtual StringObject operator+(const StringObject &) const;
  virtual StringObject operator+(char) const;
  virtual StringObject operator+(int) const;
  virtual StringObject operator+(double) const;
  virtual StringObject operator+(long) const;
  virtual void tolower();
  virtual void toupper();
  virtual void DeleteAll(char victim);
  virtual void ReplaceAll(char preImage,char image);
  virtual bool IsSubstring(const StringObject &) const;
  virtual bool IsPrefixOf(const StringObject &) const;
  virtual int SubstringIndex(const StringObject &) const;
  virtual StringObject GetSubstring(int start,int end) const;
  virtual operator int() const; // computes hash value
  virtual int atoi() const;
private:
  char *TheData;
  int Size; // # chars allocated to TheData (not string-length!)
protected:
  char *GetTheData() const;
  int GetSize() const;
};


ostream &operator<<(ostream &,const StringObject &);
istream &operator>>(istream &,StringObject &);


#endif
