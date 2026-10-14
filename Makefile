CC ?= gcc
CFLAGS = -std=c11 -Wall -Wextra -pedantic -g
LDFLAGS =

TARGET = hostel_mgmt

SRCS = main.c student.c room.c queue.c stack.c storage.c utils.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS)

test: $(OBJS) test_dsa.o
	$(CC) $(CFLAGS) -o test_dsa test_dsa.o student.o room.o queue.o stack.o storage.o utils.o $(LDFLAGS)
	./test_dsa

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) test_dsa.o $(TARGET) test_dsa
	rm -rf $(TARGET).dSYM test_dsa.dSYM

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run test
