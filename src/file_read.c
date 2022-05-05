#include "str.h"
#include "function.h"
#include "variable.h"
#include "file_move.h"
#include "file_read.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>


int schr(char *s, char c)
{
    while (*s) {
        if (c == *s++) return 1;
    }

    return 0;
}

int is_allowed_symbol(char ch)
{
    return schr(S_ACC_NAME_C, ch);
}

int check_type_correct(String *type)
{
    if (!type) return 0;
    if (type->size == 0) return 0;

    if ((!isalpha(type->data[0])) && (type->data[0] != '_')) return 0;

    for (int i = 0; i < type->size; i++) {
        if (!is_allowed_symbol(type->data[i])) {
            if ((type->data[i] != '*') || (i != type->size - 1)) return 0;
        }
    }

    return 1;
}

int check_name_correct(String *name)
{
    if (!name) return 0;
    if (name->size == 0) return 0;

    if ((!isalpha(name->data[0])) && (name->data[0] != '_')) return 0;

    for (int i = 0; i < name->size; i++) {
        if (!is_allowed_symbol(name->data[i])) return 0;
    }

    return 1;
}

String *read_type(FILE *file, String *type)
{
	if (!file || !type) return NULL;

    char ch = ' ';

    while (1) {
        ch = fgetc(file);

        if (ch == '\n') cur_file_line++;
        if (isspace(ch)) break;

        string_add_back(type, ch);
    }

    return type;
}

String *read_function_name(FILE *file, String *name)
{
	if (!file || !name) return NULL;

    char ch = ' ';

    while (1) {
        ch = fgetc(file);

        if (ch == '\n') cur_file_line++;
        else if (ch == '(') break;

        string_add_back(name, ch);
    }

    return name;
}

FunctionInfo *read_function_head(FILE *file, FunctionInfo *fu_info)
{
    go_while(file, GO_FORWARD, S_SPACE);
    fu_info->dec_line = cur_file_line;
    if (!read_type(file, fu_info->type)) return NULL;

    go_while(file, GO_FORWARD, S_SPACE);
    if (!read_function_name(file, fu_info->name)) return NULL;

    fu_info_format_name(fu_info);

    string_space_clear(fu_info->type);
    if (!check_type_correct(fu_info->type)) return NULL;
    string_space_clear(fu_info->name);
    if (!check_name_correct(fu_info->name)) return NULL;
    
    return fu_info;
}

VariableList *read_function_params(FILE *file, VariableList *fu_params)
{
    if (!file || !fu_params) return NULL;

    char ch = ' ';

    while (1) {
        Variable *var = var_create();
        if (!var) return NULL;

        go_while(file, GO_FORWARD, S_SPACE);

        if (fgetc(file) == ')') return fu_params;
        fseek(file, -1, SEEK_CUR);

        while (1) {
            ch = fgetc(file);

            if (ch == '\n') cur_file_line++;

            if (isspace(ch)) {
                string_space_clear(var->type);
                if (!check_type_correct(var->type)) return NULL;
                break;
            } else if (ch == ')') {
                var_free(var);
                return NULL;
            }

            string_add_back(var->type, ch);
        }

        go_while(file, GO_FORWARD, S_SPACE);

        while (1) {
            ch = fgetc(file);

            if (ch == '\n') cur_file_line++;

            if (ch == ',' || ch == ')') {
                string_space_clear(var->name);
                var_format_name(var);
                if (!check_name_correct(var->name)) return NULL;
                var->dec_line = cur_file_line;
                var->pos = ftell(file);
                var_list_add(fu_params, var);
                break;
            }

            string_add_back(var->name, ch);
        }

        if (ch == ')') break;
    }

    return fu_params;
}

String *read_var_name(FILE *file, String *name)
{
    char ch = ' ';

    while (1) {
        ch = fgetc(file);

        if (ch == '\n') cur_file_line++;
        else if (ch == ';' || ch == '=' || ch == ',' || ch == '[') break;

        string_add_back(name, ch);
    }

    return name;
}

Variable *read_var(FILE *file, Variable *var)
{
    go_search(file, GO_FORWARD, S_ST_NAME_C, ";");
    var->dec_line = cur_file_line;
    var->pos = ftell(file);
    if (!read_type(file, var->type)) return NULL;

    go_while(file, GO_FORWARD, S_SPACE);
    if (!read_var_name(file, var->name)) return NULL;
    var_format_name(var);

    string_space_clear(var->type);
    if (!check_type_correct(var->type)) return NULL;
    string_space_clear(var->name);
    if (!check_name_correct(var->name)) return NULL;

    return var;
}

VariableList *read_vars(FILE *file, VariableList *vars, int current_nesting_level)
{
    Variable *var = var_create();
    if (!var) return NULL;
    
    if (read_var(file, var)) {
        if (strcmp(var->type->data, "return") != 0) var_list_add(vars, var);
        else {
        	free(var);
        	return NULL;
        }

        var->nesting_level = current_nesting_level;
    } else {
    	free(var);
    	return NULL;
    }
    
	fseek(file, -1, SEEK_CUR);
    while (1) {
    	if (!go_search(file, GO_FORWARD, ",", "[]=;")) break;
        printf("(%s)", var->name->data);
    	fseek(file, 1, SEEK_CUR);
    	
    	Variable *t = var_create();
    	if (!t) return NULL;
    	
    	t->dec_line = cur_file_line;
    	t->pos = ftell(file);
        t->nesting_level = current_nesting_level;
    	read_var_name(file, t->name);
    	string_data_copy(t->type, var->type);
    	string_space_clear(t->name);
    	var_format_name(t);
    	if (!check_name_correct(t->name)) {
    		free(t);
    		break;
    	}
    	
    	var_list_add(vars, t);
    	fseek(file, -1, SEEK_CUR);
    }

    return vars;
}

