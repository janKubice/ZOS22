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

vfs_t *create_vfs();

vfs_t *init_vfs(vfs_t *vfs, int size);

void commands_loop(vfs_t *vfs, bool *continue_w);

void destroy_vfs(vfs_t *vfs);

directory_item *search_directory(vfs_t *vfs, char * name_directory);

directory_item *search_file(vfs_t *vfs, char *name_file);

directory_item *search_parent_directory(vfs_t *vfs, char *name_directory);

directory_item *previus_dir(vfs_t *vfs, char *name_directory);

void root_folder(vfs_t *vfs);

int count_of_dir(vfs_t *vfs, char *name_directory);

int seach_free_index(vfs_t *vfs);

int next_index(vfs_t *vfs, int index);

char *path(vfs_t *vfs, char *argument);

char *path_string(vfs_t *vfs);

#endif