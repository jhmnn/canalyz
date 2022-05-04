#include "str.h"
#include "variable.h"
#include "function.h"

#include <stdio.h>


#ifndef FILE_READ_H
#define FILE_READ_H

#define S_SPACE " \n"
#define S_APLHA "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define S_ACC_NAME_C "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890_[]"
#define S_ST_NAME_C "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"
#define S_DIGIT "1234567890"

int schr(char *s, char c);
int is_allowed_symbol(char ch);
int check_type_correct(String *type);
int check_name_correct(String *name);
String *read_type(FILE *file, String *type);
String *read_function_name(FILE *file, String *name);
FunctionInfo *read_function_head(FILE *file, FunctionInfo *fu_info);
VariableList *read_function_params(FILE *file, VariableList *fu_params);
String *read_var_name(FILE *file, String *name);
Variable *read_var(FILE *file, Variable *var);
VariableList *read_vars(FILE *file, VariableList *vars, int current_nesting_level);
VariableList *read_function_vars(FILE *file, VariableList *vars);
int read_all_fu_prototypes(FILE *file, FunctionInfoList *fu_info_list);
int read_all_fu_declarations(FILE *file, FunctionInfoList *fu_info_list);
void read_calls(FILE *file, FunctionInfoList *fu_info_list, int end_pos);
void count_functions_calls(FILE *file, FunctionInfoList *fu_info_list);
void read_all_global_vars(FILE *file, VariableList *global_vars);
int read_file(FILE *file, FunctionInfoList *fu_infos, VariableList *global_vars);

#endif