VariableList *read_function_vars(FILE *file, VariableList *vars)
{
    go_search(file, GO_FORWARD, "{", "{");

    int end_pos = get_function_end(file);
    int curly_braces_count = 0;

    while (1) {
        if (ftell(file) >= end_pos) break;

        if (!go_search(file, GO_FORWARD, S_ST_NAME_C, "{}")) {
            fseek(file, -1, SEEK_CUR);
            if (fgetc(file) == '{') curly_braces_count++;
            else curly_braces_count--;
            continue;
        }

        if (!find_var(file)) continue;

        read_vars(file, vars, curly_braces_count);

        go_search(file, GO_FORWARD, S_ST_NAME_C, "}");
    }

    return vars;
}

int read_all_fu_prototypes(FILE *file, FunctionInfoList *fu_info_list)
{
    fseek(file, 0, SEEK_SET);
    cur_file_line = 1;

    while (1) {
    	if (feof(file)) break;

		if (!go_search(file, GO_FORWARD, ";", "{")) {
			if (feof(file)) break;
            fseek(file, -1, SEEK_CUR);
			if (!go_skip_block(file)) return 0;
			continue;
		}

        fseek(file, -1, SEEK_CUR);
        
        if (find_function_begin(file)) {
        	FunctionInfo *fu_info = fu_info_create();
            if (!fu_info) return -1;

            if (read_function_head(file, fu_info)) {
                if (read_function_params(file, fu_info->params))
                {
                    fu_info->is_prototype = 1;
                    fu_info_list_add(fu_info_list, fu_info);
                } else {
                    fu_info_free(fu_info);
                }
            } else {
                fu_info_free(fu_info);
            }
        }

        if (go_search(file, GO_FORWARD, ";", "{")) fseek(file, 1, SEEK_CUR);
    }

    return 0;
}

int read_all_fu_declarations(FILE *file, FunctionInfoList *fu_info_list)
{
    fseek(file, 0, SEEK_SET);
    cur_file_line = 1;

    while (1) {
    	if (feof(file)) return 1;
		if (!go_search(file, GO_FORWARD, "{", "{")) return 0;

        fseek(file, -1, SEEK_CUR);

        if (find_function_begin(file)) {
        	FunctionInfo *fu_info = fu_info_create();
            if (!fu_info) return -1;
            if (read_function_head(file, fu_info)) {
                if (read_function_params(file, fu_info->params))
                {
                    read_function_vars(file, fu_info->vars);
                    fu_info->is_prototype = 0;
                    fu_info_list_add(fu_info_list, fu_info);
                } else {
                    fu_info_free(fu_info);
                }
            } else {
                fu_info_free(fu_info);
            }
        } else {
            go_skip_block(file);
        }
    }
}

void read_calls(FILE *file, FunctionInfoList *fu_info_list, int end_pos)
{
    FunctionInfo *fu_info = fu_info_create(), *tmp = NULL;
    if (!fu_info) return;

    while (1) {
        if (feof(file)) break;
        if (ftell(file) >= end_pos) break;

        if (!go_search(file, GO_FORWARD, "(", "}")) continue;
        
        if (!find_function_call_begin(file)) {
            go_search(file, GO_FORWARD, "(", "(");
            fseek(file, 1, SEEK_CUR);
            continue;
        }

        fseek(file, 1, SEEK_CUR);
        read_function_name(file, fu_info->name);
        string_space_clear(fu_info->name);
        
        tmp = fu_info_list_lookup(fu_info_list, fu_info->name);
        
        if (tmp) {
            tmp->calls++;
        }

        string_data_clear(fu_info->name);
    }

    free(fu_info);
}

void count_functions_calls(FILE *file, FunctionInfoList *fu_info_list)
{
    fseek(file, 0, SEEK_SET);
    cur_file_line = 1;

    int end_pos;

    while (1) {
        if (feof(file)) return;

        if (!go_search(file, GO_FORWARD, "{", "{")) return;
        fseek(file, -1, SEEK_CUR);
        
        if (!find_function_begin(file)) {
            go_skip_block(file);
            continue;
        }

        go_search(file, GO_FORWARD, "{", "{");
        end_pos = get_function_end(file);
		
        read_calls(file, fu_info_list, end_pos);
    }
}

void read_all_global_vars(FILE *file, VariableList *global_vars)
{
    fseek(file, 0, SEEK_SET);
    cur_file_line = 1;

    while (1) {
        if (feof(file)) return;

        if (!go_search(file, GO_FORWARD, S_ST_NAME_C, "{()")) {
            fseek(file, -1, SEEK_CUR);
            go_skip_block(file);
            continue;
        }

        if (!find_var(file)) {
            fseek(file, -1, SEEK_CUR);
            if (!go_search(file, GO_FORWARD, "({", ";")) go_skip_block(file);
            continue;
        }

        read_vars(file, global_vars, 0);
    }
}

int read_file(FILE *file, FunctionInfoList *fu_infos, VariableList *global_vars)
{
    if (!file) return -1;

    if (read_all_fu_prototypes(file, fu_infos) == 1) return 1;
    if (read_all_fu_declarations(file, fu_infos) == 1) return 1;

    count_functions_calls(file, fu_infos);
    read_all_global_vars(file, global_vars);

    return 0;
}
