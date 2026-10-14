#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "utils.h"
#include "student.h"
#include "room.h"
#include "queue.h"
#include "stack.h"
#include "storage.h"

/* Global Hostel State Container */
typedef struct {
    Student *students;          /* Head of singly linked list */
    Room rooms[MAX_ROOMS];      /* Array of rooms */
    int room_count;             /* Number of configured rooms */
    WaitingQueue waiting_queue; /* FIFO Waiting Queue */
    ActionStack action_stack;   /* LIFO Administrative Action Stack */
} HostelSystem;

/* Forward declarations of menu action handlers */
static void handle_add_student(HostelSystem *sys);
static void handle_display_students(HostelSystem *sys);
static void handle_search_student(HostelSystem *sys);
static void handle_update_student(HostelSystem *sys);
static void handle_delete_student(HostelSystem *sys);
static void handle_allocate_room(HostelSystem *sys);
static void handle_vacate_room(HostelSystem *sys);
static void handle_display_all_rooms(HostelSystem *sys);
static void handle_display_available_rooms(HostelSystem *sys);
static void handle_manage_fees(HostelSystem *sys);
static void handle_display_pending_fees(HostelSystem *sys);
static void handle_manage_waiting_list(HostelSystem *sys);
static void handle_sort_by_id(HostelSystem *sys);
static void handle_sort_by_name(HostelSystem *sys);
static void handle_display_statistics(HostelSystem *sys);

/* Display application main menu */
static void print_main_menu(void) {
    printf("\n+===================================================================+\n");
    printf("|                    HOSTEL MANAGEMENT SYSTEM                       |\n");
    printf("|                DSA Project - C11 Standard Library                 |\n");
    printf("+===================================================================+\n");
    printf("|  [1]  Add New Student            |  [9]  Display Available Rooms  |\n");
    printf("|  [2]  Display All Students       |  [10] Manage Hostel Fees       |\n");
    printf("|  [3]  Search Student by ID       |  [11] Students with Pending Fee|\n");
    printf("|  [4]  Update Student Details     |  [12] Manage Waiting List      |\n");
    printf("|  [5]  Delete Student Record      |  [13] Sort Students by ID      |\n");
    printf("|  [6]  Allocate Hostel Room       |  [14] Sort Students by Name    |\n");
    printf("|  [7]  Vacate Hostel Room         |  [15] Display Hostel Statistics|\n");
    printf("|  [8]  Display All Rooms          |  [16] Exit Application         |\n");
    printf("+===================================================================+\n");
}

/* 1. Add New Student */
static void handle_add_student(HostelSystem *sys) {
    utils_print_banner("1. ADD NEW STUDENT");

    char id[MAX_ID_LEN];
    while (1) {
        utils_get_string("Enter Student ID (e.g. STU101): ", id, sizeof(id), 0);
        if (student_id_exists(sys->students, id)) {
            printf("  [!] Student with ID '%s' already exists. Please use a unique ID.\n", id);
            continue;
        }
        break;
    }

    char name[MAX_NAME_LEN];
    utils_get_string("Enter Full Name: ", name, sizeof(name), 0);

    int age;
    utils_get_int("Enter Age (16 - 60): ", 16, 60, &age);

    char gender[MAX_GENDER_LEN];
    utils_get_string("Enter Gender (Male/Female/Other): ", gender, sizeof(gender), 0);

    char course[MAX_COURSE_LEN];
    utils_get_string("Enter Course & Semester (e.g. B.Tech CSE Sem-4): ", course, sizeof(course), 0);

    char phone[MAX_PHONE_LEN];
    utils_get_string("Enter Phone Number: ", phone, sizeof(phone), 0);

    double total_fee;
    utils_get_double("Enter Total Hostel Fee (Rs.): ", 1000.0, 500000.0, &total_fee);

    double amount_paid;
    utils_get_double("Enter Initial Amount Paid (Rs.): ", 0.0, total_fee, &amount_paid);

    Student *new_student = student_create(id, name, age, gender, course, phone, 0, total_fee, amount_paid);
    if (!new_student) {
        printf("  [!] Error creating student record due to memory allocation failure.\n");
        return;
    }

    if (!student_insert(&sys->students, new_student)) {
        printf("  [!] Failed to insert student record.\n");
        student_free(new_student);
        return;
    }

    char act_desc[128];
    snprintf(act_desc, sizeof(act_desc), "Registered student %s (%s)", id, name);
    stack_push(&sys->action_stack, ACTION_ADD_STUDENT, act_desc);

    printf("\n  [+] Student '%s' (ID: %s) successfully registered!\n", name, id);

    /* Offer immediate room allocation */
    if (utils_get_confirmation("\nWould you like to allocate a hostel room for this student now?")) {
        int avail_room = room_find_first_available(sys->rooms, sys->room_count);
        if (avail_room == -1) {
            printf("\n  [!] Notice: All hostel rooms are currently full!\n");
            if (utils_get_confirmation("Would you like to add this student to the Waiting List queue?")) {
                char ts[32];
                utils_get_timestamp(ts, sizeof(ts));
                if (queue_enqueue(&sys->waiting_queue, id, name, ts)) {
                    printf("  [+] Student %s added to Waiting List (Position #%d).\n", id, sys->waiting_queue.count);
                    snprintf(act_desc, sizeof(act_desc), "Enqueued %s to waiting list", id);
                    stack_push(&sys->action_stack, ACTION_ALLOCATE_ROOM, act_desc);
                } else {
                    printf("  [!] Student is already in the waiting list.\n");
                }
            }
        } else {
            room_display_available(sys->rooms, sys->room_count);
            int alloc_choice;
            printf("\nAllocation Options:\n  1. Auto-allocate first available room (Room %d)\n  2. Select specific room\n", avail_room);
            utils_get_int("Enter option (1 or 2): ", 1, 2, &alloc_choice);

            int target_room = -1;
            if (alloc_choice == 1) {
                target_room = avail_room;
            } else {
                while (1) {
                    utils_get_int("Enter desired Room Number: ", 1, 9999, &target_room);
                    Room *r = room_find(sys->rooms, sys->room_count, target_room);
                    if (!r) {
                        printf("  [!] Room %d does not exist. Please pick from available rooms.\n", target_room);
                        continue;
                    }
                    if (!room_has_space(r)) {
                        printf("  [!] Room %d is already full. Please pick another room.\n", target_room);
                        continue;
                    }
                    break;
                }
            }

            if (room_allocate_bed(sys->rooms, sys->room_count, target_room)) {
                new_student->room_number = target_room;
                printf("  [+] Successfully allocated Room %d to student %s!\n", target_room, id);
                snprintf(act_desc, sizeof(act_desc), "Allocated Room %d to %s", target_room, id);
                stack_push(&sys->action_stack, ACTION_ALLOCATE_ROOM, act_desc);
            }
        }
    }

    storage_save_all(sys->students, sys->rooms, sys->room_count, &sys->waiting_queue);
}

