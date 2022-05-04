#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>


#define GO_BACK 0
#define GO_FORWARD 1

#define S_SPACE " \n"
#define S_APLHA "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define S_ACC_NAME_C "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_[]"
#define S_ST_NAME_C "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"
#define S_DIGIT "1234567890"

#define INIT_FU_NAME_SIZE 20
#define INIT_FU_TYPE_SIZE 10
#define INIT_VAR_NAME_SIZE 10
#define INIT_VAR_TYPE_SIZE 10

int pro(int x, char e);

typedef struct {
    size_t capacity;
    size_t size;

    char *data;
} String;

typedef struct StringList {
    struct StringList *next;

    String *string;
} StringList;

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

typedef struct {
    VariableList *params;
    VariableList *vars;

    String *type;
    String *name;

    int dec_line;
    int calls;

    short is_prototype;
} FunctionInfo;

int nice();

typedef struct FunctionInfoList {
    struct FunctionInfoList *next;
    
    FunctionInfo *fu_info;
} FunctionInfoList;

String *string_data_clear(String *string)
{
    for (int i = 0; i < string->capacity; i++) {
        string->data[i] = '\0';
    }

    string->size = 0;

    return string;
}

String *string_create(size_t capacity) 
{
    String *string = malloc(sizeof(String));

    if (!string) {
        return NULL;
    }

    string->data = malloc(capacity * sizeof(char));

    if (!string->data) {
        return NULL;
    }

    string->capacity = capacity;
    string->size = 0;

    string_data_clear(string);

    return string;
}

String *string_capacity_increase(String *string)
{
    string->data = realloc(string->data, string->capacity * 2 * sizeof(char));

    if (!string) {
        return NULL;
    }

    string->capacity *= 2;

    return string;
}

String *string_add_back(String *string, char ch)
{
    if (string->size == string->capacity - 1) {
        if (!string_capacity_increase(string)) {
            return NULL;
        }
    }

    string->data[string->size] = ch;
    string->data[string->size + 1] = '\0';
    string->size++;

    return string;
}

String *string_data_copy(String *in, String *from)
{
    for (int i = 0; i < from->size; i++) {
        string_add_back(in, from->data[i]);
    }

    return in;
}

void string_delete_index(String *string, int index)
{
	if (index >= string->size) {
		return;
	}

	string->data[index] = '\0';

	for (int i = index + 1; i < string->size; i++) {
		string->data[i - 1] = string->data[i];
		string->data[i] = '\0';
	}

    string->size--;
}

String *string_space_clear(String *string)
{
    for (int i = 1; i < string->size; i++) {
        if (!isspace(string->data[i - 1])) {
            break;
        }

        for (int j = i; j < string->size; j++) {
            string->data[j - 1] = string->data[j];
            string->data[j] = '\0';
        }
        
        string->size--;
    }

    while (1) {
        if (!isspace(string->data[string->size - 1])) {
            break;
        }

        string->data[string->size - 1] = '\0';
        string->size--;
    }

    return string;
}

void string_free(String *string)
{
    if (!string) return;

    free(string->data);
    free(string);
}

StringList *string_list_create()
{
    StringList *list = malloc(sizeof(StringList));
    if (!list) return NULL;

    list->string = string_create(10);
    if (!list->string) return NULL;

    list->next = NULL;

    return list;
}

StringList *string_list_add(StringList *list, String *string)
{
    if (!list) return NULL;
    if (!string) return NULL;

    StringList *node = string_list_create();
    if (!node) return NULL;

    while (list->next) {
        list = list->next;
    }

    node->string = string;
    list->next = node;

    return list;
}

StringList *string_list_del_first(StringList *list)
{
    if (!list) return NULL;

    StringList *t_list = list->next;
    string_free(list->string);
    free(list);

    return t_list;
}

void string_list_free(StringList *list)
{
    while (list) {
        StringList *t_node = list->next;
        string_free(list->string);
        free(list);
        list = t_node;
    }
}

