#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "list/list.h"

typedef struct DIRECTORY_{
    char *name;
    char *path;
    struct DIRECTORY_ *parent;
    list directories;
    list files;
}Directory;

#endif // DIRECTORY_H