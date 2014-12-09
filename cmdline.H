// =======================================
// cmdline.h
//
// Class for processing the command line
//
//
// =======================================

#ifndef INCL_CMDLINE_H
#define INCL_CMDLINE_H

#include "libsrc/StringObject.H"

// ****************************************
//	     class CommandLine
// ****************************************
class CommandLine 
{
  bool Debugging;
  char *Filename;
  int argc;
  char **argv;
  int firstProgramArg;

  void ProcessCmdLine(int argc,char *argv[]);
  bool ProcessArg(char *);
  void ProcessOption(char *); // <-- add new options inside here
public:
  CommandLine(int argc,char *argv[]);
  ~CommandLine();
  bool AreWeDebugging();
  char *GetFilename();
  char **getProgArgv();
  int getProgArgc();
};



// ****************************************
//		 globals
// ****************************************
extern CommandLine *CmdLine;


#endif
