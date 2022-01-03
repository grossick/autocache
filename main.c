#include <string.h>

#include "autocache.h"

void test_read(){
    size_t bytes_read = 0;
    const char *filename = "test";
    FILE *file = fopen_cached(filename, "r");
    printf("FILE: %p\n", (void *) file);
    char buf[100];
    memset(buf, 0, sizeof(buf));
    bytes_read = fread(buf, 1, 100, file);
    printf("bytes read: %zu\n", bytes_read);
    printf("Test file contents: %s\n", buf);
    fclose(file);
}

void test_write() {
    FILE *file = fopen_cached("write_file", "w");
    const char *stuff = "asdf";
    size_t bytes_written = fwrite(stuff, 1, strlen(stuff), file);
    printf("wrote %zu bytes\n", bytes_written);
    fflush(file);
    fclose(file);
    fflush(file);

    struct CachedFile *cached_file = get_cached_file("write_file", "r");
    update_len_cached_file(cached_file);
    printf("write_file contents: %s\n", cached_file->data);
}

int main(int argc, char *argv[]) {
    init_cache();
    test_read();
    test_read();
    test_read();
    test_write();
}