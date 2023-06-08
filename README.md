# Config Parser  

A library to deserialize a configuration file as per the below:  

- Leading whitespace in a line is ignored. All other whitespace (excluding \n) is considered as part of the section heading, key, or value.  
- Section headings are text from an initial '[' to the first ']'. Any other characters after this closing bracket will be ignored.  
- Keys and values are delimited by '='.  
- If key value pairs are provided without a section heading they are ignored.
