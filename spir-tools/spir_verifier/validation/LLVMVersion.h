#if defined(LLVM_VER_MAJOR) && defined(LLVM_VER_MINOR)
  #if (LLVM_VER_MAJOR == 3)
    #if (LLVM_VER_MINOR < 3)
      #define LLVM_VERSION 3200
    #else
      #define LLVM_VERSION 3300
    #endif
  #else
    #error("unsupported LLVM version")
  #endif
#endif
