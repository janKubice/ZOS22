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
 * @brief Zmeni aktualni cestu do adresare
 * 
 * @param vfs struktura virtualniho file systemu
 * @param s adresar kam se cheme presunout
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
 * @return int vysledek operace
 */
int rm(vfs *vfs, char *s);

/**
 * @brief Vypise obsah adresare, bez parametru vypise aktualni adresar
 * 
 * @param vfs struktura virtualniho file systemu
 * @param current aktualni adresar
 * @return int vysledek operace
 */
int ls(vfs *vfs, directory_item *current);

/**
 * @brief Vypise aktualni adresu
 * 
 * @param vfs struktura virtualniho file systemu
 * @return int vysledek operace
 */
int pwd(vfs *vfs);

/**
 * @brief Vytvori adresar 
 * 
 * @param vfs struktura virtualniho file systemu
 * @param current aktualni slozka
 * @return int vysledek operace
 */
int mkdir(vfs *vfs, char *name);

/**
 * @brief Smaze prazdny adresar
 * 
 * @param vfs struktura virtualniho file systemu
 * @param name nazev adresare
 * @return int vysledek operace
 */
int rmdir(vfs *vfs, char *name);

/**
 * @brief Vypise inforamce o souboru ci adresari
 * 
 * @param vfs struktura virtualniho file systemu
 * @param name cesta co chceme vypsat
 * @return int vysledek operace
 */
int info(vfs *vfs, char *name);

/**
 * @brief vysledek obsah souboru
 * 
 * @param vfs struktura virtualniho file systemu
 * @param name soubor k vypsani
 * @return int vysledek operace
 */
int cat(vfs *vfs, char *name);

/**
 * @brief Zkopiruje soubor s1 do umisteni s2 
 * 
 * @param vfs struktura virtualniho file systemu
 * @param s nazev slozky kam chceme premistit soubor
 * @return int vysledek operace
 */
int cp(vfs *vfs, directory_item *one, char *name_two);

/**
 * @brief Nahraje soubor z pevneho disku na FS
 * 
 * @param vfs struktura virtualniho file systemu
 * @param input_file co chceme nahrat
 * @param file_name kam to chcene nahrat
 * @return int vysledek operace
 */
int incp(vfs *vfs, char *input_file, char *file_name);

/**
 * @brief nahraje z soubor z FS do pevneho disku
 * 
 * @param vfs struktura virtualniho file systemu
 * @param file_name co chceme nahrat
 * @param output_file kam chcem nahrat
 * @return int vysledek operace
 */
int outcp(vfs *vfs, char *file_name, char *output_file);

/**
 * @brief Nacte soubor z pevneho disku s prikazy
 * 
 * @param vfs struktura virtualniho file systemu
 * @param file_name cesta
 * @return int vysledek operace
 */
int load(vfs *vfs, char *file_name);

/**
 * @brief Zkontroluje zda je FS v poradku
 * 
 * @param vfs struktura virtualniho file systemu
 * @return int vysledek operace
 */
int check(vfs *vfs);

/**
 * @brief Udela chybu v souboru
 * 
 * @param vfs struktura virtualniho file systemu
 * @param file soubor k poskozeni
 * @return int vysledek operace
 */
int bug(vfs *vfs, char file);

/**
 * @brief Provede format souboru
 * 
 * @param vfs struktura virtualniho file systemu
 * @param size velikost formatu
 * @return vfs* vrati ukazatel na strukturu VFS
 */
vfs *format(vfs *vfs, int size);

/**
 * @brief Ukocni FS
 * 
 * @param vfs struktura virtualniho file systemu
 * @param continue_w 
 * @return int vysledek operace
 */
int exit_fs(vfs *vfs, bool *continue_w);

#endif