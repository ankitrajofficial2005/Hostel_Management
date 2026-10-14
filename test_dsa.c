#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "student.h"
#include "room.h"
#include "queue.h"
#include "stack.h"
#include "storage.h"

static int tests_passed = 0;
static int tests_total = 0;

#define TEST_ASSERT(cond, msg) do { \
    tests_total++; \
    if (cond) { \
        tests_passed++; \
        printf("  [PASS] %s\n", msg); \
    } else { \
        printf("  [FAIL] %s (Line %d)\n", msg, __LINE__); \
    } \
} while(0)

void test_student_linked_list(void) {
    printf("\n--- Testing Student Singly Linked List & Merge Sort ---\n");
    Student *head = NULL;

    TEST_ASSERT(student_count(head) == 0, "Empty list count is 0");
    TEST_ASSERT(student_find_by_id(head, "S1") == NULL, "Search in empty list returns NULL");

    /* Insert students */
    Student *s1 = student_create("S103", "Charlie Brown", 20, "Male", "CSE", "9876543210", 0, 50000.0, 20000.0);
    Student *s2 = student_create("S101", "Alice Smith", 19, "Female", "ECE", "9876543211", 0, 55000.0, 55000.0);
    Student *s3 = student_create("S102", "Bob Davis", 21, "Male", "ME", "9876543212", 0, 60000.0, 0.0);

    TEST_ASSERT(student_insert(&head, s1) == 1, "Insert s1 into list");
    TEST_ASSERT(student_insert(&head, s2) == 1, "Insert s2 into list");
    TEST_ASSERT(student_insert(&head, s3) == 1, "Insert s3 into list");
    TEST_ASSERT(student_count(head) == 3, "List has 3 students");

    /* Duplicate rejection */
    Student *dup = student_create("S101", "Duplicate Alice", 22, "Female", "Civil", "111", 0, 50000.0, 0.0);
    TEST_ASSERT(student_insert(&head, dup) == 0, "Duplicate ID rejected");
    student_free(dup);

    /* Search */
    Student *found = student_find_by_id(head, "s102"); /* case insensitive */
    TEST_ASSERT(found != NULL && strcmp(found->name, "Bob Davis") == 0, "Case-insensitive search found Bob");
    TEST_ASSERT(found->pending_balance == 60000.0, "Bob pending balance correctly calculated");

    /* Sort by ID */
    student_sort_by_id(&head);
    TEST_ASSERT(strcmp(head->id, "S101") == 0, "Sorted by ID: 1st is S101");
    TEST_ASSERT(strcmp(head->next->id, "S102") == 0, "Sorted by ID: 2nd is S102");
    TEST_ASSERT(strcmp(head->next->next->id, "S103") == 0, "Sorted by ID: 3rd is S103");

    /* Sort by Name */
    student_sort_by_name(&head);
    TEST_ASSERT(strcmp(head->name, "Alice Smith") == 0, "Sorted by Name: 1st is Alice Smith");
    TEST_ASSERT(strcmp(head->next->name, "Bob Davis") == 0, "Sorted by Name: 2nd is Bob Davis");
    TEST_ASSERT(strcmp(head->next->next->name, "Charlie Brown") == 0, "Sorted by Name: 3rd is Charlie Brown");

    /* Delete */
    TEST_ASSERT(student_delete(&head, "S102") == 1, "Delete Bob Davis (S102)");
    TEST_ASSERT(student_count(head) == 2, "List now has 2 students");
    TEST_ASSERT(student_find_by_id(head, "S102") == NULL, "S102 not in list after deletion");
    TEST_ASSERT(student_delete(&head, "NON_EXIST") == 0, "Deleting non-existent student returns 0");

    student_free_list(&head);
    TEST_ASSERT(head == NULL, "student_free_list properly resets head to NULL");
}

