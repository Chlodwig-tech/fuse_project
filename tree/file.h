#ifndef FILE_H
#define FILE_H

#include "directory.h"

typedef struct FILE_H{
    char *name;
    char *path;
    Directory *parent;
}File;

#endif // FILE_H