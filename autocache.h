#ifndef AUTOCACHE_H
#define AUTOCACHE_H

#include <stdio.h>

#define MAX_FILE_SIZE 100000000 //10 MB
#define MAX_FILE_NAME_LEN 100

struct CachedFile {
    char data[MAX_FILE_SIZE];
    size_t size;
    char filename[MAX_FILE_NAME_LEN];
};

FILE *fopen_cached(const char *filename, const char *mode);

void init_cache();

int update_cached_file(const char *filename, void *data, size_t size);

size_t update_len_cached_file(struct CachedFile *cached_file);

struct CachedFile *get_cached_file(const char *filename, const char *mode);

FILE *fmemopen_test();
void fmemopen_check();

#endif /* AUTOCACHE_H */