/* 2. Display All Students */
static void handle_display_students(HostelSystem *sys) {
    utils_print_banner("2. ALL REGISTERED STUDENTS");
    student_display_table(sys->students);
}

/* 3. Search Student by ID */
static void handle_search_student(HostelSystem *sys) {
    utils_print_banner("3. SEARCH STUDENT BY ID");

    char id[MAX_ID_LEN];
    utils_get_string("Enter Student ID to search: ", id, sizeof(id), 0);

    Student *s = student_find_by_id(sys->students, id);
    if (!s) {
        printf("\n  [!] Student with ID '%s' was not found in the records.\n", id);
        return;
    }

    student_display_card(s);
}

/* 4. Update Student Details */
static void handle_update_student(HostelSystem *sys) {
    utils_print_banner("4. UPDATE STUDENT DETAILS");

    char id[MAX_ID_LEN];
    utils_get_string("Enter Student ID to update: ", id, sizeof(id), 0);

    Student *s = student_find_by_id(sys->students, id);
    if (!s) {
        printf("\n  [!] Student with ID '%s' was not found in the records.\n", id);
        return;
    }

    student_display_card(s);

    printf("\nChoose field to update:\n");
    printf("  1. Full Name\n");
    printf("  2. Age\n");
    printf("  3. Gender\n");
    printf("  4. Course & Semester\n");
    printf("  5. Phone Number\n");
    printf("  6. Cancel / Return\n");

    int choice;
    utils_get_int("Enter selection (1-6): ", 1, 6, &choice);

    char act_desc[128];
    switch (choice) {
        case 1: {
            char new_name[MAX_NAME_LEN];
            utils_get_string("Enter new Full Name: ", new_name, sizeof(new_name), 0);
            snprintf(s->name, sizeof(s->name), "%s", new_name);
            printf("  [+] Student name updated to '%s'.\n", s->name);
            snprintf(act_desc, sizeof(act_desc), "Updated name for student %s", s->id);
            stack_push(&sys->action_stack, ACTION_UPDATE_STUDENT, act_desc);
            break;
        }
        case 2: {
            int new_age;
            utils_get_int("Enter new Age (16 - 60): ", 16, 60, &new_age);
            s->age = new_age;
            printf("  [+] Student age updated to %d.\n", s->age);
            snprintf(act_desc, sizeof(act_desc), "Updated age for student %s", s->id);
            stack_push(&sys->action_stack, ACTION_UPDATE_STUDENT, act_desc);
            break;
        }
        case 3: {
            char new_gender[MAX_GENDER_LEN];
            utils_get_string("Enter new Gender: ", new_gender, sizeof(new_gender), 0);
            snprintf(s->gender, sizeof(s->gender), "%s", new_gender);
            printf("  [+] Gender updated to '%s'.\n", s->gender);
            snprintf(act_desc, sizeof(act_desc), "Updated gender for student %s", s->id);
            stack_push(&sys->action_stack, ACTION_UPDATE_STUDENT, act_desc);
            break;
        }
        case 4: {
            char new_course[MAX_COURSE_LEN];
            utils_get_string("Enter new Course & Semester: ", new_course, sizeof(new_course), 0);
            snprintf(s->course, sizeof(s->course), "%s", new_course);
            printf("  [+] Course updated to '%s'.\n", s->course);
            snprintf(act_desc, sizeof(act_desc), "Updated course for student %s", s->id);
            stack_push(&sys->action_stack, ACTION_UPDATE_STUDENT, act_desc);
            break;
        }
        case 5: {
            char new_phone[MAX_PHONE_LEN];
            utils_get_string("Enter new Phone Number: ", new_phone, sizeof(new_phone), 0);
            snprintf(s->phone, sizeof(s->phone), "%s", new_phone);
            printf("  [+] Phone updated to '%s'.\n", s->phone);
            snprintf(act_desc, sizeof(act_desc), "Updated phone for student %s", s->id);
            stack_push(&sys->action_stack, ACTION_UPDATE_STUDENT, act_desc);
            break;
        }
        case 6:
        default:
            printf("  [i] Update cancelled.\n");
            return;
    }

    storage_save_all(sys->students, sys->rooms, sys->room_count, &sys->waiting_queue);
}

