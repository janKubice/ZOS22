#ifndef STRUCTURE
#define STRUCTURE


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>


typedef struct superblock_s {
    char signature[13];             //login autora FS
    int disk_size;              	//celkova velikost VFS
    int cluster_size;              	//velikost clusteru
    int cluster_count;          	//pocet clusteru
    int fat_count;        	    	//pocet polozek kazde FAT tabulce
    int fat1_start_address;	    	//adresa pocatku FAT1 tabulky
    int fat2_start_address;     	//adresa pocatku FAT2 tabulky
    int data_start_address;     	//adresa pocatku datovych bloku (hl. adresar)  
}superblock_t;

typedef struct directory_item_s {
	
    char item_name[13];              		//8+3 + /0 C/C++ ukoncovaci string znak
    bool isFile;		     				//identifikace zda je soubor (TRUE), nebo adresář (FALSE)
    int size;                    		    //velikost souboru, u adresáře 0 (bude zabirat jeden blok)
    int start_cluster;           			//počáteční cluster položky
	char parent_name[13]; 					//jmeno nadřazené složky
    struct directory_item_s *next_element;
}directory_item;

typedef struct vfs_s {
    superblock_t *superblock;				//Ukazatel na superblock systemu
    int *fat_table1;						//Ukazatel na první fat tabulku
    int *fat_table2;						//Ukazatel na druhou fat tabulku
    
    char **data_block;						//Ukazatel na pole datových bloků

    directory_item *current;				//Ukazatel na aktualně zvolenou složku

    directory_item *all_elements;           //Ukazatel na prvni element spojoveho seznamu

    const char *file;					    //Ukazatel na soubor s file systémem
    
}vfs_t;
 
#endif

