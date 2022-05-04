#include "file_read.h"
#include "file_move.h"

#include <stdio.h>
#include <stdlib.h>


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
