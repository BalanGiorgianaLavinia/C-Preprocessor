#define SIZEHASHMAP 16
#define SIZEDIRSLIST 16

struct entry {
	void *key;
	void *value;
	struct entry *next;
};

struct hashmap {
	int sizeOfMap;
	struct entry **list;
};

struct dirsList {
	int size;
	char **paths;
	char *primaryDir;
};

struct hashmap *initMap(void);
int hashCode(char *key);
int addKeyValue(struct hashmap *myHashMap, char *key, char *value);
void *getValue(struct hashmap *myHashMap, char *key);
void removeKey(struct hashmap *myHashMap, char *key);
void destroy(struct hashmap *myHashMap);

struct dirsList *initDirsList(void);
int addDir(struct dirsList *list, char *path);
void destroyDirs(struct dirsList *list);
