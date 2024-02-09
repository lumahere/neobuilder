/*
MIT License

Copyright (c) 2024 Nathanael Adrean

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 * */

#ifndef H_NCPM_BUILDER
#define H_NCPM_BUILDER
// Version Control
#define CPM_MAJOR_VERSION 0
#define CPM_MINOR_VERSON 1
#define CPM_PATCH_VERSION 2
// 0.1.2

#define DEFAULT_COMPILER "cc"

#define UNIMPLEMENTED{ \
  cpm_log(CPM_ERROR, "function is not implemented yet O^O; at line %s\n", __LINE__);\
  exit(EXIT_FAILURE);}\


#define KERR "\x1B[31m" // RED
#define KWAR "\x1B[33m" // YELLOW
#define KMSG "\x1B[35m" // MAGENTA
#define KNRM "\x1B[0m"  // NORMAL COLOR

#include <time.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
typedef struct String {
  size_t size;
  size_t cap;
  char *str;
} String;

typedef struct MemFile {
  size_t size;
  void *data;
} MemFile;

typedef enum loglvl {
  CPM_INFO,
  CPM_WARNING,
  CPM_ERROR,
} Loglevel;

// Redefinition of String = Cmd;; OWO
typedef String Cmd;

#define STRING_INIT_CAPA 255 // IDK Should work?

void string_append(String *str, const char *src) {
  size_t srcsize = strlen(src);
  if (!str->str) {
    str->cap = STRING_INIT_CAPA;
    str->size = 0;
    str->str =
        (char *)calloc(STRING_INIT_CAPA, sizeof(char)); // Malloc cant'locc
  }
  if (str->size + srcsize >= str->cap) {
    str->cap *= 2;
    str->str = (char *)realloc(str->str, str->cap);
  }
  str->str = strcat(str->str, src);
  str->size += srcsize;
}
#define string_free(string) free(string.str);

void appendcmdnull(Cmd *cmd, ...) {
  va_list args;
  va_start(args, cmd);
  char *arg = va_arg(args, char *);
  while (arg != NULL) {
    string_append(cmd, arg);
    string_append(cmd, " ");
    arg = va_arg(args, char *);
  }
  va_end(args);
}

#define cpm_cmd_append(cmdptr, ...) appendcmdnull(cmdptr, __VA_ARGS__, NULL)

#define cpm_cmd_exec(cmd)                                                      \
  {                                                                            \
    system(cmd.str);                                                           \
    free(cmd.str);                                                             \
  }

void cpm_log(Loglevel lvl, const char *fmt, ...) {
  String msg = {0};
  time_t time_now;
  struct tm *time_info;
  time(&time_now);
  time_info = localtime(&time_now);
  char timenow[128];
  sprintf(timenow, "(%02d:%02d:%02d)", time_info->tm_hour, time_info->tm_min,
          time_info->tm_sec);
  switch (lvl) {
  case CPM_INFO:
    string_append(&msg, KMSG);
    string_append(&msg, "LOG");
    string_append(&msg, " ");
    string_append(&msg, timenow);
    string_append(&msg, ":");
    string_append(&msg, " ");
    fprintf(stdout, "%s", msg.str);
    break;
  case CPM_WARNING:
    string_append(&msg, KWAR);
    string_append(&msg, "WARNING");
    string_append(&msg, " ");
    string_append(&msg, timenow);
    string_append(&msg, ":");
    string_append(&msg, " ");
    fprintf(stdout, "%s", msg.str);
    break;
  case CPM_ERROR:
    string_append(&msg, KERR);
    string_append(&msg, "ERROR");
    string_append(&msg, " ");
    string_append(&msg, timenow);
    string_append(&msg, ":");
    string_append(&msg, " ");
    fprintf(stdout, "%s", msg.str);
    break;
  }
  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  va_end(args);
  fprintf(stdout, "%s", KNRM);
  string_free(msg);
}

// compares file1 against file2 returning true if file1 is newer than file2
bool cmp_modtime(const char *file1, const char *file2) {
  struct stat oneInfo, twoInfo;
  if (-1 == stat(file1, &oneInfo)) {
    cpm_log(CPM_ERROR, "Failed to stat file1 at %d\n", __LINE__);
    perror("stat: ");
    exit(-1);
  }
  if (-1 == stat(file2, &twoInfo)) {
    cpm_log(CPM_ERROR, "Failed to stat file2 at %d\n", __LINE__);
    perror("stat: ");
    exit(-1);
  }
  return oneInfo.st_mtime > twoInfo.st_mtime;
}

#define GETFILENAME(name) (strrchr(name, '/') ? strrchr(name, '/') + 1 : name)
#define CPM_REBUILD_SELF(argv)                                                 \
                                                                               \
  if (cmp_modtime(__FILE__, argv[0])) {                                        \
    Cmd self_changename_ = {0};                                                \
    Cmd self_rebuild_ = {0};                                                   \
    cpm_cmd_append(&self_changename_, "mv");                                   \
    cpm_cmd_append(&self_changename_, argv[0]);                                \
    string_append(&self_changename_, argv[0]);                                 \
    string_append(&self_changename_, ".old");                                  \
    cpm_log(CPM_WARNING, "changing current executable to old\n");              \
    cpm_cmd_exec(self_changename_);                                            \
                                                                               \
    cpm_cmd_append(&self_rebuild_, DEFAULT_COMPILER);                                  \
    cpm_cmd_append(&self_rebuild_, __FILE__);                                  \
    cpm_cmd_append(&self_rebuild_, "-o", GETFILENAME(argv[0]));                \
    cpm_log(CPM_WARNING, "rebuilding the builder\n");                          \
    cpm_cmd_exec(self_rebuild_);                                               \
    cpm_log(CPM_INFO, "running new builder!\n");                               \
    if (system(argv[0])) {                                                     \
      cpm_log(CPM_ERROR, "failed to run new builder!\n");                      \
      exit(1);                                                                 \
    }                                                                          \
    exit(0);                                                                   \
  }

//FILE OPS
bool file_exists(const char* file_path){
  FILE* file;
  file = fopen(file_path, "r");
  if (!file){
    return false;
  }
  fclose(file);
  return 1;
}

MemFile load_file_to_mem(const char* file_path){
  MemFile file = {0};
  FILE* actfile =  fopen(file_path, "rb");
  if( actfile != 0){
    fseek(actfile, 0, SEEK_END);
    file.size = ftell(actfile);
    fseek(actfile, 0, SEEK_SET);
  } else {
    cpm_log(CPM_ERROR, "Failed to read and load %s to memory, exiting with exit failure\n", file_path);
    perror("Errno at load_file_to_mem: ");
    exit(EXIT_FAILURE);
  }
  void* memoryfile = malloc(file.size);
  fread(memoryfile, sizeof(char), file.size, actfile);
  file.data = memoryfile;
  fclose(actfile);
  return file;
}

//only run compilation if the src is newer than obj
void cpm_recompile(const char* srcpath, const char* objpath, Cmd compilation_cmd){
  if(!file_exists(objpath)){
    cpm_log(CPM_INFO, "%s", compilation_cmd.str);
    cpm_cmd_exec(compilation_cmd);
  }
  else if(cmp_modtime(srcpath, objpath)){
    cpm_log(CPM_INFO, "recompiling %s => %s\n", srcpath, compilation_cmd.str);
    cpm_cmd_exec(compilation_cmd);
  } else {
    cpm_log(CPM_INFO, "Skipping compilation of %s\n", srcpath);
  }
}


#endif
