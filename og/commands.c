#include "commands.h"


int ls(vfs *vfs, directory_item *current) {

    directory_item *node = NULL;

    if(!current || !vfs) {
        printf("ERROR!!");
        return -1;
    }

    node = vfs->all_elements;

    printf("Aktualni slozka: %s\n", current->item_name);

    if(strcmp(current->item_name, "root") != 0) {
        printf("Nadrazena slozka: %s\n", current->parent_name);
    }

    printf("Podadresare:");
    while (node != NULL)
    {
        
        if((strcmp(node->parent_name, current->item_name) == 0) && (node->isFile == false)) {

            if((strcmp(node->item_name, current->item_name) != 0) && (strcmp(node->item_name, current->parent_name) != 0)) {
                
                printf("%s, ", node->item_name);
            }
        }


        node = node->next_element;

        
    }
    printf("\n");

    printf("Soubory:");

    node = vfs->all_elements;

    while (node != NULL)
    {
        
        if((strcmp(node->parent_name, current->item_name) == 0) && (node->isFile == true)) {
 
            printf("%s, ", node->item_name);
        }


        node = node->next_element;

        
    }

    printf("\n");

    return 0;
}

int pwd(vfs *vfs) {

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

    printf("Path: %s\n", path);
    
    return 0;
}

int mkdir(vfs *vfs, char *name) {
    
    directory_item *current = NULL;
    int index = 0;

    index = seach_free_index(vfs);
    vfs->fat_table1[index] = FAT_FILE_END;
    current = vfs->all_elements;

    while (current->next_element != NULL)
    {
        current = current->next_element;
    }
    
    current->next_element = (directory_item *)malloc(sizeof(directory_item));
    strcpy(current->next_element->item_name, name);
    current->next_element->isFile = false;
    current->next_element->size = 0;
    current->next_element->start_cluster = index;
    strcpy(current->next_element->parent_name, vfs->current->item_name);
    current->next_element->next_element = NULL;

    return 0;
}

int cd(vfs *vfs, char *s) {
    if (vfs == NULL || s == NULL){
        return -1;
    }

    directory_item *next = NULL;
    next = vfs->all_elements;

    if (next == NULL){
        return -1;
    }

    while (next != NULL)
    {
        if(strcmp(s, "..") == 0) {

            if(strcmp(vfs->current->item_name, "root") == 0) 
            {
                vfs->current = vfs->current;
                return 0;
            } 
            else {
                if(strcmp(next->item_name, vfs->current->parent_name) == 0) 
                {
                    vfs->current = next;
                    return 0;
                }
            }
        } 
        else {
            if(strcmp(next->item_name, s) == 0 && strcmp(next->parent_name, vfs->current->item_name) == 0) 
            {
                vfs->current = next;
                return 0;
            }
        }
        next = next->next_element;
    }
    
    return -1;
}

int rmdir(vfs *vfs, char *name) {

    int count = 0;
    directory_item *choose = NULL;
    directory_item *previus = NULL;

    if(strcmp(name, "root") != 0) {

        choose = search_directory(vfs, name);

        if(choose != NULL) {
            
            count = count_of_dir(vfs, choose->item_name);
            if(count == 0) {
                vfs->fat_table1[choose->start_cluster] = FAT_UNUSED;
                previus = previus_dir(vfs, choose->item_name);
                previus->next_element = choose->next_element;
                free(choose);
                choose = NULL;
                return 0;
            } else {
                return -1;
            }

        }

    } else {
        
        return -2;

    }

    return -3;

}

int rm(vfs *vfs, char *s) {

    int index = 0;
    int index_previus = 0;
    directory_item *choose = NULL;
    directory_item *previus = NULL;

    choose = search_file(vfs, s);

    if(choose != NULL) {

        printf("%d\n", choose->start_cluster);

        int i = 0;
        for (i = 0; i < CLUSTER_SIZE; i++)
        {
            vfs->data_block[choose->start_cluster][i] = '\0';
        }
        
        index = next_index(vfs, choose->start_cluster);
        vfs->fat_table1[choose->start_cluster] = FAT_UNUSED;
        index_previus = index;
        while (index != FAT_FILE_END)
        {
            int i = 0;
            for (i = 0; i < CLUSTER_SIZE; i++)
            {
                vfs->data_block[index][i] = '\0';
            }

            index = next_index(vfs, index);
            vfs->fat_table1[index_previus] = FAT_UNUSED;
            index_previus = index;
        }

        
        previus = previus_dir(vfs, choose->item_name);
        previus->next_element = choose->next_element;
        free(choose);
        choose = NULL;

        return 0;
            
    }

    return -1;

}

