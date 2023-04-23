#include "valvepak.h"
#include "../smkl/type.h"
#include "../smkl/string.h"
#include <stdio.h>

#define MAX_FILES 2048
#define SIZE_FEXT 8
#define SIZE_FDIR 64
#define SIZE_FNAME 32
#define SIZE_VPKENTRY 18
#define SIZE_VPKHEADER 12

static inline uint8_t
_strcmp (const char* a, const char* b)
{
    while (*a++ == *b++) if (*a=='\0') return 1;
    return 0;
}
// abcdefghijklmnopqrstuvwxyz

static inline uint8_t
_read_string (FILE* f, char* str )
{
    char* cur = (char*)malloc(1);
    uint8_t len = 0;
    do
    {
        fread(cur,1,1,f);
        *(str++)=*cur; len++;
    }
    while(*cur);
    len--;
    *str='\0';
    free(cur);
    return len;
}

static inline uint16_t*
_short_hash (uint8_t len, const char* data)
{
    uint16_t* hash = (uint16_t*)malloc(20);
    uint16_t* short_hash = (uint16_t*)malloc(10);
    uint16_t* ret = short_hash;
    uint8_t i = 0;
    // SHA1(data, len, hash);
    // take only 10 bits (5 bytes -> 5 steps)
    while ( (*short_hash++=*hash++) && (i < 4) ) i++;
    return ret;
}

void
init_hashlist_buffer (hashlist* buffer, FILE* f)
{
    int* list = (int*)malloc(2*MAX_FILES);
    int* list_start = list;
    uint16_t len = 0;
    fseek(f, SIZE_VPKHEADER, SEEK_SET);
    while (1)
    {
        char ext [SIZE_FEXT];
        uint8_t len_ext = _read_string(f,ext);
        if (strcmp(ext,"")==0) break;
        while (1)
        {
            char dir [SIZE_FDIR];
            uint8_t len_dir = _read_string(f,dir);
            if (strcmp(dir,"")==0) break;
            while (1)
            {
                char name [SIZE_FNAME];
                uint8_t len_name = _read_string(f,name);
                if (strcmp(name,"")==0) break;

                uint8_t len_path = len_ext+len_dir+len_name;
                char path[len_path+1];
                path[0] = '\0';
                char *p = path;
                p = mk_strcat(p,dir);
                p = mk_strcat(p,name);
                p = mk_strcat(p,ext);
                *list++ = _short_hash(len_path, path);
                len++;
                // FSEEK BY SIZE OF PAKENTRY
                fseek(f,SIZE_VPKENTRY,SEEK_CUR);
            }
        }
    }
    buffer->len = len;
    buffer->list = list_start;
}

static inline void
cat_hashlist_buffer (hashlist* dest, hashlist* a, hashlist* b)
{
    int* iarr = (int*) malloc (a->len + b->len);
    int* iarr_start = iarr;
    while (*a->iarr) *iarr++=*a->iarr++;
    while (*b->iarr) *iarr++=*b->iarr++;
    dest->len = a->len + b->len;
    dest->iarr = iarr_start;
}

static inline char
_spot_dup (const int len, const int* iarr)
{
    // stop immediately once dup is found. O(1)
    int* a = iarr;
    int* b = iarr; b++;
    int i = 0;
    while (*a++ != *b++) i++;
    if ( i == len - 1) return 0;
    else return 1;
}

void close_hashlist_buffer (hashlist* buffer)
{
    free(buffer->iarr);
    free(buffer);
}

char
compare_paks (const char *path_a,
              const char *path_b)
{
    FILE* file_a = fopen(path_a, "rb"); // open file here because we want to select read-only access
    FILE* file_b = fopen(path_b, "rb");

    hashlist* buffer_a = (hashlist*)malloc(4+4);
    init_hashlist_buffer(buff_a, file_a);
    fclose(fa);
    hashlist* buffer_b = (hashlist*)malloc(4+4);
    init_hashlist_buffer(buff_b, file_b);
    fclose(file_b);
    // version 1
    hashlist* buffer_merge = (hashlist*)malloc(4+4);
    cat_hashlist_buffer(buffer_merge, buffer_a, buffer_b);

    mk_sort(buffer_merge->len, buffer_merge->iarr);
    ret = _spot_dup(buffer_c->len, buffer_c->iarr);

    close_hashlist_buffer(buffer_a);
    close_hashlist_buffer(buffer_b);
    close_hashlist_buffer(buffer_c);
    return ret;
}
