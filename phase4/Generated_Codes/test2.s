# x86-64 assembly generated for mC
.text

.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    movq $2, %rax
    pushq %rax
    movq $3, %rax
    pushq %rax
    movq $5, %rax
    pushq %rax
    popq %rcx
    popq %rax
    imulq %rcx, %rax
    pushq %rax
    movq $7, %rax
    pushq %rax
    popq %rcx
    popq %rax
    cqto
    idivq %rcx
    pushq %rax
    popq %rcx
    popq %rax
    addq %rcx, %rax
    pushq %rax
    leaq -8(%rbp), %rax
    pushq %rax
    popq %rcx
    popq %rax
    movq %rax, (%rcx)
    pushq %rax
    addq $8, %rsp
    movq -8(%rbp), %rax
    pushq %rax
    movq $1, %rax
    pushq %rax
    popq %rcx
    popq %rax
    addq %rcx, %rax
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

