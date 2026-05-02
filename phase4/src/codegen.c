#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "strtab.h"

/* Keep enum values aligned with parser.y / tree.c. */
enum nodeTypes {
    PROGRAM, DECLLIST, DECL, VARDECL, TYPESPEC, FUNDECL,
    FORMALDECLLIST, FORMALDECL, FUNBODY, LOCALDECLLIST,
    STATEMENTLIST, STATEMENT, COMPOUNDSTMT, ASSIGNSTMT,
    CONDSTMT, LOOPSTMT, RETURNSTMT, EXPRESSION, RELOP,
    ADDEXPR, ADDOP, TERM, MULOP, FACTOR, FUNCCALLEXPR,
    ARGLIST, INTEGER, IDENTIFIER, VAR, ARRAYDECL, CHAR,
    FUNCTYPENAME
};

enum opType {ADD, SUB, MUL, DIV, LT, LTE, EQ, GTE, GT, NEQ};

#define MAX_SYMBOLS MAXIDS
#define WORD_SIZE 8

typedef struct {
    int sym_index;
    int offset;          /* rbp-relative stack slot offset */
    int is_param;
    int is_array_param;  /* parameter declared as array -> stored pointer */
    int array_len;       /* for local arrays */
} local_binding;

typedef struct {
    int func_sym_index;
    char *name;
    int return_type;
    int local_count;
    local_binding locals[MAX_SYMBOLS];
    int frame_size;
    int end_label;
} function_ctx;

static FILE *out = NULL;
static int label_counter = 0;
static function_ctx *current_function = NULL;

static void gen_program(tree *node);
static void gen_decl(tree *node);
static void gen_function(tree *node);
static void gen_statement(tree *node);
static void gen_statement_list(tree *node);
static void gen_expr(tree *node);
static void gen_assignment(tree *node, int leave_value_on_stack);
static void gen_lvalue_address(tree *node);
static void emit_load_symbol_value(int sym_index);
static void emit_store_to_var(tree *node);

static int new_label(void) {
    return label_counter++;
}

static symEntry *entry_from_id_node(tree *id_node) {
    if (id_node == NULL || id_node->nodeKind != IDENTIFIER || id_node->val < 0) {
        return NULL;
    }
    return ST_get_entry(id_node->val);
}

static symEntry *entry_from_var_node(tree *node) {
    if (node == NULL || node->numChildren == 0) {
        return NULL;
    }
    return entry_from_id_node(node->children[0]);
}

// Finds local variable or parameter in current function context and returns its index
static int find_local_binding(function_ctx *ctx, int sym_index) {
    if (ctx == NULL) {
        return -1;
    }
    for (int i = 0; i < ctx->local_count; ++i) {
        if (ctx->locals[i].sym_index == sym_index) {
            return i;
        }
    }
    return -1;
}

// Adds a local variable or parameter to function context and assigns it a stack slot
static int add_local_binding(function_ctx *ctx, int sym_index, int words, int is_param, int is_array_param) {
    if (ctx == NULL || sym_index < 0) {
        return -1;
    }
    int existing = find_local_binding(ctx, sym_index);
    if (existing >= 0) {
        return existing;
    }
    if (ctx->local_count >= MAX_SYMBOLS) {
        fprintf(stderr, "codegen: too many locals in function %s\n", ctx->name ? ctx->name : "<anon>");
        exit(1);
    }

    int slot = ctx->local_count++;
    ctx->frame_size += words * WORD_SIZE;
    ctx->locals[slot].sym_index = sym_index;
    ctx->locals[slot].offset = -ctx->frame_size;
    ctx->locals[slot].is_param = is_param;
    ctx->locals[slot].is_array_param = is_array_param;
    ctx->locals[slot].array_len = words;
    return slot;
}

static int align16(int n) {
    return (n + 15) & ~15;
}

// Recursively collect local variable declarations in function body and assign stack slots
static void collect_var_decls(tree *node, function_ctx *ctx) {
    if (node == NULL || ctx == NULL) {
        return;
    }

    if (node->nodeKind == VARDECL && node->numChildren >= 2) {
        tree *id = node->children[1];
        int len = 1;
        if (node->numChildren >= 3 && node->children[2] != NULL && node->children[2]->nodeKind == INTEGER) {
            len = node->children[2]->val;
            if (len < 1) len = 1;
        }
        add_local_binding(ctx, id->val, len, 0, 0);
        return;
    }

    if (node->nodeKind == FUNDECL) {
        return;
    }

    for (int i = 0; i < node->numChildren; ++i) {
        collect_var_decls(node->children[i], ctx);
    }
}

