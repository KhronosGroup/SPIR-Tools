SPIR Name Mangler
=================

The NameMangler Library Converts the given function descriptor to a string
that represents the function's prototype.

The mangling algorithm is based on clang 3.0 Itanium mangling algorithm
(http://sourcery.mentor.com/public/cxx-abi/abi.html#mangling).

The algorithm is adapted to support mangling of OpenCL C built-in
functions for SPIR and was tested on OpenCL C built-ins only.

For usage example see unittest/spir_name_mangler.



