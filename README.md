# Config Parser  

A library to deserialize a configuration file of the below format:  

> Leading whitespace in a line is ignored, however all other whitespace is considered as part of the section heading, key, or value. 
> Section headings are text from an initial '[' to a ']'. Any other characters after the closing bracket will be ignored.
> Keys and values are delimited by '=' 
> If key value pairs are provided without a section heading they are ignored.
