#ifndef Q3_INTERN_H
#define Q3_INTERN_H

struct xs_node {
    char *data;
    struct xs_node *next;
};

struct xs_node* get_interning(char *str);

struct xs_node* add_interning(const char *str);

void add_interning_address(char *data);

void remove_interning(char *str);

#endif
