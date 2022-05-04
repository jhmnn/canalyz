#include "str.h"
#include "variable.h"
#include "function.h"
#include "file_move.h"
#include "file_read.h"
#include "output.h"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int cur_file_line = 1;

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

    FunctionInfoList *fu_infos = fu_info_list_create(); 
    if (!fu_infos) return 1;
    VariableList *global_vars = var_list_create();
    if (!global_vars) return 1;
    
    int result = read_file(file, fu_infos, global_vars);

    if (result) printf("Syntax error in file\n");
    
    fu_infos = fu_info_list_del_first(fu_infos);
    global_vars = var_list_del_first(global_vars);

    fseek(file, 0, SEEK_SET);
    cur_file_line = 1;
    
    putchar('\n');
    output_fu_info(fu_infos);
    putchar('\n');
    output_global_vars(global_vars);
    putchar('\n');
    output_conflicts(file, fu_infos, global_vars);

    fclose(file);
    return 0;
}
