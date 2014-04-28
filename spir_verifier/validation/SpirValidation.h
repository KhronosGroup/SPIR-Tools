//
//                     SPIR Tools
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//

#ifndef __SPIR_VALIDATION_H__
#define __SPIR_VALIDATION_H__

#include "SpirErrors.h"
#include "llvm/Pass.h"

namespace SPIR {

/// @brief Indicates whether a given module is a valid SPIR module
///        according to SPIR 1.2 spec.
class SpirValidation : public llvm::ModulePass {
public:

  /// @brief Pass identification, replacement for typeid.
  static char ID;

  /// @brief Constructor.
  SpirValidation();

  /// @brief Distructor.
  virtual ~SpirValidation();

  /// @brief Provides name of pass.
  virtual const char *getPassName() const;

  /// @brief LLVM Module pass entry.
  /// @param M Module to transform.
  /// @returns true if changed.
  bool runOnModule(llvm::Module&);

  /// @brief returns instance of ErrorPrinter implementation.
  /// @returns error printer instance.
  const ErrorPrinter *getErrorPrinter() const {
    return &ErrHolder;
  }
  
private:

  /// @brief Holder for errors found in the module
  ErrorHolder ErrHolder;
};

} // End SPIR namespace

#endif // __SPIR_VALIDATION_H__
