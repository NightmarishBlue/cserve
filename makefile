COMPILER_OPTS := -Wall -Wpedantic

TARGET := cserve

SRC_DIR := src/
OUT_DIR := bin/

INC_DIR := inc/

# build the program
build: $(OUT_DIR)$(TARGET)

# make the out dir
make_out: $(OUT_DIR)

test: build
	./$(OUT_DIR)$(TARGET)

debug: COMPILER_OPTS += -g
debug: build

$(OUT_DIR)$(TARGET): $(SRC_DIR)* | make_out
	gcc "$(SRC_DIR)"* -I "$(INC_DIR)" -o "$(OUT_DIR)$(TARGET)" $(COMPILER_OPTS)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)