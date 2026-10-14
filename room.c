#include "room.h"
#include <stdio.h>
#include <string.h>
#include <strings.h>

const char* room_status_to_string(RoomStatus status) {
    switch (status) {
        case ROOM_AVAILABLE: return "AVAILABLE";
        case ROOM_PARTIAL:   return "PARTIAL";
        case ROOM_FULL:      return "FULL";
        default:             return "UNKNOWN";
    }
}

RoomStatus room_string_to_status(const char *str) {
    if (!str) return ROOM_AVAILABLE;
    if (strcasecmp(str, "FULL") == 0) return ROOM_FULL;
    if (strcasecmp(str, "PARTIAL") == 0) return ROOM_PARTIAL;
    return ROOM_AVAILABLE;
}

void room_update_status(Room *room) {
    if (!room) return;
    if (room->occupancy <= 0) {
        room->occupancy = 0;
        room->status = ROOM_AVAILABLE;
    } else if (room->occupancy >= room->capacity) {
        room->occupancy = room->capacity;
        room->status = ROOM_FULL;
    } else {
        room->status = ROOM_PARTIAL;
    }
}

void room_init_defaults(Room rooms[], int *count) {
    if (!rooms || !count) return;

    /* Initialize 12 rooms: 4 on Floor 1, 4 on Floor 2, 4 on Floor 3 */
    int room_nums[] = {101, 102, 103, 104, 201, 202, 203, 204, 301, 302, 303, 304};
    int capacities[] = {2,   2,   3,   3,   2,   2,   3,   3,   2,   2,   3,   3};
    int n = sizeof(room_nums) / sizeof(room_nums[0]);

    for (int i = 0; i < n; i++) {
        rooms[i].room_number = room_nums[i];
        rooms[i].capacity = capacities[i];
        rooms[i].occupancy = 0;
        rooms[i].status = ROOM_AVAILABLE;
    }

    *count = n;
}

Room* room_find(Room rooms[], int count, int room_number) {
    if (!rooms) return NULL;
    for (int i = 0; i < count; i++) {
        if (rooms[i].room_number == room_number) {
            return &rooms[i];
        }
    }
    return NULL;
}

int room_exists(const Room rooms[], int count, int room_number) {
    return (room_find((Room*)rooms, count, room_number) != NULL);
}

int room_has_space(const Room *room) {
    if (!room) return 0;
    return (room->occupancy < room->capacity);
}

int room_find_first_available(const Room rooms[], int count) {
    if (!rooms) return -1;
    for (int i = 0; i < count; i++) {
        if (room_has_space(&rooms[i])) {
            return rooms[i].room_number;
        }
    }
    return -1;
}

int room_allocate_bed(Room rooms[], int count, int room_number) {
    Room *r = room_find(rooms, count, room_number);
    if (!r) return 0;

    if (!room_has_space(r)) {
        return 0; /* Room is full */
    }

    r->occupancy++;
    room_update_status(r);
    return 1;
}

int room_vacate_bed(Room rooms[], int count, int room_number) {
    Room *r = room_find(rooms, count, room_number);
    if (!r) return 0;

    if (r->occupancy <= 0) {
        return 0; /* Already empty */
    }

    r->occupancy--;
    room_update_status(r);
    return 1;
}

void room_sync_occupancies(Room rooms[], int count, const Student *student_head) {
    if (!rooms) return;

    /* Reset all occupancies to 0 */
    for (int i = 0; i < count; i++) {
        rooms[i].occupancy = 0;
    }

    /* Recalculate based on active student records */
    const Student *curr = student_head;
    while (curr) {
        if (curr->room_number > 0) {
            Room *r = room_find(rooms, count, curr->room_number);
            if (r) {
                r->occupancy++;
            }
        }
        curr = curr->next;
    }

    /* Sync statuses */
    for (int i = 0; i < count; i++) {
        room_update_status(&rooms[i]);
    }
}

void room_display_all(const Room rooms[], int count, const Student *student_head) {
    if (!rooms || count == 0) {
        printf("\n  [i] No room records found.\n");
        return;
    }

    printf("\n+----------+----------+----------+-----------+------------+------------------------------------------+\n");
    printf("| %-8s | %-8s | %-8s | %-9s | %-10s | %-40s |\n",
           "Room No", "Capacity", "Occupied", "Available", "Status", "Current Occupants (Student IDs)");
    printf("+----------+----------+----------+-----------+------------+------------------------------------------+\n");

    for (int i = 0; i < count; i++) {
        const Room *r = &rooms[i];
        int avail = r->capacity - r->occupancy;
        if (avail < 0) avail = 0;

        /* Gather occupants for this room from student linked list */
        char occupants[128] = "";
        int occ_count = 0;
        const Student *s = student_head;
        while (s) {
            if (s->room_number == r->room_number) {
                if (occ_count > 0) {
                    strncat(occupants, ", ", sizeof(occupants) - strlen(occupants) - 1);
                }
                strncat(occupants, s->id, sizeof(occupants) - strlen(occupants) - 1);
                occ_count++;
            }
            s = s->next;
        }

        if (occ_count == 0) {
            snprintf(occupants, sizeof(occupants), "(Vacant)");
        }

        printf("| %-8d | %-8d | %-8d | %-9d | %-10s | %-40.40s |\n",
               r->room_number, r->capacity, r->occupancy, avail,
               room_status_to_string(r->status), occupants);
    }

    printf("+----------+----------+----------+-----------+------------+------------------------------------------+\n");
    printf("  Total Rooms: %d\n", count);
}

void room_display_available(const Room rooms[], int count) {
    if (!rooms || count == 0) {
        printf("\n  [i] No room records found.\n");
        return;
    }

    int avail_count = 0;
    int total_vacant_beds = 0;

    printf("\n+----------+----------+----------+-----------+------------+\n");
    printf("| %-8s | %-8s | %-8s | %-9s | %-10s |\n",
           "Room No", "Capacity", "Occupied", "Vacant Beds", "Status");
    printf("+----------+----------+----------+-----------+------------+\n");

    for (int i = 0; i < count; i++) {
        const Room *r = &rooms[i];
        if (room_has_space(r)) {
            int vacant = r->capacity - r->occupancy;
            printf("| %-8d | %-8d | %-8d | %-11d | %-10s |\n",
                   r->room_number, r->capacity, r->occupancy, vacant,
                   room_status_to_string(r->status));
            avail_count++;
            total_vacant_beds += vacant;
        }
    }

    printf("+----------+----------+----------+-----------+------------+\n");
    if (avail_count == 0) {
        printf("  [!] All hostel rooms are currently fully occupied (0 vacant beds).\n");
    } else {
        printf("  Available Rooms: %d | Total Vacant Beds: %d\n", avail_count, total_vacant_beds);
    }
}
