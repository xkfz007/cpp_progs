/*
output the linked list reversely.
*/
#ifdef _LIST_REVERSE
#include <stdio.h>
#include <stdlib.h>
typedef struct node{
    int val;
    struct node* next;
}Node;
void reverse_out(Node* list){
    if(list==NULL)
        return;
    reverse_out(list->next);
    printf("%d\n",list->val);
}
int main(int argc,char*argv[]){
    int i=0;
    Node *list=NULL,*p,*t;
    //FILE *fp=fopen(argv[1],"r");
    char ss[20];
    int val;
    while(scanf("%d",&val)!=EOF) {
 //       sscanf(ss,"%d",&val);
        if(val==-1)
            break;
        if(list==NULL) {
            list=malloc(sizeof(Node));
            list->val=val;
            p=list;
        }
        else{
            t=malloc(sizeof(Node));
            t->val=val;
            p->next=t;
            p=t;
        }
    }

    reverse_out(list);
    p=list;
    while(p!=NULL){
        list=p->next;
        free(p);
        p=list;
    }

    return 0;
}
#endif