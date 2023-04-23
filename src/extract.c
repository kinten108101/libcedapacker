#include <stdio.h> // for file operations
#include <stdlib.h> // for exit
#include <sys/stat.h> // for mkdir
#include "vpk.h"
#include "../smkl/string.h" // for mk_strcat, mk_strlen
#include "../smkl/type.h"
#include "../smkl/io.h" /* for mk_mkdir */

#define SIZE_FEXT 8
#define SIZE_FDIR 64
#define SIZE_FNAME 32
#define SIZE_VPKHEADER 12
#define SIZE_VPKENTRY 18
#define BASEDIR "/home/kinten/Garage/mates-vpk"
#define SIZE_BASE_DIR 40

static inline int
read_string (FILE *f, char *str) 
{
  char cur_char;
  int len = 0;
  for (;;)
  {
    fread (&cur_char,1,1,f);
    if (!cur_char) break;
    len++;
    *(str++)=cur_char;
  }
  *str='\0';
  return len;
}

void 
write_tree_iter (const char* path,  
                 FILE* f,
                 unsigned int tree_size, 
                 unsigned int offset, 
                 unsigned int length)
{
  char content [length];
  long prev_offset = ftell (f);

  fseek (f, SIZE_VPKHEADER + tree_size + offset, SEEK_SET);
  fread (content,1,length,f);
  fseek (f, prev_offset, SEEK_SET);
    
  mk_mkdir (path);
  FILE *f_out = fopen (path,"w");
  if (f_out==NULL)
    mkdie ("Something is wrong!\n");
  fwrite (content, 1, length, f_out);

  fclose (f_out);
}

void
_pathcat (char *path_buffer,
      const char *base,
      const char *dir,
      const char *name,
      const char *ext)
{
  path_buffer[0] = '\0';
  char *p = path_buffer;
  p = mk_strcat(p, base);

  p = mk_strcat(p, "/");
  if ( mk_strcmp(dir, " ") == 0 ) goto files;
  p = mk_strcat(p,dir);
  p = mk_strcat(p,"/");
  files :
  p = mk_strcat(p,name);
  p = mk_strcat(p,".");
  p = mk_strcat(p,ext);
}

void
write_tree (int size_tree, FILE* stream_fvpk) 
{
  while (1)
  {
    char ext [SIZE_FEXT];
    int readlen = read_string (stream_fvpk, &ext[0]);
    if (!readlen) break;
    while (1)
    {
      char dir [SIZE_FDIR];
      int readlen = read_string (stream_fvpk, &dir[0]);
      if (!readlen) break;
      while (1)
      {
        char name [SIZE_FNAME];
        int readlen = read_string(stream_fvpk, &name[0]);
        if (!readlen) break;
      
        char path[200];
        _pathcat(&path[0], BASEDIR, dir, name, ext);
        
        VpkDirEntry entry;
        fread(&entry, 1, SIZE_VPKENTRY, stream_fvpk);
        write_tree_iter (path,  
                stream_fvpk,
                size_tree,
                entry.entry_offset,
                entry.entry_size);
      }
    }
  }
}

VpkDirHeader*
read_header (FILE* f)
{
  VpkDirHeader* header = (VpkDirHeader*)malloc(SIZE_VPKHEADER);
  fread(header,1,SIZE_VPKHEADER,f);
  // count vs step
  return header;
}

void
extract_pak (FILE* f)
{
  VpkDirHeader* header = read_header(f);
  {
    printf ("Singature: 0x%04x\n",header->sig);
    printf ("ver: %d\n",header->ver);
    printf ("Tree size (bytes): %d\n",header->tree_size);
  }
  write_tree(header->tree_size,f);
}