void test_room_array(void) {
    printf("\n--- Testing Room Array & Occupancy Management ---\n");
    Room rooms[MAX_ROOMS];
    int count = 0;

    room_init_defaults(rooms, &count);
    TEST_ASSERT(count == 12, "Default 12 rooms initialized");
    TEST_ASSERT(rooms[0].room_number == 101, "First room is 101");
    TEST_ASSERT(rooms[0].capacity == 2, "Room 101 capacity is 2");
    TEST_ASSERT(rooms[0].occupancy == 0, "Room 101 initial occupancy is 0");
    TEST_ASSERT(rooms[0].status == ROOM_AVAILABLE, "Room 101 status is AVAILABLE");

    /* Allocate bed 1 */
    TEST_ASSERT(room_allocate_bed(rooms, count, 101) == 1, "Allocate bed 1 in room 101");
    Room *r101 = room_find(rooms, count, 101);
    TEST_ASSERT(r101->occupancy == 1 && r101->status == ROOM_PARTIAL, "Room 101 is now PARTIAL (1/2)");

    /* Allocate bed 2 */
    TEST_ASSERT(room_allocate_bed(rooms, count, 101) == 1, "Allocate bed 2 in room 101");
    TEST_ASSERT(r101->occupancy == 2 && r101->status == ROOM_FULL, "Room 101 is now FULL (2/2)");

    /* Attempt over-allocation */
    TEST_ASSERT(room_allocate_bed(rooms, count, 101) == 0, "Over-allocation rejected for full room");

    /* First available */
    int next_avail = room_find_first_available(rooms, count);
    TEST_ASSERT(next_avail == 102, "Next available room is 102");

    /* Vacate */
    TEST_ASSERT(room_vacate_bed(rooms, count, 101) == 1, "Vacated 1 bed in room 101");
    TEST_ASSERT(r101->occupancy == 1 && r101->status == ROOM_PARTIAL, "Room 101 is back to PARTIAL");

    TEST_ASSERT(room_vacate_bed(rooms, count, 101) == 1, "Vacated remaining bed in room 101");
    TEST_ASSERT(r101->occupancy == 0 && r101->status == ROOM_AVAILABLE, "Room 101 is AVAILABLE");

    TEST_ASSERT(room_vacate_bed(rooms, count, 101) == 0, "Vacating an empty room rejected");
}

void test_waiting_queue(void) {
    printf("\n--- Testing Waiting List FIFO Queue ---\n");
    WaitingQueue q;
    queue_init(&q);

    TEST_ASSERT(queue_is_empty(&q) == 1, "Queue starts empty");

    TEST_ASSERT(queue_enqueue(&q, "STU1", "Student One", "2026-10-13 10:00:00") == 1, "Enqueue STU1");
    TEST_ASSERT(queue_enqueue(&q, "STU2", "Student Two", "2026-10-13 10:05:00") == 1, "Enqueue STU2");
    TEST_ASSERT(queue_enqueue(&q, "STU3", "Student Three", "2026-10-13 10:10:00") == 1, "Enqueue STU3");
    TEST_ASSERT(q.count == 3, "Queue count is 3");

    /* Duplicate check */
    TEST_ASSERT(queue_enqueue(&q, "STU2", "Student Two", "2026-10-13 10:15:00") == 0, "Duplicate enqueue rejected");
    TEST_ASSERT(queue_contains(&q, "STU2") == 1, "Queue contains STU2");
    TEST_ASSERT(queue_contains(&q, "STU99") == 0, "Queue does not contain STU99");

    /* FIFO Dequeue */
    char out_id[32], out_name[64];
    TEST_ASSERT(queue_dequeue(&q, out_id, out_name) == 1, "Dequeue succeeded");
    TEST_ASSERT(strcmp(out_id, "STU1") == 0, "FIFO ordering: 1st dequeued is STU1");

    /* Remove specific */
    TEST_ASSERT(queue_remove_by_id(&q, "STU3") == 1, "Remove STU3 from queue");
    TEST_ASSERT(q.count == 1, "Queue count is 1 after removal");

    TEST_ASSERT(queue_dequeue(&q, out_id, out_name) == 1, "Dequeue last element");
    TEST_ASSERT(strcmp(out_id, "STU2") == 0, "Dequeued element is STU2");
    TEST_ASSERT(queue_is_empty(&q) == 1, "Queue is empty now");

    queue_free(&q);
}

