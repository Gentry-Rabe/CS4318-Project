# x86-64 assembly generated for mC
.text

.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $128, %rsp
    movq $15, %rax
    pushq %rax
    movq $5, %rax
    pushq %rax
    popq %rcx
    imulq $8, %rcx
    leaq -120(%rbp), %rax
    addq %rcx, %rax
    pushq %rax
    popq %rcx
    popq %rax
    movq %rax, (%rcx)
    pushq %rax
    addq $8, %rsp
    movq $10, %rax
    pushq %rax
    movq $0, %rax
    pushq %rax
    popq %rcx
    imulq $8, %rcx
    leaq -40(%rbp), %rax
    addq %rcx, %rax
    pushq %rax
    popq %rcx
    popq %rax
    movq %rax, (%rcx)
    pushq %rax
    addq $8, %rsp
    movq $5, %rax
    pushq %rax
    popq %rcx
    imulq $8, %rcx
    leaq -120(%rbp), %rax
    addq %rcx, %rax
    pushq %rax
    popq %rax
    movq (%rax), %rax
    pushq %rax
    movq $0, %rax
    pushq %rax
    popq %rcx
    imulq $8, %rcx
    leaq -40(%rbp), %rax
    addq %rcx, %rax
    pushq %rax
    popq %rax
    movq (%rax), %rax
    pushq %rax
    popq %rcx
    popq %rax
    addq %rcx, %rax
    pushq %rax
    leaq -128(%rbp), %rax
    pushq %rax
    popq %rcx
    popq %rax
    movq %rax, (%rcx)
    pushq %rax
    addq $8, %rsp
    movq $0, %rax
.Lfunc_end_0:
    leave
    ret

