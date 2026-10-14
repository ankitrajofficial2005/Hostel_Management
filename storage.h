#ifndef STORAGE_H
#define STORAGE_H

#include "student.h"
#include "room.h"
#include "queue.h"

#define DATA_DIR "data"
#define STUDENTS_FILE "data/students.txt"
#define ROOMS_FILE "data/rooms.txt"
#define QUEUE_FILE "data/waiting_list.txt"

/* Ensure data directory exists */
int storage_init_dir(void);

/* Save all student linked list records to disk */
int storage_save_students(const char *filepath, const Student *head);

/* Load student records from disk into singly linked list */
int storage_load_students(const char *filepath, Student **head);

/* Save rooms array to disk */
int storage_save_rooms(const char *filepath, const Room rooms[], int count);

/* Load rooms array from disk */
int storage_load_rooms(const char *filepath, Room rooms[], int *count, int max_rooms);

/* Save waiting list queue to disk */
int storage_save_queue(const char *filepath, const WaitingQueue *q);

/* Load waiting list queue from disk */
int storage_load_queue(const char *filepath, WaitingQueue *q);

/* Convenience functions to load/save all subsystems */
int storage_load_all(Student **student_head, Room rooms[], int *room_count, int max_rooms, WaitingQueue *q);
int storage_save_all(const Student *student_head, const Room rooms[], int room_count, const WaitingQueue *q);

#endif /* STORAGE_H */
