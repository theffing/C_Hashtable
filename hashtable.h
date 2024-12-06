// item struct
typedef struct {
    char* key;
    char* value;
} item;

// table struct
typedef struct {
    int size;
    int count;
    int base_size;
    item** items;
} hashtable;

// functions
static item* new_item(const char* k, const char* v);
hashtable* new_hashtable();
static hashtable* new_sized_hashtable(const int base_size);
static void resize(hashtable* ht, const int base_size);
static void resize_up(hashtable* ht);
static void resize_down(hashtable* ht);
static void del_item(item* i);
void del_table(hashtable* ht);
static int hasher(const char* s, const int a, const int m);
static int get_hash(const char*s, const int buckets, const int attempt);

void insert(hashtable* ht, const char* key, const char* value);
char* search(hashtable* ht, const char* key);
void delete(hashtable* ht, const char* key);
