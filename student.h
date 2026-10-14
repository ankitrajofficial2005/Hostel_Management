#ifndef STUDENT_H
#define STUDENT_H

#include <stddef.h>

#define MAX_ID_LEN 32
#define MAX_NAME_LEN 64
#define MAX_GENDER_LEN 16
#define MAX_COURSE_LEN 64
#define MAX_PHONE_LEN 20

/* Student structure representing a node in the singly linked list */
typedef struct Student {
    char id[MAX_ID_LEN];             /* Unique Student ID (e.g. STU101) */
    char name[MAX_NAME_LEN];         /* Student Full Name */
    int age;                         /* Student Age (16 - 100) */
    char gender[MAX_GENDER_LEN];     /* Gender (e.g. Male/Female/Other) */
    char course[MAX_COURSE_LEN];     /* Course & Semester (e.g. B.Tech CSE Sem-4) */
    char phone[MAX_PHONE_LEN];       /* Contact Phone Number */
    int room_number;                 /* Allocated room number (0 = Unallocated) */
    double total_fee;                /* Total hostel fee */
    double amount_paid;              /* Amount already paid */
    double pending_balance;          /* total_fee - amount_paid */
    struct Student *next;            /* Pointer to next student node */
} Student;

/* Allocate a new Student node on the heap */
Student* student_create(const char *id, const char *name, int age,
                        const char *gender, const char *course, const char *phone,
                        int room_number, double total_fee, double amount_paid);

/* Free an individual student node */
void student_free(Student *s);

/* Free the entire linked list of students */
void student_free_list(Student **head);

/* Search student by ID (returns pointer to node or NULL if not found) */
Student* student_find_by_id(Student *head, const char *id);

/* Check if an ID already exists in the list (1 = exists, 0 = does not) */
int student_id_exists(Student *head, const char *id);

/* Insert a student at the end of the singly linked list */
int student_insert(Student **head, Student *new_student);

/* Delete a student by ID from the list. Returns 1 if deleted, 0 if not found. */
int student_delete(Student **head, const char *id);

/* Count total students in the list */
int student_count(const Student *head);

/* Merge Sort on Singly Linked List: O(N log N) */
void student_sort_by_id(Student **head);
void student_sort_by_name(Student **head);

/* UI Display functions */
void student_display_table(const Student *head);
void student_display_card(const Student *s);

#endif /* STUDENT_H */
