/**
 * threadpool.c
 *
 * This file will contain your implementation of a threadpool.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <assert.h>
#include "threadpool.h"


typedef struct node{
   dispatch_fn d;
   void *arg;
   struct node *next;

}_node;

typedef struct queue{
   _node *head;
   _node *tail;
   int sz;
}_queue;

void init_queue(_queue *q){
   q->head = NULL;
   q->tail = NULL;
   q->sz = 0;

}
int queue_empty(_queue *q){
   //if(q->sz = 0) return 0;
   //return 1;
   //assert(q->sz >= 0);
   return(q->sz == 0);
   //return(q->head == q->tail);
}
void enqueue(_queue *q, _node *n){
   // _node *ne = n;
   if(!n){
     printf("Not enough memory");
     return;
   }
   n->next = NULL;

   if(queue_empty(q)){
     q->head = n;
     q->tail = n;
     
   } else{
     q->tail->next = n;
     q->tail = n;
     
   }
   q->sz++;
   return;
}
_node * dequeue(_queue *q){
   _node *o;
  // o = (_node*)malloc(sizeof(_node));
   assert(!queue_empty(q));

   if(q->head == q->tail){
      o = q->head;
      q->head = NULL;
      q->tail = NULL;
   }else{
      o = q->head;
      q->head = q->head->next;
   }
   q->sz--;
      return o;

}

// _threadpool is the internal threadpool structure that is
// cast to type "threadpool" before it given out to callers
typedef struct _threadpool_st {
   // you should fill in this structure with whatever you need
   _queue *q;
   int tcount;
   int shutdown;
   pthread_t *t;
   pthread_mutex_t m;
   pthread_cond_t  cond;
} _threadpool;

void * thread_main(threadpool from_me){
  _threadpool *pool = (_threadpool *) from_me;
  _node *temp = (_node*)malloc(sizeof(_node));
  for(;;){
    pthread_mutex_lock(&(pool->m));
    while(queue_empty(pool->q) && (pool->shutdown == 0))     {
         pthread_cond_wait(&(pool->cond), &(pool->m));
   }
     if (pool->shutdown == 1){
      pthread_mutex_unlock(&(pool->m));
      break;
     }

 temp = NULL;
     if (!queue_empty(pool->q)) {
      temp = dequeue(pool->q);
      //pool->tcount--;
      }

      pthread_mutex_unlock(&(pool->m));
      //execute task function
     if (temp){
     (temp->d) (temp->arg);
      free(temp);
      }
    
  }
  
  pthread_exit(NULL);
}

threadpool create_threadpool(int num_threads_in_pool) {
  _threadpool *pool;
  int i;
  // sanity check the argument
  
  if ((num_threads_in_pool <= 0) || (num_threads_in_pool > MAXT_IN_POOL))
    return NULL;

  pool = (_threadpool *) malloc(sizeof(_threadpool));
  if (pool == NULL) {
    fprintf(stderr, "Out of memory creating a new threadpool!\n");
    return NULL;
  }

  // add your code here to initialize the newly created threadpool
  //memset(pool, 0, sizeof(pool));
  pool->t = (pthread_t*)malloc(sizeof(pthread_t) * num_threads_in_pool);
  pool->q = (_queue*)malloc(sizeof(_queue));
  pool->shutdown = 0;
  pool->tcount = num_threads_in_pool;
  init_queue(pool->q);
  if(pthread_mutex_init(&(pool->m), NULL)){
     fprintf(stderr, "Error creating threadpool\n");
     return NULL;
  }
  if(pthread_cond_init(&(pool->cond), NULL)){
     fprintf(stderr, "Error creating threadpool\n");
     return NULL;
  }

  for(i = 0; i < num_threads_in_pool; i++){ 
    if(pthread_create(&(pool->t[i]),NULL, thread_main, pool )){
      fprintf(stderr, "Error creating threadpool\n");
    return NULL;
    }   
  }
  return (threadpool) pool;
}


void dispatch(threadpool from_me, dispatch_fn dispatch_to_here,
	      void *arg) {
  _threadpool *pool = (_threadpool *) from_me;

  // add your code here to dispatch a thread
  _node *t =(_node*)malloc(sizeof(_node));
  if(t == NULL){
    fprintf(stderr, "No memory to create node\n");
    return;
  }
  pthread_mutex_lock(&(pool->m));
  t->d = dispatch_to_here;
  t->arg = arg;
  t->next = NULL;
  enqueue(pool->q, t);
  pthread_cond_signal(&(pool->cond));
  pthread_mutex_unlock(&(pool->m));
}

void destroy_threadpool(threadpool destroyme) {
  _threadpool *pool = (_threadpool *) destroyme;

  // add your code here to kill a threadpool
  void* n;
   int i = 0;
  pthread_mutex_lock(&pool->m);
  pool->shutdown = 1;
  pthread_cond_broadcast(&(pool->cond));
  for(;i< pool->tcount; i++){
     pthread_join(pool->t[i],&n);
  }
  pthread_mutex_destroy(&(pool->m));
  pthread_cond_destroy(&(pool->cond));
  free(pool);
  return;

}

