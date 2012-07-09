#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/select.h>
#include "adlist.h"

typedef void* (*fp)(void*);
enum Task_Type {Repeat, Once};

typedef struct Tw_t {
    void **wheels;
    int wheels_size;
    int index;
    int granularity;
    int rotation_count;
} Tw;

typedef struct Task_t {
    enum Task_Type tt;
    int first;
    int interval;
    int rotation_count;
    fp func;
    void* arg;
} Task;

Tw* init(int wheels_size, int granularity) {

    Tw *tw = malloc(sizeof(*tw));
    tw->index = 0;
    tw->granularity = granularity;
    tw->rotation_count = 0;
    tw->wheels_size = wheels_size;
    tw->wheels = calloc(sizeof(void*), wheels_size);
    return tw;
}

Task* add(Tw *tw, int first, int interval, fp func, void* arg, enum Task_Type tt) {

    Task *task = malloc(sizeof(*task));

    int index = (tw->index + (first / tw->granularity)) % tw->wheels_size;
    int rotation_count = (tw->index + (first / tw->granularity)) / tw->wheels_size;

    task->tt = tt;
    task->first = first;
    task->interval = interval / tw->granularity;
    task->rotation_count = rotation_count;
    task->func = func;
    task->arg = arg;

    if (NULL == tw->wheels[index]) {
        list *l = listCreate();
        tw->wheels[index] = l;
    }
    listAddNodeHead(tw->wheels[index], task);

    return task;
}

struct timeval to_tv(int ms) {

    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = ms % 1000 * 1000;
    return tv;
}

int select_sleep (int ms) {

    struct timeval tv = to_tv(ms);
    while(1) {  
        if ((tv.tv_sec == 0) && (tv.tv_usec == 0)) {   
            break;
        }   
        select(0, NULL, NULL, NULL, &tv);
    }
}

void do_task(Tw *tw, listNode *node) {
   
    Task *task = listNodeValue(node);
    int next_index;
    if (task->rotation_count == tw->rotation_count) {
        /* this round */
        task->func(task->arg);

        listDelNode(tw->wheels[tw->index], node);

        if (task->tt == Repeat) {
            next_index = (tw->index + task->interval) % tw->wheels_size; 
            task->rotation_count = tw->rotation_count + (tw->index + task->interval) / tw->wheels_size; 

            if (NULL == tw->wheels[next_index]) {
                list *l = listCreate();
                tw->wheels[next_index] = l;
            }
            listAddNodeHead(tw->wheels[next_index], task);
        }
    }
}

void do_tasks(Tw *tw, list* tasks) {

    listIter *iter = listGetIterator(tasks, AL_START_HEAD);
    listNode *node;
    while ((node = listNext(iter)) != NULL) {
        do_task(tw, node);
    }
    listReleaseIterator(iter);
}

Task *start(Tw *tw) {

    while(1) {
        list *tasks = tw->wheels[tw->index];
        if (NULL != tasks) {
            do_tasks(tw, tasks); 
        }
        if (++tw->index == tw->wheels_size) {
            tw->index = 0;
            tw->rotation_count++;
        }
        select_sleep(tw->granularity);
    }
}
void* a1(void *arg) {

    int id = (int)(arg);
    printf("task %d %d\n", id, time(NULL));
    return NULL;
}

/* arg is tw */
void* p1(void *arg) {
   
    Tw *tw = arg; 
    int index = 0;
    Task *task;
    listNode *node;

    for(; index < tw->wheels_size; index++) {
        list *tasks = tw->wheels[index];
        if (NULL != tasks) {
            listIter *iter = listGetIterator(tasks, AL_START_HEAD);
            while ((node = listNext(iter)) != NULL) {
                task = listNodeValue(node);
                printf("%d %p %p\n", task->interval, task->func, task->arg);
            }
            listReleaseIterator(iter);
        }
    }
    return NULL;
}

int main() {

    Tw *tw = init(60, 200);
    add(tw, 1000, 2000, a1, (void*)1, Repeat); 
    add(tw, 100000, 200000, a1, (void*)2, Once); 
    add(tw, 0, 10000, p1, tw, Repeat); 
    start(tw);
}

