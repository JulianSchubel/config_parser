/*  
  ____ ___  _   _ _____ ___ ____   ____   _    ____  ____  _____ ____  
 / ___/ _ \| \ | |  ___|_ _/ ___| |  _ \ / \  |  _ \/ ___|| ____|  _ \ 
| |  | | | |  \| | |_   | | |  _  | |_) / _ \ | |_) \___ \|  _| | |_) |
| |__| |_| | |\  |  _|  | | |_| | |  __/ ___ \|  _ < ___) | |___|  _ < 
 \____\___/|_| \_|_|   |___\____| |_| /_/   \_\_| \_\____/|_____|_| \_\
                                                                       
 */

#include "./config_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SECTION_NUMBER 32
#define TEMPORARY_BUFFER_SIZE 2048

unsigned int system_status_code = 0;

enum config_parser_system_codes_ {
    SUCCESS = 0,
};

struct system_message {
    enum config_parser_system_codes_ error_code;
    const char * const message;
};

/* Status code numbers and their index in error_table must align */
struct system_message status_table[] = {
    {0, "Success"},
};

static void initialize_config(struct config_t *config) {
    config->lookup_table = NULL;
    config->section_count = 0;
    config->key_counts = NULL;
}

void deallocate_config(struct config_t *config) {
    for(unsigned int i = 0; i < config->section_count; ++i) {
        /* checking for NULL for non-compliant implementations: i.e. acting defensively despite the fact that standards compliant versions of the C library should treat free(NULL) as a no-op */
        if(config->lookup_table[i].section) {
            free(config->lookup_table[i].section);
        }
        for(unsigned int j = 0; j < (config->key_counts[i]); ++j) {
            if(config->lookup_table[i].key_value_pairs[j].key) {
                free(config->lookup_table[i].key_value_pairs[j].key);
            }
            if(config->lookup_table[i].key_value_pairs[j].value) {
                free(config->lookup_table[i].key_value_pairs[j].value);
            }
        }
        if(config->lookup_table[i].key_value_pairs) {
            free(config->lookup_table[i].key_value_pairs);
        }
    }
    if(config->key_counts) {
        free(config->key_counts);
    }
    if(config->lookup_table) {
        free(config->lookup_table);
    }
}

void display_config(struct config_t * config) {
    for(unsigned int i = 0; i < config->section_count; ++i) {
        printf("%s\n", config->lookup_table[i].section);      
        for(unsigned int j = 0; j < config->key_counts[i]; ++j) {
            printf("%s=%s\n",config->lookup_table[i].key_value_pairs[j].key, config->lookup_table[i].key_value_pairs[j].value);
        }
    }
}

/* output_temporary_buffer: displays the contents of the temporary buffer */
static void output_temporary_buffer(char *temporary_buffer, unsigned int temporary_buffer_size) {
    for(unsigned int i = 0; i < temporary_buffer_size; ++i) {
        printf("%c", temporary_buffer[i]);
    }
    printf("\n");
}

/* output_system_message: outputs a message indicating the status of the system based on the current value of system_status_code */
static void output_system_message() {
    printf("%s\n", status_table[system_status_code].message);
}

struct config_t * deserialize_config(const char *config_path) {
    extern unsigned int system_status_code;

    /* Open a file pointer */
    FILE * configuration_file = fopen(config_path, "r");

    /* Check that the file pointer is valid */
    if(configuration_file == NULL) {
        perror("Unable to open file");
        exit(EXIT_FAILURE);
    }

    /* Allocate a config object on the heap */
    struct config_t *config = (struct config_t *) malloc(sizeof(struct config_t));
    initialize_config(config);

    /* Initialize counters */
    unsigned int section_count = 0;
    unsigned int key_counts[MAX_SECTION_NUMBER];
    for(unsigned int i = 0; i < MAX_SECTION_NUMBER; ++i) {
        key_counts[i] = 0;
    }

    /* For use by getline; setting line_capacity and line to 0 and NULL respectively cause getline to handle allocation */
    ssize_t n_read = 0;
    size_t line_capacity = 0;
    char *line = NULL;
    while((n_read = getline(&line, &line_capacity, configuration_file)) != -1) {
        int c = 0;
        unsigned int i = 0;
        /* ignore leading whitespace */
        while((c = line[i++]) == ' ' || c == '\t') {
            ;
        }
        if(c == '[') {
            ++section_count;
        }
        else{
            if(section_count > 0) {
                ++key_counts[section_count-1];
            }
        }
    }

    //display_config(section_count, key_counts);
    fseek(configuration_file, 0L, SEEK_SET);

    config->section_count = section_count;
    /* Allocating memory to the config structure for key_counts array*/
    config->key_counts = (unsigned int *) malloc(sizeof(unsigned int) * section_count);
    for(unsigned int i = 0; i < section_count; ++i) {
        config->key_counts[i] = key_counts[i];
    }

