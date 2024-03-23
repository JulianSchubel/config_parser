/*  
  ____ ___  _   _ _____ ___ ____   ____   _    ____  ____  _____ ____  
 / ___/ _ \| \ | |  ___|_ _/ ___| |  _ \ / \  |  _ \/ ___|| ____|  _ \
| |  | | | |  \| | |_   | | |  _  | |_) / _ \ | |_) \___ \|  _| | |_) |
| |__| |_| | |\  |  _|  | | |_| | |  __/ ___ \|  _ < ___) | |___|  _ < 
 \____\___/|_| \_|_|   |___\____| |_| /_/   \_\_| \_\____/|_____|_| \_\
                                                                       
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct key_pair_t {
    char *key;
    char *value;
};

struct section_t {
   char *section; 
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