int info(vfs *vfs, char *name) {

    directory_item *choose = NULL;
    int index = 0;

    choose = search_file(vfs, name);
    if(choose != NULL) {
        printf("Size of File: %d\n", choose->size);
        printf("%d", choose->start_cluster);
        index = next_index(vfs, choose->start_cluster);
        if(index != FAT_FILE_END) {
            printf(", ");
        }
        while (index != FAT_FILE_END)
        {
            printf("%d", index);
            index = next_index(vfs, index);
            if(index != FAT_FILE_END) {
                printf(", ");
            }
        }

        printf("\n");

        return 0;
            
    } else {
        choose = search_directory(vfs, name);
        if(choose != NULL) {
            printf("%d", choose->start_cluster);
            index = next_index(vfs, choose->start_cluster);
            if(index != FAT_FILE_END) {
                printf(", ");
            }
            while (index != FAT_FILE_END) {
                printf("%d", index);
                index = next_index(vfs, index);
                if(index != FAT_FILE_END) {
                    printf(", ");
                }
            }

            printf("\n");
                
            return 0;
        }
        
    }

    return -1;

}

int cat(vfs *vfs, char *name) {

    directory_item *choose = NULL;
    int index = 0;
    
    choose = search_file(vfs, name);
    if(choose != NULL) {

        size_t i = 0;
        for (i = 0; i < CLUSTER_SIZE; i++)
        {

            printf("%c", vfs->data_block[choose->start_cluster][i]);

        }

        index = vfs->fat_table1[choose->start_cluster];
        while (index != FAT_FILE_END)
        {
            size_t i = 0;
            for (i = 0; i < CLUSTER_SIZE; i++)
            {
                    
                printf("%c", vfs->data_block[index][i]);

            }

            index = vfs->fat_table1[index];
        }

        printf("\n");
        return 0;

        }

    return -1;

}


int mv(vfs *vfs, directory_item *s1, char *s2) {
    if (vfs == NULL){
        return -1;
    }

    if (s1 == NULL || s2 == NULL){
        return -1;
    }


    if(strcmp(s1->parent_name, vfs->current->item_name) == 0) {

        strcpy(s1->item_name, s2);
        printf("OK\n");
        return 0;   
    } else {
        strcpy(s1->parent_name, vfs->current->item_name);
        strcpy(s1->item_name, s2);
        printf("OK\n");
        return 0;
    }

}

