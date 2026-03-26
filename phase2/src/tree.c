#include <tree.h>
#include <strtab.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct treenode tree;

/* Keep this enum in sync with parser.y */
enum nodeTypes {
    PROGRAM, DECLLIST, DECL, VARDECL, TYPESPEC, FUNDECL,
    FORMALDECLLIST, FORMALDECL, FUNBODY, LOCALDECLLIST,
    STATEMENTLIST, STATEMENT, COMPOUNDSTMT, ASSIGNSTMT,
    CONDSTMT, LOOPSTMT, RETURNSTMT, EXPRESSION, RELOP,
    ADDEXPR, ADDOP, TERM, MULOP, FACTOR, FUNCCALLEXPR,
    ARGLIST, INTEGER, IDENTIFIER, VAR, ARRAYDECL, CHAR,
    FUNCTYPENAME
};

tree *ast;

static const char *kindName[] = {
    "program", "declList", "decl", "varDecl", "typeSpecifier", "funDecl",
    "formalDeclList", "formalDecl", "funBody", "localDeclList",
    "statementList", "statement", "compoundStmt", "assignStmt",
    "condStmt", "loopStmt", "returnStmt", "expression", "relop",
    "addExpr", "addop", "term", "mulop", "factor", "funCallExpr",
    "argList", "integer", "identifier", "var", "arrayDecl", "char",
    "funcTypeName"
};

static const char *typeName(int t) {
    switch (t) {
        case INT_TYPE:  return "int";
        case CHAR_TYPE: return "char";
        case VOID_TYPE: return "void";
        default:        return "?";
    }
}

static const char *opName(int kind, int v) {
    if (kind == ADDOP) {
        return (v == 0) ? "+" : "-";
    }
    if (kind == MULOP) {
        return (v == 0) ? "*" : "/";
    }
    if (kind == RELOP) {
        switch (v) {
            case 0: return "<=";
            case 1: return "<";
            case 2: return ">";
            case 3: return ">=";
            case 4: return "==";
            case 5: return "!=";
        }
    }
    return "?";
}

tree *maketree(int kind) {
    tree *n = calloc(1, sizeof(tree));
    if (!n) {
        perror("calloc");
        exit(1);
    }
    n->nodeKind = kind;
    n->numChildren = 0;
    n->val = 0;
    n->parent = NULL;
    n->text = NULL;
    return n;
}

tree *maketreeWithVal(int kind, int val) {
    tree *n = maketree(kind);
    n->val = val;
    return n;
}

void addChild(tree *parent, tree *child) {
    if (!parent || !child) return;
    if (parent->numChildren >= MAXCHILDREN) return;
    parent->children[parent->numChildren++] = child;
    child->parent = parent;
}

static void printNode(tree *root) {
    switch (root->nodeKind) {
        case TYPESPEC:
            printf("%s,%s", kindName[root->nodeKind], typeName(root->val));
            break;

        case IDENTIFIER:
            printf("%s,%s", kindName[root->nodeKind], root->text ? root->text : "");
            break;

        case INTEGER:
            printf("%s,%d", kindName[root->nodeKind], root->val);
            break;

        case CHAR:
            printf("%s,%d", kindName[root->nodeKind], root->val);
            break;

        case ADDOP:
        case MULOP:
        case RELOP:
            printf("%s,%s", kindName[root->nodeKind], opName(root->nodeKind, root->val));
            break;

        case FUNCTYPENAME:
            printf("%s", kindName[root->nodeKind]);
            break;

        default:
            if (root->nodeKind >= 0 &&
                root->nodeKind < (int)(sizeof(kindName) / sizeof(kindName[0]))) {
                printf("%s", kindName[root->nodeKind]);
            } else {
                printf("node%d", root->nodeKind);
            }
            break;
    }
}

void printAst(tree *root, int nestLevel) {
    if (!root) return;

    printf("%*s", nestLevel * 2, "");
    printNode(root);
    putchar('\n');

    for (int i = 0; i < root->numChildren; ++i) {
        printAst(root->children[i], nestLevel + 1);
    }
}