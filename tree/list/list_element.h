#ifndef LIST_ELEMENT_H
#define LIST_ELEMENT_H

typedef struct LIST_ELEMENT{
    void *data;
    struct LIST_ELEMENT *next;
    struct LIST_ELEMENT *previous;
}list_element;

#endif // LIST_ELEMENT_H