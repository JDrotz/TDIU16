/* do not forget the guard against multiple includes */
#pragma once

#include <stdbool.h>

typedef char *value_t;
typedef int key_t;
#define MAP_SIZE 32

struct map
{
    value_t content[MAP_SIZE];
};

void map_init(struct map *map);

key_t map_insert(struct map *map, value_t value);

value_t map_find(struct map *map, key_t key);

value_t map_remove(struct map *map, key_t key);

void map_for_each(struct map *m,
                  void (*exec)(key_t k, value_t v, int aux),
                  int aux);

void map_remove_if(struct map *m,
                   bool (*cond)(key_t k, value_t v, int aux),
                   int aux);