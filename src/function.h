#include "str.h"
#include "variable.h"

#include <string.h>


#ifndef FUNCTION_H
#define FUNCTION_H

#define INIT_FU_NAME_SIZE 20
#define INIT_FU_TYPE_SIZE 10

typedef struct {
    VariableList *params;
    VariableList *vars;

    String *type;
    String *name;

    int dec_line;
    int calls;

    short is_prototype;
} FunctionInfo;

typedef struct FunctionInfoList {
    struct FunctionInfoList *next;
    
    FunctionInfo *fu_info;
} FunctionInfoList;


FunctionInfo *fu_info_create();
FunctionInfo *fu_info_format_name(FunctionInfo *fu_info);
FunctionInfo *fu_info_clear(FunctionInfo *fu_info);
void fu_info_free(FunctionInfo *fu_info);
FunctionInfoList *fu_info_list_create();
FunctionInfoList *fu_info_list_add(FunctionInfoList *list, FunctionInfo *fu_info);
FunctionInfo *fu_info_list_lookup(FunctionInfoList *list, String *name);
FunctionInfoList *fu_info_list_del_first(FunctionInfoList *list);

#endif
