#ifndef STACK_H
#define STACK_H

#define MAX_ACTION_DESC_LEN 128
#define MAX_STACK_SIZE 50

typedef enum {
    ACTION_ADD_STUDENT,
    ACTION_UPDATE_STUDENT,
    ACTION_DELETE_STUDENT,
    ACTION_ALLOCATE_ROOM,
    ACTION_VACATE_ROOM,
    ACTION_FEE_PAYMENT
} ActionType;

typedef struct StackNode {
    ActionType type;
    char description[MAX_ACTION_DESC_LEN];
    char timestamp[32];
    struct StackNode *next;
} StackNode;

typedef struct {
    StackNode *top;
    int size;
    int max_size;
} ActionStack;

/* Initialize the action stack */
void stack_init(ActionStack *s, int max_size);

/* Push an action onto the top of the stack */
int stack_push(ActionStack *s, ActionType type, const char *description);

/* Pop the most recent action from the stack */
int stack_pop(ActionStack *s, ActionType *out_type, char *out_description);

/* Display the recent administrative action history (LIFO order) */
void stack_display(const ActionStack *s);

/* Free all nodes in the stack */
void stack_free(ActionStack *s);

#endif /* STACK_H */
