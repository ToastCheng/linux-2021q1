#include "list_sort.h"
#include "list.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

void gen_person(struct list_head *head)
{
    FILE *fp = fopen("data/person.txt", "r");
    char ln[10];
    char fn[10];
    int a, bm, bd;
    int r = fscanf(fp, "%s %s %d %d %d\n", ln, fn, &a, &bm, &bd);
    while (r != EOF) {
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


int main() {
    LIST_HEAD(head);
    INIT_LIST_HEAD(&head);

    gen_person(&head);

    // printf("sort by name: \n");
    // list_sort(NULL, &head, sort_by_name);
    // display(&head);

    printf("sort by age: \n");
    list_sort(NULL, &head, sort_by_age);
    display(&head);

    // printf("sort by birthday: \n");
    // list_sort(NULL, &head, sort_by_birthday);
    // display(&head);
}