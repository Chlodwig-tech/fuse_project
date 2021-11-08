#ifndef LIST_H
#define LIST_H

#include "list_element.h"

typedef struct LIST{
    list_element *first;
    list_element *last;
    unsigned int size;
}list;

void list_init(list *l){
    l->first=NULL;
    l->last=NULL;
    l->size=0;
}

void list_append(list *l,void *data){
    if(l->first==NULL){
        l->first=l->last=(list_element*)malloc(sizeof(list_element));
        l->first->data=data;
    }else{
        list_element *helper=(list_element*)malloc(sizeof(list_element));
        helper->data=data;
        helper->next=NULL;
        helper->previous=l->last;
        l->last->next=helper;
        l->last=helper;
    }
    l->size++;
}

void list_pop(list *l){
    if(l->size>0){
        list_element *helper=l->last;
        if(l->first==l->last){
            l->first=l->last=NULL;
        }else{
            l->last=l->last->previous;
            l->last->next=NULL;
        }
        free(helper);
        l->size--;
    }
}

void *list_get_data(list *l,int index){
    if(index<0 || index>=l->size){
        return NULL;
    }
    list_element *helper=l->first;
    for(int i=0;i<index;i++){
        helper=helper->next;
    }
    return helper->data;
}

#endif // LIST_H