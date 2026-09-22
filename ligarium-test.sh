#!/usr/bin/env bash
clear

set -e

cmake --preset debug-linux \
    -DLIGARIUM_BUILD_TESTS=ON

cmake --build --preset build-debug-linux

clear

ctest --test-dir build/debug --output-on-failure 2>&1 |
sed '/=== Stack trace ===/,/=== End of stack trace ===/d' |
tee test-result.log

echo
echo 'see results in "test-result.log"'