/* 5. Delete Student Record */
static void handle_delete_student(HostelSystem *sys) {
    utils_print_banner("5. DELETE STUDENT RECORD");

    char id[MAX_ID_LEN];
    utils_get_string("Enter Student ID to delete: ", id, sizeof(id), 0);

    Student *s = student_find_by_id(sys->students, id);
    if (!s) {
        printf("\n  [!] Student with ID '%s' was not found in the records.\n", id);
        return;
    }

    student_display_card(s);

    printf("\n  [!] WARNING: Deleting a student permanently removes their registration.\n");
    if (!utils_get_confirmation("Are you sure you want to delete this student record?")) {
        printf("  [i] Deletion cancelled.\n");
        return;
    }

    int vacated_room = s->room_number;
    char s_name[MAX_NAME_LEN];
    snprintf(s_name, sizeof(s_name), "%s", s->name);

    /* If student has an allocated room, vacate bed */
    if (vacated_room > 0) {
        room_vacate_bed(sys->rooms, sys->room_count, vacated_room);
        printf("  [i] Vacated bed in Room %d previously occupied by %s.\n", vacated_room, id);
    }

    /* If student was in waiting queue, remove them */
    if (queue_contains(&sys->waiting_queue, id)) {
        queue_remove_by_id(&sys->waiting_queue, id);
        printf("  [i] Removed %s from waiting list queue.\n", id);
    }

    /* Delete from singly linked list */
    if (student_delete(&sys->students, id)) {
        printf("  [+] Student %s (ID: %s) successfully deleted from system.\n", s_name, id);
        char act_desc[128];
        snprintf(act_desc, sizeof(act_desc), "Deleted student record %s (%s)", id, s_name);
        stack_push(&sys->action_stack, ACTION_DELETE_STUDENT, act_desc);
    } else {
        printf("  [!] Error occurred while deleting student.\n");
        return;
    }

    /* Check waiting queue if a room was vacated */
    if (vacated_room > 0 && !queue_is_empty(&sys->waiting_queue)) {
        char next_id[MAX_ID_LEN];
        char next_name[MAX_NAME_LEN];
        queue_peek(&sys->waiting_queue, next_id, next_name);

        printf("\n  [i] Room %d now has a vacant bed and Waiting List has %d student(s).\n",
               vacated_room, sys->waiting_queue.count);
        printf("      Next waiting student: %s (%s)\n", next_name, next_id);

        if (utils_get_confirmation("Would you like to allocate Room to this waiting student?")) {
            queue_dequeue(&sys->waiting_queue, next_id, next_name);
            Student *next_student = student_find_by_id(sys->students, next_id);
            if (next_student) {
                next_student->room_number = vacated_room;
                room_allocate_bed(sys->rooms, sys->room_count, vacated_room);
                printf("  [+] Room %d successfully allocated to waiting student %s (%s)!\n",
                       vacated_room, next_name, next_id);

                char act_desc[128];
                snprintf(act_desc, sizeof(act_desc), "Allocated Room %d to waiting student %s", vacated_room, next_id);
                stack_push(&sys->action_stack, ACTION_ALLOCATE_ROOM, act_desc);
            }
        }
    }

    storage_save_all(sys->students, sys->rooms, sys->room_count, &sys->waiting_queue);
}

