//===------------------------- ManglingUtils.cpp -------------------------===//
//
//                              SPIR Tools
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===---------------------------------------------------------------------===//

#include "ManglingUtils.h"

namespace SPIR {

  // String represenration for the primitive types.
  static const char* PrimitiveNames[PRIMITIVE_NUM] ={
    "bool",
    "uchar",
    "char",
    "ushort",
    "short",
    "uint",
    "int",
    "ulong",
    "long",
    "half",
    "float",
    "double",
    "void",
    "...",
    "image1d_t",
    "image2d_t",
    "image3d_t",
    "image1d_buffer_t",
    "image1d_array_t",
    "image2d_array_t",
    "event_t",
    "sampler_t"
  };

  const char* mangledTypes[PRIMITIVE_NUM] = {
    "b",  //BOOL
    "h",  //UCHAR
    "c",  //CHAR
    "t",  //USHORT
    "s",  //SHORT
    "j",  //UINT
    "i",  //INT
    "m",  //ULONG
    "l",  //LONG
    "Dh", //HALF
    "f",  //FLOAT
    "d",  //DOUBLE
    "v",  //VOID
    "z",  //VarArg
    "11ocl_image1d",            //PRIMITIVE_IMAGE_1D_T
    "11ocl_image2d",            //PRIMITIVE_IMAGE_2D_T
    "11ocl_image3d",            //PRIMITIVE_IMAGE_3D_T
    "17ocl_image1dbuffer",      //PRIMITIVE_IMAGE_1D_BUFFER_T
    "16ocl_image1darray",       //PRIMITIVE_IMAGE_1D_ARRAY_T
    "16ocl_image2darray",       //PRIMITIVE_IMAGE_2D_ARRAY_T
    "9ocl_event",               //PRIMITIVE_EVENT_T
    "11ocl_sampler"             //PRIMITIVE_SAMPLER_T
  };

  const char* readableAttribute[ATTR_NUM] = {
    "restrict",
    "volatile",
    "const",
    "__private",
    "__global",
    "__constant",
    "__local",
  };

  const char* mangledAttribute[ATTR_NUM] = {
    "r",
    "V",
    "K",
    "",
    "U3AS1",
    "U3AS2",
    "U3AS3",
  };

  const char* mangledPrimitiveString(TypePrimitiveEnum t) {
    return mangledTypes[t];
  }

  const char* readablePrimitiveString(TypePrimitiveEnum t) {
    return PrimitiveNames[t];
  }

  const char* getMangledAttribute(TypeAttributeEnum attribute) {
    return mangledAttribute[attribute];
  }

  const char* getReadableAttribute(TypeAttributeEnum attribute) {
    return readableAttribute[attribute];
  }

} // End SPIR namespace
