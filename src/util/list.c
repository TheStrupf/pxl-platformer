#include "list.h"
#include "shared.h"

list *list_create_sized(int cap, size_t element_size)
{
        void *memptr = malloc(sizeof(list));
        if (memptr) {
                void *memptr2 = malloc(element_size * cap);
                if (!memptr2) {
                        free(memptr);
                        return NULL;
                }
                list *l = memptr;
                l->data = memptr2;
                l->element_size = element_size;
                l->n = 0;
                l->cap = cap;
                return l;
        }
        return NULL;
}

void list_push(list *l, void *p)
{
        if (l->n == l->cap) {
                void *temp = realloc(l->data, l->element_size * l->cap * 2);
                if (temp) {
                        l->cap *= 2;
                        l->data = temp;
                }
        }
        list_set(l, p, l->n++);
}

void *list_pop(list *l)
{
        if (l->n == 0)
                return NULL;
        return list_get(l, --l->n);
}

void list_set(list *l, void *p, int i)
{
        memcpy((char *)l->data + i * l->element_size, p, l->element_size);
}

void list_del_at(list *l, int i)
{
        if (i < 0 || i >= l->n)
                return;
        if (--l->n > 0)
                memmove(
                    (char *)l->data + i * l->element_size,
                    (char *)l->data + (i + 1) * l->element_size,
                    l->element_size * (l->n - i));
}

void list_del(list *l, void *p, bool (*c)(const void *, const void *))
{
        list_del_at(l, list_find(l, p, c));
}

void *list_get(list *l, int i)
{
        if (i < l->n)
                return ((char *)l->data + i * l->element_size);
        return NULL;
}

void list_destroy(list *l)
{
        if (l) {
                free(l->data);
                free(l);
        }
}

int list_find(list *l, void *p, bool (*c)(const void *, const void *))
{
        char *it = l->data;
        for (int n = 0; n < l->n; n++) {
                if (c(p, (void *)it))
                        return n;
                it += l->element_size;
        }
        return -1;
}