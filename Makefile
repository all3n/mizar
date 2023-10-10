CC := gcc
CFLAGS := -g


BUILD := build
BIN := bin
SRCS := $(wildcard *.c)
TEST_SRCS := $(wildcard tests/*.c)
UTILS_SRCS := $(wildcard utils/*.c)


SRC_OBJ := $(patsubst %.c,$(BUILD)/%.o,$(SRCS))
TEST_OBJ := $(patsubst tests/%.c,$(BUILD)/%.o,$(TEST_SRCS))
UTILS_OBJ := $(patsubst %.c,$(BUILD)/%.o,$(UTILS_SRCS))

EXES := $(patsubst %.c,$(BIN)/%,$(SRCS))
TEST_BIN := $(patsubst tests/%.c,$(BIN)/%,$(TEST_SRCS))


.PHONY: makedirs all clean
.PRECIOUS: $(SRC_OBJ) $(UTILS_OBJ) $(TEST_OBJ)


all: makedirs $(EXES) $(TEST_BIN)

debug:
	@echo $(TEST_BIN)

makedirs: 
	@mkdir -p $(BIN)
	@mkdir -p $(BUILD)
	@mkdir -p $(BUILD)/utils

$(BIN): 
	@mkdir -p $@


$(BIN)/%: $(BUILD)/%.o $(UTILS_OBJ)
	$(CC) $(CFLAGS) -o $@ $^


$(BUILD)/%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILD)/%.o : tests/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

$(BUILD)/utils/%.o : utils/%.c
	$(CC) $(CFLAGS) -c -o $@ $^

clean:
	rm -rf $(BIN) $(BUILD)

