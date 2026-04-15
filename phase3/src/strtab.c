#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "strtab.h"

const char *dataTypeStr[3] = {"int", "char", "void"};
const char *symbolTypeStr[3] = {"", "[]", "()"};

param *working_list_head = NULL;
param *working_list_end = NULL;
table_node *current_scope = NULL;

static symEntry symtab[MAXIDS];
static int symtab_count = 0;
static char current_function_name[256] = "";

static char *dup_str(const char *s) {
    const char *src = s ? s : "";
    size_t len = strlen(src) + 1;
    char *out = (char *)malloc(len);
    if (!out) {
        perror("malloc");
        exit(1);
    }
    memcpy(out, src, len);
    return out;
}

static int hash(unsigned char *str) {
    unsigned long h = 5381;
    int c;
    while ((c = *str++)) {
        h = ((h << 5) + h) + (unsigned char)c;
    }
    return (int)(h % MAXIDS);
}

static void ensure_root(void) {
    if (current_scope != NULL) {
        return;
    }
    current_scope = (table_node *)calloc(1, sizeof(table_node));
    if (!current_scope) {
        perror("calloc");
        exit(1);
    }
}

static int entry_index(const symEntry *entry) {
    if (!entry) {
        return -1;
    }
    return (int)(entry - symtab);
}

static const char *scope_name_for_insertion(void) {
    if (current_scope && current_scope->parent != NULL) {
        return current_function_name;
    }
    return "";
}

void set_current_function_name(const char *name) {
    snprintf(current_function_name, sizeof(current_function_name), "%s", name ? name : "");
}

static symEntry *lookup_in_scope(table_node *scope, char *id) {
    if (!scope || !id) {
        return NULL;
    }
    int start = hash((unsigned char *)id);
    for (int offset = 0; offset < MAXIDS; ++offset) {
        int idx = (start + offset) % MAXIDS;
        symEntry *entry = scope->strTable[idx];
        if (entry == NULL) {
            return NULL;
        }
        if (entry->id && strcmp(entry->id, id) == 0) {
            return entry;
        }
    }
    return NULL;
}

static int lookup_index_in_scope(table_node *scope, char *id) {
    symEntry *entry = lookup_in_scope(scope, id);
    return entry ? entry_index(entry) : -1;
}

static symEntry *insert_into_current_scope(char *id, int data_type, int symbol_type) {
    ensure_root();
    if (!id) {
        id = "";
    }

    if (lookup_in_scope(current_scope, id) != NULL) {
        return NULL;
    }
    if (symtab_count >= MAXIDS) {
        return NULL;
    }

    int start = hash((unsigned char *)id);
    for (int offset = 0; offset < MAXIDS; ++offset) {
        int idx = (start + offset) % MAXIDS;
        if (current_scope->strTable[idx] == NULL) {
            symEntry *entry = &symtab[symtab_count++];
            entry->id = dup_str(id);
            entry->scope = dup_str(scope_name_for_insertion());
            entry->data_type = data_type;
            entry->symbol_type = symbol_type;
            entry->size = 0;
            entry->params = NULL;
            current_scope->strTable[idx] = entry;

            if (symbol_type == FUNCTION && current_scope->parent == NULL) {
                snprintf(current_function_name, sizeof(current_function_name), "%s", id);
            }
            return entry;
        }
    }
    return NULL;
}

int ST_insert(char *id, int data_type, int symbol_type, int* scope) {
    (void)scope;
    symEntry *entry = insert_into_current_scope(id, data_type, symbol_type);
    return entry ? entry_index(entry) : -1;
}

symEntry* ST_lookup(char *id) {
    ensure_root();
    for (table_node *scope = current_scope; scope != NULL; scope = scope->parent) {
        symEntry *entry = lookup_in_scope(scope, id);
        if (entry != NULL) {
            return entry;
        }
    }
    return NULL;
}

int ST_lookup_index(char *id) {
    symEntry *entry = ST_lookup(id);
    return entry ? entry_index(entry) : -1;
}

int ST_lookup_current_index(char *id) {
    ensure_root();
    return lookup_index_in_scope(current_scope, id);
}

void add_param(int data_type, int symbol_type) {
    param *node = (param *)malloc(sizeof(param));
    if (!node) {
        perror("malloc");
        exit(1);
    }
    node->data_type = data_type;
    node->symbol_type = symbol_type;
    node->next = NULL;

    if (working_list_head == NULL) {
        working_list_head = node;
        working_list_end = node;
    } else {
        working_list_end->next = node;
        working_list_end = node;
    }
}

void connect_params(int i, int num_params) {
    if (i < 0 || i >= symtab_count) {
        working_list_head = NULL;
        working_list_end = NULL;
        return;
    }
    if (symtab[i].params == NULL && symtab[i].size == 0) {
        symtab[i].params = working_list_head;
        symtab[i].size = num_params;
    }
    working_list_head = NULL;
    working_list_end = NULL;
}

void new_scope(void) {
    ensure_root();
    table_node *child = (table_node *)calloc(1, sizeof(table_node));
    if (!child) {
        perror("calloc");
        exit(1);
    }
    child->parent = current_scope;

    if (current_scope->first_child == NULL) {
        current_scope->first_child = child;
        current_scope->last_child = child;
    } else {
        current_scope->last_child->next = child;
        current_scope->last_child = child;
    }
    current_scope->numChildren++;
    current_scope = child;
}

void up_scope(void) {
    ensure_root();
    if (current_scope->parent != NULL) {
        current_scope = current_scope->parent;
    }
    if (current_scope->parent == NULL) {
        current_function_name[0] = '\0';
    }
}

char *get_symbol_id(int idx) {
    if (idx < 0 || idx >= symtab_count) {
        return NULL;
    }
    return symtab[idx].id;
}

void output_entry(int i) {
    if (i < 0 || i >= symtab_count || symtab[i].id == NULL) {
        return;
    }
    printf("%d: %s ", i, dataTypeStr[symtab[i].data_type]);
    printf("%s:%s%s\n", symtab[i].scope, symtab[i].id, symbolTypeStr[symtab[i].symbol_type]);
}

static void print_scope(table_node *scope) {
    if (scope == NULL) {
        return;
    }
    for (int i = 0; i < MAXIDS; ++i) {
        symEntry *entry = scope->strTable[i];
        if (entry != NULL) {
            printf("%d: %s ", i, dataTypeStr[entry->data_type]);
            printf("%s:%s%s\n", entry->scope, entry->id, symbolTypeStr[entry->symbol_type]);
        }
    }
    for (table_node *child = scope->first_child; child != NULL; child = child->next) {
        print_scope(child);
    }
}

void print_sym_tab(void) {
    ensure_root();
    table_node *root = current_scope;
    while (root->parent != NULL) {
        root = root->parent;
    }
    print_scope(root);
}