/* 6. Allocate Hostel Room */
static void handle_allocate_room(HostelSystem *sys) {
    utils_print_banner("6. ALLOCATE HOSTEL ROOM");

    char id[MAX_ID_LEN];
    utils_get_string("Enter Student ID to allocate room: ", id, sizeof(id), 0);

    Student *s = student_find_by_id(sys->students, id);
    if (!s) {
        printf("\n  [!] Student with ID '%s' does not exist.\n", id);
        return;
    }

    if (s->room_number > 0) {
        printf("\n  [!] Student %s is already allocated to Room %d.\n", s->id, s->room_number);
        printf("      Please vacate their current room before allocating a new one.\n");
        return;
    }

    int first_avail = room_find_first_available(sys->rooms, sys->room_count);
    if (first_avail == -1) {
        printf("\n  [!] All hostel rooms are currently FULL (0 vacant beds)!\n");
        if (queue_contains(&sys->waiting_queue, id)) {
            printf("  [i] Student %s is already registered in the Waiting List queue.\n", id);
        } else {
            if (utils_get_confirmation("Would you like to add this student to the Waiting List?")) {
                char ts[32];
                utils_get_timestamp(ts, sizeof(ts));
                queue_enqueue(&sys->waiting_queue, s->id, s->name, ts);
                printf("  [+] Student %s added to Waiting List (Position #%d).\n", s->id, sys->waiting_queue.count);

                char act_desc[128];
                snprintf(act_desc, sizeof(act_desc), "Enqueued %s to waiting list", s->id);
                stack_push(&sys->action_stack, ACTION_ALLOCATE_ROOM, act_desc);
                storage_save_all(sys->students, sys->rooms, sys->room_count, &sys->waiting_queue);
            }
        }
        return;
    }

    room_display_available(sys->rooms, sys->room_count);

    int opt;
    printf("\nAllocation Options:\n  1. Auto-allocate first available room (Room %d)\n  2. Enter specific room number\n", first_avail);
    utils_get_int("Enter selection (1 or 2): ", 1, 2, &opt);

    int target_room = -1;
    if (opt == 1) {
        target_room = first_avail;
    } else {
        while (1) {
            utils_get_int("Enter desired Room Number: ", 1, 9999, &target_room);
            Room *r = room_find(sys->rooms, sys->room_count, target_room);
            if (!r) {
                printf("  [!] Room %d does not exist in the hostel. Please try again.\n", target_room);
                continue;
            }
            if (!room_has_space(r)) {
                printf("  [!] Room %d is already full (Occupancy: %d/%d). Please select another room.\n",
                       target_room, r->occupancy, r->capacity);
                continue;
            }
            break;
        }
    }

    if (room_allocate_bed(sys->rooms, sys->room_count, target_room)) {
        s->room_number = target_room;

        /* If student was in waiting queue, remove them */
        if (queue_contains(&sys->waiting_queue, s->id)) {
            queue_remove_by_id(&sys->waiting_queue, s->id);
            printf("  [i] Removed %s from waiting list queue upon room allocation.\n", s->id);
        }

        printf("\n  [+] Successfully allocated Room %d to student %s (%s)!\n",
               target_room, s->name, s->id);

        char act_desc[128];
        snprintf(act_desc, sizeof(act_desc), "Allocated Room %d to student %s", target_room, s->id);
        stack_push(&sys->action_stack, ACTION_ALLOCATE_ROOM, act_desc);

        storage_save_all(sys->students, sys->rooms, sys->room_count, &sys->waiting_queue);
    } else {
        printf("  [!] Allocation failed.\n");
    }
}

/* 7. Vacate Hostel Room */
static void handle_vacate_room(HostelSystem *sys) {
    utils_print_banner("7. VACATE HOSTEL ROOM");

    printf("Vacate method:\n");
    printf("  1. Vacate by Student ID\n");
    printf("  2. Vacate by Room Number\n");
    printf("  3. Cancel\n");

    int choice;
    utils_get_int("Enter choice (1-3): ", 1, 3, &choice);

    if (choice == 3) return;

    int vacated_room = -1;
    char target_student_id[MAX_ID_LEN] = "";

    if (choice == 1) {
        utils_get_string("Enter Student ID to vacate room: ", target_student_id, sizeof(target_student_id), 0);
        Student *s = student_find_by_id(sys->students, target_student_id);
        if (!s) {
            printf("\n  [!] Student with ID '%s' was not found.\n", target_student_id);
            return;
        }
        if (s->room_number <= 0) {
            printf("\n  [!] Student %s is not currently allocated to any room.\n", s->id);
            return;
        }

        vacated_room = s->room_number;
        printf("\nStudent %s (%s) is allocated to Room %d.\n", s->name, s->id, vacated_room);
        if (!utils_get_confirmation("Confirm vacating this room for student?")) {
            printf("  [i] Operation cancelled.\n");
            return;
        }

        s->room_number = 0;
        room_vacate_bed(sys->rooms, sys->room_count, vacated_room);
        printf("  [+] Successfully vacated bed in Room %d for student %s.\n", vacated_room, target_student_id);

        char act_desc[128];
        snprintf(act_desc, sizeof(act_desc), "Vacated Room %d for student %s", vacated_room, target_student_id);
        stack_push(&sys->action_stack, ACTION_VACATE_ROOM, act_desc);
    } else {
        int rnum;
        utils_get_int("Enter Room Number to vacate from: ", 1, 9999, &rnum);
        Room *r = room_find(sys->rooms, sys->room_count, rnum);
        if (!r) {
            printf("\n  [!] Room %d does not exist.\n", rnum);
            return;
        }
        if (r->occupancy <= 0) {
            printf("\n  [!] Room %d is already completely vacant (0 occupants).\n", rnum);
            return;
        }

        /* List occupants of this room */
        printf("\nCurrent occupants of Room %d:\n", rnum);
        Student *curr = sys->students;
        int count = 0;
        while (curr) {
            if (curr->room_number == rnum) {
                count++;
                printf("  %d. %s - %s (%s)\n", count, curr->id, curr->name, curr->course);
            }
            curr = curr->next;
        }

        utils_get_string("\nEnter Student ID to vacate from this room: ", target_student_id, sizeof(target_student_id), 0);
        Student *s = student_find_by_id(sys->students, target_student_id);
        if (!s || s->room_number != rnum) {
            printf("  [!] Student %s is not an occupant of Room %d.\n", target_student_id, rnum);
            return;
        }

        if (!utils_get_confirmation("Confirm vacating room for this student?")) {
            printf("  [i] Operation cancelled.\n");
            return;
        }

        s->room_number = 0;
        vacated_room = rnum;
        room_vacate_bed(sys->rooms, sys->room_count, vacated_room);
        printf("  [+] Successfully vacated Room %d for student %s.\n", vacated_room, target_student_id);

        char act_desc[128];
        snprintf(act_desc, sizeof(act_desc), "Vacated Room %d for student %s", vacated_room, target_student_id);
        stack_push(&sys->action_stack, ACTION_VACATE_ROOM, act_desc);
    }

    /* Automatically check waiting queue to offer allocation */
    if (vacated_room > 0 && !queue_is_empty(&sys->waiting_queue)) {
        char next_id[MAX_ID_LEN];
        char next_name[MAX_NAME_LEN];
        queue_peek(&sys->waiting_queue, next_id, next_name);

        printf("\n  [i] Waiting List has %d student(s). Next waiting student: %s (%s).\n",
               sys->waiting_queue.count, next_name, next_id);

        if (utils_get_confirmation("Would you like to allocate the newly vacated bed to this waiting student?")) {
            queue_dequeue(&sys->waiting_queue, next_id, next_name);
            Student *next_s = student_find_by_id(sys->students, next_id);
            if (next_s) {
                next_s->room_number = vacated_room;
                room_allocate_bed(sys->rooms, sys->room_count, vacated_room);
                printf("  [+] Room %d allocated to waiting student %s (%s)!\n",
                       vacated_room, next_name, next_id);

                char act_desc[128];
                snprintf(act_desc, sizeof(act_desc), "Allocated Room %d to waiting student %s", vacated_room, next_id);
                stack_push(&sys->action_stack, ACTION_ALLOCATE_ROOM, act_desc);
            }
        }
    }

    storage_save_all(sys->students, sys->rooms, sys->room_count, &sys->waiting_queue);
}

