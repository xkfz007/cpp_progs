/*
 * this is the implementation of BSTree
 */
#ifdef _BSTREE
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#define TRUE 1
#define FALSE 0
typedef int BOOL;
typedef struct Node 
{
    struct Node *left,*right;
    size_t size;
    char data[];
}Node_t;
typedef const void *GetKeyFunc_t(const void*dData);
typedef int CmpFunc_t(const void *pKey1,const void *pKey2);
typedef struct {
    struct Node *pRoot;
    CmpFunc_t *cmp;
    GetKeyFunc_t *getKey;

}BST_t;
BST_t *newBST(CmpFunc_t *cmp,GetKeyFunc_t*getKey);
BOOL BST_insert(BST_t *pBST,const void *pData,size_t size);
const void *BST_search(BST_t*pBST,const void *pKey);
BOOL BST_erase(BST_t*pBST,const void *pKey);
void BST_clear(BST_t *pBST);
int BST_inorder(BST_t *pBST,BOOL(*action)(void *pData));
int BST_rev_inorder(BST_t *pBST,BOOL(*action)(void *pData));
int BST_preorder(BST_t *pBST,BOOL(*action)(void *pData));
int BST_postorder(BST_t *pBST,BOOL(*action)(void *pData));
const void *defaultGetKey(const void *pData)
{
    return pData;
}
BST_t *newBST(CmpFunc_t *cmp,GetKeyFunc_t*getKey)
{
    BST_t *pBST=NULL;
    if(cmp!=NULL)
        pBST=malloc(sizeof(BST_t));
    if(pBST!=NULL)
    {
        pBST->pRoot=NULL;
        pBST->cmp=cmp;
        pBST->getKey=(getKey!=NULL)?getKey:defaultGetKey;
    }
    return pBST;
}
static BOOL insert(BST_t *pBST,Node_t **ppNode,const void *pData,size_t size);
BOOL BST_insert(BST_t *pBST,const void *pData,size_t size)
{
    if(pBST==NULL||pData==NULL||size==0)
        return FALSE;
    return insert(pBST,&(pBST->pRoot),pData,size);
}
static BOOL insert(BST_t *pBST,Node_t **ppNode,const void *pData,size_t size)
{
    Node_t *pNode=*ppNode;

    if(pNode==NULL)
    {
        pNode=malloc(sizeof(Node_t)+size);
        if(pNode!=NULL)
        {
            pNode->left=NULL;
            pNode->right=NULL;
            pNode->size=size;
            memcpy(pNode->data,pData,size);
            *ppNode=pNode;
            return TRUE;
        }
        else
            return FALSE;

    }
    else
    {
        const void *key1=pBST->getKey(pData),
              *key2=pBST->getKey(pNode->data);
        if(pBST->cmp(key1,key2)<0)
            return insert(pBST,&(pNode->left),pData,size);
        else
            return insert(pBST,&(pNode->right),pData,size);
    }
}
static const void *search(BST_t *pBST,const Node_t*pNode,const void *pKey);
const void *BST_search(BST_t*pBST,const void *pKey)
{
    if(pBST==NULL||pKey==NULL)
        return NULL;
    return search(pBST,pBST->pRoot,pKey);
}
static const void *search(BST_t *pBST,const Node_t*pNode,const void *pKey)
{
    if(pNode==NULL)
        return NULL;
    else
    {
        const void *key=pBST->getKey(pNode->data);
        int cmp_res=pBST->cmp(pKey,key);
        if(cmp_res==0)
            return pNode->data;
        else if(cmp_res<0)
            return search(pBST,pNode->left,pKey);
        else
            return search(pBST,pNode->right,pKey);

    }
}
static Node_t*detachMin(Node_t **ppNode)
{
    Node_t *pNode=*ppNode;
    if(pNode==NULL)
        return NULL;
    else if(pNode->left!=NULL)
        return detachMin(&(pNode->left));
    else
    {
        *ppNode=pNode->right;
        return pNode;
    }

}
static BOOL erase(BST_t *pBST,Node_t**ppNode,const void *pKey);
BOOL BST_erase(BST_t*pBST,const void *pKey)
{
    if(pBST==NULL||pKey==NULL)
        return FALSE;
    else
        return erase(pBST,&(pBST->pRoot),pKey);
}
static BOOL erase(BST_t *pBST,Node_t**ppNode,const void *pKey)
{
    Node_t *pNode=*ppNode;
    if(pNode==NULL)
        return FALSE;
    int cmp_res=pBST->cmp(pKey,pBST->getKey(pNode->data));
    if(cmp_res<0)
        return erase(pBST,&(pNode->left),pKey);
    else if(cmp_res>0)
        return erase(pBST,&(pNode->right),pKey);
    else
    {
        if(pNode->left==NULL)
            *ppNode=pNode->right;
        else if(pNode->right=NULL)
            *ppNode=pNode->left;
        else
        {
            Node_t *pMin=detachMin(&(pNode->right));
            *ppNode=pMin;
            pMin->left=pNode->left;
            pMin->right=pNode->right;
        }
        free(pNode);
        return TRUE;
    }
}
static void clear(Node_t *pNode);

