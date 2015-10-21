//===--------- MangleTest.cpp - Test for SPIR mangler ----------*- C++ -*-===//
//
//                              SPIR Tools
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===---------------------------------------------------------------------===//

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
  // "abs(int)"
  NameMangler nm(SPIR12);
  const char* s = "_Z3absi";
  FunctionDescriptor fd;
  RefParamType primitiveInt(new PrimitiveType(PRIMITIVE_INT));

  fd.name = "abs";
  fd.parameters.push_back(primitiveInt);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasic, scalarintMangler) {
  // "abs(int)"
  const char* s = "_Z3absi";
  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType primitiveInt(new PrimitiveType(PRIMITIVE_INT));

  fd.name = "abs";
  fd.parameters.push_back(primitiveInt);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasic, scalarfloat) {
  // "fabs(float)"
  const char* s = "_Z4fabsf";
  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType primitiveFloat(new PrimitiveType(PRIMITIVE_FLOAT));

  fd.name = "fabs";
  fd.parameters.push_back(primitiveFloat);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasic, scalardouble) {
  // "fabs(double)"
  const char* s = "_Z4fabsd";
  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType primitiveDouble(new PrimitiveType(PRIMITIVE_DOUBLE));

  fd.name = "fabs";
  fd.parameters.push_back(primitiveDouble);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasic, PtrType) {
  // fract_ret2ptr(float, __private float *, __private float *)
  const char*const s = "_Z13fract_ret2ptrfPfS0_";
  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType F(new PrimitiveType(PRIMITIVE_FLOAT));
  RefParamType PF(new PointerType(F));

  fd.name = "fract_ret2ptr";
  fd.parameters.push_back(F);
  fd.parameters.push_back(PF);
  fd.parameters.push_back(PF);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasic, AtomicInt) {
  // "atomic_store(volatile __global atomic_int *, int)"
  const char* s = "_Z12atomic_storePVU3AS1U7_Atomicii";
  NameMangler nm(SPIR20);
  FunctionDescriptor fd;
  RefParamType primitiveInt(new PrimitiveType(PRIMITIVE_INT));
  RefParamType AtomicInt(new AtomicType(primitiveInt));
  PointerType *AtomicIntPtr = new PointerType(AtomicInt);
  AtomicIntPtr->setAddressSpace(ATTR_GLOBAL);
  AtomicIntPtr->setQualifier(ATTR_VOLATILE,true);
  RefParamType ptrRef(AtomicIntPtr);

  fd.name = "atomic_store";
  fd.parameters.push_back(ptrRef);
  fd.parameters.push_back(primitiveInt);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());

  // Test SPIR version correctness
  NameMangler nm2(SPIR12);
  MangleError err2 = nm2.mangle(fd, mangled);
  ASSERT_EQ(err2, MANGLE_TYPE_NOT_SUPPORTED);
  ASSERT_STREQ("Type volatile __global atomic_int * is not supported in SPIR 1.2", mangled.c_str());
}

TEST(MangleBasic, AtomicTypes) {
  // "myfunc(atomic_int, atomic_uint, atomic_long,
  //         atomic_ulong, atomic_float, atomic_double)"
  const char* s = "_Z6myfuncU7_AtomiciU7_AtomicjU7_Atomicl"
                  "U7_AtomicmU7_AtomicfU7_Atomicd";
  NameMangler nm(SPIR20);
  FunctionDescriptor fd;
  RefParamType primitiveInt(new PrimitiveType(PRIMITIVE_INT));
  RefParamType AtomicInt(new AtomicType(primitiveInt));
  RefParamType primitiveUInt(new PrimitiveType(PRIMITIVE_UINT));
  RefParamType AtomicUInt(new AtomicType(primitiveUInt));
  RefParamType primitiveLong(new PrimitiveType(PRIMITIVE_LONG));
  RefParamType AtomicLong(new AtomicType(primitiveLong));
  RefParamType primitiveULong(new PrimitiveType(PRIMITIVE_ULONG));
  RefParamType AtomicULong(new AtomicType(primitiveULong));
  RefParamType primitiveFloat(new PrimitiveType(PRIMITIVE_FLOAT));
  RefParamType AtomicFloat(new AtomicType(primitiveFloat));
  RefParamType primitiveDouble(new PrimitiveType(PRIMITIVE_DOUBLE));
  RefParamType AtomicDouble(new AtomicType(primitiveDouble));

  fd.name = "myfunc";
  fd.parameters.push_back(AtomicInt);
  fd.parameters.push_back(AtomicUInt);
  fd.parameters.push_back(AtomicLong);
  fd.parameters.push_back(AtomicULong);
  fd.parameters.push_back(AtomicFloat);
  fd.parameters.push_back(AtomicDouble);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());

  // Test SPIR version correctness
  NameMangler nm2(SPIR12);
  MangleError err2 = nm2.mangle(fd, mangled);
  ASSERT_EQ(err2, MANGLE_TYPE_NOT_SUPPORTED);
  ASSERT_STREQ("Type atomic_int is not supported in SPIR 1.2", mangled.c_str());
}


