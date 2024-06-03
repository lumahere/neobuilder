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

#pragma once
#ifndef H_NCPM_BUILDER
#define H_NCPM_BUILDER
#define CPM_MAJOR_VERSION 0
#define CPM_MINOR_VERSON 4
#define CPM_PATCH_VERSION 3

#define DEFAULT_COMPILER "cc"
#define DEFAULT_CPM_PATH "."

#define UNIMPLEMENTED                                                    \
  {                                                                      \
    cpm_log(CPM_ERROR,                                                   \
            "function is not implemented yet O^O; CPM.h at line %d and " \
            "function %s\n",                                             \
            __LINE__, __func__);                                         \
    exit(EXIT_FAILURE);                                                  \
  }

#define TODO                                                                   \
  {                                                                            \
    cpm_log(CPM_ERROR,                                                         \
            "function is still being worked on; CPM.h at line %d and fuction " \
            "%s\n",                                                            \
            __LINE__, __func__);                                               \
    exit(EXIT_FAILURE);                                                        \
  }

/*WARNING: DEBUG CAUSES SO MUCH MEMORY ERRORS, DON'T INCLUDE IN RELEASES*/
#define DEBUG(var, is_string)                                                \
  {                                                                          \
    if (is_string)                                                           \
      cpm_log(CPM_MSG,                                                       \
              "DEBUG (%s in %s at line %d): float: %f, char: %c int: %d, "   \
              "hex: %x, uint: %u, address: %p, string: %s\n",                \
              #var, __FILE__, __LINE__, var, var, var, var, var, &var, var); \
    else                                                                     \
      cpm_log(CPM_MSG,                                                       \
              "DEBUG (%s in %s at line %d): float: %f, char: %c int: %d, "   \
              "hex: %x, uint: %u, address: %p\n",                            \
              #var, __FILE__, __LINE__, var, var, var, var, var, &var);      \
  }
#define KERR "\x1B[31m" // RED
#define KWAR "\x1B[33m" // YELLOW
#define KMSG "\x1B[35m" // MAGENTA
#define KNRM "\x1B[0m"  // NORMAL COLOR

/*NOT WINDOWS FRIENDLY*/
#include <dirent.h>
#include <errno.h>
#include <fnmatch.h>
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

typedef struct String
{
  size_t size;
  size_t cap;
  char *str;
} String;

typedef struct MemFile
{
  size_t size;
  void *data;
} MemFile;

typedef enum loglvl
{
  CPM_INFO,
  CPM_WARNING,
  CPM_ERROR,

  // NO TIME INFO
  CPM_MSG
} Loglevel;

typedef struct StringArray
{
  String **array;
  size_t cap;
  size_t count;

} StringArray;

typedef StringArray Arguments;

typedef struct CliCommand
{
  const char *name;
  const char *desc;
  void (*function)(Arguments); // argc, argv
} CliCommand;

typedef struct CliCommandArray
{
  CliCommand *cmds;
  int cmdcount;
  int cmdcap;
} CliCommandArray;

typedef struct CliEnv
{
  CliCommandArray cmds;
  Arguments args;
} CliEnv;

// Redefinition of String = Cmd;; OWO
typedef String Cmd;
typedef String Path;
typedef StringArray PathArray;

typedef enum CliReturn
{
  BUILD,
  RUN,
  CLEAN
} CliReturn;

#define STRING_INIT_CAPA 125 // IDK Should work?

void cpm_string_append(String *str, const char *src)
{
  if (!src)
    return;
  size_t srcsize = strlen(src);
  if (!str->str)
  {
    str->cap = STRING_INIT_CAPA;
    str->size = 0;
    str->str =
        (char *)calloc(STRING_INIT_CAPA, sizeof(char)); // Malloc cant'lloc
  }
  if (str->size + srcsize >= str->cap)
  {
    str->cap *= 2;
    str->str = (char *)realloc(str->str, str->cap);
  }
  else
  {
    str->str = strcat(str->str, src);
    str->size += srcsize;
  }
}

String cpm_string_from_cstr(const char *cstr)
{
  String res = {0};
  cpm_string_append(&res, cstr);
  return res;
}

#define cpm_string_free(string) free(string.str);

void appendcmdnull(Cmd *cmd, ...)
{
  va_list args;
  va_start(args, cmd);
  char *arg = va_arg(args, char *);
  while (arg != NULL)
  {
    cpm_string_append(cmd, arg);
    cpm_string_append(cmd, " ");
    arg = va_arg(args, char *);
  }
  va_end(args);
}

StringArray cpm_str_split_at(const char *str, const char *delimiter)
{
  StringArray res = {0};
  res.cap = 5;
  res.count = 0;
  res.array = (String **)calloc(res.cap, sizeof(String *));
  for (int i = 0; i < res.cap; i++)
  {
    res.array[i] = (String *)calloc(1, sizeof(String));
  }
  char *srcpy = strdup(str);
  char *token = strtok(srcpy, delimiter);
  while (token)
  {
    if (res.count == res.cap)
    {
      res.cap += 5;
      res.array = (String **)realloc(res.array, res.cap * sizeof(String *));
      for (int i = res.count; i < res.cap; i++)
      {
        res.array[i] = (String *)calloc(1, sizeof(String));
      }
    }
    ++res.count;
    cpm_string_append(res.array[res.count - 1], token);
    token = strtok(NULL, delimiter);
  }
  free(token);
  free(srcpy);

  return res;
}

StringArray cpm_string_array_new()
{
  StringArray res = {0};
  res.cap = 5;
  res.array = (String **)calloc(res.cap, sizeof(String));
  for (int i = 0; i < res.cap; i++)
  {
    res.array[i] = (String *)calloc(1, sizeof(String));
  }
  return res;
}

void cpm_string_array_append(StringArray *sp, const String str)
{
  if (sp->array == NULL)
  {
    sp->count = 0;
    sp->cap = 5;
    sp->array = (String **)calloc(sp->cap, sizeof(String));
    for (int i = 0; i < sp->cap; i++)
    {
      sp->array[i] = (String *)calloc(1, sizeof(String));
    }
  }
  if (sp->count == sp->cap)
  {
    sp->cap += 5;
    sp->array = (String **)realloc(sp->array, sp->cap * sizeof(String *));
    for (int i = sp->count; i < sp->cap; i++)
    {
      sp->array[i] = (String *)calloc(1, sizeof(String));
    }
  }
  cpm_string_append(
      sp->array[sp->count],
      str.str); // STRDUP REMOVED, IF SIGFAULT THEN ADD STRDUP BACK;
  sp->count++;
}

String cpm_string_copy(String *other)
{
  String res = cpm_string_from_cstr(other->str);
  return res;
}

void cpm_string_array_append_cstr(StringArray *sp, const char *cstr)
{
  String src = cpm_string_from_cstr(cstr);
  cpm_string_array_append(sp, src);
  cpm_string_free(src);
}

void cpm_string_array_free(StringArray str)
{
  for (int a = 0; a < str.count; a++)
  {
    free(str.array[a]->str);
  }
  for (int a = 0; a < str.cap; a++)
  {
    free(str.array[a]);
  }
  free(str.array);
}
String cpm_string_array_join(const StringArray *src, const char *delimiter)
{
  String dest = {0};
  for (int i = 0; i < src->count; i++)
  {
    cpm_string_append(&dest, src->array[i]->str);
    cpm_string_append(&dest, delimiter);
  }
  return dest;
}

String _INTERNAL_cpm_string_from_cstr_fmt(const char *fmt, ...)
{
  va_list arg;
  va_start(arg, fmt);
  char *tmp = (char *)malloc(strlen(fmt) + 1);
  vsnprintf(tmp, strlen(fmt), fmt, arg);
  String res = cpm_string_from_cstr(tmp);
  va_end(arg);
  free(tmp);
  return res;
}

#define cpm_string_from_cstr_fmt(cstr, ...) \
  _INTERNAL_cpm_string_from_cstr_fmt(cstr, __VA_ARGS__, NULL)

Path cpm_path_from_cstr_unchecked(const char *cstr)
{
  Path res = cpm_string_from_cstr(cstr);
  return res;
}

String cpm_path_get_destination(const Path *pth)
{
  StringArray split = cpm_str_split_at(pth->str, "/");
  String end = cpm_string_from_cstr(split.array[split.count - 1]->str);
  cpm_string_array_free(split);
  return end;
}

void cpm_path_append_cstr(Path *pth, const char *dest)
{
  cpm_string_append(pth, "/");
  cpm_string_append(pth, dest);
}
#define cpm_path_array_new() cpm_string_array_new()
#define cpm_path_array_append(PathArrayptr, Path) \
  cpm_string_array_append(PathArrayptr, Path)

#define cpm_cmd_append(cmdptr, ...) appendcmdnull(cmdptr, __VA_ARGS__, NULL)

void cpm_log(Loglevel lvl, const char *fmt, ...)
{
  String msg = {0};
  time_t time_now;
  struct tm *time_info;
  time(&time_now);
  time_info = localtime(&time_now);
  char timenow[128];
  sprintf(timenow, "(%02d:%02d:%02d)", time_info->tm_hour, time_info->tm_min,
          time_info->tm_sec);
  switch (lvl)
  {
  case CPM_INFO:
    cpm_string_append(&msg, KMSG);
    cpm_string_append(&msg, "LOG");
    cpm_string_append(&msg, " ");
    cpm_string_append(&msg, timenow);
    cpm_string_append(&msg, ":");
    cpm_string_append(&msg, " ");
    fprintf(stdout, "%s", msg.str);
    fflush(stdout);
    break;
  case CPM_WARNING:
    cpm_string_append(&msg, KWAR);
    cpm_string_append(&msg, "WARNING");
    cpm_string_append(&msg, " ");
    cpm_string_append(&msg, timenow);
    cpm_string_append(&msg, ":");
    cpm_string_append(&msg, " ");
    fprintf(stdout, "%s", msg.str);
    fflush(stdout);
    break;
  case CPM_ERROR:
    cpm_string_append(&msg, KERR);
    cpm_string_append(&msg, "ERROR");
    cpm_string_append(&msg, " ");
    cpm_string_append(&msg, timenow);
    cpm_string_append(&msg, ":");
    cpm_string_append(&msg, " ");
    fprintf(stderr, "%s", msg.str);
    fflush(stderr);
    break;
  case CPM_MSG:
    cpm_string_append(&msg, KMSG);
    fprintf(stdout, "%s", msg.str);
    fflush(stdout);
    break;
  }

  va_list args;
  va_start(args, fmt);
  vfprintf(stdout, fmt, args);
  va_end(args);
  fprintf(stdout, "%s", KNRM);
  cpm_string_free(msg);
}

Path cpm_path_from_cstr(const char *cstr)
{
  // does have '/' but dont have spaces
  if (strrchr(cstr, '/') != NULL && strchr(cstr, ' ') == NULL)
  {

    Path res = cpm_string_from_cstr(cstr);
    return res;
  }
  cpm_log(CPM_ERROR, "Path: %s is an invalid path", cstr);
  exit(1);
}

void cpm_cmd_exec(Cmd cmd)
{
  // cpm_log(CPM_INFO, "%s\n", cmd.str);
  if (system(cmd.str))
    cpm_log(CPM_ERROR, "cmd: %s failed to execute\n", cmd.str);
  free(cmd.str);
}
// compares file1 against file2 returning true if file1 is newer than file2
bool cmp_modtime(const char *file1, const char *file2)
{
  struct stat oneInfo, twoInfo;
  if (-1 == stat(file1, &oneInfo))
  {
    cpm_log(CPM_ERROR, "Failed to stat file1 at %d\n", __LINE__);
    perror("stat: ");
    exit(-1);
  }
  if (-1 == stat(file2, &twoInfo))
  {
    cpm_log(CPM_ERROR, "Failed to stat file2 at %d\n", __LINE__);
    perror("stat: ");
    exit(-1);
  }
  return oneInfo.st_mtime > twoInfo.st_mtime;
}

#define GETFILENAME(name) (strrchr(name, '/') ? strrchr(name, '/') + 1 : name)

// FILE OPS
bool cpm_file_exists(const char *file_path)
{
  FILE *file;
  file = fopen(file_path, "rb");
  if (file == NULL)
  {
    return false;
  }
  fclose(file);
  return true;
}

MemFile cpm_load_file_to_mem(const char *file_path)
{
  MemFile file = {0};
  FILE *actfile = fopen(file_path, "rb");
  if (actfile != 0)
  {
    fseek(actfile, 0, SEEK_END);
    file.size = ftell(actfile);
    fseek(actfile, 0, SEEK_SET);
  }
  else
  {
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

void cpm_compile(Cmd compilation_cmd)
{

  StringArray tokens = cpm_str_split_at(compilation_cmd.str, " ");
  StringArray srcs = cpm_string_array_new();
  String output = {0};
  // sum1code was here
  for (int i = 0; i < tokens.count; i++)
  {
    if (strstr(tokens.array[i]->str, ".c") != NULL)
    {
      cpm_string_array_append(&srcs, *tokens.array[i]);
    }
    if (strstr(tokens.array[i]->str, ".o") != NULL)
    {
      cpm_string_array_append(&srcs, *tokens.array[i]);
    }

    if (!strcmp(tokens.array[i]->str, "-o"))
    {
      cpm_string_append(&output, tokens.array[i + 1]->str);
    }
  }

  if (srcs.count == 0 || output.size == 0)
  {
    cpm_log(CPM_WARNING, "cannot grab input or output from command \"%s\"\n",
            compilation_cmd.str);
    cpm_log(CPM_WARNING, "Will not track dependencies of \"%s\"\n",
            compilation_cmd.str);
    cpm_cmd_exec(compilation_cmd);

    cpm_string_array_free(tokens);
    cpm_string_array_free(srcs);
    return;
  }
  if (!cpm_file_exists(output.str))
  {
    cpm_log(CPM_INFO, "compiling %s => %s\n", srcs.array[0]->str,
            compilation_cmd.str);
    cpm_cmd_exec(compilation_cmd);
    return;
  }
  for (int i = 0; i < srcs.count; i++)
  {
    if (cmp_modtime(srcs.array[i]->str, output.str))
    {
      cpm_log(CPM_INFO, "recompiling %s => %s\n", output.str,
              compilation_cmd.str);
      cpm_cmd_exec(compilation_cmd);
      cpm_string_array_free(tokens);
      cpm_string_array_free(srcs);
      return;
    }
    else
    {
      cpm_log(CPM_INFO, "Skipping compilation: %s\n", compilation_cmd.str);
      return;
    }
  }
}

// RETURNS PID OF CHILD
int cpm_compile_async(Cmd compile_command)
{
  int child = fork();
  if (child == -1)
  {
    cpm_log(CPM_WARNING, "Could not fork process for async compilation, "
                         "defaulting to non-async compilation\n");
    perror("At fork: ");
    cpm_compile(compile_command);
  }
  else if (child == 0)
  {
    cpm_compile(compile_command);
    return child;
  }
  return 0;
}

// POLLS ASYNC COMPILATION AND AWAITS IT
void cpm_compile_poll(int compile_index)
{
  int status;
  if (compile_index != 0)
  {
    if (waitpid(compile_index, &status, 0) == -1)
      cpm_log(CPM_ERROR, "waitpid error\n");
    perror("error at waitpid: ");
    if (WIFSIGNALED(status))
    {
      cpm_log(CPM_ERROR, "Compilation terminated by signal: %d\n",
              WTERMSIG(status));
    }
  }
}

void cpm_cp(const char *srcpath, const char *destpath)
{
  Cmd cmd = {0};
  cpm_cmd_append(&cmd, "cp", "-r", srcpath, destpath);
  cpm_cmd_exec(cmd);
}

void cpm_rm(const char *path)
{
  Cmd cmd = {0};
  cpm_cmd_append(&cmd, "rm", "-rf", path);
  cpm_cmd_exec(cmd);
}
void cpm_mv(const char *srcpath, const char *destpath)
{
  Cmd cmd = {0};
  cpm_cmd_append(&cmd, "mv", srcpath, destpath);
  cpm_cmd_exec(cmd);
}

// Support makefiles and build.c, Arguments are freed when entered, arguments
// can be NULL
void cpm_submodule(const char *path_to_folder, Arguments *args)
{
  cpm_log(CPM_INFO,
          "========================BEGIN "
          "SUBMODULE %s =============================\n\n",
          path_to_folder);
  Path cpmpath = cpm_path_from_cstr_unchecked(path_to_folder);
  cpm_path_append_cstr(&cpmpath, "cpm.h");
  String makefile = {0};
  cpm_string_append(&makefile, path_to_folder);
  cpm_string_append(&makefile, "/");
  cpm_string_append(&makefile, "makefile");
  String Makefile = {0};
  cpm_string_append(&Makefile, path_to_folder);
  cpm_string_append(&Makefile, "/");
  cpm_string_append(&Makefile, "Makefile");
  String buildc = {0};
  cpm_string_append(&buildc, path_to_folder);
  cpm_string_append(&buildc, "/");
  cpm_string_append(&buildc, "build.c");

  if (cpm_file_exists(buildc.str))
  {
    cpm_log(CPM_INFO, "Building submodule %s => build.c\n", path_to_folder);
    cpm_cp("./cpm.h", path_to_folder);
    Cmd cmd = {0};
    Cmd run = {0};
    cpm_cmd_append(&cmd, DEFAULT_COMPILER, buildc.str, "-o");
    cpm_string_append(&cmd, path_to_folder);
    cpm_string_append(&cmd, "/buildscript ");
    cpm_cmd_append(&cmd, "-I", ".");
    cpm_compile(cmd);

    cpm_cmd_append(&run, "cd", path_to_folder, "&&");
    cpm_cmd_append(&run, "./buildscript");
    if (args != NULL)
    {
      for (int i = 0; i < args->count; i++)
      {
        cpm_cmd_append(&cmd, args->array[i]->str);
      }
    }
    cpm_cmd_exec(run);
    if (args != NULL)
      cpm_string_array_free(*args);
  }
  else if (cpm_file_exists(makefile.str) || cpm_file_exists(Makefile.str))
  {
    cpm_log(CPM_INFO, "Building submodule %s => makefile\n", path_to_folder);
    Cmd cmd = {0};
    cpm_cmd_append(&cmd, "cd", path_to_folder, "&&", "make");
    if (args != NULL)
    {
      for (int i = 0; i < args->count; i++)
      {
        cpm_cmd_append(&cmd, args->array[i]->str);
      }
    }
    cpm_cmd_exec(cmd);
    if (args != NULL)
      cpm_string_array_free(*args);
  }
  else
  {
    cpm_log(CPM_ERROR, "submodule \"%s\" doesn't have a makefile or build.c\n",
            path_to_folder);
  }
  cpm_string_free(Makefile);
  cpm_string_free(makefile);
  cpm_string_free(buildc);
  cpm_rm(cpmpath.str);
  cpm_string_free(cpmpath);
  cpm_log(
      CPM_INFO,
      "========================END SUBMODULE=============================\n\n");
}

void cpm_mkdir(const char *dirpath)
{
  Cmd cmd = {0};
  cpm_cmd_append(&cmd, "mkdir", "-p", dirpath);
  cpm_cmd_exec(cmd);
}

int cpm_strcmp(const char *str1, const char *str2)
{
  return !strcmp(str1, str2);
}

PathArray cpm_dir_glob(const char *dir_path, const char *pattern)
{
  DIR *dir;
  struct dirent *entry;
  if ((dir = opendir(dir_path)) == NULL)
  {
    cpm_log(CPM_ERROR, "Directory globbing failed\n");
    perror("reason:");
  }
  String res = {0};

  while ((entry = readdir(dir)) != NULL)
  {
    if (fnmatch(pattern, entry->d_name, 0) == 0)
    {
      if (!strcmp(entry->d_name, "build.c"))
      {
        continue;
      }
      cpm_string_append(&res, dir_path);
      cpm_string_append(&res, "/");
      cpm_cmd_append(&res, entry->d_name);
    }
  }
  free(dir);
  free(entry);
  StringArray trueres = cpm_str_split_at(res.str, " ");
  return trueres;
}

Arguments _INTERNAL_cpm_Cargs2cpm_args(int argc, char **argv)
{
  Arguments args = {0};
  for (int i = 1; i < argc; i++)
  {
    cpm_string_array_append_cstr(&args, argv[i]);
  }
  return args;
}

CliCommand cpm_create_cli_command(const char *name, const char *desc,
                                  void (*function)(Arguments))
{
  CliCommand newcmd = {0};
  newcmd.name = name;
  newcmd.desc = desc;
  newcmd.function = function;
  return newcmd;
}

void cpm_append_cli_cmd_arr(CliCommandArray *arr, CliCommand cmd)
{
  if (!arr->cmds)
  {
    arr->cmdcount = 0;
    arr->cmdcap = 5;
    arr->cmds = (CliCommand *)calloc(arr->cmdcap, sizeof(CliCommand));
  }
  if (arr->cmdcount == arr->cmdcap)
  {
    arr->cmdcap += 5;
    arr->cmds =
        (CliCommand *)realloc(arr->cmds, arr->cmdcap * sizeof(CliCommand));
  }
  arr->cmds[arr->cmdcount] = cmd;
  arr->cmdcount++;
}

CliEnv cpm_create_cliEnv_Cargs(int argc, char **argv)
{
  Arguments args = _INTERNAL_cpm_Cargs2cpm_args(argc, argv);
  CliEnv env = {0};
  CliCommandArray arr = {0};
  env.cmds = arr;
  env.args = args;
  return env;
}

CliEnv cpm_create_cliEnv(Arguments args)
{
  CliEnv env = {0};
  CliCommandArray arr = {0};
  env.cmds = arr;
  env.args = args;
  return env;
}

void cpm_append_env_commands(CliEnv *env, CliCommand cmd)
{
  cpm_append_cli_cmd_arr(&env->cmds, cmd);
};

void cpm_free_env(CliEnv env)
{
  cpm_string_array_free(env.args);
  free(env.cmds.cmds);
}
void cpm_append_arguments(Arguments *args, const char *cstr)
{
  cpm_string_array_append_cstr(args, cstr);
}

#ifndef CPM_NO_INTERACTIVE
void cpm_CLI(CliEnv env)
{
  char input[31] = {0};
  bool interactive = false;
  if (env.args.count < 1)
  {
    cpm_log(CPM_WARNING,
            "No initial argument, running in interactive mode...\n");
    cpm_log(CPM_MSG, "Entering CLI Mode\nCPM CLI V.1.1\n");
    cpm_log(CPM_MSG, "type \"help\" to print all available commands\n");
    interactive = true;
    while (interactive)
    {
      Arguments interactive_args = {0};
      printf("> ");
      char *res = fgets(input, 30, stdin);
      input[strcspn(input, "\n")] = 0;
      if (res != NULL)
      {
        StringArray input_sliced = cpm_str_split_at(input, " ");
        for (int i = 0; i < input_sliced.count; i++)
        {
          cpm_string_array_append(&interactive_args, *input_sliced.array[i]);
        }

        if (!strcmp(input, "help"))
        {
          printf("- help: prints this message\n");
          for (int i = 0; i < env.cmds.cmdcount; i++)
          {
            printf("- %s: %s\n", env.cmds.cmds[i].name, env.cmds.cmds[i].desc);
          }
          printf("- exit: exit the CLI (CTRL-D)\n");
        }
        if (!strcmp(input, "exit"))
        {
          free(env.cmds.cmds);
          exit(EXIT_SUCCESS);
        }
        for (int i = 0; i < env.cmds.cmdcount; i++)
        {
          if (!strcmp(input_sliced.array[0]->str, env.cmds.cmds[i].name))
          {
            env.cmds.cmds[i].function(interactive_args); // cmd execution
            cpm_string_array_free(interactive_args);
            cpm_string_array_free(input_sliced);
            break;
          }
        }
      }
      else
      {
        cpm_free_env(env);
        exit(EXIT_SUCCESS);
      }
    }
  }
  else
  {
    if (!strcmp(env.args.array[0]->str, "help"))
    {
      printf("- help: prints this message\n");
      for (int i = 0; i < env.cmds.cmdcount; i++)
      {
        printf("- %s: %s\n", env.cmds.cmds[i].name, env.cmds.cmds[i].desc);
      }
    }
    for (int i = 0; i < env.cmds.cmdcount; i++)
    {
      if (!strcmp(env.args.array[0]->str, env.cmds.cmds[i].name))
      {
        env.cmds.cmds[i].function(env.args);
        break;
      }
    }
    cpm_free_env(env);
    exit(EXIT_SUCCESS);
  }
}
#else
void cpm_CLI(CliEnv env)
{
  cpm_log(CPM_WARNING, "Interactive CLI has been desabled by #CPM_NO_CLI, "
                       "run with help to print available cmds\n");

  if (env.args.count < 1)
  {
    exit(1);
  }
  if (!strcmp(env.args.array[0]->str, "help"))
  {
    printf("- help: prints this message\n");
    for (int i = 0; i < env.cmds.cmdcount; i++)
    {
      printf("- %s: %s\n", env.cmds.cmds[i].name, env.cmds.cmds[i].desc);
    }
  }
  for (int i = 0; i < env.cmds.cmdcount; i++)
  {
    if (!strcmp(env.args.array[0]->str, env.cmds.cmds[i].name))
    {
      env.cmds.cmds[i].function(env.args);
      break;
    }
  }
  exit(EXIT_SUCCESS);
}
#endif

#define CPM_REBUILD_SELF(argc, argv)                               \
  {                                                                \
    if (cmp_modtime(__FILE__, argv[0]))                            \
    {                                                              \
                                                                   \
      Arguments NWargs = _INTERNAL_cpm_Cargs2cpm_args(argc, argv); \
      Cmd Irebuild = {0};                                          \
      cpm_cmd_append(&Irebuild, DEFAULT_COMPILER);                 \
      cpm_cmd_append(&Irebuild, __FILE__, "-o", argv[0]);          \
      cpm_compile(Irebuild);                                       \
      Cmd Irerun = {0};                                            \
      cpm_cmd_append(&Irerun, argv[0]);                            \
      String initargstr = cpm_string_array_join(&NWargs, " ");     \
      cpm_cmd_append(&Irerun, initargstr.str);                         \
      cpm_cmd_exec(Irerun);                                        \
      exit(0);                                                     \
    }                                                              \
  }
#endif
