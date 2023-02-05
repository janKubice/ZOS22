#include "filesystem.h"

vfs_t *create_vfs() {

    superblock_t *sb = NULL;
    vfs_t *vfs = NULL;

    sb = (superblock_t *)malloc(sizeof(superblock_t));
    if(!sb) {
        printf("Nebylo mozne vytvorit strukturu superbloku");
        return NULL;
    }

    vfs = (vfs_t *)malloc(sizeof(vfs_t));
    if(!vfs) {
        printf("Nebylo mozne vytvorit strukturu virutalni filesystemu");
        free(sb);
        return NULL;
    }

    vfs->superblock = sb;
    vfs->fat_table1 = NULL;
    vfs->fat_table2 = NULL;
    vfs->data_block = NULL;
    vfs->current = NULL;
    vfs->file = NULL;


    return vfs;
}

vfs_t *init_vfs(vfs_t *vfs, int size) {

    int *fat_table1 = NULL;
    int *fat_table2 = NULL;
    char **data_block = NULL;
    directory_item *root = NULL;
    directory_item *file = NULL;
    
    if((size / CLUSTER_SIZE) <= 0) {
        printf("Nebyla zadana minimalni velikost disku\n");
        free(vfs->superblock);
        free(vfs);
        return NULL;    
    }
    strcpy(vfs->superblock->signature, "Daniel Cifka");
    vfs->superblock->disk_size = size;
    vfs->superblock->cluster_size = CLUSTER_SIZE;
    vfs->superblock->cluster_count = (size / CLUSTER_SIZE);
    vfs->superblock->fat_count = (size / CLUSTER_SIZE);
    vfs->superblock->fat1_start_address = sizeof(superblock_t);
    vfs->superblock->fat2_start_address = vfs->superblock->fat1_start_address + (vfs->superblock->fat_count * sizeof(int));
    vfs->superblock->data_start_address = vfs->superblock->fat2_start_address + (vfs->superblock->fat_count * sizeof(int));

    fat_table1 = (int *)malloc(vfs->superblock->fat_count * sizeof(int));
    if(fat_table1 == NULL) {
        printf("Nastala chyba");
        free(vfs->superblock->signature);
        free(vfs->superblock);
        free(vfs);
        return NULL;
    }
    fat_table2 = (int *)malloc(vfs->superblock->fat_count * sizeof(int));
    if(fat_table2 == NULL) {
        printf("Nastala chyba");
        free(fat_table1);
        free(vfs->superblock->signature);
        free(vfs->superblock);
        free(vfs);
        return NULL;
    }

    for (int i = 0; i < vfs->superblock->fat_count; i++) {

        fat_table1[i] = FAT_UNUSED;
        fat_table2[i] = FAT_UNUSED;
    }
   
    data_block = (char **)malloc(vfs->superblock->fat_count * sizeof(char *));
    if(data_block == NULL) {
        return NULL;
    }
    memset(data_block, 0, vfs->superblock->fat_count * sizeof(char *));
    
    for (int i = 0; i < vfs->superblock->fat_count; i++)
    {
        data_block[i] = (char *)malloc(sizeof(char) * CLUSTER_SIZE);
        if(data_block[i] == NULL) {
            return NULL;
        }
        memset(data_block[i], 0, sizeof(char) * CLUSTER_SIZE);
    }
    

    root = (directory_item *)malloc(sizeof(directory_item));
    if(root == NULL) {
        printf("Nastala chyba");
        free(fat_table2);
        free(fat_table1);
        free(vfs->superblock->signature);
        free(vfs->superblock);
        free(vfs);
        return NULL;
    }


    
    fat_table1[0] = FAT_FILE_END;
    fat_table2[0] = FAT_FILE_END;

    strcpy(root->item_name, "root");
    root->isFile = false;
    root->size = 0;
    root->start_cluster = 0;
    root->next_element = NULL;
    strcpy(root->parent_name, root->item_name);

    file = (directory_item *)malloc(sizeof(directory_item));
    if(file == NULL) {
        return NULL;
    }
    strcpy(file->item_name, "aaaa.txt");
    file->isFile = true;
    file->size = 9000;
    file->start_cluster = 1;
    strcpy(file->parent_name, root->item_name);

    fat_table1[file->start_cluster] = 5;
    fat_table1[fat_table1[file->start_cluster]] = FAT_FILE_END;

    memset(data_block[file->start_cluster], 'A', sizeof(char) * CLUSTER_SIZE);
    data_block[file->start_cluster][5] = ' ';
    memset(data_block[fat_table1[file->start_cluster]], 'B', sizeof(char) * 4000);

    root->next_element = file;

    file = NULL;

    file = (directory_item *)malloc(sizeof(directory_item));
    if(file == NULL) {
        return NULL;
    }
    strcpy(file->item_name, "dada");
    file->isFile = false;
    file->size = 0;
    file->start_cluster = 2;
    strcpy(file->parent_name, root->item_name);

    fat_table1[file->start_cluster] = FAT_FILE_END;

    root->next_element->next_element = file;
    root->next_element->next_element->next_element = NULL;

    file = NULL;

    vfs->fat_table1 = fat_table1;
    vfs->fat_table2 = fat_table2;
    vfs->data_block = data_block;
    vfs->current = root;
    vfs->all_elements = root;


    return vfs;
}

