// ================================================
// Exception.C
//
// Bill Majoros -- 3/28/96
// ================================================

//#define BOOL_IS_ALREADY_DEFINED
#include "Exception.H"
#include <strstream.h>



// ********************************************************
//                    RootException methods
// ********************************************************
RootException::RootException(const StringObject &msg)
  : Message(msg)
{
  // ctor
}



RootException::RootException(const char *msg)
  : Message(msg)
{
  // ctor
}



RootException::RootException(const RootException &e) : Message(e.Message)
{
  // copy ctor
}



const StringObject &RootException::GetMessage() const
{
  return Message;
}



void RootException::SetMessage(const char *msg)
{
  // for my subclasses

  Message.CopyFrom(msg);
}






// *************************************************************
//                 ArrayIndexException methods
// *************************************************************
ArrayIndexException::ArrayIndexException(unsigned Index,const char *
  MoreMessage) : RootException("")
{
  // ctor

  ostrstream os;
  os << "Index out of bounds (" << Index << ')';
  if(MoreMessage)
    os << ' ' << MoreMessage;
  os << ends;

  SetMessage(os.str());
  os.rdbuf()->freeze(0);
}





// *************************************************************
//                   FileErrorException methods
// *************************************************************
FileErrorException::FileErrorException(const char *Filename,const char *Reason)
  : RootException("")
{
  // ctor

  ostrstream os;
  os << "Error handling file \"" << Filename << "\"";
  if(Reason)
    os << " : " << Reason;
  os << ends;

  SetMessage(os.str());
  os.rdbuf()->freeze(0);
}



// *************************************************************
//                  NotFoundException methods
// *************************************************************

NotFoundException::NotFoundException(const StringObject &ThingNotFound)
  : RootException(ThingNotFound+" not found")
{
  // ctor
}


