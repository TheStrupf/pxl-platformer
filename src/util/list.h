#ifndef LIST_H
#define LIST_H

#include "shared.h"

typedef struct list {
        int cap;
        int n;
        size_t element_size;
        void *data;
} list;

list *list_create_sized(int cap, size_t element_size);
void list_push(list *, void *);
void list_set(list *, void *, int);
void list_del_at(list *, int);
void list_del(list *, void *, bool (*c)(const void *, const void *));
void list_destroy(list *);
int list_find(list *, void *, bool (*c)(const void *, const void *));
void list_pop(list *, void *);
void list_get(list *, int, void *);

// simple function for function pointer in list_find, list_del
inline bool ptrsame(const void *a, const void *b)
{
        return a == b;
}

#endif