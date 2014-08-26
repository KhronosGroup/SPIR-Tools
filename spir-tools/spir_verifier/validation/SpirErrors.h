//===--------------------------- SpirErrors.h ----------------------------===//
//
//                              SPIR Tools
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===---------------------------------------------------------------------===//

#ifndef __SPIR_ERRORS_H__
#define __SPIR_ERRORS_H__

#include <list>

namespace llvm {
  class Type;
  class Value;
  class MDNode;
  class NamedMDNode;
  class StringRef;
  class raw_ostream;
}

using namespace llvm;
namespace SPIR {
//
// Validation Errors
//

typedef enum {
  // Module (general) errors
  ERR_INVALID_TRIPLE,
  ERR_INVALID_DATA_LAYOUT,
  ERR_MISMATCH_TRIPLE_AND_DATA_LAYOUT,
  // Type errors
  ERR_INVALID_OCL_TYPE,
  ERR_INVALID_LLVM_TYPE,
  ERR_INVALID_KERNEL_RETURN_TYPE,
  ERR_KERNEL_ARG_PTRPTR,
  ERR_KERNEL_ARG_AS0,
  ERR_MISMATCH_OCL_AND_LLVM_TYPES,
  ERR_INVALID_GLOBAL_AS3_VAR,
  ERR_INVALID_GLOBAL_VAR_ADDRESS_SPACE,
  // Instruction errors
  ERR_INVALID_INTRINSIC,
  ERR_INVALID_ADDR_SPACE,
  ERR_INVALID_ADDR_SPACE_CAST,
  ERR_INVALID_INDIRECT_CALL,
  ERR_INVALID_MEM_FENCE,
  // Function errors
  ERR_INVALID_CALLING_CONVENTION,
  ERR_INVALID_LINKAGE_TYPE,
  // Metadata errors
  ERR_INVALID_CORE_FEATURE,
  ERR_INVALID_KHR_EXT,
  ERR_INVALID_COMPILER_OPTION,
  ERR_MISSING_NAMED_METADATA,
  ERR_INVALID_METADATA_KERNEL,
  ERR_INVALID_METADATA_KERNEL_INFO,
  ERR_MISSING_METADATA_KERNEL_INFO,
  ERR_INVALID_METADATA_VERSION,
  ERR_MISMATCH_METADATA_ADDR_SPACE,

  SPIR_ERROR_NUM
} SPIR_ERROR_TYPE;

struct ErrorPrinter {
  /// @brief prints all errors to given output stream.
  /// @param S output stream.
  /// @param LITMode prints error names only in when set to true
  virtual void print(llvm::raw_ostream &S, bool LITMode) const = 0;

  /// @brief Checks if the module has errors.
  /// @returns true if errors list is not emtpy.
  virtual bool hasErrors() const = 0;
};

struct ErrorCreator {

  /// @brief Creates and adds new error to the error list.
  /// @param Err error type to be added.
  /// @param S llvm string that leaded to the error.
  virtual void addError(SPIR_ERROR_TYPE Err, const llvm::StringRef S) = 0;

  /// @brief Creates and adds new error to the error list.
  /// @param Err error type to be added.
  /// @param V llvm value that leaded to the error.
  virtual void addError(SPIR_ERROR_TYPE Err, const llvm::Value *V) = 0;

  /// @brief Creates and adds new error to the error list.
  /// @param Err error type to be added.
  /// @param NMD llvm named metadata node that leaded to the error.
  virtual void addError(SPIR_ERROR_TYPE Err, const llvm::NamedMDNode *NMD) = 0;

  /// @brief Creates and adds new error to the error list.
  /// @param Err error type to be added.
  /// @param T llvm type that leaded to the error.
  /// @param S name of the function that T in its prototype.
  virtual void addError(SPIR_ERROR_TYPE Err, const llvm::Type *T,
                                             const llvm::StringRef S) = 0;

  /// @brief Creates and adds new error to the error list.
  /// @param Err error type to be added.
  /// @param T llvm type that leaded to the error.
  /// @param V value that T is its type.
  virtual void addError(SPIR_ERROR_TYPE Err, const llvm::Type *T,
                                             const llvm::Value *V) = 0;

};

struct ValidationError;
typedef std::list<const ValidationError*> ErrorList;


struct ErrorHolder : ErrorCreator, ErrorPrinter {
  ErrorHolder();
  ~ErrorHolder();

  /// Implementation of the pure virtual methods of ErrorCreator interface
  virtual void addError(SPIR_ERROR_TYPE Err, const llvm::StringRef S);
  virtual void addError(SPIR_ERROR_TYPE Err, const llvm::Value *V);
  virtual void addError(SPIR_ERROR_TYPE Err, const llvm::NamedMDNode *NMD);
  virtual void addError(SPIR_ERROR_TYPE Err, const llvm::Type *T,
                                             const llvm::StringRef S);
  virtual void addError(SPIR_ERROR_TYPE Err, const llvm::Type *T,
                                             const llvm::Value *V);

  /// Implementation of the pure virtual methods of ErrorPrinter interface
  virtual void print(llvm::raw_ostream &S, bool LITMode) const;
  virtual bool hasErrors() const;

private:
  /// @brief List of errors found in the module
  ErrorList EL;
};


} // End SPIR namespace
#endif // __SPIR_ERRORS_H__
