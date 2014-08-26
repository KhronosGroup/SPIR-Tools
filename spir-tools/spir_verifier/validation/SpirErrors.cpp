//===-------------------------- SpirErrors.cpp ---------------------------===//
//
//                              SPIR Tools
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===---------------------------------------------------------------------===//

#include "LLVMVersion.h"
#include "SpirErrors.h"
#include "SpirTables.h"

#if LLVM_VERSION==3200
  #include "llvm/Type.h"
  #include "llvm/Value.h"
  #include "llvm/Metadata.h"
#else
  #include "llvm/IR/Type.h"
  #include "llvm/IR/Value.h"
  #include "llvm/IR/Metadata.h"
#endif
#include "llvm/Support/raw_ostream.h"

#include <string>
#include <sstream>
#include <map>

using namespace llvm;

namespace SPIR {

 typedef enum {
  INFO_NONE = 0,
  // Information types
  INFO_TRIPLE,
  INFO_DATA_LAYOUT,
  INFO_OCL_TYPE,
  INFO_LLVM_TYPE,
  INFO_KERNEL_RETURN_TYPE,
  INFO_KERNEL_ARG_ADDRESS_SPACE,
  INFO_GLOBAL_AS3_VAR,
  INFO_GLOBAL_VAR_ADDRES_SPACES,
  INFO_OCL_TO_LLVM_TYPE,
  INFO_CORE_FEATURE_METADATA,
  INFO_KHR_EXT_METADATA,
  INFO_COMPILER_OPTION_METADATA,
  INFO_INTRINSIC,
  INFO_ADDRESS_SPACE,
  INFO_CALLING_CONVENTION,
  INFO_LINKAGE_TYPE,
  INFO_INDIRECT_CALL,
  INFO_NAMED_METADATA,
  INFO_METADATA_KERNEL_ARG_INFO,
  INFO_METADATA_VERSION,
  INFO_MEM_FENCE,

  SPIR_INFO_NUM
} SPIR_INFO_TYPE;


/// @brief Base error class, all error types inherit from it.
class ValidationError {
public:
  /// @brief Constructor.
  /// @param T error type
  /// @param S error message
  ValidationError(SPIR_ERROR_TYPE T, llvm::StringRef S) :
    ErrType(T), ErrMSG(S) {
  }

  /// @brief Get error type.
  /// @returns error type
  virtual SPIR_ERROR_TYPE getErrorType() const {
    return ErrType;
  }

  /// @brief Get error message.
  /// @returns error message
  virtual llvm::StringRef toString() const {
    return StringRef(ErrMSG);
  }

  /// @brief Destructor.
  virtual ~ValidationError() {}

protected:
  SPIR_ERROR_TYPE ErrType;
  std::string ErrMSG;
};

struct ErrorComperator {
  const ValidationError * LHS;

  ErrorComperator(const ValidationError *Err) : LHS(Err) {}

