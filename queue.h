#ifndef QUEUE_H
#define QUEUE_H

#include "student.h"

typedef struct QueueNode {
    char student_id[MAX_ID_LEN];
    char student_name[MAX_NAME_LEN];
    char request_date[32];
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *front;
    QueueNode *rear;
    int count;
} WaitingQueue;

/* Initialize an empty waiting queue */
void queue_init(WaitingQueue *q);

/* Check if queue is empty (returns 1 if empty, 0 otherwise) */
int queue_is_empty(const WaitingQueue *q);

/* Check if a student ID is already present in the waiting queue */
int queue_contains(const WaitingQueue *q, const char *student_id);

/* Enqueue a student onto the waiting list (FIFO rear insertion) */
int queue_enqueue(WaitingQueue *q, const char *student_id, const char *student_name, const char *request_date);

/* Dequeue the front student (FIFO front deletion) */
int queue_dequeue(WaitingQueue *q, char *out_id, char *out_name);

/* Peek at the front student without removing */
int queue_peek(const WaitingQueue *q, char *out_id, char *out_name);

/* Remove a specific student ID from anywhere in the waiting queue */
int queue_remove_by_id(WaitingQueue *q, const char *student_id);

/* Display formatted table of the waiting list */
void queue_display(const WaitingQueue *q, const Student *student_head);

/* Free all nodes in the waiting queue */
void queue_free(WaitingQueue *q);

#endif /* QUEUE_H */
