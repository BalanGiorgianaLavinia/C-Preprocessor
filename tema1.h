#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

int f_undef(struct hashmap *map, char *line);
int f_if(struct hashmap *map, char *line, int *ok_to_execute);
int f_ifdef(struct hashmap *map, char *line, int *ok_to_execute);
int f_replace(struct hashmap *map, char *line, char *copy, FILE *output);
int f_include(struct dirsList *dirs, char *line, FILE *output,
		struct hashmap *map);
int f_define(struct hashmap *map, char *line);
