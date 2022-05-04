#include "str.h"

#include <stdlib.h>
#include <string.h>


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
