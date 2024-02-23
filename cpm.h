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
// 0.1.2
#define CPM_MAJOR_VERSION 0
#define CPM_MINOR_VERSON 2
#define CPM_PATCH_VERSION 3

#define DEFAULT_COMPILER "cc"
#define DEFAULT_CPM_PATH "."

#define UNIMPLEMENTED                                                          \
  {                                                                            \
    cpm_log(CPM_ERROR,                                                         \
            "function is not implemented yet O^O; CPM.h at line %d\n",         \
            __LINE__);                                                         \
    exit(EXIT_FAILURE);                                                        \
  }

#define KERR "\x1B[31m" // RED
#define KWAR "\x1B[33m" // YELLOW
#define KMSG "\x1B[35m" // MAGENTA
#define KNRM "\x1B[0m"  // NORMAL COLOR

#include <errno.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include <dirent.h>
#include <fnmatch.h>

typedef struct String {
  size_t size;
  size_t cap;
  char *str;
} String;

typedef struct MemFile {
  size_t size;
  void *data;
} MemFile;

typedef enum loglvl { CPM_INFO, CPM_WARNING, CPM_ERROR, CPM_DEBUG } Loglevel;

// Redefinition of String = Cmd;; OWO
typedef String Cmd;

typedef struct SplitString {
  String **strarr;
  size_t cap;
  size_t size;

} StringArray;

typedef enum CliReturn { BUILD, RUN, CLEAN, NONE } CliReturn;

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
  if (!src) {
    str->str = strcat(str->str, "");
  } else {
    str->str = strcat(str->str, src);
    str->size += srcsize;
  }
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

StringArray str_split_at(const char *str, const char *delimiter) {
  StringArray res = {0};
  res.cap = 5;
  res.size = 0;
  res.strarr = (String **)calloc(res.cap, sizeof(String *));
  for (int i = 0; i < res.cap; i++) {
    res.strarr[i] = (String *)calloc(1, sizeof(String));
  }
  char *srcpy = strdup(str);
  char *token = strtok(srcpy, delimiter);
  while (token) {
    if (res.size == res.cap) {
      res.cap += 5;
      res.strarr = (String **)realloc(res.strarr, res.cap * sizeof(String *));
      for (int i = res.size; i < res.cap; i++) {
        res.strarr[i] = (String *)calloc(1, sizeof(String));
      }
    }
    ++res.size;
    string_append(res.strarr[res.size - 1], token);
    token = strtok(NULL, " ");
  }
  free(token);
  free(srcpy);

  return res;
}

void string_array_append(StringArray *sp, String str) {
  if (sp->strarr == NULL) {
    sp->size = 0;
    sp->cap = 5;
    sp->strarr = (String **)calloc(sp->cap, sizeof(String));
    for (int i = 0; i < sp->cap; i++) {
      sp->strarr[i] = (String *)calloc(1, sizeof(String));
    }
  }

  if (sp->size == sp->cap) {
    sp->cap += 5;
    sp->strarr = (String **)realloc(sp->strarr, sp->cap * sizeof(String *));
    for (int i = sp->size; i < sp->cap; i++) {
      sp->strarr[i] = (String *)calloc(1, sizeof(String));
    }
  }
  string_append(sp->strarr[sp->size], strdup(str.str));
  sp->size++;
}

void string_array_join(StringArray *src, String *dest, const char *delimiter) {
  for (int i = 0; i < src->size; i++) {
    string_append(dest, src->strarr[i]->str);
    string_append(dest, delimiter);
  }
}

#define cpm_cmd_append(cmdptr, ...) appendcmdnull(cmdptr, __VA_ARGS__, NULL)

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
    fflush(stdout);
    break;
  case CPM_WARNING:
    string_append(&msg, KWAR);
    string_append(&msg, "WARNING");
    string_append(&msg, " ");
    string_append(&msg, timenow);
    string_append(&msg, ":");
    string_append(&msg, " ");
    fprintf(stdout, "%s", msg.str);
    fflush(stdout);
    break;
  case CPM_ERROR:
    string_append(&msg, KERR);
    string_append(&msg, "ERROR");
    string_append(&msg, " ");
    string_append(&msg, timenow);
    string_append(&msg, ":");
    string_append(&msg, " ");
    fprintf(stderr, "%s", msg.str);
    fflush(stderr);
    break;
