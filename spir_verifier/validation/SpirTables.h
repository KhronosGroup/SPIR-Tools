//
//                     SPIR Tools
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//

#ifndef __SPIR_TABLES_H__
#define __SPIR_TABLES_H__

#include <string>

namespace SPIR {

//
// Constant definitions.
//

extern const char *SPIR32_TRIPLE;
extern const char *SPIR64_TRIPLE ;
extern const char *SPIR32_DATA_LAYOUT;
extern const char *SPIR64_DATA_LAYOUT;

#define EXTREN_DCL_ARRAY_LENGTH(arr) extern const unsigned arr##_len

extern const char *CORE_FEATURE_CL_DOUBLES;
extern const char *CORE_FEATURE_CL_IMAGES;
extern const char *g_valid_core_feature[];
EXTREN_DCL_ARRAY_LENGTH(g_valid_core_feature);

extern const char *EXTENSION_CL_KHR_FP16;
extern const char *g_valid_khr_ext[];
EXTREN_DCL_ARRAY_LENGTH(g_valid_khr_ext);

extern const char *g_valid_compiler_options[];
EXTREN_DCL_ARRAY_LENGTH(g_valid_compiler_options);

///
/// OpenCL C Type tables
///
extern const char *g_valid_ocl_primitives[];
EXTREN_DCL_ARRAY_LENGTH(g_valid_ocl_primitives);

extern const char *g_valid_ocl_vector_element_types[];
EXTREN_DCL_ARRAY_LENGTH(g_valid_ocl_vector_element_types);

extern const char *g_valid_ocl_opaque_types[];
EXTREN_DCL_ARRAY_LENGTH(g_valid_ocl_opaque_types);

extern const char *g_opencl_opaque_sufix;

extern const char *g_ignored_ocl_types[];
EXTREN_DCL_ARRAY_LENGTH(g_ignored_ocl_types);

///
/// OpenCL C Type tables
///
extern const char *g_valid_llvm_primitives[];
EXTREN_DCL_ARRAY_LENGTH(g_valid_llvm_primitives);

extern const char *g_valid_llvm_vector_element_types[];
EXTREN_DCL_ARRAY_LENGTH(g_valid_llvm_vector_element_types);

extern const char *g_valid_llvm_image_types[];
EXTREN_DCL_ARRAY_LENGTH(g_valid_llvm_image_types);

extern const char *g_valid_llvm_opaque_types[];
EXTREN_DCL_ARRAY_LENGTH(g_valid_llvm_opaque_types);

extern const char *g_llvm_opaque_prefix;

///
/// Other OpenCL Tables
///
extern const char *g_valid_vector_type_lengths[];
EXTREN_DCL_ARRAY_LENGTH(g_valid_vector_type_lengths);

extern const char *g_valid_instrinsic[];
EXTREN_DCL_ARRAY_LENGTH(g_valid_instrinsic);

extern const char *g_ignored_instrinsic[];
EXTREN_DCL_ARRAY_LENGTH(g_ignored_instrinsic);

extern const char *g_valid_sync_bi[];
EXTREN_DCL_ARRAY_LENGTH(g_valid_sync_bi);

extern const char *g_valid_address_space[];
EXTREN_DCL_ARRAY_LENGTH(g_valid_address_space);

extern const char *g_valid_calling_convention[];
EXTREN_DCL_ARRAY_LENGTH(g_valid_calling_convention);

extern const char *OPENCL_KERNELS;
extern const char *OPENCL_SPIR_VERSION;
extern const char *OPENCL_OCL_VERSION;
extern const char *OPENCL_KHR_EXTENSIONS;
extern const char *OPENCL_CORE_FEATURES;
extern const char *OPENCL_COMPILER_OPTIONS;
extern const char *g_valid_named_metadata[];
EXTREN_DCL_ARRAY_LENGTH(g_valid_named_metadata);

extern const char *KERNEL_ARG_ADDR_SPACE;
extern const char *KERNEL_ARG_TY;
extern const char *KERNEL_ARG_BASE_TY;
extern const char *g_valid_kernel_arg_info[];
EXTREN_DCL_ARRAY_LENGTH(g_valid_kernel_arg_info);

extern const char *g_valid_version_names[];
EXTREN_DCL_ARRAY_LENGTH(g_valid_version_names);

extern const char *g_valid_spir_versions[][2];
EXTREN_DCL_ARRAY_LENGTH(g_valid_spir_versions);

extern const char *g_valid_ocl_versions[][2];
EXTREN_DCL_ARRAY_LENGTH(g_valid_ocl_versions);




///
/// get error info message functions
///

extern std::string getValidTripleMsg();

extern std::string getValidDataLayoutMsg();

extern std::string getValidOpenCLTypeMsg();

extern std::string getValidLLVMTypeMsg();

extern std::string getValidIntrinsicMsg();

extern std::string getValidAddressSpaceMsg();

extern std::string getValidCallingConventionMsg();

extern std::string getValidIndirectCallMsg();

extern std::string getValidKernelArgInfoMsg();

extern std::string getValidVersionMsg();

extern std::string getValidMemFenceMsg();

extern std::string getMapOpenCLToLLVMMsg();

extern std::string getValidNamedMetadataMsg();

extern std::string getValidCoreFeaturesMsg();

extern std::string getValidKHRExtensionsMsg();

extern std::string getValidCompilerOptionsMsg();

} // End SPIR namespace

#endif // __SPIR_TABLES_H__