int cp(vfs *vfs, directory_item *one, char *name_two) {

    directory_item *current = NULL;
    int index = 0;
    int index_one = 0;
    int index_previous = 0;


    if(strcmp(vfs->current->item_name, one->parent_name) == 0) {
        if(strcmp(one->item_name, name_two) == 0) {
            printf("You copy similar file!!\n");
            return 0;
        } else {

            if(one->size <= CLUSTER_SIZE) {
                index = seach_free_index(vfs);
                vfs->fat_table1[index] = FAT_FILE_END;
                current = vfs->all_elements;

                while (current->next_element != NULL)
                {
                    current = current->next_element;
                }

                current->next_element = (directory_item *)malloc(sizeof(directory_item));
                strcpy(current->next_element->item_name, name_two);
                current->next_element->isFile = true;
                current->next_element->size = one->size;
                current->next_element->start_cluster = index;
                strcpy(current->next_element->parent_name, vfs->current->item_name);
                current->next_element->next_element = NULL;

                vfs->fat_table1[index] = FAT_FILE_END;
                memcpy(vfs->data_block[index], vfs->data_block[one->start_cluster], sizeof(char) * CLUSTER_SIZE);
                return 0;

            } else {

                index = seach_free_index(vfs);
                current = vfs->all_elements;

                while (current->next_element != NULL)
                {
                    current = current->next_element;
                }

                current->next_element = (directory_item *)malloc(sizeof(directory_item));
                strcpy(current->next_element->item_name, name_two);
                current->next_element->isFile = true;
                current->next_element->size = one->size;
                current->next_element->start_cluster = index;
                strcpy(current->next_element->parent_name, vfs->current->item_name);
                current->next_element->next_element = NULL;
                vfs->fat_table1[index] = FAT_FILE_END;
                memcpy(vfs->data_block[index], vfs->data_block[one->start_cluster], sizeof(char) * CLUSTER_SIZE);

                index_previous = index;
                index_one = vfs->fat_table1[one->start_cluster];

                while (index_one != FAT_FILE_END)
                {
                    index = seach_free_index(vfs);
                    vfs->fat_table1[index_previous] = index;
                    vfs->fat_table1[index] = FAT_FILE_END;
                    memcpy(vfs->data_block[index], vfs->data_block[index_one], sizeof(char) * CLUSTER_SIZE);
                    index_one = vfs->fat_table1[index_one];
                }
                
                return 0;

            }

        }
    } else {
       
        if(one->size <= CLUSTER_SIZE) {
            index = seach_free_index(vfs);
            vfs->fat_table1[index] = FAT_FILE_END;
            current = vfs->all_elements;

            while (current->next_element != NULL)
            {
                current = current->next_element;
            }

            current->next_element = (directory_item *)malloc(sizeof(directory_item));
            strcpy(current->next_element->item_name, name_two);
            current->next_element->isFile = true;
            current->next_element->size = one->size;
            current->next_element->start_cluster = index;
            strcpy(current->next_element->parent_name, vfs->current->item_name);
            current->next_element->next_element = NULL;

            vfs->fat_table1[index] = FAT_FILE_END;
            memcpy(vfs->data_block[index], vfs->data_block[one->start_cluster], sizeof(char) * CLUSTER_SIZE);
            return 0;
        } else {

            index = seach_free_index(vfs);
            current = vfs->all_elements;

            while (current->next_element != NULL)
            {
                current = current->next_element;
            }

            current->next_element = (directory_item *)malloc(sizeof(directory_item));
            strcpy(current->next_element->item_name, name_two);
            current->next_element->isFile = true;
            current->next_element->size = one->size;
            current->next_element->start_cluster = index;
            strcpy(current->next_element->parent_name, vfs->current->item_name);
            current->next_element->next_element = NULL;
            vfs->fat_table1[index] = FAT_FILE_END;
            memcpy(vfs->data_block[index], vfs->data_block[one->start_cluster], sizeof(char) * CLUSTER_SIZE);

            index_previous = index;
            index_one = vfs->fat_table1[one->start_cluster];

            while (index_one != FAT_FILE_END)
            {
                index = seach_free_index(vfs);
                vfs->fat_table1[index_previous] = index;
                vfs->fat_table1[index] = FAT_FILE_END;
                memcpy(vfs->data_block[index], vfs->data_block[index_one], sizeof(char) * CLUSTER_SIZE);
                index_one = vfs->fat_table1[index_one];
            }
                
        return 0;

        }
    }

    return -1;
}

int incp(vfs *vfs, char *input_file, char *file_name) {


    FILE *fp = NULL;
    int index = 0;
    int index_previus = 0;
    int counter = 0;
    int read_byte = 0;
    int size = 0;
    directory_item *current = NULL;
    char *file = NULL;

    fp = fopen(input_file, "rb");

    if(fp == NULL) {
        printf("FILE NOT FOUND (není zdroj)\n");
        return -1;
    }

    fseek(fp, 0L, SEEK_END);

    size = ftell(fp);

    file = (char *)malloc(sizeof(char) * size);

    rewind(fp);

    read_byte = fread(file, sizeof(char), size, fp);
    if(read_byte != size) {
        printf("Chyba 1\n");
        return -1;
    }

    printf("%d\n", read_byte);

    index = seach_free_index(vfs);
    vfs->fat_table1[index] = FAT_FILE_END;
    current = vfs->all_elements;

    while (current->next_element != NULL)
    {
        current = current->next_element;
    }

    current->next_element = (directory_item *)malloc(sizeof(directory_item));
    if(current->next_element == NULL) {
        printf("Chyba 2");
        return -2;
    }

    
    strcpy(current->next_element->item_name, file_name);
    current->next_element->isFile = true;
    current->next_element->size = size;
    current->next_element->start_cluster = index;
    strcpy(current->next_element->parent_name, vfs->current->item_name);
    current->next_element->next_element = NULL;
    index_previus = index;

    int i = 0;
    for (i = 0; i < size; i++)
    {
        vfs->data_block[index][counter] = file[i];

        if(counter == (CLUSTER_SIZE - 1)) {
            index = seach_free_index(vfs);
            vfs->fat_table1[index_previus] = index;
            vfs->fat_table1[index] = FAT_FILE_END;
            index_previus = index;
            counter = 0;
        } else {
            counter++;
        }
    }
    
    

    fclose(fp);
    fp = NULL;

    return 0;

}

