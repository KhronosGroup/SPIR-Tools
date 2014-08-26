set(TARGET_NAME SpirValidation)

set(SOURCE_FILES
  SpirErrors.cpp
  SpirIterators.cpp
  SpirTables.cpp
  SpirValidation.cpp
  )

set(HEADER_FILES
  SpirErrors.h
  SpirIterators.h
  SpirTables.h
  SpirValidation.h
  )

include_directories(
  ${CMAKE_SOURCE_DIR}/backend/passes
  )

add_llvm_library(${TARGET_NAME}
  ${SOURCE_FILES}
  ${HEADER_FILES}
  )


add_definitions(-DLLVM_VER_MAJOR=${LLVM_VERSION_MAJOR})
add_definitions(-DLLVM_VER_MINOR=${LLVM_VERSION_MINOR})