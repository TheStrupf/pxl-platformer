#include "list.h"
#include "mem.h"
#include "shared.h"

list *list_create_sized(int cap, size_t esize)
{
        void *memptr = mem_alloc(sizeof(list));
        if (memptr) {
                void *memptr2 = mem_alloc(esize * cap);
                if (!memptr2) {
                        mem_free(memptr);
                        return NULL;
                }
                list *l = memptr;
                l->data = memptr2;
                l->esize = esize;
                l->n = 0;
                l->cap = cap;
                return l;
        }
        return NULL;
}

void list_push(list *l, void *p)
{
        if (l->n == l->cap) {
                void *temp = mem_realloc(l->data, l->esize * l->cap * 2);
                if (!temp)
                        return;
                l->cap *= 2;
                l->data = temp;
        }
        list_set(l, p, l->n++);
}

void list_set(list *l, void *p, int i)
{
        memcpy((char *)l->data + i * l->esize, p, l->esize);
}

void list_del_at(list *l, int i)
{
        if (i < 0 || i >= l->n)
                return;
        if (--l->n > 0)
                memmove(
                    (char *)l->data + i * l->esize,
                    (char *)l->data + (i + 1) * l->esize,
                    l->esize * (l->n - i));
}

void list_del(list *l, void *p, bool (*c)(const void *, const void *))
{
        list_del_at(l, list_find(l, p, c));
}

void list_destroy(list *l)
{
        if (l) {
                mem_free(l->data);
                mem_free(l);
        }
}

int list_find(list *l, void *p, bool (*c)(const void *, const void *))
{
        char *it = l->data;
        for (int n = 0; n < l->n; n++) {
                if (c(p, (void *)it))
                        return n;
                it += l->esize;
        }
        return -1;
}

void list_pop(list *l, void *out)
{
        if (l->n == 0)
                memset(out, 0, l->esize);
        list_get(l, l->n - 1, out);
        list_del_at(l, l->n - 1);
}

void list_get(list *l, int i, void *out)
{
        if (i < l->n)
                memcpy(out, (char *)l->data + i * l->esize, l->esize);
        else
                memset(out, 0, l->esize);
}