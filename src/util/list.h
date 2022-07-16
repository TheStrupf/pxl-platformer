#ifndef LIST_H
#define LIST_H

#include "shared.h"

typedef struct list {
        int cap;
        int n;
        size_t esize;
        void *data;
} list;

list *list_create_sized(int cap, size_t esize);
void list_push(list *, void *);
void list_set(list *, void *, int);
void list_del_at(list *, int);
void list_del(list *, void *, bool (*c)(const void *, const void *));
void list_destroy(list *);
int list_find(list *, void *, bool (*c)(const void *, const void *));
void list_pop(list *, void *);
void list_get(list *, int, void *);

#endif