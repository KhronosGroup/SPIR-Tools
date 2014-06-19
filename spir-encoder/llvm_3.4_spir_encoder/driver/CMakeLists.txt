set(TARGET_NAME spir-encoder)

add_llvm_tool(${TARGET_NAME}
  SpirEncoder.cpp
  )

include_directories(
  ${SPIR_ENCODER_ROOT_DIR}
  )

target_link_libraries(${TARGET_NAME}
  SpirEncoder
  LLVMIRReader
  )