Variable *var_create() 
{
    Variable *var = malloc(sizeof(Variable));
    if (!var) return NULL;

    var->type = string_create(INIT_VAR_TYPE_SIZE);
    if (!var->type) return NULL;
    var->name = string_create(INIT_FU_NAME_SIZE);
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

int check_stop_symbol(char ch)
{
    size_t symbols_count = 3;
    char stop_symbols[] = {'{', '}', ';'};

    for (int i = 0; i < symbols_count; i++) {
        if (ch == stop_symbols[i]) return 1;
    }

    return 0;
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

int check_fu_var_correct(Variable *fu_param)
{
    if (!fu_param) return 0;

    if ((!isalpha(fu_param->type->data[0])) && (fu_param->type->data[0] != '_')) return 0;
    if ((!isalpha(fu_param->name->data[0])) && (fu_param->name->data[0] != '_')) return 0;

    for (int i = 0; i < fu_param->type->size; i++) {
        if (!is_allowed_symbol(fu_param->type->data[i])) {
            if ((fu_param->type->data[i] != '*') || (i != fu_param->type->size - 1)) return 0;
        }
    }

    for (int i = 0; i < fu_param->name->size; i++) {
        if (!is_allowed_symbol(fu_param->name->data[i])) return 0;
    }

    return 1;
}

int cur_file_line = 1;

int go_search(FILE *file, int direction, char *search, char *stop)
{
    int single_quotes_count = 0;
    int double_quotes_count = 0;

	char ch = ' ', ch_prev;

	while (1) {
		if (direction == GO_BACK) {
            if (!ftell(file)) return 0;
        } else {
            if (feof(file)) return 0;
        }
	
		ch_prev = ch;
		ch = fgetc(file);
		
        if (ch == '\n') {
            cur_file_line = direction == GO_BACK ? cur_file_line - 1 : cur_file_line + 1;
        }
            
        if (ch == '\'' && ch_prev != '\\') {
            single_quotes_count++;
        } else if (ch == '\"' && ch_prev != '\\') {
            double_quotes_count++;
        } else if (schr(search, ch) && (single_quotes_count % 2 == 0 && double_quotes_count % 2 == 0)) {
			fseek(file, -1, SEEK_CUR);
            return 1;
		} else if (schr(stop, ch) && (single_quotes_count % 2 == 0 && double_quotes_count % 2 == 0)) {
			return 0;
		}
		
		if (direction == GO_BACK) fseek(file, -2, SEEK_CUR);
	}
}

int go_while(FILE *file, int direction, char *acc)
{
    char ch = ' ';

	while (1) {
		if (direction == GO_BACK) {
            if (!ftell(file)) return 0;
        } else {
            if (feof(file)) return 0;
        }
	
		ch = fgetc(file);
		
        if (ch == '\n') {
            cur_file_line = direction == GO_BACK ? cur_file_line - 1 : cur_file_line + 1;
        }

        if (!schr(acc, ch)) {
            fseek(file, -1, SEEK_CUR);
            return 1;
        }
		
		if (direction == GO_BACK) fseek(file, -2, SEEK_CUR);
	}
}

int go_while_not(FILE *file, int direction, char *rej)
{
    char ch = ' ';

	while (1) {
		if (direction == GO_BACK) {
            if (!ftell(file)) return 0;
        } else {
            if (feof(file)) return 0;
        }
	
		ch = fgetc(file);
		
        if (ch == '\n') cur_file_line--;

        if (schr(rej, ch)) {
            fseek(file, -1, SEEK_CUR);
            return 1;
        }
		
		if (direction == GO_BACK) fseek(file, -2, SEEK_CUR);
	}
}

int go_skip_block(FILE *file)
{
    int curly_braces_count = 0;
    int single_quotes_count = 0;
    int double_quotes_count = 0;

    char ch = ' ', ch_prev;

    while (1) {
        ch_prev = ch;
        ch = fgetc(file);

        if (feof(file)) return 0;

        if (ch == '\n') cur_file_line++;

        if (ch == '\'' && ch_prev != '\\') {
            single_quotes_count++;
        } else if (ch == '\"' && ch_prev != '\\') {
            double_quotes_count++;
        } else if (ch == '}' && (single_quotes_count % 2 == 0 && double_quotes_count % 2 == 0)) {
            curly_braces_count--;
            if (curly_braces_count == 0) return 1;
        } else if (ch == '{' && (single_quotes_count % 2 == 0 && double_quotes_count % 2 == 0)) {
            curly_braces_count++;
        }
    }
}

void go_to_pos(FILE *file, int pos)
{
    char ch = ' ';

    if (ftell(file) < pos) {
        while (1) {
            ch = fgetc(file);
            if (ch == '\n') cur_file_line++;
            
            if (ftell(file) == pos) break;
        }
    } else {
        while (1) {
            ch = fgetc(file);
            if (ch == '\n') cur_file_line--;

            fseek(file, -2, SEEK_CUR);
            
            if (ftell(file) == pos) break;
        }
    }
}

int get_function_end(FILE *file)
{
    int start_pos, end_pos;

    start_pos = ftell(file);
    go_skip_block(file);
    fseek(file, -1, SEEK_CUR);
    end_pos = ftell(file);
    go_to_pos(file, start_pos);

    return end_pos;
}

int find_function_begin(FILE *file)
{	
    if (!go_search(file, GO_BACK, ")", "{};")) return 0;
    if (!go_search(file, GO_BACK, "(", "{};")) return 0;
    fseek(file, -1, SEEK_CUR);

    if (!go_while(file, GO_BACK, S_SPACE)) return 0;
    if (!go_while_not(file, GO_BACK, S_SPACE)) return 0;
    fseek(file, -1, SEEK_CUR);
    if (!go_while(file, GO_BACK, S_SPACE)) return 0;
    
    go_search(file, GO_BACK, S_SPACE, "{};");

    return 1;
}

int find_function_call_begin(FILE *file)
{
    if (!go_search(file, GO_BACK, "(", "{};)")) return 0;
    fseek(file, -1, SEEK_CUR);
    if (!go_while(file, GO_BACK, S_SPACE)) return 0;
    go_while(file, GO_BACK, S_ACC_NAME_C);
    
    return 1;
}

int find_var(FILE *file)
{
    int start_pos = ftell(file);

    if (!go_search(file, GO_FORWARD, S_SPACE, "{}();")) return 0;
    if (!go_search(file, GO_FORWARD, S_ST_NAME_C, "{}();")) return 0;
    if (!go_search(file, GO_FORWARD, "=;,[", "{}()")) return 0;

    go_to_pos(file, start_pos);

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
    	if (!go_search(file, GO_FORWARD, ",", ";")) break;
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

int check_vars_conflict(FILE *file, Variable *v1, Variable *v2)
{
    if (strcmp(v1->name->data, v2->name->data) != 0) return 0;

    go_to_pos(file, v2->pos);
    fseek(file, -1, SEEK_CUR);
    go_while(file, GO_BACK, S_SPACE);
    if (fgetc(file) == '(' && v1->nesting_level == v2->nesting_level) return 0;

    go_to_pos(file, v1->pos);
    fseek(file, -1, SEEK_CUR);
    go_while(file, GO_BACK, S_SPACE);
    if (fgetc(file) == '(') {
        go_skip_block(file);
        if (v2->pos < ftell(file) - 1) return 1;
        else return 0;
    }

    go_to_pos(file, v1->pos);
    while (1) {
        if (go_search(file, GO_FORWARD, "}", "{")) {
            if (v2->pos > ftell(file)) return 0;
        }
        if (v2->pos < ftell(file)) return 1;

        fseek(file, -1, SEEK_CUR);
        go_skip_block(file);
        if (v2->pos < ftell(file)) return 0;
    }
}

void output_fu_info(FunctionInfoList *node)
{
    int count = 0;

    while (node) {
        printf("\e[1;37m%d: \e[1;34m%s \e[1;33m%s\e[0m (", node->fu_info->dec_line, node->fu_info->type->data, node->fu_info->name->data);

        node->fu_info->params = var_list_del_first(node->fu_info->params);
        VariableList *params = node->fu_info->params;

        if (params) {
            while (params) {
                printf("\e[1;34m%s \e[1;36m%s\e[0m, ", params->var->type->data, params->var->name->data);
                params = params->next;
            }

            printf("\b\b)");
        } else {
            printf(")");
        }

        if (node->fu_info->is_prototype) printf(" [\e[1;35mprototype\e[0m] ");
        else printf(" [\e[1;35mdeclaration\e[0m] ");
        printf("|\e[1;37m%d\e[0m|\n{", node->fu_info->calls);

        node->fu_info->vars = var_list_del_first(node->fu_info->vars);
        VariableList *vars = node->fu_info->vars;

        if (vars) {
            while (vars) {
                printf("\e[1;34m%s \e[1;36m%s\e[0m, ", vars->var->type->data, vars->var->name->data);
                vars = vars->next;
            }

            printf("\b\b}\n\n");
        } else {
            printf("}\n\n");
        }

        node = node->next;
        count++;
    }

    printf("\e[1;92mFunctions: \e[1;37m%d\e[0m\n", count);
    printf("----------");
}

void output_global_vars(VariableList *list)
{
    int count = 0;

    while (list) {
        printf("\e[1;37m%d: \e[1;34m%s \e[1;36m%s\e[0m\n", list->var->dec_line, list->var->type->data, list->var->name->data);
        list = list->next;
        count++;
    }

    if (count) printf("\n\e[1;92mGlobal variables: \e[1;37m%d\e[0m\n", count);
    printf("-----------------");
}

void output_shadowed_global_vars_conflict(FunctionInfoList *fu_infos, VariableList *global_vars)
{
    FunctionInfoList *t_list = fu_infos;
    while (global_vars) {
        while (fu_infos) {
            if (!fu_infos->fu_info->vars) {
                fu_infos = fu_infos->next;
                continue;
            }

            Variable *t = var_list_lookup(fu_infos->fu_info->vars, global_vars->var->name);
            if (t && t->pos > global_vars->var->pos) {
                printf("\e[1;31mWARNING:\e[0m global variable \e[1;36m%s\e[0m is shadowed by local variable in \e[1;33m%s\e[0m, " \
                "line \e[1;37m%d\e[0m\n", global_vars->var->name->data, fu_infos->fu_info->name->data, t->dec_line);
            }

            fu_infos = fu_infos->next;
        }

        fu_infos = t_list;
        while (fu_infos) {
            if (!fu_infos->fu_info->params) {
                fu_infos = fu_infos->next;
                continue;
            }

            Variable *t = var_list_lookup(fu_infos->fu_info->params, global_vars->var->name);
            if (t && t->pos > global_vars->var->pos) {
                printf("\e[1;31mWARNING:\e[0m global variable \e[1;36m%s\e[0m is shadowed by local variable in \e[1;33m%s\e[0m, " \
                "line \e[1;37m%d\e[0m\n", global_vars->var->name->data, fu_infos->fu_info->name->data, t->dec_line);
            }

            fu_infos = fu_infos->next;
        }

        fu_infos = t_list;
        global_vars = global_vars->next;
    }
}

void output_vars_conflicts(FILE *file, FunctionInfoList *fu_infos)
{
    while (fu_infos) {
        VariableList *t1 = fu_infos->fu_info->vars;
        while (t1) {
            VariableList *t2 = t1;
            while (t2->next) {
                t2 = t2->next;
                if (check_vars_conflict(file, t1->var, t2->var)) {
                    printf("\e[1;31mERROR:\e[0m conflicting variable \e[1;36m%s\e[0m in \e[1;33m%s\e[0m, lines \e[1;37m%d\e[0m " \
                    "and \e[1;37m%d\e[0m\n", t1->var->name->data, fu_infos->fu_info->name->data, t1->var->dec_line, t2->var->dec_line);
                }
            }

            t1 = t1->next;
        }

        fu_infos = fu_infos->next;
    }
}

void output_params_conflicts(FILE *file, FunctionInfoList *fu_infos)
{
    while (fu_infos) {
        VariableList *t1 = fu_infos->fu_info->params;
        while (t1) {
            VariableList *t2 = t1;
            while (t2->next) {
                t2 = t2->next;
                if (strcmp(t1->var->name->data, t2->var->name->data) == 0) {
                    printf("\e[1;31mERROR:\e[0m conflicting parameter \e[1;36m%s\e[0m in \e[1;33m%s\e[0m, lines \e[1;37m%d\e[0m " \
                    "and \e[1;37m%d\e[0m\n", t1->var->name->data, fu_infos->fu_info->name->data, t1->var->dec_line, t2->var->dec_line);
                }
            }

            t1 = t1->next;
        }

        fu_infos = fu_infos->next;
    }
}

void output_params_and_vars_conflicts(FILE *file, FunctionInfoList *fu_infos)
{
    while (fu_infos) {
        VariableList *params = fu_infos->fu_info->params;
        while (params) {
            Variable *t = var_list_lookup(fu_infos->fu_info->vars, params->var->name);
            if (t) {
                printf("\e[1;31mERROR:\e[0m conflicting variable \e[1;36m%s\e[0m in \e[1;33m%s\e[0m, lines \e[1;37m%d\e[0m " \
                "and \e[1;37m%d\e[0m\n",t->name->data, fu_infos->fu_info->name->data, params->var->dec_line, t->dec_line);
            }

            params = params->next;
        }

        fu_infos = fu_infos->next;
    }
}

void output_conflicts(FILE *file, FunctionInfoList *fu_infos, VariableList *global_vars)
{
    output_shadowed_global_vars_conflict(fu_infos, global_vars);
    output_vars_conflicts(file, fu_infos);
    output_params_conflicts(file, fu_infos);
    output_params_and_vars_conflicts(file, fu_infos);
}

int read_file(FILE *file, int cur_file_line)
{
    if (!file) return -1;

    FunctionInfoList *fu_info_list = fu_info_list_create(); 
    if (!fu_info_list) return -1;
    VariableList *global_vars = var_list_create();

    if (read_all_fu_prototypes(file, fu_info_list) == 1) return 1;
    if (read_all_fu_declarations(file, fu_info_list) == 1) return 1;

    fu_info_list = fu_info_list_del_first(fu_info_list);

    count_functions_calls(file, fu_info_list);
    read_all_global_vars(file, global_vars);

    global_vars = var_list_del_first(global_vars);

    output_fu_info(fu_info_list);
    putchar('\n');
    output_global_vars(global_vars);
    putchar('\n');
    output_conflicts(file, fu_info_list, global_vars);

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        printf("Usage:\n");
        printf("./canalyz file_name\n");
        return 0;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        printf("No such file.\n");
        return 0;
    }
    
    int result = read_file(file, 0);
    
    if (result) printf("Syntax error in file\n");
    
    fclose(file);

    return 0;
}
