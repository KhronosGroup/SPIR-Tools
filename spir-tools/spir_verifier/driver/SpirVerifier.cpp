//===------------------------ SpirVerifier.cpp ---------------------------===//
//
//                              SPIR Tools
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===---------------------------------------------------------------------===//

#include "validation/LLVMVersion.h"
#include "validation/SpirValidation.h"

#if LLVM_VERSION==3200
  #include "llvm/LLVMContext.h"
#else
  #include "llvm/IR/LLVMContext.h"
#endif
#include "llvm/ADT/OwningPtr.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/system_error.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;
using namespace SPIR;

static cl::opt<std::string>
InputFilename(cl::Positional, cl::desc("<input bitcode file>"),
    cl::init(""), cl::value_desc("filename"));

static cl::opt<bool> LITMode("LIT-test-mode", cl::init(false), cl::Hidden, cl::desc("Print output errors' names only, for LIT tests usage"));

const char *HelpMessage = "SPIR Verifier expects argument <path to file name>...\n";

int main(int argc, const char *argv[]) {
  cl::ParseCommandLineOptions(argc, argv, "SPIR verifier");

  if (InputFilename.empty()) {
    errs() << HelpMessage;
    return 1;
  }

  StringRef Path = InputFilename;
  LLVMContext Ctx;
  OwningPtr<MemoryBuffer> result;

  // Parse the bitcode file into a module.
  error_code ErrCode = MemoryBuffer::getFile(Path, result);

  if (!result.get()) {
    errs() << "Buffer Creation Error. " << ErrCode.message() << "\n";
    return 1;
  }

  std::string ErrMsg;
  Module *M = ParseBitcodeFile(result.get(), Ctx, &ErrMsg);
  if (!M) {
    outs() << "According to this SPIR Verifier, " << Path << " is an invalid SPIR module.\n";
    errs() << "Bitcode parsing error. " << ErrMsg << "\n";
    return 1;
  }

  // Run the verification pass, and report errors if necessary.
  SpirValidation Validation;
  Validation.runOnModule(*M);
  const ErrorPrinter *EP = Validation.getErrorPrinter();
  if (EP->hasErrors()) {
    outs() << "According to this SPIR Verifier, " << Path << " is an invalid SPIR module.\n";
    errs() << "The module contains the following errors:\n\n";
    EP->print(errs(), LITMode.getValue());
    return 1;
  }

  outs() << "According to this SPIR Verifier, " << Path << " is a valid SPIR module.\n";
  return 0;
}