void destroy_vfs(vfs_t *vfs) {
	
	free(vfs->superblock);
	free(vfs->fat_table1);
	free(vfs->fat_table2);
	free(vfs->current);
	free(vfs);
}

void commands_loop(vfs_t *vfs, bool *continue_w) {

    char *input_command = NULL;
    char *replace_string = NULL;
    char *command = NULL;
    char *argument1 = NULL;
    char *argument2 = NULL;
    char *argument3 = NULL;
    char *result = NULL;
    size_t len_c = 0;
    size_t len_replace_string = 0;
    directory_item *current_folder = NULL;
    directory_item *now = NULL;
    directory_item *one = NULL;
    directory_item *two = NULL;

    printf("%s>", path_string(vfs));
    getline(&input_command, &len_c, stdin);
    len_replace_string = strlen(input_command);
    replace_string = (char *)malloc(sizeof(char) * len_replace_string);
    memset(replace_string, 0, sizeof(char) * len_replace_string);
    strncpy(replace_string, input_command, len_replace_string - 1);
    current_folder = vfs->current;
    if(strncmp(replace_string, "pwd", len_replace_string - 1) == 0) {
        pwd(vfs);
    } else if(strncmp(replace_string, "ls", len_replace_string - 1) == 0) {
        ls(vfs, vfs->current);
    } else if(strncmp(replace_string, "exit", len_replace_string - 1) == 0){
        exit_fs(vfs, continue_w);
    } else {

        command = strtok(replace_string, " ");
        if(strcmp(command, "mkdir") == 0) {

            argument1 = strtok(NULL, "\0");
            result = path(vfs, argument1);
            if(strcmp(result, "NPF") == 0) {
                printf("PATH NOT FOUND (neexistuje zadaná cesta)\n");
            } else if(search_directory(vfs, result) != NULL) {
                printf("EXIST (nelze založit, již existuje)\n");
            } else if(search_file(vfs, result) != NULL) {
                printf("EXIST (nelze založit, již existuje)\n");
            } else {
                mkdir(vfs, result);
                printf("OK\n");
            }

            vfs->current = current_folder;
            

        } else if(strcmp(command,"cd") == 0) {

            argument1 = strtok(NULL, "\0");
            result = path(vfs ,argument1);
            
            if(strcmp(result, "NPF") == 0) {
                printf("PATH NOT FOUND (neexistující cesta)\n");
                vfs->current = current_folder;
            } else if(strcmp(result, "..") == 0) {
                cd(vfs, result);
            } else if(strcmp(result, ".") == 0) {

            } else if(search_directory(vfs, result) == NULL) {
                printf("PATH NOT FOUND (neexistující cesta)\n");
                vfs->current = current_folder;
            } else {
                cd(vfs, result);
            }

        } else if(strcmp(command, "rmdir") == 0) {

            argument1 = strtok(NULL, "\0");
            result = path(vfs ,argument1);
            if(strcmp(result, "NPF") == 0) {
                printf("FILE NOT FOUND (neexistující adresář)\n");
            } else if(search_directory(vfs, result) == NULL) {
                printf("FILE NOT FOUND (neexistující adresář)\n");
            } else {
                if(rmdir(vfs, result) == 0) {
                    printf("OK\n");
                } else if(rmdir(vfs, result) == -1) {
                    printf("NOT EMPTY (adresář obsahuje podadresáře, nebo soubory)\n");
                } else if(rmdir(vfs, result) == -2) {
                    printf("YOU CAN'T DELETE ROOT FOLDER!!\n");
                } else {
                    printf("FILE NOT FOUND (neexistující adresář)\n");
                }
            }

            vfs->current = current_folder;

        } else if(strcmp(command, "rm") == 0) {

            argument1 = strtok(NULL, "\0");
            result = path(vfs, argument1);
            
            if(strcmp(result, "NPF") == 0) {
                printf("FILE NOT FOUND\n");
            } else if(search_file(vfs, result) == NULL) {
                printf("FILE NOT FOUND\n");
            } else {
                if(rm(vfs, result) == 0) {
                    printf("OK\n");
                } else {
                    printf("FILE NOT FOUND\n");
                }
            }

            vfs->current = current_folder;

            

        } else if(strcmp(command, "info") == 0) {

            argument1 = strtok(NULL, "\0");
            result = path(vfs, argument1);

            if(strcmp(result, "NPF") == 0) {
                printf("FILE NOT FOUND (není zdroj)\n");
            } else {
                if(info(vfs, result) == -1) {
                    printf("FILE NOT FOUND (není zdroj)\n");
                }
            }

            vfs->current = current_folder;

        } else if(strcmp(command, "cat") == 0) {

            argument1 = strtok(NULL, "\0");
            result = path(vfs, argument1);
            if(strcmp(result, "NPF") == 0) {
                printf("FILE NOT FOUND (není zdroj)\n");
            } else if(search_file(vfs, result) == NULL) {
                printf("FILE NOT FOUND (není zdroj)\n");
            } else {
                if(cat(vfs, result) == -1) {
                    printf("FILE NOT FOUND (není zdroj)\n");
                }
            }

            vfs->current = current_folder;

        } else if(strcmp(command, "mv") == 0) {

            argument1 = strtok(NULL, " ");
            argument2 = strtok(NULL, "\0");
            result = path(vfs ,argument1);
            if(strcmp(result, "NPF") == 0) {
                printf("FILE NOT FOUND (není zdroj)\n");
            } else if(search_file(vfs, result) == NULL) {
                printf("FILE NOT FOUND (není zdroj)\n");
            } else {
                now = search_file(vfs, result);
                vfs->current = current_folder;
                result = NULL;
                result = path(vfs, argument2);
                if(strcmp(result, "NPF") == 0) {
                    printf("PATH NOT FOUND (neexistuje cílová cesta)\n");
                } else {
                    if(search_file(vfs, result) != NULL) {
                        rm(vfs, result);
                        mv(vfs, now, result);
                    } else {
                        mv(vfs, now, result);
                    }
                }
            }
            now = NULL;
            vfs->current = current_folder;  

        } else if(strcmp(command, "cp") == 0) {

            argument1 = strtok(NULL, " ");
            argument2 = strtok(NULL, "\0");
            result = path(vfs ,argument1);
            if(strcmp(result, "NPF") == 0) {
                printf("FILE NOT FOUND (není zdroj)\n");
            } else if(search_file(vfs, result) == NULL) {
                printf("FILE NOT FOUND (není zdroj)\n");
            } else {
                now = search_file(vfs, result);
                vfs->current = current_folder;
                result = NULL;
                result = path(vfs, argument2);
                if(strcmp(result, "NPF") == 0) {
                    printf("PATH NOT FOUND (neexistuje cílová cesta)\n");
                } else if(search_file(vfs, result) != NULL) {
                    rm(vfs, result);
                    if(cp(vfs, now, result) == 0) {
                        printf("OK\n");
                    } else {
                        printf("PATH NOT FOUND (neexistuje cílová cesta)\n");
                    }
                } else {
                    if(cp(vfs, now, result) == 0) {
                        printf("OK\n");
                    } else {
                        printf("PATH NOT FOUND (neexistuje cílová cesta)\n");
                    }
                }
            }

            now = NULL; 
            vfs->current = current_folder;  

        } else if(strcmp(command, "ls") == 0) {
            argument1 = strtok(NULL, "\0");

            result = path(vfs, argument1);
            if(strcmp(result, "NPF") == 0) {
                printf("PATH NOT FOUND (neexistující adresář)");
            } else if (search_directory(vfs, result) == NULL) {
                printf("PATH NOT FOUND (neexistující adresář)\n");
            } else {
                cd(vfs, result);
                ls(vfs, vfs->current);
            }
        } else if(strcmp(command, "incp") == 0) {

            int rest = 0;
            argument1 = strtok(NULL, " ");

            argument2 = strtok(NULL, "\0");

            result = path(vfs, argument2);

            if(strcmp(result, "NPF") == 0) {
               printf("PATH NOT FOUND (neexistuje cílová cesta)\n");
            } else {
                
                rest = incp(vfs, argument1, result);
                if(rest == -1) {
                    printf("FILE NOT FOUND (není zdroj)\n");
                } else if(rest == -2) {
                    printf("PATH NOT FOUND (neexistuje cílová cesta)\n");
                }else {
                    printf("OK\n");
                }
            }

            vfs->current = current_folder;

        } else if(strcmp(command, "outcp") == 0) {

            int rest = 0;

            argument1 = strtok(NULL, " ");

            result = path(vfs, argument1);

            argument2 = strtok(NULL, "\0");

            if(strcmp(result, "NPF") == 0) {
                printf("FILE NOT FOUND (není zdroj)\n");
            } else if(search_file(vfs, result) == NULL) { 
                printf("FILE NOT FOUND (není zdroj)\n");
            } else {

                rest = outcp(vfs, result, argument2);
                if(rest == 0) {
                    printf("OK\n");
                }
            }

            vfs->current = current_folder;


        } else if(strcmp(command, "short") == 0) {

            argument1 = strtok(NULL, "\0");

            result = path(vfs, argument1);

            if(strcmp(result, "NPF") == 0) {
                printf("FILE NOT FOUND (není zdroj)\n");
            } else if(search_file(vfs, result) == NULL) { 
                printf("FILE NOT FOUND (není zdroj)\n");
            } else {

                if(shortf(vfs, result) == 0) {
                    printf("OK\n");
                }
            }

            vfs->current = current_folder;
        } else if(strcmp(command, "xcp") == 0) {

            argument1 = strtok(NULL, " ");
            argument2 = strtok(NULL, " ");
            argument3 = strtok(NULL, "\0");

            result = path(vfs, argument1);

            if(strcmp(result, "NPF") == 0) {
                printf("FILE NOT FOUND (není zdroj)\n");
            } else if(search_file(vfs, result) == NULL) {
                printf("FILE NOT FOUND (není zdroj)\n");
            } else {
                one = search_file(vfs, result);
                vfs->current = current_folder;
                result = NULL;
                result = path(vfs, argument2);

                if(strcmp(result, "NPF") == 0) {
                    printf("FILE NOT FOUND (není zdroj)\n");
                } else if(search_file(vfs, result) == NULL) {
                    printf("FILE NOT FOUND (není zdroj)\n");
                } else {
                    two = search_file(vfs, result);
                    vfs->current = current_folder;
                    result = NULL;
                    result = path(vfs, argument3);

                    if(strcmp(result, "NPF") == 0) {
                        printf("PATH NOT FOUND (neexistuje cílová cesta)\n");
                    } else if(search_file(vfs, result) != NULL) {
                        rm(vfs, result);
                        if(xcp(vfs, one, two, result) == 0) {
                            printf("OK\n");
                        } else {
                            printf("PATH NOT FOUND (neexistuje cílová cesta)\n");
                        }
                    } else {
                        if(xcp(vfs, one, two, result) == 0) {
                            printf("OK\n");
                        } else {
                            printf("PATH NOT FOUND (neexistuje cílová cesta)\n");
                        }
                    }                          
                }
            }

            vfs->current = current_folder;     
        } else if(strcmp(command, "load") == 0) {
            argument1 = strtok(NULL, "\0");

            if(load(vfs, argument1) == 0) {
                printf("OK\n");
            } else if(load(vfs, argument1) == -1) {
                printf("FILE NOT FOUND (není zdroj)\n");
            }

            vfs->current = current_folder;
        } else if(strcmp(command, "format") == 0) {

            char *unit = NULL;
            int size = 0;

            argument1 = strtok(NULL, "\0");

            for (size_t i = 0; i < strlen(argument1); i++) {
                if((argument1[i] == 'M') && (argument1[i + 1] == 'B')) {
                    unit = (char *)malloc(sizeof(char) * i);
                    size = atoi(strncpy(unit, argument1, i));
                    size = size * 1000000;
                    break;
                } else if((argument1[i] == 'G') && (argument1[i + 1] == 'B')) {
                    unit = (char *)malloc(sizeof(char) * i);
                    size = atoi(strncpy(unit, argument1, i));
                    size = size * 1000000000;
                    break;
                }
            }

            vfs = format(vfs, size);
            if(vfs == NULL) {
                printf("CANNOT CREATE FILE\n");
                continue_w = false;
            } else if (vfs != NULL) {
                printf("OK\n");
            }

        }
    }
}

