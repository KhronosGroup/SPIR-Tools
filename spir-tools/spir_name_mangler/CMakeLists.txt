set(TARGET_NAME SpirNameMangler)

set(SOURCE_FILES
  FunctionDescriptor.cpp
  Mangler.cpp
  ManglingUtils.cpp
  ParameterType.cpp
  )

set(HEADER_FILES
  FunctionDescriptor.h
  ManglingUtils.h
  NameMangleAPI.h
  ParameterType.h
  Refcount.h
  )

add_llvm_library(${TARGET_NAME}
  ${SOURCE_FILES}
  ${HEADER_FILES}
  )

set(HEADER_INSTALL_FILES
  Refcount.h
  FunctionDescriptor.h
  NameMangleAPI.h
  ParameterType.h
  )

install(FILES ${HEADER_INSTALL_FILES} DESTINATION include/llvm/SpirTools)