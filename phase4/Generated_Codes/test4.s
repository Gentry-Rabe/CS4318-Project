# x86-64 assembly generated for mC
.text

.globl main
main:
    pushq %rbp
    movq %rsp, %rbp
    subq $16, %rsp
    movq $10, %rax
    pushq %rax
    movq $15, %rax
    pushq %rax
    popq %rcx
    popq %rax
    cmpq %rcx, %rax
    setl %al
    movzbq %al, %rax
    pushq %rax
    popq %rax
    cmpq $0, %rax
    je .L1
    movq $0, %rax
    pushq %rax
    leaq -8(%rbp), %rax
    pushq %rax
    popq %rcx
    popq %rax
    movq %rax, (%rcx)
    pushq %rax
    addq $8, %rsp
    jmp .L2
.L1:
    movq $5, %rax
    pushq %rax
    leaq -8(%rbp), %rax
    pushq %rax
    popq %rcx
    popq %rax
    movq %rax, (%rcx)
    pushq %rax
    addq $8, %rsp
.L2:
    movq $0, %rax
.Lfunc_end_0:
    leave
    ret

