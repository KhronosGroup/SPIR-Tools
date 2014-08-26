target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v16:16:16-v24:32:32-v32:32:32-v48:64:64-v64:64:64-v96:128:128-v128:128:128-v192:256:256-v256:256:256-v512:512:512-v1024:1024:1024"
target triple = "spir64-unknown-unknown"

; RUN: llvm-as -o %t.bc %s
; RUN: not spir_verifier -LIT-test-mode %t.bc 2>%t.out
; RUN: FileCheck %s <%t.out

; Global address space should not appear in module-scope vars
; CHECK: ERR_INVALID_GLOBAL_VAR_ADDRESS_SPACE
; CHECK-NEXT: @as0
@as0 = internal global float zeroinitializer

; Local address space should not appear in module-scope vars
; CHECK: ERR_INVALID_GLOBAL_VAR_ADDRESS_SPACE
; CHECK-NEXT: @as1
@as1 = internal addrspace(1) global float zeroinitializer

; Constant address space is valid for module-scope vars
; CHECK-NOT: @as2
@as2 = internal addrspace(2) global float zeroinitializer

; Completely invalid address space shall be reported
; CHECK: ERR_INVALID_GLOBAL_VAR_ADDRESS_SPACE
; CHECK-NEXT: @asX
@asX = internal addrspace(17) global float zeroinitializer

