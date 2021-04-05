 int count();
 struct node
{
    int data;
    struct node *next;
};
void append(int num);
void add( int num );
void addafter(int num, int loc);
void insert(int num);
int delete(int num);
void  display(struct node *r);
void freeall();
