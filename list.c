#include <dirent.h>
#include <stdio.h>
void list(const char* path){
    struct dirent *files;
    DIR *directory=opendir(path);
    while ((files = readdir(directory)) != NULL) {
    printf ("%s\n", files->d_name);
  }
closedir(directory);
return;
}