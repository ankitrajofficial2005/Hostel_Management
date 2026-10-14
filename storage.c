#include "storage.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

int storage_init_dir(void) {
    struct stat st;
    if (stat(DATA_DIR, &st) == -1) {
        if (mkdir(DATA_DIR, 0755) != 0 && errno != EEXIST) {
            fprintf(stderr, "[Warning] Could not create data directory '%s'.\n", DATA_DIR);
            return 0;
        }
    }
    return 1;
}

/* Helper to split a line by delimiter into tokens array */
static int split_line(char *line, char delim, char *tokens[], int max_tokens) {
    int count = 0;
    char *start = line;
    char *p = line;

    while (*p && count < max_tokens) {
        if (*p == delim) {
            *p = '\0';
            tokens[count++] = start;
            start = p + 1;
        }
        p++;
    }

    if (count < max_tokens) {
        tokens[count++] = start;
    }

    return count;
}

int storage_save_students(const char *filepath, const Student *head) {
    storage_init_dir();
    FILE *fp = fopen(filepath, "w");
    if (!fp) {
        fprintf(stderr, "[Error] Failed to open student file '%s' for writing.\n", filepath);
        return 0;
    }

    const Student *curr = head;
    while (curr) {
        fprintf(fp, "%s|%s|%d|%s|%s|%s|%d|%.2f|%.2f|%.2f\n",
                curr->id, curr->name, curr->age, curr->gender,
                curr->course, curr->phone, curr->room_number,
                curr->total_fee, curr->amount_paid, curr->pending_balance);
        curr = curr->next;
    }

    fclose(fp);
    return 1;
}

int storage_load_students(const char *filepath, Student **head) {
    if (!head) return 0;
    *head = NULL;

    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        /* File does not exist yet (first run) - this is normal */
        return 0;
    }

    char line[512];
    int loaded = 0;

    while (fgets(line, sizeof(line), fp)) {
        utils_trim(line);
        if (line[0] == '\0' || line[0] == '#') continue;

        char *tokens[12];
        int num_tokens = split_line(line, '|', tokens, 12);
        if (num_tokens < 10) {
            continue; /* Skip corrupt line */
        }

        char *id = tokens[0];
        char *name = tokens[1];
        int age = (int)strtol(tokens[2], NULL, 10);
        char *gender = tokens[3];
        char *course = tokens[4];
        char *phone = tokens[5];
        int room_no = (int)strtol(tokens[6], NULL, 10);
        double total_fee = strtod(tokens[7], NULL);
        double amount_paid = strtod(tokens[8], NULL);

        Student *s = student_create(id, name, age, gender, course, phone, room_no, total_fee, amount_paid);
        if (s) {
            if (student_insert(head, s)) {
                loaded++;
            } else {
                student_free(s);
            }
        }
    }

    fclose(fp);
    return loaded;
}

int storage_save_rooms(const char *filepath, const Room rooms[], int count) {
    storage_init_dir();
    FILE *fp = fopen(filepath, "w");
    if (!fp) {
        fprintf(stderr, "[Error] Failed to open rooms file '%s' for writing.\n", filepath);
        return 0;
    }

    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d|%d|%d|%s\n",
                rooms[i].room_number,
                rooms[i].capacity,
                rooms[i].occupancy,
                room_status_to_string(rooms[i].status));
    }

    fclose(fp);
    return 1;
}

int storage_load_rooms(const char *filepath, Room rooms[], int *count, int max_rooms) {
    if (!rooms || !count) return 0;

    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        /* File doesn't exist yet, populate default rooms */
        room_init_defaults(rooms, count);
        return *count;
    }

    char line[256];
    int idx = 0;

    while (fgets(line, sizeof(line), fp) && idx < max_rooms) {
        utils_trim(line);
        if (line[0] == '\0' || line[0] == '#') continue;

        char *tokens[5];
        int num_tokens = split_line(line, '|', tokens, 5);
        if (num_tokens < 4) continue;

        rooms[idx].room_number = (int)strtol(tokens[0], NULL, 10);
        rooms[idx].capacity = (int)strtol(tokens[1], NULL, 10);
        rooms[idx].occupancy = (int)strtol(tokens[2], NULL, 10);
        rooms[idx].status = room_string_to_status(tokens[3]);
        idx++;
    }

    fclose(fp);

    if (idx == 0) {
        room_init_defaults(rooms, count);
    } else {
        *count = idx;
    }

    return *count;
}

int storage_save_queue(const char *filepath, const WaitingQueue *q) {
    storage_init_dir();
    FILE *fp = fopen(filepath, "w");
    if (!fp) {
        fprintf(stderr, "[Error] Failed to open queue file '%s' for writing.\n", filepath);
        return 0;
    }

    if (q) {
        QueueNode *curr = q->front;
        while (curr) {
            fprintf(fp, "%s|%s|%s\n", curr->student_id, curr->student_name, curr->request_date);
            curr = curr->next;
        }
    }

    fclose(fp);
    return 1;
}

int storage_load_queue(const char *filepath, WaitingQueue *q) {
    if (!q) return 0;
    queue_init(q);

    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        return 0;
    }

    char line[256];
    int loaded = 0;

    while (fgets(line, sizeof(line), fp)) {
        utils_trim(line);
        if (line[0] == '\0' || line[0] == '#') continue;

        char *tokens[4];
        int num_tokens = split_line(line, '|', tokens, 4);
        if (num_tokens < 3) continue;

        if (queue_enqueue(q, tokens[0], tokens[1], tokens[2])) {
            loaded++;
        }
    }

    fclose(fp);
    return loaded;
}

int storage_load_all(Student **student_head, Room rooms[], int *room_count, int max_rooms, WaitingQueue *q) {
    storage_init_dir();
    storage_load_students(STUDENTS_FILE, student_head);
    storage_load_rooms(ROOMS_FILE, rooms, room_count, max_rooms);
    storage_load_queue(QUEUE_FILE, q);

    /* Guarantee room occupancies and statuses match active student linked list */
    room_sync_occupancies(rooms, *room_count, *student_head);

    return 1;
}

int storage_save_all(const Student *student_head, const Room rooms[], int room_count, const WaitingQueue *q) {
    storage_init_dir();
    int s_ok = storage_save_students(STUDENTS_FILE, student_head);
    int r_ok = storage_save_rooms(ROOMS_FILE, rooms, room_count);
    int q_ok = storage_save_queue(QUEUE_FILE, q);

    return (s_ok && r_ok && q_ok);
}