#ifndef CPM_DEBUG
  case CPM_DEBUG:
    string_append(&msg, KMSG);
    string_append(&msg, "DEBUG");
    string_append(&msg, " ");
    string_append(&msg, timenow);
    string_append(&msg, ":");
    string_append(&msg, " ");
    fprintf(stdout, "%s", msg.str);
    break;
#endif
  }

  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  va_end(args);
  fprintf(stdout, "%s", KNRM);
  string_free(msg);
}

void cpm_cmd_exec(Cmd cmd) {
  // cpm_log(CPM_INFO, "%s\n", cmd.str);
  if (system(cmd.str))
    cpm_log(CPM_ERROR, "cmd: %s failed to execute\n", cmd.str);
  free(cmd.str);
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
    cpm_cmd_append(&self_rebuild_, DEFAULT_COMPILER);                          \
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

// FILE OPS
bool file_exists(const char *file_path) {
  FILE *file;
  file = fopen(file_path, "rb");
  if (file == NULL) {
    return false;
  }
  fclose(file);
  return true;
}

MemFile load_file_to_mem(const char *file_path) {
  MemFile file = {0};
  FILE *actfile = fopen(file_path, "rb");
  if (actfile != 0) {
    fseek(actfile, 0, SEEK_END);
    file.size = ftell(actfile);
    fseek(actfile, 0, SEEK_SET);
  } else {
    cpm_log(CPM_ERROR,
            "Failed to read and load %s to memory, exiting with exit failure\n",
            file_path);
    perror("Errno at load_file_to_mem: ");
    exit(EXIT_FAILURE);
  }
  void *memoryfile = malloc(file.size);
  fread(memoryfile, sizeof(char), file.size, actfile);
  file.data = memoryfile;
  fclose(actfile);
  return file;
}

// how to parse
// typical cmd: clang PATH -o PATH {FLAGS}
bool cpm_compile(Cmd compilation_cmd) {

  cpm_log(CPM_INFO, "%s\n", compilation_cmd.str);
  size_t arrcap = 5;
  size_t arrcount = 0;
  char **strarr = (char **)calloc(arrcap, sizeof(char *));
  char *srcpy = strdup(compilation_cmd.str);
  char *token = strtok(srcpy, " ");
  while (token) {
    if (arrcount == arrcap) {
      arrcap *= 2;
      strarr = (char **)realloc(strarr, arrcap * sizeof(char *));
    }
    ++arrcount;
    strarr[arrcount - 1] = strdup(token);
    token = strtok(NULL, " ");
  }
  free(token);
  free(srcpy);

  char *out = 0;
  char *in = 0;
  // PURPOSE: DO THIS IN ONE LOOP "kuriosa to demiso"
  for (int i = 0; i < arrcount; ++i) {
    // Check if the current str is -o and after that it means the output path
    if (strcmp(strarr[i], "-o") == 0) {
      i++; // Skip an iteration to output path
      out = strdup(strarr[i]);
    }
    if (strstr(strarr[i], ".c")) {
      in = strdup(strarr[i]);
    }
  }

  bool res;
  if (!file_exists(out)) {
    cpm_cmd_exec(compilation_cmd);
    res = true;
  } else if (cmp_modtime(in, out)) {
    cpm_log(CPM_INFO, "recompiling %s => %s\n", in, compilation_cmd.str);
    cpm_cmd_exec(compilation_cmd);
    res = true;
  } else {
    cpm_log(CPM_INFO, "Skipping compilation of %s\n", in);
    free(compilation_cmd.str);
    res = false;
  }

  free(in);
  free(out);
  for (int i = 0; i < arrcap; i++) {
    free(strarr[i]);
  }
  free(strarr);
  return res;
}

// RETURNS PID OF CHILD
int cpm_compile_async(Cmd compile_command) {
  int child = fork();
  if (child == -1) {
    cpm_log(CPM_WARNING, "Could not fork process for async compilation, "
                         "defaulting to non-async compilation\n");
    perror("At fork: ");
    cpm_compile(compile_command);

  } else if (child == 0) {
    if (!cpm_compile(compile_command))
      return 0;
    else
      return child;
  }
  return 0;
}

// POLLS ASYNC COMPILATION
void cpm_compile_poll(int compile_index) {
  int status;
  if (compile_index != 0) {
    if (waitpid(compile_index, &status, 0) == -1)
      cpm_log(CPM_ERROR, "waitpid error\n");
    perror("error at waitpid: ");
    if (WIFSIGNALED(status)) {
      cpm_log(CPM_ERROR, "Compilation terminated by signal: %d\n",
              WTERMSIG(status));
    }
  }
}

// Support makefiles and build.c
void cpm_submodule(const char *path_to_folder, const char *options) {
  String makefile = {0};
  string_append(&makefile, path_to_folder);
  string_append(&makefile, "/");
  string_append(&makefile, "makefile");
  String Makefile = {0};
  string_append(&Makefile, path_to_folder);
  string_append(&Makefile, "/");
  string_append(&Makefile, "Makefile");
  String buildc = {0};
  string_append(&buildc, path_to_folder);
  string_append(&buildc, "/");
  string_append(&buildc, "build.c");

  if (file_exists(buildc.str)) {
    cpm_log(CPM_INFO, "Building submodule %s => build.c\n", path_to_folder);
    Cmd cmd = {0};
    Cmd run = {0};
    cpm_cmd_append(&cmd, DEFAULT_COMPILER, buildc.str, "-o");
    string_append(&cmd, path_to_folder);
    string_append(&cmd, "/buildscript ");
    cpm_cmd_append(&cmd, "-I", ".");
    cpm_compile(cmd);

    cpm_cmd_append(&run, "cd", path_to_folder, "&&");
    cpm_cmd_append(&run, "./buildscript");
    cpm_log(CPM_INFO, "========================================================"
                      "==========\n\n");
    if (options != 0)
      cpm_cmd_append(&run, options);
    cpm_cmd_exec(run);
  } else if (file_exists(makefile.str) || file_exists(Makefile.str)) {
    cpm_log(CPM_INFO, "Building submodule %s => makefile\n", path_to_folder);
    Cmd cmd = {0};
    cpm_cmd_append(&cmd, "cd", path_to_folder, "&&", "make");
    if (options != 0)
      cpm_cmd_append(&cmd, options);
    cpm_cmd_exec(cmd);
  } else {
    cpm_log(CPM_ERROR, "submodule \"%s\" doesn't have a makefile or build.c\n",
            path_to_folder);
  }
  string_free(Makefile);
  string_free(makefile);
  string_free(buildc);
}

void cpm_mkdir(const char *dirpath) {
  Cmd cmd = {0};
  cpm_cmd_append(&cmd, "mkdir", "-p", dirpath);
  cpm_cmd_exec(cmd);
}
void cpm_rm(const char *path) {
  Cmd cmd = {0};
  cpm_cmd_append(&cmd, "rm", "-rf", path);
  cpm_cmd_exec(cmd);
}

void cpm_cp(const char *srcpath, const char *destpath) {
  Cmd cmd = {0};
  cpm_cmd_append(&cmd, "cp", "-r", srcpath, destpath);
  cpm_cmd_exec(cmd);
}

StringArray dir_glob(const char *dir_path, const char *pattern) {
  DIR *dir;
  struct dirent *entry;
  if ((dir = opendir(dir_path)) == NULL) {
    cpm_log(CPM_ERROR, "Directory globbing failed\n");
    perror("reason:");
  }
  String res = {0};

  while ((entry = readdir(dir)) != NULL) {
    if (fnmatch(pattern, entry->d_name, 0) == 0) {
      if (!strcmp(entry->d_name, "build.c")) {
        continue;
      }
      string_append(&res, dir_path);
      string_append(&res, "/");
      cpm_cmd_append(&res, entry->d_name);
    }
  }
  free(dir);
  free(entry);
  StringArray trueres = str_split_at(res.str, " ");
  return trueres;
}

CliReturn cpm_input_check(int argc, char **argv) {
  if (argc < 2) {
    cpm_log(CPM_ERROR,
            "Invalid use\nUsage: %s [COMMAND] [FLAGS]\navailable "
            "commands\nrun\nbuild\nclean",
            argv[0]);
    exit(1);
  }
  if (!strcmp(argv[1], "clean")) {
    return CLEAN;
  } else if (!strcmp(argv[1], "build")) {
    return BUILD;

  } else if (!strcmp(argv[1], "run")) {
    return RUN;
  }

  return NONE;
}

void cpm_enable_project_variables() { UNIMPLEMENTED; }

#endif
