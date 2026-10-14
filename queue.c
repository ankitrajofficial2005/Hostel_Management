#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

void queue_init(WaitingQueue *q) {
    if (!q) return;
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
}

int queue_is_empty(const WaitingQueue *q) {
    if (!q) return 1;
    return (q->front == NULL);
}

int queue_contains(const WaitingQueue *q, const char *student_id) {
    if (!q || !student_id) return 0;
    QueueNode *curr = q->front;
    while (curr) {
        if (strcasecmp(curr->student_id, student_id) == 0) {
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

int queue_enqueue(WaitingQueue *q, const char *student_id, const char *student_name, const char *request_date) {
    if (!q || !student_id) return 0;

    /* Prevent duplicate entries in the waiting list */
    if (queue_contains(q, student_id)) {
        return 0;
    }

    QueueNode *node = (QueueNode *)malloc(sizeof(QueueNode));
    if (!node) {
        fprintf(stderr, "[Error] Memory allocation failed for QueueNode.\n");
        return 0;
    }

    snprintf(node->student_id, sizeof(node->student_id), "%s", student_id);
    snprintf(node->student_name, sizeof(node->student_name), "%s", student_name ? student_name : "");
    snprintf(node->request_date, sizeof(node->request_date), "%s", request_date ? request_date : "");
    node->next = NULL;

    if (q->rear == NULL) {
        q->front = node;
        q->rear = node;
    } else {
        q->rear->next = node;
        q->rear = node;
    }

    q->count++;
    return 1;
}

int queue_dequeue(WaitingQueue *q, char *out_id, char *out_name) {
    if (!q || queue_is_empty(q)) return 0;

    QueueNode *temp = q->front;

    if (out_id) {
        snprintf(out_id, MAX_ID_LEN, "%s", temp->student_id);
    }
    if (out_name) {
        snprintf(out_name, MAX_NAME_LEN, "%s", temp->student_name);
    }

    q->front = q->front->next;
    if (q->front == NULL) {
        q->rear = NULL;
    }

    free(temp);
    q->count--;
    return 1;
}

int queue_peek(const WaitingQueue *q, char *out_id, char *out_name) {
    if (!q || queue_is_empty(q)) return 0;

    if (out_id) {
        snprintf(out_id, MAX_ID_LEN, "%s", q->front->student_id);
    }
    if (out_name) {
        snprintf(out_name, MAX_NAME_LEN, "%s", q->front->student_name);
    }

    return 1;
}

int queue_remove_by_id(WaitingQueue *q, const char *student_id) {
    if (!q || queue_is_empty(q) || !student_id) return 0;

    QueueNode *curr = q->front;
    QueueNode *prev = NULL;

    while (curr) {
        if (strcasecmp(curr->student_id, student_id) == 0) {
            if (prev == NULL) {
                q->front = curr->next;
            } else {
                prev->next = curr->next;
            }

            if (curr == q->rear) {
                q->rear = prev;
            }

            free(curr);
            q->count--;
            return 1;
        }
        prev = curr;
        curr = curr->next;
    }

    return 0;
}

void queue_display(const WaitingQueue *q, const Student *student_head) {
    if (!q || queue_is_empty(q)) {
        printf("\n  [i] The waiting list is currently empty.\n");
        return;
    }

    printf("\n+-------+----------+----------------------+----------------------+---------------------+\n");
    printf("| %-5s | %-8s | %-20s | %-20s | %-19s |\n",
           "Pos", "ID", "Student Name", "Course & Sem", "Requested At");
    printf("+-------+----------+----------------------+----------------------+---------------------+\n");

    QueueNode *curr = q->front;
    int pos = 1;
    while (curr) {
        /* Lookup student record for course info */
        const Student *s = student_head;
        const char *course = "N/A";
        while (s) {
            if (strcasecmp(s->id, curr->student_id) == 0) {
                course = s->course;
                break;
            }
            s = s->next;
        }

        printf("| #%-4d | %-8s | %-20.20s | %-20.20s | %-19.19s |\n",
               pos, curr->student_id, curr->student_name, course, curr->request_date);
        pos++;
        curr = curr->next;
    }

    printf("+-------+----------+----------------------+----------------------+---------------------+\n");
    printf("  Total Students on Waiting List: %d\n", q->count);
}

void queue_free(WaitingQueue *q) {
    if (!q) return;
    char id[MAX_ID_LEN];
    while (queue_dequeue(q, id, NULL)) {
        /* continuously dequeue until empty */
    }
}
