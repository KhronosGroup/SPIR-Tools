set(TARGET_NAME SpirNameManglerTests)

add_llvm_unittest(${TARGET_NAME}
  MangleTest.cpp
  )

target_link_libraries (${TARGET_NAME}
  SpirNameMangler
  )