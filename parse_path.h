#ifndef PARSE_PATH_H
#define PARSE_PATH_H

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "commands.h"

char *relativ_path(vfs *vfs, char *path);

char *absolutle_path(vfs *vfs, char *path);


#endif