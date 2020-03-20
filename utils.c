#include "utils.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

/* this function return NULL if something went wrong, */
/* or a new hashMap otherwise */
struct hashmap *initMap(void)
{
	int i = 0;
	struct entry **list = NULL;
	struct hashmap *myHashMap = malloc(sizeof(struct hashmap));

	if (myHashMap == NULL)
		return NULL;

	list = malloc(SIZEHASHMAP * sizeof(struct entry *));
	if (list == NULL) {
		free(myHashMap);
		return NULL;
	}

	for (i = 0; i < SIZEHASHMAP; i++)
		list[i] = NULL;

	myHashMap->sizeOfMap = SIZEHASHMAP;
	myHashMap->list = list;

	return myHashMap;
}


int hashCode(char *key)
{
	int code = 0;
	char firstLetter = key[0];

	code = ((int) firstLetter) % SIZEHASHMAP;

	return code;
}

int addKeyValue(struct hashmap *myHashMap, char *key, char *value)
{
	int hash = hashCode(key);
	struct entry *head = NULL;

	head = myHashMap->list[hash];
	while (head != NULL) {
		if (strcmp(head->key, key) == 0) {
			free(head->value);
			head->value = malloc((strlen(value) + 1) *
					sizeof(char));
			if (head->value == NULL)
				return -ENOMEM;
			strcpy(head->value, value);
			return 0;
		}
		head = head->next;
	}

	head = malloc(sizeof(struct entry));
	if (head == NULL)
		return -ENOMEM;

	head->key = malloc((strlen(key) + 1) * sizeof(char));
	if (head->key == NULL)
		return -ENOMEM;

	head->value = malloc((strlen(value) + 1) * sizeof(char));
	if (head->value == NULL)
		return -ENOMEM;

	strcpy(head->key, key);
	strcpy(head->value, value);

	head->next = myHashMap->list[hash];
	myHashMap->list[hash] = head;


	return 0;
}

void *getValue(struct hashmap *myHashMap, char *key)
{
	char *value = NULL;
	int hash = hashCode(key);
	struct entry *head = NULL;

	if (myHashMap == NULL)
		return NULL;

	head = myHashMap->list[hash];

	/* search for key in map and take its value if existing*/
	while (head != NULL) {
		if (strcmp(head->key, key) == 0)
			return head->value;
		head = head->next;
	}

	return value;

}

void removeKey(struct hashmap *myHashMap, char *key)
{
	int hash = hashCode(key);
	struct entry *head = NULL;
	struct entry *prevElem = NULL;

	if (myHashMap == NULL)
		return;

	head = myHashMap->list[hash];
	prevElem = head;

	/* serach for given key in map and remove it if existing */
	while (head != NULL) {
		if (strcmp(head->key, key) == 0) {
			if (prevElem == myHashMap->list[hash])
				myHashMap->list[hash] = head->next;
			else
				prevElem->next = head->next;

			free(head->key);
			free(head->value);
			free(head);
			return;
		}

		prevElem = head;
		head = head->next;
	}


}

void destroy(struct hashmap *myHashMap)
{
	int i = 0;
	struct entry *head = NULL;
	struct entry *prevElem = NULL;

	for (i = 0; i < SIZEHASHMAP; i++) {
		head = myHashMap->list[i];
		prevElem = head;

		while (head != NULL) {
			prevElem = head;
			head = head->next;

			free(prevElem->key);
			free(prevElem->value);
			free(prevElem);
		}
	}

	free(myHashMap->list);
	free(myHashMap);
}


struct dirsList *initDirsList(void)
{
	int i = 0;

	struct dirsList *list = malloc(sizeof(struct dirsList));

	if (list == NULL)
		return NULL;

	list->size = SIZEDIRSLIST;
	list->primaryDir = NULL;
	list->paths = malloc(SIZEDIRSLIST * sizeof(char *));
	if (list->paths == NULL) {
		free(list);
		return NULL;
	}

	for (i = 0; i < list->size; i++)
		list->paths[i] = NULL;

	return list;
}

int addDir(struct dirsList *list, char *path)
{
	int error = 0;
	int i = 0;
	char **new = NULL;

	/* add new path at first free space in list if existing*/
	/* else realloc the array */
	for (i = 0; i < list->size; i++) {
		if (list->paths[i] == NULL) {
			list->paths[i] = path;
			return 0;
		}
	}

	new = realloc(list->paths, 2 * list->size);
	if (new == NULL) {
		if (list->primaryDir != NULL)
			free(list->primaryDir);

		free(list);
		return -ENOMEM;
	}

	/* make the new half of array point to NULL */
	/* except the first new slot where the path should be */
	i = list->size;
	list->paths = new;
	list->size *= 2;
	list->paths[i++] = path;
	for (; i < list->size; i++)
		list->paths[i] = NULL;


	return error;
}

void destroyDirs(struct dirsList *list)
{
	int i = 0;

	if (list == NULL)
		return;

	for (; i < list->size; i++)
		if (list->paths[i] != NULL)
			free(list->paths[i]);

	free(list->paths);

	if (list->primaryDir != NULL)
		free(list->primaryDir);

	free(list);
}


