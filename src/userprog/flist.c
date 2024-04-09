#include <stddef.h>

#include "flist.h"

void map_init(struct map *map)
{
    for (int i = 2; i < MAP_SIZE; i++)
        map->content[i] = NULL;
}

key_t map_insert(struct map *map, value_t value)
{
    // default -1 (fail) om den inte ändras till en korrekt key i loopen
    key_t key = -1;
    for (int i = 2; i < MAP_SIZE; i++)
    {
        if (map->content[i] == NULL)
        {
            map->content[i] = value;
            key = i;
            break;
        }
    }
    return key;
}

value_t map_find(struct map *map, key_t key)
{
    if (map->content[key] != NULL)
        return map->content[key];
    return NULL;
}

value_t map_remove(struct map *map, key_t key)
{
    value_t val = NULL;
    if (map->content[key] != NULL)
    {
        val = map->content[key];
        map->content[key] = NULL;
    }
    return val;
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
