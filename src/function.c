#include "str.h"
#include "function.h"

#include <stdlib.h>
#include <string.h>


FunctionInfo *fu_info_create()
{
    FunctionInfo *fu_info = malloc(sizeof(FunctionInfo));
    if (!fu_info) return NULL;

    fu_info->type = string_create(INIT_FU_TYPE_SIZE);
    if (!fu_info->type) return NULL;
    fu_info->name = string_create(INIT_FU_NAME_SIZE);
    if (!fu_info->name) return NULL;
    fu_info->params = var_list_create();
    if (!fu_info->params) return NULL;
    fu_info->vars = var_list_create();
    if (!fu_info->vars) return NULL;

    fu_info->dec_line = 0;
    fu_info->calls = 0;
    fu_info->is_prototype = 0;

    return fu_info;
}

FunctionInfo *fu_info_format_name(FunctionInfo *fu_info)
{
	while (fu_info->name->data[0] == '*') {
		string_delete_index(fu_info->name, 0);
		string_add_back(fu_info->type, '*');
	}
	
	return fu_info;
}

FunctionInfo *fu_info_clear(FunctionInfo *fu_info)
{
    if (!fu_info) return NULL;

    string_data_clear(fu_info->type);
    string_data_clear(fu_info->name);
    var_list_free(fu_info->params);
    var_list_free(fu_info->vars);

    fu_info->params = var_list_create();
    fu_info->vars = var_list_create();

    fu_info->dec_line = 0;
    fu_info->calls = 0;
    fu_info->is_prototype = 0;

    return fu_info;
}

void fu_info_free(FunctionInfo *fu_info)
{
    if (!fu_info) return;

    var_list_free(fu_info->params);
    var_list_free(fu_info->vars);
    free(fu_info->name);
    free(fu_info->type);
    free(fu_info);
}

FunctionInfoList *fu_info_list_create()
{
    FunctionInfoList *list = malloc(sizeof(FunctionInfoList));
    if (!list) return NULL;

    list->fu_info = fu_info_create();
    if (!list->fu_info) return NULL;

    list->next = NULL;

    return list;
}

FunctionInfoList *fu_info_list_add(FunctionInfoList *list, FunctionInfo *fu_info)
{
    if (!list || !fu_info) return NULL;

    FunctionInfoList *node = fu_info_list_create();
    if (!node) return NULL;

    while (list->next) {
        list = list->next;
    }

    node->fu_info = fu_info;
    list->next = node;

    return list;
}

FunctionInfo *fu_info_list_lookup(FunctionInfoList *list, String *name)
{
    while (list) {
        if (strcmp(name->data, list->fu_info->name->data) != 0) {
            list = list->next;
            continue;
        }

        return list->fu_info;
    }

    return NULL;
}

FunctionInfoList *fu_info_list_del_first(FunctionInfoList *list)
{
    if (!list) return NULL;

    FunctionInfoList *t_list = list->next;
    fu_info_free(list->fu_info);
    free(list);
    list = t_list;

    return list;
}
