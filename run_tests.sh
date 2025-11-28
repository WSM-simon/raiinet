#!/bin/bash
# Run all RAIInet tests

cd "$(dirname "$0")"

echo "========================================"
echo "       RAIInet Test Suite"
echo "========================================"
echo ""

PASS=0
FAIL=0

for testfile in tests/test_*.txt; do
    testname=$(basename "$testfile" .txt)
    echo -n "Testing $testname... "
    
    # Run test using sequence command
    output=$(echo -e "sequence $testfile\nquit" | ./raiinet 2>&1)
    exitcode=$?
    
    # Check for crashes (segfault, etc)
    if [ $exitcode -ne 0 ]; then
        echo "FAIL (exit code: $exitcode)"
        FAIL=$((FAIL + 1))
    # Check for common error patterns that indicate bugs
    elif echo "$output" | grep -q "Segmentation fault\|Abort\|core dumped"; then
        echo "FAIL (crash detected)"
        FAIL=$((FAIL + 1))
    else
        echo "PASS"
        PASS=$((PASS + 1))
    fi
done

echo ""
echo "========================================"
echo "Results: $PASS passed, $FAIL failed"
echo "========================================"

if [ $FAIL -eq 0 ]; then
    echo "All tests passed!"
    exit 0
else
    echo "Some tests failed!"
    exit 1
fi

