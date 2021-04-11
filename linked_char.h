 #include <string.h>
 #include <stdio.h>
 int countC();
 struct nodeC
{
    char* data;
    struct nodeC *next;
}*headC;
void appendC(char* num);
void addC( char* num );
void addafterC(char* num, int loc);
void insertC(char* num);
int deleteC(char* num);
void  displayC(struct nodeC *r);
void freeallC();
