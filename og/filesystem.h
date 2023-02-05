#ifndef FILESYSTEM
#define FILESYSTEM


#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>


#include "structures.h"
#include "constant.h"
#include "commands.h"

vfs *create_vfs();

vfs *init_vfs(vfs *vfs, int size);

void commands_loop(vfs *vfs, bool *continue_w);

void destroy_vfs(vfs *vfs);

directory_item *search_directory(vfs *vfs, char * name_directory);

directory_item *search_file(vfs *vfs, char *name_file);

directory_item *search_parent_directory(vfs *vfs, char *name_directory);

directory_item *previus_dir(vfs *vfs, char *name_directory);

void root_folder(vfs *vfs);

int count_of_dir(vfs *vfs, char *name_directory);

int seach_free_index(vfs *vfs);

int next_index(vfs *vfs, int index);

char *path(vfs *vfs, char *argument);

char *path_string(vfs *vfs);

#endif