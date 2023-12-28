CC = clang
CFLAGS = -Wall -Wextra -pedantic -g

SRCS = main.c mem_region.c
OBJS = $(SRCS:.c=.o)
TARGET = test_prog

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

