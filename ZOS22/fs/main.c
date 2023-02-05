#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "structures.h"
#include "filesystem.h"


int main(int argc, char const *argv[])
{
	
	FILE *fp = NULL;
    vfs_t *vfs = NULL;
    char *input_argument = NULL;
    size_t len_a = 0, len_r = 0;
    char *unit = NULL;
    int size = 0;
    bool continue_w = true;

    directory_item *current = NULL;
	
    if(argc == 2) {

        vfs = create_vfs();
        if(vfs != NULL) {

            if((fp = fopen(argv[1],"rb"))!=NULL) {

                fread(vfs->superblock, sizeof(superblock_t), 1, fp);
                vfs->fat_table1 = (int *)malloc(sizeof(int) * vfs->superblock->fat_count);
                fread(vfs->fat_table1, sizeof(int), vfs->superblock->fat_count, fp);
                vfs->fat_table2 = (int *)malloc(sizeof(int) * vfs->superblock->fat_count);
                fread(vfs->fat_table2, sizeof(int), vfs->superblock->fat_count, fp);
                vfs->data_block = (char **)malloc(vfs->superblock->fat_count * sizeof(char *));
                for (int i = 0; i < vfs->superblock->fat_count; i++)
                {
                    vfs->data_block[i] = (char *)malloc(sizeof(char) * CLUSTER_SIZE);
                    memset(vfs->data_block[i], 0, sizeof(char) * CLUSTER_SIZE);
                    fread(vfs->data_block[i], sizeof(char), CLUSTER_SIZE, fp);
                }
                
                vfs->all_elements = (directory_item *)malloc(sizeof(directory_item));
                len_r = fread(vfs->all_elements, sizeof(directory_item), 1, fp);
                vfs->all_elements->next_element = NULL;
                vfs->current = vfs->all_elements;
                current = vfs->all_elements;
                while (len_r == 1)
                {
                    current->next_element = (directory_item *)malloc(sizeof(directory_item));
                    len_r = fread(current->next_element, sizeof(directory_item), 1, fp);
                    current = current->next_element;
                }
                
                /*while (direc_read)
                {
                    direc_read = (directory_item *)malloc(sizeof(directory_item));
                    fread(direc_read, sizeof(directory_item), 1, fp);
                    next = vfs->all_elements->next_element;
                    while (next)
                    {
                        next = next->next_element;
                    }
                    next->next_element = direc_read;
                }*/
                
                


                /*fclose(fp);
                printf("Soubor s filesystemem nebyl nalezen,spustime funkci format pro tvorbu filesystemu\n");
                printf("Velikost jednosho clusteru je 5000B\n");
                printf("Zadejte velikost disku ve formatu <velikost> <MB,GB>: ");
                getline(&input_argument, &len_a, stdin);
                for (size_t i = 0; i < strlen(input_argument); i++)
                {
                    if((input_argument[i] == 'M') && (input_argument[i + 1] == 'B')) {
                        unit = (char *)malloc(sizeof(char) * i);
                        size = atoi(strncpy(unit, input_argument, i));
                        size = size * 1000000;
                        break;
                    }
                    if((input_argument[i] == 'G') && (input_argument[i + 1] == 'B')) {
                        unit = (char *)malloc(sizeof(char) * i);
                        size = atoi(strncpy(unit, input_argument, i));
                        size = size * 1000000000;
                        break;
                    }
                }
                
                free(unit);
                vfs = init_vfs(vfs, size);
                if(!vfs) {
                    return -1;
                }*/
                vfs->file = argv[1];

                while(continue_w) {

                    commands_loop(vfs, &continue_w);
                }
        
            } else {
                printf("Soubor s filesystemem nebyl nalezen,spustime funkci format pro tvorbu filesystemu\n");
                printf("Velikost jednosho clusteru je 5000B\n");
                printf("Zadejte velikost disku ve formatu <velikost> <MB,GB>: ");
                getline(&input_argument, &len_a, stdin);
                for (size_t i = 0; i < strlen(input_argument); i++)
                {
                    if((input_argument[i] == 'M') && (input_argument[i + 1] == 'B')) {
                        unit = (char *)malloc(sizeof(char) * i);
                        size = atoi(strncpy(unit, input_argument, i));
                        size = size * 1000000;
                        break;
                    }
                    if((input_argument[i] == 'G') && (input_argument[i + 1] == 'B')) {
                        unit = (char *)malloc(sizeof(char) * i);
                        size = atoi(strncpy(unit, input_argument, i));
                        size = size * 1000000000;
                        break;
                    }
                }
                
                free(unit);
                /*if(strcmp(unit, "MB") == 0){
                    size = size * 1000000;
                } else if(strcmp(unit, "GB") == 0) {
                    size = size * 1000000000;
                }*/
                
                vfs = init_vfs(vfs, size);

                if(vfs != NULL) {
                    printf("OK\n");
                    vfs->file = argv[1];

                    while(continue_w) {

                        commands_loop(vfs, &continue_w);
                    }
                    return 0;
                } else {
                    printf("CANNOT CREATE FILE\n");
                    return -1;
                }
                
            }
        } else {
            printf("CANNOT CREATE FILE\n");
             return -1;
        }

    } else {
        printf("Nebyl zadan pozadovany pocet vstupnich argumentu");
        return 1;
    }
}
