#ifndef LIST_H
#define LIST_H

#include "list_element.h"

typedef struct LIST{
    list_element *first;
    list_element *last;
    unsigned int size;
}list;

#endif // LIST_H