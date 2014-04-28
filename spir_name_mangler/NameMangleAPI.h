//
//                     SPIR Tools
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//

#include "FunctionDescriptor.h"
#include <string>

namespace SPIR {

/// @brief Converts the given function descriptor to string that represents
///        the function's prototype.
///        The mangling algorithm is based on Itanium mangling algorithm
///        (http://sourcery.mentor.com/public/cxx-abi/abi.html#mangling), with
///        SPIR extensions.
/// @param FunctionDescriptor function to be mangled.
/// @return std::string representing the mangled name.
std::string mangle(const FunctionDescriptor&);

} // End SPIR namespace