  bool operator() (const ValidationError *RHS) {
    if (LHS == RHS)
      return true;

    return
      (LHS->getErrorType() == RHS->getErrorType()) &&
      (LHS->toString() == RHS->toString());
  }
};

#define MAX_ERROR_INFO_PER_ERROR (4)
struct SPIR_ERROR_DATA {
  SPIR_ERROR_TYPE T;
  std::string MSG;
  SPIR_INFO_TYPE InfoList[MAX_ERROR_INFO_PER_ERROR];
  std::string ErrTypeStr;
};

typedef std::string (GetInfoMsgFunc)();
struct SPIR_INFO_DATA {
  SPIR_INFO_TYPE T;
  GetInfoMsgFunc *GetMsg;
};

typedef std::map<SPIR_INFO_TYPE, unsigned> SPIRInfoTypeNumMap;

const SPIR_ERROR_DATA g_ErrorData[SPIR_ERROR_NUM] = {
  // Module (general) errors
  {ERR_INVALID_TRIPLE, "Invalid triple",
      {INFO_TRIPLE}, "ERR_INVALID_TRIPLE"},
  {ERR_INVALID_DATA_LAYOUT, "Invalid data layout",
      {INFO_DATA_LAYOUT}, "ERR_INVALID_DATA_LAYOUT"},
  {ERR_MISMATCH_TRIPLE_AND_DATA_LAYOUT, "Mismatch between triple and data layout",
      {INFO_TRIPLE, INFO_DATA_LAYOUT},"ERR_MISMATCH_TRIPLE_AND_DATA_LAYOUT"},
  // Type errors
  {ERR_INVALID_OCL_TYPE, "Invalid OpenCL C type",
      {INFO_OCL_TYPE, INFO_CORE_FEATURE_METADATA, INFO_KHR_EXT_METADATA}, "ERR_INVALID_OCL_TYPE"},
  {ERR_INVALID_LLVM_TYPE, "Invalid LLVM type",
      {INFO_LLVM_TYPE, INFO_CORE_FEATURE_METADATA, INFO_KHR_EXT_METADATA}, "ERR_INVALID_LLVM_TYPE"},
  {ERR_INVALID_KERNEL_RETURN_TYPE, "Invalid SPIR kernel return type",
      {INFO_KERNEL_RETURN_TYPE}, "ERR_INVALID_KERNEL_RETURN_TYPE"},
  {ERR_KERNEL_ARG_PTRPTR, "SPIR kernel argument is a pointer to pointer",
      {}, "ERR_KERNEL_ARG_PTRPTR"},
  {ERR_KERNEL_ARG_AS0, "SPIR kernel argument is a pointer to private address space",
      {INFO_KERNEL_ARG_ADDRESS_SPACE}, "ERR_KERNEL_ARG_AS0"},
  {ERR_MISMATCH_OCL_AND_LLVM_TYPES, "Mismatch between OpenCL C and LLVM types",
      {INFO_OCL_TO_LLVM_TYPE}, "ERR_MISMATCH_OCL_AND_LLVM_TYPES"},
  {ERR_INVALID_GLOBAL_AS3_VAR, "Invalid program scope __local variable",
    {INFO_GLOBAL_AS3_VAR}, "ERR_INVALID_GLOBAL_AS3_VAR"},
  {ERR_INVALID_GLOBAL_VAR_ADDRESS_SPACE, "program scope variable in a wrong address space",
    {INFO_GLOBAL_VAR_ADDRES_SPACES}, "ERR_INVALID_GLOBAL_VAR_ADDRESS_SPACE"},
  // Instruction errors
  {ERR_INVALID_INTRINSIC, "Invalid intrinsic",
      {INFO_INTRINSIC},"ERR_INVALID_INTRINSIC"},
  {ERR_INVALID_ADDR_SPACE, "Invalid address space",
      {INFO_ADDRESS_SPACE}, "ERR_INVALID_ADDR_SPACE"},
  {ERR_INVALID_ADDR_SPACE_CAST, "Invalid address space cast",
      {INFO_ADDRESS_SPACE}, "ERR_INVALID_ADDR_SPACE_CAST"},
  {ERR_INVALID_INDIRECT_CALL, "Invalid indirect call",
      {INFO_INDIRECT_CALL}, "ERR_INVALID_INDIRECT_CALL"},
  {ERR_INVALID_MEM_FENCE, "Invalid cl_mem_fence value",
      {INFO_MEM_FENCE}, "ERR_INVALID_MEM_FENCE"},
  // Function errors
  {ERR_INVALID_CALLING_CONVENTION, "Invalid calling convention",
      {INFO_CALLING_CONVENTION}, "ERR_INVALID_CALLING_CONVENTION"},
  {ERR_INVALID_LINKAGE_TYPE, "Invalid linkage type",
      {INFO_LINKAGE_TYPE}, "ERR_INVALID_LINKAGE_TYPE"},
  // Metadata errors
  {ERR_INVALID_CORE_FEATURE, "Invalid core features",
      {INFO_CORE_FEATURE_METADATA}, "ERR_INVALID_CORE_FEATURE"},
  {ERR_INVALID_KHR_EXT, "Invalid KHR extensions",
      {INFO_KHR_EXT_METADATA}, "ERR_INVALID_KHR_EXT"},
  {ERR_INVALID_COMPILER_OPTION, "Invalid compiler options",
      {INFO_COMPILER_OPTION_METADATA},"ERR_INVALID_COMPILER_OPTION"},
  {ERR_MISSING_NAMED_METADATA, "Named Metadata is missing",
      {INFO_NAMED_METADATA}, "ERR_MISSING_NAMED_METADATA"},
  {ERR_INVALID_METADATA_KERNEL, "Invalid kernel metatdata",
      {}, "ERR_INVALID_METADATA_KERNEL"},
  {ERR_INVALID_METADATA_KERNEL_INFO, "Invalid kernel metadata ARG Info",
      {INFO_METADATA_KERNEL_ARG_INFO}, "ERR_INVALID_METADATA_KERNEL_INFO"},
  {ERR_MISSING_METADATA_KERNEL_INFO, "Kernel metadata is missing ARG Info",
      {INFO_METADATA_KERNEL_ARG_INFO}, "ERR_MISSING_METADATA_KERNEL_INFO"},
  {ERR_INVALID_METADATA_VERSION, "Invalid OpenCL (OCL/SPIR) version",
      {INFO_METADATA_VERSION}, "ERR_INVALID_METADATA_VERSION"},
  {ERR_MISMATCH_METADATA_ADDR_SPACE, "Address space mismatch between kernel prototype and metadata",
      {}, "ERR_MISMATCH_METADATA_ADDR_SPACE"}
};

const SPIR_INFO_DATA g_InfoData[SPIR_INFO_NUM] = {
  {INFO_NONE, NULL},
  {INFO_TRIPLE, getValidTripleMsg},
  {INFO_DATA_LAYOUT, getValidDataLayoutMsg},
  {INFO_OCL_TYPE, getValidOpenCLTypeMsg},
  {INFO_LLVM_TYPE, getValidLLVMTypeMsg},
  {INFO_KERNEL_RETURN_TYPE, getValidKernelReturnTypeMsg},
  {INFO_KERNEL_ARG_ADDRESS_SPACE, getValidKernelArgAddressSpaceMsg},
  {INFO_GLOBAL_AS3_VAR, getValidGlobalAS3VariableMsg},
  {INFO_GLOBAL_VAR_ADDRES_SPACES, getValidGlobalVarAddressSpacesMsg},
  {INFO_OCL_TO_LLVM_TYPE, getMapOpenCLToLLVMMsg},
  {INFO_CORE_FEATURE_METADATA, getValidCoreFeaturesMsg},
  {INFO_KHR_EXT_METADATA, getValidKHRExtensionsMsg},
  {INFO_COMPILER_OPTION_METADATA, getValidCompilerOptionsMsg},
  {INFO_INTRINSIC, getValidIntrinsicMsg},
  {INFO_ADDRESS_SPACE, getValidAddressSpaceMsg},
  {INFO_CALLING_CONVENTION, getValidCallingConventionMsg},
  {INFO_LINKAGE_TYPE, getValidLinkageTypeMsg},
  {INFO_INDIRECT_CALL, getValidIndirectCallMsg},
  {INFO_NAMED_METADATA, getValidNamedMetadataMsg},
  {INFO_METADATA_KERNEL_ARG_INFO, getValidKernelArgInfoMsg},
  {INFO_METADATA_VERSION, getValidVersionMsg},
  {INFO_MEM_FENCE, getValidMemFenceMsg}
};

static bool isValidTables() {
  for (unsigned i=0; i<SPIR_ERROR_NUM; i++) {
    if (g_ErrorData[i].T != (SPIR_ERROR_TYPE)i)
      return false;
  }

  for (unsigned i=0; i<SPIR_INFO_NUM; i++) {
    if (g_InfoData[i].T != (SPIR_INFO_TYPE)i)
      return false;
  }

  return true;
}

//
// Validation Errors
//

/// @brief Returns type's name.
/// @param Ty type
/// @returns type's name as std::string.
static std::string getObjectAsString(const Type *Ty) {
  std::string type_str;
  llvm::raw_string_ostream rso(type_str);
  Ty->print(rso);
  return rso.str();
}

/// @brief Returns Value as string.
/// @param V value
/// @returns value as std::string.
static std::string getObjectAsString(const llvm::Value *V) {
  std::string type_str;
  llvm::raw_string_ostream rso(type_str);
  V->print(rso);
  return rso.str();
}

/// @brief Returns NamedMDNode as string.
/// @param NMD named metadata node
/// @returns named metadata node as std::string.
static std::string getObjectAsString(const llvm::NamedMDNode *NMD) {
  std::string type_str;
  llvm::raw_string_ostream rso(type_str);
  NMD->print(rso);
  return rso.str();
}

ErrorHolder::ErrorHolder() {
  assert(isValidTables() && "SPIR Error/Info data tables are invalid!");
}

ErrorHolder::~ErrorHolder() {
  for (ErrorList::iterator ei=EL.begin(), ee=EL.end(); ei!=ee; ei++) {
    const ValidationError *Err = *ei;
    delete Err;
  }
}

void ErrorHolder::addError(SPIR_ERROR_TYPE Err, const llvm::StringRef S) {
  std::string ErrMsg;
  ErrMsg += S.str() + "\n";
  ValidationError *VE = new ValidationError(Err, ErrMsg);
  EL.push_back(VE);
}

void ErrorHolder::addError(SPIR_ERROR_TYPE Err, const llvm::Value *V) {
  ValidationError *VE = new ValidationError(Err, getObjectAsString(V));
  EL.push_back(VE);
}

void ErrorHolder::addError(SPIR_ERROR_TYPE Err, const llvm::NamedMDNode *NMD) {
  ValidationError *VE = new ValidationError(Err, getObjectAsString(NMD));
  EL.push_back(VE);
}

void ErrorHolder::addError(SPIR_ERROR_TYPE Err, const llvm::Type *T,
                                                const llvm::StringRef S) {
  std::string ErrMsg;
  ErrMsg += "Type: " + getObjectAsString(T) + "\n";
  ErrMsg += "Found in prototype of Function: " + S.str() + "\n";
  ValidationError *VE = new ValidationError(Err, ErrMsg);
  EL.push_back(VE);
}

void ErrorHolder::addError(SPIR_ERROR_TYPE Err, const llvm::Type *T,
                                                const llvm::Value *V) {
  std::string ErrMsg;
  ErrMsg += "Type: " + getObjectAsString(T) + "\n";
  ErrMsg += "Found in: " + getObjectAsString(V) + "\n";
  ValidationError *VE = new ValidationError(Err, ErrMsg);
  EL.push_back(VE);
}

void ErrorHolder::print(llvm::raw_ostream &S, bool LITMode) const {
  ErrorList UEL;
  SPIRInfoTypeNumMap ITmap;
  // Calculate unique error list
  // Collect relevant info types
  for (ErrorList::const_iterator ei=EL.begin(), ee=EL.end(); ei!=ee; ei++) {
    const ValidationError *Err = *ei;
    ErrorComperator Dup(Err);
    if (std::find_if(UEL.begin(), UEL.end(), Dup) == UEL.end()) {
      // Add to unique error list
      UEL.push_back(Err);
      // Add to info type map, initialize number to zero
      SPIR_ERROR_TYPE ErrType = Err->getErrorType();
      for (unsigned i=0; i<MAX_ERROR_INFO_PER_ERROR; i++) {
        SPIR_INFO_TYPE InfoType = g_ErrorData[ErrType].InfoList[i];
        if (InfoType != INFO_NONE) {
          ITmap[InfoType] = 0;
        }
      }
    }
  }

  // Assign error info number for each relevant info type
  // Create SPIR Info message
  std::string InfoMsg;
  if (!LITMode) {
    unsigned ErrInfoNum = 0;
    InfoMsg += "---------------------------------------------";
    InfoMsg += "---------------------------------------------\n";
    for (unsigned i=0; i<SPIR_INFO_NUM; i++) {
      SPIR_INFO_TYPE InfoType = g_InfoData[i].T;
      if (ITmap.count(InfoType) != 0) {
        // Set error info number
        ITmap[InfoType] = ++ErrInfoNum;
        // Append error info message
        std::stringstream SS;
        SS << "[" << ErrInfoNum << "] " << g_InfoData[i].GetMsg() << "\n";
        InfoMsg += SS.str();
      }
    }
  }

  // Create error message
  std::string ErrMsg;
  unsigned ErrNum = 0;

  for (ErrorList::const_iterator ei=UEL.begin(), ee=UEL.end(); ei!=ee; ei++) {
    const ValidationError *Err = *ei;
    std::stringstream SS;
    SPIR_ERROR_TYPE ErrType = Err->getErrorType();
    SS << "(" << ++ErrNum << ") Error";
    if(!LITMode) {
      for (unsigned i=0; i<MAX_ERROR_INFO_PER_ERROR; i++) {
        SPIR_INFO_TYPE InfoType = g_ErrorData[ErrType].InfoList[i];
        if (InfoType != INFO_NONE) {
          SS << "[" << ITmap[InfoType] << "]";
        }
      }
      SS << " " << g_ErrorData[ErrType].MSG.c_str() << ":\n";
    } else {
      SS << " " << g_ErrorData[ErrType].ErrTypeStr << ":\n";
    }
    SS << Err->toString().str().c_str() << "\n";
    ErrMsg += SS.str();
  }

  // Print error message and SPIR info message to output stream
  S << ErrMsg;
  S << InfoMsg;
}

bool ErrorHolder::hasErrors() const {
  return !EL.empty();
}

} // End SPIR namespace
