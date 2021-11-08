#ifndef DIRECTORY_H
#define DIRECTORY_H

typedef struct DIRECTORY_{
    char *name;
    char *path;
    struct DIRECTORY_ *parent;
};

#endif // DIRECTORY_H