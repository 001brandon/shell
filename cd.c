#include "sh.h"
void cd(const char * path){
    int error=chdir(path);
    return;
}