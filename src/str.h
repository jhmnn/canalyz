#include <string.h>


#ifndef STR_H
#define STR_H

typedef struct {
    size_t capacity;
    size_t size;

    char *data;
} String;

typedef struct StringList {
    struct StringList *next;

    String *string;
} StringList;


String *string_data_clear(String *string);
String *string_create(size_t capacity);
String *string_capacity_increase(String *string);
String *string_add_back(String *string, char ch);
String *string_data_copy(String *in, String *from);
void string_delete_index(String *string, int index);
String *string_space_clear(String *string);
void string_free(String *string);
StringList *string_list_create();
StringList *string_list_add(StringList *list, String *string);
StringList *string_list_del_first(StringList *list);
void string_list_free(StringList *list);

#endif
