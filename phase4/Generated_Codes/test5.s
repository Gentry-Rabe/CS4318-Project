# x86-64 assembly generated for mC
.text

.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    movq $0, %rax
    pushq %rax
    leaq -8(%rbp), %rax
    pushq %rax
    popq %rcx
    popq %rax
    movq %rax, (%rcx)
    pushq %rax
    addq $8, %rsp
.L1:
    movq -8(%rbp), %rax
    pushq %rax
    movq $10, %rax
    pushq %rax
    popq %rcx
    popq %rax
    cmpq %rcx, %rax
    setl %al
    movzbq %al, %rax
    pushq %rax
    popq %rax
    cmpq $0, %rax
    je .L3
.L2:
    movq $10, %rax
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
    jmp .L1
.L3:
    movq $0, %rax
.Lfunc_end_0:
    leave
    ret

