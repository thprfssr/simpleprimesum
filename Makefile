SRC_DIR = ./src
OBJ_DIR = ./obj
SOURCE = $(addprefix $(SRC_DIR)/, bitarray.c primeerr.c primesum.c main.c)
HEADER = $(addprefix $(SRC_DIR)/, bitarray.h primeerr.h primesum.h)
OBJECT = $(addprefix $(OBJ_DIR)/, bitarray.o primeerr.o primesum.o main.o)
OUTPUT = primesum
LFLAGS = -lpthread -lm

ifeq ($(CC), clang)
CC = clang
CFLAGS = -Weverything -Werror
else
CC = gcc
CFLAGS = -Wall -Werror
endif

# Compilation flags to be added from the commandline (e.g. `-g` or `-DDEBUG`)
CFLAGS += $(CLIFLAGS)

all: $(OBJECT)
	$(CC) -o $(OUTPUT) $^ $(LFLAGS)

$(OBJ_DIR)/primesum.o: $(addprefix $(SRC_DIR)/, primesum.c primesum.h)
	mkdir -p $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/primeerr.o: $(addprefix $(SRC_DIR)/, primeerr.c primeerr.h)
	mkdir -p $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/bitarray.o: $(addprefix $(SRC_DIR)/BitArray/, \
		bitarray.c bitarray.h)
	mkdir -p $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/main.o: $(addprefix $(SRC_DIR)/, main.c)
	mkdir -p $(OBJ_DIR)
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJ_DIR)/* $(OUTPUT)
.PHONY: clean

