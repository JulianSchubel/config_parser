# .cfg / .ini Parser 

A simple deserializer of configuration files.

Note the following:

- Leading whitespace in a line is ignored. All other whitespace (excluding \n) is considered as part of the section heading, key, or value.
- Section headings are text from an initial '[' to the first ']'. Any other characters after this closing bracket will be ignored.  
- Keys and values are delimited by '='.  
- If key value pairs are provided without a section heading they are ignored.

## Usage  

Given a config file my\_config.cfg with contents as below:  

```
[SECTION A]
key1=value A1
key2=value A2

[SECTION B]
    key1=value B1
    key2=value B2
```

The following code snippet

```
#include "./config_parser.h"

int main(int argc, char *argv[])
{

    struct config_t * conf = deserialize_config("./my_config.cfg");
    char * value = lookup_value(conf, "SECTION A", "key1");
    printf("%s\n", value);
    value = lookup_value(conf, "SECTION B", "key1");
    printf("%s\n", value);
    return EXIT_SUCCESS;
}
```

produces the result

```
value A1
value B1
```

## Compilation

Simply add the source file for linkage. E.g.

```
gcc main.c config_parser.c
```
