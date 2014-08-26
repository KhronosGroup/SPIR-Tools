
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v16:16:16-v24:32:32-v32:32:32-v48:64:64-v64:64:64-v96:128:128-v128:128:128-v192:256:256-v256:256:256-v512:512:512-v1024:1024:1024"
target triple = "spir64-unknown-unknown"

; RUN: llvm-as -o %t.bc %s
; RUN: not spir_verifier -LIT-test-mode %t.bc 2>%t.out
; RUN: FileCheck %s <%t.out

; testing kernel
define spir_kernel void @foo() {
  %1 = alloca float
  store float 1.0, float* %1
  %2 = load float* %1
  store float %2, float addrspace(3) * @foo.var
  store float %2, float addrspace(3) * @bar.var
  ret void
}

; This is OK as there is @foo is a defined kernel
; CHECK-NOT: @foo.var
@foo.var = internal addrspace(3) global float zeroinitializer

; This is not OK: no corresponding function
; CHECK: ERR_INVALID_GLOBAL_AS3_VAR
; CHECK-NEXT: @bar.var
@bar.var = internal addrspace(3) global float zeroinitializer
