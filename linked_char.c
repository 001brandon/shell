#include<stdio.h>
#include<stdlib.h>
#include "linked_char.h"
 

 int countC();
 


 
 
void appendC(char* num)
{
    struct nodeC *temp,*right;
    temp= (struct nodeC *)malloc(sizeof(struct nodeC));
    temp->data=num;
    right=(struct nodeC *)headC;
    while(right->next != NULL)
    right=right->next;
    right->next =temp;
    right=temp;
    right->next=NULL;
}
 
 
 
void addC( char* num )
{
    struct nodeC *temp;
    temp=(struct nodeC *)malloc(sizeof(struct nodeC));
    temp->data=num;
    if (headC== NULL)
    {
    headC=temp;
    headC->next=NULL;
    }
    else
    {
    temp->next=headC;
    headC=temp;
    }
}
void addafterC(char *num, int loc)
{
    int i;
    struct nodeC *temp,*left,*right;
    right=headC;
    for(i=1;i<loc;i++)
    {
    left=right;
    right=right->next;
    }
    temp=(struct nodeC *)malloc(sizeof(struct nodeC));
    temp->data=num;
    left->next=temp;
    left=temp;
    left->next=right;
    return;
}
 
 
 
void insertC(char* num)
{
    int c=0;
    struct nodeC *temp;
    temp=headC;
    if(temp==NULL)
    {
    addC(num);
    }
    else
    {
    while(temp!=NULL)
    {
        if(temp->data<num)
        c++;
        temp=temp->next;
    }
    if(c==0)
        addC(num);
    else if(c<countC())
        addafterC(num,++c);
    else
        appendC(num);
    }
}
 
 
 
int deleteC(char* num)
{
    struct nodeC **indirect=&headC;
    struct nodeC *temp=NULL;
    while(*indirect!=NULL){
        if(strcmp((*indirect)->data,num)==0){
            temp=*indirect;
            *indirect=(*indirect)->next;
            free(temp);
        } else{
            indirect=&((*indirect)->next);
        }
    }

    return 0;
}
 
 
void  displayC(struct nodeC *r)
{
    r=headC;
    if(r==NULL)
    {
    return;
    }
    while(r!=NULL)
    {
    printf("%s ",r->data);
    r=r->next;
    }
    printf("\n");
}
 
 
int countC()
{
    struct nodeC *n;
    int c=0;
    n=headC;
    while(n!=NULL)
    {
    n=n->next;
    c++;
    }
    return c;
}

void freeallC(){
    struct nodeC *temp=headC;
    while(headC!=NULL){
        temp=headC;
        headC=headC->next;
        free(temp);
    }
}
 