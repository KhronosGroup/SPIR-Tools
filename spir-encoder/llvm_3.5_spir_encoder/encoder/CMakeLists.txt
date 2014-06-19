set(TARGET_NAME SpirEncoder)

set(SOURCE_FILES
  SpirBitcodeWriter.cpp
  ${LLVM_MAIN_SRC_DIR}/lib/Bitcode/Writer/ValueEnumerator.cpp
  )

set(HEADER_FILES
  BitCodes.h
  BitstreamWriter.h
  LLVMBitCodes.h
  SpirBitcodeWriter.h
  ${LLVM_MAIN_SRC_DIR}/lib/Bitcode/Writer/ValueEnumerator.h
  )

include_directories(
  ${LLVM_MAIN_SRC_DIR}/lib/Bitcode/Writer/
  )

add_llvm_library(${TARGET_NAME}
  ${SOURCE_FILES}
  ${HEADER_FILES}
  )

set(HEADER_INSTALL_FILES
  SpirBitcodeWriter.h
  )

install(FILES ${HEADER_INSTALL_FILES} DESTINATION include/llvm/SpirTools)
