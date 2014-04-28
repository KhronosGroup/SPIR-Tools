//
//                     SPIR Tools
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//

#include "spir_name_mangler/FunctionDescriptor.h"
#include "spir_name_mangler/NameMangleAPI.h"
#include "spir_name_mangler/ParameterType.h"
#include "gtest/gtest.h"

using namespace SPIR;

namespace namemangling { namespace tests {

//
// Tests
//

TEST(MangleBasic, scalarint) {
  // "func(int)"
  const char* s = "_Z4funci";
  FunctionDescriptor fd;
  RefParamType primitiveInt(new PrimitiveType(PRIMITIVE_INT));

  fd.name = "func";
  fd.parameters.push_back(primitiveInt);
  
  std::string mangled = mangle(fd);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasic, scalarfloat) {
  // "func(float)"
  const char* s = "_Z4funcf";
  FunctionDescriptor fd;
  RefParamType primitiveFloat(new PrimitiveType(PRIMITIVE_FLOAT));

  fd.name = "func";
  fd.parameters.push_back(primitiveFloat);
  
  std::string mangled = mangle(fd);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasic, scalardouble) {
  // "func(double)"
  const char* s = "_Z4funcd";
  FunctionDescriptor fd;
  RefParamType primitiveDouble(new PrimitiveType(PRIMITIVE_DOUBLE));

  fd.name = "func";
  fd.parameters.push_back(primitiveDouble);
  
  std::string mangled = mangle(fd);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasic, PtrType) {
  // "func(float, float*, float*)"
  const char*const s = "_Z4FuncfPfPf";
  FunctionDescriptor fd;
  RefParamType F(new PrimitiveType(PRIMITIVE_FLOAT));
  RefParamType PF(new PointerType(F));

  fd.name = "Func";
  fd.parameters.push_back(F);
  fd.parameters.push_back(PF);
  fd.parameters.push_back(PF);
  
  std::string mangled = mangle(fd);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasic, imageBuiltin) {
  // "read_image(image2d_t, sampler_t, float2)"
  const char* s = "_Z11read_imagef11ocl_image2d11ocl_samplerDv2_f";
  FunctionDescriptor fd;
  RefParamType primitiveImage2d(new PrimitiveType(PRIMITIVE_IMAGE_2D_T));
  RefParamType primitiveSampler(new PrimitiveType(PRIMITIVE_SAMPLER_T));
  RefParamType primitiveFloat(new PrimitiveType(PRIMITIVE_FLOAT));
  RefParamType vectorFloat(new VectorType(primitiveFloat,2));

  fd.name="read_imagef";
  fd.parameters.push_back(primitiveImage2d);
  fd.parameters.push_back(primitiveSampler);
  fd.parameters.push_back(vectorFloat);

  std::string mangled = mangle(fd);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasic, userDefinedTypes) {
  // "myfunc(myTy1, myTy2)"
  const char* s = "_Z6myfunc5myTy15myTy2";
  FunctionDescriptor fd;
  RefParamType userType1(new UserDefinedType("myTy1"));
  RefParamType userType2(new UserDefinedType("myTy2"));

  fd.name = "myfunc";
  fd.parameters.push_back(userType1);
  fd.parameters.push_back(userType2);

  std::string mangled = mangle(fd);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleTest, vecAndVecPtr) {
  // "frexp(float2, __global int2*)"
  const char* s = "_Z5frexpDv2_fPU3AS1Dv2_i";
  FunctionDescriptor fd;
  RefParamType primitiveFloat(new PrimitiveType(PRIMITIVE_FLOAT));
  RefParamType vectorFloat(new VectorType(primitiveFloat, 2));

  RefParamType primitiveInt(new PrimitiveType(PRIMITIVE_INT));
  RefParamType vectorInt(new VectorType(primitiveInt, 2));
  PointerType *ptrInt = new PointerType(vectorInt);
  ptrInt->setAddressSpace(ATTR_GLOBAL);
  RefParamType ptrIntRef(ptrInt);

  fd.name = "frexp";
  fd.parameters.push_back(vectorFloat);
  fd.parameters.push_back(ptrIntRef);

  std::string mangled = mangle(fd);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleTest, mask_fmax) {
  // "mask_fmax(ushort, float16, float16)"
  const char* s = "_Z9mask_fmaxtDv16_fDv16_f";
  FunctionDescriptor fd;
  RefParamType primitiveUshort(new PrimitiveType(PRIMITIVE_USHORT));
  RefParamType primitiveFloat(new PrimitiveType(PRIMITIVE_FLOAT));
  RefParamType vectorFloat(new VectorType(primitiveFloat, 16));

  fd.name = "mask_fmax";
  fd.parameters.push_back(primitiveUshort);
  fd.parameters.push_back(vectorFloat);
  fd.parameters.push_back(vectorFloat);

  std::string mangled = mangle(fd);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleTest, duplicateParam) {
  // "soa_cross3(float16, float16, float16, float16, float16,
  //             float16, float16*, float16*, float16*)"
  const char* s =
    "_Z10soa_cross3Dv16_fDv16_fDv16_fDv16_fDv16_fDv16_fPDv16_fPDv16_fPDv16_f";
  FunctionDescriptor fd;
  RefParamType primitiveFloat(new PrimitiveType(PRIMITIVE_FLOAT));
  RefParamType vectorFloat(new VectorType(primitiveFloat,16));
  RefParamType ptrFloat(new PointerType(vectorFloat));

  fd.name = "soa_cross3";
  fd.parameters.push_back(vectorFloat);
  fd.parameters.push_back(vectorFloat);
  fd.parameters.push_back(vectorFloat);
  fd.parameters.push_back(vectorFloat);
  fd.parameters.push_back(vectorFloat);
  fd.parameters.push_back(vectorFloat);
  fd.parameters.push_back(ptrFloat);
  fd.parameters.push_back(ptrFloat);
  fd.parameters.push_back(ptrFloat);

  std::string mangled = mangle(fd);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleTest, addrSpaceAttrs) {
  // "async_work_group_copy(__local char2 *, __global const char2 *,
  //                        __private char2 *, __constant char2 *)"
  const char* s =
    "_Z21async_work_group_copyPU3AS3Dv2_cPKU3AS1Dv2_cPDv2_cPU3AS2Dv2_c";

  FunctionDescriptor fd;
  RefParamType primitiveChar(new PrimitiveType(PRIMITIVE_CHAR));
  RefParamType vectorChar(new VectorType(primitiveChar, 2));

  PointerType *ptrChar1 = new PointerType(vectorChar);
  ptrChar1->setAddressSpace(ATTR_LOCAL);
  RefParamType ptrChar1Ref(ptrChar1);

  PointerType *ptrChar2 = new PointerType(vectorChar);
  ptrChar2->setQualifier(ATTR_CONST, true);
  ptrChar2->setAddressSpace(ATTR_GLOBAL);
  RefParamType ptrChar2Ref(ptrChar2);

  PointerType *ptrChar3 = new PointerType(vectorChar);
  ptrChar3->setAddressSpace(ATTR_PRIVATE);
  RefParamType ptrChar3Ref(ptrChar3);

  PointerType *ptrChar4 = new PointerType(vectorChar);
  ptrChar4->setAddressSpace(ATTR_CONSTANT);
  RefParamType ptrChar4Ref(ptrChar4);

  fd.name = "async_work_group_copy";
  fd.parameters.push_back(ptrChar1Ref);
  fd.parameters.push_back(ptrChar2Ref);
  fd.parameters.push_back(ptrChar3Ref);
  fd.parameters.push_back(ptrChar4Ref);

  std::string mangled = mangle(fd);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleTest, addressSpaceAndUserDefTy) {
  // "myf(__constant mta*)"
  const char* s = "_Z3myfPU3AS23mta";

  FunctionDescriptor fd;
  RefParamType userType(new UserDefinedType("mta"));
  PointerType* userTypePtr(new PointerType(userType));
  userTypePtr->setAddressSpace(ATTR_CONSTANT);
  RefParamType userTypePtrRef(userTypePtr);

  fd.name = "myf";
  fd.parameters.push_back(userTypePtrRef);

  std::string mangled = mangle(fd);
  ASSERT_STREQ(s, mangled.c_str());
}

// Order should be: restrict, volatile, const, address space.
TEST(AttrOrderTest, pointerAttributes1) {
  // func(restrict volatile const __constant int*)
  const char* s = "_Z4funcPrVKU3AS2i";
  FunctionDescriptor fd;
  RefParamType primitiveInt(new PrimitiveType(PRIMITIVE_INT));
  PointerType *ptrInt = new PointerType(primitiveInt);
  ptrInt->setQualifier(ATTR_RESTRICT, true);
  ptrInt->setQualifier(ATTR_VOLATILE, true);
  ptrInt->setQualifier(ATTR_CONST, true);
  ptrInt->setAddressSpace(ATTR_CONSTANT);
  RefParamType ptrIntRef(ptrInt);

  fd.name = "func";
  fd.parameters.push_back(ptrIntRef);

  std::string mangled = mangle(fd);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(AttrOrderTest, pointerAttributes2) {
  // func(restrict volatile const __constant int*)
  const char* s = "_Z4funcPrVKU3AS2i";

  FunctionDescriptor fd;
  RefParamType primitiveInt(new PrimitiveType(PRIMITIVE_INT));
  PointerType *ptrInt = new PointerType(primitiveInt);
  ptrInt->setAddressSpace(ATTR_CONSTANT);
  ptrInt->setQualifier(ATTR_RESTRICT, true);
  ptrInt->setQualifier(ATTR_CONST, true);
  ptrInt->setQualifier(ATTR_VOLATILE, true);
  RefParamType ptrIntRef(ptrInt);

  fd.name = "func";
  fd.parameters.push_back(ptrIntRef);

  std::string mangled = mangle(fd);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(AttrOrderTest, pointerAttributes3) {
  // func(restrict const __constant int*)
  const char* s = "_Z4funcPrKU3AS2i";

  FunctionDescriptor fd;
  RefParamType primitiveInt(new PrimitiveType(PRIMITIVE_INT));
  PointerType *ptrInt = new PointerType(primitiveInt);
  ptrInt->setAddressSpace(ATTR_CONSTANT);
  ptrInt->setQualifier(ATTR_RESTRICT, true);
  ptrInt->setQualifier(ATTR_CONST, true);
  RefParamType ptrIntRef(ptrInt);

  fd.name = "func";
  fd.parameters.push_back(ptrIntRef);

  std::string mangled = mangle(fd);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(AttrOrderTest, pointerAttributes4) {
  // func(restrict const int*)
  const char* s = "_Z4funcPrKi";

  FunctionDescriptor fd;
  RefParamType primitiveInt(new PrimitiveType(PRIMITIVE_INT));
  PointerType *ptrInt = new PointerType(primitiveInt);
  ptrInt->setQualifier(ATTR_RESTRICT, true);
  ptrInt->setQualifier(ATTR_CONST, true);
  RefParamType ptrIntRef(ptrInt);

  fd.name = "func";
  fd.parameters.push_back(ptrIntRef);

  std::string mangled = mangle(fd);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(AttrOrderTest, setPtrAdrsSpace) {
  // func(restrict volatile const __local int*)
  const char* s = "_Z4funcPrU3AS3i";

  FunctionDescriptor fd;
  RefParamType primitiveInt(new PrimitiveType(PRIMITIVE_INT));
  PointerType *ptrInt = new PointerType(primitiveInt);
  ptrInt->setQualifier(ATTR_RESTRICT, true);
  ptrInt->setAddressSpace(ATTR_CONSTANT);
  ptrInt->setAddressSpace(ATTR_LOCAL);
  ptrInt->setAddressSpace(ATTR_GLOBAL);
  ptrInt->setAddressSpace(ATTR_LOCAL);
  RefParamType ptrIntRef(ptrInt);

  fd.name = "func";
  fd.parameters.push_back(ptrIntRef);

  std::string mangled = mangle(fd);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(AttrOrderTest, removePtrQualifiers) {
  // func(int*)
  const char* s = "_Z4funcPi";

  FunctionDescriptor fd;
  RefParamType primitiveInt(new PrimitiveType(PRIMITIVE_INT));
  PointerType *ptrInt = new PointerType(primitiveInt);
  ptrInt->setQualifier(ATTR_RESTRICT, true);
  ptrInt->setQualifier(ATTR_RESTRICT, true);
  ptrInt->setQualifier(ATTR_VOLATILE, true);
  ptrInt->setQualifier(ATTR_RESTRICT, false);
  ptrInt->setQualifier(ATTR_VOLATILE, false);
  RefParamType ptrIntRef(ptrInt);

  fd.name = "func";
  fd.parameters.push_back(ptrIntRef);

  std::string mangled = mangle(fd);
  ASSERT_STREQ(s, mangled.c_str());
}

}// End namespace test
}// End namespace namemangling

int main(int argc, char** argv)
{
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
