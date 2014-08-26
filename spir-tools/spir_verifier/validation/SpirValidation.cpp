//===------------------------ SpirValidation.cpp -------------------------===//
//
//                              SPIR Tools
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===---------------------------------------------------------------------===//

#include "LLVMVersion.h"
#include "SpirValidation.h"
#include "SpirErrors.h"
#include "SpirIterators.h"

#if LLVM_VERSION==3200
  #include "llvm/Module.h"
  #include "llvm/Instructions.h"
  #include "llvm/DataLayout.h"
#else
  #include "llvm/IR/Module.h"
  #include "llvm/IR/Instructions.h"
  #include "llvm/IR/DataLayout.h"
#endif
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace SPIR {

//
// SpirValidation class public methods.
//

char SpirValidation::ID = 0;

SpirValidation::SpirValidation() : ModulePass(ID) {
}

SpirValidation::~SpirValidation() {
}

const char* SpirValidation::getPassName() const {
  return "Spir validation";
}

bool SpirValidation::runOnModule(Module& M) {
  // Holder for initialized data in the module
  DataHolder Data;

  // Initialize instruction verifiers.
  InstructionExecutorList iel;
  // Bitcast instruction verifier.
  VerifyBitcast vb(&ErrHolder);
  iel.push_back(&vb);
  // Call instruction verifier.
  VerifyCall vc(&ErrHolder);
  iel.push_back(&vc);
  // Instruction type verifier.
  VerifyInstructionType vit(&ErrHolder, &Data);
  iel.push_back(&vit);

  // Initialize function verifiers.
  FunctionExecutorList fel;
  // Function prototype verifier.
  VerifyFunctionPrototype vfp(&ErrHolder, &Data);
  fel.push_back(&vfp);
  // Kernel prototype verifier
  VerifyKernelPrototype vkp(&ErrHolder, &Data);
  fel.push_back(&vkp);

  // Initialize global variable verifiers
  GlobalVariableExecutorList gel;
  // Global variable verifier
  VerifyGlobalVariable vgv(&ErrHolder, &Data);
  gel.push_back(&vgv);

  // Initialize module verifiers.
  ModuleExecutorList mel;
  // Module triple and target data layout verifier.
  VerifyTripleAndDataLayout vtdl(&ErrHolder, &Data);
  mel.push_back(&vtdl);
  // Module metadata kernels verifier.
  VerifyMetadataKernels vkmd(&ErrHolder, &Data);
  mel.push_back(&vkmd);
  // Module OCL version verifier.
  VerifyMetadataVersions voclv(
    &ErrHolder, VerifyMetadataVersions::VERSION_OCL);
  mel.push_back(&voclv);
  // Module SPIR version verifier.
  VerifyMetadataVersions vspirv(
    &ErrHolder, VerifyMetadataVersions::VERSION_SPIR);
  mel.push_back(&vspirv);
  // Module metadata optional core features verifier.
  VerifyMetadataCoreFeatures vmdcf(&ErrHolder, &Data);
  mel.push_back(&vmdcf);
  // Module metadata KHR extensions verifier.
  VerifyMetadataKHRExtensions vmdext(&ErrHolder, &Data);
  mel.push_back(&vmdext);
  // Module metadata compiler options verifier.
  VerifyMetadataCompilerOptions vmdco(&ErrHolder, &Data);
  mel.push_back(&vmdco);

  // Initialize basic block iterator.
  BasicBlockIterator BBI(iel);

  // Initialize function iterator.
  FunctionIterator FI(fel, &BBI);

  // Initialize global variable iterator.
  GlobalVariableIterator GI(gel);

  // Initialize module iterator.
  ModuleIterator MI(mel, &FI, &GI);

  // Run validation.
  MI.execute(M);

  return false;
}


} // End SPIR namespace
 extern "C" {
  ModulePass *createSpirValidationPass() {
    return new SPIR::SpirValidation();
 }
}