int outcp(vfs *vfs, char *file_name, char *output_file) {

    FILE *fp = NULL;
    int index = 0;
    int counter = 0;
    int write_byte = 0;
    directory_item *current = NULL;
    char *file = NULL;
    

    fp = fopen(output_file, "wb");

    current = search_file(vfs, file_name);

    file = (char *)malloc(sizeof(char) * current->size);

    index = current->start_cluster;

    int i = 0;
    for (i = 0; i < current->size; i++)
    {
        file[i] = vfs->data_block[index][counter];

        if(counter == (CLUSTER_SIZE -1)) {
            index = next_index(vfs, index);
            counter = 0;
        } else {
            counter++;
        }
    }

    write_byte = fwrite(file, sizeof(char), current->size, fp);
    if(write_byte != current->size) {
        return -1;
    }
    
    fclose(fp);
    fp = NULL;

    return 0;
}

int shortf(vfs *vfs, char *name) {

    directory_item  *current = NULL;
    int index = 0;
    int index_previus = 0;

    current = search_file(vfs, name);

    if(current->size <= CLUSTER_SIZE) {
        return 0;
    }

    index = next_index(vfs, current->start_cluster);
    vfs->fat_table1[current->start_cluster] = FAT_FILE_END;
    current->size = CLUSTER_SIZE;
    index_previus = index;


    while (index != FAT_FILE_END)
    {
        memset(vfs->data_block[index], 0, sizeof(char) * CLUSTER_SIZE);
        index = next_index(vfs, index);
        vfs->fat_table1[index_previus] = FAT_UNUSED;
    }

    return 0;

}

int xcp(vfs *vfs, directory_item *one, directory_item *two, char *name) {

    directory_item *current = NULL;
    int index = 0;
    int index_one = 0;
    int index_two = 0;
    int index_previous = 0;

    current = vfs->all_elements;

        while (current->next_element != NULL) {
            current = current->next_element;
        }

    current->next_element = (directory_item *)malloc(sizeof(directory_item));
    strcpy(current->next_element->item_name, name);
    current->next_element->isFile = true;
    current->next_element->size = (one->size + two->size);
    strcpy(current->next_element->parent_name, vfs->current->item_name);

    if(one->size <= CLUSTER_SIZE) {
        index = seach_free_index(vfs);
        vfs->fat_table1[index] = FAT_FILE_END;
        index_previous = index;

        current->next_element->start_cluster = index;
        current->next_element->next_element = NULL;

        vfs->fat_table1[index] = FAT_FILE_END;
        memcpy(vfs->data_block[index], vfs->data_block[one->start_cluster], sizeof(char) * CLUSTER_SIZE);

    } else {

        index = seach_free_index(vfs);

        current->next_element->start_cluster = index;
        current->next_element->next_element = NULL;

        vfs->fat_table1[index] = FAT_FILE_END;
        memcpy(vfs->data_block[index], vfs->data_block[one->start_cluster], sizeof(char) * CLUSTER_SIZE);

        index_previous = index;
        index_one = vfs->fat_table1[one->start_cluster];

        while(index_one != FAT_FILE_END) {
            index = seach_free_index(vfs);
            vfs->fat_table1[index_previous] = index;
            vfs->fat_table1[index] = FAT_FILE_END;
            index_previous = index;
            memcpy(vfs->data_block[index], vfs->data_block[index_one], sizeof(char) * CLUSTER_SIZE);
            index_one = vfs->fat_table1[index_one];
        }
    }

    if(two->size <= CLUSTER_SIZE) {
        index = seach_free_index(vfs);
        vfs->fat_table1[index] = FAT_FILE_END;
        vfs->fat_table1[index_previous] = index;
        index_previous = index;

        memcpy(vfs->data_block[index], vfs->data_block[two->start_cluster], sizeof(char) * CLUSTER_SIZE);

    } else {

        index = seach_free_index(vfs);

        vfs->fat_table1[index] = FAT_FILE_END;
        vfs->fat_table1[index_previous] = index;
        memcpy(vfs->data_block[index], vfs->data_block[two->start_cluster], sizeof(char) * CLUSTER_SIZE);

        index_previous = index;
        index_two = vfs->fat_table1[two->start_cluster];

        while(index_one != FAT_FILE_END) {
            index = seach_free_index(vfs);
            vfs->fat_table1[index_previous] = index;
            vfs->fat_table1[index] = FAT_FILE_END;
            index_previous = index;
            memcpy(vfs->data_block[index], vfs->data_block[index_two], sizeof(char) * CLUSTER_SIZE);
            index_two = vfs->fat_table1[index_two];
        }
    }

    return 0;

}

