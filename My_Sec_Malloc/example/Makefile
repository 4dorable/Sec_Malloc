# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11 -fPIC -Iinclude
CRITERION_FLAGS = -lcriterion

# Directories
SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = test
LOG_DIR = log

# Files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(SRC_DIR)/%.o)
STATIC_LIB = libmy_secmalloc.a
DYNAMIC_LIB = libmy_secmalloc.so
TEST_EXEC = test_exec
LOG_FILE = $(LOG_DIR)/logfile.log

# Targets
.PHONY: all static dynamic clean test_static test_dynamic

all: static dynamic $(TEST_EXEC)

static: $(STATIC_LIB)

dynamic: $(DYNAMIC_LIB)

test_static: static $(TEST_EXEC) | create_log_dir
	env MSM_OUTPUT=$(PWD)/$(LOG_FILE) ./$(TEST_EXEC)

test_dynamic: dynamic $(TEST_EXEC) | create_log_dir
	env MSM_OUTPUT=$(PWD)/$(LOG_FILE) LD_PRELOAD=$(PWD)/$(DYNAMIC_LIB) LD_LIBRARY_PATH=$(PWD):$(LD_LIBRARY_PATH) ./$(TEST_EXEC)

clean:
	rm -rf $(OBJ) $(STATIC_LIB) $(DYNAMIC_LIB) $(TEST_EXEC)

$(SRC_DIR)/%.o: $(SRC_DIR)/%.c $(INCLUDE_DIR)/my_secmalloc.h $(INCLUDE_DIR)/my_secmalloc.private.h
	$(CC) $(CFLAGS) -c $< -o $@

$(STATIC_LIB): $(OBJ)
	ar rcs $@ $^

$(DYNAMIC_LIB): $(OBJ)
	$(CC) -shared -o $@ $^

$(TEST_EXEC): $(TEST_DIR)/test.c $(STATIC_LIB)
	$(CC) $(CFLAGS) -o $@ $< $(STATIC_LIB) $(CRITERION_FLAGS)

# Rule to create log directory
create_log_dir:
	mkdir -p $(LOG_DIR)

# Rule to create empty logfile.log if not exists
$(LOG_FILE):
	touch $(LOG_FILE)


