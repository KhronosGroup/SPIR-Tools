add_custom_target(SpirUnitTests)
set_target_properties(SpirUnitTests PROPERTIES FOLDER "Tests")

function(add_llvm_unittest test_dirname)
  add_unittest(SpirUnitTests ${test_dirname} ${ARGN})
endfunction()

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  )

add_subdirectory(spir_name_mangler)