    /* Have the number of sections and key=value pairs: can allocate memory for lookup_table and each key_value_pairs array */
    config->lookup_table = (struct section_t *) malloc(sizeof(struct section_t) * section_count);
    for(unsigned int i = 0; i < section_count; ++i) {
        config->lookup_table[i].key_value_pairs = (struct key_pair_t *) malloc(sizeof(struct key_pair_t) * key_counts[i]);
        config->lookup_table[i].key_value_pairs[key_counts[i] - 1].key = NULL;
        config->lookup_table[i].key_value_pairs[key_counts[i] - 1].value = NULL;
    }

    char temporary_buffer[TEMPORARY_BUFFER_SIZE];
    unsigned int temporary_buffer_index;

    /* Reset section and key counters */
    for(unsigned int i = 0; i < section_count; ++i) {
        key_counts[i] = 0;
    }
    section_count = 0;

    while((n_read = getline(&line, &line_capacity, configuration_file)) != -1) {
        if(system_status_code) {
            output_system_message();
            deallocate_config(config);
            free(line);
            fclose(configuration_file);
            exit(system_status_code);
        }
        int c = 0;
        unsigned int i = 0;
        /* ignore leading whitespace */
        while((c = line[i++]) == ' ' || c == '\t') {
            ;
        }
        if(c == '[') {
            ++section_count;
            temporary_buffer_index = 0;
            /* Read section heading value */ 
            while((c = line[i++]) != '\n' && c != ']' && temporary_buffer_index < TEMPORARY_BUFFER_SIZE-1) {
                /* count brackets to check that the section heading is enclosed */
                temporary_buffer[temporary_buffer_index++] = c;
            }
            temporary_buffer[temporary_buffer_index++] = '\0';

            /* Write the section heading to storage */
            config->lookup_table[section_count-1].section = (char *) malloc(sizeof(char) * temporary_buffer_index);
            for(unsigned int j = 0; j < temporary_buffer_index; ++j) {
                config->lookup_table[section_count-1].section[j] = temporary_buffer[j];
            }
        }
        else{
            /* Reset the temporary buffer index */
            temporary_buffer_index = 0;
            if(section_count > 0) {
                ++key_counts[section_count-1];
                
                /* Read key value into temporary buffer: in order to obtain value size for memory allocation */
                while(c != '=' && c != '\n' && temporary_buffer_index < TEMPORARY_BUFFER_SIZE-1) {
                    temporary_buffer[temporary_buffer_index++] = c;
                    c = line[i++];
                }
                temporary_buffer[temporary_buffer_index++] = '\0';
                /* Write key to storage */
                config->lookup_table[section_count-1].key_value_pairs[(key_counts[section_count-1])-1].key = (char*) malloc(sizeof(char) * temporary_buffer_index);
                for(unsigned int j = 0; j < temporary_buffer_index; ++j) {
                    config->lookup_table[section_count-1].key_value_pairs[(key_counts[section_count-1])-1].key[j] = temporary_buffer[j];
                }
                
                /* Reset the temporary buffer index */
                temporary_buffer_index = 0;
                /* Read value value into temporary buffer: in order to obtain value size for memory allocation */
                while((c = line[i++]) != '\n' && temporary_buffer_index < TEMPORARY_BUFFER_SIZE-1) {
                    temporary_buffer[temporary_buffer_index++] = c;
                }
                temporary_buffer[temporary_buffer_index++] = '\0';
                /* Write value to storage */
                config->lookup_table[section_count-1].key_value_pairs[(key_counts[section_count-1])-1].value = (char*) malloc(sizeof(char) * temporary_buffer_index);
                for(unsigned int j = 0; j < temporary_buffer_index; ++j) {
                    config->lookup_table[section_count-1].key_value_pairs[(key_counts[section_count-1])-1].value[j] = temporary_buffer[j];
                }
            }
        }
    }

    /* Must free line array after final use by getline() */
    free(line);
    /* Release the file pointer */
    fclose(configuration_file);
    return config;
}

char * lookup_value(struct config_t * config, const char * const section, const char * const key) {
    char section_found = 0;
    char key_found = 0;
    for(unsigned int i = 0; i < config->section_count; ++i) {
        if(strcmp(config->lookup_table[i].section, section) == 0) {
            section_found = 1;
            for(unsigned int j = 0; j < config->key_counts[i]; ++j) {
                if(strcmp(config->lookup_table[i].key_value_pairs[j].key, key) == 0) {
                    key_found = 1;
                    return config->lookup_table[i].key_value_pairs[j].value;
                }
            }
        }
    }
    if(section_found == 0) {
        printf("Unable to find section: %s\n", section);
    }
    else if (key_found == 0) {
        printf("Unable to find key: %s, under section: %s\n", key, section);
    }
    return NULL;
}
