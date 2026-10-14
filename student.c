#include "student.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <strings.h> /* for strcasecmp */

Student* student_create(const char *id, const char *name, int age,
                        const char *gender, const char *course, const char *phone,
                        int room_number, double total_fee, double amount_paid) {
    Student *s = (Student *)malloc(sizeof(Student));
    if (!s) {
        fprintf(stderr, "[Error] Memory allocation failed for Student node.\n");
        return NULL;
    }

    memset(s, 0, sizeof(Student));
    snprintf(s->id, sizeof(s->id), "%s", id ? id : "");
    snprintf(s->name, sizeof(s->name), "%s", name ? name : "");
    s->age = age;
    snprintf(s->gender, sizeof(s->gender), "%s", gender ? gender : "");
    snprintf(s->course, sizeof(s->course), "%s", course ? course : "");
    snprintf(s->phone, sizeof(s->phone), "%s", phone ? phone : "");
    s->room_number = room_number;
    s->total_fee = total_fee;
    s->amount_paid = amount_paid;
    s->pending_balance = total_fee - amount_paid;
    if (s->pending_balance < 0.0) {
        s->pending_balance = 0.0;
    }
    s->next = NULL;

    return s;
}

void student_free(Student *s) {
    if (s) {
        free(s);
    }
}

void student_free_list(Student **head) {
    if (!head) return;
    Student *current = *head;
    while (current) {
        Student *temp = current->next;
        free(current);
        current = temp;
    }
    *head = NULL;
}

