#include "list_sort.h"
#include "list.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_TEST 6

// call this function to start a nanosecond-resolution timer
struct timespec timer_start(){
    struct timespec start_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_time);
    return start_time;
}

// call this function to end a timer, returning nanoseconds elapsed as a long
long timer_end(struct timespec start_time){
    struct timespec end_time;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_time);
    long diffInNanos = (end_time.tv_sec - start_time.tv_sec) * (long)1e9 + (end_time.tv_nsec - start_time.tv_nsec);
    return diffInNanos;
}

typedef struct {
    char *first_name;
    char *last_name;
    int age;
    int birth_month;
    int birth_date;
    struct list_head list;
} person;

int sort_by_name(void *priv, struct list_head *a, struct list_head *b)
{
    person *pa = list_entry(a, person, list);
    person *pb = list_entry(b, person, list);
    int diff = strcmp(pa->last_name, pb->last_name);
    if (diff)
        return diff;
    diff = strcmp(pa->first_name, pb->first_name);
    return diff;
}

int sort_by_age(void *priv, struct list_head *a, struct list_head *b)
{
    person *pa = list_entry(a, person, list);
    person *pb = list_entry(b, person, list);
    return pa->age > pb->age;
}

int sort_by_birthday(void *priv, struct list_head *a, struct list_head *b)
{
    person *pa = list_entry(a, person, list);
    person *pb = list_entry(b, person, list);
    if (pa->birth_month != pb->birth_month)
        return pa->birth_month > pb->birth_month;
    return pa->birth_date > pb->birth_date;
}

void gen_person(struct list_head *head, int cnt)
{
    FILE *fp = fopen("data/person_big.txt", "r");
    char ln[10];
    char fn[10];
    int a, bm, bd;
    int r = fscanf(fp, "%s %s %d %d %d\n", ln, fn, &a, &bm, &bd);

    while (r != EOF && cnt-- > 0) {
        // printf("%s %s %d %d %d\n", ln, fn, a, bm, bd);
        person *p = malloc(sizeof(person));
        p->last_name = strdup(ln);
        p->first_name = strdup(fn);
        p->age = a;
        p->birth_month = bm;
        p->birth_date = bd;
        list_add_tail(&p->list, head);
        r = fscanf(fp, "%s %s %d %d %d\n", ln, fn, &a, &bm, &bd);
    }
    fclose(fp);
}

void display(struct list_head *head)
{
    struct list_head *it = NULL;
    list_for_each(it, head) {
        person *p = list_entry(it, person, list);
        printf("%s%s age: %d birthday: %d/%d\n", p->last_name, p->first_name, p->age, p->birth_month, p->birth_date);
    }
}

int main(int argc, char *argv[]) {
    LIST_HEAD(head);


    FILE *fp = fopen("data/bm.txt", "a");
    int num = 1;
    for (int i = 0; i < NUM_TEST; i++) {
        INIT_LIST_HEAD(&head);
        gen_person(&head, num);
        num *= 10;
        struct timespec vartime = timer_start();
        list_sort(NULL, &head, sort_by_name);
        long time_elapsed_nanos = timer_end(vartime);
        // printf("%ld\n", time_elapsed_nanos);
        
        if (i < NUM_TEST - 1) {
            fprintf(fp, "%ld\t", time_elapsed_nanos);
        } else {
            fprintf(fp, "%ld\n", time_elapsed_nanos);
        }
    }
    fclose(fp);
    
}