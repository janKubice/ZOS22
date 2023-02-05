#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "structures.h"
#include "filesystem.h"
#include "parse_path.h"

int ls(vfs_t *vfs, directory_item *current);

int pwd(vfs_t *vfs);

int cd(vfs_t *vfs, char *name);

int mkdir(vfs_t *vfs, char *name);

int rmdir(vfs_t *vfs, char *name);

int rm(vfs_t *vfs, char *name);

int info(vfs_t *vfs, char *name);

int cat(vfs_t *vfs, char *name);

int mv(vfs_t *vfs, directory_item *one, char *name_two);

int cp(vfs_t *vfs, directory_item *one, char *name_two);

int incp(vfs_t *vfs, char *input_file, char *file_name);

int outcp(vfs_t *vfs, char *file_name, char *output_file);

int shortf(vfs_t *vfs, char *name);

int xcp(vfs_t *vfs, directory_item *one, directory_item *two, char *name);

int load(vfs_t *vfs, char *file_name);

vfs_t *format(vfs_t *vfs, int size);

int exit_fs(vfs_t *vfs, bool *continue_w);

#endif