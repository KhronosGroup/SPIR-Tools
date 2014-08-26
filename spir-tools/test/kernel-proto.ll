target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v16:16:16-v24:32:32-v32:32:32-v48:64:64-v64:64:64-v96:128:128-v128:128:128-v192:256:256-v256:256:256-v512:512:512-v1024:1024:1024"
target triple = "spir64-unknown-unknown"

; RUN: llvm-as -o %t.bc %s
; RUN: not spir_verifier -LIT-test-mode %t.bc 2>%t.out
; RUN: FileCheck %s <%t.out


; The tool shall report invalid return type error
; (kernels have to return void)
; CHECK: ERR_INVALID_KERNEL_RETURN_TYPE
; CHECK-NEXT: i32
; CHECK-NEXT: ret_type
define spir_kernel i32 @ret_type() {
  ret i32 0
}

; The tool shall report invalid kernel arg error
; (kernels cannot accept pointers-to-pointers)
; CHECK: ERR_KERNEL_ARG_PTRPTR
; CHECK-NEXT: i32 addrspace(1)*
; CHECK-NEXT: ptrptr
define spir_kernel void @ptrptr(i32 addrspace(1)* addrspace(1)* %foo) {
  ret void
}

; The tool shall report invalid kernel arg error
; (kernels cannot accept pointers to private addrspace)
; CHECK: ERR_KERNEL_ARG_AS0
; CHECK-NEXT: i32*
; CHECK-NEXT: ptr_as0
define spir_kernel void @ptr_as0(i32* %foo) {
  ret void
}
