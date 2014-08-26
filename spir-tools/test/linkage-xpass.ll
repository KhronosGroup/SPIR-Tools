target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v16:16:16-v24:32:32-v32:32:32-v48:64:64-v64:64:64-v96:128:128-v128:128:128-v192:256:256-v256:256:256-v512:512:512-v1024:1024:1024"
target triple = "spir64-unknown-unknown"

; RUN: llvm-as -o %t.bc %s
; RUN: not spir_verifier -LIT-test-mode %t.bc 2>%t.out
; RUN: FileCheck %s <%t.out

; the SPIR verifier shall report NOT invalid linkage for these kernels
;
; CHECK-NOT: Invalid linkage

define cc76 void @kernel0() {
  ret void
}

define private cc76 void @kernel1() {
  ret void
}

define available_externally cc76 void @kernel2() {
  ret void
}

define internal cc76 void @kernel3() {
  ret void
}

define external cc76 void @kernel4() {
  ret void
}
