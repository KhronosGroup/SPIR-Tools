//===------------------------ SpirValidation.h ---------------------------===//
//
//                              SPIR Tools
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===---------------------------------------------------------------------===//

#include "LLVMVersion.h"
#include "SpirIterators.h"
#include "SpirErrors.h"
#include "SpirTables.h"

#if LLVM_VERSION==3200
  #include "llvm/Module.h"
  #include "llvm/Function.h"
  #include "llvm/Instruction.h"
  #include "llvm/Instructions.h"
  #include "llvm/Value.h"
#else
  #include "llvm/IR/Module.h"
  #include "llvm/IR/Function.h"
  #include "llvm/IR/Instruction.h"
  #include "llvm/IR/Instructions.h"
  #include "llvm/IR/Value.h"
#endif


#include <sstream>
#include <algorithm>

namespace SPIR {

//
// Iterator classes (impl).
//

void BasicBlockIterator::execute(const llvm::BasicBlock& BB) {
  // Run over all instructions in basic block.
  BasicBlock::const_iterator ii = BB.begin(), ie = BB.end();
  for (; ii != ie; ii++) {
    // For each instruction apply all executors from the list.
    const Instruction *I = &*ii;
    InstructionExecutorList::iterator iei = m_iel.begin(), iee = m_iel.end();
    for (; iei != iee; iei++) {
      (*iei)->execute(I);
    }
  }
}

void FunctionIterator::execute(const llvm::Function& F) {
  // Apply all executors from the list on the given function.
  FunctionExecutorList::iterator fei = m_fel.begin(), fee = m_fel.end();
  for (; fei != fee; fei++) {
    (*fei)->execute(&F);
  }
  // If basic block iterator available
  // Apply it for each basic block in the given function.
  if (m_bbi) {
    Function::const_iterator bi = F.begin(), be = F.end();
    for (; bi != be; bi++) {
      const BasicBlock *BB = &*bi;
      m_bbi->execute(*BB);
    }
  }
}

void GlobalVariableIterator::execute(const llvm::GlobalVariable& GV) {
  // Apply all executors from the list on the given global variable.
  GlobalVariableExecutorList::iterator gei = m_gvel.begin(),
                                       gee = m_gvel.end();
  for (; gei != gee; gei++) {
    (*gei)->execute(&GV);
  }
}

void ModuleIterator::execute(const llvm::Module& M) {
  // Apply all executors from the list on the given module.
  ModuleExecutorList::iterator mei = m_mel.begin(), mee = m_mel.end();
  for (; mei != mee; mei++) {
    (*mei)->execute(&M);
  }
  // If function iterator available
  // Apply it for each function in the given module.
  if (m_fi) {
    Module::const_iterator fi = M.begin(), fe = M.end();
    for (; fi != fe; fi++) {
      const Function *F = &*fi;
      m_fi->execute(*F);
    }
  }
  // If global variable iterator available
  // Apply it for each global variable in the given module.
  if (m_gi) {
    Module::const_global_iterator gi = M.global_begin(), ge = M.global_end();
    for (; gi != ge; gi++) {
      const GlobalVariable *GV = &*gi;
      m_gi->execute(*GV);
    }
  }
}

void MetaDataIterator::execute(const llvm::MDNode& Node) {
  // Apply all executors from the list on the given Metadata node.
  for (unsigned int i=0; i<Node.getNumOperands(); ++i) {
    const llvm::MDNode *Op = dyn_cast<llvm::MDNode>(Node.getOperand(i));
    if (Op) {
      MDNodeExecutorList::iterator nei = m_nel.begin(), nee = m_nel.end();
      for (; nei != nee; nei++) {
        (*nei)->execute(Op);
      }
    }
  }
}

//
// Utility functions.
//

/// @brief Check if given name is valid according to given valid list.
/// @param Name given name to validate.
/// @param ValidList given valid list to validate against.
/// @returns true if name is valid, false otherwise.
static bool isValidNameOf(StringRef Name, const char *ValidList[], unsigned len) {
  for (unsigned i=0; i<len; i++) {
    StringRef candidate(ValidList[i]);
    if (Name == candidate)
      return true;
  }
  return false;
}

/// @brief Check if given name start with valid prefix according to given valid list.
/// @param Name given name to validate.
/// @param ValidList given valid list to validate against.
/// @returns size of valid prefix, 0 if no valid prefix.
static int hasPrefixValidNameOf(StringRef Name, const char *ValidList[], unsigned len) {
  for (unsigned i=0; i<len; i++) {
    StringRef candidate(ValidList[i]);
    if (Name.startswith(candidate))
      return candidate.size();
  }
  return 0;
}

// Returns true if the string is a legal name.
static bool isValidTypeName(StringRef TyName) {
  // Check if type start with a prefix of ignored type
  if (hasPrefixValidNameOf(TyName, g_ignored_ocl_types,
                                   g_ignored_ocl_types_len)) {
    return true;
  }
  // Check if type is a valid OCL type.
  if( isValidNameOf(TyName, g_valid_ocl_opaque_types,
                            g_valid_ocl_opaque_types_len) ) {
    return true;
  }
  // Check if type is a valid vector element type.
  int prefixLen = hasPrefixValidNameOf(TyName,
    g_valid_ocl_vector_element_types, g_valid_ocl_vector_element_types_len);
  if (prefixLen) {
    TyName = TyName.substr(prefixLen);
    // Check for vector length suffix.
    prefixLen = hasPrefixValidNameOf(TyName,
      g_valid_vector_type_lengths, g_valid_vector_type_lengths_len);
    TyName = TyName.substr(prefixLen);
  } else {
    // Check if type is a valid scalar primitive type.
    prefixLen = hasPrefixValidNameOf(TyName,
      g_valid_ocl_primitives, g_valid_ocl_primitives_len);
    TyName = TyName.substr(prefixLen);
  }
  // '*' is the only possible suffix now (spaces are ignored).
    for (unsigned int pos = 0; pos < TyName.size(); ++pos) {
      if (TyName[pos] == ' ')
        continue;
      if (TyName[pos] == '[') {
        // Array types are ignored. TODO: are they allowed?
        return true;
      }
      if (TyName[pos] != '*')
        return false;
    }
    return true;
}

/// @brief Check if given Value is an MDNode of given Type name.
/// @param V given value to validate.
/// @param type given type name validate against.
/// @returns true if match, false otherwise.
static bool isMDNodeTypeOf(const Value *V, StringRef type) {
  if (!isa<MDNode>(V))
    return false;

  const MDNode *N = cast<MDNode>(V);
  const MDString *StringVal = dyn_cast<MDString>(N->getOperand(0));
  return StringVal && StringVal->getString() == type;
}

static bool isAllowedIntrinsic(StringRef FName) {
  bool IsValidIntrinsic = hasPrefixValidNameOf(FName,
    g_valid_instrinsic, g_valid_instrinsic_len) != 0;
  bool IsIgnoredIntrinsic = hasPrefixValidNameOf(FName,
    g_ignored_instrinsic, g_ignored_instrinsic_len) != 0;
  return IsValidIntrinsic || IsIgnoredIntrinsic;
}

//
// LLVM types validaiton
//
static bool isValidPrimitiveType(Type *Ty, DataHolder *D, bool isPointer) {
  return
    (Ty->isDoubleTy() && D->HasDoubleFeature) ||
    (Ty->isHalfTy() && (D->HASFp16Extension || isPointer)) ||
    Ty->isFloatTy() ||
    Ty->isIntegerTy(1) || Ty->isIntegerTy(8) || Ty->isIntegerTy(16) ||
    Ty->isIntegerTy(32) || Ty->isIntegerTy(64) || Ty->isVoidTy();
}

static bool isIgnoredPrimitiveType(Type *Ty) {
  return Ty->isMetadataTy();
}

static bool isValidVectorElementType(Type *Ty, DataHolder *D, bool isPointer) {
  return
    (Ty->isDoubleTy() && D->HasDoubleFeature) ||
    (Ty->isHalfTy() && (D->HASFp16Extension || isPointer)) ||
    Ty->isFloatTy() ||
    Ty->isIntegerTy(8) || Ty->isIntegerTy(16) || Ty->isIntegerTy(32) ||
    Ty->isIntegerTy(64);
}

static bool isValidVectorElementsNum(unsigned ElementsNum) {
  return ElementsNum == 2 || ElementsNum == 3 || ElementsNum == 4 ||
    ElementsNum == 8 || ElementsNum == 16;
}

static bool isValidOCLOpaqueType(const StructType *Ty, DataHolder *D) {
  return
    isValidNameOf(Ty->getName(), g_valid_llvm_opaque_types,
                                 g_valid_llvm_opaque_types_len) ||
    (isValidNameOf(Ty->getName(), g_valid_llvm_image_types,
                                  g_valid_llvm_image_types_len) &&
     (!D || D->HasImageFeature));
}

static bool isValidType(Type *Ty, DataHolder *D,
                        bool isBoolAllowed, bool isOpaqueAllowed,
                        bool isBoolVecAllowed, bool isPointer) {
  // Check if it is a pointer
  if (Ty->isPointerTy()) {
    return isValidType(Ty->getContainedType(0), D,
      true, true, isBoolVecAllowed, true);
  }

  // Check if it is an Array
  if (Ty->isArrayTy()) {
    return isValidType(Ty->getContainedType(0), D,
      false, false, isBoolVecAllowed, isPointer);
  }

  // Check if it is a Structure
  if (const StructType *STy = dyn_cast<StructType>(Ty)) {
    if (STy->isOpaque()) {
      if (!isOpaqueAllowed) {
        return false;
      }
      // Check of it is allowed OpenCL opaque types
      if (isValidOCLOpaqueType(STy, D)) {
        return true;
      }
      // Ignor other opaque type
      // TODO: check if it is supported by SPIR
      return true;
    }
    for (unsigned i=0; i<STy->getStructNumElements(); i++) {
      if (!isValidType(STy->getStructElementType(i), D,
          false, false, isBoolVecAllowed, isPointer)) {
        return false;
      }
    }
    return true;
  }

  // Check if it is a vector
  if (const VectorType *VTy = dyn_cast<VectorType>(Ty)) {
    if (!isValidVectorElementsNum(VTy->getNumElements())) {
      return false;
    }
    if (isBoolVecAllowed) {
      return isValidType(VTy->getElementType(), D,
        isBoolAllowed, false, isBoolVecAllowed, isPointer);
    }
    return isValidType(VTy->getElementType(), D,
      false, false, isBoolVecAllowed, isPointer);
  }

  // Check if it is a valid primitive type
  if (!isBoolAllowed) {
    return isValidVectorElementType(Ty, D, isPointer);
  }
  return isValidPrimitiveType(Ty, D, isPointer) || isIgnoredPrimitiveType(Ty);

}

static std::string MapLLVMToOCL(Type *Ty, bool &Ignore) {
  // Check if it is a pointer
  if (Ty->isPointerTy()) {
    return MapLLVMToOCL(Ty->getContainedType(0), Ignore) + "*";
  }

  // Check if it is an Array
  if (Ty->isArrayTy()) {
    // Do not know how to handle Array, mark this check as ignored
    Ignore = true;
    return "";
  }

  // Check if it is a Structure
  if (const StructType *STy = dyn_cast<StructType>(Ty)) {
    if (STy->isOpaque() && isValidOCLOpaqueType(STy, NULL)) {
      std::string TypeName = STy->getStructName();
      TypeName = TypeName.substr(StringRef(g_llvm_opaque_prefix).size());
      return TypeName;
    }
    std::string TypeName = STy->getStructName();
    TypeName = "struct " + TypeName.substr(StringRef("struct.").size());
    // TODO: maybe it is better to just Ignore this check
    Ignore = true;
    return TypeName;
  }

  // Check if it is a vector
  if (const VectorType *VTy = dyn_cast<VectorType>(Ty)) {
    std::stringstream SS;
    SS << MapLLVMToOCL(VTy->getElementType(), Ignore) << VTy->getNumElements();
    return SS.str();
  }

  // Check if it is a valid primitive type
  if (Ty->isFloatTy()) {
    return "float";
  }
  if (Ty->isDoubleTy()) {
    return "double";
  }
  if (Ty->isHalfTy()) {
    return "half";
  }
  if (Ty->isIntegerTy(1)) {
    return "bool";
  }
  if (Ty->isIntegerTy(8)) {
    return "char";
  }
  if (Ty->isIntegerTy(16)) {
    return "short";
  }
  if (Ty->isIntegerTy(32)) {
    return "int";
  }
  if (Ty->isIntegerTy(64)) {
    return "long";
  }
  // Any other type is not allowed, return empty string
  return "";
}

static bool isValidMapOCLToLLVM(StringRef TyName, Type *Ty, DataHolder *D) {
  // Check if type start with a prefix of ignored type
  if (hasPrefixValidNameOf(TyName, g_ignored_ocl_types,
                                   g_ignored_ocl_types_len)) {
    return true;
  }

  bool Ignore = false;
  std::string ConvertedName = MapLLVMToOCL(Ty, Ignore);

  if (Ignore) {
    // Do not know how to convert, ignore this check.
    return true;
  }

  if (TyName.find("void") != std::string::npos) {
    // TODO: Can 'void' be verified?
    return true;
  }

  std::string StrName = TyName;
  // Handle special type conversions
  if( isValidNameOf(TyName, g_valid_ocl_opaque_types,
                            g_valid_ocl_opaque_types_len) ) {
    if (TyName == "sampler_t") {
      StrName = "int"; //"i32"
    }
    else if (TyName == "size_t" || TyName == "ptrdiff_t" ||
        TyName == "uintptr_t" || TyName == "intptr_t") {
      if (D->Is32Bit) {
        StrName = "int"; //"i32"
      }
      else {
        StrName = "long";
      }
    }
    else {
      StrName += "*";
    }
  }
  else if (TyName.startswith("unsigned")) {
    StrName = TyName.substr(StringRef("unsigned").size());
  }
  else if (TyName.startswith("u")) {
    StrName = TyName.substr(StringRef("u").size());
  }

  return (StrName == ConvertedName);
}

static bool isValidAddrSpaceCast(const BitCastInst *BI) {
  const PointerType *LHS = dyn_cast<PointerType>(BI->getDestTy()),
                    *RHS = dyn_cast<PointerType>(BI->getSrcTy());
  if (!LHS || !RHS)
    return true;

  const unsigned int DstAddress = LHS->getAddressSpace();
  const unsigned int SrcAddress = RHS->getAddressSpace();
  return (SrcAddress == DstAddress);
}

static bool isValidAddrSpaceCast(const User *V) {

  for (unsigned i = 0; i < V->getNumOperands(); i++) {
    // If the operand is not a constant expression, we will (or already did),
    // visit it as a command from the main block iteration.
    if (const ConstantExpr *CE = dyn_cast<ConstantExpr>(V->getOperand(i)))
      isValidAddrSpaceCast(CE);
  }

  const ConstantExpr *CE = dyn_cast<ConstantExpr>(V);
  if (!CE)
    return true;

  const PointerType *PTy  = dyn_cast<PointerType>(CE->getType());
  if (!PTy)
    return true;

  const unsigned int DstAddress = PTy->getAddressSpace();
  if (Instruction::BitCast == CE->getOpcode()) {
    const PointerType *STy = dyn_cast<PointerType>(CE->getOperand(0)->getType());
    if (STy) {
      const unsigned int SrcAddress = STy->getAddressSpace();
      return (SrcAddress == DstAddress);
    }
  }

  return true;
}

static bool isValidAddrSpace(unsigned AddSpace) {
  assert(g_valid_address_space_len == 4 &&
    "In SPIR 1.2 we have only 4 address spaces");
  return AddSpace < g_valid_address_space_len;
}

static bool isValidOCLVersion(unsigned Major, unsigned Minor) {
  return (Major == 1 && Minor == 2);
}

static bool isValidSPIRVersion(unsigned Major, unsigned Minor) {
  return (Major == 1 && Minor == 2);
}

static bool isValidMemfence(unsigned Val) {
  return (Val == 1 || Val == 2 || Val == 3);
}

static bool isValidLinkageType(llvm::GlobalValue::LinkageTypes LT) {
  return LT == llvm::GlobalValue::ExternalLinkage
      || LT == llvm::GlobalValue::PrivateLinkage
      || LT == llvm::GlobalValue::InternalLinkage
      || LT == llvm::GlobalValue::AvailableExternallyLinkage;
}

//
// Verify Executor classes (impl).
//
void VerifyCall::execute(const Instruction *I) {
  const CallInst *CI = dyn_cast<CallInst>(I);
  if (!CI)
    return;

  // Verify that this call is not indirect.
  const Function *F = CI->getCalledFunction();
  if (!F) {
    ErrCreator->addError(ERR_INVALID_INDIRECT_CALL, I);
    return;
  }

  if (!F->isDeclaration()) {
    // Verify that this call has valid calling convention.
    if (CI->getCallingConv() != CallingConv::SPIR_KERNEL &&
        CI->getCallingConv() != CallingConv::SPIR_FUNC) {
        ErrCreator->addError(ERR_INVALID_CALLING_CONVENTION, I);
    }
  }

  // Verify valid memfence for synchronize functions
  if (hasPrefixValidNameOf(F->getName(), g_valid_sync_bi, g_valid_sync_bi_len)) {
    if (CI->getNumArgOperands() != 1) {
      ErrCreator->addError(ERR_INVALID_MEM_FENCE, I);
    }
    ConstantInt *MemfenceVal = dyn_cast<ConstantInt>(CI->getOperand(0));
    if (!MemfenceVal ||
      !isValidMemfence((unsigned)MemfenceVal->getZExtValue())) {
      ErrCreator->addError(ERR_INVALID_MEM_FENCE, I);
    }
  }

  // Verify that this call is valid intrinsic.
  if (F->isIntrinsic() && !isAllowedIntrinsic(F->getName())) {
    ErrCreator->addError(ERR_INVALID_INTRINSIC, I);
  }
}

void VerifyBitcast::execute(const Instruction *I) {
  if (const BitCastInst *BI = dyn_cast<BitCastInst>(I)) {
    // Verify that this bitcast is not adress space cast.
    if (!isValidAddrSpaceCast(BI))
      ErrCreator->addError(ERR_INVALID_ADDR_SPACE_CAST, I);
  }

  for (unsigned i = 0; i < I->getNumOperands(); i++) {
    // Verify that each opernad is not const expression adress space cast.
    if (const User *Usr = dyn_cast<User>(I->getOperand(i)))
      if (!isValidAddrSpaceCast(Usr) )
        ErrCreator->addError(ERR_INVALID_ADDR_SPACE_CAST, I);
  }
}

void VerifyInstructionType::execute(const Instruction *I) {
  Type *Ty = I->getType();
  bool isValid = true;
  switch(I->getOpcode()) {
  case Instruction::ICmp:
  case Instruction::FCmp:
    isValid = isValidType(Ty, Data, true, true, true, false);
    break;
  default:
    isValid = isValidType(Ty, Data, true, true, false, false);
    break;
  }
  if (!isValid)
    ErrCreator->addError(ERR_INVALID_LLVM_TYPE, Ty, I);
}

void VerifyFunctionPrototype::execute(const Function *F) {
  if (!F->isDeclaration()) {
    // Verify calling convention for user defined functions
    if (F->getCallingConv() != CallingConv::SPIR_KERNEL &&
        F->getCallingConv() != CallingConv::SPIR_FUNC)
          ErrCreator->addError(ERR_INVALID_CALLING_CONVENTION, F->getName());
  }
  Function::const_arg_iterator ai = F->arg_begin(), ae = F->arg_end();
  for (; ai != ae; ai++) {
    Type *Ty = ai->getType();
    if (!isValidType(Ty, Data, true, true, false, false)) {
      ErrCreator->addError(ERR_INVALID_LLVM_TYPE, Ty, F->getName());
    }
  }
  // Verify function linkage
  if (!isValidLinkageType(F->getLinkage())) {
    ErrCreator->addError(ERR_INVALID_LINKAGE_TYPE, F->getName());
  }
  // Verify function return type.
  if (!isValidType(F->getReturnType(), Data, true, true, false, false)) {
    ErrCreator->addError(
      ERR_INVALID_LLVM_TYPE, F->getReturnType(), F->getName());
  }
}

void VerifyKernelPrototype::execute(const Function *F) {
  // detect kernel by looking at the calling convention
  if (F->getCallingConv() != CallingConv::SPIR_KERNEL)
    return;

  // check arguments
  Function::const_arg_iterator ai = F->arg_begin(), ae = F->arg_end();
  for (; ai != ae; ai++) {
    Type *Ty = ai->getType();

    // kernel arguments shall not be pointers to pointers
    if (Ty->isPointerTy() && Ty->getPointerElementType()->isPointerTy()) {
      ErrCreator->addError(ERR_KERNEL_ARG_PTRPTR, Ty, F->getName());
    }

    // kernel arguments shall not be pointers to a private addrspace
    if (Ty->isPointerTy() && Ty->getPointerAddressSpace() == PRIVATE_ADDR_SPACE) {
      ErrCreator->addError(ERR_KERNEL_ARG_AS0, Ty, F->getName());
    }
  }

  // the return type shall be void
  if (!F->getReturnType()->isVoidTy()) {
    ErrCreator->addError(
      ERR_INVALID_KERNEL_RETURN_TYPE, F->getReturnType(), F->getName());
  }
}

void VerifyGlobalVariable::execute(const GlobalVariable *GV) {
  // Verify variable linkage
  if (!isValidLinkageType(GV->getLinkage())) {
    ErrCreator->addError(ERR_INVALID_LINKAGE_TYPE, GV->getName());
  }

  // check the global variable address space
  switch (GV->getType()->getPointerAddressSpace()) {
  case CONSTANT_ADDR_SPACE:
    // constant address space: everything OK
    break;
  case LOCAL_ADDR_SPACE: {
    // local address space:
    // it is a function-scope variable,
    // must contain a prefix that is equal to the name of a function
    // and should be used only in it
    for (Value::const_use_iterator ib = GV->use_begin(), ie = GV->use_end(); ib != ie; ++ib) {
      if (const Instruction *Inst = dyn_cast<Instruction>(*ib)) {
        const Function * func = Inst->getParent()->getParent();
        if (!(GV->getName().startswith(func->getName().str() + "."))) {
           ErrCreator->addError(ERR_INVALID_GLOBAL_AS3_VAR, GV);
           break;
        }
      }
    }
    break;
  }
  default:
    ErrCreator->addError(ERR_INVALID_GLOBAL_VAR_ADDRESS_SPACE, GV);
    break;
  }
}

void VerifyTripleAndDataLayout::execute(const Module *M) {
  StringRef Triple(M->getTargetTriple());
  StringRef DL(M->getDataLayout());

  bool isTriple32 = (Triple == SPIR32_TRIPLE);
  bool isTriple64 = (Triple == SPIR64_TRIPLE);
  bool isDataLayout32 = (DL == SPIR32_DATA_LAYOUT);
  bool isDataLayout64 = (DL == SPIR64_DATA_LAYOUT);

  // Verify triple is valid.
  if (!(isTriple32 || isTriple64)) {
    ErrCreator->addError(ERR_INVALID_TRIPLE, Triple);
  }
  else {
    // Valid Triple, set sizeof pointer according to it.
    Data->Is32Bit = isTriple32;
  }

  // Verify target data layout is valid.
  if (!(isDataLayout32 || isDataLayout64)) {
    ErrCreator->addError(ERR_INVALID_DATA_LAYOUT, DL);
  }

  // Verify triple matches target data layout.
  if ((isTriple32 && isDataLayout64) || (isTriple64 && isDataLayout32)) {
    std::string S = Triple.str() + "\n" + DL.str();
    ErrCreator->addError(ERR_MISMATCH_TRIPLE_AND_DATA_LAYOUT, S);
  }
}

void VerifyMetadataArgAddrSpace::execute(const llvm::MDNode *Node) {
  if (!isMDNodeTypeOf(Node, KERNEL_ARG_ADDR_SPACE))
    return;

  WasFound = true;
  // Verify that kernel arg base type metadata list is valid.
  for (unsigned i=1; i<Node->getNumOperands(); i++) {
    ConstantInt *AddrSpace = dyn_cast<ConstantInt>(Node->getOperand(i));
    if (!AddrSpace) {
      ErrCreator->addError(ERR_INVALID_METADATA_KERNEL_INFO, Node);
      continue;
    }
    unsigned AddrSpaceVal = (unsigned)AddrSpace->getZExtValue();
    if (!isValidAddrSpace(AddrSpaceVal)) {
      ErrCreator->addError(ERR_INVALID_ADDR_SPACE, Node);
    }
    // Verify that LLVM Type of relevant function prototype argument
    // has same address space.
    const unsigned ArgIndex = i-1;
    Type *Ty = Func->getFunctionType()->getParamType(ArgIndex);
    unsigned ArgAddrSpace = 0;
    if (PointerType *PTy = dyn_cast<PointerType>(Ty)) {
      ArgAddrSpace = PTy->getAddressSpace();
    }
    if (ArgAddrSpace != AddrSpaceVal) {
      ErrCreator->addError(ERR_INVALID_METADATA_KERNEL_INFO, Node);
      ErrCreator->addError(ERR_MISMATCH_METADATA_ADDR_SPACE, Ty,
                                                            Func->getName());
    }
  }
}

void VerifyMetadataArgType::execute(const llvm::MDNode *Node) {
  if (isMDNodeTypeOf(Node, KERNEL_ARG_TY)) {
    WasFound = true;
  }
}

void VerifyMetadataArgBaseType::execute(const llvm::MDNode *Node) {
  if (!isMDNodeTypeOf(Node, KERNEL_ARG_BASE_TY))
    return;

  WasFound = true;
  // Verify that kernel arg base type metadata list is valid.
  for (unsigned i=1; i<Node->getNumOperands(); i++) {
    MDString *StringValue = dyn_cast<MDString>(Node->getOperand(i));
    if (!StringValue) {
      ErrCreator->addError(ERR_INVALID_METADATA_KERNEL_INFO, Node);
      continue;
    }
    StringRef S = StringValue->getString();
    if (!isValidTypeName(S)) {
      ErrCreator->addError(ERR_INVALID_OCL_TYPE, Node);
      continue;
    }
    // Verify that LLVM Type of relevant function prototype argument
    // has same address space.
    const unsigned ArgIndex = i-1;
    Type *Ty = Func->getFunctionType()->getParamType(ArgIndex);
    if (!isValidMapOCLToLLVM(S, Ty, Data)) {
      ErrCreator->addError(ERR_MISMATCH_OCL_AND_LLVM_TYPES, Node);
      ErrCreator->addError(ERR_MISMATCH_OCL_AND_LLVM_TYPES, Ty,
                                                            Func->getName());
    }
  }
}

void VerifyMetadataKernel::execute(const llvm::MDNode *Node) {
  // Verify that first operand is a valid function type.
  if (Node->getNumOperands() < 1) {
    ErrCreator->addError(ERR_INVALID_METADATA_KERNEL, Node);
    return;
  }
  Function *F = dyn_cast<Function>(Node->getOperand(0));
  if (!F) {
    ErrCreator->addError(ERR_INVALID_METADATA_KERNEL, Node);
    return;
  }
  if (F->getCallingConv() != CallingConv::SPIR_KERNEL) {
    ErrCreator->addError(ERR_INVALID_METADATA_KERNEL, Node);
  }
  if (FoundMap.count(F)) {
    // Function has two kernel metadata nodes
    // Mark both of them as invalid metadata kernel
    ErrCreator->addError(ERR_INVALID_METADATA_KERNEL, FoundMap[F]);
    ErrCreator->addError(ERR_INVALID_METADATA_KERNEL, Node);
  }
  // Insert <F, Node> pair to the found map.
  FoundMap[F] = Node;

  // Initialize second level executors.
  MDNodeExecutorList nel;
  // kernel arg address space metadata verifier.
  VerifyMetadataArgAddrSpace vmdaas(ErrCreator, F);
  nel.push_back(&vmdaas);
  // kernel arg type metadata verifier.
  VerifyMetadataArgType vmdat(ErrCreator);
  nel.push_back(&vmdat);
  // kernel arg base type metadata verifier.
  VerifyMetadataArgBaseType vmdabt(ErrCreator, F, Data);
  nel.push_back(&vmdabt);

  MetaDataIterator mdi(nel);
  mdi.execute(*Node);

  // Varify that metadata arg address space exists.
  if (!vmdaas.found()) {
    ErrCreator->addError(ERR_MISSING_METADATA_KERNEL_INFO, Node);
  }

  // Varify that metadata arg type exists.
  if (!vmdat.found()) {
    ErrCreator->addError(ERR_MISSING_METADATA_KERNEL_INFO, Node);
  }

  // Varify that metadata arg base type exists.
  if (!vmdabt.found()) {
    ErrCreator->addError(ERR_MISSING_METADATA_KERNEL_INFO, Node);
  }
}

void VerifyMetadataKernels::execute(const llvm::Module *M) {
  // Counting the number of kernels in the module.
  unsigned int NumKernels = 0;
  Module::const_iterator fi = M->begin(), fe = M->end();
  for (; fi != fe; fi++) {
    const Function *F = &*fi;
    if (F->getCallingConv() == CallingConv::SPIR_KERNEL) {
      NumKernels++;
    }
  }

  // Acquiring kernels node.
  NamedMDNode *MDKernels = M->getNamedMetadata(OPENCL_KERNELS);
  if (!MDKernels) {
    ErrCreator->addError(ERR_MISSING_NAMED_METADATA, OPENCL_KERNELS);
    return;
  }

  // Verify that number of function kernels mach number of metadata kernels.
  const unsigned int NumMDKernels = MDKernels->getNumOperands();

  if (NumKernels != NumMDKernels) {
    std::stringstream Msg;
    Msg << "inconsistency in kernels nodes. ";
    Msg << "The module has " << NumMDKernels << " metadata nodes, but ";
    Msg << NumKernels << " kernels";
    ErrCreator->addError(ERR_INVALID_METADATA_KERNEL, Msg.str());
  }

  // If there are no kernels, we have no more tests to do.
  if (!NumKernels)
    return;

  //Kernel MetaData structure:
  // !opencl.kernels = {!0, !1, ...}
  // !0 = {llvm::Function*, !10, !11, ...}
  // ...
  // !10 = {metadata !"kernel_arg_base_type", metadata !"<TY1>", ...}
  // !11 = {metadata !"kernel_arg_type", metadata !"<TY1>", ...}

  FunctionToMDNodeMap FoundMap;
  VerifyMetadataKernel vmk(ErrCreator, Data, FoundMap);
  for (unsigned i = 0; i < NumMDKernels; i++) {
    MDNode *N = dyn_cast<MDNode>(MDKernels->getOperand(i));
    if (!N) {
      // Is this possible for LLVM valid IR?
      ErrCreator->addError(ERR_INVALID_METADATA_KERNEL, MDKernels);
    }
    // Apply Metadata kernel executor.
    vmk.execute(N);
  }
}

void VerifyMetadataVersions::execute(const llvm::Module *M) {
  const char *VersionName = NULL;
  switch (VType) {
  case VERSION_OCL:
    VersionName = OPENCL_OCL_VERSION;
    break;
  case VERSION_SPIR:
    VersionName = OPENCL_SPIR_VERSION;
    break;
  default:
    assert(false && "Unknown OpenCL version type");
  }

  // Verify version exists.
  NamedMDNode *NMDVersion = M->getNamedMetadata(VersionName);
  if (!NMDVersion) {
    ErrCreator->addError(ERR_MISSING_NAMED_METADATA, VersionName);
    return;
  }

  // version MetaData structure:
  // !opencl.ocl.version  = {!6}
  // !opencl.spir.version = {!7}
  // ...
  // !6 = metadata !{i32 1, i32 2}
  // !7 = metadata !{i32 1, i32 2}

  // Verify that there is exactly one version.
  if (NMDVersion->getNumOperands() != 1) {
    ErrCreator->addError(ERR_INVALID_METADATA_VERSION, NMDVersion);
    return;
  }

  MDNode *VersionNode = NMDVersion->getOperand(0);

  // Verify valid version node
  if (VersionNode->getNumOperands() != 2) {
    ErrCreator->addError(ERR_INVALID_METADATA_VERSION, VersionNode);
    return;
  }

  ConstantInt *VersionMajor = dyn_cast<ConstantInt>(VersionNode->getOperand(0));
  ConstantInt *VersionMinor = dyn_cast<ConstantInt>(VersionNode->getOperand(1));

  if (!VersionMajor || !VersionMinor) {
    ErrCreator->addError(ERR_INVALID_METADATA_VERSION, VersionNode);
    return;
  }

  unsigned Major = (unsigned)VersionMajor->getZExtValue();
  unsigned Minor = (unsigned)VersionMinor->getZExtValue();

  bool IsValidVersion = false;
  switch (VType) {
  case VERSION_OCL:
    IsValidVersion = isValidOCLVersion(Major, Minor);
    break;
  case VERSION_SPIR:
    IsValidVersion = isValidSPIRVersion(Major, Minor);
    break;
  default:
    assert(false && "Unknown OpenCL version type");
  }

  if (!IsValidVersion) {
    ErrCreator->addError(ERR_INVALID_METADATA_VERSION, VersionNode);
    return;
  }
}

void VerifyMetadataCoreFeatures::execute(const llvm::Module *M) {
  // Verify OpenCL optional core features metadata exists.
  NamedMDNode *NMDCoreFeatures = M->getNamedMetadata(OPENCL_CORE_FEATURES);
  if (!NMDCoreFeatures) {
    ErrCreator->addError(ERR_MISSING_NAMED_METADATA, OPENCL_CORE_FEATURES);
    return;
  }

  // Optional core features MetaData structure:
  // !opencl.used.optional.core.features  = {!8}
  // ...
  // !8 = metadata !{metadata !"cl_doubles", metadata !"cl_images"}

  // Verify that there is exactly one entry.
  if (NMDCoreFeatures->getNumOperands() != 1) {
    ErrCreator->addError(ERR_INVALID_CORE_FEATURE, NMDCoreFeatures);
    return;
  }

  MDNode *Node = NMDCoreFeatures->getOperand(0);

  // Verify valid optional core feature nodes
  for (unsigned i=0; i<Node->getNumOperands(); i++) {
    MDString *StringValue = dyn_cast<MDString>(Node->getOperand(i));
    if (!StringValue || !isValidNameOf(StringValue->getString(),
                                       g_valid_core_feature,
                                       g_valid_core_feature_len)) {
      ErrCreator->addError(ERR_INVALID_CORE_FEATURE, Node);
      continue;
    }
    if (StringValue->getString() == CORE_FEATURE_CL_DOUBLES) {
      if (Data->HasDoubleFeature) {
        // Core feature appears twice
        ErrCreator->addError(ERR_INVALID_CORE_FEATURE, Node);
        continue;
      }
      Data->HasDoubleFeature = true;
    }
    else if(StringValue->getString() == CORE_FEATURE_CL_IMAGES) {
      if (Data->HasImageFeature) {
        // Core feature appears twice
        ErrCreator->addError(ERR_INVALID_CORE_FEATURE, Node);
        continue;
      }
      Data->HasImageFeature = true;
    }
    else {
      assert(false && "Unhandled core feature");
    }
  }
}


void VerifyMetadataKHRExtensions::execute(const llvm::Module *M) {
  // Verify OpenCL optional KHR extensions metadata exists.
  NamedMDNode *NMDExts = M->getNamedMetadata(OPENCL_KHR_EXTENSIONS);
  if (!NMDExts) {
    ErrCreator->addError(ERR_MISSING_NAMED_METADATA, OPENCL_KHR_EXTENSIONS);
    return;
  }

  // KHR extensions MetaData structure:
  // !opencl.used.extensions = !{!6}
  // ...
  // !6 = metadata !{metadata !"cl_khr_fp16", metadata !"cl_khr_int64_base_atomics"}

  // Verify that there is exactly one entry.
  if (NMDExts->getNumOperands() != 1) {
    ErrCreator->addError(ERR_INVALID_CORE_FEATURE, NMDExts);
    return;
  }

  MDNode *Node = NMDExts->getOperand(0);

  // Verify valid optional core feature nodes
  for (unsigned i=0; i<Node->getNumOperands(); i++) {
    MDString *StringValue = dyn_cast<MDString>(Node->getOperand(i));
    if (!StringValue || !isValidNameOf(StringValue->getString(),
                                       g_valid_khr_ext,
                                       g_valid_khr_ext_len)) {
      ErrCreator->addError(ERR_INVALID_KHR_EXT, Node);
      continue;
    }
    if (StringValue->getString() == EXTENSION_CL_KHR_FP16) {
      if (Data->HASFp16Extension) {
        // KHR extension appears twice
        ErrCreator->addError(ERR_INVALID_KHR_EXT, Node);
        continue;
      }
      Data->HasDoubleFeature = true;
    }
    else {
      // TODO enable the following assertion once all extensions are handled.
      //assert(false && "Unhandled KHR extension");
    }
  }
}

void VerifyMetadataCompilerOptions::execute(const llvm::Module *M) {
  // Verify OpenCL compiler options metadata exists.
  NamedMDNode *NMDOptions = M->getNamedMetadata(OPENCL_COMPILER_OPTIONS);
  if (!NMDOptions) {
    ErrCreator->addError(ERR_MISSING_NAMED_METADATA, OPENCL_COMPILER_OPTIONS);
    return;
  }

  // Compiler options MetaData structure:
  // !opencl.compiler.options = !{!9}
  // ...
  // !9 = metadata !{metadata !"-cl-mad-enable", metadata !"-cl-denorms-are-zero"}

  // Verify that there is exactly one entry.
  if (NMDOptions->getNumOperands() != 1) {
    ErrCreator->addError(ERR_INVALID_COMPILER_OPTION, NMDOptions);
    return;
  }

  MDNode *Node = NMDOptions->getOperand(0);

  // Verify valid optional core feature nodes
  for (unsigned i=0; i<Node->getNumOperands(); i++) {
    MDString *StringValue = dyn_cast<MDString>(Node->getOperand(i));
    if (!StringValue || !isValidNameOf(StringValue->getString(),
                                       g_valid_compiler_options,
                                       g_valid_compiler_options_len)) {
      ErrCreator->addError(ERR_INVALID_COMPILER_OPTION, Node);
      continue;
    }
  }
}

} // End SPIR namespace