Student* student_find_by_id(Student *head, const char *id) {
    if (!head || !id) return NULL;
    Student *curr = head;
    while (curr) {
        if (strcasecmp(curr->id, id) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

int student_id_exists(Student *head, const char *id) {
    return (student_find_by_id(head, id) != NULL);
}

int student_insert(Student **head, Student *new_student) {
    if (!head || !new_student) return 0;

    /* Prevent duplicate ID insertion */
    if (student_id_exists(*head, new_student->id)) {
        return 0;
    }

    new_student->next = NULL;

    if (*head == NULL) {
        *head = new_student;
        return 1;
    }

    Student *curr = *head;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = new_student;
    return 1;
}

int student_delete(Student **head, const char *id) {
    if (!head || !*head || !id) return 0;

    Student *curr = *head;
    Student *prev = NULL;

    while (curr) {
        if (strcasecmp(curr->id, id) == 0) {
            if (prev == NULL) {
                *head = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr);
            return 1;
        }
        prev = curr;
        curr = curr->next;
    }
    return 0;
}

int student_count(const Student *head) {
    int count = 0;
    const Student *curr = head;
    while (curr) {
        count++;
        curr = curr->next;
    }
    return count;
}

/* Helper to compare IDs naturally (supporting numeric and alphanumeric) */
static int compare_id(const char *id1, const char *id2) {
    /* If both are digits only, compare as integers */
    int all_digits1 = 1, all_digits2 = 1;
    for (int i = 0; id1[i]; i++) {
        if (!isdigit((unsigned char)id1[i])) { all_digits1 = 0; break; }
    }
    for (int i = 0; id2[i]; i++) {
        if (!isdigit((unsigned char)id2[i])) { all_digits2 = 0; break; }
    }

    if (all_digits1 && all_digits2) {
        long v1 = strtol(id1, NULL, 10);
        long v2 = strtol(id2, NULL, 10);
        if (v1 < v2) return -1;
        if (v1 > v2) return 1;
        return 0;
    }

    return strcasecmp(id1, id2);
}

/* Split linked list into two halves using slow/fast pointer */
static void split_list(Student *source, Student **front, Student **back) {
    Student *fast;
    Student *slow;
    slow = source;
    fast = source->next;

    while (fast != NULL) {
        fast = fast->next;
        if (fast != NULL) {
            slow = slow->next;
            fast = fast->next;
        }
    }

    *front = source;
    *back = slow->next;
    slow->next = NULL;
}

/* Merge two sorted lists by ID */
static Student* merge_by_id_helper(Student *a, Student *b) {
    if (!a) return b;
    if (!b) return a;

    Student *result = NULL;
    if (compare_id(a->id, b->id) <= 0) {
        result = a;
        result->next = merge_by_id_helper(a->next, b);
    } else {
        result = b;
        result->next = merge_by_id_helper(a, b->next);
    }
    return result;
}

/* Merge Sort for ID */
static void merge_sort_id(Student **head_ref) {
    Student *head = *head_ref;
    if (!head || !head->next) return;

    Student *a = NULL;
    Student *b = NULL;

    split_list(head, &a, &b);

    merge_sort_id(&a);
    merge_sort_id(&b);

    *head_ref = merge_by_id_helper(a, b);
}

void student_sort_by_id(Student **head) {
    if (head && *head) {
        merge_sort_id(head);
    }
}

/* Merge two sorted lists by Name */
static Student* merge_by_name_helper(Student *a, Student *b) {
    if (!a) return b;
    if (!b) return a;

    Student *result = NULL;
    if (strcasecmp(a->name, b->name) <= 0) {
        result = a;
        result->next = merge_by_name_helper(a->next, b);
    } else {
        result = b;
        result->next = merge_by_name_helper(a, b->next);
    }
    return result;
}

/* Merge Sort for Name */
static void merge_sort_name(Student **head_ref) {
    Student *head = *head_ref;
    if (!head || !head->next) return;

    Student *a = NULL;
    Student *b = NULL;

    split_list(head, &a, &b);

    merge_sort_name(&a);
    merge_sort_name(&b);

    *head_ref = merge_by_name_helper(a, b);
}

void student_sort_by_name(Student **head) {
    if (head && *head) {
        merge_sort_name(head);
    }
}

void student_display_table(const Student *head) {
    if (!head) {
        printf("\n  [i] No student records found. The student list is currently empty.\n");
        return;
    }

    printf("\n+----------+----------------------+-----+--------+----------------------+---------------+--------+------------+------------+------------+\n");
    printf("| %-8s | %-20s | %-3s | %-6s | %-20s | %-13s | %-6s | %-10s | %-10s | %-10s |\n",
           "ID", "Name", "Age", "Gender", "Course & Sem", "Phone", "Room", "Total Fee", "Paid", "Balance");
    printf("+----------+----------------------+-----+--------+----------------------+---------------+--------+------------+------------+------------+\n");

    const Student *curr = head;
    while (curr) {
        char room_str[16];
        if (curr->room_number > 0) {
            snprintf(room_str, sizeof(room_str), "R-%d", curr->room_number);
        } else {
            snprintf(room_str, sizeof(room_str), "None");
        }

        printf("| %-8s | %-20.20s | %-3d | %-6.6s | %-20.20s | %-13.13s | %-6s | %10.2f | %10.2f | %10.2f |\n",
               curr->id, curr->name, curr->age, curr->gender, curr->course, curr->phone,
               room_str, curr->total_fee, curr->amount_paid, curr->pending_balance);

        curr = curr->next;
    }

    printf("+----------+----------------------+-----+--------+----------------------+---------------+--------+------------+------------+------------+\n");
    printf("  Total Students: %d\n", student_count(head));
}

void student_display_card(const Student *s) {
    if (!s) return;

    printf("\n+-------------------------------------------------------------+\n");
    printf("|                    STUDENT PROFILE RECORD                   |\n");
    printf("+-------------------------------------------------------------+\n");
    printf("|  Student ID      : %-40s |\n", s->id);
    printf("|  Full Name       : %-40s |\n", s->name);
    printf("|  Age             : %-40d |\n", s->age);
    printf("|  Gender          : %-40s |\n", s->gender);
    printf("|  Course & Sem    : %-40s |\n", s->course);
    printf("|  Phone Number    : %-40s |\n", s->phone);
    if (s->room_number > 0) {
        char rstr[32];
        snprintf(rstr, sizeof(rstr), "Room %d (Allocated)", s->room_number);
        printf("|  Hostel Room     : %-40s |\n", rstr);
    } else {
        printf("|  Hostel Room     : %-40s |\n", "Not Allocated (Waitlist/None)");
    }
    printf("+-------------------------------------------------------------+\n");
    printf("|  Total Fee       : Rs. %-36.2f |\n", s->total_fee);
    printf("|  Amount Paid     : Rs. %-36.2f |\n", s->amount_paid);
    printf("|  Pending Balance : Rs. %-36.2f |\n", s->pending_balance);
    printf("+-------------------------------------------------------------+\n");
}
