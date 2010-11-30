#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>

#include "utils.h"

extern void str_truncate(char* str, int length) {
  if(strlen(str)>(unsigned)length)
    str[length] = 0;
}

extern void str_formatOnlyAlphaAndUnderscore(char* str) {
  int n = strlen(str);
  while(n-->0)
    if(!isalnum(str[n]))
      str[n] = '_';
}

extern char* str_trim(char *s) {
    while(isspace(*s)) s++;
    char* last;
    for (last = s+strlen(s)-1; (last >= s) && isspace(*last); --last);
    last[1] = '\0';
    return s;
}


int util_abs(int n) {
  return n<0 ? -n : n;
}

static void consumeArg(int index, char * argv[]) {
  *argv[index] = '\0';
}

extern int util_containsArg(int argc, char * argv[], char* key) {
  int i;
  for(i=1; i<argc; ++i)
    if(strcmp(argv[i], key)==0)
      return 1;
  return 0;
}

extern int util_getArgValue(int argc, char * argv[], char * key, int * value) {
  int i, ret = 0;
  int val;
  char *endptr, *str;
  for(i=1; i<argc-1; ++i) { // don't check the last arg because there is no value after it
    if(strcmp(argv[i], key)==0) { // key match
      errno = 0;
      str = argv[i+1];
      val = strtol(str, &endptr, 10);
      if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0))
        ret = 3;
      if (endptr == str) // No digits were found
        ret = 2;
      consumeArg(i, argv);
      if(ret) return ret;
      consumeArg(i+1, argv);
      *value = val;
      return 0;
    }
  }
  return 1;
}

extern int util_getArgString(int argc, char * argv[], char * key, char** value) {
  int i;
  char* val;
  for(i=1; i<argc-1; ++i) { // don't check the last arg because there is no value after it
    if(strcmp(argv[i], key)==0) { // key match
      errno = 0;
      val = malloc(strlen(argv[i+1])*sizeof(char)+1);
      strcpy(val, argv[i+1]);
      consumeArg(i, argv);
      consumeArg(i+1, argv);
      *value = val;
      return 0;
    }
  }
  return 1;
}
