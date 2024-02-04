/*
 *

Copyright 2024 Nathanael Frederic Adrean

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
may be used to endorse or promote products derived from this software without
specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * */

#ifndef H_NCPM_BUILDER
#define H_NCPM_BUILDER

#include <time.h>
#define KERR "\x1B[31m" // RED
#define KWAR "\x1B[33m" // YELLOW
#define KMSG "\x1B[35m" // MAGENTA
#define KNRM "\x1B[0m"  // NORMAL COLOR

#include <stdarg.h>
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

// compares file1 against file2
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

#define CPM_REBUILD_SELF(argv)                                                 \
                                                                               \
  if (cmp_modtime(__FILE__, argv[0])) {                                        \
    Cmd self_changename_ = {0};                                                \
    Cmd self_rebuild_ = {0};                                                   \
    cpm_cmd_append(&self_changename_, "mv");                                   \
    cpm_cmd_append(&self_changename_, argv[0]);                                \
    string_append(&self_changename_, argv[0]);                                 \
    string_append(&self_changename_, ".old");                                  \
  }

#endif
