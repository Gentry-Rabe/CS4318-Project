#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "strtab.h"

extern FILE *yyin;
extern int yyparse(void);
void codegen(FILE *target, tree *root);

static void printhelp(void){
    printf("Usage: mcc [--ast] [--sym] [-o OUTFILE] [-h|--help] FILE\n");
    printf("\t--ast:\t\tPrint a textual representation of the constructed abstract syntax tree.\n");
    printf("\t--sym:\t\tPrint a textual representation of the constructed symbol table.\n");
    printf("\t-o OUTFILE:\tWrite generated assembly to OUTFILE (default: stdout).\n");
    printf("\t-h,--help:\tPrint this help information and exit.\n\n");
}

int main(int argc, char *argv[]) {
    int p_ast = 0;
    int p_symtab = 0;
    
    const char *outfile = NULL;
    const char *infile = NULL;

    // Skip first arg (program name), then check all but last for options.
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printhelp();
            return 0;
        } else if (strcmp(argv[i], "--ast") == 0) {
            p_ast = 1;
        } else if (strcmp(argv[i], "--sym") == 0) {
            p_symtab = 1;
        } else if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 >= argc) {
                printhelp();
                return 1;
            }
            outfile = argv[++i];
        } else if (argv[i][0] == '-') {
            printhelp();
            return 1;
        } else {
            infile = argv[i];
        }
    }

    if (infile == NULL) {
        printhelp();
        return 1;
    }

    yyin = fopen(infile, "r");
    if (!yyin) {
        printf("error: unable to read source file %s\n", infile);
        return -1;
    }

    // Parse input file and optionally print AST and symbol table
    if (!yyparse()) {
        if (p_ast) {
            printAst(ast, 1);
        }
        if (p_symtab) {
            print_sym_tab();
        }

        FILE *out = stdout;
        if (outfile != NULL) {
            out = fopen(outfile, "w");
            if (!out) {
                printf("error: unable to open output file %s\n", outfile);
                fclose(yyin);
                return -1;
            }
        }

        codegen(out, ast);
        if (outfile != NULL) {
            fclose(out);
        }
    }

    fclose(yyin);
    return 0;
}
