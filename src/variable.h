#include "str.h"

#include <string.h>


#ifndef VARIABLE_H
#define VARIABLE_H

#define INIT_VAR_NAME_SIZE 10
#define INIT_VAR_TYPE_SIZE 10

typedef struct {
    String *type;
    String *name;

    int dec_line;
    int pos;

    int nesting_level;
} Variable;

typedef struct VariableList {
    struct VariableList *next;

    Variable *var;
} VariableList;


Variable *var_create();
Variable *var_format_name(Variable *var);
void var_free(Variable *var);
VariableList *var_list_create();
VariableList *var_list_add(VariableList *list, Variable *var);
Variable *var_list_lookup(VariableList *list, String *name);
VariableList *var_list_del_first(VariableList *list);
void var_list_free(VariableList *list);

#endif
