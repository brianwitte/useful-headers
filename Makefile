CFLAGS = -Wall -Wextra -O2 -std=c99 -g2 -Wno-unused-function -I. -I$(TEST_DIR)
LDFLAGS =

SRC_DIR = .
TEST_DIR = test
BIN_DIR = $(TEST_DIR)/bin

# Source and header files
LIB_HEADERS = $(SRC_DIR)/bstr.h $(SRC_DIR)/uptest.h
TEST_SOURCES = $(TEST_DIR)/test_bstr.c
TEST_BINARIES = $(BIN_DIR)/test_bstr

# Default target: Ensure the test binary is built
all: $(BIN_DIR) $(TEST_BINARIES)

# Ensure the bin directory exists
$(BIN_DIR):
	@mkdir -p $(BIN_DIR)

# Build test binaries explicitly, no wildcards
$(BIN_DIR)/test_bstr: $(TEST_DIR)/test_bstr.c $(LIB_HEADERS)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

test: all
	$(BIN_DIR)/test_bstr

clean:
	@rm -rf $(BIN_DIR)

zen:
	@echo "Makefile Zen:"
	@echo "1. Simplicity: Explicit rules, no wildcards."
	@echo "2. Portability: No OS-specific assumptions."
	@echo "3. Minimalism: Essential flags and rules only."
	@echo "4. Modularity: Clean separation of targets."
	@echo "5. Transparency: No hidden logic."
	@echo "6. Feedback: Clear output for each step."

help:
	@echo "Usage: make [TARGET]"
	@echo ""
	@echo "Available targets:"
	@echo "  all       - Build the test binaries."
	@echo "  test      - Build and run the unit tests."
	@echo "  clean     - Clean the build artifacts."
	@echo "  zen       - Display the Makefile Zen principles."
	@echo "  help      - Display this help message."

.PHONY: all clean test zen help
