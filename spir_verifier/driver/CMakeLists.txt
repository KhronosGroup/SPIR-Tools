set(TARGET_NAME spir_verifier)

add_llvm_tool(${TARGET_NAME}
  SpirVerifier.cpp
  )

include_directories(
  ${SPIR_ROOT_DIR}
  )

if (NOT WIN32)
  set(THREAD_LIB 
    pthread
    dl
    )
endif(NOT WIN32)

target_link_libraries(${TARGET_NAME}
  SpirValidation
  LLVMBitReader
  LLVMCore
  LLVMSupport
  ${THREAD_LIB}
  )

