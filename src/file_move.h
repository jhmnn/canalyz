#include <stdio.h>


#ifndef FILE_MOVE_H
#define FILE_MOVE_H

#define GO_BACK 0
#define GO_FORWARD 1

extern int cur_file_line;

int go_search(FILE *file, int direction, char *search, char *stop);
int go_while(FILE *file, int direction, char *acc);
int go_while_not(FILE *file, int direction, char *rej);
int go_skip_block(FILE *file);
void go_to_pos(FILE *file, int pos);
int get_function_end(FILE *file);
int find_function_begin(FILE *file);
int find_function_call_begin(FILE *file);
int find_var(FILE *file);

#endif
