// =======================================
// cmdline.cpp
//
// Class for processing the command line
//
//
// =======================================

#include "libsrc/typeinfo.H"
#include "cmdline.H"
#include <string.h>
#include <iostream.h>



// ****************************************
//			       globals
// ****************************************
extern CommandLine *CmdLine=0;



// ****************************************
//			  CommandLine methods
// ****************************************
CommandLine::CommandLine(int argc,char *argv[])
  : Debugging(false), Filename(0)
{
  // ctor : add initializers to ctor-initializer list when
  //        you add new command-line options, if necessary
  
  ProcessCmdLine(argc,argv);
}



void CommandLine::ProcessOption(char *option)
{
  // *** Add new command-line options inside this method
  
  if(!strcasecmp(option,"-debug")) 
    Debugging=true;
}



bool CommandLine::ProcessArg(char *arg)
{
  // Do not touch this method
  
  if(arg[0]=='-')
    {
      // Command-line option
      ProcessOption(arg);
    }
  else
    {
      if(Filename==NULL)
	Filename=arg;
      else
	return false;
    }

  return true;
}




CommandLine::~CommandLine()
{
  // dtor
  
  // Do not touch this method
  
  delete [] Filename;
}



void CommandLine::ProcessCmdLine(int argc,char *argv[])
{
  this->argc=argc;
  this->argv=argv;

  // Do not touch this method
  
  for(int i=1 ; i<argc ; i++)
    if(!ProcessArg(argv[i]))
      break;
  firstProgramArg=i;
}



char **CommandLine::getProgArgv()
{
  return argv+firstProgramArg;
}



int CommandLine::getProgArgc()
{
  return argc-firstProgramArg;
}




bool CommandLine::AreWeDebugging()
{
  return Debugging;
}



char *CommandLine::GetFilename()
{
  return Filename;
}




