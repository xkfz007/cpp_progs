/*
 * this is the reader and producer problem.
 */
#ifdef _CONSUMER_PRODUCER
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
struct msg{
    struct msg*next;
    int num;
};
struct msg *head;
pthread_cond_t has_product=PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;
void *consumer(void*p)
{
    struct msg*mp;
    for(;;){
        pthread_mutex_lock(&lock);
        while(head==NULL)
            pthread_cond_wait(&has_product,&lock);
        mp=head;
        head=mp->next;
        pthread_mutex_unlock(&lock);
        printf("Consume %d\n",mp->num);
        free(mp);
        sleep(rand()%5);
    }
}
void *producer(void*p)
{
    struct msg *mp;
    for(;;){
        mp=malloc(sizeof(struct msg));
        mp->num=rand()%1000+1;
        printf("Produce %d\n",mp->num);
        pthread_mutex_lock(&lock);
        mp->next=head;
        head=mp;
        pthread_mutex_unlock(&lock);
        pthread_cond_signal(&has_product);
        sleep(rand()%5);
    }
}
int main()
{
    pthread_t pid,cid;
    srand(time(NULL));
    pthread_create(&pid,NULL,producer,NULL);
    pthread_create(&cid,NULL,consumer,NULL);
    pthread_join(pid,NULL);
    pthread_join(cid,NULL);

    return 0;
}
#endif

/*
 * this reader-producer problem uses semaphore.
 */
#ifdef _CONSUMER_PRODUCER_SEMAPHORE
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#define NUM 5
int queue[NUM];
sem_t blank_num,product_num;
void *producer(void *arg)
{
    int p=0;
    while(1){
        sem_wait(&blank_num);
        queue[p]=rand()%1000+1;
        printf("Produce %d\n",queue[p]);
        sem_post(&product_num);
        p=(p+1)%NUM;
        sleep(rand()%5);
    }
}
void *consumer(void *arg)
{
    int c=0;
    while(1){
        sem_wait(&product_num);
        printf("Consume %d\n",queue[c]);
        queue[c]=0;
        sem_post(&blank_num);
        c=(c+1)%NUM;
        sleep(rand()%5);
    }
}
int main()
{
    pthread_t pid,cid;
    sem_init(&blank_num,0,NUM);
    sem_init(&product_num,0,0);
    pthread_create(&pid,NULL,producer,NULL);
    pthread_create(&pid,NULL,consumer,NULL);
    pthread_join(pid,NULL);
    pthread_join(cid,NULL);
    sem_destroy(&blank_num);
    sem_destroy(&product_num);

    return 0;
}
#endif
