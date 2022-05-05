#include "str.h"
#include "variable.h"
#include "function.h"
#include "file_move.h"
#include "file_read.h"
#include "output.h"

#include <stdio.h>
#include <string.h>


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

    printf("\n----------\n\e[1;92mFunctions\e[0m\n----------\n");

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

            printf("\b\b}\n");
        } else {
            printf("}\n");
        }

        node = node->next;
        count++;
    }

    printf("----------\n\e[1;92mFunctions: \e[1;37m%d\e[0m\n----------", count);
}

void output_global_vars(VariableList *list)
{
    int count = 0;

    printf("\n-----------------\n\e[1;92mGlobal variables\e[0m\n-----------------\n");

    while (list) {
        printf("\e[1;37m%d: \e[1;34m%s \e[1;36m%s\e[0m\n", list->var->dec_line, list->var->type->data, list->var->name->data);
        list = list->next;
        count++;
    }

    printf("-----------------\n\e[1;92mGlobal variables: \e[1;37m%d\e[0m\n-----------------\n", count);
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
