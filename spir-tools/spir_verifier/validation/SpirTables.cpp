//===-------------------------- SpirTables.cpp ---------------------------===//
//
//                              SPIR Tools
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===---------------------------------------------------------------------===//

#include "SpirTables.h"
#include <string>
#include <sstream>

namespace SPIR {

//
// Constant definitions.
//

const char *SPIR32_TRIPLE = "spir-unknown-unknown";
const char *SPIR64_TRIPLE = "spir64-unknown-unknown";
const char *SPIR32_DATA_LAYOUT =
  "e-p:32:32:32-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-"
  "f32:32:32-f64:64:64-v16:16:16-v24:32:32-v32:32:32-v48:64:64-"
  "v64:64:64-v96:128:128-v128:128:128-v192:256:256-v256:256:256-"
  "v512:512:512-v1024:1024:1024";
const char *SPIR64_DATA_LAYOUT =
  "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-"
  "f32:32:32-f64:64:64-v16:16:16-v24:32:32-v32:32:32-v48:64:64-"
  "v64:64:64-v96:128:128-v128:128:128-v192:256:256-v256:256:256-"
  "v512:512:512-v1024:1024:1024";


#define DCL_ARRAY_LENGTH(arr) \
  const unsigned arr##_len = (sizeof(arr)/sizeof(char*))

#define STR_IND1 std::string("  ")
#define STR_IND2 std::string("    ")
#define STR_SPIR std::string("SPIR")
#define STR_NOTE std::string("Note: ")


const char *CORE_FEATURE_CL_DOUBLES = "cl_doubles";
const char *CORE_FEATURE_CL_IMAGES = "cl_images";
const char *g_valid_core_feature[] = {
  CORE_FEATURE_CL_DOUBLES,
  CORE_FEATURE_CL_IMAGES
};
DCL_ARRAY_LENGTH(g_valid_core_feature);
const char *g_valid_core_feature_prefix = "cl_";


const char *EXTENSION_CL_KHR_FP16 = "cl_khr_fp16";
const char *g_valid_khr_ext[] = {
  "cl_khr_int64_base_atomics",
  "cl_khr_int64_extended_atomics",
  EXTENSION_CL_KHR_FP16,
  "cl_khr_gl_sharing",
  "cl_khr_gl_event",
  "cl_khr_d3d10_sharing",
  "cl_khr_media_sharing",
  "cl_khr_d3d11_sharing",
  "cl_khr_global_int32_base_atomics",
  "cl_khr_global_int32_extended_atomics",
  "cl_khr_local_int32_base_atomics",
  "cl_khr_local_int32_extended_atomics",
  "cl_khr_byte_addressable_store",
  "cl_khr_3d_image_writes",
  "cl_khr_gl_msaa_sharing",
  "cl_khr_depth_images",
  "cl_khr_gl_depth_images"
};
DCL_ARRAY_LENGTH(g_valid_khr_ext);
const char *g_valid_khr_ext_prefix = "cl_khr_";


const char *g_valid_compiler_options[] = {
  "-cl-single-precision-constant",
  "-cl-denorms-are-zero",
  "-cl-fp32-correctly-rounded-divide-sqrt",
  "-cl-opt-disable",
  "-cl-mad-enable",
  "-cl-no-signed-zeros",
  "-cl-unsafe-math-optimizations",
  "-cl-finite-math-only",
  "-cl-fast-relaxed-math",
  "-w",
  "-Werror",
  "-cl-kernel-arg-info"
};
DCL_ARRAY_LENGTH(g_valid_compiler_options);

///
/// OpenCL C Type tables
///
const char *g_valid_ocl_primitives[] = {
  "bool"  ,
  "char"  , "uchar" , "unsigned char" ,
  "short" , "ushort", "unsigned short",
  "int"   , "uint"  , "unsigned int"  ,
  "long"  , "ulong" , "unsigned long" ,
  "half"  ,
  "float" ,
  "double",
  "void"
};
DCL_ARRAY_LENGTH(g_valid_ocl_primitives);

const char *g_valid_ocl_vector_element_types[] = {
  "char"  , "uchar" , "unsigned char" ,
  "short" , "ushort", "unsigned short",
  "int"   , "uint"  , "unsigned int"  ,
  "long"  , "ulong" , "unsigned long" ,
  "half"  ,
  "float" ,
  "double"
};
DCL_ARRAY_LENGTH(g_valid_ocl_vector_element_types);

const char *g_valid_ocl_opaque_types[] = {
  "image1d_t",
  "image1d_array_t",
  "image1d_buffer_t",
  "image2d_t",
  "image2d_depth_t",
  "image2d_msaa_t",
  "image2d_msaa_depth_t",
  "image2d_array_t",
  "image2d_array_depth_t",
  "image2d_array_msaa_t",
  "image2d_array_msaa_depth_t",
  "image3d_t",
  "event_t",
  "sampler_t",
  // TODO: The following are not part of 'kernel_arg_base_type' metadata -
  // according to SPIR generator. Should they still be allowed (ignored)?
  "size_t",
  "ptrdiff_t",
  "uintptr_t",
  "intptr_t"
};
DCL_ARRAY_LENGTH(g_valid_ocl_opaque_types);

const char *g_opencl_opaque_sufix = "_t";

const char *g_ignored_ocl_types[] = {
  "struct ", "union ", "enum "
};
DCL_ARRAY_LENGTH(g_ignored_ocl_types);

///
/// OpenCL C Type tables
///
const char *g_valid_llvm_primitives[] = {
  "i1", "i8", "i16", "i32", "i64", "half", "float", "double", "void"
};
DCL_ARRAY_LENGTH(g_valid_llvm_primitives);

const char *g_valid_llvm_vector_element_types[] = {
  "i8", "i16", "i32", "i64", "half", "float", "double"
};
DCL_ARRAY_LENGTH(g_valid_llvm_vector_element_types);

const char *g_valid_llvm_image_types[] = {
  "opencl.image1d_t",
  "opencl.image1d_array_t",
  "opencl.image1d_buffer_t",
  "opencl.image2d_t",
  "opencl.image2d_depth_t",
  "opencl.image2d_msaa_t",
  "opencl.image2d_msaa_depth_t",
  "opencl.image2d_array_t",
  "opencl.image2d_array_msaa_t",
  "opencl.image2d_array_msaa_depth_t",
  "opencl.image2d_array_depth_t",
  "opencl.image3d_t",
};
DCL_ARRAY_LENGTH(g_valid_llvm_image_types);

const char *g_valid_llvm_opaque_types[] = {
  "opencl.event_t"
};
DCL_ARRAY_LENGTH(g_valid_llvm_opaque_types);

const char *g_llvm_opaque_prefix = "opencl.";

const char *g_valid_vector_type_lengths[] = {
  "2" , "3" , "4" , "8" , "16"
};
DCL_ARRAY_LENGTH(g_valid_vector_type_lengths);


const char *g_valid_instrinsic[] = {
  "llvm.memcpy."
};
DCL_ARRAY_LENGTH(g_valid_instrinsic);

const char *g_ignored_instrinsic[] = {
  "llvm.dbg."
};
DCL_ARRAY_LENGTH(g_ignored_instrinsic);


const char *g_valid_sync_bi[] = {
  "_Z7barrier"
};
DCL_ARRAY_LENGTH(g_valid_sync_bi);

const char *g_valid_address_space[] = {
  "private",
  "global",
  "constant",
  "local"
};
DCL_ARRAY_LENGTH(g_valid_address_space);

const char *g_valid_calling_convention[] = {
  "SPIR_FUNC",
  "SPIR_KERNEL"
};
DCL_ARRAY_LENGTH(g_valid_calling_convention);

const char *g_valid_linkage_type[] = {
  "private",
  "internal",
  "available_externally",
  "external"
};
DCL_ARRAY_LENGTH(g_valid_linkage_type);


const char *OPENCL_KERNELS = "opencl.kernels";
const char *OPENCL_SPIR_VERSION = "opencl.spir.version";
const char *OPENCL_OCL_VERSION = "opencl.ocl.version";
const char *OPENCL_KHR_EXTENSIONS = "opencl.used.extensions";
const char *OPENCL_CORE_FEATURES = "opencl.used.optional.core.features";
const char *OPENCL_COMPILER_OPTIONS = "opencl.compiler.options";
const char *g_valid_named_metadata[] = {
  "opencl.kernels",
  "opencl.enable.FP_CONTRACT",
  OPENCL_SPIR_VERSION,
  OPENCL_OCL_VERSION,
  OPENCL_KHR_EXTENSIONS,
  OPENCL_CORE_FEATURES,
  OPENCL_COMPILER_OPTIONS
};
DCL_ARRAY_LENGTH(g_valid_named_metadata);

const char *KERNEL_ARG_ADDR_SPACE = "kernel_arg_addr_space";
const char *KERNEL_ARG_TY = "kernel_arg_type";
const char *KERNEL_ARG_BASE_TY = "kernel_arg_base_type";
const char *g_valid_kernel_arg_info[] = {
  KERNEL_ARG_ADDR_SPACE,
  "kernel_arg_access_qual",
  KERNEL_ARG_TY,
  KERNEL_ARG_BASE_TY,
  "kernel_arg_type_qual",
  "kernel_arg_name",
};
DCL_ARRAY_LENGTH(g_valid_kernel_arg_info);

const char *g_valid_version_names[] = {
  "opencl.ocl.version",
  "opencl.spir.version"
};
DCL_ARRAY_LENGTH(g_valid_version_names);

const char *g_valid_spir_versions[][2] = {
  {"1", "2"}
};
DCL_ARRAY_LENGTH(g_valid_spir_versions)/2;

const char *g_valid_ocl_versions[][2] = {
  {"1", "2"}
};
DCL_ARRAY_LENGTH(g_valid_ocl_versions)/2;


///
/// get error info message functions
///

std::string getValidTripleMsg() {
  std::string Msg;
  Msg += "Valid " + STR_SPIR + " Triple options are:\n";
  Msg += STR_IND1 + "(32bit): ";
  Msg += SPIR32_TRIPLE;
  Msg += "\n";
  Msg += STR_IND1 + "(64bit): ";
  Msg += SPIR64_TRIPLE;
  Msg += "\n";
  return Msg;
}

std::string getValidDataLayoutMsg() {
  std::string Msg;
  Msg += "Valid " + STR_SPIR + " Data Layout options are:\n";
  Msg += STR_IND1 + "(32bit): ";
  Msg += SPIR32_DATA_LAYOUT;
  Msg += "\n";
  Msg += STR_IND1 + "(64bit): ";
  Msg += SPIR64_DATA_LAYOUT;
  Msg += "\n";
  return Msg;
}

std::string getValidOpenCLTypeMsg() {
  std::string Msg;
  Msg += "Valid OpenCL C Types in " + STR_SPIR + "\n";
  Msg += STR_IND1 + "Primitive types: ";
  for (unsigned i=0; i<g_valid_ocl_primitives_len; i++) {
    Msg += ((i==0) ? "" : ", ");
    Msg += g_valid_ocl_primitives[i];
  }
  Msg += "\n\n";
  Msg += STR_IND1 + "Opaque types:";
  for (unsigned i=0; i<g_valid_ocl_opaque_types_len; i++) {
    Msg += "\n" + STR_IND2;
    Msg += g_valid_ocl_opaque_types[i];
  }
  Msg += "\n\n";
  Msg += STR_IND1 + "Vector element types: ";
  for (unsigned i=0; i<g_valid_ocl_vector_element_types_len; i++) {
    Msg += ((i==0) ? "" : ", ");
    Msg += g_valid_ocl_vector_element_types[i];
  }
  Msg += "\n\n";
  Msg += STR_IND1 + "Vector type lengthes: ";
  for (unsigned i=0; i<g_valid_vector_type_lengths_len; i++) {
    Msg += ((i==0) ? "" : ", ");
    Msg += g_valid_vector_type_lengths[i];
  }
  Msg += "\n";
  return Msg;
}

std::string getValidLLVMTypeMsg() {
  std::string Msg;
  Msg += "Valid LLVM Types in " + STR_SPIR + "\n";
  Msg += STR_IND1 + "Primitive types: ";
  for (unsigned i=0; i<g_valid_llvm_primitives_len; i++) {
    Msg += ((i==0) ? "" : ", ");
    Msg += g_valid_llvm_primitives[i];
  }
  Msg += "\n\n";
  Msg += STR_IND1 + "Image opaque types:";
  for (unsigned i=0; i<g_valid_llvm_image_types_len; i++) {
    Msg += "\n" + STR_IND2;
    Msg += g_valid_llvm_image_types[i];
  }
  Msg += "\n\n";
  Msg += STR_IND1 + "Other opaque types:";
  for (unsigned i=0; i<g_valid_llvm_opaque_types_len; i++) {
    Msg += "\n" + STR_IND2;
    Msg += g_valid_llvm_opaque_types[i];
  }
  Msg += "\n\n";
  Msg += STR_IND1 + "Vector element types: ";
  for (unsigned i=0; i<g_valid_llvm_vector_element_types_len; i++) {
    Msg += ((i==0) ? "" : ", ");
    Msg += g_valid_llvm_vector_element_types[i];
  }
  Msg += "\n\n";
  Msg += STR_IND1 + "Vector type lengthes: ";
  for (unsigned i=0; i<g_valid_vector_type_lengths_len; i++) {
    Msg += ((i==0) ? "" : ", ");
    Msg += g_valid_vector_type_lengths[i];
  }
  Msg += "\n";
  Msg += "\n" + STR_IND1 + STR_NOTE +
    "In addition, arrays and structures of the above types are allowed.\n";
  return Msg;
}

std::string getValidKernelReturnTypeMsg() {
  return "SPIR kernel has to return void";
}

std::string getValidIntrinsicMsg() {
  std::string Msg;
  Msg += "Valid intrinsic in " + STR_SPIR + " are:\n";
  for (unsigned i=0; i<g_valid_instrinsic_len; i++) {
    std::stringstream SS;
    SS << STR_IND1 << g_valid_instrinsic[i] << "\n";
    Msg += SS.str();
  }
  return Msg;
}

std::string getValidAddressSpaceMsg() {
  std::string Msg;
  Msg += "Valid address space in " + STR_SPIR + " are:\n";
  for (unsigned i=0; i<g_valid_address_space_len; i++) {
    std::stringstream SS;
    SS << STR_IND1 << i << " - " << g_valid_address_space[i] << "\n";
    Msg += SS.str();
  }
  Msg += "\n" + STR_IND1 + STR_NOTE +
    "Casts between address spaces is disallowed in " + STR_SPIR + "\n";

  return Msg;
}

std::string getValidKernelArgAddressSpaceMsg() {
  std::string Msg;
  Msg += "Valid address spaces for kernel arguments in " + STR_SPIR + " are:\n";
  std::stringstream SS;
  SS << STR_IND1 << GLOBAL_ADDR_SPACE << " - " << g_valid_address_space[GLOBAL_ADDR_SPACE] << "\n";
  SS << STR_IND1 << CONSTANT_ADDR_SPACE << " - " << g_valid_address_space[CONSTANT_ADDR_SPACE] << "\n";
  SS << STR_IND1 << LOCAL_ADDR_SPACE << " - " << g_valid_address_space[LOCAL_ADDR_SPACE] << "\n";
  Msg += SS.str();
  return Msg;
}

extern std::string getValidGlobalAS3VariableMsg() {
  std::string Msg = "Function-scope variables in the local address space\n";
  Msg += STR_IND1 + "are represented by module-scope variables with addrspace(3).\n";
  Msg += STR_IND1 + "The name of the variables has to have the following format:\n";
  Msg += STR_IND1 + "@<function_name>.<variable_name>\n";
  return Msg;
}

extern std::string getValidGlobalVarAddressSpacesMsg() {
  std::string Msg;
  Msg += "Valid address spaces for module-scope variables " + STR_SPIR + " are:\n";
  std::stringstream SS;
  SS << STR_IND1 << CONSTANT_ADDR_SPACE << " - " << g_valid_address_space[CONSTANT_ADDR_SPACE] << "\n";
  SS << STR_IND1 << LOCAL_ADDR_SPACE << " - " << g_valid_address_space[LOCAL_ADDR_SPACE] << "\n";
  Msg += SS.str();
  return Msg;
}

std::string getValidCallingConventionMsg() {
  std::string Msg;
  Msg += "Valid user defined functions calling convention in ";
  Msg += STR_SPIR + " are:\n";
  for (unsigned i=0; i<g_valid_calling_convention_len; i++) {
    Msg += STR_IND1 + g_valid_calling_convention[i] + "\n";
  }
  return Msg;
}

std::string getValidLinkageTypeMsg() {
  std::string Msg;
  Msg += "Valid linkage types in " + STR_SPIR + " are:\n";
  for (unsigned i = 0; i < g_valid_linkage_type_len; i++) {
    Msg += STR_IND1 + g_valid_linkage_type[i] + "\n";
  }
  return Msg;
}

std::string getValidIndirectCallMsg() {
  std::string Msg;
  Msg += "Indirect Calls are not allowed in " + STR_SPIR +"\n";
  return Msg;
}

std::string getValidKernelArgInfoMsg() {
  std::string Msg;
  Msg += "Valid kernel arg info in " + STR_SPIR + " are:\n";
  for (unsigned i=0; i<g_valid_kernel_arg_info_len; i++) {
    Msg += STR_IND1 + g_valid_kernel_arg_info[i] + "\n";
  }
  Msg += "\n" + STR_IND1 + STR_NOTE +
    "All above are mandatory for each metadata kernel,"
    "except 'kernel_arg_name' which is optional.\n";
  return Msg;
}

std::string getValidVersionMsg() {
  std::string Msg;
  Msg += "Module in " + STR_SPIR + " must have these metadata versions:\n";
  for (unsigned i=0; i<g_valid_version_names_len; i++) {
    Msg += STR_IND1 + g_valid_version_names[i] + "\n";
  }
  Msg += "\n";
  Msg += STR_IND1 + "Valid versions for '" + OPENCL_OCL_VERSION + "'\n";
  for (unsigned i=0; i<g_valid_ocl_versions_len; i++) {
    const char *Major = g_valid_ocl_versions[i][0];
    const char *Minor = g_valid_ocl_versions[i][1];
    Msg += STR_IND2 + "{" + Major + "," + Minor + "}" + "\n";
  }
  Msg += "\n";
  Msg += STR_IND1 + "Valid versions for '" + OPENCL_SPIR_VERSION + "'\n";
  for (unsigned i=0; i<g_valid_spir_versions_len; i++) {
    const char *Major = g_valid_spir_versions[i][0];
    const char *Minor = g_valid_spir_versions[i][1];
    Msg += STR_IND2 + "{" + Major + "," + Minor + "}" + "\n";
  }

  return Msg;
}

std::string getValidMemFenceMsg() {
  std::string Msg;
  Msg += "Synchronization functions accept 'cl_mem_fence_flags' enumeration "
         "as an argument. This argument is i32 bitmap value.\n";
  Msg += STR_IND1 + "Valid values for 'cl_mem_fence_flags' are:\n";
  Msg += STR_IND2 + "1 - CLK_LOCAL_MEM_FENCE\n";
  Msg += STR_IND2 + "2 - CLK_GLOBAL_MEM_FENCE\n";
  Msg += STR_IND2 + "3 - CLK_LOCAL_MEM_FENCE | CLK_GLOBAL_MEM_FENCE\n";
  return Msg;
}

std::string getMapOpenCLToLLVMMsg() {
  std::string Msg;
  Msg += "OpenCL C mapping to SPIR\n";
  Msg += STR_IND1 + "Built-in Scalar Data Types:\n";
  Msg += STR_IND2 + "bool                          -> i1\n";
  Msg += STR_IND2 + "char, unsigned char, uchar    -> i8\n";
  Msg += STR_IND2 + "short, unsigned short, ushort -> i16\n";
  Msg += STR_IND2 + "int, unsigned int, uint       -> i32\n";
  Msg += STR_IND2 + "long, unsigned long, ulong    -> i64\n";
  Msg += STR_IND2 + "float                         -> float\n";
  Msg += STR_IND2 + "double                        -> double\n";
  Msg += STR_IND2 + "half                          -> half\n";
  Msg += STR_IND2 + "void                          -> void\n";
  Msg += "\n";
  Msg += STR_IND1 + "Built-in Vector Data Types (n = 2, 3, 4, 8, and 16):\n";
  Msg += STR_IND2 + "charn    -> < n x i8 >\n";
  Msg += STR_IND2 + "ucharn   -> < n x i8 >\n";
  Msg += STR_IND2 + "shortn   -> < n x i16 >\n";
  Msg += STR_IND2 + "ushortn  -> < n x i16 >\n";
  Msg += STR_IND2 + "intn     -> < n x i32 >\n";
  Msg += STR_IND2 + "uintn    -> < n x i32 >\n";
  Msg += STR_IND2 + "longn    -> < n x i64 >\n";
  Msg += STR_IND2 + "ulongn   -> < n x i64 >\n";
  Msg += STR_IND2 + "halfn    -> < n x half >\n";
  Msg += STR_IND2 + "floatn   -> < n x float >\n";
  Msg += STR_IND2 + "doublen  -> < n x double >\n";
  Msg += "\n";
  Msg += STR_IND1 + "Other Built-in Data Types:\n";
  Msg += STR_IND2 + "image1d_t                  -> opencl.image1d_t\n";
  Msg += STR_IND2 + "image1d_array_t            -> opencl.image1d_array_t\n";
  Msg += STR_IND2 + "image1d_buffer_t           -> opencl.image1d_buffer_t\n";
  Msg += STR_IND2 + "image2d_t                  -> opencl.image2d_t\n";
  Msg += STR_IND2 + "image2d_depth_t            -> opencl.image2d_depth_t\n";
  Msg += STR_IND2 + "image2d_msaa_t             -> opencl.image2d_msaa_t\n";
  Msg += STR_IND2 + "image2d_msaa_depth_t       -> opencl.image2d_msaa_depth_t\n";
  Msg += STR_IND2 + "image2d_array_t            -> opencl.image2d_array_t\n";
  Msg += STR_IND2 + "image2d_array_depth_t      -> opencl.image2d_array_depth_t\n";
  Msg += STR_IND2 + "image2d_array_msaa_t       -> opencl.image2d_array_msaa_t\n";
  Msg += STR_IND2 + "image2d_array_msaa_depth_t -> opencl.image2d_array_msaa_depth_t\n";
  Msg += STR_IND2 + "image3d_t                  -> opencl.image3d_t\n";
  Msg += STR_IND2 + "event_t                    -> opencl.event_t\n";
  Msg += STR_IND2 + "sampler_t                  -> i32\n";
  Msg += STR_IND2 + "size_t                     -> i32 or i64\n";
  Msg += STR_IND2 + "ptrdiff_t                  -> i32 or i64\n";
  Msg += STR_IND2 + "uintptr_t                  -> i32 or i64\n";
  Msg += STR_IND2 + "intptr_t                   -> i32 or i64\n";
  return Msg;
}

std::string getValidNamedMetadataMsg() {
  std::string Msg;
  Msg += "Valid named metadata in " + STR_SPIR + " are:\n";
  for (unsigned i=0; i<g_valid_named_metadata_len; i++) {
    Msg += STR_IND1 + g_valid_named_metadata[i] + "\n";
  }
  Msg += "\n" + STR_IND1 + STR_NOTE +
    "Except 'opencl.enable.FP_CONTRACT' all the others are mandatory.\n";
  return Msg;
}

std::string getValidCoreFeaturesMsg() {
  std::string Msg;
  Msg += "Valid optional core features:\n";
  for (unsigned i=0; i<g_valid_core_feature_len; i++) {
    std::string Str = g_valid_core_feature[i];
    Str = Str.substr(std::string(g_valid_core_feature_prefix).size());
    Msg += STR_IND2 + g_valid_core_feature[i];
    Msg += " - must be declared to indicate that " + Str + " types are used";
    Msg += "\n";
  }
  return Msg;
}

std::string getValidKHRExtensionsMsg() {
  std::string Msg;
  Msg += "Valid KHR extensions:\n";
  for (unsigned i=0; i<g_valid_khr_ext_len; i++) {
    std::string Str = g_valid_khr_ext[i];
    Str = Str.substr(std::string(g_valid_khr_ext_prefix).size());
    Msg += STR_IND2 + g_valid_khr_ext[i];
    //Msg += " - must be declared to indicate that " + Str + " extension is used";
    Msg += "\n";
  }
  return Msg;
}

std::string getValidCompilerOptionsMsg() {
  std::string Msg;
  Msg += "Valid compiler options:\n";
  for (unsigned i=0; i<g_valid_compiler_options_len; i++) {
    Msg += STR_IND2 + g_valid_compiler_options[i];
    Msg += "\n";
  }
  return Msg;
}

} // End SPIR namespace

