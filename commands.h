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

/**
 * @brief Zkopiruje soubor s1 do umisteni s2 
 * 
 * @param vfs struktura virtualniho file systemu
 * @param s nazev slozky kam chceme premistit soubor
 * @return int vysledek operace
 */
int cd(vfs *vfs, char *s);

/**
 * @brief Presune soubor s1 do umisteni s2, nebo prejmenuje s1 na s2
 * 
 * @param vfs struktura virtualniho file systemu
 * @param s1 umisteni nebo jmeno souboru 1
 * @param s2 umisteni nebo jmeno souboru 2
 * @return int vysledek operace
 */
int mv(vfs *vfs, directory_item *s1, char *s2);

/**
 * @brief Smaze soubor s
 * 
 * @param vfs struktura virtualniho file systemu
 * @param s soubor s
 * @return int int vysledek operace
 */
int rm(vfs *vfs, char *s);

int ls(vfs *vfs, directory_item *current);

int pwd(vfs *vfs);

int mkdir(vfs *vfs, char *name);

int rmdir(vfs *vfs, char *name);

int info(vfs *vfs, char *name);

int cat(vfs *vfs, char *name);

int cp(vfs *vfs, directory_item *one, char *name_two);

int incp(vfs *vfs, char *input_file, char *file_name);

int outcp(vfs *vfs, char *file_name, char *output_file);

int shortf(vfs *vfs, char *name);

int xcp(vfs *vfs, directory_item *one, directory_item *two, char *name);

int load(vfs *vfs, char *file_name);

vfs *format(vfs *vfs, int size);

int exit_fs(vfs *vfs, bool *continue_w);

#endif