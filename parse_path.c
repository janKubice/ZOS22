#include "parse_path.h"

char *relativ_path(vfs *vfs, char *path) {
    
    char *modife = NULL;
    size_t parse = 0;

    for (size_t i = 0; i < strlen(path); i++)
    {
        if(path[i] == '/' && (strlen(path) -1) == i) {
            path[i] = '\0';
        } else if(path[i] == '/') {
            parse++;
        }
    }

    if(path[0] == '/' && parse == 1) {
        modife = strtok(path, "/");
        return modife;
    } else if(path[0] == '/' && parse != 1) {
        modife = strtok(path, "/");

        for (size_t i = 0; i < parse; i++)
        {
            if(i == parse - 1) {
                return modife;
            } else {
                if(search_directory(vfs, modife) == NULL) {
                    return "PNF";
                } else {
                    cd(vfs, modife);
                }
            }

            modife = strtok(NULL, "/");
        }
        
    } else {
        if(parse == 0) {
            return path;
        } else if(parse != 0) {
            modife = strtok(path, "/");
            if(strcmp(modife, "..") == 0) {
                cd(vfs, modife);
            } else if(strcmp(modife, ".") == 0) {

                for (size_t i = 0; i < parse; i++) {

                    modife = strtok(NULL, "/");

                    if(i == parse -1) {
                        return modife;
                    } else {
                        if(strcmp(modife, "..") == 0) {
                            cd(vfs, modife);
                        } else if(strcmp(modife, ".") == 0) {

                        } else if(search_directory(vfs, modife) == NULL) {
                            return "PNF";
                        } else {
                            cd(vfs, modife);
                        }
                    }
                }

            } else {
                if(search_directory(vfs, modife) == NULL) {
                    return "PNF";
                } else {
                    cd(vfs, modife);
                    for (size_t i = 0; i < parse; i++)
                    {
                        modife = strtok(NULL, "/");

                        if(i == parse -1) {
                            return modife;
                        } else {
                             if(strcmp(modife, "..") == 0) {
                                cd(vfs, modife);
                            } else if(strcmp(modife, ".") == 0) {
                            
                            } else if(search_directory(vfs, modife) == NULL) {
                                return "PNF";
                            } else {
                                cd(vfs, modife);
                            }
                        }
                    }
                
                }
            }
            
        }
    }
    return "PNF";
}

char *absolutle_path(vfs *vfs,char *path) {

    size_t parse = 0;
    char *next = NULL;

    for (size_t i = 0; i < strlen(path); i++)
    {
        if(path[i] == '/' && (strlen(path) -1) == i) {
            path[i] = '\0';
        } else if(path[i] == '/') {
            parse++;
        }
    }

    if(parse == 0) {
        return "root";
    }

    next = strtok(path, "/");
    for (size_t i = 0; i < parse; i++)
    {
        
        if(i == parse -1) {
            return next;
        }

        if(search_directory(vfs, next) != NULL) {
            cd(vfs, next);
        } else {
            return "NPF";
        }

        next = strtok(NULL, "/");
    }

    return "NPF";
}