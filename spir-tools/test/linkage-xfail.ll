target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v16:16:16-v24:32:32-v32:32:32-v48:64:64-v64:64:64-v96:128:128-v128:128:128-v192:256:256-v256:256:256-v512:512:512-v1024:1024:1024"
target triple = "spir64-unknown-unknown"

; RUN: llvm-as -o %t.bc %s
; RUN: not spir_verifier -LIT-test-mode %t.bc 2>%t.out
; RUN: FileCheck %s <%t.out

; the SPIR verifier shall report invalid linkage,
; once per each kernel here
;
; CHECK: ERR_INVALID_LINKAGE_TYPE
; CHECK-NEXT: kernel1
; CHECK: ERR_INVALID_LINKAGE_TYPE
; CHECK-NEXT: kernel2
; CHECK: ERR_INVALID_LINKAGE_TYPE
; CHECK-NEXT: kernel3
; CHECK: ERR_INVALID_LINKAGE_TYPE
; CHECK-NEXT: kernel4
; CHECK: ERR_INVALID_LINKAGE_TYPE
; CHECK-NEXT: kernel5
; CHECK: ERR_INVALID_LINKAGE_TYPE
; CHECK-NEXT: kernel6
; CHECK: ERR_INVALID_LINKAGE_TYPE
; CHECK-NEXT: kernel7

define weak cc76 void @kernel1() {
  ret void
}

define linker_private cc76 void @kernel2() {
  ret void
}

define linkonce cc76 void @kernel3() {
  ret void
}

define dllexport cc76 void @kernel4() {
  ret void
}

define linkonce_odr cc76 void @kernel5() {
  ret void
}

define weak_odr cc76 void @kernel6() {
  ret void
}

define linker_private_weak cc76 void @kernel7() {
  ret void
}
