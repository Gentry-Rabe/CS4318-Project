#ifndef STRTAB_H
#define STRTAB_H

#define MAXIDS 1000

enum dataType {INT_TYPE, CHAR_TYPE, VOID_TYPE};
enum symbolType {SCALAR, ARRAY, FUNCTION};

typedef struct param {
    int data_type;
    int symbol_type;
    struct param* next;
} param;

typedef struct strEntry {
    char* id;
    char* scope;
    int data_type;
    int symbol_type;
    int size;   /* array length or number of function parameters */
    param* params;
} symEntry;

typedef struct table_node {
    symEntry* strTable[MAXIDS];
    int numChildren;
    struct table_node* parent;
    struct table_node* first_child;
    struct table_node* last_child;
    struct table_node* next;
} table_node;

extern param *working_list_head;
extern param *working_list_end;
extern table_node* current_scope;

int ST_insert(char *id, int data_type, int symbol_type, int* scope);
symEntry* ST_lookup(char *id);
int ST_lookup_index(char *id);
int ST_lookup_current_index(char *id);
symEntry* ST_lookup_function(char *id);
void add_param(int data_type, int symbol_type);
void connect_params(int i, int num_params);
void set_current_function_name(const char *name);
void new_scope(void);
void up_scope(void);
char *get_symbol_id(int idx);
void print_sym_tab(void);

#endif
