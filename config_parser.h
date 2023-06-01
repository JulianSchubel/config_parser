#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct key_pair_t {
    char *key;
    char *value;
};

struct section_t {
   char *section; 
   unsigned int section_index;
   struct key_pair_t *key_value_pairs;
};

struct config_t {
    struct section_t *lookup_table;
    unsigned int section_count;
    unsigned int *key_counts;
};

/* desearialize_config: desearializes a .cfg file into a config structure */
struct config_t * deserialize_config(const char*);
/* initialize_config: initializes a config structure */
static void initialize_config(struct config_t*);
/* deallocate_config: deallocates the memory of a config structure. Must be called by the client program */
void deallocate_config(struct config_t*);
/* get_value: returns the value associated with key under section in the .cfg file, else returns NULL. */
char * lookup_value(struct config_t *config, const char* section, const char* key);
/* display_config: outputs the contents of a config structure */
void display_config(struct config_t * config);

