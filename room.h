#ifndef ROOM_H
#define ROOM_H

#include "student.h"

#define MAX_ROOMS 50

typedef enum {
    ROOM_AVAILABLE, /* Occupancy == 0 */
    ROOM_PARTIAL,   /* 0 < Occupancy < Capacity */
    ROOM_FULL       /* Occupancy >= Capacity */
} RoomStatus;

typedef struct {
    int room_number;     /* Room Number (e.g. 101, 102, etc.) */
    int capacity;        /* Maximum beds/occupants */
    int occupancy;       /* Current occupants */
    RoomStatus status;   /* ROOM_AVAILABLE, ROOM_PARTIAL, ROOM_FULL */
} Room;

/* Convert RoomStatus enum to string */
const char* room_status_to_string(RoomStatus status);

/* Convert string to RoomStatus enum */
RoomStatus room_string_to_status(const char *str);

/* Initialize default rooms if no saved configuration exists */
void room_init_defaults(Room rooms[], int *count);

/* Find room by room number in the array. Returns pointer or NULL. */
Room* room_find(Room rooms[], int count, int room_number);

/* Check if a room exists in the array */
int room_exists(const Room rooms[], int count, int room_number);

/* Recalculate room status based on current occupancy and capacity */
void room_update_status(Room *room);

/* Check if a room has at least one vacant bed */
int room_has_space(const Room *room);

/* Find first room that has at least one vacant bed. Returns room_number or -1 if full. */
int room_find_first_available(const Room rooms[], int count);

/* Allocate a bed in the given room number (increments occupancy). Returns 1 on success, 0 on failure. */
int room_allocate_bed(Room rooms[], int count, int room_number);

/* Vacate a bed in the given room number (decrements occupancy). Returns 1 on success, 0 on failure. */
int room_vacate_bed(Room rooms[], int count, int room_number);

/* Synchronize room occupancies and statuses against the student linked list */
void room_sync_occupancies(Room rooms[], int count, const Student *student_head);

/* Display all rooms formatted in a table, including occupants */
void room_display_all(const Room rooms[], int count, const Student *student_head);

/* Display only available/partial rooms with vacant beds */
void room_display_available(const Room rooms[], int count);

#endif /* ROOM_H */