TEST(MangleBasic, BlockType) {
  // "myfunc(void (int, float)*);
  const char* s = "_Z6myfuncU13block_pointerFvifE";
  NameMangler nm(SPIR20);
  FunctionDescriptor fd;
  RefParamType primitiveInt(new PrimitiveType(PRIMITIVE_INT));
  RefParamType primitiveFloat(new PrimitiveType(PRIMITIVE_FLOAT));
  BlockType *block = new BlockType();
  block->setParam(0, primitiveInt);
  block->setParam(1, primitiveFloat);

  fd.name = "myfunc";
  fd.parameters.push_back(block);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());

  // Test SPIR version correctness
  NameMangler nm2(SPIR12);
  MangleError err2 = nm2.mangle(fd, mangled);
  ASSERT_EQ(err2, MANGLE_TYPE_NOT_SUPPORTED);
  ASSERT_STREQ("Type void (int, float)* is not supported in SPIR 1.2", mangled.c_str());
}


TEST(MangleBasic, EventType) {
 //"async_work_group_copy(__local int *,const __global int *, size_t, event_t)"
  const char* s = "_Z21async_work_group_copyPU3AS3iPKU3AS1ij9ocl_event";
  NameMangler nm(SPIR20);
  FunctionDescriptor fd;
  RefParamType primInt(new PrimitiveType(PRIMITIVE_INT));
  PointerType *intPtr1 = new PointerType(primInt);
  intPtr1->setAddressSpace(ATTR_LOCAL);
  RefParamType ptrRef1(intPtr1);
  PointerType *intPtr2 = new PointerType(primInt);
  intPtr2->setAddressSpace(ATTR_GLOBAL);
  intPtr2->setQualifier(ATTR_CONST,true);
  RefParamType ptrRef2(intPtr2);
  RefParamType primUInt(new PrimitiveType(PRIMITIVE_UINT)); //size_t
  RefParamType primitiveEvent(new PrimitiveType(PRIMITIVE_EVENT_T));

  fd.name="async_work_group_copy";
  fd.parameters.push_back(ptrRef1);
  fd.parameters.push_back(ptrRef2);
  fd.parameters.push_back(primUInt);
  fd.parameters.push_back(primitiveEvent);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasic, ClkEventType) {
 //"retain_event(clk_event_t)"
  const char* s = "_Z12retain_event12ocl_clkevent";
  NameMangler nm(SPIR20);
  FunctionDescriptor fd;
  RefParamType primClkEvent(new PrimitiveType(PRIMITIVE_CLK_EVENT_T));

  fd.name="retain_event";
  fd.parameters.push_back(primClkEvent);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());

  // Test SPIR version correctness
  NameMangler nm2(SPIR12);
  MangleError err2 = nm2.mangle(fd, mangled);
  ASSERT_EQ(err2, MANGLE_TYPE_NOT_SUPPORTED);
  ASSERT_STREQ("Type clk_event_t is not supported in SPIR 1.2", mangled.c_str());
}


