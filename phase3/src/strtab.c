#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "strtab.h"

const char *dataTypeStr[3] = {"int", "char", "void"};
const char *symbolTypeStr[3] = {"", "[]", "()"};

/* The symbolTable, which will be implemented as a hash table
  */
struct strEntry strTable[MAXIDS];

// Hash function for strings, using djb2 algorithm
// Indexes into the symbol table
int hash(unsigned char *str) {
  unsigned long hash = 5381;
  int c;
  while ((c = *str++)) {
    hash += (hash << 5) + c;
  }
  return hash % MAXIDS; // MAXIDS defined in strtab.h
}

// Helper function to duplicate strings safely
// Ensures string is not NULL, even if the input is NULL.
static char *dup_str(const char *s) {
  const char *src = s ? s : "";
  size_t len = strlen(src) + 1;
  char *out = malloc(len);
  if (!out) {
    perror("malloc");
    exit(1);
  }
  memcpy(out, src, len);
  return out;
}

// Create a unique key for the symbol table based on scope and id
// Allows handling symbols with same name in different scopes.
static char *make_key(const char *scope, const char *id) {
  const char *safe_scope = scope ? scope : "";
  const char *safe_id = id ? id : "";
  size_t scope_len = strlen(safe_scope);
  size_t id_len = strlen(safe_id);
  char *key = malloc(scope_len + id_len + 1);
  if (!key) {
    perror("malloc");
    exit(1);
  }
  memcpy(key, safe_scope, scope_len);
  memcpy(key + scope_len, safe_id, id_len);
  key[scope_len + id_len] = '\0';
  return key;
}

// Insert a symbol into the symbol table, returns index or -1 on failure
int ST_insert(char *id, char *scope, int data_type, int symbol_type){
  char *key = make_key(scope, id);
  int start = hash((unsigned char *)key);
  free(key);

  int existing = ST_lookup(id, scope);
  if (existing != -1) {
    return existing;
  }

  for (int offset = 0; offset < MAXIDS; ++offset) {
    int idx = (start + offset) % MAXIDS;
    if (strTable[idx].id == NULL) {
      strTable[idx].id = dup_str(id);
      strTable[idx].scope = dup_str(scope);
      strTable[idx].data_type = data_type;
      strTable[idx].symbol_type = symbol_type;
      return idx;
    }
  }

  return -1;
}

// Lookup a symbol in the symbol table, returns index or -1 if not found
int ST_lookup(char *id, int *scope) {
  char *key = make_key(scope, id);
  int start = hash((unsigned char *)key);
  free(key);

  for (int offset = 0; offset < MAXIDS; ++offset) {
    int idx = (start + offset) % MAXIDS;
    if (strTable[idx].id == NULL) {
      return -1;
    }
    if (strTable[idx].id != NULL &&
        strcmp(strTable[idx].id, id ? id : "") == 0 &&
        strcmp(strTable[idx].scope, scope ? scope : "") == 0) {
      return idx;
    }
  }

  return -1;
}

//Accessor function
int ST_get_symbol_type(int idx) {
    if (idx < 0 || idx >= MAXIDS || strTable[idx].id == NULL) {
        return -1;
    }
    return strTable[idx].symbol_type;
}

void output_entry(int i){
  printf("%d: %s ", i, dataTypeStr[strTable[i].data_type]);
  printf("%s:%s%s\n",
    strTable[i].scope, strTable[i].id, symbolTypeStr[strTable[i].symbol_type]);
}