directory_item *search_directory(vfs_t *vfs, char * name_directory) {

    directory_item *current = NULL;

    current = vfs->all_elements;

    while (current != NULL)
    {
        if(strcmp(current->item_name, name_directory) == 0 && strcmp(current->parent_name, vfs->current->item_name) == 0 && current->isFile == false) {
            return current;
        }

        current = current->next_element;
    }

    return NULL;
    
}

directory_item *search_file(vfs_t *vfs, char *name_file) {

    directory_item *current = NULL;

    current = vfs->all_elements;
    while (current)
    {
        if(current->isFile == true) {
            if(strcmp(current->item_name, name_file) == 0 && strcmp(current->parent_name, vfs->current->item_name) == 0 && current->isFile == true) {
                return current;
            } 
        }

        current = current->next_element;
    }

    return NULL;
}

directory_item *previus_dir(vfs_t *vfs, char *name_directory) {

    directory_item *current = NULL;

    current = vfs->all_elements;

    while (current != NULL)
    {
        if(strcmp(current->next_element->item_name, name_directory) == 0 && strcmp(current->next_element->parent_name, vfs->current->item_name) == 0) {
            return current;
        }

        current = current->next_element;
    }

    return NULL;
    
}

directory_item *search_parent_directory(vfs_t *vfs, char *name_directory) {

    directory_item *current = NULL;

    current = vfs->all_elements;

    while (current != NULL)
    {
        if(current->isFile == false) {

            if(strcmp(current->item_name, name_directory) == 0) {
                return current;
            }

        }

        current = current->next_element;
    }

    return NULL;

}