TEST(MangleBasic, PipeAndReserveID) { //TODO - check if should fix function
 //"read_pipe(pipe int p, reserve_id_t reserve_id, uint index, int * ptr)"
  const char* s = "_Z9read_pipePU3AS18ocl_pipe13ocl_reserveidjPU3AS4vjj";
  NameMangler nm(SPIR20);
  FunctionDescriptor fd;
  RefParamType primPipe(new PrimitiveType(PRIMITIVE_PIPE_T));
  PointerType *pipePtr(new PointerType(primPipe));
  pipePtr->setAddressSpace(ATTR_GLOBAL);
  RefParamType pipePtrRef(pipePtr);
  RefParamType primReserveID(new PrimitiveType(PRIMITIVE_RESERVE_ID_T));
  RefParamType prmUInt(new PrimitiveType(PRIMITIVE_UINT));
  RefParamType primGentype(new PrimitiveType(PRIMITIVE_VOID));
  PointerType *gentypePtr(new PointerType(primGentype));
  gentypePtr->setAddressSpace(ATTR_GENERIC);
  RefParamType gentypePtrRef(gentypePtr);

  fd.name="read_pipe";
  fd.parameters.push_back(pipePtrRef);
  fd.parameters.push_back(primReserveID);
  fd.parameters.push_back(prmUInt);
  fd.parameters.push_back(gentypePtrRef);
  fd.parameters.push_back(prmUInt);
  fd.parameters.push_back(prmUInt);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());

  // Test SPIR version correctness
  NameMangler nm2(SPIR12);
  MangleError err2 = nm2.mangle(fd, mangled);
  ASSERT_EQ(err2, MANGLE_TYPE_NOT_SUPPORTED);
  ASSERT_STREQ("Type __global pipe_t * is not supported in SPIR 1.2", mangled.c_str());
}

TEST(MangleBasic, queueNDrangeBlock) {
 //"enqueue_kernel(queue_t queue, kernel_enqueue_flags_t flags,
 //               ndrange_t ndrange, void (^block)(void))"
  const char* s = "_Z14enqueue_kernel9ocl_queuei"
                  "9ndrange_tU13block_pointerFvvE";
  NameMangler nm(SPIR20);
  FunctionDescriptor fd;
  RefParamType primQueue(new PrimitiveType(PRIMITIVE_QUEUE_T));
  RefParamType primInt(new PrimitiveType(PRIMITIVE_INT));
  RefParamType primNDrange(new PrimitiveType(PRIMITIVE_NDRANGE_T));
  RefParamType primVoid(new PrimitiveType(PRIMITIVE_VOID));
  BlockType *block = new BlockType();
  block->setParam(0, primVoid);
  RefParamType blockRef(block);

  fd.name="enqueue_kernel";
  fd.parameters.push_back(primQueue);
  fd.parameters.push_back(primInt);
  fd.parameters.push_back(primNDrange);
  fd.parameters.push_back(blockRef);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());

  // Test SPIR version correctness
  NameMangler nm2(SPIR12);
  MangleError err2 = nm2.mangle(fd, mangled);
  ASSERT_EQ(err2, MANGLE_TYPE_NOT_SUPPORTED);
  ASSERT_STREQ("Type queue_t is not supported in SPIR 1.2", mangled.c_str());
}

TEST(MangleBasic, NDrangeVersion) {
 // Test SPIR version correctness
 //"myfunc(ndrange_t ndrange)"
  const char* s = "_Z6myfunc9ndrange_t";
  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType primNDrange(new PrimitiveType(PRIMITIVE_NDRANGE_T));

  fd.name="myfunc";
  fd.parameters.push_back(primNDrange);

  std::string mangled;
  MangleError err = nm.mangle(fd, mangled);
  ASSERT_EQ(err, MANGLE_TYPE_NOT_SUPPORTED);
  ASSERT_STREQ("Type ndrange_t is not supported in SPIR 1.2", mangled.c_str());
}