/* 8. Display All Rooms */
static void handle_display_all_rooms(HostelSystem *sys) {
    utils_print_banner("8. ALL HOSTEL ROOMS");
    room_display_all(sys->rooms, sys->room_count, sys->students);
}

/* 9. Display Available Rooms */
static void handle_display_available_rooms(HostelSystem *sys) {
    utils_print_banner("9. AVAILABLE HOSTEL ROOMS");
    room_display_available(sys->rooms, sys->room_count);
}

/* 10. Manage Hostel Fees */
static void handle_manage_fees(HostelSystem *sys) {
    utils_print_banner("10. MANAGE HOSTEL FEES");

    printf("Fee Management Options:\n");
    printf("  1. Record Fee Payment for a Student\n");
    printf("  2. View Overall Fee Financial Summary\n");
    printf("  3. Return to Main Menu\n");

    int opt;
    utils_get_int("Enter selection (1-3): ", 1, 3, &opt);

    if (opt == 1) {
        char id[MAX_ID_LEN];
        utils_get_string("Enter Student ID: ", id, sizeof(id), 0);

        Student *s = student_find_by_id(sys->students, id);
        if (!s) {
            printf("\n  [!] Student with ID '%s' was not found.\n", id);
            return;
        }

        printf("\n+-------------------------------------------------------------+\n");
        printf("|                     FEE ACCOUNT STATUS                      |\n");
        printf("+-------------------------------------------------------------+\n");
        printf("|  Student ID      : %-40s |\n", s->id);
        printf("|  Student Name    : %-40s |\n", s->name);
        printf("|  Total Fee       : Rs. %-36.2f |\n", s->total_fee);
        printf("|  Amount Paid     : Rs. %-36.2f |\n", s->amount_paid);
        printf("|  Pending Balance : Rs. %-36.2f |\n", s->pending_balance);
        printf("+-------------------------------------------------------------+\n");

        if (s->pending_balance <= 0.0) {
            printf("\n  [+] Outstanding balance is Rs. 0.00. All fees have been fully settled!\n");
            return;
        }

        double payment;
        while (1) {
            utils_get_double("Enter Payment Amount to Record (Rs.): ", 1.0, s->pending_balance, &payment);
            if (payment <= 0.0) {
                printf("  [!] Payment amount must be strictly greater than zero.\n");
                continue;
            }
            if (payment > s->pending_balance) {
                printf("  [!] Payment cannot exceed outstanding balance of Rs. %.2f.\n", s->pending_balance);
                continue;
            }
            break;
        }

        s->amount_paid += payment;
        s->pending_balance = s->total_fee - s->amount_paid;
        if (s->pending_balance < 0.0) s->pending_balance = 0.0;

        printf("\n  [+] Payment of Rs. %.2f successfully recorded for %s (%s)!\n", payment, s->name, s->id);
        printf("      Remaining Outstanding Balance: Rs. %.2f\n", s->pending_balance);

        char act_desc[128];
        snprintf(act_desc, sizeof(act_desc), "Paid Rs. %.2f for student %s", payment, s->id);
        stack_push(&sys->action_stack, ACTION_FEE_PAYMENT, act_desc);

        storage_save_all(sys->students, sys->rooms, sys->room_count, &sys->waiting_queue);
    } else if (opt == 2) {
        double total_billed = 0.0;
        double total_collected = 0.0;
        double total_pending = 0.0;
        int fully_paid = 0;
        int partial_paid = 0;
        int zero_paid = 0;
        int count = 0;

        const Student *curr = sys->students;
        while (curr) {
            total_billed += curr->total_fee;
            total_collected += curr->amount_paid;
            total_pending += curr->pending_balance;
            if (curr->pending_balance <= 0.0) {
                fully_paid++;
            } else if (curr->amount_paid > 0.0) {
                partial_paid++;
            } else {
                zero_paid++;
            }
            count++;
            curr = curr->next;
        }

        double collection_rate = (total_billed > 0.0) ? (total_collected / total_billed) * 100.0 : 0.0;

        printf("\n+=============================================================+\n");
        printf("|                 OVERALL FEE FINANCIAL SUMMARY               |\n");
        printf("+=============================================================+\n");
        printf("|  Total Registered Students  : %-29d |\n", count);
        printf("|  Total Fees Billed          : Rs. %-25.2f |\n", total_billed);
        printf("|  Total Fees Collected       : Rs. %-25.2f |\n", total_collected);
        printf("|  Total Outstanding Dues     : Rs. %-25.2f |\n", total_pending);
        printf("|  Fee Collection Rate        : %-28.2f%% |\n", collection_rate);
        printf("+-------------------------------------------------------------+\n");
        printf("|  Fully Paid Students        : %-29d |\n", fully_paid);
        printf("|  Partial Paid Students      : %-29d |\n", partial_paid);
        printf("|  Zero Payment Students      : %-29d |\n", zero_paid);
        printf("+=============================================================+\n");
    }
}

