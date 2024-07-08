TARGET := cserve

ENTRYPOINT := main.c

SRC_DIR := src/
OUT_DIR := bin/

# build the program
build: $(OUT_DIR)$(TARGET)

# make the out dir
make_out: $(OUT_DIR)

test: build
	./$(OUT_DIR)$(TARGET)

$(OUT_DIR)$(TARGET): $(SRC_DIR)* | $(OUT_DIR)
	gcc "$(SRC_DIR)$(ENTRYPOINT)" -o "$(OUT_DIR)$(TARGET)" -Wall -Wpedantic

$(OUT_DIR):
	mkdir -p $(OUT_DIR)