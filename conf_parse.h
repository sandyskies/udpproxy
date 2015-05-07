#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#ifndef _COMMON_LIB_H_
#define _COMMON_LIB_H_
#include "common_lib.h"
#endif
//extern conf_t global_conf;
extern char* getKeyValue(char *filename, char *section, char *key);
extern void parse_conf(char* conf_dir, struct conf_s *cp);