/* 11. Display Students with Pending Fees */
static void handle_display_pending_fees(HostelSystem *sys) {
    utils_print_banner("11. STUDENTS WITH PENDING FEES");

    int pending_count = 0;
    double total_pending_sum = 0.0;

    const Student *curr = sys->students;
    while (curr) {
        if (curr->pending_balance > 0.0) {
            pending_count++;
            total_pending_sum += curr->pending_balance;
        }
        curr = curr->next;
    }

    if (pending_count == 0) {
        printf("\n  [+] All students have settled their hostel dues in full! No pending fees.\n");
        return;
    }

    printf("\n+----------+----------------------+--------+------------+------------+------------+---------------+\n");
    printf("| %-8s | %-20s | %-6s | %-10s | %-10s | %-10s | %-13s |\n",
           "ID", "Student Name", "Room", "Total Fee", "Paid", "Pending Due", "Phone");
    printf("+----------+----------------------+--------+------------+------------+------------+---------------+\n");

    curr = sys->students;
    while (curr) {
        if (curr->pending_balance > 0.0) {
            char rstr[16];
            if (curr->room_number > 0) {
                snprintf(rstr, sizeof(rstr), "R-%d", curr->room_number);
            } else {
                snprintf(rstr, sizeof(rstr), "None");
            }

            printf("| %-8s | %-20.20s | %-6s | %10.2f | %10.2f | %10.2f | %-13.13s |\n",
                   curr->id, curr->name, rstr, curr->total_fee, curr->amount_paid,
                   curr->pending_balance, curr->phone);
        }
        curr = curr->next;
    }

    printf("+----------+----------------------+--------+------------+------------+------------+---------------+\n");
    printf("  Total Students with Dues: %d | Total Outstanding Balance: Rs. %.2f\n",
           pending_count, total_pending_sum);
}

