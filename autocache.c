#include "autocache.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#define MAX_FILES_CACHED 10

struct CachedFile cached_file_db[MAX_FILES_CACHED];

int write_FILE_to_CachedFile(FILE *file, struct CachedFile *cached_file) {
    size_t bytes_read = 0;

    memset(cached_file->data, 0, sizeof(cached_file->data));
    fseek(file, 0, SEEK_SET);
    bytes_read = fread(cached_file->data, 1, MAX_FILE_SIZE, file);

    cached_file->size = bytes_read;

    return bytes_read;
}

int write_data_to_CachedFile(void *data, size_t size, struct CachedFile *cached_file){
    memset(cached_file->data, 0, sizeof(cached_file->data));
    if (size > sizeof(cached_file->data)) {
        return -1;
    }
    memcpy(cached_file->data, data, size);
    cached_file->size = size;
}

int prepare_memory_cached_file(struct CachedFile *cached_file, const char *filename, const char *mode, int fresh) {
    if (fresh){
        strncpy(cached_file->filename, filename, MAX_FILE_NAME_LEN);
    }
    if (mode[0] == 'r' && fresh){
        FILE *file = fopen(filename, "r");
        if (file == NULL){
            return -1;
        }
        write_FILE_to_CachedFile(file, cached_file);
        fclose(file);

    } else if(mode[0] == 'w'){
        memset(cached_file->data, 0, sizeof(cached_file->data));
        cached_file->size = 0;
    }
    return 0;
}

int search_cached_file(const char *filename, struct CachedFile **cached_file){
    int i = 0;
    for (i=0; i< MAX_FILES_CACHED; i++){
        if (strncmp(filename, cached_file_db[i].filename, MAX_FILE_NAME_LEN) == 0){
            *cached_file = &cached_file_db[i];
            return 0;
        }
    }
    return -1;
}

int get_unused_cache(struct CachedFile **cached_file){
    int i = 0;
    for (i=0; i < MAX_FILES_CACHED; i++) {
        if (cached_file_db[i].filename[0] == 0){
            *cached_file = &(cached_file_db[i]);
            return 0;
        }
    }
    return -1;
}

struct CachedFile *get_cached_file(const char *filename, const char *mode) {
    struct CachedFile *cached_file = NULL;
    int prepared = -1;
    if (search_cached_file(filename, &cached_file) < 0){
        if (get_unused_cache(&cached_file) < 0){
            return NULL;
        }
        prepared = prepare_memory_cached_file(cached_file, filename, mode, 1);
    }
    else {
        prepared = prepare_memory_cached_file(cached_file, filename, mode, 0);
    }

    if (prepared < 0) {
        return NULL;
    }

    return cached_file;
}

int update_cached_file(const char *filename, void *data, size_t size){
    if (size > MAX_FILE_SIZE){
        return -1;
    }

    // Use mode of 'w' here to wipe out old data and avoid reading data from disk
    struct CachedFile *cached_file = get_cached_file(filename, "w");

    write_data_to_CachedFile(data, size, cached_file);
}

size_t update_len_cached_file(struct CachedFile *cached_file) {
    size_t size;
    for (size = MAX_FILE_SIZE - 1; size > 0; size--) {
        if (cached_file->data[size] != 0) {
            cached_file->size = size + 1;
            printf("UPDATED FILESIZE FOR %s is %zu\n", cached_file->filename, cached_file->size);
            return cached_file->size;
        }
    }
    return 0;
}

FILE *fopen_cached(const char *filename, const char *mode) {
    if (mode[0] == 'a'){
        // Append is not currently supported
        return fopen(filename, mode);
    }
    struct CachedFile *cached_file = NULL;
    if ((cached_file = get_cached_file(filename, mode)) == NULL) {
        return fopen(filename, mode);
    }


    // 'w' does not truncate the file in fmemopen like it does on fopen
    const char *fmemopen_mode = (mode[0] == 'w') ? "w" : mode;
    size_t fmemopen_size = (mode[0] == 'w') ? MAX_FILE_SIZE : cached_file->size;
    return fmemopen(cached_file->data, fmemopen_size, fmemopen_mode);
}

void init_cache(){
    //cached_file_db = mmap(NULL, sizeof(struct CachedFile)*MAX_FILES_CACHED,
	//	    PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    memset(cached_file_db, 0, sizeof(cached_file_db));
}

