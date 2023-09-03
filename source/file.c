
#include "file.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "alloc.h"

void resolve(const char *p, char out[URL_LENGTH])
{
    sprintf_s(out, URL_LENGTH, "%s%s", file->prefix, p);
}

void file_init(const char *pfx)
{
    file = (FileData *)alloc_global(sizeof(FileData));
    clear(file, sizeof(FileData));
    int n = (int)strlen(pfx);
    if (n > 128)
        n = 128;
    memcpy(file->prefix, pfx, n);
}

File *file_read(const char *p)
{
    char b[URL_LENGTH];
    resolve(p, b);

    FILE *f;
    fopen_s(&f, b, "r");

    if (f == NULL)
    {
        perror("failed to open the file");
        exit(EXIT_FAILURE);
        return NULL;
    }

    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    File *buff = (File *)alloc_stack(sizeof(File));
    buff->length = file_size;

    buff->text = (char *)alloc_stack(file_size + 1);
    size_t bytes_read = fread(buff->text, 1, file_size, f);
    buff->text[bytes_read] = '\0';
    fclose(f);
    return buff;
}

void file_destroy(File *f)
{
    free_stack(f->text);
    free_stack(f);
}