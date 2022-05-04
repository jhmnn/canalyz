#include "str.h"
#include "variable.h"

#include <stdlib.h>
#include <string.h>


Variable *var_create() 
{
    Variable *var = malloc(sizeof(Variable));
    if (!var) return NULL;

    var->type = string_create(INIT_VAR_TYPE_SIZE);
    if (!var->type) return NULL;
    var->name = string_create(INIT_VAR_NAME_SIZE);
    if (!var->name) return NULL;

    var->dec_line = 0;
    var->pos = 0;
    var->nesting_level = 0;

    return var;
}

Variable *var_format_name(Variable *var)
{
    if (var->type->data[var->type->size - 1] == '*' && var->name->data[0] == '*') {
        string_delete_index(var->name, 0);
        return var;
    }

    while (var->name->data[0] == '*') {
		string_delete_index(var->name, 0);
		string_add_back(var->type, '*');
	}
	
	return var;
}

void var_free(Variable *var)
{
    if (!var) return;

    free(var->type);
    free(var->name);
    free(var);
}

VariableList *var_list_create()
{
    VariableList *list = malloc(sizeof(VariableList));
    if (!list) return NULL;

    list->var = var_create();
    if (!list->var) return NULL;

    list->next = NULL;

    return list;
}

VariableList *var_list_add(VariableList *list, Variable *var)
{
    if (!list || !var) return NULL;

    VariableList *node = var_list_create();
    if (!node) return NULL;

    while (list->next) {
        list = list->next;
    }

    node->var = var;
    list->next = node;

    return list;
}

Variable *var_list_lookup(VariableList *list, String *name)
{
    while (list) {
        if (strcmp(name->data, list->var->name->data) != 0) {
            list = list->next;
            continue;
        }

        return list->var;
    }

    return NULL;
}

VariableList *var_list_del_first(VariableList *list)
{
    if (!list) return NULL;

    VariableList *t_list = list->next;
    var_free(list->var);
    free(list);
    list = t_list;

    return list;
}

void var_list_free(VariableList *list)
{
    while (list) {
        VariableList *t_node = list->next;
        var_free(list->var);
        free(list);
        list = t_node;
    }
}
