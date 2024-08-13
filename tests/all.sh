#!/bin/sh
# execute all scripts
TEST_DIR="$(dirname "$(realpath "$0")")"

# unit tests will go here if that ever happens

# tests that require a shell
echo "STARTING SHELL TESTS"
for file in "$TEST_DIR/shell/"*; do
    "$file" || exit 1
done

echo "ALL TESTS PASSED"