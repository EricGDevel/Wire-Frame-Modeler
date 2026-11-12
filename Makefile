# Compiler and flags (matches build-linux.sh)
CC          ?= gcc
STD         := -std=c99
WARN        := -Wall -Wno-missing-braces -Wno-unused-function
OPT         ?= -O2
CFLAGS      := $(STD) $(WARN) $(OPT)
DBGFLAGS    ?= -ggdb3 -O0 -fno-omit-frame-pointer -fno-inline -DDEBUG

INCLUDES    := -Ilib/include
LDFLAGS     := -Wl,-rpath,'$$ORIGIN' -Llib/lib-linux
LDLIBS      := -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
TEST_LDLIBS := $(LDFLAGS) $(LDLIBS)

# Project paths and files
TARGET      := cg
BUILD_DIR   := build
TEST_DIR    := tests
HEADERS     := $(wildcard *.h)

# Automatically include any .c file that has a matching .h file (e.g., model.h -> model.c)
MODULE_SRCS := $(wildcard $(HEADERS:.h=.c))
MODULE_OBJS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(MODULE_SRCS))
MAIN_OBJ    := $(BUILD_DIR)/main.o
OBJS        := $(MAIN_OBJ) $(MODULE_OBJS)

# Auto-discover unit tests (supports tests/*.c and tests/<suite>/*.c)
TEST_SRCS   := $(sort $(wildcard $(TEST_DIR)/*.c $(TEST_DIR)/*/*.c))
TEST_BINS   := $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/$(TEST_DIR)/%,$(TEST_SRCS))
TEST_DEPS   := $(HEADERS) $(wildcard $(TEST_DIR)/*.h $(TEST_DIR)/*/*.h)
TEST_SUITES := $(sort $(patsubst $(TEST_DIR)/%/,%,$(dir $(wildcard $(TEST_DIR)/*/*.c))))
TEST_NAMES  := $(notdir $(basename $(TEST_SRCS)))

.PHONY: all debug test clean help \
        $(addprefix test-,$(TEST_SUITES)) \
        $(addprefix test-,$(TEST_NAMES))

# Default target: build main executable
all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "Linking $(TARGET)..."
	$(CC) $(OBJS) $(LDFLAGS) $(LDLIBS) $(CFLAGS) -o $@

# Rule for main.c (depends on all headers)
$(MAIN_OBJ): main.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) -c $< $(INCLUDES) $(CFLAGS) -o $@

# Pattern rule: each module .c file is compiled with its matching .h file
$(BUILD_DIR)/%.o: %.c %.h
	@mkdir -p $(dir $@)
	$(CC) -c $< $(INCLUDES) $(CFLAGS) -o $@

# Build with debug symbols and no optimizations for gdb debugging
debug: CFLAGS += $(DBGFLAGS)
debug: all

# Pattern rule: compile any tests/<path>.c -> build/tests/<path> (linked with module objects)
$(BUILD_DIR)/$(TEST_DIR)/%: $(TEST_DIR)/%.c $(MODULE_OBJS) $(TEST_DEPS)
	@mkdir -p $(dir $@)
	$(CC) $< $(MODULE_OBJS) $(INCLUDES) $(CFLAGS) $(TEST_LDLIBS) -o $@

# Run all unit tests
test: $(TEST_BINS)
	@failed=0; \
	for bin in $(TEST_BINS); do \
		echo "--> Running $$bin"; \
		./$$bin || failed=$$((failed + 1)); \
		echo ""; \
	done; \
	if [ $$failed -ne 0 ]; then \
		echo "$$failed test binary(s) failed."; \
		exit 1; \
	fi

# Run a test suite folder: e.g. `make test-math`
$(addprefix test-,$(TEST_SUITES)): test-%:
	@$(MAKE) --no-print-directory $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/$(TEST_DIR)/%,$(wildcard $(TEST_DIR)/$*/*.c))
	@failed=0; \
	for bin in $(patsubst $(TEST_DIR)/%.c,$(BUILD_DIR)/$(TEST_DIR)/%,$(wildcard $(TEST_DIR)/$*/*.c)); do \
		echo "--> Running $$bin"; \
		./$$bin || failed=$$((failed + 1)); \
		echo ""; \
	done; \
	exit $$failed

# Run a single test by short name: e.g. `make test-vec3` or `make test-mat3`
$(addprefix test-,$(TEST_NAMES)): test-%:
	@$(MAKE) --no-print-directory $(filter %/$*,$(TEST_BINS))
	@for bin in $(filter %/$*,$(TEST_BINS)); do \
		echo "--> Running $$bin"; \
		./$$bin || exit 1; \
	done

clean:
	rm -rf $(TARGET) $(BUILD_DIR)

help:
	@echo "Targets:"
	@echo "  make                  Build $(TARGET) (with OPT=-O2)"
	@echo "  make debug            Build $(TARGET) without optimizations (OPT=-O0)"
	@echo "  make test             Compile and run all unit tests"
	@echo "  make test-<suite>     Run all tests in a folder (e.g., make test-math)"
	@echo "  make test-<name>      Run a single test file (e.g., make test-vec3, make test-mat3)"
	@echo "  make clean            Remove $(TARGET) and $(BUILD_DIR)/"