/* 12. Manage Waiting List */
static void handle_manage_waiting_list(HostelSystem *sys) {
    utils_print_banner("12. MANAGE WAITING LIST QUEUE (FIFO)");

    printf("Waiting List Operations:\n");
    printf("  1. View Current Waiting List\n");
    printf("  2. Add Student to Waiting List (Enqueue)\n");
    printf("  3. Allocate Next Student from Queue (Dequeue & Allocate)\n");
    printf("  4. Remove Student from Waiting List\n");
    printf("  5. Return to Main Menu\n");

    int opt;
    utils_get_int("Enter selection (1-5): ", 1, 5, &opt);

    switch (opt) {
        case 1: {
            queue_display(&sys->waiting_queue, sys->students);
            break;
        }
        case 2: {
            char id[MAX_ID_LEN];
            utils_get_string("Enter Student ID to enqueue: ", id, sizeof(id), 0);

            Student *s = student_find_by_id(sys->students, id);
            if (!s) {
                printf("  [!] Student ID '%s' not found. Please register the student first.\n", id);
                return;
            }

            if (s->room_number > 0) {
                printf("  [!] Student %s is already allocated to Room %d. Cannot add to waiting list.\n",
                       s->id, s->room_number);
                return;
            }

            if (queue_contains(&sys->waiting_queue, s->id)) {
                printf("  [!] Student %s is already in the waiting list queue.\n", s->id);
                return;
            }

            char ts[32];
            utils_get_timestamp(ts, sizeof(ts));
            if (queue_enqueue(&sys->waiting_queue, s->id, s->name, ts)) {
                printf("  [+] Student %s (%s) successfully enqueued at position #%d.\n",
                       s->name, s->id, sys->waiting_queue.count);

                char act_desc[128];
                snprintf(act_desc, sizeof(act_desc), "Enqueued %s to waiting list", s->id);
                stack_push(&sys->action_stack, ACTION_ALLOCATE_ROOM, act_desc);
                storage_save_all(sys->students, sys->rooms, sys->room_count, &sys->waiting_queue);
            }
            break;
        }
        case 3: {
            if (queue_is_empty(&sys->waiting_queue)) {
                printf("  [i] The waiting list queue is empty. No students to allocate.\n");
                return;
            }

            int first_avail = room_find_first_available(sys->rooms, sys->room_count);
            if (first_avail == -1) {
                printf("  [!] Cannot allocate: All hostel rooms are currently full!\n");
                return;
            }

            char next_id[MAX_ID_LEN];
            char next_name[MAX_NAME_LEN];
            queue_peek(&sys->waiting_queue, next_id, next_name);

            printf("\nNext student in queue: %s (%s)\n", next_name, next_id);
            room_display_available(sys->rooms, sys->room_count);

            int alloc_choice;
            printf("Allocation options:\n  1. Auto-allocate Room %d\n  2. Select specific available room\n", first_avail);
            utils_get_int("Enter selection (1 or 2): ", 1, 2, &alloc_choice);

            int target_room = -1;
            if (alloc_choice == 1) {
                target_room = first_avail;
            } else {
                while (1) {
                    utils_get_int("Enter Room Number: ", 1, 9999, &target_room);
                    Room *r = room_find(sys->rooms, sys->room_count, target_room);
                    if (!r || !room_has_space(r)) {
                        printf("  [!] Invalid or full room. Try again.\n");
                        continue;
                    }
                    break;
                }
            }

            queue_dequeue(&sys->waiting_queue, next_id, next_name);
            Student *s = student_find_by_id(sys->students, next_id);
            if (s) {
                s->room_number = target_room;
                room_allocate_bed(sys->rooms, sys->room_count, target_room);
                printf("  [+] Room %d allocated to student %s (%s) from waiting list!\n",
                       target_room, next_name, next_id);

                char act_desc[128];
                snprintf(act_desc, sizeof(act_desc), "Allocated Room %d to waiting student %s", target_room, next_id);
                stack_push(&sys->action_stack, ACTION_ALLOCATE_ROOM, act_desc);
                storage_save_all(sys->students, sys->rooms, sys->room_count, &sys->waiting_queue);
            }
            break;
        }
        case 4: {
            if (queue_is_empty(&sys->waiting_queue)) {
                printf("  [i] The waiting list is already empty.\n");
                return;
            }

            char id[MAX_ID_LEN];
            utils_get_string("Enter Student ID to remove from waiting list: ", id, sizeof(id), 0);

            if (queue_remove_by_id(&sys->waiting_queue, id)) {
                printf("  [+] Student %s removed from waiting list queue.\n", id);
                char act_desc[128];
                snprintf(act_desc, sizeof(act_desc), "Removed %s from waiting list", id);
                stack_push(&sys->action_stack, ACTION_ALLOCATE_ROOM, act_desc);
                storage_save_all(sys->students, sys->rooms, sys->room_count, &sys->waiting_queue);
            } else {
                printf("  [!] Student ID '%s' was not found in the waiting list.\n", id);
            }
            break;
        }
        case 5:
        default:
            break;
    }
}

/* 13. Sort Students by ID */
static void handle_sort_by_id(HostelSystem *sys) {
    utils_print_banner("13. SORT STUDENTS BY ID");

    if (!sys->students) {
        printf("  [i] No student records to sort.\n");
        return;
    }

    student_sort_by_id(&sys->students);
    printf("  [+] Student linked list sorted by ID (O(N log N) Merge Sort).\n");
    student_display_table(sys->students);

    stack_push(&sys->action_stack, ACTION_UPDATE_STUDENT, "Sorted student records by ID");
    storage_save_all(sys->students, sys->rooms, sys->room_count, &sys->waiting_queue);
}

/* 14. Sort Students by Name */
static void handle_sort_by_name(HostelSystem *sys) {
    utils_print_banner("14. SORT STUDENTS BY NAME");

    if (!sys->students) {
        printf("  [i] No student records to sort.\n");
        return;
    }

    student_sort_by_name(&sys->students);
    printf("  [+] Student linked list sorted alphabetically by Name (O(N log N) Merge Sort).\n");
    student_display_table(sys->students);

    stack_push(&sys->action_stack, ACTION_UPDATE_STUDENT, "Sorted student records by Name");
    storage_save_all(sys->students, sys->rooms, sys->room_count, &sys->waiting_queue);
}

