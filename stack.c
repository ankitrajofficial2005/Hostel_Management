#include "stack.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char* action_type_to_str(ActionType type) {
    switch (type) {
        case ACTION_ADD_STUDENT:     return "ADD STUDENT";
        case ACTION_UPDATE_STUDENT:  return "UPDATE STUDENT";
        case ACTION_DELETE_STUDENT:  return "DELETE STUDENT";
        case ACTION_ALLOCATE_ROOM:   return "ALLOCATE ROOM";
        case ACTION_VACATE_ROOM:     return "VACATE ROOM";
        case ACTION_FEE_PAYMENT:     return "FEE PAYMENT";
        default:                     return "GENERAL";
    }
}

void stack_init(ActionStack *s, int max_size) {
    if (!s) return;
    s->top = NULL;
    s->size = 0;
    s->max_size = (max_size > 0) ? max_size : MAX_STACK_SIZE;
}

int stack_push(ActionStack *s, ActionType type, const char *description) {
    if (!s) return 0;

    /* If stack size reaches max_size, remove the bottom-most node */
    if (s->size >= s->max_size && s->top != NULL) {
        StackNode *curr = s->top;
        StackNode *prev = NULL;
        while (curr->next != NULL) {
            prev = curr;
            curr = curr->next;
        }
        if (prev != NULL) {
            prev->next = NULL;
            free(curr);
            s->size--;
        }
    }

    StackNode *node = (StackNode *)malloc(sizeof(StackNode));
    if (!node) {
        fprintf(stderr, "[Error] Memory allocation failed for StackNode.\n");
        return 0;
    }

    node->type = type;
    snprintf(node->description, sizeof(node->description), "%s", description ? description : "");
    utils_get_timestamp(node->timestamp, sizeof(node->timestamp));
    node->next = s->top;

    s->top = node;
    s->size++;
    return 1;
}

int stack_pop(ActionStack *s, ActionType *out_type, char *out_description) {
    if (!s || s->top == NULL) return 0;

    StackNode *temp = s->top;
    if (out_type) {
        *out_type = temp->type;
    }
    if (out_description) {
        snprintf(out_description, MAX_ACTION_DESC_LEN, "%s", temp->description);
    }

    s->top = s->top->next;
    free(temp);
    s->size--;
    return 1;
}

void stack_display(const ActionStack *s) {
    if (!s || s->top == NULL) {
        printf("\n  [i] No recent administrative actions recorded in the stack.\n");
        return;
    }

    printf("\n+-----+---------------------+------------------+------------------------------------------------+\n");
    printf("| %-3s | %-19s | %-16s | %-46s |\n",
           "#", "Timestamp", "Action Type", "Description");
    printf("+-----+---------------------+------------------+------------------------------------------------+\n");

    StackNode *curr = s->top;
    int index = 1;
    while (curr) {
        printf("| %-3d | %-19.19s | %-16.16s | %-46.46s |\n",
               index, curr->timestamp, action_type_to_str(curr->type), curr->description);
        index++;
        curr = curr->next;
    }

    printf("+-----+---------------------+------------------+------------------------------------------------+\n");
    printf("  Total Actions in Stack (LIFO): %d\n", s->size);
}

void stack_free(ActionStack *s) {
    if (!s) return;
    StackNode *curr = s->top;
    while (curr) {
        StackNode *temp = curr->next;
        free(curr);
        curr = temp;
    }
    s->top = NULL;
    s->size = 0;
}
