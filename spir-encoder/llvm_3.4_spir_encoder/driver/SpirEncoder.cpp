//===------------------------- SpirEncoder.cpp ---------------------------===//
//
//                              SPIR Tools
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===---------------------------------------------------------------------===//


#include "encoder/SpirBitcodeWriter.h"

#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/Support/SourceMgr.h>

#include <iostream>

using namespace llvm;
using namespace std;

// Command line arguments.
static cl::opt<std::string>
InputFilename(cl::Positional, cl::desc("<input bitcode file>"), cl::Required);
static cl::opt<std::string>
OutputFilename(cl::Positional, cl::desc("<output bitcode file>"), cl::Required);

int main(int argc, char *argv[])
{
  // Parse command line arguments
  cl::ParseCommandLineOptions(argc, argv, "SPIR Encoder");

  // Load bitcode module from file
  SMDiagnostic ErrInfo;
  LLVMContext& Context = getGlobalContext();
  Module *Input = ParseIRFile(InputFilename, ErrInfo, Context);
  if (!Input)
  {
    raw_os_ostream StdErr(cerr);
    ErrInfo.print(argv[0], StdErr);
    return 1;
  }

  // Open output file
  string ErrStr;
  raw_fd_ostream Output(OutputFilename.c_str(), ErrStr, sys::fs::F_None);
  if (!ErrStr.empty())
  {
    cerr << ErrStr << endl;
    return 1;
  }

  // Output re-encoded module
  SPIR::WriteBitcodeToFile_SPIR(Input, Output);

  return 0;
}