int load(vfs *vfs, char *file_name) {
    
    FILE *fp = NULL;
    char line[1024] = { 0 };
    char *command = NULL;
    char *argument1 = NULL;
    char *argument2 = NULL;
    char *argument3 = NULL;
    char *result = NULL;
    directory_item *current_folder = NULL;
    directory_item *now = NULL;
    directory_item *one = NULL;
    directory_item *two = NULL;
    bool end = true;

    current_folder = vfs->current;

    printf("%s\n", file_name);

    fp = fopen(file_name, "r");

    if(fp != NULL) {
        return -1;
    }

    while (fgets(line, 1024, fp))
    {
        command = strtok(line, " ");
        if(strcmp(command, "mkdir") == 0) {

            argument1 = strtok(NULL, "\0");
            result = path(vfs, argument1);
            if(strcmp(result, "NPF") == 0) {
                printf("PATH NOT FOUND (neexistuje zadaná cesta)\n");
                end = false;
                break;
            } else if(search_directory(vfs, result) != NULL) {
                printf("EXIST (nelze založit, již existuje)\n");
                 end = false;
                break;
            } else if(search_file(vfs, result) != NULL) {
                printf("EXIST (nelze založit, již existuje)\n");
                end = false;
                break;
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
                end = false;
                break;
            } else if(strcmp(result, "..") == 0) {
                cd(vfs, result);
            } else if(strcmp(result, ".") == 0) {

            } else if(search_directory(vfs, result) == NULL) {
                printf("PATH NOT FOUND (neexistující cesta)\n");
                end = false;
                break;
            } else {
                cd(vfs, result);
            }

        } else if(strcmp(command, "rmdir") == 0) {

            argument1 = strtok(NULL, "\0");
            result = path(vfs ,argument1);
            if(strcmp(result, "NPF") == 0) {
                printf("FILE NOT FOUND (neexistující adresář)\n");
                end = false;
                break;
            } else if(search_directory(vfs, result) == NULL) {
                printf("FILE NOT FOUND (neexistující adresář)\n");
                end = false;
                break;
            } else {
                if(rmdir(vfs, result) == 0) {
                    printf("OK\n");
                } else if(rmdir(vfs, result) == -1) {
                    printf("NOT EMPTY (adresář obsahuje podadresáře, nebo soubory)\n");
                    end = false;
                    break;
                } else if(rmdir(vfs, result) == -2) {
                    printf("YOU CAN'T DELETE ROOT FOLDER!!\n");
                    end = false;
                    break;
                } else {
                    printf("FILE NOT FOUND (neexistující adresář)\n");
                    end = false;
                    break;
                }
            }

            vfs->current = current_folder;

        } else if(strcmp(command, "rm") == 0) {

            argument1 = strtok(NULL, "\0");
            result = path(vfs, argument1);
            
            if(strcmp(result, "NPF") == 0) {
                printf("FILE NOT FOUND\n");
                end = false;
                break;
            } else if(search_file(vfs, result) == NULL) {
                printf("FILE NOT FOUND\n");
                end = false;
                break;
            } else {
                if(rm(vfs, result) == 0) {
                    printf("OK\n");
                } else {
                    printf("FILE NOT FOUND\n");
                    end = false;
                    break;
                }
            }

            vfs->current = current_folder;

            

        } else if(strcmp(command, "info") == 0) {

            argument1 = strtok(NULL, "\0");
            result = path(vfs, argument1);

            if(strcmp(result, "NPF") == 0) {
                printf("FILE NOT FOUND (není zdroj)\n");
                end = false;
                break;
            } else {
                if(info(vfs, result) == -1) {
                    printf("FILE NOT FOUND (není zdroj)\n");
                    end = false;
                    break;
                }
            }

            vfs->current = current_folder;

        } else if(strcmp(command, "cat") == 0) {

            argument1 = strtok(NULL, "\0");
            result = path(vfs, argument1);
            if(strcmp(result, "NPF") == 0) {
                printf("FILE NOT FOUND (není zdroj)\n");
                end = false;
                break;
            } else if(search_file(vfs, result) == NULL) {
                printf("FILE NOT FOUND (není zdroj)\n");
                end = false;
                break;
            } else {
                if(cat(vfs, result) == -1) {
                    printf("FILE NOT FOUND (není zdroj)\n");
                    end = false;
                    break;
                }
            }

            vfs->current = current_folder;

        } else if(strcmp(command, "mv") == 0) {

            argument1 = strtok(NULL, " ");
            argument2 = strtok(NULL, "\0");
            result = path(vfs ,argument1);
            if(strcmp(result, "NPF") == 0) {
                printf("FILE NOT FOUND (není zdroj)\n");
                end = false;
                break;
            } else if(search_file(vfs, result) == NULL) {
                printf("FILE NOT FOUND (není zdroj)\n");
                end = false;
                break;
            } else {
                now = search_file(vfs, result);
                vfs->current = current_folder;
                result = NULL;
                result = path(vfs, argument2);
                if(strcmp(result, "NPF") == 0) {
                    printf("PATH NOT FOUND (neexistuje cílová cesta)\n");
                    end = false;
                    break;
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
                end = false;
                break;
            } else if(search_file(vfs, result) == NULL) {
                printf("FILE NOT FOUND (není zdroj)\n");
                end = false;
                break;
            } else {
                now = search_file(vfs, result);
                vfs->current = current_folder;
                result = NULL;
                result = path(vfs, argument2);
                if(strcmp(result, "NPF") == 0) {
                    printf("PATH NOT FOUND (neexistuje cílová cesta)\n");
                    end = false;
                    break;
                } else if(search_file(vfs, result) != NULL) {
                    rm(vfs, result);
                    if(cp(vfs, now, result) == 0) {
                        printf("OK\n");
                    } else {
                        printf("PATH NOT FOUND (neexistuje cílová cesta)\n");
                        end = false;
                        break;
                    }
                } else {
                    if(cp(vfs, now, result) == 0) {
                        printf("OK\n");
                    } else {
                        printf("PATH NOT FOUND (neexistuje cílová cesta)\n");
                        end = false;
                        break;
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
                end = false;
                break;
            } else if (search_directory(vfs, result) == NULL) {
                printf("PATH NOT FOUND (neexistující adresář)\n");
                end = false;
                break;
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
                end = false;
                break;
            } else if(search_file(vfs, result) == NULL) { 
                printf("PATH NOT FOUND (neexistuje cílová cesta)\n");
                end = false;
                break;
            } else {
                
                rest = incp(vfs, argument1, result);
                if(rest == -1) {
                    printf("FILE NOT FOUND (není zdroj)\n");
                    end = false;
                    break;
                } else if(rest == -2) {
                    printf("PATH NOT FOUND (neexistuje cílová cesta)\n");
                    end = false;
                    break;
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
                end = false;
                break;
            } else if(search_file(vfs, result) == NULL) { 
                printf("FILE NOT FOUND (není zdroj)\n");
                end = false;
                break;
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
                end = false;
                break;
            } else if(search_file(vfs, result) == NULL) { 
                printf("FILE NOT FOUND (není zdroj)\n");
                end = false;
                break;
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
                end = false;
                break;
            } else if(search_file(vfs, result) == NULL) {
                printf("FILE NOT FOUND (není zdroj)\n");
                end = false;
                break;
            } else {
                one = search_file(vfs, result);
                vfs->current = current_folder;
                result = NULL;
                result = path(vfs, argument2);

                if(strcmp(result, "NPF") == 0) {
                    printf("FILE NOT FOUND (není zdroj)\n");
                    end = false;
                    break;
                } else if(search_file(vfs, result) == NULL) {
                    printf("FILE NOT FOUND (není zdroj)\n");
                    end = false;
                    break;
                } else {
                    two = search_file(vfs, result);
                    vfs->current = current_folder;
                    result = NULL;
                    result = path(vfs, argument3);

                    if(strcmp(result, "NPF") == 0) {
                        printf("PATH NOT FOUND (neexistuje cílová cesta)\n");
                        end = false;
                        break;
                    } else if(search_file(vfs, result) != NULL) {
                        rm(vfs, result);
                        if(xcp(vfs, one, two, result) == 0) {
                            printf("OK\n");
                        } else {
                            printf("PATH NOT FOUND (neexistuje cílová cesta)\n");
                            end = false;
                            break;
                        }
                    } else {
                        if(xcp(vfs, one, two, result) == 0) {
                            printf("OK\n");
                        } else {
                            printf("PATH NOT FOUND (neexistuje cílová cesta)\n");
                            end = false;
                            break;
                        }
                    }                          
                }
            }       
        }
    }

    if(end == true) {
        return 0;
    } else {
        return -2;
    }
    
}

vfs *format(vfs *vfs, int size) {

    directory_item *current = NULL;
    directory_item *free_directory = NULL;
    int *fat_table1 = NULL;
    int *fat_table2 = NULL;
    char **data_block = NULL;
    directory_item *root = NULL;

    free(vfs->fat_table1);
    vfs->fat_table1 = NULL;
    free(vfs->fat_table2);
    vfs->fat_table2 = NULL;

    int i = 0;
    for (i = 0; i < vfs->superblock->fat_count; i++)
    {
        memset(vfs->data_block[i], 0, sizeof(char) * CLUSTER_SIZE);
        free(vfs->data_block[i]);
        vfs->data_block[i] = NULL;
    }

    memset(vfs->data_block, 0, sizeof(char) * vfs->superblock->fat_count);
    free(vfs->data_block);
    vfs->data_block = NULL;

    vfs->current = NULL;

    free_directory = vfs->all_elements;
    current = vfs->all_elements->next_element;

    free(free_directory);
    free_directory = NULL;

    while (current != NULL)
    {
        free_directory = current;
        current = current->next_element;
        free(free_directory);
        free_directory = NULL;
    }

    vfs->all_elements = NULL;

    vfs->superblock->disk_size = size;
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

    i = 0;
    for (i = 0; i < vfs->superblock->fat_count; i++) {

        fat_table1[i] = FAT_UNUSED;
        fat_table2[i] = FAT_UNUSED;
    }

    data_block = (char **)malloc(vfs->superblock->fat_count * sizeof(char *));
    if(data_block == NULL) {
        return NULL;
    }
    memset(data_block, 0, vfs->superblock->fat_count * sizeof(char *));
    
    i = 0;
    for (i = 0; i < vfs->superblock->fat_count; i++)
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

    vfs->fat_table1 = fat_table1;
    vfs->fat_table2 = fat_table2;
    vfs->data_block = data_block;
    vfs->current = root;
    vfs->all_elements = root;

    return vfs;
}

int exit_fs(vfs *vfs, bool *continue_w) {
    
    *continue_w = false;
    directory_item *direct;
    FILE *fp = NULL;

    fp = fopen(vfs->file, "wb");


    fwrite(vfs->superblock, sizeof(superblock_t), 1, fp);
    fwrite(vfs->fat_table1, sizeof(int), vfs->superblock->fat_count, fp);
    fwrite(vfs->fat_table2, sizeof(int), vfs->superblock->fat_count, fp);

    int i = 0;
    for (i = 0; i < vfs->superblock->fat_count; i++)
    {
        fwrite(vfs->data_block[i], sizeof(char), CLUSTER_SIZE, fp);
    }
    
    direct = vfs->all_elements;
    while(direct) {

        fwrite(direct, sizeof(directory_item), 1, fp);
        direct = direct->next_element;
    }
    fclose(fp);

    return 0;
}