TEST(MangleBasic, ReserveIDVersion) {
 // Test SPIR version correctness
 //"myfunc(reserve_id_t id)"
  const char* s = "_Z6myfunc13ocl_reserveid";
  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType primReserveID(new PrimitiveType(PRIMITIVE_RESERVE_ID_T));

  fd.name="myfunc";
  fd.parameters.push_back(primReserveID);

  std::string mangled;
  MangleError err = nm.mangle(fd, mangled);
  ASSERT_EQ(err, MANGLE_TYPE_NOT_SUPPORTED);
  ASSERT_STREQ("Type reserve_id_t is not supported in SPIR 1.2", mangled.c_str());
}

TEST(MangleBasic, userDefinedTypes) {
  // "myfunc(myTy1, myTy2)"
  const char* s = "_Z6myfunc5myTy15myTy2";
  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType userType1(new UserDefinedType("myTy1"));
  RefParamType userType2(new UserDefinedType("myTy2"));

  fd.name = "myfunc";
  fd.parameters.push_back(userType1);
  fd.parameters.push_back(userType2);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasicImage, imageBuiltin1) {
  // "read_imagef(image1d_t, sampler_t, float2)"
  const char* s = "_Z11read_imagef11ocl_image1d11ocl_samplerDv2_f";
  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType primImage1d(new PrimitiveType(PRIMITIVE_IMAGE_1D_T));
  RefParamType primitiveSampler(new PrimitiveType(PRIMITIVE_SAMPLER_T));
  RefParamType primitiveFloat(new PrimitiveType(PRIMITIVE_FLOAT));
  RefParamType vectorFloat(new VectorType(primitiveFloat,2));

  fd.name="read_imagef";
  fd.parameters.push_back(primImage1d);
  fd.parameters.push_back(primitiveSampler);
  fd.parameters.push_back(vectorFloat);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasicImage, imageBuiltin2) {
  // "read_imagef(image1d_array_t, sampler_t, float2)"
  const char* s = "_Z11read_imagef16ocl_image1darray11ocl_samplerDv2_f";
  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType primImage1dArray(new PrimitiveType(PRIMITIVE_IMAGE_1D_ARRAY_T));
  RefParamType primitiveSampler(new PrimitiveType(PRIMITIVE_SAMPLER_T));
  RefParamType primitiveFloat(new PrimitiveType(PRIMITIVE_FLOAT));
  RefParamType vectorFloat(new VectorType(primitiveFloat,2));

  fd.name="read_imagef";
  fd.parameters.push_back(primImage1dArray);
  fd.parameters.push_back(primitiveSampler);
  fd.parameters.push_back(vectorFloat);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasicImage, imageBuiltin3) {
  // "read_imagef(image1d_buffer_t, float2)"
  const char* s = "_Z11read_imagef17ocl_image1dbufferDv2_f";
  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType primImage1dBuff(new PrimitiveType(PRIMITIVE_IMAGE_1D_BUFFER_T));
  RefParamType primitiveFloat(new PrimitiveType(PRIMITIVE_FLOAT));
  RefParamType vectorFloat(new VectorType(primitiveFloat,2));

  fd.name="read_imagef";
  fd.parameters.push_back(primImage1dBuff);
  fd.parameters.push_back(vectorFloat);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasicImage, imageBuiltin4) {
  // "read_imagef(image2d_t, sampler_t, float2)"
  const char* s = "_Z11read_imagef11ocl_image2d11ocl_samplerDv2_f";
  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType primImage2d(new PrimitiveType(PRIMITIVE_IMAGE_2D_T));
  RefParamType primitiveSampler(new PrimitiveType(PRIMITIVE_SAMPLER_T));
  RefParamType primitiveFloat(new PrimitiveType(PRIMITIVE_FLOAT));
  RefParamType vectorFloat(new VectorType(primitiveFloat,2));

  fd.name="read_imagef";
  fd.parameters.push_back(primImage2d);
  fd.parameters.push_back(primitiveSampler);
  fd.parameters.push_back(vectorFloat);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasicImage, imageBuiltin5) {
  // "read_imagef(image2d_array_t, sampler_t, float4)"
  const char* s = "_Z11read_imagef16ocl_image2darray11ocl_samplerDv4_f";
  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType primImage2dArray(new PrimitiveType(PRIMITIVE_IMAGE_2D_ARRAY_T));
  RefParamType primitiveSampler(new PrimitiveType(PRIMITIVE_SAMPLER_T));
  RefParamType primitiveFloat(new PrimitiveType(PRIMITIVE_FLOAT));
  RefParamType vectorFloat(new VectorType(primitiveFloat,4));

  fd.name="read_imagef";
  fd.parameters.push_back(primImage2dArray);
  fd.parameters.push_back(primitiveSampler);
  fd.parameters.push_back(vectorFloat);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasicImage, imageBuiltin6) {
  // "read_imagef(image3d_t, sampler_t, float4)"
  const char* s = "_Z11read_imagef11ocl_image3d11ocl_samplerDv4_f";
  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType primImage3d(new PrimitiveType(PRIMITIVE_IMAGE_3D_T));
  RefParamType primitiveSampler(new PrimitiveType(PRIMITIVE_SAMPLER_T));
  RefParamType primitiveFloat(new PrimitiveType(PRIMITIVE_FLOAT));
  RefParamType vectorFloat(new VectorType(primitiveFloat,4));

  fd.name="read_imagef";
  fd.parameters.push_back(primImage3d);
  fd.parameters.push_back(primitiveSampler);
  fd.parameters.push_back(vectorFloat);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasicImage, imageBuiltin7) {
  // "read_imagef(image2d_msaa_t, int2, int)"
  const char* s = "_Z11read_imagef15ocl_image2dmsaaDv2_ii";
  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType primImage2dMsaa(new PrimitiveType(PRIMITIVE_IMAGE_2D_MSAA_T));
  RefParamType primitiveInteger(new PrimitiveType(PRIMITIVE_INT));
  RefParamType vectorInt(new VectorType(primitiveInteger,2));

  fd.name="read_imagef";
  fd.parameters.push_back(primImage2dMsaa);
  fd.parameters.push_back(vectorInt);
  fd.parameters.push_back(primitiveInteger);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasicImage, imageBuiltin8) {
  // "read_imagef(image2d_array_msaa_t, int4, int)"
  const char* s = "_Z11read_imagef20ocl_image2darraymsaaDv4_ii";
  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType primImage2dArrMsaa(new
    PrimitiveType(PRIMITIVE_IMAGE_2D_ARRAY_MSAA_T));
  RefParamType primitiveInteger(new PrimitiveType(PRIMITIVE_INT));
  RefParamType vectorInt(new VectorType(primitiveInteger,4));

  fd.name="read_imagef";
  fd.parameters.push_back(primImage2dArrMsaa);
  fd.parameters.push_back(vectorInt);
  fd.parameters.push_back(primitiveInteger);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasicImage, imageBuiltin9) {
  // "read_imagef(image2d_msaa_depth_t, int2, int)"
  const char* s = "_Z11read_imagef20ocl_image2dmsaadepthDv2_ii";
  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType primImage2dMsaaDepth(new 
    PrimitiveType(PRIMITIVE_IMAGE_2D_MSAA_DEPTH_T));
  RefParamType primitiveInteger(new PrimitiveType(PRIMITIVE_INT));
  RefParamType vectorInt(new VectorType(primitiveInteger,2));

  fd.name="read_imagef";
  fd.parameters.push_back(primImage2dMsaaDepth);
  fd.parameters.push_back(vectorInt);
  fd.parameters.push_back(primitiveInteger);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasicImage, imageBuiltin10) {
  // "read_imagef(image2d_array_msaa_depth_t, int2, int)"
  const char* s = "_Z11read_imagef25ocl_image2darraymsaadepthDv2_ii";
  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType primImage2dArrMsaaDepth(new
    PrimitiveType(PRIMITIVE_IMAGE_2D_ARRAY_MSAA_DEPTH_T));
  RefParamType primitiveInteger(new PrimitiveType(PRIMITIVE_INT));
  RefParamType vectorInt(new VectorType(primitiveInteger,2));

  fd.name="read_imagef";
  fd.parameters.push_back(primImage2dArrMsaaDepth);
  fd.parameters.push_back(vectorInt);
  fd.parameters.push_back(primitiveInteger);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasicImage, imageBuiltin11) {
  // "read_imagef(image2d_depth_t, int2, int, float)"
  const char* s = "_Z11read_imagef16ocl_image2ddepthDv2_iif";
  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType primImage2dDepth(new PrimitiveType(PRIMITIVE_IMAGE_2D_DEPTH_T));
  RefParamType primitiveInteger(new PrimitiveType(PRIMITIVE_INT));
  RefParamType vectorInt(new VectorType(primitiveInteger,2));
  RefParamType primitiveFloat(new PrimitiveType(PRIMITIVE_FLOAT));

  fd.name="read_imagef";
  fd.parameters.push_back(primImage2dDepth);
  fd.parameters.push_back(vectorInt);
  fd.parameters.push_back(primitiveInteger);
  fd.parameters.push_back(primitiveFloat);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasicImage, imageBuiltin12) {
  // "read_imagef(image2d_array_depth_t, sampler_t, float4)"
  const char* s = "_Z11read_imagef21ocl_image2darraydepth11ocl_samplerDv4_f";
  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType primImage2dArrayDepth(new
  PrimitiveType(PRIMITIVE_IMAGE_2D_ARRAY_DEPTH_T));
  RefParamType primitiveSampler(new PrimitiveType(PRIMITIVE_SAMPLER_T));
  RefParamType primitiveFloat(new PrimitiveType(PRIMITIVE_FLOAT));
  RefParamType vectorFloat(new VectorType(primitiveFloat,4));

  fd.name="read_imagef";
  fd.parameters.push_back(primImage2dArrayDepth);
  fd.parameters.push_back(primitiveSampler);
  fd.parameters.push_back(vectorFloat);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleTest, vecAndVecPtr) {
  // "frexp(float2, __global int2*)"
  const char* s = "_Z5frexpDv2_fPU3AS1Dv2_i";
  NameMangler nm(SPIR12);
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

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleTest, mask_fmax) {
  // "mask_fmax(ushort, float16, float16)"
  const char* s = "_Z9mask_fmaxtDv16_fDv16_f";
  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType primitiveUshort(new PrimitiveType(PRIMITIVE_USHORT));
  RefParamType primitiveFloat(new PrimitiveType(PRIMITIVE_FLOAT));
  RefParamType vectorFloat(new VectorType(primitiveFloat, 16));

  fd.name = "mask_fmax";
  fd.parameters.push_back(primitiveUshort);
  fd.parameters.push_back(vectorFloat);
  fd.parameters.push_back(vectorFloat);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleTest, duplicateParam) {
  // "soa_cross3(float16, float16, float16, float16, float16,
  //             float16, float16*, float16*, float16*)"
  const char* s =
    "_Z10soa_cross3Dv16_fDv16_fDv16_fDv16_fDv16_fDv16_fPDv16_fS0_S0_";
  NameMangler nm(SPIR12);
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

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleTest, addrSpaceAttrs) {
  // "async_work_group_copy(__local char2 *, __global const char2 *,
  //                        __private char2 *, __constant char2 *)"
  const char* s =
    "_Z21async_work_group_copyPU3AS3Dv2_cPKU3AS1Dv2_cPDv2_cPU3AS2Dv2_c";

  NameMangler nm(SPIR12);
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

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleTest, addressSpaceAndUserDefTy) {
  // "myf(__constant mta*)"
  const char* s = "_Z3myfPU3AS23mta";

  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType userType(new UserDefinedType("mta"));
  PointerType* userTypePtr(new PointerType(userType));
  userTypePtr->setAddressSpace(ATTR_CONSTANT);
  RefParamType userTypePtrRef(userTypePtr);

  fd.name = "myf";
  fd.parameters.push_back(userTypePtrRef);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleTest, substitutionTest) {
  // ndrange_t ndrange_2D (const size_t global_work_offset[2], const size_t global_work_size[2], const size_t local_work_size[2])
  const char* s = "_Z10ndrange_2DPKiS0_S0_";

  NameMangler nm(SPIR20);
  FunctionDescriptor fd;
  RefParamType intType(new PrimitiveType(PRIMITIVE_INT));
  PointerType* intTypePtr(new PointerType(intType));
  intTypePtr->setQualifier(ATTR_CONST, true);
  RefParamType intTypePtrRef(intTypePtr);

  fd.name = "ndrange_2D";
  fd.parameters.push_back(intTypePtrRef);
  fd.parameters.push_back(intTypePtrRef);
  fd.parameters.push_back(intTypePtrRef);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleTest, substitutionOrderTest) {
  // void func (const size_t x[2], float *y, const size_t z[2], int w, float *a)
  const char* s = "_Z4funcPKiPfS0_iS1_";

  NameMangler nm(SPIR20);
  FunctionDescriptor fd;
  RefParamType intType(new PrimitiveType(PRIMITIVE_INT));
  PointerType* intTypePtr(new PointerType(intType));
  intTypePtr->setQualifier(ATTR_CONST, true);
  RefParamType intTypePtrRef(intTypePtr);
  RefParamType floatType(new PrimitiveType(PRIMITIVE_FLOAT));
  PointerType* floatTypePtr(new PointerType(floatType));

  fd.name = "func";
  fd.parameters.push_back(intTypePtrRef);
  fd.parameters.push_back(floatTypePtr);
  fd.parameters.push_back(intTypePtrRef);
  fd.parameters.push_back(intType);
  fd.parameters.push_back(floatTypePtr);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(MangleBasic, userDefinedTypesSubstitution) {
  // "myfunc(myTy1, myTy2)"
  const char* s = "_Z6myfunc5myTy1S0_";
  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType userType1(new UserDefinedType("myTy1"));

  fd.name = "myfunc";
  fd.parameters.push_back(userType1);
  fd.parameters.push_back(userType1);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

// Order should be: restrict, volatile, const, address space.
TEST(AttrOrderTest, pointerAttributes1) {
  // func(restrict volatile const __constant int*)
  const char* s = "_Z4funcPrVKU3AS2i";

  NameMangler nm(SPIR12);
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

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(AttrOrderTest, pointerAttributes2) {
  // func(restrict volatile const __constant int*)
  const char* s = "_Z4funcPrVKU3AS2i";

  NameMangler nm(SPIR12);
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

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(AttrOrderTest, pointerAttributes3) {
  // func(restrict const __constant int*)
  const char* s = "_Z4funcPrKU3AS2i";

  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType primitiveInt(new PrimitiveType(PRIMITIVE_INT));
  PointerType *ptrInt = new PointerType(primitiveInt);
  ptrInt->setAddressSpace(ATTR_CONSTANT);
  ptrInt->setQualifier(ATTR_RESTRICT, true);
  ptrInt->setQualifier(ATTR_CONST, true);
  RefParamType ptrIntRef(ptrInt);

  fd.name = "func";
  fd.parameters.push_back(ptrIntRef);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(AttrOrderTest, pointerAttributes4) {
  // func(restrict const int*)
  const char* s = "_Z4funcPrKi";

  NameMangler nm(SPIR12);
  FunctionDescriptor fd;
  RefParamType primitiveInt(new PrimitiveType(PRIMITIVE_INT));
  PointerType *ptrInt = new PointerType(primitiveInt);
  ptrInt->setQualifier(ATTR_RESTRICT, true);
  ptrInt->setQualifier(ATTR_CONST, true);
  RefParamType ptrIntRef(ptrInt);

  fd.name = "func";
  fd.parameters.push_back(ptrIntRef);

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(AttrOrderTest, setPtrAdrsSpace) {
  // func(restrict volatile const __local int*)
  const char* s = "_Z4funcPrU3AS3i";

  NameMangler nm(SPIR12);
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

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

TEST(AttrOrderTest, removePtrQualifiers) {
  // func(int*)
  const char* s = "_Z4funcPi";

  NameMangler nm(SPIR12);
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

  std::string mangled;
  MangleError err = nm.mangle(fd,mangled);
  ASSERT_EQ(err, MANGLE_SUCCESS);
  ASSERT_STREQ(s, mangled.c_str());
}

}// End namespace test
}// End namespace namemangling

int main(int argc, char** argv)
{
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}
