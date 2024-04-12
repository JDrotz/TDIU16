#include <stddef.h>

#include "flist.h"

void map_init(struct map *map)
{
    for (int i = 0; i < MAP_SIZE; i++)
        map->content[i] = NULL;
}

key_t map_insert(struct map *map, value_t value)
{
    // default -1 (fail) om den inte ändras till en korrekt key i loopen
    for (int i = 0; i < MAP_SIZE; i++)
    {
        if (map->content[i] == NULL)
        {
            map->content[i] = value;
            return i + 2;
        }
    }
    return -1;
}

value_t map_find(struct map *map, key_t key)
{
    if (key < 2 || key >= MAP_SIZE + 2)
        return NULL;

    return map->content[key - 2];
}

value_t map_remove(struct map *map, key_t key)
{
    // value_t val = NULL;
    if (key < 2 || key >= MAP_SIZE + 2)
        return NULL;

    struct file *temp = map->content[key - 2];
    map->content[key - 2] = NULL;

    return temp;
}

void map_for_each(struct map *m,
                  void (*exec)(key_t k, value_t v, int aux),
                  int aux)
{
    for (int i = 0; i < MAP_SIZE; i++)
        if (map_find(m, i) != NULL)
            exec(i, map_find(m, i), aux);
}

void map_remove_if(struct map *m,
                   bool (*cond)(key_t k, value_t v, int aux),
                   int aux)
{
    for (int i = 0; i < MAP_SIZE; i++)
        if (map_find(m, i) != NULL && cond(i, map_find(m, i), aux))
            map_remove(m, i);
}
