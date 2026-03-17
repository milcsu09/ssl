
CC := cc

CCFLAGS := -std=c99 -Wall -Wextra -Wpedantic -g3
LDFLAGS := -lm

SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)

TARGET := ssl

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

clean:
	rm $(OBJ)

