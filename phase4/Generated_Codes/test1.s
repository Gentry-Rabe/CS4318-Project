# x86-64 assembly generated for mC
.text

.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    movq $12, %rax
    pushq %rax
    leaq -16(%rbp), %rax
    pushq %rax
    popq %rcx
    popq %rax
    movq %rax, (%rcx)
    pushq %rax
    addq $8, %rsp
    movq -16(%rbp), %rax
    pushq %rax
    movq $10, %rax
    pushq %rax
    popq %rcx
    popq %rax
    addq %rcx, %rax
    pushq %rax
    leaq -16(%rbp), %rax
    pushq %rax
    popq %rcx
    popq %rax
    movq %rax, (%rcx)
    pushq %rax
    addq $8, %rsp
    movq $99, %rax
    pushq %rax
    leaq -8(%rbp), %rax
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

