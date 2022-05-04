#include "str.h"
#include "variable.h"
#include "function.h"

#include <stdio.h>


#ifndef OUTPUT_H
#define OUTPUT_H

int check_vars_conflict(FILE *file, Variable *v1, Variable *v2);
void output_fu_info(FunctionInfoList *node);
void output_global_vars(VariableList *list);
void output_shadowed_global_vars_conflict(FunctionInfoList *fu_infos, VariableList *global_vars);
void output_vars_conflicts(FILE *file, FunctionInfoList *fu_infos);
void output_params_conflicts(FILE *file, FunctionInfoList *fu_infos);
void output_params_and_vars_conflicts(FILE *file, FunctionInfoList *fu_infos);
void output_conflicts(FILE *file, FunctionInfoList *fu_infos, VariableList *global_vars);

#endif