int count_of_dir(vfs_t *vfs, char *name_directory) {

    int count_of_dir = 0;
    directory_item *current = NULL;

    current = vfs->all_elements;

    while (current != NULL)
    {
        if(strcmp(current->parent_name, name_directory) == 0) {
            count_of_dir++;
        }
        current = current->next_element;
    }

    return count_of_dir;
}

int seach_free_index(vfs_t *vfs) {
    if(!vfs) {
        return -1;
    }
    

    for (int i = 0; i < vfs->superblock->fat_count; i++)
    {
        if(vfs->fat_table1[i] == FAT_UNUSED) {
            return i;
        }
    }

    return -1;
}

int next_index(vfs_t *vfs, int index) {

    int new_index = 0;

    new_index = vfs->fat_table1[index];

    return new_index;
}

char *path(vfs_t *vfs, char *argument) {

    char *modife  = NULL;
    char *isFile = NULL;

    modife = (char *)malloc(sizeof(char) * strlen(argument));

    strcpy(modife, argument);

    if(modife[0] == '/') {

        root_folder(vfs);
        isFile = absolutle_path(vfs, argument);

    } else {

        isFile = relativ_path(vfs, argument);
        
    }

    return isFile;
}

char *path_string(vfs_t *vfs) {
    size_t total_len = 0;
    char *path = NULL;
    char *old_path = NULL;
    directory_item *up_folder = NULL;

    total_len = strlen(vfs->current->item_name) + 2;
    path = (char *)malloc(sizeof(char) * total_len);
    strcpy(path, "/");
    strcat(path, vfs->current->item_name);

    if(strcmp(vfs->current->item_name, "root") != 0) {

        up_folder = vfs->current;

        while (strcmp(up_folder->parent_name, "root") != 0)
        {
            old_path = path;
            up_folder = search_parent_directory(vfs, up_folder->parent_name);
            total_len = total_len + strlen(up_folder->item_name) + 1;
            path = (char *)malloc(sizeof(char) * total_len);
            strcpy(path, "/");
            strcat(path, up_folder->item_name);
            strcat(path, old_path);
        }

        old_path = path;
        up_folder = search_parent_directory(vfs, "root");
        total_len = total_len + strlen(up_folder->item_name) + 1;
        path = (char *)malloc(sizeof(char) * total_len);
        strcpy(path, "/");
        strcat(path, up_folder->item_name);
        strcat(path, old_path);
    }

    return path;
}

void root_folder(vfs_t *vfs) {

    vfs->current = vfs->all_elements;


}