void test_action_stack(void) {
    printf("\n--- Testing Action History LIFO Stack ---\n");
    ActionStack s;
    stack_init(&s, 3); /* test with small max_size = 3 */

    TEST_ASSERT(s.size == 0, "Stack initialized empty");

    stack_push(&s, ACTION_ADD_STUDENT, "Action 1: Added S1");
    stack_push(&s, ACTION_ALLOCATE_ROOM, "Action 2: Allocated Room 101");
    stack_push(&s, ACTION_FEE_PAYMENT, "Action 3: Paid Rs. 5000");

    TEST_ASSERT(s.size == 3, "Stack size is 3");

    /* Pushing 4th item when max_size is 3 should trim the oldest item (Action 1) */
    stack_push(&s, ACTION_VACATE_ROOM, "Action 4: Vacated Room 101");
    TEST_ASSERT(s.size == 3, "Stack capped at max_size 3");

    ActionType type;
    char desc[128];
    TEST_ASSERT(stack_pop(&s, &type, desc) == 1, "Pop top action");
    TEST_ASSERT(type == ACTION_VACATE_ROOM && strcmp(desc, "Action 4: Vacated Room 101") == 0,
                "LIFO order verified: popped Action 4 first");

    TEST_ASSERT(stack_pop(&s, &type, desc) == 1, "Pop next action");
    TEST_ASSERT(type == ACTION_FEE_PAYMENT && strcmp(desc, "Action 3: Paid Rs. 5000") == 0,
                "Popped Action 3 second");

    TEST_ASSERT(stack_pop(&s, &type, desc) == 1, "Pop next action");
    TEST_ASSERT(type == ACTION_ALLOCATE_ROOM && strcmp(desc, "Action 2: Allocated Room 101") == 0,
                "Popped Action 2 third (Action 1 was trimmed)");

    TEST_ASSERT(stack_pop(&s, &type, desc) == 0, "Stack is empty now, pop returns 0");

    stack_free(&s);
    TEST_ASSERT(s.size == 0 && s.top == NULL, "Stack freed cleanly");
}

void test_persistence(void) {
    printf("\n--- Testing Storage & Data Persistence ---\n");
    Student *students = NULL;
    Student *s1 = student_create("P101", "Persist Student", 21, "Male", "IT", "9998887770", 102, 50000.0, 30000.0);
    student_insert(&students, s1);

    Room rooms[MAX_ROOMS];
    int room_count = 0;
    room_init_defaults(rooms, &room_count);
    room_allocate_bed(rooms, room_count, 102);

    WaitingQueue q;
    queue_init(&q);
    queue_enqueue(&q, "P102", "Wait Student", "2026-10-13 12:00:00");

    TEST_ASSERT(storage_save_all(students, rooms, room_count, &q) == 1, "Save all data to files");

    /* Free memory */
    student_free_list(&students);
    queue_free(&q);

    /* Reload from disk */
    Student *loaded_students = NULL;
    Room loaded_rooms[MAX_ROOMS];
    int loaded_room_count = 0;
    WaitingQueue loaded_q;
    queue_init(&loaded_q);

    TEST_ASSERT(storage_load_all(&loaded_students, loaded_rooms, &loaded_room_count, MAX_ROOMS, &loaded_q) == 1,
                "Load all data from files");

    TEST_ASSERT(student_count(loaded_students) == 1, "Loaded 1 student");
    Student *loaded_s = student_find_by_id(loaded_students, "P101");
    TEST_ASSERT(loaded_s != NULL && strcmp(loaded_s->name, "Persist Student") == 0, "Loaded student data verified");
    TEST_ASSERT(loaded_s->pending_balance == 20000.0, "Loaded balance is 20000.0");
    TEST_ASSERT(loaded_s->room_number == 102, "Loaded room is 102");

    Room *loaded_r102 = room_find(loaded_rooms, loaded_room_count, 102);
    TEST_ASSERT(loaded_r102 != NULL && loaded_r102->occupancy == 1, "Room 102 occupancy synced to 1");

    TEST_ASSERT(loaded_q.count == 1 && queue_contains(&loaded_q, "P102") == 1, "Waiting queue loaded 1 student P102");

    student_free_list(&loaded_students);
    queue_free(&loaded_q);
}

int main(void) {
    printf("=========================================\n");
    printf("  RUNNING DSA DATA STRUCTURE TEST SUITE  \n");
    printf("=========================================\n");

    test_student_linked_list();
    test_room_array();
    test_waiting_queue();
    test_action_stack();
    test_persistence();

    printf("\n=========================================\n");
    printf("  TEST RESULTS: %d / %d PASSED (%.1f%%)\n",
           tests_passed, tests_total, ((double)tests_passed / tests_total) * 100.0);
    printf("=========================================\n");

    return (tests_passed == tests_total) ? 0 : 1;
}
