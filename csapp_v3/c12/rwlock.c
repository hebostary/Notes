#include "csapp.h"

#define NHASH 3
#define HASH(id) (((unsigned long)id)%NHASH)

typedef unsigned long taskid_t ;

struct task
{
    struct task *t_pre;
    struct task * t_next;

    pthread_t thread_id;
    taskid_t task_id;
    
    // more data
};

struct queue
{
    struct task *q_head;
    struct task *q_tail;
    pthread_rwlock_t q_lock;
};

int init_queue(struct queue *qp);
void insert_task(struct queue * qp, struct task * t);
void append_task(struct  queue *qp, struct task *t);
void remove_task(struct  queue *qp, struct task *t);
struct task *find_task(struct  queue *qp, pthread_t id);
struct task *fetch_one_task(struct  queue *qp);

static struct queue *taskQueue = NULL;

/*
* init a queue
*/
int init_queue(struct queue *qp) {
    int err = 1;

    if (qp == NULL) {
        return err;
    }

    qp->q_head = NULL;
    qp->q_tail = NULL;
    err = pthread_rwlock_init(&qp->q_lock, NULL);
    if (err != 0)
    {
        return err;
    }
    
    // do more initialization
    return 0;
}

/*
* insert a task to head of queue
*/
void insert_task(struct queue * qp, struct task * t) {
    if (qp == NULL || t == NULL) {
        return;
    }
    
    pthread_rwlock_wrlock(&qp->q_lock);
    t->t_next = qp->q_head;
    t->t_pre = NULL;

    if (qp->q_head != NULL)
    {
        qp->q_head->t_pre = t;
    } else { /* empty queue */
        qp->q_tail = t;
    }
    
    qp->q_head = t;
    pthread_rwlock_unlock(&qp->q_lock);
}


/*
* append a task to end of queue
*/
void append_task(struct  queue *qp, struct task *t) {
    if (qp == NULL || t == NULL) {
        return;
    }

    pthread_rwlock_wrlock(&qp->q_lock);

    t->t_pre =qp->q_tail;
    t->t_next = NULL;

    if (qp->q_tail != NULL)
    {
        qp->q_tail->t_next = t;    
    } else { /* empty queue */
        qp->q_head = t;
    }

    qp->q_tail = t;
    pthread_rwlock_unlock(&qp->q_lock);
}


/*
* Remove the given job from a queue
*/
void remove_task(struct  queue *qp, struct task *t) {
    if (qp == NULL || t == NULL) {
        return;
    }

    pthread_rwlock_wrlock(&qp->q_lock);
    if (qp->q_head == t) { //1. 移除的task在队列头部
        qp->q_head = t->t_next;
        if (qp->q_tail == t) { //队列里只有一个task的情况
            qp->q_tail = NULL;
        } else {
            t->t_next->t_pre = t->t_pre; //t->t_pre shoule be NULL
        }
    } else if (qp->q_tail == t) { //2 .移除的task在队列尾部的情况，并且不是队列头部的task
        qp->q_tail = t->t_pre;
        t->t_pre->t_next = t->t_next;
    } else { // 3.移除的task在队列中间的情况
        t->t_pre->t_next = t->t_next;
        t->t_next->t_pre = t->t_pre;
    }
    
    pthread_rwlock_unlock(&qp->q_lock);
}

/*
* Find a task for given thread id
*/
struct task *find_task(struct  queue *qp, pthread_t id) {
    struct task * tp;

    if (qp == NULL) {
        return NULL;
    }

    if (pthread_rwlock_rdlock(&qp->q_lock) != 0){
        return NULL;
    }

    for (tp = qp->q_head; tp != NULL; tp = tp->t_next) {
        if (pthread_equal(tp->thread_id, id)) {
            break; //found it
        }
    }

    pthread_rwlock_unlock(&qp->q_lock);
    return tp;
}

/*
* Fetch one task from head of task queue
*/
struct task *fetch_one_task(struct  queue *qp) {
    struct task * tp = NULL;

    if (qp == NULL) {
        return NULL;
    }

    pthread_rwlock_wrlock(&qp->q_lock);

    if(qp->q_head != NULL) {
        tp = qp->q_head;
        qp->q_head = tp->t_next;
        if(qp->q_head == NULL) { //队列中只有一个task，q_head和q_tail指向同一个指针
            qp->q_tail = NULL;
        } else { //队列中至少两个task
            tp->t_next->t_pre = tp->t_pre;
        }
    }

    pthread_rwlock_unlock(&qp->q_lock);
    return tp;
}

void *thread_task_worker(void *arg) {
    struct task *tp;
    pthread_t tid = pthread_self();

    pthread_detach(tid);

    while (1) {
        if((tp = fetch_one_task(taskQueue)) == NULL) {
            sleep(1);
            continue;
        } 

        // fetched one task
        printf("Worker[%x], handling task[%ld] begin.\n", (unsigned int)tid, (taskid_t)tp->task_id);
        sleep(1);
        printf("Worker[%x], handling task[%ld] end.\n", (unsigned int)tid, (taskid_t)tp->task_id);

        free(tp);
        tp = NULL;
    }
}

void *thread_task_creator(void *arg) {
    struct task *tp;
    size_t i;
    pthread_t tid = pthread_self();

   for (i = 0; i < 20; i++) {
       if((tp = malloc(sizeof(struct task))) == NULL) {
            printf("Creator[%x], failed to create task[%ld].\n", (unsigned int)tid, (taskid_t)i);
            continue;
       }

       tp->task_id = i;
       append_task(taskQueue, tp);
       printf("Creator[%x], created task[%ld].\n", (unsigned int)tid, (taskid_t)i);
       sleep(1);
       tp = NULL;
   }

   printf("creator thread exited.\n");
   return (void *)0;
}

int main() {
    int err;
    pthread_t tid;
    void *tret;
    size_t i;
    
    printf("Rwlock demo run begin.\n");
    if((taskQueue = (struct queue *)malloc(sizeof(struct queue))) == NULL) {
        printf("Failed to create task queue object.\n");
        exit(0);
    }
    if((err = init_queue(taskQueue)) != 0) {
        printf("Failed to init task queue, error:%d\n", err);
        exit(0);
    }

    for (i = 0; i < NHASH; i++) {
        if ((err = pthread_create(&tid, NULL, thread_task_worker, NULL)) != 0) {
            posix_error(err, "Create worker thread error.");
        }
    }
    
    if ((err = pthread_create(&tid, NULL, thread_task_creator, NULL)) != 0) {
        posix_error(err, "Create creator thread error.");
    }
    
    if ((err = pthread_join(tid, &tret)) != 0) {
        posix_error(err, "Can not join the creator thread.");
    }
    
    sleep(3); //waiiting work thread done
    
    pthread_mutex_destroy(&taskQueue->q_lock);
    free(taskQueue);
    exit(0);
}