// Collects formal parameters into function context and assigns stack slots
static void collect_formals(tree *formal_list, function_ctx *ctx) {
    if (formal_list == NULL || ctx == NULL) {
        return;
    }
    // First 6 parameters go in registers, rest go on stack
    for (int i = 0; i < formal_list->numChildren; ++i) {
        tree *formal = formal_list->children[i];
        if (formal == NULL || formal->nodeKind != FORMALDECL || formal->numChildren < 2) {
            continue;
        }
        tree *id = formal->children[1];
        int is_array_param = (formal->numChildren >= 4 || (formal->sym_type == ARRAY));
        add_local_binding(ctx, id->val, 1, 1, is_array_param);
    }
}

// Function prologue
static void emit_function_prologue(function_ctx *ctx, tree *formal_list) {
    static const char *arg_regs[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    fprintf(out, ".globl %s\n", ctx->name);
    fprintf(out, "%s:\n", ctx->name);
    fprintf(out, "    pushq %%rbp\n");
    fprintf(out, "    movq %%rsp, %%rbp\n");
    if (ctx->frame_size > 0) {
        fprintf(out, "    subq $%d, %%rsp\n", align16(ctx->frame_size));
    }

    if (formal_list == NULL) {
        return;
    }

    // Move arguments from registers/stack into their assigned stack slots
    int arg_index = 0;
    for (int i = 0; i < formal_list->numChildren; ++i) {
        tree *formal = formal_list->children[i];
        if (formal == NULL || formal->nodeKind != FORMALDECL || formal->numChildren < 2) {
            continue;
        }
        tree *id = formal->children[1];
        int binding = find_local_binding(ctx, id->val);
        if (binding < 0) {
            continue;
        }
        if (arg_index < 6) {
            fprintf(out, "    movq %s, %d(%%rbp)\n", arg_regs[arg_index], ctx->locals[binding].offset);
        } else {
            int stack_arg_offset = 16 + 8 * (arg_index - 6);
            fprintf(out, "    movq %d(%%rbp), %%rax\n", stack_arg_offset);
            fprintf(out, "    movq %%rax, %d(%%rbp)\n", ctx->locals[binding].offset);
        }
        arg_index++;
    }
}

// Function epilogue with label for return statements to jump to
static void emit_function_epilogue(function_ctx *ctx) {
    fprintf(out, ".Lfunc_end_%d:\n", ctx->end_label);
    fprintf(out, "    leave\n");
    fprintf(out, "    ret\n\n");
}

// Generates code for global variable declaration
static void emit_global_decl(tree *node) {
    if (node == NULL || node->nodeKind != VARDECL || node->numChildren < 2) {
        return;
    }
    symEntry *entry = entry_from_id_node(node->children[1]);
    if (entry == NULL || entry->id == NULL) {
        return;
    }

    int count = 1;
    if (node->numChildren >= 3 && node->children[2] != NULL && node->children[2]->nodeKind == INTEGER) {
        count = node->children[2]->val;
        if (count < 1) count = 1;
    }
    fprintf(out, ".comm %s,%d,8\n", entry->id, count * WORD_SIZE);
}

static void push_rax(void) {
    fprintf(out, "    pushq %%rax\n");
}

static void pop_rcx_rax(void) {
    fprintf(out, "    popq %%rcx\n");
    fprintf(out, "    popq %%rax\n");
}

// Generates code to compare two values in rax and rcx
// Set rax to 1 if condition is true and 0 otherwise
static void emit_compare_set(int relop_val) {
    fprintf(out, "    cmpq %%rcx, %%rax\n");
    switch (relop_val) {
        case LT:  fprintf(out, "    setl %%al\n"); break;
        case LTE: fprintf(out, "    setle %%al\n"); break;
        case GT:  fprintf(out, "    setg %%al\n"); break;
        case GTE: fprintf(out, "    setge %%al\n"); break;
        case EQ:  fprintf(out, "    sete %%al\n"); break;
        case NEQ: fprintf(out, "    setne %%al\n"); break;
        default:  fprintf(out, "    sete %%al\n"); break;
    }
    fprintf(out, "    movzbq %%al, %%rax\n");
    push_rax();
}

// Generates code to load value of var/array element and push it on stack
static void emit_load_symbol_value(int sym_index) {
    symEntry *entry = ST_get_entry(sym_index);
    if (entry == NULL) {
        fprintf(out, "    movq $0, %%rax\n");
        push_rax();
        return;
    }

    if (current_function != NULL) {
        int idx = find_local_binding(current_function, sym_index);
        if (idx >= 0) {
            if (current_function->locals[idx].is_array_param) {
                fprintf(out, "    movq %d(%%rbp), %%rax\n", current_function->locals[idx].offset);
            } else {
                fprintf(out, "    movq %d(%%rbp), %%rax\n", current_function->locals[idx].offset);
            }
            push_rax();
            return;
        }
    }

    fprintf(out, "    movq %s(%%rip), %%rax\n", entry->id);
    push_rax();
}

// Generates code to compute address of var/array element and push it on stack
static void gen_lvalue_address(tree *node) {
    if (node == NULL || node->numChildren == 0) {
        fprintf(out, "    movq $0, %%rax\n");
        push_rax();
        return;
    }

    tree *id = node->children[0];
    symEntry *entry = entry_from_id_node(id);
    int binding = (current_function != NULL) ? find_local_binding(current_function, id->val) : -1;

    // For simple variable access just compute address. For array access compute address of first element and add offset computed from index expression
    if (node->nodeKind == VAR) {
        if (binding >= 0) {
            fprintf(out, "    leaq %d(%%rbp), %%rax\n", current_function->locals[binding].offset);
        } else if (entry != NULL && entry->id != NULL) {
            fprintf(out, "    leaq %s(%%rip), %%rax\n", entry->id);
        } else {
            fprintf(out, "    movq $0, %%rax\n");
        }
        push_rax();
        return;
    }

    /* ARRAYDECL */
    gen_expr(node->children[1]);
    fprintf(out, "    popq %%rcx\n");
    fprintf(out, "    imulq $8, %%rcx\n");

    // For array parameters load the pointer and add the offset, for local arrays compute address of first element and add offset
    if (binding >= 0) {
        if (current_function->locals[binding].is_array_param) {
            fprintf(out, "    movq %d(%%rbp), %%rax\n", current_function->locals[binding].offset);
            fprintf(out, "    addq %%rcx, %%rax\n");
        } else {
            fprintf(out, "    leaq %d(%%rbp), %%rax\n", current_function->locals[binding].offset);
            fprintf(out, "    addq %%rcx, %%rax\n");
        }
    } else if (entry != NULL && entry->id != NULL) {
        fprintf(out, "    leaq %s(%%rip), %%rax\n", entry->id);
        fprintf(out, "    addq %%rcx, %%rax\n");
    } else {
        fprintf(out, "    movq $0, %%rax\n");
    }
    push_rax();
}

// Generates code to store value from top of stack into var
static void emit_store_to_var(tree *node) {
    gen_lvalue_address(node);
    fprintf(out, "    popq %%rcx\n");
    fprintf(out, "    popq %%rax\n");
    fprintf(out, "    movq %%rax, (%%rcx)\n");
    push_rax();
}

// Generates code for function call expr
static void gen_call(tree *node) {
    static const char *arg_regs[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};
    if (node == NULL || node->numChildren == 0) {
        fprintf(out, "    movq $0, %%rax\n");
        push_rax();
        return;
    }

    // Evaluate arguments and push them on stack. First 6 arguments will be popped into registers, rest will remain on stack for callee to access
    symEntry *entry = entry_from_id_node(node->children[0]);
    tree *args = (node->numChildren >= 2) ? node->children[1] : NULL;
    int argc = (args != NULL) ? args->numChildren : 0;
    int stack_args = (argc > 6) ? argc - 6 : 0;

    for (int i = argc - 1; i >= 0; --i) {
        gen_expr(args->children[i]);
    }

    for (int i = 0; i < argc && i < 6; ++i) {
        fprintf(out, "    popq %s\n", arg_regs[i]);
    }

    if ((stack_args & 1) != 0) {
        fprintf(out, "    subq $8, %%rsp\n");
    }

    if (entry != NULL && entry->id != NULL) {
        fprintf(out, "    call %s\n", entry->id);
    } else {
        fprintf(out, "    movq $0, %%rax\n");
    }

    if (stack_args > 0) {
        fprintf(out, "    addq $%d, %%rsp\n", stack_args * 8);
    }
    if ((stack_args & 1) != 0) {
        fprintf(out, "    addq $8, %%rsp\n");
    }

    push_rax();
}

// Generates code for expression node
static void gen_expr(tree *node) {
    if (node == NULL) {
        fprintf(out, "    movq $0, %%rax\n");
        push_rax();
        return;
    }

    switch (node->nodeKind) {
        case INTEGER:
        case CHAR:
            fprintf(out, "    movq $%d, %%rax\n", node->val);
            push_rax();
            break;

        case VAR: {
            tree *id = node->children[0];
            emit_load_symbol_value(id->val);
            break;
        }

        case ARRAYDECL:
            gen_lvalue_address(node);
            fprintf(out, "    popq %%rax\n");
            fprintf(out, "    movq (%%rax), %%rax\n");
            push_rax();
            break;

        case FUNCCALLEXPR:
            gen_call(node);
            break;

        case ADDEXPR:
        case TERM:
            if (node->numChildren == 1) {
                gen_expr(node->children[0]);
                break;
            }
            gen_expr(node->children[0]);
            gen_expr(node->children[2]);
            pop_rcx_rax();
            if (node->children[1]->val == ADD) {
                fprintf(out, "    addq %%rcx, %%rax\n");
            } else if (node->children[1]->val == SUB) {
                fprintf(out, "    subq %%rcx, %%rax\n");
            } else if (node->children[1]->val == MUL) {
                fprintf(out, "    imulq %%rcx, %%rax\n");
            } else if (node->children[1]->val == DIV) {
                fprintf(out, "    cqto\n");
                fprintf(out, "    idivq %%rcx\n");
            }
            push_rax();
            break;

        case EXPRESSION:
            if (node->numChildren == 1) {
                gen_expr(node->children[0]);
                break;
            }
            gen_expr(node->children[0]);
            gen_expr(node->children[2]);
            pop_rcx_rax();
            emit_compare_set(node->children[1]->val);
            break;

        case ASSIGNSTMT:
            gen_assignment(node, 1);
            break;

        default:
            if (node->numChildren == 1) {
                gen_expr(node->children[0]);
            } else {
                fprintf(out, "    movq $0, %%rax\n");
                push_rax();
            }
            break;
    }
}

// Generates code for assignment statement
static void gen_assignment(tree *node, int leave_value_on_stack) {
    if (node == NULL || node->numChildren < 2) {
        return;
    }
    gen_expr(node->children[1]);
    emit_store_to_var(node->children[0]);
    if (!leave_value_on_stack) {
        fprintf(out, "    addq $8, %%rsp\n");
    }
}

// Handles if-else statements
static void gen_cond(tree *node) {
    int l_else = new_label();
    int l_end = new_label();

    gen_expr(node->children[0]);
    fprintf(out, "    popq %%rax\n");
    fprintf(out, "    cmpq $0, %%rax\n");
    fprintf(out, "    je .L%d\n", (node->numChildren == 3) ? l_else : l_end);

    gen_statement(node->children[1]);
    if (node->numChildren == 3) {
        fprintf(out, "    jmp .L%d\n", l_end);
        fprintf(out, ".L%d:\n", l_else);
        gen_statement(node->children[2]);
    }
    fprintf(out, ".L%d:\n", l_end);
}

// Handles while and for loops
static void gen_loop(tree *node) {
    int l_test = new_label();
    int l_body = new_label();
    int l_end = new_label();

    if (node->numChildren == 2) {
        fprintf(out, ".L%d:\n", l_test);
        gen_expr(node->children[0]);
        fprintf(out, "    popq %%rax\n");
        fprintf(out, "    cmpq $0, %%rax\n");
        fprintf(out, "    je .L%d\n", l_end);
        gen_statement(node->children[1]);
        fprintf(out, "    jmp .L%d\n", l_test);
        fprintf(out, ".L%d:\n", l_end);
        return;
    }

    /* for-loop encoded as init, cond, step, body */
    if (node->numChildren >= 1 && node->children[0] != NULL) {
        gen_statement(node->children[0]);
    }
    fprintf(out, ".L%d:\n", l_test);
    if (node->numChildren >= 2 && node->children[1] != NULL) {
        gen_expr(node->children[1]);
        fprintf(out, "    popq %%rax\n");
        fprintf(out, "    cmpq $0, %%rax\n");
        fprintf(out, "    je .L%d\n", l_end);
    }
    fprintf(out, ".L%d:\n", l_body);
    if (node->numChildren >= 4 && node->children[3] != NULL) {
        gen_statement(node->children[3]);
    }
    if (node->numChildren >= 3 && node->children[2] != NULL) {
        gen_statement(node->children[2]);
    }
    fprintf(out, "    jmp .L%d\n", l_test);
    fprintf(out, ".L%d:\n", l_end);
}

// Generates code for statement node
static void gen_statement(tree *node) {
    if (node == NULL) {
        return;
    }

    switch (node->nodeKind) {
        case STATEMENT:
        case VARDECL:
            break;
        case STATEMENTLIST:
            gen_statement_list(node);
            break;
        case COMPOUNDSTMT:
        case FUNBODY:
            if (node->numChildren > 0) {
                gen_statement_list(node->children[0]);
            }
            break;
        case ASSIGNSTMT:
            gen_assignment(node, 0);
            break;
        case CONDSTMT:
            gen_cond(node);
            break;
        case LOOPSTMT:
            gen_loop(node);
            break;
        case RETURNSTMT:
            if (node->numChildren > 0) {
                gen_expr(node->children[0]);
                fprintf(out, "    popq %%rax\n");
            } else {
                fprintf(out, "    movq $0, %%rax\n");
            }
            fprintf(out, "    jmp .Lfunc_end_%d\n", current_function->end_label);
            break;
        case EXPRESSION:
        case ADDEXPR:
        case TERM:
        case VAR:
        case ARRAYDECL:
        case FUNCCALLEXPR:
        case INTEGER:
        case CHAR:
            gen_expr(node);
            fprintf(out, "    addq $8, %%rsp\n");
            break;
        default:
            for (int i = 0; i < node->numChildren; ++i) {
                gen_statement(node->children[i]);
            }
            break;
    }
}

// Generates code for list of statements
static void gen_statement_list(tree *node) {
    if (node == NULL) {
        return;
    }
    for (int i = 0; i < node->numChildren; ++i) {
        gen_statement(node->children[i]);
    }
}

// Generates code for function decl
static void gen_function(tree *node) {
    if (node == NULL || node->nodeKind != FUNDECL || node->numChildren < 2) {
        return;
    }

    tree *ft = node->children[0];
    tree *id = (ft != NULL && ft->numChildren >= 2) ? ft->children[1] : NULL;
    symEntry *entry = entry_from_id_node(id);
    if (entry == NULL || entry->id == NULL) {
        return;
    }

    // Initialize function context and collect info about parameters and local variables
    function_ctx ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.func_sym_index = id->val;
    ctx.name = entry->id;
    ctx.return_type = entry->data_type;
    ctx.end_label = new_label();

    tree *formal_list = NULL;
    tree *body = NULL;
    // FUNBODY can have either 2 children (formal_list, body) or just 1 child (body) if no parameters
    if (node->numChildren == 3) {
        formal_list = node->children[1];
        body = node->children[2];
    } else if (node->numChildren == 2) {
        body = node->children[1];
    }

    collect_formals(formal_list, &ctx);
    collect_var_decls(body, &ctx);

    current_function = &ctx;
    emit_function_prologue(&ctx, formal_list);
    gen_statement(body);

    if (ctx.return_type == VOID_TYPE || strcmp(ctx.name, "main") == 0) {
        fprintf(out, "    movq $0, %%rax\n");
    }
    emit_function_epilogue(&ctx);
    current_function = NULL;
}

// Handles global var decls and function decl
static void gen_decl(tree *node) {
    if (node == NULL) {
        return;
    }
    if (node->nodeKind == DECL && node->numChildren > 0) {
        gen_decl(node->children[0]);
        return;
    }
    if (node->nodeKind == VARDECL) {
        emit_global_decl(node);
        return;
    }
    if (node->nodeKind == FUNDECL) {
        gen_function(node);
        return;
    }
    for (int i = 0; i < node->numChildren; ++i) {
        gen_decl(node->children[i]);
    }
}

// Handles global var decls and function decls. Local var decls are handled during function codegen.
static void gen_program(tree *node) {
    if (node == NULL) {
        return;
    }
    fprintf(out, ".text\n");
    // First emit global variable declarations so they appear before functions in assembly output
    if (node->nodeKind == PROGRAM && node->numChildren > 0) {
        tree *decls = node->children[0];
        if (decls != NULL) {
            for (int i = 0; i < decls->numChildren; ++i) {
                tree *decl = decls->children[i];
                if (decl != NULL && decl->numChildren > 0 && decl->children[0] != NULL && decl->children[0]->nodeKind == VARDECL) {
                    emit_global_decl(decl->children[0]);
                }
            }
            fprintf(out, "\n");
            for (int i = 0; i < decls->numChildren; ++i) {
                tree *decl = decls->children[i];
                if (decl != NULL && decl->numChildren > 0 && decl->children[0] != NULL && decl->children[0]->nodeKind == FUNDECL) {
                    gen_function(decl->children[0]);
                }
            }
        }
    }
}

// Entry point for code gen
void codegen(FILE *target, tree *root) {
    if (target == NULL || root == NULL) {
        return;
    }
    out = target;
    fprintf(out, "# x86-64 assembly generated for mC\n");
    gen_program(root);
}