void BST_clear(BST_t *pBST)
{
    if(pBST!=NULL)
    {
        clear(pBST->pRoot);
        pBST->pRoot=NULL;
    }
}

static void clear(Node_t *pNode)
{
    if(pNode!=NULL)
    {
        clear(pNode->left);
        clear(pNode->right);
        free(pNode);
    }
}
static int inorder(Node_t *pNode,BOOL (*action)(void *pData));
int BST_inorder(BST_t *pBST,BOOL (*action)(void *pData))
{
    if(pBST==NULL||action==NULL)
        return 0;
    else
        return inorder(pBST->pRoot,action);

}
static int inorder(Node_t *pNode,BOOL (*action)(void *pData))
{
    int count=0;
    if(pNode==NULL)
        return 0;
    count=inorder(pNode->left,action);
    if(action(pNode->data))
        count++;
    count+=inorder(pNode->right,action);
    return count;
}

static int preorder(Node_t *pNode,BOOL (*action)(void *pData));
int BST_preorder(BST_t *pBST,BOOL (*action)(void *pData))
{
    if(pBST==NULL||action==NULL)
        return 0;
    else
        return preorder(pBST->pRoot,action);

}
static int preorder(Node_t *pNode,BOOL (*action)(void *pData))
{
    int count=0;
    if(pNode==NULL)
        return 0;
    if(action(pNode->data))
        count++;
    count=preorder(pNode->left,action);
    count+=preorder(pNode->right,action);
    return count;
}

static int postorder(Node_t *pNode,BOOL (*action)(void *pData));
int BST_postorder(BST_t *pBST,BOOL (*action)(void *pData))
{
    if(pBST==NULL||action==NULL)
        return 0;
    else
        return postorder(pBST->pRoot,action);

}
static int postorder(Node_t *pNode,BOOL (*action)(void *pData))
{
    int count=0;
    if(pNode==NULL)
        return 0;
    count=postorder(pNode->left,action);
    count+=postorder(pNode->right,action);
    if(action(pNode->data))
        count++;
    return count;
}
#define LEN_MAX 1000
char buffer[LEN_MAX];
BOOL printStr(void *str){
    return printf("%s",str)>=0;
}
int main(int argc,char**argv)
{
    BST_t *pStrTree=newBST((CmpFunc_t*)strcmp,NULL);
    int n;
    FILE* fp=fopen(argv[1],"r");
    while(fgets(buffer,LEN_MAX,fp)!=NULL)
    {
        size_t len=strlen(buffer);
        if(!BST_insert(pStrTree,buffer,len+1))
            break;
    }
    if(!feof(fp))
    {
        fprintf(stderr,"sortlines:Error reading or storing text input.\n");
        exit(EXIT_FAILURE);
    }
    fclose(fp);
    n=BST_inorder(pStrTree,printStr);
    fprintf(stderr,"\nsortlines:Printed %d lines.\n",n);
    BST_clear(pStrTree);
}
#endif