/* 15. Display Hostel Statistics */
static void handle_display_statistics(HostelSystem *sys) {
    utils_print_banner("15. HOSTEL METRICS & STATISTICS");

    int total_students = student_count(sys->students);
    int accommodated_students = 0;
    int unallocated_students = 0;

    double total_fees = 0.0;
    double total_paid = 0.0;
    double total_pending = 0.0;

    const Student *curr = sys->students;
    while (curr) {
        if (curr->room_number > 0) {
            accommodated_students++;
        } else {
            unallocated_students++;
        }
        total_fees += curr->total_fee;
        total_paid += curr->amount_paid;
        total_pending += curr->pending_balance;
        curr = curr->next;
    }

    int total_capacity = 0;
    int total_occupied_beds = 0;
    int full_rooms = 0;
    int partial_rooms = 0;
    int empty_rooms = 0;

    for (int i = 0; i < sys->room_count; i++) {
        total_capacity += sys->rooms[i].capacity;
        total_occupied_beds += sys->rooms[i].occupancy;
        if (sys->rooms[i].status == ROOM_FULL) {
            full_rooms++;
        } else if (sys->rooms[i].status == ROOM_PARTIAL) {
            partial_rooms++;
        } else {
            empty_rooms++;
        }
    }

    int vacant_beds = total_capacity - total_occupied_beds;
    if (vacant_beds < 0) vacant_beds = 0;

    double bed_occupancy_rate = (total_capacity > 0) ? ((double)total_occupied_beds / total_capacity) * 100.0 : 0.0;
    double fee_collection_rate = (total_fees > 0.0) ? (total_paid / total_fees) * 100.0 : 0.0;

    printf("\n+===================================================================+\n");
    printf("|                       ROOM & BED CAPACITY                         |\n");
    printf("+===================================================================+\n");
    printf("|  Total Configured Rooms      : %-34d |\n", sys->room_count);
    printf("|  Total Hostel Bed Capacity   : %-34d |\n", total_capacity);
    printf("|  Currently Occupied Beds     : %-34d |\n", total_occupied_beds);
    printf("|  Available / Vacant Beds     : %-34d |\n", vacant_beds);
    printf("|  Bed Occupancy Rate          : %-33.2f%% |\n", bed_occupancy_rate);
    printf("|  Full Rooms (Capacity reached): %-33d |\n", full_rooms);
    printf("|  Partial Rooms               : %-34d |\n", partial_rooms);
    printf("|  Completely Vacant Rooms     : %-34d |\n", empty_rooms);
    printf("+===================================================================+\n");
    printf("|                      STUDENT DEMOGRAPHICS                         |\n");
    printf("+===================================================================+\n");
    printf("|  Total Registered Students   : %-34d |\n", total_students);
    printf("|  Accommodated in Hostel      : %-34d |\n", accommodated_students);
    printf("|  Students on Waiting Queue   : %-34d |\n", sys->waiting_queue.count);
    printf("|  Unallocated Students        : %-34d |\n", unallocated_students);
    printf("+===================================================================+\n");
    printf("|                       FINANCIAL REVENUE                           |\n");
    printf("+===================================================================+\n");
    printf("|  Total Billed Hostel Fees    : Rs. %-30.2f |\n", total_fees);
    printf("|  Total Amount Collected      : Rs. %-30.2f |\n", total_paid);
    printf("|  Total Pending Balances      : Rs. %-30.2f |\n", total_pending);
    printf("|  Fee Collection Efficiency   : %-33.2f%% |\n", fee_collection_rate);
    printf("+===================================================================+\n");

    /* Prompt to view administrative audit stack */
    if (utils_get_confirmation("\nWould you like to view the Recent Administrative Actions log (Stack)?")) {
        stack_display(&sys->action_stack);
    }
}

int main(void) {
    HostelSystem sys;
    memset(&sys, 0, sizeof(HostelSystem));

    queue_init(&sys.waiting_queue);
    stack_init(&sys.action_stack, MAX_STACK_SIZE);

    /* Initialize and load persistent data */
    storage_load_all(&sys.students, sys.rooms, &sys.room_count, MAX_ROOMS, &sys.waiting_queue);

    printf("\n+===================================================================+\n");
    printf("|                  HOSTEL MANAGEMENT SYSTEM INITIALIZED             |\n");
    printf("+===================================================================+\n");
    printf("  [i] Loaded %d students, %d rooms, and %d waiting list entries.\n",
           student_count(sys.students), sys.room_count, sys.waiting_queue.count);

    int running = 1;
    while (running) {
        print_main_menu();

        int choice;
        if (!utils_get_int("Enter your choice (1-16): ", 1, 16, &choice)) {
            break;
        }

        switch (choice) {
            case 1:  handle_add_student(&sys); break;
            case 2:  handle_display_students(&sys); break;
            case 3:  handle_search_student(&sys); break;
            case 4:  handle_update_student(&sys); break;
            case 5:  handle_delete_student(&sys); break;
            case 6:  handle_allocate_room(&sys); break;
            case 7:  handle_vacate_room(&sys); break;
            case 8:  handle_display_all_rooms(&sys); break;
            case 9:  handle_display_available_rooms(&sys); break;
            case 10: handle_manage_fees(&sys); break;
            case 11: handle_display_pending_fees(&sys); break;
            case 12: handle_manage_waiting_list(&sys); break;
            case 13: handle_sort_by_id(&sys); break;
            case 14: handle_sort_by_name(&sys); break;
            case 15: handle_display_statistics(&sys); break;
            case 16:
                if (utils_get_confirmation("\nAre you sure you want to exit the application?")) {
                    running = 0;
                }
                break;
            default:
                printf("  [!] Invalid choice. Please select 1 - 16.\n");
                break;
        }

        if (running && choice != 16) {
            utils_pause();
        }
    }

    /* Save all data before shutdown */
    storage_save_all(sys.students, sys.rooms, sys.room_count, &sys.waiting_queue);

    /* Free all dynamically allocated memory */
    student_free_list(&sys.students);
    queue_free(&sys.waiting_queue);
    stack_free(&sys.action_stack);

    printf("\n+===================================================================+\n");
    printf("|        All data saved successfully to disk. Goodbye!              |\n");
    printf("+===================================================================+\n\n");

    return 0;
}
