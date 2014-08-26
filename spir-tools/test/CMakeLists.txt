configure_lit_site_cfg(
  ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.in
  ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg
  )

# Don't include check-spir-tools into check-all without LLVM_BUILD_TOOLS.
if(NOT LLVM_BUILD_TOOLS)
  set(EXCLUDE_FROM_ALL ON)
endif()

# Set the depends list as a variable so that it can grow conditionally.
set(RS2SPIR_TEST_DEPENDS
          llvm-config
          llc
          lli
          lli-child-target
          llvm-ar
          llvm-as
          llvm-bcanalyzer
          llvm-c-test
          llvm-cov
          llvm-diff
          llvm-dis
          llvm-extract
          llvm-link
          llvm-lto
          llvm-mc
          llvm-mcmarkup
          llvm-nm
          llvm-objdump
          llvm-readobj
          llvm-rtdyld
          llvm-symbolizer
          opt
          FileCheck
          count
          not
          spir_verifier
        )

add_lit_testsuite(
	check-spir-tools "Running the SPIR tools tests"
	${CMAKE_CURRENT_BINARY_DIR}
	PARAMS llvm_site_config=${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg
	       build_config=
	DEPENDS ${RS2SPIR_TEST_DEPENDS}
)
set_target_properties(check-spir-tools PROPERTIES FOLDER "Tests")
